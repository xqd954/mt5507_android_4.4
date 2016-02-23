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
*  bt_pbap_hdl.c
*
* Project:
* -------- 
*  WISE
*
* Description:
* ------------
*  phonebook access profile
*
* Author: Xueling Li
* -------
*
*============================================================================
*             HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Log$
*
*
* 
*
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
//#include "osapi.h"    
//#include "eventmgr.h"
#include "ext_osal.h"
#include "bt_os_api.h"
#include "bt_osal_vcard.h"
//#include "bt_adp_fstream.h"
#include "bluetooth_pbap_message.h"
#include "bluetooth_pbap_struct.h"
#include "bt_pbap_api.h"
#include "bt_pbap_hdl.h"
//#include "MBTType.h"
//#include "MBTSdc.h"   
#include "ext_ucs2.h"

extern MBT_BOOL mbt_postevent(T_MBTEVT Evt, MBT_SHORT conn_idx);
extern void btmtk_gap_handle_authorize_req(MBT_SERVICE_ID MBTSvcID, T_MBT_BDADDR RemoteBDAddr, MBT_CHAR* Name);
#ifdef BTMTK_ON_WISE
extern int wsConvertFileNameToNativeFormat(unsigned short *targetBuff_p, U8* fileName);
extern int wsConvertNativeFormatToFileName(unsigned short *targetBuff_p, U8* fileName);
//extern MBT_VOID* mbt_sdc_getrecord(T_MBT_SDC_RECID MBTSDCRecID);
#endif
/*functions  handle message from internal adp */
static void btmtk_pbap_handle_activate_cnf(void *pmsg);
static void btmtk_pbap_handle_deactivate_cnf(void *pmsg);
static void btmtk_pbap_handle_authorize_ind(void *pmsg);
static void btmtk_pbap_handle_client_connect_ind(void *pmsg);
static void btmtk_pbap_handle_client_auth_challenge_ind(void *pmsg);
static void btmtk_pbap_handle_disconnect_ind(void *pmsg);
static void btmtk_pbap_handle_abort_ind(void *pmsg);
static void btmtk_pbap_handle_oper_success_ind(void *pmsg);
static void btmtk_pbap_handle_set_path_ind(void *pmsg);
static void btmtk_pbap_handle_read_entry_ind(void *pmsg);
static void btmtk_pbap_handle_read_folder_ind(void *pmsg);
static void btmtk_pbap_handle_read_list_ind(void *pmsg);

/*utilty function */
static void btmtk_pbap_string_swapcpy(U8* dest_str, const U8* src_str);
static U16 btmtk_pbap_ucs2_str_to_asc_str(S8 *pOutBuffer, S8 *pInBuffer);

static U32 btmtk_pbap_get_entry_index(U16 * path);
static U8 btmtk_pbap_set_folder(U16*path);
static PbapPhonebookName btmtk_pbap_get_vcardlist_dir(U16*path);
static PbPhonebookPath btmtk_pbap_get_pb_folder(U16*path);

static void notify_event_to_application(T_MBTEVT event_id);

void btmtk_pbap_handle_operation(void);
static void btmtk_pbap_read_vcarddata_from_application(void);
static void btmtk_pbap_read_vcardlist_from_application(void);
static void btmtk_pbap_obex_auth_event_delay_timer(EvmTimer *timer);

/*context data of   external ADP */
BT_PBAP_APP_CTX  bt_pbap_app;
U8 bt_pbap_app_file_path[BT_PBAP_MAX_FILEPATH_NAME_LEN];

MBT_VOID ShowBdAddr(T_MBT_BDADDR p_data)
{
	char	buff[20] = { 0, };

	sprintf(buff, "%02x-%02x-%02x-%02x-%02x-%02x", p_data[0], p_data[1], p_data[2], p_data[3], p_data[4], p_data[5]);
	PBAP_MBT_LOG1("[BTMTK] [PBAP] Present sdcPBAPStatus BDAddr: %s", buff);	//%z: ASCII, %s:Unicode
}
MBT_VOID UpdateBdAddr(T_MBT_BDADDR p_data)
{
	char	buff[20] = { 0, };

	sprintf(buff, "%02x-%02x-%02x-%02x-%02x-%02x", p_data[0], p_data[1], p_data[2], p_data[3], p_data[4], p_data[5]);
	PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus BDAddr to : %s", buff);	//%z: ASCII, %s:Unicode
}


void btmtk_pbap_app_set_state(BT_PBAP_APP_STATE state)
{
	bt_trace(TRACE_FUNC, BT_PBAP_MBT_SET_STATE, state);
    PBAP_MBT_LOG1("[BTMTK] [PBAP] Set External ADP State to %d", state);
    bt_pbap_app.server_state = state;
}

BT_PBAP_APP_STATE btmtk_pbap_app_get_state(void)
{
    return (bt_pbap_app.server_state);	
}

MBT_BOOL  btmtk_pbap_is_connected(void)
{
    MBT_BOOL ret;
    if(bt_pbap_app.server_state >= BT_PBAP_APP_STATE_CONNECTED &&
		bt_pbap_app.server_state < BT_PBAP_APP_STATE_DISCONNECTING)
    {
        ret = TRUE;
    }
    else
    {
        ret =  FALSE;
    }
    return ret;
}

MBT_BOOL btmtk_pbap_is_dev_connected(T_MBT_BDADDR RemoteBDAddr)
{
	T_MBT_PBAP_STATUS *sdcPBAPStatus;
	MBT_INT result = 0;

	sdcPBAPStatus = (T_MBT_PBAP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_PBAP_STATUS);
	result = memcmp(sdcPBAPStatus->BDAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR));
	if ((0x00 == result) && (MBT_TRUE == sdcPBAPStatus->bEnabled) && 
		((MBT_PBAP_STATE_CONNECTED == sdcPBAPStatus->State)||(MBT_PBAP_STATE_SENDING == sdcPBAPStatus->State)))
	{
		return MBT_TRUE;
	}
		return MBT_FALSE;
}



/*   messesge handler of pbap external adp */
void btmtk_pbap_handle_message(ilm_struct *message)
{

#ifndef BTMTK_ON_WISE 
     void* pmsg = message->ilm_data;
#else
    void* pmsg = message ->local_para_ptr;
#endif
    bt_trace(TRACE_FUNC, BT_PBAP_MBT_EXT_HANDLE_MESSAGE, message->msg_id);
    PBAP_MBT_LOG1("[BTMTK] [PBAP] External ADP  handle message, Message ID =  %d", message->msg_id);
    switch(message->msg_id)
    {	
		/*      message from bt task   */
	case  MSG_ID_BT_PBAP_REGISTER_CNF:
		btmtk_pbap_handle_activate_cnf(pmsg);
		break;
	case MSG_ID_BT_PBAP_DEREGISTER_CNF:
		btmtk_pbap_handle_deactivate_cnf(pmsg);
		break;  
	case MSG_ID_BT_PBAP_AUTHORIZE_IND:
		btmtk_pbap_handle_authorize_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_CLIENT_CONNECT_IND:
		btmtk_pbap_handle_client_connect_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_IND:
		btmtk_pbap_handle_client_auth_challenge_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_DISCONNECT_IND:
		btmtk_pbap_handle_disconnect_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_ABORT_IND:
		btmtk_pbap_handle_abort_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_OPER_SUCCESS_IND:
		btmtk_pbap_handle_oper_success_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_SET_PATH_IND:
		btmtk_pbap_handle_set_path_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_READ_ENTRY_IND:
		btmtk_pbap_handle_read_entry_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_READ_FOLDER_IND:
		btmtk_pbap_handle_read_folder_ind(pmsg);
		break;
	case MSG_ID_BT_PBAP_READ_LIST_IND:
		btmtk_pbap_handle_read_list_ind(pmsg);
		break;		
		/*  message from mmi task,  in the other mod, if in the same task  may call the function interface directly  */		
	case  MSG_ID_BT_PBAP_REGISTER_REQ:
		{
		    bt_pbap_register_req_struct* msg = (bt_pbap_register_req_struct*) pmsg;
		    btmtk_pbap_active_req(msg->security_level, msg->support_repositories);
		}
		break;
	case MSG_ID_BT_PBAP_DEREGISTER_REQ:
		btmtk_pbap_deactive_req();
		break;
		
	case MSG_ID_BT_PBAP_AUTHORIZE_RSP: 
		{
			bt_pbap_authorize_rsp_struct* msg = (bt_pbap_authorize_rsp_struct*) pmsg;
			btmtk_pbap_authrize_rsp(msg->cnf_code);
		}
		break;
	case MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_RSP:
		{
			bt_pbap_client_auth_challenge_rsp_struct *msg = (bt_pbap_client_auth_challenge_rsp_struct*) pmsg;
			btmtk_pbap_obex_auth_challege_rsp(msg->cancel, 
				msg->password, 20,
				msg->userId, 20);
		}	
		break;
		
	case MSG_ID_BT_PBAP_DISCONNECT_REQ:
		btmtk_pbap_disconnect_req(TRUE);
		break;
		
	case MSG_ID_BT_PBAP_READ_ENTRY_RSP:		
	case MSG_ID_BT_PBAP_READ_FOLDER_RSP:
	case MSG_ID_BT_PBAP_READ_LIST_RSP:
		btmtk_pbap_handle_operation();
		break;
		
	default:
		break;
    }
	
}


