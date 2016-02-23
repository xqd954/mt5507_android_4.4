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
 *     $Workfile:mescoif.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:   This is file contains the code for the Bluetooth 
 *                Management Entity interface. These are the functions
 *                called by applications and ME clients to manage and
 *                control SCO data links.
 *
 * Created:       May 19, 2000
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
#if NUM_SCO_CONNS > 0
#include "mesco.h"
#include "bt_feature.h"

extern void BTBMLocalWriteScanEnabled(U8 mode);

static void SCO_VerifyScoNodeOnList(BtScoConnect *scoCon);

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  SCO_CreateLink
 * DESCRIPTION
 *  Creates an SCO (voice) connection to the remote device. An  ACL link must
 *  already be established
 * PARAMETERS
 *  scocon          [IN]        
 *  remDevice       [IN]        
 *  pt              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_CreateLink(BtScoConnect **scocon, BtRemoteDevice *remDevice, BtScoPacketType pt)
{
    BtStatus status;
    BtScoConnect *scon;
    BtScoTxParms scoTxParms;

#if (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED)
    I16 same_cnt, different_cnt, i;
#endif 

    OS_LockStack();

    CheckLockedParm(BT_STATUS_INVALID_PARM, remDevice && remDevice->scoHandler && scocon && (pt != 0));

#if (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED)

    /* How many SCO connections already exist for the remote device? */
    same_cnt = 0;
    different_cnt = 0;
    /*
     * Go through the ScoConnect array 
     * count up whether the remDev's are the same.
     */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scon = (BtScoConnect*) & MEC(scoTable[i]);
        if (scon->scostate == BDS_CONNECTED)
        {
            if (remDevice == scon->scord)
            {
                same_cnt++;
            }
            else
            {
                different_cnt++;
            }
        }
    }

    /*
     * The following conditions indicate that there are no more SCO
     * resources available:
     * 3 SCO connections already connected
     * 2 SCO links already connected from different devices
     * 1 connection to this device, and one to another
     */
    CheckLockedParm(
        BT_STATUS_NO_RESOURCES,
        (same_cnt < 3) && (different_cnt <= 1) && !((different_cnt == 1) && (same_cnt == 1)));

#endif /* (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED) */

    /*
     * See if we can start a SCO connection.
     * First we need to get a free BtScoConnect structure
     */
    scon = ScoGetFreeConnect();
    if (scon == 0)
    {
        status = BT_STATUS_NO_RESOURCES;
        goto done;
    }

    /* No, send it to SetupLink with standard TX parms */
    scoTxParms.transmitBandwidth = 0x1f40;  /* 8000 */
    scoTxParms.receiveBandwidth = 0x1f40;   /* 8000 */
    scoTxParms.maxLatency = 0x0007;         /* 7ms */
    scoTxParms.voiceSetting = MEC(vSettings);
    scoTxParms.retransmissionEffort = 0x00; /* No Retries */
    scoTxParms.eScoPktType = 0;
    if (MEC(btFeatures[1]) & 0x08)
    {
        scoTxParms.eScoPktType |= BESPT_HV1;
    }
    if (MEC(btFeatures[1]) & 0x10)
    {
        scoTxParms.eScoPktType |= BESPT_HV2;
    }
    if (MEC(btFeatures[1]) & 0x20)
    {
        scoTxParms.eScoPktType |= BESPT_HV3;
    }

    /* Release the BtScoConnect structure */
    scon->scostate = BDS_DISCONNECTED;

    /* Call SetupLink */
    status = SCO_SetupLink(scocon, remDevice, &scoTxParms);
