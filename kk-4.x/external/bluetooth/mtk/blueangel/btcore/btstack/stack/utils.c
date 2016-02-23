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
 *     $Workfile:utils.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains utility function code for stacks
 *     and add-in components. Including functions for list 
 *     manipulation. These functions may be mapped to OS function 
 *     calls in some instances.
 *
 * Created:
 *     June 20, 1995
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

#include "bt_common.h"
#include "osapi.h"
#include "utils.h"
#include "btalloc.h"

#if XA_USE_ENDIAN_MACROS == XA_DISABLED


/*****************************************************************************
 * FUNCTION
 *  BEtoHost16
 * DESCRIPTION
 *  Retrieve a 16-bit number from the given buffer. The number
 *  is in Big-Endian format.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  16-bit number.
 *****************************************************************************/
U16 BEtoHost16(const U8 *ptr)
{
    return (U16) (((U16) * ptr << 8) | ((U16) * (ptr + 1)));
}


/*****************************************************************************
 * FUNCTION
 *  BEtoHost32
 * DESCRIPTION
 *  Retrieve a 32-bit number from the given buffer. The number
 *  is in Big-Endian format.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  32-bit number.
 *****************************************************************************/
U32 BEtoHost32(const U8 *ptr)
{
    return (U32) (((U32) * ptr << 24) | ((U32) * (ptr + 1) << 16) | ((U32) * (ptr + 2) << 8) | ((U32) * (ptr + 3)));
}


/*****************************************************************************
 * FUNCTION
 *  BEtoHost64
 * DESCRIPTION
 *  Retrieve a 64-bit number from the given buffer. The number
 *  is in Big-Endian format.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  32-bit number.
 *****************************************************************************/
U64 BEtoHost64(const U8 *ptr)
{
    return (U64) (((U64) * ptr << 56) | 
                  ((U64) * (ptr + 1) << 48) | 
                  ((U64) * (ptr + 2) << 40) | 
                  ((U64) * (ptr + 3) << 32) |
                  ((U64) * (ptr + 4) << 24) | 
                  ((U64) * (ptr + 5) << 16) | 
                  ((U64) * (ptr + 6) << 8) | 
                  ((U64) * (ptr + 7)));
}


/*****************************************************************************
 * FUNCTION
 *  LEtoHost16
 * DESCRIPTION
 *  Retrieve a 16-bit number from the given buffer. The number
 *  is in Little-Endian format.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  16-bit number.
 *****************************************************************************/
U16 LEtoHost16(const U8 *ptr)
{
    return (U16) (((U16) * (ptr + 1) << 8) | ((U16) * ptr));
}


/*****************************************************************************
 * FUNCTION
 *  LEtoHost32
 * DESCRIPTION
 *  Retrieve a 32-bit number from the given buffer. The number
 *  is in Little-Endian format.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  32-bit number.
 *****************************************************************************/
U32 LEtoHost32(const U8 *ptr)
{
    return (U32) (((U32) * (ptr + 3) << 24) | ((U32) * (ptr + 2) << 16) | ((U32) * (ptr + 1) << 8) | ((U32) * (ptr)));
}


/*****************************************************************************
 * FUNCTION
 *  LEtoHost64
 * DESCRIPTION
 *  Retrieve a 64-bit number from the given buffer. The number
 *  is in Little-Endian format.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  32-bit number.
 *****************************************************************************/
U64 LEtoHost64(const U8 *ptr)
{
    return (U64) (((U64) * (ptr + 7) << 56) | 
                  ((U64) * (ptr + 6) << 48) | 
                  ((U64) * (ptr + 5) << 40) | 
                  ((U64) * (ptr + 4) << 32) | 
                  ((U64) * (ptr + 3) << 24) | 
                  ((U32) * (ptr + 2) << 16) | 
                  ((U32) * (ptr + 1) << 8) | 
                  ((U32) * (ptr)));
}


