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
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include "windows.h"
#include "assert.h"
#include <stdio.h>

#include "bt_types.h"
#include "bt_adp_fs.h"

#define READ_TIMEOUT      2 

#define MTK_USB_MAX_BUFFER_SIZE (4096*4)
#define MTK_USB_MAX_READ_BUFFER_SIZE (1024 *4)


unsigned char MTK_USB_BUFFER[MTK_USB_MAX_BUFFER_SIZE];
unsigned char MTK_USB_READ_BUFFER[MTK_USB_MAX_READ_BUFFER_SIZE];

unsigned long MTK_USB_RX_START = 0;
unsigned long MTK_USB_RX_END = 0;
unsigned long MTK_USB_RX_BUFFER_SIZE = 0;



typedef void (*UartCallback) (unsigned char event);

extern unsigned char BTCoreVerifySysInitState(void);

typedef struct {
    UartCallback cb_func;
}bt_adp_uart;

bt_adp_uart BTAdpUartCtx;



BOOL openComm(void);
BOOL setupConnection(unsigned short speed);

BOOL usb_port_used = FALSE;
HANDLE       commPort = NULL;
OVERLAPPED osWrite = {0};
OVERLAPPED osReader = {0};
BOOL fWaitingOnRead = FALSE;
unsigned char uartRxBuffer[1024];

unsigned char BTUartWinInit(UartCallback func)
{
   // Create this write operation's OVERLAPPED structure's hEvent.
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osWrite.hEvent == NULL)
        return 0x00;
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osReader.hEvent == NULL)
      return 0x00;
    fWaitingOnRead = FALSE;
    BTAdpUartCtx.cb_func = func;
    openComm();
	MTK_USB_RX_START = 0;
	MTK_USB_RX_END = 0;
	MTK_USB_RX_BUFFER_SIZE = 0;
    return 0x00;
}


unsigned char BTUartWinShutdown(void)
{
    PurgeComm(commPort, PURGE_RXABORT);        
    PurgeComm(commPort, PURGE_TXABORT);        
    PurgeComm(commPort, PURGE_RXCLEAR);        
    PurgeComm(commPort, PURGE_TXCLEAR);  
    fWaitingOnRead = FALSE;
    CloseHandle(osWrite.hEvent);
    CloseHandle(osReader.hEvent);   
    CloseHandle(commPort);
    commPort = 0;
    return 0x00;
}

void BTUartTriggerReadUart(void)
{
    if(BTCoreVerifySysInitState() == 1)
        BTAdpUartCtx.cb_func(0x01);
}

