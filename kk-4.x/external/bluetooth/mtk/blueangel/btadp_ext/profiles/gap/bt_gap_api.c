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
 * Bt_gap_api.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to expose common api for profile&gap
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
#include <cutils/properties.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <private/android_filesystem_config.h>
#include "cutils/sockets.h"
#ifndef MTK_BSP_PACKAGE
#include "custom_prop.h"
#endif
#include <pthread.h>

#include "btproperty.h"
#include "bt_adp_cust.h"
#include "bt_cust_api.h"
#include "bt_ext_timer.h"
#include "bt_ext_apis.h"
#include "bt_ext_common.h"
#include "bt_gap_struct.h"
#include "bt_gap_hdl.h"
#include "bt_utils.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bt_profile_manager.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "bt_gap_api.c"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
bt_gap_context_t g_ext_gap_context;

const unsigned short g_sdp_uuid[] = {
BTMTK_SDP_UUID_HEADSET,
BTMTK_SDP_UUID_HEADSET_AUDIO_GATEWAY,
BTMTK_SDP_UUID_HF_HANDSFREE,
BTMTK_SDP_UUID_AG_HANDSFREE,
BTMTK_SDP_UUID_AUDIO_SOURCE,
BTMTK_SDP_UUID_AUDIO_SINK,
BTMTK_SDP_UUID_AV_REMOTE_CONTROL_TARGET,
BTMTK_SDP_UUID_ADV_AUDIO_DISTRIBUTION,
BTMTK_SDP_UUID_AV_REMOTE_CONTROL,
BTMTK_SDP_UUID_OBEX_OBJECT_PUSH,
BTMTK_SDP_UUID_SERIAL_PORT,
BTMTK_SDP_UUID_DIALUP_NETWORKING,
BTMTK_SDP_UUID_OBEX_FILE_TRANSFER,
BTMTK_SDP_UUID_PANU,
BTMTK_SDP_UUID_PAN_NAP,
BTMTK_SDP_UUID_PAN_GN,
BTMTK_SDP_UUID_DIRECT_PRINTING,
BTMTK_SDP_UUID_IMAGING_RESPONDER,
BTMTK_SDP_UUID_BASIC_PRINTING,
BTMTK_SDP_UUID_HUMAN_INTERFACE,
BTMTK_SDP_UUID_SAP,
BTMTK_SDP_UUID_PBAP_PCE,
BTMTK_SDP_UUID_PBAP_PSE,
BTMTK_SDP_UUID_GENERIC_AUDIO,
BTMTK_SDP_UUID_MAP,
BTMTK_SDP_UUID_INSECURE,
BTMTK_SDP_UUID_SECURE,
BTMTK_SDP_UUID_FAX,
BTMTK_SDP_UUID_IRMC_SYNC,
0};

const unsigned short g_sdp_uuid_le[] = {
BTMTK_SDP_UUID_PRX,
0};

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

extern void *btif_task(void *arg);
kal_bool btmtk_gap_remove_bond(bt_addr_struct *remote_addr);
void btmtk_gap_cancel_rebond();
kal_bool btmtk_config_hci_logging(uint8_t enable);

btmtk_device_entry_struct *btmtk_paired_dev_cache_find(bt_addr_struct *addr)
{
    int i;

    ALOGI("[GAP] btmtk_paired_dev_cache_find: addr=0x%lX:0x%X:0x%X", addr->lap, addr->uap, addr->nap);
    for (i = 0; i < g_ext_gap_context.paired_cache_no; i++)
    {
        if (btmtk_util_equal_bdaddr(&g_ext_gap_context.paired_dev_cache[i].addr, addr))
        {
            ALOGI("[GAP] btmtk_paired_dev_cache_find found");
            return &g_ext_gap_context.paired_dev_cache[i];
        }
    }
    ALOGW("[GAP] btmtk_paired_dev_cache_find not found");
    return NULL;
}

