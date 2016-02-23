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
 *     $Workfile:l2cap_sm.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the functions which make up the L2CAP
 *     state machine.
 *
 * Created:
 *     July 15, 1999
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
#include "bt_feature.h"
#include "sys/l2cap_i.h"
#include "sys/debug.h"
#ifdef __BT_4_0_BLE__
#include "btconfig_le.h"
#endif
#include "btbm.h"

#include "btbm.h"
extern BOOL l2cap_query_testing_state(void);

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/
static void L2CapState_CONFIG(L2CAP_Channel *Channel, I8 Event);
static void L2Cap_HandleConfigReq(L2CAP_Channel *Channel);
static void L2Cap_HandleConfigRsp(L2CAP_Channel *Channel);

void SendAutoConfigReq(L2CAP_Channel *Channel);
void SendAutoConfigRsp(L2CAP_Channel *Channel);
void SendAutoConfigRspPending(L2CAP_Channel *Channel);
U16 L2Cap_ProcessBLEConnectParmsUpdate(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Source);

#ifdef __BT_2_1_ERTM__
BtStatus L2Cap_SendFixChannelReq(L2CAP_Channel *Channel);
#endif

extern L2CAP_Channel *L2Cap_GetValidChannelFixChannelCid03(BtRemoteDevice *link);
extern U8 L2Cap_VerifyLockStepConfig(L2CAP_Channel *channel);
extern void L2Cap_InitLockStepConfig(L2CAP_Channel *Channel);
extern BOOL L2CapAMPPriorizationAlgorithm(BtRemoteDevice *link);
extern L2CAP_Channel *L2Cap_GetAMPChannelId(BtRemoteDevice *link, U16 Cid);
void L2CAP_DumpChannel();
void L2CAP_AMPListenController(L2CAP_Channel *Channel, U8 newControllerId);
void l2CAP_AMPStartAMPTrafficForCreateChannel(L2CAP_Channel *Channel);
void L2Cap_StartRegularMoveAMPChannelTimer(L2CAP_Channel *Channel, U32 timeout);
BtStatus L2CAP_SendLockStepConfigRsp(L2capChannelId ChannelId, U16 result);
BtStatus L2Cap_SendCreateChannelReqInternal(L2CAP_Channel *Channel, U8 RemoteControllerId);
BtStatus L2Cap_SendCreateChannelRspInternal(L2CAP_Channel *Channel, U8 rsp_status);
U16 L2Cap_GetSequenceWrapValue(L2CAP_Channel *Channel);
void L2Cap_HandleEnhancedRetransmissionTimout(L2CAP_Channel *Channel);
void L2Cap_HandleEnhancedMonitorTimout(L2CAP_Channel *Channel);
void L2CapAMPState_W4_Create(L2CAP_Channel *Channel, I8 Event);
void L2CapAMPState_W4_CreateRsp(L2CAP_Channel *Channel, I8 Event);
void L2CapAMPState_W4_Move(L2CAP_Channel *Channel, I8 Event);
void L2CapAMPState_W4_MoveRsp(L2CAP_Channel *Channel, I8 Event);
void L2CapAMPState_W4_MoveCnf(L2CAP_Channel *Channel, I8 Event);
void L2CapAMPState_W4_MoveCnfRsp(L2CAP_Channel *Channel, I8 Event);


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessReq
 * DESCRIPTION
 *  Process or Dispatches the received L2CAP Request packet.
 *  RxData points to the signalling command.
 *  RxDataLen is the length of RxData, minus the code, identifier
 *  and length fields.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 *  Source          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessReq(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Source)
{
    L2CAP_Channel *channel, *new_channel;
    U16 reason;
    U8 Opcode = RxData[0];
    U8 Ident = RxData[1];
    U8 doRemoveExtOption = 0; // Remove the l2cap-info-rsp that we support ext-flow-option
#if 0    
    BTBMRemoteNameSetting_T *entry = 0;
#endif

    /* These variables are used for sending error responses if necessary.
       The shortcut macro WriteSignalHeader() has knowledge of these fields. */
    U8 ws[L2SIG_DATA_LEN + BT_PACKET_HEADER_LEN - 4];   /* Response Command Workspace */
    U16 wsLen;                                          /* Workspace length, set by macro */
    BtPacket *btp;                                      /* Response packet pointer */
    U32 extended_feature;
    
    bt_trace(TRACE_FUNC, FUNC_L2CAP_PROCESS_REQ_OP, Opcode);   
    
    /*
     * Check if this request is a duplicate. Note: if this request has an
     * invalid identifier (=0), don't run the check because we'll likely get
     * a false positive result.
     */
    if (Ident && (0 != (channel = L2Cap_CheckForDupilicateRequest(Ident, Source))))
    {
        bt_trace(
            TRACE_GROUP_1,
            L2CAP_DUPLICATE_REQUEST_IGNORED_CID_BT_DEBUGx04X_OPCODE_BT_DEBUGx02X_IDENT_BT_DEBUGx02XxS,
            channel->localCid,
            Opcode,
            Ident);
        return;
    }

    /* Store pointer to response data and its length */
    L2C(currRxData) = RxData;
    L2C(currRxDataLen) = RxDataLen;

    switch (Opcode)
    {

        case LLC_CONN_REQ:
            /* PSM +4 and Source CID +6 */
            new_channel = L2Cap_AllocPsmChannel(LEtoHost16(RxData + 4), &reason);
            if (new_channel)
            {
                new_channel->remoteIdent = Ident;
                new_channel->link = Source;
                new_channel->remoteCid = LEtoHost16(RxData + 6);
                Source->refCount++;
                Report(("l2cap: LLC_CONN_REQ "));
                L2Cap_StateMachine(new_channel, Opcode);
                return;
            }else{
                Report(("l2cap: No new channel"));
            }

            /* No resources or no Psm Send a ConnRspNeg packet */
            WriteSignalHeader(LLC_CONN_RSP, Ident, 8);
            ws[4] = RxData[6];
            ws[5] = RxData[7];
            ws[6] = 0;
            ws[7] = 0;
            StoreLE16(ws + 8, reason);  /* Result */
            StoreLE16(ws + 10, 0x00);  /* Status */	
            break;
        case LLC_CONFIG_REQ:
            /* Dest CID +4, Flags +6 */
            if (0 != (channel = L2Cap_GetValidChannel(LEtoHost16(RxData + 4))))
            {
                channel->remoteIdent = Ident;
                Report(("LLC_CONFIG_REQ"));

                L2Cap_StateMachine(channel, LLC_CONFIG_REQ);
                return;
            }

            /* Invalid Dest CID Send Command Reject Message */
            WriteSignalHeader(LLC_COMMAND_REJ, Ident, 6);
            ws[4] = 2;          /* Reason: Invalid CID */
            ws[6] = RxData[4];  /* local CID (relative to sender) */
            ws[7] = RxData[5];
            break;              /* remote CID unknown */

        case LLC_DISC_REQ:
            /* Dest CID +4, Source CID +6 */
            if (0 != (channel = L2Cap_GetValidChannel(LEtoHost16(RxData + 4))))
            {
                if (channel->remoteCid == LEtoHost16(RxData + 6))
                {
                    channel->remoteIdent = Ident;

                    L2Cap_StateMachine(channel, LLC_DISC_REQ);
                    return;
                }
                /* Invalid Source CID, ignore the request. */
                return;
            }

            /* Remote doesn't send the local cid. Use remote-cid + handle to find l2cap_channel ALPS00438762 */
            if( 0 == LEtoHost16(RxData + 4) ) {
               if( 0 != Source ){
                   channel = L2Cap_FindChannelByRemoteCid( LEtoHost16(RxData + 6), Source->hciHandle);
                   if( 0 != channel ){
                       OS_Report("l2cap find by remote cid:0x%x handle:0x%x", LEtoHost16(RxData + 6), (Source->hciHandle& 0x0FFF));
                       channel->remoteIdent = Ident; 
                       L2Cap_StateMachine(channel, LLC_DISC_REQ);
                       return;
                   }else{
                       L2CAP_DumpChannel();
                   }
               }
            } 

            /* Invalid Dest CID Send Command Reject Message */
            WriteSignalHeader(LLC_COMMAND_REJ, Ident, 6);
            ws[4] = 2;          /* Reason: Invalid CID */
            ws[6] = RxData[4];  /* local CID (relative to sender) */
            ws[7] = RxData[5];
            ws[8] = RxData[6];  /* remote CID (relative to sender) */
            ws[9] = RxData[7];
            break;
#ifdef __BT_2_1_ERTM__
		case LLC_CREATE_CHANNEL_REQ:
            /* PSM +4 and Source CID +6 */
            new_channel = L2Cap_AllocPsmChannel(LEtoHost16(RxData + 4), &reason);
            if (new_channel)
            {
                new_channel->remoteIdent = Ident;
                new_channel->link = Source;
                new_channel->remoteCid = LEtoHost16(RxData + 6);
				new_channel->currentAmpControllerId = RxData[8];
                Source->refCount++;
                Report(("LLC_CREATE_CHANNEL_REQ"));
                L2Cap_StateMachine(new_channel, Opcode);
                return;
            }

	    /* No resources or no Psm Send a ConnRspNeg packet */
            WriteSignalHeader(LLC_CREATE_CHANNEL_RSP, Ident, 8);

            ws[4] = RxData[6];
            ws[5] = RxData[7];
            ws[6] = 0;
            ws[7] = 0;
            StoreLE16(ws + 8, reason);  /* Result */
            StoreLE16(ws + 10, 0x00);  /* Status */			
	    break;
	case LLC_MOV_CHANNEL_REQ:
	    /* Src CID +4, Flags +6, Result +8 */
            channel = L2Cap_GetAMPChannelId(Source, LEtoHost16(RxData + 4));
            if(channel !=0)
            {
                channel->remoteMoveIdent= Ident;
    			L2Cap_StateMachine(channel, LLC_MOV_CHANNEL_REQ);
            }
            return;
		case LLC_MOV_CHANNEL_CNF_REQ:
            channel = L2Cap_GetAMPChannelId(Source, LEtoHost16(RxData + 4));
            if(channel !=0)
            {
                channel->result = LEtoHost16(RxData + 6);
                channel->remoteMoveIdent= Ident;
                L2Cap_StateMachine(channel, LLC_MOV_CHANNEL_CNF_REQ);
            }
            return;
#endif

        case LLC_ECHO_REQ:
            /*
             * Send an echo response. We echo back up to L2SIG_DATA_LEN-4
             * * bytes of echo data. 
             */
            RxDataLen = min(L2SIG_DATA_LEN - 4, RxDataLen);

            WriteSignalHeader(LLC_ECHO_RSP, Ident, RxDataLen);
            OS_MemCopy(ws + 4, RxData + 4, RxDataLen);
            break;

        case LLC_INFO_REQ:
            /* Send an Info response */
            OS_Report( "l2cap Info_req:%d", LEtoHost16(RxData + 4));
            if (LEtoHost16(RxData + 4) == L2INFO_CONNECTIONLESS_MTU)
            {
                WriteSignalHeader(LLC_INFO_RSP, Ident, 6);
                ws[4] = RxData[4];          /* Info Type (+4 in the request) */
                ws[5] = RxData[5];
                /* Result (6,7): Success */
                ws[8] = (U8) (L2CAP_MTU);   /* Connectionless MTU */
                ws[9] = (U8) (L2CAP_MTU >> 8);
                break;
            }
#ifdef __BT_3_0_UNICAST__
            Report(("__BT_3_0_UNICAST__ supported"));
            if (LEtoHost16(RxData + 4) == L2INFO_FIXED_CHANNEL_SUPPORTED)
            {
                U32 supportedFixChannel = L2FIX_CHANNEL_SUPPORTING_SIGNALING | L2FIX_CHANNEL_SUPPORTING_CONNECTIONLESS;
                WriteSignalHeader(LLC_INFO_RSP, Ident, 12);
                ws[4] = RxData[4];          /* Info Type (+4 in the request) */
                ws[5] = RxData[5];
                /* Result (6,7): Success */
                /* Write 0~3 octets */
#ifndef __BT_3_0_HS__
//                StoreLE32(ws + 8, (L2FIX_CHANNEL_SUPPORTING_SIGNALING | L2FIX_CHANNEL_SUPPORTING_CONNECTIONLESS));
#else
                if(L2C(hs_enabled)){
                    supportedFixChannel |= L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL;
                    //StoreLE32(ws + 8, (L2FIX_CHANNEL_SUPPORTING_SIGNALING | L2FIX_CHANNEL_SUPPORTING_CONNECTIONLESS | L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL));
                }else{
                    //StoreLE32(ws + 8, (L2FIX_CHANNEL_SUPPORTING_SIGNALING | L2FIX_CHANNEL_SUPPORTING_CONNECTIONLESS));
                }
#endif
                if( btGetCustvalue(CUSTID_LE_SUPPORT) != 0 ){
                    supportedFixChannel |= L2FIX_CHANNEL_SUPPORTING_ATT_CHANNEL | L2FIX_CHANNEL_SUPPORTING_SM_CHANNEL;
                }
                StoreLE32(ws + 8, supportedFixChannel);
                /* Write 4~7 octets */
                StoreLE32(ws + 12, 0x00);
                break;
            }
#endif             
#ifdef __BT_3_0_HS__
            Report(("BT3.0+HS supported"));
            if (LEtoHost16(RxData + 4) == L2INFO_FIXED_CHANNEL_SUPPORTED)
            {
                WriteSignalHeader(LLC_INFO_RSP, Ident, 12);
                ws[4] = RxData[4];          /* Info Type (+4 in the request) */
                ws[5] = RxData[5];
                /* Result (6,7): Success */
                /* Write 0~3 octets */
                if(L2C(hs_enabled)){
                    StoreLE32(ws + 8, (L2FIX_CHANNEL_SUPPORTING_SIGNALING | L2FIX_CHANNEL_SUPPORTING_CONNECTIONLESS | L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL));
                }else{
                    StoreLE32(ws + 8, (L2FIX_CHANNEL_SUPPORTING_SIGNALING | L2FIX_CHANNEL_SUPPORTING_CONNECTIONLESS));
                }
                //                StoreLE32(ws + 8, 0);
                /* Write 4~7 octets */
                StoreLE32(ws + 12, 0x00);
                break;
            }
#endif    


            if (LEtoHost16(RxData + 4) == L2INFO_EXTENDED_FEATURES)
            {
                WriteSignalHeader(LLC_INFO_RSP, Ident, 8);
                ws[4] = RxData[4];  /* Info Type (+4 in the request) */
                ws[5] = RxData[5];
                extended_feature = 0;
                
#if L2CAP_FLOW_CONTROL == XA_ENABLED
                extended_feature |= L2EXTENDED_FEATURE_FLOW_CONTROL | L2EXTENDED_FEATURE_RETRANSMISSIONS;
#endif  
#if L2CAP_STREAM_FLOW_CONTROL == XA_ENABLED
                extended_feature |= L2EXTENDED_FEATURE_STREAMMING;
#endif

#if L2CAP_ENHANCED_FLOW_CONTROL == XA_ENABLED
                extended_feature |= L2EXTENDED_FEATURE_ENHANCED_RETRANSMISSIONS | L2EXTENDED_FEATURE_STREAMMING;
                extended_feature |= L2EXTENDED_FEATURE_FCS_OPTION ;
#endif 
#if ((L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED) || (L2CAP_EXTENDED_FLOW_SPEC == XA_ENABLED))
#ifdef __BT_3_0_HS__
                // IVT will use l2cap-create-channel for sdp if this is set.
                extended_feature |= L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION;
#endif
#endif 
#ifdef __BT_3_0_HS__
                extended_feature |= L2EXTENDED_FEATURE_FIXED_CHANNEL_SUPPORTED;
#endif

#if L2CAP_FLOW_CONFIG_EXTENDED_TX_WINDOW == XA_ENABLED
                extended_feature |= L2EXTENDED_WINDOW_SIZE;
#endif

#ifdef __BT_3_0_UNICAST__
                extended_feature |= L2EXTENDED_FEATURE_UNICAST;
#endif 
                // ALPS01378547 - carkit will use ERTM to connect sdp 0x64:0xd4:0xbd and then disconnect 
                if( Source && Source->bdAddr.addr[5] == 0x64 && Source->bdAddr.addr[4] == 0xd4 && Source->bdAddr.addr[3] == 0xbd){
                    OS_Report("L2cap: warning ! remote ertm. Carkit IOT will use ertm. ");
                    OS_Report("L2cap: extended_feature before 0x%x", extended_feature);
                    extended_feature &= ~(L2EXTENDED_FEATURE_ENHANCED_RETRANSMISSIONS | L2EXTENDED_FEATURE_STREAMMING);
                    OS_Report("L2cap: extended_feature after 0x%x", extended_feature);

                }
                // ALPS01378547 - 38c096 addr
                if( Source )
                {
                    if( Source->bdAddr.addr[5] == 0x38 && Source->bdAddr.addr[4] == 0xc0 && Source->bdAddr.addr[3] == 0x96)
                    {
                        doRemoveExtOption = 1;
                    }

#if 0
                    // need btbm provide this
                    entry = btbmUtilConnectedDeviceNameEntryLookUp(&Source->bdAddr);
                    if(entry && entry->dev_name_len > 0 ){
                        if( TRUE ==  OS_MemCmp("Honda HFT", 9, entry->dev_name, 9)){
                            doRemoveExtOption = 1;
                        }
                        OS_Report("L2cap: remote name is %s", entry->dev_name);
                    }else{
                        OS_Report("L2cap: remote name is empty");
                    }
#endif                    
                }

                if( 1 == doRemoveExtOption )
                {
                    OS_Report("L2cap: extended_feature before 0x%x", extended_feature);
                    extended_feature &= ~(L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION);
                }
                

//                extended_feature = L2EXTENDED_FEATURE_FLOW_CONTROL | L2EXTENDED_FEATURE_RETRANSMISSIONS;
                OS_Report("L2Cap: extended_feature 0x%x", extended_feature);

                StoreLE32(ws + 8, extended_feature);
                break;
            }
            else
            {
                WriteSignalHeader(LLC_INFO_RSP, Ident, 4);
                ws[4] = RxData[4];  /* Info Type (+4 in the request) */
                ws[5] = RxData[5];
                ws[6] = 0x01;       /* Result: Not supported */
            }
            break;

        default:
            /* Unknown request, send a Command Reject packet */
            WriteSignalHeader(LLC_COMMAND_REJ, Ident, 2);
            /* Reason: 0 = Command not understood */
            break;
    }

    /* Come here to send packet ONLY! */
    if (0 != (btp = L2Cap_GetSysPkt(TRUE)))
    {
        /* Assert */
        Assert(wsLen > 3 && wsLen <= (L2SIG_DATA_LEN + BT_PACKET_HEADER_LEN - 4));
        btp->llpContext = 0x0001;
        btp->flags |= BTP_FLAG_INUSE;
        btp->remoteCid = 0x0001;
        /* Copy last 4 bytes of workspace into data area */
        OS_MemCopy(btp->header + BT_PACKET_HEADER_LEN - 4, ws, 4);
        btp->headerLen = 4;
        wsLen -= 4;
        OS_MemCopy(btp->data, ws + 4, wsLen);
        btp->dataLen = wsLen;
        btp->priority_type = BTP_FLAG_HIGH_PRIPORTY;
        
        OS_Report("L2Cap: L2Cap_Send signal packet OpCode:0x%x len(%d)", LLC_INFO_RSP, btp->dataLen);
        L2Cap_QueueTxPacket(0, Source, btp); 
    }
    else
    {
        Assert(0);
    }

    return;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessRsp
 * DESCRIPTION
 *  RxData points to data after the signalling command header has been stripped off.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessRsp(U8 *RxData, U16 RxDataLen)
{
    L2CAP_Channel *channel;
    U8 Opcode = RxData[0];
    U8 Ident = RxData[1];

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    U32 features;
#endif 

    /*
     * Since we sent the request, we can find the channel by
     * looking for the channel with a matching localIdent.
     */
    channel = L2Cap_FindChannelByLocalIdent(Ident);
    if (!channel)
    {
        /* Invalid response */
        bt_trace(TRACE_GROUP_1, L2CAP_PROCESSRSP_RECEIVED_RESPONSE_TO_UNKNOWN_REQUEST_IDENT_BT_DEBUGx02X, Ident);
        return;
    }
    if (channel->state == L2C_CLOSED)
    {
        /*
         * Channel is closed (it probably timed out). Ignore responses
         * while waiting for 'inUse' resources to be returned.
         */
        return;
    }
    L2C(callbackParms).owner.psm = channel->psmInfo;

    /* Store pointer to response data and its length */
    L2C(currRxData) = RxData;
    L2C(currRxDataLen) = RxDataLen;

    /* Stop retransmit timer and clear the acknowledged identifier. */
    L2Cap_StopTimer(channel);

    channel->localIdent = 0;

    OS_Report("L2Cap: ProcessRsp opcode:0x%x() psm:0x%x", Opcode , (channel->psmInfo==0?0:channel->psmInfo->psm));

    switch (Opcode)
    {

        case LLC_CONN_RSP:
            /* Dest CID +4, Src CID +6, Result +8, Status +10 */
            /* Only check SourceCID if result is not conn rejected */
            if (LEtoHost16(RxData + 8) == L2CONN_PENDING)
            {
                channel->localIdent = Ident;
            }
            OS_Report("l2cap conn_rsp result:%d", (LEtoHost16(RxData + 8)));

            if ((LEtoHost16(RxData + 8) > L2CONN_PENDING) || (channel->localCid == LEtoHost16(RxData + 6)))
            {

                L2Cap_StateMachine(channel, LLC_CONN_RSP);
                return;
            }
            break;

        case LLC_CONFIG_RSP:
            /* Src CID +4, Flags +6, Result +8 */
            if (channel->localCid == LEtoHost16(RxData + 4))
            {
                if (LEtoHost16(RxData + 8) == L2CFG_RESULT_PENDING)
                {
                    channel->localIdent = Ident;
                }
                L2Cap_StateMachine(channel, LLC_CONFIG_RSP);
                return;
            }
            break;

        case LLC_DISC_RSP:
            /* Verify that Dst CID (+4) & Src CID's (+6) match */
            if ((channel->localCid == LEtoHost16(RxData + 6)) && (channel->remoteCid == LEtoHost16(RxData + 4)))
            {

                L2Cap_StateMachine(channel, LLC_DISC_RSP);
                return;
            }
            break;

        #if L2CAP_PING_SUPPORT == XA_ENABLED
        case LLC_ECHO_RSP:
            /* Indicate response to Requester */
            if (channel->state == L2C_W4_ECHO_CNF)
            {
                channel->state = L2C_CLOSED;

                L2C(callbackParms).ptrs.data = RxData + 4;
                L2C(callbackParms).dataLen = RxDataLen;
                L2Cap_NotifyPsm(channel, L2EVENT_COMPLETE, BT_STATUS_SUCCESS);  /* L2CAP_PING_CNF */

                L2Cap_FreeChannel(channel);
            }
            return;
        #endif /* L2CAP_PING_SUPPORT == XA_ENABLED */
#ifdef __BT_3_0_HS__
        case LLC_CREATE_CHANNEL_RSP:
            /* Dest CID +4, Src CID +6, Result +8, Status +10 */
            /* Only check SourceCID if result is not conn rejected */
	    /* We did not use the create channel request */
            if (LEtoHost16(RxData + 8) == L2CONN_PENDING)
            {
                channel->localIdent = Ident;
            }

            if ((LEtoHost16(RxData + 8) > L2CONN_PENDING) || (channel->localCid == LEtoHost16(RxData + 6)))
            {

                L2Cap_StateMachine(channel, LLC_CREATE_CHANNEL_RSP);
                return;
            }
            break;

        case LLC_MOV_CHANNEL_RSP:
            /* Src CID +4, Flags +6, Result +8 */
            if (channel->localCid == LEtoHost16(RxData + 4))
            {
                if (LEtoHost16(RxData + 6) == L2AMP_RESULT_PENDING)
                {
                    channel->localIdent = Ident;
                }
                L2Cap_StateMachine(channel, LLC_MOV_CHANNEL_RSP);
                return;
            }
            break;
		case LLC_MOV_CHANNEL_CNF_RSP:
            if (channel->localCid == LEtoHost16(RxData + 4))
            {
                L2Cap_StateMachine(channel, LLC_MOV_CHANNEL_CNF_RSP);
                return;
            }
            break;
#endif
        case LLC_INFO_RSP:
            /* Indicate response to the requester. */
        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            if ((channel->state == L2C_W4_CONN_CNF) || (channel->state == L2C_W4_CONN_RSP) ||
				(channel->state == L2C_W4_CREATE) || (channel->state == L2C_W4_CREATE_RSP))
            {
                if(channel->flags2 & L2CHF2_FEATURES_REQ)
                {
                    channel->flags2 &= ~L2CHF2_FEATURES_REQ;
                    if ((RxData[6] == 0) && (RxData[7] == 0) && (RxDataLen == 8))
                    {
                        /* Store the features in the BtRemoteDevice structure. */
                        features = LEtoHost32(RxData + 8);
                        channel->link->l2cap_feature = features;
                        channel->link->linkModes = L2MODE_BASIC;
                        if (features & L2EXTENDED_FEATURE_ENHANCED_RETRANSMISSIONS)
                        {
                            channel->link->linkModes |= L2MODE_ENHANCEDMENT_RETRANSMISSION;
                        }
                        if (features & L2EXTENDED_FEATURE_STREAMMING)
                        {
                            channel->link->linkModes |= L2MODE_STREAMMING;
                        }
                        if (features & L2EXTENDED_FEATURE_FLOW_CONTROL)
                        {
                            channel->link->linkModes |= L2MODE_FLOW_CONTROL;
                        }
                        if (features & L2EXTENDED_FEATURE_RETRANSMISSIONS)
                        {
                            channel->link->linkModes |= L2MODE_RETRANSMISSION;
                        }
                    }
                    else if (channel->link->linkModes == L2MODE_UNKNOWN)
                    {
                        channel->link->linkModes = L2MODE_BASIC;
                    }
                    OS_Report("l: final linkModes:0x%x", channel->link->linkModes);
#ifdef __BT_2_1_ERTM__
                    L2Cap_SendFixChannelReq(channel);
                    return;
#endif
                    channel->infoReqDone = 1;

                }
#ifdef __BT_2_1_ERTM__                        
                else if (channel->flags2 & L2CHF2_FIX_CHANNEL_REQ)
                {
                    channel->flags2 &= ~L2CHF2_FIX_CHANNEL_REQ;
                    Report(("RxData[6]:%d",RxData[6]));
                    Report(("RxData[7]:%d",RxData[7]));  
                    Report(("RxData[8]:%d",RxData[8]));                    
                    if ((RxData[6] == 0) && (RxData[7] == 0))
                    {
                    	/* save the info response*/
                        OS_MemCopy(channel->link->FixedChannelSupported, &RxData[8], 8);
                        if(L2C(hs_enabled) == 0){
                           RxData[8] &= ~L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL ;
                        }
                        if(RxData[8] & L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL) 
                        {
                            L2CAP_Channel *amp_channel;
                            /* Detected the A2MP exists */
                            L2C(callbackParms).aclLink = channel->link; 
                            /* For UPF test */
                            channel->a2mpSupported = 1;
                            amp_channel = L2Cap_GetValidChannelFixChannelCid03(channel->link);
                            if( 0 != amp_channel ){
                                L2Cap_NotifyPsm(amp_channel, L2EVENT_AMP_CHANNEL_DETECTED, LEtoHost16(RxData + 6)); 
                            }
                        }
#if 0
                        if(channel->psmInfo->highSpeedRequired == 1)
                        {
                            channel->a2mpSupported = 1;
                        }
                        else
                        {
                            if(RxData[8] & L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL) 
                            {
                                L2CAP_Channel *amp_channel;
                                /* Detected the A2MP exists */
                                L2C(callbackParms).aclLink = channel->link; 
                                /* For UPF test */
                                channel->a2mpSupported = 1;
                                amp_channel = L2Cap_GetValidChannelFixChannelCid03(channel->link);
                                L2Cap_NotifyPsm(amp_channel, L2EVENT_AMP_CHANNEL_DETECTED, LEtoHost16(RxData + 6)); 
                            }
                        }
#endif                        
                    }
                    else
                    {
                        channel->a2mpSupported =0;
                    }
                    channel->infoReqDone = 1;
                }
#endif           
                L2Cap_StateMachine(channel, LLC_INFO_RSP);
                return;
            }
        #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

        #if L2CAP_GET_INFO_SUPPORT == XA_ENABLED
            Assert(channel->state == L2C_W4_INFO_CNF);

            channel->state = L2C_CLOSED;

            L2C(callbackParms).ptrs.data = RxData + 8;  /* Data */
            L2C(callbackParms).dataLen = RxDataLen - 4; /* subtract Result & InfoType fields */

            L2Cap_NotifyPsm(channel, L2EVENT_COMPLETE, LEtoHost16(RxData + 6)); /* L2CAP_INFO_CNF */

            L2Cap_FreeChannel(channel);
        #endif /* L2CAP_GET_INFO_SUPPORT == XA_ENABLED */
            return;

        case LLC_COMMAND_REJ:
            /*
             * This is bad.... We sent a request and got this blank stare back.
             * We know what channel sent the request so just feed it a negative
             * response.
             */
            L2Cap_StateMachine(channel, LLC_COMMAND_REJ);
            return;

        default:
            /*
             * Unrecognized response.
             */
            return;
    }

    /*
     * If we get here, the response could not be correlated with a 
     * request we made. Restart Rtx() so we at least timeout the request.
     */
    bt_trace(
        TRACE_GROUP_1,
        L2CAP_PROCESSRSPBT_DEBUGx04X_RESPONSE_CID_DID_NOT_MATCH_EXPECTED_CID_OPCODE_xD,
        channel->localCid,
        Opcode);
    L2Cap_StartRtx(channel);
}


#ifdef __BT_4_0_BLE__
/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessBLEReq
 * DESCRIPTION
 *  Process or Dispatches the received L2CAP Request packet.
 *  RxData points to the signalling command.
 *  RxDataLen is the length of RxData, minus the code, identifier
 *  and length fields.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 *  Source          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessBLEReq(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Source)
{
    L2CAP_Channel *channel;
    U16 reason;
    U8 Opcode = RxData[0];
    U8 Ident = RxData[1];
    U16 result;
    U8 ws[L2SIG_DATA_LEN + BT_PACKET_HEADER_LEN - 4];   /* Response Command Workspace */
    U16 wsLen;                                          /* Workspace length, set by macro */
    BtPacket *btp;                                      /* Response packet pointer */
    BtStatus status = BT_STATUS_FAILED;
    
    /*
     * Check if this request is a duplicate. Note: if this request has an
     * invalid identifier (=0), don't run the check because we'll likely get
     * a false positive result.
     */
    if (Ident && (0 != (channel = L2Cap_CheckForDupilicateRequest(Ident, Source))))
    {
        bt_trace(
            TRACE_GROUP_1,
            L2CAP_DUPLICATE_REQUEST_IGNORED_CID_BT_DEBUGx04X_OPCODE_BT_DEBUGx02X_IDENT_BT_DEBUGx02XxS,
            channel->localCid,
            Opcode,
            Ident);
        return;
    }

    /* Store pointer to response data and its length */
    L2C(currRxData) = RxData;
    L2C(currRxDataLen) = RxDataLen;

    switch (Opcode)
    {

        case LLC_CONN_PARAM_UPDATE_REQ:
            /* Send an Info response */
            if(Source->role == BCR_SLAVE)
            {
                WriteSignalHeader(LLC_COMMAND_REJ, Ident, 2);
                StoreLE16(ws+2, 0x00);
               break;
            }
            result =  L2Cap_ProcessBLEConnectParmsUpdate(RxData, RxDataLen, Source);
            WriteSignalHeader(LLC_CONN_PARAM_UPDATE_RSP, Ident, 2);
            StoreLE16(ws+4, result);
            break;
        default:
            /* Unknown request, send a Command Reject packet */
            WriteSignalHeader(LLC_COMMAND_REJ, Ident, 2);
            /* Reason: 0 = Command not understood */
            break;
    }

    /* Come here to send packet ONLY! */
    if (0 != (btp = L2Cap_GetSysPkt(TRUE)))
    {
        /* Assert */
        Assert(wsLen > 3 && wsLen <= (L2SIG_DATA_LEN + BT_PACKET_HEADER_LEN - 4));
        btp->llpContext = 0x0001;
        btp->flags |= BTP_FLAG_INUSE;
        btp->remoteCid = 0x0005;
        /* Copy last 4 bytes of workspace into data area */
        OS_MemCopy(btp->header + BT_PACKET_HEADER_LEN - 4, ws, 4);
        btp->headerLen = 4;
        wsLen -= 4;
        OS_MemCopy(btp->data, ws + 4, wsLen);
        btp->dataLen = wsLen;
        btp->priority_type = BTP_FLAG_HIGH_PRIPORTY;
        status = L2Cap_QueueTxPacket(0, Source, btp);
    
        if ( status == BT_STATUS_FAILED )
        {
            ReturnSysPkt(btp);
        }

    }
    else
    {
        Assert(0);
    }

    return;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessBLERsp
 * DESCRIPTION
 *  RxData points to data after the signalling command header has been stripped off.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessBLERsp(U8 *RxData, U16 RxDataLen)
{
    U8 Opcode = RxData[0];
    U8 Ident = RxData[1];
    L2CAP_Channel *channel;

    channel = L2Cap_FindChannelByLocalIdent(Ident);
    if (!channel)
    {
        /* Invalid response */
        bt_trace(TRACE_GROUP_1, L2CAP_PROCESSRSP_RECEIVED_RESPONSE_TO_UNKNOWN_REQUEST_IDENT_BT_DEBUGx02X, Ident);
        return;
    }
    L2Cap_StopTimer(channel);
    switch (Opcode)
    {

        case LLC_CONN_PARAM_UPDATE_RSP:
            channel->state = L2C_CLOSED;
            L2Cap_FreeChannel(channel);
            return;
        case LLC_COMMAND_REJ:
            return;

        default:
            /*
             * Unrecognized response.
             */
            return;
    }

}
#endif

/*****************************************************************************
 * FUNCTION
 *  L2CapState_OPEN
 * DESCRIPTION
 *  Processes signalling commands that occur while the channel is
 *  in the OPEN state. The only valid commands are ConfigReq and
 *  DiscReq.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_OPEN(L2CAP_Channel *Channel, I8 Event)
{
#if __BT_30_DEBUG__ == XA_ENABLED
    bt_trace(TRACE_FUNC, FUNC_L2CAP_STATE_OPEN_CID_EVENT, Channel->localCid, Event);   
#endif

    if (Event == LLC_CONFIG_REQ)
    {
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
        Channel->reConfigState = TRUE;
        Channel->configLockStep = FALSE;
        Channel->lockstepTxPendingDone = FALSE;
        Channel->lockstepRxPendingDone = FALSE;
        Channel->lockstepNegoDone = FALSE;
        Report(("Channel->inLinkMode:%d",Channel->inLinkMode));
        Report(("Channel->psmInfo->channelUsage:%d",Channel->psmInfo->channelUsage));        
        if(((Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION) ||
            (Channel->inLinkMode == LINK_MODE_STREAMMING)) &&
            (Channel->psmInfo->channelUsage == L2CAP_CHANNEL_DEDICATED))
        {
            L2Cap_InitLockStepConfig(Channel);
            Channel->configLockStep = TRUE;
        }
        else if(((Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION) ||
            (Channel->inLinkMode == LINK_MODE_STREAMMING)) &&
            (L2Cap_VerifyLockStepConfig(Channel) == 1))
        {
            L2Cap_InitLockStepConfig(Channel);
            Channel->configLockStep = TRUE;
        }
#endif
        Channel->state = L2C_CONFIG;
        Channel->flags &= ~(L2CHF_IN_CONFIG_OK | L2CHF_OUT_CONFIG_OK | L2CHF_CONFIG_REQ_SENT);
#if 0   /* The FC option looks not required */
    #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        Channel->flags2 &= ~L2CHF2_FOUND_FC_OPTION;
    #endif 
#endif
        /* Tell the application that we're reconfiguring the channel. */
        L2Cap_NotifyPsm(Channel, L2EVENT_CONFIGURING, BT_STATUS_SUCCESS);

    #if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
        if (Channel->flags & L2CHF_AUTO_CONFIG)
    #endif 
            SendAutoConfigReq(Channel);

        L2CapState_CONFIG(Channel, LLC_CONFIG_REQ);
        return;
    }

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    if (Event == LLC_SEND_PACKET)
    {
        /*
         * We are waiting for a system packet. Send the Receiver-Ready
         * or Reject packet now.
         */
        Assert((Channel->flags & (L2CHF_SEND_REQUEST | L2CHF_SEND_RESPONSE)) == L2CHF_SEND_REQUEST);

        /* Send an S-Frame: RR or Reject */
        if((Channel->inLinkMode != LINK_MODE_STREAMMING) &&
            (Channel->inLinkMode != LINK_MODE_BASIC))
            L2Cap_SendSFrame(Channel);
        return;
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

#ifdef __BT_3_0_HS__
    /* Event == LLC_DISC_REQ: Is handled by the core State Machine code. */
    /* Event == LLC_COMMAND_REJ: Is handled by the core State Machine code. */
    if(Event == LLC_MOV_CHANNEL_REQ)
    {
        L2C(callbackParms).ptrs.moveChannelReq.controllerId = L2C(currRxData)[6];
        L2C(callbackParms).ptrs.moveChannelReq.oldControllerId = Channel->currentAmpControllerId;
        L2Cap_NotifyPsm(Channel, L2EVENT_MOVE_CHANNEL_REQ, BT_STATUS_SUCCESS);
        return;
    }

    if ((Event <= LLC_MOV_CHANNEL_CNF_RSP) &&
        (Event >= LLC_CREATE_CHANNEL_RSP))
        
    {
        return;
    }
#endif

    Assert(Event == LLC_TX_DONE);   /* RR or REJECT S-Frame */
    if( Event != LLC_TX_DONE ){
      OS_Report("Event:%d", Event);
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2CapState_W4_DISC_CNF
 * DESCRIPTION
 *  We've sent a disconnect request and are wating for the response.
 *  If both sides disconnect simultaneously, W4_DISC_RSP is the
 *  resulting state.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_W4_DISC_CNF(L2CAP_Channel *Channel, I8 Event)
{
    if (Event == LLC_SEND_PACKET)
    {
        /* We are waiting for a system packet, send the disconnect now. */
        Assert((Channel->flags & (L2CHF_SEND_REQUEST | L2CHF_SEND_RESPONSE)) == L2CHF_SEND_REQUEST);
        (void)L2Cap_SendDiscReqInternal(Channel);
        return;
    }

    if (Event == LLC_DISC_RSP)
    {
        Report(("L2C(callbackParms).status:%d",L2C(callbackParms).status));
        if (L2C(callbackParms).status == L2DISC_REASON_UNKNOWN)
        {
            L2Cap_NotifyUserAndCloseChannel(Channel, L2DISC_USER_REQUEST);
        }
        else
        {
            L2Cap_NotifyUserAndCloseChannel(Channel, L2C(callbackParms).status);
        }
        return;
    }

    /* Event == LLC_COMMAND_REJ: Handled by the core State Machine code. */
    /* Event == LLC_DISC_REQ: Handled by the core State Machine code. */

    /*
     * Since we roll to W4_DISC_CNF state on reception of a negative
     * config response, there is the chance that a config request is
     * waiting for us to process. Just ignore it. 
     */
    Assert(Event == LLC_CONFIG_REQ || Event == LLC_TX_DONE);
    if (Event == LLC_TX_DONE)
    {
        bt_trace(TRACE_GROUP_1,BTLOG_L2CAP_DISC_PENDING_PKT_HANDLED);
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2CapState_W4_DISC_RSP
 * DESCRIPTION
 *  We received a Disconnect request and are waiting for our
 *  response to be transmitted so we can close the channel. If
 *  both sides disconnect simultaneously, this is the resulting state.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_W4_DISC_RSP(L2CAP_Channel *Channel, I8 Event)
{
    if (Event == LLC_TX_DONE)
    {
        // remove '(Channel->l2capTxCount == 0) &&' since remote sends the disc-req ALPS00460439
        OS_Report("l2cap: pending disc-rsp notify txCount:%d flags:0x%x ", Channel->l2capTxCount, Channel->flags);
        if ( ((Channel->flags & L2CHF_SEND_RESPONSE) == 0))
        {
            /*
             * We already indicated a DISCONNECT_IND, this event tells the app
             * * that we're completely disconnected now. Notice that we need to
             * * wait until all packets are sent. That way we know the disconnect
             * * response has been sent.
             */
            L2Cap_NotifyUserAndCloseChannel(Channel, L2DISC_USER_REQUEST);
        }
        return;
    }

    if (Event == LLC_SEND_PACKET)
    {
        Assert((Channel->flags & (L2CHF_SEND_REQUEST | L2CHF_SEND_RESPONSE)) == L2CHF_SEND_RESPONSE);
        /* We are waiting for a system packet, send the disconnect response now. */
        (void)L2Cap_SendDiscRspInternal(Channel);
        return;
    }

    /*
     * If both sides simultaneously disconnect. We can receive their
     * response while in this state. But were only waiting for our response
     * to be sent before we close the channel.
     */
}

void L2Cap_TriggerHciSend(BtRemoteDevice *remDev){
OS_Report("l2cap triggerHciSend handle:%d state:%d", remDev->hciHandle, remDev->state);
if (remDev->state == BDS_CONNECTED)
{
    if ((remDev->mode != BLM_ACTIVE_MODE)   )
    {
        /* Alert the ME that we have data to send. */
        MeHandleDataReqWhileNotActive(remDev);
    }

    /* Need both checks to cover MeSuspendTransmitters logic too. */
    if ((remDev->mode != BLM_HOLD_MODE) && (remDev->okToSend))
    {
        AssertEval(HCI_RequestToSend(remDev->hciHandle) == BT_STATUS_SUCCESS);
    }
}

}


/*****************************************************************************
 * FUNCTION
 *  L2CapState_CONFIG
 * DESCRIPTION
 *  Sending and receiving configuration commands.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_CONFIG(L2CAP_Channel *Channel, I8 Event)
{
    U16 flags;
    U8 lockStepDone = 0;
    bt_trace(TRACE_FUNC, FUNC_L2CAP_STATE_CONFIG_CID_EVENT, Channel->localCid, Event);   
    
    switch (Event)
    {
        case LLC_CONFIG_REQ:
            if ((Channel->flags & (L2CHF_OUT_CONFIG_OK | L2CHF_RTX_ACTIVE)) == (L2CHF_OUT_CONFIG_OK | L2CHF_RTX_ACTIVE))
            {
                /* We were timing their config request. Stop the timer now. */
                L2Cap_StopTimer(Channel);
            }
            L2Cap_HandleConfigReq(Channel);
            break;

        case LLC_CONFIG_RSP:
            L2Cap_HandleConfigRsp(Channel);
            /* Fall through to check for transition OPEN state */

        case LLC_TX_DONE:
            flags = Channel->flags & (L2CHF_IN_CONFIG_OK | L2CHF_OUT_CONFIG_OK);
            /// other case may fail in this case
            if( Event == LLC_TX_DONE ){
            bt_trace(TRACE_GROUP_1,BTLOG_L2CAP_CONFIG_PKT_HANDLED);
            }
            OS_Report("l2cap flags:0x%x txCount:%d", flags, Channel->l2capTxCount);
            if ((flags == (L2CHF_IN_CONFIG_OK | L2CHF_OUT_CONFIG_OK)) ) //&& (Channel->l2capTxCount == 0))
            {

                /*
                        * In and Out Configuration are done and there are no packets
                        * outstanding. Were Connected!
                        */
                /* bt_trace(TRACE_GROUP_1,L2CAP_CONFIGBT_DEBUGx04X_STATE_ADVANCED_TO_OPEN , Channel->localCid); */
                Channel->state = L2C_OPEN;
                OS_Report("l2cap: enter open state");
                {
                    //trigger hci to send 
                    L2Cap_TriggerHciSend(Channel->link);
                }
#ifdef __BT_3_0_HS__
                Channel->reConfigState = FALSE;
                if(Channel->configLockStep == TRUE)
                {
                    lockStepDone = TRUE;
                    Channel->lockstepNegoDone = TRUE;
                }
                else
                {
                    lockStepDone = FALSE;
                    Channel->lockstepNegoDone = FALSE;
                }
                
                Channel->configLockStep = FALSE;
                Channel->lockstepTxPendingDone = FALSE;
                if ((Channel->inLinkMode != Channel->outLinkMode) &&
                    ((Channel->inLinkMode == LINK_MODE_BASIC) || (Channel->outLinkMode == LINK_MODE_BASIC)))
                {
                    /* Invalid channel configuration, disconnect. */
                    L2C(callbackParms).status = L2DISC_CONFIG_UNACCEPT;
                    Report(("L2Cap: L2ERR Invalid Config in:(%d) out(%d)",Channel->inLinkMode, Channel->outLinkMode));	
                    AssertEval(L2Cap_SendDiscReqInternal(Channel) == BT_STATUS_PENDING);
                    return;
                }

                if ((Channel->inLinkMode != LINK_MODE_BASIC) &&
                    (Channel->inLinkMode != LINK_MODE_STREAMMING))/* Either direction can be tested */
                {
                    Report(("Start L2Cap_StartMonitorTimer 1 timer"));
                    L2Cap_StartMonitorTimer(Channel);
                }
                if (Channel->flags & L2CHF_CONNECTED)
                {
                    if((Channel->psmInfo->lockStepNeeded == 1) &&
						(Channel->a2mpSupported == 1))
                        /* Only if the channel is create by us, we will initiate the create physical channel process */
                    {
                        if(Channel->setupByCreateChannel == 1)
                        {
                            L2CAP_AMPListenController(Channel, Channel->currentAmpControllerId);
                            L2C(callbackParms).aclLink = Channel->link;
                            L2CAP_AMPListenController(Channel, Channel->link->remoteWifiAMP.remoteControllerId);
                            Channel->currentAmpControllerId = Channel->link->remoteWifiAMP.remoteControllerId;
                            l2CAP_AMPStartAMPTrafficForCreateChannel(Channel);
                            if(Channel->connectedEventReported == 0)
                                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                            Channel->connectedEventReported = 1;
                            Channel->setupByCreateChannel = 0;
                        }
                        else if(Channel->authorization.incoming == FALSE)
                        {
                            L2C(callbackParms).aclLink = Channel->link;
                            L2Cap_NotifyPsm(Channel, L2EVENT_LOCK_STEP_DONE, BT_STATUS_SUCCESS);
                            if(Channel->connectedEventReported == 0)
                                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                            Channel->connectedEventReported = 1;
                        }
                    }
                    else
                        L2Cap_NotifyPsm(Channel, L2EVENT_RECONFIGURED, BT_STATUS_SUCCESS);
                }
                else
                {
                    Channel->flags |= L2CHF_CONNECTED;
                    Report(("Channel->psmInfo->lockStepNeeded:%d",Channel->psmInfo->lockStepNeeded));
                    if(Channel->psmInfo->lockStepNeeded == 1)
                        /* Only if the channel is create by us, we will initiate the create physical channel process */
                    {
                        Report(("Channel->authorization.incoming:%d",Channel->authorization.incoming));
                        if(Channel->authorization.incoming == FALSE)
                        {
                            Channel->waitPreviousChannelConnected = 1;
                            Report(("Channel->a2mpSupported:%d",Channel->a2mpSupported));
                            if(Channel->a2mpSupported == 1)
                            {
                                L2C(callbackParms).aclLink = Channel->link;
                                Channel->connectedEventReported = 0;
                                Report(("Channel->rxFlowAvailable:%d",Channel->rxFlowAvailable));
                                Report(("Channel->txFlowAvailable:%d",Channel->txFlowAvailable));
                                
                                if((Channel->rxFlowAvailable == 1) && (Channel->txFlowAvailable == 1))
                                {
                                    Report(("Channel->setupByCreateChannel:%d",Channel->setupByCreateChannel));
                                    if(Channel->setupByCreateChannel == 0) /* Create channel has done the lock step process */                                        
                                    {                                
                                        L2Cap_NotifyPsm(Channel, L2EVENT_LOCK_STEP_DONE, BT_STATUS_SUCCESS);
                                    }
                                    Channel->connectedEventReported = 1;
                                    if(Channel->setupByCreateChannel == 1)
                                    {
                                        L2CAP_AMPListenController(Channel,Channel->link->remoteWifiAMP.remoteControllerId);
                                        l2CAP_AMPStartAMPTrafficForCreateChannel(Channel);
                                        Channel->currentAmpControllerId = Channel->link->remoteWifiAMP.remoteControllerId;
                                        Channel->setupByCreateChannel = 0;
                                    }
                                    L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                                    L2Cap_StartRegularMoveAMPChannelTimer(Channel, 10000);                                    
                                }
                                else
                                {
                                    Report(("Notified data1"));

                                    L2C(callbackParms).aclLink = Channel->link;
                                    L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED_START_CREATE_PHYSICAL_LINK, BT_STATUS_SUCCESS);  //daylong 110124
                                    //L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                                    L2Cap_StartRegularMoveAMPChannelTimer(Channel, 10000);
                                }
                            }
                            else
                            {
                                Report(("Notified data2"));
                                Channel->connectedEventReported = 1;
                                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                            }
                        }
                        else
                        {
                            Report(("Channel->setupByCreateChannel:%d",Channel->setupByCreateChannel));
                            Channel->waitPreviousChannelConnected = 0;
                            Channel->connectedEventReported = 1;
                            if(Channel->setupByCreateChannel == 1)
                            {
                                L2CAP_AMPListenController(Channel,Channel->link->remoteWifiAMP.remoteControllerId);
                                l2CAP_AMPStartAMPTrafficForCreateChannel(Channel);
                                Channel->currentAmpControllerId = Channel->link->remoteWifiAMP.remoteControllerId;
                                Channel->setupByCreateChannel = 0;
                            }
                            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                            //                            L2Cap_StartRegularCreateAMPChannelTimer(Channel, 1000);
                            L2Cap_StartRegularMoveAMPChannelTimer(Channel, 10000);
                        }
                    }
                    else
                    {
                        Report(("Notified data3"));
                        Channel->waitPreviousChannelConnected = 0;
                        Channel->connectedEventReported = 1;
                        L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                    }
                }
#else
                if (Channel->flags & L2CHF_CONNECTED)
                {
                    Report(("Notified data5"));
                    L2Cap_NotifyPsm(Channel, L2EVENT_RECONFIGURED, BT_STATUS_SUCCESS);
                }                   
                else
                {
                    Report(("Notified data4"));
                    Channel->flags |= L2CHF_CONNECTED;                    
                    L2Cap_NotifyPsm(Channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
                }

#endif
                break;
            }

            if ((flags == L2CHF_OUT_CONFIG_OK) && (Channel->l2capTxCount == 0))
            {
                /*
                         * A little extra protection from unresponsive devices. We set a
                         * timer while waiting for their config request.
                        */
                L2Cap_StartRtx(Channel);
            }
            break;
        case LLC_SEND_PACKET:
            /*
             * We are waiting for a system packet. Send the Configuration
             * request or response packet now. We give precidence to responses
             * to try to avoid timing out the remote's request.
             */
            if (Channel->flags & L2CHF_SEND_RESPONSE)
            {
                SendAutoConfigRsp(Channel);
            }
            else
            {
                Assert(Channel->flags & L2CHF_SEND_REQUEST);
                SendAutoConfigReq(Channel);
            }
            break;

            /* case LLC_DISC_REQ: Is handled by the core State Machine code. */
            /* case LLC_COMMAND_REJ: Is handled by the core State Machine code. */
#ifdef __BT_2_1_ERTM__
        case LLC_CREATE_CHANNEL_REQ:
            break;
        case LLC_CREATE_CHANNEL_RSP:
            break;
        case LLC_MOV_CHANNEL_REQ:
            break;
        case LLC_MOV_CHANNEL_RSP:
            break;
        case LLC_MOV_CHANNEL_CNF_REQ:
            break;
        case LLC_MOV_CHANNEL_CNF_RSP:
            break;
        case LLC_AMP_LOGICAL_LINK_IND_SUCCESS:
            if(Channel->setupByCreateChannel == 1)
            {
                L2CAP_SendLockStepConfigRsp(Channel->localCid, BT_STATUS_SUCCESS);
            }
            break;
        case LLC_AMP_LOGICAL_LINK_IND_FAILED:
            if(Channel->setupByCreateChannel == 1)
            {
                /* Create Logical link failed. */
                L2CAP_SendLockStepConfigRsp(Channel->localCid, BT_STATUS_FAILED);
                MeDisconnectLink(Channel->link ,25);
            }			
            break;
#endif
        default:
            Assert(0);
            break;

    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  L2CapState_W4_CONN_CNF
 * DESCRIPTION
 *  Waiting for a Connection Response packet from remote device.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_W4_CONN_CNF(L2CAP_Channel *Channel, I8 Event)
{
    BtStatus status;

    if (Event == LLC_CONN_RSP)
    {
        switch (LEtoHost16(L2C(currRxData) + 8))
        {
                /* Result */
            case L2CONN_ACCEPTED:
                Channel->state = L2C_CONFIG;
                Channel->remoteCid = LEtoHost16(L2C(currRxData) + 4);

                /* Initiate configuration process. */
                Assert(!(Channel->flags & (L2CHF_OUT_CONFIG_OK | L2CHF_CONFIG_REQ_SENT | L2CHF_IN_CONFIG_OK)));

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
                L2Cap_InitFlowControlSettings(Channel);
        #endif 

        #if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
                if ((Channel->flags & L2CHF_AUTO_CONFIG) == 0)
                {
                    L2Cap_NotifyPsm(Channel, L2EVENT_CONFIGURING, BT_STATUS_SUCCESS);
                    break;
                }
        #endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */ 
                SendAutoConfigReq(Channel);
                break;

            case L2CONN_PENDING:
                Channel->remoteCid = LEtoHost16(L2C(currRxData) + 4);

                L2Cap_StartErtx(Channel);
                break;
            case L2CONN_REJECT_NO_RESOURCES:
                status = L2DISC_CONN_NO_RESOURCES;
                goto IndicateDisc;
            default:
                /* Connection Rejected, set Result */
                status = LEtoHost16(L2C(currRxData) + 8) | 0x0040;
                goto IndicateDisc;
        }
        return;
    }

    if ((Event == LLC_SEND_PACKET) || (Event == LLC_ACCESS_RSP) || (Event == LLC_INFO_RSP))
    {
        /*
         * It's time to send the connect request. We were waiting for
         * security clearance or for a transmit packet.
         */
        Assert((Event == LLC_SEND_PACKET) ? Channel->flags & L2CHF_SEND_REQUEST : TRUE);

        Channel->flags &= ~L2CHF_SEND_REQUEST;

        status = L2Cap_SendConnReqInternal(Channel);
        if (status != BT_STATUS_PENDING)
        {
            if (status == BT_STATUS_NOT_SUPPORTED)
            {
                status = L2DISC_CONFIG_UNACCEPT;
            }
            else
            {
                status = L2DISC_REASON_UNKNOWN;
            }
            goto IndicateDisc;
        }
        return;
    }
#ifdef __BT_3_0_HS__
    if(Event == LLC_AMP_ACCESS_RSP)
    {

        Channel->flags &= ~L2CHF_SEND_REQUEST;

        status = L2Cap_SendCreateChannelReqInternal(Channel, Channel->remoteAmpControllerId);
        if (status != BT_STATUS_PENDING)
        {
            if (status == BT_STATUS_NOT_SUPPORTED)
            {
                status = L2DISC_CONFIG_UNACCEPT;
            }
            else
            {
                status = L2DISC_REASON_UNKNOWN;
            }
            goto IndicateDisc;
        }
        return;
    }
#endif
    if ((Event == LLC_ACCESS_REJ) || (Event == LLC_AMP_ACCESS_REJ))
    {
        /*
         * The PSM is NOT allowed to send the connect request. 
         * L2Cap_FreeChannel() will return the system packet.
         */
        status = L2DISC_SECURITY_BLOCK;
      IndicateDisc:

        L2Cap_NotifyUserAndCloseChannel(Channel, (U16) status); /* aka L2CAP_CONN_REJ */
        return;
    }

    if (Event == LLC_TX_DONE)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_L2CAP_CON_PENDING_PKT_HANDLED);
    }

    /* case LLC_COMMAND_REJ: Handled by the core State Machine code. */
}


/*****************************************************************************
 * FUNCTION
 *  L2CapState_W4_CONN_RSP
 * DESCRIPTION
 *  Received a Connect Request packet from the remote device.
 *  Waiting for the application to send a response and for the send
 *  to complete.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_W4_CONN_RSP(L2CAP_Channel *Channel, I8 Event)
{
    if (Event == LLC_TX_DONE)
    {

    #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        if (Channel->flags2 & L2CHF2_FEATURES_REQ)
        {
            /* The info request has been sent, ignore this event. */
            return;
        }
        if (Channel->flags2 & L2CHF2_FIX_CHANNEL_REQ)
        {
            /* The info request has been sent, ignore this event. */
            return;
        }        
    #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 

        /*
         * The Connect Response has been sent.
         * * If we sent a Positive response, advance to Configuration state. 
         */
        if (Channel->result == L2CONN_ACCEPTED)
        {
            Channel->state = L2C_CONFIG;

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            L2Cap_InitFlowControlSettings(Channel);
        #endif 

        #if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
            if ((Channel->flags & L2CHF_AUTO_CONFIG) == 0)
            {
                L2Cap_NotifyPsm(Channel, L2EVENT_CONFIGURING, BT_STATUS_SUCCESS);
                return;
            }
        #endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */ 
            SendAutoConfigReq(Channel);
            return;
        }

        /* If we sent a pending response, there is nothing to do here. */
        if (Channel->result == L2CONN_PENDING)
        {
            return;
        }

        /* We sent a Negative Confirmation, close the channel. */
        L2Cap_NotifyUserAndCloseChannel(Channel, L2DISC_USER_REQUEST);
        return;
    }

    if (Event == LLC_ACCESS_RSP)
    {
        /*
         * The Security Manager accepted our access request.
         * Deliver a connect indication to the upper layer protocol. 
         */
        Channel->flags &= ~(L2CHF_SEC_PENDING | L2CHF_NOT_VISIBLE);
        if (Channel->psmInfo->authorizedSetting)
        {
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_AUTORIZED, BT_STATUS_SUCCESS);
            return;
        }
        else 
        {
            L2Cap_StopTimer(Channel);
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            if(Channel->infoReqDone ==1)
                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
#else
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
#endif            
            return;
        }
    }
    if (Event == LLC_ACCESS_ACCEPT)
    {
        /*
         * The Security Manager accepted our access request.
         * Deliver a connect indication to the upper layer protocol. 
         */
        Channel->flags &= ~(L2CHF_SEC_PENDING | L2CHF_NOT_VISIBLE);
        L2Cap_StopTimer(Channel);
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        if ((Channel->psmInfo->authorizedSetting) && (Channel->infoReqDone == 1))
        {
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
        }
#else
        if (Channel->psmInfo->authorizedSetting)
        {
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
        }
#endif
        return;
    }

    if (Event == LLC_ACCESS_REJ)
    {
        /*
         * The Security Manager rejected our access request. 
         * Reject the Incoming connection request.
         */

        Channel->flags &= ~L2CHF_SEC_PENDING;
        Channel->result = L2CONN_REJECT_SECURITY_BLOCK;

        L2Cap_StopTimer(Channel);
        AssertEval(L2Cap_SendConnRspInternal(Channel) != BT_STATUS_FAILED);
        return;
    }

    if (Event == LLC_SEND_PACKET)
    {
        Assert((Event == LLC_SEND_PACKET ? (Channel->flags & L2CHF_SEND_RESPONSE) : 1));

        Channel->flags &= ~L2CHF_SEND_RESPONSE;

        AssertEval(L2Cap_SendConnRspInternal(Channel) != BT_STATUS_FAILED);
        return;
    }
    if (Event == LLC_INFO_RSP)
    {
        Assert((Event == LLC_SEND_PACKET ? (Channel->flags & L2CHF_SEND_RESPONSE) : 1));

        Channel->flags &= ~L2CHF_SEND_RESPONSE;
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        if(Channel->infoReqDone == 1)
            AssertEval(L2Cap_SendConnRspInternal(Channel) != BT_STATUS_FAILED);
#else
        AssertEval(L2Cap_SendConnRspInternal(Channel) != BT_STATUS_FAILED);
#endif
        return;
    }
