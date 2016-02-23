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
 * WinSharedMem.c
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
 * Dlight Ting
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
#include <windows.h> 
#include <stdio.h>
#include <assert.h>
#include "bt_types.h"
#include "bt_mmi.h"

#ifdef __WIN_SHARED_MEMORY__
#define BT_MAX_TASK_NO 3

#define SHARED_MEMORY_BUF_SIZE 4096*2

#define BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT 0x00
#define BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER 0x01

#define BT_SHARED_MEMORY_CLIENT 0x00
#define BT_SHARED_MEMORY_SERVER 0x01

#define BT_SHARED_MEMORY_GET 0x01
#define BT_SHARED_MEMORY_PUT 0x00



HANDLE btSharedMemoryMutex[BT_MAX_TASK_NO][2][2]; 
HANDLE hMapFile[BT_MAX_TASK_NO][2][2];

#define MAX_THREAD_BUFFER_SIZE 2048

typedef struct 
{ 
   U16 length; 
   U8 buffer[MAX_THREAD_BUFFER_SIZE];
} THREAD_MESSAGE_BUFFER; 


THREAD_MESSAGE_BUFFER Bt_Task_Msg_Buffer;
THREAD_MESSAGE_BUFFER Dispatch_Server_Msg_Buffer;
THREAD_MESSAGE_BUFFER Ap_Task_Msg_Buffer;


typedef struct {
    U8 bt_shared_mem[SHARED_MEMORY_BUF_SIZE];
    U16 queue_start_index;
    U16 queue_end_index;
    U16 total_size;
}BT_SHARED_MEMORY;

BT_SHARED_MEMORY *bt_shared_mem[BT_MAX_TASK_NO][2][2];

U16 counter_memory_alloc = 0;

void win_ResumeThread(module_type type);

#define BT_MEM_GUARD1 0xABABABAB
#define BT_MEM_GUARD2 0xCDCDCDCD
#define BT_MEM_GUARD3 0xEEEEEEEE
#define BT_MEM_GUARD4 0xFFFFFFFF

void *bt_win_malloc(U32 size)
{
    U8 *ptr;

    ptr = (U8 *)malloc(size + 12); /* GUARD1 + GUARD2 + size */
    *(U32 *)ptr = BT_MEM_GUARD1;
    *(U32 *)(ptr + 4) = size;
    *(U32 *)(ptr + size + 8) = BT_MEM_GUARD2;
    return (void *)(ptr + 8);
}

void bt_win_free(void *ptr)
{
    U8 *ptr1 = ptr;
    S32 size = *(S32 *)(ptr1 - 4);
    if (*(U32 *)(ptr1 - 8) != BT_MEM_GUARD1)
    {
        assert(0);
    }
    if (*(U32 *)(ptr1 + size) != BT_MEM_GUARD2)
    {
        assert(0);
    }
    *(U32 *)(ptr1 - 8) = BT_MEM_GUARD3;
    *(U32 *)(ptr1 + size) = BT_MEM_GUARD4;
    free((ptr1 - 8));
}

void free_ilm(U8 *ptr)
{
	counter_memory_alloc--;
//    printf("External platform, free memory:%d\n",counter_memory_alloc);
#ifdef __WIN_QUEUE__    
	bt_win_free(ptr);
#endif
}

void *construct_local_para(U16 size, U8 type)
{
    local_para_struct *ptr;
    
	counter_memory_alloc++;
    if(size > MAX_ILM_BUFFER_SIZE)
        assert(0);
        
    ptr = (local_para_struct *)bt_win_malloc(size);
    memset(ptr, 0, size);
    ptr->ref_count = 1;
    ptr->msg_len = size;
    return (void *)ptr;
}

void free_local_para(void *ptr)
{
	counter_memory_alloc--;
//    printf("External platform, free memory:%d\n",counter_memory_alloc);
    bt_win_free(ptr);
    return;
}



void win_messagehdl_init(void)
{
	U16 i=0;
    U16 j=0;

    char lpszMutexname[] = "bt_mtk_mutex ";
    char szNameShared[]  = "Global\\MTKBTSharedObject ";


	memset((void *)&Bt_Task_Msg_Buffer, 0 , sizeof(THREAD_MESSAGE_BUFFER));
	memset((void *)&Dispatch_Server_Msg_Buffer, 0 , sizeof(THREAD_MESSAGE_BUFFER));
	memset((void *)&Ap_Task_Msg_Buffer, 0 , sizeof(THREAD_MESSAGE_BUFFER));

	memset((void *)&btSharedMemoryMutex, 0 , sizeof(btSharedMemoryMutex));
	for(i=0;i<BT_MAX_TASK_NO; i++)
	{
        for(j=0; j<2; j++)
        {
            lpszMutexname[sizeof(lpszMutexname)-2] = i*2+j+0x30;

            btSharedMemoryMutex[i][j][0] = CreateMutex( 
                NULL,                       // no security attributes
                FALSE,                      // initially not owned
                lpszMutexname);            // name of mutex
            btSharedMemoryMutex[i][j][1] = CreateMutex( 
                NULL,                       // no security attributes
                FALSE,                      // initially not owned
                lpszMutexname);            // name of mutex

        }
        
        for(j=0; j<2; j++)
        {
            szNameShared[sizeof(szNameShared)-2] = i*2+j+0x30;
            hMapFile[i][j][0] = CreateFileMapping(
                INVALID_HANDLE_VALUE,    // use paging file
                NULL,                    // default security 
                PAGE_READWRITE,          // read/write access
                0,                       // max. object size 
                sizeof(BT_SHARED_MEMORY),  // buffer size  
                szNameShared);                                     
            if (hMapFile[i][j][0] == NULL) 
            { 
               printf("Could not create file mapping object (%d).\n", GetLastError());
               return;
            }
            bt_shared_mem[i][j][0] = (BT_SHARED_MEMORY *)  
                                    MapViewOfFile(hMapFile[i][j][0],   // handle to map object
                                    FILE_MAP_ALL_ACCESS,            // read/write permission
                                    0,                   
                                    0,                   
                                    sizeof(BT_SHARED_MEMORY));           
            if (bt_shared_mem[i][j][0] == NULL) 
            { 
               printf("Could not create file mapping object (%d).\n", GetLastError());
               return;
            }
            else
            {
                memset(bt_shared_mem[i][j][BT_SHARED_MEMORY_CLIENT], 0, sizeof(BT_SHARED_MEMORY));
            }
            hMapFile[i][j][1] = CreateFileMapping(
                INVALID_HANDLE_VALUE,    // use paging file
                NULL,                    // default security 
                PAGE_READWRITE,          // read/write access
                0,                       // max. object size 
                sizeof(BT_SHARED_MEMORY),  // buffer size  
                szNameShared);                                     
            if (hMapFile[i][j][1] == NULL) 
            { 
               printf("Could not create file mapping object (%d).\n", GetLastError());
               return;
            }
            bt_shared_mem[i][j][1] = (BT_SHARED_MEMORY *)  
                                    MapViewOfFile(hMapFile[i][j][1],   // handle to map object
                                    FILE_MAP_ALL_ACCESS,            // read/write permission
                                    0,                   
                                    0,                   
                                    sizeof(BT_SHARED_MEMORY));           
            if (bt_shared_mem[i][j][1] == NULL) 
            { 
               printf("Could not create file mapping object (%d).\n", GetLastError());
               return;
            }
            else
            {
                memset(bt_shared_mem[i][j][1], 0, sizeof(BT_SHARED_MEMORY));
            }            
        }
	}
}


void win_message_putToSharedMemory(U8 index, U8 txrx, U8 *ptr, U16 length)
{
    DWORD dwWaitResult; 
    BT_SHARED_MEMORY *shared_memory = 0;
    // Request ownership of mutex.
    dwWaitResult = WaitForSingleObject(btSharedMemoryMutex[index][txrx][BT_SHARED_MEMORY_PUT], INFINITE);
#if 0    
    if(index == 0x00)
    {
        if(txrx == 0x00)
        {
            printf("Server Put data to MOD_BT, length:%d\n",length);
        }
        else
        {
            printf("MOD_BT Put data to Server, length:%d\n",length);
        }   
    }
    else if(index == 0x01)
    {
        if(txrx == 0x00)
        {
            printf("Server Put data to MOD_Timer, length:%d\n",length);
        }
        else
        {
            printf("MOD_Timer Put data to Server, length:%d\n",length);
        }        
    }
    else if(index == 0x02)
    {
        if(txrx == 0x00)
        {
            printf("Server Put data to MOD_MMI, length:%d\n",length);
        }
        else
        {
            printf("MOD_MMI Put data to Server, length:%d\n",length);
        }        
    }
#endif    
    switch (dwWaitResult) 
    {
        // The thread got mutex ownership.
        case WAIT_OBJECT_0: 
            __try 
            {   
                shared_memory = bt_shared_mem[index][txrx][BT_SHARED_MEMORY_PUT];
        		if ((shared_memory->total_size + length) > SHARED_MEMORY_BUF_SIZE)
        		{
        			assert(0);
        		}
        		if((shared_memory->queue_end_index + length) > SHARED_MEMORY_BUF_SIZE)
        		{
        			memcpy(
                            shared_memory->bt_shared_mem + shared_memory->queue_end_index, 
                            ptr,
        				    SHARED_MEMORY_BUF_SIZE - shared_memory->queue_end_index);
        			memcpy(shared_memory->bt_shared_mem, ptr + (SHARED_MEMORY_BUF_SIZE - shared_memory->queue_end_index),
        				length - (SHARED_MEMORY_BUF_SIZE - shared_memory->queue_end_index));
        			shared_memory->queue_end_index = length - (SHARED_MEMORY_BUF_SIZE - shared_memory->queue_end_index);
        		}
        		else
        		{
        			memcpy(
                        shared_memory->bt_shared_mem + shared_memory->queue_end_index,
                        ptr,
                        length);
        			shared_memory->queue_end_index += length;
        		}
                if(shared_memory->queue_end_index == SHARED_MEMORY_BUF_SIZE)
                    shared_memory->queue_end_index = 0;
        		shared_memory->total_size += length;
            } 
            __finally 
            { 
                // Release ownership of the mutex object.
                if (! ReleaseMutex(btSharedMemoryMutex[index][txrx][BT_SHARED_MEMORY_PUT])) 
                { 
                    // Deal with error.
                } 
                break; 
            } 

	}
}

U16 win_message_GetFromSharedMemory(U8 index, U8 txrx, U8 *ptr, U16 length)
{
    DWORD dwWaitResult; 
    BT_SHARED_MEMORY *shared_memory = 0;
    U16 bytesToCopy = 0;
    
    dwWaitResult = WaitForSingleObject(btSharedMemoryMutex[index][txrx][BT_SHARED_MEMORY_GET], 1);

    switch (dwWaitResult) 
    {
        // The thread got mutex ownership.
        case WAIT_OBJECT_0: 
            __try 
            {   
                if((index == 0x00) && (txrx == 0x01))
                {
                    bytesToCopy = 0x00;
                }
                shared_memory = bt_shared_mem[index][txrx][BT_SHARED_MEMORY_GET];
            	if(shared_memory->total_size !=0)
            	{
                    if(length >= shared_memory->total_size)
                    {
                        bytesToCopy = shared_memory->total_size;
                    
                    }
                    else
                    {
                        bytesToCopy = length;
                    }
            		if(shared_memory->queue_start_index > shared_memory->queue_end_index)
            		{
                        unsigned long copy_remain_length = 0;
                        if(bytesToCopy !=0)
                        {
                            copy_remain_length = SHARED_MEMORY_BUF_SIZE - shared_memory->queue_start_index;
                            if(copy_remain_length >= bytesToCopy) 
                            {
                                memcpy(ptr, shared_memory->bt_shared_mem + shared_memory->queue_start_index, bytesToCopy);   
                                shared_memory->queue_start_index += bytesToCopy;
                            }
                            else
                            {
                                /* Copy data larger than copy_offset */
                                memcpy(ptr, shared_memory->bt_shared_mem + shared_memory->queue_start_index, copy_remain_length);   
                                memcpy(ptr + copy_remain_length, shared_memory->bt_shared_mem, bytesToCopy - copy_remain_length);   
                                shared_memory->queue_start_index = bytesToCopy - copy_remain_length;
                            }
            				if(shared_memory->queue_start_index == SHARED_MEMORY_BUF_SIZE)
            					shared_memory->queue_start_index = 0;
                            
                        }
                        shared_memory->total_size = shared_memory->total_size - bytesToCopy;
            		}
            		else
            		{
            		    /* End > Start*/
            			unsigned long copy_offset = 0;

                        if(bytesToCopy !=0)
                        {
                            memcpy(ptr, shared_memory->bt_shared_mem + shared_memory->queue_start_index, bytesToCopy);
                            shared_memory->queue_start_index += bytesToCopy;
            				if(shared_memory->queue_start_index == SHARED_MEMORY_BUF_SIZE)
            					shared_memory->queue_start_index = 0;                    
                        }
                        shared_memory->total_size = shared_memory->total_size - bytesToCopy;
            		}
                        
            	}
            	else
            	{
            		bytesToCopy = 0;
            	}                
            } 
            __finally 
            { 
                if (! ReleaseMutex(btSharedMemoryMutex[index][txrx][BT_SHARED_MEMORY_GET])) 
                { 
                    // Deal with error.
                } 
                // Release ownership of the mutex object.
                break; 
            } 

	}
    if(bytesToCopy !=0)
    {
#if 0
        if(index == 0x00)
        {
            if(txrx == 0x00)
            {
                printf("MOD_BT Get data from Server, length:%d\n",bytesToCopy);
            }
            else
            {
                printf("Server Get data from MOD_BT, length:%d\n",bytesToCopy);
            }
        }
        else if(index == 0x01)
        {
            if(txrx == 0x00)
            {
                printf("MOD_Timer Get data from Server, length:%d\n",bytesToCopy);
            }
            else
            {
                printf("Server Get data from MOD_Timer, length:%d\n",bytesToCopy);
            }

        }
        else if(index == 0x02)
        {
            if(txrx == 0x00)
            {
                printf("MOD_MMI Get data from Server, length:%d\n",bytesToCopy);
            }
            else
            {
                printf("Server Get data from MOD_MMI, length:%d\n",bytesToCopy);
            }        
        }
#endif

    }

    return bytesToCopy;
}


