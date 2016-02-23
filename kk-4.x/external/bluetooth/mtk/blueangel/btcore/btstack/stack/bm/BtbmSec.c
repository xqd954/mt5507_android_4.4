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
 * btbmInq.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions provide the service to MMI or JAVA to 
 *   make the operation of command and event
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2008/5/20
 *
  *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "bt_feature.h"
#include "bluetooth_gap_struct.h"
#include "ddb.h"
#include "bttypes.h"
#include "btalloc.h"
#include "btconfig.h"
#include "sec.h"
#include "sdp.h"
#include "btproperty.h"
#include "btbm_adp.h"
#include "btbm.h"
#include "sdap.h"
#include "btaccctrl.h"
//#include "wndrv_api.h"
#include "osapi.h"
#include "btbm_int.h"
#include "btutils.h"
#ifdef __BT_4_0_BLE__
#include "btconfig_le.h"
#endif

#if defined (__BT_2_1_BLOCK_DEVICE__)
#include "btbmblocklist.h"
#endif 

#ifdef MTK_BB_TESTING
#include "BtBBtesting.h"
#endif 

#if !defined(__MTK_TARGET__)
#define BTBM_TIMER_TICKS (50)
#else 
#define BTBM_TIMER_TICKS (4.615)
#endif 

/***************
Internal Globals 
****************/
/* MTK Bluetooth Manager Context */
extern BMCONTENT_T BMController;

#define MTKBMC(f)  (BMController.f)

extern BtHandler globalbtbmHandler;
extern BtHandler linkbmHandler;

#ifdef __BQE_TESTER_
extern U8 gaptester_bond_without_sdp;
#endif 

extern void CONVERT_BDADDRSRC2ARRAY(U8 *dest, btbm_bd_addr_t *source);
extern BtStatus btbmBleNameReq(BtDeviceContext *bdc, BtCallBack callback);

void btbmSendBondingDeviceCfm(U8 result, btbm_bd_addr_t bd_addr);
void btbmSendBondingResultInd(
                U8 result, 
                U8 type, 
                btbm_bd_addr_t bd_addr, 
                U32 cod, 
                U8 key_type, 
                U8 *key);
static void btbmSendBondingCancelCfm(U8 result, btbm_bd_addr_t bd_addr);
extern BtStatus SDAP_DeleteDevDBRecord(BD_ADDR *bd_addr);
extern BtStatus HfgDelSDPRecord(BD_ADDR *bd_addr);

/*****************************************************************************
 * FUNCTION
 *  BTBMGapWritePINCode
 * DESCRIPTION
 *  This function is used to response the PIN Code request when the user press the pin code
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_pin_code_rsp_struct
 *  bd_addr: Remote bluetooth address
 *  pin_len: The length of pin code
 *  pin_code: The PIN CODE
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapWritePINCode(bt_bm_pin_code_rsp_struct *btbm_adp_input_msg)
{
    BD_ADDR remote_addr;
    btbm_bd_addr_t *remoteaddr;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(remote_addr.addr, remoteaddr);
    OS_Report("BTBMGapWritePINCode");
    Assert(MTKBMC(pin_req_timer).func == 0);
    OS_Report("MTKBMC(bm_opactivity)=0x%x", MTKBMC(bm_opactivity));
    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
            btbmUtilCheckBondingState(BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            break;
        case BM_OP_PAIRING:
            btbmUtilCheckPairingState(&remote_addr, BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            break;
        default:
            btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_PAIRING, btbm_adp_input_msg->bd_addr, 0, 0, NULL);
            return;
    }
    if (btbm_adp_input_msg->pin_len != 0)
    {
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        if (MTKBMC(pin_bdc) != 0)
        {
            if (btbm_adp_input_msg->pin_len == 16)
            {
                MTKBMC(pin_bdc)->gapKeyType = BT_LINK_KEY_GAP_TYPE_COMBINATION_16;
            }
            else
            {
                MTKBMC(pin_bdc)->gapKeyType = BT_LINK_KEY_GAP_TYPE_COMBINATION_NONE16;
            }
        }
    #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        SEC_SetPinByAddr(remote_addr, btbm_adp_input_msg->pin_code, btbm_adp_input_msg->pin_len, BPT_SAVE_TRUSTED);

    }
    else
    {
        SEC_SetPinByAddr(remote_addr, 0x00, 0x00, BPT_SAVE_TRUSTED);
        if (MTKBMC(bm_opactivity) == BM_OP_PAIRING)
        {
            BtRemoteDevice *remDev = NULL;

            remDev = ME_FindRemoteDeviceP(remote_addr.addr);
            if ((remDev != NULL) && (remDev->state == BDS_CONNECTED))
            {        
                MeDisconnectLink(remDev,30);
            }   
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapBondingDevice
 * DESCRIPTION
 *  This function is used to make the bond with remote device
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_bonding_req_struct
 *  bd_addr: Remote bluetooth address
 *  sdap_len: The sdap uuid number, if there is no sdap operation need to do,set sdap_len =0
 *  sdap_uuid: The UUID list of the SDAP operation
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapBondingDevice(bt_bm_bonding_req_struct *btbm_adp_input_msg)
{
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t *remoteaddr;
    btbm_bd_addr_t tempAddr;
    BtStatus status;
    BD_ADDR addr;
    U8 i = 0;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    tempAddr = btbm_adp_input_msg->bd_addr;

    CONVERT_BDADDRSRC2ARRAY(addr.addr, remoteaddr);

    if (MTKBMC(bm_opactivity) != BM_OP_NONE)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PREVIOUS_ONGOING, MTKBMC(bm_opactivity));
        btbmSendBondingDeviceCfm(BTBM_ADP_FAILED, btbm_adp_input_msg->bd_addr);
        return;
    }
#ifdef __BQE_TESTER_
    if (gaptester_bond_without_sdp == 1)
    {
        MTKBMC(sdap_len) = 0;
    }
    else
    {
        MTKBMC(sdap_len) = btbm_adp_input_msg->sdap_len;
    }
#else /* __BQE_TESTER_ */ 
    MTKBMC(sdap_len) = btbm_adp_input_msg->sdap_len;
#endif /* __BQE_TESTER_ */ 
    for (i = 0; i < MTKBMC(sdap_len); i++)
    {
        MTKBMC(sdap_uuid)[i] = btbm_adp_input_msg->sdap_uuid[i];
    }
    CONVERT_BDADDRSRC2ARRAY(MTKBMC(service_search_addr).addr, remoteaddr);
    CONVERT_BDADDRSRC2ARRAY(MTKBMC(remote_addr).addr, remoteaddr);
    if ((bdc = DS_FindDevice(&(MTKBMC(remote_addr)))) == 0)
    {
        status = BtbmAllocateFreeDevices((MTKBMC(remote_addr)), &bdc);
        if (status != BT_STATUS_SUCCESS)
        {
            btbmSendBondingDeviceCfm(BTBM_ADP_FAILED, tempAddr);
            MTKBMC(bm_opactivity) = BM_OP_NONE;
            return;
        }
    }
	/* 
		ALPS00105714 : In MAUI, we set discFlag to FALSE to prevent ACL disconnected.
    Guess the reason is that the following is to connect profile, so it can prevent ACL
    disconnect then connect. But for ALPS CTS, we let MMI to do service search independently,
    the ACL will not be disconnected after SDP operation. If bonding is following SDP
    operation, the bonding will be failed due to the ACL is already exist. So remove
    the check.
    */
    /* if (bdc->link == 0 || bdc->link->state <= BDS_CONNECTED) */
    {


        MTKBMC(pin_bdc) = bdc;  //MAUI_01327197
      
        bt_trace(TRACE_GROUP_1, BTBM_CREATE_CONNECTION, 0x01);
#ifdef __BT_4_0_BLE__
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            SmPairAttribute attr;

			attr.mitm = SM_DEFAULT_MITM_PROTECT;
			attr.oob = SM_DEFAULT_OOB_FLAG;
			attr.bond = SM_DEFAULT_BOND_FLAG;
			if(MTKBMC(sdap_len) !=0)
			{
				attr.service_no = MTKBMC(sdap_len);
				attr.service_list = (U8*)MTKBMC(sdap_uuid);
			}
			else
			{
				attr.service_list = NULL;
				attr.service_no = 0;
			}
  #ifdef __GAP_TESTER_
            attr.bond = (MTKBMC(gapNonBondingState) == 0x0);
  #endif

			if((BMController.bm_opactivity != BM_OP_BONDING) &&
				(BMController.bm_opactivity != BM_OP_PAIRING))
			{
				/* If the bonding is ongoing, we shall not change the state */
				MTKBMC(bonding_state) = BM_BONDING_NONE;	//MAUI_01327197
			}
				
			/* SM_Bond function will check the bond is ongoing or not, if yes, then will return pending */
                    
            if ((status = SM_Bond(bdc, &attr)) != BT_STATUS_PENDING)
            {
                /* Failed! */
                btbmSendBondingDeviceCfm((U8) bt_util_convert_err2(status), tempAddr);
            }

        }
        else
