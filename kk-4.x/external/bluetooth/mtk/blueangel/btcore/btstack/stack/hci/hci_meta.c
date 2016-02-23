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

#include "bt_common.h"
#include "btalloc.h"
#include "hci_meta.h"


/*****************************************************************************
 * FUNCTION
 *  HciMetaModeEnabled
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HciMetaModeEnabled(void)
{
    HCC(metaMode) = TRUE;
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  HciMetaSendTestData
 * DESCRIPTION
 *  
 * PARAMETERS
 *  con_hdl         [IN]        
 *  data_len        [IN]        
 *  raw_data        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HciMetaSendTestData(U16 con_hdl, U16 data_len, U8 *raw_data)
{
    BtPacket *bt_packet;

    if (!HCC(metaMode))
    {
        return BT_STATUS_FAILED;
    }

    if (IsListEmpty(&HCC(metaTxDataList)))
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        bt_packet = (BtPacket*) RemoveHeadList(&HCC(metaTxDataList));
        bt_packet->llpContext = con_hdl;
        bt_packet->data = raw_data;
        bt_packet->dataLen = data_len;
        bt_packet->headerLen = 0;
        bt_packet->tailLen = 0;
        InsertTailList(&(HCC(metaTxQueueList)), &bt_packet->node);
        HCI_RequestToSend(con_hdl);
        return BT_STATUS_SUCCESS;
    }

}


/*****************************************************************************
 * FUNCTION
 *  HciMetaStartTxPureTest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  con_hdl         [IN]        
 *  u2PktLen        [IN]        
 *  u2PktNum        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HciMetaStartTxPureTest(kal_uint16 con_hdl, kal_uint16 u2PktLen, kal_uint16 u2PktNum)
{
    U16 i = 0;

    BtPacket *bt_packet;

    if (!HCC(metaMode))
    {
        return BT_STATUS_FAILED;
    }
    /*
    if (u2PktNum > 0xFFFF)
    {
        return BT_STATUS_FAILED;
    }
    */
    if (IsListEmpty(&HCC(metaTxDataList)))
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        HCC(metaTxCounter) = u2PktNum;
        HCC(metaTxSentCounter) = 0;
        HCC(metaTxRealSentCounter) = 0;
        HCC(metaTxTimerCounter) = OS_GetSystemTime();
        HCC(metaTxDataBuffer) = (U8*) get_ctrl_buffer(u2PktLen);
        for (i = 0; i < HCC(metaTxCounter); i++)
        {
            if (IsListEmpty(&HCC(metaTxDataList)))
            {
                break;
            }
            bt_packet = (BtPacket*) RemoveHeadList(&HCC(metaTxDataList));
            bt_packet->llpContext = con_hdl;
            bt_packet->data = HCC(metaTxDataBuffer);
            bt_packet->dataLen = u2PktLen;
            bt_packet->headerLen = 0;
            bt_packet->tailLen = 0;
            InsertTailList(&(HCC(metaTxQueueList)), &bt_packet->node);
            HCC(metaTxSentCounter)++;
            HCI_RequestToSend(con_hdl);
        }
        return BT_STATUS_SUCCESS;
    }
}


