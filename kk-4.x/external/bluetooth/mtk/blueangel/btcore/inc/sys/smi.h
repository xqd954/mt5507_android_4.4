/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * smi.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy Security Manager Protocol internal header
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef __SMI_H
#define __SMI_H

#define SM_L2CAP_CID 0x0006
#define SM_L2CAP_MTU 23
#define SM_PASSKEY_SIZE 6

#define SM_MAX_PKT_SIZE 23
#define SM_MAX_PKT_NUM  5
#define SM_MAX_SIGN_QUEUE_SIZE 3
#define SM_REQUEST_TIMEOUT  30000 /* 30s */

#define SMC_PAIR_REQ       0x01
#define SMC_PAIR_RSP       0x02
#define SMC_PAIR_CONFIRM   0x03
#define SMC_PAIR_RANDOM    0x04
#define SMC_PAIR_FAILED    0x05
#define SMC_ENCRYPT_INFO   0x06
#define SMC_MASTER_IDEN    0x07
#define SMC_IDEN_INFO      0x08
#define SMC_IDEN_ADDR_INFO 0x09
#define SMC_SIGN_INFO      0x0A
#define SMC_SECURITY_REQ   0x0B

typedef enum
{
    SM_MODEL_JUST_WORK,        /* 0x00 */
    SM_MODEL_PASSKEY_ENTRY,    /* 0x01 */
    SM_MODEL_OUT_OF_BAND       /* 0x02 */
} SmPairModel;

typedef enum
{
    SM_PASSKEY_ROLE_INPUT,     /* 0x00 */
    SM_PASSKEY_ROLE_DISPLAY   /* 0x01 */
} SmPasskeyEntryRole;

typedef enum
{
    SM_REASON_RESERVED,                /* 0x00 */
    SM_REASON_PASSKEY_ENTRY_FAILED,    /* 0x01 */
    SM_REASON_OOB_NOT_AVAILABLE,       /* 0x02 */
    SM_REASON_AUTH_REQUIREMENT,        /* 0x03 */
    SM_REASON_CONFIRM_VALUE_FAILED,    /* 0x04 */
    SM_REASON_PAIR_NOT_SUPPORTED,      /* 0x05 */
    SM_REASON_ENCRYPT_KEY_SIZE,        /* 0x06 */
    SM_REASON_COMMAND_NOT_SUPPORTED,   /* 0x07 */
    SM_REASON_UNSPECIFIED,             /* 0x08 */
    SM_REASON_REPEATED_ATTEMPTS,       /* 0x09 */
    SM_REASON_INVALID_PARAM            /* 0x0A */
} SmPairFailReason;

typedef enum
{
    SM_KEY_DIST_NONE = 0x0,
    SM_KEY_DIST_LTK = 0x01,
    SM_KEY_DIST_IRK = 0x02,
    SM_KEY_DIST_CSRK = 0x04
} SmKeyDistribute;

typedef enum
{
    SM_VALUE_DIST_MCONFIRM = 0x01,
    SM_VALUE_DIST_SCONFIRM = 0x02,
    SM_VALUE_DIST_MRAND = 0x04,
    SM_VALUE_DIST_SRAND = 0x08,
} SmValueDistribute;

typedef struct
{
    U8 code;
    U8 IOCapability;  /* SmIOCapability */
    U8 OOBFlag;       /* SmOOBFlag */
    U8 authReq;
    U8 maxEncryptKeySize;
    U8 iKeyDist;      /* SmKeyDistribute */
    U8 rKeyDist;      /* SmKeyDistribute */
} SmPairReq;