#endif
        {
            bdc->under_used |= BDS_BONDING;
            MTKBMC(bm_opactivity) = BM_OP_BONDING;	//MAUI_01327197
            MTKBMC(bonding_state) = BM_BONDING_NONE;	//MAUI_01327197
            status = ME_CreateLink(&linkbmHandler, &(MTKBMC(remote_addr)), &(bdc->psi), &(bdc->link));
        }

        if (status == BT_STATUS_SUCCESS)
        {
                SEC_DeleteDeviceRecord(&(MTKBMC(remote_addr)));
                btbmRemoveLinkCallback(bdc->link);
                BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
                /* ACL link exists, do authenticate link directly */
                btbmUtilCheckBondingState(BM_BONDING_ACL_CREATED, BEC_NO_ERROR);
        }
        else if(status == BT_STATUS_PENDING)
        {
            SEC_DeleteDeviceRecord(&(MTKBMC(remote_addr)));
            /* WCNCR00006958 : bond when remote device is disconnecting */
            //btbmRemoveLinkCallback(bdc->link);        
        }
        else
        {
            if(status == BT_STATUS_NO_RESOURCES){
                btbmSendBondingDeviceCfm(BTBM_ADP_FAIL_NO_RESOURCE, tempAddr);
            }else{
                btbmSendBondingDeviceCfm(BTBM_ADP_FAILED, tempAddr);
            }
            btbmUtilPairingProcedureClear();
            return;
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGapBondingCancel
 * DESCRIPTION
 *  This function is used to cancel the bonding procedure with remote device
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_bonding_cancel_req_struct
 *  bd_addr: Remote bluetooth address
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapBondingCancel(bt_bm_bonding_cancel_req_struct *btbm_adp_input_msg)
{
    BtDeviceContext *bdc = 0;
    BD_ADDR remoteaddr;

    CONVERT_BDADDRSRC2ARRAY(remoteaddr.addr, &btbm_adp_input_msg->bd_addr);
    if (OS_MemCmp(MTKBMC(remote_addr).addr, 6, remoteaddr.addr, 6) == TRUE)
    {
        bdc = DS_FindDevice(&remoteaddr);
        btbmUtilPairingProcedureClear();
        btbmHandlePinReqCancelOperation();
        btbmSendBondingCancelCfm(BTBM_ADP_SUCCESS, btbm_adp_input_msg->bd_addr);
        if (bdc->link != 0)
        {
            #ifdef __BT_4_0_BLE__
            if (bdc->devType == BT_DEV_TYPE_LE)
            {
                SM_CancelBonding(bdc);
            }
            else
            #endif
            {
                ME_CancelBonding(bdc->link);
            }      
            btbmAddLinkCallback(bdc->link);
            bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x08);
            if (ME_DisconnectLink(&linkbmHandler, bdc->link) == BT_STATUS_IN_PROGRESS)
            {
                MTKBMC(bonding_state) = BM_BONDING_CANCELING;
            }
            btbmRemoveLinkCallback(bdc->link);
        }
    }
    else
    {
        btbmSendBondingCancelCfm(BTBM_ADP_FAILED, btbm_adp_input_msg->bd_addr);
    }
}

#ifdef __GAP_TESTER_
#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  BTBMGapEnabledNonBondingTSDevice
 * DESCRIPTION
 *  This function is used to enabled auto reject bonding procedure in GAP v2.1 test cases.
 * PARAMETERS
 *  enabled     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapEnabledNonBondingTSDevice(U8 enabled)
{
    MTKBMC(gapNonBondingState) = enabled;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapSetSSPBondingTSDevice
 * DESCRIPTION
 *  This function is used to enabled dedicated bonding or general bonding in GAP v2.1 test cases.
 * PARAMETERS
 *  type        [IN]        
 *  enabled: 0x00: disabled 0x01: enabled
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapSetSSPBondingTSDevice(U8 type)
{
    MTKBMC(gapCurrentBondingState) = type;
}
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  BTBMGapBondingforGAPTSDevice
 * DESCRIPTION
 *  This function is used to make the bond with remote device in GAP test cases.
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_bonding_req_struct
 *  bd_addr: Remote bluetooth address
 *  sdap_len: The sdap uuid number, if there is no sdap operation need to do,set sdap_len =0
 *  sdap_uuid: The UUID list of the SDAP operation
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapBondingforGAPTSDevice(bt_bm_bonding_req_struct *btbm_adp_input_msg)
{
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t *remoteaddr;
    BtStatus status;
    U8 i = 0;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    if (MTKBMC(bm_opactivity) != BM_OP_NONE)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PREVIOUS_ONGOING, MTKBMC(bm_opactivity));
        btbmSendBondingDeviceCfm(BTBM_ADP_FAILED, btbm_adp_input_msg->bd_addr);
        return;
    }
    MTKBMC(sdap_len) = btbm_adp_input_msg->sdap_len;
    for (i = 0; i < MTKBMC(sdap_len); i++)
    {
        MTKBMC(sdap_uuid)[i] = btbm_adp_input_msg->sdap_uuid[i];
    }
    CONVERT_BDADDRSRC2ARRAY(MTKBMC(remote_addr).addr, remoteaddr);
    MTKBMC(bm_opactivity) = BM_OP_BONDING;
    if ((bdc = DS_FindDevice(&(MTKBMC(remote_addr)))) == 0)
    {
        status = BtbmAllocateFreeDevices((MTKBMC(remote_addr)), &bdc);
        if (status != BT_STATUS_SUCCESS)
        {
            btbmSendBondingDeviceCfm(BTBM_ADP_FAILED, btbm_adp_input_msg->bd_addr);
            return;
        }
        bdc->under_used |= BDS_BONDING;
        Report(("bdc->under_used=0x%x", bdc->under_used));
    }
    MTKBMC(bonding_state) = BM_BONDING_NONE;
    MTKBMC(pin_bdc) = bdc;
    if (bdc->link == 0)
    {
        bt_trace(TRACE_GROUP_1, BTBM_CREATE_CONNECTION, 0x03);
        ME_CreateLink(&linkbmHandler, &(MTKBMC(remote_addr)), &(bdc->psi), &(bdc->link));
        btbmRemoveLinkCallback(bdc->link);
    }
    else
    {
        BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
        /* ACL link exists, do authenticate link directly */
        MTKBMC(pin_bdc)->link->pairingType = BPT_SAVE_NOT_TRUSTED;
        btbmUtilCheckBondingState(BM_BONDING_ACL_CREATED, BEC_NO_ERROR);
    }
    return;
}

#endif /* __GAP_TESTER_ */ 

#ifdef __BT_HIDH_PROFILE__
void hid_virtual_cable_unplug_callback(BD_ADDR *hidha_bd_addr){
    BMCallbackParms parms;
    BtRemoteDevice *remDev = ME_FindRemoteDevice(hidha_bd_addr);;
    OS_Report("hid_virtual_cable_unplug_callback. remDev=%p", remDev);
    /* Disconnect link */
    if(remDev){
        btbmDisconnectAclLinkWithoutForce(remDev, BEC_POWER_OFF, 2);
    }
    /* callback with delete trust cnf */
    parms.event = BTBM_EVENT_DELETE_ONE_TRUST_DEVICE_RESULT;
    parms.p.DeleteOneTrustDeviceResult.result = BTBM_ADP_SUCCESS;
    CONVERT_ARRAY2BDADDR(&parms.p.DeleteOneTrustDeviceResult.bd_addr, hidha_bd_addr->addr);
    MTKBMC(bmCB)(&parms);
}
extern BtStatus btmtk_adp_hidh_virtual_cable_unplug_req(BD_ADDR *bt_addr);
#endif
/*****************************************************************************
 * FUNCTION
 *  BTBMGapDeleteTrustDevice
 * DESCRIPTION
 *  This function is used to delete the link key of the device
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_delete_trust_req_struct
 *  bd_addr: Remote bluetooth address
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapDeleteTrustDevice(bt_bm_delete_trust_req_struct *btbm_adp_input_msg)
{
    btbm_bd_addr_t *remoteaddr;
    BtDeviceContext *bdc = 0;
    BMCallbackParms parms;
#ifdef __BT_HIDH_PROFILE__
    BtStatus hid_unplug = BT_STATUS_FAILED;
#endif

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    parms.event = BTBM_EVENT_DELETE_ONE_TRUST_DEVICE_RESULT;

    if (MTKBMC(bm_opactivity) != BM_OP_NONE)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PREVIOUS_ONGOING, MTKBMC(bm_opactivity));
        /*
        parms.p.DeleteOneTrustDeviceResult.result = BTBM_ADP_FAILED;
        parms.p.DeleteOneTrustDeviceResult.bd_addr = btbm_adp_input_msg->bd_addr;
        MTKBMC(bmCB)(&parms);
        return;
        */
    }
    CONVERT_BDADDRSRC2ARRAY(MTKBMC(remote_addr).addr, remoteaddr);
    
    /* Clear "In Range" status for all devices. */
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
    {
        BTBMDumpBdAddr(bdc->addr.addr);
        if (bdc->link != 0)
        {
            bt_trace(TRACE_GROUP_1, BDCLINKSTATEBT_DEBUGx02X, bdc->link->state);
            if ((bdc->link->state == BDS_CONNECTED) &&
                (StrnCmp(bdc->link->bdAddr.addr, 6, MTKBMC(remote_addr).addr, 6)))
            {
                bdc->link->discFlag = TRUE;
#ifdef __BT_HIDH_PROFILE__
                hid_unplug = btmtk_adp_hidh_virtual_cable_unplug_req(&MTKBMC(remote_addr));
                OS_Report("hid_unplug=%d", hid_unplug);
                if(hid_unplug != BT_STATUS_SUCCESS && hid_unplug != BT_STATUS_PENDING)
#endif                    
                {
                    btbmDisconnectAclLinkWithoutForce(bdc->link, BEC_POWER_OFF, 2);
                }
                break;
            }
        }
    }
#ifdef __BT_HFG_PROFILE__
    HfgDelSDPRecord(&MTKBMC(remote_addr));
#endif
    SDAP_DeleteDevDBRecord(&MTKBMC(remote_addr));

    if(OS_MemCmp(MTKBMC(remote_addr).addr, 6, MTKBMC(service_search_addr).addr, 6)){
        /* Cancel timer if unpair happened before SDP query is executed */
        EVM_CancelTimer(&MTKBMC(service_search_delay_timer));
    }
    SEC_DeleteDeviceRecord(&(MTKBMC(remote_addr)));
#ifdef __BT_HIDH_PROFILE__
    if(hid_unplug != BT_STATUS_SUCCESS && hid_unplug != BT_STATUS_PENDING){
        parms.p.DeleteOneTrustDeviceResult.result = BTBM_ADP_SUCCESS;
        parms.p.DeleteOneTrustDeviceResult.bd_addr = btbm_adp_input_msg->bd_addr;
        MTKBMC(bmCB)(&parms);
    }else{
        OS_Report("Delay delete trust cnf until HID unplug");
    }
#else
    parms.p.DeleteOneTrustDeviceResult.result = BTBM_ADP_SUCCESS;
    parms.p.DeleteOneTrustDeviceResult.bd_addr = btbm_adp_input_msg->bd_addr;
    MTKBMC(bmCB)(&parms);
