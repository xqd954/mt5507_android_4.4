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
 * Bt_map_hdl.c
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
 * Dlight Ting
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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
//#include "bt_mmi_msg.h"
#include "bt_ext_utility.h"
//#include "bt_ext_system.h"
//#include "bt_common.h"  //temp for trace
#include "ext_osal.h"

#include "bluetooth_map_message.h"
#include "bluetooth_map_struct.h"
#include "bt_map_api.h"
#include "bt_ext_debug.h"

#include "bt_map_util.h"

#define MAP_GET_MSG_PARA(a) (a)->ilm_data

#ifdef BTMTK_ON_LINUX
extern map_ext_cntx_struct g_pi_map_cntx;
extern void map_postevent(U32 event, void* arg);
extern void BTCMD_SendMessage(U32 msg_id, module_type dest_mod, void *ptr, U16 size);
extern void btmtk_map_server_reset_context();
#endif


void btmtk_maps_handle_activate_cnf(ilm_struct *message)
{
 	bt_maps_activate_cnf_struct *msg;

    msg = (bt_maps_activate_cnf_struct *) MAP_GET_MSG_PARA(message);
 //   bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_HANDLE_ACTIVATE_CNF, msg->result);
 	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_activate_cnf: result is %d", msg->result);
    if (msg->result == BT_MAP_SUCCESS)
    { 
    	g_pi_map_cntx.state = BTMTK_MAP_STATE_INITIALIZED;
		map_postevent (MBTEVT_MAP_SERVER_ENABLE_SUCCESS,0);

    }
    else
    {
        map_postevent(MBTEVT_MAP_SERVER_ENABLE_FAIL, 0);
    }

}

void btmtk_maps_handle_deactivate_cnf(ilm_struct *message)
{
	bt_maps_deactivate_cnf_struct *msg;

    msg = (bt_maps_deactivate_cnf_struct *) MAP_GET_MSG_PARA(message);
    bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_deactivate_cnf,the result is %d", msg->result);
    if (msg->result == BT_MAP_SUCCESS)
    {
        map_postevent(MBTEVT_MAP_SERVER_DISABLE_SUCCESS, 0);
		btmtk_map_server_reset_context();
    }
    else
    {
        map_postevent(MBTEVT_MAP_SERVER_DISABLE_FAIL, 0);
    }

}

void btmtk_maps_handle_register_cnf(ilm_struct *message)
{
	bt_maps_register_cnf_struct *msg;
	MBT_MAP_MAS_STATUS * mas_status;
	msg = (bt_maps_register_cnf_struct *) MAP_GET_MSG_PARA(message);    

	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_register_cnf(): id is %d , result is %d",
				msg->mas_id, msg->result);

	mas_status = btmtk_map_util_search_mas_instance(msg->mas_id);
	if (!mas_status)
	{
		bt_ext_err("[BT][MAP][EXT]received an unkwon confirm");
		return;		
	}
	if (msg->result == BT_MAP_SUCCESS)
    {
    	mas_status->registered = TRUE;
		map_postevent(MBTEVT_MAP_SERVER_REGISTER_SUCCESS, &msg->mas_id);
		return;      
    }
    else
    {
    	//reset mas instance info
        map_postevent(MBTEVT_MAP_SERVER_REGISTER_FAIL, &msg->mas_id);
    	memset(&mas_status->masInfo, 0x0, sizeof(MBT_MAP_MAS_INFO));
		
    }
}

void btmtk_maps_handle_deregister_cnf(ilm_struct *message)
{
	bt_maps_deregister_cnf_struct *msg;	
	
    msg = (bt_maps_deregister_cnf_struct *) MAP_GET_MSG_PARA(message);
	bt_ext_log("[BT][MAP][EXT] btmtk_maps_handle_deregister_cnf, Id=%d, result=%d",
				msg->mas_id,msg->result);
	
    if (msg->result == BT_MAP_SUCCESS)
    {
		MBT_MAP_MAS_STATUS * mas_status;
		
		mas_status = btmtk_map_util_search_mas_instance(msg->mas_id);
		if (!mas_status)
		{
			bt_ext_log("[MAP] error: reveive a invalid instance ID");
			return;
		}
		
		//reset mas instance info
		mas_status->registered = FALSE;
		memset(&mas_status->masInfo, 0x0, sizeof(MBT_MAP_MAS_INFO));
		/*end*/	
		map_postevent(MBTEVT_MAP_SERVER_DEREGISTER_SUCCESS, &msg->mas_id);
    }
    else
    {
    	
		bt_ext_err("[MAP] error in deregister instance");
        map_postevent(MBTEVT_MAP_SERVER_DEREGISTER_FAIL, &msg->mas_id );
    }

}

