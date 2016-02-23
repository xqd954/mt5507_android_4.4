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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfgi.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the internal function prototypes and data
 *              structures for the hands free profile
 *             
 * Created:     March 15, 2005
 *
 * Copyright 2001-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 05 17 2012 huirong.liao
 * [ALPS00102390] <elephant ES8100 >
 * .
 *
 * 04 25 2012 huirong.liao
 * [ALPS00270892] [BlueTooth][HFP]DUT Connected with PC "phone audio" will disconnected itself after a while "This issue also exist on MT6577"
 * .
 *
 * Jul 7 2009 mtk80029
 * [MAUI_01875157] [BT HFP] Remove build warning for turning off HFP profile project
 * 
 *
 * Jun 30 2009 mtk80029
 * [MAUI_01873300] [1] Assert fail: 0 conmgr.c 919 - BT
 * add link flag: SCO_CONNECTING
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#ifndef __HFGI_H_
#define __HFGI_H_

#include "hfg.h"

/* identifier stored in channel context that indicates channel type as 
   headset or handsfree audio gateway */
typedef enum
{
    AG_TYPE_HS = 0x00,
    AG_TYPE_HF = 0x01, 
}AgType;

/* Channel States */
typedef enum
{
    HFG_STATE_CLOSED        = 0x00,
    HFG_STATE_CONN_PENDING  = 0x01,
    HFG_STATE_CONN_INCOMING = 0x02,
    HFG_STATE_DISCONNECTING = 0x03,
    HFG_STATE_OPEN          = 0x04,
    HFG_STATE_SLC           = 0x05,  
}HfgStateType;
 
/*****************************************************
*  Definition of sub states
*****************************************************/
typedef enum
{
    /* Sub states of state HFG_STATE_CLOSED */
    HFG_SUBSTATE_CLOSED            = HFG_STATE_CLOSED,
    
    /* Sub states of state HFG_STATE_CONN_PENDING */
    /* HFG_SUBSTATE_CONN_OUT_W4ACL    : wait for ACL connection done    */
    /* HFG_SUBSTATE_CONN_OUT_W4SDP    : wait for SDP query done         */
    /* HFG_SUBSTATE_CONN_OUT_W4RFCOMM : wait for RFCOMM connection done */
    HFG_SUBSTATE_CONN_OUT_W4ACL    = HFG_STATE_CONN_PENDING,
    HFG_SUBSTATE_CONN_OUT_W4SDP    = HFG_STATE_CONN_PENDING | 0x10,
    HFG_SUBSTATE_CONN_OUT_W4RFCOMM = HFG_STATE_CONN_PENDING | 0x20,

    /* Sub states of state HFG_STATE_CONN_INCOMING */
    /* HFG_SUBSTATE_CONN_IN_W4RSP    : wait for upper layer responding RFEVENT_OPEN_IND */
    /* HFG_SUBSTATE_CONN_IN_W4RFCOMM : wait for RFCOMM processing accept/reject         */
    HFG_SUBSTATE_CONN_IN_W4RSP     = HFG_STATE_CONN_INCOMING,
    HFG_SUBSTATE_CONN_IN_W4RFCOMM  = HFG_STATE_CONN_INCOMING | 0x10,

    /* Sub states of state HFG_STATE_DISCONNECTING */
    HFG_SUBSTATE_DISC              = HFG_STATE_DISCONNECTING,

    /* Sub states of state HFG_STATE_OPEN */
    /* HFG_SUBSTATE_OPEN       : without SDP query in progress */
    /* HFG_SUBSTATE_OPEN_W4SDP : with SDP query in progress    */
    HFG_SUBSTATE_OPEN              = HFG_STATE_OPEN,
    HFG_SUBSTATE_OPEN_W4SDP        = HFG_STATE_OPEN | 0x10,

    /* Sub states of state HFG_STATE_SLC */
    /* HFG_SUBSTATE_SLC       : without SCO connection/disconnection in progress */
    /* HFG_SUBSTATE_SLC_W4SCO : with SCO connection/disconnection in progress    */
    HFG_SUBSTATE_SLC               = HFG_STATE_SLC,
    HFG_SUBSTATE_SLC_W4SCO         = HFG_STATE_SLC | 0x10,            
}HfgSubStateType;

