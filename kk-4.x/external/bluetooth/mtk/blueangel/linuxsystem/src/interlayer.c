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
 * Interlayer.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Dlight Ting
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
#if defined(BTMTK_ON_WISESDK)
#include <windows.h>
#elif defined(BTMTK_ON_LINUX)
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/msg.h>
#include <errno.h>
#include "utils.h"
#include "cutils/sockets.h"
#include "cmdqueue.h"
#endif
#include <assert.h>
#include "bt_types.h"
#include "bt_mmi.h"
#if defined(BTMTK_ON_WISESDK)
#include "win_service.h"
#endif
#include "bt_message.h"
#include "osapi.h"

#if defined(BTMTK_ON_LINUX)

#define SOCKET_SUPPORTED

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

extern BOOL hdp_adp_check_socket_address(const char *source);
extern BOOL hdp_deQueue_data(U8 *source, void *data, U32 size);

#ifdef SOCKET_SUPPORTED
int g_serverSocket_backup = -1;
int g_serverSocket = -1;
int g_clientSocket = -1;
int g_a2dpstreamSocket = -1;
struct sockaddr_un g_soaddr_app_gap;
socklen_t   g_soaddr_app_gap_len;
struct sockaddr_un g_soaddr_app_gap_twin;
socklen_t   g_soaddr_app_gap_twin_len;
struct sockaddr_un g_soaddr_ext_adp;
socklen_t   g_soaddr_ext_adp_len;
struct sockaddr_un g_soaddr_int_adp;
socklen_t   g_soaddr_int_adp_len;
struct sockaddr_un g_soaddr_a2dp_stream;
socklen_t   g_soaddr_a2dp_stream_len;
/* Keep the address of the last received packet */
static struct sockaddr_un g_cmdaddr;
static socklen_t   g_cmdaddrlen;
#ifdef __BT_HS_30__
static struct sockaddr_un g_soaddr_pal;
static socklen_t   g_soaddr_pal_len;
#endif
#endif

#define BT_MEM_GUARD1 0xABABABAB
#define BT_MEM_GUARD2 0xCDCDCDCD
#define BT_MEM_GUARD3 0xEEEEEEEE
#define BT_MEM_GUARD4 0xFFFFFFFF

static PROFILE_ADDR g_profileAddr[NUM_OF_PROFILE];

kal_bool bt_setProfileAddress(PROFILE_enum profile, struct sockaddr_un *addr, socklen_t addrlen)
{
    bt_prompt_trace(MOD_BT,"bt_setProfileAddress(%d, %s, %d)", profile, &addr->sun_path[1], addrlen);
    bt_prompt_trace(MOD_BT,"address family=%d", addr->sun_family);
    if(addr && addrlen)
   {
        memcpy(&g_profileAddr[profile].addr, addr, sizeof(*addr));
        g_profileAddr[profile].addrlen = addrlen;
        return KAL_TRUE;
    }
    bt_prompt_trace(MOD_BT,"bt_setProfileAddress failed : addr=0x%X, addrlen=%d", addr, &addrlen);
    return KAL_FALSE;
}

void bt_removeProfileAddress(PROFILE_enum profile)
{
    bt_prompt_trace(MOD_BT,"bt_removeProfileAddress(%d)", profile);
    g_profileAddr[profile].addrlen = 0;
}


U16 counter_memory_alloc = 0;
void *bt_win_malloc(U32 size)
{
    U8 *ptr;

    size = ((size+3)&0xFFFFFFFC);
    ptr = (U8 *)malloc(size + 12); /* GUARD1 + GUARD2 + size */
    *(U32 *)ptr = BT_MEM_GUARD1;
    *(U32 *)(ptr + 4) = size;
    *(U32 *)(ptr + size + 8) = BT_MEM_GUARD2;
    return (void *)(ptr + 8);
}

void bt_win_free(void *ptr)
{
    U8 *ptr1 = ptr;
    S32 size = *(S32 *)(ptr1 - 4);
    if (*(U32 *)(ptr1 - 8) != BT_MEM_GUARD1)
    {
        abort();
    }
    if (*(U32 *)(ptr1 + size) != BT_MEM_GUARD2)
    {
        abort();
    }
    *(U32 *)(ptr1 - 8) = BT_MEM_GUARD3;
    *(U32 *)(ptr1 + size) = BT_MEM_GUARD4;
    free((ptr1 - 8));
}

void free_ilm(void *ptr)
{
	counter_memory_alloc--;
	bt_win_free(ptr);
}

void *construct_local_para(U16 size, U8 type)
{
    local_para_struct *ptr;
    
	counter_memory_alloc++;
    if(size > MAX_ILM_BUFFER_SIZE)
        assert(0);

    ptr = (local_para_struct *)bt_win_malloc(size);
    memset(ptr, 0, size);
    ptr->ref_count = 1;
    ptr->msg_len = size;
    return (void *)ptr;
}

void free_local_para(void *ptr)
{
	counter_memory_alloc--;
    bt_win_free(ptr);
    return;
}

