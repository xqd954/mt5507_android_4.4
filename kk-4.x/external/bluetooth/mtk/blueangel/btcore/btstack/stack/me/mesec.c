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
 *     $Workfile:mesec.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the code for the Bluetooth Security
 *     Manager portion of the Management Entity.
 *
 * Created:
 *     January 31, 2000
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
#include "sys/l2capxp.h"
#include "sys/sdpi.h"
#include "sys/rfcexp.h"
#include "ddb.h"
#include "bt_feature.h"
#include "x_file.h"
#include "stdlib.h"
#ifdef __BT_4_0_BLE__
#include "btconfig_le.h"
#include "le_adp.h"
#endif
#include "btbm.h"
#include "a2mp_ctl.h"
#include "a2mp_sec.h"
/****************************************************************************
 *
 * Macros
 *
 ****************************************************************************/

/* Macro for accessing a RemoteDevice structure given the curOp */

#define RDEV ((BtRemoteDevice*)MEC(curOp))
#define SECOP ((BtSecurityOp*)MEC(curOp))

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

static void HandleSecurityCommandComplete(U8 event, U8 len, U8 *parm);
static void StartLinkKeyRspTask(void);
static void StartPinCodeRspTask(void);
static void StartPinCodeRspByAddrTask(void);

#if defined (__BT_2_1_SIMPLE_PAIRING__)
static void StartIoCapabilityRspByAddrTask(void);
static void StartUserConfirmRspByAddrTask(void);
static void StartDeleteStoredLinkKeyTask(void);
static void StartUserOobDataRspTask(void);

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

static void StartAuthenticateTask(void);
static void StartEncryptionTask(void);
static void StartWriteAuthenticateEnableTask(void);
static void StartWriteEncryptModeTask(void);

static void SecHandleErrorTaskEnd(void);

#if BT_SECURITY == XA_DISABLED
static void StartDenyPinCodeRspTask(void);
#endif 

extern void Me2BMCallback(const BtEvent *Event);
extern U16 BT_XFile_EncryptionCommandOpcode(U16 opcode);

#if BT_SECURITY == XA_ENABLED
/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  MeHandleMonitorPairResultTimeoutHandler
 * DESCRIPTION
 *  Handle the acceptor's non authentication event notification cases
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleMonitorPairResultTimeoutHandler(EvmTimer* timer)
{
    BtRemoteDevice* timeout_link = (BtRemoteDevice*)timer->context;
    timer->func = 0;
    bt_trace(TRACE_GROUP_1,BT_LOG_MPR_TIMEOUT_LINKSTATE_AUTHSTATE,timeout_link->state,timeout_link->authState);
    if (timeout_link->state == BDS_CONNECTED)
    {
        MeDisconnectLink(timeout_link, 8);
    }
    else
    {
        bt_trace(TRACE_GROUP_1,BT_LOG_MPR_TIMEOUT_LINK_NOT_CONNECTED);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleAuthenticateOp
 * DESCRIPTION
 *  Handle the authenticate operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleAuthenticateOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartAuthenticateTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;

            /* This is the connection status event */
            OS_Report("SecHandleAuthenticateOp : authState==%u, err=%u", SECOP->remDev->authState, MEC(btEvent).errCode);
            if(SECOP->remDev->authState == BAS_START_AUTHENTICATE){
                if (MEC(btEvent).errCode != 0)
                {
                    SecReportAuthenticateResult(SECOP->remDev);
                }
                else
                {
                    /*
                     * We are in the process of authentication. We expect to get
                     * a request for a link key so set the authState accordingly 
                     */
                    SECOP->remDev->authState = BAS_WAITING_KEY_REQ;
                }
            }
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  SecHandleLinkKeyRspOp
 * DESCRIPTION
 *  Handle the Link key response operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleLinkKeyRspOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartLinkKeyRspTask();
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0 && SECOP->remDev != 0)    
            {
                /* Clear authentication state */
                SECOP->remDev->authState = BAS_NOT_AUTHENTICATED;
            }

            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            break;

        default:
            SecHandleErrorTaskEnd();
            break;
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  SecHandlePinCodeRspOp
 * DESCRIPTION
 *  Handle the Link key response operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandlePinCodeRspOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartPinCodeRspTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  SecHandlePinCodeByAddrRspOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandlePinCodeByAddrRspOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartPinCodeRspByAddrTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  SecHandleIoCapabilityAddrRspOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleIoCapabilityAddrRspOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartIoCapabilityRspByAddrTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  SecHandleUserConfirmByAddrRspOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleUserConfirmByAddrRspOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartUserConfirmRspByAddrTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

/*****************************************************************************
 * FUNCTION
 *  SecHandleUserOobDataRspOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleUserOobDataRspOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartUserOobDataRspTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

/*****************************************************************************
 * FUNCTION
 *  SecHandleDeleteStoredLinkKeyOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleDeleteStoredLinkKeyOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartDeleteStoredLinkKeyTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            SecProcessAllSecurityTokens(SECOP->remDev);
            break;
        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  SecHandleAuthorizeOp
 * DESCRIPTION
 *  Handle the Authorize request operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleAuthorizeOp(BtOpEvent event)
{
    SECOP->op.opType = BOP_NOP;

    switch (event)
    {
        case BOE_START:
            MEC(btEvent).errCode = BEC_NO_ERROR;
            break;

        case BOE_TASK_ERR:
            MEC(btEvent).errCode = BEC_REQUEST_CANCELLED;
            break;

        case BOE_TASK_END:
            goto done;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Call the authorize handler if it exists */
    if (MEC(authorizeHandler) != 0)
    {
        bt_trace(TRACE_GROUP_1, MESEC_CALLING_AUTHORIZATION_HANDLER);
        SecHandleAuthorizeRequest(SECOP->remDev);
    }
    else
    {
        /* No handler so were done. Process all tokens */
        /* This can cause the call stack too large. */
        bt_trace(TRACE_GROUP_1, MESEC_NO_AUTHORIZATION_HANDLER);
        SECOP->remDev->secAccessState |= BAS_AUTHORIZE_COMPLETE;
        SECOP->remDev->authorizeState = BAS_NOT_AUTHORIZED;
        SecProcessAllSecurityTokens(SECOP->remDev);
    }

  done:
    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  SecHandleAuthorizeRequest
 * DESCRIPTION
 *  Indicate the Authorize request operation.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleAuthorizeRequest(BtRemoteDevice *remDev)
{
    if (MEC(btEvent).errCode != BEC_NO_ERROR)
    {
        /* Got some error during authorization, restart. */
        remDev->secAccessState |= BAS_AUTH_COMPLETE;
        remDev->authorizeState = BAS_NOT_AUTHORIZED;
    }

    if (MEC(authorizeHandler) != 0)
    {
        MEC(btEvent).eType = BTEVENT_AUTHORIZATION_REQ;
        MEC(btEvent).p.remDev = remDev;
        MEC(btEvent).handler = MEC(authorizeHandler);
        MEC(authorizeHandler)->callback(&MEC(btEvent));
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleEncryptOp
 * DESCRIPTION
 *  Handle the Encryption request operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleEncryptOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    switch (event)
    {
        case BOE_START:
            StartEncryptionTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            SECOP->op.opType = BOP_NOP;
            remDev = SECOP->remDev;

            /* Start another operation if one exists */
            MeOperationEnd();

            /* This is the connection status event */
            if (MEC(btEvent).errCode != 0)
            {
                SecReportEncryptionResult(remDev, 0x00);
            }
            break;

        default:
            SecHandleErrorTaskEnd();
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleSecurityModeOp
 * DESCRIPTION
 *  Handle the authenticate operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleSecurityModeOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Check if this is an encryption only operation */
            if (SECOP->x.secMode.mode == BSM_SEC_ENCRYPT)
            {
                /* It is, so set the encryption mode only */
                SECOP->x.secMode.mode = BSM_SEC_LEVEL_3;
                MEC(secModeState) = BSMS_START_LEVEL3;
                MEC(opState) = 1;
                StartWriteEncryptModeTask();
                return;
            }
            MEC(opState) = 0;
            StartWriteAuthenticateEnableTask();
            return;

        case BOE_TASK_END:
            if (MEC(opState) == 0)
            {
                /* Authentication enable is complete. See if it was successful */
                if (MEC(btEvent).errCode == 0)
                {
                    /* It was successful so see if we need to set encryption mode */
                    if (SECOP->x.secMode.encrypt == TRUE)
                    {
                        /* Set the encryption mode */
                        MEC(opState) = 1;
                        StartWriteEncryptModeTask();
                        return;
                    }
                }
            }
            /*
             * If we get here then we need to report the results. First deal with the result
             * of setting the encryption state
             */
            if (MEC(opState) == 1)
            {
                /*
                 * We just finished the encryption mode task. If its was successful
                 * then set the global flag. Make sure that the errCode field is set
                 * to indication success
                 */
                if (MEC(btEvent).errCode != 0)
                {
                    /* Failed to set the encryption mode */
                    bt_trace(TRACE_GROUP_1, MECSEC_ERROR_SETTING_ENCRYPTION_MODE_ERR__xD, MEC(btEvent).errCode);
                    MEC(btEvent).errCode = 0;
                }
                else
                {
                    /* Encrypt mode operation was successful so set the flag appropriately */
                    if ((SECOP->x.secMode.mode == BSM_SEC_LEVEL_3) && (SECOP->x.secMode.encrypt == TRUE))
                    {
                        MEC(secModeEncrypt) = TRUE;
                    }
                    else
                    {
                        MEC(secModeEncrypt) = FALSE;
                    }
                }
            }

            /* Report the results */
            SECOP->op.opType = BOP_NOP;
            SecReportSecurityModeResult();
            break;

        default:
            SecHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  StartAuthenticateTask
 * DESCRIPTION
 *  Start the link accept task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartAuthenticateTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    /* Store the connection handle */
    StoreLE16(MEC(hciGenCmd).parms, SECOP->remDev->hciHandle);

    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    bt_trace(TRACE_GROUP_1, MESEC_STARTING_AUTHENTICATION_REQUESTED_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
    MeSendHciCommand(HCC_AUTH_REQ, 2);
}


/*****************************************************************************
 * FUNCTION
 *  StartLinkKeyRspTask
 * DESCRIPTION
 *  Start the link key response task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartLinkKeyRspTask(void)
{
    BtDeviceRecord record;

    /* Set the task handler */
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->bdAddr.addr, 6);
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
    if (SECOP->remDev != 0)
    {
        /* We have a remote device so see if a link key exists */
        if (SEC_FindDeviceRecord(&(SECOP->bdAddr), &record) == BT_STATUS_SUCCESS)
        {

            if (((record.paired_with_simap == FALSE) && (SECOP->remDev->simap_issue_authentication_procedure == FALSE))
                || (record.paired_with_simap == TRUE && record.pin_code_len == 16))
            {

                bt_trace(
                    TRACE_GROUP_1,
                    BT_LOG_LINK_KEY_RSP_TASK_PAIRED_LEN,
                    record.paired_with_simap,
                    record.pin_code_len);
                /* Found a record, set the authentication state and check authorization. */
                if (SECOP->remDev->authState != BAS_SENDING_KEY)
                {
                    /* remote initiate authentication: no authentication complete event, so set state here */
                    SECOP->remDev->authState = BAS_AUTHENTICATED;
                }
                SECOP->remDev->pairingType = BPT_SAVE_NOT_TRUSTED;
                /* Set the trusted flag based on the stored record */
                if (record.trusted)
                {
                    SECOP->remDev->authorizeState = BAS_AUTHORIZED;
                    SECOP->remDev->pairingType = BPT_SAVE_TRUSTED;
                }
                /* The link key was found so return it to the Host controller */
                OS_MemCopy(MEC(hciGenCmd).parms + 6, record.linkKey, 16);
                bt_trace(TRACE_GROUP_1, MESEC_KEY_EXISTS__SENDING_LINK_KEY_REQUEST_REPLY_COMMAND);
                BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 22);
                MeSendHciCommand(HCC_LINK_KEY_REQ_REPL, 22);
                return;
            }
        }
#else /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    if (SECOP->remDev != 0)
    {
        /* We have a remote device so see if a link key exists */
        if (SEC_FindDeviceRecord(&(SECOP->bdAddr), &record) == BT_STATUS_SUCCESS)
        {
            /* Found a record, set the authentication state and check authorization. */
            if (SECOP->remDev->authState != BAS_SENDING_KEY)
            {
                /* remote initiate authentication: no authentication complete event, so set state here */
                SECOP->remDev->authState = BAS_AUTHENTICATED;
            }
            SECOP->remDev->pairingType = BPT_SAVE_NOT_TRUSTED;
            /* Set the trusted flag based on the stored record */
            if (record.trusted)
            {
                SECOP->remDev->authorizeState = BAS_AUTHORIZED;
                SECOP->remDev->pairingType = BPT_SAVE_TRUSTED;
            }
            /* The link key was found so return it to the Host controller */
            OS_MemCopy(MEC(hciGenCmd).parms + 6, record.linkKey, 16);
            bt_trace(TRACE_GROUP_1, MESEC_KEY_EXISTS__SENDING_LINK_KEY_REQUEST_REPLY_COMMAND);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 22);
            MeSendHciCommand(HCC_LINK_KEY_REQ_REPL, 22);
            return;
        }
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        else
        {
            /* We need a pin code so respond in the negative */
            bt_trace(TRACE_GROUP_1, MESEC_NO_LINK_KEY__NEED_A_PIN_CODE);
        }
    }

    /* We need to respond in the negative */
    bt_trace(TRACE_GROUP_1, MESEC_SENDING_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
    MeSendHciCommand(HCC_LINK_KEY_REQ_NEG_REPL, 6);
}


/*****************************************************************************
 * FUNCTION
 *  StartPinCodeRspTask
 * DESCRIPTION
 *  Start the Pin Code response task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartPinCodeRspTask(void)
{
    /* Set the task handler */
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->bdAddr.addr, 6);

    if (SECOP->remDev != 0)
    {
        /* We have a remote device so send the pin code */
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_PIN_CODE_REQUEST_REPLY_COMMAND);
        if(!((SECOP->remDev->authState == BAS_WAITING_FOR_PIN) ||
               (SECOP->remDev->authState == BAS_WAITING_FOR_PIN_R)))
        {
            Assert(0);
        }

        if (SECOP->x.pLen == 0x00)
        {
            bt_trace(TRACE_GROUP_1, MESEC_SENDING_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
            MeSendHciCommand(HCC_PIN_CODE_REQ_NEG_REPL, 6);
            return;
        }
        /* Set the state accordingly */
        if (SECOP->remDev->authState == BAS_WAITING_FOR_PIN)
        {
            SECOP->remDev->authState = BAS_WAITING_FOR_KEY;
        }
        else
        {
            SECOP->remDev->authState = BAS_WAITING_FOR_KEY_R;
        }

        MEC(hciGenCmd).parms[6] = SECOP->x.pLen;
    #if !defined (__BT_2_1_SIMPLE_PAIRING__)
        SECOP->remDev->pin_code_len = MEC(hciGenCmd).parms[6];
        bt_trace(TRACE_GROUP_1, BT_LOG_START_PIN_CODE_RSP_TASK_PIN_LEN, SECOP->remDev->pin_code_len);
    #endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        OS_MemCopy(MEC(hciGenCmd).parms + 7, SECOP->pin, SECOP->x.pLen);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 23);
        MeSendHciCommand(HCC_PIN_CODE_REQ_REPL, 23);
    }
    else
    {
        /* We need to respond in the negative */
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
        MeSendHciCommand(HCC_PIN_CODE_REQ_NEG_REPL, 6);
    }
}


