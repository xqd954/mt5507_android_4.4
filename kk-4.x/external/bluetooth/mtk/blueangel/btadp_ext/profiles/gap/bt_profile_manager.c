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
 * bt_profile_manager.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to expose common api use to manager profile connection
 *
 * Author:
 * -------
 * Liang Cheng
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

#include "utils/Log.h"

#include <stdlib.h>
#include <hardware/bluetooth.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h>


//#include "bttypes.h"
#include "bt_utils.h"
#include "bt_mmi.h"
#include "bt_ext_apis.h"
#include "bluetooth_struct.h"
#include "bt_gap_struct.h"
#include "bt_profile_manager.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "bt_profile_manager.c"

typedef struct {
	bt_addr_struct      bd_addr;
	int                 isbusy;
	profile_op_queue_t *op_list;
}dev_op;

typedef struct {
	ProfileStatus       state[NUM_OF_PROFILE];
}dev_state;

static bt_addr_struct                     g_cur_dev = {0, 0, 0};
static dev_op                             g_dev_oplist[MAX_DEV_NUM] = {0};  //dev0 is current dev dev1 is new dev
static dev_state                          g_dev_state[MAX_DEV_NUM] = {0};   //dev0 is current dev dev1 is new dev
static profile_state_changed_callback_t  *g_cb_list = NULL;

extern int btif_hf_connect( bt_addr_struct *bd_addr );
extern int btif_hf_disconnect( bt_addr_struct *bd_addr );
extern int btif_av_connect(bt_addr_struct *bd_addr);
extern int btif_av_disconnect(bt_addr_struct *bd_addr);

void insertNode(btmtk_list_header_struct **list, btmtk_list_header_struct *node)
{
    btmtk_list_header_struct *ptr = *list;

    if (*list == NULL)
    {
        *list = node;
    }
    else
    {
        while (ptr->next)
        {
            ptr = ptr->next;
        }
        ptr->next = node;
    }
}

int isEmpty(btmtk_list_header_struct **list)
{
	if(*list == NULL)
		return 1;

	return 0;
}

void removeNode(btmtk_list_header_struct **list, btmtk_list_header_struct *node)
{
    btmtk_list_header_struct *ptr = *list;
	btmtk_list_header_struct *cur = ptr;
    btmtk_list_header_struct *next = NULL;
	
    if (list == NULL || *list == NULL)
    	return;

	if(ptr == node)
	{
		*list = ptr->next;
		free(ptr);
		ptr = NULL;
	}
	else
	{
		cur = ptr;
		ptr = ptr->next;
	    while (ptr)
	    {
	    	if(ptr == node)
			{
				cur->next = ptr->next;
				free(ptr);
				ptr = NULL;
				break;
			}
			cur = ptr;
	        ptr = cur->next;
	    }
	}
}

void removeAll(btmtk_list_header_struct **list)
{
    btmtk_list_header_struct *ptr = *list;
	btmtk_list_header_struct *next = *list;
	
    if (list == NULL || *list == NULL)
    	return;

    while (ptr)
    {
    	next = ptr->next;
		free(ptr);
        ptr = next;
    }
	*list = NULL;
}

btmtk_list_header_struct *getHeader(btmtk_list_header_struct **list)
{
    if (list == NULL || *list == NULL)
    	return NULL;

    return *list;
}

void registerProfileStateChangedCB(ProfileStateChanged pCB)
{
	profile_state_changed_callback_t *ptr = NULL;
	
	ALOGI("%s\n", __FUNCTION__);
	ptr = (profile_state_changed_callback_t *)calloc(1, sizeof(profile_state_changed_callback_t));
	if(NULL == ptr)
	{
		ALOGE("No Memory\n");
		return;
	}
	ptr->func = pCB;
	insertNode((btmtk_list_header_struct **)&g_cb_list, (btmtk_list_header_struct *)ptr);
	return;
}

void unregisterProfileStateChangedCB(ProfileStateChanged pCB)
{
	profile_state_changed_callback_t *ptr = g_cb_list;

	ALOGI("%s\n", __FUNCTION__);
	while(ptr)
	{
		if(ptr->func == pCB)
		{
			removeNode((btmtk_list_header_struct **)&g_cb_list, (btmtk_list_header_struct *)ptr);
			break;
		}
		ptr = ptr->next;
	}
	
}

