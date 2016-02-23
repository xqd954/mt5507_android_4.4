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
 *     $Workfile:rxbuff.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains receive buffers management routines for the HCI 
 *     transport.
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
#include "btalloc.h"


/*****************************************************************************
 * FUNCTION
 *  RXBUFF_Init
 * DESCRIPTION
 *  Initializes receive buffer memory and lists.  This should be
 *  called during stack initialization.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RXBUFF_Init(void)
{
    U8 *ptr;
    I8 i;

    /* Initialize RX/TX queues */
#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS != 0)
    InitializeListHead(&(RXBUF(rxSCOFreeList)));
#endif /* NUM_SCO_CONNS != 0 */
    InitializeListHead(&(RXBUF(rxACLFreeList)));
    InitializeListHead(&(RXBUF(rxEvtFreeList)));

#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS != 0)
    /* Init SCO RX buffers */
    ptr = RXBUF(rxSCOBuffer);
    for (i = 0; i < TRANS_NUM_SCO_BUFFERS; i++)
    {
        RXBUF(rxSCOBuffers)[i].flags |= HCI_BUFTYPE_SCO_DATA;
        RXBUF(rxSCOBuffers)[i].buffer = ptr;
        InsertTailList(&(RXBUF(rxSCOFreeList)), &(RXBUF(rxSCOBuffers)[i].node));
        ptr += TRANS_MAX_SCO_BUFF_SIZE;
    }