void btmtk_host_cache_write()
{
    int fd, len;
	int ret = 0;
    int size = sizeof(btmtk_host_cache_struct);

    ALOGI("[GAP] btmtk_host_cache_write");
    if ((fd = open(BTMTK_HOST_CACHE_PATH, O_WRONLY | O_CREAT, 0666)) < 0)
    {
        ALOGE("[GAP] btmtk_host_cache_write open error %d: %s", errno, strerror(errno));
        return;
    }

	chown(BTMTK_HOST_CACHE_PATH, -1, AID_NET_BT_STACK);
    chmod(BTMTK_HOST_CACHE_PATH, 0660);
	len = write(fd, &g_ext_gap_context.host_cache, size);
	if (len < 0 || len != size) 
    {
        ALOGE("[GAP] btmtk_host_cache_write %d byte error %d: %s", len, errno, strerror(errno));
	}
    close(fd);
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_host_cache_init
 * DESCRIPTION
 *  Read all paired device cache data from file
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_host_cache_init()
{
    int fd;
    struct stat sts;
    int size = sizeof(btmtk_host_cache_struct);
    
    ALOGI("[GAP] btmtk_host_cache_init");
    memset(&g_ext_gap_context.host_cache, 0x0, size);
    if (stat(BTMTK_HOST_CACHE_PATH, &sts) == -1 || 
        sts.st_size != size ||
        (fd = open(BTMTK_HOST_CACHE_PATH, O_RDONLY)) < 0)
    {
    	unsigned int value = 0;
		unsigned int ret = 0;
		BT_CUST_ID id = 0;
		
		id = getCustID("LocalNameCustPermission");
    	ret = getCustValue(id, &value);
		ALOGI("[GAP] btmtk_host_cache_init btGetCustvalue ret:%d, value:%d",ret, value);
		if(ret == 0 || value == 0)
		{
#ifdef MTK_BSP_PACKAGE
			unlink(BTMTK_HOST_CACHE_PATH);
			strncpy(g_ext_gap_context.host_cache.name, BTMTK_ANDROID_DEFAULT_LOCAL_NAME, BTBM_ADP_MAX_NAME_LEN);
#else
			char value[MAX_VALUE_LEN];
			int result = custom_get_string(MODULE_BLUETOOTH, HOST_NAME, value, BTMTK_ANDROID_DEFAULT_LOCAL_NAME);
			unlink(BTMTK_HOST_CACHE_PATH);
			strncpy(g_ext_gap_context.host_cache.name, value, BTBM_ADP_MAX_NAME_LEN);
#endif
		}
		else
		{
    		char property_value[PATH_MAX];
			int ret = property_get("ro.product.model", property_value, NULL);
	        if(ret <=0 ){
	            ALOGE("get property %s failed : %d", "ro.product.model", ret);
	        }else{
	            ALOGI("get property ro.product.model success : %s", property_value);
	            strncpy(g_ext_gap_context.host_cache.name, property_value, BTBM_ADP_MAX_NAME_LEN);              
	        }
		}
		g_ext_gap_context.host_cache.scan_mode = BTBM_ADP_P_OFF_I_OFF;
		g_ext_gap_context.host_cache.scan_mode_timeout = 120;
        btmtk_host_cache_write();
        ALOGE("[GAP] btmtk_host_cache_init open error %d: %s", errno, strerror(errno));
    }
    else
    {
        int read_byte = TEMP_FAILURE_RETRY(read(fd, &g_ext_gap_context.host_cache, size)), i;
        if (read_byte < 0) 
        {
            ALOGE("[GAP] btmtk_host_cache_init read error %d: %s", errno, strerror(errno));
        } 
        else if (read_byte != size) 
        {
            ALOGE("[GAP] btmtk_host_cache_init read error(%d bytes read) %d: %s", read_byte, errno, strerror(errno));
        }
		else
		{
			ALOGI("[GAP] scan_mode %d, scan_mode_timeout %d!\n", g_ext_gap_context.host_cache.scan_mode, g_ext_gap_context.host_cache.scan_mode_timeout);
			g_ext_gap_context.host_cache.scan_mode = BTBM_ADP_P_OFF_I_OFF;
		}
        close(fd);
    }
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_paired_dev_cache_init
 * DESCRIPTION
 *  Read all paired device cache data from file
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_paired_dev_cache_init()
{
    int fd;
    struct stat sts;
    int size = BTBM_ADP_MAX_PAIRED_LIST_NO * sizeof(btmtk_device_entry_struct);
    
    ALOGI("[GAP] btmtk_paired_dev_cache_init");
    memset(g_ext_gap_context.paired_dev_cache, 0x0, size);
    if (stat(BTMTK_DEV_CACHE_PATH, &sts) == -1 || 
        sts.st_size != size ||
        (fd = open(BTMTK_DEV_CACHE_PATH, O_RDONLY)) < 0)
    {
        unlink(BTMTK_DEV_CACHE_PATH);
        ALOGE("[GAP] btmtk_paired_dev_cache_init open error %d: %s", errno, strerror(errno));
    }
    else
    {
        int read_byte = TEMP_FAILURE_RETRY(read(fd, g_ext_gap_context.paired_dev_cache, size)), i;
        if (read_byte < 0) 
        {
            ALOGE("[GAP] btmtk_paired_dev_cache_init read error %d: %s", errno, strerror(errno));
        } 
        else if (read_byte != size) 
        {
            ALOGE("[GAP] btmtk_paired_dev_cache_init read error(%d bytes read) %d: %s", read_byte, errno, strerror(errno));
        }
        close(fd);

        for (i = 0; i < BTBM_ADP_MAX_PAIRED_LIST_NO; i++)
        {
            if (g_ext_gap_context.paired_dev_cache[i].addr.lap == 0 && 
                g_ext_gap_context.paired_dev_cache[i].addr.uap == 0 && 
                g_ext_gap_context.paired_dev_cache[i].addr.nap == 0)
            {
                break;
            }
        }
        g_ext_gap_context.paired_cache_no = i;
    }
}


void btmtk_paired_dev_cache_write()
{
    int fd, len;
	int ret = 0;
    int size = BTBM_ADP_MAX_PAIRED_LIST_NO * sizeof(btmtk_device_entry_struct);

    ALOGI("[GAP] btmtk_paired_dev_cache_write");
    if ((fd = open(BTMTK_DEV_CACHE_PATH, O_WRONLY | O_CREAT, 0666)) < 0)
    {
        ALOGE("[GAP] btmtk_paired_dev_cache_write open error %d: %s", errno, strerror(errno));
        return;
    }
	
	chown(BTMTK_DEV_CACHE_PATH, -1, AID_NET_BT_STACK);
    chmod(BTMTK_DEV_CACHE_PATH, 0660);
	len = write(fd, &g_ext_gap_context.paired_dev_cache, size);
	if (len < 0 || len != size) 
    {
        ALOGE("[GAP] btmtk_paired_dev_cache_write %d byte error %d: %s", len, errno, strerror(errno));
	}
    close(fd);
}


void btmtk_paired_dev_cache_del(bt_addr_struct *addr)
{
    int i, idx = -1;

    ALOGI("[GAP] btmtk_paired_dev_cache_del: addr=0x%lX:%X:%X", addr->lap, addr->uap, addr->nap);
    for (i = 0; i < g_ext_gap_context.paired_cache_no; i++)
    {
        if (btmtk_util_equal_bdaddr(&g_ext_gap_context.paired_dev_cache[i].addr, addr))
        {
            ALOGI("[GAP] btmtk_paired_dev_cache_del: found");
            idx = i;
            break;
        }
    }

    /* pack */
    if (idx != -1)
    {
        memset(&g_ext_gap_context.paired_dev_cache[idx], 0x0, sizeof(btmtk_device_entry_struct));
        for (i = idx + 1; i < g_ext_gap_context.paired_cache_no; i++)
        {
            memcpy(&g_ext_gap_context.paired_dev_cache[i - 1], &g_ext_gap_context.paired_dev_cache[i], sizeof(btmtk_device_entry_struct));
        }
        memset(&g_ext_gap_context.paired_dev_cache[g_ext_gap_context.paired_cache_no - 1], 0x0, sizeof(btmtk_device_entry_struct));
        g_ext_gap_context.paired_cache_no--;
    }
    ALOGI("[GAP] btmtk_paired_dev_cache_del: %d device paired", g_ext_gap_context.paired_cache_no);
    btmtk_paired_dev_cache_write();
}


void btmtk_paired_dev_cache_add(btmtk_device_entry_struct *entry)
{
    int i;
    btmtk_device_entry_struct *ptr = NULL;

    ALOGI("[GAP] btmtk_paired_dev_cache_add: addr=0x%lX:0x%X:0x%X", entry->addr.lap, entry->addr.uap, entry->addr.nap);
    entry->paired = BTMTK_BOND_STATE_BONDED;
    for (i = 0; i < g_ext_gap_context.paired_cache_no; i++)
    {
        if (btmtk_util_equal_bdaddr(&g_ext_gap_context.paired_dev_cache[i].addr, &entry->addr))
        {
            ALOGI("[GAP] btmtk_paired_dev_cache_add: already paired");
            ptr = &g_ext_gap_context.paired_dev_cache[i];
            break;
        }
    }
    if (!ptr)
    {
        if (g_ext_gap_context.paired_cache_no == BTBM_ADP_MAX_PAIRED_LIST_NO)
        {
            ALOGE("[GAP] btmtk_paired_dev_cache_add: exceed %d entries", BTBM_ADP_MAX_PAIRED_LIST_NO);
            return;
        }
        ptr = &g_ext_gap_context.paired_dev_cache[g_ext_gap_context.paired_cache_no++];
    }
    ALOGI("[GAP] btmtk_paired_dev_cache_add: %d device paired", g_ext_gap_context.paired_cache_no);
    memcpy(ptr, entry, sizeof(btmtk_device_entry_struct));
    btmtk_paired_dev_cache_write();
}

btmtk_device_entry_struct *btmtk_inquired_dev_cache_find(bt_addr_struct *addr)
{
    int i;

    ALOGI("[GAP] btmtk_inquired_dev_cache_find: addr=0x%lX:0x%X:0x%X", addr->lap, addr->uap, addr->nap);
    for (i = 0; i < g_ext_gap_context.inquired_cache_no; i++)
    {
        if (btmtk_util_equal_bdaddr(&g_ext_gap_context.inquired_dev_cache[i].addr, addr))
        {
            ALOGI("[GAP] btmtk_inquired_dev_cache_find found");
            return &g_ext_gap_context.inquired_dev_cache[i];
        }
    }
    ALOGW("[GAP] btmtk_inquired_dev_cache_find not found");
    return NULL;
}

btmtk_device_entry_struct *btmtk_inquired_dev_cache_add(btmtk_device_entry_struct *entry)
{
    int i;
    btmtk_device_entry_struct *ptr = NULL;
	if (entry == NULL)
	{
		ALOGE("[GAP] btmtk_inquired_dev_cache_add: entry is null ");
		return NULL;
	}

    ALOGI("[GAP] btmtk_inquired_dev_cache_add: addr=0x%lX:0x%X:0x%X", entry->addr.lap, entry->addr.uap, entry->addr.nap);

    for (i = 0; i < g_ext_gap_context.inquired_cache_no; i++)
    {
        if (btmtk_util_equal_bdaddr(&g_ext_gap_context.inquired_dev_cache[i].addr, &entry->addr))
        {
            ALOGI("[GAP] btmtk_inquired_dev_cache_add: already paired");
            ptr = &g_ext_gap_context.inquired_dev_cache[i];
            break;
        }
    }

    if (!ptr)
    {
        if (g_ext_gap_context.inquired_cache_no == BTBM_ADP_MAX_INQUIRY_NO)
        {
            ALOGE("[GAP] btmtk_inquired_dev_cache_add: exceed %d entries", BTBM_ADP_MAX_INQUIRY_NO);
            return NULL;
        }
        ptr = &g_ext_gap_context.inquired_dev_cache[g_ext_gap_context.inquired_cache_no++];
    }
    ALOGI("[GAP] btmtk_inquired_dev_cache_add: %d device inquired", g_ext_gap_context.inquired_cache_no);
    memcpy(ptr, entry, sizeof(btmtk_device_entry_struct));
	return ptr;
}

void btmtk_inquired_dev_cache_clear()
{
    ALOGI("[GAP] btmtk_inquired_dev_cache_clear");
	memset(g_ext_gap_context.inquired_dev_cache, 0x0, sizeof(btmtk_device_entry_struct)*BTBM_ADP_MAX_INQUIRY_NO);
    g_ext_gap_context.inquired_cache_no = 0;
}

btmtk_bond_req_struct *btmtk_util_find_bond_request(bt_addr_struct *addr)
{
    btmtk_bond_req_struct *ptr = g_ext_gap_context.requests;
    ALOGI("[GAP] btmtk_util_find_bond_request search 0x%lX:0x%X:0x%X", addr->lap, addr->uap, addr->nap);
    while (ptr)
    {
        ALOGI("[GAP] btmtk_util_find_bond_request found 0x%lX:0x%X:0x%X", ptr->addr.lap, ptr->addr.uap, ptr->addr.nap);
        if (btmtk_util_equal_bdaddr(&ptr->addr, addr))
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

btmtk_unbond_pending_list *btmtk_util_unbond_pending_list_find(bt_addr_struct *addr)
{
	btmtk_unbond_pending_list *ptr = g_ext_gap_context.unbond_pending_list;

	while(ptr)
	{
		if(btmtk_util_equal_bdaddr(&ptr->bd_addr, addr))
			return ptr;

		ptr = ptr->next;
	}
	return NULL;
}

void btmtk_util_unbond_pending_list_add(bt_addr_struct *addr)
{
	btmtk_unbond_pending_list *ptr = NULL;
	
	ALOGI("%s addr:[0x%lX:0x%X:0x%X]!\n",
		  __FUNCTION__, addr->lap, addr->uap, addr->nap);
	ptr = (btmtk_unbond_pending_list *)calloc(1, sizeof(btmtk_unbond_pending_list));
	if(NULL == ptr)
	{
		ALOGE("%s No Memory!", __FUNCTION__);
		return;
	}
	memcpy(&ptr->bd_addr, addr, sizeof(bt_addr_struct));
	btmtk_util_list_append((btmtk_list_header_struct **)&g_ext_gap_context.unbond_pending_list, (btmtk_list_header_struct *)ptr);
}

void btmtk_util_unbond_pending_list_del(bt_addr_struct *addr)
{
	btmtk_unbond_pending_list *ptr = btmtk_util_unbond_pending_list_find(addr);

	if(NULL == ptr)
	{
		ALOGE("%s No Memory!", __FUNCTION__);
		return;
	}

	btmtk_util_list_remove((btmtk_list_header_struct **)&g_ext_gap_context.unbond_pending_list, (btmtk_list_header_struct *)ptr);
}

void btmtk_gap_disconnect(bt_addr_struct *addr)
{
	int state = PROFILE_STATE_DISCONNECTED;
	int id = 0;

	for(id = 0; id < NUM_OF_PROFILE; id++)
	{
		disconnect_profile(addr, id);
	}	
}

static void profileStateChanged(bt_addr_struct *bd_addr, int id, int state)
{
	btmtk_unbond_pending_list *node = btmtk_util_unbond_pending_list_find(bd_addr);

	if(isDisconnected(bd_addr))
	{
		if(node != NULL)
		{
			btmtk_gap_remove_bond(&node->bd_addr);
			btmtk_util_unbond_pending_list_del(&node->bd_addr);
		}
	}
}

void get_default_cod()
{
	kal_uint32 cod = 0;
	unsigned int ret = 0;
	BT_CUST_ID id = 0;
	
	id = getCustID("ClassOfDevice");
	ret = getCustValue(id, (unsigned int *)&cod);
	if(ret == 0 || cod == 0x004A020C)
	{
		g_ext_gap_context.local_cod = BT_DEFAULT_COD;
	}
	else
	{
		g_ext_gap_context.local_cod = cod;
	}
}

kal_bool btmtk_gap_init()
{
	pthread_attr_t attr1;
    kal_bool ret = KAL_FALSE;

	/* Initialize mutex and condition variable objects for events and timeouts */
	memset(&g_ext_gap_context, 0, sizeof(bt_gap_context_t));
    pthread_mutex_init(&g_ext_gap_context.thread_mutex, NULL);
    pthread_cond_init (&g_ext_gap_context.thread_cond, NULL);
    pthread_attr_init(&attr1);
	
	initCust(NULL);
	get_default_cod();
	btmtk_host_cache_init();
	btmtk_paired_dev_cache_init();
	init_profile_manager();
	
	g_ext_gap_context.bt_status = BTMTK_POWER_STATE_OFF;
	
	g_ext_gap_context.appsock = socket_local_server(BT_SOCK_NAME_APP_GAP, 
											        ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
											        SOCK_DGRAM);
	if (g_ext_gap_context.appsock < 0)
	{
		ALOGE("create gap app socket failed : %s, errno=%d!\n", strerror(errno), errno);
		return ret;
	}
	
    g_ext_gap_context.pollData = (struct pollfd *)calloc(DEFAULT_INITIAL_POLLFD_COUNT, sizeof(struct pollfd));
    if (!g_ext_gap_context.pollData) {
        ALOGE("out of memory error!\n");
        goto done;
    }

    g_ext_gap_context.pollDataSize = DEFAULT_INITIAL_POLLFD_COUNT;
    g_ext_gap_context.pollMemberCount = 0;
	btmtk_profile_register(PROFILE_GAP, btmtk_ext_gap_message_handler, &g_ext_gap_context);	
	registerProfileStateChangedCB((ProfileStateChanged)profileStateChanged);
    pthread_create(&(g_ext_gap_context.bt_evt_task_id), NULL, btif_task, &g_ext_gap_context);
    ret = KAL_TRUE;

done:
    if (KAL_FALSE == ret) {
        if (g_ext_gap_context.pollData) 
			free(g_ext_gap_context.pollData);
        g_ext_gap_context.pollData = NULL;
        g_ext_gap_context.pollDataSize = 0;
        g_ext_gap_context.pollMemberCount = 0;
    }
	return ret;
}

kal_bool btmtk_gap_power_on()
{
    kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_power_on");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_POWERON_REQ;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, 0))
	{
		g_ext_gap_context.bt_status = BTMTK_POWER_STATE_TURNING_ON;
		ret = KAL_TRUE;
	}

    ALOGI("[GAP] btmtk_gap_power_on return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_status_ind(int status)
{
    kal_bool ret = KAL_FALSE;
	btmtk_update_bt_status_struct *ptr;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_status_ind");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_BT_STATUS_IND;
	ptr = (btmtk_update_bt_status_struct *)ilm.ilm_data;
	ptr->status= status;
	if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_update_bt_status_struct)))
	{
//		g_ext_gap_context.bt_status = status;
		ret = KAL_TRUE;
	}

    ALOGI("[GAP] btmtk_gap_status_ind return:%d!\n", ret);
    return ret;
}


kal_bool btmtk_gap_disable(bt_gap_context_t *pContext)
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_disable");
	btmtk_gap_cancel_rebond();
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_DISABLE;
	if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0))
	{
		g_ext_gap_context.bt_status = BTMTK_POWER_STATE_TURNING_OFF;
		ret = KAL_TRUE;
	}

    ALOGI("[GAP] btmtk_gap_disable return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_power_off()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_power_off");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_POWEROFF_REQ;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, 0))
	{
		ret = KAL_TRUE;
	}

    ALOGI("[GAP] btmtk_gap_power_off return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_acl_disconnect()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_acl_disconnect");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_LINK_DISALLOW_REQ;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, 0))
	{
		g_ext_gap_context.disc_acl_action = KAL_TRUE;
		ret = KAL_TRUE;
	}

    ALOGI("[GAP] btmtk_gap_acl_disconnect return:%d!\n", ret);
    return ret;
}