#ifdef __BT_2_1_ERTM__
    if ((Event <= LLC_MOV_CHANNEL_CNF_RSP) &&
        (Event >= LLC_CREATE_CHANNEL_REQ))
        
    {
        return;
    }
#endif
    Channel->link->disconnectACLCheckCreatACLAlwaysForce = 1;
    MeDisconnectLink(Channel->link, 19);
    /* Assert(0); */

}


/*****************************************************************************
 * FUNCTION
 *  L2CapState_CLOSED
 * DESCRIPTION
 *  Generally waiting for a Connect Request.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CapState_CLOSED(L2CAP_Channel *Channel, I8 Event)
{
#if BT_SECURITY == XA_ENABLED
    BtStatus status;
#endif /* BT_SECURITY */
    OS_Report("L2CapState_CLOSED event:%d", Event );
    if ((Event == LLC_CONN_REQ) ||
		(Event == LLC_CREATE_CHANNEL_REQ))
    {
    	if(Event == LLC_CONN_REQ){
            Channel->state = L2C_W4_CONN_RSP;
    	}else{
	    Channel->state = L2C_W4_CREATE;
    	}

    #if BT_SECURITY == XA_ENABLED
        /*
         * Query the Security Manager to see if this protocol is
         * authorized to connect to us.
         */
        Channel->authorization.remDev = Channel->link;
        Channel->authorization.channel = Channel->psmInfo->psm;
        Channel->authorization.incoming = TRUE;
        Channel->authorization.id = SEC_L2CAP_ID;
#ifdef __BT_2_1_ERTM__
	if(Event == LLC_CREATE_CHANNEL_REQ)
	{
	    Channel->currentAmpControllerId = L2C(currRxData)[8];
	    Channel->MoveOperationAmpControllerId = L2C(currRxData)[8];
            OS_Report("l2cap curCtrlId:%d MoveCtrlId:%d", Channel->currentAmpControllerId, Channel->MoveOperationAmpControllerId);
	}
#endif    
    #if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
        status = SEC_AccessRequest(&Channel->authorization);
        OS_Report("l2cap sec_accessReq:%d authorizedSetting:%d ", status, Channel->psmInfo->authorizedSetting);

        if ((status == BT_STATUS_SUCCESS) && (Channel->psmInfo->authorizedSetting))
        {
            Channel->result = L2CONN_PENDING;
            L2Cap_StartErtx(Channel);
            L2Cap_SendConnRspInternal(Channel);
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_AUTORIZED, BT_STATUS_SUCCESS);
            return;

        }
    #else /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
        if (Channel->psmInfo->authorizedSetting)
        {
            Channel->result = L2CONN_PENDING;
            L2Cap_StartErtx(Channel);
            L2Cap_SendConnRspInternal(Channel);
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_AUTORIZED, BT_STATUS_SUCCESS);
            return;

        }
        else
        {
            status = SEC_AccessRequest(&Channel->authorization);
        }
    #endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