ilm_struct g_rec_ilm;

static int g_bt_msgqueue;

char *getMessageStringLog(msg_type msg_id)
{
    switch (msg_id)
    {
    case MSG_ID_BT_POWERON_REQ:
        return "MSG_ID_BT_POWERON_REQ";
    case MSG_ID_BT_POWEROFF_REQ:
        return "MSG_ID_BT_POWEROFF_REQ";
    case MSG_ID_BT_BM_DISCOVERY_REQ:
        return "MSG_ID_BT_BM_DISCOVERY_REQ";
    case MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ:
        return "MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ";
    case MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ:
        return "MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ";
    case MSG_ID_BT_BM_READ_LOCAL_NAME_REQ:
        return "MSG_ID_BT_BM_READ_LOCAL_NAME_REQ";
    case MSG_ID_BT_BM_READ_REMOTE_NAME_REQ:
        return "MSG_ID_BT_BM_READ_REMOTE_NAME_REQ";
    case MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_REQ:
        return "MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_REQ";
    case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ:
        return "MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ";
    case MSG_ID_BT_BM_READ_SCANENABLE_MODE_REQ:
        return "MSG_ID_BT_BM_READ_SCANENABLE_MODE_REQ";
    case MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ:
        return "MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ";
    case MSG_ID_BT_BM_READ_LOCAL_COD_REQ:
        return "MSG_ID_BT_BM_READ_LOCAL_COD_REQ";
    case MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ:
        return "MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ";
    case MSG_ID_BT_BM_READ_PROPERTY_REQ:
        return "MSG_ID_BT_BM_READ_PROPERTY_REQ";
    case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ:
        return "MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ";
    case MSG_ID_BT_BM_PIN_CODE_RSP:
        return "MSG_ID_BT_BM_PIN_CODE_RSP";
    case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP:
        return "MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP";
    case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_RSP:
        return "MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_RSP";
    case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_CANCEL_REQ:   
        return "MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_CANCEL_REQ";
    case MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_REQ:    
        return "MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_REQ";
    case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_REQ:
        return "MSG_ID_BT_BM_BLOCK_LIST_UPDATE_REQ";
    case MSG_ID_BT_BM_BONDING_REQ:
        return "MSG_ID_BT_BM_BONDING_REQ";
    case MSG_ID_BT_BM_DELETE_TRUST_REQ:
        return "MSG_ID_BT_BM_DELETE_TRUST_REQ";
    case MSG_ID_BT_BM_DELETE_TRUST_ALL_REQ:
        return "MSG_ID_BT_BM_DELETE_TRUST_ALL_REQ";
    case MSG_ID_BT_BM_BONDING_CANCEL_REQ:
        return "MSG_ID_BT_BM_BONDING_CANCEL_REQ";
    case MSG_ID_BT_BM_PAIRING_CANCEL_REQ:
        return "MSG_ID_BT_BM_PAIRING_CANCEL_REQ";
    case MSG_ID_BT_BM_SAVE_SETTING_REQ:
        return "MSG_ID_BT_BM_SAVE_SETTING_REQ";
    case MSG_ID_BT_BM_RESTORE_SETTING_REQ:
        return "MSG_ID_BT_BM_RESTORE_SETTING_REQ";
    case MSG_ID_BT_BM_SERVICE_SEARCH_REQ:
        return "MSG_ID_BT_BM_SERVICE_SEARCH_REQ";
    case MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_REQ:
        return "MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_REQ";
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_REQ:
        return "MSG_ID_BT_BM_SEARCH_ATTRIBUTE_REQ";
    case MSG_ID_BT_BM_SEARCH_RAW_REQ:
        return "MSG_ID_BT_BM_SEARCH_RAW_REQ";
    case MSG_ID_BT_BM_LINK_ALLOW_REQ:
        return "MSG_ID_BT_BM_LINK_ALLOW_REQ";
    case MSG_ID_BT_BM_LINK_DISALLOW_REQ:
        return "MSG_ID_BT_BM_LINK_DISALLOW_REQ";
    case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_NOT_AUTO_REQ:
        return "MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_NOT_AUTO_REQ";
    case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_RSP:    
        return "MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_RSP";
    case MSG_ID_BT_BM_GET_LINK_STATE_REQ:
        return "MSG_ID_BT_BM_GET_LINK_STATE_REQ";
    case MSG_ID_BT_BM_SET_LINK_STATE_REQ:   
        return "MSG_ID_BT_BM_SET_LINK_STATE_REQ";
    case MSG_ID_BT_BM_GET_SCATTERNET_STATE_REQ:
        return "MSG_ID_BT_BM_GET_SCATTERNET_STATE_REQ";
    case MSG_ID_BT_READ_RSSI_REQ:
        return "MSG_ID_BT_READ_RSSI_REQ";
    case MSG_ID_BT_POWERON_CNF:
        return "MSG_ID_BT_POWERON_CNF";
    case MSG_ID_BT_POWEROFF_CNF:
        return "MSG_ID_BT_POWEROFF_CNF";
    case MSG_ID_BT_BM_DISCOVERY_RESULT_IND:
        return "MSG_ID_BT_BM_DISCOVERY_RESULT_IND";
    case MSG_ID_BT_BM_DISCOVERY_CNF:
        return "MSG_ID_BT_BM_DISCOVERY_CNF";
    case MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:
        return "MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF";
    case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:
        return "MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF";
    case MSG_ID_BT_BM_READ_LOCAL_NAME_CNF:
        return "MSG_ID_BT_BM_READ_LOCAL_NAME_CNF";
    case MSG_ID_BT_BM_READ_REMOTE_NAME_CNF:
        return "MSG_ID_BT_BM_READ_REMOTE_NAME_CNF";
    case MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF:
        return "MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF";
    case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:
        return "MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF";
    case MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF:
        return "MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF";
    case MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF:
        return "MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF";
    case MSG_ID_BT_BM_READ_LOCAL_COD_CNF:
        return "MSG_ID_BT_BM_READ_LOCAL_COD_CNF";
    case MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF:
        return "MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF";
    case MSG_ID_BT_BM_READ_PROPERTY_CNF:
        return "MSG_ID_BT_BM_READ_PROPERTY_CNF";
    case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF:
        return "MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF";
    case MSG_ID_BT_BM_PIN_CODE_IND:
        return "MSG_ID_BT_BM_PIN_CODE_IND";
    case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND:
        return "MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND";
    case MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND:
        return "MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND";
    case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_IND:
        return "MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_IND";
    case MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_CNF:
        return "MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_CNF";
    case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_CNF:
        return "MSG_ID_BT_BM_BLOCK_LIST_UPDATE_CNF";
    case MSG_ID_BT_BM_BONDING_CNF:
        return "MSG_ID_BT_BM_BONDING_CNF";
    case MSG_ID_BT_BM_BONDING_RESULT_IND:
        return "MSG_ID_BT_BM_BONDING_RESULT_IND";
    case MSG_ID_BT_BM_DELETE_TRUST_CNF:
        return "MSG_ID_BT_BM_DELETE_TRUST_CNF";
    case MSG_ID_BT_BM_DELETE_TRUST_ALL_CNF:
        return "MSG_ID_BT_BM_DELETE_TRUST_ALL_CNF";
    case MSG_ID_BT_BM_BONDING_CANCEL_CNF:
        return "MSG_ID_BT_BM_BONDING_CANCEL_CNF";
    case MSG_ID_BT_BM_PAIRING_CANCEL_CNF:
        return "MSG_ID_BT_BM_PAIRING_CANCEL_CNF";
    case MSG_ID_BT_BM_SAVE_SETTING_CNF:
        return "MSG_ID_BT_BM_SAVE_SETTING_CNF";
    case MSG_ID_BT_BM_RESTORE_SETTING_CNF:
        return "MSG_ID_BT_BM_RESTORE_SETTING_CNF";
    case MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND:
        return "MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND";
    case MSG_ID_BT_BM_SERVICE_SEARCH_CNF:
        return "MSG_ID_BT_BM_SERVICE_SEARCH_CNF";
    case MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_CNF:
        return "MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_CNF";
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND:
        return "MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND";
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF:
        return "MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF";
    case MSG_ID_BT_BM_SEARCH_RAW_RESULT:
        return "MSG_ID_BT_BM_SEARCH_RAW_RESULT";
    case MSG_ID_BT_BM_SEARCH_RAW_CNF:
        return "MSG_ID_BT_BM_SEARCH_RAW_CNF";
    case MSG_ID_BT_BM_LINK_STATE_IND:
        return "MSG_ID_BT_BM_LINK_STATE_IND";
    case MSG_ID_BT_BM_LINK_ALLOW_CNF:
        return "MSG_ID_BT_BM_LINK_ALLOW_CNF";
    case MSG_ID_BT_BM_LINK_DISALLOW_CNF:    
        return "MSG_ID_BT_BM_LINK_DISALLOW_CNF";
    case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND:    
        return "MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND";
    case MSG_ID_BT_BM_GET_LINK_STATE_CNF:   
        return "MSG_ID_BT_BM_GET_LINK_STATE_CNF";
    case MSG_ID_BT_BM_SET_LINK_STATE_CNF:
        return "MSG_ID_BT_BM_SET_LINK_STATE_CNF";
    case MSG_ID_BT_BM_GET_SCATTERNET_STATE_CNF:
        return "MSG_ID_BT_BM_GET_SCATTERNET_STATE_CNF";
    case MSG_ID_BT_READ_RSSI_CNF:
        return "MSG_ID_BT_READ_RSSI_CNF";
    default:
        return "unknown";
    }
}
void setProfileAddress( PROFILE_enum profile, char *name)
{
    struct sockaddr_un addr;
    socklen_t   addrlen;

    if( socket_make_sockaddr_un(name, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&addr, &addrlen) < 0)
    {
        bt_prompt_trace(MOD_BT, "setProfileAddress(%d,%s) failed", profile, name);
    }
    bt_setProfileAddress(profile, (struct sockaddr_un*)&addr, addrlen);
}