void *win_bt_receive_message(void)
{
    U16 length;
    U8 *ptr;
    U16 readSize;
    U16 message_size;
    U16 header_size;

    /* Message length are the first two bytes. It includes the first two bytes */
    if(Bt_Task_Msg_Buffer.length < 2)
    {
        length = 2 - Bt_Task_Msg_Buffer.length;
        readSize = win_message_GetFromSharedMemory(0,
                                            BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT, 
                                            Bt_Task_Msg_Buffer.buffer + Bt_Task_Msg_Buffer.length, 
                                            length);
        header_size = readSize + Bt_Task_Msg_Buffer.length;
        Bt_Task_Msg_Buffer.length += readSize;
    }
    else
    {
        header_size = 2;
    }
    if(header_size >=2)
    {
        /* We get the length header completed           */
        /* We can extract the message size now          */
        ptr = (U8 *)Bt_Task_Msg_Buffer.buffer;
        message_size = ((U16) *((U8*)(ptr)+1) << 8) |((U16) *((U8*)(ptr)));
        length = message_size - Bt_Task_Msg_Buffer.length;
        readSize = win_message_GetFromSharedMemory(
                                0,
                                BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT, 
                                Bt_Task_Msg_Buffer.buffer + Bt_Task_Msg_Buffer.length, 
                                length);

        Bt_Task_Msg_Buffer.length += readSize;
        if(Bt_Task_Msg_Buffer.length == message_size)
        {
            Bt_Task_Msg_Buffer.length = 0;
            return (Bt_Task_Msg_Buffer.buffer+2);
        }
        else
            return 0;        
    }
    return 0; 

}

void *win_ap_receive_message(void)
{
    U16 length;
    U8 *ptr;
    U16 readSize;
    U16 message_size;
    U16 header_size;

    /* Message length are the first two bytes. It includes the first two bytes */
    if(Ap_Task_Msg_Buffer.length < 2)
    {
        length = 2 - Ap_Task_Msg_Buffer.length;
        readSize = win_message_GetFromSharedMemory(2,
                                            BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT, 
                                            Ap_Task_Msg_Buffer.buffer + Ap_Task_Msg_Buffer.length, 
                                            length);
        header_size = readSize + Ap_Task_Msg_Buffer.length;
        Ap_Task_Msg_Buffer.length += readSize;
    }
    else
    {
        header_size = 2;
    }
    if(header_size >=2)
    {
        /* We get the length header completed           */
        /* We can extract the message size now          */
        ptr = (U8 *)Ap_Task_Msg_Buffer.buffer;
        message_size = ((U16) *((U8*)(ptr)+1) << 8) |((U16) *((U8*)(ptr)));
        length = message_size - Ap_Task_Msg_Buffer.length;
        readSize = win_message_GetFromSharedMemory(
                                2,
                                BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT, 
                                Ap_Task_Msg_Buffer.buffer + Ap_Task_Msg_Buffer.length, 
                                length);

        Ap_Task_Msg_Buffer.length += readSize;
        if(Ap_Task_Msg_Buffer.length == message_size)
        {
            Ap_Task_Msg_Buffer.length = 0;
            return (Ap_Task_Msg_Buffer.buffer+2);
        }
        else
            return 0;        
    }
    return 0; 
}

