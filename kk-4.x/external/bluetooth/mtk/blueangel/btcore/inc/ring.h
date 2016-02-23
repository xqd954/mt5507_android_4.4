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
 *     $Workfile:ring.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Header file for ring buffer management (ring.c)
 *
 * Created:
 *     February 29, 2000
 *
 * Copyright 2000-2005 Extended Systems, Inc.
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
#ifndef _RING_H
#define _RING_H

#include "xatypes.h"

/****************************************************************************
 *
 * Ring Buffer Management layer
 *
 * Ring Buffer Management is a collection of utilities for managing 
 * circular - or ring - buffers. It is targeted for those platforms that
 * do not have their own ring buffer libraries.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Configuration Constants
 *
 ****************************************************************************/
#if 0
#define __SPP_ORG_TRANSMIT_METHOD__
#endif /* 0 */ 

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RingBuf structure
 *      This structure contains all fields required to manipulate a singe
 *      ring buffer. All fields are private and should not be modified.
 */
typedef struct
{
#ifdef __SPP_ORG_TRANSMIT_METHOD__
    /* === Internal use only === */
    U8 *start;      /* 1st byte in ring buffer */
    U8 *end;        /* last byte in ring buffer + 1 */
    U8 *data;       /* 1st byte of date to be removed */
    U8 *free;       /* 1st byte of free space */
    S16 dataLen;    /* data bytes currently in buffer */
#else /* __SPP_ORG_TRANSMIT_METHOD__ */ 
    U8 *pStart;
    U8 *pEnd;
    U8 *pRead;
    U8 *pWrite;
#endif /* __SPP_ORG_TRANSMIT_METHOD__ */ 
} RingBuf;

/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RING_BufInit()
 * Initialize a ring buffer structure. This function must be called before
 * any other ring buffer API.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *      buf  - allocated buffer which will become the ring buffer
 *      len  - length of buf
 */

void RING_BufInit(RingBuf *ring, char buf[], S16 len);

/*---------------------------------------------------------------------------
 * RING_BufFlush() 
 *      Clear data from a ring buffer.
 *
 * Parameters:
 *      Pointer to RingBuf struct.
 */
void RING_BufFlush(RingBuf *ring);

/*---------------------------------------------------------------------------
 * RING_WriteData()
 * Copy data into the ring buffer from caller's buffer.
 * If there is not enough free space to copy the caller's entire buffer,
 * no data at all is copied.
 *
 * Parameters
 *      ring - pointer to RingBuf struct
 *      buf  - caller's destination buffer
 *      len  - max amount to copy
 *  Returns: 
 *          actual amount copied to buf
 */
S16 RING_WriteData(RingBuf *ring, char buf[], S16 len);

/*---------------------------------------------------------------------------
 * RING_ReadData()
 * Move data from the ring buffer to caller's buffer.
 * Once the data is copied, the space in the ring buffer is considered
 * free.
 *
 * Parameters: 
 *      ring - pointer to RingBuf struct
 *      buf  - caller's destination buffer
 *      len  - max amount to copy
 *  Returns: actual amount copied to buf
 */
S16 RING_ReadData(RingBuf *ring, char buf[], S16 len);

/*---------------------------------------------------------------------------
 * RING_Size()
 * Determine the size in bytes of a ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *                  
 *  Returns: size in bytes of the buffer
 */

S16 RING_Size(RingBuf *ring);

#ifdef __SPP_ORG_TRANSMIT_METHOD__
#define RING_Size(ring)          (ring->end - ring->start)
#endif 

/*---------------------------------------------------------------------------
 * RING_FreeSpace()
 * Determine the free space in the ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *                  
 *  Returns: 
 *      amount of free space in the buffer
 */
S16 RING_FreeSpace(RingBuf *ring);

#ifdef __SPP_ORG_TRANSMIT_METHOD__
#define RING_FreeSpace(ring)     (RING_Size(ring) - ring->dataLen)
#endif 

/*---------------------------------------------------------------------------
 * RING_DataLen()
 * Determine length of data in the ring buffer.
 * The value returned reflects the entire length including wrap-around.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *
 *  Returns: 
 *      amount of data in the buffer
 */
S16 RING_DataLen(RingBuf *ring);

#ifdef __SPP_ORG_TRANSMIT_METHOD__
#define RING_DataLen(ring)       ring->dataLen
#endif 

/*---------------------------------------------------------------------------
 * RING_ContigDataLen()
 * Determine length of contiguous data in the ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *
 *  Returns: 
 *      amount of contiguous data in the buffer
 */
S16 RING_ContigDataLen(RingBuf *ring);

#ifdef __SPP_ORG_TRANSMIT_METHOD__
#define RING_ContigDataLen(ring) ((ring)->dataLen > (U16)((ring)->end - (ring)->data) \
                                 ? ((ring)->end - (ring)->data) : (ring)->dataLen)
#endif /* __SPP_ORG_TRANSMIT_METHOD__ */ 

/*---------------------------------------------------------------------------
 * RING_GetDataPtr() 
 *      Get a pointer to the data and the number of bytes. The number of
 *      bytes is for contiguous data only. This function might be used
 *      by a routine that did not have it's own buffer yet wanted access
 *      to data in the ring buffer.
 *
 * Parameters:
 *      ring - pointer to RingBuf struct
 *      data - address of character pointer
 *      len  - address of an int
 *
 * Returns:
 *      *data is updated to poS16 to the beginning of data; *len is updated
 *      with the number of bytes of data. *len is zero if no data is 
 *      available. 
 */
void RING_GetDataPtr(RingBuf *ring, char **data, S16 *len);

/*---------------------------------------------------------------------------
 * RING_BufDelete() 
 *      Deletes bytes from the buffer. Bytes are removed starting with
 *      the oldest or "first in" data.
 *
 * Parameters:
 *      ring - pointer to ring buffer structure
 *      len  - bytes to delete; if len > existing bytes, existing bytes
 *             are removed
 */
void RING_BufDelete(RingBuf *ring, S16 len);

/*---------------------------------------------------------------------------
 * RING_ReadDataNoDelete()
 * Copy data from the ring buffer to caller's buffer.
 * The data is still stored in the buffer ring.
 *
 * Parameters: 
 *      ring - pointer to RingBuf struct
 *      buf  - caller's destination buffer
 *      len  - max amount to copy
 *  Returns: actual amount copied to buf
 */
S16 RING_ReadDataNoDelete(RingBuf *ring, char buf[], S16 len);


#endif /* _RING_H */ 

