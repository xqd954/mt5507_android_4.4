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

/* basic system service headers */
#include "bt_common.h"
#include "osapi.h"
#include "parseopts.h"
#include "bt.h"
#include "bt_feature.h"

#ifndef __BT_MP_RELEASE__

#ifndef __BT_VIRTUAL_SNIFFER_C_
#define __BT_VIRTUAL_SNIFFER_C_
//#define __BT_VIRTUAL_SNIFFER__

U8 BT_VIRTUAL_SNIFFER_FLAG = 0;

#ifdef __BT_VIRTUAL_SNIFFER__
U8 BT_VIRTUAL_DATA[36000];
U32 BT_VIRTUAL_DATA_LEN = 0;
#endif

#ifdef __BT_VIRTUAL_SNIFFER__
kal_wchar bt_virtual_path[10];
kal_wchar bt_virtual_file[20];
kal_wchar bt_virtual_file1[20];
extern BOOL BTCoreVerifySysInitState(void);
static U8 BtTempKey0, BtTempKey1, BtTempKey2, BtTempKey3;
static void BTVirtualSnifferDecodeCommand(unsigned char *source, int length);
static void BTVirtualSnifferDecodeEvent(unsigned char *source, int length);
static void BTVirtualSnifferDecodeWrite(U8 type, U8 *raw_data, U16 length, U32 tick);
#endif

void BTWriteToVirtualSniffer(U8 type, U8 *raw_data, U16 length)
{
#ifdef __BT_VIRTUAL_SNIFFER__
    U8 *temp_buffer = BT_VIRTUAL_DATA + BT_VIRTUAL_DATA_LEN;
    U32 current_tick;
    if(length != 0x00)
    {
        current_tick = OS_GetSystemTime();
        temp_buffer[0] = type;
        temp_buffer[1] = (length >> 8);
        temp_buffer[2] = (U8)(length);
        memcpy(temp_buffer+3, (U8 *)&current_tick, 4);
        memcpy(temp_buffer+7, raw_data, length);
        BT_VIRTUAL_DATA_LEN += (7+length);

        if(BT_VIRTUAL_DATA_LEN > 0x3000)
        {
            FS_HANDLE file_handle;
            UINT written;
    
            file_handle = FS_Open(bt_virtual_path, FS_OPEN_DIR);
            if (file_handle == FS_FILE_NOT_FOUND)
            {
                FS_CreateDir(bt_virtual_path);
            }
            file_handle = FS_Open(bt_virtual_file, FS_READ_WRITE);

            if (file_handle == FS_FILE_NOT_FOUND)   /* first time, create the file */
            {
                file_handle = FS_Open(bt_virtual_file, FS_CREATE_ALWAYS);
                FS_Write(file_handle, BT_VIRTUAL_DATA, BT_VIRTUAL_DATA_LEN, &written);
                FS_Close(file_handle);
            }
            else
            {
                FS_Seek(file_handle, 0, FS_FILE_END);
                kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
                FS_Write(file_handle, BT_VIRTUAL_DATA, BT_VIRTUAL_DATA_LEN, &written);
                FS_Close(file_handle);
            }
            BT_VIRTUAL_DATA_LEN = 0;
        }
    }
#endif    
}

/*
 * This function is used to get drive letter and init the path for virtual sniffer log.
 * We'll try to get the storage in phone or removable memory card.
 */
BOOL BTConfigDrive(void)
{
#ifdef __BT_VIRTUAL_SNIFFER__
    int type = 0;
    S32 drive = FS_GetDrive(FS_DRIVE_V_NORMAL, 2, FS_DRIVE_V_NORMAL | FS_DRIVE_V_REMOVABLE);
    
    if (drive > 0)        
    {
    	kal_wsprintf(bt_virtual_path, "%c:\\", (S8)drive);
        kal_wsprintf(bt_virtual_file, "%c:\\dlight.log", (S8)drive);
        kal_wsprintf(bt_virtual_file1, "%c:\\dlight1.log", (S8)drive);

        type = FS_GetDevType(bt_virtual_path);       
        if (type != FS_DEVICE_TYPE_NAND)            
        {
            /* In this case, dlibht.log may be stored in memory card.
             * If the memory card isn't inserted, the log will not be stored.
             */
            bt_trace(TRACE_GROUP_1, BTLOG_OPEN_DRIVE_FAILED, 1);
        }        
    } 
    else 
    {
        return FALSE;
    }
    return TRUE;
#endif    
    return TRUE;
}

void BTWriteToVirtualSnifferClose(void)
{
#ifdef __BT_VIRTUAL_SNIFFER__
    if(BT_VIRTUAL_DATA_LEN > 0)
    {
        FS_HANDLE file_handle;
        UINT written;

        file_handle = FS_Open(bt_virtual_path, FS_OPEN_DIR);
        if (file_handle == FS_FILE_NOT_FOUND)
        {
            FS_CreateDir(bt_virtual_path);
        }
        file_handle = FS_Open(bt_virtual_file, FS_READ_WRITE);

        if (file_handle == FS_FILE_NOT_FOUND)   /* first time, create the file */
        {
            file_handle = FS_Open(bt_virtual_file, FS_CREATE_ALWAYS);
            FS_Write(file_handle, BT_VIRTUAL_DATA, BT_VIRTUAL_DATA_LEN, &written);
            FS_Close(file_handle);
        }
        else
        {
            FS_Seek(file_handle, 0, FS_FILE_END);
            kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
            FS_Write(file_handle, BT_VIRTUAL_DATA, BT_VIRTUAL_DATA_LEN, &written);
            FS_Close(file_handle);
        }
        BT_VIRTUAL_DATA_LEN = 0;
    }
#endif    
}

#define BT_VIRTUAL_SNIFFER_READ_TYPE 0x00
#define BT_VIRTUAL_SNIFFER_READ_LENGTH_MSB 0x01
#define BT_VIRTUAL_SNIFFER_READ_LENGTH_LSB 0x02
#define BT_VIRTUAL_SNIFFER_READ_TIMER_TICK1 0x03
#define BT_VIRTUAL_SNIFFER_READ_TIMER_TICK2 0x04
#define BT_VIRTUAL_SNIFFER_READ_TIMER_TICK3 0x05
#define BT_VIRTUAL_SNIFFER_READ_TIMER_TICK4 0x06
#define BT_VIRTUAL_SNIFFER_READ_RAW_DATA 0x07


