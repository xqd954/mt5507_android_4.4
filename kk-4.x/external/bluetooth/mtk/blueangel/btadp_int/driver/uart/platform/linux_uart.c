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

/***************************************************************************
 *
 * File:
 *     $Workfile:uart.c$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision: #1 $
 *
 * Description:
 *      HCI Transport - UART physical driver.
 *      This file provides the interface between the UART HCI Transport 
 *      Driver that is platform independent and the controller or driver
 *      for the controller.
 *
 *      This particular driver uses Microsoft's VCOMM driver for accessing
 *      the Bluetooth controller.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined (BTMTK_ON_LINUX)
#include "assert.h"
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

#include "bttypes.h"
#include "bt_types.h"
#include "bt_adp_fs.h"
#include "bt_debug.h"
#include "bt_feature.h"
#include "cust_bt.h"

extern int set_bluetooth_power(int on);
extern int set_speed(int fd, struct termios *ti, int speed);

#define MOD_BT 0
#if defined(__ANDROID_EMULATOR__)
#define CUST_BT_SERIAL_PORT "/dev/ttyS2"
#endif

typedef void (*UartCallback) (unsigned char event);

extern unsigned char BTCoreVerifySysInitState(void);

typedef struct {
    UartCallback cb_func;
}bt_adp_uart;

bt_adp_uart BTAdpUartCtx;

int openComm(void);

int commPort = -1;

/*************************************************
 *  Name : BTUartLinuxInit
 *  Parameter : 
 *    func : If there is Rx data, callback to stack to handle
 *  Return value : 
 *    BT_STATUS_SUCCESS : Success
 *    BT_STATUS_FAILED : Failed
 *************************************************/
BtStatus BTUartLinuxInit(UartCallback func)
{
    // Checking if the uart is already opened
    // if com port is opened, let opencomm to close then open again
    /*
    if (commPort >= 0)
    {
        bt_android_log("[UART][ERR] the uart already opened : %d", commPort);
        bt_prompt_trace(MOD_BT, "[UART][ERR] the uart already opened : %d", commPort);
        return BT_STATUS_SUCCESS;
    }
    */
   // Create this write operation's OVERLAPPED structure's hEvent.
    BTAdpUartCtx.cb_func = func;
    //Power on BT before open uart
    set_bluetooth_power(0);
    /* Power on controller */
    if( set_bluetooth_power(1) < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] failed to set bt power");
        return BT_STATUS_FAILED;
    }

    if ( openComm() != 0 ){
        set_bluetooth_power(0);
        return BT_STATUS_FAILED;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
}

unsigned char BTUartLinuxShutdown(void)
{
    bt_prompt_trace(MOD_BT, "[UART] Close UART port : %d", commPort);
    if(commPort >= 0)
    {
        bt_android_log("[UART] Close UART port : %d", commPort);
        if( close(commPort) < 0 ) {
            bt_prompt_trace(MOD_BT, "[UART] Close UART port failed : %d, %s", errno, strerror(errno));
            bt_android_log("[UART][ERR] Close UART port failed : %d, %s", errno, strerror(errno));
        }
        commPort = -1;
    }
    return 0x00;
}

void BTUartTriggerReadUart(void)
{
    if(BTCoreVerifySysInitState() == 1)
        BTAdpUartCtx.cb_func(0x01);
}

static void waitWholeChipResetComplete(){
    int res = 0;
    char tmp;
    /*
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(commPort, &readfs);
    do{
        res = select(commPort+1,&readfs, NULL, NULL, NULL);
    }while((res < 0 && errno == EAGAIN));
    */
    do{
        res = read(commPort, &tmp, 1);
        if( res < 0 ){
            if( errno == 99 ){
                bt_prompt_trace(MOD_BT, "[RESET] 99 (WholeChipReset completed)");
                return;
            }else if(errno == 88){
                bt_prompt_trace(MOD_BT, "[RESET] 88 usleep(1000)");
                usleep(10000);
            }else if(errno != EAGAIN){
                break;
            }
        }else{
            break;
        }
    }while(1);
    bt_prompt_trace(MOD_BT, "[RESET] 99 (WholeChipReset completed) not waited : %d : errno=%d", res, errno);
}

static void wholeChipReset(int err)
{
  bt_prompt_trace(MOD_BT, "[RESET] wholeChipReset : errno=%d", errno);
  Assert(0);
  if(err == 88){
		waitWholeChipResetComplete();
  }
	close(commPort);
	commPort = -1;
}

