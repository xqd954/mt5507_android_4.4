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
 * BtAccCtrl.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains the access control policy to enable only several wanted profiles.
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CCCQ. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Aug 5 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jun 26 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * Oct 8 2007 mtk01239
 * [MAUI_00556508] [BT]Shrinking BT code size
 * 
 *
 * Sep 3 2007 mtk01411
 * [MAUI_00541459] [Bluetooth] Add function header comments
 * 
 *
 * Jul 15 2007 mtk01411
 * [MAUI_00416972] [Bluetooth] SPP with SCO functionality
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Mar 26 2007 mtk01239
 * [MAUI_00231225] bt_hid disconnected indication pop up, but it return to disconnecting screen
 * 
 *
 * Jan 8 2007 mtk01411
 * [MAUI_00355519] [New Feature] Bluetooth SIM Access Profile w0701 check in
 * Modify btAccCtrl.c : check accessiblity , should not return TRUE directly
 *
 * Jan 2 2007 mtk01411
 * [MAUI_00355519] [New Feature] Bluetooth SIM Access Profile w0701 check in
 * Add MTKACCCTLC(accCtrlSet) = FALSE in btAccCtrlUnset()
 *
 * Dec 31 2006 mtk01239
 * [MAUI_00355405] [Bluetooth] turn off the panic instead of assert
 * 
 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#include "bt_common.h"
#include "bluetooth_gap_struct.h"
#include "btaccctrl.h"
#include "sdp.h"

/* Add by mtk01411 2007-0330 */
#include "btconfig.h"

/***************
Internal Globals 
****************/
/* MTK Bluetooth Access Controll Context */
#ifdef __LINUX_SUPPRESS_ERROR__
static BTACCCTRLCONTENT_T BtAccController;  /* = {0}; */
#else
static BTACCCTRLCONTENT_T BtAccController = {0};
#endif