void btmtk_gap_cleanup(int force)
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;
	pthread_t curThreadId = pthread_self();

    ALOGI("[GAP] btmtk_gap_cleanup");
	unregisterProfileStateChangedCB((ProfileStateChanged)profileStateChanged);
	deinit_profile_manager();
	btmtk_config_hci_logging(0);
	if(force)
	{
	    memset(&ilm, 0x0, sizeof(ilm_struct));
	    ilm.msg_id = EV_BT_CB_TASK_EXIT;
		if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0))
		{
			ret = KAL_TRUE;
		}
	}

	ALOGI("[GAP] Wait btif_task exit!\n");
	if(curThreadId != g_ext_gap_context.bt_evt_task_id)
	{
	    if (pthread_join(g_ext_gap_context.bt_evt_task_id, NULL) < 0 )
	    {
	        ALOGE( "pthread_join() FAILED!\n");
	    }

		btmtk_profile_unregister(PROFILE_GAP);
		btmtk_clear_handlers();
	}
    ALOGI("[GAP] btmtk_gap_cleanup return:%d!\n", ret);
    return;
}

kal_bool btmtk_gap_get_adapter_properties()
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
	ALOGI("[GAP] btmtk_gap_get_adapter_properties]!\n");
	
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_READ_ADAPTER_PROPERTY_ALL;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0))
    {
    	ret = KAL_TRUE;	
    }

	ALOGI("[GAP] btmtk_gap_get_adapter_properties return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_get_adapter_property(bt_property_type_t type)
{
	kal_bool ret = KAL_FALSE;
	btmtk_read_adapter_property_struct *ptr;
	ilm_struct ilm;
	ALOGI("[GAP] btmtk_gap_get_adapter_property type:[%d]+++!\n", type);
	
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_READ_ADAPTER_PROPERTY;
	ptr = (btmtk_read_adapter_property_struct *)ilm.ilm_data;
	ptr->type = type;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_read_adapter_property_struct)))
    {
    	ret = KAL_TRUE;	
    }

	ALOGI("[GAP] btmtk_gap_get_adapter_property return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_set_local_name(const char *name, int name_len)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
	bt_bm_write_local_name_req_struct *req_p;
	int namelen = 0;
	
	ALOGI("[GAP] btmtk_gap_set_local_name name:[%s]!\n", name);
	if(name == NULL || !name[0] || (strlen ((char *)name) > BTBM_ADP_MAX_NAME_LEN))
		return ret;
		
	memset(&ilm, 0x0, sizeof(ilm_struct));
	req_p = (bt_bm_write_local_name_req_struct *)ilm.ilm_data;
	namelen = (name_len > BTBM_ADP_MAX_NAME_LEN) ? BTBM_ADP_MAX_NAME_LEN : name_len;
	ilm.msg_id = MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ;
	strncpy((char *)req_p->name, name, namelen);
	strncpy((char *)g_ext_gap_context.host_cache.name, name, namelen);
	g_ext_gap_context.host_cache.name[namelen] = '\0';
	ALOGI("[GAP] btmtk_gap_set_local_name new name:[%s]!\n", g_ext_gap_context.host_cache.name);
	req_p->name_len = namelen;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_write_local_name_req_struct)))
    {
    	ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_set_local_name return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_set_local_cod(kal_uint32 cod)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
	bt_bm_write_local_cod_req_struct *req_p;
	
	ALOGI("[GAP] btmtk_gap_set_local_cod cod:[%x]!\n", cod);
	memset(&ilm, 0x0, sizeof(ilm_struct));
	req_p = (bt_bm_write_local_cod_req_struct *)ilm.ilm_data;
	ilm.msg_id = MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ;
	req_p->cod = cod;
	req_p->write_type = BTBM_WRITE_COD_NEW;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_write_local_cod_req_struct)))
    {
    	ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_set_local_cod return:%d!\n", ret);
	return ret;
}


