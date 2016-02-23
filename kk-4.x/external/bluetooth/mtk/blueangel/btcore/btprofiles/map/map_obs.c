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
 *   map_obs.c.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains primitive/event handlers for Message Access Profile (MAP).
 *
 * Author:
 * -------
 *   Autumn Li
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
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)

#include "bt_common.h"
#include "bttypes.h"
#include "utils.h"

#include "obex.h"
#include "goep.h"
#include "sec_adp.h"
#include "map_adp.h"
#include "map.h"

extern MapContext g_map_cntx;
map_obj_struct *maps_obs_new(U8 conn_id);

BtStatus map_obs_init(void)
{
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
    U8 i;
    /* Initialize the object store entries */
#ifdef __BT_MAPS_PROFILE__
    InitializeListHead(&g_map_cntx.obsListS);
    for (i = 0; i < MAPS_OBS_NUM; i++) 
    {
        InsertTailList(&g_map_cntx.obsListS, &g_map_cntx.obsEntriesS[i].node);
    }
#endif

#ifdef __BT_MAPC_PROFILE__
    InitializeListHead(&g_map_cntx.obsListC);
    for (i = 0; i < MAPC_OBS_NUM; i++) 
    {
        InsertTailList(&g_map_cntx.obsListC, &g_map_cntx.obsEntriesC[i].node);
    }
#endif
#endif

    return OB_STATUS_SUCCESS;
}

#ifdef __BT_MAPS_PROFILE__
map_obj_struct *maps_obs_new(U8 conn_id)
{
    map_obj_struct *obs = NULL;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_OBS_NEW, conn_id);
    if (IsListEmpty(&g_map_cntx.obsListS) == FALSE) 
    {
        obs = (map_obj_struct *)RemoveHeadList(&g_map_cntx.obsListS);
        OS_MemSet((U8 *)obs, 0x0, sizeof(map_obj_struct));
        obs->fhandle = -1;
        obs->conn_id = conn_id;
    }
    else 
    {
        Assert(0);
    }

    return obs;
}

BtStatus maps_obs_close(map_obj_struct *obj)
{
    BtStatus status = OB_STATUS_SUCCESS;

    Assert(obj);
    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_OBS_CLOSE, obj->conn_id);
    OS_MemSet((U8 *)obj, 0x0, sizeof(map_obj_struct));
    InsertTailList(&g_map_cntx.obsListS, &(obj->node));
    return status;
}

map_obj_struct *maps_obs_find(U8 conn_id)
{
    int i;
    for (i = 0; i < MAPS_OBS_NUM; i++) 
    {
        if(IsNodeOnList(&g_map_cntx.obsListS, &(g_map_cntx.obsEntriesS[i].node)) == FALSE && 
            g_map_cntx.obsEntriesS[i].conn_id == conn_id)
        {
            return &g_map_cntx.obsEntriesS[i];
        }
    }

    return NULL;

}

void maps_obs_check(map_obj_struct * obs)
{
	Assert(obs);

	switch (obs->type) 
	{
	case MAP_OBJ_MESSAGE_LIST:
		if (obs->u.message_list.flag_max_list_count == FALSE)
		{
			obs->u.message_list.max_list_count = MAP_MAX_LIST_COUNT;
		}
		if (obs->u.message_list.flag_subject_length == FALSE)
		{
			obs->u.message_list.subject_length = MAP_MAX_SUBJECT_LEN;
		}		
		break;
	case MAP_OBJ_FOLDER_LIST:
		if (obs->u.folder_list.flag_max_list_count == FALSE)
		{
			obs->u.folder_list.max_list_count = MAP_MAX_LIST_COUNT;
		}
		break;	
       default:
              break;
	}
	
}



#endif  /* __BT_MAPS_PROFILE__ */

#ifdef __BT_MAPC_PROFILE__
map_obj_struct *mapc_obs_new(U8 conn_id)
{
    map_obj_struct *obs = NULL;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_OBS_NEW, conn_id);
    if (IsListEmpty(&g_map_cntx.obsListC) == FALSE) 
    {
        obs = (map_obj_struct *)RemoveHeadList(&g_map_cntx.obsListC);
        OS_MemSet((U8 *)obs, 0x0, sizeof(map_obj_struct));
        obs->fhandle = -1;
        obs->conn_id = conn_id;
    }
    else 
    {
        Assert(0);
    }

    return obs;
}

BtStatus mapc_obs_close(map_obj_struct *obj)
{
    BtStatus status = OB_STATUS_SUCCESS;

    Assert(obj);
    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_OBS_CLOSE, obj->conn_id);
    OS_MemSet((U8 *)obj, 0x0, sizeof(map_obj_struct));
    InsertTailList(&g_map_cntx.obsListC, &(obj->node));
    return status;
}

map_obj_struct *mapc_obs_find(U8 conn_id)
{
    int i;
    for (i = 0; i < MAPC_OBS_NUM; i++) 
    {
        if(IsNodeOnList(&g_map_cntx.obsListC, &(g_map_cntx.obsEntriesC[i].node)) == FALSE && 
            g_map_cntx.obsEntriesC[i].conn_id == conn_id)
        {
            return &g_map_cntx.obsEntriesC[i];
        }
    }

    return NULL;

}
#endif  /* __BT_MAPC_PROFILE__ */
#endif /* defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__) */
