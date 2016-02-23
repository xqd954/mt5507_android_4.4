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

#ifdef BTMTK_ON_WISE 
/* basic system service headers */
#include "bt_common.h"
#include "fs_type.h"
#include "fs_errcode.h"
#include "fs_func.h"

#include "nvram_struct.h"       /* for NVRAM read/write */
#include "Nvram_user_defs.h"
#include "bluetooth_trc.h"
#include "osapi.h"
#include "uart.h"
#include "bt.h"
#include "bt_feature.h"
#include "bt_uartif.h"

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
/*
 * The Motorola radios require some special handling for power up and
 * * the HCI RESET command. To initialize the stack reliably, the following
 * * must be done:
 * *
 * * 1. Wait at least 2 seconds before issuing the first reset command.
 * * 2. Discard all data from the serial port until the reset command
 * *    has been issued.
 * * 3. Discard all data from the serial port after a reset command until
 * *    the reset command complete event packet is received.
 * *
 * * Step 2 and 3 are useful for other radios as well, so they are enabled
 * * for all radios.
 */
#ifndef __BT_MP_RELEASE__
extern U8 BT_VIRTUAL_SNIFFER_FLAG;
#endif

static U8 resetOffset = 0;
static U8 resetComplete[] = {0x04, 0x0E, 0x04};
static enum
{
    DISCARD_ALL, RESET, RESET_DONE
} resetState = DISCARD_ALL;


/****************************************************************************
 *
 * Local Prototypes
 *
 ***************************************************************************/


extern kal_uint8 bt_uart_port;
extern bt_context_struct *bt_p;
extern void bt_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff);

extern void BTWriteToVirtualSniffer(U8 type, U8 *raw_data, U16 length);
extern void BTWriteToVirtualSnifferClose(void);

/****************************************************************************
 *
 * Public Functions
 *
 ***************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  BTUartMauiInit
 * DESCRIPTION
 *  Called by the stack (during EVM_Init) to intialize the UART hardware
 *  driver.
 * PARAMETERS
 *  func        [IN]        
 * RETURNS
 *  BT_STATUS_PENDING - Initialization is pending.
 *  BT_STATUS_FAILED - Initialization failed.
 *****************************************************************************/
BtStatus BTUartMauiInit(UartCallback func)
{
#if !defined(__MTK_TARGET__)
#ifndef __MTK_STP_SUPPORTED__
    UARTDCBStruct dcb = 
    {
        UART_BAUD_115200,   /* init */
        len_8,              /* dataBits; */
        sb_1,               /* stopBits; */
        pa_none,
        fc_none,
        0x11,               /* xonChar; */
        0x13,               /* xoffChar; */
        KAL_FALSE
    };
#endif /* __MTK_STP_SUPPORTED__ */
#endif /* !defined(__MTK_TARGET__) */ 

    /* Initialize the uart */
    bt_p->UartCallback = func;

    /* Allen 20060727 , move to radiomod.c BtRadio_SetupGpioPins() */
#if !defined(__MTK_TARGET__)
#ifndef __MTK_STP_SUPPORTED__
    /* For MT6616: UART_Open and UART_SetDCBConfig will be invoked in radionmod.c */
    BT_Transport_Open(bt_uart_port, MOD_BT);
    BT_Transport_SetDCBConfig(bt_uart_port, &dcb, MOD_BT);
#endif    
#endif /* !defined(__MTK_TARGET__) */ 

    return BT_STATUS_SUCCESS;

}

/*****************************************************************************
 * FUNCTION
 *  BTUartMauiShutdown
 * DESCRIPTION
 *  Release any resources, close the connection if open
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus BTUartMauiShutdown(void)
{
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, UART_DEINIT_START);
#endif 

#ifndef __MTK_STP_SUPPORTED__
    BT_Transport_Close(bt_uart_port, MOD_BT);
#endif

#if defined(__MTK_TARGET__)
#ifndef __MTK_STP_SUPPORTED__
    BT_Transport_TurnOnPower(bt_uart_port, KAL_FALSE);
#endif 
#endif

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, UART_DEINITIALIZED);
#endif 
    return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  BTUartMauiRead
 * DESCRIPTION
 *  Read bytes from the comm port
 * PARAMETERS
 *  buffer      allocated buffer to receive the bytes
 *  length      max number of bytes to read        
 * RETURNS
 *  number of bytes read
 *****************************************************************************/