kal_bool btmtk_gap_set_scan_mode(btbm_scan_enable_type scan_mode)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
	bt_bm_write_scanenable_mode_req_struct *req_p;
	
 	ALOGI("[GAP] btmtk_gap_set_scan_mode [%x]!\n", scan_mode);
	memset(&ilm, 0x0, sizeof(ilm_struct));
	req_p = (bt_bm_write_scanenable_mode_req_struct *)ilm.ilm_data;
	ilm.msg_id = MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ;
	req_p->mode = scan_mode;
	g_ext_gap_context.host_cache.scan_mode = scan_mode;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_write_scanenable_mode_req_struct)))
    {
    	ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_set_scan_mode return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_set_scan_mode_timeout(kal_uint32 scan_mode_timeout)
{
	kal_bool ret = KAL_FALSE;
	btmtk_set_scan_mode_timeout_struct *ptr;
	ilm_struct ilm;
	
 	ALOGI("[GAP] btmtk_gap_set_scan_mode_timeout [%ld]!\n", scan_mode_timeout);
	memset(&ilm, 0x0, sizeof(ilm_struct));
	ilm.msg_id = EV_BT_CB_WRITE_SCAN_MODE_TIMEOUT;
	ptr = (btmtk_set_scan_mode_timeout_struct *)ilm.ilm_data;
	ptr->scan_mode_timeout = scan_mode_timeout;
	if (btmtk_sendevt(PROFILE_GAP, &ilm, sizeof(btmtk_set_scan_mode_timeout_struct)))
    {
        ret = KAL_TRUE;
	}
	ALOGI("[GAP] btmtk_gap_set_scan_mode_timeout return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_set_adapter_property(const bt_property_t *prop)
{
	kal_bool ret = KAL_FALSE;
	ALOGI("[GAP] btif_set_adapter_property] type:[%d], len:[%d]+++!\n", 
		   prop->type, prop->len);

	switch(prop->type)
	{
	case BT_PROPERTY_BDNAME:
		{
			char bd_name[BTBM_ADP_MAX_NAME_LEN +1];
    		int  name_len = 0;

			name_len = prop->len > BTBM_ADP_MAX_NAME_LEN ? BTBM_ADP_MAX_NAME_LEN : prop->len;
            memcpy(bd_name,prop->val, name_len);
            bd_name[name_len] = '\0';
			
			if (btmtk_gap_set_local_name(bd_name, name_len))
		    {
		    	ret = KAL_TRUE;
		    }
		}
		break;
	case BT_PROPERTY_ADAPTER_SCAN_MODE:
		{
			bt_scan_mode_t mode = *(bt_scan_mode_t*)prop->val;
			btbm_scan_enable_type scan_mode = BTBM_ADP_P_ON_I_OFF;
				
			switch(mode)
            {
            case BT_SCAN_MODE_NONE:
                scan_mode = BTBM_ADP_P_OFF_I_OFF;
                break;
            case BT_SCAN_MODE_CONNECTABLE:
				scan_mode = BTBM_ADP_P_ON_I_OFF;
                break;
            case BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE:
                scan_mode = BTBM_ADP_P_ON_I_ON;
                break;
            default:
                ALOGE("invalid scan mode (0x%x)", mode);
                return BT_STATUS_PARM_INVALID;
            }
			if (btmtk_gap_set_scan_mode(scan_mode))
		    {
		    	ret = KAL_TRUE;
		    }
		}
		break;
	case BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT:
		if (btmtk_gap_set_scan_mode_timeout(*(kal_uint32 *)prop->val))
	    {
	        ret = KAL_TRUE;
		}
		break;
	default:
		ALOGE("[btmtk_gap_set_adapter_property]:other property!\n");
		break;
	}

	ALOGI("[GAP] btmtk_gap_set_adapter_property return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_get_remote_device_properties(bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	btmtk_read_remote_property_struct *ptr;
	ilm_struct ilm;
	ALOGI("[GAP] btmtk_gap_get_remote_device_properties addr:[0x%lX:0x%X:0x%X]+++!\n",
		  remote_addr->lap, remote_addr->uap, remote_addr->nap);
	
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_READ_REMOTE_PROPERTY_ALL;
	ptr = (btmtk_read_remote_property_struct *)ilm.ilm_data;
	memcpy(&(ptr->addr), remote_addr, sizeof(bt_addr_struct));
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_read_remote_property_struct)))
    {
    	ret = KAL_TRUE;
    }

	ALOGI("[GAP] btmtk_gap_set_adapter_property return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_get_remote_device_property(bt_addr_struct *remote_addr,
	                                                  bt_property_type_t type)
{
	kal_bool ret = KAL_FALSE;
	btmtk_read_remote_property_struct *ptr;
	ilm_struct ilm;
	ALOGI("[GAP] btmtk_gap_get_remote_device_property addr:[0x%lX:0x%X:0x%X]+++!\n",
		  remote_addr->lap, remote_addr->uap, remote_addr->nap);
	
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_READ_REMOTE_PROPERTY;
	ptr = (btmtk_read_remote_property_struct *)ilm.ilm_data;
	memcpy(&(ptr->addr), remote_addr, sizeof(bt_addr_struct));
	ptr->type = type;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_read_remote_property_struct)))
    {
    	ret = KAL_TRUE;
    }

	ALOGI("[GAP] btmtk_gap_set_adapter_property return:%d!\n", ret);
	return ret;
}


kal_bool btmtk_gap_set_remote_device_property(bt_addr_struct *remote_addr,
                                                      const bt_property_t *prop)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
	ALOGI("[GAP] btmtk_gap_set_remote_device_property addr:[0x%lX:0x%X:0x%X], type:[%d], len:[%d]+++!\n",
		  remote_addr->lap, remote_addr->uap, remote_addr->nap,
		  prop->type, prop->len);
	
	memset(&ilm, 0x0, sizeof(ilm_struct));
	switch(prop->type)
	{
	case BT_PROPERTY_REMOTE_FRIENDLY_NAME:
		{
			btmtk_paired_device_rename_struct *dev_rename_req = (btmtk_paired_device_rename_struct *)ilm.ilm_data;

			ilm.msg_id = EV_BT_CB_PAIRED_DEVICE_RENAME;
			memcpy(&(dev_rename_req->addr), remote_addr, sizeof(bt_addr_struct));
			dev_rename_req->name_len = (prop->len > BTBM_ADP_MAX_NAME_LEN) ? BTBM_ADP_MAX_NAME_LEN : prop->len;
		    strncpy(dev_rename_req->name, prop->val, dev_rename_req->name_len);
			ALOGI("name is %s,len is %d",dev_rename_req->name,dev_rename_req->name_len);
		    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_paired_device_rename_struct)))
		    {
		        ret = KAL_TRUE;
		    }
		}
		break;
	default:
		ALOGE("[GAP] btmtk_gap_set_remote_device_property:other property!\n");
		break;
	}

	ALOGI("[GAP] btmtk_gap_set_remote_device_property return:%d!\n", ret);
	return ret;
}