/*****************************************************************************
 * FUNCTION
 *  StartPinCodeRspByAddrTask
 * DESCRIPTION
 *  Start the Pin Code by Addr response task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartPinCodeRspByAddrTask(void)
{
    /* Set the task handler */

    BtRemoteDevice *remDev = NULL;
 
    OS_Report("StartPinCodeRspByAddrTask");
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->bdAddr.addr, 6);


    remDev = ME_FindRemoteDeviceP(SECOP->bdAddr.addr);
    if (remDev != NULL)
    {
#if !defined (__BT_2_1_SIMPLE_PAIRING__)    
        remDev->pin_code_len = SECOP->x.pLen;
        bt_trace(TRACE_GROUP_1, BT_LOG_START_PIN_CODE_RSP_BY_ADDR_TASK_LEN, remDev->pin_code_len);
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */        
    }
    else
    {
        OS_Report("StartPinCodeRspByAddrTask assert");
        Assert(0);
    }
    
    if (SECOP->x.pLen == 0x00)
    {
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
        MeSendHciCommand(HCC_PIN_CODE_REQ_NEG_REPL, 6);
      
        /* MAUI_01498320 by mtk01411 */
        /* Start a MPR(monitor pair result) timer */
        if (remDev->state == BDS_CONNECTED)
        {
            bt_trace(TRACE_GROUP_1,BT_LOG_START_MPR_TIMER_CASE,1);
            remDev->monitorPairResultTimer.func = MeHandleMonitorPairResultTimeoutHandler;
            remDev->monitorPairResultTimer.context = (void*)remDev; 
            EVM_StartTimer(&remDev->monitorPairResultTimer,25000);
        }
        return;
    }

    MEC(hciGenCmd).parms[6] = SECOP->x.pLen;
    OS_MemCopy(MEC(hciGenCmd).parms + 7, SECOP->pin, SECOP->x.pLen);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 23);
    MeSendHciCommand(HCC_PIN_CODE_REQ_REPL, 23);

    /* MAUI_01498320 by mtk01411 */
    /* Start a MPR (monitor pair result) timer */
    if (remDev->state == BDS_CONNECTED)
    {
        bt_trace(TRACE_GROUP_1,BT_LOG_START_MPR_TIMER_CASE,2);
        remDev->monitorPairResultTimer.func = MeHandleMonitorPairResultTimeoutHandler;
        remDev->monitorPairResultTimer.context = (void*)remDev; 
        EVM_StartTimer(&remDev->monitorPairResultTimer,25000);
    }
    
}

#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  StartIoCapabilityRspByAddrTask
 * DESCRIPTION
 *  Start the Io Capability Response by Addr response task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartIoCapabilityRspByAddrTask(void)
{
    /* Set the task handler */
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->bdAddr.addr, 6);
    bt_trace(TRACE_GROUP_1, BTLOG_STARTIOCAPABILITYRSPBYADDRTASKx02X, SECOP->x.ioCapability);
    if (SECOP->x.ioCapability == 0x00)
    {
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_IO_CAPABILITY_NEGATIVE_REPLY_COMMAND);
        MEC(hciGenCmd).parms[6] = HC_STATUS_PAIRING_DISALLOWED;
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 7);
        MeSendHciCommand(HCC_IO_CAPABILITY_NEG_REPL, 7);
    }
    else
    {

        MEC(hciGenCmd).parms[6] = BT_SSP_IO_CAPABILITY;
        MEC(hciGenCmd).parms[7] = BT_SSP_OOB_PRESENT_STATE;
        MEC(hciGenCmd).parms[8] = MEC(sspAuthenRequirements);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 9);
        MeSendHciCommand(HCC_IO_CAPABILITY_REPL, 9);
    }
}


/*****************************************************************************
 * FUNCTION
 *  StartUserConfirmRspByAddrTask
 * DESCRIPTION
 *  Start the User confirm by Addr response task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartUserConfirmRspByAddrTask(void)
{
    /* Set the task handler */
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->bdAddr.addr, 6);
    bt_trace(TRACE_GROUP_1, BTLOG_STARTUSERCONFIRMRSPBYADDRTASKx02X, SECOP->userConfirm);
    if (SECOP->userConfirm == TRUE)
    {
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_USER_CONFIRM_COMMAND);
        /* Set the state accordingly */
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
        MeSendHciCommand(HCC_USER_CONFIRM_REPL, 6);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_USER_CONFIRM_NEG_COMMAND);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
        MeSendHciCommand(HCC_USER_CONFIRM_NEG_REPL, 6);
    }
}


/*****************************************************************************
 * FUNCTION
 *  StartUserOobDataRspTask
 * DESCRIPTION
 *  Start the User OOB data response task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartUserOobDataRspTask(void)
{
    /* Set the task handler */
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->bdAddr.addr, 6);
    bt_trace(TRACE_GROUP_1, BTLOG_STARTUSERCONFIRMRSPBYADDRTASKx02X, SECOP->userConfirm);
    if (SECOP->userConfirm == TRUE)
    {
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_USER_CONFIRM_COMMAND);
        OS_MemCopy(MEC(hciGenCmd).parms + 6, SECOP->c, 16);
        OS_MemCopy(MEC(hciGenCmd).parms + 22, SECOP->r, 16);
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 38);
        MeSendHciCommand(HCC_USER_OOB_DATA_REQ_REPL, 38);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_SENDING_USER_CONFIRM_NEG_COMMAND);
        MEC(hciGenCmd).parms[6] = HC_STATUS_PAIRING_DISALLOWED;
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
        MeSendHciCommand(HCC_USER_OOB_DATA_REQ_NEG_REPL, 6);
    }
}

/*****************************************************************************
 * FUNCTION
 *  StartDeleteStoredLinkKeyTask
 * DESCRIPTION
 *  Delete the stored link key operation
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartDeleteStoredLinkKeyTask(void)
{
    /* Set the task handler */
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    OS_MemSet(MEC(hciGenCmd).parms, 0, sizeof(MEC(hciGenCmd).parms));

    /* Put in the BD_ADDR portion of the response */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    OS_MemCopy(MEC(hciGenCmd).parms, SECOP->remDev->bdAddr.addr, 6);
    MEC(hciGenCmd).parms[6] = 0x00;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEDELETESTOREDLINKKEYOP);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 7);
    MeSendHciCommand(HCC_DEL_STORED_LINK_KEY, 7);
}
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  StartEncryptionTask
 * DESCRIPTION
 *  Start the encryption task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartEncryptionTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    /* Store the connection handle */
    StoreLE16(MEC(hciGenCmd).parms, SECOP->remDev->hciHandle);
    MEC(hciGenCmd).parms[2] = SECOP->x.eMode;

    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    bt_trace(TRACE_GROUP_1, BTLOG_CHANGE_ENCRYPTION_MODE_TO_HDLx04X_MODEx02X , SECOP->remDev->hciHandle, SECOP->x.eMode);
    bt_trace(TRACE_GROUP_1, MESEC_STARTING_SET_CONNECTION_ENCRYPTION_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 3);
    MeSendHciCommand(HCC_SET_CONN_ENCRYPT, 3);
}


/*****************************************************************************
 * FUNCTION
 *  StartWriteAuthenticateEnableTask
 * DESCRIPTION
 *  Start the write authenticat enable task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartWriteAuthenticateEnableTask(void)
{
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;

    /* Set the proper mode */
    if (SECOP->x.secMode.mode == BSM_SEC_LEVEL_3)
    {
        bt_trace(TRACE_GROUP_1, MESEC_ENABLING_AUTHENTICATION_FOR_ALL_CONNECTIONS);
        MEC(hciGenCmd).parms[0] = 0x01;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_DISABLING_AUTHENTICATION_FOR_ALL_CONNECTIONS);
        MEC(hciGenCmd).parms[0] = 0x00;
    }
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    bt_trace(TRACE_GROUP_1, MESEC_SENDING_AUTHENTICATION_ENABLE_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
    MeSendHciCommand(HCC_WRITE_AUTH_ENABLE, 1);
}