#endif
    return;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapDeleteTrustAllDevice
 * DESCRIPTION
 *  This function is used to delete all the link key
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapDeleteTrustAllDevice(void)
{
    BMCallbackParms parms;

    parms.event = BTBM_EVENT_DELETE_ALL_TRUST_DEVICE_RESULT;

    if (MTKBMC(bm_opactivity) != BM_OP_NONE)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PREVIOUS_ONGOING, MTKBMC(bm_opactivity));
        parms.p.result = BTBM_ADP_FAILED;
        MTKBMC(bmCB)(&parms);
        return;
    }
    DDB_FlushAllRecord();
    parms.p.result = BTBM_ADP_SUCCESS;
    MTKBMC(bmCB)(&parms);
    return;
}


#if defined (__BT_2_1_SIMPLE_PAIRING__)
/*****************************************************************************
 * FUNCTION
 *  BTBMGapPasskeyEntryRsp
 * DESCRIPTION
 *  This function is used to respond with user passkey
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *    bt_bm_security_passkey_entry_rsp_struct
 *    - bd_addr: Remote bluetooth address
 *    - accept: The user accept the SSP or not
 *    - passkey
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapPasskeyEntryRsp(bt_bm_security_passkey_entry_rsp_struct *btbm_adp_input_msg)
{
    BD_ADDR remote_addr;

    CONVERT_BDADDRSRC2ARRAY(remote_addr.addr, &btbm_adp_input_msg->bd_addr);
    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
        case BM_OP_PAIRING:
            SEC_PasskeyEntry(remote_addr, btbm_adp_input_msg->accept, btbm_adp_input_msg->passkey);
            break;
        default:
            btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_PAIRING, btbm_adp_input_msg->bd_addr, 0, 0, NULL);
            return;
    }
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGapPasskeyNotifyRsp
 * DESCRIPTION
 *  This function is used to respond with user passkey
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *    bt_bm_security_passkey_notify_rsp_struct
 *    - bd_addr: Remote bluetooth address
 *    - accept: The user accept the SSP or not
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapPasskeyNotifyRsp(bt_bm_security_passkey_notify_rsp_struct *btbm_adp_input_msg)
{
    BD_ADDR remote_addr;

    CONVERT_BDADDRSRC2ARRAY(remote_addr.addr, &btbm_adp_input_msg->bd_addr);
    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
        case BM_OP_PAIRING:
#ifdef __BT_4_0_BLE__
            /* do nothing */
#else
            SEC_PasskeyEntry(remote_addr, btbm_adp_input_msg->accept, 0);
#endif
            break;
        default:
            btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_PAIRING, btbm_adp_input_msg->bd_addr, 0, 0, NULL);
            return;
    }
}

/*****************************************************************************
 * FUNCTION
 *  BTBMSSPKeyPressRsp
 * DESCRIPTION
 *  The MMI will response that it has received the Key Press Indication.
 *  We can send the next key press indication to MMI.
 *  This can aviod too many messages allocated and send to MMI at the same time.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMSSPKeyPressRsp(void)
{
    BMCallbackParms parms;
    
    if ((MTKBMC(SSPDeviceInfo).sspType == BT_BM_SSP_PIN_PASSKEY_ENTRY) && 
        ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING)))
    {
        if (MTKBMC(SSPDeviceInfo).keyPressEventOccurs == TRUE)
        {
            MTKBMC(SSPDeviceInfo).keyPressEventOccurs = FALSE;
            parms.event = BTBM_EVENT_SSP_KEY_PRESS_INDICATION;
            parms.p.SSPKeyPressIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.SSPKeyPressIndication.event = MTKBMC(SSPDeviceInfo).keyPressNo;    
            MTKBMC(bmCB)(&parms); 
        }
        else
        {
            MTKBMC(SSPDeviceInfo).keyPressEventReporting = FALSE;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMSSPKeyPressAbort
 * DESCRIPTION
 *  The MMI will request to disconnect the ACL
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_security_passkey_notify_cancel_struct the bluetooth address
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMSSPKeyPressAbort(bt_bm_security_passkey_notify_cancel_struct *btbm_adp_input_msg)
{
    BD_ADDR addr;
    btbm_bd_addr_t *remoteaddr;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(addr.addr, remoteaddr);
    bt_trace(TRACE_GROUP_1,
        BTLOG_BTBM_SPP_KEY_PRESS_ABORT, 
        MTKBMC(SSPDeviceInfo).sspType,
        MTKBMC(bm_opactivity));

    if ((MTKBMC(SSPDeviceInfo).sspType == BT_BM_SSP_PIN_PASSKEY_ENTRY) && 
        ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING)))
    {
        btbmHandleForceDisconnectLink(&addr);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapWriteUserConfirm
 * DESCRIPTION
 *  This function is used to response the user Confirm Response
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_security_user_confirm_rsp_struct
 *  bd_addr: Remote bluetooth address
 *  accept: The user accept the SSP or not
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapWriteUserConfirm(bt_bm_security_user_confirm_rsp_struct *btbm_adp_input_msg)
{
    BD_ADDR remote_addr;
    btbm_bd_addr_t *remoteaddr;
#ifdef __BT_4_0_BLE__
    BtDeviceContext *bdc;
#endif

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(remote_addr.addr, remoteaddr);
    Assert(MTKBMC(pin_req_timer).func == 0);

#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
    goto checked;
  #else
    bdc = DS_FindDevice(&remote_addr);
    if(bdc){
        OS_Report("BTBMGapWriteUserConfirm() bdc->devType=%d", bdc->devType);
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            goto checked;
        }
    }else{
        OS_Report("BTBMGapWriteUserConfirm() bdc is null");
    }
  #endif
#endif
    bt_prompt_trace(MOD_BT, "BTBMGapWriteUserConfirm : bm_opactivity=%u",MTKBMC(bm_opactivity));
    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
            btbmUtilCheckBondingState(BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            break;
        case BM_OP_PAIRING:
            btbmUtilCheckPairingState(&remote_addr, BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            break;
        default:
            btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_PAIRING, btbm_adp_input_msg->bd_addr, 0, 0, NULL);
            return;
    }

checked:
    OS_Report("BTBMGapWriteUserConfirm() accept=%d", btbm_adp_input_msg->accept);
    if (btbm_adp_input_msg->accept != 0)
    {
        SEC_SetUserConfirmByAddr(remote_addr, TRUE);
    }
    else
    {
        SEC_SetUserConfirmByAddr(remote_addr, FALSE);
    }
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGapOOBDataRsp
 * DESCRIPTION
 *  This function is used to response the user Confirm Response
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_security_user_confirm_rsp_struct
 *  bd_addr: Remote bluetooth address
 *  accept: The user accept the SSP or not
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapOOBDataRsp(bt_bm_security_oob_data_rsp_struct *btbm_adp_input_msg)
{
    BD_ADDR remote_addr;
    btbm_bd_addr_t *remoteaddr;
#ifdef __BT_4_0_BLE__
    BtDeviceContext *bdc;
#endif

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(remote_addr.addr, remoteaddr);
    Assert(MTKBMC(pin_req_timer).func == 0);

#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
    goto checked;
  #else
    bdc = DS_FindDevice(&remote_addr);
    if (bdc->devType == BT_DEV_TYPE_LE)
    {
        goto checked;
    }
  #endif
#endif

    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
            btbmUtilCheckBondingState(BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            break;
        case BM_OP_PAIRING:
            btbmUtilCheckPairingState(&remote_addr, BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            break;
        default:
            btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_PAIRING, btbm_adp_input_msg->bd_addr, 0, 0, NULL);
            return;
    }

checked:
    if (btbm_adp_input_msg->accept != 0)
    {
        SEC_SetOobData(remote_addr, TRUE, btbm_adp_input_msg->c, btbm_adp_input_msg->r);
    }
    else
    {
        SEC_SetOobData(remote_addr, FALSE, NULL, NULL);
    }
}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

/*****************************************************************************
 * FUNCTION
 *  btbmHandlerPinCodeRequest
 * DESCRIPTION
 *  To handle the Pin code request event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerPinCodeRequest(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtStatus status;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PIN_CODE_REQ, Event->errCode);
    if (Event->errCode != BEC_NO_ERROR)
    {
        return;
    }
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    if ((bdc == MTKBMC(pin_bdc)) && (MTKBMC(bm_opactivity) == BM_OP_BONDING))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_BONDING_PIN_REQ);
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        MTKBMC(SSPDeviceInfo).sspType = BT_BM_SSP_PIN_GENERAL_CODE;
    #endif 
        btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
    }
    else
    {
        /*
         * MAUI_01924058: The remove device may do the authentication 
         * before host receive connection complete event.
         * In this case, the connection could not be disconnected by MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_REQ.
         * Therefore, we should check if the remote device is blocked, 
         * before report pin code indication to MMI.
         */
    #if defined (__BT_2_1_BLOCK_DEVICE__)        
        if (BTBMCheckBlockList(Event->p.remDev->bdAddr.addr) == TRUE)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_IN_BLOCK_LIST_REJECT);
            SEC_SetPinByAddr(Event->p.remDev->bdAddr, 0x00, 0x00, BPT_SAVE_NOT_TRUSTED);
            return;
        }
    #endif /* #if defined (__BT_2_1_BLOCK_DEVICE__) */
        
        if (bdc == 0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_BDC_IS_ZERO);
            status = BtbmAllocateFreeDevices(Event->p.remDev->bdAddr, &bdc);
            if (status != BT_STATUS_SUCCESS)
            {
                SEC_SetPinByAddr(Event->p.remDev->bdAddr, 0x00, 0x00, BPT_SAVE_NOT_TRUSTED);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PIN_REJECT);
                return;
            }
        }
        if (MTKBMC(bm_opactivity) != BM_OP_NONE)
        {
            SEC_SetPinByAddr(bdc->addr, 0x00, 0x00, BPT_SAVE_NOT_TRUSTED);
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_OP_NONE_REJECT_BMOPACTIVITY, MTKBMC(bm_opactivity));
        }
        else
        {
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_REMOTE_PIN_REQ);
            MTKBMC(bm_opactivity) = BM_OP_PAIRING;
            MTKBMC(bonding_state) = BM_BONDING_NONE;
            MTKBMC(pin_bdc) = bdc;
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
            MTKBMC(SSPDeviceInfo).sspType = BT_BM_SSP_PIN_GENERAL_CODE;
        #endif 
            btbmUtilCheckPairingState(&(bdc->addr), BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerPairingComplete
 * DESCRIPTION
 *  To handle the Pin Code completed
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerPairingComplete(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev = 0;

        
    remDev = Event->p.remDev;
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    bt_trace(TRACE_GROUP_1, BT_BTBM_EVENT_PAIRING_COMPLETED, 
        MTKBMC(pin_bdc), bdc, Event->errCode, MTKBMC(bm_opactivity));

    if ((MTKBMC(pin_bdc) == bdc) && (Event->errCode == BEC_NO_ERROR))
    {
        if (MTKBMC(bm_opactivity) == BM_OP_BONDING)
        {
            btbmUtilCheckBondingState(BM_BONDING_SUCCESS_COMPLETED, BEC_NO_ERROR);
        }
        else if (MTKBMC(bm_opactivity) == BM_OP_PAIRING)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_REMDEVSTATEx02X, remDev->state);
            if (remDev->state == BDS_CONNECTED)
            {
                btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_ACL_CREATED, BEC_NO_ERROR);
            }
            btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_SUCCESS_COMPLETED, BEC_NO_ERROR);
        }
    }
}

