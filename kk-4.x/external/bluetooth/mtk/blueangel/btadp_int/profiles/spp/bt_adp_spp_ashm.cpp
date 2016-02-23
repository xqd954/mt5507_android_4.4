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


#include "bt_adp_ashm.h"
#include "bt_types.h"
#include "bluetooth_spp_struct.h"

// keep the memory object during using shared memory
static android::sp<android::IMemoryHeap> g_adp_spp_get_mem;
static android::sp<android::IMemoryHeap> g_adp_spp_put_mem;

// keep the shared memory
U8* g_adp_spp_get_buffer;
U8* g_adp_spp_put_buffer;

extern "C"
{
// will be called at profile init
void btmtk_adp_spp_malloc_ashm_buffer()
{
	// start the ashm server(android service) with given name and size
	btmtk_ashm_malloc_memory((const char*)SPP_GET_ASHM_NAME, SPP_GET_DATA_SHARED_BUFFER_SIZE);
	btmtk_ashm_malloc_memory((const char*)SPP_PUT_ASHM_NAME, SPP_PUT_DATA_SHARED_BUFFER_SIZE);

	// initialize the memory object and keep the shared memory
	g_adp_spp_get_mem = btmtk_ashm_get_memory((const char*)SPP_GET_ASHM_NAME);
	g_adp_spp_get_buffer = (U8*)g_adp_spp_get_mem->getBase();

	g_adp_spp_put_mem = btmtk_ashm_get_memory((const char*)SPP_PUT_ASHM_NAME);
	g_adp_spp_put_buffer = (U8*)g_adp_spp_put_mem->getBase();
}

// return the gotten shared memory for profile use
void* btmtk_adp_spp_obtain_ashm_get_buffer()
{
	return g_adp_spp_get_buffer;
}

// release the gotten memory object (and shared memory)
void btmtk_adp_spp_free_ashm_get_buffer()
{
	g_adp_spp_get_buffer = NULL;
	btmtk_ashm_free_memory(SPP_GET_ASHM_NAME);
}

// return the gotten shared memory for profile use
void* btmtk_adp_spp_obtain_ashm_put_buffer()
{
	return g_adp_spp_put_buffer;
}

// release the gotten memory object (and shared memory)
void btmtk_adp_spp_free_ashm_put_buffer()
{
	g_adp_spp_put_buffer = NULL;
	btmtk_ashm_free_memory(SPP_PUT_ASHM_NAME);
}

}
