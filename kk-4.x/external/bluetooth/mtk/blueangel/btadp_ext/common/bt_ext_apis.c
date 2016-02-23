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
 * Bt_ext_apis.c
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
#include "cutils/sockets.h"
#include <pthread.h>
#include <sys/syscall.h>

#include "bt_utils.h"
#include "bt_ext_common.h"
#include "bt_gap_struct.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "bt_ext_apis.c"

extern bt_gap_context_t g_ext_gap_context;

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

extern btmtk_device_entry_struct *btmtk_inquired_dev_cache_find(bt_addr_struct *addr);
extern btmtk_device_entry_struct *btmtk_paired_dev_cache_find(bt_addr_struct *addr);
kal_bool btmtk_sendevt(int profile_id, void* ptr, int size);


btmtk_profile_disconnect_callback *g_disc_cb;


const char *g_ext_sock_name[] =
{
    BT_SOCK_NAME_EXT_ADP_A2DP,
    BT_SOCK_NAME_EXT_ADP_A2DP_DATA,
    BT_SOCK_NAME_EXT_ADP_HFP,
    BT_SOCK_NAME_EXT_ADP_SPP,
    BT_SOCK_NAME_EXT_ADP_AVRCP,
    BT_SOCK_NAME_EXT_ADP_OPP,
    BT_SOCK_NAME_EXT_ADP_BIP,
    BT_SOCK_NAME_EXT_ADP_BPP,
    BT_SOCK_NAME_EXT_ADP_FTP,
    BT_SOCK_NAME_EXT_ADP_PBAP,
    BT_SOCK_NAME_EXT_ADP_HID,
    BT_SOCK_NAME_EXT_ADP_PAN,
    BT_SOCK_NAME_EXT_ADP_SIMAP,
    BT_SOCK_NAME_EXT_ADP_JSR82,
    BT_SOCK_NAME_EXT_ADP_PRX,
    BT_SOCK_NAME_EXT_ADP_MAP,
    BT_SOCK_NAME_EXT_ADP_TIME,
    BT_SOCK_NAME_EXT_ADP_HDP,
    BT_SOCK_NAME_EXT_ADP_HTP,
    BT_SOCK_NAME_EXT_ADP_FMP,
    BT_SOCK_NAME_EXT_ADP_GATTC,
    BT_SOCK_NAME_EXT_ADP_GATTS,
    BT_SOCK_NAME_EXT_ADP,
    BT_SOCK_NAME_EXT_ADP_ADVANCED,
};

static int g_ext_socket[NUM_OF_PROFILE] = {0};

/*******************************************************************************/
#include <sys/select.h>
#include <netinet/in.h>

//#include "socket_local.h"

#define LISTEN_BACKLOG 4

/**
 * Binds a pre-created socket(AF_LOCAL) 's' to 'name'
 * returns 's' on success, -1 on fail
 *
 * Does not call listen()
 */
int btmtk_socket_bind(int s, const char *name, int namespaceId, int type)
{
    struct sockaddr_un addr;
    socklen_t alen;
    int n;
    int err;

    err = socket_make_sockaddr_un(name, namespaceId, &addr, &alen);

    if (err < 0) {
        return -1;
    }

    n = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));

	n = 0;
    setsockopt(s, SOL_SOCKET, SO_LINGER, &n, sizeof(n));
    if(bind(s, (struct sockaddr *) &addr, alen) < 0) {
        return -1;
    }

    return s;

}


/** Open a server-side UNIX domain datagram socket in the Linux non-filesystem 
 *  namespace
 *
 *  Returns fd on success, -1 on fail
 */

int btmtk_socket_create(const char *name, int namespace, int type)
{
    int err;
    int s;
    
    s = socket(AF_LOCAL, type, 0);
    if (s < 0) return -1;

    err = btmtk_socket_bind(s, name, namespace, type);

    if (err < 0) {
        close(s);
        return -1;
    }

    if (type == SOCK_STREAM) {
        int ret;

        ret = listen(s, LISTEN_BACKLOG);

        if (ret < 0) {
            close(s);
            return -1;
        }
    }

    return s;
}

/*******************************************************************************/

/*******************************************************************************
**
** Function         addWatch
**
** Description     add external socket for btif_task listened
**
** Returns          NULL
**
*******************************************************************************/
void addWatch(int newFD, short events) {
		int y = 0;
    for (y = 0; y< g_ext_gap_context.pollMemberCount; y++) {
        if ((g_ext_gap_context.pollData[y].fd == newFD) &&
            (g_ext_gap_context.pollData[y].events == events)) {
            ALOGV("[JNI] duplicate add");
            return;
        }
    }
	
    if (g_ext_gap_context.pollMemberCount == g_ext_gap_context.pollDataSize) {
        ALOGV("Bluetooth EventLoop poll struct growing");
        struct pollfd *temp = (struct pollfd *)malloc(sizeof(struct pollfd) * (g_ext_gap_context.pollMemberCount+1));
        if (!temp) {
            return;
        }
        memcpy(temp, g_ext_gap_context.pollData, sizeof(struct pollfd) * g_ext_gap_context.pollMemberCount);
        free(g_ext_gap_context.pollData);
        g_ext_gap_context.pollData = temp;
        g_ext_gap_context.pollDataSize++;
    }
    g_ext_gap_context.pollData[g_ext_gap_context.pollMemberCount].fd = newFD;
    g_ext_gap_context.pollData[g_ext_gap_context.pollMemberCount].revents = 0;
    g_ext_gap_context.pollData[g_ext_gap_context.pollMemberCount].events = events;
    /* nat->watchData[nat->pollMemberCount] = watch; */
    g_ext_gap_context.pollMemberCount++;
}

