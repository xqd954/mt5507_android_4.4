/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_map_api.c
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
 * Adam CHen
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

#include <sys/socket.h>
#include <errno.h>

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
//#include "bt_mmi_msg.h"
#include "ext_osal.h"
#include "bt_ext_utility.h"
//#include "bt_common.h" //temp for trace

#include "bluetooth_map_message.h"
#include "bluetooth_map_struct.h"

#include "bt_map_api.h"
#include "bt_map_util.h"
#include "bt_map_obj.h"
#include "bt_ext_debug.h"

#ifdef BTMTK_ON_LINUX

map_ext_cntx_struct g_pi_map_cntx;

int ext_map_api_socket;
int ext_map_server_socket;

void btmtk_map_server_reset_context()
{
	U8 i;

	g_pi_map_cntx.state = BTMTK_MAP_STATE_NULL;
	memset(g_pi_map_cntx.masStatus, 0x00, sizeof(MBT_MAP_MAS_STATUS)*MBT_MAP_MAX_MAS_INST_NUM);
	memset(g_pi_map_cntx.mnsStatus, 0x00, sizeof(MBT_MAP_MNS_STATUS)*MBT_MAP_MAX_MCE_NUM);
	memset(g_pi_map_cntx.file_buffer, 0x00, MAP_ADP_BUFFER_SIZE);
	memset(g_pi_map_cntx.builder_buffer, 0x00, MAP_ADP_BUFFER_SIZE);
	
	for (i = 0; i < MBT_MAP_MAX_MAS_INST_NUM; i++)
	{
		/*initialize mas instance ID as invalid value*/
		g_pi_map_cntx.masStatus[i].masInfo.MasInstId= MBT_MAP_INVALID_INSTACE_ID;
	}
}

/*initialize the MASInstId as invalid value*/
void btmtk_map_server_init(BTMTK_MAP_MMI_CALLBACK mmicallback)
{
	memset(&g_pi_map_cntx, 0x0, sizeof(map_ext_cntx_struct));
	
	btmtk_map_server_reset_context();
	
	btmtk_map_register_callback(mmicallback);
}


BT_BOOL btmtk_map_server_enable()
{
    if (g_pi_map_cntx.state == BTMTK_MAP_STATE_INITIALIZED)
    {
    	bt_ext_log("[BT][MAP][EXT]map server has been initialized");
        return TRUE;
    }
    else
    {
        g_pi_map_cntx.state = BTMTK_MAP_STATE_INITIALIZING;
 //       btmtk_map_set_cntx(BTMTK_MAP_OP_ENABLE, 0, dummy_addr, NULL, 0);
        BTCMD_SendMessage(MSG_ID_BT_MAPS_ACTIVATE_REQ, MOD_BT, NULL, 0);
    }
	return TRUE;
}

BT_BOOL btmtk_map_server_disable()
{
 //   btmtk_map_op_struct *op = (btmtk_map_op_struct *) message;

 	bt_ext_log("[BT][MAP][EXT]btmtk_map_server_disable:= %d", g_pi_map_cntx.state);
	
    if (g_pi_map_cntx.state == BTMTK_MAP_STATE_NULL)
    {
          /* do nothing */
    }
    else if (g_pi_map_cntx.state == BTMTK_MAP_STATE_DEINITIALIZING)
    {
        /* do nothing */
    }
    else
    {
        g_pi_map_cntx.state = BTMTK_MAP_STATE_DEINITIALIZING;
        BTCMD_SendMessage(MSG_ID_BT_MAPS_DEACTIVATE_REQ, MOD_BT, NULL, 0);
    }
	return TRUE;
}

BT_BOOL btmtk_map_server_register(void *message)
{
    MBT_MAP_MAS_INFO *info = (MBT_MAP_MAS_INFO *) message;
    bt_maps_register_req_struct req;
	MBT_MAP_MAS_STATUS * mas_status = NULL;

	if (message == NULL) 
	{
		bt_ext_log("the info to be registered is null");
		return FALSE;
	}		

	if (g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED)
	{
		bt_ext_log("the MAP ext has not been initialized");
		return TRUE;
	}  	
	memset(&req, 0x0, sizeof(bt_maps_register_req_struct));
	mas_status = btmtk_map_util_search_mas_instance(info->MasInstId);
	if (mas_status) 
	{
		bt_ext_log("the instance has been be registered or registering");
		return FALSE;			
	}
	//set mas when cnf is received
	mas_status = btmtk_map_util_search_unregistered_mas_instance();
	if (!mas_status)
	{
		bt_ext_log("no available instance to be registered");		
		return FALSE;
	}

	/*put the regster info in the context*/
	/*to do:is the state neccessary?*/
	//mas_status->State = MBT_MAP_STATE_REGISTERING;  
	//btmtk_map_set_cntx(BTMTK_MAPS_OP_REGISTER,&op->info.registration);
	mas_status->registered = TRUE;
	memcpy(&mas_status->masInfo, info , sizeof(MBT_MAP_MAS_INFO));
	/*end*/

    req.mas_id = info->MasInstId;
    req.srv_name_len = strlen((const char*)info->SvcName);
    req.srv_name_len = (req.srv_name_len > BT_MAP_MAX_SRV_NAME_LEN) ? BT_MAP_MAX_SRV_NAME_LEN : req.srv_name_len;
    strncpy((char*)req.srv_name, (const char*)info->SvcName, BT_MAP_MAX_SRV_NAME_LEN);
    req.msg_type = btmtk_map_util_translate_msg_type_mbt2bt(info->SupMsgType);
    BTCMD_SendMessage(MSG_ID_BT_MAPS_REGISTER_REQ, MOD_BT, &req, sizeof(bt_maps_register_req_struct));
	return TRUE;
}

