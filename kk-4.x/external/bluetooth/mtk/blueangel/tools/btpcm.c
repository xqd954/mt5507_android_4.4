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
#define LOG_TAG "btpcm"

typedef enum _PCM_CMD
{
    PCM_CMD_SETTING = 1,
    PCM_CMD_SWITCH = 2,
    PCM_CMD_SCO_ACCEPT = 3,
    NUM_OF_PCM_CMD,
}PCM_CMD;

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

int sendMessage(int sockfd, msg_type msg_id, void *ptr, kal_uint16 size)
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
    
    if(sockfd)
    {
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0)
        {
            printf("send msg fail : %s, %d\n", strerror(errno), errno);
        }
        else
        {
            printf("send msg success : %d\n", ret);
        }
    }
    else
    {
        printf("socket uninitialized\n");
    }
    return ret;
}

int waitResponse(int sockfd, msg_type resp_id, ilm_struct *ilm, kal_uint16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;

    Timeout.tv_usec = 0;
    Timeout.tv_sec  = 5;
    FD_ZERO(&readfs);
    if(sockfd)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        printf("sockfd == 0. exit\n");
    }
    res = select(sockfd+1, &readfs, NULL, NULL, &Timeout);
    if(res > 0)
    {
        res = recvfrom(sockfd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
        if(res < 0)
        {
            printf("[JNI] recvfrom failed : %s, %d\n", strerror(errno), errno);
        }
        else
        {
            if( ilm->msg_id != resp_id)
            {
                res = -1;
                printf("[JNI] recv resp=%u is mismatch, expected : %u\n", (unsigned int)ilm->msg_id, (unsigned int)resp_id);
            }
            else
            {
                *size = res;            
                printf("[JNI] recv resp=%u, bytes=%d\n", (unsigned int)ilm->msg_id, res);
            }
        }
    }
    else if(res == 0)
    {
        printf("[JNI] timeout waiting response, in 5 milliseconds\n");
    }
    else
    {
        printf("[JNI] wait select failed : %s, %d\n", strerror(errno), errno);
    }
    return res;
}

int openSocket()
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
int setPCM(unsigned char *value, unsigned short len){
    int fd = -1;
    ilm_struct ilm;
    kal_uint16 size = sizeof(bt_set_dual_pcm_setting_req_struct);
    bt_set_dual_pcm_setting_req_struct *req_p = (bt_set_dual_pcm_setting_req_struct*)ilm.ilm_data;

    ALOGD("setPCM(%p, %d)", value, len);

    if(!value || len == 0 || len > 32){
        ALOGE("setPCM failed");
        return -1;
    }
    
    fd = openSocket();
    if(fd < 0){
        return -1;
    }
    memcpy(req_p->value, value, len);
    req_p->val_len = len;
    if(sendMessage(fd, MSG_ID_BT_SET_DUAL_PCM_SETTING_REQ, (void*)&ilm, sizeof(bt_set_dual_pcm_setting_req_struct)) < 0 || 
        waitResponse(fd, MSG_ID_BT_SET_DUAL_PCM_SETTING_CNF, &ilm, &size) < 0){		 
        ALOGE("setPCM failed\n");
        close(fd);
        return -1;
    }
    ALOGD("setPCM success\n");
    close(fd);
    return 0;
}

int switchPCM(unsigned char *value, unsigned short len){
    int fd = -1;
    ilm_struct ilm;
    kal_uint16 size = sizeof(bt_set_dual_pcm_switch_req_struct);
    bt_set_dual_pcm_switch_req_struct *req_p = (bt_set_dual_pcm_switch_req_struct*)ilm.ilm_data;

    ALOGD("switchPCM(%p, %d)", value, len);
    if(!value || len == 0 || len > 32){
        ALOGE("switchPCM failed");
        return -1;
    }
    fd = openSocket();
    if(fd < 0){
        return -1;
    }
    memcpy(req_p->value, value, len);
    req_p->val_len = len;
    if(sendMessage(fd, MSG_ID_BT_SET_DUAL_PCM_SWITCH_REQ, (void*)&ilm, sizeof(bt_set_dual_pcm_switch_req_struct)) < 0 || 
        waitResponse(fd, MSG_ID_BT_SET_DUAL_PCM_SWITCH_CNF, &ilm, &size) < 0){		 
        ALOGE("switchPCM failed\n");
        close(fd);
        return -1;
    }
    close(fd);
    ALOGD("switchPCM success\n");
    return 0;
}