/** HFP 1.6 Codec Negotiation 
 ** Codec connection state 
 **   INIT ------------(+BCS)----------------> CODEC_SELECTING <----------------
 **    ^                                           |                            |
 **    |                                       (AT+BCS)                         |
 **    |                                           |                            |
 **    |                                           v                            |
 **    |---------------(AT+BAC  )---------------CODEC_DECIDED --(SCO on fail)----
 ** 
 **/
/** INIT            : Init                                            **/ 
/** CODEC_SELECTING : AG has sent +BCS to HF with a selected codec ID **/
/** CODEC_DECIDED   : AG has received AT+BCS with the same codec ID   **/
typedef enum
{
    HFG_CODEC_STATE_INIT            = 0x00,
    HFG_CODEC_STATE_CODEC_SELECTING = 0x01,
    HFG_CODEC_STATE_CODEC_DECIDED   = 0x02,
}HfgCodecStateType;

/*****************************************************
*  Definition of Link Flags (Value of HfgChannel.linkFlags)
*****************************************************/
/* DESC : ACL link created and associated. It is set/reset through CMGR_CreateDataLink 
 *            and CMGR_RemoveDataLink.
 *
 * SET : ACL link is associated.
 * UNSET : Not associated ACL link.
 */
#define HFG_LINK_ACL                     0x01
/* DESC : Indicate if SCO connection creation is ongoing.
 *
 * SET : SCO connection creation is ongoing.
 * UNSET : No SCO connection creation is ongoing.
 */
#define HFG_LINK_SCO_CONNECTING          0x02
/* DESC : Indicate if SCO connection creation is ongoing.
 *
 * SET : SCO connection removal is ongoing.
 * UNSET : No SCO connection removal is ongoing.
 */
#define HFG_LINK_SCO_DISCONNECTING       0x04
/* DESC : Indicate if RFCOMM channel for HFG is created or not
 *
 * SET : RFCOMM channel for HFG is created.
 * UNSET : No RFCOMM channel for HFG is created.
 */
#define HFG_LINK_HANDSFREE               0x08
/* DESC : Indicate if RFCOMM channel shall be removed after the SCO connection is removed. 
 *       It is used in disconnection procedure.
 *
 * SET : RFCOMM channel shall be removed after SCO is disconnected.
 * UNSET : RFCOMM channel shall not be removed after SCO is disconnected.
 */
 #define HFG_LINK_ACL_CONNECTING         0x10
/* DESC : Not used
 */
#define HFG_LINK_CONNECTING              0x20
/* DESC : Not used
 */
#define HFG_LINK_DISCONNECTING           0x40
/* DESC : Not used
 */
#define HFG_LINK_REM_DISC                0x80


/* Internal State Machine Events */
#define EVENT_OPEN_IND         0x80
#define EVENT_OPEN             0x81
#define EVENT_AT_DATA          0x82
#define EVENT_CLOSED           0x83
#define EVENT_RF_PACKET_SENT   0x85

/*****************************************************
*  Definition of channel flags (Value of HfgChannel.flags)
*****************************************************/
/* DESC : Indicate the connected connection is HF originated or HFG initiated.
 *
 * SET : The connection is initiated by HFG
 * UNSET : The connection is initiated by HF device.
 */
#define CHANNEL_FLAG_OUTGOING            0x0001
/* DESC : If one packet is sent through RFCOMM channel but the packet handled 
 *            indication is not received yet. It is set after the RFCOMM API - AtSendRfPacket
 *            returns pending. And reset after RFEVENT_PACKET_HANDLED is receive and 
 *       indicate the packet is totally sent.
 *
 * SET : One packet is sent, but handled indication is not received yet.
 * UNSET : No packets transmission is ongoing.
 */