void ProfileStateChangedCB(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileStatus state)
{
	profile_state_changed_callback_t *ptr = g_cb_list;

    ALOGI("%s\n", __FUNCTION__);
	while(ptr)
	{
		if(ptr->func != NULL)
		{
			ptr->func(bd_addr, id, state);
		}
		ptr = ptr->next;
	}
}

int findDevice(bt_addr_struct *bd_addr)
{
	int i = 0;
	
	for(i = 0; i < MAX_DEV_NUM; i++)
	{
		if(btmtk_util_equal_bdaddr(&g_dev_oplist[i].bd_addr, bd_addr))
			return i;
	}
	//ALOGI("%s Find free device\n", __FUNCTION__);
	for(i = 0; i < MAX_DEV_NUM; i++)
	{
		if(isDisconnected(&g_dev_oplist[i].bd_addr))
			return i;
	}
	//ALOGW("%s all inused\n", __FUNCTION__);
    return MAX_DEV_NUM;
}

int isCurrentDevice(bt_addr_struct *bd_addr)
{
	if(btmtk_util_equal_bdaddr(&g_cur_dev, bd_addr))
		return 1;

	return 0;
}

dev_op *getDeviceOpList(bt_addr_struct *bd_addr)
{
	int dev_id = findDevice(bd_addr);
	
	//ALOGI("%s addr:[0x%lX:0x%X:0x%X], dev_id[%d]!\n",
	//	  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, dev_id);
	if(dev_id >= MAX_DEV_NUM)
	{
		return NULL;
	}
	return &g_dev_oplist[dev_id];
}

int isSingleConnection(PROFILE_enum id)
{
	if((PROFILE_A2DP == id) 
		|| (PROFILE_HFP == id))
		return 1;

	return 0;
}

int isBusy(bt_addr_struct *bd_addr)
{
    dev_op *devOp = getDeviceOpList(bd_addr);

	if(devOp == NULL)
	{
		return 0;
	}
	ALOGI("%s state:%s\n", __FUNCTION__, devOp->isbusy?"Busy":"Idle");
	return devOp->isbusy;
}

void setBusy(bt_addr_struct *bd_addr, int isbusy)
{
    dev_op *devOp = getDeviceOpList(bd_addr);

	if(devOp == NULL)
	{
		return;
	}
	ALOGI("%s state:%s\n", __FUNCTION__, isbusy?"Busy":"Idle");
	devOp->isbusy = isbusy;
}

BtStatus insertOp(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileOpType op_type)
{
	profile_op_queue_t *ptr = NULL;
	dev_op             *devOp = getDeviceOpList(bd_addr);
	
	if(devOp == NULL)
	{
		return BT_STATUS_FAILED;
	}
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d], op[%s]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id, op_type?"Disconnect":"Connect");
	ptr = (profile_op_queue_t *)calloc(1, sizeof(profile_op_queue_t));
	if(NULL == ptr)
	{
		ALOGE("%s No Memory!", __FUNCTION__);
		return BT_STATUS_FAILED;
	}
	ptr->node.profile_id = id;
	ptr->node.op = op_type;
	insertNode((btmtk_list_header_struct **)&devOp->op_list, (btmtk_list_header_struct *)ptr);
	return BT_STATUS_SUCCESS;
}

void removeOp(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileOpType op_type)
{
	profile_op_queue_t *opCur = NULL;
	profile_op_queue_t *opNext = NULL;
	dev_op             *devOp = getDeviceOpList(bd_addr);
	
	if(devOp == NULL || isEmpty((btmtk_list_header_struct **)&devOp->op_list))
	{
		return;
	}
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d], op[%s]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id, op_type?"Disconnect":"Connect");
	opCur = (profile_op_queue_t*)getHeader((btmtk_list_header_struct **)&devOp->op_list);
	while(opCur)
	{
		opNext = opCur->next;
		if((id == opCur->node.profile_id) && (op_type == opCur->node.op))
		{
			removeNode((btmtk_list_header_struct **)&devOp->op_list, (btmtk_list_header_struct *)opCur);
		}
		opCur = opNext;
	}
	return;
}

