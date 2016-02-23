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

#include <stdlib.h>
#include <assert.h>
#include "utils.h"
#include "cmdqueue.h"

static ListEntry g_listHead = {&g_listHead, &g_listHead};

/* When init or error happened, call this function to reset cmd queue  */
void resetCmdQueue(void)
{
    assert(IsListCircular(&g_listHead));
    while(!IsListEmpty(&g_listHead))
        freeBtCmd((BtCommand*)RemoveHeadList(&g_listHead));
}

/* system command : 0=no, 1=yes */
int isSysCmd(unsigned long cmd_id)
{
    switch(cmd_id)
    {
    case MSG_ID_TIMER_EXPIRY:
    case MSG_ID_BT_NOTIFY_EVM_IND:
        return 1;
    default:
        return 0;
    }
}

/* Check if command is already in executing : 0=no, 1=yes */
int checkCmdExist(unsigned long cmd_id)
{
    BtCommand *pCmd = (BtCommand*)GetHeadList(&g_listHead);
    while(&pCmd->node != &g_listHead)
    {
        pCmd = (BtCommand*)GetHeadList(&pCmd->node);
        if(pCmd->cmd_id == cmd_id)
        {
            return 1;
        }
    }
    return 0;
}

/* Feed request message id and return response ID. return 0 means no response needed. */
unsigned long checkResponseId(unsigned long req_id)
{
    unsigned long resp;
    switch(req_id)
    {
    case MSG_ID_BT_POWERON_REQ:
        resp = MSG_ID_BT_POWERON_CNF;
        break;
    case MSG_ID_BT_POWEROFF_REQ:
        resp = MSG_ID_BT_POWEROFF_CNF;
        break;
    case MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ:
        resp = MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF; 
        break;
    case MSG_ID_BT_BM_READ_LOCAL_NAME_REQ:
        resp = MSG_ID_BT_BM_READ_LOCAL_NAME_CNF;
        break;
    case MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ:
        resp = MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF;
        break;
    case MSG_ID_BT_BM_READ_LOCAL_COD_REQ:
        resp = MSG_ID_BT_BM_READ_LOCAL_COD_CNF;
        break;
    case MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ:
        resp = MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF;
        break;
    case MSG_ID_BT_BM_READ_LOCAL_UUID_REQ:
	    resp = MSG_ID_BT_BM_READ_LOCAL_UUID_CNF;
	    break;
    case MSG_ID_BT_BM_READ_SCANENABLE_MODE_REQ:
        resp = MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF;
        break;
    case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ:
        resp = MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF;
        break;
    case MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ:
        resp = MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF;
        break;
    case MSG_ID_BT_BM_BONDING_CANCEL_REQ:
        resp = MSG_ID_BT_BM_BONDING_CANCEL_CNF;
        break;
    case MSG_ID_BT_BM_LINK_DISALLOW_REQ:        
        resp = MSG_ID_BT_BM_LINK_DISALLOW_CNF;
        break;
    case MSG_ID_BT_BM_LINK_ALLOW_REQ:
        resp = MSG_ID_BT_BM_LINK_ALLOW_CNF;
        break;
    /* Due to async call of delete trust of BlueZ, we let event loop to get the cnf message as indication*/
    /*
    case MSG_ID_BT_BM_DELETE_TRUST_REQ:
        resp = MSG_ID_BT_BM_DELETE_TRUST_CNF;
        break;
    */
    case MSG_ID_BT_BM_READ_REMOTE_NAME_REQ:
        resp = MSG_ID_BT_BM_READ_REMOTE_NAME_CNF;
        break;
    case MSG_ID_BT_BM_GET_LINK_KEY_REQ:
        resp = MSG_ID_BT_BM_GET_LINK_KEY_CNF;
        break;
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_REQ:
        resp = MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF;
        break;
    case MSG_ID_BT_APP_SDPDB_GET_HANDLE_REQ:
        resp = MSG_ID_BT_APP_SDPDB_GET_HANDLE_CNF;
        break;
    case MSG_ID_BT_APP_SDPDB_REGISTER_REQ:
        resp = MSG_ID_BT_APP_SDPDB_REGISTER_CNF;
        break;
    case MSG_ID_BT_APP_SDPDB_DEREGISTER_REQ:
        resp = MSG_ID_BT_APP_SDPDB_DEREGISTER_CNF;
        break;
    case MSG_ID_BT_SSP_DEBUG_MODE_REQ:
        resp = MSG_ID_BT_SSP_DEBUG_MODE_CNF;
        break;
    /* HFG request / response */
    case MSG_ID_BT_HFG_ACTIVATE_REQ:
        resp = MSG_ID_BT_HFG_ACTIVATE_CNF;
        break;
    case MSG_ID_BT_HFG_DEACTIVATE_REQ:
        resp = MSG_ID_BT_HFG_DEACTIVATE_CNF;
        break;
    case MSG_ID_BT_HFG_ACCEPT_CHANNEL_REQ:
        resp = MSG_ID_BT_HFG_ACCEPT_CHANNEL_CNF;
        break;
    case MSG_ID_BT_HFG_REJECT_CHANNEL_REQ:
        resp = MSG_ID_BT_HFG_REJECT_CHANNEL_CNF;
        break;
    case MSG_ID_BT_HFG_SEND_DATA_REQ:
        resp = MSG_ID_BT_HFG_SEND_DATA_CNF;
        break;
    case MSG_ID_BT_HFG_INDICATOR_REPORT_REQ:
        resp = MSG_ID_BT_HFG_INDICATOR_REPORT_CNF;
        break;      
    case MSG_ID_BT_HFG_ERROR_REQ:
        resp = MSG_ID_BT_HFG_ERROR_CNF;
        break;    
    case MSG_ID_BT_HFG_RING_REQ:
        resp = MSG_ID_BT_HFG_RING_CNF;
        break;    
    case MSG_ID_BT_HFG_CALL_LIST_REQ:
        resp = MSG_ID_BT_HFG_CALL_LIST_CNF;
        break;    
    case MSG_ID_BT_HFG_CALL_ID_REQ:
        resp = MSG_ID_BT_HFG_CALL_ID_CNF;
        break;           
    case MSG_ID_BT_HFG_VOICE_REC_REQ:
        resp = MSG_ID_BT_HFG_VOICE_REC_CNF;
        break;      
    case MSG_ID_BT_HFG_OPERATOR_NAME_REQ:
        resp = MSG_ID_BT_HFG_OPERATOR_NAME_CNF;
        break;    
    case MSG_ID_BT_HFG_MIC_GAIN_REQ:
        resp = MSG_ID_BT_HFG_MIC_GAIN_CNF;
        break;    
    case MSG_ID_BT_HFG_SPEAKER_GAIN_REQ:
        resp = MSG_ID_BT_HFG_SPEAKER_GAIN_CNF;
        break;    
    case MSG_ID_BT_HFG_CALL_WAITING_REQ:
        resp = MSG_ID_BT_HFG_CALL_WAITING_CNF;
        break;    
    case MSG_ID_BT_HFG_READ_INDICATOR_REQ:
        resp = MSG_ID_BT_HFG_READ_INDICATOR_CNF;
        break;  
    /* PBAP request / response */
    case MSG_ID_BT_PBAP_REGISTER_REQ:
        resp = MSG_ID_BT_PBAP_REGISTER_CNF;
        break;
    case MSG_ID_BT_PBAP_DEREGISTER_REQ:
        resp = MSG_ID_BT_PBAP_DEREGISTER_CNF;
        break;
    /* Log setting */
    case MSG_ID_BT_ENGINEER_MODE_LOG_MASK_REQ:
	resp = MSG_ID_BT_ENGINEER_MODE_LOG_MASK_CNF;
	break;
    case MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_REQ:
	resp = MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_CNF;
	break;
    case MSG_ID_BT_DBG_QUERY_CONTEXT_REQ:
	resp = MSG_ID_BT_DBG_QUERY_CONTEXT_CNF;
	break;
    case MSG_ID_BT_SET_SCO_ACCEPT_REQ:
        resp = MSG_ID_BT_SET_SCO_ACCEPT_CNF;
        break;
    case MSG_ID_BT_SET_DUAL_PCM_SETTING_REQ:
        resp = MSG_ID_BT_SET_DUAL_PCM_SETTING_CNF;
        break;
    case MSG_ID_BT_SET_DUAL_PCM_SWITCH_REQ:
        resp = MSG_ID_BT_SET_DUAL_PCM_SWITCH_CNF;
        break;
	case MSG_ID_BT_HDP_REGISTER_INSTANCE_REQ:
		resp = MSG_ID_BT_HDP_REGISTER_INSTANCE_CNF;
		break;
	case MSG_ID_BT_HDP_DEREGISTER_INSTANCE_REQ:
		resp = MSG_ID_BT_HDP_DEREGISTER_INSTANCE_CNF;
		break;
	case MSG_ID_BT_HDP_GET_MAIN_CHANNEL_REQ:
		resp = MSG_ID_BT_HDP_GET_MAIN_CHANNEL_CNF;
		break;

	case MSG_ID_BT_HDP_GET_INSTANCE_REQ:
		resp = MSG_ID_BT_HDP_GET_INSTANCE_CNF;
		break;
	case MSG_ID_BT_HDP_GET_L2CAP_CHANNEL_REQ:
		resp = MSG_ID_BT_HDP_GET_L2CAP_CHANNEL_CNF;
		break;
    case MSG_ID_BT_TEST_MODE_REQ:
        resp = MSG_ID_BT_TEST_MODE_CNF;
        break;
    case MSG_ID_BT_BTTM_SWITCH_PORT_REQ:
        resp = MSG_ID_BT_BTTM_SWITCH_PORT_CNF;
        break;
    case MSG_ID_BT_TEST_CMD_REQ:
        resp = MSG_ID_BT_TEST_CMD_CNF;
        break;
    case MSG_ID_BT_GATTC_GET_DEVICE_TYPE_REQ:
        resp = MSG_ID_BT_GATTC_GET_DEVICE_TYPE_CNF;
        break;
    default:
        resp = 0;
        break;
    }
    return resp;
}