typedef enum
{
    SM_SESSION_NULL,              /* 0 */
    SM_SESSION_INIT,              /* 1 */
        
    SM_PHASE1_PAIR_REQ_SEND,      /* 2 */
    SM_PHASE1_PAIR_REQ_RECEIVE,   /* 3 */
    SM_PHASE1_PAIR_RSP_RECEIVE,   /* 4 */
    SM_PHASE1_PAIR_RSP_SEND,      /* 5 */
    SM_PHASE1_USER_PAIR,          /* 6 */
    
    SM_PHASE2_RAND_GEN,           /* 7 */
    SM_PHASE2_CONFIRM_CAL,        /* 8 */
    SM_PHASE2_CONFIRM_WAIT,       /* 9 */
    SM_PHASE2_CONFIRM_SEND,       /* 10 */
    SM_PHASE2_CONFIRM_RECEIVE,    /* 11 */
    SM_PHASE2_RAND_SEND,          /* 12 */
    SM_PHASE2_RAND_RECEIVE,       /* 13 */
    SM_PHASE2_CONFIRM_VERIFY,     /* 14 */
    SM_PHASE2_CALC_STK,           /* 15 */
    SM_PHASE2_PAIR_DONE,          /* 16 */
    SM_PHASE2_ENCRYPT,            /* 17 */
    
    SM_PHASE3_ENCRYPTED,          /* 18 */
    SM_PHASE3_KEY_RECEIVE,        /* 19 */
    SM_PHASE3_KEY_DIST,           /* 20 */
    SM_PHASE3_DONE                /* 21 */
} BtSMState;

typedef enum
{
    SM_GC_STATE_NULL,
    SM_GC_STATE_SEND_PAIR_FAIL,
    SM_GC_STATE_DISC,
    SM_GC_STATE_RESET_CNTX
} BtSMGCState;

typedef enum
{
    SM_CMAC_STATE_NULL,
    SM_CMAC_STATE_INIT,
    SM_CMAC_STATE_GEN_SUBKEY_1,
    SM_CMAC_STATE_GEN_SUBKEY_2,
    SM_CMAC_STATE_BLOCK_CIPHER,
    SM_CMAC_STATE_BLOCK_CIPHER_LOOP,
    SM_CMAC_STATE_DONE
} BtSMCmacState;

typedef enum
{
    SM_PAIR_TYPE_BOND,
    SM_PAIR_TYPE_PAIR
} BtSMPairType;


/*
 * --------------------------------------------------------------------------
 * Command Packet
 *
 * Used by SM for sending command packets. A combination of a BtPacket
 * and the workspace necessary for building commands.
 */
typedef struct _SmCmdPacket
{
    BtPacket packet;
    U8 pdu[SM_MAX_PKT_SIZE];
} SmCmdPacket;

typedef struct _SmpairingInfo
{
    SmIOCapability ioCapability;
    SmPairModel model;
    SmPasskeyEntryRole role;
        
    SmKeyDistribute initKeyDist;
    SmKeyDistribute respKeyDist;
    U8 encKeySize;
    
    U8 TK[16];
    U8 preq[7];
    U8 pres[7];

    /* phase 2 */
    SmValueDistribute valueDist;
    U8 mRand[SM_RANDOM_VALUE_LENGTH];
    U8 sRand[SM_RANDOM_VALUE_LENGTH];
    U8 mConfirm[SM_CONFIRM_VALUE_LENGTH];
    U8 sConfirm[SM_CONFIRM_VALUE_LENGTH];
    U8 STK[SM_STK_VALUE_LENGTH];

    /* phase 3 */
    BtDeviceRecord record;
} SmPairInfo;

typedef struct _SmSession
{
    BtSMPairType type;
    BtDeviceContext *bdc;
    BtHandler smHandler;  /* Current session FSM */
    BtSMState smState;    /* Current session state */
    
    SmPairAttribute pairAttr;
    SmPairInfo pairInfo;
    EvmTimer smTimer;     /* 30s SMP timer */

    /* Deinit */
    BOOL smpFailSent;
    BtSMGCState smGCState;    /* Garbage collection procedure list */
    BtStatus pairStatus;
    EvmTimer smDiscTimer; /* SMP timer to wait for data transmission */
    U8 smDiscCount;  /* SMP timer count to wait for data transmission */
    U8 *service_search;
    U8 service_no;
} SmSession;