void initProfileAddress(void)
{
    /* PROFILE_A2DP */
    setProfileAddress(PROFILE_A2DP, BT_SOCK_NAME_EXT_ADP_A2DP);
    /* PROFILE_A2DP_DATA */
    setProfileAddress(PROFILE_A2DP_DATA, BT_SOCK_NAME_EXT_ADP_A2DP_DATA);
    /* PROFILE_HFP */
    setProfileAddress(PROFILE_HFP, BT_SOCK_NAME_EXT_ADP_HFP);
    /* PROFILE_SPP */
    setProfileAddress(PROFILE_SPP, BT_SOCK_NAME_EXT_ADP_SPP);
    /* PROFILE_AVRCP */
    setProfileAddress(PROFILE_AVRCP, BT_SOCK_NAME_EXT_ADP_AVRCP);
    /* PROFILE_OPP */
    setProfileAddress(PROFILE_OPP, BT_SOCK_NAME_EXT_ADP_OPP);
    /* PROFILE_BIP */
    setProfileAddress(PROFILE_BIP, BT_SOCK_NAME_EXT_ADP_BIP);
    /* PROFILE_BPP */
    setProfileAddress(PROFILE_BPP, BT_SOCK_NAME_EXT_ADP_BPP);
    /* PROFILE_FTP */
    setProfileAddress(PROFILE_FTP, BT_SOCK_NAME_EXT_ADP_FTP);
    /* PROFILE_PBAP */
    setProfileAddress(PROFILE_PBAP, BT_SOCK_NAME_EXT_ADP_PBAP);
    /* PROFILE_HID */
    setProfileAddress(PROFILE_HID, BT_SOCK_NAME_EXT_ADP_HID);
    /* PROFILE_PAN */
    setProfileAddress(PROFILE_PAN, BT_SOCK_NAME_EXT_ADP_PAN);
    /* PROFILE_SIMAP */
    setProfileAddress(PROFILE_SIMAP, BT_SOCK_NAME_EXT_ADP_SIMAP);
    /* PROFILE_JSR82 */
    setProfileAddress(PROFILE_JSR82, BT_SOCK_NAME_EXT_ADP_JSR82);
    /* PROFILE_PRX */
    setProfileAddress(PROFILE_PRX, BT_SOCK_NAME_EXT_ADP_PRX);
    /* PROFILE_MAP */
    setProfileAddress(PROFILE_MAP, BT_SOCK_NAME_EXT_ADP_MAP);
    /* PROFILE_TIME */
    setProfileAddress(PROFILE_TIME, BT_SOCK_NAME_EXT_ADP_TIME);
	/* PROFILE_HDP */
    setProfileAddress(PROFILE_HDP, BT_SOCK_NAME_EXT_ADP_HDP);
    /* PROFILE_HTP */
    setProfileAddress(PROFILE_HTP, BT_SOCK_NAME_EXT_ADP_HTP);
    /* PROFILE_FMP */
    setProfileAddress(PROFILE_FMP, BT_SOCK_NAME_EXT_ADP_FMP);
	/* PROFILE_ADVANCED */
	setProfileAddress(PROFILE_ADVANCED, BT_SOCK_NAME_EXT_ADP_ADVANCED);
    /* PROFILE_GATTC */
    setProfileAddress(PROFILE_GATTC, BT_SOCK_NAME_EXT_ADP_GATTC);
    /* PROFILE_GATTS */
    setProfileAddress(PROFILE_GATTS, BT_SOCK_NAME_EXT_ADP_GATTS);
}