void btmtk_maps_handle_authorize_ind(ilm_struct *message)
{
	bt_maps_authorize_ind_struct *msg;
	bt_map_addr_struct *addr;

  //  extern void btmtk_gap_handle_authorize_req(MBT_SERVICE_ID MBTSvcID, MBT_BDADDR RemoteBDAddr, MBT_CHAR* Name);

	msg = (bt_maps_authorize_ind_struct *) MAP_GET_MSG_PARA(message);

	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_authorize_ind");
	addr = &msg->addr;

	//gap authorize
//	btmtk_gap_handle_authorize_req(MBT_SVCUUID_MAP, addr, msg->dev_name);
  
    map_postevent(MBTEVT_MAP_SERVER_AUTHREQ, addr);

}

void btmtk_maps_handle_connect_ind(ilm_struct *message)
{
	bt_maps_connect_ind_struct *msg;
    bt_map_addr_struct addr;

    msg = (bt_maps_connect_ind_struct *) MAP_GET_MSG_PARA(message);

	MBT_MAP_MAS_STATUS * mas_status;

	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_connect_ind(): mas id is %d", msg->mas_id);
		
	//mas_status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	mas_status = btmtk_map_util_search_mas_instance(msg->mas_id);
 	if(!mas_status || mas_status->State != MBT_MAP_STATE_NONE)
 	{
 		bt_ext_log("[BT][MAP][EXT]MCE request an unregistered or occupied mas");
		btmtk_maps_connect_rsp(msg->mas_id, FALSE, msg->conn_id, &msg->addr);
 	} else {	
		memcpy(&mas_status->BdAddr, &msg->addr, sizeof(bt_map_addr_struct));;
	//	strncpy(mas_status->DevName, msg->dev_name, MBT_MAX_NAME_LEN);	
		mas_status->State = MBT_MAP_STATE_CONNECTED;		
		strncpy(mas_status->CurFolderPath, (const char*)mas_status->masInfo.RootPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
		mas_status->CurFolderPath[MBT_MAP_MAX_FOLDER_PATH_LEN - 1] = '\0';
		
    	btmtk_maps_connect_rsp(msg->mas_id, TRUE, msg->conn_id, &msg->addr);
    	map_postevent(MBTEVT_MAP_SERVER_CONNECT_IND, mas_status);
 	}
}

void btmtk_maps_handle_set_folder_ind(ilm_struct *message)
{
	bt_maps_set_folder_ind_struct *msg;	    
	MBT_MAP_MAS_STATUS * status;
        
    msg = (bt_maps_set_folder_ind_struct *) MAP_GET_MSG_PARA(message);

	status = btmtk_map_util_search_mas_client(msg->mas_id,&msg->addr);

	/*to do: check instance and set context in a single function*/
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_set_folder_ind");
	
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
    {
        bt_maps_set_folder_rsp_struct rsp;

        bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_FOLDER_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_folder_rsp_struct));
    }
    else
    {  
        if (msg->flag == BT_MAP_FOLDER_OP_ROOT)
        {
            strncpy(status->Oper.SetFolderReq.FolderPath, (const char*)status->masInfo.RootPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
        }
        else if (msg->flag == BT_MAP_FOLDER_OP_NEXT)
        {
            char *ptr;     
	    strncpy(status->Oper.SetFolderReq.FolderPath, status->CurFolderPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
            ptr = strrchr(status->Oper.SetFolderReq.FolderPath, '/');
            if (++ptr)
            {
                *ptr = '\0';
                strcat(status->Oper.SetFolderReq.FolderPath, (const char*)msg->folder);
            }
        }
        else if (msg->flag == BT_MAP_FOLDER_OP_DOWN)
        {
            strncpy(status->Oper.SetFolderReq.FolderPath, status->CurFolderPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
			//Some MCE may request to get root folder list in this command after connecting with MSE, such as MecApp
			//So check whether the rewuest folder is empty firstly
			if(strlen((const char*)msg->folder) > 0)
			{
            	strcat(status->Oper.SetFolderReq.FolderPath, "/");
            	strcat(status->Oper.SetFolderReq.FolderPath, (const char*)msg->folder);
			}
        }
        else if (msg->flag == BT_MAP_FOLDER_OP_UP)
        {
            char *ptr;
            strncpy(status->Oper.SetFolderReq.FolderPath, status->CurFolderPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
             ptr = strrchr(status->Oper.SetFolderReq.FolderPath, '/');
            *ptr = '\0';
        }
        else
        {
            bt_maps_set_folder_rsp_struct rsp;
            
            rsp.result = BT_MAP_FAIL_NOT_FOUND;
            rsp.mas_id = msg->mas_id;
            memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
            BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_FOLDER_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_folder_rsp_struct));
        }
        status->State = MBT_MAP_STATE_OPERATING;
        status->OperType = MBT_MAP_OPER_SET_FOLDER;
    
        map_postevent(MBTEVT_MAP_SERVER_SET_FOLDER_IND, status);
    }
}

void btmtk_maps_handle_get_folder_listing_ind(ilm_struct *message)
{
	bt_maps_get_folder_listing_ind_struct *msg;
   	MBT_MAP_MAS_STATUS * status;
    S8 mas_index;

	msg = (bt_maps_get_folder_listing_ind_struct *) MAP_GET_MSG_PARA(message);

	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_get_folder_listing_ind()");
	
    status = btmtk_map_util_search_mas_client(msg->mas_id,&msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
    {
    	bt_maps_get_folder_listing_rsp_struct rsp;
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        rsp.data_size = 0;
        rsp.list_size = 0;
		bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
		BTCMD_SendMessage(MSG_ID_BT_MAPS_GET_FOLDER_LISTING_RSP, MOD_BT, &rsp, sizeof(bt_maps_get_folder_listing_rsp_struct));
    }
    else
    {
    	/*prepare a file to contain message list info for MCE*/
        char filename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
    	mas_index = msg->mas_id;
        sprintf(filename, BT_MAP_FOLDER_LIST_FILE, mas_index);
        ext_fs_delete((const U8*)filename);
        status->State = MBT_MAP_STATE_OPERATING;
        status->OperType = MBT_MAP_OPER_GET_FOLDER_LIST;
		status->Oper.GetFolderList.Req.MaxListCount = msg->list_size;
        status->Oper.GetFolderList.Req.ListStartOffset = msg->list_offset;
    
     	map_postevent(MBTEVT_MAP_SERVER_GET_FOLDER_LIST_IND, status);       
    }
}

void btmtk_maps_handle_get_message_listing_ind(ilm_struct *message)
{
    bt_maps_get_message_listing_ind_struct *msg;
    MBT_MAP_MAS_STATUS * status;
	MBT_MAP_GET_MSG_LIST_REQ *mas_req;
    S8 mas_index ;
        
    msg = (bt_maps_get_message_listing_ind_struct *) MAP_GET_MSG_PARA(message);
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_get_message_listing_ind");
	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
    {
        bt_maps_get_message_listing_rsp_struct rsp;
        
    //    bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_CONNECTION_NOT_FOUND);
    	bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
        memset(&rsp, 0x0, sizeof(bt_maps_get_message_listing_rsp_struct));
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_GET_MESSAGE_LISTING_RSP, MOD_BT, &rsp, sizeof(bt_maps_get_message_listing_rsp_struct));
    }
    else
    {	
        char filename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
		mas_index = msg->mas_id;
    	sprintf(filename, BT_MAP_MESSAGE_LIST_FILE, mas_index);
        ext_fs_delete((const U8*)filename);
		
  		status->State = MBT_MAP_STATE_OPERATING;
		status->OperType = MBT_MAP_OPER_GET_MSG_LIST;
		mas_req = &status->Oper.GetMsgList.Req;
        strncpy(mas_req->FolderPath, status->CurFolderPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
        if (strlen((const char*)msg->child_folder))
        {
            strcat(mas_req->FolderPath, "/");
            strcat(mas_req->FolderPath, (const char*)msg->child_folder);
        }

		bt_ext_log("[BT][MAP][EXT]request path:%s", msg->child_folder);
		bt_ext_log("[BT][MAP][EXT]current path:%s", mas_req->FolderPath);
	
        
        mas_req->SubjectLen = msg->max_subject_len;
        mas_req->MaxListCount = msg->list_size;
        mas_req->ListStartOffset = msg->list_offset;
        mas_req->ParamMask = msg->mask;
        mas_req->FilterMsgType = btmtk_map_util_translate_filter_msg_type_bt2mbt(msg->filter_msg);
        strncpy(mas_req->FilterPeriodBegin, (const char*)msg->filter_begin, MBT_MAP_MAX_DATETIME_LEN);
        strncpy(mas_req->FilterPeriodEnd, (const char*)msg->filter_end, MBT_MAP_MAX_DATETIME_LEN);
        mas_req->FilterReadStatus = btmtk_map_util_translate_filter_status_bt2mbt(msg->filter_status);
        strncpy(mas_req->FilterRecipient, (const char*)msg->filter_rec, MBT_MAP_MAX_FILTER_TEXT_LEN);
        strncpy(mas_req->FilterOriginator, (const char*)msg->filter_orig, MBT_MAP_MAX_FILTER_TEXT_LEN);
        mas_req->FilterPriority = btmtk_map_util_translate_filter_priority_bt2mbt(msg->filter_prio);

        map_postevent(MBTEVT_MAP_SERVER_GET_MESSAGE_LIST_IND, status);      
    }
}

void btmtk_maps_handle_get_message_ind(ilm_struct *message)
{
    bt_maps_get_message_ind_struct *msg;
    MBT_MAP_MAS_STATUS * status;
    S8 mas_index;
        
    msg = (bt_maps_get_message_ind_struct *) MAP_GET_MSG_PARA(message);
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_get_message_ind");

	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
    {
        bt_maps_get_message_rsp_struct rsp;
        
     //   bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_CONNECTION_NOT_FOUND);
     	bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
        memset(&rsp, 0x0, sizeof(bt_maps_get_message_rsp_struct));
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_GET_MESSAGE_RSP, MOD_BT, &rsp, sizeof(bt_maps_get_message_rsp_struct));
    }
    else
    {
        MBT_MAP_GET_MSG_REQ *mas_req ; 
        char filename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
    	mas_index = msg->mas_id;
        sprintf(filename, BT_MAP_MESSAGE_GET_FILE, mas_index);
        ext_fs_delete((const U8*)filename);
        mas_req = &status->Oper.GetMsg.Req;
     //   mas_req->MsgHandle = btmtk_map_util_str2ull(msg->handle);
     	mas_req->MsgHandle = btmtk_map_util_str2xll((char*)msg->handle);
        mas_req->Attach = (msg->attachment ? MBT_MAP_ATTACH_TYPE_ON : MBT_MAP_ATTACH_TYPE_OFF);
        mas_req->Charset = btmtk_map_util_translate_charset_bt2mbt(msg->charset);
        mas_req->FracReq = btmtk_map_util_translate_fraction_req_bt2mbt(msg->fraction_req);

		status->State = MBT_MAP_STATE_OPERATING;
		status->OperType = MBT_MAP_OPER_GET_MSG;
        map_postevent(MBTEVT_MAP_SERVER_GET_MESSAGE_IND, status);
    }
}

void btmtk_maps_handle_push_message_ind(ilm_struct *message)
{

    bt_maps_push_message_ind_struct *msg;
	MBT_MAP_MAS_STATUS * status;
	S8 mas_index;

	msg = (bt_maps_push_message_ind_struct *) MAP_GET_MSG_PARA(message);
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_push_message_ind");

	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
	{

        bt_maps_push_message_rsp_struct rsp;
        
       // bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_CONNECTION_NOT_FOUND);
       	bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
        memset(&rsp, 0x0, sizeof(bt_maps_push_message_rsp_struct));
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_PUSH_MESSAGE_RSP, MOD_BT, &rsp, sizeof(bt_maps_push_message_rsp_struct));
    }
    else
    {
        MBT_MAP_PUSH_MSG_REQ *mas_req = &status->Oper.PushMsg.Req;
	memset(mas_req, 0x0, sizeof(MBT_MAP_PUSH_MSG_REQ));
        U8 rename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
		mas_index = msg->mas_id;
        sprintf((char*)rename, BT_MAP_RCV_MESSAGE_PUSH_FILE, mas_index);
        ext_fs_delete(rename);
        if(ext_fs_rename(msg->file, rename) != EXT_FS_OK)
        {
			bt_ext_err("fail to rename file");
		}
        
        status->State = MBT_MAP_STATE_OPERATING;
        status->OperType = MBT_MAP_OPER_PUSH_MSG;

        strncpy(mas_req->FolderPath, status->CurFolderPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
        if (strlen((char*)msg->child_folder))
        {
            strcat(mas_req->FolderPath, "/");
            strcat(mas_req->FolderPath, (char*)msg->child_folder);
        }
        mas_req->Transp = btmtk_map_util_translate_transparent_bt2mbt(msg->sent_op);
        mas_req->Retry = (msg->retry ? MBT_MAP_RETRY_TYPE_ON : MBT_MAP_RETRY_TYPE_OFF);
        mas_req->Charset = btmtk_map_util_translate_charset_bt2mbt(msg->charset);
      //  mas_req->StorageType = MBT_MAP_STORAGE_TYPE_BUFFER;
      	mas_req->StorageType = MBT_MAP_STORAGE_TYPE_FILE;
  
        map_postevent(MBTEVT_MAP_SERVER_PUSH_MESSAGE_IND, status);
    }
}

void btmtk_maps_handle_set_notif_registration_ind(ilm_struct *message)
{
   bt_maps_set_notif_registration_ind_struct *msg;
	MBT_MAP_MAS_STATUS * status;
	S8 mas_index;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_set_notif_registration_ind");

	msg = (bt_maps_set_notif_registration_ind_struct *) MAP_GET_MSG_PARA(message);
	
	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
	{
        bt_maps_set_notif_registration_rsp_struct rsp;

        bt_ext_err("[MAP] no MAS is found or state is occupied");
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_NOTIF_REGISTRATION_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_notif_registration_rsp_struct));
    }
    else
    {
        status->State = MBT_MAP_STATE_OPERATING;
        status->OperType = MBT_MAP_OPER_SET_NOTIF_REG;
        status->Oper.SetNotifReg.NotifStatus = (msg->on ? MBT_MAP_NOTIF_STATUS_ON : MBT_MAP_NOTIF_STATUS_OFF);
	    map_postevent(MBTEVT_MAP_SERVER_SET_NOTIFICATION_IND, status);
    }
}

void btmtk_maps_handle_set_message_status_ind(ilm_struct *message)
{
    bt_maps_set_message_status_ind_struct *msg;
	MBT_MAP_MAS_STATUS * status;
	S8 mas_index;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_set_message_status_ind");

	msg = (bt_maps_set_message_status_ind_struct *) MAP_GET_MSG_PARA(message);

	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
	{
        bt_maps_set_message_status_rsp_struct rsp;

        bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_MESSAGE_STATUS_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_message_status_rsp_struct));
    }
    else
    {
    	MBT_MAP_SET_MSG_STATUS_REQ	*SetMsgStatusReq = &status->Oper.SetMsgStatusReq;
        status->State = MBT_MAP_STATE_OPERATING;
        status->OperType = MBT_MAP_OPER_SET_MSG_STATUS;
		
      //  SetMsgStatusReq->MsgHandle = btmtk_map_util_str2ull(msg->handle);
      	SetMsgStatusReq->MsgHandle = btmtk_map_util_str2xll((char*)msg->handle);
        switch(msg->status)
        {
        case BT_MAP_MSG_STATUS_READ:
            SetMsgStatusReq->StatusInicator = MBT_MAP_STATUS_IND_READ;
            SetMsgStatusReq->StatusVal = MBT_MAP_STATUS_VAL_YES;
            break;
        case BT_MAP_MSG_STATUS_UNREAD:
            SetMsgStatusReq->StatusInicator = MBT_MAP_STATUS_IND_READ;
            SetMsgStatusReq->StatusVal = MBT_MAP_STATUS_VAL_NO;
            break;
        case BT_MAP_MSG_STATUS_DELETE:
            SetMsgStatusReq->StatusInicator = MBT_MAP_STATUS_IND_DELETE;
            SetMsgStatusReq->StatusVal = MBT_MAP_STATUS_VAL_YES;
            break;
        case BT_MAP_MSG_STATUS_UNDELETE:
            SetMsgStatusReq->StatusInicator = MBT_MAP_STATUS_IND_DELETE;
            SetMsgStatusReq->StatusVal = MBT_MAP_STATUS_VAL_NO;
            break;
        default:
     //       BTMTK_ASSERT(0);
     		bt_ext_log("unexpected msg status");
	 		return;
        }

		//to do: add instance id  to MMI
        map_postevent(MBTEVT_MAP_SERVER_SET_MESSAGE_STATUS_IND, status);
    }
}

void btmtk_maps_handle_update_inbox_ind(ilm_struct *message)
{
    bt_maps_update_inbox_ind_struct *msg;
	MBT_MAP_MAS_STATUS * status; 
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_update_inbox_ind");

	msg = (bt_maps_update_inbox_ind_struct *) MAP_GET_MSG_PARA(message);
	
	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status || status->State != MBT_MAP_STATE_CONNECTED)
	{
		bt_maps_update_inbox_rsp_struct rsp;
		bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");

        rsp.result = BT_MAP_FAIL_FORBIDDEN;
        rsp.mas_id = msg->mas_id;
        memcpy(&rsp.addr, &msg->addr, sizeof(bt_map_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_UPDATE_INBOX_RSP, MOD_BT, &rsp, sizeof(bt_maps_update_inbox_rsp_struct));
    }
    else
    {
        status->State = MBT_MAP_STATE_OPERATING;
        status->OperType = MBT_MAP_OPER_UPDATE_INBOX;
        
        map_postevent(MBTEVT_MAP_SERVER_UPDATE_INBOX, status);
    }
}

void btmtk_maps_handle_abort_cnf(ilm_struct *message)
{
    bt_maps_abort_cnf_struct *msg;
    MBT_MAP_MAS_STATUS * status;
    S8 mas_index;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_abort_cnf");

    msg = (bt_maps_abort_cnf_struct *) MAP_GET_MSG_PARA(message);
	
	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
    if(!status || status->State != MBT_MAP_STATE_CONNECTED)
    { 
		bt_ext_log("[BT][MAP][EXT]no MAS is found or MAS is occupied");
       // BTMTK_ASSERT(0);
    }
    else
    {	
    	//it seems no need to do this
        map_postevent(MBTEVT_MAP_SERVER_CANCEL, status);
    }
}

