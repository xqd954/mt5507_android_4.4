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
*  permission of MediaTek Inc. (C) 2006
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * a2dputil.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file implements bt a2dp utility function
 *
 * Author:
 * -------
 * Tim Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * Sep 3 2009 mtk00968
 * [MAUI_01949824] A2DP Restructure
 * A2DP Restructure
 *
 * Sep 3 2009 mtk00968
 * [MAUI_01949824] A2DP Restructure
 * A2DP Restructure
 *
 * May 11 2009 mtk01393
 * [MAUI_01745558] A8box_it the A8box can't play any more.
 * 
 *
 * Jun 20 2007 mtk01393
 * [MAUI_00407244] [A2DP] fix Non_A2DP codec can not proceed problem
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Sep 22 2006 mtk00798
 * [MAUI_00331575] [New Feature] MTK BT A2DP with MP3 and SBC streaming capabilities
 * 
 *
 * Jun 27 2006 mtk00798
 * [MAUI_00206133] Patch for A2DP register SDP record bug
 * 
 *
 * May 22 2006 mtk00798
 * [MAUI_00195645] Add BT MTK A2DP feature
 * 
 *
 * May 22 2006 mtk00798
 * [MAUI_00195645] Add BT MTK A2DP feature
 * 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_A2DP_PROFILE__

#include "xatypes.h"
#include "bttypes.h"
#include "a2dp.h"
#include "bluetooth_a2dp_struct.h"
#include "avdtp.h"


#define AFTER(n) ((U8*)(n) + sizeof(mem_block) + (n)->header.rsize)
#define ADJACENT(n1,n2) ((U8*)(n1) + sizeof(mem_block) + (n1)->header.rsize == (U8*)(n2))
#define AVAILABLE 0
#define ALLOCATED 1

typedef union _mem_block
{
    U32 align;
    struct
    {
        U32 rsize;   /* real size */
        U32 usize;   /* user size */
        union _mem_block *next;
    } header;
} mem_block;

static mem_block *available = NULL;
static mem_block *allocated = NULL;
static unsigned long total = 0;

/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_cmp_bt_device_addr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bt_addr1    [IN]
 *  bt_addr2    [IN]
 * RETURNS
 *  BOOL
 *****************************************************************************/
BOOL bt_a2dp_cmp_bt_device_addr(bt_addr_struct *bt_addr1, bt_addr_struct *bt_addr2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL result;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    

    if( (bt_addr1->lap & 0x00FFFFFF) == (bt_addr2->lap & 0x00FFFFFF) &&
        bt_addr1->uap == bt_addr2->uap && bt_addr1->nap == bt_addr2->nap)
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return result;
}

/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_get_bd_addr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src     [?]     
 *  dst     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_get_bd_addr(bt_addr_struct *src, BD_ADDR *dst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemCopy((U8 *)&dst->addr[0], (U8 *)&src->lap, 3);
    dst->addr[3] = src->uap;
    OS_MemCopy((U8 *)&dst->addr[4], (U8 *)&src->nap, 2);
}


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_get_bt_device_addr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src     [?]     
 *  dst     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_get_bt_device_addr(BD_ADDR *src, bt_addr_struct *dst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    dst->lap = 0; // clear the MSB
    OS_MemCopy((U8 *)&dst->lap, (U8 *)&src->addr[0], 3);
    dst->uap = src->addr[3];
    OS_MemCopy((U8 *)&dst->nap, (U8 *)&src->addr[4], 2);
}


/*****************************************************************************
 * FUNCTION
 *  remove_mem_block
 * DESCRIPTION
 *  
 * PARAMETERS
 *  type        [IN]        
 *  n           [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void remove_mem_block(U8 type, mem_block *n)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mem_block *c, *p, **head;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    head = (type == ALLOCATED) ? &allocated : &available;

    c = p = *head;
    while (c != NULL && (U8*) c > (U8*) n)
    {
        p = c;
        c = c->header.next;
    }

    if (c == p)
    {
        *head = c->header.next;
    }
    else
    {
        p->header.next = c->header.next;
    }
}


/*****************************************************************************
 * FUNCTION
 *  insert_mem_block
 * DESCRIPTION
 *  
 * PARAMETERS
 *  type        [IN]        
 *  n           [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void insert_mem_block(U8 type, mem_block *n)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mem_block *c, *p, **head;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    head = (type == ALLOCATED) ? &allocated : &available;

    c = p = *head;
    while (c != NULL && (U8*) c > (U8*) n)
    {
        p = c;
        c = c->header.next;
    }

    if ((type == AVAILABLE) && (*head != NULL) && (c != NULL) && ADJACENT(c, n))
    {
        c->header.rsize += sizeof(mem_block) + n->header.rsize;
        if (ADJACENT(c, p))
        {
            remove_mem_block(AVAILABLE, p);
            c->header.rsize += sizeof(mem_block) + p->header.rsize;
        }
    }
    else
    {
        n->header.next = c;
        if (c == p)
        {
            *head = n;
        }
        else
        {
            p->header.next = n;
            if ((type == AVAILABLE) && (*head != NULL) && ADJACENT(n, p))
            {
                remove_mem_block(AVAILABLE, p);
                n->header.rsize += sizeof(mem_block) + p->header.rsize;
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_mem_alloc
 * DESCRIPTION
 *  
 * PARAMETERS
 *  size        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 *bt_a2dp_mem_alloc(U32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mem_block *c, *n;
    U32 leftover, aligned_size;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (size == 0)
    {
        return NULL;
    }

    aligned_size = (size + 3) & ~3;

    for (c = available; c != NULL && c->header.rsize < aligned_size; c = c->header.next)
    {
    }

    if (c == NULL)
    {
        return NULL;
    }

    remove_mem_block(AVAILABLE, c);

    if (c->header.rsize >= aligned_size + sizeof(mem_block) + 256)
    {
        leftover = c->header.rsize - aligned_size - sizeof(mem_block);
        c->header.rsize = aligned_size;

        n = (mem_block*) ((U8*) c + sizeof(mem_block) + c->header.rsize);
        n->header.rsize = leftover;

        insert_mem_block(AVAILABLE, n);
    }

    c->header.usize = size;

    insert_mem_block(ALLOCATED, c);

    if (c == NULL)
    {
        return NULL;
    }

    total += size;

    return (U8*) c + sizeof(mem_block);
}


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_mem_free
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_mem_free(U8 *ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mem_block *c;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(ptr != NULL);

    for (c = allocated; c != NULL && (U8*) c + sizeof(mem_block) > ptr; c = c->header.next)
    {
    }

    ASSERT(c != NULL);

    ASSERT((U8*) c + sizeof(mem_block) == ptr);

    remove_mem_block(ALLOCATED, c);
    insert_mem_block(AVAILABLE, c);

    total -= c->header.usize;
}


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_mem_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mem_addr        [?]         
 *  size            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_mem_init(U8 *mem_addr, U32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mem_block *n = (mem_block*) mem_addr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    n->header.rsize = size - sizeof(mem_block);
    n->header.next = NULL;

    allocated = NULL;
    available = n;
    total = 0;
}


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_mem_get_alloc_size
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U32 bt_a2dp_mem_get_alloc_size(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return total;
}

#endif /* __BT_A2DP_PROFILE__ */

