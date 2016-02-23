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
 *     $Workfile:sdpserv.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Source code for the Bluetooth Service Discovery Protocol Server.
 *
 * Created:
 *     October 18, 1999
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
#include "btrom.h"
#include "sys/sdpi.h"
#include "sys/mei.h"
#include "sys/debug.h"
#include "sdptester.h"
#include "bt_feature.h"
#include "bt_adp_system.h"
#include "me.h"

#if SDP_SERVER_SUPPORT == XA_ENABLED
#define BT_SDP_EIR_ADD_RECORD 0x01
#define BT_SDP_EIR_REMOVE_RECORD 0x02

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/
void SdpServL2CapCallback(U16 ChannelId, L2capCallbackParms *Info);
void SdpServParseQuery(BtRemoteDevice *remDev, I16 len, U8 *data);
BOOL SdpFindUuidInRecord(SdpRecord *record, const U8 *uuid);
BOOL SdpFindUuidInAttrib(const U8 *aVal, const U8 *uuid, U16 uuidLen, U16 level);
U16 SdpServMarkMatches(BtRemoteDevice *remDev, U8 *searchPat);
BtRemoteDevice *FindRemDevWithSdpServChannel(U16 channel);
BtPacket *SdpServGetPacket(void);
void SdpServSendError(BtRemoteDevice *remDev, BtPacket *packet, SdpErrorCode errCode);
void SdpServHandleServiceSearch(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len);
void SdpServHandleAttribReq(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len);

void SdpServHandleServiceSearchAttribReq(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len);
U8 SdpServGetNextContState(void);
SdpRecord *SdpServFindSdpRecord(U32 handle);
SdpRecord *SdpServFindMarkedRecord(U16 mask);
U32 SdpServMarkAttribs(BtRemoteDevice *remDev, SdpRecord *record, U8 *list);
U16 SdpMarkAttribute(BtRemoteDevice *remDev, SdpRecord *record, U16 id);
BOOL SdpVerifyParmLength(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len, I16 len2);
U16 SdpStoreAttribData(BtRemoteDevice *remDev, BtPacket *packet, U16 offset, U16 final);
void SdpServFinishUpPacket(BtRemoteDevice *remDev, BtPacket *packet, I16 offset, I16 hOffset);
void SdpServDIDInit(void);

#ifdef BT_FOR_NONE_MP_ONLY
BOOL SdpServVerifyRecord(SdpRecord *record);
#endif 
BtStatus SdpUpdateServiceDatabaseState(void);

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
void SdpUpdateEirDataBase(U16 len, const U8 *attribute_value, U8 action);
#endif 

void SdpHandleMarkedRecord(SdpRecord *record);
SdpServerInfo *SdpServGetInfo(void);

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
extern void ME_AddEIRServiceClass(U16 service_class);
extern void ME_RemoveEIRServiceClass(U16 service_class);
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  SdpInitServer
 * DESCRIPTION
 *  Initialize SDP server.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 *****************************************************************************/
BtStatus SdpInitServer(void)
{
    I8 i;
    U16 mask;

    SDPS(SdpServPsm).callback = SdpServL2CapCallback;
    SDPS(SdpServPsm).psm = BT_PSM_SDP;
    SDPS(SdpServPsm).localMtu = SDP_SERVER_LOCAL_MTU;
    SDPS(SdpServPsm).minRemoteMtu = SDP_SERVER_MIN_REMOTE_MTU;
    SDPS(SdpServPsm).authorizedSetting = FALSE;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    SDPS(SdpServPsm).inLinkMode = SDPS(SdpServPsm).outLinkMode = L2MODE_BASIC;
#ifdef __BT_2_1_ERTM__
    SDPS(SdpServPsm).lockStepNeeded = 0;
    SDPS(SdpServPsm).highSpeedRequired = 0;

#endif

#if 0
    SDPS(SdpServPsm).inLinkMode = SDPS(SdpServPsm).outLinkMode = L2MODE_RETRANSMISSION | 
                                                                 L2MODE_FLOW_CONTROL | L2MODE_BASIC;
    SDPS(SdpServPsm).inTransmitMax = 10;
    SDPS(SdpServPsm).inBufferSize = SDP_SERVER_LOCAL_MTU;
    SDPS(SdpServPsm).inRetransTimeout = 100;
    SDPS(SdpServPsm).inMonitorTimeout = 200;
#endif    
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    if (L2CAP_RegisterPsm((L2capPsm*) (&SDPS(SdpServPsm))) != BT_STATUS_SUCCESS)
    {
        return BT_STATUS_FAILED;
    }

    /* Initialize the list of records */
    InitializeListHead(&SDPS(records));

    /*
     * Initialize the sdpRecMask of each SdpServiceInfo structure based
     * on the number of SDP clients. Also initializes the transmit packet.
     */
    mask = 1;
    for (i = 0; i < SDP_ACTIVE_CLIENTS; i++)
    {
        SDPS(serverInfos)[i].recMask = mask;
        mask <<= 1;

        /* Init the transmit packet */
        SDPS(serverInfos)[i].txPacket.data = SDPS(serverInfos)[i].txBuff;
        SDPS(serverInfos)[i].txPacketBusy = FALSE;
    }

    /* Initialize the record 0 attributes and record 0 itself. */
    SDPS(record0Attribs)[0].id = AID_SERVICE_CLASS_ID_LIST;
    SDPS(record0Attribs)[0].len = SDP_ZERO_SERV_CLASS_ID_SIZE;
    SDPS(record0Attribs)[0].value = ZeroServClassIdVal;

    SDPS(record0Attribs)[1].id = AID_VERSION_NUMBER_LIST;
    SDPS(record0Attribs)[1].len = SDP_ZERO_VER_NUMBER_SIZE;
    SDPS(record0Attribs)[1].value = ZeroVersionNumberList;

    /*
     * Initialize the ServiceDatabaseState value of the 
     * SDP Server.
     */
    SDPS(ZeroServiceDatabaseState[0]) = DETD_UINT + DESD_4BYTES;
    /* Use OS_GetSystemTime() to prevent rollover problems */
    StoreBE32(SDPS(ZeroServiceDatabaseState + 1), OS_GetSystemTime());

    SDPS(record0Attribs)[2].id = AID_SERVICE_DATABASE_STATE;
    SDPS(record0Attribs)[2].len = SDP_ZERO_SERVICE_DATABASE_STATE_SIZE;
    SDPS(record0Attribs)[2].value = SDPS(ZeroServiceDatabaseState);

    SDPS(record0).attribs = SDPS(record0Attribs);
    SDPS(record0).num = 3;

    /*
     * Set SDP server's service record handle to zero. Should already be zero. 
     * This code is here for readability. 
     */
    SDPS(record0).handle = SDP_SERV_OWN_HANDLE;

    /* SDP is unlocked and can accept queries */
    SDPS(sdpLocked) = FALSE;

    /* Add an Sevice Discovery entry (Record Zero) to the SDP server */
    InsertTailList(&(SDPS(records)), &(SDPS(record0).node));

    /* Initialize the first SDP record */
    SDPS(nextHandle) = SDP_SERV_FIRST_HANDLE;
    SdpServDIDInit();
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  SdpDeinitServer
 * DESCRIPTION
 *  Deinitialize SDP client.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SdpDeinitServer(void)
{
#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    L2CAP_DeregisterPsm((L2capPsm*) (&SDPS(SdpServPsm)));
#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

    bt_trace(TRACE_GROUP_1, SDP_SERVER_DEINITIALIZED);
}


/*****************************************************************************
 * FUNCTION
 *  SDP_AddRecord
 * DESCRIPTION
 *  Add a record to the SDP database.
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if record
 *  already exists, and BT_STATUS_INVALID_PARM if the new
 *  record is formed incorrectly.
 *****************************************************************************/
BtStatus SDP_AddRecord(SdpRecord *record)
{
    U8 i;
    BtStatus status = BT_STATUS_SUCCESS;

#ifdef BT_FOR_NONE_MP_ONLY
    if ((!record) || (!SdpServVerifyRecord(record)))
    {
        return BT_STATUS_INVALID_PARM;
    }
    Assert(record && SdpServVerifyRecord(record));
#endif /* BT_FOR_NONE_MP_ONLY */ 

    /* Lock the stack */
    OS_LockStack();

    if (IsNodeOnList(&SDPS(records), &(record->node)))
    {
        /* If JSR82 registers a duplicated record (using same handler), return failure */            
        bt_trace(TRACE_GROUP_1, BTLOG_THIS_RECORD_IS_ALREADY_IN_SDPS_LIST);
    #ifdef MULTI_RF_CHANNEL   
        status = BT_STATUS_SUCCESS;  
    #else
        status = BT_STATUS_FAILED;  
    #endif
        goto Done;
    }

    /* Insert the record at the end of the list */
    InsertTailList(&(SDPS(records)), &(record->node));
    record->handle = SDPS(nextHandle);
    bt_trace(TRACE_GROUP_1, SERVICE_REG_RECORD_HANDLERBT_DEBUGx04X, record->handle);

    SDPS(nextHandle) += 1;
    /* Set the initial Service Record State for this record */
    record->recordState = OS_GetSystemTime();
    /* Clear all of the internal flags */
    record->flags = 0;
    record->handleFlag = 0;
    record->stateFlag = 0;

    /* Go through the attributes and clear the flags */
    for (i = 0; i < record->num; i++)
    {
        record->attribs[i].flags = 0;
    }

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    bt_trace(TRACE_GROUP_1, BTLOG_RECORDNUMxD, record->num);
    for (i = 0; i < record->num; i++)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_RECORDATTRIBSIDxD, record->attribs->id);
        if (record->attribs->id == AID_SERVICE_CLASS_ID_LIST)
        {
            SdpUpdateEirDataBase(record->attribs->len, record->attribs->value, BT_SDP_EIR_ADD_RECORD);
            break;
        }

    }
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

    /* Update the SDP Server's ServiceDatabaseState */
    status = SdpUpdateServiceDatabaseState();
  Done:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            SDP_RemoveRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Remove a record from the SDP database. 
 *
 * Return:    BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if failure, 
 *            and BT_STATUS_INVALID_PARM if the parameters are invalid.
 */