#if defined (__BT_2_1_SIMPLE_PAIRING__)

#define IS_DEDICATE_BONDING(a) ((a) == BT_SSP_MITM_NOT_REQUIRED_DEDICATED_BONDING || (a) == BT_SSP_MITM_REQUIRED_DEDICATED_BONDING)
#define IS_GENERAL_BONDING(a) ((a) == BT_SSP_MITM_NOT_REQUIRED_GENERAL_BONDING || (a) == BT_SSP_MITM_REQUIRED_GENERAL_BONDING)
#define IS_NO_BONDING(a) ((a) <= BT_SSP_MITM_REQUIRED_NO_BONDING)
static U8 btbmUpdatePairingType(BtRemoteDevice *remDev){
    OS_Report("[SEC] btbmUpdatePairingType : ioCapabilitiyState=%u", remDev->ioCapabilitiyState);
    if( (remDev->ioCapabilitiyState & (BIO_RSPONSE_REMOTE | BIO_REQUESTD_LOCAL)) == 
         (BIO_RSPONSE_REMOTE | BIO_REQUESTD_LOCAL)){
        if((IS_NO_BONDING(MTKBMC(SSPDeviceInfo).localAuthRequirements) 
            && !IS_DEDICATE_BONDING(MTKBMC(SSPDeviceInfo).authRequirements)) ||
            (IS_NO_BONDING(MTKBMC(SSPDeviceInfo).authRequirements) 
            && !IS_DEDICATE_BONDING(MTKBMC(SSPDeviceInfo).localAuthRequirements))){
            OS_Report("[SEC] btbmUpdatePairingType : set pairing to BPT_NOT_SAVED");
            remDev->pairingType = BPT_NOT_SAVED;
        }
    }
    return remDev->pairingType;
}

/*****************************************************************************
 * FUNCTION
 *  btbmHandleIoCapabilityRequest
 * DESCRIPTION
 *  Handle the IoCapability Request from remote peer.
 * PARAMETERS
 *  remDev      [IN]        Remote device
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleIoCapabilityRequest(BtRemoteDevice *remDev)
{
    U8 bonding_mode = BT_SSP_MITM_REQUIRED_GENERAL_BONDING;
    BOOL accept = TRUE;
    BtDeviceContext *bdc = 0;
    BtStatus status;

    bt_trace(TRACE_GROUP_1, BTLOG_CURRENT_BM_OPACTIVITY, MTKBMC(bm_opactivity));
    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
            Assert(MTKBMC(pin_bdc) != 0);
            if (OS_MemCmp(MTKBMC(pin_bdc)->addr.addr, 6, remDev->bdAddr.addr, 6) == FALSE)
            {
                accept = FALSE;
                goto response_ssp;
            }
        #ifndef __GAP_TESTER_
                    bonding_mode = BT_SSP_MITM_REQUIRED_DEDICATED_BONDING;
        #else
            bonding_mode = MTKBMC(gapCurrentBondingState);
        #endif
            break;
        case BM_OP_NONE:
            bdc = DS_FindDevice(&(remDev->bdAddr));
            if (bdc == 0)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_BDC_IS_ZERO);
                status = BtbmAllocateFreeDevices(remDev->bdAddr, &bdc);
                if (status != BT_STATUS_SUCCESS)
                {
                    bt_trace(TRACE_GROUP_1, BTLOG_BDC_NOT_FIND_RESPONSE_FAILED);
                    accept = FALSE;
                    goto response_ssp;

                }
            }
            MTKBMC(bm_opactivity) = BM_OP_PAIRING;
            MTKBMC(bonding_state) = BM_BONDING_NONE;
            MTKBMC(pin_bdc) = bdc;
            /* fall-through */

        case BM_OP_PAIRING:
            Assert(MTKBMC(pin_bdc) != 0);
            if (OS_MemCmp(MTKBMC(pin_bdc)->addr.addr, 6, remDev->bdAddr.addr, 6) == FALSE)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_ADDRESS_NOT_MATCHED_RETURN);
                accept = FALSE;
                goto response_ssp;
            }
        #ifdef __GAP_TESTER_
            if (MTKBMC(gapNonBondingState) == 0x01)
            {
                Report(("MTKBMC(SSPDeviceInfo).authRequirements:%d", MTKBMC(SSPDeviceInfo).authRequirements));
                if (MTKBMC(SSPDeviceInfo).authRequirements != BT_SSP_MITM_NOT_REQUIRED_NO_BONDING &&
                    MTKBMC(SSPDeviceInfo).authRequirements != BT_SSP_MITM_REQUIRED_NO_BONDING)
                {
                    accept = FALSE;
                    goto response_ssp;
                }
            }
            bonding_mode = MTKBMC(gapCurrentBondingState);
        #else 
            /* remDev->gapSecurityLevel = BT_SEC_HIGH; */
            /* If sec low, use MITM not required no bonding */
            OS_Report("[SEC] btbmHandleIoCapabilityRequest : gapSecurityLevel is %u, ioCapabilitiyState=%u", remDev->gapSecurityLevel, remDev->ioCapabilitiyState);
            if(remDev->gapSecurityLevel == BT_SEC_UNKNOWN && (remDev->ioCapabilitiyState & BIO_RSPONSE_REMOTE)){
                bonding_mode = MTKBMC(SSPDeviceInfo).authRequirements;
                if((bonding_mode>>0x01) == 0x01 /* Dedicate bonding */){
                    OS_Report("Remote request dedicated bonding. Set MITM required");
                    bonding_mode |= BT_SSP_MITM_REQUIRED;
                }
            }else if(remDev->gapSecurityLevel == BT_SEC_LOW){
                bonding_mode = BT_SSP_MITM_NOT_REQUIRED_NO_BONDING;
            }else{
                bonding_mode = BT_SSP_MITM_REQUIRED_GENERAL_BONDING;
            }
        #endif
            break;

        default:
            accept = FALSE;
            goto response_ssp;
    }
    OS_MemCopy(MTKBMC(SSPDeviceInfo).addr, remDev->bdAddr.addr, 6);
    MTKBMC(SSPDeviceInfo).keyPressNo = 0;
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);

  response_ssp:
    if (accept == TRUE)
    {
        MTKBMC(SSPDeviceInfo).localAuthRequirements = bonding_mode;
        OS_Report("[SEC] btbmHandleIoCapabilityRequest : jsr82 used=%u, CTSTest=%u", remDev->jsr82_used, MTKBMC(CTSTest));
        if((remDev !=0) && (remDev->jsr82_used == 1))
        {
            bonding_mode = BT_SSP_MITM_NOT_REQUIRED_NO_BONDING;
            MTKBMC(SSPDeviceInfo).localAuthRequirements = bonding_mode;
        }
        if(MTKBMC(CTSTest) == 1)
        {
            bonding_mode = BT_SSP_MITM_NOT_REQUIRED_NO_BONDING;
            MTKBMC(SSPDeviceInfo).localAuthRequirements = bonding_mode;
        }
        bt_trace(TRACE_GROUP_1, BTLOG_BTBM_SPP_AUTHENTICATION_REQUIREMENTS, bonding_mode);
        btbmUpdatePairingType(remDev);
    }
    SEC_SetIoCapabilityByAddr(remDev->bdAddr, bonding_mode, accept);
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleIoCapabilityResponse
 * DESCRIPTION
 *  Handle the IoCapability response from remote peer.
 * PARAMETERS
 *  Event       [IN]        Reported from ME layer.
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleIoCapabilityResponse(const BtEvent *Event)
{
    BtRemoteDevice *remDev;
    BtDeviceContext *bdc = 0;
    BtStatus status;

    remDev = Event->p.ioCapabilityResponse.remDev;
    switch (MTKBMC(bm_opactivity))
    {
        case BM_OP_BONDING:
        case BM_OP_PAIRING:
            if (OS_MemCmp(MTKBMC(pin_bdc)->addr.addr, 6, remDev->bdAddr.addr, 6) == FALSE)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_ADDRESS_NOT_MATCHED_RETURN);
                return;
            }
            break;
        case BM_OP_NONE:
            bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
            if (bdc == 0)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_BDC_IS_ZERO);
                status = BtbmAllocateFreeDevices(remDev->bdAddr, &bdc);
                if (status != BT_STATUS_SUCCESS)
                {
                    bt_trace(TRACE_GROUP_1, BTLOG_BDC_NOT_FIND_RESPONSE_FAILED);
                    return;
                }
            }
            MTKBMC(bm_opactivity) = BM_OP_PAIRING;
            MTKBMC(bonding_state) = BM_BONDING_NONE;
            MTKBMC(pin_bdc) = bdc;
            break;
        default:
            return;
    }
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
    OS_MemCopy(MTKBMC(SSPDeviceInfo).addr, remDev->bdAddr.addr, 6);
    MTKBMC(SSPDeviceInfo).keyPressNo = 0;
    MTKBMC(SSPDeviceInfo).ioCapability = Event->p.ioCapabilityResponse.ioCapability;
    MTKBMC(SSPDeviceInfo).OOBDataPresent = Event->p.ioCapabilityResponse.OOBDataPresent;
    MTKBMC(SSPDeviceInfo).authRequirements = Event->p.ioCapabilityResponse.authRequirements;
    BTBMDumpBdAddr(MTKBMC(SSPDeviceInfo).addr);
    bt_trace(
        TRACE_GROUP_1,
        BTLOG_BTBMHANDLEIOCAPABILITYRESPONSEx02Xx02Xx02X,
        MTKBMC(SSPDeviceInfo).ioCapability,
        MTKBMC(SSPDeviceInfo).OOBDataPresent,
        MTKBMC(SSPDeviceInfo).authRequirements);
    btbmUpdatePairingType(Event->p.remDev);
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleUserConfirmRequest
 * DESCRIPTION
 *  Handle the User confirm Request.
 * PARAMETERS
 *  Event       [IN]        Reported from ME layer.
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleUserConfirmRequest(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;

    if (OS_MemCmp(MTKBMC(SSPDeviceInfo).addr, 6, Event->p.userConfirmRequest.remDev->bdAddr.addr, 6) == FALSE)
    {
        BTBMDumpBdAddr(MTKBMC(SSPDeviceInfo).addr);
        BTBMDumpBdAddr(Event->p.userConfirmRequest.remDev->bdAddr.addr);
        SEC_SetUserConfirmByAddr(Event->p.userConfirmRequest.remDev->bdAddr, FALSE);
        /* Dlight: If the remote side also do pairing with us. We will reject in both side. 
                  Detail log sample in MAUI_02127139        */
        return;
    }
    bt_trace(TRACE_GROUP_1, BTLOG_BTBMHANDLEUSERCONFIRMREQUESTxD, Event->p.userConfirmRequest.numericValue);
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    if ((bdc == MTKBMC(pin_bdc)) &&
        ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING)))
    {
        if(Event->p.remDev->jsr82_used ==1)
        {
            bt_bm_security_user_confirm_rsp_struct data;
            CONVERT_ARRAY2BDADDR(&data.bd_addr, Event->p.remDev->bdAddr.addr);
            data.accept = TRUE;
            Report(("Auto accept the pair request"));
            btbmUtilCheckBondingState(BM_BONDING_NAME_REQUESTED, BEC_NO_ERROR);            
            btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
            BTBMGapWriteUserConfirm(&data);
        }
        else
        {
            Report(("MTKBMC(CTSTest):%d",MTKBMC(CTSTest)));
            if(MTKBMC(CTSTest) == 0)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_BTBM_BONDING_RX_USER_CONFIRM_REQUEST);
                MTKBMC(SSPDeviceInfo).numericValue = Event->p.userConfirmRequest.numericValue;
                MTKBMC(SSPDeviceInfo).sspType = BT_BM_SSP_PIN_USER_CONFIRM;
                btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
            }
            else
            {
                bt_bm_security_user_confirm_rsp_struct data;
                CONVERT_ARRAY2BDADDR(&data.bd_addr, Event->p.remDev->bdAddr.addr);
                data.accept = TRUE;
                Report(("Auto accept the pair request"));
                btbmUtilCheckBondingState(BM_BONDING_NAME_REQUESTED, BEC_NO_ERROR);
                btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
                BTBMGapWriteUserConfirm(&data);
            }
        }
    }
    else
    {
        SEC_SetUserConfirmByAddr(Event->p.userConfirmRequest.remDev->bdAddr, FALSE);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_OP_NONE_REJECT_BMOPACTIVITY, 0xFF);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleOobDataRequest
 * DESCRIPTION
 *  Handle the User Oob data Request.
 * PARAMETERS
 *  Event       [IN]        Reported from ME layer.
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleOobDataRequest(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;

    bt_trace(TRACE_GROUP_1, BTLOG_BTBMHANDLEOOBDATAREQUEST);
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    if ((bdc == MTKBMC(pin_bdc)) &&
        ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING)))
    {
        MTKBMC(SSPDeviceInfo).sspType = BT_BM_SSP_PIN_OOB_DATA;
        btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
    }
    else
    {
        SEC_SetOobData(Event->p.remDev->bdAddr, FALSE, NULL, NULL);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_OP_NONE_REJECT_BMOPACTIVITY, MTKBMC(bm_opactivity));
    }
}

