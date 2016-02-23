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

#ifndef __MEI_H
#define __MEI_H
/****************************************************************************
 *
 * File:
 *     $Workfile:mei.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     Management Enity.
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

#include "me.h"
#include "sec.h"
#include "sys/mexp.h"
#include "utils.h"
#include "hcitrans.h"
#include "sys/hci.h"
#include "sys/l2capxp.h"
#include "bt_feature.h"

#if NUM_SCO_CONNS > 0
#include "mesco.h"
#endif 
#if NUM_KNOWN_DEVICES > 0
#include "medev.h"
#endif 

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtOpMask is used to indicate which operations are in progress.
 */
typedef U16 BtOpMask;   /* Used internally */

#define BOM_INQUIRY            0x0001
#define BOM_CANCEL_INQUIRY     0x0002
#define BOM_ACCESS_MODE_AUTO   0x0008
#define BOM_HCICOMMAND_BUSY    0x0010
#define BOM_INITRADIO_BUSY     0x0020

/*---------------------------------------------------------------------------
 *
 * Authenticate State of a Remote Device (BtAuthState). The typedef is in 
 * me.h
 */
#define BAS_NOT_AUTHENTICATED  0x00
#define BAS_START_AUTHENTICATE 0x01
#define BAS_WAITING_KEY_REQ    0x02
#define BAS_SENDING_KEY        0x03
#define BAS_WAITING_FOR_PIN    0x04
#define BAS_WAITING_FOR_PIN_R  0x05
#define BAS_WAITING_FOR_KEY    0x06
#define BAS_WAITING_FOR_KEY_R  0x07
#define BAS_AUTHENTICATED      0x08
#define BAS_CANCEL_AUTHENTICATE 0x09

/*---------------------------------------------------------------------------
 *
 * Authorize State of a Remote Device (BtAuthorizeState). The typedef is in 
 * me.h
 */
#define BAS_NOT_AUTHORIZED     0x00
#define BAS_START_AUTHORIZE    0x01
#define BAS_AUTHORIZED         0x02
#define BAS_AUTHORIZED_SERVICE 0x03

#define BIO_NOT_REQUESTD       0x00
#define BIO_REQUESTD_REMOTE    0x01
#define BIO_RSPONSE_REMOTE     0x02
#define BIO_REQUESTD_LOCAL     0x04
#define BIO_RESPONSE_LOCAL     0x08

/*---------------------------------------------------------------------------
 *
 * Encyrption State of a Remote Device (BtEncryptState). The typedef is in 
 * me.h
 */
#define BES_NOT_ENCRYPTED      0x00
#define BES_START_ENCRYPT      0x01
#define BES_ENCRYPTED          0x02
#define BES_END_ENCRYPT        0x03

#define BKS_NOT_REAUTHENTICATED      0x00
#define BKS_REAUTHENTICATED          0x01

/*---------------------------------------------------------------------------
 *
 * Security Access State is used to keep track of the access procedure. The
 * typedef is in me.h.
 */

#define BAS_NO_ACCESS          0x00
#define BAS_AUTH_COMPLETE      0x01
#define BAS_AUTHORIZE_COMPLETE 0x02
#define BAS_ENCRYPT_COMPLETE   0x04

/*---------------------------------------------------------------------------
 *
 * Security Mode state is used to keep track of the security state. 
 */

typedef U8 BtSecurityModeState;

#define BSMS_LEVEL2       0x00
#define BSMS_START_LEVEL3 0x01
#define BSMS_LEVEL3       0x02
#define BSMS_END_LEVEL3   0x03

/*---------------------------------------------------------------------------
 *
 * Link Receive Buffer State (BtLinkRxState) is used to track the L2CAP
 * receive buffer state. The typedef is in me.h.
 */

#define BRXS_COMPLETE      0x00
#define BRXS_COPYING       0x01
#define BRXS_FORWARDING    0x02
#define BRXS_FLUSH         0x03
#define BRXS_BROADCAST     0x04
#define BRXS_DISCONNECTING 0x05

/*---------------------------------------------------------------------------
 *
 * Values used for the Scan_Enable parameter of the Write_Scan_Enable command.
 */
#define BSE_INQUIRY    0x01
#define BSE_PAGE       0x02

/*---------------------------------------------------------------------------
 *
 * Values used in flags field of MeCommandToken.
 */
#define MCTF_NEED_CHECK  0x01
#define MCTF_ASYNC       0x02

/*---------------------------------------------------------------------------
 *
 * MeTaskHandler is a pointer to a function to handle the HCI event for a
 * given task.
 */
typedef void (*MeTaskHandler) (U8 event, U8 len, U8 *data);

/*---------------------------------------------------------------------------
 *
 * MeOpHandler is a pointer to a function to handle events for a
 * given operation.
 */
typedef void (*MeOpHandler) (BtOpEvent event);

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtInquiryOp is used to hold an inquiry operation on a queue. 
 */
typedef struct
{
    BtOperation op;
    BtIac lap;
    U8 len; /* Duration of Inquiry */
    U8 max; /* Maximum number of responses */
} BtInquiryOp;

/*---------------------------------------------------------------------------
 *
 * BtInquiryTxPwrOp is used to hold an Inquiry Tx Power operation on a queue. 
 */
typedef struct
{
    BtOperation op;
    S8 pwr; /* Inquiry Tx power */
} BtInquiryTxPwrOp;

/*---------------------------------------------------------------------------
 *
 * BtScanOp is used to hold an inquiry or page scan operation on a queue. 
 */
typedef struct
{
    BtOperation op; /* This must be the first item */
    BtAccessibleMode mode;
} BtScanOp;

/*---------------------------------------------------------------------------
 *
 * BtSecurityOp is used to deal with asynchronous security requests. 
 */
typedef struct
{
    BtOperation op;
    BtRemoteDevice *remDev;
    BD_ADDR bdAddr;
    U8 pin[16];
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    BOOL userConfirm;
    U8 c[16];
    U8 r[16];
#endif 
    /* Extra data */
    union
    {
        U8 pLen;
        BtEncryptMode eMode;
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        BOOL ioCapability;
    #endif 
        struct
        {
            BtSecurityMode mode;
            BOOL encrypt;
        } secMode;
    } x;

} BtSecurityOp;

#if NUM_BT_DEVICES == 1
/*
 * Add one more BtSecurityOp if NUM_BT_DEVICES == 1.
 * Some radios require at least two.
 */
#define NUM_SEC_OPS     2
#else /* NUM_BT_DEVICES == 1 */ 
#define NUM_SEC_OPS     NUM_BT_DEVICES
#endif /* NUM_BT_DEVICES == 1 */ 

/*---------------------------------------------------------------------------
 *
 * BtConRejectOp is used to reject ACL and SCO connections. 
 */
typedef struct
{
    BtOperation op;
    BD_ADDR bdAddr;
    U8 rejectReason;
} BtConRejectOp;

/*---------------------------------------------------------------------------
 *
 * BtMeContext contains all the data used used internally by the
 * Management Enity.
 */

typedef struct
{
    ListEntry regList;      /* List of registered global handlers */
    ListEntry opList;       /* Operation queue */
    ListEntry secList;      /* Security record list */
    ListEntry cmdList;      /* List of pending commands */
    ListEntry pendList;     /* Pending operation list */
    BtEvent btEvent;        /* Bluetooth event */
    BtOpMask opMask;        /* Indicates which operations are in progress */

    BtOperation *curOp; /* Current operation */

    HciCommand hciGenCmd;       /* For sending general HCI commands */
    MeTaskHandler taskHandler;  /* Handle HCI events */
    U8 taskEvent;               /* Event expected for taskHandler */
    MeOpHandler opHandler;      /* Handle operation events */

    BtHandler *authorizeHandler;
    BtHandler *secModeHandler;

    /* Handler for the pairing operation */
    BtHandler *pairingHandler;

    /* Incoming connection fields */
    BtHandler *acceptHandler;
    U32 command_tick;
    U8 stackState;
    U8 opState;
    U8 taskState;
    U16 conn_acceptable_timeout;
#if NUM_SCO_CONNS > 0
    BtScoConnect scoTable[NUM_SCO_CONNS];
    BtScoAudioSettings vSettings;
#endif /* NUM_SCO_CONNS > 0 */
    EvmTimer encryptChipTimer;
    U8 *BT_Key;
    /* Device table */
    BtRemoteDevice devTable[NUM_BT_DEVICES];
#ifdef __BT_3_0_HS__
    BtOperation readDataBlockSize;
    BtDisconnectAMPCommand disconnectAMPCommand[NUM_BT_DEVICES];
#endif
    BtSecurityOp secOpTable[NUM_SEC_OPS];
    U8 *hciTable;
    U8 rxBuffTable[NUM_BT_DEVICES][L2CAP_MTU + 6 + L2CAP_PRELUDE_SIZE];

#if NUM_KNOWN_DEVICES > 0
    /* Non-Connected Device Selection list */
    ListEntry deviceList;
    ListEntry btDeviceFreeList;
    BtDeviceContext btDevices[NUM_KNOWN_DEVICES];
    BtHandler *dsMonitor;
    U8 allocated_number;
#endif /* NUM_KNOWN_DEVICES > 0 */ 
    U8 *BT_Sres1;
    BtConRejectOp rejectOp;

    U8 pendCons;    /* Number of outgoing and incoming connections in process */
    U8 activeCons;  /* Number of active connections */
    U8 inHoldCons;  /* Number of incoming holds */
    U8 holdCons;    /* Number of outgoing holds */
    U8 *BT_Sres2;
    /* Access Mode info */
    /* Current settings */
    BOOL isLiac;    /* TRUE if LIAC or FALSE if GIAC */
    BtAccessibleMode accModeCur;
    BtAccessibleMode accModeExpValue;
    U8 *BT_Rand;
    /* Operations */
    BtInquiryOp inquiryOp;  /* Inquiry operation */
    BtInquiryTxPwrOp inquiryTxPwrOp;  /* set inquiry tx power operation */
    BtScanOp writeScanModeOp;
    BtScanOp writeIacOp;
    BtOperation codOp;      /* Class of device operation */
    BtOperation afhOp;      /* Write AFH operation */
    BtOperation pta1Op;      /* Write PTA operation */
    BtOperation pta2Op;      /* Write PTA operation */    
    BtOperation ptaInfoOp;      /* Write PTA Info operation */
    BtOperation changeUartSpeedOp;      /* Write Change Uart Speed operation */
    BtOperation setRxRangeOp;                /* Write WIFI request RX range operation */
#ifdef __BT_SINGLE_ANTENNA__
    BtOperation setTxDefaultPowerOp;                
    BtOperation setTxUpdatePowerOp;                
#endif    

    U8          changedUartSpeed;
    BtOperation nameOp;     /* Write Local name operation */
    BtOperation policyOp;   /* Write Default Link Policy operation */
    BtOperation channelOp;  /* Write Channel Classification (AFH) operation */
    BtOperation loopbackOp; /* Loop back mode operation */
    BtOperation authenChipOp; /* Loop back mode operation */
    BtOperation encrypChipOp; /* Loop back mode operation */
    U8 *BT_Keyaddr;
    BtOperation eventmaskOp;    /* Set HCI Event Mask */
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    BtOperation enabledMT66xxFlowControl;
#endif
    /* Security mode state and encryption setting */
    BtSecurityModeState secModeState;
    U8 secLevel;
    BOOL secModeEncrypt;

    BtClassOfDevice classOfDevice;
    BtClassOfDevice sdpServCoD;
    BtClassOfDevice writeCoD;

    U16 curTask;
    U8 *BT_KeyClk;
    BtLinkPolicy inAclPolicy;
    BtLinkPolicy outAclPolicy;

    /* Local radio settings */
    U16 radioPageTimeout;
    BD_ADDR bdaddr;
    const U8 *localName;
    U8 localNameLen;
    U8 btVersion;
    U8 btFeatures[8];
    U8 btChipVer;
    U8 *BT_TempKey;
    U8 btLMPVer;
    EvmTimer timer;                 /* Timer for ME_RadioShutdown */
    U16 pendingEvent;               /* Pending ME_RadioShutdown event */
    BtChannelClass channelClass;    /* Channel Classification Map for AFH */
    U8 autoMode;                    /* AFH auto mode enabled */
    U8 loopbackMode;
    BOOL commandPending;
    
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    U8 eirRecordUpdated;
    BtEIRSdpRecord eirSdpCtx[MAX_EIR_SDP_SIZE];
    BtOperation eirOp;              /* Extended Inquiry response */
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    U8 sspAuthenRequirements;
#endif 
    U8 setWifiRxRangeHigh;
    U8 setWifiRxRangeLow;
#ifdef __BT_SINGLE_ANTENNA__
    U32 setWifiTxDefaultPowerConHandle;
    U8 ucPower;
#endif

#ifdef __BT_2_1_SIMPLE_PAIRING__
    BtOperation sspDebugOp;
    U8 sspDebugMode;
#endif 
#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__    
    BtOperation dualPCMSetOp;
    U8          dualPCMSetValue[32];
    U16        dualPCMSetValue_len; 
    BtOperation dualPCMSwitchOp;
    U8          dualPCMSwitchValue[32];
    U16        dualPCMSwitchValue_len;
#endif /* __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__ */
#ifdef __BT_TEST_MODE_ENABLED__
    U8 bttm_seq;
    BtOperation bttmOp;
    BD_ADDR bttmAddr;
    U8 proprietaryFeatures[8];
#endif
    U8 btCommands[64];
    U8 btCodecs[NUM_SCO_CODECS];
} BtMeContext;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ME_AddOperation(BtOperation* op)
 *
 * Description:   Add an operation to the operation queue. 
 *
 * Parameters:    op - pointer to operation to add 
 *
 * Returns:       void 
 */
#define MeAddOperation(o) (InsertTailList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationFront(o) (InsertHeadList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationPend(o) (InsertTailList(&MEC(pendList), &((o)->op.node)))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void MeStartOperation(void)
 *
 * Description:   Add an operation to the operation queue. 
 *
 * Parameters:    none 
 *
 * Returns:       void 
 */
void MeStartOperation(void);

void MeOperationEnd(void);
void MeStartInquiry(BtInquiryOp *op);
void MeReportResults(BtEventMask mask);
void MeHandleCompleteTask(U8 event, U8 len, U8 *parm);
BtRemoteDevice *MeGetFreeRemoteDevice(void);
void MeFreeRemoteDevice(BtRemoteDevice *remDev);
void MeHandleLinkConnectOp(BtOpEvent event);
void MeHandleConnectionStatusOnly(U8 event, U8 len, U8 *parm);
void MeCallLinkHandlers(BtRemoteDevice *remDev, BtEvent *event);
BOOL MeHandleEvents(U8 event, U8 len, U8 *parm);
void MeHandleLinkConnectReq(U8 len, U8 *parm);
void MeHandleLinkAcceptOp(BtOpEvent event);
void MeHandleConnectComplete(U8 len, U8 *parm);
void MeReportLinkUp(BtRemoteDevice *remDev, BtEventType eType);
BtRemoteDevice *MeMapHciToRemoteDevice(HciHandle hciHandle);
void MeDisconnectLink(BtRemoteDevice *remDev, U8 index);
void MeHandleDisconnectComplete(U8 len, U8 *parm);
void MeHandleLinkDisconnectOp(BtOpEvent event);
void MePendingCommandHandler(EvmTimer *timer);
void MeRemovePendingRemoteOperation(BtRemoteDevice *remDev);
void MeHandleWriteScanModeOp(BtOpEvent event);
void MeHandleWriteIacOp(BtOpEvent event);

BtSecurityOp *SecBtGetFreeSecurityOp(void);
BtSecurityOp *SecBtFindSecurityOp(BtRemoteDevice *remDev);


#if BT_SECURITY == XA_ENABLED
void SecHandleLinkKeyReq(U8 len, U8 *parm);
void SecHandlePinCodeReq(U8 len, U8 *parm);
void SecHandleAuthenticateOp(BtOpEvent event);
void SecHandleLinkKeyRspOp(BtOpEvent event);
void SecHandlePinCodeRspOp(BtOpEvent event);
void SecHandlePinCodeByAddrRspOp(BtOpEvent event);

#if defined (__BT_2_1_SIMPLE_PAIRING__)
void SecHandleIoCapabilityAddrRspOp(BtOpEvent event);
void SecHandleUserConfirmByAddrRspOp(BtOpEvent event);
void SecHandleDeleteStoredLinkKeyOp(BtOpEvent event);
void SecHandleUserOobDataRspOp(BtOpEvent event);
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

void SecHandleAuthenticateComplete(U8 len, U8 *parm);
void SecHandleLinkKeyNotify(U8 len, U8 *parm);
BtStatus SecAddAuthenticateOpToQueue(BtRemoteDevice *remDev);
BtStatus SecRemoveAuthenticateOpFromQueue(BtRemoteDevice *remDev);
BtStatus SecProcessSecurityToken(BtSecurityToken *token);

#if BT_SECURITY_TIMEOUT != 0
void SecAccessTimeoutHandler(EvmTimer *timer);
#endif /* BT_SECURITY_TIMEOUT */
void SecHandleAuthorizeOp(BtOpEvent event);
void SecHandleAuthorizeRequest(BtRemoteDevice *remDev);
void SecHandleEncryptOp(BtOpEvent event);
void SecProcessAllSecurityTokens(BtRemoteDevice *remDev);
void SecHandleEncryptChange(U8 len, U8 *parm);
BtStatus SecSetSecurityMode(BtHandler *handler, BtSecurityMode mode, BOOL encrypt);
BtStatus SecSetEncryptMode(BtHandler *handler, BOOL encrypt);
void SecHandleSecurityModeOp(BtOpEvent event);
void SecReportAuthenticateResult(BtRemoteDevice *remDev);
void SecReportEncryptionResult(BtRemoteDevice *remDev, U8 mode);
void SecReportSecurityModeResult(void);

#if defined (__BT_2_1_SIMPLE_PAIRING__)
void SecHandleIoCapabilityRequest(U8 len, U8 *parm);
void SecHandleIoCapabilityResponse(U8 len, U8 *parm);
void SecHandleUserConfirmRequest(U8 len, U8 *parm);
void SecHandleUserPasskeyNotification(U8 len, U8 *parm);
void SecHandleUserKeyPress(U8 len, U8 *parm);
void SecHandleSimplePairingComplete(U8 len, U8 *parm);
void SecHandleRemoteOobDataRequest(U8 len, U8 *parm);
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
#else /* BT_SECURITY == XA_ENABLED */
void SecHandleDenyPinCodeReq(U8 len, U8 *parm);
void SecHandleDenyPinCodeRspOp(BtOpEvent event);
#endif /* BT_SECURITY == XA_ENABLED */

BtRemoteDevice *MeEnumerateRemoteDevices(I8 i);
void MeReportInqCancelComplete(void);
void MeHandleRemNameReqOp(BtOpEvent event);
MeCommandToken *MeFindMatchingToken(MeCommandToken *token, ListEntry *list);
void MeReportMeCommandComplete(MeCommandToken *token);
void MeHandleRemNameReqTimeoutNoRspCancelNameReq(EvmTimer *timer);
void MeHandleRemNameReqChipNoResponse(EvmTimer *timer);
void MeHandleRemNameReqComplete(U8 len, U8 *parm);
void MeProcessToken(MeCommandToken *token);
void MeHandleLinkRejectOp(BtOpEvent event);
void MeSuspendTransmitters(void);
void MeSuspendPacket(BtRemoteDevice * remDev, U32 time);
void MeRestartTransmitters(void);
void MeHandleHoldModeOp(BtOpEvent event);
void MeHandleModeChange(U8 len, U8 *parm);
void MeHandleSniffModeOp(BtOpEvent event);
void MeHandleParkModeOp(BtOpEvent event);
void MeHandleExitParkModeOp(BtOpEvent event);
void MeHandleExitSniffModeOp(BtOpEvent event);
void MeHandleSwitchRoleOp(BtOpEvent event);
void MeHandleRoleChange(U8 len, U8 *parm);
void MeHandleQoSSetupOp(BtOpEvent event);

#if (SPP_SCO_SUPPORT == XA_ENABLED) || (BT_READ_RSSI == XA_ENABLED)
void MeHandleReadRSSIOp(BtOpEvent event);
#endif 
void MeHandleReadWIFIRSSIOp(BtOpEvent event);
void MeHandleDiscoverRole(BtOpEvent event);
void MeHandleReadRemoteFeature(U8 len, U8 *parm);
void MeHandleReadRemoteExtFeature(U8 len, U8 *parm);
void MeHandleReadRemoteVersion(U8 len, U8 *parm);
void MeHandleReadRemoteClockOffset(U8 len, U8 *parm);

#if defined (__BT_2_1_SUPERVISION_TIMEOUT_EVENT__)
void MeHandleSupervisionTimeoutChange(U8 len, U8 *parm);
#endif 

#if defined (__BT_2_1_AUTO_FLUSH__)
void MeHandleEnhancedFlush(U8 len, U8 *parm);
#endif 

void MeWriteClassOfDevice(void);
#ifdef __BT_2_1_SIMPLE_PAIRING__
void MeWriteSSPDebug(void);
#endif

#ifdef __GAP_TESTER_
void MeDisableAFH(void);
#endif 
void MeWriteDefaultLinkPolicy(void);
void MeWriteLocalName(void);

BtStatus ME_SendHciCommand(MeCommandToken *token);
MeCommandToken *MeFindGeneralEvent(BtOpEvent event, U8 *parm);
void MeProcessGenToken(MeCommandToken *token);
void MeReportNMResults(void);
void MeHandleWriteLinkPolicyOp(BtOpEvent event);
void MeHandleChangePacketTypeOp(BtOpEvent event);
void MeHandleWriteSupervisionTimeoutOp(BtOpEvent event);

#if defined (__BT_2_1_AUTO_FLUSH__)
void MeHandleWriteAutomaticallyFlushOp(BtOpEvent event);
#endif 

#if defined (__BT_2_1_SNIFF_SUBRATING__)
void MeHandleWriteSniffSubratingOp(BtOpEvent event);
#endif 

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
void MeHandleWriteEirOp(BtOpEvent event);
#endif 

void MeHandleReadRemoteFeatureTypeOp(BtOpEvent event);
void MeHandleReadRemoteExtFeatureTypeOp(BtOpEvent event);
void MeHandleReadRemoteVersionTypeOp(BtOpEvent event);
void MeHandleReadRemoteClockOffsetTypeOp(BtOpEvent event);
void MeHandleCancelConnectOp(BtOpEvent event);
void MeHandleCancelRemNameOp(BtOpEvent event);
BtStatus MeCheckModeChange(BtRemoteDevice *remDev, BtOpType bop);
void MeHandleDataReqWhileNotActive(BtRemoteDevice *remDev);
U8 MeSetBits(U8 byte, U8 bit, U8 n, BOOL state);

#define MeSetBits(a, b, n, u) ((a & ((~0 << (b)) | (~(~0 << (b - n))))) |  \
                                   (((u ? 0xff : 0x00) & ~(~0 << n)) << (b - n)));

void MeDevEventHandler(void);
void MeDevInquiryStart(void);

#if NUM_SCO_CONNS > 0
void ScoHandleLinkAcceptOp(BtOpEvent event);
BtScoConnect *ScoMapHciToConnect(HciHandle hcihandle);
void MeScoDisconnectLink(BtScoConnect *scoConnect);
void ScoHandleLinkRejectOp(BtOpEvent event);
void ScoHandleVoiceSettingsOp(BtOpEvent event);
void ScoHandleConnectComplete(U8 len, U8 *parm);
void ScoHandleConnChanged(U8 len, U8 *parm);
void ScoHandleLinkSetupOp(BtOpEvent event);
void ScoHandleLinkDisconnectOp(BtOpEvent event);
BOOL ScoHandleLinkConnectReq(U8 len, U8 *parm);
BOOL ScoHandleDisconnectComplete(U8 len, U8 *parm);
void ScoHandleDisconnectAcl(BtRemoteDevice *remDev);
void ScoReportScoLinkUp(BtScoConnect *scocon, BtEventType evnt);
BtScoConnect *ScoGetFreeConnect(void);

#if BT_SCO_HCI_DATA == XA_ENABLED
void SCO_Init(void);
BtStatus Sco_Send(BtScoConnect *scocon, BtPacket *Packet);
void ScoDataCallback(U8 event, HciCallbackParms *parms);
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

#endif /* NUM_SCO_CONNS > 0 */

U8 meUtilWriteEirName(U8 *buf, U8 bufLen, const U8 *name, U8 nameLen);
#ifdef __BT_4_0_BLE__
U8 meUtilWriteEirBLEServiceList(U8 *buf, U8 bufLen);
#endif

#endif /* __MEI_H */ 