/*****************************************************************************
 * FUNCTION
 *  StartWriteEncryptModeTask
 * DESCRIPTION
 *  Start the write encryption mode task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartWriteEncryptModeTask(void)
{
    MEC(taskHandler) = HandleSecurityCommandComplete;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;
    MEC(opState) = 1;

    /* Set the proper mode */
    if ((SECOP->x.secMode.mode == BSM_SEC_LEVEL_3) && (SECOP->x.secMode.encrypt == TRUE))
    {
        bt_trace(TRACE_GROUP_1, MESEC_ENABLING_ENCRYPTION_FOR_ALL_CONNECTIONS);
        MEC(hciGenCmd).parms[0] = 0x01;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_DISABLING_ENCRYPTION_FOR_ALL_CONNECTIONS);
        MEC(hciGenCmd).parms[0] = 0x00;
    }
    if (!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    bt_trace(TRACE_GROUP_1, MESEC_SENDING_ENCYRPTION_MODE_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
    MeSendHciCommand(HCC_WRITE_ENCRYPT_MODE, 1);
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleLinkKeyReq
 * DESCRIPTION
 *  Handle a link key request event
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleLinkKeyReq(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtSecurityOp *secOp;

    /* UNUSED_PARAMETER(len); */

    /* Get a security op to use */
    secOp = SecBtGetFreeSecurityOp();
    Assert(secOp != 0);

    /* Save the BD_ADDR */
    OS_MemCopy(secOp->bdAddr.addr, parm, 6);
    secOp->op.opType = BOP_LINK_KEY_RSP;
    MeAddOperationFront(secOp);

    /* Find the remote device associated with this request. */
    remDev = ME_FindRemoteDeviceP(parm);

    /*
     * If we are in the process of performing security or already secure
     * then something is wrong so we should reply in the negative.
     * Setting secOp->remDev to 0 is the way to send a negative response.
     */
    secOp->remDev = 0;
    if (remDev != 0)
    {
        if (remDev->authState == BAS_WAITING_KEY_REQ)
        {
            /*
             * We got the key request as part of our authentication so
             * change to next state
             */
            remDev->authState = BAS_SENDING_KEY;
        }
        else if(remDev->authState == BAS_START_AUTHENTICATE)
        {
            OS_Report("SecHandleLinkKeyReq : conflict with local bonding");
            SecRemoveAuthenticateOpFromQueue(remDev);
            //#if !defined (__AUTH_CONFLICT_NO_FAIL__)
            /* Remote initiate pairing concurrently, report bongding fail first */
            //MEC(btEvent).errCode = BEC_PAIRING_NOT_ALLOWED;
            //SecReportAuthenticateResult(remDev);
            //#endif
            remDev->authState = BAS_SENDING_KEY;
        }
        else
        {
            /*
             * We got the key request because of the remote device's 
             * authentication. We don't want to change states because
             * we don't consider the link to be in the process of authentication.
             */
            bt_trace(TRACE_GROUP_1, MESEC_REMOTE_DEVICE_PERFORMING_AUTHENTICATION);
        }
        bt_trace(TRACE_GROUP_1, MESEC_CREATING_OPERATION_FOR_LINK_KEY_RESPONSE);
        secOp->remDev = remDev;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_REQ_FOR_INVALID_BD_ADDR__LINK_KEY_NEG_RSP);
    }
    /* Attempt to start the operation */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  SecHandlePinCodeReq
 * DESCRIPTION
 *  Handle a pin code request event
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandlePinCodeReq(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtSecurityOp *secOp;

    /* UNUSED_PARAMETER(len); */

    bt_trace(TRACE_GROUP_1, MESEC_RECEIVED_PIN_CODE_REQUEST_EVENT);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        /* 
         * We need to call the pairing handler to get a Pin code.
         * If there is no pairing handler then respond in the negative 
         */
        if (MEC(pairingHandler) != 0)
        {
            if (remDev->authState == BAS_WAITING_FOR_PIN_R)
            {
                /* Pairing is already in progress, continue waiting. */
                bt_trace(TRACE_GROUP_1, MESEC_IGNORING_SECOND_PIN_REQUEST);
                return;
            }

            /* Add 2007-1116 */
            if (remDev->authState == BAS_WAITING_KEY_REQ)
            {
                /*
                 * Start authentication requesst: (authState is changed to BAS_WAITING_KEY_REQ)
                 * But not receiving LINK_KEY_REQ event, only PIN_CODE_REQ event is reported from Controller
                 * So authState is changed to BAS_SENDING_KEY by itself to avoid assert!
                 */
                remDev->authState = BAS_SENDING_KEY;
            }
            else if(remDev->authState == BAS_START_AUTHENTICATE)
            {
                OS_Report("SecHandlePinCodeReq : conflict with local authenticate");
                SecRemoveAuthenticateOpFromQueue(remDev);
                //#if !defined (__AUTH_CONFLICT_NO_FAIL__)
                /* Remote initiate pairing concurrently, report bongding fail first */
                //MEC(btEvent).errCode = BEC_PAIRING_NOT_ALLOWED;
                //SecReportAuthenticateResult(remDev);
                //#endif
                remDev->authState = BAS_SENDING_KEY;
            }

            if(!((remDev->authState == BAS_NOT_AUTHENTICATED) ||
                   (remDev->authState == BAS_WAITING_FOR_KEY_R) ||
                   (remDev->authState == BAS_AUTHENTICATED) || (remDev->authState == BAS_SENDING_KEY)))
            {
                Assert(0);
            }

            if (remDev->authState == BAS_SENDING_KEY)
            {
                /* Local request for authentication */
                remDev->authState = BAS_WAITING_FOR_PIN;
            }
            else
            {
                /*
                 * A remote device is doing pairing. We need to keep track
                 * of this state just it case it fails and we do not get
                 * an event indicated failure
                 */
                bt_trace(TRACE_GROUP_1, MESEC_REMOTE_DEVICE_PERFORMING_PAIRING);
                remDev->authState = BAS_WAITING_FOR_PIN_R;
            }

            /* Get the pin code */
            bt_trace(TRACE_GROUP_1, MESEC_CALLING_PAIRING_HANDLER_FOR_A_PIN_CODE);
        #if !defined (__BT_2_1_SIMPLE_PAIRING__)
            if (remDev->simap_issue_authentication_procedure)
            {
                bt_trace(TRACE_GROUP_1, BT_LOG_SEC_PIN_CODE_REQ_ISSUED_BY_SIMAP);
            }
        #endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
            MEC(btEvent).eType = BTEVENT_PIN_REQ;
            MEC(btEvent).p.remDev = remDev;
            MEC(btEvent).handler = MEC(pairingHandler);
            MeDevEventHandler();
            /* PIN_CODE_REQ event only has BD_ADDR para,no status para (so errCode=0) 2006-12-04 */
            MEC(btEvent).errCode = 0;
            MEC(pairingHandler)->callback(&MEC(btEvent));
            /* Start a disconnect that may be waiting */
            MeStartOperation();
            return;
        }
        else
        {
            /* Pairing is not allowed so set state to NOT_SECURE */
            bt_trace(TRACE_GROUP_1, MESEC_NO_PAIRING_HANDLER);
            remDev->authState = BAS_NOT_AUTHENTICATED;
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_REMOTE_DEVICE_DOES_NOT_EXIST_FOR_PAIRING);
    }

    /* Setup a security op to send the rejection */
    secOp = SecBtGetFreeSecurityOp();
    Assert(secOp != 0);

    /* Set the remDev to 0 indicating a negative response */
    secOp->remDev = 0;

    /* Copy in the BD_ADDR */
    OS_MemCopy(secOp->bdAddr.addr, parm, 6);

    secOp->op.opType = BOP_PIN_CODE_RSP;
    MeAddOperationFront(secOp);

    /* Attempt to start the operation */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleAuthenticateComplete
 * DESCRIPTION
 *  Start the link accept task.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleAuthenticateComplete(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    /* UNUSED_PARAMETER(len); */

    /* Locate the remote device */
    remDev = MeMapHciToRemoteDevice((HciHandle) (LEtoHost16(parm + 1) & 0x0fff));
    if (remDev != 0)
    {
        bt_trace(TRACE_GROUP_1, MESEC_RECEIVED_AUTHENTICATE_COMPLETE_EVENT_ERR__xD, parm[0]);
    #if !defined (__BT_2_1_SIMPLE_PAIRING__)
        if (remDev->simap_issue_authentication_procedure)
        {
            bt_trace(TRACE_GROUP_1, BT_LOG_SEC_AUTH_COMPLETE_RESET_SIMAP_ISSUE_AUTH_PROCEDURE);
            remDev->simap_issue_authentication_procedure = FALSE;
        }
    #endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        
        /* MAUI_01498320: Cancel MPR Timer mtk01411 */
        if (remDev->monitorPairResultTimer.func != 0)
        {
            bt_trace(TRACE_GROUP_1,BT_LOG_MPR_CANCEL_TIMER_IN_AUTHSTATE_CASE,remDev->authState,2);
            EVM_ResetTimer(&remDev->monitorPairResultTimer);
        }
        
        if(remDev->authState != BAS_CANCEL_AUTHENTICATE){
            MEC(btEvent).errCode = parm[0];
            SecReportAuthenticateResult(remDev);
        }else{
            remDev->authState = BAS_NOT_AUTHENTICATED;
            OS_Report("SecHandleAuthenticateComplete : authState is cancel authenticate. Just return");
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_RECEIVED_AUTHENTICATE_COMPLETE_EVENT__NO_REMOTE_DEVICE);
    }

}

#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  SecHandleLinkKeyNotify
 * DESCRIPTION
 *  Handle a link key notify event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleLinkKeyNotify(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtDeviceRecord record;
    BtStatus status;

#if defined (__BT_2_1_SIMPLE_PAIRING__)
    BtDeviceContext *bdc;
#endif 
    OS_MemSet((U8*) & record, 0, sizeof(record));
    bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_NOTIFY_EVENT);

    /* Find the remote device associate with this link key */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        /* MAUI_01498320: Cancel MPR Timer mtk01411 */
        if (remDev->monitorPairResultTimer.func != 0)
        {
            bt_trace(TRACE_GROUP_1,BT_LOG_MPR_CANCEL_TIMER_IN_AUTHSTATE_CASE,remDev->authState,0);
            EVM_ResetTimer(&remDev->monitorPairResultTimer);
        }

        /* ALPS00528225 : pair complete after MMI has sent bonding cancel. */
        if(remDev->authState == BAS_NOT_AUTHENTICATED){
            OS_Report("[WARN] SecHandleLinkKeyNotify : bonding has been canceled. Just return");
            return;
        }
        
        /* Set the security state to authenticated */
        remDev->authState = BAS_AUTHENTICATED;
        if(((remDev->remoteCapability > 0)) && ((random() % 3) ==0))
        {
#ifdef __BT_DLIGHT1_DEBUG__
            Report(("autumn try to do MeDisconnectLink"));
#endif
            remDev->authState = BAS_NOT_AUTHENTICATED;
            parm[9] = parm[14];
            parm[11] = parm[9];
            remDev->authState = BAS_NOT_AUTHENTICATED;
        }
        status = BT_STATUS_SUCCESS;
        /* See if we need to store the link key. */
        record.bdAddr = remDev->bdAddr;
        OS_MemCopy(record.linkKey, parm + 6, 16);
        bt_trace(TRACE_GROUP_1, BTLOG_THE_LINK_KEY_OF_ADDRESS_x02Xx02Xx02Xx02Xx02Xx02X ,
                parm[0], parm[1], parm[2], parm[3], parm[4], parm[5]);
        bt_trace(TRACE_GROUP_1, BTLOG_THE_LINK_KEY_x02Xx02Xx02Xx02Xx02Xx02Xx02Xx02X ,
                parm[6], parm[7], parm[8], parm[9], parm[10], parm[11], parm[12], parm[13]);
        bt_trace(TRACE_GROUP_1, BTLOG_THE_LINK_KEY_x02Xx02Xx02Xx02Xx02Xx02Xx02Xx02X ,
                parm[14], parm[15], parm[16], parm[17], parm[18], parm[19], parm[20], parm[21]);
#ifdef __BT_3_0_HS__
        Report(("BTA2MP_GenerateInitGAMPLK"));
        BTA2MP_GenerateInitGAMPLK(record.linkKey, record.gampKey);
        Report(("gampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.gampKey[0], record.gampKey[1], record.gampKey[2], record.gampKey[3], record.gampKey[4], record.gampKey[5], record.gampKey[6], record.gampKey[7]));
        Report(("gampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.gampKey[8], record.gampKey[9], record.gampKey[10], record.gampKey[11], record.gampKey[12], record.gampKey[13], record.gampKey[14], record.gampKey[15]));
        Report(("gampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.gampKey[16], record.gampKey[17], record.gampKey[18], record.gampKey[19], record.gampKey[20], record.gampKey[21], record.gampKey[22], record.gampKey[23]));
        Report(("gampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.gampKey[24], record.gampKey[25], record.gampKey[26], record.gampKey[27], record.gampKey[28], record.gampKey[29], record.gampKey[30], record.gampKey[31]));
        Report(("BTA2MP_DedicatedAMPLK"));
        BTA2MP_DedicatedAMPLK(record.gampKey, record.dampKey);
        Report(("dampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.dampKey[0], record.dampKey[1], record.dampKey[2], record.dampKey[3], record.dampKey[4], record.dampKey[5], record.dampKey[6], record.dampKey[7]));
        Report(("dampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.dampKey[8], record.dampKey[9], record.dampKey[10], record.dampKey[11], record.dampKey[12], record.dampKey[13], record.dampKey[14], record.dampKey[15]));
        Report(("dampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.dampKey[16], record.dampKey[17], record.dampKey[18], record.dampKey[19], record.dampKey[20], record.dampKey[21], record.dampKey[22], record.dampKey[23]));
        Report(("dampKey:%02x%02x%02x%02x%02x%02x%02x%02x",record.dampKey[24], record.dampKey[25], record.dampKey[26], record.dampKey[27], record.dampKey[28], record.dampKey[29], record.dampKey[30], record.dampKey[31]));

#endif

        if (len > 22)
        {
            record.keyType = parm[22];
            /* MT6611 always report authentication key. We will get the key type from Host */
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
            bdc = DS_FindDevice(&(remDev->bdAddr));
            bt_trace(TRACE_GROUP_1, BTLOG_LINK_KEY_BDCx08X, bdc);
            bt_trace(TRACE_GROUP_1, BTLOG_LINK_NOTIFICATIONx02X , record.keyType);
        #ifdef __GAP_TESTER_
            if (bdc != 0)
            {
                record.gapKeyType = bdc->gapKeyType;
            }
            else
            {
                record.gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
            }
            bt_trace(TRACE_GROUP_1, BTLOG_RECORDGAPKEYTYPExD, record.gapKeyType);
        #else /* __GAP_TESTER_ */ 
            record.gapKeyType = record.keyType;
            if ((record.gapKeyType == 0x00) && (bdc != 0))
            {
                /* 0x00: Combination KeyType according to HCI Spec */
                bt_trace(TRACE_GROUP_1, BTLOG_ME_LINKKEY_NOTIFY_BDC_GAPKEYTYPE, bdc->gapKeyType);
                /* 
                 * In BTstack, it has two types of combination key: recorded in bdc->gapKeyType 
                 * BT_LINK_KEY_GAP_TYPE_COMBINATION_NONE16 and BT_LINK_KEY_GAP_TYPE_COMBINATION_16 
                 */
                record.gapKeyType = bdc->gapKeyType;
            }
            else if ((record.gapKeyType == 0x05) && (bdc != 0))
            {
                /* 0x05: Authenticated Combination KeyType according to HCI Spec */
                bt_trace(TRACE_GROUP_1, BTLOG_ME_LINKKEY_NOTIFY_BDC_GAPKEYTYPE, bdc->gapKeyType);
                /* In BTStack local definition: this case should be mapped to BT_LINK_KEY_GAP_TYPE_AUTHENTICATED (0x04) */
                record.gapKeyType = bdc->gapKeyType;
            }
        #endif /* __GAP_TESTER_ */ 
            remDev->gapKeyType = record.gapKeyType;
            bt_trace(TRACE_GROUP_1, BTLOG_CURRENT_KEY_TYPExD, record.gapKeyType);
        #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        }
        if (remDev->pairingType == BPT_SAVE_TRUSTED)
        {
            record.trusted = TRUE;
            remDev->authorizeState = BAS_AUTHORIZED;
            bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_IS_SAVED_DEVICE_TRUSTED);
        }
        else
        {
            record.trusted = FALSE;
            remDev->authorizeState = BAS_NOT_AUTHORIZED;
            bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_IS_SAVED_DEVICE_NOT_TRUSTED);
        }
        /* Store the link key. */
        status = SEC_AddDeviceRecord(&record);
        status = BT_STATUS_SUCCESS;
        /* If we have a pairing handler then report the results */
        if (MEC(pairingHandler) != 0)
        {
            bt_trace(TRACE_GROUP_1, MESEC_CALLING_PAIRING_HANDLER_INDICATING_PAIRING_COMPLETE);
            MEC(btEvent).eType = BTEVENT_PAIRING_COMPLETE;
            MEC(btEvent).p.remDev = remDev;
            MEC(btEvent).handler = MEC(pairingHandler);

            /* Set the errCode appropriately */
            if (status == BT_STATUS_SUCCESS)
            {
                MEC(btEvent).errCode = BEC_NO_ERROR;
            }
            else
            {
                bt_trace(TRACE_GROUP_1, MESEC_ERROR_SAVING_LINK_KEY);
                MEC(btEvent).errCode = BEC_STORE_LINK_KEY_ERR;
            }

            /* Call the pairing handler */
            MEC(pairingHandler)->callback(&MEC(btEvent));
        }
        else
        {
            bt_trace(TRACE_GROUP_1, MESEC_NO_PAIRING_HANDLER_TO_REPORT_PAIRING_COMPLETE);
        }
#ifdef __BT_3_0_HS__
        if(remDev->remoteWifiAMP.physical_link_sate == BDS_CONNECTED)
        {
            Report(("Try to disconnect Physical link"));
            BTA2MP_PhysicalLinkClose(remDev);
        }
#endif        
    }
    else
    {
        record.bdAddr = remDev->bdAddr;
        OS_MemCopy(record.linkKey, parm + 6, 16);
        if (len > 22)
        {
            record.keyType = parm[22];
        }
        record.trusted = TRUE;
        bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_IS_SAVED_DEVICE_TRUSTED);
        SEC_AddDeviceRecord(&record);
    }

}
#else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  SecHandleLinkKeyNotify
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleLinkKeyNotify(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtDeviceRecord record;
    BtStatus status;

    OS_MemSet((U8*) & record, 0, sizeof(record));
    bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_NOTIFY_EVENT);
    /* Find the remote device associate with this link key */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        /* MAUI_01498320: Cancel MPR Timer mtk01411 */
        if (remDev->monitorPairResultTimer.func != 0)
        {
            bt_trace(TRACE_GROUP_1,BT_LOG_MPR_CANCEL_TIMER_IN_AUTHSTATE_CASE,remDev->authState,0);
            EVM_ResetTimer(&remDev->monitorPairResultTimer);
        }
    
        /* Set the security state to authenticated */
        remDev->authState = BAS_AUTHENTICATED;
        status = BT_STATUS_SUCCESS;

        /* See if we need to store the link key. */
        record.bdAddr = remDev->bdAddr;
        OS_MemCopy(record.linkKey, parm + 6, 16);
        if (len > 22)
        {
            record.keyType = parm[22];
        }
        if (remDev->pairingType == BPT_SAVE_TRUSTED)
        {
            record.trusted = TRUE;
            remDev->authorizeState = BAS_AUTHORIZED;
            bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_IS_SAVED_DEVICE_TRUSTED);
        }
        else
        {
            record.trusted = FALSE;
            remDev->authorizeState = BAS_NOT_AUTHORIZED;
            bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_IS_SAVED_DEVICE_NOT_TRUSTED);
        }
        /* Store the link key. */
    #if !defined (__BT_2_1_SIMPLE_PAIRING__)
        if (remDev->simap_issue_authentication_procedure == TRUE)
        {
            remDev->simap_issue_authentication_procedure = FALSE;
            record.paired_with_simap = TRUE;
            bt_trace(TRACE_GROUP_1, BT_LOG_SEC_LINK_KEY_NOTIFY_RESET_FALGS_CASE, 1);
        }
#ifdef __BT_3_0_HS__
        BTA2MP_GenerateInitGAMPLK(&record.linkKey, &record.gampKey);
        BTA2MP_DedicatedAMPLK(&record.gampKey, &record.dampKey);
#endif

        record.pin_code_len = remDev->pin_code_len;
        bt_trace(TRACE_GROUP_1, BT_LOG_SEC_LINK_KEY_NOTIFY_PIN_CODE_LEN_CASE, 1, record.pin_code_len);
        bt_trace(TRACE_GROUP_1, BT_LOG_HANDLE_LINK_KEY_NOTIFY_CHECK_PAIRED_SIMAP_FLAG, 1, record.paired_with_simap);
    #endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        status = SEC_AddDeviceRecord(&record);
        /* If we have a pairing handler then report the results */
        if (MEC(pairingHandler) != 0)
        {
            bt_trace(TRACE_GROUP_1, MESEC_CALLING_PAIRING_HANDLER_INDICATING_PAIRING_COMPLETE);
            MEC(btEvent).eType = BTEVENT_PAIRING_COMPLETE;
            MEC(btEvent).p.remDev = remDev;
            MEC(btEvent).handler = MEC(pairingHandler);
            /* Set the errCode appropriately */
            if (status == BT_STATUS_SUCCESS)
            {
                MEC(btEvent).errCode = BEC_NO_ERROR;
            }
            else
            {
                bt_trace(TRACE_GROUP_1, MESEC_ERROR_SAVING_LINK_KEY);
                MEC(btEvent).errCode = BEC_STORE_LINK_KEY_ERR;
            }

            /* Call the pairing handler */
            MEC(pairingHandler)->callback(&MEC(btEvent));
        }
        else
        {
            bt_trace(TRACE_GROUP_1, MESEC_NO_PAIRING_HANDLER_TO_REPORT_PAIRING_COMPLETE);
        }
    }
    else
    {
        record.bdAddr = remDev->bdAddr;
        OS_MemCopy(record.linkKey, parm + 6, 16);
        if (len > 22)
        {
            record.keyType = parm[22];
        }
        record.trusted = TRUE;
        bt_trace(TRACE_GROUP_1, MESEC_LINK_KEY_IS_SAVED_DEVICE_TRUSTED);
        if (remDev->simap_issue_authentication_procedure == TRUE)
        {
            remDev->simap_issue_authentication_procedure = FALSE;
            record.paired_with_simap = TRUE;
            bt_trace(TRACE_GROUP_1, BT_LOG_SEC_LINK_KEY_NOTIFY_RESET_FALGS_CASE, 2);
        }
        record.pin_code_len = remDev->pin_code_len;
        bt_trace(TRACE_GROUP_1, BT_LOG_SEC_LINK_KEY_NOTIFY_PIN_CODE_LEN_CASE, 2, record.pin_code_len);
        bt_trace(TRACE_GROUP_1, BT_LOG_HANDLE_LINK_KEY_NOTIFY_CHECK_PAIRED_SIMAP_FLAG, 2, record.paired_with_simap);
        SEC_AddDeviceRecord(&record);
    }
}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  SecHandleEncryptChange
 * DESCRIPTION
 *  Handle an encryption change event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleEncryptChange(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    /* UNUSED_PARAMETER(len); */


    /* Locate the remote device */
    remDev = MeMapHciToRemoteDevice((HciHandle) (LEtoHost16(parm + 1) & 0x0fff));
    if (remDev != 0)
    {
        bt_trace(TRACE_GROUP_1, MESEC_RECEIVED_ENCRYPTION_CHANGE_ERR__xD, parm[0]);
        bt_trace(TRACE_GROUP_1, BTLOG_ENCRYPTION_HDLx04X_MODEx02X , remDev->hciHandle, parm[3]);
        MEC(btEvent).errCode = parm[0];
        SecReportEncryptionResult(remDev, parm[3]);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_RECEIVED_ENCRYPTION_CHANGE_EVENT__NO_REMOTE_DEVICE);
    }
}

#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  SecHandleIoCapabilityRequest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleIoCapabilityRequest(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtSecurityOp *secOp;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEIOCAPABILITYREQUEST);
    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if ((remDev != 0) && ((remDev->ioCapabilitiyState & BIO_REQUESTD_LOCAL) == 0))
    {
        remDev->ioCapabilitiyState |= BIO_REQUESTD_LOCAL;
        bt_trace(TRACE_GROUP_1, BTLOG_BTEVENT_IO_CAPABILITY_REQUEST);
        MEC(btEvent).eType = BTEVENT_IO_CAPABILITY_REQUEST;
        MEC(btEvent).p.remDev = remDev;
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = BEC_NO_ERROR;
        MEC(pairingHandler)->callback(&MEC(btEvent));
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEIOCAPABILITYREQUEST_NO_REMDEV);
        /* Response negative IO capability response */
        secOp = SecBtGetFreeSecurityOp();
        Assert(secOp != 0);
        secOp->x.ioCapability = 0;
        secOp->remDev = 0;
        /* Copy in the BD_ADDR */
        OS_MemCopy(secOp->bdAddr.addr, parm, 6);
        secOp->op.opType = BOP_WRITE_IO_CAPABILITY_RESPONSE;
        MeAddOperationFront(secOp);
        /* Attempt to start the operation */
        MeStartOperation();
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleIoCapabilityResponse
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleIoCapabilityResponse(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEIOCAPABILITYREQUEST);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if ((remDev != 0) && ((remDev->ioCapabilitiyState & BIO_RSPONSE_REMOTE) == 0))
    {
        if(remDev->authState == BAS_START_AUTHENTICATE){
            /* Conflict */
            OS_Report("SecHandleIoCapabilityResponse : conflict with local bonding");
            SecRemoveAuthenticateOpFromQueue(remDev);
            //#if !defined (__AUTH_CONFLICT_NO_FAIL__)
            /* Remote initiate pairing concurrently, report bongding fail first */
            //MEC(btEvent).errCode = BEC_PAIRING_NOT_ALLOWED;
            //SecReportAuthenticateResult(remDev);
            //#endif
            /* Set to sending key to prevent another auth op to be queued */
            remDev->authState = BAS_SENDING_KEY;
        }
        remDev->ioCapabilitiyState |= BIO_RSPONSE_REMOTE;
        bt_trace(TRACE_GROUP_1, BTLOG_BTEVENT_IO_CAPABILITY_REQUEST);
        MEC(btEvent).eType = BTEVENT_IO_CAPABILITY_RESPONSE;
        MEC(btEvent).p.ioCapabilityResponse.remDev = remDev;
        MEC(btEvent).p.ioCapabilityResponse.ioCapability = parm[6];
        MEC(btEvent).p.ioCapabilityResponse.OOBDataPresent = parm[7];
        MEC(btEvent).p.ioCapabilityResponse.authRequirements = parm[8];
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = BEC_NO_ERROR;
        MEC(pairingHandler)->callback(&MEC(btEvent));
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEIOCAPABILITYRESPONSE_NO_REMDEV);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleUserConfirmRequest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleUserConfirmRequest(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtSecurityOp *secOp;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEUSERCONFIRMREQUEST);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        if (remDev->authState == BAS_WAITING_FOR_PIN_R)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_PAIRING_IS_IN_PROGRESS);
            return;
        }
        if (remDev->authState == BAS_WAITING_KEY_REQ)
        {
            remDev->authState = BAS_SENDING_KEY;
        }
        if(!((remDev->authState == BAS_NOT_AUTHENTICATED) ||
               (remDev->authState == BAS_WAITING_FOR_KEY_R) ||
               (remDev->authState == BAS_AUTHENTICATED) || (remDev->authState == BAS_SENDING_KEY)))
        {
            Assert(0);
        }
        if (remDev->authState == BAS_SENDING_KEY)
        {
            remDev->authState = BAS_WAITING_FOR_PIN;
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BTLOG_REMOTE_DEVICE_PERFORM_USER_CONFIRM_REQUEST);
            remDev->authState = BAS_WAITING_FOR_PIN_R;
        }
        MEC(btEvent).eType = BTEVENT_USER_CONFIRM_REQUEST;
        MEC(btEvent).p.userConfirmRequest.remDev = remDev;
        MEC(btEvent).p.userConfirmRequest.numericValue = LEtoHost32(parm + 6);
        bt_trace(
            TRACE_GROUP_1,
            BTLOG_USERCONFIRMREQUEST_NUMERICVALUE_x08X,
            MEC(btEvent).p.userConfirmRequest.numericValue);
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = BEC_NO_ERROR;
        Assert((MEC(pairingHandler) != 0));
        MEC(pairingHandler)->callback(&MEC(btEvent));
        MeStartOperation();
        return;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_REMOVE_DEVICE_IS_EMPTY);
    }

    /* Setup a security op to send the rejection */
    secOp = SecBtGetFreeSecurityOp();
    Assert(secOp != 0);

    /* Set the remDev to 0 indicating a negative response */
    secOp->remDev = 0;

    /* Copy in the BD_ADDR */
    OS_MemCopy(secOp->bdAddr.addr, parm, 6);
    secOp->userConfirm = FALSE;
    secOp->op.opType = BOP_USER_CONFIRM_BT_ADDR_RSP;
    MeAddOperationFront(secOp);
    /* Attempt to start the operation */
    MeStartOperation();
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleUserPasskeyNotification
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleUserPasskeyNotification(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEUSERPASSKEYNOTIFICATION);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        if (remDev->authState == BAS_WAITING_FOR_PIN_R)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_PAIRING_IS_IN_PROGRESS);
            return;
        }
        if (remDev->authState == BAS_WAITING_KEY_REQ)
        {
            remDev->authState = BAS_SENDING_KEY;
        }
        if(!((remDev->authState == BAS_NOT_AUTHENTICATED) ||
               (remDev->authState == BAS_WAITING_FOR_KEY_R) ||
               (remDev->authState == BAS_AUTHENTICATED) || (remDev->authState == BAS_SENDING_KEY)))
        {
            Assert(0);
        }

        if (remDev->authState == BAS_SENDING_KEY)
        {
            remDev->authState = BAS_WAITING_FOR_KEY;
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BTLOG_REMOTE_DEVICE_PERFORM_SHOW_KEY_REQUEST);
            remDev->authState = BAS_WAITING_FOR_KEY_R;
        }
        MEC(btEvent).eType = BTEVENT_PASSKEY_NOTIFICATION;
        MEC(btEvent).p.passKeyEntry.remDev = remDev;
        MEC(btEvent).p.passKeyEntry.numericValue = LEtoHost32(parm + 6);
        bt_trace(TRACE_GROUP_1, BTLOG_PASSKEY_ENTRY_NUMERICVALUE_x08X, MEC(btEvent).p.passKeyEntry.numericValue);
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = BEC_NO_ERROR;
        Assert((MEC(pairingHandler) != 0));
        MEC(pairingHandler)->callback(&MEC(btEvent));
        MeStartOperation();
        return;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_REMOVE_DEVICE_IS_EMPTY);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecHandleUserKeyPress
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleUserKeyPress(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEUSERKEYPRESS);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        if ((remDev->authState != BAS_WAITING_FOR_PIN_R) && (remDev->authState != BAS_WAITING_FOR_PIN))

        {
            bt_trace(TRACE_GROUP_1, BTLOG_REMOTE_DEVICE_IS_NOT_WAITING_FOR_LINK_KEY);
            return;
        }
        MEC(btEvent).eType = BTEVENT_KEYPRESS_EVENT;
        MEC(btEvent).p.passKeyPress.remDev = remDev;
        MEC(btEvent).p.passKeyPress.passkeyEvent = parm[6];
        bt_trace(TRACE_GROUP_1, BTLOG_PASSKEY_PRESS_EVENT_x02X, MEC(btEvent).p.passKeyPress.passkeyEvent);
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = BEC_NO_ERROR;
        Assert((MEC(pairingHandler) != 0));
        MEC(pairingHandler)->callback(&MEC(btEvent));
        MeStartOperation();
        return;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_REMOVE_DEVICE_IS_EMPTY);
    }
}