/*****************************************************************************
 * FUNCTION
 *  btbmHandlePasskeyEntryNotification
 * DESCRIPTION
 *  Handle the Passkey Entry Request.
 * PARAMETERS
 *  Event       [IN]        Reported from ME layer.
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlePasskeyEntryNotification(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;

    if (OS_MemCmp(MTKBMC(SSPDeviceInfo).addr, 6, Event->p.passKeyEntry.remDev->bdAddr.addr, 6) == FALSE)
    {
        BTBMDumpBdAddr(MTKBMC(SSPDeviceInfo).addr);
        BTBMDumpBdAddr(Event->p.passKeyEntry.remDev->bdAddr.addr);
        Assert(("addr!=key" == 0));
    }
    bt_trace(TRACE_GROUP_1, BTLOG_BTBMHANDLEPASSKEYENTRYNOTIFICATIONxD, Event->p.passKeyEntry.numericValue);
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    if ((bdc == MTKBMC(pin_bdc)) &&
        ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING)))
    {
        bt_trace(TRACE_GROUP_1, BTLOG_BTBM_BONDING_RX_PASSKEY_ENTRY_REQUEST);
        MTKBMC(SSPDeviceInfo).numericValue = Event->p.passKeyEntry.numericValue;
        MTKBMC(SSPDeviceInfo).sspType = BT_BM_SSP_PIN_PASSKEY_ENTRY;
        btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_PASSKEY_ENTRY_NOTIFICATION_WRONG_STATE_IGNORE_IT);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlePasskeyPressIndication
 * DESCRIPTION
 *  Handle the Key Press Indication.
 * PARAMETERS
 *  Event       [IN]        Reported from ME layer.
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlePasskeyPressIndication(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BMCallbackParms parms;
    
    if (OS_MemCmp(MTKBMC(SSPDeviceInfo).addr, 6, Event->p.passKeyPress.remDev->bdAddr.addr, 6) == FALSE)
    {
        BTBMDumpBdAddr(MTKBMC(SSPDeviceInfo).addr);
        BTBMDumpBdAddr(Event->p.passKeyPress.remDev->bdAddr.addr);
        Assert(("addr!= key" == 0));
    }
    bt_trace(TRACE_GROUP_1, BTLOG_BTBMHANDLEPASSKEYENTRYNOTIFICATIONxD, Event->p.passKeyPress.passkeyEvent);
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    if ((bdc == MTKBMC(pin_bdc)) && (MTKBMC(SSPDeviceInfo).sspType == BT_BM_SSP_PIN_PASSKEY_ENTRY) &&
        ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING)))
    {
        switch (Event->p.passKeyPress.passkeyEvent)
        {
            case BT_BM_SSP_KEYPRESS_STARTED:
            case BT_BM_SSP_KEYPRESS_CLEARED:
                MTKBMC(SSPDeviceInfo).keyPressNo = 0;
                break;
            case BT_BM_SSP_KEYPRESS_ENTERED:
                if (MTKBMC(SSPDeviceInfo).keyPressNo < 0xFF)
                {
                    MTKBMC(SSPDeviceInfo).keyPressNo++;
                }
                break;
            case BT_BM_SSP_KEYPRESS_ERASED:
                if (MTKBMC(SSPDeviceInfo).keyPressNo > 0)
                {
                    MTKBMC(SSPDeviceInfo).keyPressNo--;
                }
                break;
            case BT_BM_SSP_KEYPRESS_COMPLETED:
                break;
        }
        MTKBMC(SSPDeviceInfo).keyPressEventOccurs = TRUE;
        bt_trace(TRACE_GROUP_1, BTLOG_BTBMHANDLE_CURRENT_KEY_PRESSxD, MTKBMC(SSPDeviceInfo).keyPressNo);
        bt_trace(TRACE_GROUP_1, BTLOG_MTKBMCPIN_REQ_TIMERFUNCx08X, MTKBMC(pin_req_timer).func);
        bt_trace(TRACE_GROUP_1, BTLOG_KEYPRESSEVENTREPORTINGxD, MTKBMC(SSPDeviceInfo).keyPressEventReporting);

        if (((MTKBMC(bonding_state) & BM_BONDING_NAME_REQUESTED) != 0) &&
            (MTKBMC(SSPDeviceInfo).keyPressEventReporting != TRUE))
        {
            MTKBMC(SSPDeviceInfo).keyPressEventReporting = TRUE;
            MTKBMC(SSPDeviceInfo).keyPressEventOccurs = FALSE;
            parms.event = BTBM_EVENT_SSP_KEY_PRESS_INDICATION;
            parms.p.SSPKeyPressIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.SSPKeyPressIndication.event = MTKBMC(SSPDeviceInfo).keyPressNo;    
            MTKBMC(bmCB)(&parms); 
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerSimplePairingComplete
 * DESCRIPTION
 *  To handle the SimplePairing completed
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerSimplePairingComplete(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev = 0;

    remDev = Event->p.remDev;
    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    bt_trace(TRACE_GROUP_1, BTLOG_SIMPLE_PAIRING_PROCEDURE_IS_COMPLETED,
              MTKBMC(pin_bdc), 
              bdc, 
              Event->errCode, 
              MTKBMC(bm_opactivity));

    if ((MTKBMC(pin_bdc) == bdc) && (Event->errCode != BEC_NO_ERROR))
    {
        if (MTKBMC(bm_opactivity) == BM_OP_BONDING)
        {
            btbmUtilCheckBondingState(BM_BONDING_FAILED_COMPLETED, Event->errCode);
        }
        else if (MTKBMC(bm_opactivity) == BM_OP_PAIRING)
        {
            btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_FAILED_COMPLETED, Event->errCode);
        }
    }
    Report(("a, b:%d,%d",remDev->jsr82_used, MTKBMC(CTSTest)));
    if((remDev->jsr82_used == 1) || (MTKBMC(CTSTest) == 1))
    {
        if((Event->errCode != BEC_NO_ERROR) &&
            (MTKBMC(SSPDeviceInfo).localAuthRequirements == 0) &&
            (MTKBMC(SSPDeviceInfo).authRequirements ==0))
        {
            Report(("shall not store record"));
            SEC_DeleteDeviceRecord(&(remDev->bdAddr));
        }
        
    }
}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  btbmHandlePinReqTimeout
 * DESCRIPTION
 *  When the 200 ms timeout from the name request complete event, we will report the Pin code request event
 *  to MMI.
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlePinReqTimeout(EvmTimer *Timer)
{
    BD_ADDR bd_addr_t;
    BMCallbackParms parms;
    
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
#ifdef __BT_SIM_PROFILE__
    BtRemoteDevice *remDev = NULL;
#endif
    U8 enable_16digits_pin_code = 0;
#else /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    BtRemoteDevice *remDev = NULL;
    kal_bool displayNumeric;
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    U32 cod;
    BtCodRecord record;

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBMHANDLEPINREQTIMEOUT);
    MTKBMC(pin_req_timer).func = 0;
    CONVERT_BDADDR2ARRAY(bd_addr_t.addr,
                         MTKBMC(pin_req_device).bd_addr.lap,
                         MTKBMC(pin_req_device).bd_addr.uap,
                         MTKBMC(pin_req_device).bd_addr.nap);

    if (DDB_COD_FindRecord(&bd_addr_t, &record) == BT_STATUS_SUCCESS)
    {
        cod = record.cod;
    }
    else
    {
        cod = 0;
    }
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
#ifdef __BT_SIM_PROFILE__
    remDev = ME_FindRemoteDevice(&bd_addr_t);
    if (remDev != NULL)
    {
        if (remDev->simap_issue_authentication_procedure)
        {
            enable_16digits_pin_code = 1;
            bt_trace(TRACE_GROUP_1, BT_LOG_NOTIFY_MMI_SIMAP_ISSUE_16_DIGITS);
        }

    }
#endif /* __BT_SIM_PROFILE__ */ 
    parms.event = BTBM_EVENT_PINCODE_REQUEST_INDICATION;
    parms.p.PinCodeRequestIndication.pin_code_16_digit_required = enable_16digits_pin_code;
    parms.p.PinCodeRequestIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
    parms.p.PinCodeRequestIndication.name_len = MTKBMC(pin_req_device).name_len;
    parms.p.PinCodeRequestIndication.name = MTKBMC(pin_req_device).name;
    parms.p.PinCodeRequestIndication.cod = cod;
    MTKBMC(bmCB)(&parms); 
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);

