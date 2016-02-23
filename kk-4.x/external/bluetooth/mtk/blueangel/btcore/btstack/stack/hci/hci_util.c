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
 *     $Workfile:hci_util.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains utility functions for the Host Controller 
 *     Interface.
 *
 * Created:
 *     September 20, 1999
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
#include "hci_meta.h"
#include "bt_feature.h"

/*****************************************************************************
 * FUNCTION
 *  HciGetTableEntry
 * DESCRIPTION
 *  Convert a Host Controller handle to an HCI handle
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  A valid HCI handle or INVALID_HCI_HANDLE.
 *****************************************************************************/
U16 HciGetTableEntry(HciHandle hciHandle)
{
    U16 i;

    /*
     * Search the connection table for the Host Handle and
     * return the table entry.
     */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if (HCC(con)[i].conFlags & HCI_ACTIVE_CON)
        {
            if ((hciHandle & 0x0FFF) == (HCC(con)[i].hciHandle & 0x0FFF))
            {
                return (i);
            }
        }
    }
    return INVALID_HCI_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *  HciSetNewBcHandle
 * DESCRIPTION
 *  Searches for a new (unused) handle to use for broadcasts.
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void HciSetNewBcHandle(HciHandle *hciHandle)
{
    HciHandle handle = *hciHandle & 0x0FFF;
    BOOL found = TRUE;

    if(handle == INVALID_HCI_HANDLE)
    {
        Assert(0);
    }
    while (found)
    {
        /* Try a different handle */
        handle--;
        if (handle > 0x0EFF)
        {
            handle = 0x0EFF;
        }

        if (HciGetTableEntry(handle) == INVALID_HCI_HANDLE)
        {
            found = FALSE;
        }
    }

    /* Assign the new handle */
    *hciHandle = handle & (*hciHandle & 0xF000);
}