/*****************************************************************************
 * FUNCTION
 *  SDP_RemoveRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SDP_RemoveRecord(SdpRecord *record)
{
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    U8 i;
#endif 

    BtStatus status = BT_STATUS_FAILED;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!record)
    {
        return BT_STATUS_INVALID_PARM;
    }

#endif /* XA_ERROR_CHECK == XA_ENABLED */

    /* Lock the stack */
    OS_LockStack();

	if (BTCoreVerifySysInitState() == FALSE) {
		OS_Report("%s() When BT is off. Ignore.", __FUNCTION__);
		goto Done;
	}

    if (!IsNodeOnList(&SDPS(records), &(record->node)))
    {
        bt_trace(TRACE_GROUP_1, BT_SDP_REMOVE_RECORD_NOT_IN_LIST);
        goto Done;
    }
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    bt_trace(TRACE_GROUP_1, BTLOG_RECORDNUMxD, record->num);
    for (i = 0; i < record->num; i++)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_RECORDATTRIBSIDxD, record->attribs->id);
        if (record->attribs->id == AID_SERVICE_CLASS_ID_LIST)
        {
            SdpUpdateEirDataBase(record->attribs->len, record->attribs->value, BT_SDP_EIR_REMOVE_RECORD);
            break;
        }

    }
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
    /* Remove the SDP record */
    RemoveEntryList(&(record->node));
    /* Clear out any continuation state info for any in-progress queries */
    SdpHandleMarkedRecord(record);

    /*
     * Update the SDP class of device value and if it has changed
     * Write the class of device to the radio 
     */
    SdpServUpdateClassOfDevice();

    /* Update the SDP Server's ServiceDatabaseState */
    status = SdpUpdateServiceDatabaseState();

  Done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}

#ifdef BT_FOR_NONE_MP_ONLY


/*****************************************************************************
 * FUNCTION
 *  SDP_LockRecord
 * DESCRIPTION
 *  If no current SDP Queries are in progress and if the SDP record
 *  to update is a valid entry, lock SDP for record updating.  SDP
 *  will refuse any SDP queries via an SDP error response until the
 *  record has been updated (signaled by SDP_UnlockRecord()).  In
 *  order to guarantee safe SDP record updates, this function should
 *  be called prior to altering any SDP record.  SDP can only be
 *  locked one time, thus preventing simultaneous record updated by
 *  multiple tasks.
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if failure,
 *  and BT_STATUS_INVALID_PARM if the parameters are invalid.
 *****************************************************************************/
BtStatus SDP_LockRecord(SdpRecord *record)
{
    I8 i;
    BtStatus status = BT_STATUS_FAILED;

#ifdef BT_FOR_NONE_MP_ONLY
    if ((!record) || (!SdpServVerifyRecord(record)))
    {
        /* Can't lock a badly formed record  */
        return BT_STATUS_INVALID_PARM;
    }
    Assert(record && SdpServVerifyRecord(record));

#endif /* BT_FOR_NONE_MP_ONLY */ 

    /* Lock the stack */
    OS_LockStack();

    if (!IsNodeOnList(&SDPS(records), &(record->node)))
    {
        /* Can't lock a non-existent record */
        goto Done;
    }

    /*
     * Check if an SDP Query is in progress (ie txPacketBusy is TRUE for
     * any of the serverInfos structures 
     */
    for (i = 0; i < SDP_ACTIVE_CLIENTS; i++)
    {
        if (SDPS(serverInfos)[i].txPacketBusy == TRUE)
        {
            goto Done;
        }
    }

    /* SDP can only be locked by one task */
    if (SDPS(sdpLocked))
    {
        goto Done;
    }

    /* SDP is locked and cannot accept queries */
    SDPS(sdpLocked) = TRUE;
    status = BT_STATUS_SUCCESS;

  Done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SDP_UnlockRecord
 * DESCRIPTION
 *  Unlocks SDP from record updating, and once again allows SDP
 *  queries to be handled normally.  Once this function is called,
 *  it is no longer valid to alter any SDP record. If the unlocked
 *  record has changed, its internal fields are updated
 *  appropriately. SDP must be locked prior to calling this
 *  function.
 * PARAMETERS
 *  record      [IN]        
 *  updated     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if failure,
 *  and BT_STATUS_INVALID_PARM if the parameters are invalid.
 *****************************************************************************/
BtStatus SDP_UnlockRecord(SdpRecord *record, BOOL updated)
{
    I8 i;
    BtStatus status = BT_STATUS_FAILED;

#ifdef BT_FOR_NONE_MP_ONLY
    if ((!record) || (!SdpServVerifyRecord(record)))
    {
        /* Can't unlock a badly formed record  */
        return BT_STATUS_INVALID_PARM;
    }
    Assert(record && SdpServVerifyRecord(record));

#endif /* BT_FOR_NONE_MP_ONLY */ 

    /* Lock the stack */
    OS_LockStack();

    if (!IsNodeOnList(&SDPS(records), &(record->node)))
    {
        /* Can't unlock a non-existent record */
        goto Done;
    }

    /* SDP must already be locked */
    if (!SDPS(sdpLocked))
    {
        goto Done;
    }

    /* SDP record has been changed */
    if (updated)
    {
        /*
         * Clear out any continuation state info for any old 
         * in-progress queries 
         */
        SdpHandleMarkedRecord(record);

        /* Update the Service Record State for this record */
        record->recordState = OS_GetSystemTime();
        /* Clear all of the internal flags */
        record->flags = 0;
        record->handleFlag = 0;
        record->stateFlag = 0;
        /* Keep the existing service record handle */

        /* Go through the attributes and clear the flags */
        for (i = 0; i < record->num; i++)
        {
            record->attribs[i].flags = 0;
        }

        /*
         * Update the SDP class of device value and if it has changed
         * Write the class of device to the radio 
         */
        SdpServUpdateClassOfDevice();
    }

    /* SDP is unlocked and can accept queries */
    SDPS(sdpLocked) = FALSE;
    status = BT_STATUS_SUCCESS;

  Done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}

#endif /* BT_FOR_NONE_MP_ONLY */ 


/*****************************************************************************
 * FUNCTION
 *  SdpServL2CapCallback
 * DESCRIPTION
 *  Handle the callbacks from L2CAP
 * PARAMETERS
 *  channelId       [IN]        
 *  info            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServL2CapCallback(U16 channelId, L2capCallbackParms *info)
{
    BtRemoteDevice *remDev;
    U16 respCode;
    BtStatus status;
    BtPacket *packet;

    remDev = info->aclLink;

    switch (info->event)
    {
        case L2EVENT_CONNECT_IND:
            if ((SDPS(numClients) < SDP_NUM_CLIENTS) && (remDev->sdpServInfo == 0))
            {
                /* Accept the connection */
                respCode = L2CONN_ACCEPTED;

                /* Assign an SdpServerInfo structure to the remote device */
                remDev->sdpServInfo = SdpServGetInfo();
                Assert(remDev->sdpServInfo != 0);
                SDPS(numClients) += 1;

                remDev->sdpServInfo->channel = channelId;
                bt_trace(TRACE_GROUP_1, SDP_SERVER_INCOMING_L2CAP_CONNECTION_ACCEPTED);
                {

                    SdpRecord *record;

                    record = (SdpRecord*) SDPS(records).Flink;
                    while ((ListEntry*) record != &SDPS(records))
                    {
                        bt_trace(TRACE_GROUP_1, RECORD_HANDLERBT_DEBUGx04X, record->handle);
                        /* Advance to the next record */
                        record = (SdpRecord*) record->node.Flink;
                    }
                }
                (void)L2CAP_ConnectRsp(channelId, respCode, 0);
            }
            else
            {
                respCode = L2CONN_REJECT_NO_RESOURCES;
                bt_trace(TRACE_GROUP_1, SDP_SERVER_INCOMING_L2CAP_CONNECTION_REJECTED);
                if (remDev->sdpServInfo == 0)
                {
                    bt_trace(TRACE_GROUP_1, SERIOUS_ERROR);
                    MeDisconnectLink(remDev, 12);
                }
                else
                {
                    (void)L2CAP_ConnectRsp(channelId, respCode, 0);
                }
            }
            break;

        case L2EVENT_CONNECTED:
            /* The connection is up. Find the remote device */
            if(remDev == 0)  
                return;
            if((remDev->sdpServInfo == 0) || (SDPS(numClients) == 0))
            {
                remDev->discFlag = TRUE;
                MeDisconnectLink(remDev, 13);                
                break;
            }
            bt_trace(TRACE_GROUP_1, SDP_SERVER_CONNECTION_COMPLETE);
            break;

        case L2EVENT_DISCONNECTED:
            bt_trace(TRACE_GROUP_1, SDP_SERVER_CONNECTION_DISCONNECTED);
            remDev = SdpFindRemDevWithSdpChannel(channelId, BST_SDP_SERVER);
            if (remDev != 0)
            {
                if (SDPS(numClients) == 0) 
                {
                    Assert(0);
                }
                SDPS(numClients) -= 1;

                /* Free the SdpServerInfo record */
                if (remDev->sdpServInfo == 0)
                {
                    Assert(0);
                }
                remDev->sdpServInfo->busy = FALSE;
                remDev->sdpServInfo->txPacketBusy = FALSE;
                remDev->sdpServInfo = 0;
            }
            break;

        case L2EVENT_DATA_IND:
            /* We have received data from an SDP client */
            bt_trace(TRACE_GROUP_1, SDP_SERVER_RECEIVED_DATA);
            if (remDev == 0)
            {
                return;
            }
            if (remDev->sdpServInfo == 0)
            {
                remDev->discFlag = TRUE;
                MeDisconnectLink(remDev, 14);                
                break;
            }
            if ((remDev->sdpServInfo->txPacketBusy == FALSE) && (!SDPS(sdpLocked)))
            {
                /*
                 * Only parse the query if SDP is unlocked and the remote
                 * device's transmit packet is free.
                 */
                SdpServParseQuery(remDev, (I16) info->dataLen, info->ptrs.data);
            }
            else if ((SDPS(sdpLocked)) && (remDev->sdpServInfo->txPacketBusy == FALSE))
            {
                /*
                 * SDP is locked due to a record update.  Reject this SDP Query. 
                 * The remote device will have to try again later.
                 */

                /* Get a packet for the error response */
                packet = &(remDev->sdpServInfo->txPacket);
                remDev->sdpServInfo->txPacketBusy = TRUE;
                SdpServSendError(remDev, packet, BSEC_OUT_OF_RESOURCES);
            }
            else
            {
                /*
                 * Packet is still in use so the SDP client has violated the protocol
                 * we need to disconnect.
                 */
                status = L2CAP_DisconnectReq(channelId);
                AssertEval((status == BT_STATUS_PENDING) || (status == BT_STATUS_BUSY));
            }
            break;

        case L2EVENT_PACKET_HANDLED:
            bt_trace(TRACE_GROUP_1, SDP_SERVER_PACKET_RETURNED);
            Assert(remDev->sdpServInfo != 0);
            remDev->sdpServInfo->txPacketBusy = FALSE;
            break;
    }

}


/*****************************************************************************
 * FUNCTION
 *  SdpServParseQuery
 * DESCRIPTION
 *  Parse the SDP query
 * PARAMETERS
 *  remDev      [IN]        
 *  len         [IN]        
 *  data        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServParseQuery(BtRemoteDevice *remDev, I16 len, U8 *data)
{
    U16 transID;
    BtPacket *packet;
    I8 offset;

    offset = BT_PACKET_HEADER_LEN - 5;

    /* 
    Before calling the function, make sure that the 
    remDev->sdpServInfo->txPacketBusy == FALSE
    */
    /* Get a packet for the response */
    packet = &(remDev->sdpServInfo->txPacket);
    remDev->sdpServInfo->txPacketBusy = TRUE;

    /* Get the trans ID. If it does not exist then set it to 0. */
    if (len < 3)
    {
        transID = 0;
    }
    else
    {
        transID = BEtoHost16(data + 1);
    }

    /* Insert the transID into the response */
    StoreBE16(packet->header + offset + 1, transID);

    /* Verify that the packet contains the minimal amount of data */
    if (len < SDP_QUERY_MIN_LEN)
    {
        /* for BQB Change to fit the PTS's bugs */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_QUERY_LEN_IS_SHORT);
        SdpServSendError(remDev, packet, BSEC_BAD_SYNTAX);
        return;
    }

    switch (data[0])
    {
        case BSQT_SERVICE_SEARCH_REQ:
            SdpServHandleServiceSearch(remDev, packet, data, len);
            break;

        case BSQT_ATTRIB_REQ:
            SdpServHandleAttribReq(remDev, packet, data, len);
            break;

        case BSQT_SERVICE_SEARCH_ATTRIB_REQ:
            SdpServHandleServiceSearchAttribReq(remDev, packet, data, len);
            break;

        default:
            /* Not a valid query so return BSEC_BAD_SYNTAX error */
            SdpServSendError(remDev, packet, BSEC_BAD_SYNTAX);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SdpServHandleServiceSearch
 * DESCRIPTION
 *  Mark all records that match the search pattern and return the
 *  the number of matches.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  data        [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServHandleServiceSearch(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len)
{
    SdpRecord *record;
    U32 plen;
    U16 offset;
    U16 matches;
    U16 scratch;
    I8 hOffset;
    U8 contLen;
    struct _SdpServerInfo *sdpServInfo = 0;

    hOffset = BT_PACKET_HEADER_LEN - 5;

    /* Verify that the Service Search pattern is legally formed */
    if (!SdpValidateQueryParms(data[0], data + 5, (U16) (len - 5), 0, 0, BSQM_DONT_CARE))
    {
        /* Send an error packet with reason BSEC_BAD_SYNTAX */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_QUERY_PARAMETERS_ARE_NOT_VALID);
        SdpServSendError(remDev, packet, BSEC_BAD_SYNTAX);
        return;
    }

    /* Get the length of the continuation state */
    plen = SdpParseElement(data + 5, &offset);
    plen += offset;
    contLen = data[5 + plen + 2];

    /* Verify the length of the parameters.  */
    if (!SdpVerifyParmLength(remDev, packet, data, len, (I16) (plen + 3 + contLen)))
    {
        bt_trace(TRACE_GROUP_1, SDP_SERVER_HANDLESERVSEARCH__BAD_PARAMETER_LEN);
        return;
    }

    /* Put in the response opcode */
    packet->header[hOffset] = BSQR_SERVICE_SEARCH_RESP;
    sdpServInfo = remDev->sdpServInfo;

    /*
     * If the continuation state is 0 then this is a new query. We will
     * * throw out any old continuation states and start fresh
     */
    if (contLen == 0)
    {
        /* First mark all records that meet the service search pattern */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_SERVICE_SEARCH_RCV_CONTINUATION_STATE__0);
        sdpServInfo->totalMatches = SdpServMarkMatches(remDev, data + 5);
        sdpServInfo->totalMatches = min(sdpServInfo->totalMatches, BEtoHost16(data + 5 + plen));
        sdpServInfo->matchesSent = 0;
        sdpServInfo->contState = 0;
    }
    else
    {
        /* We have a continuation state so see if it is valid */
        if ((contLen != 1) || (data[5 + plen + 3] != sdpServInfo->contState))
        {
            bt_trace(TRACE_GROUP_1, SDP_SERVER_SERVICE_SEARCH_RCV_BAD_CONTINUATION_STATE);
            SdpServSendError(remDev, packet, BSEC_BAD_CONTINUATION);
            return;
        }
        Assert(sdpServInfo->totalMatches > 0);
        bt_trace(
            TRACE_GROUP_1,
            SDP_SERVER_SERVICE_SEARCH_RCV_CONT_STATE_SIZE__xD_VAL__xD,
            contLen,
            sdpServInfo->contState);
    }
    /* Insert the Total number of matches into the response packet */
    StoreBE16(packet->data, sdpServInfo->totalMatches);
    scratch = 0;
    offset = 4;

    if (sdpServInfo->totalMatches > 0)
    {
        /*
         * We have some matches so create the response. First determine how
         * many matches we can send in the first packet. We determine this
         * by starting with the maximum size we are allowed to send then
         * subtracting off the 5 bytes for the header, subracting off 4 bytes 
         * for the total record count and the current record count. Finally, 
         * we subract off 1 for the continuation state. We divide the result 
         * by 4 to determine the number of Service Record handles we can return.
         * The maximum size we can send is the minimum of the L2CAP MTU size
         * and our self imposed size.
         */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_MATCH_FOUND);
        scratch = min(L2CAP_GetTxMtu(sdpServInfo->channel), SDP_SERVER_SEND_SIZE);
        scratch -= 10;
        matches = scratch / 4;
        if (matches < (sdpServInfo->totalMatches - sdpServInfo->matchesSent))
        {
            /*
             * We do not fit in a single packet so determine the amount
             * that will fit and get a continuation state
             */
            matches = (scratch - 1) / 4;
            sdpServInfo->contState = SdpServGetNextContState();
        }
        else
        {
            /* We do fit into a single packet so continuation state is 0 */
            matches = sdpServInfo->totalMatches - sdpServInfo->matchesSent;
            sdpServInfo->contState = 0;
        }

        /* Update the matches sent */
        sdpServInfo->matchesSent += matches;

        /*
         * Now go through the records and store the values into the
         * packet. Unmark the matches.
         */
        record = (SdpRecord*) SDPS(records).Flink;
        scratch = matches;
        while ((ListEntry*) record != &SDPS(records))
        {
            if ((record->flags & sdpServInfo->recMask) && (matches > 0))
            {
                StoreBE32(packet->data + offset, record->handle);
                offset += 4;
                matches--;

                /* Unmark the record */
                record->flags &= ~sdpServInfo->recMask;
            }

            if (sdpServInfo->contState == 0)
            {
                /* This is the last or only packet so unmark the rest as well */
                record->flags &= ~sdpServInfo->recMask;
            }

            /* Advance to the next record */
            record = (SdpRecord*) record->node.Flink;
        }
        Assert(matches == 0);
    }
    else
    {
        /* We have no matches so return 0 found */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_MATCH_NOT_FOUND);
    }
    /* Put the number of current matches into the packet */
    StoreBE16(packet->data + 2, scratch);

    /* Finish sending the packet */
    SdpServFinishUpPacket(remDev, packet, (I16) offset, (I16) hOffset);

}


/*****************************************************************************
 * FUNCTION
 *  SdpServHandleAttribReq
 * DESCRIPTION
 *  Handle an attribute value request.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  data        [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServHandleAttribReq(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len)
{
    U32 plen;
    U16 offset;
    U16 scratch;
    I8 hOffset;
    U8 contLen;
    struct _SdpServerInfo *sdpServInfo = 0;

    hOffset = BT_PACKET_HEADER_LEN - 5;

    /* Verify that the Attribute request is legally formed */
    if (!SdpValidateQueryParms(data[0], data + 5, (U16) (len - 5), 0, 0, BSQM_DONT_CARE))
    {
        /* Send an error packet with reason BSEC_BAD_SYNTAX */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_QUERY_PARAMETERS_ARE_NOT_VALID);
        SdpServSendError(remDev, packet, BSEC_BAD_SYNTAX);
        return;
    }

    /* Get the length of the continuation state */
    plen = SdpParseElement(data + (5 + 6), &offset);
    plen += offset;
    /*
     * Add the header, service record handle, max attr. byte count,
     * and attribute id list length to get to the continuation state.
     */
    contLen = data[plen + (5 + 6)];

    /* Verify the length of the parameters.  */
    if (!SdpVerifyParmLength(remDev, packet, data, len, (I16) (plen + 7 + contLen)))
    {
        bt_trace(TRACE_GROUP_1, SDP_SERVER_HANDLESERVATTRIB__BAD_PARAMETER_LEN);
        return;
    }
    sdpServInfo = remDev->sdpServInfo;
    /* Determine if the service record handle is valid */
    sdpServInfo->sdpRecord = SdpServFindSdpRecord(BEtoHost32(data + 5));
    if (sdpServInfo->sdpRecord == 0)
    {
        bt_trace(TRACE_GROUP_1, SDP_SERVER_INVALID_RECORD_HANDLE);
        SdpServSendError(remDev, packet, BSEC_BAD_HANDLE);
        return;
    }

    /* Put in the response opcode */
    packet->header[hOffset] = BSQR_ATTRIB_RESP;

    /*
     * If the continuation state is 0 then this is a new query. We will
     * throw out any old continuation states and start fresh
     */
    if (contLen == 0)
    {
        /*
         * First mark all the attributes in the record that should be returned
         * and get the total size. We need to add three for the header.
         */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_ATTRIB_SEARCH_RCV_CONTINUATION_STATE__0);
        sdpServInfo->totalAttribSize = SdpServMarkAttribs(remDev, sdpServInfo->sdpRecord, data + (5 + 6));
        if (sdpServInfo->totalAttribSize > 255)
        {
            sdpServInfo->totalAttribSize += 3;
        }
        else
        {
            sdpServInfo->totalAttribSize += 2;
        }
        bt_trace(TRACE_GROUP_1, SDP_SERVER_REMDEVSDPSERVINFOTOTALATTRIBSIZE__xD, sdpServInfo->totalAttribSize);
        sdpServInfo->attribState = 0;
        sdpServInfo->contState = 0;
    }
    else
    {
        /* We have a continuation state so see if it is valid */
        if ((contLen != 1) || (data[5 + plen + 7] != sdpServInfo->contState))
        {
            bt_trace(TRACE_GROUP_1, SDP_SERVER_ATTRIB_SEARCH_RCV_BAD_CONTINUATION_STATE);
            SdpServSendError(remDev, packet, BSEC_BAD_CONTINUATION);
            return;
        }
        bt_trace(
            TRACE_GROUP_1,
            SDP_SERVER_ATTRIB_SEARCH_RCV_CONT_STATE_SIZE__xD_VAL__xD,
            contLen,
            sdpServInfo->contState);
    }

    /*
     * Determine the amount of attribute ID list data we can send in one L2CAP packet. This
     * is the largest packet we can send minus 5 bytes for the header, 2 bytes for the
     * the attribute list byte count and 2 bytes for the continuation state
     */

    /* Get the L2CAP MTU size - largest packet we can send */
    /* scratch = min(L2CAP_GetTxMtu(remDev->sdpServInfo->channel),0); */
    /* scratch = L2CAP_GetTxMtu(sdpServInfo->channel); */
    /* For MAUI_01259466: If it does not use the min value, the memory copy will be exceed the txbuff's boundary and cause mem corruption */
    scratch = min(L2CAP_GetTxMtu(remDev->sdpServInfo->channel),SDP_SERVER_SEND_SIZE);
    bt_trace(TRACE_GROUP_1, SDP_SERVER_A_SCRATCH__xD, scratch);

    /* Subtract header, attribute list byte count, and continuation state bytes */
    scratch -= (5 + 4);

    /* Now see if the MaxAttributeByteCount requires us to be smaller */
    scratch = min(scratch, BEtoHost16(data + (5 + 4)));
    bt_trace(TRACE_GROUP_1, SDP_SERVER_B_SCRATCH__xD, scratch);
    /*
     * Scratch now contains the total amount of space available for attribute data.
     * Determine if we need a continuation state or not.
     */
    if (sdpServInfo->totalAttribSize > ((U32) scratch))
    {
        /* We need at least another packet to send the data so get a continuation state */
        bt_trace(
            TRACE_GROUP_1,
            A_SDP_SERVER_REMDEVSDPSERVINFOTOTALATTRIBSIZE__xD,
            sdpServInfo->totalAttribSize);
        bt_trace(TRACE_GROUP_1, B_SDP_SERVER_SCRATCH__xD, scratch);
        sdpServInfo->contState = SdpServGetNextContState();
    }
    else
    {
        /* This fits into one packet so continuation state is 0 */
        scratch = (U16) sdpServInfo->totalAttribSize;
        bt_trace(TRACE_GROUP_1, C_SDP_SERVER_SCRATCH__xD, scratch);
        sdpServInfo->contState = 0;
    }

    /*
     * Store the attribute list byte count. Advance offset past the attrib 
     * list byte count 
     */
    StoreBE16(packet->data, scratch);
    offset = 2;

    /* Store the attribute data. This is a state machine. */
    offset = SdpStoreAttribData(remDev, packet, offset, (U16) (scratch + 2));

    /* Update the total attribute size and send the packet */
    sdpServInfo->totalAttribSize -= (U32) scratch;
    SdpServFinishUpPacket(remDev, packet, (I16) offset, (I16) hOffset);

}


/*****************************************************************************
 * FUNCTION
 *  SdpServHandleServiceSearchAttribReq
 * DESCRIPTION
 *  Handle a service search attribute value request.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  data        [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServHandleServiceSearchAttribReq(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len)
{
    SdpRecord *record;
    U32 slen, alen;
    U16 soffset, aoffset;
    U16 scratch;
    I8 hOffset;
    U8 contLen;
    I8 extraTotalAttribSize;
    U32 tempAttribSize = 0;
    BOOL sdpTesterFlag = FALSE;
    struct _SdpServerInfo *sdpServInfo = 0;

#ifdef __SDP_TESTER_
    sdpTesterFlag = sdp_query_testing_state();
#else /* __SDP_TESTER_ */ 
    sdpTesterFlag = FALSE;

#endif /* __SDP_TESTER_ */ 

    hOffset = BT_PACKET_HEADER_LEN - 5;

    /* Verify that the Service search attribute request is legally formed */
    if (!SdpValidateQueryParms(data[0], data + 5, (U16) (len - 5), 0, 0, BSQM_DONT_CARE))
    {
        /* Send an error packet with reason BSEC_BAD_SYNTAX */
        bt_trace(TRACE_GROUP_1, SDP_SERVER_SERSEARATTRREQ__QUERY_PARAMETERS_ARE_NOT_VALID);
        SdpServSendError(remDev, packet, BSEC_BAD_SYNTAX);
        return;
    }

    /* First get the length of the service search pattern */
    slen = SdpParseElement(data + 5, &soffset);
    slen += soffset;

    /* Now get the length of the attribute ID list */
    alen = SdpParseElement(data + 5 + slen + 2, &aoffset);
    alen += aoffset;

    /* Now get the length of the continuation state */
    contLen = data[5 + slen + 2 + alen];

    /* Verify the length of the parameters.  */
    if (!SdpVerifyParmLength(remDev, packet, data, len, (I16) (slen + alen + 3 + contLen)))
    {
        bt_trace(TRACE_GROUP_1, SDP_SERVER_HANDLESERVSEARCHATTRIB__BAD_PARAMETER_LEN);
        return;
    }

    /* Put in the response opcode */
    packet->header[hOffset] = BSQR_SERVICE_SEARCH_ATTRIB_RESP;

    /*
     * If the continuation state is 0 then this is a new query. We will
     * * throw out any old continuation states and start fresh
     */
    aoffset = 0;
    sdpServInfo = remDev->sdpServInfo;
    
    if (contLen == 0)
    {
        bt_trace(TRACE_GROUP_1, SDP_SERVER_SERVICE_SEARCH_RCV_CONINUATION_STATE__0);
        /* First mark all records that meet the service search pattern */
        SdpServMarkMatches(remDev, data + 5);

        /* Determine the total size of the attribute ID list. */
        if (!sdpTesterFlag)
        {
            sdpServInfo->totalServAttribSize = 3;
        }
        else
        {
            sdpServInfo->totalServAttribSize = 0;
        }
        record = (SdpRecord*) SDPS(records).Flink;
        while ((ListEntry*) record != &SDPS(records))
        {
            if (record->flags & sdpServInfo->recMask)
            {
                /* This is a marked record so mark the attributes */
                tempAttribSize = SdpServMarkAttribs(remDev, record, data + (5 + 2 + slen));
                if (tempAttribSize > 255)
                {
                    sdpServInfo->totalServAttribSize += (tempAttribSize + 3);
                }
                else
                {
                    sdpServInfo->totalServAttribSize += (tempAttribSize + 2);
                }
            }
            /* Advance to the next record */
            record = (SdpRecord*) record->node.Flink;
        }

        if (!sdpTesterFlag)
        {
            /* Put in the header for the attribute ID list */
            if (sdpServInfo->totalServAttribSize > 255)
            {
                packet->data[2] = DETD_SEQ + DESD_ADD_16BITS;
                StoreBE16(packet->data + 3, (U16) (sdpServInfo->totalServAttribSize - 3));
                aoffset = 3;
                extraTotalAttribSize = 3;
            }
            else
            {
                sdpServInfo->totalServAttribSize -= 1;
                packet->data[2] = DETD_SEQ + DESD_ADD_8BITS;
                packet->data[3] = (U8) (remDev->sdpServInfo->totalServAttribSize - 2);
                aoffset = 2;
                extraTotalAttribSize = 2;
            }
        }
        else
        {
            aoffset = 0;
            extraTotalAttribSize = 0;
        }
        sdpServInfo->contState = 0;
        sdpServInfo->attribState = 6;
    }
    else
    {
        /* We have a continuation state so see if it is valid */
        if ((contLen != 1) || (data[5 + slen + alen + 3] != sdpServInfo->contState))
        {
            bt_trace(TRACE_GROUP_1, SDP_SERVER_HANDLESERVSEARCHATTRIB__RCV_BAD_CONTINUATION_STATE);
            SdpServSendError(remDev, packet, BSEC_BAD_CONTINUATION);
            return;
        }
        bt_trace(
            TRACE_GROUP_1,
            SDP_SERVER_HANDLESERVSEARCHATTRIB__RCV_CONT_STATE_SIZE__xD_VAL__xD,
            contLen,
            sdpServInfo->contState);
        extraTotalAttribSize = 0;
    }

    /*
     * Determine the amount of attribute ID list data we can send in one L2CAP packet. This
     * is the largest packet we can send minus 5 bytes for the header, 2 bytes for the
     * the attribute list byte count and 2 bytes for the continuation state
     */

    /* Get the L2CAP MTU size - largest packet we can send */
    scratch = min(L2CAP_GetTxMtu(sdpServInfo->channel), SDP_SERVER_SEND_SIZE);

    /* Subtract header, attribute list byte count, and continuation state bytes */
    scratch -= (5 + 4);

    /* Now see if the MaxAttributeByteCount requires us to be smaller */
    scratch = min(scratch, BEtoHost16(data + (5 + slen)));

    /*
     * Scratch now contains the total amount of space available for attribute ID list
     * data in a packet. Determine if we need a continuation state or not.
     */
    if (sdpServInfo->totalServAttribSize > (U32) scratch)
    {
        /* We need at least another packet to send the data so get a continuation state */
        sdpServInfo->contState = SdpServGetNextContState();
    }
    else
    {
        /* This fits into one packet so continuation state is 0 */
        scratch = (U16) sdpServInfo->totalServAttribSize;
        sdpServInfo->contState = 0;
    }

    /*
     * Store the attribute lists byte count. Advance offset past the attrib 
     * list byte count 
     */
    StoreBE16(packet->data, scratch);
    aoffset += 2;

    /* Adjust scratch to account for attrib lists byte count */
    scratch += 2;

    /* Adjust totalServAttribSize for the initial list header */
    sdpServInfo->totalServAttribSize -= extraTotalAttribSize;

    /* Now load in the attribute ID list info */
    while (aoffset < scratch)
    {
        if (sdpServInfo->attribState < 6)
        {
            /* Storing the attribute data */
            soffset = (U16) min(sdpServInfo->totalAttribSize, (U32) (scratch - aoffset));
            aoffset = SdpStoreAttribData(remDev, packet, aoffset, (U16) (soffset + aoffset));
            sdpServInfo->totalAttribSize -= (U32) soffset;
            sdpServInfo->totalServAttribSize -= (U32) soffset;

            /* Determine the next state */
            if (sdpServInfo->totalAttribSize == 0)
            {
                /* This record is done so goto the next record */
                sdpServInfo->attribState = 6;
            }
        }
        else
        {
            Assert(sdpServInfo->attribState == 6);
            /* Get the next record */
            sdpServInfo->sdpRecord = SdpServFindMarkedRecord(sdpServInfo->recMask);
            Assert(sdpServInfo->sdpRecord != 0);

            /* Mark the attributes and determine the amount of data to store */
            sdpServInfo->totalAttribSize = SdpServMarkAttribs(remDev, sdpServInfo->sdpRecord, data + (5 + 2 + slen));
            if (sdpServInfo->totalAttribSize > 255)
            {
                sdpServInfo->totalAttribSize += 3;
            }
            else
            {
                sdpServInfo->totalAttribSize += 2;
            }
            sdpServInfo->totalAttribSize = min(sdpServInfo->totalAttribSize, sdpServInfo->totalServAttribSize);
            sdpServInfo->attribState = 0;

            /* Clear the record flag */
            sdpServInfo->sdpRecord->flags &= ~sdpServInfo->recMask;
        }
    }

    /* Finish sending the packet */
    SdpServFinishUpPacket(remDev, packet, (I16) aoffset, (I16) hOffset);

}