#ifdef __BT_2_1_ERTM__
        OS_Report("l2cap sec_accessReq:%d infoReqDone:%d", status, (Channel!=0?Channel->infoReqDone:0) );
#endif        
        if (status == BT_STATUS_PENDING)
        {
            /*
             * Send a Connect Pending response. We start E-RTX to track the
             * remote devices timeout. In case the local application doesn't
             * respond to the authorization request for a very long time.
             */

            Channel->result = L2CONN_PENDING;
            Channel->flags |= (L2CHF_NOT_VISIBLE | L2CHF_SEC_PENDING);

            L2Cap_StartErtx(Channel);
#ifdef __BT_3_0_HS__
	    Channel->ampPsm = LEtoHost16(L2C(currRxData) + 4);
            if(Event != LLC_CREATE_CHANNEL_REQ)
            {
                Channel->currentAmpControllerId = 0;
       	        Channel->MoveOperationAmpControllerId = 0;                
            }
            else
            {
	        Channel->currentAmpControllerId = L2C(currRxData)[8];
           	    Channel->MoveOperationAmpControllerId = L2C_INVALID_MAP_LOCAL_CONTROLLER_ID;
            }

	    if(Event == LLC_CREATE_CHANNEL_REQ)
	    {
	        L2Cap_SendCreateChannelRspInternal(Channel, L2CONN_PENDING);
	    }
	    else
	    {   
            L2Cap_SendConnRspInternal(Channel);
	    }

#endif
            return;
        }
        if (status == BT_STATUS_FAILED)
        {
            /* Send a Connect Reject response */
            Channel->flags |= L2CHF_NOT_VISIBLE;
            Channel->result = L2CONN_REJECT_SECURITY_BLOCK;
#ifdef __BT_3_0_HS__
            if(Event == LLC_CREATE_CHANNEL_REQ)
            {
                L2Cap_SendCreateChannelRspInternal(Channel, L2CONN_REJECT_SECURITY_BLOCK);
            }
            else
            {
                AssertEval(L2Cap_SendConnRspInternal(Channel) != BT_STATUS_FAILED);
            }
#else
            AssertEval(L2Cap_SendConnRspInternal(Channel) != BT_STATUS_FAILED);
#endif
            return;
        }
    #endif /* BT_SECURITY == XA_ENABLED */
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        if(Channel->infoReqDone != 1)
            /* Wait the information done.*/
        {
            Channel->flags |= (L2CHF_NOT_VISIBLE | L2CHF_SEC_PENDING);
            Channel->result = L2CONN_PENDING;
            L2Cap_StartErtx(Channel);
#ifdef __BT_3_0_HS__
	    if(Event == LLC_CREATE_CHANNEL_REQ)
	    {
	        L2Cap_SendCreateChannelRspInternal(Channel,L2CONN_PENDING);
                L2C(callbackParms).aclLink = Channel->link;
                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS); //L2EVENT_CREATE_CHANNEL_REQ

        }
	    else
