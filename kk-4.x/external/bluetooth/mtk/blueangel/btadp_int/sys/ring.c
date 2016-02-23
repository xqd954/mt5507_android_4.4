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
 *     $Workfile:ring.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *      This file contains a collection of utilities to manage ring- or
 *      circular buffers.
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

#include "ring.h"
#include "utils.h"
#include "osapi.h"
#ifndef BTMTK_ON_LINUX
#include "bt_os_api.h"
#endif

#ifdef BTMTK_ON_LINUX
#define btmtk_os_memcpy memcpy
#endif


#ifdef __SPP_ORG_TRANSMIT_METHOD__


/*****************************************************************************
 * FUNCTION
 *  RING_BufInit
 * DESCRIPTION
 *  Initialize a ring buffer structure.
 * PARAMETERS
 *  ring        [IN]         Pointer to RingBuf struct
 *  buf         [IN]         Allocated buffer which will become the ring buffer
 *  len         [IN]         Length of buf
 * RETURNS
 *  void
 *****************************************************************************/
void RING_BufInit(RingBuf *ring, char buf[], S16 len)
{
    ring->start = (U8*) buf;
    ring->end = (U8*) buf + len;    /* 'end' is 1 beyond of the buffer */
    RING_BufFlush(ring);
}


/*****************************************************************************
 * FUNCTION
 *  RING_BufFlush
 * DESCRIPTION
 *  Clear data from a ring buffer.
 * PARAMETERS
 *  ring        [IN]     Pointer to RingBuf struct.
 * RETURNS
 *  void
 *****************************************************************************/
void RING_BufFlush(RingBuf *ring)
{
    ring->data = ring->start;
    ring->free = ring->start;
    ring->dataLen = 0;
}


/*****************************************************************************
 * FUNCTION
 *  RING_WriteData
 * DESCRIPTION
 *  Copy data into the ring buffer from caller's buffer.
 *  Does NOT overwrite data already in the buffer.
 * PARAMETERS
 *  ring        [IN]         Ring - pointer to RingBuf struct
 *  buf         [IN]         Buf  - caller's destination buffer
 *  len         [IN]         Len  - max amount to copy
 * RETURNS
 *  amount copied to buf
 *****************************************************************************/
S16 RING_WriteData(RingBuf *ring, char *buf, S16 len)
{
    S16 bytes;

    bytes = min(len, RING_FreeSpace(ring));

    len = bytes;
    ring->dataLen += bytes;

    while (bytes--)
    {
        *ring->free++ = *buf++;

        if (ring->free >= ring->end)
        {
            ring->free = ring->start;
        }
    }

    return len;

}


/*****************************************************************************
 * FUNCTION
 *  RING_ReadData
 * DESCRIPTION
 *  Move data from the ring buffer to caller's buffer.
 *  Once the data is copied, the space in the ring buffer is considered
 *  free.
 * PARAMETERS
 *  ring        [IN]         Ring - pointer to RingBuf struct
 *  buf         [IN]         Buf  - caller's destination buffer
 *  len         [IN]         Len  - max amount to copy
 * RETURNS
 *  amount copied to buf
 *****************************************************************************/
S16 RING_ReadData(RingBuf *ring, char *buf, S16 len)
{
    S16 bytes = 0;  /* bytes copied */

    len = min(len, ring->dataLen);

    while (bytes < len)
    {
        *buf++ = *ring->data++;
        bytes++;

        if (ring->data >= ring->end)
        {
            ring->data = ring->start;
        }
    }

    ring->dataLen -= bytes;

    return bytes;
}


/*****************************************************************************
 * FUNCTION
 *  RING_GetDataPtr
 * DESCRIPTION
 *  Get a pointer to the data and the number of bytes. The number of
 *  bytes is for contiguous data only. This function might be used
 *  by a routine that did not have it's own buffer yet wanted access
 *  to data in the ring buffer.
 * PARAMETERS
 *  ring        [IN]         Pointer to ring buffer structure
 *  data        [IN]         Address of pointer that this function will update
 *  len         [IN]         Pointer to number of bytes requested
 * RETURNS
 *  data    contains pointer to beginning of data
 *  len     contains number of bytes available not exceeding original
 *  value; len represents the number of contiguous bytes.
 *****************************************************************************/