/*****************************************************************************
 * FUNCTION
 *  SecHandleRemoteOobDataRequest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleRemoteOobDataRequest(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtSecurityOp *secOp;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLEREMOTEOOBDATAREQUEST);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm);
    if (remDev != 0)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_TOKENREMDEVAUTHSTATExD, remDev->authState);
        if (remDev->authState == BAS_WAITING_FOR_PIN_R)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_PAIRING_IS_IN_PROGRESS);
            return;
        }
        if (remDev->authState == BAS_WAITING_KEY_REQ)
        {
            remDev->authState = BAS_SENDING_KEY;
        }
        if(!((remDev->authState == BAS_NOT_AUTHENTICATED) ||
               (remDev->authState == BAS_WAITING_FOR_KEY_R) ||
               (remDev->authState == BAS_AUTHENTICATED) || (remDev->authState == BAS_SENDING_KEY)))
        {
            Assert(0);
        }
        if (remDev->authState == BAS_SENDING_KEY)
        {
            remDev->authState = BAS_WAITING_FOR_PIN;
        }
        else
        {
            remDev->authState = BAS_WAITING_FOR_PIN_R;
        }
        MEC(btEvent).eType = BTEVENT_OOB_DATA_REQUEST;
        MEC(btEvent).p.remDev = remDev;
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = BEC_NO_ERROR;
        Assert((MEC(pairingHandler) != 0));
        MEC(pairingHandler)->callback(&MEC(btEvent));
        MeStartOperation();
        return;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_REMOVE_DEVICE_IS_EMPTY);
    }

    /* Setup a security op to send the rejection */
    secOp = SecBtGetFreeSecurityOp();
    Assert(secOp != 0);

    /* Set the remDev to 0 indicating a negative response */
    secOp->remDev = 0;

    /* Copy in the BD_ADDR */
    OS_MemCopy(secOp->bdAddr.addr, parm, 6);
    secOp->userConfirm = FALSE;
    secOp->op.opType = BOP_USER_CONFIRM_BT_ADDR_RSP;
    MeAddOperationFront(secOp);
    /* Attempt to start the operation */
    MeStartOperation();
}

