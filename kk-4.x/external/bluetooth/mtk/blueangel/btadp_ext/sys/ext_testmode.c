/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/sockets.h>
#include <sys/stat.h>

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_ext_debug.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "ext_interlayer.h"

#if 0 // defined(MTK_TC1_FEATURE)
#include "tc1_partition.h"
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ext_testmode" 

int queryTestMode(void);

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif 

#if 0
/*******************************
*  Start - Dummy code
*******************************/
static unsigned char g_btaddr[6] = {0,0x11,0x22,0x33,0x44,0x55};
int LGE_FacReadBtAddr(unsigned char *btAddr){
    if(btAddr){
        memcpy(btAddr, g_btaddr, 6);
    }else{
        return 0;
    }
    return 1;
}
int LGE_FacWriteBtAddr(unsigned char *btAddr,int needFlashProgram){
    if(btAddr){
        memcpy(g_btaddr, btAddr, 6);
    }else{
        return 0;
    }
    return 1;
}
/*******************************
*  End - Dummy code
*******************************/
#endif

static int openSocket()
{
    int fd = -1;
    ALOGD("openSocket");
#if 1
    fd = socket_local_server("bt.bttm.client", ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    if(fd < 0){
        ALOGE("openSocket failed : %s, errno=%d", strerror(errno), errno);
    }
#else
    struct sockaddr_un addr;
    socklen_t   addrlen;
     
    /* set socket name */
    addr.sun_family = AF_LOCAL;
    addrlen = sizeof(short);
    fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if(fd < 0){
        ALOGE("create socket failed\n");
        return -1;
    }	
    if(bind (fd, (struct sockaddr *)&addr, addrlen) < 0){
        ALOGE(" bind socket failed : %s, errno=%d\n", strerror(errno), errno);
        close(fd);
        return -1;
    } 
#endif
    if ( fchmod(fd, 0777) < 0 ){
        ALOGE("fchmod failed : %s, errno=%d\n", strerror(errno), errno);
    }else{
        ALOGD("fchmod success");
    }    
    return fd;
}

int switchServerPort(int fd, int bttm){
    int ret = -1;    
#if __BTTM_USE_ORIGINAL_PORT__
    ret = socket_local_client_connect(fd, BT_SOCK_NAME_INT_ADP, ANDROID_SOCKET_NAMESPACE_FILESYSTEM, SOCK_DGRAM);
#else
    if(bttm){
        ret = socket_local_client_connect(fd, BT_SOCK_NAME_INT_ADP, ANDROID_SOCKET_NAMESPACE_FILESYSTEM, SOCK_DGRAM);
    }else{
        ret = socket_local_client_connect(fd, BT_SOCK_NAME_INT_ADP_BTTM, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    }
#endif
    return ret;
}

/* send req and wait resp. If resp == 0, that means do not wait */
static int send_req_to_bt( int sockfd, ilm_struct* ilm, msg_type resp, int size ){	
    ilm->sap_id = 0;
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    if(send_message(sockfd, ilm, size) < 0){
        ALOGE("send msg(%lu) failed", ilm->msg_id);
        return -1;
    }else{
        if(resp){
            if(recv_message_timeout(sockfd, ilm, 15000) < 0 ){
                ALOGE("wait %d failed", resp);
                return -1;
            }else{
                if(ilm->msg_id != resp){
                    ALOGE("received message %lu is not match resp=%d", ilm->msg_id, resp);
                    return -1;
                }
            }
        }
    }
    ALOGD("send_req_to_bt success\n");
    return 0;
}

static int writeTmpAddr(unsigned char *addr){
    int ret= -1;
    FILE *fp = fopen("/data/BT_Addr", "r+");
    unsigned char ori_addr[6];
    ALOGD("writeTmpAddr(%p)", addr);
    if(fp){
        ret = fseek(fp, 0, SEEK_SET);
        //printf("fseek return %d\n", ret);
        if(!addr){
            addr = ori_addr;
            // TODO: Roll back original addr
#if 0 // defined(MTK_TC1_FEATURE)       
			if( TC1_FAC_NAME(FacReadBtAddr(addr)) == 0 )
#else
			if (0)
#endif
			{
                ALOGE("LGE_FacReadBtAddr failed");
                fclose(fp);
                return -1;
            }
        }
        if( fwrite(addr, 6, 1, fp) == 1){
            ret = 1;
        }else{
            ALOGE("fwrite failed : ferror=%d", ferror(fp));
        }
        fclose(fp);
    }else{
        ALOGE("open /data/BT_Addr failed : ferror=%d"); //, ferror(fp));
    }
    return ret;
}

static int readTmpAddr(unsigned char *addr){
    int ret= -1;
    FILE *fp = fopen("/data/BT_Addr", "r");
    unsigned char ori_addr[6];
    ALOGD("readTmpAddr(%p)", addr);
    if(fp){
        if(addr){
            ret = fread(addr, 6, 1, fp);
            if( ret == 1){
                ret = 1;
            }else{
                ALOGE("fread failed : ret=%d, ferror=%d", ret, ferror(fp));
            }
        }
        fclose(fp);
    }else{
        ALOGE("open /data/BT_Addr failed : ferror=%d", ferror(fp));
    }
    return ret;
}


typedef union _union_bttm_msgs{
    bt_bttm_switch_port_req_struct switch_req;
    bt_bttm_switch_port_cnf_struct switch_cnf;
    bt_poweron_cnf_struct poweron_cnf;
    bt_bm_write_scanenable_mode_req_struct writescanmode_req;
    bt_bm_write_scanenable_mode_cnf_struct writescanmode_cnf;
    bt_test_mode_req_struct testmode_req;
    bt_test_mode_cnf_struct testmode_cnf;
}union_bttm_msgs;

/* enable : 0 - disable, others - enable   */
int enableTestMode(int enable, unsigned char *addr){
    int ret = -1;
    int fd;
    struct sockaddr_un btname;
    socklen_t   btnamelen;
    int hd_size = (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE);
    int max_size = sizeof(union_bttm_msgs) + hd_size;
    ilm_struct *ilm = NULL;
    union_bttm_msgs *msg;
    

    /* check if the bttm is already in the mode requested */
    if(queryTestMode() == enable){
        ALOGW("Test mode is already enabled/disabled");
        return 0;
    }

    /* Open socket */
    fd = openSocket();
    if(fd < 0){
        return -1;
    }

    /* Connect socket to current BT server address */
    ret = switchServerPort(fd, enable);
    if(ret < 0){
        ALOGE("failed to connect to bt server");
        goto err_exit;
    }

    /* Alloc ilm message */
    ilm = (ilm_struct*)malloc(max_size);
    if(ilm == NULL){
        ALOGE("Can not alloc mem for ilm message");
        goto err_exit;
    }
    msg = (union_bttm_msgs*)ilm->ilm_data;
        
    if(enable){
        // TODO: send msg to switch server address
        ilm->msg_id = MSG_ID_BT_BTTM_SWITCH_PORT_REQ;
        msg->switch_req.bttm = KAL_TRUE;
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_BTTM_SWITCH_PORT_CNF, hd_size+sizeof(bt_bttm_switch_port_req_struct)) < 0){
            ALOGE("Switch server port failed");
            goto err_exit;
        }
        if(msg->switch_cnf.result == KAL_FALSE){
            ALOGE("Switch port failed");
            goto err_exit;
        }
        ret = switchServerPort(fd, 0);
        if(ret < 0){
            ALOGE("failed to connect to bt server");
            goto err_exit;
        }
        // TODO: MOVE after BT on
        //if(writeTmpAddr(addr) < 0)
        //    goto err_exit;
        // TODO: power on BT
        ilm->msg_id = MSG_ID_BT_POWERON_REQ;
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_POWERON_CNF, hd_size) < 0){
            ALOGE("Power on failed");
            goto err_exit;
        }
        if(msg->poweron_cnf.result == KAL_FALSE){
            ALOGE("Power on failed");
            goto err_exit;
        }
        // TODO: change BT address to specified address
        if(writeTmpAddr(addr) < 0)
            goto err_exit;
        // TODO: write scan mode
        ilm->msg_id = MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ;
        //msg->writescanmode_req.modenotconnected = 3;
        msg->writescanmode_req.mode = 3;
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF, hd_size+sizeof(bt_bm_write_scanenable_mode_req_struct)) < 0){
            ALOGE("send_req_to_bt()");
            goto err_exit;
        }
        if(msg->writescanmode_cnf.result != BTBM_ADP_SUCCESS){
            ALOGE("write scanable mode failed : %d", msg->writescanmode_cnf.result);
            goto err_exit;
        }        
        // TODO: send  BTTM enable command
        ilm->msg_id = MSG_ID_BT_TEST_MODE_REQ;
        msg->testmode_req.mode = 1;
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_TEST_MODE_CNF, hd_size+sizeof(bt_test_mode_req_struct)) < 0)
            goto err_exit;
    }else{
        // TODO: send  BTTM disable command
        /*
        ilm->msg_id = MSG_ID_BT_TEST_MODE_REQ;
        msg->testmode_req.mode = 2;
        if(send_req_to_bt(fd, ilm, 0, hd_size+sizeof(bt_test_mode_req_struct)) < 0)
            goto err_exit;
        */
        // TODO: power off BT
        ilm->msg_id = MSG_ID_BT_POWEROFF_REQ;
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_POWEROFF_CNF, hd_size) < 0)
            goto err_exit;
        // TODO: change BT address to original address
        if(writeTmpAddr(NULL) < 0)
            goto err_exit;        
        // TODO: send msg to switch server address
        ilm->msg_id = MSG_ID_BT_BTTM_SWITCH_PORT_REQ;
        msg->switch_req.bttm = KAL_FALSE;
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_BTTM_SWITCH_PORT_CNF, hd_size+sizeof(bt_bttm_switch_port_req_struct)) < 0)
            goto err_exit;
        if(msg->switch_cnf.result == KAL_FALSE){
            ALOGE("Switch back port failed");
            goto err_exit;
        }
    }
    ret = 1;