/* insert cmd into cmd queue*/
/* If it is cancel cmd, it only replaces the cmd_id in the BtCommand but not add new cmd. */
/* 1 : success. 0 : mean fail (alloc memory failed) */
int addBtCmd(struct sockaddr_un *addr, socklen_t len, unsigned long cmd_id)
{
    unsigned long resp;
    BtCommand *pCmd = NULL;
    resp = checkResponseId(cmd_id);
    bt_prompt_trace_lv3(MOD_BT, "[QUE] addBtCmd : cmd=%d, resp=%d", cmd_id, resp);
    if(resp)
    {
        pCmd = (BtCommand*)malloc(sizeof(BtCommand));
        if(pCmd)
        {
            pCmd->addr.sun_family = addr->sun_family;
            //strcpy(pCmd->addr.sun_path,addr->sun_path);
            memcpy(&pCmd->addr, addr, len+2);
            pCmd->addrlen = len;
            pCmd->cmd_id = cmd_id;
            pCmd->resp_id = resp;
            bt_prompt_trace_lv3(MOD_BT, "[QUE] Insert Cmd=%d", cmd_id);
            InsertTailList(&g_listHead, &pCmd->node);
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

/* retrieve the cmd in the cmd queue. NULL=not found */
BtCommand *removeBtCmd(unsigned long resp_id)
{
    BtCommand *pCmd = (BtCommand*)GetHeadList(&g_listHead);
    bt_prompt_trace_lv3(MOD_BT, "[QUE] removeBtCmd resp=%d", resp_id);
    while(&pCmd->node != &g_listHead)
    {
        if(pCmd->resp_id == resp_id)
        {
            RemoveEntryList(&pCmd->node);
            bt_prompt_trace_lv3(MOD_BT, "[QUE] cmd=%d found", pCmd->cmd_id);
            return pCmd;
        }
        pCmd = (BtCommand*)GetHeadList(&pCmd->node);
    }
    bt_prompt_trace_lv3(MOD_BT, "[QUE] cmd not found");
    return NULL;
}

/* Call this func to free cmd */
void freeBtCmd(BtCommand *cmd)
{
    bt_prompt_trace_lv3(MOD_BT, "[QUE] free cmd=%d", cmd->cmd_id);
    if(cmd)
        free(cmd);
}