kal_bool btmtk_gap_get_remote_service_record(bt_addr_struct *remote_addr,
                                                     bt_uuid_t *uuid)
{
	ALOGI("[GAP] btmtk_gap_get_remote_service_record addr:[0x%lX:0x%X:0x%X]+++!\n",
		  remote_addr->lap, remote_addr->uap, remote_addr->nap);

	ALOGI("[GAP] btmtk_gap_get_remote_service_record!\n");
	return BT_STATUS_SUCCESS;
}

kal_bool btmtk_gap_get_remote_services(bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;
    bt_bm_service_search_req_struct *req_p;
    btmtk_device_entry_struct *entry = NULL;
	btmtk_bond_req_struct *req = NULL;
    int idx = 0, idx_le;

	req = btmtk_util_find_bond_request(remote_addr);
	if(req == NULL)
	{
		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
		memcpy(&req->addr, remote_addr, sizeof(bt_addr_struct));
		btmtk_util_list_append((btmtk_list_header_struct **)&g_ext_gap_context.requests, (btmtk_list_header_struct *)req);
	}
	req->autopair_attempts = 0;
	req->activity = BTMTK_BOND_ACTIVITY_SDP;
	req->auth_type = BTMTK_AUTH_TYPE_NONE;
	
    ALOGI("[GAP] btmtk_gap_get_remote_services addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_SERVICE_SEARCH_REQ;
    req_p = (bt_bm_service_search_req_struct *)ilm.ilm_data;
    memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));

	entry = btmtk_inquired_dev_cache_find(remote_addr);
	if (entry == NULL) 
	{
		entry = btmtk_paired_dev_cache_find(remote_addr);
		if (entry == NULL) 
		{
			ALOGE("The device is not in cache list");
			return ret;
		}
	}
	if (entry != NULL && entry->device_type != BTBM_DEVICE_TYPE_LE)
    {
        for (idx = 0; idx < BTBM_ADP_MAX_SDAP_UUID_NO && g_sdp_uuid[idx] != 0; idx++)
        {
            req_p->sdap_uuid[idx] = (kal_uint32)g_sdp_uuid[idx];
        }
        for (idx_le = 0; idx < BTBM_ADP_MAX_SDAP_UUID_NO && g_sdp_uuid_le[idx_le] != 0; idx_le++, idx++)
        {
            req_p->sdap_uuid[idx] = (kal_uint32)g_sdp_uuid_le[idx_le];
        }
    }
    req_p->sdap_len = idx;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_service_search_req_struct)))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_get_remote_services return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_ev_discovery_started()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_ev_discovery_starting");
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_DISCOVERY_STARTED;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_ev_discovery_starting return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_ev_discovery_stopped()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_gap_ev_discovery_stopped");
	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_DISCOVERY_STOPPED;
    if (btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_ev_discovery_stopped return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_start_discovery(btbm_discovery_type mode)
{
    kal_bool ret = KAL_FALSE;
    ilm_struct ilm;
    bt_bm_discovery_req_struct *req_p;
	unsigned int inquiry_length = 0;
	BT_CUST_ID id = 0;

    ALOGI("[GAP] btmtk_gap_start_discovery");

	if(g_ext_gap_context.activity & BTMTK_GAP_ACT_INQUIRY)
	{
		ALOGI("[GAP] btmtk_gap_start_discovery discovering...!\n");
		return KAL_TRUE;
	}
	
	id = getCustID("InquiryTimeout");
    getCustValue(id, &inquiry_length);
	if(inquiry_length == 0)
	{
		inquiry_length = 0x30;
	}
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_DISCOVERY_REQ;
    req_p = (bt_bm_discovery_req_struct*)ilm.ilm_data;
    req_p->cod = 0xFFFFFFFF;
    req_p->inquiry_length = inquiry_length;
    req_p->inquiry_number = (BTBM_ADP_MAX_INQUIRY_NO-1);//0xFF;
    req_p->discovery_mode = BTBM_DISCOVERY_WITH_NAME;
    req_p->access_mode = BTBM_GENERAL_INQUIRY;
    req_p->discovery_type = mode;
	btmtk_gap_ev_discovery_started();
    if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_discovery_req_struct)))
    {
        ret = KAL_TRUE;
    }
	else
	{
		btmtk_gap_ev_discovery_stopped();
	}
	ALOGI("[GAP] btmtk_gap_start_discovery return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_cancel_discovery()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;
	
    ALOGI("[GAP] btmtk_gap_cancel_discovery!\n");
	memset(&ilm, 0x0, sizeof(ilm_struct));
	if((g_ext_gap_context.activity & BTMTK_GAP_ACT_INQUIRY) &&
		(g_ext_gap_context.discovery_cancel_pending == KAL_FALSE))
	{
		//g_ext_gap_context.activity = (btmtk_gap_activity_enum)(g_ext_gap_context.activity & ~BTMTK_GAP_ACT_INQUIRY);
		ilm.msg_id = MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ;
		g_ext_gap_context.discovery_cancel_pending = KAL_TRUE;
		if(btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, 0))
	    {
	        ret = KAL_TRUE;
	    }
		else
		{
			g_ext_gap_context.discovery_cancel_pending = KAL_FALSE;
		}
	}
		
	ALOGI("[GAP] btmtk_gap_cancel_discovery return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_create_bond(const bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    btmtk_bonding_req_struct *req_p;

    ALOGI("[GAP] btmtk_gap_create_bond: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
	ilm.msg_id = EV_BT_CB_CREATE_BOND;
	req_p = (btmtk_bonding_req_struct*)ilm.ilm_data;
	memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
	if(g_ext_gap_context.bond_state != BTMTK_BOND_STATE_BONDING)
	{
		g_ext_gap_context.bond_state = BTMTK_BOND_STATE_BONDING;
		req_p->result = KAL_TRUE;
	}
	else
	{
		req_p->result = KAL_FALSE;
	}
	
	if(btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_bonding_req_struct)))
    {
        ret = KAL_TRUE;
    }
	else
	{
		g_ext_gap_context.bond_state = BTMTK_BOND_STATE_UNBOND;
	}
	ALOGI("[GAP] btmtk_gap_create_bond return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_bond_create(const bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    bt_bm_bonding_req_struct *req_p;
	int idx = 0, idx_le;

    ALOGI("[GAP] btmtk_gap_bond_create: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_BONDING_REQ;
    req_p = (bt_bm_bonding_req_struct*)ilm.ilm_data;
    memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
	for (idx = 0; idx < BTBM_ADP_MAX_SDAP_UUID_NO && g_sdp_uuid[idx] != 0; idx++)
    {
        req_p->sdap_uuid[idx] = (kal_uint32)g_sdp_uuid[idx];
    }
    for (idx_le = 0; idx < BTBM_ADP_MAX_SDAP_UUID_NO && g_sdp_uuid_le[idx_le] != 0; idx_le++, idx++)
    {
        req_p->sdap_uuid[idx] = (kal_uint32)g_sdp_uuid_le[idx_le];
    }
    req_p->sdap_len = idx;
	if(btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_bonding_req_struct)))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_bond_create return:%d!\n", ret);
    return ret;
}

static void btmtk_rebond_timeout_handler(void *data)
{
	bt_gap_context_t *pContext = (bt_gap_context_t *)data;

	ALOGI("+++[GAP] btmtk_rebond_timeout_handler+++!\n");
	timer_Stop(&pContext->rebond_timer);
	pContext->rebond_timer.load = 0;
	btmtk_gap_bond_create(&g_ext_gap_context.rebond_dev_addr);
	ALOGI("---[GAP] btmtk_rebond_timeout_handler---!\n");
}

void btmtk_gap_rebond(bt_addr_struct *remote_addr)
{
	memcpy(&g_ext_gap_context.rebond_dev_addr, remote_addr, sizeof(bt_addr_struct));
	g_ext_gap_context.rebond_timer.load = (15*SECTICKS)/10;
	g_ext_gap_context.rebond_timer.func = btmtk_rebond_timeout_handler;
	g_ext_gap_context.rebond_timer.name = "rebond timer";
	g_ext_gap_context.rebond_timer.arg = &g_ext_gap_context;
	timer_Start(&g_ext_gap_context.rebond_timer);
}

void btmtk_gap_cancel_rebond()
{
	timer_Stop(&g_ext_gap_context.rebond_timer);
	g_ext_gap_context.rebond_timer.load = 0;
}