int switchPort(int bttm){
    bt_prompt_trace(MOD_BT, "[BT] switchPort(%d)", bttm);
#if __BTTM_USE_ORIGINAL_PORT__
    if(bttm){
        memcpy(&g_soaddr_app_gap, &g_cmdaddr, sizeof(struct sockaddr_un));
        g_soaddr_app_gap_len = g_cmdaddrlen;
        bt_prompt_trace(MOD_BT, "g_soaddr_app_gap=%s, g_soaddr_app_gap_len=%d", &g_soaddr_app_gap.sun_path[1], g_soaddr_app_gap_len);
    }else{
        /* restore gap address */
        if( socket_make_sockaddr_un(BT_SOCK_NAME_APP_GAP, 
                                                        ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
                                                        (struct sockaddr_un*)&g_soaddr_app_gap, 
                                                        &g_soaddr_app_gap_len) < 0){
            bt_prompt_trace(MOD_BT, "set gap application sock address failed");
        }
    }
#else
    if(bttm){
        g_serverSocket_backup = g_serverSocket;
        g_serverSocket = socket_local_server(BT_SOCK_NAME_INT_ADP_BTTM,
                                                                    ANDROID_SOCKET_NAMESPACE_ABSTRACT,
                                                                    SOCK_DGRAM);
        bt_prompt_trace(MOD_BT, "g_serverSocket_backup=%d, g_serverSocket=%d", g_serverSocket_backup, g_serverSocket);
        socket_make_sockaddr_un(BT_SOCK_NAME_INT_ADP_BTTM, ANDROID_SOCKET_NAMESPACE_ABSTRACT, &g_soaddr_int_adp, &g_soaddr_int_adp_len);
        if(g_serverSocket < 0){
            g_serverSocket = g_serverSocket_backup;
            bt_prompt_trace(MOD_BT, "[ERR] failed to create server socket : %s, errno=%d", strerror(errno), errno);
            return -1;
        }
        memcpy(&g_soaddr_app_gap, &g_cmdaddr, sizeof(struct sockaddr_un));
        g_soaddr_app_gap_len = g_cmdaddrlen;
        bt_prompt_trace(MOD_BT, "g_soaddr_app_gap=%s, g_soaddr_app_gap_len=%d", &g_soaddr_app_gap.sun_path[1], g_soaddr_app_gap_len);
    }else{
        if(g_serverSocket_backup >=0 ){
            g_serverSocket = g_serverSocket_backup;
            g_serverSocket_backup = -1;
            /* restore server address */
            g_soaddr_int_adp.sun_family = AF_UNIX;
            strcpy (g_soaddr_int_adp.sun_path, BT_SOCK_NAME_INT_ADP);
            /* restore gap address */
            g_soaddr_int_adp_len = (offsetof (struct sockaddr_un, sun_path) + strlen (g_soaddr_int_adp.sun_path) + 1);            
            if( socket_make_sockaddr_un(BT_SOCK_NAME_APP_GAP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&g_soaddr_app_gap, &g_soaddr_app_gap_len) < 0)
            {
                bt_prompt_trace(MOD_BT, "set gap application sock address failed");
            }
        }else{
            bt_prompt_trace(MOD_BT, "[ERR] server backup is %d", g_serverSocket);
            return -1;
        }
        
    }
    if( connect(g_clientSocket, (struct sockaddr *) &g_soaddr_int_adp, g_soaddr_int_adp_len) < 0 ){
        bt_prompt_trace(MOD_BT, "[ERR] failed to internally connect to server : %s, errno=%d", strerror(errno), errno);
        return -1;
    }
#endif
    return 1;
}

