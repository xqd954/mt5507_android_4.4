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

#include <stdio.h>
#include "string.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_mmi_msg.h"
#include "bluetooth_struct.h"
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include "cutils/sockets.h"
#include <cutils/log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "btem"

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

static int sendMessage(int sockfd, msg_type msg_id, void *ptr, kal_uint16 size)
{
    int ret = -1;
    ilm_struct *ilm;
    U16 *field;
	
    ret = -1;
    ilm = (ilm_struct*)ptr;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd >= 0)
    {
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0)
        {
            ALOGE("send msg fail : %s, %d\n", strerror(errno), errno);
        }
        else
        {
            ALOGD("send msg success : %d\n", ret);
        }
    }
    else
    {
        ALOGE("socket uninitialized\n");
    }
    return ret;
}

static int waitResponse(int sockfd, msg_type resp_id, ilm_struct *ilm, kal_uint16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;

    Timeout.tv_usec = 0;
    Timeout.tv_sec  = 5;
    FD_ZERO(&readfs);
    if(sockfd >= 0)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        ALOGE("sockfd <= 0. exit\n");
        return -1;
    }
    res = select(sockfd+1, &readfs, NULL, NULL, &Timeout);
    if(res > 0)
    {
        res = recvfrom(sockfd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
        if(res < 0)
        {
            ALOGE("[JNI] recvfrom failed : %s, %d\n", strerror(errno), errno);
        }
        else
        {
            if( ilm->msg_id != resp_id)
            {
                res = -1;
                ALOGE("[JNI] recv resp=%u is mismatch, expected : %u\n", (unsigned int)ilm->msg_id, (unsigned int)resp_id);
            }
            else
            {
                *size = res;            
                ALOGD("[JNI] recv resp=%u, bytes=%d\n", (unsigned int)ilm->msg_id, res);
            }
        }
    }
    else if(res == 0)
    {
        ALOGE("[JNI] timeout waiting response, in 5 milliseconds\n");
    }
    else
    {
        ALOGE("[JNI] wait select failed : %s, %d\n", strerror(errno), errno);
    }
    return res;
}

static int openSocket()
{
    int fd = -1;
    struct sockaddr_un addr;
    socklen_t   addrlen;
    struct sockaddr_un btname;
    socklen_t   btnamelen;

    /* Set bt server address */
    btname.sun_family = AF_UNIX;
    strcpy (btname.sun_path, BT_SOCK_NAME_INT_ADP);
    btnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (btname.sun_path) + 1);
    /* set socket name */
    socket_make_sockaddr_un("btpcm", ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&addr, &addrlen);

    fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        ALOGE("create socket failed\n");
        return -1;
    }	
    if(bind (fd, (struct sockaddr *)&addr, addrlen) < 0)
    {
        ALOGE("[ERR] bind socket failed : %s, errno=%d\n", strerror(errno), errno);
        close(fd);
        return -1;
    }
    if(connect(fd, (const struct sockaddr*)&btname, btnamelen) < 0)
    {
        ALOGE("[ERR] connect to /data/btserv failed : %s, errno=%d\n", strerror(errno), errno);
        close(fd);
        return -1;
    }
    return fd;
}

/**************************************
* exported API
**************************************/
/* Param : 0 for disable, others for bitmask of fw core dump (Currently set to 0xFFFFFFFF) */
/* Return : 0 success, others fail */
int setFWDump(unsigned int fwdump){
    int fd = -1;
    ilm_struct ilm;
    kal_uint16 size = sizeof(bt_test_cmd_req_struct);
    bt_test_cmd_req_struct *req_p = (bt_test_cmd_req_struct*)ilm.ilm_data;

    ALOGD("setFWDump(%X)", fwdump);
    
    fd = openSocket();
    if(fd < 0){
        return -1;
    }
    sprintf((char*)req_p->test_cmd_content, "fwdump=%X", fwdump);
    if(sendMessage(fd, MSG_ID_BT_TEST_CMD_REQ, (void*)&ilm, sizeof(bt_test_cmd_req_struct)) < 0 || 
        waitResponse(fd, MSG_ID_BT_TEST_CMD_CNF, &ilm, &size) < 0){		 
        ALOGE("setFWDump failed\n");
        close(fd);
        return -1;
    }
    ALOGD("setFWDump success\n");
    close(fd);
    return 0;
}