/*****************************************************************************
 * FUNCTION
 *  StoreLE16
 * DESCRIPTION
 *  Store 16 bit value into a buffer in Little Endian format.
 * PARAMETERS
 *  buff            [OUT]
 *  le_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void StoreLE16(U8 *buff, U16 le_value)
{
    buff[1] = (U8) (le_value >> 8);
    buff[0] = (U8) le_value;
}


/*****************************************************************************
 * FUNCTION
 *  StoreLE32
 * DESCRIPTION
 *  Store 32 bit value into a buffer in Little Endian format.
 * PARAMETERS
 *  buff            [OUT]
 *  le_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void StoreLE32(U8 *buff, U32 le_value)
{
    buff[3] = (U8) (le_value >> 24);
    buff[2] = (U8) (le_value >> 16);
    buff[1] = (U8) (le_value >> 8);
    buff[0] = (U8) le_value;
}


/*****************************************************************************
 * FUNCTION
 *  StoreLE64
 * DESCRIPTION
 *  Store 64 bit value into a buffer in Little Endian format.
 * PARAMETERS
 *  buff            [OUT]
 *  le_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void StoreLE64(U8 *buff, U64 le_value)
{
    buff[7] = (U8) (le_value >> 56);
    buff[6] = (U8) (le_value >> 48);
    buff[5] = (U8) (le_value >> 40);
    buff[4] = (U8) (le_value >> 32);
    buff[3] = (U8) (le_value >> 24);
    buff[2] = (U8) (le_value >> 16);
    buff[1] = (U8) (le_value >> 8);
    buff[0] = (U8) le_value;
}


/*****************************************************************************
 * FUNCTION
 *  StoreBE16
 * DESCRIPTION
 *  Store 16 bit value into a buffer in Big Endian format.
 * PARAMETERS
 *  buff            [OUT]
 *  be_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void StoreBE16(U8 *buff, U16 be_value)
{
    buff[0] = (U8) (be_value >> 8);
    buff[1] = (U8) be_value;
}


/*****************************************************************************
 * FUNCTION
 *  StoreBE32
 * DESCRIPTION
 *  Store 32 bit value into a buffer in Big Endian format.
 * PARAMETERS
 *  buff            [OUT]
 *  be_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void StoreBE32(U8 *buff, U32 be_value)
{
    buff[0] = (U8) (be_value >> 24);
    buff[1] = (U8) (be_value >> 16);
    buff[2] = (U8) (be_value >> 8);
    buff[3] = (U8) be_value;
}

/*****************************************************************************
 * FUNCTION
 *  StoreBE64
 * DESCRIPTION
 *  Store 64 bit value into a buffer in Big Endian format.
 * PARAMETERS
 *  buff            [OUT]
 *  be_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void StoreBE64(U8 *buff, U64 be_value)
{
    buff[0] = (U8) (be_value >> 56);
    buff[1] = (U8) (be_value >> 48);
    buff[2] = (U8) (be_value >> 40);
    buff[3] = (U8) (be_value >> 32);
    buff[4] = (U8) (be_value >> 24);
    buff[5] = (U8) (be_value >> 16);
    buff[6] = (U8) (be_value >> 8);
    buff[7] = (U8) be_value;
}

#endif /* XA_USE_ENDIAN_MACROS == XA_DISABLED */


/*****************************************************************************
 * FUNCTION
 *  _InsertTailList
 * DESCRIPTION
 *  Insert an entry at the tail of the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void _InsertTailList(ListEntry *head, ListEntry *entry)
{
    entry->Flink = head;
    entry->Blink = head->Blink;
    head->Blink->Flink = entry;
    head->Blink = entry;
    Assert(IsNodeConnected(entry));

}


/*****************************************************************************
 * FUNCTION
 *  InsertTailList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void InsertTailList(ListEntry *head, ListEntry *entry)
{
    Assert(IsListCircular(head));
    _InsertTailList(head, entry);
    Assert(IsListCircular(head));
}


/*****************************************************************************
 * FUNCTION
 *  _InsertHeadList
 * DESCRIPTION
 *  Insert an entry at the head of the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void _InsertHeadList(ListEntry *head, ListEntry *entry)
{
    entry->Flink = head->Flink;
    entry->Blink = head;
    head->Flink->Blink = entry;
    head->Flink = entry;
    Assert(IsNodeConnected(entry));

}


/*****************************************************************************
 * FUNCTION
 *  _RemoveHeadList
 * DESCRIPTION
 *  Remove the first entry on the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 * RETURNS
 *  
 *****************************************************************************/
ListEntry *_RemoveHeadList(ListEntry *head)
{
    ListEntry *first;

    first = head->Flink;
    first->Flink->Blink = head;
    head->Flink = first->Flink;
    Assert(IsListCircular(head));
    return (first);

}


/*****************************************************************************
 * FUNCTION
 *  RemoveHeadList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  head        [OUT]     
 * RETURNS
 *  
 *****************************************************************************/
ListEntry *RemoveHeadList(ListEntry *head)
{
    Assert(IsListCircular(head));
    return (_RemoveHeadList(head));
}


/*****************************************************************************
 * FUNCTION
 *  RemoveEntryList
 * DESCRIPTION
 *  Remove the given entry from the list.
 * PARAMETERS
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void RemoveEntryList(ListEntry *entry)
{
    Assert(IsListCircular(entry));
    entry->Blink->Flink = entry->Flink;
    entry->Flink->Blink = entry->Blink;
    Assert(IsListCircular(entry->Blink));
    InitializeListEntry(entry);

}


/*****************************************************************************
 * FUNCTION
 *  IsNodeOnList
 * DESCRIPTION
 *  Determine if an entry is on the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  node        [OUT]     
 * RETURNS
 *  TRUE - the entry is on the list.
 *  FALSE - the entry is not on the list.
 *****************************************************************************/