/*****************************************************************************
 * FUNCTION
 *  HciMeta_QueryTxPureTest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U16 HciMeta_QueryTxPureTest(void)
{
    return HCC(metaTxRealSentCounter);
}


/*****************************************************************************
 * FUNCTION
 *  Hci_MetaDataCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void Hci_MetaDataCallback(U8 event, HciCallbackParms *parms)
{
    switch (event)
    {
        case HCI_DATA_IND:
            BtMeta_HciReceive(parms->ptr.rxBuff, parms->hciHandle);
            break;
        case HCI_SEND_IND:
            parms->ptr.packet = BtMeta_HciTxReady(parms);
            break;
        case HCI_SEND_IND_AMP:
            Assert(0);
            break;
        case HCI_PACKET_HANDLED:
            BtMeta_HciTxDone(parms);
            break;
        case HCI_META_PKT_SENT_OUT:
            if (HCC(metaTxCounter) == 0)
            {
                if (HCC(radioEntry) != 0)
                {
                    HCC(radioEntry) (HCI_META_PKT_SENT_OUT, parms);
                }
            }
            else
            {
                HCC(metaTxRealSentCounter) += parms->ptr.txNumber;
                if (HCC(metaTxCounter) == HCC(metaTxRealSentCounter))
                {
                    if (HCC(radioEntry) != 0)
                    {
                        HCC(radioEntry) (HCI_META_PKT_SENT_OUT, parms);
                    }
                    HCC(metaTxCounter) = 0;
                    HCC(metaTxSentCounter) = 0;
                    free_ctrl_buffer(HCC(metaTxDataBuffer));
                    HCC(metaTxDataBuffer) = 0;
                }
            }
            break;

    }
}


/*****************************************************************************
 * FUNCTION
 *  Hci_MetaCheckTxAvail
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus Hci_MetaCheckTxAvail(void)
{
    if (!HCC(metaMode))
    {
        return BT_STATUS_FAILED;
    }
    if (IsListEmpty(&HCC(metaTxDataList)))
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BtMeta_HciTxReady
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtPacket *BtMeta_HciTxReady(HciCallbackParms *parms)
{
    BtPacket *packet;
    BtPacket *txPacket = 0;
    HciHandle HciHndl;
    ListEntry *queue;

    HciHndl = parms->hciHandle;
    parms->morePacketWaiting = 0;
    queue = &HCC(metaTxQueueList);

    for (packet = (BtPacket*) GetHeadList(queue); packet != (BtPacket*) queue;
         packet = (BtPacket*) GetNextNode(&packet->node))
    {
        if (packet->llpContext == HciHndl)
        {
            txPacket = (BtPacket*) packet;
            RemoveEntryList(&packet->node);
            break;
        }
    }
    return txPacket;
}


/*****************************************************************************
 * FUNCTION
 *  BtMeta_HciTxDone
 * DESCRIPTION
 *  
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BtMeta_HciTxDone(HciCallbackParms *parms)
{
    BtPacket *Packet;
    BtPacket *TxPacket;
    U16 length;
    U16 con_hdl;

    Packet = parms->ptr.packet;
    length = Packet->dataLen;
    con_hdl = Packet->llpContext;
    InsertTailList(&HCC(metaTxDataList), &Packet->node);
    if (HCC(metaTxCounter) == 0)
    {
        if (HCC(radioEntry) != 0)
        {
            HCC(radioEntry) (HCI_PACKET_HANDLED, parms);
        }
    }
    else
    {
        if (HCC(metaTxCounter) != HCC(metaTxSentCounter))
        {
            Packet = (BtPacket*) RemoveHeadList(&HCC(metaTxDataList));
            Packet->llpContext = con_hdl;
            Packet->data = HCC(metaTxDataBuffer);
            Packet->dataLen = length;
            Packet->headerLen = 0;
            Packet->tailLen = 0;
            InsertTailList(&(HCC(metaTxQueueList)), &Packet->node);
            HCC(metaTxSentCounter)++;
        }
    }
    if (!IsListEmpty(&HCC(metaTxQueueList)))
    {
        TxPacket = (BtPacket*) GetHeadList(&HCC(metaTxQueueList));
        HCI_RequestToSend(TxPacket->llpContext);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BtMeta_HciReceive
 * DESCRIPTION
 *  
 * PARAMETERS
 *  BuffDesc        [IN]        
 *  HciHndl         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BtMeta_HciReceive(const HciBuffer *BuffDesc, HciHandle HciHndl)
{
    HciMetaBuffer rxMetaBuffer;
    HciCallbackParms parms;

    rxMetaBuffer.con_hdl = HciHndl;
    rxMetaBuffer.len = BuffDesc->len;
    rxMetaBuffer.buffer = BuffDesc->buffer;
    parms.ptr.rxMetaBuff = &rxMetaBuffer;
    if (HCC(radioEntry) != 0)
    {
        HCC(radioEntry) (HCI_DATA_IND, &parms);
    }
}