void removeAllOp(bt_addr_struct *bd_addr)
{
	dev_op *devOp = getDeviceOpList(bd_addr);
	
	if(devOp == NULL)
	{
		return;
	}
	ALOGI("%s addr:[0x%lX:0x%X:0x%X]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap);
	removeAll((btmtk_list_header_struct **)&devOp->op_list);
}

void setProfileState(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileStatus state)
{
	int dev_id = findDevice(bd_addr);

	if(dev_id >= MAX_DEV_NUM)
	{
		return;
	}
	
	g_dev_state[dev_id].state[id] = state;
}

ProfileStatus getProfileState(bt_addr_struct *bd_addr, PROFILE_enum id)
{
	int dev_id = findDevice(bd_addr);

	if(dev_id >= MAX_DEV_NUM)
	{
		return PROFILE_STATE_DISCONNECTED;
	}
	return g_dev_state[dev_id].state[id];
}

int isConnected(bt_addr_struct *bd_addr)
{
	int i = 0;
	int ret = 0;
	int state = PROFILE_STATE_DISCONNECTED;

	if(!btmtk_check_bdaddr(bd_addr))
		return ret;
	
	for(i = 0; i < NUM_OF_PROFILE; i++)
	{
		state = getProfileState(bd_addr, i);
		if(state == PROFILE_STATE_CONNECTED)
		{
			ret = 1;
			break;
		}
	}
    ALOGI("addr:[0x%lX:0x%X:0x%X] %s?[%s]!\n",
	       bd_addr->lap, bd_addr->uap, bd_addr->nap, __FUNCTION__, ret?"Yes":"No");
	return ret;
}

int isDisconnected(bt_addr_struct *bd_addr)
{
	int i = 0;
	int ret = 1;
	int state = PROFILE_STATE_DISCONNECTED;

	if(!btmtk_check_bdaddr(bd_addr))
		return ret;
	
	for(i = 0; i < NUM_OF_PROFILE; i++)
	{
		state = getProfileState(bd_addr, i);
		if(state != PROFILE_STATE_DISCONNECTED)
		{
			ret = 0;
			break;
		}
	}
	ALOGI("addr:[0x%lX:0x%X:0x%X] %s?[%s]!\n",
	       bd_addr->lap, bd_addr->uap, bd_addr->nap, __FUNCTION__, ret?"Yes":"No");
	return ret;
}

void notifyProfileStateChanged(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileStatus state)
{
	dev_op             *devOp = getDeviceOpList(bd_addr);
	int                 old_state = getProfileState(bd_addr, id);
    int                 new_state = state;
	profile_op_queue_t *opCur = NULL;
	profile_op_queue_t *opNext = NULL;
	
	if(devOp == NULL)
	{
		return;
	}

	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d], old[%d] ==> new[%d]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id, old_state, new_state);
	setProfileState(bd_addr, id, new_state);
	if(isCurrentDevice(bd_addr))
	{
	    int state = PROFILE_STATE_DISCONNECTED;

		if(new_state == PROFILE_STATE_CONNECTED || new_state == PROFILE_STATE_DISCONNECTED)
		{
		    setBusy(bd_addr, 0);				
			if(!isEmpty((btmtk_list_header_struct **)&devOp->op_list))
			{
			    opCur = (profile_op_queue_t *)getHeader((btmtk_list_header_struct **)&devOp->op_list);
				if(opCur->node.op == PROFILE_OP_CONNECT)
				{
					state = getProfileState(bd_addr, opCur->node.profile_id);
					if(state == PROFILE_STATE_DISCONNECTED)
				    	connect_profile_int(bd_addr, opCur->node.profile_id);
				}
				else if(opCur->node.op == PROFILE_OP_DISCONNECT)
				{
				    state = getProfileState(bd_addr, opCur->node.profile_id);
					if(state == PROFILE_STATE_CONNECTED)
						disconnect_profile_int(bd_addr, opCur->node.profile_id);
					else if(state == PROFILE_STATE_DISCONNECTED)
					{
						PROFILE_enum profile_id = opCur->node.profile_id;
						ProfileOpType profile_op = opCur->node.op;
						
						removeOp(bd_addr, profile_id, profile_op);
						notifyProfileStateChanged(bd_addr, profile_id, PROFILE_STATE_DISCONNECTED);
						return;
					}
				}
				removeOp(bd_addr, opCur->node.profile_id, opCur->node.op);
			}
		}
	}
	else
	{
		removeAll((btmtk_list_header_struct **)&devOp->op_list);
		if(new_state == PROFILE_STATE_CONNECTED || new_state == PROFILE_STATE_DISCONNECTED)
		{
		    setBusy(bd_addr, 0);
		}
	}
	ProfileStateChangedCB(bd_addr, id, new_state);
}