void RING_GetDataPtr(RingBuf *ring, char **data, S16 *len)
{
    *data = (char*)ring->data;
    *len = min(*len, RING_ContigDataLen(ring));
}


/*****************************************************************************
 * FUNCTION
 *  RING_BufDelete
 * DESCRIPTION
 *  Deletes bytes from the buffer. Bytes are removed starting with
 *  the oldest or "first in" data.
 * PARAMETERS
 *  ring        [IN]         Pointer to ring buffer structure
 *  len         [IN]         Bytes to delete; if len > existing bytes, existing bytes are removed
 * RETURNS
 *  void
 *****************************************************************************/
void RING_BufDelete(RingBuf *ring, S16 len)
{
    S16 wrap;

    len = min(len, RING_DataLen(ring));

    ring->data += len;
    ring->dataLen -= len;
    wrap = ring->data - ring->end;

    if (wrap >= 0)
    {
        ring->data = ring->start + wrap;
    }
}
#else /* __SPP_ORG_TRANSMIT_METHOD__ */ 


/*****************************************************************************
 * FUNCTION
 *  RING_BufInit
 * DESCRIPTION
 *  Initialize a ring buffer structure.
 * PARAMETERS
 *  ring        [IN]         Pointer to RingBuf struct
 *  buf         [IN]         Allocated buffer which will become the ring buffer
 *  len         [IN]         Length of buf
 * RETURNS
 *  void
 *****************************************************************************/
void RING_BufInit(RingBuf *ring, char buf[], S16 len)
{
    ring->pStart = (U8*) buf;
    ring->pEnd = (U8*) buf + len;   /* 'end' is 1 beyond of the buffer */
    RING_BufFlush(ring);
}


/*****************************************************************************
 * FUNCTION
 *  RING_BufFlush
 * DESCRIPTION
 *  Clear data from a ring buffer.
 * PARAMETERS
 *  ring        [IN]     Pointer to RingBuf struct.
 * RETURNS
 *  void
 *****************************************************************************/
void RING_BufFlush(RingBuf *ring)
{
    ring->pRead = ring->pStart;
    ring->pWrite = ring->pStart;

}


/*****************************************************************************
 * FUNCTION
 *  RING_WriteData
 * DESCRIPTION
 *  Copy data into the ring buffer from caller's buffer.
 *  Does NOT overwrite data already in the buffer.
 * PARAMETERS
 *  ring                    [IN]         Ring - pointer to RingBuf struct
 *  buf                     [IN]         Caller's destination buffer
 *  wanted_to_write_len     [IN]         Max amount to copy
 * RETURNS
 *  amount copied to buf
 *****************************************************************************/