done:
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SCO_SetupLink
 * DESCRIPTION
 *  Create or modify an eSCO connection.
 * PARAMETERS
 *  scocon          [IN]        
 *  remDevice       [IN]        
 *  scoTxParms      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_SetupLink(BtScoConnect **scocon, BtRemoteDevice *remDevice, BtScoTxParms *scoTxParms)
{
    BtStatus status;
    BtScoConnect *scon;
    U8 j;

#if (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED)
    I16 same_cnt, different_cnt, i;
#endif 

    OS_LockStack();

    CheckLockedParm(BT_STATUS_INVALID_PARM, remDevice && remDevice->scoHandler && scocon);

    /* See if the eSCO connection already exists. */
    for (j = 0; j < NUM_SCO_CONNS; j++)
    {
        if (((*scocon) == &(MEC(scoTable)[j])) && ((*scocon)->scostate != BDS_DISCONNECTED))
        {
            if ((*scocon)->scostate != BDS_CONNECTED)
            {
                status = BT_STATUS_IN_PROGRESS;
                goto done;
            }
            if ((*scocon)->scoLinkType != BLT_ESCO)
            {
                status = BT_STATUS_INVALID_PARM;
                goto done;
            }
            /* We have found a device in the correct state */
            break;
        }
    }

#if (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED)
    /* Do the check only for new connections. */
    if (j >= NUM_SCO_CONNS)
    {
        /* How many SCO connections already exist for the remote device? */
        same_cnt = 0;
        different_cnt = 0;
        /*
         * Go through the ScoConnect array 
         * count up whether the remDev's are the same.
         */
        for (i = 0; i < NUM_SCO_CONNS; i++)
        {
            scon = (BtScoConnect*) & MEC(scoTable[i]);
            if (scon->scostate == BDS_CONNECTED)
            {
                if (remDevice == scon->scord)
                {
                    same_cnt++;
                }
                else
                {
                    different_cnt++;
                }
            }
        }

        /*
         * The following conditions indicate that there are no more SCO
         * resources available:
         * 3 SCO connections already connected
         * 2 SCO links already connected from different devices
         * 1 connection to this device, and one to another
         */
        CheckLockedParm(
            BT_STATUS_NO_RESOURCES,
            (same_cnt < 3) && (different_cnt <= 1) && !((different_cnt == 1) && (same_cnt == 1)));
    }

#endif /* (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED) */

    /* See if we need to start a new eSCO connection. */
    if (j >= NUM_SCO_CONNS)
    {
        /* First we need to get a free BtScoConnect structure */
        scon = ScoGetFreeConnect();
        if (scon == 0)
        {
            status = BT_STATUS_NO_RESOURCES;
            goto done;
        }

        scon->scostate = BDS_OUT_CON;
        *scocon = scon;
    }
    else
    {
        scon = *scocon;
    }
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);

    scon->scoop.opType = BOP_SCO_SETUP;
    scon->scord = remDevice;
    remDevice->scOutoConnectState = 1;
#if defined (__BT_COMBO_ESCO__)
    if (scoTxParms->eScoPktType == BESPT_EV3)
#else
    if (scoTxParms->eScoPktType == 0x03C8)
#endif
    {
        scon->scoLinkType = BLT_ESCO;
    }
    else
    {
        scon->scoLinkType = BLT_SCO;
    }
    Report(("Create SCO with type:%02x", scon->scoLinkType));
    scon->scoTxParms = *scoTxParms;
    /*
    scon->scoTxParms.transmitBandwidth = scoTxParms->transmitBandwidth;
    scon->scoTxParms.receiveBandwidth = scoTxParms->receiveBandwidth;
    scon->scoTxParms.maxLatency = scoTxParms->maxLatency;
    scon->scoTxParms.voiceSetting = scoTxParms->voiceSetting;
    scon->scoTxParms.retransmissionEffort = scoTxParms->retransmissionEffort;
    scon->scoTxParms.eScoPktType = scoTxParms->eScoPktType;
    */
    SCO_VerifyScoNodeOnList(scon);

    /* We can't use MeAddOperation() here because we don't want to use remDev */
    InsertTailList(&MEC(opList), &((*scocon)->scoop.node));

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  done:
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SCO_DisconnectLink
 * DESCRIPTION
 *  Disconnect an SCO link.
 * PARAMETERS
 *  scocon      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_DisconnectLink(BtScoConnect *scocon)
{
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, scocon);
    OS_LockStack();

    if (IsNodeOnList(&MEC(opList), &(scocon->scoop.node)))
    {
        if (scocon->scostate == BDS_OUT_DISC)
        {
            OS_UnlockStack();
            return BT_STATUS_IN_PROGRESS;
        }
    }
    else
    {
        if ((scocon->scostate == BDS_OUT_CON) || (scocon->scostate == BDS_OUT_CON2) || (scocon->scostate == BDS_IN_CON))
        {
            OS_UnlockStack();

            /*
             * Connection operation is in progress so we need to wait until it
             * is finished
             */
            return BT_STATUS_BUSY;
        }
    }

    if (scocon->scostate == BDS_CONNECTED)
    {
        if (scocon->scoop.opType == BOP_NOP)
        {
            /* We're connected so we need to disconnect */
            scocon->scostate = BDS_OUT_DISC;
            /*
             * No operation is in progress so put the disconnect operation 
             * on the queue.
             */
            scocon->scoop.opType = BOP_SCO_DISCONNECT;

            Assert(!IsNodeOnList(&MEC(opList), &(scocon->scoop.node)));
            InsertTailList(&MEC(opList), &scocon->scoop.node);

            /* Attempt to start the operation */
            MeStartOperation();

            OS_UnlockStack();
            return BT_STATUS_PENDING;
        }
        return BT_STATUS_BUSY;
    }

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}