void init_profile_manager()
{
	memset(&g_cur_dev, 0, sizeof(bt_addr_struct));
    memset(g_dev_oplist, 0, sizeof(dev_op)*MAX_DEV_NUM);
	memset(g_dev_state, 0, sizeof(dev_state)*MAX_DEV_NUM);
    g_cb_list = NULL;
}

void deinit_profile_manager()
{
	memset(&g_cur_dev, 0, sizeof(bt_addr_struct));
    memset(g_dev_oplist, 0, sizeof(dev_op)*MAX_DEV_NUM);
	memset(g_dev_state, 0, sizeof(dev_state)*MAX_DEV_NUM);
    g_cb_list = NULL;
}


BtStatus connect_profile(bt_addr_struct *bd_addr, PROFILE_enum id)
{
	BtStatus ret = BT_STATUS_FAILED;
    btmtk_profile_connect_request_struct *ptr;
	ilm_struct ilm;

	
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id);
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_PROFILE_CONNECT_REQ;
	ptr = (btmtk_profile_connect_request_struct *)ilm.ilm_data;
	memcpy(&ptr->bd_addr, bd_addr, sizeof(bt_addr_struct));
	ptr->profile_id = id;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_profile_connect_request_struct)))
    {
    	ret = BT_STATUS_SUCCESS;	
    }
	return ret;
}

BtStatus disconnect_profile(bt_addr_struct *bd_addr, PROFILE_enum id)
{
	BtStatus ret = BT_STATUS_FAILED;
    btmtk_profile_disconnect_request_struct *ptr;
	ilm_struct ilm;

	
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id);
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_PROFILE_DISCONNECT_REQ;
	ptr = (btmtk_profile_disconnect_request_struct *)ilm.ilm_data;
	memcpy(&ptr->bd_addr, bd_addr, sizeof(bt_addr_struct));
	ptr->profile_id = id;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_profile_disconnect_request_struct)))
    {
    	ret = BT_STATUS_SUCCESS;	
    }
	return ret;
}

BtStatus handle_profile_connect(bt_addr_struct *bd_addr, PROFILE_enum id)
{
	int     state = PROFILE_STATE_DISCONNECTED;
	dev_op *devOp = getDeviceOpList(bd_addr);
	int     dev_id = 0;
	
	if(devOp == NULL)
	{
		ALOGW("%s null devOp list!\n", __FUNCTION__);
		return BT_STATUS_FAILED;
	}

	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d], old[%d]!\n",
		  __FUNCTION__,  bd_addr->lap, bd_addr->uap, bd_addr->nap, id, state);
	memcpy(&devOp->bd_addr, bd_addr, sizeof(bt_addr_struct));
	if(isSingleConnection(id))
	{
		for( dev_id = 0; dev_id < MAX_DEV_NUM; dev_id++ )
		{
			if(!btmtk_util_equal_bdaddr(&g_dev_oplist[dev_id].bd_addr, bd_addr))
			{
				removeOp(&g_dev_oplist[dev_id].bd_addr, id, PROFILE_OP_CONNECT);
				if((g_dev_state[dev_id].state[id] == PROFILE_STATE_CONNECTED)
					|| (g_dev_state[dev_id].state[id] == PROFILE_STATE_CONNECTING))
				{
					disconnect_profile_int(&g_dev_oplist[dev_id].bd_addr, id);
				}
			}
		}
			
	}

    state = getProfileState(bd_addr, id);
	if((state == PROFILE_STATE_DISCONNECTING) || isBusy(bd_addr))
	{
		insertOp(bd_addr, id, PROFILE_OP_CONNECT);
	}
	else if(state == PROFILE_STATE_DISCONNECTED)
	{
		connect_profile_int(bd_addr, id);
	}
	memcpy(&g_cur_dev, bd_addr, sizeof(bt_addr_struct));
	return BT_STATUS_SUCCESS;
}

