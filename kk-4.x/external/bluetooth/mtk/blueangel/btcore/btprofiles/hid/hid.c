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
 *     $Workfile:hid.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description:
 *     API Calls for the HID profile.
 *
 * Created:
 *     Aug 7, 2002
 *
 * Copyright 2002-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
 
#include <sys/hidalloc.h>
//#include <oslib.h>
//#include "kal_release.h"
//#include "bluetooth_trc.h"
#include "btconfig.h"
#include "sec.h"
#include "bt_feature.h"
#ifdef __BT_4_0_BLE__
#include "hid_blescan.h"
#endif

extern U8 HidDeviceSdpDB_Registration(void);
extern U8 HidDeviceSdpDB_DeRegistration(void);
#ifdef __BT_4_0_BLE__
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern void ATTDB_RegisterBondStatus(BtBleBondCbHandler *cb_func);
#endif

/*---------------------------------------------------------------------------
 *            HID_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the HID profile.  This function should be the 
 *            first function called.
 *
 * Return:    (See HID.H)
 */
BOOL HID_Init(void)
{
    I8 i;

    /* Initialize Memory */
    HidAlloc();

    /* Initialize state machine */
    HID(hidState)[0] = HidStateClosed;
    HID(hidState)[1] = HidStateOpen;
    HID(hidState)[2] = HidStateConnPending;
    HID(hidState)[3] = HidStateDiscPending;

    /* Initialize Lists */
    InitializeListHead(&HID(hidList));

    /* Initialize the packet pool */
    InitializeListHead(&HID(packetPool));
    for (i = 0; i < HID_NUM_TX_PACKETS; i++) {
        InsertTailList(&HID(packetPool), &HID(packets)[i].node);
    }
    kal_trace(BT_TRACE_G2_PROFILES,HID_INITIALIZED);

#ifdef __NORDIC_VOICE_COMMAND__
    bPowerOff = 0;
#endif    
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            HID_Register()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers the HID channel.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_Register(HidCallback Callback, HidRole Role)
{
    BtStatus  status = BT_STATUS_SUCCESS;

    /* Validate parameters */
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Callback != 0);

    OS_LockStack();

    //CheckLockedParm(BT_STATUS_IN_USE, 
                    //!IsNodeOnList(&HID(hidList), &(Channel->node)));

    /* Clear the Channel structure */
    //OS_MemSet((U8 *)Channel, 0, sizeof(HidChannel));

	bt_prompt_trace(MOD_BT,"[HID]HID(serviceCount) = %d", HID(serviceCount));

    if (HID(serviceCount) == 0) {
        HID(serviceCount)++;

        /* This is a first time registration for this service,
         * register the L2CAP PSM's.
         */
        HID(hidCtrlPsm).psm = BT_PSM_HID_CTRL;
        HID(hidCtrlPsm).localMtu = L2CAP_MTU;
        HID(hidCtrlPsm).minRemoteMtu = 48;
        HID(hidCtrlPsm).callback = HidL2Callback;
#ifdef HID_AUTHORIZATION
        HID(hidCtrlPsm).authorizedSetting = TRUE;        
#endif
#if L2CAP_FLOW_CONTROL == XA_ENABLED
        HID(hidCtrlPsm).inLinkMode = HID(hidCtrlPsm).outLinkMode = L2MODE_BASIC;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */
#ifdef __BT_2_1_ERTM__
        HID(hidCtrlPsm).lockStepNeeded = 0;
        HID(hidCtrlPsm).highSpeedRequired = 0;
#endif
        /* Register with L2CAP */
        if ((status = L2CAP_RegisterPsm(&(HID(hidCtrlPsm)))) == BT_STATUS_SUCCESS) {
            /* Initialize the PSM structure */
            HID(hidIntrPsm).psm = BT_PSM_HID_INTR;
            HID(hidIntrPsm).localMtu = L2CAP_MTU;
            HID(hidIntrPsm).minRemoteMtu = 48;
            HID(hidIntrPsm).callback = HidL2Callback;
#ifdef HID_AUTHORIZATION
            HID(hidIntrPsm).authorizedSetting = TRUE;        
#endif
#if L2CAP_FLOW_CONTROL == XA_ENABLED
           HID(hidIntrPsm).inLinkMode = HID(hidIntrPsm).outLinkMode = L2MODE_BASIC;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */

            if ((status = L2CAP_RegisterPsm(&(HID(hidIntrPsm)))) == BT_STATUS_SUCCESS) {
                if (Role == HID_ROLE_DEVICE)
                {
                    /* Register device's SDP entry */
                    status = HidDeviceSdpDB_Registration();
                    if (status != BT_STATUS_SUCCESS) 
                    {
                        //Assert(0);
		      bt_prompt_trace(MOD_BT,"[HID]status != BT_STATUS_SUCCESS");
		      return BT_STATUS_FAILED;
                    }
                }
/*
                    OS_MemSet((U8*)Channel, 0, sizeof(HidChannel));
                    Channel->callback = Callback;
                    Channel->hidRole = Role;
                    InitializeListHead(&Channel->transQueue);
                    InitializeListHead(&Channel->intrQueue);
                    InsertTailList(&HID(hidList), &(Channel->node));
*/
                    HID(hidRole) = Role;
                    HID(appCallback) = Callback;
                    //CMGR_RegisterHandler(&HID(cmgrHandler),HidCmgrCallback);                 
            }
            else{
                //Assert(0);
	       bt_prompt_trace(MOD_BT,"[HID]status != BT_STATUS_SUCCESS");
	       return BT_STATUS_FAILED;
            }
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)   
            kal_trace(BT_TRACE_G2_PROFILES,TRY_TO_REGISTER_SECURITY_RECORD);
            InitializeListEntry(&(HID(intrSecRecord).node));
            HID(intrSecRecord).id =  SEC_L2CAP_ID;
            HID(intrSecRecord).channel = BT_PSM_HID_INTR;
            HID(intrSecRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
            HID(intrSecRecord).level_type = BT_SEC_LOW;
#else
            HID(intrSecRecord).level_type = BT_SEC_MED;
#endif
            status = SEC_RegisterRecord(&HID(intrSecRecord));            
            kal_trace(BT_TRACE_G2_PROFILES,TRY_TO_REGISTER_SECURITY_RECORDx02X ,status);
            InitializeListEntry(&(HID(ctrlSecRecord).node));
            HID(ctrlSecRecord).id =  SEC_L2CAP_ID;
            HID(ctrlSecRecord).channel = BT_PSM_HID_CTRL;
            HID(ctrlSecRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
            HID(ctrlSecRecord).level_type = BT_SEC_LOW;
#else
            HID(ctrlSecRecord).level_type = BT_SEC_MED;
#endif
            status = SEC_RegisterRecord(&HID(ctrlSecRecord));            
            kal_trace(BT_TRACE_G2_PROFILES,TRY_TO_REGISTER_SECURITY_RECORDx02X ,status);

#endif    
        }

#ifdef __BT_4_0_BLE__
            /* register the GATT callback to handle connection */
            BT_GattRegisterConnectedEvent(&(HID(gattHandler)), HidGattEventCallback);
            /* register bond callback to obtain bond status */
            HID(bondHandler).callback = HidBondingCallback;
            ATTDB_RegisterBondStatus(&HID(bondHandler));
            HidScanAdvInit();
#endif
    }
    else
    {
        status = BT_STATUS_BUSY;    
    }

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            HID_Deregister()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisteres the HID channel.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_Deregister(HidRole Role)
{
    BtStatus status = BT_STATUS_SUCCESS;

    /* Validate parameters */
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    OS_LockStack();

    //CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    //IsNodeOnList(&HID(hidList), &(Channel->node)));

    if(HID(serviceCount) ==0)
    {
        //Assert(0);
	bt_prompt_trace(MOD_BT,"[HID]HID(serviceCount) ==0");
		return BT_STATUS_FAILED;
    }

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    L2CAP_DeregisterPsm(&(HID(hidCtrlPsm)));
    L2CAP_DeregisterPsm(&(HID(hidIntrPsm)));
#endif

#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)                
    SEC_Unregister(&HID(intrSecRecord));            
    SEC_Unregister(&HID(ctrlSecRecord));            
#endif    

    if (Role == HID_ROLE_DEVICE)
    {
        HidDeviceSdpDB_DeRegistration();
    }

#ifdef __BT_4_0_BLE__
    BT_GattUnregisterConnectedEvent(&HID(gattHandler));
    ATTDB_UnRegisterBondStatus(&HID(bondHandler));        
#endif


    HID(serviceCount)--;
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            HID_RespondOpenInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Responds to an open indication.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_RespondOpenInd(HidChannel *Channel, BOOL flag)
{
    BtStatus        status = BT_STATUS_FAILED;
    L2capChannelId  cid;

    /* Validate parameters */
    //Assert(Channel !=0);
	if(Channel ==0){
		bt_prompt_trace(MOD_BT,"[HID]Channel ==0");
		return BT_STATUS_FAILED;
		}
    //Assert(Channel->callback!=0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&HID(hidList), &(Channel->node)));
    kal_trace(BT_TRACE_G2_PROFILES,HID_RESPONDOPENIND_STATEx02X ,Channel->state);

    if (Channel->state == HID_STATE_CONN_PENDING) {
        /* See which channel was requested first */
        if (Channel->ctrlCid != 0) {
            cid = Channel->ctrlCid;
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNELCTRLCIDx02X ,Channel->ctrlCid);
        } else {
            //Assert(Channel->intrCid != 0);
		if(Channel->intrCid == 0){			
			bt_prompt_trace(MOD_BT,"[HID]Channel->intrCid == 0");
			return BT_STATUS_FAILED;
			}
		
            cid = Channel->intrCid;
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNELINTRCIDx02X ,Channel->ctrlCid);
        } 
#ifdef HID_AUTHORIZATION
        if (flag) {
            /* Accept the connection */
            status = L2Cap_AuthorizeConnectRsp(cid, TRUE);
        } else {
            /* Reject the connection */
            status = L2Cap_AuthorizeConnectRsp(cid, FALSE);
        }
        kal_trace(BT_TRACE_G2_PROFILES,HIDL2CAP_AUTHORIZECONNECTRSP_STATUSx02X ,status);
#else
        if (flag) {
            /* Accept the connection */
            status = L2CAP_ConnectRsp(cid, L2CONN_ACCEPTED, 0);
        } else {
            /* Reject the connection */
            status = L2CAP_ConnectRsp(cid, L2CONN_REJECT_SECURITY_BLOCK, 0);
        }
#endif        
    } 

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            HID_OpenChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiates the opening of a HID channel.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_OpenConnection(HidChannel **Channel, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_BUSY;
    HidChannel *ch = NULL;

    /* Validate parameters */
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel->callback != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Addr != 0);

    OS_LockStack();
    kal_trace(BT_TRACE_G2_PROFILES,HID_OPENCONNECTION);
    //CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    //IsNodeOnList(&HID(hidList), &(Channel->node)));

    if ((*Channel != NULL) && IsNodeOnList(&HID(hidList), &((*Channel)->node)))
    {
        ch = *Channel;
    }
    else
    {
        ch = HidFindChannelByAddr(Addr);
        if (ch == NULL)
        {
            ch = HidNewChannel();
        }
        if (ch == NULL)
        {
            return BT_STATUS_NO_RESOURCES;
        }
    }

    /* Connect to the control channel */
    kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNELSTATEx02X , ch->state);
    if (ch->state == HID_STATE_CLOSED) {
        /* Establish the ACL link */
        //Channel->cmgrHandler.callback = HidCmgrCallback;
        status = CMGR_CreateDataLink(&ch->cmgrHandler, Addr);
        kal_trace(BT_TRACE_G2_PROFILES,HID_STATUSx02X ,status);

        ch->state = HID_STATE_CONN_PENDING;
        ch->flags |= CHNL_FLAG_INITIATOR;
	
        if (status == BT_STATUS_SUCCESS) {
#ifdef __BT_4_0_BLE__
            kal_trace(BT_TRACE_BLE_PROFILES,HID_OPENCONNECTION_DEVTYPE, ch->cmgrHandler.bdc->devType);
            if (ch->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
            {
                status = GattClientConnect(ch->cmgrHandler.remDev);
                if (status == BT_STATUS_SUCCESS)
                {
                    ch->state = HID_STATE_OPEN;
                }
            }
            else
#endif        
            {
                /* Start the L2CAP connection */
                status = L2CAP_ConnectReq(&HID(hidCtrlPsm), BT_PSM_HID_CTRL, 
                                      ch->cmgrHandler.remDev, 0, 
                                      &(ch->ctrlCid));

            }
            kal_trace(BT_TRACE_G2_PROFILES,HID_CTRL_L2CAP_CONNECTREQSTATUSx02X ,status);
        }
        else if (status == BT_STATUS_PENDING) {
			
        }
        else
        {
            HidFreeChannel(ch);
            OS_UnlockStack();
            return status;
        }
    }
    else if (ch->state == HID_STATE_OPEN) {
        status = BT_STATUS_SUCCESS;
    }
	
    *Channel = ch;
	
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            HID_CloseChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiates the closing of a HID channel.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_CloseConnection(HidChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&HID(hidList), &(Channel->node)));

#ifdef __BT_4_0_BLE__
    if (Channel->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
    {
        if (Channel->state == HID_STATE_OPEN)
        {
            if (Channel->flags & CHNL_FLAG_SERVICE_QUERY)
            {
                BLE_StopQueryServiceDefinition(Channel->pQueryHandle);
            }
            status = GattDisconnect(Channel->cmgrHandler.remDev);
            Channel->state = HID_STATE_DISC_PENDING;
        }
        
        if (status == BT_STATUS_SUCCESS)
        {
            HidClearConnection(Channel, status, 0);
        }
    }
    else
#endif        
    {
        if (Channel->state != HID_STATE_CLOSED) {
            Channel->state = HID_STATE_DISC_PENDING;
            Channel->flags |= CHNL_FLAG_TERMINATOR;
            /* Disconnect interrupt channel first */
            status = L2CAP_DisconnectReq(Channel->intrCid);
        }
    }

    OS_UnlockStack();

    return status;
}

#if HID_HOST == XA_ENABLED
/*---------------------------------------------------------------------------
 *            HID_HostQueryDevice()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs an SDP query on the device.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_HostQueryDevice(HidChannel **Channel, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_FAILED;
    HidChannel *ch = NULL;

    /* Validate parameters */
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel->callback != 0);

    OS_LockStack();

    //CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    //IsNodeOnList(&HID(hidList), &(Channel->node)));

    Report(("[HID] host query device..."));

    if ((*Channel != NULL) && IsNodeOnList(&HID(hidList), &((*Channel)->node)))
    {
        ch = *Channel;
    }
    else
    {
        ch = HidFindChannelByAddr(Addr);
        if (ch == NULL)
        {
            ch = HidNewChannel();
        }
        if (ch == NULL)
        {
            return BT_STATUS_NO_RESOURCES;
        }
    }

    if (ch->flags & CHNL_FLAG_SERVICE_QUERY)
    {
        /* Added for BLE HID reconnection from whitelist */
        Report(("[HID] host query device ongoing..."));
        return BT_STATUS_PENDING;
    }

    if (ch->queryRsp.descriptorList != NULL)
    {
        Report(("[HID] service record already obtained!"));
        return BT_STATUS_SUCCESS;
    }


    if ((ch->state == HID_STATE_CLOSED) ||
        (ch->state == HID_STATE_OPEN)) 
    {
        /* Establish the ACL link */
        status = CMGR_CreateDataLink(&ch->cmgrHandler, Addr);
        if (status == BT_STATUS_SUCCESS) 
        {
            ch->flags |= CHNL_FLAG_SERVICE_QUERY;
#ifdef __BT_4_0_BLE__
            kal_trace(BT_TRACE_BLE_PROFILES, HID_QUERYDEVICE_DEVTYPE, ch->cmgrHandler.bdc->devType);
            if (ch->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
            {
                HidScanAdvUpdate(Addr->addr, FALSE);
                *Channel = ch;  // Need to set channel here, since the service query callback may called before the fuction is returned.
                status = GattClientConnect(ch->cmgrHandler.remDev);
                if (status == BT_STATUS_SUCCESS)
                {
                    ch->state = HID_STATE_OPEN;
                    status = HidStartGattServiceQuery(ch);
                }
            }
            else
#endif        
            {
                /* Do an SDP discovery */
                status = HidStartServiceQuery(ch);
            }
        }
        else if (status == BT_STATUS_PENDING)
        {
            ch->flags |= CHNL_FLAG_SERVICE_QUERY;

#ifdef __BT_4_0_BLE__
            if (ch->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
            {
                HidScanAdvUpdate(Addr->addr, FALSE);
            }
#endif

        }
        else
        {
            HidFreeChannel(ch);
            OS_UnlockStack();
            return status;
        }
    }
	
    *Channel = ch;
            
    OS_UnlockStack();

    return status;
}
#endif

/*---------------------------------------------------------------------------
 *            HID_SendTransaction()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a HID transaction (request or response).
 *
 * Return:    (See HID.H)
 */
BtStatus HID_SendTransaction(HidChannel *Channel, 
                             HidTransactionType TransType, 
                             HidTransaction *Trans)
{
    BtStatus        status = BT_STATUS_PENDING;
    HidTransaction *trans;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel->callback != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Trans != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&HID(hidList), &(Channel->node)));

    if (Channel->state == HID_STATE_OPEN) {
        /* Validate that a request is pending */
        switch (TransType) {
        case HID_TRANS_GET_REPORT_RSP:
        case HID_TRANS_SET_REPORT_RSP:
        case HID_TRANS_GET_PROTOCOL_RSP:
        case HID_TRANS_SET_PROTOCOL_RSP:
        case HID_TRANS_GET_IDLE_RATE_RSP:
        case HID_TRANS_SET_IDLE_RATE_RSP:
            if (!(Channel->flags & CHNL_FLAG_TRANS_INCOMING)) {
                status = BT_STATUS_BUSY;
                goto exit;
            }
            if (HID(hidRole) == HID_ROLE_HOST) {
                status = BT_STATUS_NOT_SUPPORTED;
                goto exit;
            }
            break;
        case HID_TRANS_CONTROL:
            break;
        //Add by stanley:2007-0607 
        case HID_HANDSHAKE:
            break;
        default:
            if (HID(hidRole) == HID_ROLE_DEVICE) {
                status = BT_STATUS_NOT_SUPPORTED;
                goto exit;
            }
            break;
        }
        
        /* Queue this request */
        Trans->type = TransType;
        Trans->flags = TRANS_FLAG_START;
        kal_trace(BT_TRACE_G2_PROFILES,INSERT_TO_TRANSQUEUE_WITH_TYPExD ,Trans->type);
        InsertTailList(&Channel->transQueue, &Trans->node);
    
        /* Send a transaction */
        status = HidSendNextTransaction(Channel);
        kal_trace(BT_TRACE_G2_PROFILES,HID_HIDSENDNEXTTRANSACTION_x02X ,status);                
        if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS) {
            trans = (HidTransaction *)GetHeadList(&Channel->transQueue);
            if (trans != Trans) {
                /* A previous transaction failed */
                kal_trace(BT_TRACE_G2_PROFILES,HID_HIDCOMPLETETRANSACTION);                
                HidCompleteTransaction(Channel, trans, status);
                if (!IsListEmpty(&Channel->transQueue)) {
                    status = BT_STATUS_PENDING;
                }
            } 
            RemoveEntryList(&Trans->node);
        }
    } else {
        status = BT_STATUS_NO_CONNECTION;
    }