U16 BTUartMauiRead(U8 *buffer, U16 length)
{
    U16 bytesToCopy;
#ifndef __MTK_STP_SUPPORTED__
    kal_uint8 status;
#endif
//    static kal_bool resetCmdRead = KAL_FALSE;
    static kal_bool resetCmdRead = KAL_TRUE;

    U16 retLength;

    /* Discard all data until after HCI RESET has been sent. */
    if (resetState == RESET)
    {
        U8 charBuf;

        while (1)
        {
            retLength = BT_Transport_GetBytes(bt_uart_port, &charBuf, 1, &status, MOD_BT);
#ifndef __BT_MP_RELEASE__
            if(BT_VIRTUAL_SNIFFER_FLAG ==1)
                BTWriteToVirtualSniffer(0, &charBuf, retLength);
#endif
            /* No more receive data in UART, return and wait for UART read indicaiton. */
            if (retLength == 0)
            {
                return 0;
            }

            /* Compare each character of reset complete command. */
            if (charBuf == resetComplete[resetOffset])
            {
                resetOffset++;
                if (resetOffset >= sizeof(resetComplete))
                {
                    /*
                     * Complete RESET COMMAND COMPLETE event packet has been
                     * * sent up the stack. Can now proceed as normal.
                     */
                    resetState = RESET_DONE;
                    resetOffset = 0;
                    break;
                }
            }
            else
            {
                /* Compare mismatch! Reset the compare offset. */
                resetOffset = 0;
            }
        }
    }
    else if (resetState != RESET_DONE)
    {
        U8 readBuffer[10];
        U8 idx;

        /*
         * Just discard all input until we've actually sent the HCI
         * * RESET COMMAND and matched the expected event packet reply.
         */
        for (idx = 0; idx < 10; idx++)
        {
            bytesToCopy = BT_Transport_GetBytes(bt_uart_port, readBuffer, 10, &status, MOD_BT);
#ifndef __BT_MP_RELEASE__
            if(BT_VIRTUAL_SNIFFER_FLAG ==1)
                BTWriteToVirtualSniffer(0, readBuffer, bytesToCopy);
#endif
            /* If no more receive data in UART, return and wait for UART read indicaiton. */
            if (bytesToCopy < 10)
            {
                return 0;
            }
        }

        /* Maybe more data in UART receive buffer, send read indication and return. */
        bt_send_msg(MSG_ID_BT_DATA_TO_READ_IND, MOD_BT, BT_APP_SAP, NULL, NULL);
        return 0;
    }
    if (resetCmdRead == KAL_FALSE)
    {
        U8 idx;

        bytesToCopy = min(length, sizeof(resetComplete) - resetOffset);
        for (idx = 0; idx < bytesToCopy; idx++)
        {
            *(buffer + idx) = ((U8) (resetComplete[resetOffset]));
            resetOffset++;
        }
        if (resetOffset >= sizeof(resetComplete))
        {
            resetCmdRead = KAL_TRUE;
        }
        retLength = 0;
        if (length > bytesToCopy)
        {
            retLength = BT_Transport_GetBytes(bt_uart_port, (buffer + bytesToCopy), (length - bytesToCopy), &status, MOD_BT);
#ifndef __BT_MP_RELEASE__
            if(BT_VIRTUAL_SNIFFER_FLAG ==1)
                BTWriteToVirtualSniffer(0, buffer + bytesToCopy, retLength);
#endif
        }
        return (bytesToCopy + retLength);
    }
    bytesToCopy = BT_Transport_GetBytes(bt_uart_port, buffer, length, &status, MOD_BT);
//    DisplayHex("RX:", buffer, (U16)length);
#ifndef __BT_MP_RELEASE__
    if(BT_VIRTUAL_SNIFFER_FLAG ==1)
        BTWriteToVirtualSniffer(0, buffer, bytesToCopy);
#endif
    return bytesToCopy;
}   /* end of UART_Read() */



/*****************************************************************************
 * FUNCTION
 *  BTUartMauiWrite
 * DESCRIPTION
 *  Write a buffer to the comm port
 * PARAMETERS
 *  buffer      data to send        
 *  length      length  bytes in buffer        
 * RETURNS
 *  Number of bytes written. If an error occurs, 0 bytes are returned.
 *****************************************************************************/
U16 BTUartMauiWrite(const U8 *buffer, U16 length)
{
    U16 bytesWritten;

//    DisplayHex("TX:", buffer, (U16)length);
    if (resetState == DISCARD_ALL)
    {
        /*
         * First write will be for the HCI RESET command so also begin
         * * to search for the HCI RESET COMMAND COMPLETE event packet.
         */
//        resetState = RESET;
        resetState = RESET_DONE;
    }
    bytesWritten = (kal_uint16) BT_Transport_PutBytes(bt_uart_port, (kal_uint8*) buffer, (kal_uint16) length, MOD_BT);
#ifndef __BT_MP_RELEASE__
    if (BT_VIRTUAL_SNIFFER_FLAG == 1)
        BTWriteToVirtualSniffer(0x01,(U8 *)buffer, (U16)length);
#endif
    return bytesWritten;
}


static int uart3_ena_flowctrl = 0;


/*****************************************************************************
 * FUNCTION
 *  BtUartMauiSetFlowControl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ena_flowctrl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BtUartMauiSetFlowControl(U8 ena_flowctrl)
{
    uart3_ena_flowctrl = (ena_flowctrl == 0) ? 0 : 1;
}


/*****************************************************************************
 * FUNCTION
 *  BTUartMauiSetSpeed
 * DESCRIPTION
 *  Called to change the speed of the UART.
 * PARAMETERS
 *  speed       speed - the speed (in KBps) at which the UART should operate (57 or 115)        
 * RETURNS
 *  void
 *****************************************************************************/
void BTUartMauiSetSpeed(U16 speed)
{

    UARTDCBStruct dcb = 
    {
        UART_BAUD_115200,   /* init */
        len_8,              /* dataBits; */
        sb_1,               /* stopBits; */
        pa_none,
        fc_none,
        0x11,               /* xonChar; */
        0x13,               /* xoffChar; */
        KAL_FALSE
    };

    if ((speed == 115) || (speed == 1152))
    {
        dcb.baud = (UART_baudrate) 115200;
    }
    else if (speed == 2304)
    {
        dcb.baud = (UART_baudrate) 230400;
    }
    else if (speed == 4608)
    {
        dcb.baud = (UART_baudrate) 460800;
    }
    else if (speed == 9216)
    {
        dcb.baud = (UART_baudrate) 921600;
    }
    else if (speed == 32500)
    {
        dcb.baud = (UART_baudrate) 3250000;
    }
    dcb.flowControl = (uart3_ena_flowctrl) ? fc_hw : fc_none;

    BT_Transport_SetDCBConfig(bt_uart_port, &dcb, MOD_BT);
}
#endif  /* BTMTK_ON_WISE */