BT_BOOL btmtk_map_server_deregister(U8 masId)
{
    U8 instance = masId;
    bt_maps_deregister_req_struct req;
	MBT_MAP_MAS_STATUS * mas_status = NULL;

  //  BTMTK_ASSERT(g_pi_map_cntx.state == BTMTK_MAP_STATE_INITIALIZED);
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED)
	{
		bt_ext_log("[BT][MAP][EXT]MAP server has not been initialized");
		return FALSE;
	}
    
	memset(&req, 0x0, sizeof(bt_maps_deregister_req_struct));
    /* check with registered record */	
	mas_status = btmtk_map_util_search_mas_instance(instance);
	if (!mas_status) 
	{
//		MapMMICallback(MBTEVT_MAP_SERVER_DEREGISTER_SUCCESS, 0);
		return FALSE;
	} 

	//do these in receiving confirm
//	mas_status->registered = FALSE;
//	memset(&mas_status->masInfo, 0x0, sizeof(MBT_MAP_MAS_INFO));
	

    req.mas_id = instance;
    BTCMD_SendMessage(MSG_ID_BT_MAPS_DEREGISTER_REQ, MOD_BT, &req, sizeof(bt_maps_deregister_req_struct));
	return TRUE;
}

BT_BOOL btmtk_maps_authorize_res(bt_map_addr_struct *addr,U8 result)
{
    bt_maps_authorize_rsp_struct rsp;

  //  BTMTK_ASSERT((g_pi_map_cntx.state == BTMTK_MAP_STATE_INITIALIZED));
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED)
	{
		bt_ext_log("[BT][MAP][EXT]MAP server has not been initialized");
		return FALSE;
	}
	memset(&rsp, 0x0, sizeof(bt_maps_authorize_rsp_struct));

	rsp.accept = result;
	memcpy(&rsp.addr, addr, sizeof(bt_map_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_AUTHORIZE_RSP, MOD_BT, &rsp, sizeof(bt_maps_authorize_rsp_struct));
	return TRUE;
}

//TODO: uncomplete
BT_BOOL btmtk_maps_connect_rsp(U8 masId, kal_bool accept,
					kal_uint32 conn_id, bt_map_addr_struct *addr)
{
   	bt_maps_connect_rsp_struct rsp;
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);

	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
	{
		bt_ext_log("[BT][MAP][EXT]MAP server has not been initialized or mas is null");
		return FALSE;
	}
	memset(&rsp, 0x0, sizeof(bt_maps_connect_rsp_struct));
    rsp.accept = accept;
    rsp.mas_id = masId;
    rsp.conn_id = conn_id;
	memcpy(&rsp.addr, addr, sizeof(bt_map_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_CONNECT_RSP, MOD_BT, &rsp, sizeof(bt_maps_connect_rsp_struct));
	return TRUE;
}

BT_BOOL btmtk_maps_disconnect(S8 masId)
{	 
	bt_maps_disconnect_req_struct req;
   	MBT_MAP_MAS_STATUS *mas_status = NULL;
		
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED )
	{		
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
	}   
	memset(&req, 0x0, sizeof(bt_maps_disconnect_req_struct));

	mas_status = btmtk_map_util_search_mas_instance(masId);
	if ((!mas_status) || 
		(mas_status->State == MBT_MAP_STATE_DISCONNECTING))
	{	
		return TRUE;	
    }    
    else
    {
        if((mas_status->State == MBT_MAP_STATE_OPERATING )	&& 
           ((mas_status->OperType == MBT_MAP_OPER_GET_FOLDER_LIST) ||
           (mas_status->OperType == MBT_MAP_OPER_GET_MSG_LIST) ||
           (mas_status->OperType == MBT_MAP_OPER_GET_MSG) ||
           (mas_status->OperType == MBT_MAP_OPER_PUSH_MSG)))
        {
            bt_maps_abort_req_struct abort_req;
            abort_req.mas_id = masId;
            BTCMD_SendMessage(MSG_ID_BT_MAPS_ABORT_REQ, MOD_BT, &abort_req, sizeof(bt_maps_abort_req_struct));
        }

        if (btmtk_map_util_get_connection_num(&mas_status->BdAddr) == 1)
        {
            btmtk_maps_mns_disconnect(&mas_status->BdAddr);
        }
        mas_status->State = MBT_MAP_STATE_DISCONNECTING;
		bt_ext_log("");
     //   MBT_LOG2("[MBT][BT][MAP][EXT][SDC] sdcMAPStatus.Mas[%d].State = %d", index, status->Mas[index].State);
        req.mas_id = masId;
	 	memcpy(&req.addr, &mas_status->BdAddr, sizeof(bt_addr_struct));
        BTCMD_SendMessage(MSG_ID_BT_MAPS_DISCONNECT_REQ, MOD_BT, &req, sizeof(bt_maps_disconnect_req_struct));
    }  
	return TRUE;
}

BT_BOOL btmtk_maps_disconnect_server(void)
{	
	int i;
	bt_maps_disconnect_req_struct req;
		
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED)
	{		
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
	}
	memset(&req, 0x0, sizeof(bt_maps_disconnect_req_struct));
    for (i = 0; i < MBT_MAP_MAX_CONN_NUM; i++)
    {	
    	MBT_MAP_MAS_STATUS* status = &g_pi_map_cntx.masStatus[i];
        
            switch (status->State)
            {
            case MBT_MAP_STATE_NONE:
//                MapMMICallback(MBTEVT_MAP_SERVER_DISCONNECT_SUCCESS, i);
                break;
            case MBT_MAP_STATE_DISCONNECTING:			
				bt_ext_log("[BT][MAP][EXT]the procedure is in progressing");
                break;
            case MBT_MAP_STATE_OPERATING:
                if((status->OperType == MBT_MAP_OPER_GET_FOLDER_LIST) ||
                   (status->OperType == MBT_MAP_OPER_GET_MSG_LIST) ||
                   (status->OperType == MBT_MAP_OPER_GET_MSG) ||
                   (status->OperType == MBT_MAP_OPER_PUSH_MSG))
                {
                    bt_maps_abort_req_struct abort_req;
                    abort_req.mas_id = status->masInfo.MasInstId;
                    BTCMD_SendMessage(MSG_ID_BT_MAPS_ABORT_REQ, MOD_BT, &abort_req, sizeof(bt_maps_abort_req_struct));
                    
                }
                /* fall-through */
            default:
                if (btmtk_map_util_get_connection_num(&status->BdAddr) == 1)
                {                    
                    btmtk_maps_mns_disconnect(&status->BdAddr);
                }
                status->State = MBT_MAP_STATE_DISCONNECTING;
                req.mas_id = status->masInfo.MasInstId;
				memcpy(&req.addr, &status->BdAddr, sizeof(bt_addr_struct));        
                BTCMD_SendMessage(MSG_ID_BT_MAPS_DISCONNECT_REQ, MOD_BT, &req, sizeof(bt_maps_disconnect_req_struct));
                break;
            }
       
    }
	return TRUE;
  
}


BT_BOOL btmtk_maps_set_folder(S8 masId, S8 result)
{
  
	/*if multi access on single instance is allowed, just define subsession in masSTATUS of context*/
   //MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_client(op->MasInstId, op->BdAddr);
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
	
    bt_maps_set_folder_rsp_struct rsp;

	if(!mas || g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED)
	{
		bt_ext_err("error: no this instance or uninitialized");
		return FALSE;
	}	
	memset(&rsp, 0x0, sizeof(bt_maps_set_folder_rsp_struct));
 
    mas->State = MBT_MAP_STATE_CONNECTED;
   	if (result == MBT_ALLOW_RES)
    {
        strncpy(mas->CurFolderPath, mas->Oper.SetFolderReq.FolderPath, MBT_MAP_MAX_FOLDER_PATH_LEN);
    }
	
	rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
	rsp.mas_id = masId;
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
	BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_FOLDER_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_folder_rsp_struct));
	return TRUE;
}