void BTVirtualSnifferDecoder(void)
{
#ifdef __BT_VIRTUAL_SNIFFER__
    FS_HANDLE source_hdl, target_hdl;
    U8 c;
    U8 *buffer;
    U8 *tx_raw_data;
    U8 *rx_raw_data; 
    int state = BT_VIRTUAL_SNIFFER_READ_TYPE;
    int read_length;
    int block_length;
    int tx_raw_length = 0;
    int rx_raw_length = 0;    
    U8 rx_state = 0;
    U8 tx_state = 0;
    U8 tx_type = 0;
    U8 rx_type = 0;
    int rx_read_length = 0;
    int tx_read_length = 0;
    int key_read_length = 0;
    U32 rx_system_tick;
    U32 tx_system_tick;    
    U8 txrx = 0;
    unsigned int read_actual_size;
    unsigned int current_read_offset = 0;
    unsigned int written;
    
    BTWriteToVirtualSnifferClose();
    if(BTCoreVerifySysInitState() == TRUE)
    {
        Report(("ERROR, the operation can not be used in Power On mode"));        
        FS_Delete(bt_virtual_file1);
        return;
    }
    tx_raw_data = BT_VIRTUAL_DATA + 3000;
    rx_raw_data = BT_VIRTUAL_DATA + 6000;
    buffer = BT_VIRTUAL_DATA + 9000;
    memset(tx_raw_data, 0, sizeof(tx_raw_data));
    memset(rx_raw_data, 0, sizeof(rx_raw_data));
    rx_state = 0;
    tx_state = 0;
    source_hdl = FS_Open(bt_virtual_path, FS_OPEN_DIR);
    if (source_hdl == FS_FILE_NOT_FOUND)
    {
        Report(("Decode error 1"));
        return;
    }
    source_hdl = FS_Open(bt_virtual_file, FS_READ_WRITE);

    if (source_hdl == FS_FILE_NOT_FOUND)
    {
        return;
    }
    else
    {
        FS_Seek(source_hdl, 0, FS_FILE_BEGIN);
        target_hdl = FS_Open(bt_virtual_file1, FS_CREATE_ALWAYS);
    }
    tx_system_tick = 0;
    rx_system_tick = 0;
    block_length = 0;
    read_length = 0;
    while(1)
    {
        FS_Read(source_hdl, buffer, 2048, &read_actual_size);
        if(read_actual_size ==0)
            break;
        for (current_read_offset=0; current_read_offset < read_actual_size; current_read_offset++)
        {
            c = buffer[current_read_offset];
            switch(state)
            {
                case BT_VIRTUAL_SNIFFER_READ_TYPE:
                    if(c == 0x00)
                    {
                        txrx = 0; /* rx */
                        rx_system_tick = 0;
                    }else if(c== 0x01)
                    {
                        txrx = 1; /* tx */
                        tx_system_tick = 0;
                    }
                    else if (c== 0x02)
                    {
                        txrx = 2;
                        key_read_length = 0;
                    }
                    else
                    {
                        Report(("parse error 1\n"));
                        goto BTVirtualSnifferEnd;
                    }
                    state++;
                    break;
                case BT_VIRTUAL_SNIFFER_READ_LENGTH_MSB:
                    block_length = 0;
                    block_length = c << 8;
                    state++;
                    break;
                case BT_VIRTUAL_SNIFFER_READ_LENGTH_LSB:
                    block_length += c;
                    if(block_length > 1024)
                    {
                        Report(("Parse error 2\n"));
                        goto BTVirtualSnifferEnd;
                    }
                    state++;
                    read_length = 0;
                    break;
                case BT_VIRTUAL_SNIFFER_READ_TIMER_TICK1:
                    if(txrx == 0x00)
                    {
                        rx_system_tick = c;
                    }
                    else if(txrx == 0x01)
                    {
                        tx_system_tick = c;
                    }
                    state++;
                    break;
                case BT_VIRTUAL_SNIFFER_READ_TIMER_TICK2:
                    if(txrx == 0x00)
                    {
                        rx_system_tick = (rx_system_tick ) | (((unsigned long)c) << 8 );
                    }
                    else if(txrx == 0x01)
                    {
                        tx_system_tick = (tx_system_tick ) | (((unsigned long)c) << 8 );
                    }
                    state++;
                    break;
                case BT_VIRTUAL_SNIFFER_READ_TIMER_TICK3:
                    if(txrx == 0x00)
                    {
                        rx_system_tick = (rx_system_tick ) | (((unsigned long)c) << 16 );
                    }
                    else if(txrx == 0x01)
                    {
                        tx_system_tick = (tx_system_tick ) | (((unsigned long)c) << 16 );
                    }
                    state++;
                    break;                
                case BT_VIRTUAL_SNIFFER_READ_TIMER_TICK4:                
                    if(txrx == 0x00)
                    {
                        rx_system_tick = (rx_system_tick ) | (((unsigned long)c) << 24 );
                    }
                    else if(txrx == 0x01)
                    {
                        tx_system_tick = (tx_system_tick ) | (((unsigned long)c) << 24 );
                    }
                    state++;
                    break;                
                case BT_VIRTUAL_SNIFFER_READ_RAW_DATA:
                    read_length++;
                    if(read_length == block_length)
                    {
                        state = 0;
                    }
                    if(txrx ==0)
                    {
                        switch(rx_state)
                        {
                            case 0x00:
                                rx_read_length = 0;
                                memset(rx_raw_data, 0, sizeof(rx_raw_data));
                                switch(c)
                                {
                                    case 0x02:
                                        /* data */
                                        rx_type = 0x02;
                                        rx_raw_length = 5;
                                        rx_read_length = 0;
                                        break;
                                    case 0x04:
                                        /* event */
                                        rx_type = 0x04;
                                        rx_raw_length = 3;
                                        rx_read_length = 0;                                    
                                        break;
                                    default:
                                        Report(("parse error 3\n"));
                                        goto BTVirtualSnifferEnd;
                                }
                                rx_state++;
                                break;
                            case 0x01:
                                if(rx_type == 0x04)
                                {
                                    rx_raw_data[rx_read_length] = c;
                                    rx_read_length++;
                                    if(rx_read_length == 2)
                                        rx_raw_length = c+2;
                                    if(rx_read_length == rx_raw_length)
                                    {
                                        /* HCI Command from Host , type = 1, source = 0*/
                                        /* HCI Data from Host  , type = 2, source = 0*/        
                                        /* HCI Data from Controller  , type = 2, source = 1*/        
                                        /* HCI Event From controller , type = 8, source = 1 */
                                        Report(("parse a completed event\n"));
                                        BTVirtualSnifferDecodeEvent(rx_raw_data, rx_read_length);
                                        BTVirtualSnifferDecodeWrite(1, rx_raw_data, rx_read_length+1, rx_system_tick);
                                        FS_Write(target_hdl, BT_VIRTUAL_DATA, 8 + rx_read_length, &written);

                                        rx_read_length = 0;
                                        rx_raw_length = 0;
                                        rx_state = 0;
                                    }
                                }
                                else
                                {
                                    rx_raw_data[rx_read_length] = c;
                                    rx_read_length++;
                                    if(rx_read_length == 3)
                                        rx_raw_length = (c);
                                    if(rx_read_length == 4)
                                    {
                                        rx_raw_length |= ((int)(c)<< 8);
                                        rx_raw_length += 4;
                                    }
                                    if((rx_read_length == rx_raw_length) && (rx_read_length!=3))
                                    {
                                        /* HCI Command from Host , type = 1, source = 0*/
                                        /* HCI Data from Host  , type = 2, source = 0*/        
                                        /* HCI Data from Controller  , type = 2, source = 1*/        
                                        /* HCI Event From controller , type = 8, source = 1 */
                                        Report(("parse a completed Controller data\n"));
                                        BTVirtualSnifferDecodeWrite(2, rx_raw_data, rx_read_length+1, rx_system_tick);
                                        FS_Write(target_hdl, BT_VIRTUAL_DATA, 8 + rx_read_length, &written);
                                        rx_read_length = 0;
                                        rx_raw_length = 0;
                                        rx_state = 0;
                                    }
                                }
                                break;
                            }
                    }
                    else if(txrx == 1)/* Tx */
                    {
                        switch(tx_state)
                        {
                            case 0x00:
                                tx_read_length = 0;
                                memset(tx_raw_data, 0, sizeof(tx_raw_data));
                                switch(c)
                                {
                                    case 0x02:
                                        /* data */
                                        tx_type = 0x02;
                                        tx_raw_length = 5;
                                        tx_read_length = 0;                                    
                                        tx_state++;
                                        break;
                                    case 0x01:
                                        /* command */
                                        tx_type = 0x01;
                                        tx_raw_length = 4;
                                        tx_read_length = 0;                                    
                                        tx_state++;
                                        break;
                                    case 0xFF:
                                        {
                                            unsigned char temp_buffer[] = {0xC0, 0xFC, 0x00 };
                                            BTVirtualSnifferDecodeWrite(0x03, temp_buffer,4, tx_system_tick);
                                            FS_Write(target_hdl, BT_VIRTUAL_DATA, 11, &written);
                                             
                                            tx_read_length = 0;
                                            tx_raw_length = 0;
                                            tx_state = 0;
                                        }
                                        break;
                                    default:
                                        Report(("parse error 4\n"));
                                        goto BTVirtualSnifferEnd;
                                }
                                break;
                            case 0x01:
                                if(tx_type == 0x01)
                                {
                                    tx_raw_data[tx_read_length] = c;
                                    tx_read_length++;
                                    if(tx_read_length == 3)
                                        tx_raw_length = c+3;
                                    if(tx_read_length == tx_raw_length)
                                    {
                                        /* HCI Command from Host , type = 1, source = 0*/
                                        /* HCI Data from Host  , type = 2, source = 0*/        
                                        /* HCI Data from Controller  , type = 2, source = 1*/        
                                        /* HCI Event From controller , type = 8, source = 1 */
                                        Report(("parse a completed Command\n"));
                                        BTVirtualSnifferDecodeCommand(tx_raw_data, tx_read_length);
                                        BTVirtualSnifferDecodeWrite(4, tx_raw_data, tx_read_length+1, tx_system_tick);
                                        FS_Write(target_hdl, BT_VIRTUAL_DATA, 8 + tx_read_length, &written);
                                        tx_read_length = 0;
                                        tx_raw_length = 0;
                                        tx_state = 0;
                                    }
                                }
                                else
                                {
                                    tx_raw_data[tx_read_length] = c;
                                    tx_read_length++;
                                    if(tx_read_length == 3)
                                        tx_raw_length = ((int)c);
                                    if(tx_read_length == 4)
                                    {
                                        tx_raw_length |= ((int)(c)<< 8);
                                        tx_raw_length += 4;
                                    }
                                    if((tx_read_length == tx_raw_length) && (tx_read_length !=3))
                                    {
                                        /* HCI Command from Host , type = 1, source = 0*/
                                        /* HCI Data from Host  , type = 2, source = 0*/        
                                        /* HCI Data from Controller  , type = 2, source = 1*/        
                                        /* HCI Event From controller , type = 8, source = 1 */
                                        Report(("parse a completed Host Data\n"));
                                        BTVirtualSnifferDecodeWrite(5, tx_raw_data, tx_read_length+1, tx_system_tick);
                                        FS_Write(target_hdl, BT_VIRTUAL_DATA, 8 + tx_read_length, &written);
                                        tx_read_length = 0;
                                        tx_raw_length = 0;
                                        tx_state = 0;
                                    }
                                }
                                break;
                            }
                        }
                        else
                        {
                        
                            switch (key_read_length)
                            {
                                case 0x00:
                                    BtTempKey0 = c;
                                    key_read_length++;
                                    break;
                                case 0x01:
                                    BtTempKey1 = c;
                                    key_read_length++;
                                    break;
                                case 0x02:
                                    BtTempKey2 = c;
                                    key_read_length++;
                                    break;
                                case 0x03:
                                    key_read_length = 0;;
                                    BtTempKey3 = c;
                                    break;
                            }
                        }
                        break;
                    default:
                        Report(("parse error 5\n"));
                        goto BTVirtualSnifferEnd;
            }
        }
    }
BTVirtualSnifferEnd:
    FS_Close(target_hdl);
    FS_Close(source_hdl);
#endif    
}


#ifdef __BT_VIRTUAL_SNIFFER__
static void BTVirtualSnifferDecodeWrite(U8 type, U8 *raw_data, U16 length, U32 tick)
{
    U8 *temp_buffer = BT_VIRTUAL_DATA;
    U32 current_tick;
    
    if(length != 0x00)
    {
        current_tick = tick;
        switch(type)
        {
            case 0x01:  /* Event */
            case 0x02:  /* Data from Controller */
                temp_buffer[0] = 0x00;
                break;
            case 0x03:  /* Wakeup command */
            case 0x04:  /* Wakeup command */
            case 0x05:  /* Data from Host */
                temp_buffer[0] = 0x01;
                break;
        }
        temp_buffer[1] = (length >> 8);
        temp_buffer[2] = (U8)(length);
        memcpy(temp_buffer+3, (U8 *)&current_tick, 4);
        switch(type)
        {
            case 0x01:  /* Event */
                temp_buffer[7] = 0x04;
                break;
            case 0x02:  /* Data from Controller */
                temp_buffer[7] = 0x02;
                break;
            case 0x03:  /* Wakeup command */
            case 0x04:  /* Wakeup command */
                temp_buffer[7] = 0x01;
                break;
            case 0x05:  /* Data from Host */
                temp_buffer[7] = 0x02;
                break;
        }
        memcpy(temp_buffer+8, raw_data, length);
    }
}

static void BTVirtualSnifferDecodeCommand(unsigned char *source, int length)
{
    int current_pos = 0;
    int index = 0;
    unsigned char temp_key0, temp0;
    unsigned char temp_key1, temp1;
    unsigned char temp_key2, temp2;
    unsigned char temp_key3, temp3;

    temp_key0 = BtTempKey0;
    temp_key1 = BtTempKey1;
    temp_key2 = BtTempKey2;
    temp_key3 = BtTempKey3;
    while (current_pos < length)
    {
        if(current_pos < 3)
        {
            if(current_pos == 0)
            {
                source[0] = source[0] ^ temp_key0;
            }
            else if(current_pos == 1)
            {
                source[1] = source[1] ^ temp_key1;
            }
            else if(current_pos == 2)
            {
            }
                
        }
        else
        {
            index = current_pos % 4;
            switch (index)
            {
                case 0x00:
                    source[current_pos] = source[current_pos] ^ temp_key3;
                    temp0 = temp_key0;
                    temp1 = temp_key1;
                    temp2 = temp_key2;
                    temp3 = temp_key3;
                    temp_key0 = (temp0 << 3) | (temp0 >> 5);
                    temp_key1 = (temp1 << 3) | (temp1 >> 5);
                    temp_key2 = (temp2 << 3) | (temp2 >> 5);
                    temp_key3 = (temp3 << 3) | (temp3 >> 5);
                    break;
                case 0x01:
                    source[current_pos] = source[current_pos] ^ temp_key0;

                    break;
                case 0x02:
                    source[current_pos] = source[current_pos] ^ temp_key1;
                    break;
                case 0x03:
                    source[current_pos] = source[current_pos] ^ temp_key2;
                    break;
            }
        }
        current_pos++;
    }
}


static void BTVirtualSnifferDecodeEvent(unsigned char *source, int length)
{
    int current_pos = 0;
    int index = 0;
    unsigned char temp_key0, temp0;
    unsigned char temp_key1, temp1;
    unsigned char temp_key2, temp2;
    unsigned char temp_key3, temp3;

    temp_key0 = BtTempKey0;
    temp_key1 = BtTempKey1;
    temp_key2 = BtTempKey2;
    temp_key3 = BtTempKey3;
    while (current_pos < length)
    {
        if(current_pos < 3)
        {
            if(current_pos == 0)
            {
                source[0] = source[0] ^ temp_key0;
            }
            else if(current_pos == 2)
            {
                source[2] = source[2] ^ temp_key1;
            }
                
        }
        else
        {
            index = current_pos % 4;
            switch (index)
            {
                case 0x00:
                    source[current_pos] = source[current_pos] ^ temp_key3;
                    temp0 = temp_key0;
                    temp1 = temp_key1;
                    temp2 = temp_key2;
                    temp3 = temp_key3;
                    temp_key0 = (temp0 << 5) | (temp0 >> 3);
                    temp_key1 = (temp1 << 5) | (temp1 >> 3);
                    temp_key2 = (temp2 << 5) | (temp2 >> 3);
                    temp_key3 = (temp3 << 5) | (temp3 >> 3);
                    break;
                case 0x01:
                    source[current_pos] = source[current_pos] ^ temp_key0;
                    break;
                case 0x02:
                    source[current_pos] = source[current_pos] ^ temp_key1;
                    break;
                case 0x03:
                    source[current_pos] = source[current_pos] ^ temp_key2;
                    break;
            }
        }
        current_pos++;
    }
}
#endif

#endif
#endif