void btmtk_pbap_handle_activate_cnf(void *pmsg)
{	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_register_cnf_struct *cnf = (bt_pbap_register_cnf_struct*)pmsg;	
	
    /*----------------------------------------------------------------*/
    /* Code Body */	
    if (bt_pbap_app.server_state == BT_PBAP_APP_STATE_ACTIVATING)
    {
		if (cnf->register_result == PBAP_CNF_SUCCESS)
		{			
			/*notify active success*/
			btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_ACTIVE);
			notify_event_to_application(MBTEVT_PBAP_SERVER_ENABLE_SUCCESS);  			
		}    
		else
		{	/*notify active fail*/
			btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_IDLE);
			notify_event_to_application(MBTEVT_PBAP_SERVER_ENABLE_FAIL);       
		}		
    }
   else
   {
   	 bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
	 MBT_ERR("[BTMTK] [PBAP]Enable Confirmation return at err state");
   }
	
}

void btmtk_pbap_handle_deactivate_cnf(void *pmsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_DEACTIVATING)
    {		
		memset(&(bt_pbap_app), 0x00, sizeof(BT_PBAP_APP_CTX));
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_IDLE);
		/*notify this event to the up layer*/
		notify_event_to_application(MBTEVT_PBAP_SERVER_DISABLE_SUCCESS);
	}
    else
   {
   	 bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
	 MBT_ERR("[BTMTK] [PBAP]Disable Confirmation return at err state");
   }
}


void btmtk_pbap_handle_authorize_ind(void *pmsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_authorize_ind_struct *ind = (bt_pbap_authorize_ind_struct*) pmsg; 
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    if (bt_pbap_app.server_state !=  BT_PBAP_APP_STATE_ACTIVE)
    {
        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
	PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Authorize Indication not at enable state, Rejected");
        btmtk_pbap_send_authorize_rsp(MOD_BT, PBAP_CNF_FAILED);		
    }
    else
    {
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_AUTHORIZING);
		
		OS_MemSet((U8*) bt_pbap_app.pbap_dev_name, 0, BT_PBAP_MAX_DEV_NAME_LEN);
		strcpy((S8*) bt_pbap_app.pbap_dev_name, (S8*) ind->dev_name);
		memcpy(&bt_pbap_app.pbap_bt_device, &ind->bd_addr, sizeof(bt_pbap_bd_addr_struct));
		#ifdef BTMTK_ON_WISE
		/*for test on the target to avoid  authrize in the gap mode*/
		btmtk_pbap_authrize_rsp(MBT_TRUE);
	       #else
		/* after updata the context data, notify the above layer,special here  */
		notify_event_to_application(MBTEVT_PBAP_SERVER_AUTHREQ);//
		/*for test on sdk to avoid  crash be obex authentiction
		btmtk_pbap_authrize_rsp(MBT_TRUE);*/
		#endif 		
	}
	
}


void btmtk_pbap_handle_client_connect_ind(void *pmsg)
{
	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_client_connect_ind_struct *ind = (bt_pbap_client_connect_ind_struct*) pmsg;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    if (bt_pbap_app.server_state != BT_PBAP_APP_STATE_AUTHORIZED)
    {		
        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
	PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Connect Indication not at authorize state, Rejected");
        btmtk_pbap_send_connect_rsp(MOD_BT, ind->cm_conn_id, PBAP_CNF_FAILED);
        notify_event_to_application(MBTEVT_PBAP_SERVER_CONNECT_FAIL);	
    }
    else
    {
		bt_pbap_app.cm_conn_id = ind->cm_conn_id;
		bt_pbap_app.current_path.storage  = PHONEBOOK_STORAGE_ROOT;
		bt_pbap_app.current_path.dir = PHONEBOOK_NONA;
		bt_pbap_app.pbap_bt_device = ind->bd_addr; 
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_CONNECTED);
		
		OS_MemSet((U8*) bt_pbap_app.pbap_dev_name, 0, BT_PBAP_MAX_DEV_NAME_LEN);
		strcpy((S8*) bt_pbap_app.pbap_dev_name, (S8*) ind->dev_name);
		memcpy(&bt_pbap_app.pbap_bt_device, &ind->bd_addr, sizeof(bt_pbap_bd_addr_struct));
		
		/*auto accept connect ind*/
		btmtk_pbap_send_connect_rsp(MOD_BT, ind->cm_conn_id, PBAP_CNF_SUCCESS);
		/*notify to application*/
		notify_event_to_application(MBTEVT_PBAP_SERVER_CONNECT_SUCCESS);
    }
}

static void btmtk_pbap_obex_auth_event_delay_timer(EvmTimer *timer)
{
	bt_pbap_app.obex_auth_timer.func = NULL;
	notify_event_to_application(MBTEVT_PBAP_SERVER_OBEX_AUTHREQ);
}


void btmtk_pbap_handle_client_auth_challenge_ind(void *pmsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_client_auth_challenge_ind_struct *ind =  (bt_pbap_client_auth_challenge_ind_struct *)pmsg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	bt_pbap_app.auth_option = ind->auth_option;	
	
  #ifdef  BTMTK_ON_WISE 
      /*target not process this event*/
	{
 	    U8  password[20] = {'0','0','0','0', 0x00};
            U8   userId[20] = {0x00};
	    btmtk_pbap_obex_auth_challege_rsp(FALSE, 
		password, 20,
		userId, 20); 
 	}  
 #else
      /*sdk will cashed if send gap authrize sent be for 
	 notify_event_to_application(MBTEVT_PBAP_SERVER_OBEX_AUTHREQ);*/
	bt_pbap_app.obex_auth_timer.func = btmtk_pbap_obex_auth_event_delay_timer;
        EVM_StartTimer(&(bt_pbap_app.obex_auth_timer), 2000);
 #endif

 
}


void btmtk_pbap_handle_disconnect_ind(void *pmsg)
{	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
   /* bt_pbap_disconnect_ind_struct *ind =  (bt_pbap_disconnect_ind_struct *)pmsg;*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	switch(bt_pbap_app.server_state)
	{
	case BT_PBAP_APP_STATE_AUTHORIZING:
	case BT_PBAP_APP_STATE_AUTHORIZED:
		//notify_event_to_application(MBTEVT_PBAP_SERVER_CONNECT_FAIL);
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_ACTIVE);
		break;
	case BT_PBAP_APP_STATE_CONNECTED:
	case BT_PBAP_APP_STATE_BUILDING_FOLDER:
	case BT_PBAP_APP_STATE_BUILDING_ENTRY:
	case BT_PBAP_APP_STATE_BUILDING_LIST:	
	case BT_PBAP_APP_STATE_DISCONNECTING:
		notify_event_to_application(MBTEVT_PBAP_SERVER_DISCONNECT_SUCCESS);
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_ACTIVE);
		break;
	default:
		break;
	}	
}


void btmtk_pbap_handle_abort_ind(void *pmsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	/*notify uplayer this event, and have considerate the state*/
	switch (bt_pbap_app.server_state)
    {
	case BT_PBAP_APP_STATE_BUILDING_LIST:
	case BT_PBAP_APP_STATE_BUILDING_ENTRY:
	case BT_PBAP_APP_STATE_BUILDING_FOLDER:
		notify_event_to_application(MBTEVT_PBAP_SERVER_OP_FAIL);
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_CONNECTED);
        break;
		/* not connected yet */
	default:
		break;
    }	
}

static void btmtk_pbap_handle_oper_success_ind(void *pmsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (bt_pbap_app.server_state)
    {
	case BT_PBAP_APP_STATE_BUILDING_LIST:
	case BT_PBAP_APP_STATE_BUILDING_ENTRY:
	case BT_PBAP_APP_STATE_BUILDING_FOLDER:
		notify_event_to_application(MBTEVT_PBAP_SERVER_OP_SUCCESS);
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_CONNECTED);
        break;
		/* not connected yet */
	default:
		break;
    }
}	

void btmtk_pbap_handle_set_path_ind(void *pmsg)
{
	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_set_path_ind_struct *ind = (bt_pbap_set_path_ind_struct*) pmsg;
    U8 result = 0x00;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	/*when recieve set path ind,assum support all the folder, not check with the appliction,
	for no operation match with application sdc*/	
    switch (ind->type)
    {
	case PBAP_ROOT_FOLDER:
		{
			bt_pbap_app.current_path.storage = PHONEBOOK_STORAGE_ROOT;
			bt_pbap_app.current_path.dir = PHONEBOOK_NONA;
			result = PBAP_CNF_SUCCESS;
		}
		break;
		
	case PBAP_BACK_FOLDER:
		{
			if(bt_pbap_app.current_path.dir != PHONEBOOK_NONA)
			{
             			bt_pbap_app.current_path.dir = PHONEBOOK_NONA;
			}
			else if(bt_pbap_app.current_path.storage == PHONEBOOK_STORAGE_SIM_TELECOM)
			{
				bt_pbap_app.current_path.storage = PHONEBOOK_STORAGE_SIM;
			}
			else
			{
				bt_pbap_app.current_path.storage = PHONEBOOK_STORAGE_ROOT;
			}	
			bt_pbap_app.current_path.dir = PHONEBOOK_NONA;
			result = PBAP_CNF_SUCCESS;
		}
		break;
		
	case PBAP_FORWARD_FOLDER:
		{
			result =btmtk_pbap_set_folder(ind->name);		  
		}
		break;
		
	default:
		result = PBAP_CNF_FAILED;
		break;
    }
    btmtk_pbap_send_set_path_rsp(MOD_BT, result);
}