BOOL IsNodeOnList(ListEntry *head, ListEntry *node)
{
    ListEntry *tmpNode;

    Assert(IsListCircular(head));
    tmpNode = GetHeadList(head);

    while (tmpNode != head)
    {
        if (tmpNode == node)
        {
            return (TRUE);
        }

        tmpNode = tmpNode->Flink;
    }
    return (FALSE);

}


/*****************************************************************************
 * FUNCTION
 *  MoveList
 * DESCRIPTION
 *  Moves a list to a new list head.  The source list is left empty.
 * PARAMETERS
 *  dest        [OUT]     
 *  src         [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void MoveList(ListEntry *dest, ListEntry *src)
{
    Assert(IsListCircular(src));
    if(IsListEmpty(src)){
        InitializeListHead(dest);
    }else{
    dest->Flink = src->Flink;
    dest->Blink = src->Blink;
    src->Flink->Blink = dest;
    src->Blink->Flink = dest;
    InitializeListHead(src);
}
}

/*****************************************************************************
 * FUNCTION
 *  InsertList
 * DESCRIPTION
 *  Insert a list to another list.  The source list is left empty.
 * PARAMETERS
 *  dest        [OUT]
 *  src         [OUT]
 *  front       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void InsertList(ListEntry *dest, ListEntry *src, U8 front)
{
    Assert(IsListCircular(src));
    Assert(IsListCircular(dest));
    if(!IsListEmpty(src)){
        
    }else{
        if(front){
            dest->Flink->Blink = src->Blink;
            src->Blink->Flink = dest->Flink;
            dest->Flink = src->Flink;
            src->Flink->Blink = dest;
        }else{
            
            dest->Blink->Flink = src->Flink;
            src->Flink->Blink = dest->Blink;
            dest->Blink = src->Blink;
            src->Blink->Flink = dest; 
        }
         InitializeListHead(src);
    }
}


/*****************************************************************************
 * FUNCTION
 *  IsListCircular
 * DESCRIPTION
 *  Determine if the list is circular. This is only used for
 *  debugging purposes.
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  TRUE - the list is circular
 *  FALSE - the list is not circular.
 *****************************************************************************/
BOOL IsListCircular(ListEntry *list)
{

    ListEntry *tmp = list;

    if (!IsNodeConnected(list))
    {
        return (FALSE);
    }

    for (tmp = tmp->Flink; tmp != list; tmp = tmp->Flink)
    { 
        if (!IsNodeConnected(tmp))
        {
            return (FALSE);
        }
    }
    return (TRUE);

}


/*****************************************************************************
 * FUNCTION
 *  InitializeListHead
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void InitializeListHead(ListEntry *list)
{
    list->Flink = list;
    list->Blink = list;
}


/*****************************************************************************
 * FUNCTION
 *  InitializeListEntry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void InitializeListEntry(ListEntry *list)
{
    list->Flink = 0;
    list->Blink = 0;
}


/*****************************************************************************
 * FUNCTION
 *  IsListEmpty
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  
 *****************************************************************************/
U8 IsListEmpty(ListEntry *list)
{
    return ((list)->Flink == (list));
}


/*****************************************************************************
 * FUNCTION
 *  getMECChipVersion
 * DESCRIPTION
 *  
 * PARAMETERS
 *      
 * RETURNS
 *  
 *****************************************************************************/
U8 getMECChipVersion()
{
    return MEC(btChipVer);		
}


/*****************************************************************************
 * FUNCTION
 *  getMECbtLMPVer
 * DESCRIPTION
 *  
 * PARAMETERS
 *      
 * RETURNS
 *  
 *****************************************************************************/
U8 getMECbtLMPVer()
{
    return MEC(btLMPVer);		
}

/*****************************************************************************
 * FUNCTION
 *  getMECbdaddrInx
 * DESCRIPTION
 *  
 * PARAMETERS
 *      
 * RETURNS
 *  
 *****************************************************************************/
U8 getMECbdaddrInx(U8 inx)
{
    return MEC(bdaddr).addr[inx];		
}

#if XA_DEBUG == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  VerifyMacros
 * DESCRIPTION
 *  Determine if the ContainingRecord() macro is functioning
 *  properly.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void VerifyMacros(void)
{
    struct MacroTests
    {
        U8 f1;
        U16 f2[3];
        U8 f3;
        U32 f4;
    } mt;

    /*
     * The following Asserts verify the ContainingRecord() macro.
     * If an assert fails, there is a problem with your compilers handling
     * of the macro. A macro compatible with your compiler should be placed
     * in the overide.h file to override the default version found in config.h.
     */
    Assert(ContainingRecord(&mt.f3, struct MacroTests, f3) == &mt);
    Assert(ContainingRecord(&mt.f4, struct MacroTests, f3) != &mt);
}

#endif /* XA_DEBUG == XA_ENABLED */ 