/*****************************************************************************
 * FUNCTION
 *  SecHandleSimplePairingComplete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleSimplePairingComplete(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    U8 errCode = 0;

    bt_trace(TRACE_GROUP_1, BTLOG_SECHANDLESIMPLEPAIRINGCOMPLETE);

    /*
     * Find the RemoteDevice that corresponds to the BD_ADDR. If we cannot
     * find one then respond in the negative
     */
    remDev = ME_FindRemoteDeviceP(parm + 1);
    errCode = parm[0];
    if ((remDev != 0))
    {
        remDev->ioCapabilitiyState = BIO_NOT_REQUESTD;
        bt_trace(TRACE_GROUP_1, BTLOG_BTEVENT_SIMPLE_PAIRING_COMPLTETE);
        MEC(btEvent).eType = BTEVENT_SIMPLE_PAIRING_COMPLTETE;
        MEC(btEvent).p.remDev = remDev;
        MEC(btEvent).handler = MEC(pairingHandler);
        MEC(btEvent).errCode = errCode;
        MEC(pairingHandler)->callback(&MEC(btEvent));
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_NOT_KNOWN_DEVICES);
    }
}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

/*---------------------------------------------------------------------------
 *            SecReportAuthenticateResult()
 *---------------------------------------------------------------------------
 *
 *   
 *
 * Return:    void
 */


/*****************************************************************************
 * FUNCTION
 *  SecReportAuthenticateResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecReportAuthenticateResult(BtRemoteDevice *remDev)
{
    BtErrorCode errCode;
    U8 authState = BAS_NOT_AUTHENTICATED;
    U8 i = 0;
    BtSecurityOp *secOp;

    /* Set the security state based on the result of the operation */
    if (MEC(btEvent).errCode == 0)
    {
        remDev->authState = BAS_AUTHENTICATED;
    }
    else
    {
        authState = remDev->authState;
        remDev->authState = BAS_NOT_AUTHENTICATED;
    }

    /* Process any security access tokens */
    remDev->secAccessState |= BAS_AUTH_COMPLETE;

    /* Auth is completed, reset authInitiator to 0 */
    OS_Report("Set authInitiator to 0");
    remDev->authInitiator = 0;
    
    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if (MEC(secOpTable)[i].remDev == remDev)
        {
            secOp = &MEC(secOpTable)[i];
            if (IsNodeOnList(&MEC(opList), &(secOp->op.node)))
            {
                bt_trace(TRACE_GROUP_1, NODE_STILL_IN_THE_OPLIST);
                RemoveEntryList(&(secOp->op.node));
                secOp->op.opType = BOP_NOP;
            }
        }
    }
    if (remDev->authHandler == 0)
    {
        if (MEC(btEvent).errCode != 0)
        {
            errCode = MEC(btEvent).errCode;
            MEC(btEvent).eType = BTEVENT_AUTHENTICATE_CNF;
            MEC(btEvent).p.remDev = remDev;
            Me2BMCallback(&(MEC(btEvent)));
            MEC(btEvent).errCode = errCode;
        }
    }
    SecProcessAllSecurityTokens(remDev);
    MEC(btEvent).eType = BTEVENT_AUTHENTICATE_CNF;
    MEC(btEvent).p.remDev = remDev;
    bt_trace(TRACE_GROUP_1, BTEVENT_AUTHENTICATE_CNF_TRC);
    if (remDev->authHandler != 0)
    {
        remDev->authHandler->callback(&(MEC(btEvent)));
    }

    if (MEC(btEvent).errCode == 0)
    {
        /*
         * Report the event to all bound handlers and all registered 
         * global handlers 
         */
        MEC(btEvent).eType = BTEVENT_AUTHENTICATED;
        MeCallLinkHandlers(remDev, &MEC(btEvent));
        MeReportResults(BEM_AUTHENTICATED);
    }


    /* MAUI_01498320 and MAUI_01363081: mtk01411 */
    /* If acceptor or initator side recvs an auth complete event with error code (authState is changed to BAS_NOT_AUTHENTICATED): It disconnects this ACL link */
    bt_trace(TRACE_GROUP_1,BT_LOG_AUTHEVENT_ERRCODE_LINKSTATE_ORG_AUTHSTATE,MEC(btEvent).errCode,remDev->state,authState);
    if (remDev->state == BDS_CONNECTED && MEC(btEvent).errCode!= 0)
    {
        bt_trace(TRACE_GROUP_1,BT_LOG_AUTHEVENT_ERR_DISC_LINK);
        MeDisconnectLink(remDev, 9);
    }
   
}


/*****************************************************************************
 * FUNCTION
 *  SecReportEncryptionResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 *  mode        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecReportEncryptionResult(BtRemoteDevice *remDev, U8 mode)
{
    BOOL forceDisconnect = FALSE;
    U8 i = 0;
    BtSecurityOp *secOp;
    U8 errorCode = MEC(btEvent).errCode;

    /* Set the security state based on the result of the operation */
    if (MEC(btEvent).errCode == 0)
    {
        /*
         * The encrypt operation was successful so set the state based on
         * the mode. We do this because we could get a mode change from the
         * other side.
         */
        if (mode == 0x00)
        {
            /* Mode indicated disabled */
            remDev->encryptState = BES_NOT_ENCRYPTED;
        }
        else
        {
            /* Mode indicated enabled */
            remDev->encryptState = BES_ENCRYPTED;
        }
    }
    else
    {
        /*
         * The encrypt operation failed. So mode is meaningless. We will
         * use the previous state to determine the new state
         */
        if (remDev->encryptState == BES_START_ENCRYPT)
        {
            remDev->encryptState = BES_NOT_ENCRYPTED;
        }
        else
        {
            if (remDev->encryptState == BES_END_ENCRYPT)
            {
                remDev->encryptState = BES_ENCRYPTED;
            }
            else if (MEC(btEvent).errCode != BEC_INVALID_HCI_PARM)  /* BEC_INVALID_HCI_PARM: no need to perform, current state is the desired state */
            {
                forceDisconnect = TRUE;
            }
        }
        if ((MEC(btEvent).errCode == HC_STATUS_LMP_RESPONSE_TIMEDOUT) && (remDev->encryptState == BES_NOT_ENCRYPTED))
        {
            forceDisconnect = TRUE;
        }
    }

    /* Process any security access tokens. */
    remDev->secAccessState |= BAS_ENCRYPT_COMPLETE;
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if ((MEC(secOpTable)[i].remDev == remDev) &&
            (OS_MemCmp(MEC(secOpTable)[i].remDev->bdAddr.addr, 6, remDev->bdAddr.addr, 6)))
        {
            secOp = &MEC(secOpTable)[i];
            if (IsNodeOnList(&MEC(opList), &(secOp->op.node)))
            {
                bt_trace(TRACE_GROUP_1, NODE_STILL_IN_THE_OPLIST);
                RemoveEntryList(&(secOp->op.node));
            }
            else
            {
                secOp->op.opType = BOP_NOP;
            }
        }
    }
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */

    SecProcessAllSecurityTokens(remDev);

    if (remDev->encryptHandler != 0)
    {
        MEC(btEvent).p.remDev = remDev;
        MEC(btEvent).eType = BTEVENT_ENCRYPT_COMPLETE;
        remDev->encryptHandler->callback(&(MEC(btEvent)));
    }

    /* Call the global handlers if the operations was successful */
    if (MEC(btEvent).errCode == 0)
    {
        /*
         * Report the event to all bound handlers and all registered 
         * global handlers 
         */
        MEC(btEvent).eType = BTEVENT_ENCRYPTION_CHANGE;
        MEC(btEvent).p.encrypt.remDev = remDev;
        MEC(btEvent).p.encrypt.mode = mode;

        MeCallLinkHandlers(remDev, &MEC(btEvent));
        MeReportResults(BEM_ENCRYPTION_CHANGE);
    }
    else
    {
        if ((MEC(btEvent).errCode != HC_STATUS_NO_CONNECTION) && (forceDisconnect))
        {
            if ((remDev != 0) && (remDev->state >= BDS_CONNECTED))
            {
                MeDisconnectLink(remDev, 10);
            }
        }
    }