S16 RING_WriteData(RingBuf *ring, char *buf, S16 wanted_to_write_len)
{
    S16 actual_written_len = 0, freespace = 0;
    U8 *pPrevWrite = ring->pWrite;
    U8 *pPrevRead = ring->pRead;

    /* Report(("In RING_WriteData(): start=%08X,read=%08X,write=%08X,end=%08X",ring->pStart,pPrevRead,pPrevWrite,ring->pEnd)); */

    /* Directly using the following codes to compute the freespace instead of the function RING_FreeSpace() */
    if (pPrevWrite >= pPrevRead)
    {
        /* case 1: pWrite >= pRead */
        /* pWrite == pRead: It means that the buffer is empty */

        /*
         * Because the context switch may happens before updating pWrite
         * So copy data to ring buffer must be one action, not use one byte by one byte 
         * If the wrapper case happens, just copy first segment data into ring buffer, not do twice pWrite pointer's update
         * s--> This will increase the complexity of context switch problem
         */

        /*
         * case 1 
         *      [             |*********|           ]
         *                 pRead        pWrite 
         */

        freespace = ring->pEnd - pPrevWrite;

        /*
         * [Important!]  
         * Consider one scenario:
         * pPrevRead is at start position, pPrevWrite > pPrevRead:
         * 
         * [***********|           ]
         * pRead      pWrite      pEnd
         * pStart
         * 
         * Currently, some data is not sent by BT task as the above figure shown.
         * Now, do write action, if freespace = pEnd - pWrite, then wirtes data to fill with all freespace,
         * pWrite is wrapped to pStart
         * Context switch happens: pRead == pWrite: BT task misunstands no data is needed to send!!
         * Context switch happens: PWrite continous to write and overwrite the data not yet send!
         * --> This is why the sniffer log finds that some data lost !! 
         * Solution: If pPrevRead is at start positon, (freespace = freespace -1)
         */

        if (pPrevRead == ring->pStart)
        {
            /* 
             * [***********|           ]
             * pRead       pWrite      pEnd
             * pStart
             */

            freespace -= 1;
            if (freespace > wanted_to_write_len)
            {
                btmtk_os_memcpy(pPrevWrite, buf, wanted_to_write_len);
                actual_written_len = wanted_to_write_len;
                pPrevWrite += actual_written_len;
            }
            else
            {
                /* freespace <= wanted_to_write_len */
                btmtk_os_memcpy(pPrevWrite, buf, freespace);
                actual_written_len = freespace;
                /* pWrite pointer is wrapped to the start position of this Ring buffer */
                pPrevWrite += actual_written_len;
            }

        }
        else
        {
            /*
             * case 1 
             *      [             |*********|           ]
             *                 pRead        pWrite 
             */

            if (freespace >= wanted_to_write_len)
            {
                btmtk_os_memcpy(pPrevWrite, buf, wanted_to_write_len);
                actual_written_len = wanted_to_write_len;
                pPrevWrite += actual_written_len;
                if (pPrevWrite == ring->pEnd)
                {
                    pPrevWrite = ring->pStart;
                }
            }
            else
            {
                /* Two memcpy is necessary */
                S16 second_freespace = pPrevRead - ring->pStart - 1;
                S16 second_wanted_write_len = 0;

                btmtk_os_memcpy(pPrevWrite, buf, freespace);
                actual_written_len = freespace;
                pPrevWrite += actual_written_len;

                /* Wrap to start position anc continous to write data! */
                pPrevWrite = ring->pStart;
                second_wanted_write_len = wanted_to_write_len - actual_written_len;
                if (second_freespace >= second_wanted_write_len)
                {
                    btmtk_os_memcpy(pPrevWrite, buf + actual_written_len, second_wanted_write_len);
                    /* Add this time's written len */
                    actual_written_len += second_wanted_write_len;
                    pPrevWrite += second_wanted_write_len;
                }
                else
                {
                    btmtk_os_memcpy(pPrevWrite, buf + actual_written_len, second_freespace);
                    /* Add this time's written len */
                    actual_written_len += second_freespace;
                    pPrevWrite += second_freespace;
                }

            }

        }

        ring->pWrite = pPrevWrite;

    }
    else
    {
        /* case 2: pWrite < pRead */
        /* If the pWrite+1 == pRead: It means that the buffer is full */

        /*
         *   Case 2:
         *     [********|                |******]
         *             pWrite   |     pRead  
         *                     pReserve
         */

        freespace = pPrevRead - pPrevWrite - 1;

        if (freespace > wanted_to_write_len)
        {
            btmtk_os_memcpy(pPrevWrite, buf, wanted_to_write_len);
            actual_written_len = wanted_to_write_len;
            pPrevWrite += actual_written_len;
        }
        else
        {
            btmtk_os_memcpy(pPrevWrite, buf, freespace);
            actual_written_len = freespace;
            /* pWrite pointer is moved forward, in case2: it is not necessary to wrap to start position */
            pPrevWrite += actual_written_len;
        }
        ring->pWrite = pPrevWrite;
    }
    return actual_written_len;
}