#else /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBMHANDLEPINREQTIMEOUT);
    MTKBMC(pin_req_timer).func = 0;
    remDev = ME_FindRemoteDevice(&bd_addr_t);
    Assert(remDev);
    switch (MTKBMC(SSPDeviceInfo).sspType)
    {
        case BT_BM_SSP_PIN_GENERAL_CODE:
            MTKBMC(pin_bdc)->gapKeyType = BT_LINK_KEY_GAP_TYPE_COMBINATION_NONE16;
            parms.event = BTBM_EVENT_PINCODE_REQUEST_INDICATION;
            parms.p.PinCodeRequestIndication.pin_code_16_digit_required = 0;
            parms.p.PinCodeRequestIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.PinCodeRequestIndication.name_len = MTKBMC(pin_req_device).name_len;
            parms.p.PinCodeRequestIndication.name = MTKBMC(pin_req_device).name;
            parms.p.PinCodeRequestIndication.cod = cod;
            MTKBMC(bmCB)(&parms); 
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
            break;
        case BT_BM_SSP_PIN_USER_CONFIRM:
            bt_trace(TRACE_GROUP_1, BTLOG_MTKBMCSSPDEVICEINFONUMERICVALUExD, MTKBMC(SSPDeviceInfo).numericValue);
            MTKBMC(pin_bdc)->gapKeyType = btbmCalculateSSPLinkKeyStrength();
            bt_trace(TRACE_GROUP_1, BTLOG_BTBM_SPP_DEVICE_INFO_IO_CAPABILITY, MTKBMC(SSPDeviceInfo).ioCapability);
            OS_Report("MTKBMC(SSPDeviceInfo).authRequirements=%u", MTKBMC(SSPDeviceInfo).authRequirements);
            OS_Report("remDev->authInitiator = %u", remDev->authInitiator);
            if (MTKBMC(SSPDeviceInfo).ioCapability == 0x03){
                OS_Report("[SEC] remote ioCapability is 0x3");
                if(btbmCheckBondingState() || remDev->authInitiator != 0){
                    bt_bm_security_user_confirm_rsp_struct data;
                    OS_Report("[SEC] Auto accept in bonding state or auth initiated by local");
                    data.bd_addr = MTKBMC(pin_req_device).bd_addr;
                    data.accept = TRUE;                    
                    btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
                    BTBMGapWriteUserConfirm(&data);
                    break;
                }else{
                    displayNumeric = KAL_FALSE;
                }
            }else if(!(MTKBMC(SSPDeviceInfo).localAuthRequirements & 0x1) &&
               !(MTKBMC(SSPDeviceInfo).authRequirements & 0x1) &&
                 ((MTKBMC(SSPDeviceInfo).localAuthRequirements>>0x1) != 0x1)){
                bt_bm_security_user_confirm_rsp_struct data;
                OS_Report("[SEC] Local and remote MITM not required and not dedicate bonding. Accept directly");
                data.bd_addr = MTKBMC(pin_req_device).bd_addr;
                data.accept = TRUE;
                btbmUtilCheckBondingState(BM_BONDING_PINCODE_REQ, BEC_NO_ERROR);
                BTBMGapWriteUserConfirm(&data);
                break;
            }else{
                displayNumeric = KAL_TRUE;
            }
            parms.event = BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION;
            parms.p.SSPSecurityPairingIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.SSPSecurityPairingIndication.ssp_type = BT_BM_SSP_PIN_USER_CONFIRM;
            parms.p.SSPSecurityPairingIndication.numericValue = MTKBMC(SSPDeviceInfo).numericValue;
            parms.p.SSPSecurityPairingIndication.name_len = MTKBMC(pin_req_device).name_len;
            parms.p.SSPSecurityPairingIndication.name = MTKBMC(pin_req_device).name;
            parms.p.SSPSecurityPairingIndication.displayNumeric = displayNumeric;
            parms.p.SSPSecurityPairingIndication.cod = cod;
            MTKBMC(bmCB)(&parms); 
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
            break;
        case BT_BM_SSP_PIN_PASSKEY_ENTRY:
            if (MTKBMC(bm_opactivity) == BM_OP_BONDING)
            {
                btbmUtilCheckBondingState(BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            }
            else
            {
                btbmUtilCheckPairingState(&bd_addr_t, BM_BONDING_PINCODE_ENTER, BEC_NO_ERROR);
            }
            MTKBMC(pin_bdc)->gapKeyType = btbmCalculateSSPLinkKeyStrength();
            parms.event = BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION;
            parms.p.SSPSecurityPairingIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.SSPSecurityPairingIndication.ssp_type = BT_BM_SSP_PIN_PASSKEY_ENTRY;
            parms.p.SSPSecurityPairingIndication.numericValue = MTKBMC(SSPDeviceInfo).numericValue;
            parms.p.SSPSecurityPairingIndication.name_len = MTKBMC(pin_req_device).name_len;
            parms.p.SSPSecurityPairingIndication.name = MTKBMC(pin_req_device).name;
            parms.p.SSPSecurityPairingIndication.displayNumeric = KAL_TRUE;
            parms.p.SSPSecurityPairingIndication.cod = cod;
            MTKBMC(bmCB)(&parms); 
            MTKBMC(SSPDeviceInfo).keyPressEventReporting = TRUE;
            MTKBMC(SSPDeviceInfo).keyPressEventOccurs = FALSE;
            parms.event = BTBM_EVENT_SSP_KEY_PRESS_INDICATION;
            parms.p.SSPKeyPressIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.SSPKeyPressIndication.event = MTKBMC(SSPDeviceInfo).keyPressNo;    
            MTKBMC(bmCB)(&parms); 
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
            break;
        case BT_BM_SSP_PIN_OOB_DATA:
            MTKBMC(pin_bdc)->gapKeyType = BT_LINK_KEY_GAP_TYPE_AUTHENTICATED;
            parms.event = BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION;
            parms.p.SSPSecurityPairingIndication.bd_addr = MTKBMC(pin_req_device).bd_addr;
            parms.p.SSPSecurityPairingIndication.ssp_type = BT_BM_SSP_PIN_OOB_DATA;
            parms.p.SSPSecurityPairingIndication.numericValue = 0;
            parms.p.SSPSecurityPairingIndication.name_len = MTKBMC(pin_req_device).name_len;
            parms.p.SSPSecurityPairingIndication.name = MTKBMC(pin_req_device).name;
            parms.p.SSPSecurityPairingIndication.displayNumeric = KAL_FALSE;
            parms.p.SSPSecurityPairingIndication.cod = cod;
            MTKBMC(bmCB)(&parms); 
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
            break;
        case BT_BM_SSP_PIN_NONE:
            break;
    }
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 

}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlePinReqStartOperation
 * DESCRIPTION
 *  When we receive the Name request completed event during pairing or bonding procedure,
 *  we shall start a 200ms timer to report the PIN code request event to MMI
 * PARAMETERS
 *  bd_addr         [IN]        The remote pairing or bonding address
 *  name_len        [IN]        The remote pairing or bonding name length
 *  name            [IN]        The remote pairing or bonding name
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlePinReqStartOperation(btbm_bd_addr_t bd_addr, U8 name_len, U8 *name)
{
    OS_MemSet((U8*) & MTKBMC(pin_req_device), 0, sizeof(MTKBMC(pin_req_device)));
    MTKBMC(pin_req_device).bd_addr = bd_addr;
    MTKBMC(pin_req_device).name_len = name_len;
    OS_MemCopy(MTKBMC(pin_req_device).name, name, name_len);
    if (MTKBMC(pin_req_timer).func != 0)
    {
        EVM_CancelTimer(&MTKBMC(pin_req_timer));
    }
    MTKBMC(pin_req_timer).func = btbmHandlePinReqTimeout;
    EVM_StartTimer(&MTKBMC(pin_req_timer), 200);
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlePinReqCancelOperation
 * DESCRIPTION
 *  To Reset the PinRequest Timer.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlePinReqCancelOperation(void)
{
    OS_MemSet((U8*) & MTKBMC(pin_req_device), 0, sizeof(MTKBMC(pin_req_device)));
    if (MTKBMC(pin_req_timer).func != 0)
    {
        EVM_CancelTimer(&MTKBMC(pin_req_timer));
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmUtilCheckBondingState
 * DESCRIPTION
 *  The handle the current bonding procedure.
 * PARAMETERS
 *  bonding_state       [IN]        The current bonding state;
 * RETURNS
 *  void
 *****************************************************************************/
void btbmUtilCheckBondingState(U8 bonding_state, BtErrorCode errCode)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtDeviceContext *bdc = 0;
    sdap_service_search_all_struct search_pattern;
    btbm_bd_addr_t bd_addr;
    BtCodRecord record;
    BtDeviceRecord dev_rec;
    BTBMRemoteNameSetting_T *entry;

    bdc = MTKBMC(pin_bdc);
    CONVERT_ARRAY2BDADDR(&bd_addr, bdc->addr.addr);

    OS_Report("MbtbmUtilCheckBondingState=0x%x | 0x%x", MTKBMC(bonding_state), bonding_state);
    MTKBMC(bonding_state) |= bonding_state;
    if (bonding_state == BM_BONDING_FAILED_COMPLETED)
    {
        btbm_gap_result result = BTBM_ADP_FAILED;
#if defined(BTMTK_ON_WISE) || defined(BTMTK_ON_WISESDK)
        result = bt_util_convert_err1(errCode);
#endif
        btbmUtilPairingProcedureClear();
        btbmHandlePinReqCancelOperation();
        btbmSendBondingResultInd((U8)result, BTBM_TYPE_BONDING, bd_addr, 0, 0, NULL);
        btbmSendBondingDeviceCfm((U8)result, bd_addr);
        if (bdc->link != 0)
        {
            btbmAddLinkCallback(bdc->link);
            bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x06);
            ME_DisconnectLink(&linkbmHandler, bdc->link);
            btbmRemoveLinkCallback(bdc->link);
        }
        return;
    }
    if (bonding_state == BM_BONDING_ACL_CREATED)
    {
        btbmRemoveLinkCallback(bdc->link);
        MTKBMC(pin_bdc)->link->pairingType = BPT_SAVE_NOT_TRUSTED;
        bdc->link->discFlag = TRUE;
        if ((bdc->link->authState == BAS_NOT_AUTHENTICATED) && (bdc->link->encryptState  == BES_NOT_ENCRYPTED))
        {
            /* Auth cmd is not allowed if link is already in encryped state unless it the peer device supports pause_encryption feature in BT2.1 */
            SEC_AuthenticateLink(&globalbtbmHandler, MTKBMC(pin_bdc)->link);
        }
        else
        {
            /* MMI stats the bonding procedure, but peer device is security level 3: link complete event may indicate it is encrypted or not */
            bt_trace(TRACE_GROUP_1, BTBM_BONDING_NOT_SEND_AUTH_CMD_AUTHSTATE_ENCRYPTSTATE,bdc->link->authState,bdc->link->encryptState);
        }
    }
    else if ((MTKBMC(bonding_state) & BM_BONDING_NAME_REQUESTED) == 0x00)
    {
       
#ifdef __BT_4_0_BLE__
#ifdef __BT_LE_STANDALONE__
        status = btbmBleNameReq(bdc, btbmHandlerPinCodeAskNameQuery);

#else
        OS_Report("btbmNameReq: devType=%d", bdc->devType);
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            status = btbmBleNameReq(bdc, btbmHandlerPinCodeAskNameQuery);
        }
        else
        {
            status = btbmNameReq(&BMController.nameTok, bdc, btbmHandlerPinCodeAskNameQuery);
        }
#endif
#else
        status = btbmNameReq(&BMController.nameTok, bdc, btbmHandlerPinCodeAskNameQuery);
                
#endif


        
        if (status != BT_STATUS_PENDING)
        {
            SEC_SetPinByAddr(bdc->addr, 0x00, 0x00, BPT_SAVE_TRUSTED);
            btbmUtilPairingProcedureClear();
            btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_BONDING, bd_addr, 0, 0, NULL);
            btbmSendBondingDeviceCfm(BTBM_ADP_FAILED, bd_addr);
            return;
        }
    }
	
    if (MTKBMC(bonding_state) ==
        ((BM_BONDING_ACL_CREATED) | (BM_BONDING_PINCODE_ENTER) | (BM_BONDING_NAME_REQUESTED) | (BM_BONDING_PINCODE_REQ)
         | (BM_BONDING_SUCCESS_COMPLETED)))
    {
        if (bdc->classOfDevice == 0)
        {
            if (DDB_COD_FindRecord(&(bdc->addr), &record) == BT_STATUS_SUCCESS)
            {
                bdc->classOfDevice = record.cod;
            }
        }

        OS_MemSet((U8 *)&dev_rec, 0x0, sizeof(BtDeviceRecord));
        SEC_FindDeviceRecord(&bdc->addr, &dev_rec);

        btbmHandlePinReqCancelOperation();
        btbmUtilPairingProcedureClear();
        btbmSendBondingResultInd(BTBM_ADP_SUCCESS, BTBM_TYPE_BONDING, bd_addr, 
            bdc->classOfDevice, dev_rec.keyType, dev_rec.linkKey);
        entry = btbmUtilConnectedDeviceNameEntryLookUp(&bdc->addr);
        if(entry && entry->dev_name_len)
            btbmSendRemoteNameInd(bd_addr, entry->dev_name_len, (U8*)entry->dev_name);
        btbmSendBondingDeviceCfm(BTBM_ADP_SUCCESS, bd_addr);

        if (MTKBMC(sdap_len) > 0)
        {
            #if 1
            btbmStartServiceSearch();
            #else
            search_pattern.bdAddr = &bdc->addr;
            search_pattern.sdap_len = MTKBMC(sdap_len);
            search_pattern.sdp_uuid = MTKBMC(sdap_uuid);
            SDAP_ServiceSearchAll(search_pattern);
            #endif
        }
        else
        {
                btbmAddLinkCallback(bdc->link);
                bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x07);
                ME_DisconnectLink(&linkbmHandler, bdc->link);
                btbmRemoveLinkCallback(bdc->link);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmUtilCheckPairingState
 * DESCRIPTION
 *  The handle the current pairing procedure.
 * PARAMETERS
 *  remote_addr         [IN]        The remote Bluetooth address
 *  pairing_state       [IN]        The current pairing statel
 * RETURNS
 *  void
 *****************************************************************************/
void btbmUtilCheckPairingState(BD_ADDR *remote_addr, U8 pairing_state, BtErrorCode errCode)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t bd_addr;
    BtCodRecord record;
    BtDeviceRecord dev_rec;
    BTBMRemoteNameSetting_T *entry;
    
    bdc = DS_FindDevice(remote_addr);
    CONVERT_ARRAY2BDADDR(&bd_addr, bdc->addr.addr);

    OS_Report("btbmUtilCheckPairingState=0x%x | 0x%x", MTKBMC(bonding_state), pairing_state);
    if (bdc != MTKBMC(pin_bdc))
    {
        return;
    }

    MTKBMC(bonding_state) |= pairing_state;
    if (pairing_state == BM_BONDING_FAILED_COMPLETED)
    {
        btbmUtilPairingProcedureClear();
        if (MTKBMC(pin_req_timer).func != 0)
        {
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
            btbmHandlePinReqCancelOperation();
        }
        else
        {
            btbm_gap_result result = BTBM_ADP_FAILED;
#if defined(BTMTK_ON_WISE) || defined(BTMTK_ON_WISESDK)
            result = bt_util_convert_err1(errCode);
#endif
            btbmSendBondingResultInd((U8)result, BTBM_TYPE_PAIRING, bd_addr, 0, 0, NULL);
        }
        return;
    }
    if (pairing_state == BM_BONDING_ACL_CREATED)
    {
    }
    else if ((MTKBMC(bonding_state) & BM_BONDING_NAME_REQUESTED) == 0)
    {
#ifdef __BT_4_0_BLE__
#ifdef __BT_LE_STANDALONE__
        status = btbmBleNameReq(bdc, btbmHandlerPinCodeAskNameQuery);

#else
        OS_Report("btbmNameReq: devType=%d", bdc->devType);
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            status = btbmBleNameReq(bdc, btbmHandlerPinCodeAskNameQuery);
        }
        else
        {
            status = btbmNameReq(&BMController.nameTok, bdc, btbmHandlerPinCodeAskNameQuery);
        }
#endif
#else
        status = btbmNameReq(&BMController.nameTok, bdc, btbmHandlerPinCodeAskNameQuery);
                
#endif        
        if (status != BT_STATUS_PENDING)
        {
            SEC_SetPinByAddr(bdc->addr, 0x00, 0x00, BPT_SAVE_TRUSTED);
            btbmUtilPairingProcedureClear();
            if (MTKBMC(pin_req_timer).func != 0)
            {
                btbmHandlePinReqCancelOperation();
                BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
            }
            else
            {
                btbmSendBondingResultInd(BTBM_ADP_FAILED, BTBM_TYPE_PAIRING, bd_addr, 0, 0, NULL);
            }
            return;
        }
    }
    if ((MTKBMC(bonding_state) & BM_BONDING_PINCODE_ENTER) && 
        (MTKBMC(bonding_state) & BM_BONDING_NAME_REQUESTED) &&
        (MTKBMC(bonding_state) & BM_BONDING_PINCODE_REQ) &&
        (MTKBMC(bonding_state) & BM_BONDING_SUCCESS_COMPLETED))
    {
        if (MTKBMC(bonding_state) & BM_BONDING_ACL_CREATED)
        {
            OS_Report("Security mode 2/4");
        }
        else
        {
            OS_Report("Security mode 3");
        }
        if (bdc->classOfDevice == 0)
        {
            if (DDB_COD_FindRecord(&(bdc->addr), &record) == BT_STATUS_SUCCESS)
            {
                bdc->classOfDevice = record.cod;
            }
        }
        OS_MemSet((U8 *)&dev_rec, 0x0, sizeof(BtDeviceRecord));
        SEC_FindDeviceRecord(&bdc->addr, &dev_rec);

        btbmUtilPairingProcedureClear();
        btbmHandlePinReqCancelOperation();
        btbmSendBondingResultInd(BTBM_ADP_SUCCESS, BTBM_TYPE_PAIRING, bd_addr, 
            bdc->classOfDevice, dev_rec.keyType, dev_rec.linkKey);
        entry = btbmUtilConnectedDeviceNameEntryLookUp(remote_addr);
        if(entry && entry->dev_name_len)
            btbmSendRemoteNameInd(bd_addr, entry->dev_name_len, (U8*)entry->dev_name);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmUtilPairingProcedureClear
 * DESCRIPTION
 *  To clear the current pairing or bonding state
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmUtilPairingProcedureClear(void)
{
    MTKBMC(bonding_state) = BM_BONDING_NONE;
    MTKBMC(bm_opactivity) = BM_OP_NONE;
    if (MTKBMC(pin_bdc))
    {
        /* Restore LinkPolicy parameter which is all disabled in MeHandleAclConComplete */
        U8 piconet_no = 0, scatternet_no = 0, sco_no = 0, esco_no = 0, msco_no = 0, ssco_no = 0;
        BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &msco_no, &ssco_no);
        Me_ChangeLinkPolicy(MTKBMC(pin_bdc)->link, ((sco_no + esco_no) ? BT_OP_DISABLE : BT_OP_ENABLE), BT_OP_ENABLE);

        MTKBMC(pin_bdc)->under_used &= ~BDS_BONDING;
        Report(("MTKBMC(pin_bdc)->under_used=0x%x", MTKBMC(pin_bdc)->under_used));
        MTKBMC(pin_bdc) = 0;
    }
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    OS_MemSet((U8*) & (MTKBMC(SSPDeviceInfo)), 0, sizeof(BTBMSSPRemoteDeviceInfo_T));
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  btbmCheckBondingState
 * DESCRIPTION
 *  Check the current state in Bonding state.
 * PARAMETERS
 *  void
 * RETURNS
 *  TRUE in bonding mode, FALSE is not in bonding mode
 *****************************************************************************/
BOOL btbmCheckBondingState(void)
{
    if (MTKBMC(bm_opactivity) == BM_OP_BONDING)
    {
        bt_trace(TRACE_GROUP_1, ACL_IN_BONDING);
        return TRUE;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, ACL_IN_NORMAL);
        return FALSE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmCheckCreateLinkKeyState
 * DESCRIPTION
 *  Check the current state in Bonding or Pairing state.
 * PARAMETERS
 *  void
 * RETURNS
 *  TRUE in bonding mode, FALSE is not in CreateLinkKey state
 *****************************************************************************/
BOOL btbmCheckCreateLinkKeyState(void)
{
    if ((MTKBMC(bm_opactivity) == BM_OP_BONDING) || (MTKBMC(bm_opactivity) == BM_OP_PAIRING))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmCheckBondingStateWhenDisconnected
 * DESCRIPTION
 *  Check the current state of a special device in Bonding state when the L2CAP try to disconnect ACL link.
 * PARAMETERS
 *  addr1       [IN]        The checked device address
 * RETURNS
 *  TRUE in bonding mode, FALSE is not in bonding mode
 *****************************************************************************/
BOOL btbmCheckBondingStateWhenDisconnected(U8 *addr1)
{
    U8 *addr;
    BOOL status = FALSE;

    bt_trace(TRACE_GROUP_1, BTBMCHECKBONDINGSTATEWHENDISCONNECTEDBT_DEBUGx02X, MTKBMC(bm_opactivity));
    if (MTKBMC(bm_opactivity) == BM_OP_BONDING || MTKBMC(bm_opactivity) == BM_OP_PAIRING)
    {
        if (MTKBMC(pin_bdc) != 0)
            if (MTKBMC(pin_bdc)->link != 0)
            {
                addr = MTKBMC(pin_bdc)->link->bdAddr.addr;
                /* While trying to disconnect link, but this link is performing bonding/pairing, Can Not Disconnect Link Now! */
                /* In pairing case: jsr82 midlet asks sdp, and gets rsp. Then starts a timer to disc acl link, but midlet issues con_req to exchange pin code while this timer is expired */
                if (OS_MemCmp(addr, 6, addr1, 6) == TRUE)
                {
                    status = TRUE;
                }
            }
    }
    bt_trace(TRACE_GROUP_1, RETURN_STATUSx08X, status);
    return status;
}

#if defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  btbmCalculateSSPLinkKeyStrength
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 btbmCalculateSSPLinkKeyStrength(void)
{
#ifndef __GAP_TESTER_
    U8 key_type = 0;

    switch (MTKBMC(SSPDeviceInfo).ioCapability)
    {
        case BT_SSP_IO_CAPABILITY_DISPLAY_ONLY:
        case BT_SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT:
            key_type = BT_LINK_KEY_GAP_TYPE_UNAUTHENTICATED;
            break;
        case BT_SSP_IO_CAPABILITY_DISPLAY_YESNO:
        case BT_SSP_IO_CAPABILITY_KEYBORAD_ONLY:
            key_type = BT_LINK_KEY_GAP_TYPE_AUTHENTICATED;
            break;
        default:
            key_type = BT_LINK_KEY_GAP_TYPE_NO_KEY;
            break;
    }
#else /* __GAP_TESTER_ */ 
    U8 key_type = BT_LINK_KEY_GAP_TYPE_UNAUTHENTICATED;

    if (((MTKBMC(SSPDeviceInfo).localAuthRequirements & 0x01) != 0) ||
        ((MTKBMC(SSPDeviceInfo).authRequirements & 0x01) != 0))
    {
        key_type = BT_LINK_KEY_GAP_TYPE_AUTHENTICATED;
    }
#endif /* __GAP_TESTER_ */ 
    bt_trace(TRACE_GROUP_1, BTLOG_SSP_KEY_TYPExD, key_type);
    return key_type;
}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

void btbmSendBondingDeviceCfm(U8 result, btbm_bd_addr_t bd_addr)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_BONDING_DEVICE_RESULT;
    parms.p.BondingDeviceCompleteResult.result = result;
    parms.p.BondingDeviceCompleteResult.bd_addr = bd_addr;
    MTKBMC(bmCB)(&parms);
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
}

void btbmSendBondingResultInd(
                U8 result, 
                U8 type, 
                btbm_bd_addr_t bd_addr, 
                U32 cod, 
                U8 key_type, 
                U8 *key)
{
    BMCallbackParms parms;
    BD_ADDR BdAddr;
    
    CONVERT_BDADDRSRC2ARRAY((U8 *)BdAddr.addr, &bd_addr);
    parms.event = BTBM_EVENT_BONDING_DEVICE_RESULT_INDICATION;
    parms.p.LinkKeyGereratedResult.result = result;
    parms.p.LinkKeyGereratedResult.type = type;
    parms.p.LinkKeyGereratedResult.bd_addr = bd_addr;
    parms.p.LinkKeyGereratedResult.cod = cod;
    parms.p.LinkKeyGereratedResult.key_type = key_type;
    parms.p.LinkKeyGereratedResult.key = key;
#ifdef __BT_4_0_BLE__
    parms.p.LinkKeyGereratedResult.device_type = bt_util_convert_dev(ME_GetDeviceType(&BdAddr));
#endif
    MTKBMC(bmCB)(&parms);    
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
}

static void btbmSendBondingCancelCfm(U8 result, btbm_bd_addr_t bd_addr)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_BONDING_CANCEL_RESULT;
    parms.p.BondCancelResult.result = result;
    parms.p.BondCancelResult.bd_addr = bd_addr;
    MTKBMC(bmCB)(&parms);
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
}

void btbmSendLinkkeyDelInd(
                U8 result, 
                const BD_ADDR *BdAddr)
{
    BMCallbackParms parms;
    bt_addr_struct bd_addr;
    
    CONVERT_ARRAY2BDADDR(&bd_addr, BdAddr->addr);
    parms.event = BTBM_EVENT_BONDING_DEVICE_RESULT_INDICATION;
    parms.p.LinkKeyGereratedResult.result = result;
    parms.p.LinkKeyGereratedResult.bd_addr = bd_addr;
    parms.p.LinkKeyGereratedResult.key = NULL;
    MTKBMC(bmCB)(&parms);    
}

#ifdef __BQE_TESTER_


/*****************************************************************************
 * FUNCTION
 *  btbmGAPTesterAuthenticationLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmGAPTesterAuthenticationLink(void)
{
    U8 i = 0;
    BtRemoteDevice *link;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if (link->state == BDS_CONNECTED)
        {
            SEC_AuthenticateLink(&globalbtbmHandler, link);
            break;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmGAPTesterEncryptedLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmGAPTesterEncryptedLink(void)
{
    U8 i = 0;
    BtRemoteDevice *link;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if (link->state == BDS_CONNECTED)
        {
            SEC_SetLinkEncryption(&globalbtbmHandler, link, BECM_ENCRYPT_ENABLE);
            break;
        }
    }
}

#endif /* __BQE_TESTER_ */ 



void btbmGAPSetCTSServerSecurityLow(U8 value)
{
    MTKBMC(CTSTest) = value;

}