#ifdef __BT_4_0_BLE__
    Report(("Dlight remDev->state:%02x",remDev->state));
    Report(("Dlight errorCode:%02x",errorCode));

    if((remDev->state == BDS_CONNECTED) && (errorCode == HC_STATUS_KEY_MISSING))
    {
        if (ME_GetDeviceType(&remDev->bdAddr) == BT_DEV_TYPE_LE)
        {
            Report(("ME_GetDeviceType BT_DEV_TYPE_LE"));

            SEC_DeleteDeviceRecord(&remDev->bdAddr);
            SmPairAttribute attr = {FALSE, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG, NULL, 0};
            Report(("SM bond starts"));
            SM_Bond(DS_FindDevice(&remDev->bdAddr), &attr);            
        }
    }
#endif
}


/*****************************************************************************
 * FUNCTION
 *  SecReportSecurityModeResult
 * DESCRIPTION
 *  Report the results of changing security mode
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SecReportSecurityModeResult(void)
{
    BtHandler *handler;

    if (MEC(btEvent).errCode == 0)
    {
        /*
         * The operation was successful so set the security mode state
         * properly.
         */
        if (MEC(secModeState) == BSMS_START_LEVEL3)
        {
            MEC(secModeState) = BSMS_LEVEL3;
            MEC(btEvent).p.secMode.mode = BSM_SEC_LEVEL_3;
        }
        else
        {
            Assert(MEC(secModeState) == BSMS_END_LEVEL3);
            MEC(secModeState) = BSMS_LEVEL2;
            MEC(btEvent).p.secMode.mode = BSM_SEC_LEVEL_2;
        }
    }
    else
    {
        /*
         * The operation ended in error so go back to our original
         * setting.
         */
        if (MEC(secModeState) == BSMS_START_LEVEL3)
        {
            MEC(secModeState) = BSMS_LEVEL2;
            MEC(btEvent).p.secMode.mode = BSM_SEC_LEVEL_2;
        }
        else
        {
            Assert(MEC(secModeState) == BSMS_END_LEVEL3);
            MEC(secModeState) = BSMS_LEVEL3;
            MEC(btEvent).p.secMode.mode = BSM_SEC_LEVEL_3;
        }
    }

    /* Set the encryption mode flag */
    MEC(btEvent).p.secMode.encrypt = MEC(secModeEncrypt);

    /* Call the handler. Make sure it is 0 for future use */
    if (MEC(secModeHandler) != 0)
    {
        handler = MEC(secModeHandler);
        MEC(secModeHandler) = 0;
        MEC(btEvent).handler = handler;
        MEC(btEvent).eType = BTEVENT_SECURITY3_COMPLETE;
        handler->callback(&MEC(btEvent));
    }

    if (MEC(btEvent).errCode == 0)
    {
        /* Call the global handlers */
        MEC(btEvent).eType = BTEVENT_SECURITY_CHANGE;
        MeReportResults(BEM_SECURITY_CHANGE);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SecAddAuthenticateOpToQueue
 * DESCRIPTION
 *  Add the authenticate operation to the operation queue.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus SecAddAuthenticateOpToQueue(BtRemoteDevice *remDev)
{
    BtSecurityOp *secOp;
    BtStatus status;

    if((!HCI_IsValidAclPacket()) && ((random() % 3) ==0))
    {    
#ifdef __BT_DLIGHT1_DEBUG__
        Report(("autumn start the authentication procedure"));
#endif 
        return BT_STATUS_PENDING;
    }
    if((!HCI_IsValidAclPacketLen()) && ((random() % 3) ==0))
    {    
#ifdef __BT_DLIGHT1_DEBUG__
        Report(("autumn start the authentication procedure"));
#endif 
        return BT_STATUS_PENDING;
    }

    if (remDev->state != BDS_CONNECTED)
    {
        //        ||
        //        remDev->encryptState != BES_NOT_ENCRYPTED) {
        status = BT_STATUS_FAILED;
    }
    else if((remDev->authState != BAS_NOT_AUTHENTICATED && remDev->authState != BAS_AUTHENTICATED)||
               remDev->ioCapabilitiyState != BIO_NOT_REQUESTD)
    {
        OS_Report("SecAddAuthenticateOpToQueue : auth is ongoing, return in progress. authState=%u, ioCapability=%u", remDev->authState, remDev->ioCapabilitiyState);
        status = BT_STATUS_IN_PROGRESS;
    }
    else
    {
        /* Get a security op */
        secOp = SecBtGetFreeSecurityOp();

        if (secOp == 0)
        {
            status = BT_STATUS_NO_RESOURCES;
        }
        else
        {
            secOp->remDev = remDev;

            /* Add the authentication operation to the queue */
            secOp->op.opType = BOP_AUTHENTICATE;

            /* store if local initiate auth that will influnce the MITM required or not */
            OS_Report("Set authInitiator to 1");
            remDev->authInitiator = 1;

            /*
             * We are not in the process of authenticating then
             * set the state appropriately. There are situations where
             * will run authentication event if we appear to be in the middle
             * of one.
             */
            if (remDev->authState == BAS_NOT_AUTHENTICATED)
            {
                remDev->authState = BAS_START_AUTHENTICATE;
            }

            if(!IsEntryAvailable(&(secOp->op.node)))
            {
                Assert(0);
            }
            MeAddOperation(secOp);

            /* Attempt to start the operation */
            MeStartOperation();
            status = BT_STATUS_PENDING;
        }
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  SecRemoveAuthenticateOpFromQueue
 * DESCRIPTION
 *  Add the authenticate operation to the operation queue.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus SecRemoveAuthenticateOpFromQueue(BtRemoteDevice *remDev)
{
    BtSecurityOp *secOp;
    BtStatus status = BT_STATUS_SUCCESS;
    secOp = SecBtFindSecurityOp(remDev);
    if(secOp){
        if(IsNodeOnList(&MEC(opList), &secOp->op.node)){
            RemoveEntryList(&secOp->op.node);
            secOp->op.opType = BOP_NOP;
            secOp->remDev = NULL;
        }else if(MEC(curOp) == &secOp->op){
            OS_Report("SecRemoveAuthenticateOpFromQueue : remove when curOp == auth op");
            status = BT_STATUS_IN_PROGRESS;
        }
    }else{
        OS_Report("SecRemoveAuthenticateOpFromQueue : not found");
        status = BT_STATUS_NOT_FOUND;
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  SecProcessAllSecurityTokens
 * DESCRIPTION
 *  Process all the security tokens for the given remote device.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
void SecProcessAllSecurityTokens(BtRemoteDevice *remDev)
{
    BtSecurityToken *token;
    BtSecurityToken *tmp;
    BtEventType eType;

    token = (BtSecurityToken*) remDev->secTokens.Flink;
    while ((ListEntry*) token != &(remDev->secTokens))
    {
        /* Point to the next token */
        tmp = (BtSecurityToken*) token->node.Flink;

        /* Process the token */
        bt_trace(TRACE_GROUP_1, MESEC_SECPROCESSALLSECURITYTOKENS_PROCESSING_A_TOKEN);
        switch (SecProcessSecurityToken(token))
        {
            case BT_STATUS_SUCCESS:
                /*
                 * This token is complete and access is granted. Call the protocol 
                 * callback to let it know.
                 */
                eType = BTEVENT_ACCESS_APPROVED;
                goto reportit;

            case BT_STATUS_FAILED:
                /*
                 * This token is complete and access is denied. Call the protocol
                 * callback to let it know.
                 */
                eType = BTEVENT_ACCESS_DENIED;
              reportit:
                /* Remove the token from the list */
                bt_trace(TRACE_GROUP_1, MESEC_SECPROCESSALLSECURITYTOKENS_REMOVING_TOKEN);
                RemoveEntryList(&(token->node));

        #if BT_SECURITY_TIMEOUT != 0
                /* Cancel the watchdog timer. */
                EVM_CancelTimer(&token->timer);
        #endif /* BT_SECURITY_TIMEOUT != 0 */

                /* Call the protocol */
                MEC(btEvent).eType = eType;
                MEC(btEvent).p.secToken = token;

                if (token->result == 0)
                {
                    token->result = eType;
                }
                else
                {
                    token->id(&(MEC(btEvent)));
                }
                break;

            case BT_STATUS_PENDING:
                /*
                 * This token is not finished yet so keep it on the list. Attempt 
                 * to start any pending operations.
                 */
                MeStartOperation();
                break;

            default:
                Assert(0);
        }
        /* Advance to the next token */
        token = tmp;
    }

    /*
     * There are no more tokens on the list so the access request
     * is complete. Clear the security access state.
     */
    if (IsListEmpty(&(remDev->secTokens)))
    {
        bt_trace(TRACE_GROUP_1, MESEC_SECURITY_TOKEN_LIST_IS_EMPTY);
        remDev->secAccessState = BAS_NO_ACCESS;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESEC_SECURITY_TOKEN_LIST_IS_NOT_EMPTY);
    }
}

#ifdef __BT_4_0_BLE__
BtStatus SecProcessLESecurityToken(BtSecurityToken *token)
{
    BtStatus status = BT_STATUS_FAILED;
    BtDeviceRecord record;

    /* Check bond strength */
    if (token->record != 0)
    {
        status = SEC_FindDeviceRecord(&(token->remDev->bdAddr), &record);
        token->remDev->gapSecurityLevel = token->record->level_type;
        switch (token->record->level_type)
        {
        case BT_SEC_HIGH:
            /* Need authenticated STK */
            if (status != BT_STATUS_SUCCESS || record.gapKeyType != BT_LINK_KEY_GAP_TYPE_AUTHENTICATED)
            {
                BtDeviceContext *bdc = DS_FindDevice(&(token->remDev->bdAddr));
                SmPairAttribute attr = {TRUE, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG, NULL, 0};
                token->remDev->authState = BAS_NOT_AUTHENTICATED;
                status = SM_Bond(bdc, &attr);
                goto done;
            }
        case BT_SEC_MED:
            if (status != BT_STATUS_SUCCESS)
            {
                BtDeviceContext *bdc = DS_FindDevice(&(token->remDev->bdAddr));
                SmPairAttribute attr = {FALSE, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG, NULL, 0};
                token->remDev->authState = BAS_NOT_AUTHENTICATED;
                status = SM_Bond(bdc, &attr);
                goto done;
            }
        default:
            break;
        }
    }

    /* Check if authorization is needed */
    if (token->level & BSL_AUTHORIZATION_IN)
    {
        /* We need to be authorized so see if we already are */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_AUTHORIZATION);
        switch (token->remDev->authorizeState)
        {
            case BAS_NOT_AUTHORIZED:
                /*
                 * We are not authorized so if we have not already atempted
                 * authorization then try it
                 */
                if ((MEC(authorizeHandler) != 0) && ((token->remDev->secAccessState & BAS_AUTHORIZE_COMPLETE) == 0))
                {
                    BtSecurityOp *secOp;
                    /*
                     * We need to request authorization from the user. Schedule
                     * a request for authorization.
                     */
                    bt_trace(TRACE_GROUP_1, MESEC_ADDING_AUTHORIZE_OPERATION_TO_QUEUE);
                    token->remDev->authorizeState = BAS_START_AUTHORIZE;
                    token->remDev->secRec = token->record;

                    /* Get a security operation token */
                    secOp = SecBtGetFreeSecurityOp();
                    if (secOp == 0)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto done;
                    }
                    secOp->remDev = token->remDev;
                    secOp->op.opType = BOP_AUTHORIZE;
                
                    if(!IsEntryAvailable(&(secOp->op.node)))
                    {
                        Assert(0);
                    }
                    if (token->remDev->secAccessState == BAS_NO_ACCESS)
                    {
                        /* This is the first call so put at end of queue */
                        MeAddOperation(secOp);
                    }
                    else
                    {
                        /* We are now in the middle of the process so put in front */
                        MeAddOperationFront(secOp);
                    }
                    goto pending;
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, MESEC_NO_AUTHORIZATION_HANDLE_OR_PREVIOUS_ATTEMPT_FAILED);
                }
                /* Authorization failed so we are done */
                goto done;

            case BAS_START_AUTHORIZE:
                /* Authorization in progress so wait until it finishes */
                goto pending;

            case BAS_AUTHORIZED:
            case BAS_AUTHORIZED_SERVICE:
                /* Already authorized so go to next step */
                break;
        }
    }
    
    /* Check if encryption is needed */
    if ((token->level & BSL_ENCRYPTION_IN) || (token->level & BSL_ENCRYPTION_OUT))
    {
        bt_trace(TRACE_GROUP_1, BTLOG_TOKENREMDEVENCRYPTSTATExD, token->remDev->encryptState);
        switch (token->remDev->encryptState)
        {
            case BES_NOT_ENCRYPTED:
                if (((token->remDev->secAccessState & BAS_ENCRYPT_COMPLETE) == 0))
                {
                    if (token->remDev->remoteLEFeature[0] & 0x1)
                    {
                        if ((status = ME_LE_EncryptLink(token->remDev, NULL)) == BT_STATUS_PENDING)
                        {
                            goto pending;
                        }
                    }
                    else
                    {
                        status = BT_STATUS_NOT_SUPPORTED;
                    }
                }
                /* Encryption failed so we are done */
                goto done;
            case BES_START_ENCRYPT:
                goto pending;
            case BES_ENCRYPTED:
                /* Encryption in place so go to next step */
                break;
        }
    }
    status = BT_STATUS_SUCCESS;
    goto done;
    
pending:
    status = BT_STATUS_PENDING;
done:
    return status;
}
#endif

#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  SecProcessSecurityToken
 * DESCRIPTION
 *  Process the security token.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus SecProcessSecurityToken(BtSecurityToken *token)
{
    BtStatus status;
    BtSecurityLevel mask;
    BtSecurityOp *secOp;
    BtOpType opType;
    BtDeviceRecord record;
    BtSecurityRecord *sr = token->record;
    BtDevType devType;

	if (ME_GetRemDevState(token->remDev) != BDS_CONNECTED){
        OS_Report("SecProcessSecurityToken : remDev state is not connected. Return failed");
        return BT_STATUS_FAILED;
    }

#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
    return SecProcessLESecurityToken(token);
  #endif
    //BtDeviceContext *bdc = DS_FindDevice(&(token->remDev->bdAddr));
    devType = ME_GetDeviceType(&(token->remDev->bdAddr));
    if (devType == BT_DEV_TYPE_LE)

    {
        return SecProcessLESecurityToken(token);
    }
#endif

    /* If SEC_LOW and remote does not support SSP, just pass SEC check */
    OS_Report("SecProcessSecurityToken(token=%p) : sr=%p", token, token->record);
    if(sr && sr->level_type == BT_SEC_LOW){
        switch(ME_IsSSPSupported(token->remDev)){
        case BT_STATUS_NOT_SUPPORTED:
            OS_Report("SEC is low and SSP(host) is not supported");
            status = BT_STATUS_SUCCESS;
            goto done;
        case BT_STATUS_PENDING:
            goto pending;
        case BT_STATUS_SUCCESS:
            OS_Report("SSP supported. keep going");
        default:
            break;
        }
    }

    /* Set the mask to the starting point  */
    if (token->incoming)
    {
        mask = BSL_AUTHENTICATION_IN;
    }
    else
    {
        mask = BSL_AUTHENTICATION_OUT;
    }
    /* Check Authentication strength */
    status = SEC_FindDeviceRecord(&(token->remDev->bdAddr), &record);
    if ((status == BT_STATUS_SUCCESS) && (token->record != 0))
    {
        bt_trace(TRACE_GROUP_1, BTLOG_TOKENRECORDLEVEL_TYPExD, token->record->level_type);
        token->remDev->gapSecurityLevel = token->record->level_type;
        switch (token->record->level_type)
        {
            case BT_SEC_HIGH:
                if ((record.gapKeyType != BT_LINK_KEY_GAP_TYPE_COMBINATION_16) &&
                    (record.gapKeyType != BT_LINK_KEY_GAP_TYPE_AUTHENTICATED))
                {
                    bt_trace(TRACE_GROUP_1, BTLOG_CURRENT_ENCRYPTION_STATExD, token->remDev->encryptState);
                    if (token->remDev->encryptState == BES_ENCRYPTED)
                    {
                        SEC_DeleteDeviceRecord(&token->remDev->bdAddr);

                        token->remDev->authHandler = 0;
                        status = SecAddAuthenticateOpToQueue(token->remDev);
                        if (status != BT_STATUS_PENDING)
                        {
                            status = BT_STATUS_FAILED;
                        }
                        goto done;
                    }
                    else if (token->remDev->encryptState == BES_NOT_ENCRYPTED)
                    {
                        bt_trace(TRACE_GROUP_1, BTLOG_TOKENREMDEVAUTHSTATExD, token->remDev->authState);
                        SEC_DeleteDeviceRecord(&(token->remDev->bdAddr));
                        if (token->remDev->authState == BAS_AUTHENTICATED)
                        {
                            token->remDev->authHandler = 0;
                            token->remDev->authState = BAS_NOT_AUTHENTICATED;
                            token->remDev->secAccessState = BAS_NO_ACCESS;
                            if (token->remDev->state != BDS_CONNECTED)
                            {
                                status = BT_STATUS_FAILED;
                            }
                            else
                            {
                                /* Delete Link Key of Chip */
                                secOp = SecBtGetFreeSecurityOp();
                                if (secOp == 0)
                                {
                                    status = BT_STATUS_NO_RESOURCES;
                                }
                                else
                                {
                                    secOp->remDev = token->remDev;
                                    secOp->op.opType = BOP_DELETE_STORED_LINK_KEY;
                                    if(!IsEntryAvailable(&(secOp->op.node)))
                                    {
                                        Assert(0);
                                    }
                                    MeAddOperation(secOp);
                                    MeStartOperation();
                                    status = BT_STATUS_PENDING;
                                }
                            }
                            goto done;
                        }
                        else
                        {
                            token->remDev->authState = BAS_NOT_AUTHENTICATED;
                            token->remDev->secAccessState = BAS_NO_ACCESS;
                            break;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    else if (token->record != 0)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_TOKENRECORDLEVEL_TYPExD, token->record->level_type);
        /* the remDev->gapSecurityLevel only keep the highest information. */
        token->remDev->gapSecurityLevel = token->record->level_type;
    }

    /* Assume failure */
    status = BT_STATUS_FAILED;

    /* Check if authentication is needed */
    if ((token->level & mask) && (token->remDev->encryptState == BES_NOT_ENCRYPTED))
    {
        /*
         * Authentication is required. Check to see if we need to start
         * authentication.
         */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_AUTHENTICATION);
        bt_trace(TRACE_GROUP_1, BTLOG_TOKENREMDEVAUTHSTATExD, token->remDev->authState);
        switch (token->remDev->authState)
        {
            case BAS_WAITING_FOR_PIN_R:
                /*
                 * This is the case where the remote device attempted to pair
                 * with us. We are waiting for the user to enter a pin code.
                 * This operation could end successfully or result in a limbo
                 * state. We will perform an authenticate operation by falling
                 * thru the case statement.
                 */
            case BAS_WAITING_FOR_KEY_R:
                /*
                 * This is the case where the remote device attempted to pair
                 * with us. We have sent the pin code but have not gotten back
                 * a link key. It could be coming or it may not (limbo state) 
                 * so we will perform an authenticate operation
                 */
            case BAS_NOT_AUTHENTICATED:
                /*
                 * If we have already tried an authenticate operation and it
                 * failed then we are done otherwise start an authentication 
                 * operation. 
                 */
                token->remDev->gapSecurityLevel = token->record->level_type;
                if ((token->remDev->secAccessState & BAS_AUTH_COMPLETE) == 0)
                {
                    /* We can attempt to authenticate */
                    /* The Authentication is not starting or completed */
                    bt_trace(TRACE_GROUP_1, MESEC_ADDING_AUTHENTICATE_OPERATION_TO_QUEUE);
                    token->remDev->authHandler = 0;
                    /* 
                     * Add btbmCheckCreateLinkKeyState() check for MAUI_01921658 
                     * We don't send authenticaton request to controller 
                     * while we are doing bonding/pairing.
                     */
                    //if (!btbmCheckCreateLinkKeyState()){                    
                        status = SecAddAuthenticateOpToQueue(token->remDev);
                        if (status != BT_STATUS_PENDING)
                        {
                            status = BT_STATUS_FAILED;
                        }
                    //}else{
                    //    OS_Report("[SEC] Bond/Pair ongoing did not send auth cmd to controller");
                    //    status = BT_STATUS_PENDING;
                    //}
                }
                goto done;

            case BAS_AUTHENTICATED:
                /* We are authenticated so go on to the next step */
                break;

            default:
                /*
                 * We are in the middle of authentication so wait until it
                 * finishes
                 */
                goto pending;
        }
    }

    /* We are authenticated so see if we need authorization */
    mask <<= 1;
    if (token->level & mask)
    {
        /* We need to be authorized so see if we already are */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_AUTHORIZATION);
        switch (token->remDev->authorizeState)
        {
            case BAS_NOT_AUTHORIZED:
                /*
                 * We are not authorized so if we have not already atempted
                 * authorization then try it
                 */
                if ((MEC(authorizeHandler) != 0) && ((token->remDev->secAccessState & BAS_AUTHORIZE_COMPLETE) == 0))
                {
                    /*
                     * We need to request authorization from the user. Schedule
                     * a request for authorization.
                     */
                    bt_trace(TRACE_GROUP_1, MESEC_ADDING_AUTHORIZE_OPERATION_TO_QUEUE);
                    opType = BOP_AUTHORIZE;
                    token->remDev->authorizeState = BAS_START_AUTHORIZE;
                    token->remDev->secRec = token->record;
                    goto addOp;
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, MESEC_NO_AUTHORIZATION_HANDLE_OR_PREVIOUS_ATTEMPT_FAILED);
                }
                /* Authorization failed so we are done */
                goto done;

            case BAS_START_AUTHORIZE:
                /* Authorization in progress so wait until it finishes */
                goto pending;

            case BAS_AUTHORIZED:
            case BAS_AUTHORIZED_SERVICE:
                /* Already authorized so go to next step */
                break;
        }
    }

    /* We are authenticated and if needed, authorized. See if we need encyrption */
    mask <<= 1;
    if (token->level & mask)
    {
        /* We need to have encryption so see if we already have it */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_ENCRYPTION);
        bt_trace(TRACE_GROUP_1, BTLOG_TOKENREMDEVENCRYPTSTATExD, token->remDev->encryptState);
        switch (token->remDev->encryptState)
        {
            case BES_NOT_ENCRYPTED:
                /*
                 * The link is not encrypted. If we have already attempted then
                 * fail otherwise start it.
                 */
                if(token->remDev->switchState != BM_ROLE_SWITCH_STOP_ENCRYPTION){
                    OS_Report("Auth : Encryption disabled and switcState is %u", token->remDev->switchState);
                    if (((token->remDev->secAccessState & BAS_ENCRYPT_COMPLETE) == 0))
                    {
                        /* We need to start encryption operation. */
                        bt_trace(TRACE_GROUP_1, MESEC_ADDING_ENCRYPTION_OPERATION_TO_QUEUE);
                        opType = BOP_ENCRYPTION;
                        token->remDev->encryptState = BES_START_ENCRYPT;
                        token->remDev->encryptHandler = 0;
                        goto addOp;
                    }
                    /* Encryption failed so we are done */
                    goto done;
                }else{
                    OS_Report("Authenticate encounter role switch disable encryption. Wait for role switch complete and enable encryption again. ");
                }
            case BES_START_ENCRYPT:
            case BES_END_ENCRYPT:
                /* Encryption in progress so wait until it finishes */
                goto pending;

            case BES_ENCRYPTED:
                /* Encryption in place so go to next step */
                break;
        }
    }

    /* If we make to here then access is allowed */
    status = BT_STATUS_SUCCESS;
    if (token->remDev->authorizeState == BAS_AUTHORIZED_SERVICE)
    {
        token->remDev->authorizeState = BAS_NOT_AUTHORIZED;
    }
    goto done;

  addOp:
    /* Get a security operation token */
    secOp = SecBtGetFreeSecurityOp();
    if (secOp == 0)
    {
        goto done;
    }
    secOp->remDev = token->remDev;

    /* If this is encryption set the mode */
    if (opType == BOP_ENCRYPTION)
    {
        secOp->op.opType = BOP_ENCRYPTION;
        secOp->x.eMode = BECM_ENCRYPT_ENABLE;
    }
    else if (opType == BOP_STOP_ENCRYPTION)
    {
        secOp->op.opType = BOP_ENCRYPTION;
        secOp->x.eMode = BECM_ENCRYPT_DISABLE;
    }

    if(!IsEntryAvailable(&(secOp->op.node)))
    {
        Assert(0);
    }
    if (token->remDev->secAccessState == BAS_NO_ACCESS)
    {
        /* This is the first call so put at end of queue */
        MeAddOperation(secOp);
    }
    else
    {
        /* We are now in the middle of the process so put in front */
        MeAddOperationFront(secOp);
    }

  pending:
    status = BT_STATUS_PENDING;
  done:
    return status;
}

