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

#ifndef __LE_ADP_H
#define __LE_ADP_H

#include "metypes.h"
#include "sys/lei.h"

typedef enum
{
    BT_DEV_TYPE_UNKNOWN,
    BT_DEV_TYPE_LE,
    BT_DEV_TYPE_BR_EDR,
    BT_DEV_TYPE_BR_EDR_LE,
    NUM_OF_BT_DEV_TYPE,
} BtDevType;

typedef enum
{
    BT_CONN_TYPE_UNKNOWN,
    BT_CONN_TYPE_LE,
    BT_CONN_TYPE_BR_EDR,
} BtConnType;

typedef enum
{
    BEF_LE_LIMITED_DISCOVERABLE_MODE       = 0x01,
    BEF_LE_GENERAL_DISCOVERABLE_MODE       = 0x02,
    BEF_BR_EDR_NOT_SUPPORTED               = 0x04,
    BEF_SIMUL_LE_BR_EDR_CAPABLE_CONTROLLER = 0x08,
    BEF_SIMUL_LE_BR_EDR_CAPABLE_HOST       = 0x10
} LeAdFlags;

typedef enum
{
    LE_APPEARANCE_UNKNOWN                  = 0,
    LE_APPEARANCE_PHONE                    = 64,
    LE_APPEARANCE_COMPUTER                 = 128,
    LE_APPEARANCE_WATCH                    = 192,
    LE_APPEARANCE_CLOCK                    = 256,
    LE_APPEARANCE_DISPLAY                  = 320,
    LE_APPEARANCE_REMOTE_CONTROL           = 384,
    LE_APPEARANCE_EYE_GLASSES              = 448,
    LE_APPEARANCE_TAG                      = 512,
    LE_APPEARANCE_KEYRING                  = 576,
    LE_APPEARANCE_MEDIA_PLAYER             = 640,
    LE_APPEARANCE_BARCODE_SCANNER          = 704,
} LeAppearanceType;


typedef enum
{
    LE_PROFILE_NONE                        = 0,
    LE_PROFILE_ANP                         = 0x00000001,
    LE_PROFILE_BLP                         = 0x00000002,
    LE_PROFILE_FMP                         = 0x00000004,
    LE_PROFILE_HOGP                        = 0x00000008,
    LE_PROFILE_HRP                         = 0x00000010,
    LE_PROFILE_HTP                         = 0x00000020,
    LE_PROFILE_PASP                        = 0x00000040,
    LE_PROFILE_PXP                         = 0x00000080,
    LE_PROFILE_SCPP                        = 0x00000100,
    LE_PROFILE_TIP                         = 0x00000200,
    LE_PROFILE_ALL                         = 0xFFFFFFFF,
} LeProfile;

typedef struct
{
    U16 intervalMin;    /* value = (desired interval/0.625ms) */
    U16 intervalMax;    /* value = (desired interval/0.625ms) */
} BtLEAdvParm;

typedef struct
{
    U16 scanInterval;  /* value = (desired interval/0.625ms) */
    U16 scanWindow;    /* value = (desired interval/0.625ms) */
    U16 connIntervalMin;  /* value = (desired interval/1.25ms) */
    U16 connIntervalMax;  /* value = (desired interval/1.25ms) */
    BOOL privacy;
} BtLELinkParm;

typedef struct
{
    U64 random;
    U16 ediv;
    U8 ltk[16];
} BtLEEncParm;

typedef struct _BtLERecord
{
    BD_ADDR bdAddr;
    BtDevType devType;
    LeAddrType addrType;
    LeProfile profiles;
} BtLERecord;


/* ------------------------------------------------------ *
 *                       SM related definition            *
 * ------------------------------------------------------ */
#define SM_CONFIRM_VALUE_LENGTH 16
#define SM_RANDOM_VALUE_LENGTH 16
#define SM_STK_VALUE_LENGTH 16
#define SM_LTK_VALUE_LENGTH 16
#define SM_IRK_VALUE_LENGTH 16
#define SM_CSRK_VALUE_LENGTH 16
#define SM_EDIV_VALUE_LENGTH 2
#define SM_RAND_VALUE_LENGTH 8
#define SM_SIGNATURE_LENGTH 12
#define SM_MAC_LENGTH 8

typedef enum
{
    SM_KEY_LTK  = 0x01,
    SM_KEY_EDIV = 0x02,
    SM_KEY_RAND = 0x04,
    SM_KEY_IRK  = 0x08,
    SM_KEY_ADDR = 0x10,
    SM_KEY_CSRK = 0x20
} SmKeyType;

typedef enum
{
    SM_IO_CAPABILITY_DISPLAY_ONLY,        /* 0x00 */
    SM_IO_CAPABILITY_DISPLAY_YESNO,       /* 0x01 */
    SM_IO_CAPABILITY_KEYBOARD_ONLY,       /* 0x02 */
    SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT,  /* 0x03 */
    SM_IO_CAPABILITY_KEYBOARD_DISPLAY     /* 0x04 */
} SmIOCapability;

typedef enum
{
    SM_OOB_DATA_PRESENT_NO,        /* 0x00 */
    SM_OOB_DATA_PRESENT_YES        /* 0x01 */
} SmOOBFlag;

typedef enum
{
    SM_BONDING_NO,        /* 0x00 */
    SM_BONDING_YES        /* 0x01 */
} SmBondingFlag;