/*****************************************************************************
 * FUNCTION
 *  SdpServSendError
 * DESCRIPTION
 *  Mark all records that match the search pattern and return the
 *  the number of matches.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  errCode     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServSendError(BtRemoteDevice *remDev, BtPacket *packet, SdpErrorCode errCode)
{
    I8 hOffset;
    BtStatus status;

    hOffset = BT_PACKET_HEADER_LEN - 5;

    /*
     * Put in the PDU ID and the parameter size. The parameter size
     * is 2 for the error code
     */
    packet->header[hOffset] = BSQR_ERROR_RESP;
    StoreBE16(packet->header + hOffset + 3, 2);

    /* Put in the error code */
    StoreBE16(packet->data, errCode);

    /* Set the sizes */
    packet->headerLen = 5;
    packet->dataLen = 2;

    /* Call L2CAP to send the data */
    status = L2CAP_SendData(remDev->sdpServInfo->channel, packet);
    if (status != BT_STATUS_PENDING)
    {
        /*
         * The send failed.  Clear the txPacketBusy flag, since the 
         * packet will never complete.
         */
        remDev->sdpServInfo->txPacketBusy = FALSE;
        bt_trace(TRACE_GROUP_1, SDP_SERVER_L2CAP_SENDDATA_CALL_FAILED_ERROR__xX, status);
    }
    bt_trace(TRACE_GROUP_1, SDP_SERVER_SENDING_AN_ERROR_ERROR__xX, errCode);
}

/*---------------------------------------------------------------------------
 *            SdpServMarkMatches()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Mark all records that match the service search pattern and 
 *            return the number of matches. 
 *
 * Return:    Number of records that match the service search pattern. 
 */


/*****************************************************************************
 * FUNCTION
 *  SdpServMarkMatches
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev          [IN]        
 *  searchPat       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 SdpServMarkMatches(BtRemoteDevice *remDev, U8 *searchPat)
{
    SdpRecord *record;
    U32 len;
    U8 *p;
    U8 *p2;
    U16 offset;
    U16 matches;

#ifdef __MTK_BT_SKIP_GATT_SERVICE__
	/* For the IOT issue which is caused by the default Bluetooth on Android 4.0.
	  * The UUID of Generic Access Profile (GAP, 0x1800) will cause some non-MTK devices fail to boot.
	  */
	U32 mtkchip = ME_IsMtkBTChip(remDev);
	U8	const UUID_GapOverGatt[] = {
		SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_GAP)
	};
#endif

	/*
	 * Mark every record in the data base that has all the UUIDs of the
	 * service search pattern
	 */
    matches = 0;
    record = (SdpRecord*) SDPS(records).Flink;
    while ((ListEntry*) record != &SDPS(records))
    {

#ifdef __MTK_BT_SKIP_GATT_SERVICE__
    	if (mtkchip == 0 && SdpFindUuidInRecord(record, UUID_GapOverGatt)) {
			// Simply skip this record
    	} else
#endif
    	{
	        /*
		         * Make sure the record flag is clean. This is needed in case
	       	  * we aborting a continuation query
	       	  */
    	    record->flags &= ~remDev->sdpServInfo->recMask;

	        /*
       		  * For each UUID in the service search pattern determine
		         * if it exists in the attributes of the record
       		  */
	        len = SdpParseElement(searchPat, &offset);
    	    p = searchPat + offset;
	        p2 = p + len;
    	    while (p < p2)
	        {
    	        if (!SdpFindUuidInRecord(record, p))
        	    {
	                /*
			                 * This UUID is not in the record so break out of the
       		       	   * the loop.
              			   */
	                break;
    	        }

	            /* Advance to the next UUID */
    	        p += SdpParseElement(p, &offset);
        	    p += offset;
	        }
	        Assert(p <= p2);

	        if (p == p2)
    	    {
        	    /* This record contains all UUIDs so mark it */
	            record->flags |= remDev->sdpServInfo->recMask;
    	        matches++;
	        }
		}
        /* Advance to the next record */
        record = (SdpRecord*) record->node.Flink;
    }
    return matches;
}


/*****************************************************************************
 * FUNCTION
 *  SdpFindUuidInRecord
 * DESCRIPTION
 *  Determine if the UUID exists in the record
 * PARAMETERS
 *  record      [IN]        
 *  uuid        [IN]        
 * RETURNS
 *  TRUE if UUID exist. FALSE otherwise.
 *****************************************************************************/
