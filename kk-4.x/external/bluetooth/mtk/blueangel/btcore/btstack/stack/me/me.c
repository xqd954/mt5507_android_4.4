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
 *     $Workfile:me.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Source code for the Bluetooth Management Entity.
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
#include "sys/hci.h"
#include "utils.h"
#include "btconfig.h"
#include "sys/l2capxp.h"
#include "sys/sdpi.h"
#include "bt_feature.h"
#include "x_file.h"
#include "bt_adp_fs.h"
#include "meamp.h"
#if RFCOMM_PROTOCOL == XA_ENABLED
#include "sys/rfcexp.h"
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#include "ddb.h"
#include "le_adp.h"
#include "a2mp.h"
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
extern void ME_AddEIRDeviceName(void);
extern BtStatus ME_StartWriteEIR(void);
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

extern void BTChangeUartRxSpeedDown(void);
extern U16 BT_XFile_EncryptionCommandOpcode(U16 opcode);
extern BtStatus HciTransmitCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail);
extern void Me_AudioInit(void);
extern void Me_AudioDeinit(void);
extern void Me_AudioRemoveOp(void);
/****************************************************************************
 *
 * Macros
 *
 ****************************************************************************/

/* Macro for accessing a Command token structure given the curOp */

#define METOKEN ((MeCommandToken*)MEC(curOp))

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
void MeRetryHciInit(void);
void MeProcessInitStatus(HciCallbackParms *parms);
void MeFailPendingCommands(void);
void MeFailCommand(BtOpType opType, void *parm);
void MeInitRadio(BtOpEvent event);
static void MeFailInProgressCommands(void);
static void HandleCompleteTaskRadioParms(U8 event, U8 len, U8 *parm);
static void HandleWriteCoDOp(BtOpEvent event);

#ifdef __GAP_TESTER_
static void MeHandleDisableAFH(BtOpEvent event);
#endif 

static void HandleWriteDefLinkPolicyOp(BtOpEvent event);
static void HandleGenCmdOp(BtOpEvent event);
static void HandleGeneralTask(U8 event, U8 len, U8 *parm);
static void ReportInquiryComplete(void);
static void HandleCancelInqOp(BtOpEvent event);
static BOOL IsMeCommandEqual(MeCommandToken *token1, MeCommandToken *token2);
static void HandleLocalNameChangeOp(BtOpEvent event);
static void MeHandleSetRxRangeOp(BtOpEvent event);
#ifdef __BT_SINGLE_ANTENNA__
static void MeHandleSetTxDefaultPowerOp(BtOpEvent event);
static void MeHandleSetTxUpdatePowerOp(BtOpEvent event);
#endif
static void HandleInquiryOp(BtOpEvent event);
static void HandleInquiryTxPwrOp(BtOpEvent event);
static void HandleInquiryComplete(U8 len, U8 *parm);
static void HandleInquiryResult(U8 len, U8 *parm, U8 event_type);
static void StartInquiryTask(void);
static void ReportCommandSent(const HciCommand *cmd);
static void MeAssignTaskHandler(U8 command_type);
static void MeHandleErrorTaskEnd(void);
#ifdef __BT_2_1_SIMPLE_PAIRING__
static void HandleWriteSSPDebug(BtOpEvent event);
#endif

#ifdef MTK_BB_TESTING
static void MeHandleWriteLoopBackMode(BtOpEvent event);
#endif /* MTK_BB_TESTING */


/* external function */
extern void BTSysAdpPowerOnCnf(BOOL reslut);
extern void BTSysAdpEnterSpeechTest(void);
extern int BtRadio_Get_Version(void);
extern void BTSysAdpRFTestResult(U8 result);
extern BOOL BTSysAdpCheckNotTestMode(void);
extern int BtRadio_Get_LMP_Version(void);
extern void MeHandleReadRemoteVersion(U8 len, U8 *parm);
extern U8 btbmUtilGetNameTokenID(MeCommandToken *nameToken);
extern void Me_HandleXFileOp1(BtOpEvent event);
extern void Me_HandleXFileOp2(BtOpEvent event);
extern void MeHandleWriteAFH(BtOpEvent event);
extern void MeHandleWritePTA1(BtOpEvent event);
extern void MeHandleWritePTA2(BtOpEvent event);
extern void MeHandleWritePTAInfo(BtOpEvent event);
extern void MeHandleWriteSendSpeed(BtOpEvent event);
extern void MeHandleReadRSSIOp(BtOpEvent event);
extern void MeHandleReadTxPowerOp(BtOpEvent event);

#if defined (__BT_MT66XX_HCI_RETRANSMISSION__)
void MeHandleWriteMT66xxFlowControlEnabled(BtOpEvent event);
void MeEnabledMT66xxFlowControlOn(void);
#endif
extern void MeHandleWriteEnableFMOverSBC(BtOpEvent event);
extern void MeHandleWriteDisableFMOverSBC(BtOpEvent event);


#ifdef __BT_3_0_HS__
extern void MeHandleCreatePhysicalLinkOp(BtOpEvent event);
extern void MeHandleAcceptPhysicalLinkOp(BtOpEvent event);
extern void MeHandleDisconnectPhysicalLinkOp(BtOpEvent event);
extern void MeHandleCreateLogicalLinkOp(BtOpEvent event);
extern void MeHandleAcceptLogicalLinkOp(BtOpEvent event);
extern void MeHandleDisconnectLogicalLinkOp(BtOpEvent event);
extern void MeHandleLogicalLinkCancelOp(BtOpEvent event);
extern void MeHandleFlowSpecModifyOp(BtOpEvent event);
extern void MeHandleReadLocalAmpInfo(BtOpEvent event);
extern void MeHandleReadLocalAmpAssoc(BtOpEvent event);
extern void MeHandleWriteRemoteAmpAssoc(BtOpEvent event);
extern void MeHandleReadDataBlockSizeOp(BtOpEvent event);
#endif

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
extern void MeHandleWritePCMSetValue(BtOpEvent event);
extern void MeHandleWritePCMSwitchValue(BtOpEvent event);
#endif /* __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__ */

#ifdef __BT_TEST_MODE_ENABLED__
void HandleTestModeEnable(BtOpEvent event);
#endif

extern U8 HCI_ReadyToSendCommand(void);
/*****************************************************************************
 * FUNCTION
 *  ME_Init
 * DESCRIPTION
 *  Initialize the Management Entity. This includes initializing
 *  the HCI, L2CAP and SDP.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 *****************************************************************************/
BOOL ME_Init(void)
{
#if (BT_SCO_HCI_DATA == XA_ENABLED) || (NUM_KNOWN_DEVICES > 0)
    I8 i;
#endif 

    /* Initialize some lists */
    InitializeListHead(&(MEC(regList)));
    InitializeListHead(&(MEC(opList)));
    InitializeListHead(&(MEC(secList)));
    InitializeListHead(&(MEC(cmdList)));
    InitializeListHead(&(MEC(pendList)));

    
#if BT_SCO_HCI_DATA == XA_ENABLED
    /* Initialize the SCO Tx queues. */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        InitializeListHead(&MEC(scoTable[i].scoTxQueue));
    }
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

#if NUM_KNOWN_DEVICES > 0
    InitializeListHead(&(MEC(deviceList)));
    InitializeListHead(&(MEC(btDeviceFreeList)));
    MEC(allocated_number) = 0;
    OS_MemSet((U8*) MEC(btDevices), 0, sizeof(MEC(btDevices)));

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    OS_MemSet((U8*) MEC(eirSdpCtx), 0, sizeof(MEC(eirSdpCtx)));
    MEC(eirRecordUpdated) = 0;
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

    for (i = 0; i < NUM_KNOWN_DEVICES; i++)
    {
        InsertTailList(&(MEC(btDeviceFreeList)), &(MEC(btDevices)[i].node));
    }
#endif /* NUM_KNOWN_DEVICES > 0 */
    OS_MemSet((U8*) MEC(devTable), 0, sizeof(MEC(devTable)));
    EvmResetTimer(&MEC(encryptChipTimer));
    MEC(encryptChipTimer).func = 0;
    MEC(encryptChipTimer).context = 0;
    MEC(rejectOp).op.opType = BOP_LINK_IN_REJECT;
    MEC(autoMode) = 0xff;
    MEC(command_tick) = OS_GetSystemTime();    
#if NUM_SCO_CONNS > 0
    MEC(vSettings) = BSAS_DEFAULT;
#endif /* NUM_SCO_CONNS > 0 */

    Me_AudioInit();
    /*
     * Reset the Bluetooth Host Controller, Link Manager and the
     * radio module.
     */
    if (L2CAP_Init() != BT_STATUS_SUCCESS)
    {
        goto failed;
    }

    if (SDP_Init() == BT_STATUS_FAILED)
    {
        goto failed;
    }

#if RFCOMM_PROTOCOL == XA_ENABLED
    if (RF_Init() == BT_STATUS_FAILED)
    {
        goto failed;
    }
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
//    BT_XFile_Init();
    btmtk_fs_init();
#ifdef __BT_3_0_HS__
    if (A2MP_Init() == BT_STATUS_FAILED)
    {
        goto failed;
    }
#endif
#ifdef __BT_4_0_BLE__
    LE_Init();
#endif

    return BT_STATUS_SUCCESS;

  failed:
    /*    Assert(0); */
    bt_trace(TRACE_GROUP_1, ME_INIT_FAILED);
    return BT_STATUS_FAILED;

}


/*****************************************************************************
 * FUNCTION
 *  ME_Deinit
 * DESCRIPTION
 *  Deinitialize the Management Entity. This includes deinitializing
 *  the HCI, L2CAP and SDP and RFCOMM.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void ME_Deinit(void)
{
    MEC(stackState) = BTSS_DEINITIALIZE;
    DDB_CloseBTDB(BTDB_BTDEV_INFO);
    Me_AudioDeinit();
    L2CAP_Deinit();
    SDP_Deinit();
#if RFCOMM_PROTOCOL == XA_ENABLED
    RF_Deinit();
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
//    BT_XFile_Deinit();
    /* Initialize list to prevent profiles clear context */
    OS_Report("Clear (regList, opList, secList, cmdList, pendList) in ME_Deinit");
    InitializeListHead(&(MEC(regList)));
    InitializeListHead(&(MEC(opList)));
    InitializeListHead(&(MEC(secList)));
    InitializeListHead(&(MEC(cmdList)));
    InitializeListHead(&(MEC(pendList)));
}


/*****************************************************************************
 * FUNCTION
 *  MeStartOperation
 * DESCRIPTION
 *  If possible start the next operation on the queue.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeStartOperation(void)
{
    if (HCC(state) == HCI_STATE_DEINIT)
    {
        if (!IsListEmpty(&MEC(opList)))
        {
            MEC(curOp) = (BtOperation*) RemoveHeadList(&MEC(opList));
            InitializeListEntry(&(MEC(curOp)->node));
            MEC(taskHandler) = 0;
            MEC(curOp) = 0;
            return;
        }
    }
    if ((MEC(stackState) == BTSS_INITIALIZED) && (MEC(curOp) == 0))
    {
        /* delay the command if hciGenCmd is in use */
        if (!IsEntryAvailable(&(MEC(hciGenCmd).node)))
        {
            bt_trace(TRACE_GROUP_1, ME_START_OPERATION_BUSY);
            MEC(opMask) |= BOM_HCICOMMAND_BUSY;
            return;
        }
        /* There is no active operation so start the next one on the queue */
        if (!IsListEmpty(&MEC(opList)))
        {
            MEC(curOp) = (BtOperation*) RemoveHeadList(&MEC(opList));
            InitializeListEntry(&(MEC(curOp)->node));

            bt_trace(TRACE_GROUP_1, ME_START_OPERATION, MEC(curOp)->opType);
            switch (MEC(curOp)->opType)
            {
                case BOP_INQUIRY:
                    /* Startup the Inquiry process */
                    MEC(opHandler) = HandleInquiryOp;
                    break;
                case BOP_INQUIRY_TX_PWR:
                    /* Startup the set inquiry tx power process */
                    MEC(opHandler) = HandleInquiryTxPwrOp;
                    break;
                case BOP_CANCEL_INQUIRY:
                    /* Start a cancel inquiry process */
                    MEC(opHandler) = HandleCancelInqOp;
                    break;
                case BOP_WRITE_ENABLE_FM_OVER_SBC:
                    MEC(opHandler) = MeHandleWriteEnableFMOverSBC;
                    break;
                case BOP_WRITE_DISABLE_FM_OVER_SBC:
                    MEC(opHandler) = MeHandleWriteDisableFMOverSBC;
                    break;

                case BOP_WRITE_SCAN_MODE:
                    MEC(opHandler) = MeHandleWriteScanModeOp;
                    break;
                case BOP_START_AUTHENTICATION_CHIP:
                    MEC(opHandler) = Me_HandleXFileOp1;
                    break;
                case BOP_START_ENCRYPTION_CHIP:
                    MEC(opHandler) = Me_HandleXFileOp2;
                    break;
                case BOP_WRITE_IAC_VALUE:
                    MEC(opHandler) = MeHandleWriteIacOp;
                    break;

                case BOP_LINK_CONNECT:
                    /* Start a link connection to a remote device */
                    MEC(opHandler) = MeHandleLinkConnectOp;
                    break;

                case BOP_LINK_ACCEPT:
                    /* Accept an incoming link connection from a remote device */
                    MEC(opHandler) = MeHandleLinkAcceptOp;
                    break;

                case BOP_LINK_REJECT:
                    /* Reject an incoming link connection from a remote device */
                    MEC(opHandler) = MeHandleLinkRejectOp;
                    break;

                case BOP_LINK_DISCONNECT:
                    /* Disconnect a link */
                    MEC(opHandler) = MeHandleLinkDisconnectOp;
                    break;

                case BOP_PIN_CODE_RSP:
                    MEC(opHandler) = SecHandlePinCodeRspOp;
                    break;
                case BOP_PIN_CODE_BY_ADDR_RSP:
                    MEC(opHandler) = SecHandlePinCodeByAddrRspOp;
                    break;
                case BOP_AUTHENTICATE:
                    MEC(opHandler) = SecHandleAuthenticateOp;
                    break;
                case BOP_WRITE_SUPERVISION_TIMEOUT:
                    MEC(opHandler) = MeHandleWriteSupervisionTimeoutOp;
                    break;
            #if defined (__BT_MT66XX_HCI_RETRANSMISSION__)                    
                case BOP_WRITE_MT66XX_FLOW_CONTROL_ENABLED:
                    MEC(opHandler) = MeHandleWriteMT66xxFlowControlEnabled;
                    break;
            #endif /* defined ((__BT_MT66XX_HCI_RETRANSMISSION__)   */
            #if defined (__BT_2_1_AUTO_FLUSH__)
                case BOP_WRITE_AUTOMATICALLY_FLUSHED_TIMEOUT:
                    MEC(opHandler) = MeHandleWriteAutomaticallyFlushOp;
                    break;
            #endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 
            #if defined (__BT_2_1_SNIFF_SUBRATING__)
                case BOP_SNIFF_SUBRATING_MODE:
                    MEC(opHandler) = MeHandleWriteSniffSubratingOp;
                    break;
            #endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 
            #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                case BOP_EXTENDED_INQUIRY_RESPONSE:
                    MEC(opHandler) = MeHandleWriteEirOp;
                    break;
            #endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
            #if defined (__BT_2_1_SIMPLE_PAIRING__)
                case BOP_WRITE_IO_CAPABILITY_RESPONSE:
                    MEC(opHandler) = SecHandleIoCapabilityAddrRspOp;
                    break;
                case BOP_USER_CONFIRM_BT_ADDR_RSP:
                    MEC(opHandler) = SecHandleUserConfirmByAddrRspOp;
                    break;
                case BOP_DELETE_STORED_LINK_KEY:
                    MEC(opHandler) = SecHandleDeleteStoredLinkKeyOp;
                    break;
                case BOP_USER_OOB_DATA_RSP:
                    MEC(opHandler) = SecHandleUserOobDataRspOp;
                    break;
            #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
                case BOP_LINK_KEY_RSP:
                    MEC(opHandler) = SecHandleLinkKeyRspOp;
                    break;

                case BOP_AUTHORIZE:
                    MEC(opHandler) = SecHandleAuthorizeOp;
                    break;

                case BOP_ENCRYPTION:
                    MEC(opHandler) = SecHandleEncryptOp;
                    break;

                case BOP_SECURITY_MODE:
                    MEC(opHandler) = SecHandleSecurityModeOp;
                    break;

                case BOP_CANCEL_AUTHORIZE:
                    MEC(opHandler) = SecHandleAuthorizeOp;
                    MEC(opHandler) (BOE_TASK_ERR);
                    return;
                    
                case BOP_REM_NAME_REQ:
                    MEC(opHandler) = MeHandleRemNameReqOp;
                    break;

                case BOP_LOCAL_NAME_CHG:
                    MEC(opHandler) = HandleLocalNameChangeOp;
                    break;

                case BOP_HOLD_MODE:
                    Assert(0);
                    break;

                case BOP_SNIFF_MODE:
                    MEC(opHandler) = MeHandleSniffModeOp;
                    break;

                case BOP_EXIT_SNIFF_MODE:
                    MEC(opHandler) = MeHandleExitSniffModeOp;
                    break;

                case BOP_EXIT_PARK_MODE:
                    MEC(opHandler) = MeHandleExitParkModeOp;
                    break;

                case BOP_QOS_SETUP:
                    MEC(opHandler) = MeHandleQoSSetupOp;
                    break;

                case BOP_SWITCH_ROLE:
                    MEC(opHandler) = MeHandleSwitchRoleOp;
                    break;
                case BOP_WRITE_COD:
                    MEC(opHandler) = HandleWriteCoDOp;
                    break;

                case BOP_WRITE_DEF_LNK_POL:
                    MEC(opHandler) = HandleWriteDefLinkPolicyOp;
                    break;

                case BOP_GEN_COMMAND:
                    MEC(opHandler) = HandleGenCmdOp;
                    break;

                case BOP_LINK_IN_REJECT:
                    /* Internally reject a link */
                    MEC(opHandler) = MeHandleLinkRejectOp;
                    break;

                case BOP_WRITE_LINK_POLICY:
                    MEC(opHandler) = MeHandleWriteLinkPolicyOp;
                    break;
                    
                case BOP_CHANGE_PACKET_TYPE:
                    MEC(opHandler) = MeHandleChangePacketTypeOp;
                    break;
                    
                case BOP_READ_REMOTE_FEATURE:
                    MEC(opHandler) = MeHandleReadRemoteFeatureTypeOp;
                    break;

                case BOP_READ_REMOTE_EXT_FEATURE:
                    MEC(opHandler) = MeHandleReadRemoteExtFeatureTypeOp;
                    break;
                    
                case BOP_READ_REMOTE_VERSION:
                    MEC(opHandler) = MeHandleReadRemoteVersionTypeOp;
                    break;

                case BOP_READ_REMOTE_CLOCK_OFFSET:
                    MEC(opHandler) = MeHandleReadRemoteClockOffsetTypeOp;
                    break;
                    
                case BOP_CANCEL_CONNECT:
                    MEC(opHandler) = MeHandleCancelConnectOp;
                    break;

                case BOP_CANCEL_REM_NAME:
                    MEC(opHandler) = MeHandleCancelRemNameOp;
                    break;
                    
                case BOP_DISCOVER_ROLE:
                    MEC(opHandler) = MeHandleDiscoverRole;
                    break;
                case BOP_WRITE_AFH:
                    MEC(opHandler) = MeHandleWriteAFH;
                    break;
                case BOP_WRITE_PTA1:
                    MEC(opHandler) = MeHandleWritePTA1;
                    break;
                case BOP_WRITE_PTA2:
                    MEC(opHandler) = MeHandleWritePTA2;
                    break;                    
                case BOP_WRITE_PTAINFO:
                    MEC(opHandler) = MeHandleWritePTAInfo;
                    break;
                case BOP_WRITE_UART_SEND_SPEED:
                    MEC(opHandler) = MeHandleWriteSendSpeed;
                    break;
            #ifdef __GAP_TESTER_
                case BOP_DISABLE_AFH:
                    MEC(opHandler) = MeHandleDisableAFH;
                    break;
            #endif /* __GAP_TESTER_ */ 

            #ifdef MTK_BB_TESTING
                case BOP_WRITE_LOCAL_LOOPBACK:
                    MEC(opHandler) = MeHandleWriteLoopBackMode;
                    break;
            #endif /* MTK_BB_TESTING */ 
                    
            #if NUM_SCO_CONNS > 0
                case BOP_SCO_SETUP:
                    /* Start a SCO link connection to a remote device */
                    MEC(opHandler) = ScoHandleLinkSetupOp;
                    break;

                case BOP_SCO_ACCEPT:
                    MEC(opHandler) = ScoHandleLinkAcceptOp;
                    break;

                case BOP_SCO_REJECT:
                    MEC(opHandler) = ScoHandleLinkRejectOp;
                    break;

                case BOP_SCO_DISCONNECT:
                    /* Disconnect a link */
                    MEC(opHandler) = ScoHandleLinkDisconnectOp;
                    break;

                case BOP_SCO_VOICESET:
                    /* SCO Voice Settings */
                    MEC(opHandler) = ScoHandleVoiceSettingsOp;
                    break;
            #endif /* NUM_SCO_CONNS > 0 */
                case BOP_READ_TX_POWER:
                    MEC(opHandler) = MeHandleReadTxPowerOp;
                    break;
                case BOP_READ_RSSI:
                    MEC(opHandler) = MeHandleReadRSSIOp;
                    break;
#ifdef __BT_3_0_HS__
                case BOP_CREATE_PHYSICAL_LINK:
                    MEC(opHandler) = MeHandleCreatePhysicalLinkOp;
                    break;
                case BOP_ACCEPT_PHYSICAL_LINK:
                    MEC(opHandler) = MeHandleAcceptPhysicalLinkOp;
                    break;
                case BOP_DISCONNECT_PHYSICAL_LINK:
                    MEC(opHandler) = MeHandleDisconnectPhysicalLinkOp;
                    break;
                case BOP_CREATE_LOGICAL_LINK:
                    MEC(opHandler) = MeHandleCreateLogicalLinkOp;
                    break;
                case BOP_ACCEPT_LOGICAL_LINK:
                    MEC(opHandler) = MeHandleAcceptLogicalLinkOp;
                    break;
                case BOP_DISCONNECT_LOGICAL_LINK:
                    MEC(opHandler) = MeHandleDisconnectLogicalLinkOp;
                    break;    
                case BOP_LOGICAL_LINK_CANCEL:
                    MEC(opHandler) = MeHandleLogicalLinkCancelOp;
                    break;
                case BOP_FLOW_SPEC_MODIFY:
                    MEC(opHandler) = MeHandleFlowSpecModifyOp;
                    break;
                case BOP_READ_LOCAL_AMP_INFO:
                    MEC(opHandler) = MeHandleReadLocalAmpInfo;
                    break;    
                case BOP_READ_LOCAL_AMP_ASSOC:
                    MEC(opHandler) = MeHandleReadLocalAmpAssoc;                    
                    break;
                case BOP_WRITE_REMOTE_AMP_ASSOC:
                    MEC(opHandler) = MeHandleWriteRemoteAmpAssoc;
                    break; 
                case BOP_READ_DATA_BLOCK_SIZE:
                    MEC(opHandler) = MeHandleReadDataBlockSizeOp;
                    break;
#endif
                case BOP_READ_WIFI_RSSI:
                    MEC(opHandler) = MeHandleReadWIFIRSSIOp;
                    break;
                case BOP_SET_RX_RANGE:
                    MEC(opHandler) = MeHandleSetRxRangeOp;
                    break;
#ifdef __BT_SINGLE_ANTENNA__
                case BOP_SET_TX_DEFAULT_POWER:
                    MEC(opHandler) = MeHandleSetTxDefaultPowerOp;
                    break;
                case BOP_SET_TX_UPDATE_POWER:
                    MEC(opHandler) = MeHandleSetTxUpdatePowerOp;
                    break;
#endif
#ifdef __BT_2_1_SIMPLE_PAIRING__
                case BOP_WRITE_SSP_DEBUG:
                    MEC(opHandler) = HandleWriteSSPDebug;
                    break;
#endif
            #ifdef __BT_4_0_BLE__
                case BOP_LE:
                    MEC(opHandler) = LeHandleOp;
                    break;
                case BOP_LE_SET_SCAN_PARM:
                    MEC(opHandler) = LeHandleSetScanParmOp;
                    break;
                case BOP_LE_SET_SCAN_ENABLED:
                    MEC(opHandler) = LeHandleSetScanEnabledOp;
                    break;
            #endif
#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
                case BOP_SET_DUAL_PCM_SETTING:
                    MEC(opHandler) = MeHandleWritePCMSetValue;
                    break;
                case BOP_SET_DUAL_PCM_SWITCH:
                    MEC(opHandler) = MeHandleWritePCMSwitchValue;
                    break;
#endif
#ifdef __BT_TEST_MODE_ENABLED__
                case BOP_TEST_MODE:
                    MEC(opHandler) = HandleTestModeEnable;
                    break;
#endif
                default:
                    Assert(0);
            }

            /* Call the handler */
            MEC(opHandler) (BOE_START);
        }
    }
    else
    {
        int opType;
        opType = (MEC(stackState) == BTSS_INITIALIZED) ? MEC(curOp)->opType : 0;
        bt_trace(TRACE_GROUP_1, ME_START_OPERATION_FAIL_STATE, MEC(stackState), opType);
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeWriteClassOfDevice
 * DESCRIPTION
 *  If one is not already started then start a Write class of
 *  device operation.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeWriteClassOfDevice(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(codOp).opType == BOP_NOP))
    {

        MEC(codOp).opType = BOP_WRITE_COD;
        if(!IsEntryAvailable(&(MEC(codOp).node)))
        {
            Assert(0);
        }
        InsertTailList(&MEC(opList), &(MEC(codOp).node));

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}


#ifdef __BT_2_1_SIMPLE_PAIRING__
/*****************************************************************************
 * FUNCTION
 *  MeWriteSSPDebug
 * DESCRIPTION
 *  If one is not already started then start a Write class of
 *  device operation.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeWriteSSPDebug(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(sspDebugOp).opType == BOP_NOP))
    {

        MEC(sspDebugOp).opType = BOP_WRITE_SSP_DEBUG;
        if(!IsEntryAvailable(&(MEC(sspDebugOp).node)))
        {
            Assert(0);
        }
        InsertTailList(&MEC(opList), &(MEC(sspDebugOp).node));

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  MeChangePacketType
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeChangePacketType(BtRemoteDevice *remDev)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->packetTypeOp.opType == BOP_NOP))
    {

        remDev->packetTypeOp.opType = BOP_CHANGE_PACKET_TYPE;
        InsertTailList(&MEC(opList), &((remDev->packetTypeOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeWriteSupervisionTimeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeWriteSupervisionTimeout(BtRemoteDevice *remDev)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->supervisionOp.opType == BOP_NOP))
    {

        remDev->supervisionOp.opType = BOP_WRITE_SUPERVISION_TIMEOUT;
        InsertTailList(&MEC(opList), &((remDev->supervisionOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}

#if defined (__BT_2_1_AUTO_FLUSH__)


/*****************************************************************************
 * FUNCTION
 *  MeWriteAutomaticallyFlushedTimeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeWriteAutomaticallyFlushedTimeout(BtRemoteDevice *remDev)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->automaticallyFlushedOp.opType == BOP_NOP))
    {

        remDev->automaticallyFlushedOp.opType = BOP_WRITE_AUTOMATICALLY_FLUSHED_TIMEOUT;
        InsertTailList(&MEC(opList), &((remDev->automaticallyFlushedOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}
#endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 


/*****************************************************************************
 * FUNCTION
 *  MeReadRemoteSupportedFeature
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReadRemoteSupportedFeature(BtRemoteDevice *remDev)
{
    OS_Report("[ME] MeReadRemoteSupportedFeature(%p)", remDev);
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->remoteFeatureOp.opType == BOP_NOP))
    {
        remDev->remoteFeatureOp.opType = BOP_READ_REMOTE_FEATURE;
        InsertTailList(&MEC(opList), &((remDev->remoteFeatureOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}

/*****************************************************************************
 * FUNCTION
 *  MeReadRemoteSupportedExtFeature
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReadRemoteSupportedExtFeature(BtRemoteDevice *remDev)
{
    OS_Report("[ME] MeReadRemoteSupportedExtFeature(%p)", remDev);
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->remoteExtFeatureOp.opType == BOP_NOP))
    {

        remDev->remoteExtFeatureOp.opType = BOP_READ_REMOTE_EXT_FEATURE;
        /* Speed to get ext feature, to prevent auth before knowning if remote support SSP or not  */
        InsertHeadList(&MEC(opList), &((remDev->remoteExtFeatureOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeReadRemoteVersion
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReadRemoteVersion(BtRemoteDevice *remDev)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->remoteVersionOp.opType == BOP_NOP))
    {

        remDev->remoteVersionOp.opType = BOP_READ_REMOTE_VERSION;
        InsertTailList(&MEC(opList), &((remDev->remoteVersionOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}

/*****************************************************************************
 * FUNCTION
 *  MeReadRemoteClockOffset
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReadRemoteClockOffset(BtRemoteDevice *remDev)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (remDev->remoteClockOffsetOp.opType == BOP_NOP))
    {
        remDev->remoteClockOffsetOp.opType = BOP_READ_REMOTE_CLOCK_OFFSET;
        InsertTailList(&MEC(opList), &((remDev->remoteClockOffsetOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}


#ifdef __GAP_TESTER_


/*****************************************************************************
 * FUNCTION
 *  MeDisableAFH
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeDisableAFH(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(codOp).opType == BOP_NOP))
    {

        MEC(codOp).opType = BOP_DISABLE_AFH;
        if(!IsEntryAvailable(&(MEC(codOp).node)))
        {
            Assert(0);
        }
        InsertTailList(&MEC(opList), &(MEC(codOp).node));

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}
#endif /* __GAP_TESTER_ */ 


/*****************************************************************************
 * FUNCTION
 *  MeWriteLocalName
 * DESCRIPTION
 *  If one is not already started then start a Local Name Change
 *  operation.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeWriteLocalName(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(nameOp).opType == BOP_NOP) &&
        IsEntryAvailable(&(MEC(nameOp).node)))
    {

        MEC(nameOp).opType = BOP_LOCAL_NAME_CHG;
        InsertTailList(&MEC(opList), &(MEC(nameOp).node));

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeWriteDefaultLinkPolicy
 * DESCRIPTION
 *  If one is not already started then start a Write Default Link
 *  policy operation.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeWriteDefaultLinkPolicy(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(policyOp).opType == BOP_NOP))
    {

        MEC(policyOp).opType = BOP_WRITE_DEF_LNK_POL;
        InsertTailList(&MEC(opList), &(MEC(policyOp).node));

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}

#ifdef MTK_BB_TESTING
/*****************************************************************************
 * FUNCTION
 *  MeEnabledLocalLoopback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeEnabledLocalLoopback(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(loopbackOp).opType == BOP_NOP))
    {

        MEC(loopbackOp).opType = BOP_WRITE_LOCAL_LOOPBACK;
        InsertTailList(&MEC(opList), &(MEC(loopbackOp).node));

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  HandleInquiryOp
 * DESCRIPTION
 *  Start an Inquiry process.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleInquiryOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the inquiry task */
            MEC(taskHandler) = MeHandleConnectionStatusOnly;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
            StartInquiryTask();
            return;

        case BOE_TASK_END:
            /* The operation is done check for errors */
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                bt_trace(TRACE_GROUP_1, ME_INQUIRY_OPERATION_COMPLETED_WITH_ERROR__xX, MEC(btEvent).errCode);
                ReportInquiryComplete();
            }
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}

/*****************************************************************************
 * FUNCTION
 *  StartInquiryTxPwrTask
 * DESCRIPTION
 *  Start the Iqnuiry tx power Task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartInquiryTxPwrTask(void)
{
    if(!(MEC(curOp) == (BtOperation*) (&(MEC(inquiryTxPwrOp)))))
    {
        Assert(0);
    }

    MEC(hciGenCmd).parms[0] = (U8)MEC(inquiryTxPwrOp).pwr;

    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
    
    MeSendHciCommand(HCC_WRITE_INQUIRY_TX_PWR, 1);
}

/*****************************************************************************
 * FUNCTION
 *  HandleInquiryTxPwrOp
 * DESCRIPTION
 *  Start an Inquiry process.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleInquiryTxPwrOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the inquiry task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StartInquiryTxPwrTask();
            return;

        case BOE_TASK_END:
            /* The operation is done check for errors */
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                OS_Report("Set inquiry tx pwr success : %u", MEC(btEvent).errCode);
                bt_trace(TRACE_GROUP_1, ME_INQUIRY_OPERATION_COMPLETED_WITH_ERROR__xX, MEC(btEvent).errCode);
            }
            break;
        case BOE_TASK_ERR:
            OS_Report("HandleInquiryTxPwrOp : Failed");
        default:
            MeHandleErrorTaskEnd();
            break;
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  HandleCancelInqOp
 * DESCRIPTION
 *  Start a Cancel Inquiry process.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleCancelInqOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }

            MeSendHciCommand(HCC_INQUIRY_CANCEL, 0);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            /* The operation is done check for errors */
            bt_trace(TRACE_GROUP_1, ME_INQUIRY_CANCEL_OPERATION_COMPLETED_ERROR__xX, MEC(btEvent).errCode);
            MeReportInqCancelComplete();
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  HandleWriteCoDOp
 * DESCRIPTION
 *  Start a Write Class of Device operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleWriteCoDOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }
            MEC(writeCoD) = MEC(classOfDevice) | MEC(sdpServCoD);
            StoreLE32(MEC(hciGenCmd).parms, MEC(writeCoD));
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 3);
            MeSendHciCommand(HCC_WRITE_CLASS_OF_DEVICE, 3);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(codOp).opType = BOP_NOP;
            if (MEC(writeCoD) != (MEC(classOfDevice) | MEC(sdpServCoD)))
            {
                MeWriteClassOfDevice();
            }
            /* Operation done. When we put in error event then report any errors */
            bt_trace(TRACE_GROUP_1, ME_WRITE_CLASS_OF_DEVICE_OPERATION_COMPLETED_ERROR__xX, MEC(btEvent).errCode);
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


#ifdef __BT_2_1_SIMPLE_PAIRING__
/*****************************************************************************
 * FUNCTION
 *  HandleWriteSSPDebug
 * DESCRIPTION
 *  Start a Write Class of Device operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleWriteSSPDebug(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }
            MEC(hciGenCmd).parms[0] = MEC(sspDebugMode);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
            MeSendHciCommand(HCC_WRITE_SSP_DEBUG_MODE, 1);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(sspDebugOp).opType = BOP_NOP;
            Report(("HandleWriteSSPDebug(): set %d end", MEC(sspDebugMode)));
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}
#endif
#ifdef __GAP_TESTER_


/*****************************************************************************
 * FUNCTION
 *  MeHandleDisableAFH
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeHandleDisableAFH(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            MEC(hciGenCmd).parms[0] = 0x00;
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
            MeSendHciCommand(HCC_WRITE_AFH_CHAN_ASSESS_MODE, 1);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(codOp).opType = BOP_NOP;
            /* Operation done. When we put in error event then report any errors */
            bt_trace(TRACE_GROUP_1, ME_WRITE_AFH_COMPLETED_ERROR__xX, MEC(btEvent).errCode);
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}
#endif /* __GAP_TESTER_ */ 

#ifdef MTK_BB_TESTING
/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteLoopBackMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeHandleWriteLoopBackMode(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MeAssignTaskHandler(HCE_COMMAND_COMPLETE);
            MEC(hciGenCmd).parms[0] = MEC(loopbackMode);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
            MeSendHciCommand(HCC_WRITE_LOOPBACK_MODE, 1);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            bt_trace(TRACE_GROUP_1, LOOPBACK_MODE_COMPLETED);
            MEC(loopbackOp).opType = BOP_NOP;
            GORMevt_HCC_LOOPBACKMODE_COMPLETE_SUCCESS(MEC(loopbackMode));
            break;
        default:
            MeHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}
#endif /* MTK_BB_LOOPBACK_MODE */ 

/*****************************************************************************
 * FUNCTION
 *  HandleLocalNameChangeOp
 * DESCRIPTION
 *  Start an Inquiry process.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleLocalNameChangeOp(BtOpEvent event)
{
//    U8 data[248];
    
    switch (event)
    {
        case BOE_START:
            MeAssignTaskHandler(HCE_COMMAND_COMPLETE);
            if (MEC(localNameLen) > 0)
            {
                OS_MemCopy(MEC(hciGenCmd).parms, MEC(localName), MEC(localNameLen) > 248 ? 248 : MEC(localNameLen));
            }
            else
            {
                MEC(hciGenCmd).parms[0] = 0;
            }

            bt_trace(TRACE_GROUP_1, MECON_STARTING_LOCAL_NAME_CHANGE_COMMAND_LEN__xD, MEC(localNameLen));
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 248);
            MeSendHciCommand(HCC_CHNG_LOCAL_NAME, 248);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(nameOp).opType = BOP_NOP;
        #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
            ME_AddEIRDeviceName();
        #endif 
        #ifdef __BT_4_0_BLE__
            leUtilUpdateAdvertiseData();
        #endif
            MEC(btEvent).eType = BTEVENT_LOCAL_NAME_CHANGE;
            MeReportNMResults();

            /* The operation is done check */
            bt_trace(TRACE_GROUP_1, ME_LOCAL_NAME_CHANGE_OPERATION_COMPLETED_STATUS__0XxX, MEC(btEvent).errCode);
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}



static void MeHandleSetRxRangeOp(BtOpEvent event)
{
    U8 *parms;
    U8 i = 0;

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            MEC(hciGenCmd).parms[0] = MEC(setWifiRxRangeLow);
            MEC(hciGenCmd).parms[1] = MEC(setWifiRxRangeHigh);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(0xfc80, 2);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
	    MEC(setRxRangeOp).opType = BOP_NOP;
#ifdef __BT_SINGLE_ANTENNA__
            BT_WMT_Send_Set_Rx_Range_Cnf();
#endif
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}


#ifdef __BT_SINGLE_ANTENNA__
static void MeHandleSetTxDefaultPowerOp(BtOpEvent event)
{
    U8 *parms;
    U8 i = 0;

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            MEC(hciGenCmd).parms[0] = MEC(ucPower);
            MEC(hciGenCmd).parms[1] = 0x80;
            MEC(hciGenCmd).parms[2] = 0x00;
            MEC(hciGenCmd).parms[3] = 0x06;
            MEC(hciGenCmd).parms[4] = 0x03;
            MEC(hciGenCmd).parms[5] = MEC(ucPower);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
            MeSendHciCommand(0xfc79, 6);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            MEC(setTxDefaultPowerOp).opType = BOP_NOP;
            BT_WMT_Send_Tx_Default_Power_Cnf();
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}

static void MeHandleSetTxUpdatePowerOp(BtOpEvent event)
{
    U8 *parms;
    U8 i = 0;

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StoreLE16(MEC(hciGenCmd).parms, MEC(setWifiTxDefaultPowerConHandle));
            MEC(hciGenCmd).parms[2] = MEC(ucPower);
            MEC(hciGenCmd).parms[3] = 0x00;
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
            MeSendHciCommand(0xfc17, 4);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            MEC(setTxUpdatePowerOp).opType = BOP_NOP;
            BT_WMT_Send_Tx_Update_Power_Cnf();
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}


#endif

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)


/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteEirOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteEirOp(BtOpEvent event)
{
    U8 eir_response[251];
    U8 offset;
    U8 index = 0;
    U8 eirRecordNum = 0;
    U8 eirWriteNum;
    U8 completed_flag;
    U8 local_name_size = 0;

    /* Get a pointer to the remote device */
    bt_trace(TRACE_GROUP_1, BTLOG_MEHANDLEWRITEEIROP);
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            OS_MemSet(eir_response, 0, sizeof(eir_response));
            /* Reserved one byte for FEC required or not */
            offset = 1;

            /* Compose the EIR headset for local name */
            bt_trace(TRACE_GROUP_1, BTLOG_MECLOCALNAMELENxD, MEC(localNameLen));
            local_name_size = MEC(localNameLen);
            if (local_name_size != 0)
            {
                if (MEC(localName)[local_name_size - 1] == 0x00)
                {
                    local_name_size--;
                }
                /* Ingnore the last 0x00 name terminate */

            }
            if (local_name_size > MTK_EXTENDED_INQUIRY_RESPONSE_MAX_NAME_SIZE)
            {
                eir_response[offset] = MTK_EXTENDED_INQUIRY_RESPONSE_MAX_NAME_SIZE + 1;
                eir_response[offset + 1] = BT_EIR_NAME_SHORTENED;
                OS_MemCopy(eir_response + offset + 2, MEC(localName), MTK_EXTENDED_INQUIRY_RESPONSE_MAX_NAME_SIZE);
                offset += MTK_EXTENDED_INQUIRY_RESPONSE_MAX_NAME_SIZE + 2;
            }
            else if (local_name_size != 0)
            {
                eir_response[offset] = local_name_size + 1;
                eir_response[offset + 1] = BT_EIR_NAME_COMPLETE;
                OS_MemCopy(eir_response + offset + 2, MEC(localName), local_name_size);
                offset += local_name_size + 2;
            }

            /* Compose the EIR headset for Service Class UUID List */
            /* Count total EIR UUID item number */
            for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
            {
                if (MEC(eirSdpCtx)[index].used != 0)
                {
                    eirRecordNum++;
                }
            }
            if ((offset + eirRecordNum * 2 + 2) > 241)
            {
                /* EIR UUID is not completed */
                completed_flag = 0;
                eirRecordNum = (241 - offset - 2) / 2;

            }
            else
            {
                /* EIR UUID is completed */
                completed_flag = 1;
            }
            bt_trace(TRACE_GROUP_1, BTLOG_EIRRECORDNUMxD, eirRecordNum);
            if (eirRecordNum != 0)
            {
                if (completed_flag == 0)
                {
                    eir_response[offset + 1] = BT_EIR_SERVICE_CLASS_16UUID_MORE;
                }
                else
                {
                    eir_response[offset + 1] = BT_EIR_SERVICE_CLASS_16UUID_COMPLETE;
                }
                eir_response[offset] = eirRecordNum * 2 + 1;
                offset += 2;
                eirWriteNum = 0;
                for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
                {
                    if (MEC(eirSdpCtx)[index].used != 0)
                    {
                        eir_response[offset] = (MEC(eirSdpCtx)[index].service_class & 0x00FF);
                        eir_response[offset + 1] = ((MEC(eirSdpCtx)[index].service_class & 0xFF00) >> 8);
                        eirWriteNum++;
                        offset += 2;
                    }
                    if (eirWriteNum == eirRecordNum)
                    {
                        break;
                    }
                }
                if (offset > 241)
                {
                    Assert(0);
                }
            }

            /* Compose the Last byte for NULL terminate */
            /* 0x00 terminate */
            offset++;
            MEC(eirRecordUpdated) = 0;
            if ((offset - 1) < 224) /* DM5 largest packet size */
            {
                eir_response[0] = 0x01; /* FEC is required */
            }
            else
            {
                eir_response[0] = 0x00; /* FEC is not required */
            }
            OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));
            OS_MemCopy(MEC(hciGenCmd).parms, eir_response, offset);
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 241);
            MeSendHciCommand(HCC_WRITE_EXT_INQUIRY_RESPONSE, 241);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(eirOp).opType = BOP_NOP;
            bt_trace(TRACE_GROUP_1, BTLOG_WRITE_EIR_RESPONSE, MEC(btEvent).errCode);
            break;
        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
    if (MEC(eirRecordUpdated) == 1)
    {
        ME_StartWriteEIR();
    }

}

#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 


/*****************************************************************************
 * FUNCTION
 *  HandleWriteDefLinkPolicyOp
 * DESCRIPTION
 *  Start a Write Class of Device operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleWriteDefLinkPolicyOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MeAssignTaskHandler(HCE_COMMAND_COMPLETE);
            StoreLE16(MEC(hciGenCmd).parms, MEC(inAclPolicy));
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(HCC_WRITE_DEFAULT_LINK_POLICY, 2);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(policyOp).opType = BOP_NOP;
            /* Operation done. */
            bt_trace(
                TRACE_GROUP_1,
                ME_WRITE_DEFAULT_LINK_POLICY_OPERATION_COMPLETED_ERROR__0XxX,
                MEC(btEvent).errCode);
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  HandleGenCmdOp
 * DESCRIPTION
 *  Perform a general HCI command.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleGenCmdOp(BtOpEvent event)
{
    BtOpType opType;

    /* Save the op type (it may change when processing token) */
    opType = MEC(curOp)->opType;

    switch (event)
    {
        case BOE_START:
            /* Put the command on the command queue */
            InsertHeadList(&MEC(cmdList), &(METOKEN->op.node));
            MEC(taskHandler) = HandleGeneralTask;
            MEC(taskEvent) = METOKEN->p.general.in.event;
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }
            bt_trace(TRACE_GROUP_1, MECON_STARTING_GENERAL_COMMAND);
            OS_MemCopy(MEC(hciGenCmd).parms, METOKEN->p.general.in.parms, METOKEN->p.general.in.parmLen);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, METOKEN->p.general.in.parmLen);
            MeSendHciCommand(
                        METOKEN->p.general.in.hciCommand,
                        METOKEN->p.general.in.parmLen);
            return;

        case BOE_CONT:
            /*
             * This event occurs when a general command has received its command status
             * and is waiting for the next event. If it is an async event then start the
             * the next operation
             */
            if (!(METOKEN->flags & MCTF_ASYNC))
            {
                return;
            }
            break;

        case BOE_TASK_END:
            /* Task is complete so report it. */
            bt_trace(TRACE_GROUP_1, ME_GENERAL_COMMAND_COMPLETED_STATUS__0XxX, MEC(btEvent).errCode);
            MeProcessGenToken(METOKEN);
            break;

        default:
            MeHandleErrorTaskEnd();
    }

    /*
     * Make sure this event belongs to the current operation
     * (it could have come in asynchronously)
     */
    if (opType == BOP_GEN_COMMAND)
    {
        /* Start another operation if one exists */
        MeOperationEnd();
    }
}


/*****************************************************************************
 * FUNCTION
 *  StartInquiryTask
 * DESCRIPTION
 *  Start the Iqnuiry Task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartInquiryTask(void)
{
    if(!(MEC(curOp) == (BtOperation*) (&(MEC(inquiryOp)))))
    {
        Assert(0);
    }

    MEC(hciGenCmd).parms[0] = (U8) MEC(inquiryOp).lap;
    MEC(hciGenCmd).parms[1] = (U8) ((MEC(inquiryOp.lap)) >> 8);
    MEC(hciGenCmd).parms[2] = (U8) ((MEC(inquiryOp.lap)) >> 16);

    /* Load duration of inquiry */
    MEC(hciGenCmd).parms[3] = MEC(inquiryOp).len;

    /* Load max number of responses */
    MEC(hciGenCmd).parms[4] = MEC(inquiryOp).max;

    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 5);
    
    MeSendHciCommand(HCC_INQUIRY, 5);
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleCompleteTask
 * DESCRIPTION
 *  Handle HCI events for the Complete task.
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleCompleteTask(U8 event, U8 len, U8 *parm)
{
    U16 opcode = 0;

    /* UNUSED_PARAMETER(len); */

    switch (event)
    {
        case HCE_COMMAND_COMPLETE:
            /* Save the status in case it is an error */
            opcode = LEtoHost16((parm + 1));
            MEC(btEvent).errCode = parm[3];
            MEC(btEvent).raw_data = parm;
            break;
        case HCE_COMMAND_STATUS:
            /* Save the status in case it is an error */
            opcode = LEtoHost16((parm + 2));
            MEC(btEvent).errCode = parm[0];
            break;

        default:
            Assert(0);
    }

    /* Make sure the opcode matches. If not ignore the response */
    if (opcode == MEC(curTask))
    {
#ifdef __BT_4_0_BLE__
        if(opcode == HCC_LE_SET_SCAN_ENABLE)
        {
            LeHciCmd *curHciCmd = (LeHciCmd *)MEC(curOp);
            Report(("Ting sets hciCmd->cmdParm.enable A1 to :%02x",curHciCmd->cmdParm.enable));
            
            curHciCmd->parmLen = len - 4;
            OS_MemCopy(curHciCmd->parms, &parm[4], curHciCmd->parmLen);        
            Report(("Ting sets hciCmd->cmdParm.enable A2 to :%02x",curHciCmd->cmdParm.enable));

        }
        else if(opcode == HCC_LE_SET_SCAN_PARAM)
        {
            LeHciCmd *curHciCmd = (LeHciCmd *)MEC(curOp);
            curHciCmd->parmLen = len - 4;
            OS_MemCopy(curHciCmd->parms, &parm[4], curHciCmd->parmLen);            
        }
        else if (opcode >= HCC_LE_SET_EVENT_MASK && opcode <= HCC_LE_TEST_END)
        {
            LeHciCmd *curHciCmd = LeFindHciToken(MEC(curOp));
            curHciCmd->parmLen = len - 4;
            OS_MemCopy(curHciCmd->parms, &parm[4], curHciCmd->parmLen);
        }
#endif
        /* Clear the task if this is the end. */
        if ((event != HCE_COMMAND_STATUS) || ((event == HCE_COMMAND_STATUS) && (parm[0] != 0)))
        {
            MEC(curTask) = 0;
        }

        /* Call the operation handler */
        if (MEC(btEvent).errCode == 0)
        {
            MEC(opHandler) (BOE_TASK_END);
        }
        else
        {
            bt_trace(TRACE_GROUP_1, MEHCI_COMMAND_COMPLETE_ERROR, MEC(btEvent).errCode);
            MEC(opHandler) (BOE_TASK_ERR);
        }

    }
    else
    {
        bt_trace(TRACE_GROUP_1, ME_MEHANDLECOMPLETETASK_OPCODE_MISMATCH_OPCODE__xX_CURTASK__xX, opcode, MEC(curTask));
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleConnectionStatusOnly
 * DESCRIPTION
 *  Handle the tasks where the only event from HCI is
 *  the connection status event.
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleConnectionStatusOnly(U8 event, U8 len, U8 *parm)
{
    //UNUSED_PARAMETER(len);
    //UNUSED_PARAMETER(event);

    if(event != HCE_COMMAND_STATUS)
    {
        Assert(0);
    }
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCONN_STATUSBT_DEBUGx02X, parm[0]);

    /* Make sure the opcode matches. If not ignore the response */
    if (MEC(curTask) == LEtoHost16((parm + 2)))
    {
        /* Task is considered complete on status event */
        MEC(curTask) = 0;
        MEC(btEvent).errCode = parm[0];
        MEC(opHandler) (BOE_TASK_END);
    }
}


/*****************************************************************************
 * FUNCTION
 *  HandleGeneralTask
 * DESCRIPTION
 *  The task handler for General commands.
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleGeneralTask(U8 event, U8 len, U8 *parm)
{
    BtOpEvent opEvent;

    bt_trace(TRACE_GROUP_1, RECEIVE_EVENT_FOR_GENERAL_COMMAND_EVENT__0XxX, event);

    /*
     * Ignore command complete and command status events that do not
     * match the current task
     */
    if (((event == HCE_COMMAND_STATUS) &&
         (MEC(curTask) != LEtoHost16((parm + 2)))) ||
        ((event == HCE_COMMAND_COMPLETE) && (MEC(curTask) != LEtoHost16((parm + 1)))))
    {
        /* Ignore this event */
        bt_trace(TRACE_GROUP_1, ME_HANDLEGENERALTASK_OPCODE_MISMATCH);
        return;
    }

    /* Clear the task if this is the end. */
    if ((event != HCE_COMMAND_STATUS) || ((event == HCE_COMMAND_STATUS) && (parm[0] != 0)))
    {
        MEC(curTask) = 0;
    }

    /* Copy the info into the token in case it is needed */
    METOKEN->p.general.out.status = BT_STATUS_SUCCESS;
    METOKEN->p.general.out.event = event;
    METOKEN->p.general.out.parmLen = len;
    METOKEN->p.general.out.parms = parm;

    opEvent = BOE_TASK_END;
    if (METOKEN->flags & MCTF_ASYNC)
    {
        /*
         * This is an async command so we expect command status. If we
         * get a Command status event with an error or some other event
         * then we have problems so stop the command and report.
         */
        if ((event == HCE_COMMAND_STATUS) && (parm[0] == 0))
        {
            opEvent = BOE_CONT;
        }
    }
    else
    {
        /*
         * This is a sync event. If we get a legal command status and it is not
         * the event we are looking for then continue otherwise stop the command
         * and report.
         */
        if ((event == HCE_COMMAND_STATUS) && (parm[0] == 0) && (METOKEN->p.general.in.event != event))
        {
            opEvent = BOE_CONT;
        }
    }

    MEC(btEvent).errCode = BEC_NO_ERROR;
    if (event == HCE_COMMAND_STATUS)
    {
        /* Set the error. */
        MEC(btEvent).errCode = parm[0];
    }
    MEC(opHandler) (opEvent);
}


/*****************************************************************************
 * FUNCTION
 *  HandleInquiryResult
 * DESCRIPTION
 *  Handle the HCI Inquiry_Result event.
 * PARAMETERS
 *  len             [IN]        
 *  parm            [IN]        
 *  event_type      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleInquiryResult(U8 len, U8 *parm, U8 event_type)
{
    U16 i;
    U8 *p;

    /* Found one or more devices. Report to clients */
    for (i = 0; i < (U16) (parm[0] * 14); i += 14)
    {
        /* Get the parameters */
        OS_MemCopy(MEC(btEvent).p.inqResult.bdAddr.addr, parm + 1 + i, 6);
        MEC(btEvent).p.inqResult.psi.psRepMode = parm[7 + i];
        MEC(btEvent).p.inqResult.psPeriodMode = parm[8 + i];
        p = parm + 9 + i;
        /* PS Mode is no longer provided (parm[9+i]) */
        MEC(btEvent).p.inqResult.psi.psMode = 0;

        /* Reserved parameter not present in RSSI responses */
        if (event_type == HCE_INQUIRY_RESULT)
        {
            p++;
        }

        MEC(btEvent).p.inqResult.classOfDevice = (U32) (p[0]) | ((U32) (p[1]) << 8) | ((U32) (p[2]) << 16);
        p += 3;

        MEC(btEvent).p.inqResult.psi.clockOffset = LEtoHost16(p);
        p += 2;

    #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
        MEC(btEvent).p.inqResult.supported_eir = 0;
    #endif 
    #ifdef __BT_4_0_BLE__
        MEC(btEvent).p.inqResult.addrType = LE_ADDR_PUBLIC;
        MEC(btEvent).p.inqResult.devType = BT_DEV_TYPE_BR_EDR;
    #endif

        switch (event_type)
        {
        #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
            case HCE_INQUIRY_RESULT_WITH_EIR:
                MEC(btEvent).p.inqResult.rssi = (S8) (*p++);
                MEC(btEvent).p.inqResult.eirResponse = p;
                MEC(btEvent).p.inqResult.supported_eir = 1;
                p += 240;   /* EIR Offset 240 bytes */
                break;
        #endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
            case HCE_INQUIRY_RESULT_WITH_RSSI:
                MEC(btEvent).p.inqResult.rssi = (S8) (*p++);
                break;
            case HCE_INQUIRY_RESULT:
                MEC(btEvent).p.inqResult.rssi = BT_INVALID_RSSI;
                break;
        }

        /* Report this one to the clients */
        MEC(btEvent).errCode = 0;
        MEC(btEvent).eType = BTEVENT_INQUIRY_RESULT;
        MeReportResults(BEM_INQUIRY_RESULT);
    }
}


/*****************************************************************************
 * FUNCTION
 *  HandleInquiryComplete
 * DESCRIPTION
 *  Handle the HCI Inquiry_Complete event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleInquiryComplete(U8 len, U8 *parm)
{
    /* UNUSED_PARAMETER(len); */

    /*
     * The Inquiry operation completed. Store the status and number of
     * responses in btEvent. Then tell all registered handlers.
     */
    bt_trace(TRACE_GROUP_1, ME_RECEIVED_INQUIRY_COMPLETE_EVENT, parm[0]);
    MEC(btEvent).errCode = parm[0];
    ReportInquiryComplete();
}