#endif /* NUM_SCO_CONNS != 0 */

    /* Init ACL RX buffers */
    ptr = RXBUF(rxACLBuffer);
    for (i = 0; i < TRANS_NUM_ACL_BUFFERS; i++)
    {
        RXBUF(rxACLBuffers)[i].flags |= HCI_BUFTYPE_ACL_DATA;
        RXBUF(rxACLBuffers)[i].buffer = ptr;
        InsertTailList(&(RXBUF(rxACLFreeList)), &(RXBUF(rxACLBuffers)[i].node));
        ptr += TRANS_MAX_ACL_BUFF_SIZE;
    }

    /* Init Event RX buffers */
    ptr = RXBUF(rxEvtBuffer);
    for (i = 0; i < HCI_NUM_EVENTS; i++)
    {
        RXBUF(rxEvtBuffers)[i].flags |= HCI_BUFTYPE_EVENT;
        RXBUF(rxEvtBuffers)[i].buffer = ptr;
        InsertTailList(&(RXBUF(rxEvtFreeList)), &(RXBUF(rxEvtBuffers)[i].node));
        ptr += TRANS_EVENT_BUFF_SIZE;
    }

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  RXBUFF_Alloc
 * DESCRIPTION
 *  Allocates a receive buffer and prepares it to receive data.
 *  This function is intended to be called at interrupt time or
 *  with interrupts disabled (OS_StopHardware()).
 * PARAMETERS
 *  rxBuffHandle        [IN]
 *  hciConnHandle       [IN]        
 *  len                 [IN]        
 *  type                [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - the buffer was allocated successfully.
 *  BT_STATUS_NO_RESOURCES - a buffer is not available
 *  BT_STATUS_INVALID_PARM - one of the parameters contained an
 *  invalid value.
 *****************************************************************************/
BtStatus RXBUFF_Alloc(RxBuffHandle *rxBuffHandle, HciConnHandle hciConnHandle, U16 len, U8 type)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    HciBuffer *hciBuffer = 0;

    /* UNUSED_PARAMETER(hciConnHandle); */

    switch (type)
    {
        case HCI_BUFTYPE_ACL_DATA:

            if (len > TRANS_MAX_ACL_BUFF_SIZE)
            {
                status = BT_STATUS_INVALID_PARM;
                break;
            }

            if (!IsListEmpty(&(RXBUF(rxACLFreeList))))
            {
                XASTAT_PeakInc(hci_num_acl_buffers, 1);
                XASTAT_PeakSet(hci_acl_buff_size, len);
                hciBuffer = (HciBuffer*) RemoveHeadList(&(RXBUF(rxACLFreeList)));
                hciBuffer->flags |= HCI_BUFTYPE_ACL_DATA;
                status = BT_STATUS_SUCCESS;
            }
        #if XA_STATISTICS == XA_ENABLED
            else
            {
                XASTAT_CounterInc(hci_num_no_acl_rxbuf, 1);
            }
        #endif /* XA_STATISTICS == XA_ENABLED */ 
            break;

        #if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS != 0)
        case HCI_BUFTYPE_SCO_DATA:

            if (len > TRANS_MAX_SCO_BUFF_SIZE)
            {
                status = BT_STATUS_INVALID_PARM;
                break;
            }

            if (!IsListEmpty(&(RXBUF(rxSCOFreeList))))
            {
                XASTAT_PeakInc(hci_num_sco_buffers, 1);
                XASTAT_PeakSet(hci_sco_buff_size, len);
                hciBuffer = (HciBuffer*) RemoveHeadList(&(RXBUF(rxSCOFreeList)));
                hciBuffer->flags |= HCI_BUFTYPE_SCO_DATA;
                status = BT_STATUS_SUCCESS;
            }
        #if XA_STATISTICS == XA_ENABLED
            else
            {
                XASTAT_CounterInc(hci_num_no_sco_rxbuf, 1);
            }
        #endif /* XA_STATISTICS == XA_ENABLED */ 
            break;
        #endif /* (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS != 0) */
        case HCI_BUFTYPE_EVENT:

            if (len > TRANS_EVENT_BUFF_SIZE)
            {
                status = BT_STATUS_INVALID_PARM;
                break;
            }

            if (!IsListEmpty(&(RXBUF(rxEvtFreeList))))
            {
                XASTAT_PeakInc(hci_num_event_buffers, 1);
                hciBuffer = (HciBuffer*) RemoveHeadList(&(RXBUF(rxEvtFreeList)));
                hciBuffer->flags |= HCI_BUFTYPE_EVENT;
                status = BT_STATUS_SUCCESS;
            }
            break;

        default:
            status = BT_STATUS_INVALID_PARM;
            break;
    }

    if (status == BT_STATUS_SUCCESS)
    {
        XASTAT_TimerStart(hci_rx_timer, hciBuffer->hci_timer);
        hciBuffer->len = len;
        *rxBuffHandle = hciBuffer;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RXBUFF_Free
 * DESCRIPTION
 *  Frees a buffer.  The buffer must have been allocated previously
 *  by a call to RXBUFF_Alloc().  This function is intended to be
 *  called at interrupt time or with interrupts disabled
 *  (OS_StopHardware()).
 * PARAMETERS
 *  rxBuffHandle        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RXBUFF_Free(RxBuffHandle rxBuffHandle)
{
    HciBuffer *hciBuffer = rxBuffHandle;

    XASTAT_TimerStop(hciBuffer->len, hci_rx_timer, hciBuffer->hci_timer);

#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS != 0)
    if (hciBuffer->flags & HCI_BUFTYPE_SCO_DATA)
    {
        if (!IsNodeOnList(&RXBUF(rxSCOFreeList), &(hciBuffer->node)))
        {
            XASTAT_PeakDec(hci_num_sco_buffers, 1);
            InsertTailList(&(RXBUF(rxSCOFreeList)), &(hciBuffer->node));
        }
    }
    else
#endif /* NUM_SCO_CONNS != 0 */
    if (hciBuffer->flags & HCI_BUFTYPE_ACL_DATA)
    {
        if (!IsNodeOnList(&RXBUF(rxACLFreeList), &(hciBuffer->node)))
        {
            XASTAT_PeakDec(hci_num_acl_buffers, 1);
            InsertTailList(&(RXBUF(rxACLFreeList)), &(hciBuffer->node));
        }
    }
    else if (hciBuffer->flags & HCI_BUFTYPE_EVENT)
    {
        if (!IsNodeOnList(&RXBUF(rxEvtFreeList), &(hciBuffer->node)))
        {
            XASTAT_PeakDec(hci_num_event_buffers, 1);
            InsertTailList(&(RXBUF(rxEvtFreeList)), &(hciBuffer->node));
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, RXBUFF_INVALID_BUFFER_TYPE_ON_FREE);
        Assert(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RXBUFF_SetLength
 * DESCRIPTION
 *  Adjust the length of the HCI receive buffer.
 * PARAMETERS
 *  rxBuffHandle        [IN]        
 *  len                 [IN]        
 * RETURNS
 *  BT_STATUS_INVALID_PARM
 *  BT_STATUS_SUCCESS
 *****************************************************************************/
BtStatus RXBUFF_SetLength(RxBuffHandle rxBuffHandle, U16 len)
{
    HciBuffer *hciBuffer;

    if (rxBuffHandle->flags & HCI_BUFTYPE_ACL_DATA)
    {
        if (len > TRANS_MAX_ACL_BUFF_SIZE)
        {
            return BT_STATUS_INVALID_PARM;
        }
        XASTAT_PeakSet(hci_acl_buff_size, len);
    }
    else if (rxBuffHandle->flags & HCI_BUFTYPE_SCO_DATA)
    {
        if (len > TRANS_MAX_SCO_BUFF_SIZE)
        {
            return BT_STATUS_INVALID_PARM;
        }
        XASTAT_PeakSet(hci_sco_buff_size, len);
    }
    else
    {
        /* Event buffer */
        if (len > TRANS_EVENT_BUFF_SIZE)
        {
            return BT_STATUS_INVALID_PARM;
        }
    }

    hciBuffer = rxBuffHandle;
    hciBuffer->len = len;
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  RXBUFF_Deinit
 * DESCRIPTION
 *  Deinitializes receive buffer memory and lists.  This should be
 *  called during stack initialization.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void RXBUFF_Deinit(void)
{
    /* If receive buffers are allocated dynamically, then can be freed here */
    return;
}