void btmtk_pbap_handle_read_entry_ind(void *pmsg)
{
	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_read_entry_ind_struct *msg = (bt_pbap_read_entry_ind_struct*) pmsg;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/	
	
    if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_CONNECTED
		&&  bt_pbap_app.current_path.dir != PHONEBOOK_NONA)
    {
		memcpy(bt_pbap_app.vcard_entry.filter.byte, msg->filter.byte, PBAP_FILTER_SIZE);
		bt_pbap_app.vcard_entry.format = msg->format;
		
		/*check the storage*/
		bt_pbap_app.vcard_entry.storage =  bt_pbap_app.current_path.storage;
		bt_pbap_app.vcard_entry.dir=  bt_pbap_app.current_path.dir;	
		bt_pbap_app.vcard_entry.entry_index = btmtk_pbap_get_entry_index(msg->objectName);
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_BUILDING_ENTRY);
		/* after updata the context data, notify the above layer */
		notify_event_to_application(MBTEVT_PBAP_SERVER_OP_REQUEST);
    }
    else
    {
		/*response*/
		bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
		PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Pull vCard Entry Indication Return Fail");
		btmtk_pbap_send_read_entry_rsp(MOD_BT, PBAP_CNF_FAILED, NULL, 0x00);
		
    }
	
}

void btmtk_pbap_handle_read_folder_ind(void *pmsg)
{
	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_pbap_read_folder_ind_struct *ind = (bt_pbap_read_folder_ind_struct*) pmsg;
	PbPhonebookPath  dest_path; 
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/	
	if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_CONNECTED)
	{
		dest_path  =  btmtk_pbap_get_pb_folder(ind->pbName);
		if((dest_path.dir != PHONEBOOK_NONA)&&
			(dest_path.storage == PHONEBOOK_STORAGE_TELECOM ||dest_path.storage == PHONEBOOK_STORAGE_SIM_TELECOM))
		{
			bt_pbap_app.vcard_folder.storage = dest_path.storage;
			bt_pbap_app.vcard_folder.pb_name = dest_path.dir;
			btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_BUILDING_FOLDER);
			
			memcpy(bt_pbap_app.vcard_folder.filter.byte, ind->filter.byte, PBAP_FILTER_SIZE);
			bt_pbap_app.vcard_folder.format = ind->format;
			bt_pbap_app.vcard_folder.maxListCount = ind->maxListCount;
			bt_pbap_app.vcard_folder.listStartOffset = ind->listStartOffset;		
			/* after updata the context data, notify the above layer */
			notify_event_to_application(MBTEVT_PBAP_SERVER_OP_REQUEST);
		}
		else
		{
			/*response with fail*/
			        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
			PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Pull Phone book Indication Return Fail");
			btmtk_pbap_send_read_folder_rsp(MOD_BT, PBAP_CNF_FAILED, PBAP_INVALID_COUNT, PBAP_INVALID_COUNT, NULL, 0x00);
		}
	}
	else
	{
		/*resposet with fail*/
		        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
		PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Pull Phone book Indication Return Fail");
		btmtk_pbap_send_read_folder_rsp(MOD_BT, PBAP_CNF_FAILED, PBAP_INVALID_COUNT, PBAP_INVALID_COUNT,  NULL, 0x00);
    }
	
}



void btmtk_pbap_handle_read_list_ind(void *pmsg)
{
	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    bt_pbap_read_list_ind_struct *ind = (bt_pbap_read_list_ind_struct*) pmsg;
    PbapPhonebookName dest_pb; 
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
	if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_CONNECTED)
	{
		
		dest_pb = btmtk_pbap_get_vcardlist_dir((U16*)ind->folderName);
		if(dest_pb!= PHONEBOOK_NONA)
		{
			bt_pbap_app.vcard_listing.dir = dest_pb;
		}
		else if(bt_pbap_app.current_path.dir != PHONEBOOK_NONA)
		{
			bt_pbap_app.vcard_listing.dir  = bt_pbap_app.current_path.dir;
		}
		
		if(bt_pbap_app.vcard_listing.dir != PHONEBOOK_NONA)
		{
			bt_pbap_app.vcard_listing.searchValueLength = 
				(ind->searchValueLength >= MAX_PBAP_SEARCH_VALUE_LENGTH)? MAX_PBAP_SEARCH_VALUE_LENGTH : ind->searchValueLength;
			bt_pbap_app.vcard_listing.searchAttribute = ind->searchAttribute;
			bt_pbap_app.vcard_listing.order = ind->order;
			bt_pbap_app.vcard_listing.maxListCount = ind->maxListCount;
			bt_pbap_app.vcard_listing.listStartOffset = ind->listStartOffset;
			bt_pbap_app.vcard_listing.storage = bt_pbap_app.current_path.storage;
			OS_MemSet(bt_pbap_app.vcard_listing.searchValue, 0x00, MAX_PBAP_SEARCH_VALUE_LENGTH+1);
			memcpy(bt_pbap_app.vcard_listing.searchValue, ind->searchValue, 
				bt_pbap_app.vcard_listing.searchValueLength);
			/* after updata the context data, notify the above layer */
			 bt_pbap_app.cur_list_Count = -1;
			btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_BUILDING_LIST);
			notify_event_to_application(MBTEVT_PBAP_SERVER_OP_REQUEST);
		}
		else
		{
		        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
			PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Pull vCard Listing Indication Return Fail");
			btmtk_pbap_send_read_list_rsp(MOD_BT, PBAP_CNF_FAILED, PBAP_INVALID_COUNT, PBAP_INVALID_COUNT, NULL, 0x00);
		}
	}
	else
	{
	        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
		PBAP_MBT_LOG("[BTMTK] [PBAP]Recieve Pull vCard Listing Indication Return Fail");
		btmtk_pbap_send_read_list_rsp(MOD_BT, PBAP_CNF_FAILED, PBAP_INVALID_COUNT, PBAP_INVALID_COUNT, NULL, 0x00);
	}
	
	/*notify the aplication*/
}

void btmtk_pbap_active_req(U8 security_level, U8 support_repos)
{

	if(btmtk_pbap_app_get_state() == BT_PBAP_APP_STATE_IDLE)
	{
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_ACTIVATING); 
		PBAP_MBT_LOG("[BTMTK] [PBAP] Externl ADPSend Enable Requst to internal ADP");
		btmtk_pbap_send_active_req(MOD_BT, security_level, support_repos);
	}
      else
      {
      	 MBT_ERR("[BTMTK] [PBAP]Enable Request at Wrong state");
   	 bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
      }

}

void btmtk_pbap_deactive_req(void)
{
	
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/	
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check all connections state */
    /*just for test if could sent disable fail to UI
    btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_AUTHORIZING);*/
	
    /**/
   if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_AUTHORIZING)
    {
    	notify_event_to_application( MBTEVT_PBAP_SERVER_DISABLE_FAIL);    	
    }
    else if(bt_pbap_app.server_state != BT_PBAP_APP_STATE_IDLE 
		&& bt_pbap_app.server_state != BT_PBAP_APP_STATE_DEACTIVATING)
    {
		PBAP_MBT_LOG("[BTMTK] [PBAP] External ADP Send Disable Requst to internal ADP");
		btmtk_pbap_send_deactive_req(MOD_BT);
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_DEACTIVATING);
    } 
       else
   {
   	 bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
   }
}


void btmtk_pbap_authrize_rsp(U8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/	
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(btmtk_pbap_app_get_state() == BT_PBAP_APP_STATE_AUTHORIZING)
	{
		
		if (cnf_code == MBT_TRUE)
		{
			PBAP_MBT_LOG("[BTMTK] [PBAP] External ADP Send Authorize Response to internal ADP");
			btmtk_pbap_send_authorize_rsp(MOD_BT, PBAP_CNF_SUCCESS);
			btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_AUTHORIZED);
		}
		else
		{
		        bt_trace(TRACE_FUNC, BT_PBAP_MBT_RSP_WITH_FAIL);
			PBAP_MBT_LOG("[BTMTK] [PBAP] External ADP Send Authorize Response to internal ADP");
			btmtk_pbap_send_authorize_rsp(MOD_BT, PBAP_CNF_FAILED);
			btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_ACTIVE); 
		}		
	}
       else
      {
         MBT_ERR("[BTMTK] [PBAP] External ADP Recive authrize response in error state");
   	 bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
      }
}

/**/
void btmtk_pbap_disconnect_req(BT_BOOL disconnect_tp_directly)
{
	
	U32 cm_conn_id =bt_pbap_app.cm_conn_id;
	
	switch (bt_pbap_app.server_state)
	{           
	case BT_PBAP_APP_STATE_CONNECTED:
	case BT_PBAP_APP_STATE_AUTHORIZED:
	case BT_PBAP_APP_STATE_BUILDING_LIST:
	case BT_PBAP_APP_STATE_BUILDING_FOLDER:
	case BT_PBAP_APP_STATE_BUILDING_ENTRY:
		btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_DISCONNECTING);
		PBAP_MBT_LOG("[BTMTK] [PBAP]External ADP Send Disconnect Requst to internal ADP");
		btmtk_pbap_send_disconnect_req(MOD_BT, cm_conn_id, (kal_bool)disconnect_tp_directly);
		break;
	case BT_PBAP_APP_STATE_AUTHORIZING:
		/*in this state,disconnect  will lead to problem*/
		break;
	default:
		break;
	}
	
}


void btmtk_pbap_read_entry_rsp(U8 result)
{	
	if(btmtk_pbap_app_get_state() == BT_PBAP_APP_STATE_BUILDING_ENTRY)
	{
		btmtk_pbap_send_read_entry_rsp(MOD_BT, result,
			(U8*)bt_pbap_app_file_path, 
			sizeof(bt_pbap_app_file_path));		
	}
	 else
       {
           MBT_ERR("[BTMTK] [PBAP] External ADP recieve  pull vcard entry response in error state");
   	   bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
	}
}


