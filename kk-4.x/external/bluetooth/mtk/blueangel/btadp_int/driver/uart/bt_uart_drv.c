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
 *     $Workfile:uart.c$ for iAnywhere Blue SDK, Version 2.1.1
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
 * Portions copyright 2005 iAnywhere Solutions, Inc.
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
/***************************************************************************** 
* Include
*****************************************************************************/
/* basic system service headers */
#include "bt_common.h"
#include "uart.h"
#ifdef BTMTK_ON_WISE 
#include "platform/uart_maui.h"
#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include "platform/uart_win.h"
#elif defined (BTMTK_ON_LINUX)
#include "platform/uart_linux.h"
#include <unistd.h>
#endif
/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Global Data
 *
 ***************************************************************************/
#ifdef __BT_TRANSPORT_DRV__
int commPort = -1;
static UartCallback s_callback = NULL;
#endif /* __BT_TRANSPORT_DRV__ */

/****************************************************************************
 *
 * Local Prototypes
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Public Functions
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Extern Functions
 *
 ***************************************************************************/
#ifdef __BT_TRANSPORT_DRV__
extern int mtk_bt_enable(int arg, void *func_cb);
extern int mtk_bt_disable(int bt_fd);
extern int mtk_bt_write(int bt_fd, const unsigned char *buffer, unsigned long len);
extern int mtk_bt_read(int bt_fd, unsigned char *buffer, unsigned long len);
extern unsigned char BTCoreVerifySysInitState(void);