unsigned short BTUartWinRead(unsigned char *buffer, unsigned short length)
{
    unsigned short bytesToCopy = 0;
    DWORD dwRead = 0;
    DWORD dwRes = 0;

	if(usb_port_used == 1)
	{
		if(fWaitingOnRead != TRUE)
		{
			if (!ReadFile(commPort, MTK_USB_READ_BUFFER, MTK_USB_MAX_READ_BUFFER_SIZE, &dwRead, &osReader)) 
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					goto check_buffer;
					// Error in communications; report it.
				}
				else
				{
					dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
					switch(dwRes)
					{
						case WAIT_OBJECT_0:
							GetOverlappedResult(commPort, &osReader, &dwRead, FALSE);
							fWaitingOnRead = FALSE;
							break;
						case WAIT_TIMEOUT:
							fWaitingOnRead = TRUE;
							if(dwRead > 0x00)
								assert(0);
							break;
						default:
							dwRead = 0;
							break;
					}
				}
			}
		}
		else
		{
			dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
			switch(dwRes)
			{
				case WAIT_OBJECT_0:
					GetOverlappedResult(commPort, &osReader, &dwRead, FALSE);
					fWaitingOnRead = FALSE;
					break;
				case WAIT_TIMEOUT:
					fWaitingOnRead = TRUE;
					if(dwRead > 0x00)
						assert(0);
					break;
				default:
					dwRead = 0;
					break;
			}
		
		}
		if(dwRead > 0)
		{
			if ((MTK_USB_RX_BUFFER_SIZE + dwRead) > MTK_USB_MAX_BUFFER_SIZE)
			{
				assert(0);
			}
			if((MTK_USB_RX_END + dwRead) > MTK_USB_MAX_BUFFER_SIZE)
			{
				memcpy(MTK_USB_BUFFER+MTK_USB_RX_END,MTK_USB_READ_BUFFER,
					MTK_USB_MAX_BUFFER_SIZE - MTK_USB_RX_END);
				memcpy(MTK_USB_BUFFER,MTK_USB_READ_BUFFER + (MTK_USB_MAX_BUFFER_SIZE - MTK_USB_RX_END),
					dwRead - (MTK_USB_MAX_BUFFER_SIZE - MTK_USB_RX_END));
				MTK_USB_RX_END = dwRead - (MTK_USB_MAX_BUFFER_SIZE - MTK_USB_RX_END);
			}
			else
			{
				memcpy(MTK_USB_BUFFER+MTK_USB_RX_END,MTK_USB_READ_BUFFER,dwRead);
				MTK_USB_RX_END += dwRead;
			}
            if(MTK_USB_RX_END == MTK_USB_MAX_BUFFER_SIZE)
                MTK_USB_RX_END = 0;
            MTK_USB_RX_BUFFER_SIZE += dwRead;
		}
	check_buffer:
        if(MTK_USB_RX_BUFFER_SIZE !=0)
        {
            if(length >= MTK_USB_RX_BUFFER_SIZE)
            {
                bytesToCopy = (unsigned short)MTK_USB_RX_BUFFER_SIZE;
            }
            else
            {
                bytesToCopy = length;
            }
            if(MTK_USB_RX_START > MTK_USB_RX_END)
            {
                unsigned long copy_remain_length = 0;
                if(bytesToCopy !=0)
                {
                    copy_remain_length = MTK_USB_MAX_BUFFER_SIZE - MTK_USB_RX_START;
                    if(copy_remain_length >= bytesToCopy) 
                    {
                        memcpy(buffer, MTK_USB_BUFFER + MTK_USB_RX_START, bytesToCopy);   
                        MTK_USB_RX_START += bytesToCopy;
                    }
                    else
                    {
                        /* Copy data larger than copy_offset */
                        memcpy(buffer, MTK_USB_BUFFER + MTK_USB_RX_START, copy_remain_length);   
                        memcpy(buffer + copy_remain_length, MTK_USB_BUFFER , bytesToCopy - copy_remain_length);   
                        MTK_USB_RX_START = bytesToCopy - copy_remain_length;
                    }
                    if(MTK_USB_RX_START == MTK_USB_MAX_BUFFER_SIZE)
                        MTK_USB_RX_START = 0;

                }
                MTK_USB_RX_BUFFER_SIZE = MTK_USB_RX_BUFFER_SIZE - bytesToCopy;
            }
            else
            {
                /* End > Start*/
                unsigned long copy_offset = 0;

                if(bytesToCopy !=0)
                {
                    memcpy(buffer, MTK_USB_BUFFER + MTK_USB_RX_START,bytesToCopy);
                    MTK_USB_RX_START += bytesToCopy;
                    if(MTK_USB_RX_START == MTK_USB_MAX_BUFFER_SIZE)
                        MTK_USB_RX_START = 0;                    
                }
                MTK_USB_RX_BUFFER_SIZE -= bytesToCopy;
            }

		}
		else
		{
			return 0;
		}
		return bytesToCopy;
	}
	else
	{
	    if(fWaitingOnRead != TRUE)
	    {
	        if (!ReadFile(commPort, uartRxBuffer, length, &dwRead, &osReader)) 
	        {
	            if (GetLastError() != ERROR_IO_PENDING)
	            {
	                return 0x00;
	                // Error in communications; report it.
	            }
	            else
	            {
	                dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
	                switch(dwRes)
	                {
	                    case WAIT_OBJECT_0:
	                        GetOverlappedResult(commPort, &osReader, &dwRead, FALSE);
	                        fWaitingOnRead = FALSE;
	                        break;
	                    case WAIT_TIMEOUT:
	                        fWaitingOnRead = TRUE;
	                        if(dwRead > 0x00)
	                            assert(0);
	                        break;
	                    default:
	                        dwRead = 0;
	                        break;
	                }
	            }
	        }
	    }
	    else
	    {
	        dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
	        switch(dwRes)
	        {
	            case WAIT_OBJECT_0:
	                GetOverlappedResult(commPort, &osReader, &dwRead, FALSE);
	                fWaitingOnRead = FALSE;
	                break;
	            case WAIT_TIMEOUT:
	                fWaitingOnRead = TRUE;
	                if(dwRead > 0x00)
	                    assert(0);
	                break;
	            default:
	                dwRead = 0;
	                break;
	        }
	    
	    }
	    bytesToCopy = (unsigned short)dwRead;
	    memcpy(buffer, uartRxBuffer, length);
	    if(bytesToCopy != length)
	    {
	        /* The driver shall start a timer to polling the driver again */
	    }
	    return bytesToCopy;	
	}
} 