void btmtk_maps_handle_disconnect_cnf(ilm_struct *message)
{
    bt_maps_disconnect_cnf_struct *msg;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_disconnect_cnf");

    msg = (bt_maps_disconnect_cnf_struct *) MAP_GET_MSG_PARA(message);

	MBT_MAP_MAS_STATUS * status;
		
	status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!status)
   	{	
   		//no check state (MBT_MAP_STATE_DISCONNECTING)
        bt_ext_err("no this instance id or received a invalid device address");
    }
    else
    {   
   //   memset(status, 0x0, sizeof(MBT_MAP_MAS_STATUS));
        map_postevent(MBTEVT_MAP_SERVER_DISCONNECT_SUCCESS, status);
   		//reset mas context except basic info and register info
		MBT_MAP_MAS_INFO info;
		memset(&info, 0x0, sizeof(MBT_MAP_MAS_INFO));
		memcpy(&info, &status->masInfo, sizeof(MBT_MAP_MAS_INFO));
		memset(status, 0x0, sizeof(MBT_MAP_MAS_STATUS));
		memcpy(&status->masInfo, &info, sizeof(MBT_MAP_MAS_INFO));
		status->registered = TRUE;
	//	memset(&status->State, 0x0, sizeof(MBT_MAP_MAS_STATUS)-sizeof(MBT_MAP_MAS_INFO)-sizeof(BT_BOOL));
    }
}

