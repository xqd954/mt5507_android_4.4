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
#include "bluetooth_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bluetooth_ftps_struct.h"

// keep the memory object during using shared memory
static android::sp<android::IMemoryHeap> g_adp_ftpc_mem;
static android::sp<android::IMemoryHeap> g_adp_ftps_mem;

// keep the shared memory
U8* g_adp_ftpc_buffer;
U8* g_adp_ftps_buffer;

extern "C" {

    // will be called at profile init
    void btmtk_adp_ftpc_malloc_ashm_buffer() {
		char ashm[] = FTPC_ASHM_NAME;
		// start the ashm server(android service) with given name and size
		btmtk_ashm_malloc_memory(ashm, FTPC_MEM_BUFF_SIZE);

		// initialize the memory object and keep the shared memory
		g_adp_ftpc_mem = btmtk_ashm_get_memory(ashm);
		g_adp_ftpc_buffer = (U8*)g_adp_ftpc_mem->getBase();
    }   

    // return the gotten shared memory for profile use
    void* btmtk_adp_ftpc_get_ashm_buffer() {
		return g_adp_ftpc_buffer;
    }

    // release the gotten memory object (and shared memory)
    void btmtk_adp_ftpc_return_ashm_buffer() {
		g_adp_ftpc_mem = NULL;
		btmtk_ashm_free_memory(FTPC_ASHM_NAME);
    }

    // will be called at profile init
    void btmtk_adp_ftps_malloc_ashm_buffer() {
		char ashm[] = FTPS_ASHM_NAME;
		// start the ashm server(android service) with given name and size
		btmtk_ashm_malloc_memory(ashm, FTPS_MEM_BUFF_SIZE);

		// initialize the memory object and keep the shared memory
		g_adp_ftps_mem = btmtk_ashm_get_memory(ashm);
		g_adp_ftps_buffer = (U8*)g_adp_ftps_mem->getBase();
    }

    // return the gotten shared memory for profile use
    void* btmtk_adp_ftps_get_ashm_buffer() {
		return g_adp_ftps_buffer;
    }

    // release the gotten memory object (and shared memory)
    void btmtk_adp_ftps_return_ashm_buffer() {
		g_adp_ftps_mem = NULL;
		btmtk_ashm_free_memory(FTPS_ASHM_NAME);
    }
}