unsigned short BTUartWinWrite(const unsigned char *buffer, unsigned short length)
{
   DWORD dwWritten = 0;
   DWORD dwRes;
   BOOL fRes;

    if (!WriteFile(commPort, buffer, length, &dwWritten, &osWrite)) 
    {
        if (GetLastError() != ERROR_IO_PENDING) 
        { 
            fRes = FALSE;
        }
        else
        dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
        switch(dwRes)
        {
            // OVERLAPPED structure's event has been signaled. 
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(commPort, &osWrite, &dwWritten, FALSE))
                   fRes = FALSE;
                else
                  // Write operation completed successfully.
                  fRes = TRUE;
                break;
            default:
                fRes = FALSE;
                break;
        }
   }
   else
      fRes = TRUE;
   if(usb_port_used == 1)
	   return (unsigned short) dwWritten+1;
   else
   	   return (unsigned short) dwWritten;
} 



void BTUartWinSetSpeed(unsigned short speed)
{
    setupConnection(speed);
}


int readUartSetting()
{
    int file_handle;
    int port;
    file_handle = btmtk_fs_open("\\uart.ini", BTMTK_FS_READ_ONLY);

    if (file_handle < 0)   /* first time, create the file */
    {
        char *str = "COM_PORT = COM4\n";
        file_handle = btmtk_fs_open("\\uart.ini", BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        btmtk_fs_write(file_handle, str, strlen(str));
        btmtk_fs_close(file_handle);
    }
    else
    {
        char str[20] = {0};
        btmtk_fs_read(file_handle, str, 20);
        sscanf(str, "COM_PORT = COM%d", &port);
        btmtk_fs_close(file_handle);
    }
    return port;
}


BOOL openComm(void)
{
    char                  portName[20];
    COMMTIMEOUTS  CommTimeOuts;
	int port = 4;

    /* Create the Comm port name */
//    wsprintf(portName, "\\\\.\\COM%d", 5);

    wsprintf(portName, "\\\\.\\MTKBTUSB");
    printf("Opening USB port ...");

    /* Open USB port */
    if ((commPort =
         CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
                    NULL)) == (HANDLE)-1) 
    {
        printf("[Failed]\nOpening UART port ");
    } 
    else
    {
        printf("[OK]\n");
        usb_port_used = TRUE;
        return TRUE;
    }

    wsprintf(portName, "\\\\.\\COM%d", readUartSetting());
    /* Open UART port */
    if ((commPort =
         CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
                    NULL)) == (HANDLE)-1) 
    {
        printf("[Failed]\n Please check your HW setting\n");
        return FALSE;
    } 
    else
    {
        /* Get any early notifications */
        SetCommMask(commPort, EV_RXCHAR | EV_TXEMPTY);

        /* Setup device buffers */
        SetupComm(commPort, 4096, 4096);

        /* Purge any information in the buffer */
        PurgeComm(commPort, PURGE_TXABORT | PURGE_RXABORT |
                  PURGE_TXCLEAR | PURGE_RXCLEAR);

        /* Set up for overlapped I/O */
        CommTimeOuts.ReadIntervalTimeout = 10;
        CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
        CommTimeOuts.ReadTotalTimeoutConstant = 0;
        CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
        CommTimeOuts.WriteTotalTimeoutConstant = 750; 
        SetCommTimeouts(commPort, &CommTimeOuts);
        PurgeComm(commPort, PURGE_RXABORT);        
        PurgeComm(commPort, PURGE_TXABORT);        
        PurgeComm(commPort, PURGE_RXCLEAR);        
        PurgeComm(commPort, PURGE_TXCLEAR);        

        /* Try to apply appropriate port settings */
        if (FALSE == setupConnection(115)) 
    	{
            printf("[Failed]\n Please check your HW setting\n");
            CloseHandle(commPort);
            return FALSE;
        }
        /* Make sure there's actually a radio present */

        printf("[OK]\n");
        EscapeCommFunction(commPort, SETDTR);
        return TRUE;
    }
} 


/*---------------------------------------------------------------------------
 * setupConnection() 
 *      Setup the DCB
 *
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 *      TRUE    success
 *      FALSE   failed
 */
BOOL setupConnection(unsigned short speed)
{
    DCB        dcb;

    dcb.DCBlength = sizeof(DCB);
    GetCommState(commPort, &dcb);


    dcb.BaudRate = CBR_115200;

    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
	dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = 0;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.XonLim = 100;
    dcb.XoffLim = 100;
//	dcb.fRtsControl = RTS_CONTROL_ENABLE;

    // Set the software flow control
    dcb.fInX = dcb.fOutX = 0;
    dcb.XonChar = 0x11;
    dcb.XoffChar = 0x13;
    // Set other stuff 
    dcb.fBinary = TRUE;
    dcb.fParity = TRUE;

    dcb.fDsrSensitivity = 0;
    dcb.fNull = 0;
    dcb.fAbortOnError = 0;

    return SetCommState(commPort, &dcb);
}
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */



