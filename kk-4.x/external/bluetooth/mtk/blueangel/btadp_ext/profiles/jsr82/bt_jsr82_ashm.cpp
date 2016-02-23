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
 * bt_jsr82_ashm.cpp
 * 
 * This file implement share memory interface for JSR82 usage
 */

#include "bt_types.h"
#include "bt_ext_debug.h"
#include "bluetooth_jsr82_struct.h"
#ifdef BTMTK_ON_LINUX
#include "ext_ashm.h"
#include "ext_os_api.h"


typedef struct _JSR82SharedMem
{
    U8 index;			// port number
    U32 transaction_ID;	// in type JSR82_ASHM_TYPE_CHANNEL_BUFFER, index is not yet assigned
    BT_BOOL inUse;
    android::sp<android::IMemoryHeap> shareMemObj_channel;
    android::sp<android::IMemoryHeap> shareMemObj_deliver_put;
    android::sp<android::IMemoryHeap> shareMemObj_deliver_get;
} JSR82AShareMemory;


#define JSR82_ASHM_NAME_HEADER "mtk.bt.profile.jsr82.ashm"
#define JSR82_ASHM_NAME_TYPE_CHANNEL_BUFFER ".channel."
#define JSR82_ASHM_NAME_TYPE_DELIVER_PUT ".deliver.put."
#define JSR82_ASHM_NAME_TYPE_DELIVER_GET ".deliver.get."


JSR82AShareMemory g_Jsr82AShareMemory[JSR82_PORT_NUM];