exit:
     
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            HID_SendInterrupt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends a HID interrupt.
 *
 * Return:    (See HID.H)
 */
BtStatus HID_SendInterrupt(HidChannel *Channel, HidInterrupt *Intr)
{
    BtStatus        status = BT_STATUS_PENDING;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    //CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel->callback != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Intr != 0);
    kal_trace(BT_TRACE_G2_PROFILES,HID_HID_SENDINTERRUPT);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&HID(hidList), &(Channel->node)));

    if (Channel->state == HID_STATE_OPEN) {
        /* Queue this request */
        Intr->flags = INTR_FLAG_START;
        InsertTailList(&Channel->intrQueue, &Intr->node);

        /* Send an interrupt */
        status = HidSendNextInterrupt(Channel);
        kal_trace(BT_TRACE_G2_PROFILES,HID_HIDSENDNEXTINTERRUPT_x02X ,status);        
        if (status != BT_STATUS_PENDING) 
        {
            //Assert(status != BT_STATUS_SUCCESS);
		if(status == BT_STATUS_SUCCESS){
			bt_prompt_trace(MOD_BT,"[HID]status == BT_STATUS_SUCCESS");
			return BT_STATUS_FAILED;
			}
            RemoveEntryList(&Intr->node);
        }
    } else {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*---------------------------------------------------------------------------
 *            HID_IsConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns TRUE if the channel is connected, FALSE if it is not 
 *            connected.
 *
 * Return:    (See HID.H)
 */
BOOL HID_IsConnected(HidChannel *Channel) 
{
    BOOL status = FALSE;

    /* Validate parameters */
    CheckUnlockedParm(FALSE, Channel != 0);
    //CheckUnlockedParm(FALSE, Channel->callback != 0);

    OS_LockStack();

    CheckLockedParm(FALSE, 
                    IsNodeOnList(&HID(hidList), &(Channel->node)));

    if (Channel->state == HID_STATE_OPEN) {
        status = TRUE;
    }

    OS_UnlockStack();
    kal_trace(BT_TRACE_G2_PROFILES,HID_HID_ISCONNECTEDx02X ,status);                

    return status;
}

#endif