#endif
            L2Cap_SendConnRspInternal(Channel);
            return;
        }
#endif
        /*
         * Access Granted: Deliver connection indication to the 
         * upper layer protocol.
         */
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        if(Channel->infoReqDone == 1)
        {
            if(Event == LLC_CONN_REQ)
            {
                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
            }
	    else
        {
#ifdef __BT_2_1_ERTM__        

            // treat this L2EVENT_CREATE_CHANNEL_REQ as CONNECT_IND so upper layer doesn't need to modify
            L2C(callbackParms).aclLink = Channel->link;
#ifdef __BT_3_0_HS__
            OS_Report("l2cap has info nofiy by curCtrlId:%d", Channel->currentAmpControllerId);

            if( 0 == Channel->currentAmpControllerId ){
                L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS); // L2EVENT_CREATE_CHANNEL_REQ
            }else{
                L2Cap_NotifyPsm(Channel, L2EVENT_CREATE_CHANNEL_REQ, BT_STATUS_SUCCESS);
            }
#else
            OS_Report("l2cap has info and conti.");
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS); // L2EVENT_CREATE_CHANNEL_REQ
#endif

#endif
    	}
    	}else{

            if(Event == LLC_CONN_REQ){
               L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
#ifdef __BT_3_0_HS__
            }else
            {
               L2C(callbackParms).aclLink = Channel->link;
               OS_Report("l2cap nofiy by curCtrlId:%d", Channel->currentAmpControllerId);               
               if( 0 == Channel->currentAmpControllerId ){
                   L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS); // L2EVENT_CREATE_CHANNEL_REQ
               }else{
                   L2Cap_NotifyPsm(Channel, L2EVENT_CREATE_CHANNEL_REQ, BT_STATUS_SUCCESS);
               }

#endif
            }
    	}