extern "C"
{

/*Notes: index == 0xFF indicates to create new ashm*/
void *btmtk_jsr82_malloc_ashm(U32 size, U8 index, U8 type, U8 *memName, U32 transaction_ID)
{
	char memNameLocal[JSR82_ASHM_NAME_LENGTH];
	char indexStr[10];
	android::sp<android::IMemoryHeap> memObj;
	U16 count;
	JSR82AShareMemory *jsr82SMem = NULL;
	U8 localCntxIndex = 0;

	bt_ext_log("[JSR82][ASHM] btmtk_jsr82_malloc_ashm: size=%lu, index=%d, type=%d", size, index, type);
	
	if ((JSR82_ASHM_TYPE_CHANNEL_BUFFER != type) && 
		(JSR82_ASHM_TYPE_DELIVER_PUT != type) && 
		(JSR82_ASHM_TYPE_DELIVER_GET != type))
	{
		bt_ext_log("[JSR82][ASHM] btmtk_jsr82_malloc_ashm: invalid type.");
		return NULL;
	}

	// Check whether a buffer is allocated to this index
	/*Just ignore the case that index is 0xFF*/
	for (count = 0; count < JSR82_PORT_NUM; count++)
	{
		jsr82SMem = &g_Jsr82AShareMemory[count];
		if ((index != 0xFF) && (index == jsr82SMem->index) && (TRUE == jsr82SMem->inUse))
		{
			localCntxIndex = count;
			if (((JSR82_ASHM_TYPE_CHANNEL_BUFFER == type) && (jsr82SMem->shareMemObj_channel != NULL)) || 
				((JSR82_ASHM_TYPE_DELIVER_PUT == type) && (jsr82SMem->shareMemObj_deliver_put != NULL)) || 
				((JSR82_ASHM_TYPE_DELIVER_GET == type) && (jsr82SMem->shareMemObj_deliver_get != NULL)))
			{
				bt_ext_log("[JSR82][ASHM] btmtk_jsr82_malloc_ashm: this buffer has already been assigned. index=%d, type=%d", index, type);
				memName = NULL;
				if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
				{
					return (U8*)((jsr82SMem->shareMemObj_channel)->getBase());
				}
				else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
				{
					return (U8*)((jsr82SMem->shareMemObj_channel)->getBase());
				}
				else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
				{
					return (U8*)((jsr82SMem->shareMemObj_channel)->getBase());
				}
			}
			break;
		}
	}

	// Allocate a new context
	if (JSR82_PORT_NUM <= count)
	{
		for (count = 0; count < JSR82_PORT_NUM; count++)
		{
			jsr82SMem = &g_Jsr82AShareMemory[count];
			if (TRUE != jsr82SMem->inUse)
			{
				jsr82SMem->inUse = TRUE;
				if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
				{
					jsr82SMem->transaction_ID = transaction_ID;
				}
				jsr82SMem->index = index;
				localCntxIndex = count;
				break;
			}
		}
		/*Notes: no avaliable ashm*/
		if (JSR82_PORT_NUM <= count)
		{
			bt_ext_err("[JSR82][ASHM] error, btmtk_jsr82_malloc_ashm: No more ashm is available");
			return NULL;
		}
	}

	

	// Start the ashm server(android service) with given name and size
	// Customize each size for btmtk_jsr82_malloc_ashm() & multi-port
	btmtk_os_memset(memNameLocal, 0, sizeof(memNameLocal));
	btmtk_os_memset(indexStr, 0, sizeof(indexStr));
	btmtk_os_strcpy(memNameLocal, JSR82_ASHM_NAME_HEADER);
	if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
	{
		btmtk_os_strcat(memNameLocal, JSR82_ASHM_NAME_TYPE_CHANNEL_BUFFER);
	}
	else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
	{
		btmtk_os_strcat(memNameLocal, JSR82_ASHM_NAME_TYPE_DELIVER_PUT);
	}
	else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
	{
		btmtk_os_strcat(memNameLocal, JSR82_ASHM_NAME_TYPE_DELIVER_GET);
	}
	sprintf(indexStr, "%d", localCntxIndex);
	btmtk_os_strcat(memNameLocal, indexStr);
	btmtk_os_memcpy(memName, memNameLocal, JSR82_ASHM_NAME_LENGTH);
	ext_ashm_start_service(memNameLocal, (int) size);

	// Initialize the memory object
	// Keep this memory object
	memObj = ext_ashm_get_memory(memNameLocal);
	if (memObj == NULL)
	{
		bt_ext_log("[JSR82][ASHM] JBT btmtk_jsr82_malloc_ashm failed");
		return NULL;
	}
	else
	{
		//  Return the shared memory & Record memory object
		if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
		{
			g_Jsr82AShareMemory[localCntxIndex].shareMemObj_channel = memObj;
		}
		else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
		{
			g_Jsr82AShareMemory[localCntxIndex].shareMemObj_deliver_put = memObj;
		}
		else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
		{
			g_Jsr82AShareMemory[localCntxIndex].shareMemObj_deliver_get = memObj;
		}
		else
		{
			bt_ext_log("[JSR82][ASHM] btmtk_jsr82_malloc_ashm: invalid type.");
			return NULL;
		}
		bt_ext_log("[JSR82][ASHM] btmtk_jsr82_malloc_ashm: ptr=%p, memName=%s", memObj->getBase(), memName);
		return (U8 *)memObj->getBase();
	}
}


void btmtk_jsr82_free_ashm(U8 index, U8 type)
{
	U16 count;
	JSR82AShareMemory *jsr82SMem = NULL;
	U8 localCntxIndex = 0;

        bt_ext_log("[JSR82][ASHM] btmtk_jsr82_free_ashm(index=%d, type=%d)", index, type);
	// 1. Find the corresponding memory object (of each buffer type?)
	for (count = 0; count < JSR82_PORT_NUM; count++)
	{
		jsr82SMem = &g_Jsr82AShareMemory[count];
		if ((index == jsr82SMem->index) && (TRUE == jsr82SMem->inUse))
		{
        		bt_ext_log("[JSR82][ASHM] free count=%d", count);
			localCntxIndex = count;
			break;
		}
	}

	if (JSR82_PORT_NUM == count)
	{
		bt_ext_log("[JSR82][ASHM] btmtk_jsr82_free_ashm: This buffer is not allocated.");
		return ;
	}

	// 2. Release this memory object	
	if (JSR82_ASHM_TYPE_CHANNEL_BUFFER == type)
	{
		g_Jsr82AShareMemory[localCntxIndex].shareMemObj_channel = NULL;
	}
	else if (JSR82_ASHM_TYPE_DELIVER_PUT == type)
	{
		g_Jsr82AShareMemory[localCntxIndex].shareMemObj_deliver_put = NULL;
	}
	else if (JSR82_ASHM_TYPE_DELIVER_GET == type)
	{
		g_Jsr82AShareMemory[localCntxIndex].shareMemObj_deliver_get = NULL;
	}
	else
	{
		bt_ext_log("[JSR82][ASHM] btmtk_jsr82_free_ashm: invalid type.");
	}

	// Clear this context if all related buffer are released
	if ((g_Jsr82AShareMemory[localCntxIndex].shareMemObj_channel == NULL) && 
		(g_Jsr82AShareMemory[localCntxIndex].shareMemObj_deliver_put == NULL) &&
		(g_Jsr82AShareMemory[localCntxIndex].shareMemObj_deliver_get == NULL))
	{
	       bt_ext_log("[JSR82][ASHM] Clear share memory context %d", localCntxIndex);
		btmtk_os_memset(&g_Jsr82AShareMemory[localCntxIndex], 0x0, sizeof(JSR82AShareMemory));
		g_Jsr82AShareMemory[localCntxIndex].index = 0xFE;
		g_Jsr82AShareMemory[localCntxIndex].inUse = FALSE;
	}
}


void btmtk_jsr82_update_ashm_index(U8 index, U32 transaction_ID)
{
	U16 count;
	JSR82AShareMemory *jsr82SMem = NULL;

        bt_ext_log("[JSR82][ASHM] btmtk_jsr82_update_ashm_index(index=%d, transaction_ID=%lu)", index, transaction_ID);
	// Check whether a buffer is allocated to this index
	for (count = 0; count < JSR82_PORT_NUM; count++)
	{
		jsr82SMem = &g_Jsr82AShareMemory[count];
		if ((transaction_ID == jsr82SMem->transaction_ID) && (TRUE == jsr82SMem->inUse))
		{
			jsr82SMem->index = index;
			return ;
		}
	}
}


} // extern "C"


#endif // BTMTK_ON_LINUX