typedef struct
{
    ListEntry node;

    /* Input */
    U8 k[16];
    U8 *m;
    U32 mLen;
    U32 tLen;
    BD_ADDR remoteAddr;
    BtCallBack callback;
} SmSignData;

typedef struct
{
    ListEntry node;

    U32 bufLen;
    U8 *buf;
    U8 mac[SM_MAC_LENGTH];
    BD_ADDR remoteAddr;
    BtCallBack callback;
} SmSignVerifyData;

typedef struct
{
    ListEntry signQueue;
    SmSignData signData[SM_MAX_SIGN_QUEUE_SIZE];

    ListEntry verifyQueue;
    SmSignVerifyData verifyData[SM_MAX_SIGN_QUEUE_SIZE];

    /* state */
    BtSMCmacState state;
    U32 blockNo;
    U32 blockCount;
    U8 mLast[16];
} cmacContext;

void SMP_Init(void);
BtPacket *SM_AllocCmdPkt(void);
void SM_FreeCmdPkt(BtPacket *packet);
void SM_ResetTimer(BtRemoteDevice *remDev);

SmPairModel SM_GetPairModelByIo(SmIOCapability initiator, SmIOCapability responder);
SmPasskeyEntryRole SM_GetPasskeyRole(BtSMPairType type, SmIOCapability initiator, SmIOCapability responder);
BtStatus SM_SendPairReq(BtRemoteDevice *remDev,
                        U8 *preqData,
                        SmIOCapability ioCapability, 
                        SmOOBFlag oobFlag, 
                        SmBondingFlag bondFlag, 
                        BOOL MITMProtect, 
                        U8 maxKeySize,
                        SmKeyDistribute iKeyDist,
                        SmKeyDistribute rKeyDist);
BtStatus SM_SendPairRsp(BtRemoteDevice *remDev,
                        U8 *presData,
                        SmIOCapability ioCapability, 
                        SmOOBFlag oobFlag, 
                        SmBondingFlag bondFlag, 
                        BOOL MITMProtect, 
                        U8 maxKeySize,
                        SmKeyDistribute iKeyDist,
                        SmKeyDistribute rKeyDist);
BtStatus SM_SendPairConfirm(BtRemoteDevice *remDev, U8 *confirm);
BtStatus SM_SendPairRandom(BtRemoteDevice *remDev, U8 *random);
BtStatus SM_SendPairFailed(BtRemoteDevice *remDev, SmPairFailReason reason);
BtStatus SM_SendEncryptInfo(BtRemoteDevice *remDev, U8 *ltk);
BtStatus SM_SendMasterIdentity(BtRemoteDevice *remDev, U16 ediv, U64 rand);
BtStatus SM_SendIdentityInfo(BtRemoteDevice *remDev, U8 *irk);
BtStatus SM_SendIdentityAddrInfo(BtRemoteDevice *remDev, LeAddrType addrType, BD_ADDR bdAddr);
BtStatus SM_SendSigningInfo(BtRemoteDevice *remDev, U8 *csrk);
BtStatus SM_SendSecurityReq(BtRemoteDevice *remDev, SmBondingFlag bondFlag, BOOL MITMProtect);
void SM_util_mask_MSBs(U8 *array, int len, int masklen, U8 value);
void SM_util_128bit_XOR(U8 *out, U8 *in1, U8 *in2);
void SM_util_32bit_2_128bit(U8 *out, U32 value);
void SM_util_leftshift_1bit(U8 *in, U8 *out);
void SM_util_cmac_padding (U8 *lastb, U8 *pad, int length);
BtStatus SM_CalcPairConfirm(BtRemoteDevice *remDev, U8 *tk, U8 *rand, U8 *preq, U8 *pres, LeAddrType iat, LeAddrType rat, BD_ADDR ia, BD_ADDR ra);
BtStatus SM_CalcSTK(BtRemoteDevice *remDev, U8 *tk, U8 *srand, U8 *mrand);
BtStatus SM_Rand(BtRemoteDevice *remDev, U8 len);

#endif /* __SMI_H */