BT_BOOL btmtk_maps_get_folder_list(S8 masId, S8 result)
{
//    btmtk_map_op_struct *op = (btmtk_map_op_struct *) message;
 
 //   MBT_MAP_GET_FOLDER_LIST_RSP *mas_rsp;
    MBT_MAP_GET_FOLDER_LIST_REQ *mas_req;
	MBT_MAP_GET_FOLDER_LIST_RSP *mas_rsp;

    bt_maps_get_folder_listing_rsp_struct rsp;
    S8 mas_index;

	bt_ext_log("btmtk_maps_get_folder_list()");
	
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);	

	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
	{	
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
	}

	mas_req = &mas->Oper.GetFolderList.Req;
	mas_rsp = &mas->Oper.GetFolderList.Rsp;
	if (!mas_rsp) 
	{
		bt_ext_err("mas_rsp is null ");
		return FALSE;
	}	
    
    memset(&rsp, 0x0, sizeof(bt_maps_get_folder_listing_rsp_struct));

	mas_index = masId;
	
    rsp.result = BT_MAP_SUCCESS;
    if (result == MBT_ALLOW_RES)
    {
        BT_BOOL success;
        if (mas_req->MaxListCount == 0)
        {
            rsp.list_size = mas_rsp->TotalFolderNum;
        }
        else if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER ||
                 mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_FILE)
        {
            BT_BOOL success;
            
            /* create XML and save in file */
            if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER)
            {
                success = (btmtk_map_xml_create_folder_list_by_buffer(mas_index, 
                                                                          mas_rsp->Data.Buffer.FolderNum,
                                                                          mas_rsp->Data.Buffer.FolderList,
                                                                          0,
                                                                          mas_rsp->TodoFolderNum,
                                                                          -1) > 0);
           //     bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_WRITE_LIST, 0, mas_rsp->Data.Buffer.FolderNum);
            }
            else
            {
                success = (btmtk_map_xml_create_folder_list_by_file(mas_index, 
                                                                        mas_rsp->Data.File.FileName,
                                                                        mas_rsp->Data.File.Size) > 0);
            }

            if (success)
            {
                if (mas_rsp->TodoFolderNum)
                {
                //    req_op->info.written += mas_rsp->Data.Buffer.FolderNum;
                //    MapMMICallback(MBTEVT_MAP_SERVER_ACCESS_REQUEST, mas_index);
                	bt_ext_log("[BT][MAP][EXT]TodoFolderNum is not zero");
                    return FALSE;
                }
                else
                {
                    sprintf((char*)rsp.file, BT_MAP_FOLDER_LIST_FILE, mas_index);
                    rsp.list_size = mas_rsp->TotalFolderNum;
                    rsp.data_size = ext_fs_get_filesize(rsp.file);
                }
            }
            else
            {
       //         bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_WRITE_XML_ERROR, mas_rsp->StorageType);
       			bt_ext_err("fail to write data to file, storagetype is %d", mas_rsp->StorageType);
                rsp.result = BT_MAP_FAIL;
            }
        }
        else if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER ||
                 mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_FILE)
        {
            char filename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
            
            /* save in file */
            sprintf(filename, BT_MAP_FOLDER_LIST_FILE, mas_index);
            if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER)
            {
                success = btmtk_map_util_buffer2file(filename, mas_rsp->Data.RawBuffer.RawData, mas_rsp->Data.RawBuffer.Size);
            }
            else
            {
                success = btmtk_map_util_file_copy(filename, mas_rsp->Data.File.FileName, mas_rsp->Data.File.Size);
            }

            if (success)
            {
                sprintf((char*)rsp.file, BT_MAP_FOLDER_LIST_FILE, mas_index);
                rsp.list_size = mas_rsp->TotalFolderNum;
                rsp.data_size = ext_fs_get_filesize(rsp.file);
            }
            else
            {
              	bt_ext_err("fail to write data to file, storagetype is %d", mas_rsp->StorageType);
            	rsp.result = BT_MAP_FAIL;
            }
        }
        else
        {
            rsp.result = BT_MAP_FAIL_NOT_FOUND;
        }
    }
    else
    {
        rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
    }
    rsp.mas_id = masId;
    memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_GET_FOLDER_LISTING_RSP, MOD_BT, &rsp, sizeof(bt_maps_get_folder_listing_rsp_struct));
    mas->State = MBT_MAP_STATE_CONNECTED;

	return TRUE;
}

BT_BOOL btmtk_maps_get_message_list(S8 masId, S8 result)
{
    MBT_MAP_GET_MSG_LIST_REQ *mas_req = NULL;
	MBT_MAP_GET_MSG_LIST_RSP *mas_rsp = NULL;
    bt_maps_get_message_listing_rsp_struct rsp;
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);

	S8 mas_index;
   
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
	{		
		bt_ext_err("error");
		return FALSE;
	} 

	mas_index = masId;
	mas_rsp = &mas->Oper.GetMsgList.Rsp;
	mas_req = &mas->Oper.GetMsgList.Req;

    memset(&rsp, 0x0, sizeof(bt_maps_get_message_listing_rsp_struct));
    rsp.result = BT_MAP_SUCCESS;
    if (result == MBT_ALLOW_RES)
    {
        BT_BOOL success;

        if (mas_req->MaxListCount == 0)
        {
            rsp.list_size = mas_rsp->TotalMsgNum;
            rsp.unread = (mas_rsp->NewMsg == MBT_MAP_NEW_MSG_STATUS_ON);
        }
        else if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER ||
                 mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_FILE)
        {
            /* create XML and save in file */
            if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER)
            {
                success = (btmtk_map_xml_create_message_list_by_buffer(masId, 
                                                                           mas_rsp->Data.Buffer.MsgNum,
                                                                           mas_rsp->Data.Buffer.MsgList,
                                                                           0,
                                                                           mas_rsp->TodoMsgNum,
                                                                           -1) > 0);
            }
            else
            {
                success = (btmtk_map_xml_create_message_list_by_file(masId, 
                                                                         mas_rsp->Data.File.FileName,
                                                                         mas_rsp->Data.File.Size) > 0);
            }

            if (success)
            {
                if (mas_rsp->TodoMsgNum)
                {
                  	bt_ext_log("[BT][MAP][EXT]TodoFolderNum is not zero");
                    return FALSE;
                }
                else
                {
                    rsp.result = BT_MAP_SUCCESS;
                    sprintf((char*)rsp.file, BT_MAP_MESSAGE_LIST_FILE, mas_index);
                    strncpy((char*)rsp.datetime, mas_rsp->MSETime, BT_MAP_MAX_DATETIME_STR_LEN);
                    rsp.list_size = mas_rsp->TotalMsgNum;
                    rsp.data_size = ext_fs_get_filesize(rsp.file);
                    rsp.unread = (mas_rsp->NewMsg == MBT_MAP_NEW_MSG_STATUS_ON);
                }
            }
            else
            {
                bt_ext_err("fail to create message list file");
				rsp.result = BT_MAP_FAIL;
            }
        }
        else if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER ||
                 mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_FILE)
        {
            char filename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
            
            /* save in file */
            sprintf(filename, BT_MAP_MESSAGE_LIST_FILE, mas_index);
            if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER)
            {
                success = btmtk_map_util_buffer2file(filename, mas_rsp->Data.RawBuffer.RawData, mas_rsp->Data.RawBuffer.Size);
            }
            else
            {
                success = btmtk_map_util_file_copy(filename, mas_rsp->Data.File.FileName, mas_rsp->Data.File.Size);
            }

            if (success)
            {
                rsp.result = BT_MAP_SUCCESS;
                sprintf((char*)rsp.file, BT_MAP_MESSAGE_LIST_FILE, mas_index);
                strncpy((char*)rsp.datetime, mas_rsp->MSETime, BT_MAP_MAX_DATETIME_STR_LEN);
                rsp.list_size = mas_rsp->TotalMsgNum;
                rsp.data_size = ext_fs_get_filesize(rsp.file);
                rsp.unread = (mas_rsp->NewMsg == MBT_MAP_NEW_MSG_STATUS_ON);
            }
            else
            {
        //        bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_WRITE_FILE_ERROR, mas_rsp->StorageType);
                rsp.result = BT_MAP_FAIL;
            }
        }
        else
        {
            rsp.result = BT_MAP_FAIL_NOT_FOUND;
        }
    }
    else
    {
        rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
    }
    rsp.mas_id = mas->masInfo.MasInstId;
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_GET_MESSAGE_LISTING_RSP, MOD_BT, &rsp, sizeof(bt_maps_get_message_listing_rsp_struct));
    mas->State = MBT_MAP_STATE_CONNECTED;
	return TRUE;
   
}