/*****************************************************************************
 * FUNCTION
 *  RING_ReadData
 * DESCRIPTION
 *  Move data from the ring buffer to caller's buffer.
 *  Once the data is copied, the space in the ring buffer is considered
 *  This RING_ReadData function is used by Sppa_GetBytes():
 *  If the data is copied to the app's provided buffer, the pRead pointer must be updated here!
 *  free.
 * PARAMETERS
 *  ring                    [IN]         Pointer to RingBuf struct
 *  buf                     [IN]         Caller's destination buffer
 *  wanted_to_read_len      [IN]         Max amount to copy
 * RETURNS
 *  amount copied to buf
 *****************************************************************************/
S16 RING_ReadData(RingBuf *ring, char *buf, S16 wanted_to_read_len)
{
    S16 actual_read_len = 0;

    U8 *pPrevWrite = ring->pWrite;
    U8 *pPrevRead = ring->pRead;
    S16 total_data_len = 0;

    /* Report(("In RING_ReadData():pStart=%08x,pEnd=%08x,pRead=%08x,pWrite=%08x",ring->pStart,ring->pEnd,pPrevRead,pPrevWrite)); */
    if (pPrevWrite >= pPrevRead)
    {

        /*
         * case 1 
         *      [             |*********|           ]
         *                 pRead        pWrite   
         */
        S16 readable_len = 0;

        readable_len = pPrevWrite - pPrevRead;
        /* Report(("RING_ReadData(), case1: readable_len=%d",readable_len)); */
        if (readable_len > wanted_to_read_len)
        {
            btmtk_os_memcpy(buf, pPrevRead, wanted_to_read_len);
            actual_read_len = wanted_to_read_len;
            pPrevRead += actual_read_len;
        }
        else
        {
            btmtk_os_memcpy(buf, pPrevRead, readable_len);
            actual_read_len = readable_len;
            pPrevRead += actual_read_len;
        }
        ring->pRead = pPrevRead;
    }
    else
    {
        /*
         *   Case 2:
         *     [********|                |******]
         *              pWrite           pRead  
         */

        /* In order to reduce the complex of context switch, the readable area is only pEnd - pRead */
        S16 first_readable_len = 0;

        total_data_len = RING_DataLen(ring);
        first_readable_len = ring->pEnd - pPrevRead;
        /* Report(("RING_ReadData(), case2: first_readable_len=%d,total_data_len=%d",first_readable_len,total_data_len)); */

        if (first_readable_len >= wanted_to_read_len)
        {
            btmtk_os_memcpy(buf, pPrevRead, wanted_to_read_len);
            actual_read_len = wanted_to_read_len;
            pPrevRead += actual_read_len;

            if (pPrevRead == ring->pEnd)
            {
                pPrevRead = ring->pStart;
            }
        }
        else
        {
            S16 left_wanted_to_read_len = 0, left_available_read_len = 0;

            btmtk_os_memcpy(buf, pPrevRead, first_readable_len);
            actual_read_len = first_readable_len;
            pPrevRead = ring->pStart;

            /* Report(("In RING_ReadData() case2:pStart=%08x,pEnd=%08x,pRead=%08x,pWrite=%08x",ring->pStart,ring->pEnd,pPrevRead,pPrevWrite)); */

            left_wanted_to_read_len = wanted_to_read_len - first_readable_len;
            left_available_read_len = total_data_len - first_readable_len;

            /* Report(("left_wanted_read_len=%d,left_available_data_len=%d,actual_read_len=%d",left_wanted_to_read_len,left_available_read_len,actual_read_len)); */
            if (left_available_read_len < left_wanted_to_read_len)
            {
                btmtk_os_memcpy(buf + actual_read_len, pPrevRead, left_available_read_len);
                pPrevRead += left_available_read_len;
                actual_read_len += left_available_read_len;
            }
            else
            {
                btmtk_os_memcpy(buf + actual_read_len, pPrevRead, left_wanted_to_read_len);
                pPrevRead += left_wanted_to_read_len;
                actual_read_len += left_wanted_to_read_len;
            }
            /* pPrevRead pointer will be wrapped to start position of this buffer */

        }
        ring->pRead = pPrevRead;
    }
    return actual_read_len;
}