#define CHANNEL_FLAG_TX_IN_PROGRESS      0x0002
/* DESC : Set this flag tto tell HFG to send OK response aautomatically. If this flag is set
 *       HFG will sent OK response following the completion of previous packet transmission
 *
 * SET : Send OK response automatically
 * UNSET : Do not send OK response automatically
 */
#define CHANNEL_FLAG_SEND_OK             0x0004
/* DESC : Indicate if the negotiation is started. It is set when RFCOMM channel is created.
 *            It is reset after the negotiation is completed.
 *
 * SET : Negotiation between HFG and HF device is ongoing.
 * UNSET : Negotiation between HFG and HF device is completed or not started.
 */
#define CHANNEL_FLAG_NEGOTIATE           0x0008
/* DESC : Indicate the negotiation is completed or not.
 *
 * SET : Negotiation is completed.
 * UNSET : Negotiation is completed yet.
 */
#define CHANNEL_FLAG_NEG_DONE            0x0010
/* DESC : Indicate the exchange of supported HFP features is completed or not. Some devices
 *            support only HFP 0.99 in that the AT+BRSF is not defined, so we have to get the 
 *            remote supported features from SDP record. If HF initiates the connection and the 
 *            AT+BRSF is not received in negotiation by HFG, HFG shall do SDP query to get remote 
 *       supported features.
 *
 * SET : Negotiation is completed.
 * UNSET : Negotiation is completed yet.
 */
#define CHANNEL_FLAG_FEATURES_SENT       0x0020
/* DESC : Indicate if the sniff timer is started or not.
 *
 * SET : Sniff timer is started.
 * UNSET : Sniff timer is not started.
 */
#define CHANNEL_FLAG_SNIFFING            0x0040

/*************************************************************
*  Definition of current SDP query status (Value of HfgChannel.queryFlags)
**************************************************************/
/* DESC : Indicate if Protocol desciptor is returned through the callback.
 *
 * SET : Protocol desciptor is returned.
 * UNSET : Protocol desciptor is not returned.
 */
#define SDP_QUERY_FLAG_PROTOCOL               0x01
/* DESC : Indicate if Profile desciptor is returned through the callback.
 *
 * SET : Profile desciptor is returned.
 * UNSET : Profile desciptor is not returned.
 */
#define SDP_QUERY_FLAG_PROFILE                0x02
/* DESC : Indicate if HSP remote audio vol control fkag is returned through the callback.
 *
 * SET : HSP remote audio vol control is returned.
 * UNSET : HSP remote audio vol control is not returned.
 */
#define SDP_QUERY_FLAG_VOL_CONTROL            0x04
/* DESC : Indicate if HF supported feature is returned through the callback.
 *
 * SET : HF supported feature is returned.
 * UNSET : HF supported feature is not returned.
 */
#define SDP_QUERY_FLAG_FEATURES               0x08
/* DESC : Indicate if service class id list is returned through the callback.
 *
 * SET : service class id list is returned.
 * UNSET : service class id list is not returned.
 */
#define SDP_QUERY_FLAG_SERVICE_CLASS_ID_LIST  0x10

/* DESC : Use MSB to indicate if the SDP query is ongoing, so that we know
 *       if HFG has to wait SDP response before disconnecting.
 *
 * SET : SDP query is ongoing.
 * UNSET : No SDP query is ongoing.
 */
#define SDP_QUERY_FLAG_ONGOING                0x80


/* DESC : Combination of all attributes queried by HSG. Used to check if all 
 *            attributes are returned.
 */
#define SDP_QUERY_ALL_HS_FLAGS          (SDP_QUERY_FLAG_PROTOCOL|SDP_QUERY_FLAG_PROFILE)

/* DESC : Combination of all attributes queried by HFG. Used to check if all 
 *            attributes are returned.
 */