BOOL SdpFindUuidInRecord(SdpRecord *record, const U8 *uuid)
{
    U32 len;
    U16 offset;
    U8 i;

    len = SdpParseElement(uuid, &offset);

    for (i = 0; i < record->num; i++)
    {
        if (SdpFindUuidInAttrib(record->attribs[i].value, uuid + offset, (U16) len, 0))
        {
            return TRUE;
        }
    }
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  SdpFindUuidInAttrib
 * DESCRIPTION
 *  Determine if the UUID exists in the attribute value
 * PARAMETERS
 *  aVal        [IN]        
 *  uuid        [IN]        
 *  uuidLen     [IN]        
 *  level       [IN]        
 * RETURNS
 *  TRUE if UUID exist. FALSE otherwise.
 *****************************************************************************/
BOOL SdpFindUuidInAttrib(const U8 *aVal, const U8 *uuid, U16 uuidLen, U16 level)
{
    U32 len, i;
    U16 offset;
    const U8 *p;

    if (level > SDP_SERVER_MAX_LEVEL)
    {
        bt_trace(TRACE_GROUP_1, SDPFINDUUIDINATTRIB_MAX_RECURSION_LEVEL_EXCEEDED);
        return FALSE;
    }

    switch (aVal[0] & DETD_MASK)
    {
        case DETD_UUID:
            /*
             * The UUID in the database should be in its smallest form as should
             * the query one. So for now just compare them.
             */
            len = SdpParseElement(aVal, &offset);
            return (SdpUuidCmp((const U8*)(aVal + 1), (U16) len, uuid, uuidLen));

        case DETD_SEQ:
        case DETD_ALT:
            /*
             * Go through the sequence and look for UUID entries. First
             * determine the size of the sequence.
             */
            len = SdpParseElement(aVal, &offset);
            p = (aVal + offset);
            i = 0;
            while (i < len)
            {
                /*
                 * Go through the sequence to see if any UUIDs match. Use
                 * this function recursively on each element
                 */
                if (SdpFindUuidInAttrib(p + i, uuid, uuidLen, (U16) (level + 1)))
                {
                    return TRUE;
                }

                /* Advance to the next element */
                i += SdpParseElement(p + i, &offset);
                i += (U32) offset;
            }
            Assert(i == len);
            break;
    }
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  SdpServMarkAttribs
 * DESCRIPTION
 *  Mark all records that match the search pattern and return the
 *  the number of matches.
 * PARAMETERS
 *  remDev      [IN]        
 *  record      [IN]        
 *  list        [IN]        
 * RETURNS
 *  Total length of the marked attributes
 *****************************************************************************/
U32 SdpServMarkAttribs(BtRemoteDevice *remDev, SdpRecord *record, U8 *list)
{
    U32 len, i, j, begin, last;
    U16 offset;
    U32 totalLen;

    /* First clear out any existing marks */
    record->handleFlag &= ~remDev->sdpServInfo->recMask;
    record->stateFlag &= ~remDev->sdpServInfo->recMask;
    for (i = 0; i < (U32) record->num; i++)
    {
        record->attribs[i].flags &= ~remDev->sdpServInfo->recMask;
    }

    len = SdpParseElement(list, &offset);
    i = 0;
    totalLen = 0;

    /*
     * Go through the attribute ID list and mark every attribute
     * * that needs to be returned
     */
    while (i < len)
    {
        switch (list[offset + i] & DESD_MASK)
        {
            case DESD_2BYTES:
                /* Single attribute */
                j = (U32) BEtoHost16((list + offset + i + 1));
                totalLen += (U32) SdpMarkAttribute(remDev, record, (U16) j);
                i += 3;
                break;

            case DESD_4BYTES:
                /* Range of attributes */
                begin = (U32) BEtoHost16((list + offset + i + 1));
                last = (U32) BEtoHost16((list + offset + i + 3));
                /* Mark Service Record Handle is the range */
                if (begin == 0)
                {
                    totalLen += (U32) SdpMarkAttribute(remDev, record, 0);
                }
                /* Mark Service Record State is in the range */
                if ((2 >= begin) && (2 <= last))
                {
                    totalLen += (U32) SdpMarkAttribute(remDev, record, 2);
                }

                /* Mark all attributes within the range that exist in the record */
                for (j = 0; j < record->num; j++)
                {
                    if ((record->attribs[j].id >= begin) && (record->attribs[j].id <= last))
                    {
                        totalLen += (U32) SdpMarkAttribute(remDev, record, record->attribs[j].id);
                    }
                }
                i += 5;
                break;

                /*
                 * Only 2 byte and 4 byte attribute ID values can exist.
                 * VerifyAttributeIdList() guarantees this.
                 */

        }
    }
    return totalLen;
}

/*---------------------------------------------------------------------------
 *            SdpServGetNextContState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return the next valid continuation state. 
 *
 * Return:    U8
 */


/*****************************************************************************
 * FUNCTION
 *  SdpServGetNextContState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 SdpServGetNextContState(void)
{
    SDPS(contState) += 1;
    if (SDPS(contState) == 0)
    {
        SDPS(contState) = 1;
    }
    return SDPS(contState);
}


/*****************************************************************************
 * FUNCTION
 *  SdpServFindSdpRecord
 * DESCRIPTION
 *  Return a pointer to the record with the given record number or 0.
 * PARAMETERS
 *  handle      [IN]        
 * RETURNS
 *  SdpRecord*
 *****************************************************************************/
SdpRecord *SdpServFindSdpRecord(U32 handle)
{
    SdpRecord *record;

    record = (SdpRecord*) SDPS(records).Flink;
    while ((ListEntry*) record != &SDPS(records))
    {
        if (record->handle == handle)
        {
            return record;
        }
        /* Advance to the next record */
        record = (SdpRecord*) record->node.Flink;
    }
    return 0;

}


/*****************************************************************************
 * FUNCTION
 *  SdpServFindMarkedAttribute
 * DESCRIPTION
 *  Return a pointer to the first marked attribute.
 * PARAMETERS
 *  record      [IN]        
 *  mask        [IN]        
 * RETURNS
 *  SdpAttribute*
 *****************************************************************************/
SdpAttribute *SdpServFindMarkedAttribute(SdpRecord *record, U16 mask)
{
    U8 i;

    for (i = 0; i < record->num; i++)
    {
        if (record->attribs[i].flags & mask)
        {
            return &(record->attribs[i]);
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  SdpServFindMarkedRecord
 * DESCRIPTION
 *  Return a pointer to the first marked attribute.
 * PARAMETERS
 *  mask        [IN]        
 * RETURNS
 *  SdpAttribute*
 *****************************************************************************/
SdpRecord *SdpServFindMarkedRecord(U16 mask)
{
    SdpRecord *record;

    record = (SdpRecord*) SDPS(records).Flink;
    while ((ListEntry*) record != &SDPS(records))
    {
        if (record->flags & mask)
        {
            return record;
        }
        /* Advance to the next record */
        record = (SdpRecord*) record->node.Flink;
    }
    return 0;

}


/*****************************************************************************
 * FUNCTION
 *  SdpMarkAttribute
 * DESCRIPTION
 *  Mark the given attribute in the given record if it exists.
 * PARAMETERS
 *  remDev      [IN]        
 *  record      [IN]        
 *  id          [IN]        
 * RETURNS
 *  The total size of the attribute including the header and
 *  attribute ID.
 *****************************************************************************/
U16 SdpMarkAttribute(BtRemoteDevice *remDev, SdpRecord *record, U16 id)
{
    U16 offset;
    U8 i;
    U16 size;
    struct _SdpServerInfo *sdpServInfo = 0;  

    sdpServInfo = remDev->sdpServInfo;
    Assert(sdpServInfo != 0);

    size = 0;
    for (i = 0; i < record->num; i++)
    {
        if ((id == 0) && !(record->handleFlag & sdpServInfo->recMask))
        {
            bt_trace(TRACE_GROUP_1, SDP_SERVER_ATTRIBUTE_ID__0_MARKED);
            record->handleFlag |= sdpServInfo->recMask;
            /* Header and Attribute ID + Header and Service Record Handle */
            size = 3 + 5;
            break;
        }
        else if ((id == 2) && !(record->stateFlag & sdpServInfo->recMask))
        {
            bt_trace(TRACE_GROUP_1, SDP_SERVER_ATTRIBUTE_ID__2_MARKED);
            record->stateFlag |= sdpServInfo->recMask;
            /* Header and Attribute ID + Header and Service Record State */
            size = 3 + 5;
            break;
        }
        else if ((record->attribs[i].id == id) && !(record->attribs[i].flags & sdpServInfo->recMask))
        {
            bt_trace(TRACE_GROUP_1, SDP_SERVER_ATTRIBUTE_ID__xD_MARKED, id);
            record->attribs[i].flags |= sdpServInfo->recMask;
            /* Get the length of the attribute */
            size = (U16) SdpParseElement(record->attribs[i].value, &offset);
            /* Header offset + Attribute ID and Header */
            size += offset + 3;
            break;
        }
    }
    return size;
}


/*****************************************************************************
 * FUNCTION
 *  SdpVerifyParmLength
 * DESCRIPTION
 *  Verify the length of the parameters.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  data        [IN]        
 *  len         [IN]        
 *  len2        [IN]        
 * RETURNS
 *  TRUE if valid otherwise FALSE
 *****************************************************************************/
BOOL SdpVerifyParmLength(BtRemoteDevice *remDev, BtPacket *packet, U8 *data, I16 len, I16 len2)
{
    /* Verify the length of the parameters.  */
    if (((U32) (len - 5) < len2) || (BEtoHost16(data + 3) < len2) || (BEtoHost16(data + 3) < (len - 5)) ||
        (BEtoHost16(data + 3) > (len - 5)) || (BEtoHost16(data + 3) > len2))
    {
        bt_trace(TRACE_GROUP_1, SDP_SERVER_QUERY_PARMETERS_LEN_IS_BAD);
        SdpServSendError(remDev, packet, BSEC_BAD_PDU_SIZE);
        return FALSE;
    }

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  SdpStoreAttribData
 * DESCRIPTION
 *  Store the marked attribute data into the packet.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  offset      [IN]        
 *  final       [IN]        
 * RETURNS
 *  Return the new offset
 *****************************************************************************/
U16 SdpStoreAttribData(BtRemoteDevice *remDev, BtPacket *packet, U16 offset, U16 final)
{
    U16 len;
    struct _SdpServerInfo *sdpServInfo = 0;

    sdpServInfo = remDev->sdpServInfo;
    /* Load in attribute info */
    while (offset < final)
    {
        bt_trace(TRACE_GROUP_1, OFFSETxD_FINALxDSTATExD, offset, final, sdpServInfo->attribState);
        switch (sdpServInfo->attribState)
        {
            case 0:
                /* Create the the attibute list header in the scratch */
                /* remDev->sdpServInfo->attribScratch[0] = DETD_SEQ + DESD_ADD_16BITS; */
                if (sdpServInfo->totalAttribSize > 255)
                {
                    sdpServInfo->attribScratch[0] = DETD_SEQ + DESD_ADD_16BITS;
                    StoreBE16(sdpServInfo->attribScratch + 1, (U16) (sdpServInfo->totalAttribSize - 3));
                    sdpServInfo->scratchLeft = 3;
                }
                else
                {
                    sdpServInfo->attribScratch[0] = DETD_SEQ + DESD_ADD_8BITS;
                    sdpServInfo->attribScratch[1] = (U8)(sdpServInfo->totalAttribSize - 2);
                    sdpServInfo->scratchLeft = 2;
                }
                sdpServInfo->scratchOff = 0;
                /* remDev->sdpServInfo->scratchLeft = 3; */
                sdpServInfo->attribState = 2;
                break;

            case 1:
                /* Create the attribute ID and value header in the scratch area */
                sdpServInfo->attribScratch[0] = DETD_UINT + DESD_2BYTES;
                sdpServInfo->scratchOff = 0;

                if (sdpServInfo->sdpRecord->handleFlag & sdpServInfo->recMask)
                {
                    /*
                     * We need to send the record handle attribute  so put
                     * the attibute ID and the value into the scratch area.
                     */
                    bt_trace(TRACE_GROUP_1, SDP_SERVER_STOREATTRIB__STORING_ID_0_INTO_SCRATCH);
                    StoreBE16(sdpServInfo->attribScratch + 1, 0);
                    sdpServInfo->attribScratch[3] = DETD_UINT + DESD_4BYTES;
                    StoreBE32(sdpServInfo->attribScratch + 4, sdpServInfo->sdpRecord->handle);
                    sdpServInfo->scratchLeft = 8;
                    sdpServInfo->attribState = 3;
                    sdpServInfo->sdpRecord->handleFlag &= ~sdpServInfo->recMask;
                }
                else if (sdpServInfo->sdpRecord->attribs[0].flags & sdpServInfo->recMask)
                {
                    /* We need to send the service class Id list attribute */
                    bt_trace(TRACE_GROUP_1, SDP_SERVER_STOREATTRIB__STORING_ID_1_INTO_SCRATCH);
                    sdpServInfo->sdpAttrib = SdpServFindMarkedAttribute(sdpServInfo->sdpRecord, sdpServInfo->recMask);
                    /* Make sure we have the first attribute */
                    Assert(sdpServInfo->sdpAttrib == &(sdpServInfo->sdpRecord->attribs[0]));

                    /* Store the attribute ID in the scratch area */
                    StoreBE16(sdpServInfo->attribScratch + 1, sdpServInfo->sdpAttrib->id);
                    /* This had better be the service class Id list */
                    Assert(sdpServInfo->sdpAttrib->id == 0x0001);

                    sdpServInfo->scratchLeft = 3;
                    sdpServInfo->attribState = 4;

                    /* Clear the flag for this attribute */
                    sdpServInfo->sdpRecord->attribs[0].flags &= ~sdpServInfo->recMask;

                }
                else if (sdpServInfo->sdpRecord->stateFlag & sdpServInfo->recMask)
                {
                    /*
                     * We need to send the service record state attribute  so put
                     * the attibute ID and the value into the scratch area.
                     */
                    bt_trace(TRACE_GROUP_1, SDP_SERVER_STOREATTRIB__STORING_ID_2_INTO_SCRATCH);
                    StoreBE16(sdpServInfo->attribScratch + 1, 2);
                    sdpServInfo->attribScratch[3] = DETD_UINT + DESD_4BYTES;
                    StoreBE32(sdpServInfo->attribScratch + 4, sdpServInfo->sdpRecord->recordState);
                    sdpServInfo->scratchLeft = 8;
                    sdpServInfo->attribState = 3;
                    sdpServInfo->sdpRecord->stateFlag &= ~sdpServInfo->recMask;
                }
                else
                {
                    /* Find the next marked attribute */
                    bt_trace(TRACE_GROUP_1, SDP_SERVER_ATTRIB_SEARCH_STORING_HEADER_INTO_SCRATCH);
                    sdpServInfo->sdpAttrib = SdpServFindMarkedAttribute(sdpServInfo->sdpRecord, sdpServInfo->recMask);
                    Assert(sdpServInfo->sdpAttrib != 0);

                    /* Store the attribute ID in the scratch area */
                    StoreBE16(sdpServInfo->attribScratch + 1, sdpServInfo->sdpAttrib->id);
                    sdpServInfo->scratchLeft = 3;
                    sdpServInfo->attribState = 4;
                }
                break;

            case 2:
            case 3:
            case 4:
                /* Copy the scratch into the packet */
                len = min(sdpServInfo->scratchLeft, final - offset);
                bt_trace(TRACE_GROUP_1, SDP_SERVER_COPYING_THE_SCRATCH_INTO_A_PACKET_LENxD, len);
                /* For MAUI_01259466: Boundary check to avoid that the memory copy will be exceed the txbuff's boundary to cause mem corruption */
                if ((offset + (U16)len) > (SDP_SERVER_SEND_SIZE -5))
                {   
                    /* In this case, it will corrupt the other memory context */
                    Assert(0);
                }
                OS_MemCopy(
                    packet->data + offset,
                    sdpServInfo->attribScratch + sdpServInfo->scratchOff,
                    (U16) len);
                bt_trace(
                    TRACE_GROUP_1,
                    PACKETDATAOFFSETBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
                    packet->data[offset],
                    packet->data[offset + 1],
                    packet->data[offset + 2],
                    packet->data[offset + 3]);
                offset += len;
                sdpServInfo->scratchLeft -= len;
                sdpServInfo->scratchOff += len;

                if (sdpServInfo->scratchLeft == 0)
                {
                    /* All of the scratch has been stored so set the next state */
                    if (sdpServInfo->attribState < 4)
                    {
                        /*
                         * We just stored a whole attribute or the list header
                         * so get next attribute 
                         */
                        sdpServInfo->attribState = 1;
                    }
                    else
                    {
                        /* Now we need to send the value portion */
                        sdpServInfo->attribState = 5;
                        sdpServInfo->curAttribTotal = 0;
                    }
                }
                break;

            case 5:
                /* Copy the attribute value */
                if (sdpServInfo->curAttribTotal == 0)
                {
                    /* This is the start so get the length of the value */
                    sdpServInfo->curAttribTotal =
                        (U16) SdpParseElement(
                                sdpServInfo->sdpAttrib->value,
                                &(sdpServInfo->curAttribOff));
                    sdpServInfo->curAttribTotal += sdpServInfo->curAttribOff;
                    sdpServInfo->curAttribOff = 0;
                }

                /* Copy as much of the value as we can */
                len = min(sdpServInfo->curAttribTotal, final - offset);
                /* For MAUI_01259466: Boundary check to avoid that the memory copy will be exceed the txbuff's boundary to cause mem corruption */
                if ((offset + (U16)len) > (SDP_SERVER_SEND_SIZE -5))
                {   
                    /* In this case, it will corrupt the other memory context */
                    Assert(0);
                }
                OS_MemCopy(
                    packet->data + offset,
                    sdpServInfo->sdpAttrib->value + sdpServInfo->curAttribOff,
                    (U16) len);
                offset += len;
                sdpServInfo->curAttribTotal -= len;
                sdpServInfo->curAttribOff += len;

                if (sdpServInfo->curAttribTotal == 0)
                {
                    /*
                     * We have copied the whole attribute so clear the attribute flag and
                     * get the next attribute 
                     */
                    sdpServInfo->sdpAttrib->flags &= ~sdpServInfo->recMask;
                    sdpServInfo->attribState = 1;
                }
                break;
        }
    }
    Assert(offset == final);
    return offset;
}


/*****************************************************************************
 * FUNCTION
 *  SdpServFinishUpPacket
 * DESCRIPTION
 *  Finish up sending the server packet.
 * PARAMETERS
 *  remDev      [IN]        
 *  packet      [IN]        
 *  offset      [IN]        
 *  hOffset     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpServFinishUpPacket(BtRemoteDevice *remDev, BtPacket *packet, I16 offset, I16 hOffset)
{
    BtStatus status;
    struct _SdpServerInfo *sdpServInfo = 0;

    sdpServInfo = remDev->sdpServInfo;
    /* Put in the continuation state */
    if (sdpServInfo->contState != 0)
    {
        packet->data[offset] = 1;
        offset++;
        packet->data[offset] = sdpServInfo->contState;
        bt_trace(TRACE_GROUP_1, SDP_SERVER_SENDING_CONT_STATE__xD, sdpServInfo->contState);
    }
    else
    {
        packet->data[offset] = 0;
        bt_trace(TRACE_GROUP_1, SDP_SERVER_SENDING_CONT_STATE_0);
    }

    /* Store the parameter size */
    StoreBE16((packet->header + hOffset + 3), (U16) (offset + 1));

    /* Update the sizes */
    packet->headerLen = 5;
    packet->dataLen = (U16) (offset + 1);
    packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    /* Call L2CAP to send the data */
    status = L2CAP_SendData(sdpServInfo->channel, packet);

    if (status != BT_STATUS_PENDING)
    {
        /*
         * The send failed.  Clear the txPacketBusy flag, since the 
         * packet will never complete.
         */
        sdpServInfo->txPacketBusy = FALSE;
        bt_trace(TRACE_GROUP_1, SDP_SERVER_L2CAP_SENDDATA_CALL_FAILED_ERROR__xX, status);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SdpHandleMarkedRecord
 * DESCRIPTION
 *  If this record or its attributes are marked then it may be
 *  part of an ongoing query where the result did not fit in
 *  a single L2CAP packet. This means we should set the
 *  continuation state of all connections whose mark appears in the
 *  record to 0. This will cause future queries involving this record
 *  fail gracefully.
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpHandleMarkedRecord(SdpRecord *record)
{
    I8 i;
    U16 mask;
    BtRemoteDevice *remDev;

    /*
     * Go through the table of remote devices if their mark appears in the record
     * or its attributes then set its continuation state to 0
     */
    for (i = 0; (remDev = MeEnumerateRemoteDevices(i)) != 0; i++)
    {
        Assert(remDev != 0);

        if (remDev->sdpServInfo != 0)
        {
            mask = remDev->sdpServInfo->recMask;

            if ((record->flags & mask) || (record->handleFlag & mask) || (record->stateFlag & mask) ||
                (SdpServFindMarkedAttribute(record, mask) != 0))
            {

                /* The record is marked so set the contuation state to 0 */
                bt_trace(TRACE_GROUP_1, SDPSERVER_REMOVED_RECORD_IS_MARKED_SETTING_CONT_STATE_TO_0_FOR_REMDEV_xD, i);
                remDev->sdpServInfo->contState = 0;
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  SdpServGetInfo
 * DESCRIPTION
 *  Get a free SdpServerInfo structure. Return the pointer if one
 *  is found otherwise return 0.
 * PARAMETERS
 *  void
 * RETURNS
 *  SdpServerInfo*
 *****************************************************************************/
SdpServerInfo *SdpServGetInfo(void)
{
    I8 i;
    SdpServerInfo *info;
    BtRemoteDevice *remDev;
    BtRemoteDevice *saveRemDev;

    info = 0;
    for (i = 0; i < SDP_ACTIVE_CLIENTS; i++)
    {
        if (SDPS(serverInfos)[i].busy == FALSE)
        {
            info = &(SDPS(serverInfos)[i]);
            Assert(info->txPacketBusy == FALSE);
            info->txPacketBusy = FALSE;
            goto done;
        }
    }

    /*
     * There are no free ones so we need to find a parked device
     * and take his.
     */
    saveRemDev = 0;
    for (i = 0; (remDev = MeEnumerateRemoteDevices(i)) != 0; i++)
    {
        if ((remDev->sdpServInfo != 0) && (remDev->state == BDS_CONNECTED) && (remDev->mode == BLM_PARK_MODE))
        {
            /*
             * We have candidate. The one we want will have its contState 
             * set to 0. If we can't find one of those then we will take
             * any we can find.
             */
            if (remDev->sdpServInfo->contState == 0)
            {
                info = remDev->sdpServInfo;
                remDev->sdpServInfo = 0;
                goto done;
            }
            else
            {
                /*
                 * This is not exactly what we want but we will save it just
                 * in case we can't find something better
                 */
                saveRemDev = remDev;
            }
        }
    }
    /* See if we have to use the last resort */
    if (saveRemDev != 0)
    {
        info = saveRemDev->sdpServInfo;
        saveRemDev->sdpServInfo = 0;
    }

  done:
    if (info != 0)
    {
        info->busy = TRUE;
        info->contState = 0;
    }
    return info;

}


/*****************************************************************************
 * FUNCTION
 *  SdpServUpdateClassOfDevice
 * DESCRIPTION
 *  Update the class of device field and if a change
 *  has occured then write the value out.
 * PARAMETERS
 *  void
 * RETURNS
 *  SdpRecord*
 *****************************************************************************/
void SdpServUpdateClassOfDevice(void)
{
    SdpRecord *record;
    BtClassOfDevice saveCod;

    saveCod = MEC(sdpServCoD);

    /* Preserve the Limited Discoverable state */
    MEC(sdpServCoD) = saveCod & COD_LIMITED_DISC_MASK;
    record = (SdpRecord*) SDPS(records).Flink;
    while ((ListEntry*) record != &SDPS(records))
    {
        /* Advance to the next record */
        record = (SdpRecord*) record->node.Flink;
    }
    if (MEC(sdpServCoD) != saveCod)
    {
        MeWriteClassOfDevice();
    }

}

#ifdef BT_FOR_NONE_MP_ONLY


/*****************************************************************************
 * FUNCTION
 *  SdpServVerifyRecord
 * DESCRIPTION
 *  Verify that the SDP record is properly formed. Verify that
 *  there are no duplicate Attribute IDs and that the attributes
 *  are in ascending order. Verify the values are properly formed.
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  TRUE if valid otherwise FALSE.
 *****************************************************************************/
BOOL SdpServVerifyRecord(SdpRecord *record)
{
    U8 i;
    U16 lastId;

    /* There must be at least one attribute */
    if (record->num == 0)
    {
        return FALSE;
    }

    /*
     * Since the ID *must* be in order and the AID_SERVICE_CLASS_ID_LIST
     * must be in the record it must be in the first slot. Bug 884 
     */
    if (record->attribs[0].id != AID_SERVICE_CLASS_ID_LIST)
    {
        return FALSE;
    }

    /* Check each attribute */
    lastId = 0;
    for (i = 0; i < record->num; i++)
    {
        if (record->attribs[i].id == 2)
        {
            /*
             * SDP Server automatically handles Service Record 
             * State attribute (0x0002)
             */
            return FALSE;
        }
        if (record->attribs[i].id <= lastId)
        {
            /* Found a duplicate attribute, out of order attribute ID,
               or the Service Record Handle attribute (0x0000) */
            return FALSE;
        }
        lastId = record->attribs[i].id;

        if (!SdpVerifyDataElement(record->attribs[i].value, record->attribs[i].len, SDP_SERVER_MAX_LEVEL))
        {
            return FALSE;
        }
    }
    return TRUE;
}
#endif /* BT_FOR_NONE_MP_ONLY */ 

/*---------------------------------------------------------------------------
 *            SdpUpdateServiceDatabaseState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Updates the SDP Server's Service Database State attribute.  
 *            Modification of this attribute indicates that an SDP record
 *            has been added or removed.
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  SdpUpdateServiceDatabaseState
 * DESCRIPTION
 *  Updates the SDP Server's Service Database State attribute.
 *  Modification of this attribute indicates that an SDP record
 *  has been added or removed.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 *****************************************************************************/
BtStatus SdpUpdateServiceDatabaseState(void)
{
    BtStatus status;

    if (!IsNodeOnList(&SDPS(records), &(SDPS(record0).node)))
    {
        status = BT_STATUS_FAILED;
    }
    else
    {
        RemoveEntryList(&(SDPS(record0).node));

        /*
         * Store a new value into the ServiceDatabaseState
         * Use OS_GetSystemTime() to prevent rollover problems 
         */
        StoreBE32(SDPS(ZeroServiceDatabaseState + 1), OS_GetSystemTime());

        /*
         * Add back the Sevice Discovery entry (Record Zero) to the 
         * SDP server 
         */
        InsertTailList(&(SDPS(records)), &(SDPS(record0).node));
        status = BT_STATUS_SUCCESS;
    }
    return status;
}

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)

extern U8 BTQueryPowerOnState(void);

/*****************************************************************************
 * FUNCTION
 *  SdpUpdateEirDataBase
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len                 [IN]        
 *  attribute_value     [IN]        
 *  action              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpUpdateEirDataBase(U16 len, const U8 *attribute_value, U8 action)
{
    U8 service_class_size = 0;
    U8 parse_count = 0;
    const U8 *parse_ptr = attribute_value;
    U8 avoid_infinite_loop = 0;
    U16 service_class;

    bt_trace(TRACE_GROUP_1, BTLOG_SDPUPDATEEIRDATABASEx02X, parse_ptr[0]);
    bt_trace(TRACE_GROUP_1, BTLOG_SDPUPDATEEIRDATABASEx02X, parse_ptr[1]);

    if(BTQueryPowerOnState() == 0)
    {
        Report(("In power off state, not change EIR"));
        return;
    }

    switch (SDP_GetElemType(parse_ptr[0]))
    {
        case DETD_SEQ:  /* Universally Unique identifier. Valid size 1,2,4 */
            if (SDP_GetElemSize(parse_ptr[0]) != DESD_ADD_8BITS)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_EIR_IGNORE_DATA_ELEMENT_SEQUENCE_NOT_2_BYTES);
                return;
            }
            break;
        default:
            return;
    }
    service_class_size = attribute_value[1];
    bt_trace(TRACE_GROUP_1, BTLOG_SERVICE_CLASS_SIZExD, service_class_size);
    parse_count = 0;
    parse_ptr = attribute_value + 2;
    avoid_infinite_loop = 0;
    /* We shall try to aviod the infinite loop in the while loop */
    /* JSR82 service record may happen infinite loop. */

    while ((parse_count < service_class_size) && (avoid_infinite_loop < 5))
    {
        bt_trace(TRACE_GROUP_1, BTLOG_PARSE_COUNTxD_SERVICE_CLASS_SIZExD, parse_count, service_class_size);
        avoid_infinite_loop++;
        if (parse_ptr[parse_count] != 0x19)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_PARSE_PTRPARSE_COUNTxX, parse_ptr[parse_count]);
            return;
        }
        else
        {
            service_class = ((U16) parse_ptr[parse_count + 1]) << 8 | (parse_ptr[parse_count + 2]);
            if(service_class != 0x1200) /* Ignore the DID PNPInformation */
            {
                bt_trace(TRACE_GROUP_1, BTLOG_INPUT_SERVICE_CLASSx04X, service_class);
                if (action == BT_SDP_EIR_ADD_RECORD)
                {
                    ME_AddEIRServiceClass(service_class);
                }
                else
                {
                    ME_RemoveEIRServiceClass(service_class);
                }
             }
             parse_count += 3;

        }
    }
}
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

static const U8 DIDServiceClass[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UUID_16BIT(0x1200),
};

static const U8 DIDLanguageBase[] =
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* Uint16 "en" (English) */
    SDP_UINT_16BIT(0x006A),     /* Uint16 UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Uint16 primary language base ID */
};

static const U8 DIDProviderName[] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string */
    'M', 'e','d', 'i', 'a', 't', 'e', 'k', ' ', 'I', 
    'n', 'c', '\0'
};

static const U8 DIDSpecID[] = 
{
    SDP_UINT_16BIT(0x0103)
};

static const U8 DIDVendorID[] = 
{
    SDP_UINT_16BIT(70)
};

static const U8 DIDProductID[] = 
{
    SDP_UINT_16BIT(0x0802)
    /* Production ID Naming 0xAABB*/
    /* Example 08A = 0801, 08B = 0802, 08C = 0803 */
    /* Example 09A = 0901, 09B = 0902, 09C = 0903 */  
};

static const U8 DIDVersionID[] = 
{
    SDP_UINT_16BIT(0x0903)
    /* Version ID Naming 0xAABB*/
    /* Example 0809 = 0809, 0903 = 0903*/
};

static const U8 DIDPrimaryRecord[] = 
{
    SDP_BOOL(TRUE)
};

static const U8 DIDVendorIDSource[] = 
{
    SDP_UINT_16BIT(0x0001)
};


SdpAttribute didSdpAttribs[] = 
{
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, DIDServiceClass),                  /* PnpInformation*/
	SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, DIDLanguageBase),
    SDP_ATTRIBUTE((AID_PROVIDER_NAME + 0x0100), DIDProviderName),               /* roviderName Description */
    SDP_ATTRIBUTE(0x0200, DIDSpecID),                                           /* Spec ID */
    SDP_ATTRIBUTE(0x0201, DIDVendorID),                                         /* Vendor ID */
    SDP_ATTRIBUTE(0x0202, DIDProductID),                                        /* Product ID */
    SDP_ATTRIBUTE(0x0203, DIDVersionID),                                        /* Version ID */
    SDP_ATTRIBUTE(0x0204, DIDPrimaryRecord),                                    /* PrimaryRecord*/
    SDP_ATTRIBUTE(0x0205, DIDVendorIDSource)                                    /* Vendor ID Source*/
};


void SdpServDIDInit(void)
{
    SDPS(DIDSdpRecord).attribs = (SdpAttribute *)didSdpAttribs;
    SDPS(DIDSdpRecord).num = sizeof(didSdpAttribs) / sizeof(*(didSdpAttribs));
    SDP_AddRecord(&SDPS(DIDSdpRecord));
}


#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