void btmtk_pbap_read_folder_rsp(U8 result,
								U16 phoneBookSize,
								U16 newMissedCalls)
{
	
	if(btmtk_pbap_app_get_state() == BT_PBAP_APP_STATE_BUILDING_FOLDER)
	{
		/*if client not request missed call history , set new missed call number to 0xffff,  then no sent to client */
		if(bt_pbap_app.vcard_folder.pb_name != PHONEBOOK_MCH)
		{
			newMissedCalls = PBAP_INVALID_COUNT;
		}
		btmtk_pbap_send_read_folder_rsp(MOD_BT,
			result,
			phoneBookSize,
			newMissedCalls,
			(U8*)bt_pbap_app_file_path, 
			sizeof(bt_pbap_app_file_path));
	}
	else
      {
            MBT_ERR("[BTMTK] [PBAP] External ADP recieve pull phone book response in error state");
   	    bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
	}
	
}

void btmtk_pbap_read_list_rsp(U8 result,
							  U16 phoneBookSize,
							  U16 newMissedCalls)
{
	if(btmtk_pbap_app_get_state() == BT_PBAP_APP_STATE_BUILDING_LIST)
	{
		/*if client not request missed call history , set new missed call number to 0xffff,  then no sent to client */
		if(bt_pbap_app.vcard_listing.dir != PHONEBOOK_MCH)
		{
			newMissedCalls = PBAP_INVALID_COUNT;
		}
		btmtk_pbap_send_read_list_rsp(MOD_BT,
			result,
			phoneBookSize,
			newMissedCalls, 
			(U8*)bt_pbap_app_file_path, 
			sizeof(bt_pbap_app_file_path));
	}
	 else
        {
             MBT_ERR("[BTMTK] [PBAP] External ADP recieve pull phone book response in error state");
   	     bt_trace(TRACE_FUNC, BT_PBAP_MBT_STATA_ERR);
	}
}

void btmtk_pbap_obex_auth_challege_rsp(U8 cancel,
									   U8* password,  U16 password_length,
									   U8* userID, U16 userID_length)
{
        PBAP_MBT_LOG("[BTMTK] [PBAP] External Send  Obex Authentication Response Internal ADP");
	btmtk_pbap_send_obex_auth_challege_rsp(MOD_BT, 
		cancel,  password,  password_length,  userID,  userID_length);
}

void btmtk_init_pbap(void)
{
	memset(&bt_pbap_app, 0, sizeof(BT_PBAP_APP_CTX));
	bt_pbap_app.cm_conn_id = 0xff;
	btmtk_pbap_app_set_state(BT_PBAP_APP_STATE_IDLE);
}

void  btmtk_deinit_pbap(void)
{
	btmtk_init_pbap();
}

static void btmtk_pbap_string_swapcpy(U8* dest_str, const U8* src_str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i;
    U32 bytes_len = ext_ucs2strlen((S8*) src_str) * 2;
    U8 t;	
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < bytes_len; i += 2)    /* includes null temination */
    {
        /* dest_str pointer may be same as src_str*/
        t = src_str[i];
        dest_str[i] = src_str[i + 1];
        dest_str[i + 1] = t;
    }
	
    dest_str[bytes_len] = 0;
    dest_str[bytes_len + 1] = 0;
    
}

/*****************************************************************************
* FUNCTION
*  app_ucs2_str_to_asc_str
* DESCRIPTION
*  The function is used for convert UCS2 string to ANSII string. 
*  The caller need to make sure the pOutBuffer  size must have enough space 
*  or the function causes the memory corruption. The function will add the 
*  terminated character at the end of pOutBuffer array. The byte order of 
*  UCS2 character(input param) is little endian.
* PARAMETERS
*  pOutBuffer      [OUT]   ANSII destination string  
*  pInBuffer       [IN]    UCS2 source string 
* RETURNS
*  Return the bytes to convert.
*****************************************************************************/
static U16 btmtk_pbap_ucs2_str_to_asc_str(S8 *pOutBuffer,S8* pInBuffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    U16 count = 0;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (!((*pInBuffer == 0) && (*(pInBuffer + 1) == 0)))
    {
        *pOutBuffer = *(pInBuffer);
		
#ifdef __FOR_TESTING    /* MMI_ON_HARDWARE_P */
        *pOutBuffer = *(pInBuffer + 1);
#endif 
        pInBuffer += 2;
        pOutBuffer++;
        count++;
    }
	
    *pOutBuffer = 0;
    return count;
}


/*****************************************************************************
* FUNCTION
*  btmtk_pbap_get_entry_index
* DESCRIPTION
*  get the index of the phone book entry from the name,now assume the number in path is decimal
*  No path information shall be included since the PullvCardEntry function uses relative path
* information
* PARAMETERS
*  path
* RETURNS
*  U32
*****************************************************************************/

static U32 btmtk_pbap_get_entry_index(U16 * path)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
#define GETNUM(char_d)(char_d -'0')
	BT_BOOL flag = FALSE;
       U32 temp_index = 0x0;
	if(path != NULL)
	{
		U16 i = 0;
		U8 asc_name[PBAP_MAX_NAME_LEN/2 + 1];
		
		btmtk_pbap_string_swapcpy((U8*)path,  (U8*)path);
		btmtk_pbap_ucs2_str_to_asc_str((S8 *)asc_name, (S8 *)path);
		while(asc_name[i] != '.' )
		{
			if(GETNUM(asc_name[i] )<0 ||GETNUM(asc_name[i] )>9)
			{
				flag = FALSE;
				break;
			}
			temp_index = temp_index*10 + GETNUM(asc_name[i] );
			i++;
			flag = TRUE;
		}
	}
	if(!flag)
		temp_index = 0xffff;
	return temp_index;
}

/*****************************************************************************
* FUNCTION
*  btmtk_pbap_set_folder
* DESCRIPTION
*  allows only for setting the current folder to the root, parent or a child folder
*  
* PARAMETERS
*  path
* RETURNS
*  U8
*****************************************************************************/
static U8 btmtk_pbap_set_folder(U16*path)
{    
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	PbPhonebookPath cur_path; 
	U8 asc_name[PBAP_MAX_NAME_LEN/2 + 1];
	U8 result = PBAP_CNF_FAILED; 
	PbapPhonebookStrorage storage =  PHONEBOOK_STORAGE_ROOT;
	PbapPhonebookName dir    =   PHONEBOOK_NONA;
	if(path == NULL)
	{	
		goto exit;
	}
	
	btmtk_pbap_string_swapcpy((U8*)path,  (U8*)path);
	btmtk_pbap_ucs2_str_to_asc_str((S8 *)asc_name, (S8 *)path);
	
	if (strcmp((char*)asc_name, "telecom") == 0)
	{
		storage =  PHONEBOOK_STORAGE_TELECOM;
	}
	else if (strcmp((char*)asc_name, "SIM1") == 0)
	{
		storage =  PHONEBOOK_STORAGE_SIM;
	}
	else if (strcmp((char*)asc_name, "pb") == 0)
	{
		dir =  PHONEBOOK_PB;
	}
	else if (strcmp((char*)asc_name, "ich") == 0)
	{
		dir =  PHONEBOOK_ICH;
	}
	else if (strcmp((char*)asc_name, "och") == 0)
	{
		dir =  PHONEBOOK_OCH;
	}
	else if (strcmp((char*)asc_name, "mch") == 0)
	{
		dir =  PHONEBOOK_MCH;
	}
	else if (strcmp((char*)asc_name, "cch") == 0)
	{
		dir =  PHONEBOOK_CCH;
	}
	else
	{	/*path exist, but the value  is invalid */
		dir = PHONEBOOK_INVALID;
		goto exit;
	}
	cur_path  =  bt_pbap_app.current_path; 
	switch(cur_path.storage)
	{
	case PHONEBOOK_STORAGE_ROOT:
		{
			if(storage == PHONEBOOK_STORAGE_TELECOM ||storage == PHONEBOOK_STORAGE_SIM)
			{
				bt_pbap_app.current_path.storage = storage;
				bt_pbap_app.current_path.dir = PHONEBOOK_NONA;
				result = PBAP_CNF_SUCCESS;
			}
		}
		break;	
	case PHONEBOOK_STORAGE_SIM:
		{
			if(storage == PHONEBOOK_STORAGE_TELECOM)
			{
				bt_pbap_app.current_path.storage = PHONEBOOK_STORAGE_SIM_TELECOM;
				bt_pbap_app.current_path.dir = PHONEBOOK_NONA;
				result = PBAP_CNF_SUCCESS;
			}
		}
		break;
	case PHONEBOOK_STORAGE_TELECOM:
	case PHONEBOOK_STORAGE_SIM_TELECOM:
		{
			if(bt_pbap_app.current_path.dir == PHONEBOOK_NONA
				&& dir != PHONEBOOK_NONA)
			{
				bt_pbap_app.current_path.dir = dir;
				result = PBAP_CNF_SUCCESS;
			}
		}
		break;
	default:
		break;
	}
	
exit:
	return result;
		  
}
/*****************************************************************************
* FUNCTION
*  btmtk_pbap_get_vcardlist_dir
* DESCRIPTION
*  in the pull vcardlisting(browse phone book) operation, as spec indicate the name value shall
* not contain any path information, uses  relative paths, empty name header is acceptable
* the other name is invalid
*  
* PARAMETERS
*  path
* RETURNS
*  void
*****************************************************************************/
static PbapPhonebookName btmtk_pbap_get_vcardlist_dir(U16*path)
{
	PbapPhonebookName dest_dir   =   PHONEBOOK_NONA;
    
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	if(path != NULL)
	{
		U8 asc_name[PBAP_MAX_NAME_LEN/2 + 1];
		btmtk_pbap_string_swapcpy((U8*)path,  (U8*)path);
		btmtk_pbap_ucs2_str_to_asc_str((S8 *)asc_name, (S8 *)path);
		if(asc_name[0] == 0x00)
		{
			dest_dir =  PHONEBOOK_NONA;
		}
		else if (strcmp((char*)asc_name, "pb") == 0)
		{
			dest_dir =  PHONEBOOK_PB;
		}
		else if (strcmp((char*)asc_name, "ich") == 0)
		{
			dest_dir =  PHONEBOOK_ICH;
		}
		else if (strcmp((char*)asc_name, "och") == 0)
		{
			dest_dir =  PHONEBOOK_OCH;
		}
		else if (strcmp((char*)asc_name, "mch") == 0)
		{
			dest_dir =  PHONEBOOK_MCH;
		}
		else if (strcmp((char*)asc_name, "cch") == 0)
		{
			dest_dir =  PHONEBOOK_CCH;
		}
		else
		{	/*path exist, but the value  is invalid */
			dest_dir = PHONEBOOK_INVALID;
		}
	}
	
	return dest_dir;
}