/*****************************************************************************
 * FUNCTION
 *  MeReportResults
 * DESCRIPTION
 *  Call the callback of all the handlers that have the mask set.
 * PARAMETERS
 *  mask        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReportResults(BtEventMask mask)
{
    ListEntry *entry;
    BtHandler *handler;
    BtEvent event;

    MEC(btEvent).handler = 0;
    event = MEC(btEvent);
    MeDevEventHandler();

    if(!IsListCircular(&MEC(regList)))
    {
        Assert(0);
    }
    entry = MEC(regList).Flink;
    while (entry != &MEC(regList))
    {
        /*
         * Point to the next entry before calling the callback in case the
         * callback removes the current handler
         */
        handler = (BtHandler*) entry;
        if(handler == 0)
        {
            Assert(0);
        }

        entry = entry->Flink;

        if (handler->eMask & mask)
        {
            /* Call the callback */
            event.handler = handler;
            if(handler->callback == 0)
            {
                Assert(0);
            }
            
            handler->callback(&event);
        }
        MEC(btEvent).handler = 0;
        if(!IsListCircular(entry))
        {
            Assert(0);
        }
    }
    if(!IsListCircular(&MEC(regList)))
    {
        Assert(0);
    }

    /* Start a disconnect that may be waiting */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  MeReportNMResults
 * DESCRIPTION
 *  Call the callback of all the global handlers with a Non Maskable
 *  event.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeReportNMResults(void)
{
    ListEntry *entry;
    BtHandler *handler;
    BtEvent event;

    MEC(btEvent).handler = 0;
    event = MEC(btEvent);

    MeDevEventHandler();

    if(!IsListCircular(&MEC(regList)))
    {
        Assert(0);
    }
    entry = MEC(regList).Flink;
    while (entry != &MEC(regList))
    {
        /*
         * Point to the next entry before calling the callback in case the
         * callback removes the current handler
         */
        handler = (BtHandler*) entry;
        entry = entry->Flink;

        /* Call the callback */
        MEC(btEvent).handler = handler;
        handler->callback(&MEC(btEvent));

        /*
         * If this assert fires, it means that the current handler modified a
         * portion of the BtEvent structure. This is very bad and should be fixed.
         */
        MEC(btEvent).handler = 0;
        if(!(OS_MemCmp((U8*) & event, (U16) sizeof(BtEvent), (U8*) & MEC(btEvent), (U16) sizeof(BtEvent)) == TRUE))
        {
            Assert(0);
        }
        if(!IsListCircular(entry))
        {
            Assert(0);
        }
    }
    if(!IsListCircular(&MEC(regList)))
    {
        Assert(0);
    }        
     /* Start a disconnect that may be waiting */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  ReportCommandSent
 * DESCRIPTION
 *  
 * PARAMETERS
 *  cmd     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ReportCommandSent(const HciCommand *cmd)
{
    ListEntry *entry;
    BtHandler *handler;
    BtEvent event;

    event.eType = BTEVENT_HCI_COMMAND_SENT;
    event.p.hciCmd.type = LEtoHost16(cmd->header);
    event.p.hciCmd.length = cmd->header[2];
    event.p.hciCmd.parms = cmd->parms;

    if (!IsListCircular(&MEC(regList)))
    {
        Assert(0);
    }    
    entry = MEC(regList).Flink;
    while (entry != &MEC(regList))
    {
        /*
         * Point to the next entry before calling the callback in case the
         * callback removes the current handler
         */
        handler = (BtHandler*) entry;
        entry = entry->Flink;

        /* Call the callback */
        event.handler = handler;
        handler->callback(&event);

        if(!IsListCircular(entry))
        {
            Assert(0);
        }
    }
    if (!IsListCircular(&MEC(regList)))
    {
        Assert(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  ME_Callback
 * DESCRIPTION
 *  This function is called by the HCI layer to report all events
 *  and status.
 * PARAMETERS
 *  event       [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ME_Callback(U8 event, HciCallbackParms *parms)
{
    switch (event)
    {
        case HCI_DATA_IND:
            /* We received data so see who it is for.  */
    #if BT_SCO_HCI_DATA == XA_ENABLED
            if (ScoMapHciToConnect(parms->hciHandle) != 0)
            {
                /* Data is SCO data so have SCO deal with it */
                ScoDataCallback(event, parms);
                return;
            }
    #endif /* BT_SCO_HCI_DATA == XA_ENABLED */
            /* Send data to L2CAP */
            L2CAP_HciReceive(parms->ptr.rxBuff, parms->hciHandle);
            break;

        case HCI_PACKET_HANDLED:
            /* A packet is returned from HCI see where it goes */
        #if BT_SCO_HCI_DATA == XA_ENABLED
            if (ScoMapHciToConnect(parms->hciHandle) != 0)
            {
                /* The packet belongs to SCO */
                ScoDataCallback(event, parms);
                return;
            }
        #endif /* BT_SCO_HCI_DATA == XA_ENABLED */

            /* Give the packet to L2CAP */
            L2CAP_HciTxDone(parms->ptr.packet, parms->status, parms->hciHandle);
            break;

        case HCI_COMMAND_HANDLED:
            MEC(commandPending) = FALSE;
            /* The command buffer is free */
            bt_trace(TRACE_GROUP_1, BTLOG_ME_HCIGENCMD_HANDLED);
            InitializeListEntry(&(parms->ptr.cmd->node));
            if (parms->status == BT_STATUS_SUCCESS)
            {
                ReportCommandSent(parms->ptr.cmd);
            }

            /* start operation queue, if it is waiting for the command buffer */
            if (MEC(opMask) & BOM_HCICOMMAND_BUSY)
            {
                MEC(opMask) &= ~BOM_HCICOMMAND_BUSY;
                MeStartOperation();
            }
            /* finish init radio task, if it is waiting */
            if (MEC(opMask) & BOM_INITRADIO_BUSY)
            {
                MEC(opMask) &= ~BOM_INITRADIO_BUSY;
                MeInitRadio(BOE_TASK_END);
            }
            break;
        case HCI_SEND_IND_AMP:
            parms->ptr.packet = L2CAP_HciTxReady(parms, 1);
            break;
        case HCI_SEND_IND:
            /* Request to send data */
        #if BT_SCO_HCI_DATA == XA_ENABLED
            if (ScoMapHciToConnect(parms->hciHandle) != 0)
            {
                ScoDataCallback(event, parms);
                return;
            }
        #endif /* BT_SCO_HCI_DATA == XA_ENABLED */

            /* Tell L2CAP it can send data */
            /* parms->ptr.packet = L2CAP_HciTxReady(parms->hciHandle); */
            parms->ptr.packet = L2CAP_HciTxReady(parms, 0);
            break;

        case HCI_CONTROLLER_EVENT:
            /* Handle the event. */
            (void)MeHandleEvents(parms->ptr.hciEvent->event, parms->ptr.hciEvent->len, parms->ptr.hciEvent->parms);
            break;

        case HCI_INIT_STATUS:
            /* HCI initializaiton is complete */
            bt_trace(TRACE_GROUP_1, ME_HCI_INIT_COMPLETE_STATUS_xD, parms->status);
            MeProcessInitStatus(parms);
            break;

        case HCI_DEINIT_STATUS:
            /* Set state to ensure no commands are queued. */
            MEC(stackState) = BTSS_INITIALIZE_ERR;

            /*
             * HCI deintialization is complete. All ACL and SCO connections
             * should have been disconnected by HCI. Return any existing commands.
             * and attempt to initialize HCI 
             */
            bt_trace(TRACE_GROUP_1, ME_HCI_DEINIT_COMPLETE_STATUS_xD, parms->status);

            MEC(curTask) = 0;
            MeFailPendingCommands();

            MEC(stackState) = BTSS_NOT_INITIALIZED;
            MEC(btEvent).eType = BTEVENT_HCI_DEINITIALIZED;
            MeReportNMResults();
            break;

        case HCI_TRANSPORT_ERROR:
            bt_trace(TRACE_GROUP_1, ME_HCI_TRANSPORT_ERROR);
            MEC(btEvent).eType = BTEVENT_HCI_FATAL_ERROR;
            MeReportNMResults();
            HCI_Deinit();
            break;

    }
}

#ifndef __BT_MP_RELEASE__
static char *getEventStr(U8 event)
{

    switch(event)
    {
    case HCE_INQUIRY_COMPLETE:
        return "HCE_INQUIRY_COMPLETE";
    case HCE_INQUIRY_RESULT:
        return "HCE_INQUIRY_RESULT";
    case HCE_CONNECT_COMPLETE:
        return "HCE_CONNECT_COMPLETE";
    case HCE_CONNECT_REQUEST:
        return "HCE_CONNECT_REQUEST";
    case HCE_DISCONNECT_COMPLETE:
        return "HCE_DISCONNECT_COMPLETE";
    case HCE_AUTH_COMPLETE:
        return "HCE_AUTH_COMPLETE";
    case HCE_REMOTE_NAME_REQ_COMPLETE:
        return "HCE_REMOTE_NAME_REQ_COMPLETE";
    case HCE_ENCRYPT_CHNG:
        return "HCE_ENCRYPT_CHNG";
    case HCE_CHNG_CONN_LINK_KEY_COMPLETE:
        return "HCE_CHNG_CONN_LINK_KEY_COMPLETE";
    case HCE_MASTER_LINK_KEY_COMPLETE:
        return "HCE_MASTER_LINK_KEY_COMPLETE";
    case HCE_READ_REMOTE_FEATURES_COMPLETE:
        return "HCE_READ_REMOTE_FEATURES_COMPLETE";
    case HCE_READ_REMOTE_VERSION_COMPLETE:
        return "HCE_READ_REMOTE_VERSION_COMPLETE";
    case HCE_QOS_SETUP_COMPLETE:
        return "HCE_QOS_SETUP_COMPLETE";
    case HCE_COMMAND_COMPLETE:
        return "HCE_COMMAND_COMPLETE";
    case HCE_COMMAND_STATUS:
        return "HCE_COMMAND_STATUS";
    case HCE_HARDWARE_ERROR:
        return "HCE_HARDWARE_ERROR";
    case HCE_FLUSH_OCCURRED:
        return "HCE_FLUSH_OCCURRED";
    case HCE_ROLE_CHANGE:
        return "HCE_ROLE_CHANGE";
    case HCE_NUM_COMPLETED_PACKETS:
        return "HCE_NUM_COMPLETED_PACKETS";
    case HCE_MODE_CHNG:
        return "HCE_MODE_CHNG";
    case HCE_RETURN_LINK_KEYS:
        return "HCE_RETURN_LINK_KEYS";
    case HCE_PIN_CODE_REQ:
        return "HCE_PIN_CODE_REQ";
    case HCE_LINK_KEY_REQ:
        return "HCE_LINK_KEY_REQ";
    case HCE_LINK_KEY_NOTIFY:
        return "HCE_LINK_KEY_NOTIFY";
    case HCE_LOOPBACK_COMMAND:
        return "HCE_LOOPBACK_COMMAND";
    case HCE_DATA_BUFFER_OVERFLOW:
        return "HCE_DATA_BUFFER_OVERFLOW";
    case HCE_MAX_SLOTS_CHNG:
        return "HCE_MAX_SLOTS_CHNG";
    case HCE_READ_CLOCK_OFFSET_COMPLETE:
        return "HCE_READ_CLOCK_OFFSET_COMPLETE";
    case HCE_CONN_PACKET_TYPE_CHNG:
        return "HCE_CONN_PACKET_TYPE_CHNG";
    case HCE_QOS_VIOLATION:
        return "HCE_QOS_VIOLATION";
    case HCE_PAGE_SCAN_MODE_CHANGE:
        return "HCE_PAGE_SCAN_MODE_CHANGE";
    case HCE_PAGE_SCAN_REPETITION_MODE:
        return "HCE_PAGE_SCAN_REPETITION_MODE";
    case HCE_FLOW_SPECIFICATION_COMPLETE:
        return "HCE_FLOW_SPECIFICATION_COMPLETE";
    case HCE_INQUIRY_RESULT_WITH_RSSI:
        return "HCE_INQUIRY_RESULT_WITH_RSSI";
    case HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
        return "HCE_READ_REMOTE_EXT_FEAT_COMPLETE";
    case HCE_FIXED_ADDRESS:
        return "HCE_FIXED_ADDRESS";
    case HCE_ALIAS_ADDRESS:
        return "HCE_ALIAS_ADDRESS";
    case HCE_GENERATE_ALIAS_REQ:
        return "HCE_GENERATE_ALIAS_REQ";
    case HCE_ACTIVE_ADDRESS:
        return "HCE_ACTIVE_ADDRESS";
    case HCE_ALLOW_PRIVATE_PAIRING:
        return "HCE_ALLOW_PRIVATE_PAIRING";
    case HCE_ALIAS_ADDRESS_REQ:
        return "HCE_ALIAS_ADDRESS_REQ";
    case HCE_ALIAS_NOT_RECOGNIZED:
        return "HCE_ALIAS_NOT_RECOGNIZED";
    case HCE_FIXED_ADDRESS_ATTEMPT:
        return "HCE_FIXED_ADDRESS_ATTEMPT";
    case HCE_SYNC_CONNECT_COMPLETE:
        return "HCE_SYNC_CONNECT_COMPLETE";
    case HCE_SYNC_CONN_CHANGED:
        return "HCE_SYNC_CONN_CHANGED";
    case HCE_SNIFF_SUBRATING_EVENT:
        return "HCE_SNIFF_SUBRATING_EVENT";
    case HCE_INQUIRY_RESULT_WITH_EIR:
        return "HCE_INQUIRY_RESULT_WITH_EIR";
    case HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT:
        return "HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT";
    case HCE_IO_CAPABILITY_REQUEST:
        return "HCE_IO_CAPABILITY_REQUEST";
    case HCE_IO_CAPABILITY_RESPONSE:
        return "HCE_IO_CAPABILITY_RESPONSE";
    case HCE_USER_CONFIRM_REQUSEST:
        return "HCE_USER_CONFIRM_REQUSEST";
    case HCE_USER_PASSKEY_REQUEST_EVENT:
        return "HCE_USER_PASSKEY_REQUEST_EVENT";
    case HCE_REMOTE_OOB_DATA_REQUEST_EVENT:
        return "HCE_REMOTE_OOB_DATA_REQUEST_EVENT";
    case HCE_SIMPLE_PAIRING_COMPLETE:
        return "HCE_SIMPLE_PAIRING_COMPLETE";
    case HCE_LINK_SUPERVISION_TIMEOUT_CHNG:
        return "HCE_LINK_SUPERVISION_TIMEOUT_CHNG";
    case HCE_ENHANCED_FLUSH_COMPLETE:
        return "HCE_ENHANCED_FLUSH_COMPLETE";
    case HCE_USER_PASSKEY_NOTIFICATION:
        return "HCE_USER_PASSKEY_NOTIFICATION";
    case HCE_USER_KEYPRESS:
        return "HCE_USER_KEYPRESS";
    case HCE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT:
        return "HCE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT";
    case HCE_UART_SETTING_COMPLETE:
        return "HCE_UART_SETTING_COMPLETE";
    case HCE_BLUETOOTH_LOGO:
        return "HCE_BLUETOOTH_LOGO";
    case HCE_VENDOR_SPECIFIC:    
        return "HCE_VENDOR_SPECIFIC";
    default:
        return "Unknown event";
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  MeHandleEvents
 * DESCRIPTION
 *  Handle events from the HCI.
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  TRUE if handled FALSE otherwise
 *****************************************************************************/
BOOL MeHandleEvents(U8 event, U8 len, U8 *parm)
{
    MeCommandToken *token;
    U16 opcode;
    BOOL useTaskHandler;

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ME_RX_EVENT, event);
#ifndef __BT_MP_RELEASE__
    Report(("rx_event: %s", getEventStr(event)));
#endif
    switch (event)
    {
    #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
        case HCE_INQUIRY_RESULT_WITH_EIR:
            HandleInquiryResult(len, parm, HCE_INQUIRY_RESULT_WITH_EIR);
            break;
    #endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
        case HCE_INQUIRY_RESULT_WITH_RSSI:
            HandleInquiryResult(len, parm, HCE_INQUIRY_RESULT_WITH_RSSI);
            break;
        case HCE_INQUIRY_RESULT:
            HandleInquiryResult(len, parm, HCE_INQUIRY_RESULT);
            break;

        case HCE_INQUIRY_COMPLETE:
            {
                U8 *temp;
                
                if(MEC(BT_Sres1) != 0)
                {
                    temp = (U8 *)get_ctrl_buffer(4);
                    OS_MemCopy(temp, MEC(BT_Sres1), 4);
                    free_ctrl_buffer(MEC(BT_Sres1));
                    MEC(BT_Sres1) = temp;
                }
            }             
            HandleInquiryComplete(len, parm);
            break;

        case HCE_CONNECT_REQUEST:   /* Incoming connection */
            MeHandleLinkConnectReq(len, parm);
            break;

        case HCE_CONNECT_COMPLETE:
            {
                U8 *temp;
                
                if(MEC(BT_Sres2) != 0)
                {
                    temp = (U8 *)get_ctrl_buffer(4);
                    OS_MemCopy(temp, MEC(BT_Sres2), 4);
                    free_ctrl_buffer(MEC(BT_Sres2));
                    MEC(BT_Sres2) = temp;
                }
            }            
            MeHandleConnectComplete(len, parm);
            break;

        case HCE_DISCONNECT_COMPLETE:
            MeHandleDisconnectComplete(len, parm);
            break;

    #if NUM_SCO_CONNS > 0
        case HCE_SYNC_CONNECT_COMPLETE:
            ScoHandleConnectComplete(len, parm);
            break;

        case HCE_SYNC_CONN_CHANGED:
            ScoHandleConnChanged(len, parm);
            break;
    #endif /* NUM_SCO_CONNS > 0 */

        case HCE_PIN_CODE_REQ:
            SecHandlePinCodeReq(len, parm);
            break;

        case HCE_LINK_KEY_REQ:
            SecHandleLinkKeyReq(len, parm);
            break;

        case HCE_AUTH_COMPLETE:
            SecHandleAuthenticateComplete(len, parm);
            break;

    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        case HCE_IO_CAPABILITY_REQUEST:
            SecHandleIoCapabilityRequest(len, parm);
            break;
        case HCE_IO_CAPABILITY_RESPONSE:
            SecHandleIoCapabilityResponse(len, parm);
            break;
        case HCE_USER_CONFIRM_REQUSEST:
            SecHandleUserConfirmRequest(len, parm);
            break;
        case HCE_USER_PASSKEY_NOTIFICATION:
            SecHandleUserPasskeyNotification(len, parm);
            break;
        case HCE_USER_KEYPRESS:
            SecHandleUserKeyPress(len, parm);
            break;
        case HCE_SIMPLE_PAIRING_COMPLETE:
            SecHandleSimplePairingComplete(len, parm);
            break;
        case HCE_REMOTE_OOB_DATA_REQUEST_EVENT:
            SecHandleRemoteOobDataRequest(len, parm);
            break;
    #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

        case HCE_LINK_KEY_NOTIFY:
            SecHandleLinkKeyNotify(len, parm);
            break;

        case HCE_ENCRYPT_CHNG:
            SecHandleEncryptChange(len, parm);
            break;

        case HCE_REMOTE_NAME_REQ_COMPLETE:
            MeHandleRemNameReqComplete(len, parm);
            break;

        case HCE_ROLE_CHANGE:
            MeHandleRoleChange(len, parm);
            break;

        case HCE_MODE_CHNG:
            bt_trace(TRACE_GROUP_1, MODE_CHANGE);
            MeHandleModeChange(len, parm);
            break;

        case HCE_MAX_SLOTS_CHNG:
            break;

        case HCE_QOS_SETUP_COMPLETE:
        case HCE_PAGE_SCAN_REPETITION_MODE:
            /* Ignore these events */
            break;
        case HCE_READ_REMOTE_FEATURES_COMPLETE:
            MeHandleReadRemoteFeature(len, parm);
            break;
        case HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
            MeHandleReadRemoteExtFeature(len, parm);
            break;
        case HCE_READ_REMOTE_VERSION_COMPLETE:
            MeHandleReadRemoteVersion(len, parm);
            break;
        case HCE_READ_CLOCK_OFFSET_COMPLETE:
            MeHandleReadRemoteClockOffset(len, parm);
            break;
    #if defined (__BT_2_1_SUPERVISION_TIMEOUT_EVENT__)
        case HCE_LINK_SUPERVISION_TIMEOUT_CHNG:
            MeHandleSupervisionTimeoutChange(len, parm);
            break;
    #endif /* defined (__BT_2_1_SUPERVISION_TIMEOUT_EVENT__) */ 
    #if defined (__BT_2_1_AUTO_FLUSH__)
        case HCE_ENHANCED_FLUSH_COMPLETE:
            MeHandleEnhancedFlush(len, parm);
            break;
    #endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 
    #ifdef MTK_BB_LOOPBACK_MODE
        case HCE_UART_SETTING_COMPLETE:
            GORMevt_HCC_UART_CHANGE_SUCCESS(len, parm);
    #endif /* MTK_BB_LOOPBACK_MODE */ 
            break;
    #ifdef __BT_4_0_BLE__
        case HCE_LE_META_EVENT:
            LeHandleEvents(len, parm);
            break;
    #endif
        default:
            /* First, check to see if a handler exists */
            useTaskHandler = FALSE;
            if (MEC(taskHandler) != 0)
            {
                /*
                 * Handler exists. We need to make sure it is for
                 * this event. This may be an event from an 
                 * asynchronous command or an unexpected event.
                 */

                /* Check if event has opcode */
                switch (event)
                {
                    case HCE_COMMAND_COMPLETE:
                        opcode = LEtoHost16((parm + 1));
                        break;

                    case HCE_COMMAND_STATUS:
                        opcode = LEtoHost16((parm + 2));
                        break;

                    default:
                        opcode = 0;
                        break;
                }

                /*
                 * Use the taskHandler if the opcode matches OR
                 * if no opcode, the taskEvent matches.
                 */
                if ((opcode && opcode == MEC(curTask)) || (!opcode && event == MEC(taskEvent)))
                {
                    /* use the taskHandler */
                    useTaskHandler = TRUE;
                }
            }

            /* Use the taskHandler if it matched above. */
            if (useTaskHandler == TRUE)
            {
                /* Call the task handler */
                MEC(taskHandler) (event, len, parm);
            }
            else
            {
                /*
                 * Handler does not exist so see if there is a general command
                 * waiting for the event
                 */
                token = MeFindGeneralEvent(event, parm);
                if (token != 0)
                {
                    /* We have general command looking for this event. */
                    token->p.general.out.status = BT_STATUS_SUCCESS;
                    token->p.general.out.event = event;
                    token->p.general.out.parmLen = len;
                    token->p.general.out.parms = parm;
                    MeProcessGenToken(token);
                }
                else
                {
                    /* There is no one to handle this event */
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_NO_EVENT_HDL, event);
                    /* Not handled so return FALSE */
                    MeStartOperation();
                    return FALSE;
                }
            }
    }

    /* Handled so return TRUE */
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  MeOperationEnd
 * DESCRIPTION
 *  End the current operation and start a new one if one
 *  exists.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeOperationEnd(void)
{
    MEC(taskHandler) = 0;
    MEC(curOp) = 0;
    ME_AuthenticationChip();
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  MeProcessToken
 * DESCRIPTION
 *  Add the token to the appropriate queue.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeProcessToken(MeCommandToken *token)
{
    /* If an operation is in progress then put this token on the command queue */
    if (token->flags & MCTF_NEED_CHECK)
    {
        if ((MeFindMatchingToken(token, &MEC(opList)) != 0) || (MeFindMatchingToken(token, &MEC(cmdList)) != 0))
        {
            InsertTailList(&MEC(cmdList), &(token->op.node));
            return;
        }
    }

    /* Start up an operation */
    MeAddOperation(token);

    /* Attempt to start the operation */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  MeProcessGenToken
 * DESCRIPTION
 *  Process the general token.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeProcessGenToken(MeCommandToken *token)
{
    /*
     * Remove this token from the cmd list
     */
    if(!IsNodeOnList(&MEC(cmdList), &(token->op.node)))
    {
        Assert(0);
    }
    RemoveEntryList(&(token->op.node));

    /* Report the results of this token */
    MEC(btEvent).p.meToken = token;
    MEC(btEvent).eType = token->eType;
    token->callback(&MEC(btEvent));
    /* Start a disconnect that may be waiting */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  MeFindMatchingToken
 * DESCRIPTION
 *  If a token already exists on the list that is making the same
 *  request then return a pointer to it. Otherwise return 0..
 * PARAMETERS
 *  token       [IN]        
 *  list        [IN]        
 * RETURNS
 *  MeCommandToken* or 0
 *****************************************************************************/
MeCommandToken *MeFindMatchingToken(MeCommandToken *token, ListEntry *list)
{
    MeCommandToken *curToken;

    /* Check the operation queue */
    curToken = (MeCommandToken*) list->Flink;
    while (curToken != (MeCommandToken*) list)
    {
        if (IsMeCommandEqual(token, curToken))
        {
            /* The commands are the same */
            return curToken;
        }
        curToken = (MeCommandToken*) (curToken->op.node.Flink);
    }

    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  IsMeCommandEqual
 * DESCRIPTION
 *  Determine if one token is performing the same command as
 *  another.
 * PARAMETERS
 *  token1      [IN]        
 *  token2      [IN]        
 * RETURNS
 *  TRUE - if commands are the same otherwise FALSE.
 *****************************************************************************/
static BOOL IsMeCommandEqual(MeCommandToken *token1, MeCommandToken *token2)
{
    if ((token1->op.opType == token2->op.opType) ||
        (token1->op.opType == BOP_CANCEL_REM_NAME && token2->op.opType == BOP_REM_NAME_REQ))
    {
        /*
         * We found a token performing the same operation. Now see if
         * it has the same input parameters (if necessary)
         */
        switch (token1->op.opType)
        {
            case BOP_REM_NAME_REQ:
            case BOP_CANCEL_REM_NAME:
                /* Remote Name request */
                return (OS_MemCmp(token1->p.name.bdAddr.addr, 6, token2->p.name.bdAddr.addr, 6));

            case BOP_GEN_COMMAND:
                if ((token1->flags & MCTF_ASYNC) && (token2->flags & MCTF_ASYNC) &&
                    (token1->p.general.in.event == token2->p.general.in.event))
                {
                    return TRUE;
                }
                break;

            default:
                /* Only the command needs to match */
                return TRUE;
        }
    }
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  MeFindGeneralEvent
 * DESCRIPTION
 *  Find the token on the cmdList that is looking for the specified
 *  event.
 * PARAMETERS
 *  event       [IN]        
 *  parm        [IN]        
 * RETURNS
 *  MeCommandToken* or 0
 *****************************************************************************/
MeCommandToken *MeFindGeneralEvent(BtOpEvent event, U8 *parm)
{
    U16 opcode;
    MeCommandToken *curToken;

    /* Check if event has opcode */
    switch (event)
    {
        case HCE_COMMAND_COMPLETE:
            opcode = LEtoHost16((parm + 1));
            bt_trace(TRACE_GROUP_1, OPCODEBT_DEBUGx04X, opcode);
            switch (opcode)
            {
                case 0xFCC0:
#ifdef __ENABLE_SLEEP_MODE__                    
                    bt_trace(TRACE_GROUP_1, RADIO_WAKEUP_COMPLETED);
                    {
                        U8 *temp;
                        
                        if(MEC(BT_Rand) != 0)
                        {
                            temp = (U8 *)get_ctrl_buffer(4);
                            OS_MemCopy(temp, MEC(BT_Rand), 4);
                            free_ctrl_buffer(MEC(BT_Rand));
                            MEC(BT_Rand) = temp;
                        }
                    }                   
#else
                    Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */
                    break;

                case 0xFCC1:
#ifdef __ENABLE_SLEEP_MODE__                    
                    bt_trace(TRACE_GROUP_1, HOST_WAKEUP_COMPLETED);
                    HCC(hostwakeupCounter) = 0;
                    EVM_ResetTimer(&HCC(hostwakeupTimer));
                    Radio_hostWakedup();
#else
#ifdef __BT_TRANSPORT_DRV__
										return 0;
#else
                    Assert(0);
#endif
#endif  /* __ENABLE_SLEEP_MODE__ */                     
                    break;
                case 0xFCD0:
                    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCOMMAND_COMPLETE_OF_PCM_LOOPBACK);
                    BTSysAdpEnterSpeechTest();
                    break;
                case 0xFC15:
                    bt_trace(TRACE_GROUP_1, WHITTEN_COMPLETED_PARM3BT_DEBUGx02X, parm[3]);
                    if (parm[3] == 0x00)
                    {
                        BTSysAdpRFTestResult(0);
                    }
                    else
                    {
                        BTSysAdpRFTestResult(1);
                    }
                    break;
                case 0xFC0D:
                case 0xFCD5:
                    bt_trace(TRACE_GROUP_1, RF_TEST_COMMAND_COMPLETED);
                    if (parm[3] == 0x00)
                    {
                        BTSysAdpRFTestResult(0);
                    }
                    else
                    {
                        BTSysAdpRFTestResult(1);
                    }
                    break;
                case 0xFC17:
                    bt_trace(TRACE_GROUP_1, POWER_CONTROL_COMPLETED);
                    if (parm[3] == 0x00)
                    {
                        BTSysAdpRFTestResult(0);
                    }
                    else
                    {
                        BTSysAdpRFTestResult(1);
                    }
                    break;
            }
            break;
        case HCE_COMMAND_STATUS:
            {
                U8 *temp;
                
                if(MEC(BT_Keyaddr) != 0)
                {
                    temp = (U8 *)get_ctrl_buffer(4);
                    OS_MemCopy(temp, MEC(BT_Keyaddr), 4);
                    free_ctrl_buffer(MEC(BT_Keyaddr));
                    MEC(BT_Keyaddr) = temp;
                }
            }              
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCMD_STATUSBT_DEBUGx02X, parm[0]);
            opcode = LEtoHost16((parm + 2));
        #ifdef MTK_BB_LOOPBACK_MODE
            switch (opcode)
            {
                case HCC_UART_TEST_REQ:
                    bt_trace(TRACE_GROUP_1, COMMAND_STATUS_EVENT_WITH_OPCODE_FCF1);
                    uartExpiredImmediately();
                    GORMevt_HCC_UART_CHANGE_STATUS_EVENT();
            }
        #endif /* MTK_BB_LOOPBACK_MODE */ 
            break;

        default:
            opcode = 0;
            break;
    }

    /* Check the operation queue */
    curToken = (MeCommandToken*) (MEC(cmdList).Flink);
    while (curToken != (MeCommandToken*) & (MEC(cmdList)))
    {
        if (curToken->p.general.in.event == event)
        {
            if (!opcode || (opcode == curToken->p.general.in.hciCommand))
            {
                return curToken;;
            }
        }
        curToken = (MeCommandToken*) (curToken->op.node.Flink);
    }

    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  ReportInquiryComplete
 * DESCRIPTION
 *  Report results of the inquiry complete.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void ReportInquiryComplete(void)
{
    if (!(MEC(opMask) & BOM_CANCEL_INQUIRY))
    {
        /* A cancel is not in progress so report result clear flag. */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_INQUIRY_COMPLETE);
        MEC(opMask) &= ~BOM_INQUIRY;
    }
    else
    {
        /* A cancel is in progress so wait until it returns to clear flag */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_INQUIRY_CANCELED);
    }

    MEC(btEvent).eType = BTEVENT_INQUIRY_COMPLETE;
    MeReportResults(BEM_INQUIRY_COMPLETE);
}


/*****************************************************************************
 * FUNCTION
 *  MeReportInqCancelComplete
 * DESCRIPTION
 *  Report results of the inquiry cancel complete.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeReportInqCancelComplete(void)
{
    MEC(opMask) &= ~(BOM_CANCEL_INQUIRY | BOM_INQUIRY);
    MEC(btEvent).eType = BTEVENT_INQUIRY_CANCELED;
    MeReportResults(BEM_INQUIRY_CANCELED);
}


/*****************************************************************************
 * FUNCTION
 *  MeReportMeCommandComplete
 * DESCRIPTION
 *  Report results of a ME commands.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReportMeCommandComplete(MeCommandToken *token)
{
    MeCommandToken *tmp;
    U8 tokenID = 0;

    /*
     * If this command is of the type that all tokens receive the
     * results from this one invocation. Then find all the matches.
     */
    MEC(btEvent).eType = token->eType;
    if (token->flags & MCTF_NEED_CHECK)
    {
        /* Find all matching tokens and report the results */
        while ((tmp = MeFindMatchingToken(token, &MEC(cmdList))) != 0)
        {
            /* Remove the token from the list */
            RemoveEntryList(&(tmp->op.node));

            /* Copy the parameter information in the token */
            if (tmp != token)
            {
                tmp->p = token->p;
            }

            /* Call the callback */
            MEC(btEvent).p.meToken = tmp;

            if (token->eType == BTEVENT_NAME_RESULT)
            {
                tokenID = btbmUtilGetNameTokenID(tmp);

                Report(("MeReportMeCommandComplete(): eType is Name Result, cancel timer of token=%d", tokenID));
                EVM_CancelTimer(&tmp->tokenTimer);
                tmp->tokenTimer.func = 0;

            }
            if (tmp->callback != 0)
            {
                tmp->callback(&MEC(btEvent));
            }
        }
    }
    else
    {
        /* Handle only this command */
        if (IsNodeOnList(&MEC(cmdList), &(token->op.node)))
        {
            RemoveEntryList(&(token->op.node));
        }
        if (token->eType == BTEVENT_NAME_RESULT)
        {
            tokenID = btbmUtilGetNameTokenID(token);
            Report(("MeReportMeCommandComplete(): [2] eType is Name Result, cancel timer of token=%d", tokenID));
            EVM_CancelTimer(&token->tokenTimer);
            token->tokenTimer.func = 0;

        }        
        MEC(btEvent).p.meToken = token;
        if (token->callback != 0)
        {
            token->callback(&MEC(btEvent));
        }
    }
    /* Start a disconnect that may be waiting */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  MeProcessInitStatus
 * DESCRIPTION
 *  Process the HCI_INIT_STATUS event.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeProcessInitStatus(HciCallbackParms *parms)
{
    if (parms->status == BT_STATUS_FAILED)
    {
        MEC(stackState) = BTSS_NOT_INITIALIZED;
        MEC(btEvent).eType = BTEVENT_HCI_FAILED;
        MeReportNMResults();
    }
    else if (parms->status == BT_STATUS_HCI_INIT_ERR)
    {
        MEC(stackState) = BTSS_INITIALIZE_ERR;
        MEC(btEvent).eType = BTEVENT_HCI_INIT_ERROR;
        MeReportNMResults();
    }
    else
    {
        /* HCI has initialized successfully so get local radio parameters */
        MeInitRadio(BOE_START);
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeFailPendingCommands
 * DESCRIPTION
 *  Fail all pending ME commands.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeFailPendingCommands(void)
{
    I8 i;
    BtRemoteDevice *remDev;
    BtOperation *saveCurOp;

    saveCurOp = 0;
    /* First fail the current operation */
    if (MEC(curOp) != 0)
    {
        MeFailCommand(MEC(curOp)->opType, (void*)MEC(curOp));
        saveCurOp = MEC(curOp);
    }

    /* Go through the operations queue and fail all operations */
    while (!IsListEmpty(&MEC(opList)))
    {
        MEC(curOp) = (BtOperation*) RemoveHeadList(&MEC(opList));
        InitializeListEntry(&(MEC(curOp)->node));
        MeFailCommand(MEC(curOp)->opType, (void*)MEC(curOp));
    }

    /* Clear the command queue */
    while (!IsListEmpty(&MEC(cmdList)))
    {
        MEC(curOp) = (BtOperation*) RemoveHeadList(&MEC(cmdList));

        if (MEC(curOp) != saveCurOp)
        {
            /* This is not the current operation so fail it */
            InitializeListEntry(&(MEC(curOp)->node));
            MeFailCommand(MEC(curOp)->opType, (void*)MEC(curOp));
        }
    }

    /* Clear all BtRemoteDevice structures */
    for (i = 0; (remDev = MeEnumerateRemoteDevices(i)) != 0; i++)
    {
        if (remDev->op.opType != BOP_NOP)
        {
            if (&(remDev->op) != saveCurOp)
            {
                /* This is not the current operation so fail it */
                MeFailCommand(remDev->op.opType, (void*)remDev);

                /* Verify no pending security tokens */
                if(!IsListEmpty(&(remDev->secTokens)))
                {
                    Assert(0);
                }
            }
        }
    }

    /* Fail any commands cleared from queues but still in progress */
    MeFailInProgressCommands();

    if (!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        MEC(opMask) &= ~BOM_HCICOMMAND_BUSY;
        InitializeListEntry(&(MEC(hciGenCmd).node));
    }

    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if (MEC(secOpTable)[i].op.opType != BOP_NOP)
        {
            if (MEC(secOpTable)[i].remDev != 0)
            {
                MeFailCommand(MEC(secOpTable)[i].op.opType, (void*)&(MEC(secOpTable)[i]));
            }
            MEC(secOpTable)[i].op.opType = BOP_NOP;
        }
    }

    /* Reset the ME state back to the initial state. */
    MEC(curOp) = 0;
    MEC(opMask) = 0;

    MEC(secModeState) = 0;
    MEC(secLevel) = 0;

    MEC(secModeEncrypt) = 0;

    MEC(pendCons) = 0;
    MEC(activeCons) = 0;
    MEC(inHoldCons) = 0;

    MEC(opState) = 0;
    MEC(taskState) = 0;

    MEC(isLiac) = FALSE;
    MEC(accModeCur) = 0;


    /* Non connected accessible info */
    MEC(accModeExpValue) = 0;    

    /* Verify that all BtRemoteDevice structures are free */
    for (i = 0; (remDev = MeEnumerateRemoteDevices(i)) != 0; i++)
    {
        if(remDev->op.opType != BOP_NOP)
        {
            Assert(0);
        }
        /* Assert(remDev->state == BDS_DISCONNECTED); */
        if (remDev->state != BDS_DISCONNECTED)
        {
            remDev->state = BDS_DISCONNECTED;
        }
    }

    /* Verify all security tokens are free */
    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if(MEC(secOpTable)[i].op.opType != BOP_NOP)
        {
            Assert(0);
        }
    }

}


/*****************************************************************************
 * FUNCTION
 *  MeFailCommand
 * DESCRIPTION
 *  Fail the command pointed to by the operation. For most commands
 *  a event is not generated but for those involving memory given
 *  by the application an event is generated.
 * PARAMETERS
 *  opType      [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeFailCommand(BtOpType opType, void *parm)
{
    MEC(btEvent).errCode = BEC_HARDWARE_FAILURE;

    switch (opType)
    {
        case BOP_INQUIRY:
            ReportInquiryComplete();
            break;

        case BOP_CANCEL_INQUIRY:
            MeReportInqCancelComplete();
            break;

        case BOP_REM_NAME_REQ:
            /*
             * MeFailPendingCommands() already removed the token from the list.  
             * Clear the MCTF_NEED_CHECK flag so the callback gets called directly.
             */
            ((MeCommandToken*) parm)->flags &= ~MCTF_NEED_CHECK;
            MeReportMeCommandComplete((MeCommandToken*) parm);
            break;

        case BOP_GEN_COMMAND:
            /* Remove the token from the command list prior to callback */
            if(!(IsNodeOnList(&MEC(cmdList), &(((MeCommandToken*) parm)->op.node))))
            {
                Assert(0);
            }
            RemoveEntryList(&(((MeCommandToken*) parm)->op.node));

            /* Indicate the failure */
            ((MeCommandToken*) parm)->p.general.out.status = BT_STATUS_FAILED;
            MEC(btEvent).p.meToken = (MeCommandToken*) parm;
            MEC(btEvent).eType = ((MeCommandToken*) parm)->eType;
            ((MeCommandToken*) parm)->callback(&MEC(btEvent));
            break;

        case BOP_LINK_CONNECT:
            MeReportLinkUp((BtRemoteDevice*) parm, BTEVENT_LINK_CONNECT_CNF);
            break;

        case BOP_LINK_ACCEPT:
        case BOP_LINK_REJECT:
            MeReportLinkUp((BtRemoteDevice*) parm, BTEVENT_LINK_CONNECT_IND);
            break;

        case BOP_WRITE_SCAN_MODE:
            MeHandleWriteScanModeOp(BOE_TASK_ERR);
            break;
        case BOP_WRITE_IAC_VALUE:
            MeHandleWriteIacOp(BOE_TASK_ERR);
            break;
        case BOP_ENCRYPTION:
            ((BtSecurityOp*) parm)->op.opType = BOP_NOP;
            SecReportEncryptionResult(((BtSecurityOp*) parm)->remDev, 0);
            break;

        case BOP_SECURITY_MODE:
            ((BtSecurityOp*) parm)->op.opType = BOP_NOP;
            SecReportSecurityModeResult();
            break;

        case BOP_AUTHENTICATE:
            ((BtSecurityOp*) parm)->op.opType = BOP_NOP;
            SecReportAuthenticateResult(((BtSecurityOp*) parm)->remDev);
            break;

            /* The following commands do not require that an event be generated */
        case BOP_LINK_KEY_RSP:
        case BOP_PIN_CODE_RSP:
        case BOP_PIN_CODE_BY_ADDR_RSP:
        case BOP_AUTHORIZE:
            /* Return the security op to the pool. */
            ((BtSecurityOp*) parm)->op.opType = BOP_NOP;
            break;
        case BOP_CHANGE_PACKET_TYPE:
            ((BtOperation*) parm)->opType = BOP_NOP;
            break;
        case BOP_WRITE_COD:
            /* Free the Write COD structure */
            MEC(codOp).opType = BOP_NOP;
            break;

        case BOP_LOCAL_NAME_CHG:
            /* Free the Name Op structure */
            MEC(localName) = 0;
            MEC(localNameLen) = 0;
            MEC(nameOp).opType = BOP_NOP;
            break;

        case BOP_SCO_VOICESET:
//            ((MeCommandToken*) parm)->op.opType = BOP_NOP;
//            MEC(btEvent).eType = BTEVENT_SCO_VSET_COMPLETE;
//            ((MeCommandToken*) parm)->callback(&MEC(btEvent));
            Me_AudioRemoveOp();
            break;

            /*
             * The following commands should not exist because they would be handled
             * by an ACL disconnect
             */
         default:
            break;
    }

}


/*****************************************************************************
 * FUNCTION
 *  MeFailInProgressCommands
 * DESCRIPTION
 *  Fail commands in progress indicated by ME(opmask).
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeFailInProgressCommands(void)
{
    /* Check if an inquiry is in progress */
    if (MEC(opMask) & BOM_INQUIRY)
    {
        /* Complete it */
        ReportInquiryComplete();
    }

    /* Check if an inquiry cancel is in progress */
    if (MEC(opMask) & BOM_CANCEL_INQUIRY)
    {
        /* Complete it */
        MeReportInqCancelComplete();
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeSendHciCommand
 * DESCRIPTION
 *  Send an HCI command. We can only send one HCI command at a time
 *  so they all use the same HciCommand structure.
 * PARAMETERS
 *  opCode      [IN]        
 *  parmLen     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus MeSendHciCommandRaw(HciCommandType opCode, U8 parmLen)
{
    if(MEC(commandPending) != FALSE)
    {
        Assert(0);
    }
    /* Report(("opCode:%04x", opCode)); */
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ME_HCI_COMMAND, opCode);
    MEC(commandPending) = TRUE;
    return HciTransmitCommand(opCode, parmLen, &MEC(hciGenCmd), FALSE, TRUE);
}

/*****************************************************************************
*   Init script
*****************************************************************************/
/* Return 0 means to skip this init function */
typedef U32(*MeInitFunction)(BtOpEvent event, U8 *parm);

static U32 MeInitPageTimeout(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        /* The radio's access mode is unknown. */
        MEC(accModeExpValue) = 0xff;
        /* Write page timeout (default is 5.12 seconds) */
        if(!IsEntryAvailable(&(MEC(hciGenCmd).node))){
            Assert(0);
        }
        #ifdef __BT_USE_CUST_DATA__
        StoreLE16(MEC(hciGenCmd).parms, (U32)btGetCustvalue(CUSTID_PAGE_TIMEOUT));
        #else /* __BT_USE_CUST_DATA__ */
        StoreLE16(MEC(hciGenCmd).parms, BT_DEFAULT_PAGE_TIMEOUT);
        #endif /* __BT_USE_CUST_DATA__ */
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);    
        MeSendHciCommand(HCC_WRITE_PAGE_TIMEOUT, 2);        
        break;
    case BOE_TASK_END:
        /* We just wrote the page timeout value */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_WRITE_PAGE_TIMEOUT);
        break;
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadPageTimeout(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        MeSendHciCommand(HCC_READ_PAGE_TIMEOUT, 0);      
        break;
    case BOE_TASK_END:
        /* Save Page timeout */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_STORE_PAGE_TIMEOUT, LEtoHost16((parms + 4)));
        MEC(radioPageTimeout) = LEtoHost16((parms + 4));
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadLocalVersion(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        /* Get the Bluetooth version info */
        MeSendHciCommand(HCC_READ_LOCAL_VERSION, 0);   
        break;
    case BOE_TASK_END:
        /* We now have the Bluetooth version info */
         bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_READ_VERSION_INFO);
         /* MEC(btVersion) is used in ME_SetDefaultLinkPolicy and StartScoLinkRejectTask */
         /* If invalid MEC(btVersion) is assigned, some non-correct HCI cmds may be sent to the Controller */

         if (parms[4] == 6 || parms[7] == 6){
            /* 4.0 */
            MEC(btVersion) = 6;
         }
         else if (parms[4] == 5 || parms[7] == 5)
         {
            /* 3.0 + HS */
            MEC(btVersion) = 5;
         }         
         else if (parms[4] == 4 || parms[7] == 4)
         {
             /* LMP v2.1 */
             MEC(btVersion) = 4;
         }
         else if (parms[4] == 3 || parms[7] == 3)
         {
             /* LMP v2.0 */
             MEC(btVersion) = 3;
         }
         else if (parms[4] == 2 || parms[7] == 2)
         {
             /* LMP v1.2 */
             MEC(btVersion) = 2;
         }
         else if (parms[4] >= 1 && parms[7] >= 1)
         {
             /* LMP v1.1 */
             MEC(btVersion) = 1;
         }
         else
         {
             MEC(btVersion) = 0;
             Assert(MEC(btVersion) != 0);
         }
         OS_Report("MEC(btVersion)=%u", MEC(btVersion));
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitInquiryMode(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        /* Write Inquiry Mode command */
        #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
        MEC(hciGenCmd).parms[0] = 0x02;
        #else 
        MEC(hciGenCmd).parms[0] = 0x00;
        #endif 
        bt_trace(TRACE_GROUP_1, BTLOG_WRITE_INQUIRY_MODE_PARM, MEC(hciGenCmd).parms[0]);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);    
        MeSendHciCommand(HCC_WRITE_INQ_MODE, 1);  
        break;
    case BOE_TASK_END:
        /* Write Inquiry Mode command completed */
        bt_trace(TRACE_GROUP_1, BTLOG_WRITE_INQUIRY_MODE_COMPLETED);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitInquiryScanActivity(BtOpEvent event, U8 *parms){
    U16 interval, window;
    
    switch(event){
    case BOE_START:
        #ifdef __BT_USE_CUST_DATA__
        interval = (U32)btGetCustvalue(CUSTID_INQUIRY_SCAN_INTERVAL);
        window = (U32)btGetCustvalue(CUSTID_INQUIRY_SCAN_WINDOW);
        StoreLE16(parms, interval);
        StoreLE16(parms+2, window);
        BT_XFile_EncryptionCommand(parms, 4);
        MeSendHciCommand(HCC_WRITE_INQ_SCAN_ACTIVITY, 4);  
        #else
        return 0;
        #endif
        break;
    case BOE_TASK_END:
        /* Write Inquiry Mode command completed */
        bt_trace(TRACE_GROUP_1, BTLOG_WRITE_INQUIRY_MODE_COMPLETED);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitPageScanActivity(BtOpEvent event, U8 *parms){
    U16 interval, window;
    
    switch(event){
    case BOE_START:
        #ifdef __BT_USE_CUST_DATA__
        interval = (U32)btGetCustvalue(CUSTID_PAGE_SCAN_INTERVAL);
        window = (U32)btGetCustvalue(CUSTID_PAGE_SCAN_WINDOW);
        StoreLE16(parms, interval);
        StoreLE16(parms+2, window);
        BT_XFile_EncryptionCommand(parms, 4);
        MeSendHciCommand(HCC_WRITE_PAGE_SCAN_ACTIVITY, 4);  
        #else
        return 0;
        #endif
        break;
    case BOE_TASK_END:
        /* Write Inquiry Mode command completed */
        bt_trace(TRACE_GROUP_1, BTLOG_WRITE_INQUIRY_MODE_COMPLETED);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitSSPMode(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        /* Write Inquiry Mode command */
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
        MEC(hciGenCmd).parms[0] = 0x01;
        #else 
        MEC(hciGenCmd).parms[0] = 0x00;
        #endif 
        bt_trace(TRACE_GROUP_1, BTLOG_WRITE_SIMPLE_PAIRING_MODE_PARM, MEC(hciGenCmd).parms[0]);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
        MeSendHciCommand(HCC_WRITE_SIMPLE_PAIRING_MODE, 1);
        break;
    case BOE_TASK_END:
        /* Write Inquiry Mode command completed */
        bt_trace(TRACE_GROUP_1, BTLOG_WRITE_SIMPLE_PAIRING_MODE_COMPLETED);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadLocalFeature(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        /* Get the Bluetooth features info */
        MeSendHciCommand(HCC_READ_LOCAL_FEATURES, 0);  
        break;
    case BOE_TASK_END:
        /* We now have the Bluetooth features info */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_RECEIVED_FEATURE_INFO);
        OS_MemCopy(MEC(btFeatures), parms + 4, 8);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitEventMask(BtOpEvent event, U8 *parms){
    #if defined (__BT_2_1_ENABLED__)
    const U8 event_mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    #endif 
    switch(event){
    case BOE_START:
        #if defined (__BT_2_1_ENABLED__)
        OS_MemCopy(MEC(hciGenCmd).parms, event_mask, 8);
        bt_trace(TRACE_GROUP_1, BTLOG_POWERON_SET_EVENT_MASK);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 8);
        MeSendHciCommand(HCC_SET_EVENT_MASK, 8); 
        #else
        return 0;
        #endif
        break;
    case BOE_TASK_END:
        bt_trace(TRACE_GROUP_1, BTLOG_SET_EVENT_MASK_COMMAND_COMPLETED);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadBDAddress(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_READ_ADDR);
        MeSendHciCommand(HCC_READ_BD_ADDR, 0);
        break;
    case BOE_TASK_END:
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_STORING_ADDR);
        OS_MemCopy(MEC(bdaddr).addr, parms + 4, 6);
        #ifdef BTMTK_ON_WISE
        MEC(btChipVer) = BtRadio_Get_Version();
        #else
        MEC(btChipVer) = 0x00;
        #endif
        bt_trace(TRACE_GROUP_1, MECBTCHIPVERxD, MEC(btChipVer));
        #ifdef BTMTK_ON_WISE
        MEC(btLMPVer) = BtRadio_Get_LMP_Version();
        #else
        MEC(btLMPVer) = 0x04;
        #endif
        bt_trace(TRACE_GROUP_1, MECBTLMPVERxD, MEC(btLMPVer));
        #ifdef __BT_VER_21__
        if (MEC(btLMPVer) < 0x04)
        {
            Assert(0);
        }
        #endif /* __BT_VER_21__ */
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitPairRspTimeout(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        bt_trace(TRACE_GROUP_1, SET_PIN_CODE_RSP_TIMEOUT_VALUE);
        /*
         *  Using Ceva's command to set PIN code timeout: 
         *  The HCI cmd raw data(LittleEndian first): HCI cmd(0x01),opcode(0x66FC),parmLen(0x04),LinkkeyRspTime(0x0010),PinCodeRspTime(0x409C) 
         *  raw data sent: 0166fc040010409c
         *  parms[0]parms[1] : Link key rsp timeout
         *  parms[2]parms[3] : Pin code rsp timeout 
         */
        
        MEC(hciGenCmd).parms[0] = 0x00;
        MEC(hciGenCmd).parms[1] = 0x10;
        MEC(hciGenCmd).parms[2] = 0x40;
        MEC(hciGenCmd).parms[3] = 0x9C; /* PinCodeRspTimeOut value = 625us * ox9C40 = 25sec */
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
        MeSendHciCommand(HCC_SET_PINCODE_RSP_TIMEOUT, 4);
        break;
    case BOE_TASK_END:
        bt_trace(TRACE_GROUP_1, RECV_SET_PIN_CODE_RSP_TIMEOUT_VALUE_WITH_ERR0);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitWriteLEHostSupport(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        if((U32)btGetCustvalue(CUSTID_LE_SUPPORT) == 1){
            MEC(hciGenCmd).parms[0] = 0x01;
            MEC(hciGenCmd).parms[1] = 0x00;            
            OS_Report("HCC_WRITE_LE_HOST_SUPPORT: LE_Supported_Host=%u, Simultaneous_LE_Host=%u",
                MEC(hciGenCmd).parms[0], MEC(hciGenCmd).parms[1]);            
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(HCC_WRITE_LE_HOST_SUPPORT, 2);
        }else{
            return 0;
        }
        break;
    case BOE_TASK_END:
        OS_Report("HCC_WRITE_LE_HOST_SUPPORT done");
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadProprietaryFeature(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        OS_Report("HCC_READ_PROPRIETARY_EXTENDED_FEATURES");
        /* Read Page 1 */
        MEC(hciGenCmd).parms[0] = 0x01;
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
        MeSendHciCommand(HCC_READ_PROPRIETARY_EXTENDED_FEATURES, 1);
        break;
    case BOE_TASK_END:
        OS_Report("HCC_READ_PROPRIETARY_EXTENDED_FEATURES done");
        OS_MemCopy(MEC(proprietaryFeatures), parms+6, 8);
        OS_Report("%u, %u, %u, %u, %u, %u, %u, %u"
            , *(parms+6)
            , *(parms+7)
            , *(parms+8)
            , *(parms+9)
            , *(parms+10)
            , *(parms+11)
            , *(parms+12)
            , *(parms+13));
        if(OS_GetBitsValue(MEC(proprietaryFeatures), MTK_FEATURE_FM_OVER_CONTROLLER, 1)){
            OS_SetSystemPropertyUint("fmoverbt", 1, 0);
        }else{
            OS_SetSystemPropertyUint("fmoverbt", 0, 0);
        }
        if(OS_GetBitsValue(MEC(proprietaryFeatures), MTK_FEATURE_UNIFY_IP, 1)){
            OS_SetSystemPropertyUint("sconocarescattnet", 1, 0);
        }else{
            OS_SetSystemPropertyUint("sconocarescattnet", 0, 0);
        }        
        break;
    case BOE_TASK_ERR:
        OS_Report("HCC_READ_MTK_FEATURE failed : status=%u", *(parms+3));
        break;        
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadLocalCommand(BtOpEvent event, U8 *parms){
    switch(event){
    case BOE_START:
        MeSendHciCommand(HCC_READ_LOCAL_COMMANDS, 0);  
        break;
    case BOE_TASK_END:        
        OS_Report("ME: Received Bluetooth command info");
        OS_MemCopy(MEC(btCommands), parms + 4, 64);
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

static U32 MeInitReadLocalCodec(BtOpEvent event, U8 *parms){
    U8 idx = 0;
    switch(event){
    case BOE_START:
        OS_MemSet(MEC(btCodecs), 0xff, NUM_SCO_CODECS);
        if (MEC(btCommands[29]) & 0x20)
        {
            MeSendHciCommand(HCC_READ_LOCAL_CODECS, 0);  
            break;
        }
        else
        {
            return 0;
        }
    case BOE_TASK_END:
        OS_Report("ME: Received Bluetooth codec info, codec_num=%u, vendor_codec_num=%u", 
            *(parms + 4), *(parms + 5 + *(parms + 4)));     
        while (idx < *(parms + 4))
        {
            OS_Report("ME: codec_id=%u", *(parms + 5 + idx));            
            if (idx < NUM_SCO_CODECS)
            {
                MEC(btCodecs)[idx] = *(parms + 5 + idx);
            }
            idx++;
        }      
        break;    
    default:
        Assert(0);
        return 0;
    }
    return 1;
}

struct{
    MeInitFunction pfn_init;
} MeInitScript[] = 
{
    { MeInitReadLocalCommand },
    { MeInitPageTimeout },
    { MeInitReadPageTimeout },
    { MeInitReadLocalVersion },
    { MeInitInquiryMode },
    { MeInitInquiryScanActivity },
    { MeInitPageScanActivity },
    { MeInitSSPMode },
    { MeInitReadLocalFeature },
    { MeInitEventMask },
    { MeInitReadBDAddress },
#if !defined(__MTK_BT_EXTERNAL_PLATFORM__)
    { MeInitPairRspTimeout },
    { MeInitLMPExtFeature },
    { MeInitReadProprietaryFeature },
#endif
};

/*****************************************************************************
 * FUNCTION
 *  MeInitRadio
 * DESCRIPTION
 *  Write and read radio values. When done initialization is over.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeInitRadio(BtOpEvent event)
{
    BtOperation busyOp;

#if defined (__BT_2_1_ENABLED__)
    const U8 event_mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif 
    bt_prompt_trace(MOD_BT, "MeInitRadio(%d), task state=%d", event, MEC(taskState));
    switch (event)
    {
        case BOE_START:
            /* Start the first operation */
            MEC(taskHandler) = HandleCompleteTaskRadioParms;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            MEC(taskState) = 0;

#if 0
            /* The radio's access mode is unknown. */
            MEC(accModeExpValue) = 0xff;
            /* Write page timeout (default is 5.12 seconds) */
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }
            #ifdef __BT_USE_CUST_DATA__
            StoreLE16(MEC(hciGenCmd).parms, (U16)btGetCustvalue(CUSTID_PAGE_TIMEOUT));
            #else /* __BT_USE_CUST_DATA__ */
            StoreLE16(MEC(hciGenCmd).parms, BT_DEFAULT_PAGE_TIMEOUT);
            #endif /* __BT_USE_CUST_DATA__ */
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);    
            MeSendHciCommand(HCC_WRITE_PAGE_TIMEOUT, 2);
            return;
#endif
        case BOE_TASK_END:
            if(MEC(taskState) < (sizeof(MeInitScript)/sizeof(MeInitScript[0])))
                bt_prompt_trace(MOD_BT, "[INIT] execute %u task start", MEC(taskState));
            while(MEC(taskState) < (sizeof(MeInitScript)/sizeof(MeInitScript[0])) 
                && MeInitScript[MEC(taskState)].pfn_init(BOE_START, MEC(hciGenCmd).parms) == 0){
                bt_prompt_trace(MOD_BT, "[INIT] task %u return 0", MEC(taskState));
                MEC(taskState)++;
            }
            if(MEC(taskState) == (sizeof(MeInitScript)/sizeof(MeInitScript[0]))){
                bt_prompt_trace(MOD_BT,"[INIT] init complete : MEC(taskState)=%u", MEC(taskState));
                MEC(stackState) = BTSS_INITIALIZED;

                #if BT_SECURITY == XA_ENABLED
                /* Open the device database */
                if (DDB_Open() != BT_STATUS_SUCCESS)
                {
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_OPEN_DDB_FAILED);
                }
                if (DDB_OpenBTDB(BTDB_BTDEV_INFO) != BT_STATUS_SUCCESS){
                    OS_Report("[DB] Opend BTDB_BTDEV_INFO DB failed");
                }
                #endif /* BT_SECURITY == XA_ENABLED */
                DDB_COD_Open(); /* DDB_COD */
                /*
                 * We're done so stack is initialized. Report it to all 
                 * handlers 
                 */
                if(MEC(curOp) != 0)
                {
                    Assert(0);
                }
                InitializeListEntry(&(busyOp.node));
                busyOp.opType = BOP_BUSY;
                MEC(curOp) = &busyOp;
                MEC(btEvent).errCode = BT_STATUS_SUCCESS;
                MEC(btEvent).eType = BTEVENT_HCI_INITIALIZED;
                MeReportNMResults();
                MEC(curOp) = 0;
                /* Update the class of device */
                MeWriteClassOfDevice();
                #ifdef __BT_3_0_HS__
                ME_ReadDataBlockSize();
                #endif
                /* Init default link policy */
                ME_SetDefaultLinkPolicy(BLP_MASTER_SLAVE_SWITCH | BLP_SNIFF_MODE, BLP_MASTER_SLAVE_SWITCH | BLP_SNIFF_MODE);

                #if defined (__BT_MT66XX_HCI_RETRANSMISSION__)
                MeEnabledMT66xxFlowControlOn();
                #endif
                /*
                 * Set the default Accessibility modes. Force the stack to 
                 * apply them since we cannot assume default radio accessibility.
                 */
                MEC(accModeCur) = BAM_NOT_ACCESSIBLE;
                MEC(accModeExpValue) = BAM_NOT_ACCESSIBLE;
                #ifdef __BT_LE_STANDALONE__
                (void)ME_SetInitAccessibleModeNC(BAM_NOT_ACCESSIBLE);
                #else
                if (BTSysAdpCheckNotTestMode())
                {
                    (void)ME_SetAccessibleModeNC(BAM_NOT_ACCESSIBLE);
                }
                else
                {
                    (void)ME_SetAccessibleModeNC(BAM_GENERAL_ACCESSIBLE);
                }
                #endif
                #ifdef __BT_4_0_BLE__
                ME_LE_SetAccessible(BAM_NOT_ACCESSIBLE, NULL);  /* Wait for name config */
                LeHciReadWhiteListSize();
                #ifdef __BT_4_0_PRIVACY__
                #if defined (__BT_LE_STANDALONE__) && !defined (__BT_4_0_PRIVACY_RECON_ADDR__)
                leUtilGenRandomPrivateResolvableAddress(&btCtxLe.random_bdaddr);
                //LeHciSetRandomAddress(btCtxLe.random_bdaddr);
                #else
                leUtilGenRandomPrivateNonResolvableAddress(&btCtxLe.random_bdaddr);
                LeHciSetRandomAddress(btCtxLe.random_bdaddr);
                #endif
                #endif
                #endif
                /* Attempt to start the operation, it may not be in progress. */
                /* Move this forward to any future version of MeInitRadio */
                MeStartOperation();
                /* Radio Init ok, send power on cnf to MMI */
                BTSysAdpPowerOnCnf(TRUE);

            }
#if 0                 
            MEC(taskState)++;
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                bt_prompt_trace(MOD_BT, "+MEC(hciGenCmd).node is NULL");
                Assert(0);
                bt_prompt_trace(MOD_BT, "-MEC(hciGenCmd).node is NULL");
            }
            switch (MEC(taskState))
            {
                case 1:
                    /* Perform next operation */
                    MeSendHciCommand(HCC_READ_PAGE_TIMEOUT, 0);
                    break;

                case 2:
                    /* Get the Bluetooth version info */
                    MeSendHciCommand(HCC_READ_LOCAL_VERSION, 0);
                    break;
                case 3:
                    /* Write Inquiry Mode command */
            #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                    MEC(hciGenCmd).parms[0] = 0x02;
            #else 
                    MEC(hciGenCmd).parms[0] = 0x00;
            #endif 
                    bt_trace(TRACE_GROUP_1, BTLOG_WRITE_INQUIRY_MODE_PARM, MEC(hciGenCmd).parms[0]);
                    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);    
                    MeSendHciCommand(HCC_WRITE_INQ_MODE, 1);
                    break;
                case 4:
                    /* Write Inquiry Mode command */
            #if defined (__BT_2_1_SIMPLE_PAIRING__)
                    MEC(hciGenCmd).parms[0] = 0x01;
            #else 
                    MEC(hciGenCmd).parms[0] = 0x00;
            #endif 
                    bt_trace(TRACE_GROUP_1, BTLOG_WRITE_SIMPLE_PAIRING_MODE_PARM, MEC(hciGenCmd).parms[0]);
                    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
                    MeSendHciCommand(HCC_WRITE_SIMPLE_PAIRING_MODE, 1);
                    break;

                case 5:
                    /* Get the Bluetooth features info */
                    MeSendHciCommand(HCC_READ_LOCAL_FEATURES, 0);
                    break;
            #if defined (__BT_2_1_ENABLED__)
                case 6:
                    OS_MemCopy(MEC(hciGenCmd).parms, event_mask, 8);
                    bt_trace(TRACE_GROUP_1, BTLOG_POWERON_SET_EVENT_MASK);
                    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 8);
                    MeSendHciCommand(HCC_SET_EVENT_MASK, 8);
                    break;
            #endif /* defined (__BT_2_1_ENABLED__) */ 
                case 7:
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_READ_ADDR);
                    MeSendHciCommand(HCC_READ_BD_ADDR, 0);
                    break;
                case 8:
                    bt_trace(TRACE_GROUP_1, SET_PIN_CODE_RSP_TIMEOUT_VALUE);
                    /*
                     *  Using Ceva's command to set PIN code timeout: 
                     *  The HCI cmd raw data(LittleEndian first): HCI cmd(0x01),opcode(0x66FC),parmLen(0x04),LinkkeyRspTime(0x0010),PinCodeRspTime(0x409C) 
                     *  raw data sent: 0166fc040010409c
                     *  parms[0]parms[1] : Link key rsp timeout
                     *  parms[2]parms[3] : Pin code rsp timeout 
                     */

                    MEC(hciGenCmd).parms[0] = 0x00;
                    MEC(hciGenCmd).parms[1] = 0x10;
                    MEC(hciGenCmd).parms[2] = 0x40;
                    MEC(hciGenCmd).parms[3] = 0x9C; /* PinCodeRspTimeOut value = 625us * ox9C40 = 25sec */
                    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
                    MeSendHciCommand(HCC_SET_PINCODE_RSP_TIMEOUT, 4);
                    break;
                case 9:
#ifdef __BT_4_0_BLE__
                    OS_Report("HCC_SET_LMP_EXTENDED_FEATURE: 0x3");
                    OS_MemSet(&MEC(hciGenCmd).parms[0], 0x0, 8);
                    MEC(hciGenCmd).parms[0] = 0x03;
                    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 8);
                    MeSendHciCommand(HCC_SET_LMP_EXTENDED_FEATURE, 8);
                    break;
#else
                    MEC(taskState)++;
#endif
                case 10:
                    MEC(stackState) = BTSS_INITIALIZED;

            #if BT_SECURITY == XA_ENABLED
                    /* Open the device database */
                    if (DDB_Open() != BT_STATUS_SUCCESS)
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_OPEN_DDB_FAILED);
                    }
            #endif /* BT_SECURITY == XA_ENABLED */
                    DDB_COD_Open(); /* DDB_COD */
                    /*
                     * We're done so stack is initialized. Report it to all 
                     * handlers 
                     */
                    if(MEC(curOp) != 0)
                    {
                        Assert(0);
                    }
                    InitializeListEntry(&(busyOp.node));
                    busyOp.opType = BOP_BUSY;
                    MEC(curOp) = &busyOp;
                    MEC(btEvent).errCode = BT_STATUS_SUCCESS;
                    MEC(btEvent).eType = BTEVENT_HCI_INITIALIZED;
                    MeReportNMResults();
                    MEC(curOp) = 0;
                    /* Update the class of device */
                    MeWriteClassOfDevice();
#ifdef __BT_3_0_HS__
                    ME_ReadDataBlockSize();
#endif

#if defined (__BT_MT66XX_HCI_RETRANSMISSION__)
                    MeEnabledMT66xxFlowControlOn();
#endif
                    /*
                     * Set the default Accessibility modes. Force the stack to 
                     * apply them since we cannot assume default radio accessibility.
                     */
                    MEC(accModeCur) = BAM_NOT_ACCESSIBLE;
                    MEC(accModeExpValue) = BAM_NOT_ACCESSIBLE;
#ifdef __BT_LE_STANDALONE__
                    (void)ME_SetInitAccessibleModeNC(BAM_NOT_ACCESSIBLE);
#else
                    if (BTSysAdpCheckNotTestMode())
                    {
                        (void)ME_SetAccessibleModeNC(BAM_NOT_ACCESSIBLE);
                    }
                    else
                    {
                        (void)ME_SetAccessibleModeNC(BAM_GENERAL_ACCESSIBLE);
                    }
#endif
#ifdef __BT_4_0_BLE__
                    ME_LE_SetAccessible(BAM_NOT_ACCESSIBLE, NULL);  /* Wait for name config */
  #ifdef __BT_4_0_PRIVACY__
    #if defined (__BT_LE_STANDALONE__) && !defined (__BT_4_0_PRIVACY_RECON_ADDR__)
                    leUtilGenRandomPrivateResolvableAddress(&btCtxLe.random_bdaddr);
                    //LeHciSetRandomAddress(btCtxLe.random_bdaddr);
    #else
                    leUtilGenRandomPrivateNonResolvableAddress(&btCtxLe.random_bdaddr);
                    LeHciSetRandomAddress(btCtxLe.random_bdaddr);
    #endif
  #endif
#endif
                    /* Attempt to start the operation, it may not be in progress. */
                    /* Move this forward to any future version of MeInitRadio */
                    MeStartOperation();
                    /* Radio Init ok, send power on cnf to MMI */
                    BTSysAdpPowerOnCnf(TRUE);
                    break;

                default:
                    Assert(0);
            }
#endif
            break;

        default:
            Assert(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  HandleCompleteTaskRadioParms
 * DESCRIPTION
 *  Handle HCI events for the Inquiry task.
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleCompleteTaskRadioParms(U8 event, U8 len, U8 *parm)
{
    BtErrorCode errCode = 0;

    /* UNUSED_PARAMETER(len); */

    switch (event)
    {
        case HCE_COMMAND_COMPLETE:
            /* Save the status in case it is an error */
            errCode = parm[3];
            break;

        case HCE_COMMAND_STATUS:
            /* Save the status in case it is an error */
            errCode = parm[0];
            break;

        default:
            Assert(0);
    }

    /* Process the result */
    if (errCode == 0)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_MECTASKSTATEx02X, MEC(taskState));
        /* Result is successful so store the radio parameter */
        bt_prompt_trace(MOD_BT,"[INIT] execute %d task end", MEC(taskState));
        MeInitScript[MEC(taskState)].pfn_init(BOE_TASK_END, parm);
        MEC(taskState)++;
#if 0    
        switch (MEC(taskState))
        {
            case 0:
                /* We just wrote the page timeout value */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_WRITE_PAGE_TIMEOUT);
                break;

            case 1:
                /* Save Page timeout */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_STORE_PAGE_TIMEOUT, LEtoHost16((parm + 4)));
                MEC(radioPageTimeout) = LEtoHost16((parm + 4));
                break;
            case 2:
                /* We now have the Bluetooth version info */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_READ_VERSION_INFO);
                /* MEC(btVersion) is used in ME_SetDefaultLinkPolicy and StartScoLinkRejectTask */
                /* If invalid MEC(btVersion) is assigned, some non-correct HCI cmds may be sent to the Controller */
                
                if (parm[4] == 4 || parm[7] == 4)
                {
                    /* LMP v2.1 */
                    MEC(btVersion) = 4;
                }
                else if (parm[4] == 3 || parm[7] == 3)
                {
                    /* LMP v2.0 */
                    MEC(btVersion) = 3;
                }
                else if (parm[4] == 2 || parm[7] == 2)
                {
                    /* LMP v1.2 */
                    MEC(btVersion) = 2;
                }
                else if (parm[4] >= 1 && parm[7] >= 1)
                {
                    /* LMP v1.1 */
                    MEC(btVersion) = 1;
                }
                else
                {
                    MEC(btVersion) = 0;
                }
                break;
            case 3:
                /* Write Inquiry Mode command completed */
                bt_trace(TRACE_GROUP_1, BTLOG_WRITE_INQUIRY_MODE_COMPLETED);
                break;
            case 4:
                /* Write Inquiry Mode command completed */
                bt_trace(TRACE_GROUP_1, BTLOG_WRITE_SIMPLE_PAIRING_MODE_COMPLETED);
                break;
            case 5:
                /* We now have the Bluetooth features info */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_RECEIVED_FEATURE_INFO);
                OS_MemCopy(MEC(btFeatures), parm + 4, 8);
            #if !defined (__BT_2_1_ENABLED__)
                MEC(taskState)++;
            #endif 
                break;
            case 6:
                bt_trace(TRACE_GROUP_1, BTLOG_SET_EVENT_MASK_COMMAND_COMPLETED);
                break;
            case 7:
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_STORING_ADDR);
                OS_MemCopy(MEC(bdaddr).addr, parm + 4, 6);
#ifdef BTMTK_ON_WISE
                MEC(btChipVer) = BtRadio_Get_Version();
#else
                MEC(btChipVer) = 0x00;
#endif
                bt_trace(TRACE_GROUP_1, MECBTCHIPVERxD, MEC(btChipVer));
#ifdef BTMTK_ON_WISE
                MEC(btLMPVer) = BtRadio_Get_LMP_Version();
#else
                MEC(btLMPVer) = 0x04;
#endif
                bt_trace(TRACE_GROUP_1, MECBTLMPVERxD, MEC(btLMPVer));
            #ifdef __BT_VER_21__
                if (MEC(btLMPVer) < 0x04)
                {
                    Assert(0);
                }
            #endif /* __BT_VER_21__ */ 
                break;

            case 8:
                bt_trace(TRACE_GROUP_1, RECV_SET_PIN_CODE_RSP_TIMEOUT_VALUE_WITH_ERR0);
                break;
            case 9:
                OS_Report("HCC_SET_LMP_EXTENDED_FEATURE done");
                break;
            default:
                Assert(0);
        }
#endif        
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_ERROR_CODE, MEC(curTask), errCode);
    }
    /* can't end init radio task until we get the buffer back */
    if (!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Report(("init command node not available"));
        MEC(opMask) |= BOM_INITRADIO_BUSY;
        return;
    }
    MeInitRadio(BOE_TASK_END);
}


/*****************************************************************************
 * FUNCTION
 *  MePendingCommandHandler
 * DESCRIPTION
 *  Checks for pending command.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MePendingCommandHandler(EvmTimer *timer)
{
    U32 count;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_ME_WAIT_PENDING_CMD);

    /* If the event hasn't arrived, continue waiting */
    count = (U32) timer->context;
    if (count > 0)
    {
        if (ME_HasPendingTask() != 0)
        {
            count--;
            timer->context = (void*)count;
            timer->func = MePendingCommandHandler;
            EVM_StartTimer(timer, 50);
            return;
        }
    }

    (void)ME_RadioShutdownTime(0);
}


/*****************************************************************************
 * FUNCTION
 *  MeRemovePendingRemoteOperation
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeRemovePendingRemoteOperation(BtRemoteDevice *remDev)
{

    int i = 0;

    /* to remove scoop of the disconnected remote device: 2006-10-19 */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        BtScoConnect *sconn = &MEC(scoTable)[i];

        if (sconn->scord == remDev)
        {
            if (IsNodeOnList(&MEC(opList), &(sconn->scoop.node)))
            {
                RemoveEntryList(&(sconn->scoop.node));
            }
        }

    }

    /*
     * Check the secOp in secopTable, each secOp belongs to this remDev and in opList: 
     * must be removed due to disconnect happens for this remDev
     */
    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if ((MEC(secOpTable)[i].remDev == remDev) &&
            (OS_MemCmp(MEC(secOpTable)[i].remDev->bdAddr.addr, 6, remDev->bdAddr.addr, 6)))
        {
            if (IsNodeOnList(&MEC(opList), &(MEC(secOpTable)[i].op.node)))
            {
                RemoveEntryList(&(MEC(secOpTable)[i].op.node));
                MEC(secOpTable)[i].op.opType = BOP_NOP;
            }
        }
    }

    if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
    {
        RemoveEntryList(&(remDev->op.node));
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->policyOp.node)))
    {
        RemoveEntryList(&(remDev->policyOp.node));
        remDev->policyOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->roleOp.node)))
    {
        RemoveEntryList(&(remDev->roleOp.node));
        remDev->roleOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->cancelOp.node)))
    {
        RemoveEntryList(&(remDev->cancelOp.node));
        remDev->cancelOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->packetTypeOp.node)))
    {
        RemoveEntryList(&(remDev->packetTypeOp.node));
        remDev->packetTypeOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteFeatureOp.node)))
    {
        RemoveEntryList(&(remDev->remoteFeatureOp.node));
        remDev->remoteFeatureOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->supervisionOp.node)))
    {
        RemoveEntryList(&(remDev->supervisionOp.node));
        remDev->supervisionOp.opType = BOP_NOP;
    }
