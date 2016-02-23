/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef _PRXADAPTER_
#define _PRXADAPTER_
#include "conmgr.h"  // for transport
#include "gatt.h"

#define prx_MAX_PACKET_NUM 3

/* Remember that we invoke which lower layer's api */
#define PRX_CMGR_REG   0x01
#define PRX_GATT_REG   0x02
#define PRX_RFCOMM_REG 0x04
#define PRX_CMGR_CREATE_REG 0x08

/* A temportay file for proximity profile to use ATT fucntions */

BtStatus prx_att_register(void *channel, void *callback, void *info);
BtStatus prx_att_registerrecord(void *channel);

/* This is should be a ATT packet !*/
typedef struct
{
	ListEntry  node;
	U8 checkid; /* always 0x5a(PRX_CHECK_ID_VALUE) for check */
	U8 seqid;
	U32 cmd;
	U8 type;
	U32 value;

	void *pUser; /* pCntx */
	U8 buff[16]; /* passing read/write data to gatt layer */
    U8 size;

    // gatt
    PrxMTransport *chnl;
    U16 startHandle;
    U16 endHandle;
    BtGattOp op; /* Gatt packet */
    U8 operationSuccessFlag;
    BtGattDiscoveryPrimaryServiceByUUID parameter;
    BtGattDiscoveryCharacteristicByUUID parameter1;    
    BtGattReadCharacteristicValue parameter2;
    BtGattWriteCharValueRequest parameter3;
    BtGattWriteWithoutResponse parameter18;	
} PrxPacket;

#define MAX_PROXIMITY_DATABASE PRX_MAX_REPORTER_NUM
typedef struct
{
    BtRemoteDevice *link;
    U8 used;
    U8 immAlertLevel;
    U8 txPower;
    U8 alertLevel;
} ProximityServerInfo;


BtStatus prxm_readremotevalue(
                    void *channel,
                    void *packet, 
                    U8 seqid, 
                    U32 id, 
                    U32 type);
BtStatus prxm_writeremotevalue(
        void *channel,
        void *packet, 
        U8 seqid, 
        U32 id, 
        U32 type,
		void *value) ;
/*****************************************************/
void prx_packet_pool_init();
PrxPacket *prx_get_packet(void *user);
void prx_return_packet(PrxPacket *packet);
BtStatus prx_get_rssi(void *channel);
BtStatus prx_get_local_txpower(void *channel);

BtStatus prx_att_register(void *channel, void *callback, void *info) ;
BtStatus prx_att_registerrecord(void *channel);
BtStatus prx_att_deregister(void *channel);
BtStatus prx_att_deregisterrecord(void *channel);

BtStatus prx_att_acceptincoming(void *channel);
BtStatus prx_att_rejectincoming(void *channel);
BtStatus prx_att_disconnect_by_reporter(void *channel);
BtStatus prx_att_sendpacket(void *channel, void *packet);
BtStatus prx_att_disconnect(void *channel);
BtStatus prx_att_disconnect_by_monitor(void *channel);

BtStatus prx_att_createdatalink(void *channel, char *addr);
BtStatus prx_att_removedatalink(void *channel);
BtStatus prx_att_queryservice(void *channel);
BtStatus prx_att_stopqueryservice(void *channel);
BtStatus prx_att_connectservice(void *channel);


BtStatus prxr_readlocalvalue(void *channel, U32 id, U32 type, void *value);
BtStatus prxr_writelocalvalue(void *channel, U32 id, U32 type, void *value);
BtStatus dlight_proximity_writelocalvalue(void *channel, U32 id, U32 type, void *value);
BtStatus prxm_readremotevalue(
                    void *channel,
                    void *packet, 
                    U8 seqid, 
                    U32 id, 
                    U32 type) ;
BtStatus prxm_writeremotevalue(
        void *channel,
        void *packet, 
        U8 seqid, 
        U32 id, 
        U32 type,
		void *value);
/*****************************************************/


/***** Packet *****/
#define ATT_PACKETPOOL_INIT   prx_packet_pool_init
#define ATT_GETPACKET         prx_get_packet
#define ATT_RETURNPACKET      prx_return_packet
#define ATT_GETRSSI           prx_get_rssi
#define ATT_GETTXPOWER        prx_get_local_txpower

/***** ATT connection functions *****/
// sync
#define ATT_REGISTER          prx_att_register
#define ATT_REGISTERRECORD    prx_att_registerrecord
#define ATT_DEREGISTER        prx_att_deregister
#define ATT_DEREGISTERRECORD  prx_att_deregisterrecord
// async
#define ATT_ACCEPT            prx_att_acceptincoming
#define ATT_REJECT            prx_att_rejectincoming
#define ATT_SENDPACKET        prx_att_sendpacket
#define ATT_REPORTER_DISCONNECT  prx_att_disconnect_by_reporter
#define ATT_MONITOR_DISCONNECT   prx_att_disconnect_by_monitor

/***** ATT connection events *****/
#define ATT_EVENTPACKET

#define ATT_CREATELINK        prx_att_createdatalink
#define ATT_REMOVELINK        prx_att_removedatalink
#define ATT_QUERYSERVICE      prx_att_queryservice
#define ATT_STOPQUERYSERVICE  prx_att_stopqueryservice
#define ATT_CONNECTSERVICE    prx_att_connectservice

/***** ATT Table *****/
#define ATT_READLOCALVALUE    prxr_readlocalvalue
#define ATT_WRITELOCALVALUE   prxr_writelocalvalue

/***** ATT Table *****/
#define ATT_READREMOTEVALUE   prxm_readremotevalue
#define ATT_WRITEREMOTEVALUE  prxm_writeremotevalue
#endif
