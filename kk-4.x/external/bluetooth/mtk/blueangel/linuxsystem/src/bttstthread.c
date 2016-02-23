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
 * BtTstThread.c
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
 * Autumn Li
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
#if !defined(__PLATFORM_SUPPORTED_CATCHER__)

#if defined(BTMTK_ON_LINUX)
#define LOG_TAG "MTKBT"
#endif

#include <stdio.h>
#include <assert.h>
#include "bt_types.h"
#include "bt_mmi.h"
#include "win_service.h"
#include "bt_message.h"
#include "bt_uart_log_drv.h"
#include "bt_adp_log.h"
#if defined(BTMTK_ON_LINUX)
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "cutils/sockets.h"
#endif

#include "bt_debug.h"
#include "tst_ringbuffer.h"

static pid_t tstpid = 0;
static pthread_t tstthread;
static pthread_cond_t tstcond;
kal_bool g_tstStarted = KAL_FALSE;
#define SERV_SOCK_NAME "btlog"

static int g_closePipe[2] = {-1,-1};
static int g_fd = -1;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif 

static int readData(int fd, void *dest, int size){
    int res = -1;
    U8 *ptr = dest;
    if(fd < 0 || ptr == NULL) return -1;
    while(size > 0){
        res = read(fd, ptr, size);
        if(res < 0){
            if(errno == EINTR){
                continue;
            }else{
                return res;
            }
        }else{
            size -= res;
            ptr += res;
        }
    }
    if(size == 0) return 0;
    else return res;
}

static int writeData(int fd, void *src, int size){
    U8 *ptr = src;
#if 0
    int res = -1;
    do { 
        res = write( fd, ptr, size );
        if(res < 0 ){
            if(errno != EINTR){
                bt_tst_err("write failed : errno=%d, %s", errno, strerror(errno));
                break;
            }
        }else{
            if(size != res)
            {
                bt_tst_dbg("write partial data size=%d, res=%d", size, res);
            }
            size -= res;
        }
    }while (size > 0);
    return res;
#else    
    int res = -1;
    if(fd < 0 || ptr == NULL){
        //bt_tst_err("fd=%d, ptr=%d", fd, ptr);
        return -1;
    }
    while(size > 0){
        res = write(fd, ptr, size);
        if(res < 0){
            if(errno == EINTR){
                continue;
            }else{
                bt_tst_err("write failed : errno=%d, %s", errno, strerror(errno));
                return res;
            }
        }else{
            size -= res;
            ptr += res;
            if(size != 0)
                bt_tst_err("size=%d, res=%d", size, res);
        }
    }
    if( res < 0 || size != 0)
        bt_tst_err("-writeData : res=%d, size=%d", res, size);
    if(size == 0) return 0;
    else return res;
#endif
}
 
static int listenConnect(int fd)
{
    fd_set readfs;
    int res;

    bt_tst_log("Start waiting connecting : %d", fd);
    if(fd)
    {
        while(1)
        {
            FD_ZERO(&readfs);
            FD_SET(fd, &readfs);
            if(g_closePipe[0] >= 0)
            {
                FD_SET(g_closePipe[0], &readfs);
            }
            res = select(max(fd, g_closePipe[0])+1, &readfs, NULL, NULL, NULL);
            if( res < 0 ){
                if(errno != EINTR){
                    bt_tst_err("[TST] listenConnect : select failed : errno=%d, %s", errno, strerror(errno));
                    break;
                }
            }else{
                if(FD_ISSET(g_closePipe[0], &readfs)){
                    bt_tst_dbg("close pipe is set, read pipe");
                    if( readData(g_closePipe[0], &res, sizeof(int)) < 0){
                        bt_tst_err("read pipe %d failed", g_closePipe[0]);
                    }else{
                        bt_tst_dbg("read pipe %d success : fd=%d", g_closePipe[0], res);
                        return res;
                    }
                }else{
                    res = accept(fd, NULL, NULL);
                    bt_tst_dbg("accept returns %d", res);
                    if(res < 0){
                        bt_tst_err("accept failed : errno=%d, %s", errno, strerror(errno));
                    }else {
                        return res;
                    }
                }
            }
        }
        return res;
    }
    else
    {
        return -1;
    }    
}

extern int g_tstWaitFlag;