#else
        L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS);
#endif
        return;
    }
#ifdef __BT_3_0_HS__
	else if((Event >LLC_CREATE_CHANNEL_REQ) && (Event <= LLC_MOV_CHANNEL_CNF_RSP))
	{
		return;
	}
#endif
    if (Event == LLC_TX_DONE)
    {
        /*
         * If we're waiting to close the channel so packets on the channel
         * can complete, and that is done now, close the connection.
         */
        if (Channel->state == L2C_CLOSING)
        {
        
            if (Channel->l2capTxCount == 0)
            {
                /* Channel can close safely now. */
                L2Cap_NotifyUserAndCloseChannel(Channel, Channel->result);
            }
            return;
        }
    }
#ifdef __BT_3_0_HS__
    if ((Event <= LLC_MOV_CHANNEL_CNF_RSP) &&
        (Event >= LLC_MOV_CHANNEL_REQ))
        
    {
        return;
    }

#endif
    Assert(0);
}

/*---------------------------------------------------------------------------
 *            L2Cap_StateMachine()
 *---------------------------------------------------------------------------
 *
 *   The main entrypoint into the L2CAP state machine.
 *
 */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StateMachine
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_StateMachine(L2CAP_Channel *Channel, I8 Event)
{
    U16 status = L2DISC_REASON_UNKNOWN;

    Assert(Channel->localCid != L2CID_INVALID);

    if (Channel->localCid == L2CID_INVALID)
    {
        return;
    }

    L2C(callbackParms).aclLink = Channel->link;
    L2C(callbackParms).owner.psm = Channel->psmInfo;

    /*
     * The state machine first processes events that have general handlers.
     * Then the events are passed to state specific handlers.
     */
    switch (Event)
    {
        case LLC_RTX_EXPIRED:
        case LLC_ERTX_EXPIRED:
            if ((Channel->state == L2C_W4_ECHO_CNF) || (Channel->state == L2C_W4_INFO_CNF))
            {
                Channel->state = L2C_CLOSED;

                L2Cap_NotifyPsm(Channel, L2EVENT_COMPLETE, BT_STATUS_TIMEOUT);
                L2Cap_FreeChannel(Channel);
                return;
            }

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            if (Channel->flags2 & L2CHF2_FEATURES_REQ)
            {
                Assert((Channel->state == L2C_W4_CONN_CNF) || (Channel->state == L2C_W4_CONN_RSP));
                /*
                 * The Extended Features Information request timed out. Proceed 
                 * with the connection establishment.
                 */
                Channel->flags2 &= ~L2CHF2_FEATURES_REQ;

                if (Channel->link->linkModes == L2MODE_UNKNOWN)
                {
                    Channel->link->linkModes = L2MODE_BASIC;
                }

                L2Cap_StateMachine(Channel, LLC_INFO_RSP);
                return;
            }
        #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 

        #if BT_SECURITY == XA_ENABLED
            if (Channel->flags & L2CHF_SEC_PENDING)
            {
                Assert(Channel->state == L2C_W4_CONN_RSP|| Channel->state == L2C_W4_CREATE );
                /*
                 * Our internal timer on the SEC_AccessRequest() timed out. 
                 * We set the timer to the maximum E-RTX to detect when the remote
                 * device would time out the connection waiting for us.
                 */
                Channel->flags &= ~L2CHF_SEC_PENDING;
                AssertEval(SEC_CancelAccessRequest(&Channel->authorization) == BT_STATUS_SUCCESS);
            }
        #endif /* BT_SECURITY == XA_ENABLED */

            status = L2DISC_REQUEST_TIMEDOUT;
            break;

        case LLC_DISCONNECT_IND:
            status = L2DISC_LINK_DISCON;
            break;

        case LLC_COMMAND_REJ:
            status = L2DISC_REASON_UNKNOWN;
            break;

        case LLC_DISC_REQ:
            bt_trace(TRACE_GROUP_1, L2CAP_STATE_MACHINEBT_DEBUGx04X_EVENT_LLC_DISC_REQ, Channel->localCid);
            if( KAL_FALSE == (Channel->state > L2C_CLOSING && Channel->state <= L2C_W4_MOVE_CNF_RSP) ){
                // igonre it
                OS_Report("l2cap: Ignore the second disconnect state:%d", Channel->state);
                return;
            }else{
                //OS_Report("l2cap: llc_disc_req check state:%d", Channel->state);
            }

            Assert(Channel->state > L2C_CLOSING && Channel->state <= L2C_W4_MOVE_CNF_RSP);

        #if BT_SECURITY == XA_ENABLED
            if (Channel->flags & L2CHF_SEC_PENDING)
            {
                /* Cancel out outstanding SEC_AccessRequest() */
                Channel->flags &= ~L2CHF_SEC_PENDING;
                AssertEval(SEC_CancelAccessRequest(&Channel->authorization) == BT_STATUS_SUCCESS);
            }
        #endif /* BT_SECURITY == XA_ENABLED */

            if (Channel->state == L2C_W4_DISC_CNF)
            {
                /*
                 * If both sides decide to disconnect simultaneously, this may 
                 * happen. Notice that we transition to L2C_W4_DISC_RSP because
                 * they may not respond to our disconnect request. And there is
                 * no need to wait for a response (or RTX) before closing.
                 */
                Channel->state = L2C_W4_DISC_RSP;
            }
            else
            {
                if (Channel->state != L2C_W4_DISC_RSP)
                {
                    Channel->state = L2C_W4_DISC_RSP;

                    /* Notify the protocol that the channel is disconnecting. */
                    L2Cap_NotifyPsm(Channel, L2EVENT_DISCON_IND, BT_STATUS_SUCCESS);
                }
            }

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            if (Channel->flags2 & (L2CHF2_MONITOR_ACTIVE | L2CHF2_RETRANS_ACTIVE))
            {
                /* Cancel the Monitor and Retransmit timers. */
                L2Cap_StopTimer(Channel);
            }
        #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 

            /* We expect _PENDING or _NO_CONNECTION here */
            (void)L2Cap_SendDiscRspInternal(Channel);

            /* We return here because the L2C_W4_DISC_RSP will handle closing. */
            return;

        case LLC_QOS_VIOL:
            if (Channel->state != L2C_OPEN)
            {
                return;
            }

            /*
             * It is up to the application to decide what to do when a QOS
             * violation occurs. So we return instead of automatically closing
             * the channel.
             */
            L2Cap_NotifyPsm(Channel, L2EVENT_QOS_VIOLATION, BT_STATUS_SUCCESS);
            return;

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        case LLC_RETRANS_EXPIRED:

            Assert((Channel->flags2 & L2CHF2_NO_RETRANS_OUT) == 0);
            Assert(Channel->state == L2C_OPEN);
#if 1
            if(Channel->l2capTxCount == 0){
                Report(("[err] l2capTxCount 0 cid:%d", Channel->localCid));
                Channel->l2capTxCount++;
            }
#endif            
            Assert(Channel->l2capTxCount > 0);
            Report(("Dlight set the verifyRetransmissionQueueState to 1"));
            if (Channel->outLinkMode == LINK_MODE_FLOW_CONTROL)
            {
                Assert(((Channel->sendSeq - Channel->ackSeq) & (L2Cap_GetSequenceWrapValue(Channel))) > 0);
                L2Cap_ProcessFlowControlRetransmissionTimeout(Channel);
                return;
            }
            else if (Channel->outLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
            {
                L2Cap_HandleEnhancedRetransmissionTimout(Channel);
				return;
            }
            else if(Channel->outLinkMode == LINK_MODE_RETRANSMISSION)
            {
                Channel->verifyRetransmissionQueueState = 1;
                Report(("Channel->txIframeCount[%d]:%d",Channel->ackSeq, Channel->txIframeCount[Channel->ackSeq]));
                Report(("Channel->transmitMax:%d",Channel->transmitMax));
                if((Channel->txIframeCount[Channel->ackSeq] < Channel->transmitMax) ||
                    (Channel->transmitMax == 0xFFFF))
                {
                    /* Initiate I-frame retransmission. */
                    Channel->sendSeq = Channel->ackSeq;
                    L2Cap_IssueHciRequestToSend(Channel->link);
                    if (Channel->state == L2C_OPEN)
                    {
                        /* Actually if we will request to retransmission again. */
                        /* So the retransmission shall start. But the actually starting time shall be count from L2CAP_HCITxReady start procedure */
                        /* To keep the rule a timer shall always start(monitor or retransmission, so we start the monitor timer. */
                        /* Actually, we shall start the retransmission timer, but the timer will not accurate. */
                        Report(("Start L2Cap_StartMonitorTimer 2 timer"));
                        L2Cap_StartMonitorTimer(Channel);
                    }
                    /* We return now, the HCI will continue the retransmit process */
                    return;
                }
                status = L2DISC_MAX_RETRANS;
            }
            else if(Channel->inLinkMode != LINK_MODE_STREAMMING)
            {
                L2Cap_StartMonitorTimer(Channel);
                return;
            }
            /* Link is assumed lost, return packets and indicate disconnect. */
            break;

        case LLC_MONITOR_EXPIRED:
            Assert(Channel->state == L2C_OPEN);
            Assert((Channel->inLinkMode != LINK_MODE_BASIC) && (Channel->inLinkMode != LINK_MODE_STREAMMING));
            Report(("Dlight set 1 the verifyRetransmissionQueueState to 1"));
            if (Channel->outLinkMode != LINK_MODE_FLOW_CONTROL)
                Channel->verifyRetransmissionQueueState = 1;
            if (Channel->outLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
            {
                L2Cap_HandleEnhancedMonitorTimout(Channel);
            }            
            else
            {
                /* Send an RR the remote device and restart the monitor. */
                Channel->flags2 |= L2CHF2_SEND_RR;
                L2Cap_SendSFrame(Channel);
                Report(("Start L2Cap_StartMonitorTimer 3 timer"));
                L2Cap_StartMonitorTimer(Channel);
            }
            return;
        #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

        default:
            /* 
             * The state specific handlers are dispatched here 
             */


#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("l2cap Channel psm:0x%x state:%d cid:0x%x Event:%d", (Channel->psmInfo==0?0:Channel->psmInfo->psm), Channel->state, Channel->localCid, Event));
            //Report(("Event:%d",Event));
            Report(("Channel->inLinkMode:%d out:%d",Channel->inLinkMode,Channel->outLinkMode));
#endif            
            switch (Channel->state)
            {
                case L2C_CLOSED:
                case L2C_CLOSING:
                    L2CapState_CLOSED(Channel, Event);
                    break;

                case L2C_W4_CONN_CNF:   /* Waiting for the peer to sent a response */
                    L2CapState_W4_CONN_CNF(Channel, Event);
                    break;

                case L2C_W4_CONN_RSP:   /* Waiting for protocol above us to send the response */
                    L2CapState_W4_CONN_RSP(Channel, Event);
                    break;

                case L2C_CONFIG:
                    L2CapState_CONFIG(Channel, Event);
                    break;

                case L2C_OPEN:
                    L2CapState_OPEN(Channel, Event);
                    break;

                case L2C_W4_DISC_CNF:
                    L2CapState_W4_DISC_CNF(Channel, Event);
                    break;
                case L2C_W4_DISC_RSP:
                    L2CapState_W4_DISC_RSP(Channel, Event);
                    break;                    
#ifdef __BT_3_0_HS__
				case L2C_W4_CREATE:
					L2CapAMPState_W4_Create(Channel, Event);
					break;
				case L2C_W4_CREATE_RSP:
					L2CapAMPState_W4_CreateRsp(Channel, Event);
					break;
				case L2C_W4_MOVE:
					L2CapAMPState_W4_Move(Channel, Event);
					break;
				case L2C_W4_MOVE_RSP:
					L2CapAMPState_W4_MoveRsp(Channel, Event);
					break;
				case L2C_W4_MOVE_CNF:
					L2CapAMPState_W4_MoveCnf(Channel, Event);
					break;
				case L2C_W4_MOVE_CNF_RSP:
					L2CapAMPState_W4_MoveCnfRsp(Channel, Event);
					break;		
#endif
                case L2C_W4_ECHO_CNF:
                case L2C_W4_INFO_CNF:
                    /* All events are handled elsewhere. This is general failure case. */
                    if (Event != LLC_TX_DONE)
                    {
                        L2Cap_FreeChannel(Channel);
                    }
                    break;

                default:
                    Assert(0);
                    break;
            }
            return;
    }
    /*
     * All the Timeout and most LLC events caught in the first switch
     * statement come here for final actions.
     */
    L2Cap_NotifyUserAndCloseChannel(Channel, status);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_HandleConfigReq
 * DESCRIPTION
 *  Runs the Config Request received substate of the CONFIG state.
 *  Handles parsing a Configuration Request packet for the layered
 *  protocol. Called by the state machine's CONFIG state function.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_HandleConfigReq(L2CAP_Channel *Channel)
{
    L2capConfigFlags flags;
    U8 *response;
    U8 *options;
    I8 optionLen;
    U16 result = L2CFG_RESULT_SUCCESS;
    U16 remote, local, outMtu;
    U8 *flowSpec = 0;

    bt_trace(TRACE_FUNC, FUNC_L2CAP_HANDLE_CONFIG_REQ_CID, Channel->localCid);       
#if 0   /* The FC option looks not required */
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    if ((Channel->flags & L2CHF_MORE_CONFIG) == 0)
    {
        /*
         * This request is not a continuation of the current request.
         * Initialize the inbound request options.
         */
        Channel->flags2 &= ~L2CHF2_FOUND_FC_OPTION;
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
#endif
    Channel->flags &= ~(L2CHF_IN_CONFIG_OK | L2CHF_MORE_CONFIG);
    Channel->flags |= L2CHF_CONFIG_IND_RCVD;
    Channel->wsLen = 0;

    flags = LEtoHost16(L2C(currRxData) + 6);
    if (flags & L2CFG_FLAG_MORE)
    {
        Channel->flags |= L2CHF_MORE_CONFIG;
    }

    if (0 != (outMtu = L2Cap_GetMtu(L2C(currRxData) + 8, (U16) (L2C(currRxDataLen) - 4))))
    {
        Channel->outMtu = outMtu;
    }

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    if(((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_FLOW_CONTROL) !=0) ||
       ((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_RETRANSMISSIONS) !=0) ||
       ((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_ENHANCED_RETRANSMISSIONS) !=0)) 
    {
        result = L2Cap_SetFlowControlOptions(Channel, L2C(currRxData) + 8, (U16) (L2C(currRxDataLen) - 4));
        OS_Report("l2cap SetFlow result:%d", result);
    }
    else
    {
        result = L2CFG_RESULT_ACCEPTED;
    }
    /* The results are checked after the flexible config stuff. */
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 

#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
    if ((Channel->flags & L2CHF_AUTO_CONFIG) == 0)
    {
        L2C(callbackParms).ptrs.configInd.ident = L2C(currRxData)[1];
        L2C(callbackParms).ptrs.configInd.flags = flags;
        L2C(callbackParms).ptrs.configInd.options = L2C(currRxData) + 8;
        L2C(callbackParms).ptrs.configInd.optionsLen = L2C(currRxDataLen) - 4;

        L2Cap_NotifyPsm(Channel, L2EVENT_CONFIG_IND, BT_STATUS_SUCCESS);
        return;
    }
#endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */ 

    /* Build Configure response packet in channel workspace. */
    options = L2C(currRxData) + 8;
    optionLen = L2C(currRxDataLen) - 4;
    response = Channel->workspace;

    /* Now check the results from the L2Cap_SetFlowControlOptions() call. */
#if 0
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    if ((result != L2CFG_RESULT_SUCCESS) &&
        (result !=L2CFG_RESULT_PENDING))
    {
        Assert(Channel->wsLen >= 11);
    }
    else    /* Verify the MTU meets our minimum requirements. */
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 
#endif
    if (Channel->outMtu < Channel->psmInfo->minRemoteMtu)
    {
        OS_Report("l2cap outMtu:%d minRemoteMtu:%d", Channel->outMtu ,  Channel->psmInfo->minRemoteMtu);
        result = L2CFG_RESULT_UNACCEPTABLE;
        WriteConfigOption16(response, L2CFG_OPTION_MTU, Channel->psmInfo->minRemoteMtu);
        Channel->wsLen += 4;
    }

    /* Parse until we encounter a failure or run out of options. */
    while (optionLen && (result == L2CFG_RESULT_SUCCESS))
    {
        Report(("l2cap: parse config 0x%x", *options));
        switch ((*options & ~CONFIG_OPTION_HINT))
        {

            case L2CFG_OPTION_MTU:
                /*
                 * We already parsed and verified the outbound MTU in the
                 * beginning of this function. Nothing to do here.
                 */
                break;

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            case L2CFG_OPTION_FLOW_CONTROL:
                /*
                 * We already parsed and verified the inbound Flow Control and
                 * Retransmission option in the beginning of this function.
                 * Nothing to do here.
                 */
                // dump the configure req
                OS_Report("l2cap option flow (%d)(%d)(%d)(%d)(%d)(%d)", options[0], options[1],options[2],options[3],options[4],options[5]);

                if(((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_FLOW_CONTROL) !=0) ||
                  ((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_RETRANSMISSIONS) !=0) ||
                  ((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_ENHANCED_RETRANSMISSIONS) !=0)) 
                {
                }else{
	            if( Channel->outLinkMode != options[2] ){
	                /* JabraStree2 uses ERTM to connect AVRCP. reject it with BasicMode suggeston */
	                result = L2CFG_RESULT_UNACCEPTABLE;
	                response[0] = L2CFG_OPTION_FLOW_CONTROL;
	                response[1] = 9;
	                response[2] = Channel->outLinkMode;
	                response[3] = ((Channel->rxWindow > 0x00FF) ? 0xFF: (U8) Channel->rxWindow);
	                response[4] = Channel->psmInfo->inTransmitMax;
	                Report(("Retransmission timeout:%d",Channel->retransTimeout));
	                StoreLE16(response +  5, Channel->retransTimeout);
	                Report(("monitorTimeout timeout:%d",Channel->monitorTimeout));
	                StoreLE16(response +  7, Channel->monitorTimeout);
	                /* We offer the maximum segment size possible for best throughput. */
	                StoreLE16(response +  9, Channel->psmInfo->localMtu);
	                Channel->wsLen += 11;
	                response += 11;
	                Channel->configFlowSpec = 0;
                    }
                }
                break;
        #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

            case L2CFG_OPTION_FLUSH_TIMEOUT:
                Assert(!Channel->link->restricted);

                local = L2CAP_DEFAULT_FLUSH_TIMEOUT;
                remote = LEtoHost16(options + 2);
                if (remote < local)
                {
                    WriteConfigOption16(response, L2CFG_OPTION_FLUSH_TIMEOUT, remote);
                    Channel->wsLen += 4;
                }

                break;
#ifdef __BT_2_1_ERTM__
            case L2CFG_OPTION_EXT_WIN_SIZE:                
                Channel->supportExtControl = 1;
                break;
            case L2CFG_OPTION_EXT_FLOW_SPEC:
                flowSpec = options;
                break;
            case L2CFG_OPTION_FCS:
                break;
#endif                
            case L2CFG_OPTION_QOS:
                /*
                 * We ignore their QOS because WE DON'T CARE about flow rates.
                 * However, we don't have to fail their request if they simply
                 * sent us the default service type BEST_EFFORT, or NO_TRAFFIC.
                 * We only have to reject it if they requested GUARANTEED.
                 */
                if (options[3] < L2QOS_OPTION_GUARANTEED)
                {
                    break;
                }

                /* Drop through to indicate unsupported option */

            default:
                /*
                 * Unknown option type. If the option is not a hint we have to
                 * assume failure.
                 */
                if ((*options & CONFIG_OPTION_HINT) == 0)
                {
                    /*
                     * The option is not a hint. Since we can't process it,
                     * it must be failed. 
                     */
                    result = L2CFG_RESULT_UNKN_OPTIONS;
                    response[0] = options[0];
                    Channel->wsLen += 1;
                    response += 1;
                }
                else
                {
                                /* 
                                    We still need to recognize the option if the retransmission complier option is not opened.
                                */
                    if (options[0] == (L2CFG_OPTION_FLOW_CONTROL | CONFIG_OPTION_HINT))
                    {
#ifdef __BT_2_1_ERTM__                                
                        if (options[2] == 0x00)
                        {
                            result = L2CFG_RESULT_SUCCESS;
                        }
                        else
                        {
                            result = L2CFG_RESULT_REJECTED;
                        }
                        if ((Channel->wsLen + options[1] + 2) <= L2C_WORKSPACE_LEN)
                        {
                            if(Channel->configFlowSpec == 1)
                            {
                                response[0] = L2CFG_OPTION_FLOW_CONTROL;
                                response[1] = 9;
                                response[2] = Channel->outLinkMode;
                                response[3] = ((Channel->rxWindow > 0x00FF) ? 0xFF: (U8) Channel->rxWindow);
                                response[4] = Channel->psmInfo->inTransmitMax;
                                Report(("Retransmission timeout:%d",Channel->retransTimeout));
                                StoreLE16(response +  5, Channel->retransTimeout);
                                Report(("monitorTimeout timeout:%d",Channel->monitorTimeout));
                                StoreLE16(response +  7, Channel->monitorTimeout);
                                /* We offer the maximum segment size possible for best throughput. */
                                StoreLE16(response +  9, Channel->psmInfo->localMtu);
                                Channel->wsLen += 11;
                                response += 11;
                                Channel->configFlowSpec = 0;
                            }
                        }
#else
                        result = L2CFG_RESULT_SUCCESS;
#endif
                        WriteConfigOption16(response, L2CFG_OPTION_MTU, 48);
                        Channel->wsLen += 4;
                        response += 4;
                    }
                }
                break;
        }

        optionLen -= options[1] + 2;
        options += options[1] + 2;
    }
    Channel->result = result;
    if( 0 != result ){
        OS_Report("l2cap: handleconfigreq result:%d psm:0x%x", result, (Channel->psmInfo==0?0:Channel->psmInfo->psm) );
    }
    
#ifdef __BT_2_1_ERTM__ 
    if((result == L2CFG_RESULT_SUCCESS) &&
        (Channel->psmInfo->lockStepBRNeeded == 1))
    {
        SendAutoConfigRspPending(Channel);
        L2C(callbackParms).ptrs.configInd.ident = L2C(currRxData)[1];
        L2C(callbackParms).ptrs.configInd.flags = flags;
        L2C(callbackParms).ptrs.configInd.options = flowSpec;
        L2C(callbackParms).ptrs.configInd.optionsLen = 11;
        L2Cap_NotifyPsm(Channel, L2EVENT_LOCKSTEP_IN_BR_CFG_REQ, BT_STATUS_SUCCESS);
        return;
    }
#endif	
    if(result == L2CFG_RESULT_FORCE_DISCONNECT)
    {
        L2CAP_DisconnectReq(Channel->localCid);
        return;
    }
    Assert(Channel->wsLen <= L2C_WORKSPACE_LEN);
#ifdef __BT_2_1_ERTM__
    if(result == L2CFG_RESULT_PENDING)
    {
        Channel->lockstepResult = result;
        Channel->result= result;
    }
#endif
    SendAutoConfigRsp(Channel);
#ifdef __BT_2_1_ERTM__
    if(result == L2CFG_RESULT_PENDING)
    {
        Channel->lockstepTxPendingDone = TRUE;
    }
#endif
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_HandleConfigRsp
 * DESCRIPTION
 *  Runs the Config Response received substate of the CONFIG state.
 *  We don't negotiate our Outbound configuration. So if the response
 *  from the remote side was not success, we disconnect the channel.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_HandleConfigRsp(L2CAP_Channel *Channel)
{
    U16 flags, result;
    BtStatus status;

    if ((Channel->flags & L2CHF_CONFIG_REQ_SENT) == 0)
    {
        bt_trace(TRACE_GROUP_1, L2CAP_RECEIVED_UNSOLICITED_CONFIGURATION_RESPONSE);
        /* Set the completion status. */
        L2C(callbackParms).status = L2DISC_REASON_UNKNOWN;
        goto SendDisconnect;
    }

    Assert(!(Channel->flags & L2CHF_OUT_CONFIG_OK));

    flags = LEtoHost16(L2C(currRxData) + 6);
    result = LEtoHost16(L2C(currRxData) + 8);

    OS_Report("l2cap: handleconfigrsp flags:0x%x result:%d", flags, result);

    if (((flags & L2CFG_FLAG_MORE) == 0) && (result == L2CFG_RESULT_SUCCESS))
    {
        /* Complete, successful response. */
        Channel->flags |= L2CHF_OUT_CONFIG_OK;
#ifdef __BT_2_1_ERTM__ 
        if(Channel->psmInfo->lockStepBRNeeded == 1)
        {
            L2Cap_NotifyPsm(Channel, L2EVENT_LOCKSTEP_IN_BR_CFG_RSP_SUCCESS, BT_STATUS_SUCCESS);
        }
#endif
    }
    else if(result != L2CFG_RESULT_PENDING)
    {
        Channel->flags &= ~L2CHF_CONFIG_REQ_SENT;
    }
#ifdef __BT_2_1_ERTM__ 
    else if((result == L2CFG_RESULT_REJECTED) &&
        (Channel->psmInfo->lockStepBRNeeded == 1))
    {
        goto SendDisconnect;
    }    
    if((result == L2CFG_RESULT_PENDING) &&
        (Channel->psmInfo->lockStepBRNeeded == 1))
    {
        L2Cap_NotifyPsm(Channel, L2EVENT_LOCKSTEP_IN_BR_CFG_RSP_PENDING, BT_STATUS_SUCCESS);
    }
    if((result == L2CFG_RESULT_PENDING) &&
        (Channel->configLockStep  == 1))
    {
        L2Cap_NotifyPsm(Channel, L2EVENT_LOCKSTEP_CONFIG_RSP, BT_STATUS_SUCCESS);
    }
#endif

#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
    if ((Channel->flags & L2CHF_AUTO_CONFIG) == 0)
    {
        L2C(callbackParms).ptrs.configCnf.ident = L2C(currRxData)[1];
        L2C(callbackParms).ptrs.configCnf.flags = flags;
        L2C(callbackParms).ptrs.configCnf.result = result;
        L2C(callbackParms).ptrs.configCnf.options = L2C(currRxData) + 10;
        L2C(callbackParms).ptrs.configCnf.optionsLen = L2C(currRxDataLen) - 6;

        L2Cap_NotifyPsm(Channel, L2EVENT_CONFIG_CNF, BT_STATUS_SUCCESS);
        return;
    }
#endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */ 

    if (flags & L2CFG_FLAG_MORE)
    {
        /*
         * When we receive a Configuration Response with the MORE flag,
         * we're supposed to send back an empty Configuration Request
         * to solicit the next response;
         */
        bt_trace(TRACE_GROUP_1, L2CAP_SENDING_LONE_CONFIGREQ_TO_SOLICIT_MORE_RESPONSES);
        goto SendConfigReq;
    }
#ifdef __BT_2_1_ERTM__
    if(Channel->reConfigState == TRUE)
    {
        if (Channel->lockstepRxPendingDone == FALSE)
        {
            if (result == L2CFG_RESULT_PENDING)
            {
                Channel->lockstepRxPendingDone = TRUE;
            }
            else if ((result == L2CFG_RESULT_SUCCESS) &&
                (Channel->lockstepNegoDone == 0))
            {
                
// ALPS00240974 nokia will reconfig rfcomm but it doesn't need lockStep
                if( Channel->psmInfo->lockStepNeeded == TRUE ){
                /* In lockstep, the pending shall be sent first before success */
                    Report(("LockStep, not pending before success. We shall disconnect it"));
                goto SendDisconnect;
                    
                }
            }            
            else if ((result != L2CFG_RESULT_SUCCESS) && (result !=L2CFG_RESULT_FLOW_SPEC_REJECT))
            {
#if 0
// ALPS00240974 nokia will reconfig
                /* In lockstep, the pending shall be sent first before success */
                Report(("LockStep, not pending before success. We shall disconnect"));
                goto SendDisconnect;
#endif
            }
        }
    }
    {
        U8 *Options; 
		U16 OptionsLen;
        L2capExtendedFlowSpecOption l2cap_flow;

        Options = L2C(currRxData) + 10;
        OptionsLen = L2C(currRxDataLen) - 6;
        while (OptionsLen)
        {
            if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_EXT_FLOW_SPEC)
            {
                l2cap_flow.identifier = Options[2];
                l2cap_flow.serviceType = Options[3];
                l2cap_flow.maxSduSize = LEtoHost16(Options + 4);
                l2cap_flow.sduIntArrialTime = LEtoHost32(Options + 6);
                if((Channel->psmInfo->lockStepBRNeeded == 1) &&
                    (l2cap_flow.serviceType != Channel->outExtFlowSpec.serviceType))
                {
                    goto SendDisconnect;
                }
                if((l2cap_flow.serviceType != L2QOS_OPTION_BEST_EFFORT) &&
                    (l2cap_flow.serviceType != L2QOS_OPTION_NO_TRAFFIC)&&
                    (l2cap_flow.serviceType != Channel->outExtFlowSpec.serviceType))
                {
                    goto SendDisconnect;
                }
                if(l2cap_flow.serviceType == L2QOS_OPTION_BEST_EFFORT)
                {
                    if(l2cap_flow.maxSduSize <= Channel->outExtFlowSpec.maxSduSize)
                        Channel->outExtFlowSpec.maxSduSize = l2cap_flow.maxSduSize;
                    if(l2cap_flow.sduIntArrialTime >= Channel->outExtFlowSpec.sduIntArrialTime)
                        Channel->outExtFlowSpec.sduIntArrialTime = l2cap_flow.sduIntArrialTime;
                }
	        if(result == L2CFG_RESULT_SUCCESS)
	        {
    		    Channel->txFlowAvailable = 1;
	        }
            }
            OptionsLen -= Options[1] + 2;
            Options += Options[1] + 2;
         }
    }
#endif

#ifdef __BT_2_1_ERTM__ 
    if (((flags & L2CFG_FLAG_MORE) == 0) && (result == L2CFG_RESULT_SUCCESS) &&
        (Channel->psmInfo->lockStepBRNeeded == 1)) 
    {
        /* Complete, successful response. */
            L2Cap_NotifyPsm(Channel, L2EVENT_LOCKSTEP_IN_BR_CFG_RSP_SUCCESS, BT_STATUS_SUCCESS);
    }
#endif	

    if ((result != L2CFG_RESULT_SUCCESS) && (result !=L2CFG_RESULT_PENDING))
    {
#ifdef __BT_2_1_ERTM__
        {
            U8 *Options;
            U16 OptionsLen;
            L2capExtendedFlowSpecOption l2cap_flow;

            if((result == L2CFG_RESULT_FLOW_SPEC_REJECT) &&
                (Channel->configLockStep == TRUE))
            {
                Channel->lockstepRxPendingDone = FALSE;
                Options = L2C(currRxData) + 10;
                OptionsLen = L2C(currRxDataLen) - 6;
                while (OptionsLen)
                {
                    if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_EXT_FLOW_SPEC)
                    {
                        l2cap_flow.identifier = Options[2];
                        l2cap_flow.serviceType = Options[3];
                        l2cap_flow.accessLatency = LEtoHost32(Options + 10);
                        l2cap_flow.flushTimeout = LEtoHost32(Options + 14);
                        if(l2cap_flow.serviceType == L2QOS_OPTION_GUARANTEED)
                        {
                            if(l2cap_flow.accessLatency <= Channel->outExtFlowSpec.accessLatency)
                                Channel->outExtFlowSpec.accessLatency = l2cap_flow.accessLatency;
                            if(l2cap_flow.flushTimeout >= Channel->outExtFlowSpec.flushTimeout)
                                Channel->outExtFlowSpec.flushTimeout = l2cap_flow.flushTimeout;
                        }
                    }
                    OptionsLen -= Options[1] + 2;
                    Options += Options[1] + 2;
                    OS_MemCopy((U8 *)&Channel->outExtFlowSpec, (U8 *)&l2cap_flow, sizeof(L2capExtendedFlowSpecOption));
                    goto LockStepSendConfigReq;
                 }
            }
            else
            {
                if(result == L2CFG_RESULT_FLOW_SPEC_REJECT)
                {
                    Options = L2C(currRxData) + 10;
                    OptionsLen = L2C(currRxDataLen) - 6;
                    while (OptionsLen)
                    {
                        if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_EXT_FLOW_SPEC)
                        {
                            l2cap_flow.identifier = Options[2];
                            l2cap_flow.serviceType = Options[3];
                            l2cap_flow.accessLatency = LEtoHost32(Options + 10);
                            l2cap_flow.flushTimeout = LEtoHost32(Options + 14);
                            if(l2cap_flow.serviceType == L2QOS_OPTION_GUARANTEED)
                            {
                                if(l2cap_flow.accessLatency <= Channel->outExtFlowSpec.accessLatency)
                                    Channel->outExtFlowSpec.accessLatency = l2cap_flow.accessLatency;
                                if(l2cap_flow.flushTimeout >= Channel->outExtFlowSpec.flushTimeout)
                                    Channel->outExtFlowSpec.flushTimeout = l2cap_flow.flushTimeout;
                            }
                        }
                        OptionsLen -= Options[1] + 2;
                        Options += Options[1] + 2;
                        OS_MemCopy((U8 *)&Channel->outExtFlowSpec, (U8 *)&l2cap_flow, sizeof(L2capExtendedFlowSpecOption));

                        goto SendConfigReq;
                     }
                }
            }
        }
        /* The only configuration option we negotiate is Flow Control. */
        if ((L2C(currRxDataLen) > (6 + 2)) && (L2C(currRxData)[10] == L2CFG_OPTION_FLOW_CONTROL))
        {
            bt_trace(TRACE_GROUP_1, L2CAP_RECEIVED_CONFIGRSPNEG_FOR_FLOW_CONTROL_OPTION);

            /* Convert protocol enumeration to a flag for comparison with API link modes. */
            if (Channel->psmInfo->inLinkMode & (1 << (L2C(currRxData)[10 + 2])))
            {
                /* We support the mode they suggested so use it. */
                Channel->inLinkMode = L2C(currRxData)[10 + 2];
                goto SendConfigReq;
            }

            /* We don't support their suggested mode so try one we haven't tried yet. */
            switch (Channel->inLinkMode)
            {
                case LINK_MODE_BASIC:
                case LINK_MODE_ENHANCED_RETRANSMISSION:
                case LINK_MODE_STREAMMING:
                    switch(Channel->psmInfo->op_state)
                    {
                        case L2OPERATORSTATE_STATE0:
                            Channel->inLinkMode = LINK_MODE_BASIC;
                            /* No match mode, use basic mode. */
                            goto SendConfigReq;
                        case L2OPERATORSTATE_STATE1:
                            if(Channel->inLinkMode == LINK_MODE_BASIC)
                            {
                                Channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                                goto SendConfigReq;                                
                            }
                            else if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
                            {
                                Channel->inLinkMode = LINK_MODE_STREAMMING;
                                goto SendConfigReq;
                            }
                            else if(Channel->inLinkMode == LINK_MODE_STREAMMING)
                            {
                                Channel->inLinkMode = LINK_MODE_STREAMMING;
                                goto SendDisconnect;
                            }
                            break;
                        case L2OPERATORSTATE_STATE2:
                            Channel->inLinkMode = LINK_MODE_BASIC;
                            /* No match mode, disconnect link. */
                            goto SendDisconnect;
                    }
                    break;
                case LINK_MODE_RETRANSMISSION:
                    if ((Channel->psmInfo->inLinkMode & Channel->link->linkModes) & L2MODE_FLOW_CONTROL)
                    {
                        Channel->inLinkMode = LINK_MODE_FLOW_CONTROL;
                        goto SendConfigReq;
                    }
                    /* Drop into next case */

                case LINK_MODE_FLOW_CONTROL:
                    if ((Channel->psmInfo->inLinkMode & Channel->link->linkModes) & L2MODE_BASIC)
                    {
                        Channel->inLinkMode = LINK_MODE_BASIC;
                        goto SendConfigReq;
                    }
                    /* Drop out of switch to disconnect code */
            }
        }
    #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

        bt_trace(TRACE_GROUP_1, L2CAP_RECEIVED_CONFIGRSPNEG_DISCONNECTING);
        /* Set the completion status. */
        L2C(callbackParms).status = (U16) (result | 0x80);

      SendDisconnect:
        /* Response was negative. We don't negotiate so Disconnect the channel. */
        status = L2Cap_SendDiscReqInternal(Channel);
        AssertEval(status == BT_STATUS_PENDING);
    }
    return;
#ifdef __BT_2_1_ERTM__
  LockStepSendConfigReq:
    SendAutoLockStepConfigReq(Channel);
    return;
#endif
  SendConfigReq:
    SendAutoConfigReq(Channel);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  SendAutoConfigRsp
 * DESCRIPTION
 *  Sends configuration responses for auto-configuration channels.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/

void SendAutoConfigRsp(L2CAP_Channel *Channel)
{
    BtPacket *rsp;
    BtStatus status;
    U8 length;

    bt_trace(TRACE_FUNC, FUNC_L2CAP_SEND_AUTO_CONFIG_RSP_CID, Channel->localCid);       
#ifdef __BT_2_1_ERTM__
    if( L2CFG_RESULT_ACCEPTED == Channel->result || L2CFG_RESULT_PENDING == Channel->result){
    OS_Report("l2cap lockstep:%d pendingdone:%d",Channel->configLockStep, Channel->lockstepTxPendingDone);
    if((Channel->configLockStep == TRUE)&&
        (Channel->lockstepTxPendingDone == FALSE))
    {
        SendLockStepConfigRsp(Channel, Channel->result);
        
        return;
    }
    else if((Channel->configLockStep == TRUE)&&
        (Channel->lockstepTxPendingDone == TRUE))
    {
        if( 0 != Channel->lockstepResult ){
            OS_Report("lockstepResult:%d", Channel->lockstepResult);
        }
        SendLockStepConfigRsp(Channel, Channel->lockstepResult);
        Channel->lockstepTxPendingDone = FALSE;
        return;
    }
    }
#endif

    if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_RESPONSE;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return;
    }
    Channel->flags &= ~L2CHF_SEND_RESPONSE;
    length = 0;
    
#ifdef __BT_2_1_ERTM__
    if(Channel->configFlowSpec == 1)
    {
        rsp->data[length+0] = L2CFG_OPTION_FLOW_CONTROL;
        rsp->data[length+1] = 9;
        rsp->data[length+2] = Channel->outLinkMode;
        rsp->data[length+3] = ((Channel->rxWindow > 0x00FF) ? 0xFF: (U8) Channel->rxWindow);
        rsp->data[length+4] = Channel->psmInfo->inTransmitMax;
        StoreLE16(rsp->data + length + 5, Channel->retransTimeout);
        StoreLE16(rsp->data + length + 7, Channel->monitorTimeout);
        /* We offer the maximum segment size possible for best throughput. */
        StoreLE16(rsp->data + length +  9, Channel->psmInfo->localMtu);
        length += 11;
    }
#endif    
    Channel->configFlowSpec = 0;
#ifdef __BT_2_1_ERTM__
    if((Channel->result == L2CFG_RESULT_ACCEPTED) &&
		(Channel->configExtendedFlowSpec == 1))
    {
        rsp->data[length] = L2CFG_OPTION_EXT_FLOW_SPEC;
        rsp->data[length+1] = 16;
        rsp->data[length+2] = Channel->inExtFlowSpec.identifier;
        rsp->data[length+3] = Channel->inExtFlowSpec.serviceType;
        StoreLE16(rsp->data + length+4, Channel->inExtFlowSpec.maxSduSize);
        StoreLE32(rsp->data + length+6, Channel->inExtFlowSpec.sduIntArrialTime);
        StoreLE32(rsp->data + length+10, Channel->inExtFlowSpec.accessLatency);
        StoreLE32(rsp->data + length+14, Channel->inExtFlowSpec.flushTimeout);
	length += 18;
    }
    Channel->configExtendedFlowSpec = 0;
#endif
    OS_MemCopy(rsp->data + length, Channel->workspace, Channel->wsLen);
    rsp->dataLen = length + Channel->wsLen;

    status = L2Cap_SendConfigRspInternal(
                Channel,
                rsp,
                Channel->result,
                (L2capConfigFlags) ((Channel->flags & L2CHF_MORE_CONFIG) ? L2CFG_FLAG_MORE : 0));

    Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_NO_CONNECTION));

    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }
}