/* static char packetbuf[1024]; */
unsigned short BTUartLinuxRead(unsigned char *buffer, unsigned short length)
{
    unsigned short bytesToCopy = 0;
    int nRead = 0;

    bytesToCopy = length;
    bt_prompt_trace_lv3(MOD_BT, "[BT] Read uart buffer=0x%x, length=%d, commport=%d", buffer, length, commPort);

    if(commPort < 0)
    {
        bt_prompt_trace(MOD_BT, "Read com after closed");
        return 0;
    }

    while(bytesToCopy && (nRead = read(commPort, buffer, bytesToCopy)) > 0)
    {
        bt_prompt_trace_lv3(MOD_BT, "[BT] %d bytes is read from uart", nRead);
        bytesToCopy -= nRead;
        buffer += nRead;
    }
    if(nRead < 0){
        //bt_prompt_trace(MOD_BT, "[RESET] BTUartLinuxRead return %d, errno=%d", nRead, errno);
        if(errno == 88 || errno == 99){
        	wholeChipReset(errno);
        	return 0;
        }
    }
    bt_prompt_trace_lv3(MOD_BT, "[BT] total %d bytes is read", length - bytesToCopy);
	
    return (length - bytesToCopy);
} 

/*static char packetbuf[1024];*/
unsigned short BTUartLinuxWrite(const unsigned char *buffer, unsigned short length)
{
    int nWritten = 0;
    int bytesToWrite;
    U16 i;
    unsigned char *ptr;


    bytesToWrite = length;

    /*
    ptr = packetbuf;
    for(i = 0;i < length;i++)
    {
        sprintf(ptr, "%02X ", buffer[i]);
        ptr+=3;
    }
    //bt_prompt_trace(MOD_BT, "[BT] Write %d bytes to uart", length);
    bt_prompt_trace(MOD_BT, "[BT] Write to uart %d : \"%s\"", commPort, packetbuf);
    */

    if(commPort < 0)
    {
        bt_prompt_trace(MOD_BT, "Write com after closed");
        return 0;
    }

	
    while(bytesToWrite > 0)
    {
        bt_prompt_trace_lv3(MOD_BT, "[BT] Start write to uart buffer=0x%x, bytesToWrite=%d", buffer, bytesToWrite);
        nWritten = write(commPort, buffer, bytesToWrite);
        if (nWritten > 0)
        {
            bt_prompt_trace_lv3(MOD_BT, "[BT][UART] %d bytes are written to uart", nWritten);
            bytesToWrite -= nWritten;
            buffer += nWritten;
        }
        else
        {
            bt_prompt_trace(MOD_BT, "[UART] Write uart failed : nWritten=%d, err=%s, errno=%d", nWritten, strerror(errno), errno);
            break;
        }
    }
    if(nWritten < 0){
        //bt_prompt_trace(MOD_BT, "[RESET] BTUartLinuxWrite return %d, errno=%d", nWritten, errno);
        if(errno == 88 || errno == 99){
        	wholeChipReset(errno);
        	return 0;
        }
    }
    bt_prompt_trace_lv3(MOD_BT, "[BT][UART] total wrote %d bytes to uart", length - bytesToWrite);
    return (length - bytesToWrite);
} 

void BTUartLinuxSetSpeed(unsigned short speed)
{
    struct termios options;
    bt_prompt_trace(MOD_BT, "[BT][UART] set uart speed to speed %d", speed);
    return;
    /* Get default com port setting */
    if( -1 == tcgetattr(commPort, &options) )
    {
        return;
    }

    /* Set baud rate to 115200 */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    /* Set new com port setting */
    if( -1 == tcsetattr(commPort, TCSANOW, &options) )
    {
        return;
    }
    return;
}


