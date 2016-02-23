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
 * eint.c
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
 * SH Lai
 ********************************************************************************/
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#if !defined(__MTK_BT_EXTERNAL_PLATFORM__)
#include <linux/serial.h> 
#endif
#include <pthread.h>

#define BTWLANEM_DEVNAME            "/dev/btwlan_em"
#define BTWLAN_EM_IOC_MAGIC       0xf6
#define BT_IOCTL_SET_EINT               _IOWR(BTWLAN_EM_IOC_MAGIC, 2, uint32_t)

static int g_eintFd = -1;
static int g_epollfd = -1;
/* return EINT handle */
int initEint(void)
{
    int  ret, flags;
    struct epoll_event  ev;

    bt_prompt_trace(MOD_BT, "[SM] initEint");    
    g_eintFd = open(BTWLANEM_DEVNAME, O_RDWR);

    if (g_eintFd >= 0)
    {
        flags = fcntl(g_eintFd, F_GETFL);
        fcntl(g_eintFd, F_SETFL, flags | O_NONBLOCK);
    }
    #if 0
    bt_prompt_trace(MOD_BT, "[SM] +epoll_create");
    g_epollfd = epoll_create(1);
    bt_prompt_trace(MOD_BT, "[SM] -epoll_create");    
    ev.events  = EPOLLIN|EPOLLET;
    ev.data.fd = g_eintFd;
    if(g_epollfd >= 0)
    {
        do {
            bt_prompt_trace(MOD_BT, "[SM] +epoll_ctl : g_epollfd=%d, g_eintFd=%d", g_epollfd, g_eintFd);
            ret = epoll_ctl( g_epollfd, EPOLL_CTL_ADD, g_eintFd, &ev );
            bt_prompt_trace(MOD_BT, "[SM] -epoll_ctl");
            if(ret < 0)
                bt_prompt_trace(MOD_BT,"[SM] epoll_ctl ret=%d, errno=%d, %s", ret, errno, strerror(errno));
        } while (ret < 0 && errno == EINTR);
    }
    else
    {
        bt_prompt_trace(MOD_BT,"[SM] epoll_create ret failed, errno=%d, %s", errno, strerror(errno));
    }
    bt_prompt_trace(MOD_BT,"[SM] initEint : g_epollfd=%d, g_eintFd=%d", g_epollfd, g_eintFd);
    #endif
    return g_eintFd;
}

void unmaskEint(void)
{
    unsigned int bt_eint = 1;
    bt_prompt_trace(MOD_BT,"[SM] unmaskEint");
    if(g_eintFd >= 0)
    {
        ioctl(g_eintFd, BT_IOCTL_SET_EINT, &bt_eint);
    }
}

void closeEint(void)
{
    int ret;
    bt_prompt_trace(MOD_BT,"[SM] closeEint : g_eintFd=%d", g_eintFd);

    if(g_epollfd>= 0)
    {
        do {
            ret = epoll_ctl( g_epollfd, EPOLL_CTL_DEL, g_eintFd, NULL );
        } while (ret < 0 && errno == EINTR);
        g_epollfd = -1;
    }
    
    if(g_eintFd >= 0)
    {
        close(g_eintFd);
        g_eintFd= -1;
    }
}

/* 0: disabled, 1: enabled */
int getEintHandle(void)
{
    //bt_prompt_trace(MOD_BT,"[SM] g_eintFd : %d", g_eintFd);
    return g_eintFd;
    //bt_prompt_trace(MOD_BT,"[SM] g_epollfd : %d", g_epollfd);
    //return g_epollfd;
}

