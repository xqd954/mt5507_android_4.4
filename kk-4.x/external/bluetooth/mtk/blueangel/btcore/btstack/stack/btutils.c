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
 *     $Workfile:btutils.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Utility functions for protocol stack components.
 *
 * Created:
 *     October 7, 1999
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

#include "osapi.h"
#include "bttypes.h"
#include "utils.h"
#include "me_adp.h"
#include "bluetooth_gap_struct.h"
#include "smi.h"

/*****************************************************************************
 * FUNCTION
 *  bdaddr_aton
 * DESCRIPTION
 *  Convert a MSB ASCII bb:cc:dd:ee:ff to a LSB BD_ADDR value. The
 *  Generic Access profile specifies MSB order at the UI level. 
 * PARAMETERS
 *  Addr        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BD_ADDR bdaddr_aton(const char *Addr)
{
    U8 val = 0, i = 6;
    BD_ADDR bd_addr;

    while (*Addr)
    {
        if (*Addr >= '0' && *Addr <= '9')
        {
            val = (U8) ((val << 4) + *Addr - '0');
        }
        else if (*Addr >= 'A' && *Addr <= 'F')
        {
            val = (U8) ((val << 4) + *Addr - 'A' + 10);
        }
        else if (*Addr >= 'a' && *Addr <= 'f')
        {
            val = (U8) ((val << 4) + *Addr - 'a' + 10);
        }
        else
        {
            bd_addr.addr[--i] = val;
        }

        Addr++;
    }

    bd_addr.addr[--i] = val;
    Assert(i == 0);

    return bd_addr;
}


/*****************************************************************************
 * FUNCTION
 *  bdaddr_ntoa
 * DESCRIPTION
 *  Convert a LSB BD_ADDR to MSB ASCII notation aa:bb:cc:dd:ee:ff.
 *  The Generic Access profile specifies MSB order at the UI level.
 *  
 *  AddrString should point to no less than BDADDR_NTOA_SIZE bytes.
 *  It will be null-terminated upon return.
 * PARAMETERS
 *  Addr            [IN]        
 *  AddrString      [IN] 
 * RETURNS
 *  pointer to the converted/provided AddrString
 *****************************************************************************/
char *bdaddr_ntoa(const BD_ADDR *Addr, char *AddrString)
{
    char *bp = AddrString;
    U8 u, l;
    I8 i = 6;

    while (i > 0)
    {
        u = (U8) (Addr->addr[i - 1] / 16);
        l = (U8) (Addr->addr[i - 1] % 16);

        if (u < 10)
        {
            *bp++ = (U8) ('0' + u);
        }
        else
        {
            *bp++ = (U8) ('A' + u - 10);
        }

        if (l < 10)
        {
            *bp++ = (U8) ('0' + l);
        }
        else
        {
            *bp++ = (U8) ('A' + l - 10);
        }

        *bp++ = ':';
        i--;
    }

    *--bp = 0;

    return AddrString;
}

/*****************************************************************************
 * FUNCTION
 *  CONVERT_BDADDR2ARRAY
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [OUT]         
 *  lap         [IN]        
 *  uap         [IN]        
 *  nap         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap)
{
    dest[0] = (U8) (lap & 0x000000FF);
    dest[1] = (U8) ((lap & 0x0000FF00) >> 8);
    dest[2] = (U8) ((lap & 0x00FF0000) >> 16);
    dest[3] = uap;
    dest[4] = (U8) (nap & 0x00FF);
    dest[5] = (U8) ((nap & 0xFF00) >> 8);
}

/*****************************************************************************
 * FUNCTION
 *  CONVERT_BDADDRSRC2ARRAY
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [OUT]         
 *  source         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CONVERT_BDADDRSRC2ARRAY(U8 *dest, bt_addr_struct *source)
{
    dest[0] = (U8) (source->lap & 0x000000FF);
    dest[1] = (U8) ((source->lap & 0x0000FF00) >> 8);
    dest[2] = (U8) ((source->lap & 0x00FF0000) >> 16);
    dest[3] = source->uap;
    dest[4] = (U8) (source->nap & 0x00FF);
    dest[5] = (U8) ((source->nap & 0xFF00) >> 8);

}

/*****************************************************************************
 * FUNCTION
 *  CONVERT_ARRAY2BDADDR
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [OUT]     
 *  src         [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void CONVERT_ARRAY2BDADDR(bt_addr_struct *dest, const U8 *src)
{
    dest->nap = (((U16) src[4])) | ((U16) src[5] << 8);
    dest->uap = (U8) src[3];
    dest->lap = (((U32) src[2]) << 16) | ((U32) src[1] << 8) | ((U32) src[0]);
}


/*****************************************************************************
 * FUNCTION
 *  bt_util_convert_err1
 * DESCRIPTION
 *  The handle the current bonding procedure.
 * PARAMETERS
 *  bonding_state       [IN]        The current bonding state;
 * RETURNS
 *  void
 *****************************************************************************/