#else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  SecProcessSecurityToken
 * DESCRIPTION
 *  
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SecProcessSecurityToken(BtSecurityToken *token)
{
    BtStatus status;
    BtSecurityLevel mask;
    BtSecurityOp *secOp;
    BtOpType opType;

    /* Assume failure */
    status = BT_STATUS_FAILED;

    /* Set the mask to the starting point  */
    if (token->incoming)
    {
        mask = BSL_AUTHENTICATION_IN;
    }
    else
    {
        mask = BSL_AUTHENTICATION_OUT;
    }

    /* Check if authentication is needed */
    if ((token->level & mask) && (token->remDev->encryptState == BES_NOT_ENCRYPTED))
    {
        /*
         * Authentication is required. Check to see if we need to start
         * authentication.
         */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_AUTHENTICATION);
        switch (token->remDev->authState)
        {
            case BAS_WAITING_FOR_PIN_R:
                /*
                 * This is the case where the remote device attempted to pair
                 * with us. We are waiting for the user to enter a pin code.
                 * This operation could end successfully or result in a limbo
                 * state. We will perform an authenticate operation by falling
                 * thru the case statement.
                 */
            case BAS_WAITING_FOR_KEY_R:
                /*
                 * This is the case where the remote device attempted to pair
                 * with us. We have sent the pin code but have not gotten back
                 * a link key. It could be coming or it may not (limbo state) 
                 * so we will perform an authenticate operation
                 */
            case BAS_NOT_AUTHENTICATED:
                /*
                 * If we have already tried an authenticate operation and it
                 * failed then we are done otherwise start an authentication 
                 * operation. 
                 */
                if ((token->remDev->secAccessState & BAS_AUTH_COMPLETE) == 0)
                {
                    /* We can attempt to authenticate */
                    bt_trace(TRACE_GROUP_1, MESEC_ADDING_AUTHENTICATE_OPERATION_TO_QUEUE);
                    token->remDev->authHandler = 0;
                    status = SecAddAuthenticateOpToQueue(token->remDev);
                    if (status != BT_STATUS_PENDING)
                    {
                        status = BT_STATUS_FAILED;
                    }
                }
                goto done;

            case BAS_AUTHENTICATED:
                /* We are authenticated so go on to the next step */
                break;

            default:
                /*
                 * We are in the middle of authentication so wait until it
                 * finishes
                 */
                goto pending;
        }
    }

    /* We are authenticated so see if we need authorization */
    mask <<= 1;
    if (token->level & mask)
    {
        /* We need to be authorized so see if we already are */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_AUTHORIZATION);
        switch (token->remDev->authorizeState)
        {
            case BAS_NOT_AUTHORIZED:
                /*
                 * We are not authorized so if we have not already atempted
                 * authorization then try it
                 */
                if ((MEC(authorizeHandler) != 0) && ((token->remDev->secAccessState & BAS_AUTHORIZE_COMPLETE) == 0))
                {
                    /*
                     * We need to request authorization from the user. Schedule
                     * a request for authorization.
                     */
                    bt_trace(TRACE_GROUP_1, MESEC_ADDING_AUTHORIZE_OPERATION_TO_QUEUE);
                    opType = BOP_AUTHORIZE;
                    token->remDev->authorizeState = BAS_START_AUTHORIZE;
                    token->remDev->secRec = token->record;
                    goto addOp;
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, MESEC_NO_AUTHORIZATION_HANDLE_OR_PREVIOUS_ATTEMPT_FAILED);
                }
                /* Authorization failed so we are done */
                goto done;

            case BAS_START_AUTHORIZE:
                /* Authorization in progress so wait until it finishes */
                goto pending;

            case BAS_AUTHORIZED:
            case BAS_AUTHORIZED_SERVICE:
                /* Already authorized so go to next step */
                break;
        }
    }

    /* We are authenticated and if needed, authorized. See if we need encyrption */
    mask <<= 1;
    if (token->level & mask)
    {
        /* We need to have encryption so see if we already have it */
        bt_trace(TRACE_GROUP_1, MESEC_ACCESS_REQUEST_NEEDS_ENCRYPTION);
        switch (token->remDev->encryptState)
        {
            case BES_NOT_ENCRYPTED:
                /*
                 * The link is not encrypted. If we have already attempted then
                 * fail otherwise start it.
                 */
                if (((token->remDev->secAccessState & BAS_ENCRYPT_COMPLETE) == 0))
                {
                    /* We need to start encryption operation. */
                    bt_trace(TRACE_GROUP_1, MESEC_ADDING_ENCRYPTION_OPERATION_TO_QUEUE);
                    opType = BOP_ENCRYPTION;
                    token->remDev->encryptState = BES_START_ENCRYPT;
                    token->remDev->encryptHandler = 0;
                    goto addOp;
                }
                /* Encryption failed so we are done */
                goto done;

            case BES_START_ENCRYPT:
            case BES_END_ENCRYPT:
                /* Encryption in progress so wait until it finishes */
                goto pending;

            case BES_ENCRYPTED:
                /* Encryption in place so go to next step */
                break;
        }
    }

    /* If we make to here then access is allowed */
    status = BT_STATUS_SUCCESS;
    if (token->remDev->authorizeState == BAS_AUTHORIZED_SERVICE)
    {
        token->remDev->authorizeState = BAS_NOT_AUTHORIZED;
    }
    goto done;

  addOp:
    /* Get a security operation token */
    secOp = SecBtGetFreeSecurityOp();
    if (secOp == 0)
    {
        goto done;
    }
    secOp->op.opType = opType;
    secOp->remDev = token->remDev;

    /* If this is encryption set the mode */
    if (opType == BOP_ENCRYPTION)
    {
        secOp->x.eMode = BECM_ENCRYPT_ENABLE;
    }

    Assert(IsEntryAvailable(&(secOp->op.node)));
    if (token->remDev->secAccessState == BAS_NO_ACCESS)
    {
        /* This is the first call so put at end of queue */
        MeAddOperation(secOp);
    }
    else
    {
        /* We are now in the middle of the process so put in front */
        MeAddOperationFront(secOp);
    }

  pending:
    status = BT_STATUS_PENDING;
  done:
    return status;
}
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  SecAccessTimeoutHandler
 * DESCRIPTION
 *  Handles the timeout of a security token.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecAccessTimeoutHandler(EvmTimer *timer)
{
    BtSecurityToken *token = (BtSecurityToken*) timer->context;

    bt_trace(TRACE_GROUP_1, MESEC_TIMEOUT_ON_SECURITY_ACCESS_REQUEST);

    /* Cancel the original access request */
    (void)SEC_CancelAccessRequest(token);

    /* Notify the caller of the cancellation */
    MEC(btEvent).eType = BTEVENT_ACCESS_DENIED;
    MEC(btEvent).p.secToken = token;
    token->id(&(MEC(btEvent)));
}