/** 
  * @brief Return a pending result for ConfigReq
  */
void SendAutoConfigRspPending(L2CAP_Channel *Channel)
{
    BtPacket *rsp;
    BtStatus status;
    U8 length;
    if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_RESPONSE;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return;
    }
    Channel->flags &= ~L2CHF_SEND_RESPONSE;
    Channel->result = L2CFG_RESULT_PENDING;    

#ifdef __BT_2_1_ERTM__	
    length = 18;
    rsp->dataLen = 18;
    rsp->data[0] = L2CFG_OPTION_EXT_FLOW_SPEC;
    rsp->data[1] = 16;
    rsp->data[2] = Channel->inExtFlowSpec.identifier;
    rsp->data[3] = Channel->inExtFlowSpec.serviceType;
    StoreLE16(rsp->data +  4, Channel->inExtFlowSpec.maxSduSize);
    StoreLE32(rsp->data +  6, Channel->inExtFlowSpec.sduIntArrialTime);
    StoreLE32(rsp->data +  10, Channel->inExtFlowSpec.accessLatency);
    StoreLE32(rsp->data +  14, Channel->inExtFlowSpec.flushTimeout);
    length += 18;

    status = L2Cap_SendConfigRspInternal(
                Channel,
                rsp,
                Channel->result,
                18);