/*****************************************************************************
* FUNCTION
*  btmtk_pbap_get_pb_folder
* DESCRIPTION
*  in the pull phone book(download phone book) operation, as spec indicate the name contain
* the absolute path in the virtual architecture of the PSE
*  
* PARAMETERS
*  path
* RETURNS
*  void
*****************************************************************************/
static PbPhonebookPath btmtk_pbap_get_pb_folder(U16*path)
{
	/*-----------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	PbPhonebookPath dest_path;
	dest_path.storage =  PHONEBOOK_STORAGE_ROOT;
	dest_path.dir    =   PHONEBOOK_NONA;
	if( path != NULL)
	{	U16 i;
	U8 asc_name[PBAP_MAX_NAME_LEN/2 + 1] = {0};
	i = 0;
	btmtk_pbap_string_swapcpy((U8*)path,  (U8*)path);
	btmtk_pbap_ucs2_str_to_asc_str((S8 *)asc_name, (S8 *)path);
	if (memcmp(asc_name, "telecom", 7) == 0)
	{
		dest_path.storage =  PHONEBOOK_STORAGE_TELECOM;
		i = 8;
	}
	else if (memcmp(asc_name, "SIM1/telecom", 12) == 0)
	{
		dest_path.storage =  PHONEBOOK_STORAGE_SIM_TELECOM;
		i = 13;
	}
	/*strcmp to confirm that, it is the end of the name string*/
	if (strcmp((char*)(asc_name+i), "pb.vcf") == 0)
	{
		dest_path.dir =  PHONEBOOK_PB;
	}
	else if (strcmp((char*)(asc_name+i), "ich.vcf") == 0)
	{
		dest_path.dir =  PHONEBOOK_ICH;
	}
	else if (strcmp((char*)(asc_name+i), "och.vcf") == 0)
	{
		dest_path.dir =  PHONEBOOK_OCH;
	}
	else if (strcmp((char*)(asc_name+i), "mch.vcf") == 0)
	{
		dest_path.dir =  PHONEBOOK_MCH;
	}
	else if (strcmp((char*)(asc_name+i), "cch.vcf") == 0)
	{
		dest_path.dir =  PHONEBOOK_CCH;
	}
	/*not point out the invalid value, for not need to do*/
	}
	
	return dest_path;
}

/*****************************************************************************
* FUNCTION
*  btmtk_pbap_handle_operation
* DESCRIPTION
*  after application Update SDC, handle the result data in SDC 
* PARAMETERS
*  msg_id(response message  to the indication)
* RETURNS
*  void
*****************************************************************************/
void btmtk_pbap_handle_operation(void)
{
	/*Update SDC data according the contex and */
    T_MBT_PBAP_STATUS *sdcPBAPStatus = 
        (T_MBT_PBAP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_PBAP_STATUS);
	
	switch(bt_pbap_app.server_state)
	{
		
	case BT_PBAP_APP_STATE_BUILDING_ENTRY:
		if(sdcPBAPStatus->vCardOut.Result == TRUE)
		{
			btmtk_pbap_read_vcarddata_from_application();			
			btmtk_pbap_read_entry_rsp(PBAP_CNF_SUCCESS);
		}
		else
		{
			btmtk_pbap_read_entry_rsp(PBAP_CNF_FAILED);
		}
		break;
		
	case BT_PBAP_APP_STATE_BUILDING_FOLDER:
		
		
		if(sdcPBAPStatus->vCardOut.Result == FALSE)
		{
			btmtk_pbap_read_folder_rsp(PBAP_CNF_FAILED, PBAP_INVALID_COUNT, PBAP_INVALID_COUNT);	
		}
		else
		{
			if(bt_pbap_app.vcard_folder.maxListCount == 0x00)
			{
				btmtk_pbap_read_folder_rsp(PBAP_CNF_SUCCESS, 
					sdcPBAPStatus->vCardOut.vCardTotalCount, 
					sdcPBAPStatus->vCardOut.NewMissedCallCount);
				sdcPBAPStatus->vCardOut.vCardTotalCount = 0x00;
				return;
			}
			
			btmtk_pbap_read_vcarddata_from_application();
			if(sdcPBAPStatus->vCardOut.StorageType!= MBT_MEMORY_BUFFER ||sdcPBAPStatus->vCardOut.vCardTotalCount == 0x00)
			{
				btmtk_pbap_read_folder_rsp(PBAP_CNF_SUCCESS, PBAP_INVALID_COUNT, sdcPBAPStatus->vCardOut.NewMissedCallCount);
			}
		}
		break;
		
		
	case BT_PBAP_APP_STATE_BUILDING_LIST:
		
		if(sdcPBAPStatus->vCardOut.Result == FALSE)
		{
			btmtk_pbap_read_list_rsp(PBAP_CNF_FAILED, PBAP_INVALID_COUNT, PBAP_INVALID_COUNT);	
		}
		else
		{	
			if(bt_pbap_app.vcard_listing.maxListCount == 0x00)
			{
				btmtk_pbap_read_list_rsp(PBAP_CNF_SUCCESS, 
					sdcPBAPStatus->vCardOut.vCardTotalCount, 
					sdcPBAPStatus->vCardOut.NewMissedCallCount);
				return;			     
			}
			#ifdef BTMTK_ON_WISE
			if((bt_pbap_app.vcard_listing.maxListCount > 0x00 )&&(sdcPBAPStatus->Operation ==  MBT_PBAP_OP_PULL_DIRECTORY_NUM))
			{
				notify_event_to_application(MBTEVT_PBAP_SERVER_OP_SUCCESS);
				bt_pbap_app.cur_list_Count = (S16)sdcPBAPStatus->vCardOut.vCardTotalCount;
				notify_event_to_application( MBTEVT_PBAP_SERVER_OP_REQUEST);
				return;
			}
			#endif
			btmtk_pbap_read_vcardlist_from_application();
			if(sdcPBAPStatus->vCardOut.StorageType!= MBT_MEMORY_BUFFER ||
				(sdcPBAPStatus->vCardOut.vCardListMergedCount == sdcPBAPStatus->vCardOut.vCardListMergedCount))
			{
				btmtk_pbap_read_list_rsp(PBAP_CNF_SUCCESS, PBAP_INVALID_COUNT, sdcPBAPStatus->vCardOut.NewMissedCallCount);			   	
			}
		}
		break;
		
	default:
		break;
	}
}

/*****************************************************************************
* FUNCTION
*  btmtk_pbap_read_vcarddata_from_application
* DESCRIPTION
*  read SDC data,  in buffer mod, write the data in the temp file, in file mod, give the file 
*  name to internal adp
*  need to adjust according appliction, for now it is abnormal
*  phone book or entry store in vcard
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
static void btmtk_pbap_read_vcarddata_from_application(void)
{
	U8 temp_pbap_data_file[BT_PBAP_MAX_FILEPATH_NAME_LEN];
	T_MBT_PBAP_STATUS *sdcPBAPStatus = 
        (T_MBT_PBAP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_PBAP_STATUS);
	if(sdcPBAPStatus->vCardOut.StorageType == MBT_MEMORY_BUFFER
		||((sdcPBAPStatus->vCardOut.StorageType != MBT_INTERNAL_FS)
		&&(sdcPBAPStatus->vCardOut.StorageType != MBT_EXTERNAL_FS)))
	{
		FHANDLE  fhandle;
		S32 buffer_len;
		S32 write_len;
		/*in buffer transfer mod if the temp file not exit, fist cread the file , else write data to file
		in this design is write application buffer data to file.
		on the other case may transfer buffer data to internal adp by buffer.
		*/
		if(!ext_fs_is_file_exist_ucs2((U16*)PBAP_ADP_FILE_NAME))
		{
			U32 name_len;
			
			fhandle = ext_fs_open_ucs2((U16*)PBAP_ADP_FILE_NAME, EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS);
			ext_fs_close_ucs2(fhandle);
			OS_MemSet(temp_pbap_data_file, 0x00, sizeof(temp_pbap_data_file));
			name_len = ext_ucs2strlen((S8*)PBAP_ADP_FILE_NAME);
			ext_ucs2ncpy( (S8*)temp_pbap_data_file, (const S8*)PBAP_ADP_FILE_NAME, name_len+1);	
		}
		
		if(sdcPBAPStatus->vCardOut.vCard)
		{
			buffer_len = strlen((char*)sdcPBAPStatus->vCardOut.vCard);
			fhandle = ext_fs_open_ucs2((U16*)PBAP_ADP_FILE_NAME, EXT_FS_READ_WRITE);
			ext_fs_write_ucs2(fhandle, sdcPBAPStatus->vCardOut.vCard, buffer_len, &write_len);
			ext_fs_close_ucs2(fhandle);
		}
		btmtk_os_memcpy(bt_pbap_app_file_path, temp_pbap_data_file, sizeof(bt_pbap_app_file_path));
	}
	else if((sdcPBAPStatus->vCardOut.StorageType == MBT_INTERNAL_FS)
		||(sdcPBAPStatus->vCardOut.StorageType == MBT_EXTERNAL_FS))
	{	/*in the file transfer mod, transfer the file and dir name to internal adp*/
		U32 dir_len,file_len;
		U8 full_file_path[BT_PBAP_MAX_FILEPATH_NAME_LEN] = {0x00};
		/*in the file transfer mod, transfer the file and dir name to internal adp*/
		dir_len = strlen(sdcPBAPStatus->vCardOut.DirName);
		file_len =  strlen(sdcPBAPStatus->vCardOut.FileName);
	        if(dir_len+file_len < BT_PBAP_MAX_FILEPATH_NAME_LEN-2)
		{
		       strcpy((char*)full_file_path, sdcPBAPStatus->vCardOut.DirName);
		       full_file_path[dir_len] = '/';
		       memcpy((full_file_path+dir_len+1), (sdcPBAPStatus->vCardOut.FileName), (file_len+1));
		}
		ext_chset_utf8_to_ucs2_string( (U8 *)temp_pbap_data_file, sizeof(temp_pbap_data_file) -2, full_file_path);		
		#ifdef BTMTK_ON_WISE
		{			
			wsConvertFileNameToNativeFormat((U16 *)bt_pbap_app_file_path, (U8 *)temp_pbap_data_file);
					
		}
		#else
		{
			OS_MemCopy(bt_pbap_app_file_path,  temp_pbap_data_file,sizeof(bt_pbap_app_file_path));	
		}
		#endif
		/*read data in file*/
	}
		
}