/*****************************************************************************
 * FUNCTION
 *  SecSetSecurityMode
 * DESCRIPTION
 *  Set security mode.
 * PARAMETERS
 *  handler     [IN]        
 *  mode        [IN]        
 *  encrypt     [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SecSetSecurityMode(BtHandler *handler, BtSecurityMode mode, BOOL encrypt)
{
    BtStatus status;
    BtSecurityOp *secOp;

#if XA_ERROR_CHECK == XA_ENABLED
    if (handler == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    /* Get a security op and start the process */
    secOp = SecBtGetFreeSecurityOp();
    if (secOp == 0)
    {
        status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        secOp->op.opType = BOP_SECURITY_MODE;
        secOp->x.secMode.mode = mode;
        secOp->x.secMode.encrypt = encrypt;
        MEC(secModeHandler) = handler;

        /* Set the encryption state */
        if (mode == BSM_SEC_LEVEL_3)
        {
            MEC(secModeState) = BSMS_START_LEVEL3;
        }
        else
        {
            MEC(secModeState) = BSMS_END_LEVEL3;
        }

        if(!IsEntryAvailable(&(secOp->op.node)))
        {
            Assert(0);
        }
        MeAddOperation(secOp);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SecSetEncryptMode
 * DESCRIPTION
 *  Set security mode.
 * PARAMETERS
 *  handler         [IN]        
 *  encryption      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SecSetEncryptMode(BtHandler *handler, BOOL encryption)
{
    BtStatus status;
    BtSecurityOp *secOp;

#if XA_ERROR_CHECK == XA_ENABLED
    if (handler == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    /* Get a security op and start the process */
    secOp = SecBtGetFreeSecurityOp();
    if (secOp == 0)
    {
        status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        secOp->op.opType = BOP_SECURITY_MODE;
        secOp->x.secMode.mode = BSM_SEC_ENCRYPT;
        secOp->x.secMode.encrypt = encryption;
        MEC(secModeHandler) = handler;

        if(!IsEntryAvailable(&(secOp->op.node)))
        {
            Assert(0);
        }
        MeAddOperation(secOp);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}
#endif /* BT_SECURITY == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  SecBtGetFreeSecurityOp
 * DESCRIPTION
 *  Return a pointer to a free security op structure
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtSecurityOp *SecBtGetFreeSecurityOp(void)
{
    I8 i;

    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if (MEC(secOpTable)[i].op.opType == BOP_NOP)
        {
            return &(MEC(secOpTable)[i]);
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  SecBtFindSecurityOp
 * DESCRIPTION
 *  Return a pointer to a free security op structure
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtSecurityOp *SecBtFindSecurityOp(BtRemoteDevice *remDev)
{
    I8 i;

    for (i = 0; i < NUM_SEC_OPS; i++)
    {
        if (MEC(secOpTable)[i].op.opType != BOP_NOP && MEC(secOpTable)[i].remDev == remDev)
        {
            return &MEC(secOpTable)[i];
        }
    }
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  HandleSecurityCommandComplete
 * DESCRIPTION
 *  Handle the command complete event for security tasks..
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleSecurityCommandComplete(U8 event, U8 len, U8 *parm)
{
    U16 opcode = 0;
    U8 parms[11];

    /* UNUSED_PARAMETER(len); */

    switch (event)
    {
        case HCE_COMMAND_COMPLETE:
            /* Save the status in case it is an error */
            opcode = LEtoHost16((parm + 1));
            MEC(btEvent).errCode = parm[3];
            bt_trace(TRACE_GROUP_1, MESEC_SECURITY_COMMAND_COMPLETE_EVENT_RECEIVED_ERR__xX, MEC(btEvent).errCode);
            if (parm[3] == 0x02)
            {
                if (opcode == HCC_PIN_CODE_REQ_NEG_REPL)
                {
                    parms[0] = HC_STATUS_HARDWARE_FAILURE;
                    parms[1] = 0xFF;
                    parms[2] = 0x0E;
                    OS_MemCopy(&parms[3], SECOP->bdAddr.addr, 6);
                    parms[9] = 0x01;
                    parms[10] = 0x00;
                    MeHandleConnectComplete(11, parms);
                }
            }
            break;

        case HCE_COMMAND_STATUS:
            /* Save the status in case it is an error */
            opcode = LEtoHost16((parm + 2));
            MEC(btEvent).errCode = parm[0];
            bt_trace(TRACE_GROUP_1, MESEC_SECURITY_COMMAND_STATUS_EVENT_RECEIVED_ERR__xX, MEC(btEvent).errCode);
            Assert(parm[0] != 0);
            break;

        default:
            Assert(0);
    }

    /* Make sure the opcode matches. If not ignore the response */
    if (opcode == MEC(curTask))
    {
        /* Clear the task */
        MEC(curTask) = 0;
        /* Call the operation handler */
        MEC(opHandler) (BOE_TASK_END);
    }
    else
    {
        /* Ignore this command complete event */
        bt_trace(
            TRACE_GROUP_1,
            MESEC_HANDLESECURITYCOMMANDCOMPLETE_OPCODE_MISMATCHOPCODE__xX_CURTASK__xX,
            LEtoHost16((parm + 1)),
            MEC(curTask));
    }
}

static void SecHandleErrorTaskEnd(void)
{
    Assert(0);
}

#if BT_SECURITY == XA_DISABLED


/*****************************************************************************
 * FUNCTION
 *  SecHandleDenyPinCodeReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleDenyPinCodeReq(U8 len, U8 *parm)
{

}


/*****************************************************************************
 * FUNCTION
 *  SecHandleDenyPinCodeRspOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SecHandleDenyPinCodeRspOp(BtOpEvent event)
{
}


/*****************************************************************************
 * FUNCTION
 *  StartDenyPinCodeRspTask
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartDenyPinCodeRspTask(void)
{
}

#endif /* BT_SECURITY == XA_DISABLED */