void* BTMTK_Tst(void *lpParam) 
{ 
    int sockfd = -1;
    int res = -1;

    bt_tst_log("[TST] BT tst");
    sockfd = socket_local_server(SERV_SOCK_NAME, 
                                                   ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
                                                   SOCK_STREAM);
    bt_tst_log("-socket_local_server: %d,%d", sockfd, errno);
    if(sockfd < 0){
        bt_tst_err("[TST][ERR] create hfg server socket failed : %s, errno=%d", strerror(errno), errno);
        return NULL;
    }
    res = listen(sockfd, 5);
    bt_tst_log("listen : %d,%d",res, errno);
    if( res < 0)
    {
        bt_tst_err("[TST] listen failed : %s, errno=%d", strerror(errno), errno);
        return NULL;
    }    

wait_connect:
    g_fd = listenConnect(sockfd);
    if(g_fd < 0){
        goto exit;
    }
    bt_tst_log("[TST] Listen successfully, connected to socket %d",g_fd);

    while(1)
    {
        if( tstFlushBuffer() < 0)
            goto wait_connect;
        
        g_tstWaitFlag = 1;
        res = pthread_cond_wait(&tstcond, NULL);
        g_tstWaitFlag = 0;
        if(res != 0)
        {
            bt_tst_err("[TST] pthread_cond_wait failed %d", res);
        }
    }
exit:    
    bt_tst_err("[TST] Exit TST : %s, errno=%d", strerror(errno), errno);
    if(g_fd >= 0){
        close(g_fd);
        g_fd=-1;
    }
    if(sockfd >= 0){
        close(sockfd);
    }
    return 0;
} 
static void wakeupTst()
{
    if(g_tstStarted)
    {
        if(pthread_cond_signal(&tstcond))
            bt_tst_err("[TST] pthread_cond_signal failed");
    }
    else
    {
        bt_tst_log("[TST] g_tstStarted is NULL");
    }
}
//extern int g_tstSendFlag;
static int sendTstBuf(U8 *buf, size_t size)
{
    int res, wsize;
    if(g_fd < 0){
        bt_tst_err("[TST] g_fd is not valid");
        return -1;
    }

    #if 1
    bt_tst_log("+sendTstBuf(%d, %d)", buf, size);
    res = writeData(g_fd, buf, size);
    bt_tst_log("-sendTstBuf : %d", res);
    #else
    do { 
        res = write( g_fd, buf, size );
        if(res < 0 ){
            if(errno != EINTR){
                bt_tst_err("write failed : errno=%d, %s", errno, strerror(errno));
                break;
            }
        }else{
            if(size != res)
            {
                bt_tst_dbg("write partial data size=%d, res=%d", size, res);
            }
            size -= res;
        }
    }while (size > 0);
    #endif
    return res;
}

int startTst()
{
    int res = -1;

    bt_tst_log("[TST]  startTst");
    tst_rb_init(wakeupTst, sendTstBuf);    
    if( 0 != pthread_cond_init(&tstcond, NULL))
    {
        res = -1;
        bt_tst_err("[TST] pthread_cond_init failed");
    }
    else
    {
        bt_tst_log("[TST] pthread_cond_init success");
        if( 0 != pthread_create( &tstthread, NULL, BTMTK_Tst, NULL) )
        {
            res = -1;
            pthread_cond_destroy(&tstcond);
            bt_tst_err("[TST] pthread_create failed");
        }
        else
        {
            if(pipe(g_closePipe) == -1){
                bt_tst_err("[TST] create command pipe failed");
            }else {
                bt_tst_log("[TST] Command pipe created : (%d, %d)", g_closePipe[0], g_closePipe[1]);
            }
            g_tstStarted = KAL_TRUE;
            bt_tst_log("[TST] pthread_create success");
        }
    }
    return res;
}

/*****************************************/
/*  Usage : flush catcher logs                               */
/*  milisec : timeout in miliseconds                      */
/*  force : if not 0 and catcher can not be flush in */
/*             milisec, output log directly                   */
/*  return : 0 means success, -1 means failed      */
/*****************************************/
extern void dumpTstStatus(void);
int tst_flush(int milisec, int force){
    int res = -1;
    int count = 0;
    if(g_fd >= 0) {
        while(!IS_RB_EMPTY() && count < milisec){
            bt_tst_err("waiting for catcher log");
            usleep(10000);
            count += 10;
        }

        res = fsync(g_fd);
        if(res < 0){
            bt_tst_err("fsync failed : errno=%d, %s", errno, strerror(errno));
        }
            
        if(!IS_RB_EMPTY() && force != 0){
            bt_tst_err("force to output catcher log");
            if(tstFlushBuffer() < 0) {
                bt_tst_err("tst_flush failed\n");
                return -1;
            }
        }
        res = close(g_fd);
        g_fd = -1;
        if(res < 0) {
            bt_tst_err("close failed : errno=%d, %s", errno, strerror(errno));
        }
    }else{
        bt_tst_err("g_fd = %d", g_fd);
    }
    return 0;
}

/***************************************/
int logToFile(char *name){
    char filename[128];
    sprintf(filename, "%s_%ld", name, time(NULL));
    bt_tst_dbg("logToFile(%s)", name);
    
    if(g_closePipe[1] >= 0){
        int fd = open(filename, O_CREAT|O_WRONLY, S_IRWXU|S_IRWXG|S_IRWXO);
        if(fd >= 0){
            bt_tst_dbg("open file %s success", filename);
            if ( writeData(g_closePipe[1], &fd, sizeof(int)) == 0){
                return 0;
            }else{
                bt_tst_err("Write fd %d failed", fd);
                close(fd);
                return -1;
            }
        }else{
            bt_tst_err("open file %s failed. errno=%d, %s", filename, errno, strerror(errno));
        }
    }
    return -1;
}

#endif /* !defined(__PLATFORM_SUPPORTED_CATCHER__) */