#if BT_SCO_HCI_DATA == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  SCO_SendData
 * DESCRIPTION
 *  Sends the given data on the SCO link
 * PARAMETERS
 *  scocon      [IN]        
 *  packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_SendData(BtScoConnect *scocon, BtPacket *packet)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, scocon && packet && ((packet->flags & ~BTP_FLAG_INUSE) == 0));

    packet->headerLen = 0;

    OS_LockStack();
    status = Sco_Send(scocon, packet);
    OS_UnlockStack();
    return status;
}
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  SCO_RegisterHandler
 * DESCRIPTION
 *  Registers the given SCO handler for the given remote device. One
 *  handler can be registered for any one device. There can be up
 *  to three SCO links for one device or two SCO links for different
 *  devices.
 * PARAMETERS
 *  remDev      [IN]        
 *  handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_RegisterHandler(BtRemoteDevice *remDev, BtHandler *handler)
{
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, handler && handler->callback && remDev);

    OS_LockStack();

    if (remDev->scoHandler == 0)
    {
        remDev->scoHandler = handler;

        OS_UnlockStack();
        return BT_STATUS_SUCCESS;
    }

    OS_UnlockStack();
    return BT_STATUS_BUSY;
}


/*****************************************************************************
 * FUNCTION
 *  SCO_UnregisterHandler
 * DESCRIPTION
 *  Unregister the given SCO handler for the given remote device.
 * PARAMETERS
 *  remDev      [IN]        
 *  handler     [IN]        
 * RETURNS
 *  BT_STATUS_INVALID_PARM
 *  BT_STATUS_SUCCESS
 *****************************************************************************/
BtStatus SCO_UnregisterHandler(BtRemoteDevice *remDev, BtHandler *handler)
{
    /* Lock the stack */
    OS_LockStack();

    CheckLockedParm(
        BT_STATUS_INVALID_PARM,
        handler && handler->callback && remDev && remDev->scoHandler && (remDev->scoHandler == handler));

    remDev->scoHandler = 0;

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  SCO_AcceptIncomingConnection
 * DESCRIPTION
 *  Accepts the given SCO connect from the remote device.
 * PARAMETERS
 *  scoCon      [IN]        
 *  handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_AcceptIncomingConnection(BtScoConnect *scoCon, BtHandler *handler)
{
    OS_LockStack();

    CheckLockedParm(BT_STATUS_INVALID_PARM, scoCon && (scoCon->scord->scoHandler->callback == handler->callback));

    SCO_VerifyScoNodeOnList(scoCon);

    scoCon->scoop.opType = BOP_SCO_ACCEPT;
    InsertTailList(&MEC(opList), &scoCon->scoop.node);

    /* Attempt to start the operation */
    MeStartOperation();

    OS_UnlockStack();
    return BT_STATUS_PENDING;
}


/*****************************************************************************
 * FUNCTION
 *  SCO_RejectIncomingConnection
 * DESCRIPTION
 *  Rejects the incoming SCO connection.
 * PARAMETERS
 *  scoCon      [IN]        
 *  handler     [IN]        
 *  reason      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SCO_RejectIncomingConnection(BtScoConnect *scoCon, BtHandler *handler, BtErrorCode reason)
{
    OS_LockStack();

    CheckLockedParm(BT_STATUS_INVALID_PARM, scoCon && (scoCon->scord->scoHandler->callback == handler->callback));
    scoCon->scoRejectReason = reason;
    scoCon->scoop.opType = BOP_SCO_REJECT;

    SCO_VerifyScoNodeOnList(scoCon);

    /*
     * We can't use MeAddOperation() here because we don't want to
     * use remDev
     */
    InsertTailList(&MEC(opList), &scoCon->scoop.node);

    /* Attempt to start the operation */
    MeStartOperation();

    OS_UnlockStack();
    return BT_STATUS_PENDING;
}

static void SCO_VerifyScoNodeOnList(BtScoConnect *scoCon)
{
    Assert(IsEntryAvailable(&scoCon->scoop.node));
}

#else /* NUM_SCO_CONNS > 0 */


/*****************************************************************************
 * FUNCTION
 *  dummy
 * DESCRIPTION
 *  Avoid compiler error of empty file. Calls itself to avoid to avoid
 *  unreferenced static function warning.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void dummy(void)
{
    dummy();
}

#endif /* NUM_SCO_CONNS > 0 */

