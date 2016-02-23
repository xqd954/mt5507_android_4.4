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


#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include "cutils/sockets.h"
#include "bt_common.h"
#include "bt_feature.h"
#include "bt_adp_log.h"
#include "bluetooth_bwcs.h"
#include "bt_wmt_porting.h"
#include "wmt_struct.h"

#ifdef __BT_SINGLE_ANTENNA__
#define BWCS_LOG(fmt, ...) bt_prompt_trace(MOD_BT, "[BWCS] " fmt, ## __VA_ARGS__);
#define BWCS_ERR(fmt, ...) bt_prompt_trace(MOD_BT, "[BWCS][ERR] " fmt, ## __VA_ARGS__);

static void LogBwcs(BWCS_MESSAGE_STRUCT *msg, int result)
{
    ilm_struct ilm;
    wmt_bt_status_ind_struct *ind = (wmt_bt_status_ind_struct*)ilm.ilm_data;
    ilm.dest_mod_id = MOD_BWCS;
    ilm.msg_id = MSG_ID_WMT_BT_STATUS_IND;
    ilm.sap_id = 0;
    ilm.src_mod_id = MOD_BT;
    ind->msg_len = sizeof(wmt_bt_status_ind_struct);
    ind->ref_count = 1;
    memcpy((void*)&ind->msg,  (const void*)msg, sizeof(BWCS_MESSAGE_STRUCT));
    ind->result = result;
    bt_log_primitive(&ilm);
}

static int bwcs_send_internal_event_msg(void *Msg, unsigned int size)
{
    int sockfd;
    int iResult = 0;
    struct sockaddr_un local;
    if ((sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0)
    {
        BWCS_ERR("open socket fails");
        return -1;
    }
    memset((void*)&local, 0, sizeof(local));
    local.sun_family = AF_LOCAL;
    strcpy(local.sun_path, "/data/bwcs_sock");

    do {
        iResult = sendto( sockfd, Msg, size, 0, (struct sockaddr*)&local, sizeof(local));
    }while(iResult < 0 && errno == EINTR);
    if (iResult < 0)
    {
        BWCS_ERR("send BWCS message & event fail:%s", strerror(errno));
        iResult = -1;
    }
    close(sockfd);
    LogBwcs((BWCS_MESSAGE_STRUCT*)Msg, iResult);
    return iResult;
}

int bwcs_send_internal_event(unsigned short event)
{
    BWCS_MESSAGE_STRUCT sBwcsMsg;

    BWCS_LOG("bwcs_send_internal_event(%d)", event);
    memset(&sBwcsMsg, 0, sizeof(sBwcsMsg));
    sBwcsMsg.usType = MSG_TYPE_BWCS;
    sBwcsMsg.bwcs_event = event;
    return bwcs_send_internal_event_msg(&sBwcsMsg, sizeof(sBwcsMsg));
}

int sent_aclsco_connection_info_event(U8 *sba, unsigned short handle, unsigned char type, unsigned short event){
    BWCS_MESSAGE_STRUCT sBwcsMsg;

    if(sba == NULL) {
        BWCS_ERR("sent_aclsco_connection_info_event : sba == NULL");
        return -1;
    }

    BWCS_LOG("sent_aclsco_connection_info_event(sba=%02x%02x%02x%02x%02x%02x, handle=%d, type=%d, event=%d)", 
                        sba[0], sba[1], sba[2], sba[3], sba[4], sba[5],
                        handle,
                        type,
                        event);
    memset(&sBwcsMsg, 0, sizeof(sBwcsMsg));
    sBwcsMsg.usType = MSG_TYPE_BWCS;
    sBwcsMsg.bwcs_event = event;
    sBwcsMsg.conn_num = 0x01; /*the number is acl+sco+esco*/ 
    sBwcsMsg.bt_conn.handle = handle;
    //if(event == BWCS_BT_DISCONN)    {
    //    sBwcsMsg.bt_conn.handle = handle;
    //}
    memcpy(sBwcsMsg.bt_conn.bdaddr.b, sba, 6);
    sBwcsMsg.bt_conn.type = type;
    return bwcs_send_internal_event_msg(&sBwcsMsg, sizeof(sBwcsMsg));
}

int avdtp_bwcs_send_internal_event(unsigned short handle, U8 *dst, unsigned short event)
{
    BWCS_MESSAGE_STRUCT sBwcsMsg;

    if(dst == NULL) {
        BWCS_ERR("avdtp_bwcs_send_internal_event : dst == NULL");
    }
    BWCS_LOG("avdtp_bwcs_send_internal_event(handle=%d, dst=%02X%02X%02X%02X%02X%02X, event=%d)", 
                        handle,
                        dst[0], dst[1], dst[2], dst[3], dst[4], dst[5],
                        event);
    memset(&sBwcsMsg, 0, sizeof(sBwcsMsg));
    sBwcsMsg.usType = MSG_TYPE_BWCS;
    sBwcsMsg.bwcs_event = event;
    sBwcsMsg.bt_conn.handle = handle;
    memcpy((void*)sBwcsMsg.bt_conn.bdaddr.b, dst, 6);
    return bwcs_send_internal_event_msg(&sBwcsMsg, sizeof(sBwcsMsg));
}

//extern int g_serverSocket;
#if 0
int sent_bwcs_msg(void* ptr, unsigned int size) {
    int iResult = 0;
    struct sockaddr_un name;
    socklen_t namelen;
    
    BWCS_LOG("sent_bwcs_msg(size=%d)", size);
    if(g_serverSocket < 0) {
        BWCS_ERR("server socket is not ready");
        return -1;
    }
    socket_make_sockaddr_un(WIFI_SOCK_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen);

    do {
        iResult = sendto(g_serverSocket, ptr, size, 0, (struct sockaddr*)&name, namelen);
    }while(iResult < 0 && errno == EINTR);
    if (iResult < 0)
    {
        BWCS_ERR("send BWCS confirm message fail:%s", strerror(errno));
        iResult = -1;
    }
    return iResult;
}
#endif
#endif /* __BT_SINGLE_ANTENNA__ */