void bt_messagehdl_init(void)
{
#if defined(MSGQUEUE_SUPPORTED)
    int       qid;
    key_t   msgkey;

    /* Generate our IPC key value */
    msgkey = ftok(".", 'm');
    if((g_bt_msgqueue = msgget( msgkey, IPC_CREAT | 0660 )) == -1)
    {
        bt_prompt_trace(MOD_BT, "[BT] msgget failed");
        assert(0);
    }
#elif defined(SOCKET_SUPPORTED)
    //struct sockaddr_un name;
    //size_t size;

    
    g_serverSocket = android_get_control_socket("bt.int.adp"); // BT_SOCK_NAME_INT_ADP
//    socket_local_server(BT_SOCK_NAME_INT_ADP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
//    g_serverSocket = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (g_serverSocket < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] create socket failed : %s, errno=%d", strerror(errno), errno);
        return;
    }

    /* mmi socket name */ 
    if( socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&g_soaddr_ext_adp, &g_soaddr_ext_adp_len) < 0)
    {
        bt_prompt_trace(MOD_BT, "set gap external sock address failed");
    }
    /*
    g_soaddr_ext_adp.sun_family = AF_UNIX;
    strcpy (g_soaddr_ext_adp.sun_path, BT_SOCK_NAME_EXT_ADP);
    g_soaddr_ext_adp_len = (offsetof (struct sockaddr_un, sun_path) + strlen (g_soaddr_ext_adp.sun_path) + 1);
    */
    /* Init server socket */
    g_soaddr_int_adp.sun_family = AF_UNIX;
    strcpy (g_soaddr_int_adp.sun_path, BT_SOCK_NAME_INT_ADP);
    g_soaddr_int_adp_len = (offsetof (struct sockaddr_un, sun_path) + strlen (g_soaddr_int_adp.sun_path) + 1);
    /* remove the bt.int.adp first */
/*
    unlink(g_soaddr_int_adp.sun_path);
    if (bind (g_serverSocket, (struct sockaddr *) &g_soaddr_int_adp, g_soaddr_int_adp_len) < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] bind socket failed : %s, errno=%d", strerror(errno), errno);
        return;
    }
*/
    //-----------------------------------------------------------------
    //           setup a2dp streaming socket (g_a2dpstreamSocket)
    //-----------------------------------------------------------------
    g_a2dpstreamSocket = android_get_control_socket("bt.a2dp.stream");	// BT_SOCK_NAME_A2DP_STREAM
    //g_a2dpstreamSocket = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (g_a2dpstreamSocket < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] create a2dp streaming socket failed : %s, errno=%d", strerror(errno), errno);
        return;
    }