/*******************************************************************************
**
** Function         removeWatch
**
** Description     remove external socket for btif_task listened
**
** Returns          NULL
**
*******************************************************************************/
void removeWatch(int fd) {
	int i, j = 0;
    for (i = 0; i < g_ext_gap_context.pollMemberCount; i++) {
        if ((g_ext_gap_context.pollData[i].fd == fd)) {
            for(j = i; j < (g_ext_gap_context.pollMemberCount - 1); j++)
            {
            	g_ext_gap_context.pollData[j].fd = g_ext_gap_context.pollData[j+1].fd;
			    g_ext_gap_context.pollData[j].revents = g_ext_gap_context.pollData[j+1].revents;
			    g_ext_gap_context.pollData[j].events = g_ext_gap_context.pollData[j+1].events;
            }
			g_ext_gap_context.pollMemberCount --;
			break;
        }
    }
}

btmtk_ext_msg_handler_struct *btmtk_util_find_profile_msg_handler(btmtk_ext_msg_handler_struct **list, int profileID)
{
    btmtk_ext_msg_handler_struct *ptr = *list;
    ALOGI("btmtk_util_find_profile_msg_handler search profile_id:[%d]", profileID);
    while (ptr)
    {
     //   ALOGI("[HDP] btmtk_util_find_hdp_fd found 0x%lX:0x%X:0x%X", ptr->addr.lap, ptr->addr.uap, ptr->addr.nap);
        if (ptr->profile_id == profileID)
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}


void btmtk_profile_register(int profileId, void *message_handler, void *pContext)
{
	btmtk_ext_msg_handler_struct *pHandler = (btmtk_ext_msg_handler_struct *) calloc(1, sizeof(btmtk_ext_msg_handler_struct));
	if (NULL != pHandler)
	{
		pHandler->profile_id = profileId;
		pHandler->handler = message_handler;
		pHandler->pContext = pContext;

		//if(g_ext_socket[profileId] <= 0)
		//{
		//	g_ext_socket[profileId] = socket_local_server(g_ext_sock_name[profileId], 
		//									              ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
		//									              SOCK_DGRAM);
		//}
		pHandler->socket = btmtk_socket_create(g_ext_sock_name[profileId], 
											   ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
											   SOCK_DGRAM);
		ALOGI("create %s external socket %d!\n", g_ext_sock_name[profileId], pHandler->socket);
		if (pHandler->socket < 0)
		{
			ALOGE("create %s external socket failed : %s, errno=%d!\n", 
				   g_ext_sock_name[profileId], strerror(errno), errno);
			return;
		}
		btmtk_util_list_append((btmtk_list_header_struct **)&g_ext_gap_context.pHandler, (btmtk_list_header_struct *)pHandler);
		addWatch(pHandler->socket, POLLIN);
	} 
}

void btmtk_profile_unregister(int profileId)
{
	ilm_struct ilm;
	btmtk_ext_msg_handler_struct *pHandler = NULL;
	pthread_t curThreadId = pthread_self();

	pHandler = btmtk_util_find_profile_msg_handler(&g_ext_gap_context.pHandler, profileId);

	if(curThreadId != g_ext_gap_context.bt_evt_task_id)
		pthread_mutex_lock(&(g_ext_gap_context.thread_mutex));
	if(pHandler != NULL)
	{
		removeWatch(pHandler->socket);
		close(pHandler->socket);
		pHandler->profile_id = -1;
		pHandler->handler = NULL;
		pHandler->pContext = NULL;
		pHandler->socket = -1;
		btmtk_util_list_remove((btmtk_list_header_struct **)&g_ext_gap_context.pHandler, (btmtk_list_header_struct *)pHandler);
	}
	if(curThreadId != g_ext_gap_context.bt_evt_task_id)
		pthread_mutex_unlock(&(g_ext_gap_context.thread_mutex));

	memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = EV_BT_CB_UNREGISTER_HANDLER;
	btmtk_sendevt(PROFILE_GAP, (void*)&ilm, 0);
}

void btmtk_clear_handlers()
{
	ALOGI("+++ [GAP] btmtk_clear_handlers +++!\n");
	btmtk_ext_msg_handler_struct *pHandler = g_ext_gap_context.pHandler;
	btmtk_ext_msg_handler_struct *pNext = NULL;

	pthread_mutex_lock(&(g_ext_gap_context.thread_mutex));
	while(NULL != pHandler)
	{
		pNext = pHandler->next;
		removeWatch(pHandler->socket);
		close(pHandler->socket);
		pHandler->profile_id = -1;
		pHandler->handler = NULL;
		pHandler->pContext = NULL;
		pHandler->socket = -1;
		btmtk_util_list_remove((btmtk_list_header_struct **)&g_ext_gap_context.pHandler, (btmtk_list_header_struct *)pHandler);
		pHandler = pNext;
	}
	if (g_ext_gap_context.pollData) 
 	{
		free(g_ext_gap_context.pollData);
    	g_ext_gap_context.pollData = NULL;
	}
    g_ext_gap_context.pollDataSize = 0;
    g_ext_gap_context.pollMemberCount = 0;
	if(g_ext_gap_context.appsock != -1)
	{
		close(g_ext_gap_context.appsock);
		g_ext_gap_context.appsock = -1;
	}
	pthread_mutex_unlock(&(g_ext_gap_context.thread_mutex));
	ALOGI("--- [GAP] btmtk_clear_handlers ---!\n");
}

kal_bool btmtk_sendmsg(int profile_id, void* ptr, int size)
{
    int ret = KAL_FALSE;
    ilm_struct *ilm = (ilm_struct*)ptr;
    U16 *field;
    struct sockaddr_un servname;
    socklen_t servnamelen;    
 	btmtk_ext_msg_handler_struct *pHandler = NULL;
	
	pHandler = btmtk_util_find_profile_msg_handler(&g_ext_gap_context.pHandler, profile_id);
	if(pHandler == NULL)
	{
		ALOGI("[JNI] Doesn't register msg handler!\n");
		return ret;
	}
	
    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;
	
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ALOGI("[JNI] btmtk_sendmsg(cmd=%lu, ptr=%p, len=%d)", ilm->msg_id, ptr, size);
    servname.sun_family = AF_UNIX;
    strcpy (servname.sun_path, BT_SOCK_NAME_INT_ADP);
    servnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (servname.sun_path) + 1);

    if(pHandler->socket >= 0)
    {
        ret = sendto(pHandler->socket, ptr, size, 0, (const struct sockaddr*)&servname, servnamelen);
        if(ret < 0)
        {
            ALOGE("[JNI] send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            ALOGI("[JNI] send msg success : %d", ret);
        }
    }
    else
    {
        ALOGE("[JNI] server socket uninitialized");
    }
    return (ret >= 0) ? KAL_TRUE : KAL_FALSE;
}


/* user to send event to btif_task*/
kal_bool btmtk_sendevt(int profile_id, void* ptr, int size)
{
    int ret;
    ilm_struct *ilm;
    struct sockaddr_un name;
    socklen_t namelen;
    U16 *field;

    socket_make_sockaddr_un(g_ext_sock_name[profile_id], ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen);
    
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    ALOGI("[JNI] btmtk_sendevt(ptr=0x%p, len=%d)", ptr, size);

    ret = -1;
    ilm = (ilm_struct*)ptr;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;
    
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    
    ALOGI("[JNI] send ind=%ld", ilm->msg_id);
    if(g_ext_gap_context.appsock >= 0)
    {
        ret = sendto(g_ext_gap_context.appsock, ptr, size, MSG_DONTWAIT, (const struct sockaddr*)&name, namelen);
        if(ret < 0)
        {
            ALOGE("[JNI] send ind fail : %s, %d", strerror(errno), errno);
			if(errno == EAGAIN || errno == ENOBUFS)
			{
				int sndbuf = 0;
				int optlen = sizeof(sndbuf);
				int ret = getsockopt(g_ext_gap_context.appsock, SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen);

				if(!ret)
				{
					sndbuf += 5120;
					ret = setsockopt(g_ext_gap_context.appsock, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
					if(!ret)
					{
						ret = sendto(g_ext_gap_context.appsock, ptr, size, MSG_DONTWAIT, (const struct sockaddr*)&name, namelen);
				        if(ret < 0)
				        {
				            ALOGE("[JNI] send ind fail : %s, %d", strerror(errno), errno);
						}
						else
				        {
				            ALOGI("[JNI] send ind success : %d", ret);
				        }
					}
				}
			}
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

kal_uint16 btmtk_get_remote_device_name(bt_addr_struct *remAddr, char *name, kal_uint16 len)
{
	kal_uint16 name_len = 0;
	btmtk_device_entry_struct *entry = NULL;

	if(remAddr == NULL || name == NULL)
		return 0;
	
	entry = btmtk_inquired_dev_cache_find(remAddr);
	if(entry != NULL)
	{
		ALOGI("get %s name from inquiry list!\n", entry->name);
		name_len = (len-1) > (kal_uint16)strlen(entry->name)?(kal_uint16)strlen(entry->name):(len-1);
		strncpy(name, (const char *)entry->name, name_len);
		name[name_len] = '\0';
		return name_len;
	}

	entry = btmtk_paired_dev_cache_find(remAddr);
	if(entry != NULL)
	{
		ALOGI("get %s name from pair list!\n", entry->name);
		name_len = (len-1) > (kal_uint16)strlen(entry->name)?(kal_uint16)strlen(entry->name):(len-1);
		strncpy(name, (const char *)entry->name, name_len);
		name[name_len] = '\0';
		return name_len;
	}

	return 0;
}