int readUartSetting()
{
    int file_handle;
    int port;
    file_handle = btmtk_fs_open((const U8*)"\\uart.ini", BTMTK_FS_READ_ONLY);

    if (file_handle < 0)   /* first time, create the file */
    {
        char *str = "[BT][UART]COM_PORT = COM4\n";
        file_handle = btmtk_fs_open((const U8*)"\\uart.ini", BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        btmtk_fs_write(file_handle, (U8*)str, strlen(str));
        btmtk_fs_close(file_handle);
    }
    else
    {
        char str[20] = {0};
        btmtk_fs_read(file_handle, (U8*)str, 20);
        sscanf(str, "COM_PORT = COM%d", &port);
        btmtk_fs_close(file_handle);
    }
    return port;
}

int openComm(void)
{
#if 1
    struct termios ti;
    char *device = CUST_BT_SERIAL_PORT;

    bt_prompt_trace(MOD_BT, "[UART] openComm");
    bt_android_log("[UART] openComm");
    if (commPort >= 0)  //modified by autumn
    {
        bt_prompt_trace(MOD_BT, "[UART] openComm already opened");
        bt_android_log("[UART][ERR] commPort=%d already opened.", commPort);
        bt_android_log("[BT][UART] + close uart=%d", commPort);
        close(commPort);
        bt_android_log("[BT][UART] - close uart");
    }
    bt_android_log("[BT][UART] + open uart");
    commPort = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);    // Modified by SH
    bt_android_log("[BT][UART] - open uart");
    if (commPort < 0) {
        bt_prompt_trace(MOD_BT, "[UART]Can't open serial port: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Can't open serial port: %s (%d)\n", strerror(errno), errno);
        return -1;
    }

#ifndef __MTK_STP_SUPPORTED__
    //fcntl(commPort, F_SETFL, FASYNC); // Added by SH
    tcflush(commPort, TCIOFLUSH);

    if (tcgetattr(commPort, &ti) < 0) {
        bt_prompt_trace(MOD_BT, "[UART]Can't get port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Can't get port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Close commPort");
        close(commPort);
        commPort = -1;
        return -1;
    }

    cfmakeraw(&ti);

    ti.c_cflag |= CLOCAL;
    ti.c_cflag &= ~CRTSCTS;
    #if !defined(__ANDROID_EMULATOR__)
    /* Emulator does not support flow control */
    ti.c_iflag &= ~(IXON | IXOFF | IXANY | 0x80000000);
    #endif
	      
    if (tcsetattr(commPort, TCSANOW, &ti) < 0) {
        bt_prompt_trace(MOD_BT, "[UART] Can't change port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Can't change port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Close commPort");
        close(commPort);
        commPort = -1;
        return -1;
    }
        
    if( set_speed(commPort, &ti, 115200) < 0){
        bt_android_log("[UART][ERR] set_speed failed");
        close(commPort);
        commPort = -1;
        return -1;
    }

    tcflush(commPort, TCIOFLUSH);
#endif /* ifndef __MTK_STP_SUPPORTED__ */
    bt_prompt_trace(MOD_BT, "[UART] Opening UART successfully : %d", commPort);
    bt_android_log("[UART] Opening UART successfully : %d", commPort);
    return 0;
#else
    
    char portName[20];
    
    sprintf(portName, "/dev/ttyS%d", 2/*readUartSetting()*/);

    bt_prompt_trace(MOD_BT, "Opening UART port : %s", portName);
    
    /* Open UART port */
    if (-1 == (commPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) ) 
    {
        bt_prompt_trace(MOD_BT, "[Failed] Opening UART port ");
        return 0;
    } 
    else
    {
        struct termios options;

        //fcntl(commPort, F_SETOWN, getpid());
        fcntl(commPort, F_SETFL, FASYNC);

        /* Get default com port setting */
        if( -1 == tcgetattr(commPort, &options) )
        {
            bt_prompt_trace(MOD_BT, "[Failed] tcgetattr UART port failed");
            close(commPort);
            return 0;
        }

        /* Set baud rate to 115200 */
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);

        options.c_cflag |= (CLOCAL | CREAD);

        options.c_cflag &= ~PARENB;  /* No parity */
        options.c_cflag &= ~CSTOPB;  /* 1 stop bit */
        options.c_cflag &= ~CSIZE;   /* Mask the character size bits */
        options.c_cflag |= CS8;         /* Select 8 data bits */
        /* Disable RTS & CTS */
        options.c_cflag &= ~CRTSCTS;

        tcflush(commPort, TCIFLUSH);
        /* Set new com port setting */

        if( -1 == tcsetattr(commPort, TCSANOW, &options) )
        {
            bt_prompt_trace(MOD_BT, "[Failed] tcsetattr UART port failed");
            close(commPort);
            return 0;
        }

        return 1;
    }
#endif
} 
#endif  /* #if defined (BTMTK_ON_LINUX) */