//    g_soaddr_a2dp_stream.sun_family = AF_UNIX;
//    strcpy (g_soaddr_a2dp_stream.sun_path, BT_SOCK_NAME_A2DP_STREAM);
//    g_soaddr_a2dp_stream_len = (offsetof (struct sockaddr_un, sun_path) + strlen (g_soaddr_a2dp_stream.sun_path) + 1);
    /* remove the bt.a2dp.stream first */
/*
    unlink(g_soaddr_a2dp_stream.sun_path);
    if (bind (g_a2dpstreamSocket, (struct sockaddr *) &g_soaddr_a2dp_stream, g_soaddr_a2dp_stream_len) < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] bind a2dp streaming socket failed : %s, errno=%d", strerror(errno), errno);
        return;
    }
*/
    {
        int memsize;
        memsize = 131072;
        if(setsockopt(g_a2dpstreamSocket, SOL_SOCKET , SO_RCVBUF , &memsize, sizeof(memsize)) < 0)
        {
            bt_prompt_trace(MOD_BT, "[ERR] set a2dp streaming socket buffer size fail: %s errno=%d\n", strerror(errno), errno);
        }
        else
            bt_prompt_trace(MOD_BT, "a2dp streaming socket memsize set to %d\n" , memsize);            
    }

    #ifdef __BT_HS_30__
    
    g_soaddr_pal.sun_family = AF_UNIX;
    #ifdef BT30HS_NOT_ROOT
    if( socket_make_sockaddr_un(BT_SOCK_NAME_PAL, ANDROID_SOCKET_NAMESPACE_ABSTRACT, &g_soaddr_pal, &g_soaddr_pal_len) < 0 ){
        bt_prompt_trace(MOD_BT, "[ERR] create pal socket failed");
    }
    #else
    strcpy (g_soaddr_pal.sun_path, BT_SOCK_NAME_PAL);
    g_soaddr_pal_len = (offsetof (struct sockaddr_un, sun_path) + strlen (g_soaddr_pal.sun_path) + 1);
    #endif
    #endif

    g_clientSocket = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (g_clientSocket < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] create internal socket failed");
        return;
    }
    if ( connect(g_clientSocket, (struct sockaddr *) &g_soaddr_int_adp, g_soaddr_int_adp_len) < 0 )
    {
        bt_prompt_trace(MOD_BT, "[ERR] failed to internally connect to server : %s, errno=%d", strerror(errno), errno);
    }
    /* Initialize profile server address */
    initProfileAddress();
#endif
    bt_prompt_trace(MOD_BT, "[MSG] msg queue successfully created");
}