BT_BOOL btmtk_maps_get_message(S8 masId,S8 result)
{
    MBT_MAP_GET_MSG_REQ *mas_req = NULL;
	MBT_MAP_GET_MSG_RSP *mas_rsp = NULL;
//	MBT_MAP_GET_MSG_RSP *mas_rsp = NULL;
    bt_maps_get_message_rsp_struct rsp;
 //   S8 mas_index;
	
    
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
	
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
	{		
		bt_ext_err("error");
		return FALSE;
	}

	mas_req = &mas->Oper.GetMsg.Req;
	mas_rsp = &mas->Oper.GetMsg.Rsp;
    memset(&rsp, 0x0, sizeof(bt_maps_get_message_rsp_struct));
    rsp.result = BT_MAP_SUCCESS;
    if (result == MBT_ALLOW_RES)
    {
        BT_BOOL success;

        if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER ||
            mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_FILE)
        {
            BT_BOOL success;

			/*to do: check the get message request and confirn the  */
			/*existence of fraction request and content */
			
            
            /* create XML and save in file */
            if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER)
            {
                success = (btmtk_map_vcard_create_message_by_buffer(masId,
                                                                        mas_req->FracReq,
                                                                        mas_rsp->FracDeliver,
                                                                        &mas_rsp->Data.Buffer.Msg,
                                                                        -1,
                                                                        0) > 0);
            }
            else
            {
                success = (btmtk_map_vcard_create_message_by_file(masId,
                                                                      mas_req->FracReq,
                                                                      mas_rsp->FracDeliver,
                                                                      &mas_rsp->Data.File.Msg,
                                                                      mas_rsp->Data.File.Size,
                                                                      mas_rsp->Data.File.FileName) > 0);
            }

            if (success)
            {
                rsp.fraction_rsp = btmtk_map_util_translate_fraction_rsp_mbt2bt(mas_rsp->FracDeliver);
                sprintf((char*)rsp.file, BT_MAP_MESSAGE_GET_FILE, masId);
                rsp.data_size = ext_fs_get_filesize(rsp.file);
            }
            else
            {
				bt_ext_err("failt to write VCARD,StorageType is %d", mas_rsp->StorageType);
			//	bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_WRITE_VCARD_ERROR, mas_rsp->StorageType);
                rsp.result = BT_MAP_FAIL;
            }
        }
        else if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER ||
                 mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_FILE)
        {
            char filename[BT_MAP_MAX_TEMP_FILE_NAME_LEN + 1];
            
            /* save in file */
            sprintf(filename, BT_MAP_MESSAGE_GET_FILE, masId);
            if (mas_rsp->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER)
            {
                success = btmtk_map_util_buffer2file(filename, mas_rsp->Data.RawBuffer.RawData, mas_rsp->Data.RawBuffer.Size);
            }
            else
            {
                success = btmtk_map_util_file_copy(filename, mas_rsp->Data.File.FileName, mas_rsp->Data.File.Size);
            }

            if (success)
            {
                rsp.fraction_rsp = btmtk_map_util_translate_fraction_rsp_mbt2bt(mas_rsp->FracDeliver);
                sprintf((char*)rsp.file, BT_MAP_MESSAGE_GET_FILE, masId);
                rsp.data_size = ext_fs_get_filesize(rsp.file);
            }
            else
            {
                bt_ext_err("failt to write file,StorageType is %d", mas_rsp->StorageType);
				//bt_trace(TRACE_GROUP_1, BTLOG_MBT_MAP_WRITE_FILE_ERROR, mas_rsp->StorageType);
                rsp.result = BT_MAP_FAIL;
            }
        }
        else
        {
            rsp.result = BT_MAP_FAIL_NOT_FOUND;
        }
    }
    else
    {
        rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
    }
    rsp.mas_id = masId;
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_GET_MESSAGE_RSP, MOD_BT, &rsp, sizeof(bt_maps_get_message_rsp_struct));

	mas->State = MBT_MAP_STATE_CONNECTED;    
	return TRUE;
}
BT_BOOL btmtk_maps_parse_message(S8 masId)
{	
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
	
    BT_BOOL result = TRUE;
	
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
	{		
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
	}

    MBT_MAP_PUSH_MSG_RSP *mas_rsp;
    MBT_MAP_PUSH_MSG_REQ *mas_req;
   
    S8 mas_index = masId; 
    mas_req = &mas->Oper.PushMsg.Req;

	/* Parser in PI, now parse */
	if (mas_req->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER ||
            mas_req->StorageType == MBT_MAP_STORAGE_TYPE_FILE)
    {
		/* create XML and save in file */
		if (mas_req->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER)
		{
			char to_file[MBT_MAX_FILE_NAME_LEN] = {0};
			strcpy(to_file, BT_MAP_PUSH_MESSAGE_FILE);
			result = btmtk_map_vcard_parse_message_by_buffer(mas_index,
																&mas_req->Data.Buffer.Msg,
																mas_req->Data.Buffer.AllocatedContentSize,
																to_file);
			if (strlen(to_file))
			{
				mas_req->StorageType = MBT_MAP_STORAGE_TYPE_FILE;
				strcpy(mas_req->Data.File.FileName, to_file);
            }
		}
		else
		{
			result = btmtk_map_vcard_parse_message_by_file(mas_index,
															&mas_req->Data.File.Msg,
															&mas_req->Data.File.Size,
															mas_req->Data.File.FileName);
		}

		if (!result)
		{
			bt_ext_err("fail to parse message");
		}
	}
	else if (mas_req->StorageType == MBT_MAP_STORAGE_TYPE_RAW_BUFFER ||
                 mas_req->StorageType == MBT_MAP_STORAGE_TYPE_RAW_FILE)
    {
    	bt_ext_err("wrong storage, %d", mas_req->StorageType);
		result = FALSE;
	}
	else
	{
		result = FALSE;
	}
    
	return result;
}

//notes: when succeed to parse message, it will be stored in the specific file
BT_BOOL btmtk_maps_push_message(S8 masId, S8 result)
{	
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
		
	bt_maps_push_message_rsp_struct rsp;	
	
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
	{		
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
	}

    MBT_MAP_PUSH_MSG_RSP *mas_rsp;
   
    S8 mas_index = masId; 
    mas_rsp = &mas->Oper.PushMsg.Rsp;
    memset(&rsp, 0x0, sizeof(bt_maps_push_message_rsp_struct));
    rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
//    sprintf(rsp.handle, "%llu", mas_rsp->MsgHandle);
	sprintf((char*)rsp.handle, "%llx", mas_rsp->MsgHandle);    
    rsp.mas_id = masId;
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_PUSH_MESSAGE_RSP, MOD_BT, &rsp, sizeof(bt_maps_push_message_rsp_struct));

	
	mas->State = MBT_MAP_STATE_CONNECTED;
	return TRUE;
}