/*****************************************************************************
* FUNCTION
*  btmtk_pbap_read_vcardlist_from_application
* DESCRIPTION
*  read SDC data,  in buffer mod, first build vcardlist  into xml  format, and  write the data in the temp file,
*  in file mod, give the file name to internal adp
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
static void btmtk_pbap_read_vcardlist_from_application(void)
{
    U8 temp_pbap_data_file[BT_PBAP_MAX_FILEPATH_NAME_LEN];
    T_MBT_PBAP_STATUS *sdcPBAPStatus = 
        (T_MBT_PBAP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_PBAP_STATUS);
    
	if(sdcPBAPStatus->vCardOut.StorageType == MBT_MEMORY_BUFFER
		||((sdcPBAPStatus->vCardOut.StorageType != MBT_INTERNAL_FS)
		&&(sdcPBAPStatus->vCardOut.StorageType != MBT_EXTERNAL_FS)))
	{
		FHANDLE  fhandle;
		S32 write_len;
		/*in buffer transfer mod if the temp file not exit, fist cread the file , else write data to file
		in this design is write application buffer data to file.
		on the other case may transfer buffer data to internal adp by buffer.
		*/
		if(!ext_fs_is_file_exist_ucs2((U16*)PBAP_ADP_FILE_NAME))
		{
			S32 name_len;
			fhandle = ext_fs_open_ucs2((U16*)PBAP_ADP_FILE_NAME, EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS);
			ext_fs_close_ucs2(fhandle);
			OS_MemSet(temp_pbap_data_file, 0x00, sizeof(temp_pbap_data_file));
			name_len = ext_ucs2strlen((S8*)PBAP_ADP_FILE_NAME);
			ext_ucs2ncpy( (S8*)temp_pbap_data_file, (const S8*)PBAP_ADP_FILE_NAME, name_len+1);	
		}
		/* build vcard list to xml doc by exertanl adp in the buffer transfer mod */
		if(sdcPBAPStatus->vCardOut.vCardList)
		{
			U16 index;
			U16 merge_count;
			S8  listrecord[256];
			T_MBT_PBAP_VCARD_LIST *vCardList = (T_MBT_PBAP_VCARD_LIST *)sdcPBAPStatus->vCardOut.vCardList;
			merge_count = sdcPBAPStatus->vCardOut.vCardListMergedCount;
			fhandle = ext_fs_open_ucs2((U16*)PBAP_ADP_FILE_NAME, EXT_FS_READ_WRITE);
			ext_fs_write_ucs2(fhandle, BEGIN_XML_DECL, strlen((char*)BEGIN_XML_DECL), &write_len);
			for(index = 0; index < merge_count; index++)
			{
				strcpy(listrecord, BEGIN_HANDLE);
				strcat(listrecord, vCardList->ListHandler);
				strcat(listrecord, MIDDLE_HANDLE);
				strcat(listrecord, vCardList->Name);
				strcat(listrecord, END_HANDLE);
				ext_fs_write_ucs2(fhandle, listrecord, strlen((char*)listrecord), &write_len);
				vCardList++;
			}
			ext_fs_write_ucs2(fhandle, END_FOLDER_DECL, strlen((char*)END_FOLDER_DECL), &write_len);			
			ext_fs_close_ucs2(fhandle);
		}
		btmtk_os_memcpy(bt_pbap_app_file_path, temp_pbap_data_file, sizeof(bt_pbap_app_file_path));
	}
	else if((sdcPBAPStatus->vCardOut.StorageType == MBT_INTERNAL_FS)
		||(sdcPBAPStatus->vCardOut.StorageType == MBT_EXTERNAL_FS))
	{
 #ifdef BTMTK_ON_WISE
	              U16 index;
		      FHANDLE  fhandle;
		      //FHANDLE entry_fhandle;
		      S32 write_len;
		      S32 name_len;
		      U8 dir_name[BT_PBAP_MAX_FILEPATH_NAME_LEN/2];
		      U8 file_path[BT_PBAP_MAX_FILEPATH_NAME_LEN/2];
		      U8 ucs2_file_path[BT_PBAP_MAX_FILEPATH_NAME_LEN];
		      U8 file_name[16];
		      S8  listrecord[256];
		       btmtk_vcard_data_struct  vcard_data;
		       strcpy((char*)dir_name, sdcPBAPStatus->vCardOut.DirName);			
			strcat((char*)dir_name,"/");			
                        fhandle = ext_fs_open_ucs2((U16*)PBAP_ADP_FILE_NAME, EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS);
			ext_fs_write_ucs2(fhandle, BEGIN_XML_DECL, strlen((char*)BEGIN_XML_DECL), &write_len);
			for(index = 0; index < (bt_pbap_app.cur_list_Count+1); index++)
			{
				strcpy((char*)file_path, (char*)dir_name);			
				sprintf((char*)file_name, "%d.vcf", index);
				strcat((char*)file_path,(char*)file_name);
				ext_chset_utf8_to_ucs2_string(ucs2_file_path, sizeof(ucs2_file_path)-2, file_path);
				wsConvertFileNameToNativeFormat((U16 *)temp_pbap_data_file,(U8 *)ucs2_file_path);
	  			if(ext_fs_is_file_exist_ucs2((U16*)temp_pbap_data_file))
				{
				 	S32 ret;
					ret = btmtk_vcard_parse_file_to_struct((U16*)temp_pbap_data_file, &vcard_data);
                               	strcpy(listrecord, BEGIN_HANDLE);
					strcat(listrecord, (char*)file_name);
					strcat(listrecord, MIDDLE_HANDLE);
					if(ret == BTMTK_VCD_ERR_OK)
					{					 
						strcat(listrecord, (char*)vcard_data.first_name);
						strcat(listrecord, (char*)vcard_data.second_name);	
				        	strcat(listrecord, (char*)vcard_data.third_name);	
						strcat(listrecord, (char*)vcard_data.fourth_name);	
						strcat(listrecord, (char*)vcard_data.fifth_name);
					}
					else
					{ 					   
				   	strcat(listrecord, " ");					 
					}
					strcat(listrecord, END_HANDLE);
					ext_fs_write_ucs2(fhandle, listrecord, strlen((char*)listrecord), &write_len);
				 	
				}
			}
			ext_fs_write_ucs2(fhandle, END_FOLDER_DECL, strlen((char*)END_FOLDER_DECL), &write_len);			
			ext_fs_close_ucs2(fhandle);
			name_len = ext_ucs2strlen((S8*)PBAP_ADP_FILE_NAME);
			ext_ucs2ncpy((S8*)bt_pbap_app_file_path, (const S8*)PBAP_ADP_FILE_NAME, (name_len+1));	
#endif	
	}

}
	
	
	/*****************************************************************************
	* FUNCTION
	*  btmtk_pbap_Update_req_path
	* DESCRIPTION
	*  translate the storage(telecom/sim)and dir(pb/ich/och/mch/cch) to full path
	* PARAMETERS
	*  path
	*  storage
	*  dir
	* RETURNS
	*  void
	*****************************************************************************/
	static void btmtk_pbap_update_req_path(U8* path, U8 storage, U8 dir )
	{
		U8 ret;
		U32 len;
		U8* req_path = NULL;
		ret = storage*5+dir;
		switch(ret)
		{
		case 0:
			req_path = "/root/telecom/pb";
			break;
		case 1:
			req_path = "/root/telecom/ich";
			break;
		case 2:
			req_path = "/root/telecom/och";
			break;
		case 3:
			req_path = "/root/telecom/mch";
			break;
		case 4:
			req_path = "/root/telecom/cch";
			break;
		case 5:
			req_path = "/root/SIM1/telecom/pb";
			break;
		case 6:
			req_path = "/root/SIM1/telecom/ich";
			break;
		case 7:
			req_path = "/root/SIM1/telecom/mch";
			break;
		case 8:
			req_path = "/root/SIM1/telecom/mch";
			break;
		case 9:
			req_path = "/root/SIM1/telecom/cch";
			break;
		default:
			req_path = "";
			break;
		}
		
		len = strlen((char*)req_path);
		memcpy(path, req_path, len);
		
	}


	/*****************************************************************************
	* FUNCTION
	*  notify_event_to_application
	* DESCRIPTION
	*  updata the SDC according external adp context data, then sent event to appliction
	* PARAMETERS
	*  event_id 
	* RETURNS
	*  void
	*****************************************************************************/
	static void notify_event_to_application(T_MBTEVT event_id)
	{	
		/*update SDC data according the contex and */
		T_MBT_PBAP_STATUS *sdcPBAPStatus = 
			(T_MBT_PBAP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_PBAP_STATUS);	
		switch(event_id)
		{
		case MBTEVT_PBAP_SERVER_ENABLE_SUCCESS:
			{
				PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus State  from %d to %d ",	
						sdcPBAPStatus->State,MBT_PBAP_STATE_ENABLED);				
				PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus bEnabled from %d to %d ",
						sdcPBAPStatus->bEnabled, MBT_TRUE);
				
				sdcPBAPStatus->State = MBT_PBAP_STATE_ENABLED;
				sdcPBAPStatus->bEnabled  = MBT_TRUE;
				sdcPBAPStatus->bIsObexUserID = MBT_FALSE;

				
				/* Initialize all Fields */ 
			}
			break;
		case MBTEVT_PBAP_SERVER_ENABLE_FAIL:
			break;
			
		case MBTEVT_PBAP_SERVER_DISABLE_SUCCESS:
			{
				PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus State  from %d to %d ",	
					sdcPBAPStatus->State,MBT_PBAP_STATE_DISABLED);
				PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus bEnabled from %d to %d ",
						sdcPBAPStatus->bEnabled, MBT_FALSE);
			
			sdcPBAPStatus->State = MBT_PBAP_STATE_DISABLED;
			sdcPBAPStatus->bEnabled  = MBT_FALSE;
			/* Initialize all Fields */ 
			}
			break;
			
		case MBTEVT_PBAP_SERVER_DISABLE_FAIL:
			break;
			
		case MBTEVT_PBAP_SERVER_CONNECT_SUCCESS:
			{
				PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus State  from %d to %d ",	
					sdcPBAPStatus->State,MBT_PBAP_STATE_CONNECTED);
			sdcPBAPStatus->State = MBT_PBAP_STATE_CONNECTED;
			}
			break;
			
		case MBTEVT_PBAP_SERVER_CONNECT_FAIL:
			break;
			
		case MBTEVT_PBAP_SERVER_DISCONNECT_SUCCESS:
			if(sdcPBAPStatus->State == MBT_PBAP_STATE_SENDING)
			{
				/*int the buffer to file mod need to delete the temp file */
				if(ext_fs_is_file_exist_ucs2((U16*)PBAP_ADP_FILE_NAME))
				{
					ext_fs_delete_ucs2((U16*)PBAP_ADP_FILE_NAME);
				}
			}
			PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus State  from %d to %d ",	
					sdcPBAPStatus->State,MBT_PBAP_STATE_ENABLED);
			
			sdcPBAPStatus->State = MBT_PBAP_STATE_ENABLED;
			/*Initialize every value in pbapStatus except bEnabled*/
			break;
			
		case MBTEVT_PBAP_SERVER_DISCONNECT_FAIL:
			break;
		case MBTEVT_PBAP_SERVER_CLOSE:
			break;
		case MBTEVT_PBAP_SERVER_AUTHREQ:
			{
				ShowBdAddr(sdcPBAPStatus->BDAddr);
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Present sdcPBAPStatus NameDev:%s ", sdcPBAPStatus->NameDev);
				//	T_MBT_BDADDR remote_bd;
				sdcPBAPStatus->BDAddr[5] = (unsigned char)(bt_pbap_app.pbap_bt_device.lap & 0x000000FF);
				sdcPBAPStatus->BDAddr[4] = (unsigned char)((bt_pbap_app.pbap_bt_device.lap & 0x0000FF00) >> 8);			
				sdcPBAPStatus->BDAddr[3] = (unsigned char)((bt_pbap_app.pbap_bt_device.lap & 0x00FF0000) >> 16);
				sdcPBAPStatus->BDAddr[2] = (unsigned char)(bt_pbap_app.pbap_bt_device.uap);
				sdcPBAPStatus->BDAddr[1] = (unsigned char)((bt_pbap_app.pbap_bt_device.nap & 0x00FF));
				sdcPBAPStatus->BDAddr[0] = (unsigned char)((bt_pbap_app.pbap_bt_device.nap & 0xFF00) >> 8);
				memcpy(sdcPBAPStatus->NameDev, bt_pbap_app.pbap_dev_name, strlen(bt_pbap_app.pbap_dev_name)+1);
				UpdateBdAddr(sdcPBAPStatus->BDAddr);
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus NameDev to %s ", sdcPBAPStatus->NameDev);
				btmtk_gap_handle_authorize_req(MBT_SVCUUID_PBAP_PSE ,sdcPBAPStatus->BDAddr, sdcPBAPStatus->NameDev);
				
			}
			break;
		case MBTEVT_PBAP_SERVER_OP_REQUEST:
			{

				PBAP_MBT_LOG1("[BTMTK] [PBAP] Present sdcPBAPStatus State  %d ",	sdcPBAPStatus->State);	
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Present  sdcPBAPStatus Operation  %d ", sdcPBAPStatus->Operation);
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Present vCardOut.vCardTotalCount %d ",	sdcPBAPStatus->vCardOut.vCardTotalCount);				
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Present ReqPath:  %s ", sdcPBAPStatus->ReqPath);
				if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_BUILDING_ENTRY)
				{
					
					PBAP_MBT_LOG2("[BTMTK] [PBAP] Present sdcPBAPStatus PullvCardIn.Dir  %d,  PullvCardIn.Storage  %d",
						sdcPBAPStatus->PullvCardIn.Dir, sdcPBAPStatus->PullvCardIn.Storage);
					
				        PBAP_MBT_LOG2("[BTMTK] [PBAP] Present sdcPBAPStatus PullvCardIn.Format %d,  PullvCardIn.Index %d",
						sdcPBAPStatus->PullvCardIn.Format, sdcPBAPStatus->PullvCardIn.Index);

					sdcPBAPStatus->Operation = MBT_PBAP_OP_PULL_ONE_VCARD;
					sdcPBAPStatus->PullvCardIn.Dir = (T_MBT_PBAP_DIR)bt_pbap_app.vcard_entry.dir;
					sdcPBAPStatus->PullvCardIn.Storage = (T_MBT_PBAP_STORAGE) bt_pbap_app.vcard_entry.storage;
					sdcPBAPStatus->PullvCardIn.Format =  (T_MBT_PBAP_VCARD_VERSION)bt_pbap_app.vcard_entry.format;
					memcpy(sdcPBAPStatus->PullvCardIn.vCardMask, bt_pbap_app.vcard_entry.filter.byte, 0x08);
					sdcPBAPStatus->PullvCardIn.Index =  (MBT_SHORT)bt_pbap_app.vcard_entry.entry_index;
					

					PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus Operation to %d ", MBT_PBAP_OP_PULL_ONE_VCARD);
					

					PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus PullvCardIn.Dir to %d,  PullvCardIn.Storage to %d",
						sdcPBAPStatus->PullvCardIn.Dir, sdcPBAPStatus->PullvCardIn.Storage);
					
				        PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus PullvCardIn.Format to %d,  PullvCardIn.Index to %d",
						sdcPBAPStatus->PullvCardIn.Format, sdcPBAPStatus->PullvCardIn.Index);
						

					sdcPBAPStatus->PullDirectoryDataIn.Dir = (T_MBT_PBAP_DIR) bt_pbap_app.vcard_entry.dir;
					sdcPBAPStatus->PullDirectoryDataIn.Storage =(T_MBT_PBAP_STORAGE) bt_pbap_app.vcard_entry.storage;
					btmtk_pbap_update_req_path((U8*)sdcPBAPStatus->ReqPath,  bt_pbap_app.vcard_entry.storage,
						bt_pbap_app.vcard_entry.dir);
				}
				else if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_BUILDING_FOLDER)
				{
					if(bt_pbap_app.vcard_folder.maxListCount !=  0x00)
					{
						
						PBAP_MBT_LOG2("[BTMTK] [PBAP] Present sdcPBAPStatus PullDirectoryDataIn.Dir  %d,  PullDirectoryDataIn.Storage  %d",
						 sdcPBAPStatus->PullDirectoryDataIn.Dir, sdcPBAPStatus->PullDirectoryDataIn.Storage);
					
				                PBAP_MBT_LOG1("[BTMTK] [PBAP] Present sdcPBAPStatus PullDirectoryDataIn.Format %d",
						sdcPBAPStatus->PullvCardIn.Index);

						PBAP_MBT_LOG2("[BTMTK] [PBAP] Present sdcPBAPStatus PullDirectoryDataIn.ListStartOffset %d,  PullDirectoryDataIn.MaxListCount %d",
						sdcPBAPStatus->PullDirectoryDataIn.ListStartOffset, sdcPBAPStatus->PullDirectoryDataIn.MaxListCount);
						
						sdcPBAPStatus->Operation = MBT_PBAP_OP_PULL_DIRECTORY_DATA;
						sdcPBAPStatus->PullDirectoryDataIn.Dir = (T_MBT_PBAP_DIR) bt_pbap_app.vcard_folder.pb_name;
						sdcPBAPStatus->PullDirectoryDataIn.Storage =(T_MBT_PBAP_STORAGE) bt_pbap_app.vcard_folder.storage;
						sdcPBAPStatus->PullDirectoryDataIn.Format = (T_MBT_PBAP_VCARD_VERSION)bt_pbap_app.vcard_folder.format;
						memcpy(sdcPBAPStatus->PullDirectoryDataIn.vCardMask, bt_pbap_app.vcard_folder.filter.byte, 0x08);
						sdcPBAPStatus->PullDirectoryDataIn.ListStartOffset = (MBT_UINT)bt_pbap_app.vcard_folder.listStartOffset;
						sdcPBAPStatus->PullDirectoryDataIn.MaxListCount =(MBT_UINT) bt_pbap_app.vcard_folder.maxListCount;	

						PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus Operation to %d ", MBT_PBAP_OP_PULL_DIRECTORY_DATA);
						
						PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus PullDirectoryDataIn.Dir to %d,  PullDirectoryDataIn.Storage  to %d",
						 sdcPBAPStatus->PullDirectoryDataIn.Dir, sdcPBAPStatus->PullDirectoryDataIn.Storage);
					
				                PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus PullDirectoryDataIn.Format to %d",
						sdcPBAPStatus->PullvCardIn.Index);

						PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus PullDirectoryDataIn.ListStartOffset to %d,  PullDirectoryDataIn.MaxListCount to %d",
						sdcPBAPStatus->PullDirectoryDataIn.ListStartOffset, sdcPBAPStatus->PullDirectoryDataIn.MaxListCount);
					}
					else
					{
						sdcPBAPStatus->PullDirectoryDataIn.Dir = (T_MBT_PBAP_DIR) bt_pbap_app.vcard_folder.pb_name;
						sdcPBAPStatus->PullDirectoryDataIn.Storage =(T_MBT_PBAP_STORAGE) bt_pbap_app.vcard_folder.storage;
						
						sdcPBAPStatus->Operation = MBT_PBAP_OP_PULL_DIRECTORY_NUM;
						PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus Operation to %d ", MBT_PBAP_OP_PULL_DIRECTORY_NUM);
					}
					
					btmtk_pbap_update_req_path((U8*)sdcPBAPStatus->ReqPath,  bt_pbap_app.vcard_folder.storage,
						bt_pbap_app.vcard_folder.pb_name);
					
				}
				else if(bt_pbap_app.server_state == BT_PBAP_APP_STATE_BUILDING_LIST)
				{
					if(bt_pbap_app.vcard_listing.maxListCount > 0x00)
					{

					        PBAP_MBT_LOG2("[BTMTK] [PBAP] Present sdcPBAPStatus PullDirectoryListIn.Dir  %d,  PullDirectoryListIn.Storage %d",
						 sdcPBAPStatus->PullDirectoryListIn.Dir, sdcPBAPStatus->PullDirectoryListIn.Storage);
					
				                PBAP_MBT_LOG3("[BTMTK] [PBAP] Present sdcPBAPStatus PullDirectoryListIn.SearchMethod  %d, PullDirectoryListIn.SearchVal  %s, PullDirectoryListIn.SortMethod  %d " ,
						sdcPBAPStatus->PullDirectoryListIn.SearchMethod,
						sdcPBAPStatus->PullDirectoryListIn.SearchVal,
						sdcPBAPStatus->PullDirectoryListIn.SortMethod);

						PBAP_MBT_LOG2("[BTMTK] [PBAP] Present sdcPBAPStatus PullDirectoryListIn.ListStartOffset  %d,  PullDirectoryListIn.MaxListCount  %d",
						sdcPBAPStatus->PullDirectoryListIn.ListStartOffset, sdcPBAPStatus->PullDirectoryListIn.MaxListCount);
						
						
						sdcPBAPStatus->Operation = MBT_PBAP_OP_PULL_DIRECTORY_LIST;
						
						sdcPBAPStatus->PullDirectoryListIn.Dir = (T_MBT_PBAP_DIR)bt_pbap_app.vcard_listing.dir;
						sdcPBAPStatus->PullDirectoryListIn.Storage =(T_MBT_PBAP_STORAGE)  bt_pbap_app.vcard_listing.storage;			
						sdcPBAPStatus->PullDirectoryListIn.SearchMethod  = (T_MBT_PBAP_SEARCH)bt_pbap_app.vcard_listing.searchAttribute;
						memcpy(sdcPBAPStatus->PullDirectoryListIn.SearchVal, bt_pbap_app.vcard_listing.searchValue, MBT_PBAP_MAX_NAME_LENGTH);
						sdcPBAPStatus->PullDirectoryListIn.SortMethod = (T_MBT_PBAP_SORT)bt_pbap_app.vcard_listing.order;
						sdcPBAPStatus->PullDirectoryListIn.ListStartOffset =(MBT_UINT) bt_pbap_app.vcard_listing.listStartOffset;
						sdcPBAPStatus->PullDirectoryListIn.MaxListCount = (MBT_UINT)bt_pbap_app.vcard_listing.maxListCount;
						
						
						PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus Operation to %d ", MBT_PBAP_OP_PULL_DIRECTORY_LIST);
						PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus PullDirectoryListIn.Dir to %d,  PullDirectoryListIn.Storage to %d",
						 sdcPBAPStatus->PullDirectoryListIn.Dir, sdcPBAPStatus->PullDirectoryListIn.Storage);
					
				                PBAP_MBT_LOG3("[BTMTK] [PBAP] Update sdcPBAPStatus PullDirectoryListIn.SearchMethod to %d, PullDirectoryListIn.SearchVal to %s, PullDirectoryListIn.SortMethod to %d " ,
						sdcPBAPStatus->PullDirectoryListIn.SearchMethod,
						sdcPBAPStatus->PullDirectoryListIn.SearchVal,
						sdcPBAPStatus->PullDirectoryListIn.SortMethod);

						PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus PullDirectoryListIn.ListStartOffset to %d,  PullDirectoryListIn.MaxListCount to %d",
						sdcPBAPStatus->PullDirectoryListIn.ListStartOffset, sdcPBAPStatus->PullDirectoryListIn.MaxListCount);
						
					}
					else
					{
						PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus Operation to %d ", MBT_PBAP_OP_PULL_DIRECTORY_NUM);
						sdcPBAPStatus->Operation = MBT_PBAP_OP_PULL_DIRECTORY_NUM;
					}
					#ifdef BTMTK_ON_WISE
					if((bt_pbap_app.vcard_listing.maxListCount > 0x00) && (bt_pbap_app.cur_list_Count < 0x00))
					{
						sdcPBAPStatus->Operation = MBT_PBAP_OP_PULL_DIRECTORY_NUM;
					}
					#endif
					sdcPBAPStatus->PullDirectoryDataIn.Dir = (T_MBT_PBAP_DIR) bt_pbap_app.vcard_listing.dir;
					sdcPBAPStatus->PullDirectoryDataIn.Storage =(T_MBT_PBAP_STORAGE) bt_pbap_app.vcard_listing.storage;
					btmtk_pbap_update_req_path((U8*)sdcPBAPStatus->ReqPath,  bt_pbap_app.vcard_listing.storage,
						bt_pbap_app.vcard_listing.dir);
				}
				/* in the buffer to file mod need to delete the temp file */
				if(ext_fs_is_file_exist_ucs2((U16*)PBAP_ADP_FILE_NAME))
				{
					ext_fs_delete_ucs2((U16*)PBAP_ADP_FILE_NAME);
				}
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus State  to %d ",	MBT_PBAP_STATE_SENDING);				
				sdcPBAPStatus->State = MBT_PBAP_STATE_SENDING;
				
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Update vCardOut.vCardTotalCount to %d", 0x00);				
				sdcPBAPStatus->vCardOut.vCardTotalCount = 0x00;
				
				PBAP_MBT_LOG1("[BTMTK] [PBAP] Update ReqPath  to %s ", sdcPBAPStatus->ReqPath);
			}
			break;

		case MBTEVT_PBAP_SERVER_OP_SUCCESS:
		case MBTEVT_PBAP_SERVER_OP_FAIL:
			/*in the buffer to file mod need to delete the temp file */
			if(ext_fs_is_file_exist_ucs2((U16*)PBAP_ADP_FILE_NAME))
			{
				ext_fs_delete_ucs2((U16*)PBAP_ADP_FILE_NAME);
			}
			PBAP_MBT_LOG2("[BTMTK] [PBAP] Update sdcPBAPStatus State  from %d to %d ",	
				sdcPBAPStatus->State,MBT_PBAP_STATE_CONNECTED);
			sdcPBAPStatus->State = MBT_PBAP_STATE_CONNECTED;
			break;
		case MBTEVT_PBAP_SERVER_OBEX_AUTHREQ:
			if(bt_pbap_app.auth_option == 0x00)
			{
				sdcPBAPStatus->bIsObexUserID  = MBT_FALSE;
			}
			else if(bt_pbap_app.auth_option == 0x01)
			{
				sdcPBAPStatus->bIsObexUserID  = MBT_TRUE;
			}
			PBAP_MBT_LOG1("[BTMTK] [PBAP] Update sdcPBAPStatus bIsObexUserID to %d ", sdcPBAPStatus->bIsObexUserID);
			break;  
		default:
			return;
    }
    bt_trace(TRACE_FUNC, BT_PBAP_MBT_POST_EVENT,  event_id);
    mbt_postevent(event_id, 0x00);
}