U8 bt_enQueue_message(module_type mod_type, void *ptr, U16 size)
{
#if defined(MSGQUEUE_SUPPORTED)
    ilm_struct *ilm;
    long type;
    
    ilm = (ilm_struct*)ptr;
    type = get_msg_type(ilm);
    if(type != -1)
    {
        ilm->mtype = type;
        if( -1 == msgsnd(g_bt_msgqueue, ptr, sizeof(ilm_struct)-sizeof(long), IPC_NOWAIT) )
        {
            bt_prompt_trace(MOD_BT, "[BT] msgsnd failed : %d", errno);
        }
    }
#elif defined(SOCKET_SUPPORTED)
    ilm_struct *ilm;
    ilm = (ilm_struct*)ptr;
    int ret = -1;

    //bt_prompt_trace(MOD_BT, "[MSG] enQueue msg %d", ilm->msg_id);
    
    if(mod_type == MOD_BT)
    {
        /* bt_prompt_trace(MOD_BT, "[MSG] enQueueMsg msg_id=%d", ilm->msg_id); */
        ret = send (g_clientSocket, ptr, size, MSG_DONTWAIT);
    }
    #ifdef __BT_SINGLE_ANTENNA__
    else if(mod_type == MOD_BWCS)
    {
        socket_make_sockaddr_un(WIFI_SOCK_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&g_cmdaddr, &g_cmdaddrlen);
        do {
            ret = sendto(g_serverSocket, ptr, size, 0, (const struct sockaddr*)&g_cmdaddr, g_cmdaddrlen);
        }while(ret < 0 && errno == EINTR);
    }
    #endif
    #ifdef __BT_HS_30__
    else if(mod_type == MOD_PAL) {
        ret = sendto(g_serverSocket, ptr, size, 0, (const struct sockaddr*)&g_soaddr_pal, g_soaddr_pal_len);
        if(ret < 0)
        {
            bt_prompt_trace(MOD_BT, "[MSG] PAL enQueueMsg fail : %s, %d", strerror(errno), errno);
        }
    }
    #endif
    else {
        BtCommand *pCmd;
        pCmd = removeBtCmd(ilm->msg_id);
        if(pCmd)
        {
            bt_prompt_trace_lv3(MOD_BT, "[MSG] response to cmd=%d with resp=%d", pCmd->cmd_id, pCmd->resp_id);
            bt_prompt_trace_lv3(MOD_BT, "CMD addr len=%d", pCmd->addrlen);
            ret = sendto(g_serverSocket, ptr, size, 0, (const struct sockaddr*)&pCmd->addr, pCmd->addrlen);
            freeBtCmd(pCmd);
        }
        else
        {
            bt_prompt_trace_lv3(MOD_BT, "[MSG] indication=%d", ilm->msg_id);
//            if((ilm->msg_id >= MSG_ID_BT_GAP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_GAP_GROUP_END))
            if(ilm->msg_id <= MSG_ID_BT_SDP_GROUP_END)
            {
                    bt_prompt_trace(MOD_BT, "[MSG] indication=%d(%s) to ext adp", ilm->msg_id, getMessageStringLog(ilm->msg_id));
                    ret = sendto(g_serverSocket, ptr, size, 0, (const struct sockaddr*)&g_soaddr_ext_adp, g_soaddr_ext_adp_len);
            }
            else
            {
                /* Send MSG to profiles */
                PROFILE_enum profile = NUM_OF_PROFILE;
                #if defined(__BT_A2DP_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_A2DP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_A2DP_GROUP_END))
                {
                    profile = PROFILE_A2DP; 
                }else
                if((ilm->msg_id >= MSG_ID_BT_A2DP_DATA_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_A2DP_DATA_GROUP_END))
                {
                    profile = PROFILE_A2DP_DATA; 
                }else
                #endif
                #if defined(__BT_HFG_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_HFG_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_HFG_GROUP_END))
                { 
                    profile = PROFILE_HFP; 
                }else 
                #endif
                #if defined (__BT_SPP_PROFILE__)
                if ((ilm->msg_id >= MSG_ID_BT_SPP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_SPP_GROUP_END))
                {
                    profile = PROFILE_SPP;
                }else
                #endif
                #if defined (__BT_AVRCP_PROFILE__)
                if ((ilm->msg_id >= MSG_ID_BT_AVRCP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_AVRCP_GROUP_END))
                {
                    profile = PROFILE_AVRCP;
                }else
                #endif
                #if defined(__BT_OPP_PROFILE__) 
                if( (ilm->msg_id >= MSG_ID_BT_OPPC_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_OPPS_GROUP_END) )
                {
                    profile = PROFILE_OPP;
                }else
                #endif
                #if defined (__BT_BIPI_PROFILES__) || defined (__BT_BIPR_PROFILE__)
                if ((ilm->msg_id >= MSG_ID_BT_BIPI_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_BIPR_GROUP_END))
                {
                    profile = PROFILE_BIP;
                }else
                #endif
                #if defined (__BT_BPP_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_BPP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_BPP_GROUP_END))
                {
                    profile = PROFILE_BPP;
                }else
                #endif
                #if defined(__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__)   
                if((ilm->msg_id >= MSG_ID_BT_FTPC_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_FTPS_GROUP_END))
                {
                    profile = PROFILE_FTP;
                }else
                #endif
                #if defined(__BT_PBAP_PROFILE__) 
                if((ilm->msg_id >= MSG_ID_BT_PBAP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_PBAP_GROUP_END))
                {
                    profile = PROFILE_PBAP;
                }else
                #endif
                #if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
                if ((ilm->msg_id >= MSG_ID_BT_HID_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_HID_GROUP_END))
                {
                    profile = PROFILE_HID;
                }else
                #endif                
	       #if defined(__BT_PAN_PROFILE__) 
                if ((ilm->msg_id >= MSG_ID_BT_PAN_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_PAN_GROUP_END))
                {
                    profile = PROFILE_PAN;
                }else
                #endif   
#if defined(__BT_SIM_PROFILE__)
				if ((ilm->msg_id >= MSG_ID_BT_SIMAP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_SIMAP_GROUP_END))
				{
					/* SIMAP  */
					profile = PROFILE_SIMAP;
				}else
#endif				
                #if defined(__BT_JSR82__) || defined(__BT_JSR82_L2RF__)
                if((ilm->msg_id >= MSG_ID_BT_JSR82_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_JSR82_GROUP_END))
                {
                    profile = PROFILE_JSR82;
                }else
                #endif
                #if defined(__BT_PRXR_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_PRXR_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_PRXR_GROUP_END))
                {
                    profile = PROFILE_PRX;
                }else
                #endif
                #if defined(__BT_PRXM_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_PRXM_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_PRXM_GROUP_END))
                {
                    profile = PROFILE_PRX;
                }else
                #endif
                #if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_MAP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_MAP_GROUP_END))
                {
                    profile = PROFILE_MAP;
                }else
                #endif
		   #if defined(__BT_TIMEC_PROFILE__) || defined(BT_TIMES_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_TIMEC_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_TIMES_GROUP_END))
                {
                    profile = PROFILE_TIME;
                }else
		   #endif
		#if (defined(__BT_HDP_PROFILE__))
		if((ilm->msg_id >= MSG_ID_BT_HDP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_HDP_GROUP_END))
		{
		    profile = PROFILE_HDP;
		} else
		#endif
                #if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_HTP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_HTP_GROUP_END))
                {
                    profile = PROFILE_HTP;
                }else
                #endif
                #if defined(__BT_FMPL_PROFILE__) || defined(__BT_FMPT_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_FMP_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_FMP_GROUP_END))
                {
                    profile = PROFILE_FMP;
                }else
                #endif
                #if defined(__BT_GATTC_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_GATTC_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_GATTC_GROUP_END))
                {
                    profile = PROFILE_GATTC;
                }else
                #endif
                #if defined(__BT_GATTS_PROFILE__)
                if((ilm->msg_id >= MSG_ID_BT_GATTS_GROUP_START) && (ilm->msg_id <= MSG_ID_BT_GATTS_GROUP_END))
                {
                    profile = PROFILE_GATTS;
                }else
                #endif
                {
                    bt_prompt_trace(MOD_BT, "Unknown or unsupported message : %d", ilm->msg_id);
                    ret = -1;
                }
                /* Send Message o profile */
                if(profile != NUM_OF_PROFILE)
                {
                    bt_prompt_trace_lv3(MOD_BT, "[MSG] Send indication %d to profile : %d", ilm->msg_id, profile);
                    if(g_profileAddr[profile].addrlen)
                    {
                        bt_prompt_trace_lv3(MOD_BT, "[MSG] addr family=%d, addr=%s, addrlen=%d", g_profileAddr[profile].addr.sun_family, &g_profileAddr[profile].addr.sun_path[1], g_profileAddr[profile].addrlen);
                        ret = sendto(g_serverSocket, ptr, size, 0, (const struct sockaddr*)&g_profileAddr[profile].addr, g_profileAddr[profile].addrlen);
                    }
                    else
                    {
                        ret = -1;
                    }
                }
            }
        }
    }
    if(ret < 0)
    {
        bt_prompt_trace(MOD_BT, "[MSG] enQueueMsg fail : %s, %d", strerror(errno), errno);
        return 1;
    }
    else
    {
        bt_prompt_trace_lv3(MOD_BT, "[MSG] enQueueMsg success : %d", ret);
        return 0;
    }
#endif
}