BT_BOOL btmtk_maps_set_notif_reg(U8 masId, S8 result)
{	
 	bt_ext_log("[BT][MAP][EXT]btmtk_maps_set_notif_reg(): masId is %d, result is %d", masId,result);
 	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
	MBT_MAP_MNS_STATUS *mnsState = NULL;
	
    bt_maps_set_notif_registration_rsp_struct rsp;  

	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
    {    	
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
    }
	mas->State = MBT_MAP_STATE_CONNECTED;

	//send response for notification registration
	rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
    rsp.mas_id = masId;	
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
	BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_NOTIF_REGISTRATION_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_folder_rsp_struct));

	//Update notification status
	if (mas->Oper.SetNotifReg.NotifStatus == MBT_MAP_NOTIF_STATUS_ON)
    {
        btmtk_maps_mns_connect(&mas->BdAddr);
		mnsState = btmtk_map_util_search_mns(&mas->BdAddr);		
		if (mnsState) 
		{
			mnsState->NotifReg[masId].MasInstId = masId;
            mnsState->NotifReg[masId].State = MBT_MAP_NOTIF_STATUS_ON;
		}
		else 
		{
			bt_ext_err("[BT][MAP][EXT]No available mns state when register notification");
		}
    }
    else
    {
    	int index;
		mnsState = btmtk_map_util_search_mns(&mas->BdAddr);
    	if (mnsState == NULL)
    	{
			bt_ext_err("[BT][MAP][EXT]No available mns state when deregister notification");
			return FALSE;
		}
		mnsState->NotifReg[masId].State = MBT_MAP_NOTIF_STATUS_OFF;
		//If all instance's notification is deregistered, MNS connection shall be disconnected
		for (index = 0; index < MBT_MAP_MAX_MAS_INST_NUM; index ++) 
		{
			if (mnsState->NotifReg[index].State == MBT_MAP_NOTIF_STATUS_ON)
			{
				break;
			}
		}
		if (index >= MBT_MAP_MAX_MAS_INST_NUM) 
		{
			btmtk_maps_mns_disconnect(&mas->BdAddr);
		}        
    }
	return TRUE;
}

BT_BOOL btmtk_maps_set_message_status(S8 masId, S8 result)
{
	bt_ext_log("[BT][MAP][EXT]btmtk_maps_set_message_status");

   	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
   
   	bt_maps_set_message_status_rsp_struct rsp;

    if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
    {    	
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
    }
    rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
    rsp.mas_id = masId;
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
	mas->State = MBT_MAP_STATE_CONNECTED;
    BTCMD_SendMessage(MSG_ID_BT_MAPS_SET_MESSAGE_STATUS_RSP, MOD_BT, &rsp, sizeof(bt_maps_set_message_status_rsp_struct));
   	return TRUE;
}

BT_BOOL btmtk_maps_update_inbox(S8 masId, S8 result)
{
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masId);
    bt_maps_update_inbox_rsp_struct rsp;

    if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mas)
    {    	
		bt_ext_err("map service not initialized or mas is null");
		return FALSE;
    }
	
    rsp.result = btmtk_map_util_translate_result_mbt2bt(result);
    rsp.mas_id = masId;
	memcpy(&rsp.addr, &mas->BdAddr, sizeof(bt_addr_struct));
    BTCMD_SendMessage(MSG_ID_BT_MAPS_UPDATE_INBOX_RSP, MOD_BT, &rsp, sizeof(bt_maps_update_inbox_rsp_struct));
    mas->State = MBT_MAP_STATE_CONNECTED;
	return TRUE;
}