kal_bool btmtk_gap_cancel_bond(const bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    bt_bm_bonding_cancel_req_struct *req_p;

    ALOGI("[GAP] btmtk_gap_cancel_bond: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_CANCEL_BOND;
    req_p = (bt_bm_bonding_cancel_req_struct*)ilm.ilm_data;
    memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
	if(btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_bonding_cancel_req_struct)))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_cancel_bond return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_bond_cancel(const bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    bt_bm_bonding_cancel_req_struct *req_p;

    ALOGI("[GAP] btmtk_gap_bond_cancel: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_BONDING_CANCEL_REQ;
    req_p = (bt_bm_bonding_cancel_req_struct*)ilm.ilm_data;
    memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
	if(btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_bonding_cancel_req_struct)))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_bond_cancel return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_remove_bond(bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    bt_bm_delete_trust_req_struct *req_p;

    ALOGI("[GAP] btmtk_gap_remove_bond: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
	if(isConnected(remote_addr))
	{
		btmtk_gap_disconnect(remote_addr);
		btmtk_util_unbond_pending_list_add(remote_addr);
		ret = KAL_TRUE;
	}
	else
	{
	    memset(&ilm, 0x0, sizeof(ilm_struct));
	    ilm.msg_id = EV_BT_CB_REMOVE_BOND;
	    req_p = (bt_bm_delete_trust_req_struct*)ilm.ilm_data;
	    memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
	    if(btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_delete_trust_req_struct)))
	    {
	        ret = KAL_TRUE;
	    }
	}
	ALOGI("[GAP] btmtk_gap_remove_bond return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_delete_trust(const bt_addr_struct *remote_addr)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    bt_bm_delete_trust_req_struct *req_p;

    ALOGI("[GAP] btmtk_gap_delete_trust: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_DELETE_TRUST_REQ;
    req_p = (bt_bm_delete_trust_req_struct*)ilm.ilm_data;
    memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
    if(btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_delete_trust_req_struct)))
    {
        ret = KAL_TRUE;
    }
	ALOGI("[GAP] btmtk_gap_delete_trust return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_pin_reply(const bt_addr_struct *remote_addr,
	                     	     kal_uint8  accept,
                         	     kal_uint8  pin_len,
                                 kal_uint8 *pin_code)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    bt_bm_pin_code_rsp_struct *req_p;
	btmtk_bond_req_struct *req = btmtk_util_find_bond_request((bt_addr_struct *)remote_addr);

    ALOGI("[GAP] btmtk_gap_pin_reply: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_PIN_CODE_RSP;
    req_p = (bt_bm_pin_code_rsp_struct*)ilm.ilm_data;
	memcpy(&req_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
	if(accept)
	{
    	req_p->pin_len = pin_len;
		memcpy(req_p->pin_code, pin_code, pin_len);
	}
	else
	{
		req_p->pin_len = 0;
	}
    if(btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_pin_code_rsp_struct)))
    {
        ret = KAL_TRUE;
    }
	if(ret == KAL_TRUE)
	{
		if(req != NULL && req->auth_type != BTMTK_AUTH_TYPE_NONE)
		{
			req->auth_type = BTMTK_AUTH_TYPE_NONE;
		}
	}
	ALOGI("[GAP] btmtk_gap_pin_reply return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_ssp_reply(const bt_addr_struct *remote_addr,
	                             kal_uint8  auth_type,
	                     	     kal_uint8  accept,
                                 kal_uint32 passkey)
{
	kal_bool ret = KAL_FALSE;
	ilm_struct ilm;
    btmtk_ssp_reply_struct *rsp_p;
	btmtk_bond_req_struct *req = btmtk_util_find_bond_request((bt_addr_struct *)remote_addr);

    ALOGI("[GAP] btmtk_gap_ssp_reply: addr=[0x%lX:0x%X:0x%X]", remote_addr->lap, remote_addr->uap, remote_addr->nap);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    rsp_p = (btmtk_ssp_reply_struct*)ilm.ilm_data;
	switch(auth_type)
	{
	case BT_SSP_VARIANT_PASSKEY_CONFIRMATION:
		ilm.msg_id = MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP;
		rsp_p->user_confirm_rsp.accept = accept;
	    memcpy(&rsp_p->user_confirm_rsp.bd_addr, remote_addr, sizeof(bt_addr_struct));
		break;
    case BT_SSP_VARIANT_PASSKEY_ENTRY:
		ilm.msg_id = MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_RSP;
		rsp_p->passkey_entry_rsp.accept = accept;
		rsp_p->passkey_entry_rsp.passkey = passkey;
		memcpy(&rsp_p->passkey_entry_rsp.bd_addr, remote_addr, sizeof(bt_addr_struct));
		break;
    case BT_SSP_VARIANT_CONSENT:
		ilm.msg_id = MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP;
		rsp_p->user_confirm_rsp.accept = KAL_TRUE;
	    memcpy(&rsp_p->user_confirm_rsp.bd_addr, remote_addr, sizeof(bt_addr_struct));
		break;
    case BT_SSP_VARIANT_PASSKEY_NOTIFICATION:
		ilm.msg_id = MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_RSP;
		rsp_p->passkey_notify_rsp.accept = accept;
		memcpy(&rsp_p->passkey_notify_rsp.bd_addr, remote_addr, sizeof(bt_addr_struct));
		break;
	}
	if(btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_ssp_reply_struct)))
    {
        ret = KAL_TRUE;
    }

	if(ret == KAL_TRUE)
	{
		if(req != NULL && req->auth_type != BTMTK_AUTH_TYPE_NONE)
		{
			req->auth_type = BTMTK_AUTH_TYPE_NONE;
		}
	}
	ALOGI("[GAP] btmtk_gap_ssp_reply return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_gap_oob_data(const bt_addr_struct *remote_addr,
	                             kal_bool accept, 
	                             char *hash, 
	                             char *rand)
{
	ilm_struct ilm;
    bt_bm_security_oob_data_rsp_struct *rsp_p;
    
    ALOGI("[GAP] btmtk_gap_oob_data: addr=[0x%lX:0x%X:0x%X], accept=%d", remote_addr->lap, remote_addr->uap, remote_addr->nap, accept);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_SECURITY_OOB_DATA_RSP;
    rsp_p = (bt_bm_security_oob_data_rsp_struct*)ilm.ilm_data;
    rsp_p->accept = accept;
    if (hash)
    {
        memcpy(rsp_p->c, hash, 16);
    }
    if (rand)
    {
        memcpy(rsp_p->r, rand, 16);
    }
    memcpy(&rsp_p->bd_addr, remote_addr, sizeof(bt_addr_struct));
    return btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_bm_security_oob_data_rsp_struct));
}

kal_bool btmtk_get_local_addr()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

    ALOGI("[GAP] btmtk_get_local_addr");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ;
	if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, 0))
	{
		ret = KAL_TRUE;
	}

    ALOGI("[GAP] btmtk_get_local_addr return:%d!\n", ret);
    return ret;
}

void btmtk_enable_service(uint8_t service_id)
{
	btmtk_execute_services_struct *req_p = NULL;
	ilm_struct ilm;

    ALOGI("[GAP] btmtk_enable_service id:[%d]!\n", service_id);	
	memset(&ilm, 0x0, sizeof(ilm_struct));
	ilm.msg_id = EV_BT_CB_ENABLE_SERVICE;
	req_p = (btmtk_execute_services_struct *)ilm.ilm_data;
	req_p->service_id = service_id;
	req_p->bEnable = TRUE;
	btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_execute_services_struct));
}

void btmtk_disable_service(uint8_t service_id)
{
	btmtk_execute_services_struct *req_p = NULL;
	ilm_struct ilm;

    ALOGI("[GAP] btmtk_enable_service id:[%d]!\n", service_id);	
	memset(&ilm, 0x0, sizeof(ilm_struct));
	ilm.msg_id = EV_BT_CB_DISABLE_SERVICE;
	req_p = (btmtk_execute_services_struct *)ilm.ilm_data;
	req_p->service_id = service_id;
	req_p->bEnable = FALSE;
	btmtk_sendevt(PROFILE_GAP, (void*)&ilm, sizeof(btmtk_execute_services_struct));
}