void btmtk_maps_handle_disconnect_ind(ilm_struct *message)
{

    bt_map_addr_struct addr;
    bt_maps_disconnect_ind_struct *msg;
    S16 mas_index;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_disconnect_ind");

    msg = (bt_maps_disconnect_ind_struct *) MAP_GET_MSG_PARA(message);

	MBT_MAP_MAS_STATUS * mas_status;
		
	mas_status = btmtk_map_util_search_mas_client(msg->mas_id, &msg->addr);
	if(!mas_status)
	{
		bt_ext_err("no this instance id");
	}
	else
    {
        map_postevent(MBTEVT_MAP_SERVER_DISCONNECT_SUCCESS, mas_status);		
		//reset mas context except mas info an register info
		MBT_MAP_MAS_INFO info;
		memset(&info, 0x0, sizeof(MBT_MAP_MAS_INFO));
		memcpy(&info, &mas_status->masInfo, sizeof(MBT_MAP_MAS_INFO));
		memset(mas_status, 0x0, sizeof(MBT_MAP_MAS_STATUS));
		memcpy(&mas_status->masInfo, &info, sizeof(MBT_MAP_MAS_INFO));
		mas_status->registered = TRUE;
	//	memset(&mas_status->State, 0x0, sizeof(MBT_MAP_MAS_STATUS)-sizeof(MBT_MAP_MAS_INFO)-sizeof(BT_BOOL));
    }
}


