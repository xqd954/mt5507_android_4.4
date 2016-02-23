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
 *     $Workfile:meacc.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the code for the accessibility 
 *     functions of Bluetooth Management Entity. This includes
 *     connectability, discoverability, and pageablility.
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
#include "x_file.h"

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
extern U16 BT_XFile_EncryptionCommandOpcode(U16 opcode);

static BOOL StartWriteIacLap(BtAccessibleMode newMode);
static void StartWriteScanEnable(BtAccessibleMode mode);

#define ACCOP ((BtScanOp*)MEC(curOp))


/*****************************************************************************
 * FUNCTION
 *  StartWriteScanEnable
 * DESCRIPTION
 *  Start a write scan enable function.
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void StartWriteScanEnable(BtAccessibleMode mode)
{
    /* Send the command */
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    MEC(hciGenCmd).parms[0] = mode & 0x03;
    bt_trace(TRACE_GROUP_1, MEACC_STARTING_WRITE_SCAN_ENABLE);
    bt_trace(TRACE_GROUP_1, WRITE_MODE_BT_DEBUGx02X, MEC(hciGenCmd).parms[0]);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
    MeSendHciCommand(HCC_WRITE_SCAN_ENABLE, 1);
}

/*****************************************************************************
 * FUNCTION
 *  StartWriteIacLap
 * DESCRIPTION
 *  If needed start a Write IAC LAP command.
 * PARAMETERS
 *  newMode     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL StartWriteIacLap(BtAccessibleMode newMode)
{
    U8 num;

    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
    /* First determine if a IAC change is needed */
    num = 1;
    if ((newMode & 0x10) == 0x10)
    {
        /* Changing from GIAC to LIAC so write one */
        num = 2;
        MEC(isLiac) = TRUE;
    }
    else
    {
        MEC(isLiac) = FALSE;
    }
    
 #if 0   
    /* Load up the GIAC */
    MEC(hciGenCmd).parms[0] = num;
    MEC(hciGenCmd).parms[1] = 0x33;
    MEC(hciGenCmd).parms[2] = 0x8B;
    MEC(hciGenCmd).parms[3] = 0x9E;

    if (num == 2)
    {
        /* Load up the LIAC */
        MEC(hciGenCmd).parms[4] = 0x00;
        MEC(hciGenCmd).parms[5] = 0x8B;
        MEC(hciGenCmd).parms[6] = 0x9E;
    }
#else
    /* Load up the GIAC */
    MEC(hciGenCmd).parms[0] = 2;
    MEC(hciGenCmd).parms[1] = 0x33;
    MEC(hciGenCmd).parms[2] = 0x8B;
    MEC(hciGenCmd).parms[3] = 0x9E;
    MEC(hciGenCmd).parms[4] = 0x33;
    MEC(hciGenCmd).parms[5] = 0x8B;
    MEC(hciGenCmd).parms[6] = 0x9E;
    num = 2;
    
    if (MEC(isLiac) == TRUE)
    {
        /* Load up the LIAC */
        MEC(hciGenCmd).parms[4] = 0x00;
        MEC(hciGenCmd).parms[5] = 0x8B;
        MEC(hciGenCmd).parms[6] = 0x9E;
    }
#endif
    Report(("Write IAC LAP number:%d", num));
    Report(("IAC LAP parm: %02X,%02X,%02X,%02X,%02X,%02X,%02X",MEC(hciGenCmd).parms[0],MEC(hciGenCmd).parms[1],MEC(hciGenCmd).parms[2],MEC(hciGenCmd).parms[3],MEC(hciGenCmd).parms[4],MEC(hciGenCmd).parms[5],MEC(hciGenCmd).parms[6]));
    bt_trace(TRACE_GROUP_1, MEACC_STARTING_WRITE_CURRENT_IAC_LAP);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, (U8) (1 + (3 * num)));
    MeSendHciCommand(HCC_WRITE_CURRENT_IAC_LAP, (U8) (1 + (3 * num)));
    return TRUE;
}



/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteScanModeOp
 * DESCRIPTION
 *  Handle the change write Scan mode operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteScanModeOp(BtOpEvent event)
{
    U8 operationFailedResult = 0;
    
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StartWriteScanEnable(ACCOP->mode);
            return;
        case BOE_TASK_END:
            MEC(btEvent).errCode = 0x00;
            bt_trace(TRACE_GROUP_1, MEACC_WRITE_SCAN_ENABLE_COMPLETE);
            break;
        case BOE_TASK_ERR:
            /* The operation failed so report it to the clients */
            bt_trace(TRACE_GROUP_1, MEACC_THE_SETACCESSIBLEMODE_OPERATION_FAILED, MEC(btEvent).errCode);
            MEC(btEvent).errCode = 0x0C;
            operationFailedResult = 1;
            break;
        default:
            Assert(0);
            break;
    }

    /* Report the results to the clients */
    MEC(btEvent).eType = BTEVENT_ACCESSIBLE_CHANGE;
    MEC(btEvent).p.aMode = ACCOP->mode;
    MeReportResults(BEM_ACCESSIBLE_CHANGE);
    /* Start another operation if one exists */
    MeOperationEnd();
    if((operationFailedResult !=1) && (MEC(accModeCur) != MEC(accModeExpValue)))
    {
        Report(("Current Access Mode is not the same:%02x,%02x",MEC(accModeCur), MEC(accModeExpValue)));
        ME_SetAccessibleModeNC(MEC(accModeExpValue));

    }
}



/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteIacOp
 * DESCRIPTION
 *  Handle the change inquiry access code operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteIacOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StartWriteIacLap(ACCOP->mode);
            return;
        case BOE_TASK_END:
            MEC(btEvent).errCode = 0x00;
            bt_trace(TRACE_GROUP_1, MEACC_WRITE_IAC_LAP_COMPLETE);
            break;
        case BOE_TASK_ERR:
            /* The operation failed so report it to the clients */
            bt_trace(TRACE_GROUP_1, MEACC_THE_SETACCESSIBLEMODE_OPERATION_FAILED, MEC(btEvent).errCode);
            MEC(btEvent).errCode = 0x0C;
            break;
        default:
            Assert(0);
            break;
    }
    MeOperationEnd();
    
}
