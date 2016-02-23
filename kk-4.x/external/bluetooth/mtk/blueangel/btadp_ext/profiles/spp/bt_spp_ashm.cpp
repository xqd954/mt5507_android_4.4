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


#include "bluetooth_spp_struct.h"
#include "ext_ashm.h"
#include "bt_ext_debug.h"

// keep the memory object during using shared memory
static android::sp<android::IMemoryHeap> g_spp_get_mem;
static android::sp<android::IMemoryHeap> g_spp_put_mem;

extern "C"
{
// spp ext adp will call this only once and keep the shared memory
void* btmtk_spp_obtain_ashm_get_buffer()
{
	// initialize the memory object
	g_spp_get_mem = ext_ashm_get_memory((char*)SPP_GET_ASHM_NAME);
	if (g_spp_get_mem == NULL)
	{
		bt_ext_err("spp get ashm fail");
		return NULL;
	}
	else
	{
		//  return the shared memory
		return g_spp_get_mem->getBase();
	}
}

// release the gotten memory object (and shared memory)
void btmtk_spp_free_ashm_get_buffer()
{
	g_spp_get_mem = NULL;
}

void* btmtk_spp_obtain_ashm_put_buffer()
{
	// initialize the memory object
	g_spp_put_mem = ext_ashm_get_memory((char*)SPP_PUT_ASHM_NAME);
	if (g_spp_put_mem == NULL)
	{
		bt_ext_err("spp put ashm fail");
		return NULL;
	}
	else
	{
		//  return the shared memory
		return g_spp_put_mem->getBase();
	}
}

// release the gotten memory object (and shared memory)
void btmtk_spp_free_ashm_put_buffer()
{
	g_spp_put_mem = NULL;
}

}