err_exit:
    if(ilm)
        free(ilm);
    if(fd >= 0)
        close(fd);
    return ret;
}

/* 0 : disabled, larger than 0 : enabled, smaller than 0 : failed */
int queryTestMode(){
#if __BTTM_USE_ORIGINAL_PORT__
    int fd = -1;
    int ret = -1;
    bt_test_mode_req_struct *req_p = NULL;
    bt_test_mode_cnf_struct *cnf_p = NULL;
    ilm_struct *ilm;
    int hd_size = (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE);
    int limsize = hd_size + max(sizeof(bt_test_mode_req_struct), sizeof(bt_test_mode_cnf_struct));

    fd = openSocket();
    if(fd < 0){
        return -1;
    }
    ret = socket_local_client_connect(fd, BT_SOCK_NAME_INT_ADP, ANDROID_SOCKET_NAMESPACE_FILESYSTEM, SOCK_DGRAM);
    if(ret < 0){
        ALOGE("Failed to connect to BT server socket");
        close(fd);
        return -1;
    }
    
    ilm = (ilm_struct*)malloc(limsize);
    if(ilm == NULL){
        ALOGE("Can not alloc mem for ilm message");
    }else{
        req_p = (bt_test_mode_req_struct*)ilm->ilm_data;
        cnf_p = (bt_test_mode_cnf_struct*)ilm->ilm_data;
        // TODO: send  BTTM enable command
        ilm->msg_id = MSG_ID_BT_TEST_MODE_REQ;
        req_p->mode = 0; /* 0 for query current test mode */
        if(send_req_to_bt(fd, ilm, MSG_ID_BT_TEST_MODE_CNF, hd_size+sizeof(bt_test_mode_req_struct)) < 0){
            ALOGE("Test command failed");
        }else{
            ret = cnf_p->result ? 1 : 0;
        }
        free(ilm);
    }
    close(fd);
    return ret;
#else
    int fd = socket_local_server(BT_SOCK_NAME_INT_ADP_BTTM,
                                    ANDROID_SOCKET_NAMESPACE_ABSTRACT,
                                    SOCK_DGRAM);
    ALOGD("queryTestMode fd=%d", fd);
    if(fd < 0){
        ALOGD("errno=%d, strerror=%s", errno, strerror(errno));
    }else{
        close(fd);
    }
    return (fd < 0) ? 1 : 0;
#endif
}

/* addr is 6 bytes binary address */
int writeBTAddr(unsigned char *addr){
    ALOGD("writeBTAddr(%p)", addr);
    if(addr){
        ALOGD("addr = %02X,%02X,%02X,%02X,%02X,%02X", 
            addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
#if 0 // defined(MTK_TC1_FEATURE)
		if( KAL_FALSE == TC1_FAC_NAME(FacWriteBtAddr(addr, 1)))
#else
		if (0) 
#endif
		{
            ALOGE("LGE_FacWriteBtAddr failed");
            return -1;
        }
        if( writeTmpAddr(addr) < 0 ){
            return -1;
        }
    }
    return 1;
}

/* addr is 6 bytes binary address */
int readBTAddr(unsigned char *addr){
    ALOGD("readBTAddr(%p)", addr);
    if(addr){
#if 0 // defined(MTK_TC1_FEATURE)
			return TC1_FAC_NAME(FacReadBtAddr(addr));
#else
			return -1;
#endif
    }else{
        ALOGE("addr is null");
    }
    return -1;
}