#else
status = L2Cap_SendConfigRspInternal(
			Channel,
			rsp,
			Channel->result,
			0);

#endif
    Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_NO_CONNECTION));

    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }
}

/*****************************************************************************
 * FUNCTION
 *  SendAutoConfigReq
 * DESCRIPTION
 *  Sends configuration requests for auto-configuration channels.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SendAutoConfigReq(L2CAP_Channel *Channel)
{
    BtPacket *request;
    BtStatus status;

#ifdef __BT_2_1_ERTM__
    if(Channel->configLockStep == TRUE)
    {
        SendAutoLockStepConfigReq(Channel);
        return;
    }
#endif

    if ((request = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return;
    }
    Channel->flags &= ~L2CHF_SEND_REQUEST;

    /* Only include options if this is the first config req we're sending. */
    if ((Channel->flags & L2CHF_MORE_CONFIG) == 0)
    {

        /* Include the MTU if were not using the default. */
        if (Channel->psmInfo->localMtu != L2CAP_DEFAULT_MTU)
        {
            WriteConfigOption16(request->data, L2CFG_OPTION_MTU, Channel->psmInfo->localMtu);
            request->dataLen += 4;
        }

    #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        /* Include the Flow Control option if we're not basic-only */
        Report(("Channel->inLinkMode:%d out:%d",Channel->inLinkMode, Channel->outLinkMode));
        if ((Channel->psmInfo->inLinkMode & (Channel->link->linkModes | L2MODE_BASIC)) != L2MODE_BASIC)
        {
            request->data[request->dataLen + 0] = L2CFG_OPTION_FLOW_CONTROL;
            request->data[request->dataLen + 1] = 9;
            request->data[request->dataLen + 2] = Channel->inLinkMode;
            request->data[request->dataLen + 3] = ((Channel->rxWindow > 0x00FF) ? 0xFF: (U8)Channel->rxWindow);
            request->data[request->dataLen + 4] = Channel->psmInfo->inTransmitMax;
            StoreLE16(request->data + request->dataLen + 5, Channel->psmInfo->inRetransTimeout);
            StoreLE16(request->data + request->dataLen + 7, Channel->psmInfo->inMonitorTimeout);

            if(Channel->inLinkMode == LINK_MODE_STREAMMING)
            {
                StoreLE16(request->data + request->dataLen + 5, 0x00);
                StoreLE16(request->data + request->dataLen + 7, 0x00);
            }
#ifdef __BT_3_0_HS__
            else if((Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION) &&
                (Channel->hciAMPHandler == L2PHY_TPYE_BASICEDR))
            {
                /* The enhancement retransmission in basic mode, 
                              retransmission and monitor timeout shall be set to 0 
                           */

                StoreLE16(request->data + request->dataLen + 5, 0x00);
                StoreLE16(request->data + request->dataLen + 7, 0x00);
            }
#else
            else if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
            {
                /* The enhancement retransmission in basic mode, 
                              retransmission and monitor timeout shall be set to 0 
                           */

                StoreLE16(request->data + request->dataLen + 5, 0x00);
                StoreLE16(request->data + request->dataLen + 7, 0x00);
            }
#endif

#ifdef __BT_3_0_HS__
            else if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
            {
                /* AMP Phy*/
                if(Channel->AMPInfo.flushTimeOut == 0xFFFFFFFF)
                {
                    /* The enhancement retransmission in AMP mode, no flush timeout,
                                  retransmission and monitor timeout shall be set to 0 
                                   */

                    StoreLE16(request->data + request->dataLen + 5, 0x00);
                    StoreLE16(request->data + request->dataLen + 7, 0x00);
                }
                else
                {
                    StoreLE16(request->data + request->dataLen + 5, MINIMUM_AMP_RETRANS_TIMEOUT);
                    StoreLE16(request->data + request->dataLen + 7, MINIMUM_AMP_MONITOR_TIMEOUT);
                }
            }
#endif

            /* We offer the maximum segment size possible for best throughput. */
            StoreLE16(request->data + request->dataLen + 9, Channel->psmInfo->localMtu);
            request->dataLen += 11;
            if (Channel->inLinkMode == LINK_MODE_STREAMMING)
            {
                /* streamming mode shall use the FCS option */
                Channel->outFcsUsed = L2FCS_OPTION_NO_FCS;
                request->data[request->dataLen + 0] = L2CFG_OPTION_FCS;
                request->data[request->dataLen + 1] = 1;
                request->data[request->dataLen + 2] = L2FCS_OPTION_NO_FCS;
                request->dataLen += 3;
            }
      
    #if ((L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED) || (L2CAP_EXTENDED_FLOW_SPEC == XA_ENABLED))
            if((Channel->link->l2cap_feature & L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION) !=0)
            {
                Report(("Support Extended Flow Spec"));
//                if ((Channel->inLinkMode == LINK_MODE_STREAMMING) ||
//                    (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION))
			// For UPF36, IVT suggest we shall send extended flow spec to remote side.
                {
                    request->data[request->dataLen + 0] = L2CFG_OPTION_EXT_FLOW_SPEC;
                    request->data[request->dataLen + 1] = 16;
                    request->data[request->dataLen + 2] = Channel->outExtFlowSpec.identifier;
                    request->data[request->dataLen + 3] = Channel->outExtFlowSpec.serviceType;
                    StoreLE16(request->data + request->dataLen + 4, Channel->outExtFlowSpec.maxSduSize);
                    StoreLE32(request->data + request->dataLen + 6, Channel->outExtFlowSpec.sduIntArrialTime);
                    StoreLE32(request->data + request->dataLen + 10, Channel->outExtFlowSpec.accessLatency);
                    StoreLE32(request->data + request->dataLen + 14, Channel->outExtFlowSpec.flushTimeout);
                    request->dataLen += 18;
                }            
            }
    #endif

    #if L2CAP_FLOW_CONFIG_EXTENDED_TX_WINDOW == XA_ENABLED
        if((Channel->link->l2cap_feature & L2EXTENDED_WINDOW_SIZE) !=0)
        {
            if ((Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION) ||
                (Channel->inLinkMode == LINK_MODE_STREAMMING) &&
                (Channel->psmInfo->psm != BT_PSM_A2MP))
            {
                Report(("Support Extended TxWindows Size"));
                Channel->supportExtControl = 1;
                request->data[request->dataLen + 0] = L2CFG_OPTION_EXT_WIN_SIZE;
                request->data[request->dataLen + 1] = 2;
                StoreLE16(request->data + request->dataLen + 2, Channel->rxWindow);
                request->dataLen += 4;
            }            
        }
    #endif
        }
    #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 
    }

    status = L2Cap_SendConfigReqInternal(Channel, request, L2CFG_FLAG_NONE);
    Assert((status == BT_STATUS_PENDING) || (Channel->link->state > BDS_CONNECTED));

    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(request);
    }
}