void BTServerDispatchMain(void)
{
    U8 index=0;
    U16 length;
    U8 *ptr;
    U16 readSize;
    U16 message_size;
    U16 header_size;
    ilm_struct *data;
    U8 client0_message_empty = 0;
    U8 client1_message_empty = 0;
    U8 client2_message_empty = 0;
    
    index = 0;
    while(1)
    {
        if((client0_message_empty == 0) &&
            (client1_message_empty == 0) &&
            (client2_message_empty == 0))
        {
            client0_message_empty = 1;
            client1_message_empty = 1;
            client2_message_empty = 1;
        }
        /* Message length are the first two bytes. It includes the first two bytes */
        if(Dispatch_Server_Msg_Buffer.length < 2)
        {
            length = 2 - Dispatch_Server_Msg_Buffer.length;
            readSize = win_message_GetFromSharedMemory(index, 
                                    BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER,
                                    Dispatch_Server_Msg_Buffer.buffer + Dispatch_Server_Msg_Buffer.length, 
                                    length);
            header_size = readSize + Dispatch_Server_Msg_Buffer.length;
            Dispatch_Server_Msg_Buffer.length += readSize;
        }
        else
        {
            readSize = 2;
            header_size = 2;
        }
        if(readSize ==0)
        {
            switch(index)
            {
                case 0x00:
                    client0_message_empty = 0;
                    break;
                case 0x01:
                    client1_message_empty = 0;
                    break;
                case 0x02:
                    client2_message_empty = 0;                    
                    break;
            }
            index++;
            if(index == 0x03)
                index = 0x00;
        }
        else if(header_size >=2)
        {
            switch(index)
            {
                case 0x00:
                    client0_message_empty = 1;
                    break;
                case 0x01:
                    client1_message_empty = 1;
                    break;
                case 0x02:
                    client2_message_empty = 1;                    
                    break;
            }        
            /* We get the length header completed           */
            /* We can extract the message size now          */
            ptr = (U8 *)Dispatch_Server_Msg_Buffer.buffer;
            message_size = ((U16) *((U8*)(ptr)+1) << 8) |((U16) *((U8*)(ptr)));
            length = message_size - Dispatch_Server_Msg_Buffer.length;
            readSize = win_message_GetFromSharedMemory(
                                    index,
                                    BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER,
                                    Dispatch_Server_Msg_Buffer.buffer + Dispatch_Server_Msg_Buffer.length, 
                                    length);
            Dispatch_Server_Msg_Buffer.length += readSize;
            if(Dispatch_Server_Msg_Buffer.length == message_size)
            {
                data = (ilm_struct *)(Dispatch_Server_Msg_Buffer.buffer+2);
                switch(data->dest_mod_id)
                {
                    case MOD_BT:
                        win_message_putToSharedMemory(
                            MOD_BT,
                            BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT,
                            Dispatch_Server_Msg_Buffer.buffer ,
                            message_size);
                          break;
                    case MOD_MMI:
                        win_message_putToSharedMemory(
                            MOD_MMI,
                            BT_SHARED_MEMORY_PATH_SERVER_TO_CLIENT,
                            Dispatch_Server_Msg_Buffer.buffer ,
                            message_size);
                          break;
                        break;
                    default:
                        assert(0);
                        break;
                }
                Dispatch_Server_Msg_Buffer.length = 0;
            }
        }
    }
}


void win_send_message_from_bt(void *ptr, U16 size)
{
    U8 data[2];
    data[0] = (U8) (size+2);
    data[1] = (U8) ((size+2) >> 0x08);
    win_message_putToSharedMemory(0,BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER, data, 2);
    win_message_putToSharedMemory(0,BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER, ptr, size);
}

void win_send_message_from_timer(void *ptr, U16 size)
{
    U8 data[2];
    data[0] = (U8) (size+2);
    data[1] = (U8) ((size+2) >> 0x08);
    win_message_putToSharedMemory(1,BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER, data, 2);
    win_message_putToSharedMemory(1,BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER, ptr, size);
}


void win_send_message_from_ap(void *ptr, U16 size)
{
    U8 data[2];
    data[0] = (U8) (size+2);
    data[1] = (U8) ((size+2) >> 0x08);
    win_message_putToSharedMemory(2,BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER, data, 2);
    win_message_putToSharedMemory(2,BT_SHARED_MEMORY_PATH_CLIENT_TO_SERVER, ptr, size);
}



#endif