void btmtk_maps_handle_mns_connect_cnf(ilm_struct *message)
{
    bt_maps_mns_connect_cnf_struct *msg;
	bt_maps_mns_disconnect_req_struct req;

    msg = (bt_maps_mns_connect_cnf_struct *) MAP_GET_MSG_PARA(message);

	MBT_MAP_MAS_STATUS * mas_status;
	int index ;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_mns_connect_cnf");	
	    
    if (msg->result == BT_MAP_SUCCESS)
    {
		MBT_MAP_MNS_STATUS *mns_status = btmtk_map_util_search_mns(&msg->addr);
		if(mns_status && mns_status->State == MBT_MAP_STATE_CONNECTING) 
		{
			mns_status->State = MBT_MAP_STATE_CONNECTED;
			map_postevent(MBTEVT_MAP_SERVER_MNS_CONNECT_SUCCESS, &msg->addr);

		}
		else 
		{
			bt_ext_log("no connecting MNS");
			map_postevent(MBTEVT_MAP_SERVER_MNS_CONNECT_FAIL, &msg->addr);
			goto exit;
		}
    }
    else
    {
        map_postevent(MBTEVT_MAP_SERVER_MNS_CONNECT_FAIL, &msg->addr);
    }
	return;
exit:
    memcpy(&req.addr, &msg->addr, sizeof(bt_map_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_MNS_DISCONNECT_REQ, MOD_BT, &req, sizeof(bt_maps_mns_disconnect_req_struct));
}

void btmtk_maps_handle_mns_send_event_cnf(ilm_struct *message)
{
   bt_maps_mns_send_event_cnf_struct *msg;
    S16 mas_index;

    msg = (bt_maps_mns_send_event_cnf_struct *) MAP_GET_MSG_PARA(message);

	MBT_MAP_MNS_STATUS * status;	
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_mns_send_event_cnf");

    status = btmtk_map_util_search_mns(&msg->addr);
   	if(!status)
    {
    	bt_ext_err("no mns_status for the device");
		return;
    }
	
    if (msg->result == BT_MAP_SUCCESS)
    {        
        map_postevent(MBTEVT_MAP_SERVER_SEND_EVENT_SUCCESS, status);
    }
    else
    {
        map_postevent(MBTEVT_MAP_SERVER_SEND_EVENT_FAIL, status);
      //  status->FailReason = MBT_ERROR_RES;
    }
    status->State = MBT_MAP_STATE_CONNECTED;
}

void btmtk_maps_handle_mns_disconnect_ind(ilm_struct *message)
{
	MBT_MAP_MNS_STATUS * status;	
	bt_maps_mns_disconnect_ind_struct *msg;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_mns_disconnect_ind");

    msg = (bt_maps_mns_disconnect_ind_struct *) MAP_GET_MSG_PARA(message);

    status = btmtk_map_util_search_mns(&msg->addr);

	if (!status)
    {
        /* do nothing */
		bt_ext_err("no this mns");
		
    }
    else
    {
        map_postevent(MBTEVT_MAP_SERVER_MNS_DISCONNECT_SUCCESS, status);
        memset(status, 0x0, sizeof(MBT_MAP_MNS_STATUS));
    }
}

void btmtk_maps_handle_mns_disconnect_cnf(ilm_struct *message)
{
    bt_maps_mns_disconnect_cnf_struct *msg;
	int index;
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_handle_mns_disconnect_cnf");
    msg = (bt_maps_mns_disconnect_cnf_struct *) MAP_GET_MSG_PARA(message);
 

	MBT_MAP_MNS_STATUS *mns_status = btmtk_map_util_search_mns(&msg->addr);
	if (!mns_status)
	{
		bt_ext_err("no available mns session");
		return;
	}

	MBT_MAP_MAS_STATUS *mas_status ;
	for(index = 0; index < MBT_MAP_MAX_CONN_NUM; index++)
	{ 
		mas_status = &g_pi_map_cntx.masStatus[index];
		if (mas_status &&
			mas_status->State == MBT_MAP_STATE_OPERATING &&
			mas_status->OperType == MBT_MAP_OPER_SET_NOTIF_REG && 
        	btmtk_map_util_bdaddr_equal(&mas_status->BdAddr, &msg->addr))
		{
			mas_status->State = MBT_MAP_STATE_CONNECTED;
			break;
		}
	}
	//note the sequence, if memset firstly, MMI obtain null value
   // memset(mns_status, 0x0, sizeof(MBT_MAP_MNS_STATUS));
	map_postevent(MBTEVT_MAP_SERVER_MNS_DISCONNECT_SUCCESS, mns_status);
    memset(mns_status, 0x0, sizeof(MBT_MAP_MNS_STATUS));
}

void btmtk_map_handle_message(ilm_struct *message)
{
    switch(message->msg_id)
    {
        case MSG_ID_BT_MAPS_ACTIVATE_CNF:
            btmtk_maps_handle_activate_cnf(message);
            break;
        case MSG_ID_BT_MAPS_DEACTIVATE_CNF:
            btmtk_maps_handle_deactivate_cnf(message);
            break;
        case MSG_ID_BT_MAPS_REGISTER_CNF:
            btmtk_maps_handle_register_cnf(message);
            break;
        case MSG_ID_BT_MAPS_DEREGISTER_CNF:
            btmtk_maps_handle_deregister_cnf(message);
            break;
        case MSG_ID_BT_MAPS_AUTHORIZE_IND:
            btmtk_maps_handle_authorize_ind(message);
            break;
        case MSG_ID_BT_MAPS_CONNECT_IND:
            btmtk_maps_handle_connect_ind(message);
            break;
        case MSG_ID_BT_MAPS_SET_FOLDER_IND:
            btmtk_maps_handle_set_folder_ind(message);
            break;
        case MSG_ID_BT_MAPS_GET_FOLDER_LISTING_IND:
            btmtk_maps_handle_get_folder_listing_ind(message);
            break;
        case MSG_ID_BT_MAPS_GET_MESSAGE_LISTING_IND:
            btmtk_maps_handle_get_message_listing_ind(message);
            break;
        case MSG_ID_BT_MAPS_GET_MESSAGE_IND:
            btmtk_maps_handle_get_message_ind(message);
            break;
        case MSG_ID_BT_MAPS_PUSH_MESSAGE_IND:
            btmtk_maps_handle_push_message_ind(message);
            break;
        case MSG_ID_BT_MAPS_SET_NOTIF_REGISTRATION_IND:
            btmtk_maps_handle_set_notif_registration_ind(message);
            break;
        case MSG_ID_BT_MAPS_SET_MESSAGE_STATUS_IND:
            btmtk_maps_handle_set_message_status_ind(message);
            break;
        case MSG_ID_BT_MAPS_UPDATE_INBOX_IND:
            btmtk_maps_handle_update_inbox_ind(message);
            break;
        case MSG_ID_BT_MAPS_ABORT_CNF:
            btmtk_maps_handle_abort_cnf(message);
            break;
        case MSG_ID_BT_MAPS_DISCONNECT_CNF:
            btmtk_maps_handle_disconnect_cnf(message);
            break;
        case MSG_ID_BT_MAPS_DISCONNECT_IND:
            btmtk_maps_handle_disconnect_ind(message);
            break;
        case MSG_ID_BT_MAPS_MNS_CONNECT_CNF:
            btmtk_maps_handle_mns_connect_cnf(message);
            break;
        case MSG_ID_BT_MAPS_MNS_SEND_EVENT_CNF:
            btmtk_maps_handle_mns_send_event_cnf(message);
            break;
        case MSG_ID_BT_MAPS_MNS_DISCONNECT_CNF:
            btmtk_maps_handle_mns_disconnect_cnf(message);
            break;
        case MSG_ID_BT_MAPS_MNS_DISCONNECT_IND:
            btmtk_maps_handle_mns_disconnect_ind(message);
            break;
        default:
            return;
    }
}