/*****************************************************************************
 * FUNCTION
 *  SendAutoLockStepConfigReq
 * DESCRIPTION
 *  Sends LockStep configuration requests for auto-configuration channels.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
BtStatus SendAutoLockStepConfigReq(L2CAP_Channel *Channel)
{
    BtPacket *request;
    BtStatus status;
    U32 features;

    if ((request = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }
    Channel->flags &= ~L2CHF_SEND_REQUEST;
    if(Channel->setupByCreateChannel == 1)
    {
        if (Channel->psmInfo->localMtu != L2CAP_DEFAULT_MTU)
        {
            WriteConfigOption16(request->data, L2CFG_OPTION_MTU, Channel->psmInfo->localMtu);
            request->dataLen += 4;
        }    
        request->data[request->dataLen + 0] = L2CFG_OPTION_FLOW_CONTROL;
        request->data[request->dataLen + 1] = 9;
        request->data[request->dataLen + 2] = Channel->inLinkMode;
        request->data[request->dataLen + 3] = ((Channel->rxWindow > 0x00FF) ? 0xFF: (U8)Channel->rxWindow);
        request->data[request->dataLen + 4] = Channel->psmInfo->inTransmitMax;
        StoreLE16(request->data + request->dataLen + 5, Channel->retransTimeout);
        StoreLE16(request->data + request->dataLen + 7, Channel->monitorTimeout);
        /* We offer the maximum segment size possible for best throughput. */
        StoreLE16(request->data + request->dataLen +  9, Channel->psmInfo->localMtu);
        request->dataLen += 11;
    }
    features = Channel->link->l2cap_feature;

    if (features & L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION)
    {
    request->data[request->dataLen + 0] = L2CFG_OPTION_EXT_FLOW_SPEC;
    request->data[request->dataLen + 1] = 16;
    if(Channel->outExtFlowSpec.serviceType == L2QOS_OPTION_BEST_EFFORT)
    {
        Channel->outExtFlowSpec.identifier = 0x01;
    }
    else
    {
        Channel->outExtFlowSpec.identifier = (U8) Channel->localCid;
    }
    request->data[request->dataLen + 2] = Channel->outExtFlowSpec.identifier;
    request->data[request->dataLen + 3] = Channel->outExtFlowSpec.serviceType;
    StoreLE16(request->data + request->dataLen + 4, Channel->outExtFlowSpec.maxSduSize);
    StoreLE32(request->data + request->dataLen + 6, Channel->outExtFlowSpec.sduIntArrialTime);
    StoreLE32(request->data + request->dataLen + 10, Channel->outExtFlowSpec.accessLatency);
    StoreLE32(request->data + request->dataLen + 14, Channel->outExtFlowSpec.flushTimeout);
    request->dataLen += 18;
    }
    status = L2Cap_SendConfigReqInternal(Channel, request, L2CFG_FLAG_NONE);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(request);
    }
    return status;
}

BtStatus SendLockStepConfigRsp(L2CAP_Channel *Channel, U16 result)
{
    BtPacket *request;
    BtStatus status;

    if ((request = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_RESPONSE;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        Channel->lockstepResult = result;
        return BT_STATUS_PENDING;
    }
    Channel->flags &= ~L2CHF_SEND_RESPONSE;
    
    if (Channel->link->l2cap_feature & L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION)
    {
    request->data[request->dataLen + 0] = L2CFG_OPTION_EXT_FLOW_SPEC;
    request->data[request->dataLen + 1] = 16;
    request->data[request->dataLen + 2] = Channel->inExtFlowSpec.identifier;
    request->data[request->dataLen + 3] = Channel->inExtFlowSpec.serviceType;
    StoreLE16(request->data + request->dataLen + 4, Channel->inExtFlowSpec.maxSduSize);
    StoreLE32(request->data + request->dataLen + 6, Channel->inExtFlowSpec.sduIntArrialTime);
    StoreLE32(request->data + request->dataLen + 10, Channel->inExtFlowSpec.accessLatency);
    StoreLE32(request->data + request->dataLen + 14, Channel->inExtFlowSpec.flushTimeout);
    request->dataLen += 18;
    }
    status = L2Cap_SendConfigRspInternal(Channel, request, result, L2CFG_FLAG_NONE);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(request);
    }
    else
    {
        /* Send config response pending */
        if(result  == L2CFG_RESULT_PENDING)
        {
            L2C(callbackParms).ptrs.lockstepConfigReq.flowSpec = Channel->inExtFlowSpec;
            Report(("Channel->createChannelInitiator:%d lockstepNeed:%d",Channel->createChannelInitiator, Channel->psmInfo->lockStepNeeded));
            L2C(callbackParms).ptrs.lockstepConfigReq.createChannelnitiator = Channel->createChannelInitiator;            
            if( FALSE != Channel->psmInfo->lockStepNeeded){
            L2Cap_NotifyPsm(Channel, L2EVENT_LOCKSTEP_CONFIG_REQ, BT_STATUS_SUCCESS);
            }else{
                OS_Report("Channel doesnot need lockstep. send ok directly!");
                L2CAP_SendLockStepConfigRsp(Channel->localCid, BT_STATUS_SUCCESS);    
            }
        }
    }
    return status;
}


#endif

#ifdef __BT_4_0_BLE__
U16 L2Cap_ProcessBLEConnectParmsUpdate(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Source)
{
    U16 interval_min, interval_max, slave_latency, timeout;
    U8 doReject = 0;
    BTBMRemoteNameSetting_T *deviceName = 0;

    interval_min = LEtoHost16(RxData + 4);
    interval_max = LEtoHost16(RxData + 6);
    slave_latency = LEtoHost16(RxData + 8);
    timeout = LEtoHost16(RxData + 10);

    if(Source->hciHandle > 0x0F00 ||
       interval_min < 0x0006 || interval_min > 0x0C80 ||
       interval_max < interval_min || interval_max > 0x0C80 ||
       slave_latency > 500 ||
       timeout < 0x000A || timeout > 0x0C80)
    {
        doReject = 1;
        return L2CPU_REJECT;
    }

    // check IOT by name
    deviceName = btbmUtilConnectedDeviceNameEntryLookUp(&Source->bdAddr);
    if( 0 != deviceName && deviceName->dev_name_len > 0 ){
        if( 0 == OS_StrnCmp( deviceName->dev_name, "CSR1Mouse", OS_StrLen("CSR1Mouse")) ){
            doReject = 1;
            OS_Report("L2CAP:= update-parameter-request inside reject mouse list 1");
        }
        if( 0 == OS_StrnCmp( deviceName->dev_name, "BSMBB09DS", OS_StrLen("BSMBB09DS")) ){
            doReject = 1;
            OS_Report("L2CAP:= update-parameter-request inside reject device list 2");
        }
        if( 0 == OS_StrnCmp( deviceName->dev_name, "Mad Catz M.O.U.S.9", OS_StrLen("Mad Catz M.O.U.S.9")) ){
            doReject = 1;
            OS_Report("L2CAP:= update-parameter-request inside reject device list 3");
        }
    }
    // check IOT by address (it is possible that we donot have remote-name yet.)
    if( 0 != Source ){
        // CSR1Mouse "00:02:22"
        if( Source->bdAddr.addr[5] == 0x00 && Source->bdAddr.addr[4] == 0x02 && Source->bdAddr.addr[3] == 0x22 ){
            OS_Report("L2CAP:= update-parameter-request is inside reject-address list 1");
            doReject = 1;
        }
        // BSMBB09DS "00:1B:DC"
        if( Source->bdAddr.addr[5] == 0x00 && Source->bdAddr.addr[4] == 0x1B && Source->bdAddr.addr[3] == 0xDC ){
            OS_Report("L2CAP:= update-parameter-request is inside reject-address list 2");
            doReject = 1;
        }
        // Mad Catz M.O.U.S.9 "00:15:9E"
        if( Source->bdAddr.addr[5] == 0x00 && Source->bdAddr.addr[4] == 0x15 && Source->bdAddr.addr[3] == 0x9E ){
            OS_Report("L2CAP:= update-parameter-request is inside reject-address list 3");
            doReject = 1;
        }
    }
    
    if( 1 == doReject ){
        return L2CPU_REJECT;
    }else{
        OS_Report("L2CAP update-parameter-request check pass ");
    }
    
    LeHciUpdateConnection(Source->hciHandle, 
                          interval_min, 
                          interval_max, 
                          slave_latency, 
                          timeout,
                          BT_LE_DEFAULT_MIN_CE_LENGTH,
                          BT_LE_DEFAULT_MAX_CE_LENGTH);
    return L2CPU_ACCEPT;
}
#endif