/* user to send event to advance profile service*/
kal_bool btmtk_sendmsg2adv(void* ptr, int size)
{
    int ret;
    ilm_struct *ilm;
    struct sockaddr_un name;
    socklen_t namelen;

    socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_ADVANCED, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen);
    
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    ALOGI("[JNI] btmtk_sendmsg2adv(ptr=0x%p, len=%d)", ptr, size);

    ret = -1;
    ilm = (ilm_struct*)ptr;
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ALOGI("[JNI] btmtk_sendmsg2adv = %ld", ilm->msg_id);
    if(g_ext_gap_context.appsock >= 0)
    {
        ret = sendto(g_ext_gap_context.appsock, ptr, size, 0, (const struct sockaddr*)&name, namelen);
        if(ret < 0)
        {
            ALOGE("[JNI] send ind fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            ALOGI("[JNI] send ind success : %d", ret);
        }
    }
    else
    {
        ALOGE("[JNI] server socket uninitialized");
    }
    return (ret >= 0)?KAL_TRUE:KAL_FALSE;
}

kal_bool btmtk_enable_advance_service()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

	ALOGI("[GAP] btmtk_enable_advance_service!\n");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_ENABLE_ADVANCE_SERVICE;
    if (btmtk_sendmsg2adv((void*)&ilm, 0))
    {
        ret = KAL_TRUE;
    }
	
	ALOGI("[GAP] btmtk_enable_advance_service return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_disable_advance_service()
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;

	ALOGI("[GAP] btmtk_disable_advance_service!\n");
    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_DISABLE_ADVANCE_SERVICE;
    if (btmtk_sendmsg2adv((void*)&ilm, 0))
    {
        ret = KAL_TRUE;
    }
	
	ALOGI("[GAP] btmtk_disable_advance_service return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_config_hci_logging(uint8_t enable)
{
	kal_bool ret = KAL_FALSE;
    ilm_struct ilm;
	bt_hcisnoop_config_struct *req_p = NULL;
	
    ALOGI("[GAP] btmtk_config_hci_logging old%d, new%d", g_ext_gap_context.hcisnoop_enabled, enable);
	if(g_ext_gap_context.hcisnoop_enabled != enable)
	{
	    g_ext_gap_context.hcisnoop_enabled = enable;
	    memset(&ilm, 0x0, sizeof(ilm_struct));
	    ilm.msg_id = MSG_ID_BT_HCISNOOP_CONFIG_REQ;
		req_p = (bt_hcisnoop_config_struct*)ilm.ilm_data;
		req_p->hcisnoop_enabled = enable;
		if (btmtk_sendmsg(PROFILE_GAP, (void*)&ilm, sizeof(bt_hcisnoop_config_struct)))
		{
			ret = KAL_TRUE;
		}
	}

    ALOGI("[GAP] btmtk_config_hci_logging return:%d!\n", ret);
    return ret;
}

void btmtk_gap_exit()
{
	ilm_struct ilm;

	ALOGI("[GAP] btmtk_gap_exit");
	memset(&ilm, 0x0, sizeof(ilm_struct));
	ilm.msg_id = EV_BT_CB_TASK_EXIT;
	btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0);
}

#define BTMTK_AUTO_PAIR_CONF_FILE  "/etc/bluetooth/auto_pair_blacklist.conf"
#define BTMTK_KEY_AUTOPAIR_BLACKLIST_ADDR "AddressBlacklist"
#define BTMTK_KEY_AUTOPAIR_BLACKLIST_EXACTNAME "ExactNameBlacklist"
#define BTMTK_KEY_AUTOPAIR_BLACKLIST_PARTIALNAME "PartialNameBlacklist"
#define BTMTK_KEY_AUTOPAIR_FIXPIN_KBLIST "FixedPinZerosKeyboardBlacklist"
#define BTMTK_KEY_AUTOPAIR_DYNAMIC_BLACKLIST_ADDR "DynamicAddressBlacklist"
#define BTMTK_DYNAMIC_BLACKLIST_CONF_FILE  "data/@btmtk/dynamic_blacklist"

#define BTMTK_BDADDR_STRING_SZ           (18)      /* 00:11:22:33:44:55 */
#define BTMTK_UUID_STRING_SIZE           (36+1)    /* 00001200-0000-1000-8000-00805f9b34fb; */
#define BTMTK_PINLEN_STRING_MAX_SIZE     (2)       /* ascii pinlen max chars */
#define BTMTK_KEYTYPE_STRING_MAX_SIZE    (1)       /* ascii keytype max chars */
#define BTMTK_DYNAMIC_BDADDR_MAX_NUM     (1000)    /* max dynamic bdaddr num*/


/* <18 char bd addr> <space> LIST< <36 char uuid> <;> > <keytype (dec)> <pinlen> */
#define BTMTK_REMOTE_SERVICES_ENTRY_SIZE_MAX (BTMTK_BDADDR_STRING_SZ + 1 +\
                                              BTMTK_UUID_STRING_SIZE*BT_MAX_NUM_UUIDS + \
                                              BTMTK_PINLEN_STRING_MAX_SIZE +\
                                              BTMTK_KEYTYPE_STRING_MAX_SIZE)

/* currently remote services is the potentially largest entry */
#define BTMTK_MAX_LINE_SZ (100*BTMTK_BDADDR_STRING_SZ)

#define BTMTK_AUTO_PAIR_CONF_VALUE_SEPARATOR ","
#define BTMTK_AUTO_PAIR_CONF_SPACE ' '
#define BTMTK_AUTO_PAIR_CONF_COMMENT '#'
#define BTMTK_AUTO_PAIR_CONF_KEY_VAL_DELIMETER "="

static char *g_autopair_blacklist_addr = NULL;
static char *g_autopair_blacklist_exactname = NULL;
static char *g_autopair_blacklist_partialname = NULL;
static char *g_autopair_fixpin_kblist = NULL;
static char *g_autopair_dynamic_blacklist_addr = NULL;


/*******************************************************************************
**
** Function         btmtk_util_load_autopair_device_list
**
** Description      load auto pair device list file
**
** Returns          KAL_TRUE/KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_util_load_autopair_device_list()
{
#if 1
    char *key_name, *key_value;
    int i=0;
	int len = 0;
    char linebuf[BTMTK_MAX_LINE_SZ];
    char *line;
    FILE *fp;

    /* loading of auto pair blacklist configuration  */
    fp = fopen (BTMTK_AUTO_PAIR_CONF_FILE, "r");
    if (fp == NULL)
    {
        ALOGE("%s: Failed to open auto pair blacklist conf file at %s", __FUNCTION__, BTMTK_AUTO_PAIR_CONF_FILE );
        return KAL_FALSE;
    }
	
    /* read through auto_pairing.conf file and create the key value pairs specific to  auto pair blacklist devices */
    while (fgets(linebuf, BTMTK_MAX_LINE_SZ, fp) != NULL)
    {
        /* trip  leading white spaces */
        while (linebuf[i] == BTMTK_AUTO_PAIR_CONF_SPACE)
            i++;

        /* skip  commented lines */
        if (linebuf[i] == BTMTK_AUTO_PAIR_CONF_COMMENT)
            continue;

        line = (char*)&(linebuf[i]);

        if (line == NULL)
            continue;

        key_name = strtok(line, BTMTK_AUTO_PAIR_CONF_KEY_VAL_DELIMETER);

        if (key_name == NULL)
            continue;
		else
		{
			key_value = strtok(NULL, BTMTK_AUTO_PAIR_CONF_KEY_VAL_DELIMETER);
			if(strcmp(key_name, BTMTK_KEY_AUTOPAIR_BLACKLIST_ADDR) == 0)
			{
				len = strlen(key_value);
				ALOGI("malloc g_autopair_blacklist_addr size:[%d], [%s]!\n", len, key_value);
				g_autopair_blacklist_addr = (char *)malloc(len+1);
				if(g_autopair_blacklist_addr != NULL)
				{
					strcpy(g_autopair_blacklist_addr, key_value);
					g_autopair_blacklist_addr[len] = '\0';
				}
				else
					ALOGE("malloc g_autopair_blacklist_addr failed!\n");
			}
			else if(strcmp(key_name, BTMTK_KEY_AUTOPAIR_BLACKLIST_EXACTNAME) == 0)
			{
				len = strlen(key_value);
				ALOGI("malloc g_autopair_blacklist_exactname size:[%d], [%s]!\n", len, key_value);
				g_autopair_blacklist_exactname = (char *)malloc(len+1);
				if(g_autopair_blacklist_exactname != NULL)
				{
					strcpy(g_autopair_blacklist_exactname, key_value);
					g_autopair_blacklist_exactname[len] = '\0';
				}
				else
					ALOGE("malloc g_autopair_blacklist_exactname failed!\n");
			}
			else if(strcmp(key_name, BTMTK_KEY_AUTOPAIR_FIXPIN_KBLIST) == 0)
			{
				len = strlen(key_value);
				ALOGI("malloc g_autopair_fixpin_kblist size:[%d], [%s]!\n", len, key_value);
				g_autopair_fixpin_kblist = (char *)malloc(len+1);
				if(g_autopair_fixpin_kblist != NULL)
				{
					strcpy(g_autopair_fixpin_kblist, key_value);
					g_autopair_fixpin_kblist[len] = '\0';
				}
				else
					ALOGE("malloc g_autopair_fixpin_kblist failed!\n");
			}
			else if(strcmp(key_name, BTMTK_KEY_AUTOPAIR_BLACKLIST_PARTIALNAME) == 0)
			{
				len = strlen(key_value);
				ALOGI("malloc g_autopair_blacklist_partialname size:[%d], [%s]!\n", len, key_value);
				g_autopair_blacklist_partialname = (char *)malloc(len+1);
				if(g_autopair_blacklist_partialname != NULL)
				{
					strcpy(g_autopair_blacklist_partialname, key_value);
					g_autopair_blacklist_partialname[len] = '\0';
				}
				else
					ALOGE("malloc g_autopair_blacklist_partialname failed!\n");
			}
		}
    }
    fclose(fp);