/*****************************************************************************
 * FUNCTION
 *  HciAddConnection
 * DESCRIPTION
 *  Add an entry to the connection table.
 * PARAMETERS
 *  hciHandle       [IN]        
 *  linkType        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 HciAddConnection(HciHandle hciHandle, U8 linkType)
{
    U16 i;

    Report(("HciAddConnection: hciHandle[%d], linkType[%d]", hciHandle, linkType));

    /*
     * If the handle is the same as either of broadcast handles, 
     * a new broadcast handle must be chosen.
     */
    if (hciHandle == HCC(piconetBcHandle))
    {
        i = HciGetTableEntry(hciHandle);
        if (i != INVALID_HCI_HANDLE)
        {
            HciSetNewBcHandle(&HCC(piconetBcHandle));
            HCC(con)[i].hciHandle = HCC(piconetBcHandle);
        }
    }
    else if (hciHandle == HCC(activeBcHandle))
    {
        i = HciGetTableEntry(hciHandle);
        if (i != INVALID_HCI_HANDLE)
        {
            HciSetNewBcHandle(&HCC(activeBcHandle));
            HCC(con)[i].hciHandle = HCC(activeBcHandle);
        }
    }

    /* Add the entry */
    if (HciGetTableEntry(hciHandle) == INVALID_HCI_HANDLE)
    {
        /* Search for an open entry */
        for (i = 0; i < HCI_NUM_HANDLES; i++)
        {
            if (!(HCC(con)[i].conFlags & HCI_ACTIVE_CON))
            {
                HCC(con)[i].hciHandle = hciHandle;
                InitializeListHead(&(HCC(con)[i].btPacketList));
                HCC(con)[i].conFlags = HCI_ACTIVE_CON;
                HCC(con)[i].flowControl = 0;

                if (linkType == 1)
                {
                    /* ACL data */
                #if L2CAP_NUM_GROUPS > 0
                    if (!(HCC(con)[i].hciHandle & (PICONET_BROADCAST | ACTIVE_BROADCAST)))
                    {
                        /* Not a broadcast handle */
                #endif /* L2CAP_NUM_GROUPS > 0 */

                        HCC(activeAclCons)++;

                    #if L2CAP_NUM_GROUPS > 0
                    }
                    #endif /* L2CAP_NUM_GROUPS > 0 */

                    HCC(con)[i].conFlags |= HCI_DATA_TYPE_ACL;
                }

            #if NUM_SCO_CONNS != 0
                else if ((linkType == 0) || (linkType == 2))
                {
                    /* SCO data */
                    HCC(con)[i].conFlags |= HCI_DATA_TYPE_SCO;
                }
            #endif /* NUM_SCO_CONNS != 0 */

                else
                {
                    HCC(con)[i].conFlags |= HCI_DATA_TYPE_ACL;
                    if (!(HCC(con)[i].hciHandle & (PICONET_BROADCAST | ACTIVE_BROADCAST)))
                    {
                        /* Not a broadcast handle */
                        HCC(activeAclCons)++;
                    }
                    bt_trace(TRACE_GROUP_1, WRONG_LINK_TYPE);
                }

                return i;
            }
        }
    }
    else 
    {
        Report(("HciAddConnection: hciHandle[%d] is already in the table", hciHandle));
    }
    return INVALID_HCI_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *  HciDeleteConnection
 * DESCRIPTION
 *  Delete an entry to the connection table.
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HciDeleteConnection(HciHandle hciHandle)
{
    HciCallbackParms cbParms;
    U16 i;
#ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__    
    BtRemoteDevice* link = NULL;
#endif
    i = HciGetTableEntry(hciHandle);

    if (i != INVALID_HCI_HANDLE)
    {

        if (HCC(con)[i].conFlags & HCI_DATA_TYPE_ACL)
        {
            /* ACL data */

        #if L2CAP_NUM_GROUPS > 0
            if (!(HCC(con)[i].hciHandle & (PICONET_BROADCAST | ACTIVE_BROADCAST)))
            {
                /* Not a broadcast handle */
        #endif /* L2CAP_NUM_GROUPS > 0 */

                HCC(activeAclCons)--;

            #if L2CAP_NUM_GROUPS > 0
            }
            #endif /* L2CAP_NUM_GROUPS > 0 */
            HCC(aclTxPacketsLeft) += HCC(con)[i].packetsSent;
        }

    #if NUM_SCO_CONNS != 0
        else if (HCC(con)[i].conFlags & HCI_DATA_TYPE_SCO)
        {
            /* SCO data */
            HCC(scoTxPacketsLeft) += HCC(con)[i].packetsSent;
        }
    #endif /* NUM_SCO_CONNS != 0 */

        else
        {
            Assert(0);
        }

        /* Complete any packets that have not been completely sent */
        while (!IsListEmpty(&(HCC(con)[i].btPacketList)))
        {
            cbParms.hciHandle = HCC(con)[i].hciHandle;
            cbParms.status = BT_STATUS_NO_CONNECTION;
            cbParms.ptr.packet = (BtPacket*) RemoveHeadList(&(HCC(con)[i].btPacketList));
            if (HCC(metaMode))
            {
                Hci_MetaDataCallback(HCI_PACKET_HANDLED, &cbParms);
            }
            else
            {
                ME_Callback(HCI_PACKET_HANDLED, &cbParms);
            }
        }

    #if HCI_HOST_FLOW_CONTROL == XA_ENABLED

        if (HCC(con)[i].conFlags & HCI_DATA_TYPE_ACL)
        {
            HCC(aclRxPacketsLeft) += HCC(con)[i].packetsComplete;
        }
    #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
        else if (HCC(con)[i].conFlags & HCI_DATA_TYPE_SCO)
        {
            HCC(scoRxPacketsLeft) += HCC(con)[i].packetsComplete;
            Assert(HCC(scoRxPacketsLeft) <= HCI_NUM_SCO_BUFFERS);
        }
    #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */

        HCC(con)[i].packetsComplete = 0;

    #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

        /* Reset the connection state */
        HCC(con)[i].conFlags = 0;
        HCC(con)[i].packetsSent = 0;
        HCC(con)[i].btPacket = 0;
        HCC(con)[i].buffPtr = 0;
        HCC(con)[i].remaining = 0;
    
 #ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__   
        /* MAUI_01482962 */
        link = MeMapHciToRemoteDevice((HciHandle)(hciHandle & 0x0FFF));
        if (link != NULL)
        {
            /* All Pkts are turned in this function, it can stop this monitor timer */
            /* bt_trace(TRACE_GROUP_1, BTLOG_STOP_MONITOR_NUM_COMPLETED_PKTS_TIMER,2,(hciHandle & 0x0FFF)); */
            EVM_ResetTimer(&link->monitorNumCompletedPktEventTimer);
        }
 #endif /* __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__ */       

    }
    else
    {
        return BT_STATUS_FAILED;
    }
    return BT_STATUS_SUCCESS;
}