BT_BOOL btmtk_maps_send_event(bt_map_addr_struct *addr)
{	
    bt_maps_mns_send_event_req_struct req;
 //   S8 inst_index = btmtk_map_util_search_mas_instance_index(SendEvt->MasInstId);
    int mns_index, i;

	MBT_MAP_MAS_STATUS *mas = NULL;
	MBT_MAP_MNS_STATUS *mns_status;
	MBT_MAP_SEND_EVENT_REQ *event;
	
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED) 
    {
		bt_ext_err("map service not initialized");
		return FALSE;
	}
	mns_status = btmtk_map_util_search_mns(addr);
	if(!mns_status )
	{
		bt_ext_err("mns is null");
		return FALSE;
	} else if (mns_status->State != MBT_MAP_STATE_CONNECTED) {
		bt_ext_err("the request mns is busy");
		return FALSE;
	}

	event = &mns_status->SendEvt;

	for(i = 0; i <  MBT_MAP_MAX_MAS_INST_NUM; i++)
	{
		if(mns_status->NotifReg[i].MasInstId == mns_status->SendEvt.MasInstId &&
			mns_status->NotifReg[i].State == MBT_MAP_NOTIF_STATUS_ON)
		{
		break;
		}
	}

	if(i < MBT_MAP_MAX_MAS_INST_NUM)
	{		
        mns_status->State = MBT_MAP_STATE_OPERATING;
  //      memcpy(&mns_status->SendEvt, event, sizeof(MBT_MAP_SEND_EVENT_REQ));
        memset(&req, 0x0, sizeof(bt_maps_mns_send_event_req_struct));

		req.mas_id = mns_status->SendEvt.MasInstId;
		memcpy(&req.addr, addr, sizeof(bt_map_addr_struct));
	//	mas_index = btmtk_map_util_search_mas_client_index(op->MasInstId, op->BdAddr);
		sprintf((char*)req.file, BT_MAP_EVENT_REPORT_FILE, mns_status->SendEvt.MasInstId);
		req.data_size = (U16) btmtk_map_xml_create_event_report(mns_status->SendEvt.MasInstId, &mns_status->SendEvt);
        BTCMD_SendMessage(MSG_ID_BT_MAPS_MNS_SEND_EVENT_REQ, MOD_BT, &req, sizeof(bt_maps_mns_send_event_req_struct));
    }
	else 
	{
		bt_ext_err("the notification for the mas is disable");
		return FALSE;
	}
	return TRUE;
}
BT_BOOL btmtk_maps_mns_connect(bt_addr_struct *addr)
{   
    bt_maps_mns_disconnect_req_struct req;
  	MBT_MAP_MNS_STATUS *mnsState = NULL;
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED) 
	{
		return FALSE;
	}
	/*Only one MNS connection shall be set up*/
	mnsState = btmtk_map_util_search_mns(addr);
	if (mnsState != NULL)
	{
		//If current MNS connection is busy, reject this connect request
		if (mnsState->State != MBT_MAP_STATE_NONE) 
		{
			return FALSE;
		}
	}
	else 
	{
		//allocate new mns context
		mnsState = btmtk_map_util_new_mns();
		if (!mnsState) {
			return FALSE;
		}
	}
	memcpy(&mnsState->BdAddr, addr, sizeof(bt_map_addr_struct));
	mnsState->State = MBT_MAP_STATE_CONNECTING;
	
	memcpy(&req.addr, addr, sizeof(bt_addr_struct));				
    BTCMD_SendMessage(MSG_ID_BT_MAPS_MNS_CONNECT_REQ, MOD_BT, &req, sizeof(bt_maps_mns_connect_req_struct));
  	return TRUE;
}


BT_BOOL btmtk_maps_mns_disconnect(bt_addr_struct *addr)
{   
    bt_maps_mns_disconnect_req_struct req;
  	MBT_MAP_MNS_STATUS *mnsState = btmtk_map_util_search_mns(addr);
	if(g_pi_map_cntx.state != BTMTK_MAP_STATE_INITIALIZED || !mnsState) 
	{
		return FALSE;
	}
	mnsState->State = MBT_MAP_STATE_DISCONNECTING;
	memcpy(&req.addr, addr, sizeof(bt_addr_struct));
				
    BTCMD_SendMessage(MSG_ID_BT_MAPS_MNS_DISCONNECT_REQ, MOD_BT, &req, sizeof(bt_maps_mns_disconnect_req_struct));
    
	return TRUE;
}



void btmtk_map_register_callback (BTMTK_MAP_MMI_CALLBACK mmicallback)
{
	g_pi_map_cntx.MMI_callback = mmicallback;
}

void btmtk_map_set_socket(int api_socket, int server_socket) 
{
	if (api_socket < 0)
	{
		bt_ext_log("[BT][MAP][EXT]map api socket unitialized");
	} 
	if (server_socket < 0)
	{
		bt_ext_log("[BT][MAP][EXT]map server socket unitialized");		
	}
	ext_map_api_socket = api_socket;
	ext_map_server_socket = server_socket;
}
void BTCMD_SendMessage(U32 msg_id, module_type dest_mod, void *ptr, U16 size)
{
	
    int ret = -1;
    int sockfd = -1;

    U16 *field;

    ilm_struct ilm;
	
	bt_ext_log("[BT][MAP][EXT]BTCMD_SendMessage(): msg_id is %d",msg_id);
	
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.sap_id = 0;
    ilm.src_mod_id = MOD_MMI;
    ilm.dest_mod_id = MOD_BT;

    if(ptr != NULL)
        memcpy(ilm.ilm_data, ptr, size);
    // LOCAL_PARA_HDR
    field = (U16 *)ilm.ilm_data;
    *field = 1;
    field = (U16 *)&ilm.ilm_data[2];
    *field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

    sockfd = ext_map_api_socket;
    
    if(sockfd >= 0 )
    {
        //ret = send(GOPP(apisock), (void *)&ilm, size, 0);
        //ret = send(sockfd, ptr, size, 0);
        ret = send(sockfd, (void*)&ilm, size, 0);
        if(ret < 0)
        {
            bt_ext_log("[BT][MAP][EXT]send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            bt_ext_log("[BT][MAP][EXT]send msg success : %d", ret);
        }
    }
    else
    {
        bt_ext_log("[BT][MAP][EXT]socket uninitialized");
    }
}


#endif 