#if defined (__BT_2_1_AUTO_FLUSH__)
    if (IsNodeOnList(&MEC(opList), &(remDev->automaticallyFlushedOp.node)))
    {
        RemoveEntryList(&(remDev->automaticallyFlushedOp.node));
        remDev->automaticallyFlushedOp.opType = BOP_NOP;
    }
#endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 
#if defined (__BT_2_1_SNIFF_SUBRATING__)
    if (IsNodeOnList(&MEC(opList), &(remDev->sniffSubrateOp.node)))
    {
        RemoveEntryList(&(remDev->sniffSubrateOp.node));
        remDev->sniffSubrateOp.opType = BOP_NOP;
    }
#endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 
#if defined (__BT_3_0_HS__)
    

    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.createPhysicalLinkOp.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.createPhysicalLinkOp.node));
        remDev->remoteWifiAMP.createPhysicalLinkOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.acceptPhysicalLinkOp.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.acceptPhysicalLinkOp.node));
        remDev->remoteWifiAMP.acceptPhysicalLinkOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.disconnectPhysicalLinkOp.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.disconnectPhysicalLinkOp.node));
        remDev->remoteWifiAMP.disconnectPhysicalLinkOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.createLogicalLinkOp.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.createLogicalLinkOp.node));
        remDev->remoteWifiAMP.createLogicalLinkOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.acceptLogicalLinkOp.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.acceptLogicalLinkOp.node));
        remDev->remoteWifiAMP.acceptLogicalLinkOp.opType = BOP_NOP;
    }
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.logicalLinkCancelOp.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.logicalLinkCancelOp.node));
        remDev->remoteWifiAMP.logicalLinkCancelOp.opType = BOP_NOP;
    }    
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.readLocalAmpInfo.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.readLocalAmpInfo.node));
        remDev->remoteWifiAMP.readLocalAmpInfo.opType = BOP_NOP;
    }    

    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.readLocalAmpAssoc.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.readLocalAmpAssoc.node));
        remDev->remoteWifiAMP.readLocalAmpAssoc.opType = BOP_NOP;
    }    

    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.writeRemoteAmpAssoc.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.writeRemoteAmpAssoc.node));
        remDev->remoteWifiAMP.writeRemoteAmpAssoc.opType = BOP_NOP;
    }    
    if (IsNodeOnList(&MEC(opList), &(remDev->remoteWifiAMP.readDataBlockSize.node)))
    {
        RemoveEntryList(&(remDev->remoteWifiAMP.readDataBlockSize.node));
        remDev->remoteWifiAMP.readDataBlockSize.opType = BOP_NOP;
    }   
#endif /* defined (__BT_3_0_HS__) */ 

}


/*****************************************************************************
 * FUNCTION
 *  MeAssignTaskHandler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  command_type        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeAssignTaskHandler(U8 command_type)
{
    MEC(taskHandler) = MeHandleCompleteTask;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }

}

static void MeHandleErrorTaskEnd(void)
{
    Assert(0);
}



void MeSetEventHandler(void)
{
    MEC(taskHandler) = MeHandleCompleteTask;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
}

ListEntry *MeGetHciOpList(void)
{
    return &MEC(opList);
}

HciCommand *MeGetHciCommand(void)
{
    return &MEC(hciGenCmd);
}

BtEvent *MeGetHciEvent(void)
{
    return &MEC(btEvent);
}

void MeSetCurrentTask(U16 opcode)
{
    MeSendHciCommand(opcode, 8);

}