BtStatus handle_profile_disconnect(bt_addr_struct *bd_addr, PROFILE_enum id)
{
	int     state = PROFILE_STATE_DISCONNECTED;
    dev_op *devOp = getDeviceOpList(bd_addr);
	profile_op_queue_t *opNode = NULL;

	if(devOp == NULL)
	{
		ALOGW("%s null devOp list!\n", __FUNCTION__);
		return BT_STATUS_FAILED;
	}
	
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d], old[%d]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id, state);
	memcpy(&devOp->bd_addr, bd_addr, sizeof(bt_addr_struct));
	memcpy(&g_cur_dev, bd_addr, sizeof(bt_addr_struct));
	removeOp(bd_addr, id, PROFILE_OP_CONNECT);
	state = getProfileState(bd_addr, id);
	if(((state == PROFILE_STATE_CONNECTED) && !isBusy(bd_addr))
		|| ((state == PROFILE_STATE_CONNECTING) && isBusy(bd_addr)))
	{
		disconnect_profile_int(bd_addr, id);
	}
	else
	{
		insertOp(bd_addr, id, PROFILE_OP_DISCONNECT);
	}
	/*
	if((state == PROFILE_STATE_CONNECTING)
	   || (state == PROFILE_STATE_CONNECTED))
	{
		disconnect_profile_int(bd_addr, id);
	}*/
	return BT_STATUS_SUCCESS;
}

BtStatus connect_profile_int(bt_addr_struct *bd_addr, PROFILE_enum id)
{
	int ret = BT_STATUS_FAILED;
	
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id);
	notifyProfileStateChanged(bd_addr, id, PROFILE_STATE_CONNECTING);
	switch(id)
	{
	case PROFILE_A2DP:
		ret = btif_av_connect(bd_addr);
		break;
	case PROFILE_HFP:
		ret = btif_hf_connect(bd_addr);
		break;
	default:
		ALOGW("ignore request!\n");
		return BT_STATUS_FAILED;
	}
	setBusy(bd_addr, 1);
	return ret;
}

BtStatus disconnect_profile_int(bt_addr_struct *bd_addr, PROFILE_enum id)
{
    int ret = BT_STATUS_FAILED;
	
	ALOGI("%s addr:[0x%lX:0x%X:0x%X], profile[%d]!\n",
		  __FUNCTION__, bd_addr->lap, bd_addr->uap, bd_addr->nap, id);
	notifyProfileStateChanged(bd_addr, id, PROFILE_STATE_DISCONNECTING);
	switch(id)
	{
	case PROFILE_A2DP:
		ret = btif_av_disconnect(bd_addr);
		break;
	case PROFILE_HFP:
		ret = btif_hf_disconnect(bd_addr);
		break;
	default:
		ALOGW("ignore request!\n");
		return BT_STATUS_FAILED;
	}
	setBusy(bd_addr, 1);
	return ret;
}

BtStatus disconnect_all()
{
	int dev_id = 0;
	int profile_id = 0;
	
	for( dev_id = 0; dev_id < MAX_DEV_NUM; dev_id++ )
	{
		for(profile_id = 0; profile_id < NUM_OF_PROFILE; profile_id ++)
		{
			if((g_dev_state[dev_id].state[profile_id] == PROFILE_STATE_CONNECTED)
				|| (g_dev_state[dev_id].state[profile_id] == PROFILE_STATE_CONNECTING))
			{
				disconnect_profile_int(&g_dev_oplist[dev_id].bd_addr, profile_id);
			}
		}
	}
	return BT_STATUS_SUCCESS;
}