void BTUartTriggerReadUart(void)
{
    if(BTCoreVerifySysInitState() == 1 && s_callback != NULL){
        s_callback(0x01);
    }
}
static void wholeChipReset(int err)
{
    bt_prompt_trace(MOD_BT, "[RESET] wholeChipReset : err=%d", err);
    //Assert(0);
    bt_assert_reset("wholeChipReset", __FILE__, __LINE__);
    mtk_bt_disable(commPort);
    commPort = -1;
    s_callback == NULL;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  UART_Init
 * DESCRIPTION
 *  Called by the stack (during EVM_Init) to intialize the UART hardware
 *  driver.
 * PARAMETERS
 *  func        [IN]        
 * RETURNS
 *  BT_STATUS_PENDING - Initialization is pending.
 *  BT_STATUS_FAILED - Initialization failed.
 *****************************************************************************/
BtStatus UART_Init(UartCallback func)
{
    BtStatus status;
    
    bt_android_log("[BT] + UART_Init(%p)", func);
#if defined (BTMTK_ON_WISE)
    status = BTUartMauiInit(func);
#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
    status = BTUartWinInit(func);
#elif defined (BTMTK_ON_LINUX)
#ifdef __BT_TRANSPORT_DRV__
        bt_android_log("[BT] + mtk_bt_enable");
        commPort = mtk_bt_enable(0, NULL);
        bt_android_log("[BT] - mtk_bt_enable: ret=%d", commPort);
        if(commPort < 0){
            bt_prompt_trace(MOD_BT, "[UART][ERR] mtk_bt_enable : %d", commPort);
            //Assert(commPort >= 0);
            bt_fw_assert(BT_PANIC_POWER_ON_FAIL);
            return BT_STATUS_FAILED;
        }else{
            s_callback = func;
            return BT_STATUS_SUCCESS;
        }
#else /* __BT_TRANSPORT_DRV__ */
    status = BTUartLinuxInit(func);
#endif /* __BT_TRANSPORT_DRV__ */
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
    bt_android_log("[BT] - UART_Init : ret=%d", status);
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  UART_Shutdown
 * DESCRIPTION
 *  Release any resources, close the connection if open
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus UART_Shutdown(void)
{
    BtStatus status;
    
#if defined (BTMTK_ON_WISE)
    status = BTUartMauiShutdown();
#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
    status = BTUartWinShutdown();
#elif defined (BTMTK_ON_LINUX)
#ifdef __BT_TRANSPORT_DRV__
    if(commPort >= 0){
        int ret;
        bt_android_log("[BT] + mtk_bt_disable");
        ret = mtk_bt_disable(commPort);
        bt_android_log("[BT] + mtk_bt_disable : ret=%d", ret);
        if( ret < 0 ){
            bt_prompt_trace(MOD_BT, "[UART][ERR] mtk_bt_disable: %d", ret);
        }
        commPort = -1;
        s_callback = NULL;
    }
    status = BT_STATUS_SUCCESS;
#else /* __BT_TRANSPORT_DRV__ */
    status = BTUartLinuxShutdown();
#endif /* __BT_TRANSPORT_DRV__ */
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  UART_Read
 * DESCRIPTION
 *  Read bytes from the comm port
 * PARAMETERS
 *  buffer      allocated buffer to receive the bytes
 *  length      max number of bytes to read        
 * RETURNS
 *  number of bytes read
 *****************************************************************************/
U16 UART_Read(U8 *buffer, U16 length)
{
    U16 readByte = 0;
    
#if defined (BTMTK_ON_WISE)
    readByte = BTUartMauiRead(buffer, length);
#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
    readByte = BTUartWinRead(buffer, length);
#elif defined (BTMTK_ON_LINUX)
#ifdef __BT_TRANSPORT_DRV__
    if(commPort >= 0){
        int ret = mtk_bt_read(commPort, buffer, length);
        if(ret < 0){
            if(ret == -99){
                wholeChipReset(ret);
            }else{
                bt_prompt_trace(MOD_BT, "[UART][ERR] mtk_bt_read : %d", ret);
            }
        }else{
            readByte = (U16)ret;
        }
    }
#else /* __BT_TRANSPORT_DRV__ */
    readByte = BTUartLinuxRead(buffer, length);
#endif /* __BT_TRANSPORT_DRV__ */
//To log message for debug purpose
    if ( 0 < readByte)
    {
        char string[256];
        int i = 0;
        int log_len = readByte;
        memset(string, 0, 256);

        bt_prompt_trace(MOD_BT, "[BT]GetByte: len=%d", log_len);
        if ( 20 < log_len)
    	 {
             log_len = 20;
        }		
        for(i=0; i<log_len; i++)
        {
            sprintf(string, "%s, %x", string, *(buffer+i));
        }		
        bt_prompt_trace(MOD_BT, "[BT] %s", string);
    }
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
    return readByte;
}

/*****************************************************************************
 * FUNCTION
 *  UART_Write
 * DESCRIPTION
 *  Write a buffer to the comm port
 * PARAMETERS
 *  buffer      data to send        
 *  length      length  bytes in buffer        
 * RETURNS
 *  Number of bytes written. If an error occurs, 0 bytes are returned.
 *****************************************************************************/
U16 UART_Write(const U8 *buffer, U16 length)
{
    U16 bytesWritten = 0;

//    DisplayHex("UART write\n",buffer, length);
#if defined (BTMTK_ON_WISE)
    bytesWritten = BTUartMauiWrite(buffer, length);
#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
    bytesWritten = BTUartWinWrite(buffer, length);
#elif defined (BTMTK_ON_LINUX)
#ifdef __BT_TRANSPORT_DRV__
        {
            int ret;
            int count = 0;
            while(bytesWritten < length){
                ret = mtk_bt_write(commPort, &buffer[bytesWritten], (length-bytesWritten));
                if(ret < 0){
                    if(ret == -99){
                        wholeChipReset(ret);
                    }else{
                        bt_prompt_trace(MOD_BT, "[UART][ERR] mtk_bt_write : %d", ret);
                    }
                    break;
                }else{
                    bytesWritten += (U16)ret;
                    if(count++ > 0){
                        OS_Report("[UART] retry mtk_bt_write : written=%u, length=%u", bytesWritten, length);
                    }
                }
            }
        }
#else /* __BT_TRANSPORT_DRV__ */
    bytesWritten = BTUartLinuxWrite(buffer, length);
#endif
    /* +VSNIFF */
    if(p_sniffwrite){
        p_sniffwrite(buffer, length);
    }
    /* -VSNIFF */
//To log message for debug purpose
    if ( 0 < bytesWritten )
    {
        char string[256];
        int i = 0;
        int log_len = bytesWritten;
        memset(string, 0, 256);

        bt_prompt_trace(MOD_BT, "[BT]PutByte: len=%d", log_len);
		
        if (20 < log_len)
        {
            log_len = 20;
        }
        for(i=0; i<log_len; i++)
        {
            sprintf(string, "%s, %x", string, *(buffer + i));
        }			

        bt_prompt_trace(MOD_BT, "[BT] %s", string);
    }
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
    return bytesWritten;
}


/*****************************************************************************
 * FUNCTION
 *  UART_SetFlowControl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ena_flowctrl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void UART_SetFlowControl(U8 ena_flowctrl)
{
#ifdef BTMTK_ON_WISE
    BtUartMauiSetFlowControl(ena_flowctrl);
#endif  /* BTMTK_ON_WISE */    
}


/*****************************************************************************
 * FUNCTION
 *  UART_SetSpeed
 * DESCRIPTION
 *  Called to change the speed of the UART.
 * PARAMETERS
 *  speed       speed - the speed (in KBps) at which the UART should operate (57 or 115)        
 * RETURNS
 *  void
 *****************************************************************************/
void UART_SetSpeed(U16 speed)
{
#if defined (BTMTK_ON_WISE)
    BTUartMauiSetSpeed(speed);
#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
    BTUartWinSetSpeed(speed);
#elif defined (BTMTK_ON_LINUX)
#ifdef __BT_TRANSPORT_DRV__
#else /* __BT_TRANSPORT_DRV__ */
    BTUartLinuxSetSpeed(speed);
#endif /* __BT_TRANSPORT_DRV__ */
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
}