/*****************************************************************************
 * FUNCTION
 *  RING_GetDataPtr
 * DESCRIPTION
 *  Get a pointer to the data and the number of bytes. The number of
 *  bytes is for contiguous data only. This function might be used
 *  by a routine that did not have it's own buffer yet wanted access
 *  to data in the ring buffer.
 *  This RING_GetDataPtr function is used by SPPOS_DevTx():
 *  It only points to the first readable data's position, and its readable length
 *  Because the 2nd parameter is BtPacket*, the pRead pointer will be updated after the BtPacket's pointer data copied to uart!
 *  Upon receipt RFEVENT_PACKET_HANDLED, the pRead pointer will be updated via the function RING_BufDelete()
 * PARAMETERS
 *  ring        [IN]         Pointer to ring buffer structure
 *  data        [IN]         Address of pointer that this function will update
 *  len         [IN]         Pointer to number of bytes requested
 * RETURNS
 *  data    contains pointer to beginning of data
 *  len     contains number of bytes available not exceeding original
 *  value; len represents the number of contiguous bytes.
 *****************************************************************************/
void RING_GetDataPtr(RingBuf *ring, char **data, S16 *len)
{
    *data = (char*)ring->pRead;
    *len = min(*len, RING_ContigDataLen(ring));

}


/*****************************************************************************
 * FUNCTION
 *  RING_BufDelete
 * DESCRIPTION
 *  Deletes bytes from the buffer. Bytes are removed starting with
 *  the oldest or "first in" data.
 *  [SPP download file from phone to PC's Phonesuite]
 *  Example: the BT task will invoke this function, when the event RFEVENT_PACKET_HANDLED is reported in defaultEventHandler()
 *  Because the packet's data is already sent to uart, the corresponding data stored in ring buf can be removed
 *  If the RING_DataLen() is invoked in this function, the BT task will be bloced due to waiting the semaphore it already gotten!
 *  From that moment, the BT is blocked and can not to process any messages in queue,
 *  That is why PhoneSuite in PC side only receives one response and then no furthur more rsp is sent back to PhoneSuite
 * PARAMETERS
 *  ring        [IN]         Pointer to ring buffer structure
 *  len         [IN]         Bytes to delete; if len > existing bytes, existing bytes are removed
 * RETURNS
 *  void
 *****************************************************************************/
void RING_BufDelete(RingBuf *ring, S16 len)
{
    U8 *pPrevRead = ring->pRead;

#if 0
    if ((pPrevRead + len) == ring->pEnd)
        /* pPrevRead pointer will be wrapped to start position of this buffer */
    {
        pPrevRead = ring->pStart;
    }
    else
    {
        pPrevRead += len;
    }
#endif
    if ((pPrevRead + len) >= ring->pEnd)
    {
        pPrevRead = ring->pStart + (pPrevRead + len - ring->pEnd);
    }
    else
    {
        pPrevRead += len;
    }

    ring->pRead = pPrevRead;
}


/*****************************************************************************
 * FUNCTION
 *  RING_Size
 * DESCRIPTION
 *  This RING_Size function is only invoked in this file via the function RING_FreeSpace()
 *  Because the invoker task will be obtainded ring buf's semaphore in the RING_FreeSpace() before calling the RING_Size()
 *  If the the same ring buf's semaphore is also put in this function, the invoker task will be blocked
 * PARAMETERS
 *  ring        [IN]
 * RETURNS
 *  
 *****************************************************************************/
