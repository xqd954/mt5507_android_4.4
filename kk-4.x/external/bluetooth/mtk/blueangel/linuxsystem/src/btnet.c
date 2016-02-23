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

/*******************************************************************************
 *
 * Filename:
 * ---------
 * btnet.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to communicate with bt network device
 *
 * Author:
 * -------
 * Ting Zheng
 ********************************************************************************/
#ifdef __ENABLE_BTNET_DEV__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/if_tun.h>
#include <sys/endian.h>

#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_log.h"

static int g_btnfd = -1;
static U8 g_ifname[IFNAMSIZ] = {0};

#define BTNETWORK_DEVNAME			"/dev/btn"
#define BTNETWORK_DEV_IPADDR			0xc0a80501	// 192.168.5.1
#define BTNETWORK_DEV_NETMASK		0xffffff00	//255.255.255.0

#define SIN_ADDR(x)	(((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)
#define SIN_FAMILY(x)	(((struct sockaddr_in *) (&(x)))->sin_family)

#ifdef CONFIG_BTNET_DEV
#undef CONFIG_BTNET_DEV
#endif

#if 0
void closeBtNet(void);
static S32 configBtNetDev(int unit);


int openBtNet(U8 *hw_addr)
{
    int unit = -1;
    char ipcfg[60] = {0};

    if (g_btnfd >= 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]btn device is opened already");    
        return g_btnunit;
    }
    
    bt_prompt_trace(MOD_BT, "[BTN]initBtNet");    

    g_btnfd = open(BTNETWORK_DEVNAME, O_RDWR);
  
    if (g_btnfd < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet open btn device failed, %s(%d)", strerror(errno), errno);   
        return -1;
    }
   
    ioctl(g_btnfd, BTNIOCSETMAC, hw_addr);	
    ioctl(g_btnfd, BTNIOCNEWUNIT_ETH, &unit);	
    bt_prompt_trace(MOD_BT, "[BTN]initBtNet pan fd: %d, unit index: %d", g_btnfd, unit);    

#ifdef CONFIG_BTNET_DEV
    //config ip addr
    if (unit >= 0)
    {
        if (configBtNetDev(unit) < 0)
        {
            closeBtNet();
            return -1;			
        }
    }
#endif

    g_btnunit = unit;
	
    return g_btnunit;    
}

static S32 configBtNetDev(int unit)
{
    struct ifreq ifr;
    int fd;	
    int ret = -1;	

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd <= 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet create socket failed,  %s(%d)",  fd, strerror(errno), errno);    
        return -1;
    }
    
    memset(&ifr, 0, sizeof(ifr));
    sprintf(ifr.ifr_name, "btn%d", unit);	


    SIN_FAMILY(ifr.ifr_addr) = AF_INET;
    SIN_ADDR(ifr.ifr_addr) = htonl(BTNETWORK_DEV_IPADDR);
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet set ip address failed,  %s(%d)",  strerror(errno), errno);    
        goto exit;
    }

    ifr.ifr_flags = IFF_UP;
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet up interface failed,  %s(%d)",  strerror(errno), errno);    
        goto exit;
    }


    SIN_FAMILY(ifr.ifr_netmask) = AF_INET;
    SIN_ADDR(ifr.ifr_netmask) = htonl(BTNETWORK_DEV_NETMASK);
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet set netmask failed,  %s(%d)",  strerror(errno), errno);    
        goto exit;
    }

    ret = 0;

exit:
    close(fd);
    return ret;	
}

#endif

static int tapIfDown(const char *if_name)
{
    struct ifreq ifr;
    int sk, err;

    sk = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

    ifr.ifr_flags &= ~IFF_UP;

    err = ioctl(sk, SIOCSIFFLAGS, (caddr_t) &ifr);
    close(sk);

    return 0;

}

void closeBtNet(void)
{
    /* unnecessary to use ioctl to close network unit, since it shall be destroyed with the file accordingly */
    tapIfDown((const char *) g_ifname);
    close(g_btnfd);
    g_btnfd = -1;
}

int getBtNetHandle(void)
{
    return g_btnfd;
}

static int tapIfUp(U8 *hw_addr, U8 *if_name)
{
    struct ifreq ifr;
    int sk, err;

    sk = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (const char *) if_name, IFNAMSIZ);
    err = ioctl(sk, SIOCGIFHWADDR, &ifr);
    bt_prompt_trace(MOD_BT, "[BT_TAP] get address(%d)", err);
    if( err < 0 )
    {
        bt_prompt_trace(MOD_BT, "[BT_TAP][ERR] get address:%s(%d)", strerror(errno), errno);
        close(sk);
        return -1;
    }
    
    memcpy(ifr.ifr_hwaddr.sa_data, hw_addr, 6);
    err = ioctl(sk, SIOCSIFHWADDR, (caddr_t)&ifr);
    bt_prompt_trace(MOD_BT, "[BT_TAP] set address(%d)", err);
    if( err < 0 )
    {
        bt_prompt_trace(MOD_BT, "[BT_TAP][ERR] set address:%s(%d)", strerror(errno), errno);
        close(sk);
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char const*)if_name, IFNAMSIZ);
    
    ifr.ifr_flags |= IFF_UP;
    ifr.ifr_flags |= IFF_MULTICAST;

    err = ioctl(sk, SIOCSIFFLAGS, (caddr_t) &ifr);    
    if (err < 0) 
    {
        bt_prompt_trace(MOD_BT, "[BT_TAP][ERR] Could not bring up network interface:%s, error: %s(%d)", if_name, strerror(errno), errno);
        close(sk);
        return -1;
    }

    close(sk);
    bt_prompt_trace(MOD_BT, "[BT_TAP] network interface: %s is up", if_name);
    return 0;
}

int openBtNet(U8 *hw_addr, U8 *if_name)
{
    struct ifreq ifr;
    int err;
    const char *clonedev = "/dev/tun";

    /* open the clone device */
    if (g_btnfd >= 0)
    {
        bt_prompt_trace(MOD_BT, "[BT_TAP] bt-tap device is opened already");    
        return g_btnfd;
    }

    bt_prompt_trace(MOD_BT, "[BT_TAP]initTapBtNet");  

    if( (g_btnfd = open(clonedev, O_RDWR)) < 0 ) {

        bt_prompt_trace(MOD_BT, "[BT_TAP] could not open %s, err:%s(%d)", clonedev, strerror(errno), errno);
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    strncpy(ifr.ifr_name, (char const*)if_name, IFNAMSIZ);

    memset(&g_ifname, 0, IFNAMSIZ);
    strncpy((char *) g_ifname, (char const*)if_name, IFNAMSIZ);

    /* try to create the device */
    if( (err = ioctl(g_btnfd, TUNSETIFF, (void *) &ifr)) < 0 )
    {
        bt_prompt_trace(MOD_BT, "[BT_TAP] set iff error:%s(%d)", strerror(errno), errno);
        closeBtNet();
        return -1;
    }

    if( tapIfUp(hw_addr, if_name) != 0 )
    {
        closeBtNet();
        return -1;
    }

    bt_prompt_trace(MOD_BT, "[BT_TAP] initTapBtNet pan fd: %d", g_btnfd);

    return g_btnfd;
}

#endif