#define SDP_QUERY_ALL_HF_FLAGS          (SDP_QUERY_FLAG_PROTOCOL | SDP_QUERY_FLAG_PROFILE | SDP_QUERY_FLAG_FEATURES)

/** Indicators */
#define AT_IND_CALL             0
#define AT_IND_SIGNAL           1
#define AT_IND_SERVICE          2
#define AT_IND_ROAM             3
#define AT_IND_SMS              4
#define AT_IND_BATTCHG          5
#define AT_IND_SETUP            6
#define AT_IND_CALLHELD         7

/* Unknown AT data received */
#define AT_UNKNOWN 0xFF


/***********************************************
* extern global variable
***********************************************/
extern const U8 g_indicatorRange[NUM_OF_HFG_IND][3];
extern const char *g_indicators[NUM_OF_HFG_IND][2];

/* State machine function prototype */                     
typedef void (*HfgState)(HfgChannel *Channel, U8 Event, AtCommands *Command);

/* DESC : Get current state. 
 *
 * Parameter : 
 *      HfgChannel* c : Pointer to the registered HFG channel.
 *
 * Return (U8) : Current state of the channel.
 */
#define HfgGetState(c)      (c->state & 0x0F)
/* DESC : Get current Substate. 
 *
 * Parameter : 
 *      HfgChannel* c : Pointer to the registered HFG channel.
 *
 * Return (U8) : Current substate of the channel.
 */
#define HfgGetSubstate(c)       (c->state)

/* DESC : Flush all waited responses in response queue
 *
 * Parameter : 
 *      HfgChannel* c : Pointer to the registered HFG channel.
 *
 * Return (BtStatus) : BT_STATUS_SUCCESS disconnect successfully completed
 */
BtStatus HfgHfgFlushRespQueue(HfgChannel *Channel);

/* DESC : Run disconnect procedure
 *
 * Parameter : 
 *      HfgChannel* c : Pointer to the registered HFG channel.
 *
 * Return (BtStatus) : BT_STATUS_PENDING Pending for some protocol operation
 *                  BT_STATUS_SUCCESS disconnect successfully completed
 */
BtStatus HfgDisconnecting(HfgChannel *Channel);

/* DESC : Change current Substate to new state
 *
 * Parameter : 
 *      HfgChannel* c : Pointer to the registered HFG channel.
 *      U8 newState : new substate.
 *
 * Return (void)
 */
void HfgChangeSubstate(HfgChannel *Channel, U8 newState);
BtStatus HfgRegisterServerChannel(HfgChannel *Channel);
BtStatus HfgDeregisterServerChannel(HfgChannel *Channel);

/* DESC : Disassociate ACL link.
 *
 * FLOW : Disassociate ACL link.
 *
 * Parameter : 
 *      HfgChannel* c : Pointer to the registered HFG channel.
 *
 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully disassociated.
 */
BtStatus HfgCloseChannel(HfgChannel *Channel);
BtStatus HfgOpenChannel(HfgChannel *Channel, BD_ADDR *Addr);
BtStatus HfgParseRfcommData(HfgChannel *channel, RfCallbackParms *parms, U16 *offset, AtCommands *atParms);
void HfgAppCallback(HfgChannel *Channel, HfgEvent Event, BtStatus Status, U32 data);
HfgChannel * HfgFindChannel(BtRemoteDevice *remDev);
HfgChannel * HfgFindRegisteredChannel(RfChannel *rfChannel);
RfChannel *HfgAllocRfChannel(void);
void HfgFreeRfChannel(RfChannel *rfChannel);
HfgChannel * HfgGetClosedChannel(void);
BtStatus HfgOpenRfChannel(HfgChannel *Channel);

BtStatus HfgDecodeCharset(HfgChannel *Channel, const char *charset, HfgCHarsetType *charsetType);
BtStatus HfgSetupCharsetTestRsp(HfgChannel *Channel);
const char* HfgGetCharsetDesc(HfgChannel *Channel, HfgCHarsetType charset);