#endif
    return KAL_TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_load_dynamic_blacklist
 * DESCRIPTION
 *  Read all blacklist data from file
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_util_load_dynamic_blacklist()
{
    int fd;
    struct stat sts;
    int size = BTMTK_DYNAMIC_BDADDR_MAX_NUM * BTMTK_BDADDR_STRING_SZ;
    
    ALOGI("[GAP] %s", __FUNCTION__);
	if(g_autopair_dynamic_blacklist_addr == NULL)
	{
		g_autopair_dynamic_blacklist_addr = (char *)malloc(size);
	}
    memset(g_autopair_dynamic_blacklist_addr, 0x0, size);
    if (stat(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, &sts) == -1 || 
        (fd = open(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, O_RDONLY)) < 0)
    {
    	ALOGE("[GAP] %s is not exist!\n", BTMTK_DYNAMIC_BLACKLIST_CONF_FILE);
    	unlink(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE);
    	if ((fd = open(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, O_WRONLY | O_CREAT, 0666)) < 0)
	    {
	        ALOGE("[GAP] open error %d: %s", errno, strerror(errno));
	        return;
	    }

		chown(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, -1, AID_NET_BT_STACK);
	    chmod(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, 0660);
		close(fd);
    }
    else
    {
        int read_byte = TEMP_FAILURE_RETRY(read(fd, g_autopair_dynamic_blacklist_addr, sts.st_size));
        if (read_byte < 0) 
        {
            ALOGE("[GAP]read error %d: %s", errno, strerror(errno));
        } 
        else if (read_byte != size) 
        {
            ALOGE("[GAP]read error(%d bytes read) %d: %s", read_byte, errno, strerror(errno));
        }
		else
		{
			ALOGD("%s", g_autopair_dynamic_blacklist_addr);
		}
        close(fd);
    }
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_unload_dynamic_blacklist
 * DESCRIPTION
 *  release all blacklist buffer
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_util_unload_dynamic_blacklist()
{
    if(g_autopair_dynamic_blacklist_addr != NULL)
	{
		free(g_autopair_dynamic_blacklist_addr);
		g_autopair_dynamic_blacklist_addr = NULL;
	}
}

/*******************************************************************************
**
** Function         btmtk_util_load_autopair_device_list
**
** Description      load auto pair device list file
**
** Returns          KAL_TRUE/KAL_FALSE
**
*******************************************************************************/
void btmtk_util_add_device_to_dynamic_blacklist(bt_addr_struct *addr)
{
#if 1
	int fd, len;
	int ret = 0;
	char linebuf[20] = {0};
	int size = 0;

    ALOGI("[GAP] %s", __FUNCTION__);
    if ((fd = open(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, O_WRONLY | O_APPEND, 0666)) < 0)
    {
        ALOGE("[GAP] open error %d: %s", errno, strerror(errno));
        return;
    }

	chown(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, -1, AID_NET_BT_STACK);
    chmod(BTMTK_DYNAMIC_BLACKLIST_CONF_FILE, 0660);

	btmtk_util_convert_bdaddr2string(linebuf, addr);
	strncat(linebuf, BTMTK_AUTO_PAIR_CONF_VALUE_SEPARATOR, 1);
	len = strlen(g_autopair_dynamic_blacklist_addr);
	size = strlen(linebuf);
	if((len+size) <= BTMTK_DYNAMIC_BDADDR_MAX_NUM*BTMTK_BDADDR_STRING_SZ)
	{
		strncat(g_autopair_dynamic_blacklist_addr, linebuf, size);
		len += size;
		g_autopair_dynamic_blacklist_addr[len] = '\0';
	}
	
	len = write(fd, linebuf, size);
	if (len < 0 || len != size) 
    {
        ALOGE("[GAP] %d byte error %d: %s", len, errno, strerror(errno));
	}
    close(fd);
#endif
}


/*******************************************************************************
**
** Function         btmtk_util_unload_autopair_device_list
**
** Description      unload auto pair device list file
**
** Returns         void
**
*******************************************************************************/
void btmtk_util_unload_autopair_device_list()
{
#if 1
    if(g_autopair_blacklist_addr != NULL)
	{
		free(g_autopair_blacklist_addr);
		g_autopair_blacklist_addr = NULL;
	}

	if(g_autopair_blacklist_exactname != NULL)
	{
		free(g_autopair_blacklist_exactname);
		g_autopair_blacklist_exactname = NULL;
	}

	if(g_autopair_fixpin_kblist != NULL)
	{
		free(g_autopair_fixpin_kblist);
		g_autopair_fixpin_kblist = NULL;
	}

	if(g_autopair_blacklist_partialname != NULL)
	{
		ALOGI("FREE g_autopair_blacklist_partialname");
		free(g_autopair_blacklist_partialname);
		g_autopair_blacklist_partialname = NULL;
		ALOGI("FREE g_autopair_blacklist_partialname OUT");
	}
#endif
}

/*******************************************************************************
**
** Function         btmtk_is_device_autopair_blacklisted
**
** Description      Checks if the given device is blacklisted for auto pairing
**
** Returns          KAL_TRUE/KAL_FALSE
**
*******************************************************************************/
kal_bool  btmtk_is_device_autopair_blacklisted(bt_gap_context_t * pContext, bt_addr_struct *addr)
{
    char *token;
    char bdstr[18];
    char *dev_name_str = NULL;
	btmtk_device_entry_struct *entry = NULL;

    btmtk_util_convert_bdaddr2string(bdstr, addr);
    /* Consider only  Lower Address Part from BD Address */
    bdstr[8] = '\0';

	if(g_autopair_blacklist_addr != NULL)
	{
		if (strcasestr(g_autopair_blacklist_addr, bdstr) != NULL)
            return KAL_TRUE;
	}

	entry = btmtk_inquired_dev_cache_find(addr);
	if(entry != NULL && strlen(entry->name) > 0)
	{
		dev_name_str = entry->name;
	}

	if (dev_name_str != NULL)
    {
		if(g_autopair_blacklist_exactname != NULL)
		{
			if (strcasestr(g_autopair_blacklist_exactname, dev_name_str) != NULL)
				return KAL_TRUE;
		}
        
		if(g_autopair_blacklist_partialname != NULL)
		{
			token = strtok(g_autopair_blacklist_partialname, BTMTK_AUTO_PAIR_CONF_VALUE_SEPARATOR);
            while (token != NULL)
            {
                if (strstr(dev_name_str, token) != NULL)
                    return KAL_TRUE;

                token = strtok(NULL, BTMTK_AUTO_PAIR_CONF_VALUE_SEPARATOR);
            }
		}
    }

	if(g_autopair_dynamic_blacklist_addr != NULL)
	{
		if (strcasestr(g_autopair_dynamic_blacklist_addr, bdstr) != NULL)
			return KAL_TRUE;
	}
    return KAL_FALSE;
}

/*******************************************************************************
**
** Function         btmtk_is_fixed_pin_zeros_keyboard
**
** Description      checks if this device has fixed PIN key device list
**
** Returns          KAL_TRUE/KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_is_fixed_pin_zeros_keyboard(bt_gap_context_t * pContext, bt_addr_struct *addr)
{
    char bdstr[18];

    btmtk_util_convert_bdaddr2string(bdstr, addr);
    /* Consider only  Lower Address Part from BD Address */
    bdstr[8] = '\0';

	if(g_autopair_fixpin_kblist != NULL)
	{
		if (strcasestr(g_autopair_fixpin_kblist, bdstr) != NULL)
			return KAL_TRUE;
	}
    
    return KAL_FALSE;

}


