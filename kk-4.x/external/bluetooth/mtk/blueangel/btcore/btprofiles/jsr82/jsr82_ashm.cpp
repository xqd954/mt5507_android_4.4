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
 * jsr82_ashm.cpp
 * 
 * This file implement share memory interface for JSR82 Core usage
 */

#if defined(__BT_JSR82__)

#ifdef BTMTK_ON_LINUX
#include "bt_types.h"
#include "bluetooth_jsr82_struct.h"
#include "bt_adp_ashm.h"
#include "bt_debug.h"

extern "C"
{
#include "bt_adp_log.h"
}

typedef struct _JSR82SharedMem
{
    U8 index;			// port number
    BT_BOOL inUse;
    android::sp<android::IMemoryHeap> IntshareMemObj_channel;
    android::sp<android::IMemoryHeap> IntshareMemObj_deliver_put;
    android::sp<android::IMemoryHeap> IntshareMemObj_deliver_get;
} JSR82IntAShareMemory;

JSR82IntAShareMemory g_Jsr82IntAShareMemory[JSR82_PORT_NUM];

extern "C"
{

void *jsr82_core_malloc_ashm(char *memName, U8 index, U8 type)
{
	android::sp<android::IMemoryHeap> memObj;
	U16 count;
	JSR82IntAShareMemory *jsr82SMemInt = NULL;
	U8 localCntxIndex = 0;

	bt_prompt_trace(MOD_BT, "[JSR82][JBT] jsr82_core_malloc_ashm: memName=%s, index=%d, type=%d", memName, index, type);
	
	if ((JSR82_ASHM_TYPE_CHANNEL_BUFFER != type) && 
		(JSR82_ASHM_TYPE_DELIVER_PUT != type) && 
		(JSR82_ASHM_TYPE_DELIVER_GET != type))
	{
		bt_prompt_trace(MOD_BT, "[JSR82][JBT] jsr82_core_malloc_ashm: invalid type.");
		return NULL;
	}

	// Check whether a buffer is allocated to this index
	for (count = 0; count < JSR82_PORT_NUM; count++)
	{
		jsr82SMemInt = &g_Jsr82IntAShareMemory[count];
		if ((index == jsr82SMemInt->index) && (TRUE == jsr82SMemInt->inUse))
		{
			localCntxIndex = count;
			if (((JSR82_ASHM_TYPE_CHANNEL_BUFFER == type) && (jsr82SMemInt->IntshareMemObj_channel != NULL)) || 
				((JSR82_ASHM_TYPE_DELIVER_PUT == type) && (jsr82SMemInt->IntshareMemObj_deliver_put != NULL)) || 
				((JSR82_ASHM_TYPE_DELIVER_GET == type) && (jsr82SMemInt->IntshareMemObj_deliver_get != NULL)))
			{
				bt_prompt_trace(MOD_BT, "[JSR82] jsr82_core_malloc_ashm: this buffer has already been assigned. index=%d, type=%d.", index, type);
				if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
				{
					return (U8*)((jsr82SMemInt->IntshareMemObj_channel)->getBase());
				}
				else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
				{
					return (U8*)((jsr82SMemInt->IntshareMemObj_deliver_put)->getBase());
				}
				else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
				{
					return (U8*)((jsr82SMemInt->IntshareMemObj_deliver_get)->getBase());
				}
			}
			break;
		}
	}

	// Allocate a new context
	if (JSR82_PORT_NUM == count)
	{
		for (count = 0; count < JSR82_PORT_NUM; count++)
		{
			jsr82SMemInt = &g_Jsr82IntAShareMemory[count];
			if (TRUE != jsr82SMemInt->inUse)
			{
				jsr82SMemInt->inUse = TRUE;
				jsr82SMemInt->index = index;
				localCntxIndex = count;
				break;
			}
		}
	}

	memObj = btmtk_ashm_get_memory(memName);
	if (memObj == NULL)
	{
		bt_prompt_trace(MOD_BT, "[JSR82] jsr82_core_malloc_ashm failed (%s).", memName);
		return NULL;
	}
	else
	{
		//  Return the shared memory & Record memory object
		if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
		{
			g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_channel = memObj;
		}
		else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
		{
			g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_deliver_put = memObj;
		}
		else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
		{
			g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_deliver_get = memObj;
		}
		bt_prompt_trace(MOD_BT, "[JSR82] jsr82_core_malloc_ashm: Ptr (%x).", memObj->getBase());
		return (U8*)memObj->getBase();
	}
}


void jsr82_core_free_ashm(U8 index, U8 type)
{
	U16 count;
	JSR82IntAShareMemory *jsr82SMemInt = NULL;
	U8 localCntxIndex = 0;

	// 1. Find the corresponding memory object (of each buffer type?)
	for (count = 0; count < JSR82_PORT_NUM; count++)
	{
		jsr82SMemInt = &g_Jsr82IntAShareMemory[count];
		if ((index == jsr82SMemInt->index) && (TRUE == jsr82SMemInt->inUse))
		{
			localCntxIndex = count;
			break;
		}
	}

	if (JSR82_PORT_NUM == count)
	{
		bt_prompt_trace(MOD_BT, "[JSR82][JBT] jsr82_core_free_ashm: This buffer is not allocated.");
		return ;
	}

	// 2. Release this memory object	
	if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
	{
		g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_channel = NULL;
	}
	else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
	{
		g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_deliver_put = NULL;
	}
	else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
	{
		g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_deliver_get = NULL;
	}
	else
	{
		bt_prompt_trace(MOD_BT, "[JSR82][JBT] jsr82_core_free_ashm: invalid type.");
	}

	// Clear this context if all related buffer are released
	if ((g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_channel == NULL) && 
		(g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_deliver_put == NULL) &&
		(g_Jsr82IntAShareMemory[localCntxIndex].IntshareMemObj_deliver_get == NULL))
	{
		g_Jsr82IntAShareMemory[localCntxIndex].index = 0;
		g_Jsr82IntAShareMemory[localCntxIndex].inUse = FALSE;
	}
}


} // extern "C"


#endif
#endif /* __BT_JSR82__ */