#ifdef __BT_3_0_HS__
U16 HciGetAMPTableEntry(HciHandle hciHandle)
{
    U16 i;

    /*
     * Search the connection table for the Host Handle and
     * return the table entry.
     */
    for (i = 0; i < HCI_NUM_AMP_HANDLES; i++)
    {
        if (HCC(ampCon)[i].conFlags & HCI_ACTIVE_CON)
        {
            if ((hciHandle & 0x0FFF) == (HCC(ampCon)[i].hciHandle & 0x0FFF))
            {
                return (i);
            }
        }
    }
    return INVALID_HCI_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *  HciAddConnection
 * DESCRIPTION
 *  Add an entry to the connection table.
 * PARAMETERS
 *  hciHandle       [IN]        
 *  linkType        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 HciAddAMPConnection(HciHandle hciHandle)
{
    U16 i;

    /* Add the entry */
    if (HciGetAMPTableEntry(hciHandle) == INVALID_HCI_HANDLE)
    {
        /* Search for an open entry */
        for (i = 0; i < HCI_NUM_HANDLES; i++)
        {
            if (!(HCC(ampCon)[i].conFlags & HCI_ACTIVE_CON))
            {
                HCC(ampCon)[i].hciHandle = hciHandle;
                InitializeListHead(&(HCC(ampCon)[i].btPacketList));
                HCC(ampCon)[i].conFlags = HCI_ACTIVE_CON;
                HCC(ampCon)[i].flowControl = 0;
                HCC(ampCon)[i].conFlags |= HCI_DATA_TYPE_ACL;
                return i;
            }
        }
    }
    return INVALID_HCI_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *  HciDeleteConnection
 * DESCRIPTION
 *  Delete an entry to the connection table.
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HciDeleteAMPConnection(HciHandle hciHandle)
{
    HciCallbackParms cbParms;
    U16 i;

    i = HciGetAMPTableEntry(hciHandle);

    Report(("HciDeleteAMPConnection:%d",i));
    Report(("HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));    
    if (i != INVALID_HCI_HANDLE)
    {

        if (HCC(ampCon)[i].conFlags & HCI_DATA_TYPE_ACL)
        {
            /* ACL data */ 
#ifndef __BT_30_HS_INFINITE_SENT__
            HCC(wifiController).ampTxPacketsSent -= HCC(ampCon)[i].packetsSent;;
            HCC(wifiController).numOfDataBlockbuffer += HCC(ampCon)[i].packetsSent;
            Report(("HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));    
#endif            
            
        }

        /* Complete any packets that have not been completely sent */
        while (!IsListEmpty(&(HCC(ampCon)[i].btPacketList)))
        {
            cbParms.hciHandle = HCC(ampCon)[i].hciHandle;
            cbParms.status = BT_STATUS_NO_CONNECTION;
            cbParms.ptr.packet = (BtPacket*) RemoveHeadList(&(HCC(ampCon)[i].btPacketList));
            if (HCC(metaMode))
            {
                Hci_MetaDataCallback(HCI_PACKET_HANDLED, &cbParms);
            }
            else
            {
                ME_Callback(HCI_PACKET_HANDLED, &cbParms);
            }
        }

    #if HCI_HOST_FLOW_CONTROL == XA_ENABLED

        HCC(ampCon)[i].packetsComplete = 0;

    #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

        /* Reset the connection state */
        HCC(ampCon)[i].conFlags = 0;
        HCC(ampCon)[i].packetsSent = 0;
        HCC(ampCon)[i].btPacket = 0;
        HCC(ampCon)[i].buffPtr = 0;
        HCC(ampCon)[i].remaining = 0;
    }
    return BT_STATUS_SUCCESS;
}

#endif
