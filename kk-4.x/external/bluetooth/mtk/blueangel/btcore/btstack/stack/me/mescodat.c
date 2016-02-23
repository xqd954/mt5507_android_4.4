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
 *     $Workfile:mescodat.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:   This is file contains the code to Tx and Rx SCO data.
 *                These are the internal MESCO functions called by 
 *                MESCO to send and receive SCO data. 
 *
 * Created:       May 23, 2000
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

#include "btalloc.h"
#include "ddb.h"
#if BT_SCO_HCI_DATA == XA_ENABLED
#if NUM_SCO_CONNS > 0
#include "mesco.h"


/*****************************************************************************
 * FUNCTION
 *  SCO_Init
 * DESCRIPTION
 *  Initialize SCO.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SCO_Init(void)
{
    I8 i;

    /* Initialize the SCO connection table. */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        InitializeListHead(&MEC(scoTable[i].scoTxQueue));
    }
}


/*****************************************************************************
 * FUNCTION
 *  Sco_Send
 * DESCRIPTION
 *  Internal SCO connection Send function.
 * PARAMETERS
 *  scocon      [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus Sco_Send(BtScoConnect *scocon, BtPacket *Packet)
{
#if XA_ERROR_CHECK == XA_ENABLED
    if (Packet->dataLen > 255)
    {
        return BT_STATUS_FAILED;
    }

    if (scocon->scostate != BDS_CONNECTED)
    {
        return BT_STATUS_NO_CONNECTION;
    }
#else /* XA_ERROR_CHECK == XA_ENABLED */
    Assert(scocon->scostate != BDS_DISCONNECTED);
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Packet->headerLen = 0;

    InsertTailList(&scocon->scoTxQueue, &(Packet->node));

    HCI_RequestToSend(scocon->scoHciHandle);

    return BT_STATUS_PENDING;
}


/*****************************************************************************
 * FUNCTION
 *  ScoDataCallback
 * DESCRIPTION
 *  This function is called by the HCI layer to report all events
 *  and status.
 * PARAMETERS
 *  event       [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoDataCallback(U8 event, HciCallbackParms *parms)
{
    BtScoConnect *scocon;

    switch (event)
    {
        case HCI_DATA_IND:
            /* called by HCI when data arrives on an SCO Connection. */
            if (parms->ptr.rxBuff->len < 5)
            {
                return;
            }

            scocon = ScoMapHciToConnect(parms->hciHandle);
            Assert(scocon);

            /* Now report to SCO handler */
            MEC(btEvent).p.scoDataInd.remDev = scocon->scord;
            MEC(btEvent).p.scoDataInd.scoHandle = parms->hciHandle;
            MEC(btEvent).p.scoDataInd.scoCon = scocon;
            MEC(btEvent).p.scoDataInd.ptr = parms->ptr.rxBuff->buffer;
            MEC(btEvent).p.scoDataInd.len = (U8) parms->ptr.rxBuff->len;

            MEC(btEvent).eType = BTEVENT_SCO_DATA_IND;
            scocon->scord->scoHandler->callback(&MEC(btEvent));
            break;

        case HCI_PACKET_HANDLED:
            /* Called by HCI to indicate that the SCO packet was sent. */
            Assert(parms->ptr.packet);

            scocon = ScoMapHciToConnect(parms->hciHandle);
            Assert(scocon);

            if (scocon->scord->scoHandler != 0)
            {
                /* Return packet to upper layer */
                /* Now report to SCO handler */
                MEC(btEvent).p.scoPacketHandled.remDev = scocon->scord;
                MEC(btEvent).p.scoPacketHandled.scoCon = scocon;
                MEC(btEvent).p.scoPacketHandled.scoPacket = parms->ptr.packet;
                MEC(btEvent).eType = BTEVENT_SCO_DATA_CNF;
                MEC(btEvent).errCode = (BtErrorCode) parms->status;
                scocon->scord->scoHandler->callback(&MEC(btEvent));
            }
            break;

        case HCI_SEND_IND:
            /* Called by the HCI when SCO transmit resources are available. */
            scocon = ScoMapHciToConnect(parms->hciHandle);
            Assert(scocon);

            if (IsListEmpty(&scocon->scoTxQueue))
            {
                parms->ptr.packet = 0;
            }
            else
            {
                parms->ptr.packet = (BtPacket*) RemoveHeadList(&scocon->scoTxQueue);
            }
            break;
    }
}

#endif /* NUM_SCO_CONNS > 0 */
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