btbm_gap_result bt_util_convert_err1(BtErrorCode errCode)
{
    switch (errCode)
    {
    case BEC_PAGE_TIMEOUT:
    case BEC_CONNECTION_TIMEOUT:
    case BEC_HOST_TIMEOUT:
        return BTBM_ADP_FAIL_TIMEOUT;
    case BEC_AUTHENTICATE_FAILURE:
    case BEC_MISSING_KEY:
        return BTBM_ADP_FAIL_KEYERR;
    case BEC_PAIRING_NOT_ALLOWED:
        return BTBM_ADP_FAIL_REMOTE_REJECT;
    default:
        return BTBM_ADP_FAILED;
    }        
}

btbm_gap_result bt_util_convert_err2(BtStatus status)
{
    switch (status)
    {
    case BT_STATUS_SUCCESS:
        return BTBM_ADP_SUCCESS;
    case BT_STATUS_BUSY:
    case BT_STATUS_IN_USE:
    case BT_STATUS_IN_PROGRESS:
        return BTBM_ADP_FAIL_BUSY;
    case BT_STATUS_TIMEOUT:
        return BTBM_ADP_FAIL_TIMEOUT;
    case BT_STATUS_RESTRICTED:
    case BT_STATUS_BLOCK_LIST:
    case BT_STATUS_CHIP_REASON:
        return BTBM_ADP_FAIL_NOT_ALLOW;
    case BT_STATUS_REMOTE_REJECT:
        return BTBM_ADP_FAIL_REMOTE_REJECT;
    case BT_STATUS_KEY_ERR:
        return BTBM_ADP_FAIL_KEYERR;
    case BT_STATUS_PENDING:
        Assert(0);
    default:
        return BTBM_ADP_FAILED;
    }        
}

BtStatus bt_util_convert_err3(BtErrorCode errCode)
{
    switch (errCode)
    {
    case BEC_NO_ERROR:
        return BT_STATUS_SUCCESS;
    case BEC_NO_CONNECTION:
        return BT_STATUS_NO_CONNECTION;
    case BEC_HARDWARE_FAILURE:
        return BT_STATUS_CHIP_REASON;
    case BEC_PAGE_TIMEOUT:
    case BEC_CONNECTION_TIMEOUT:
    case BEC_HOST_TIMEOUT:
    case BEC_LMP_RESPONSE_TIMEOUT:
    case BEC_DIRECTED_ADV_TIMEOUT:
        return BT_STATUS_TIMEOUT;
    case BEC_AUTHENTICATE_FAILURE:
    case BEC_MISSING_KEY:
    case BEC_SECURITY_ERROR:
    case BEC_UNIT_KEY_USED:
    case BEC_MIC_FAILURE:
        return BT_STATUS_KEY_ERR;
    case BEC_ACL_ALREADY_EXISTS:
        return BT_STATUS_CONNECTION_EXIST;
    case BEC_MAX_CONNECTIONS:
    case BEC_MAX_SCO_CONNECTIONS:
    case BEC_LIMITED_RESOURCE:
    case BEC_LOW_RESOURCES:
    case BEC_INSUFF_RESOURCES_FOR_SCATTER_MODE:
    case BEC_MEMORY_FULL:
        return BT_STATUS_NO_RESOURCES;
    case BEC_PERSONAL_DEVICE:
        return BT_STATUS_BLOCK_LIST;
    case BEC_UNSUPPORTED_FEATURE:
    case BEC_UNSUPPORTED_REMOTE:
    case BEC_QOS_NOT_SUPPORTED:
    case BEC_PAIR_UNITKEY_NO_SUPP:
    case BEC_CHANNEL_CLASSIF_NOT_SUPPORTED:
    case BEC_HOST_NOT_SUPPORT_SSP:
        return BT_STATUS_NOT_SUPPORTED;
    case BEC_INVALID_HCI_PARM:
    case BEC_INVALID_LMP_PARM:
    case BEC_UNSUPPORTED_LMP_PARM:
    case BEC_QOS_UNACCEPTABLE_PARAMETER:
    case BEC_PARAMETER_OUT_OF_MANDATORY_RANGE:
        return BT_STATUS_INVALID_PARM;
    case BEC_SCO_OFFSET_REJECT:
    case BEC_SCO_INTERVAL_REJECT:
    case BEC_SCO_AIR_MODE_REJECT:
        return BT_STATUS_SCO_REJECT;
    case BEC_ROLE_SWITCH_PENDING:
    case BEC_SCATTER_MODE_PARM_CHNG_PENDING:
    case BEC_HOST_BUSY_PAIRING:
    case BEC_CONTROLLER_BUSY:
    case BEC_DIFFERENT_TRANSACTION_COLLISION:
        return BT_STATUS_BUSY;
    case BEC_CONN_ESTABLISH_FAIL:
    case BEC_MAC_CONN_FAILED:
        return BT_STATUS_CONNECTION_FAILED;
    case BEC_NOT_FOUND:
        return BT_STATUS_NOT_FOUND;
    case BEC_REQUEST_CANCELLED:
        return BT_STATUS_CANCELLED;
    case BEC_USER_TERMINATED:
    case BEC_UNACCEPTABLE_CONN_INTERVAL:
        return BT_STATUS_REMOTE_REJECT;
    case BEC_LMP_TRANS_COLLISION:
        return BT_STATUS_IN_PROGRESS;

    case BEC_UNKNOWN_HCI_CMD:
    case BEC_COMMAND_DISALLOWED:
    case BEC_POWER_OFF:
    case BEC_LOCAL_TERMINATED:
    case BEC_REPEATED_ATTEMPTS:
    case BEC_PAIRING_NOT_ALLOWED:
    case BEC_UNKNOWN_LMP_PDU:
    case BEC_UNSPECIFIED_ERR:
    case BEC_ROLE_CHG_NOT_ALLOWED:
    case BEC_LMP_PDU_NOT_ALLOWED:
    case BEC_ENCRYP_MODE_NOT_ACC:
    case BEC_INSTANT_PASSED:
    case BEC_QOS_REJECTED:
    case BEC_INSUFFICIENT_SECURITY:
    case BEC_SCATTER_MODE_NO_LONGER_REQUIRED:
    case BEC_RESERVED_SLOT_VIOLATION:
    case BEC_SWITCH_FAILED:
    case BEC_EIR_TOO_LARGE:
    case BEC_NO_SUITABLE_CHANNEL:
    default:
        return BT_STATUS_FAILED;
    }        
}