int setSCOAccept(unsigned char value){
    int fd = -1;
    ilm_struct ilm;
    kal_uint16 size = sizeof(bt_set_sco_accept_req_struct);
    bt_set_sco_accept_req_struct *req_p = (bt_set_sco_accept_req_struct*)ilm.ilm_data;

    ALOGD("setSCOAccept(%d)", value);
    fd = openSocket();
    if(fd < 0){
        return -1;
    }
    req_p->value = value;
    if(sendMessage(fd, MSG_ID_BT_SET_SCO_ACCEPT_REQ, (void*)&ilm, sizeof(bt_set_sco_accept_req_struct)) < 0 || 
        waitResponse(fd, MSG_ID_BT_SET_SCO_ACCEPT_CNF, &ilm, &size) < 0){		 
        ALOGE("setSCOAccept failed\n");
        close(fd);
        return -1;
    }else{
        bt_set_sco_accept_cnf_struct *cnf_p = (bt_set_sco_accept_cnf_struct*)ilm.ilm_data;
        ALOGD("setSCOAccept returns %d\n", cnf_p->value);
    }
    ALOGD("setSCOAccept success\n");
    close(fd);
    return 0;
}
#if defined(EVDO_DT_SUPPORT)
/* shuliang: PCMCLK 128kHz < 1MHz, PSYNC 8kHz, long frame sync */
unsigned char pcm_setting[2][4] = 
{
    {0x23, 0x10, 0x00, 0x00}, /* PCM 1 */
    {0x03, 0x10, 0x00, 0x00}, /* PCM 2 */
};
#else
unsigned char pcm_setting[2][4] = 
{
    {0x23, 0x10, 0x00, 0x00}, /* PCM 1 */
    {0x23, 0x10, 0x00, 0x00}, /* PCM 2 */
};
#endif

unsigned char pcm_switch[2][1] = 
{
    {0x01}, /* PCM 1 */
    {0x00}, /* PCM 2 */
};

const unsigned char pcm_num = sizeof(pcm_setting)/sizeof(pcm_setting[0]);

/* 0 for PCM 1 and 1 for PCM 2 */
int setBluetoothPCM(unsigned char pcm_id){
    int ret = -1;
    ALOGD("setBluetoothPCM(%d)", pcm_id);
    ALOGD("PCM setting : 0x%02X 0x%02X 0x%02X 0x%02X",
        pcm_setting[pcm_id][0],
        pcm_setting[pcm_id][1],
        pcm_setting[pcm_id][2],
        pcm_setting[pcm_id][3]);
    if(pcm_id < pcm_num){
        if(setPCM(pcm_setting[pcm_id], 4) >= 0){
            if(switchPCM(pcm_switch[pcm_id], 1) >= 0){
                ret = 1;
            }else{
                ALOGE("switchPCM failed.");
            }
        }else{
            ALOGE("setPCM failed");
        }
    }else{
        ALOGE("wrong pcm_id = %d", pcm_id);
    }
    return ret;
}


#ifdef __BT_PCM_SWITCH_TEST__
void usage(){
    printf("Usage : btpcm_test cmd value\n");
    printf("cmd : 1-set, 2-switch, 3-sco reject\n");
    printf("value : parameter of cmd\n");    
}

int main (int argc, char **argv)
{
    kal_uint8 cmd = 0;
    kal_uint8 value[] = {1,2,3};
    if(argc < 3){
        usage();
    }else{
        cmd = atoi(argv[1]);
        //value = atoi(argv[2]);
        printf("cmd=%d, value=%d\n", cmd, value);
        switch(cmd){
        case 1:
            setPCM(value, sizeof(value));
            break;
        case 2:
            switchPCM(value, sizeof(value));
            break;
        case 3:
            setSCOAccept(value);
            break;
        default:
            usage();
            break;
        }
    }
    return 0;
}

#endif