S16 RING_Size(RingBuf *ring)
{
    S16 size = 0;

    size = (ring->pEnd - ring->pStart);
    return size;
}


/*****************************************************************************
 * FUNCTION
 *  RING_FreeSpace
 * DESCRIPTION
 *  Because the function RING_FreeSpace() can be invoked by other functions not defined in Ring.c
 *  Originally, this function is also invoked in the Ring.c (e.g., RING_WriteData())
 *  Example: DT task calls the RING_WriteData() to obtain write ring buf's semaphore and then put data in write ring buf,
 *  But if this function is called by the function RING_WriteData(), DT task will be blocked due to not obtaing ringbuf's semaphore again!
 *  [Modification]: Remove the function call RING_FreeSpace() in the function RING_WriteData() and replace them via codes directly
 * PARAMETERS
 *  ring        [IN]
 * RETURNS
 *  
 *****************************************************************************/
S16 RING_FreeSpace(RingBuf *ring)
{
    S16 freespace = 0;
    U8 *pPrevRead = ring->pRead;
    U8 *pPrevWrite = ring->pWrite;

    if (pPrevWrite >= pPrevRead)
    {
        freespace = (ring->pEnd - pPrevWrite) + (pPrevRead - ring->pStart) - 1;
    }
    else
    {
        freespace = pPrevRead - pPrevWrite - 1;
    }
    return freespace;
}


/*****************************************************************************
 * FUNCTION
 *  RING_AvailableDataLen
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ring        [IN]
 * RETURNS
 *  
 *****************************************************************************/
S16 RING_AvailableDataLen(RingBuf *ring)
{
    S16 datalen = 0;
    U8 *pPrevRead = ring->pRead;
    U8 *pPrevWrite = ring->pWrite;

    if (pPrevWrite >= pPrevRead)
    {
        datalen = pPrevWrite - pPrevRead;
    }
    else
    {
        datalen = ring->pEnd - pPrevRead;
    }

    return datalen;

}


/*****************************************************************************
 * FUNCTION
 *  RING_DataLen
 * DESCRIPTION
 *  This RING_DataLen function is invoked in the file bt_adp_hfp.c, bt_adp_spp_vp.c and Ring.c
 * PARAMETERS
 *  ring        [IN]
 * RETURNS
 *  
 *****************************************************************************/
S16 RING_DataLen(RingBuf *ring)
{
    S16 datalen = 0;
    U8 *pPrevRead = ring->pRead;
    U8 *pPrevWrite = ring->pWrite;

    if (pPrevWrite >= pPrevRead)
    {
        datalen = pPrevWrite - pPrevRead;
    }
    else
    {
        datalen = (ring->pEnd - pPrevRead) + (pPrevWrite - ring->pStart);
    }

    return datalen;

}


/*****************************************************************************
 * FUNCTION
 *  RING_ContigDataLen
 * DESCRIPTION
 *  Because this function is called by RING_GetDataPtr():
 *  In RING_GetDataPtr(): the access to RingBuf is protected by ringbuf's semaphore,
 *  In this function RING_ContigDataLen(), it must not be appeared the semaphore protection
 *  Otherwise: the task which invoke RING_GetDataPtr will be blocked in RING_ContigDataLen() due to not getting semaphore!
 *  It is important !  * PARAMETERS
 *  ring        [IN]
 * RETURNS
 *  
 *****************************************************************************/
S16 RING_ContigDataLen(RingBuf *ring)
{
    S16 readable_len = 0;
    U8 *pPrevRead = ring->pRead;
    U8 *pPrevWrite = ring->pWrite;

    if (pPrevWrite >= pPrevRead)
    {
        readable_len = pPrevWrite - pPrevRead;
    }
    else
    {
        readable_len = ring->pEnd - pPrevRead;
    }
    return readable_len;

}