/*********************
Macros With Parameters 
**********************/
#define MTKACCCTLC(f)  (BtAccController.f)

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlInit
 * DESCRIPTION
 *  This function is used to initialize the related settings for access control policy
 * PARAMETERS
 *  void
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlInit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MTKACCCTLC(accCtrlSet) = FALSE;
    MTKACCCTLC(allowedPsm) = BT_PSM_POLICY_ALL;
    MTKACCCTLC(allowedService) = BT_SERVICE_POLICY_ALL;
    MTKACCCTLC(accLink) = 0;
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlSet
 * DESCRIPTION
 *  This function is used to set access control policy.
 *  Two parameters are passed to this function: one is used to set the allowed PSM value, other is used to set the allowed service
 * PARAMETERS
 *  allowed_PSM         [IN]        E.g, BT_PSM_POLICY_SDP
 *  allowedService      [IN]        E.g, BT_SERVICE_POLICY_SAP
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlSet(U32 allowed_PSM, U32 allowedService)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(TRACE_GROUP_1, BTACCCTRLSET_ALLOWEDPSMxDALLOWEDSERVICExD, allowed_PSM, allowedService);
    MTKACCCTLC(accCtrlSet) = TRUE;
    MTKACCCTLC(allowedPsm) = allowed_PSM;
    MTKACCCTLC(allowedService) = allowedService;
    MTKACCCTLC(accLink) = 0;
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlUnset
 * DESCRIPTION
 *  This function is used to remove the limitation of access control policy and reset to default setting.
 *  The default setting of access control policy is to allow all PSM values and all services.
 * PARAMETERS
 *  void
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlUnset(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(TRACE_GROUP_1, BTACCCTRLUNSET);
    MTKACCCTLC(allowedPsm) = BT_PSM_POLICY_ALL;
    MTKACCCTLC(allowedService) = BT_SERVICE_POLICY_ALL;
    MTKACCCTLC(accLink) = 0;
    MTKACCCTLC(accCtrlSet) = FALSE;
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlAllowDeviceWithService
 * DESCRIPTION
 *  This function is used to set access control policy for some device.
 *  Three parameters are passed to this function:
 *  1st one is the device which will be applied the policy for it
 *  2nd one is used to set the allowed PSM values
 *  3rd one is used to set the allowed services
 * PARAMETERS
 *  link                [IN]        
 *  allowed_PSM         [IN]        E.g, BT_PSM_POLICY_SDP
 *  allowedService      [IN]        E.g, BT_SERVICE_POLICY_SAP
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlAllowDeviceWithService(BtRemoteDevice *link, U32 allowed_PSM, U32 allowedService)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(TRACE_GROUP_1, BTACCCTRLALLOWDEVICEALLSERVICE);
    if (MTKACCCTLC(accCtrlSet) != TRUE)
    {
        return FALSE;
    }
    if (MTKACCCTLC(accLink) != 0)
    {
        bt_trace(TRACE_GROUP_1, MTKACCCTLCACCLINK__0);

        if (link == MTKACCCTLC(accLink))
        {
            bt_trace(TRACE_GROUP_1, SET_MTKACCCTLCACCLINK);
            MTKACCCTLC(accLink) = link;
            MTKACCCTLC(allowedPsm) = allowed_PSM;
            MTKACCCTLC(allowedService) = allowedService;
        }
        else
        {
            bt_trace(TRACE_GROUP_1, NEW_LINK_AND_STORED_LINK_IS_NOT_THE_SAME);
            return FALSE;
        }
    }
    else
    {
        MTKACCCTLC(accLink) = link;
        MTKACCCTLC(allowedPsm) = allowed_PSM;
        MTKACCCTLC(allowedService) = allowedService;
    }
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlResetServiceTable
 * DESCRIPTION
 *  This function is used to reset the service table with the allowed PSM value and service passed to this function
 *  Two parameters are passed to this function to reset the service table
 * PARAMETERS
 *  allowed_PSM         [IN]        E.g, BT_PSM_POLICY_SDP
 *  allowedService      [IN]        E.g, BT_SERVICE_POLICY_SAP
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlResetServiceTable(U32 allowed_PSM, U32 allowedService)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (MTKACCCTLC(accCtrlSet) != TRUE)
    {
        return FALSE;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTACCCTRLDISALLOWDEVICEALLSERVICE);
        MTKACCCTLC(accLink) = 0;
        MTKACCCTLC(allowedPsm) = allowed_PSM;
        MTKACCCTLC(allowedService) = allowedService;
        return TRUE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlCheckConnectPolicy
 * DESCRIPTION
 *  This fuction is used to check if the ACL connection from some bluetooth address is allowed or not
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlCheckConnectPolicy(BD_ADDR *bdAddr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL status = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(TRACE_GROUP_1, MTKACCCTLCACCCTRLSETBT_DEBUGx02X, MTKACCCTLC(accCtrlSet));
    if (MTKACCCTLC(accCtrlSet) == TRUE)
    {
        bt_trace(
            TRACE_GROUP_1,
            THE_INPUT_ADDRBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
            bdAddr->addr[0],
            bdAddr->addr[1],
            bdAddr->addr[2],
            bdAddr->addr[3],
            bdAddr->addr[4],
            bdAddr->addr[5]);
        if (MTKACCCTLC(accLink) != 0)
        {
            bt_trace(
                TRACE_GROUP_1,
                THE_CMP_ADDRBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
                MTKACCCTLC(accLink)->bdAddr.addr[0],
                MTKACCCTLC(accLink)->bdAddr.addr[1],
                MTKACCCTLC(accLink)->bdAddr.addr[2],
                MTKACCCTLC(accLink)->bdAddr.addr[3],
                MTKACCCTLC(accLink)->bdAddr.addr[4],
                MTKACCCTLC(accLink)->bdAddr.addr[5]);
            if (StrnCmp(bdAddr->addr, 6, MTKACCCTLC(accLink)->bdAddr.addr, 6))
            {
                status = TRUE;
            }
            else
            {
                status = FALSE;
            }
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BTACCCTRLCHECKCONNECTPOLICY_DENY);
            status = FALSE;
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_BTACCCTRLCHECKCONNECTPOLICY);
        status = TRUE;
    }
    bt_trace(TRACE_GROUP_1, RETURN_STATUSBT_DEBUGx04X, status);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlCheckPSMPolicy
 * DESCRIPTION
 *  This function is used to check if this PSM value is allowed or not according to current access control policy
 * PARAMETERS
 *  PSM_info        [IN]        
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlCheckPSMPolicy(U16 PSM_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL status = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(TRACE_GROUP_1, MTKACCCTLCACCCTRLSETBT_DEBUGx02X, MTKACCCTLC(accCtrlSet));
    bt_trace(TRACE_GROUP_1, THE_INPUT_PSMBT_DEBUGx04X, PSM_info);
    if (MTKACCCTLC(accCtrlSet) == TRUE)
    {
        bt_trace(TRACE_GROUP_1, BTACCCTRLCHECKPSMPOLICY_SHALL_CHECK_POLICY);
        switch (PSM_info)
        {
            case PROT_SDP:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_SDP) != 0)
                {
                    status = TRUE;
                }
                break;
            case PROT_RFCOMM:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_RFCOMM) != 0)
                {
                    status = TRUE;
                }
                break;
            case PROT_TCS_BIN:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_TCS) != 0)
                {
                    status = TRUE;
                }
                break;
            case PROT_AVCTP:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_AVCTP) != 0)
                {
                    status = TRUE;
                }
                break;
            case PROT_AVDTP:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_AVDTP) != 0)
                {
                    status = TRUE;
                }
                break;
            case PROT_HIDP:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_HID) != 0)
                {
                    status = TRUE;
                }
                break;
            case PROT_BNEP:
                if ((MTKACCCTLC(allowedPsm) & BT_PSM_POLICY_BNEP) != 0)
                {
                    status = TRUE;
                }
                break;
            default:
                status = FALSE;
                break;
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_BTACCCTRLCHECKPSMPOLICY);
        status = TRUE;
    }
    bt_trace(TRACE_GROUP_1, RETURN_STATUSBT_DEBUGx04X, status);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  BTAccCtrlCheckServicePolicy
 * DESCRIPTION
 *  This function is used to check this service is allowed to be connected or not
 * PARAMETERS
 *  serv_chnl       [IN]        
 * RETURNS
 * BOOL
 *****************************************************************************/
BOOL BTAccCtrlCheckServicePolicy(U8 serv_chnl)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL status = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(TRACE_GROUP_1, MTKACCCTLCACCCTRLSETBT_DEBUGx02X, MTKACCCTLC(accCtrlSet));
    bt_trace(TRACE_GROUP_1, THE_INPUT_SERV_CHNLBT_DEBUGx02X, serv_chnl);
    bt_trace(TRACE_GROUP_1, ALLOWED_SERVICEx08X, MTKACCCTLC(allowedService));
    if (MTKACCCTLC(accCtrlSet) == TRUE)
    {
        bt_trace(TRACE_GROUP_1, BTACCCTRLCHECKSERVICEPOLICY_DENY);

    #if SPP_SERVER == XA_ENABLED
        if (((serv_chnl >= SPP_CHNL_NUM_START) && (serv_chnl <= SPP_CHNL_NUM_END))
    #if DUN_SERVER == XA_ENABLED
            || ((serv_chnl >= DUN_CHNL_NUM_START) && (serv_chnl <= DUN_CHNL_NUM_END))
    #endif 
    #if SPP_SCO_SERVER == XA_ENABLED
            || ((serv_chnl >= SPP_SCO_CHNL_NUM_START) && (serv_chnl <= SPP_SCO_CHNL_NUM_END))
    #endif 
            )   /* This ')' is used for if's condition */

        {
            bt_trace(TRACE_GROUP_1, CHECK_SPPDUNLAN_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_SPP) != 0)
            {
                status = TRUE;
            }
        }
        else
    #endif /* SPP_SERVER == XA_ENABLED */

    #if FAX_SERVER == XA_ENABLED
        if ((serv_chnl >= FAX_CHNL_NUM_START) && (serv_chnl <= FAX_CHNL_NUM_END))
        {
            bt_trace(TRACE_GROUP_1, CHECK_FAX_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_FAX) != 0)
            {
                status = TRUE;
            }
        }
        else
    #endif /* FAX_SERVER == XA_ENABLED */
        if ((serv_chnl >= HS_AG_CHNL_NUM_START) && (serv_chnl <= HS_AG_CHNL_NUM_END))
        {
            bt_trace(TRACE_GROUP_1, CHECK_HS_AG_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_HSP) != 0)
            {
                status = TRUE;
            }
        }
        else if ((serv_chnl >= HF_GW_CHNL_NUM_START) && (serv_chnl <= HF_GW_CHNL_NUM_END))
        {
            bt_trace(TRACE_GROUP_1, CHECK_HS_GW_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_HFP) != 0)
            {
                status = TRUE;
            }

        }
        else if ((serv_chnl >= FTP_CHNL_NUM_START) && (serv_chnl <= FTP_CHNL_NUM_END))
        {
            bt_trace(TRACE_GROUP_1, CHECK_FTP_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_FTP) != 0)
            {
                status = TRUE;
            }
        }
        else if ((serv_chnl >= OPP_CHNL_NUM_START) && (serv_chnl <= OPP_CHNL_NUM_END))
        {
            bt_trace(TRACE_GROUP_1, CHECK_OPP_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_OPP) != 0)
            {
                status = TRUE;
            }
        }
        else if ((serv_chnl >= SIMAP_CHNL_NUM_START) && (serv_chnl <= SIMAP_CHNL_NUM_END))
        {
            bt_trace(TRACE_GROUP_1, CHECK_SIMAP_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_SAP) != 0)
            {
                status = TRUE;
            }
        }
        else if (serv_chnl == RFCOMMTESTER_RFCOMM_SRV_CHN)
        {
            bt_trace(TRACE_GROUP_1, CHECK_RFCOMM_TESTER_POLICY);
            if ((MTKACCCTLC(allowedService) & BT_SERVICE_POLICY_TEST) != 0)
            {
                status = TRUE;
            }
        }
        else
        {
            status = FALSE;
        }

    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_BTACCCTRLCHECKSERVICEPOLICY);
        status = TRUE;
    }
    bt_trace(TRACE_GROUP_1, RETURN_STATUSBT_DEBUGx04X, status);
    return status;
}