void *bt_deQueue_message(module_type mod_type)
{
#ifdef MSGQUEUE_SUPPORTED
    if( -1 == msgrcv(g_bt_msgqueue, &g_rec_ilm, sizeof(ilm_struct), 1/* BT task group */, 0) )
    {
        bt_prompt_trace(MOD_BT, "[BT] msgrcv failed : %d", errno);
        return NULL;
    }
    else
    {
        return &g_rec_ilm;
    }
#elif defined(SOCKET_SUPPORTED)
    int ret;

    OS_MemSet(&g_cmdaddr, 0x0 ,sizeof(g_cmdaddr));	

    /* ret = recvfrom(g_serverSocket, &g_rec_ilm, sizeof(g_rec_ilm), MSG_TRUNC, &g_cmdaddr, NULL); */
    g_cmdaddrlen = sizeof(g_cmdaddr);
    bt_prompt_trace_lv3(MOD_BT, "g_cmdaddrlen=%d", g_cmdaddrlen);
    ret = recvfrom(g_serverSocket, &g_rec_ilm, sizeof(g_rec_ilm), 0, 
		              (struct sockaddr*)&g_cmdaddr, &g_cmdaddrlen);
    bt_prompt_trace_lv3(MOD_BT, "cmd addr=%s, len=%d", g_cmdaddr.sun_path, g_cmdaddrlen);
    if(ret < 0)
    {
        bt_prompt_trace(MOD_BT, "[MSG] deQueueMsg fail : %s, %d", strerror(errno), errno);
    }
#if defined(__HDP_FD_SUPPORT__)
	else if (hdp_adp_check_socket_address(&g_cmdaddr.sun_path[1]))
	{
	
		hdp_deQueue_data((U8*)&g_cmdaddr.sun_path[1], &g_rec_ilm, ret);
		return NULL;
	}
#endif
    else
    {
        U16 *field;
        bt_prompt_trace_lv3(MOD_BT, "[MSG] deQueueMsg success : %d", ret);
        /* enqueue the cmd into cmd queue */
        addBtCmd(&g_cmdaddr, g_cmdaddrlen, g_rec_ilm.msg_id);
        // recover the LOCAL_PARA_HDR
        g_rec_ilm.sap_id = INVALID_SAP;
        field = (U16 *)g_rec_ilm.ilm_data;
        *field = 1;
        *(++field) = ret-(sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE);
        //field = (U16 *)&g_rec_ilm.ilm_data[2];
        //*field = ret-(sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE);
    }
    //bt_prompt_trace(MOD_BT, "[MSG] indication=%d(%s) to int adp", g_rec_ilm.msg_id, getMessageStringLog(g_rec_ilm.msg_id));
    return &g_rec_ilm;
#else
    return NULL;
#endif
}
#endif