/*****************************************************************************
 * FUNCTION
 *  RING_ReadDataNoDelete
 * DESCRIPTION
 *  Copy data from the ring buffer to caller's buffer.
 *  The data is still stored in the buffer ring.
 *  This API does not update the pRead pointer.
 * PARAMETERS
 *  ring                    [IN]         Pointer to RingBuf struct
 *  buf                     [IN]         Caller's destination buffer
 *  wanted_to_read_len      [IN]         Max amount to copy
 * RETURNS
 *  amount copied to buf
 *****************************************************************************/
S16 RING_ReadDataNoDelete(RingBuf *ring, char *buf, S16 wanted_to_read_len)
{
    S16 actual_read_len = 0;

    U8 *pPrevWrite = ring->pWrite;
    U8 *pPrevRead = ring->pRead;
    S16 total_data_len = RING_DataLen(ring);

    /* Report(("In RING_ReadData():pStart=%08x,pEnd=%08x,pRead=%08x,pWrite=%08x",ring->pStart,ring->pEnd,pPrevRead,pPrevWrite)); */
    if (pPrevWrite >= pPrevRead)
    {

        /*
         * case 1 
         *      [             |*********|           ]
         *                 pRead        pWrite   
         */
        S16 readable_len = 0;

        readable_len = pPrevWrite - pPrevRead;
        /* Report(("RING_ReadData(), case1: readable_len=%d",readable_len)); */
        if (readable_len > wanted_to_read_len)
        {
            btmtk_os_memcpy(buf, pPrevRead, wanted_to_read_len);
            actual_read_len = wanted_to_read_len;
            pPrevRead += actual_read_len;
        }
        else
        {
            btmtk_os_memcpy(buf, pPrevRead, readable_len);
            actual_read_len = readable_len;
            pPrevRead += actual_read_len;
        }
    }
    else
    {
        /*
         *   Case 2:
         *     [********|                |******]
         *              pWrite           pRead  
         */

        /* In order to reduce the complex of context switch, the readable area is only pEnd - pRead */
        S16 first_readable_len = 0;

        //total_data_len = RING_DataLen(ring);
        first_readable_len = ring->pEnd - pPrevRead;
        /* Report(("RING_ReadData(), case2: first_readable_len=%d,total_data_len=%d",first_readable_len,total_data_len)); */

        if (first_readable_len >= wanted_to_read_len)
        {
            btmtk_os_memcpy(buf, pPrevRead, wanted_to_read_len);
            actual_read_len = wanted_to_read_len;
            pPrevRead += actual_read_len;

            if (pPrevRead == ring->pEnd)
            {
                pPrevRead = ring->pStart;
            }
        }
        else
        {
            S16 left_wanted_to_read_len = 0, left_available_read_len = 0;

            btmtk_os_memcpy(buf, pPrevRead, first_readable_len);
            actual_read_len = first_readable_len;
            pPrevRead = ring->pStart;

            /* Report(("In RING_ReadData() case2:pStart=%08x,pEnd=%08x,pRead=%08x,pWrite=%08x",ring->pStart,ring->pEnd,pPrevRead,pPrevWrite)); */

            left_wanted_to_read_len = wanted_to_read_len - first_readable_len;
            left_available_read_len = total_data_len - first_readable_len;

            /* Report(("left_wanted_read_len=%d,left_available_data_len=%d,actual_read_len=%d",left_wanted_to_read_len,left_available_read_len,actual_read_len)); */
            if (left_available_read_len < left_wanted_to_read_len)
            {
                btmtk_os_memcpy(buf + actual_read_len, pPrevRead, left_available_read_len);
                pPrevRead += left_available_read_len;
                actual_read_len += left_available_read_len;
            }
            else
            {
                btmtk_os_memcpy(buf + actual_read_len, pPrevRead, left_wanted_to_read_len);
                pPrevRead += left_wanted_to_read_len;
                actual_read_len += left_wanted_to_read_len;
            }
            /* pPrevRead pointer will be wrapped to start position of this buffer */

        }
    }
    return actual_read_len;
}


#endif /* __SPP_ORG_TRANSMIT_METHOD__ */ 