HfgResponse *HfgAllocResponse(void);
void HfgFreeResponse(HfgResponse* rsp);

/* SDP utility functions */
BtStatus HfgInitSDPDB(void);
BtStatus HfgCloseSDPDB(void);
BtStatus HfgRegisterSdpServices(BOOL bHeadset);
BtStatus HfgDeregisterSdpServices(BOOL bHeadset);
BtStatus HfgStartServiceQuery(HfgChannel *Channel, SdpQueryMode mode);
BtStatus HfgVerifySdpQueryRsp(HfgChannel *Channel, SdpQueryToken *token);
void HfgReportPacketCompleted(HfgChannel *Channel, BtStatus status);
void HfgReportServiceConnected(HfgChannel *Channel);
BOOL HfgCheckIndUpdate(HfgChannel *Channel, HfgIndicator Ind, U8 Value);
void HfgSetupIndicatorTestRsp(HfgChannel *Channel);
void HfgSetupIndicatorReadRsp(HfgChannel *Channel);
void HfgSetupCallHoldReadRsp(HfgChannel *Channel);
void HfgSetupErrorRsp(HfgChannel *Channel);
void HfgSendOK(HfgChannel *Channel);

/* Handsfree functions */
BtStatus HfgCreateAclLink(BD_ADDR *Addr, CmgrHandler *Handler);
void HfgRfCallback(RfChannel *Channel, RfCallbackParms *Parms);
void HfgServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
void HfgInitStateMachine(void);
BtStatus HfgEnableSniffTimerMode(CmgrHandler *Handler) ;
BtStatus HfgDisableSniffTimerMode(CmgrHandler *Handler) ;
void HfgCheckNegotiateDone(HfgChannel *Channel);
void HfgReportHfFeatures(HfgChannel *Channel);

/* At send functions */
BtStatus sendResults(HfgChannel *Channel, AtResults *Results);
BtStatus AtSendResults(HfgChannel *Channel, AtResults *Results, BOOL bAutoOK);
BtStatus AtSendRfPacket(HfgChannel *Channel, BtPacket *Packet);
BtStatus AtSendComplete(HfgChannel *Channel, BtPacket *Packet);
AtStatus AtParseCommand(HfgChannel *Channel, U8 *input, 
                    U16 inputLen, AtCommands *Command);


const char* HfgGetModelID(HfgChannel *Channel);
const char* HfgGetManufactureID(HfgChannel *Channel);

#ifdef __HFG_BLOCK_SIMULTANEOUS_CONNECTION__
U32 HfgCheckSuspend(void);
#endif

/* Codec negotiation functions */
void HfgSetCodecState(HfgChannel *Channel, int state);
void HfgResetCodecState(HfgChannel *Channel);
BOOL HfgIsInCodecSelecting(HfgChannel *Channel);
BOOL HfgIsInCodecDecided(HfgChannel *Channel);
BOOL HfgCheckSelectedCodec(HfgChannel *Channel, HfgCodec codecId);
void HfgSetSelectedCodec(HfgChannel *Channel, HfgCodec codecId);
BOOL HfgCheckTargetCodec(HfgChannel *Channel, HfgCodec codecId);
void HfgSetTargetCodec(HfgChannel *Channel, HfgCodec codecId);
U32 HfgGetAvailCodecMask(HfgCodec codecId);
BOOL HfgCheckAvailCodec(HfgChannel *Channel, HfgCodec codecId);
void HfgSetAvailCodec(HfgChannel *Channel, HfgCodec codecId);
void HfgClearAvailCodec(HfgChannel *Channel);
void HfgAddCodecRetryCount(HfgChannel *Channel);
void HfgResetCodecRetryCount(HfgChannel *Channel);
void HfgConfigureSpeechChannel(HfgCodec codecId);
BOOL HfgCodecNegotiation(HfgChannel *Channel);
BtStatus HfgCodecConnection(HfgChannel *Channel);
#endif /* __HFGI_H_ */