BtErrorCode bt_util_convert_err4(HcStatus status)
{
    /* 1-to-1 mapping */
    return status;
}

#ifdef __BT_4_0_BLE__
BtErrorCode bt_util_convert_err5(SmPairFailReason status)
{
    switch(status)
    {
    case SM_REASON_PASSKEY_ENTRY_FAILED:
        return BEC_AUTHENTICATE_FAILURE;
    case SM_REASON_OOB_NOT_AVAILABLE:
        return BEC_SECURITY_ERROR;
    case SM_REASON_AUTH_REQUIREMENT:
        return BEC_INSUFFICIENT_SECURITY;
    case SM_REASON_CONFIRM_VALUE_FAILED:
        return BEC_SECURITY_ERROR;
    case SM_REASON_PAIR_NOT_SUPPORTED:
        return BEC_UNSUPPORTED_FEATURE;
    case SM_REASON_ENCRYPT_KEY_SIZE:
        return BEC_INSUFFICIENT_SECURITY;
    case SM_REASON_COMMAND_NOT_SUPPORTED:
        return BEC_UNSUPPORTED_FEATURE;
    case SM_REASON_UNSPECIFIED:
        return BEC_UNSPECIFIED_ERR;
    case SM_REASON_REPEATED_ATTEMPTS:
        return BEC_REPEATED_ATTEMPTS;
    case SM_REASON_INVALID_PARAM:
        return BEC_PARAMETER_OUT_OF_MANDATORY_RANGE;
    default:
        OS_Report("bt_util_convert_err5 : no matching error code : status=%d", status);
        break;
    }
    return status;
}

btbm_device_type bt_util_convert_dev(BtDevType type)
{
    switch(type)
    {
    case BT_DEV_TYPE_LE:
        return BTBM_DEVICE_TYPE_LE;
    case BT_DEV_TYPE_BR_EDR:
        return BTBM_DEVICE_TYPE_BR_EDR;
    case BT_DEV_TYPE_BR_EDR_LE:
        return BTBM_DEVICE_TYPE_BR_EDR_LE;
    default:
   //     Assert(0);
    	return BTBM_DEVICE_TYPE_BR_EDR;
    }
}

#endif

void bt_parm_trace(U8 len, U8 *parms)
{
    int i = 0;
    for (i = 0; i < len; i += 10)
    {
        OS_Report("parm: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X", 
                  parms[i], parms[i+1], parms[i+2], parms[i+3], parms[i+4], parms[i+5], parms[i+6], parms[i+7], parms[i+8], parms[i+9]);
    }
}