typedef struct
{
    BOOL mitm;
    SmOOBFlag oob;
    SmBondingFlag bond;
	U8 *service_list;
	U8 service_no;
} SmPairAttribute;
/* ------------------------------------------------------ *
 *                  LE related API prototype              *
 * ------------------------------------------------------ */
void LE_Init(void);

U8 leUtilComposeAdvertiseData(U8 *buf, U32 bufLen, LeAdFlags flags, const U8 *name, U32 nameLen);
U8 leUtilComposeScanRspData(U8 *buf, U32 bufLen, const U8 *name, U32 nameLen);
BtStatus leUtilUpdateAdvertiseData(void);
LeHciCmd *LeFindHciToken(BtOperation *op);

void LeHciSetEventMask(U8 *mask);
void LeHciReadBufferSize(void);
void LeHciReadLocalFeatures(void);
void LeHciSetRandomAddress(BD_ADDR bdAddr);
void LeHciSetAdvertiseParm(U16 intervalMin, 
                           U16 intervalMax, 
                           HciLeAdvertiseType type, 
                           LeAddrType ownAddrType, 
                           LeAddrType directAddrType, 
                           BD_ADDR bdAddr,
                           LeAdvChannelType channelMap,
                           HciLeAdvertiseFilter filter);
void LeHciReadAdvertiseTxPower(void);
void LeHciSetAdvertiseData(U8 dataLen, U8 *data);
void LeHciSetScanRspData(U8 dataLen, U8 *data);
BtStatus LeHciSetAdvertiseEnable(BOOL enable);
void LeHciSetScanParm(HciLeScanType type, U16 interval, U16 window, LeAddrType ownAddrType, HciLeScanFilter filter);
BtStatus LeHciSetScanEnable(BOOL enable, BOOL filterDup);
BtStatus LeHciCreateConnection(struct _BtRemoteDevice *remDev,
                           U16 scanInterval, 
                           U16 scanWindow, 
                           HciLeInitiatorFilter filter, 
                           LeAddrType peerAddrType, 
                           BD_ADDR *peerAddr,
                           LeAddrType ownAddrType, 
                           U16 connIntervalMin,
                           U16 connIntervalMax,
                           U16 connLatency,
                           U16 supervisionTO,
                           U16 CEMin,
                           U16 CEMax);
void LeHciCreateConnectionCancel(void);
void LeHciReadWhiteListSize(void);
void LeHciClearWhiteList(void);
void LeHciAddToWhiteList(LeAddrType addrType, BD_ADDR bdAddr);
void LeHciRemoveFromWhiteList(LeAddrType addrType, BD_ADDR bdAddr);
U8 LeHciUpdateConnection(U16 handle, U16 intervalMin, U16 intervalMax, U16 latency, U16 sTimeout, U16 ceMin, U16 ceMax);
void LeHciSetChannelMap(U8 *map);
void LeHciReadChannelMap(U16 handle);
void LeHciReadRemoteFeatures(U16 handle);
BtStatus LeHciAES128Encrypt(BtCallBack cb, U8 *key, U8 *data);
BtStatus LeHciRand(BtCallBack cb);
BtStatus LeHciStartEncryption(U16 hciHandle, U64 random, U16 ediv, U8 *ltk);
void LeHciLTKRequestReply(U16 hciHandle, U8 *ltk);
void LeHciLTKRequestNegReply(U16 hciHandle);
void LeHciReadSupportedStates(void);
void LeHandleEvents(U8 len, U8 *parm);
void LeHandleOp(BtOpEvent event);
void LeHandleSetScanParmOp(BtOpEvent event);
void LeHandleSetScanEnabledOp(BtOpEvent event);
void LeHandleConnectTimeout(EvmTimer *timer);
void LeHciReceiverTest(U8 rxFreq);
void LeHciTransmitterTest(U8 txFreq, U8 len, U8 payload);
void LeHciTestEnd(void);
void LeUpdateConnDevList(LeConnDevEvt event);
BtStatus LeAddConnDevice(LeAddrType addrType, BD_ADDR *bdAddr);
BtStatus LeRemoveConnDevice(BD_ADDR *bdAddr, LeConnDevRemoveReason reason);

/* ------------------------------------------------------ *
 *                  SM related API prototype              *
 * ------------------------------------------------------ */
BtStatus SM_PasskeyEntry(BtDeviceContext *bdc, BOOL accept, U32 passkey);
BtStatus SM_JustWorkRsp(BtDeviceContext *bdc, BOOL accept);
BtStatus SM_SetOobData(BtDeviceContext *bdc, BOOL accept, U8 *tk);
BtStatus SM_Bond(BtDeviceContext *bdc, SmPairAttribute *attr);
BtStatus SM_CancelBonding(BtDeviceContext *bdc);
void SM_CMAC(BtCallBack callback, BD_ADDR *bdAddr, U8 *k, U8 *m, U32 mLen, U32 tLen);
void SM_CMAC_Verify(BtCallBack callback, BD_ADDR *bdAddr, U8 *k, U8 *m, U32 mLen, U32 tLen, U8 *mac);
void SM_Init(void);
void SM_GenCSRK(U8 *csrk);
void SM_GenIRK(U8 *irk);

#endif /* __LE_ADP_H */
