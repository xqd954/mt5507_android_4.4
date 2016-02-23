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
#include "osapi.h"
#include "sdp.h"
#include "btproperty.h"
#include "btbm_adp.h"
#include "btbm.h"
#include "sdap.h"
#include "btaccctrl.h"
//#include "wndrv_api.h"
#include "osapi.h"
#include "btbm_int.h"
#include "bt_devices_name.h"
#include "utils.h"
#include "btutils.h"
#include "stdlib.h"

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
static U8 s_nullVer[] = {0,0,0,0,0};

static const struct 
{
    U8 ver[5];
    DEVICE_MODEL model;
} s_devVerModel[] = 
{
    {{0x03, 0x04, 0x00, 0x10, 0x05}, DEVICE_TOSHIBA1296},
    {{0x06, 0x0d, 0x00, 0x12, 0x1f}, DEVICE_SE_XPERIA_RAY},
};

static const struct 
{
    U8 id[3]; /* addr[3], addr[4], addr[5]*/
    DEVICE_MODEL model;
} s_devAddrModel[] = 
{
    {{0x24, 0x0b, 0x00}, DEVICE_K5},
    {{0xb2, 0x1e, 0x00}, DEVICE_MTS},        
};

static DEVICE_MODEL s_roleSwitchBlackList[] =
{
    DEVICE_TOSHIBA1296,     
    DEVICE_PCM,
    DEVICE_K5,
    DEVICE_MTS
};

/* MTK Bluetooth Manager Context */
extern BMCONTENT_T BMController;

#define MTKBMC(f)  (BMController.f)

extern BtHandler globalbtbmHandler;
extern BtHandler linkbmHandler;
extern const U8 SAMSUNG_410[];
extern U8 gaptester_discovery_ask_without_name;

#if (BT_READ_RSSI == XA_ENABLED)
extern BtStatus ME_ReadRSSI(BtRemoteDevice *remDev);
#endif
extern U8 Me_WriteAudioSetting(void);


/*****************************************************************************
 * FUNCTION
 *  DummyLinkCallback
 * DESCRIPTION
 *  This function is register when we connect with other device.
 *  This is a dummy function which will be called when we call the ME_CreateLink
 * PARAMETERS
 *  Event       [IN]        Event of ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void DummyLinkCallback(const BtEvent *Event)
{
    btbm_bd_addr_t bd_addr;
    BMCallbackParms parms;
    
    switch (Event->eType)
    {
        case BTEVENT_LINK_CONNECT_REQ:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_DUMMY_LINK_CONN_REQ);
            if (MTKBMC(power_on_off_mode) == BM_POWER_ON)
            {
            #if defined (__BT_2_1_BLOCK_DEVICE__)
                if (BTBMCheckBlockList(Event->p.remDev->bdAddr.addr) == TRUE)
                {
                    bt_trace(TRACE_GROUP_1, BTLOG_IN_BLOCK_LIST_REJECT);
                    ME_RejectIncomingLink(Event->p.remDev, BEC_SECURITY_ERROR);
                    return;
                }
            #endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 
                if (MTKBMC(wait_conection_acp_rsp) == BM_LINK_ACCEPT_CMD_AUTO)
                {
                    btbmHandleLinkConnectReq(Event->p.remDev);
                    //ME_AcceptIncomingLink(Event->p.remDev, BCR_SLAVE);
                }
                else
                {
                    if (MTKBMC(wait_conection_acp_rsp) == BM_LINK_ACCEPT_CMD_NONE)
                    {
                        MTKBMC(wait_conection_acp_rsp) = BM_LINK_ACCEPT_CMD_WAIT;
                        MTKBMC(connect_acp_link) = Event->p.remDev;
                        CONVERT_ARRAY2BDADDR(&bd_addr, Event->p.remDev->bdAddr.addr);
                        parms.event = BTBM_EVENT_LINK_ACCEPT_CONFIRM_INDICATION;
                        parms.p.bd_addr = bd_addr;
                        MTKBMC(bmCB)(&parms);
                    }
                    else
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTDONT_WAIT_MMI_CONNECT_RSP_REJECT_NEW_CONENCTION);
                        ME_RejectIncomingLink(Event->p.remDev, BEC_LIMITED_RESOURCE);
                    }
                }

            }
            else
            {
                bt_trace(TRACE_GROUP_1, POWER_OFF_REJECT_IT);
                ME_RejectIncomingLink(Event->p.remDev, BEC_LIMITED_RESOURCE);
            }
            break;
    }
}

#if defined (__BT_2_1_BLOCK_DEVICE__)


/*****************************************************************************
 * FUNCTION
 *  BTBMBlockDeviceForceDisconnect
 * DESCRIPTION
 *  The MMI will request to disconnect the ACL
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMBlockDeviceForceDisconnect(bt_bm_block_active_link_req_struct *btbm_adp_input_msg)
{
    BD_ADDR addr;
    btbm_bd_addr_t *remoteaddr;
    BMCallbackParms parms;
    
    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(addr.addr, remoteaddr);
    btbmHandleForceDisconnectLink(&addr);
    parms.event = BTBM_EVENT_BLOCKLIST_DISCONNECT_RESULT;
    parms.p.bd_addr = btbm_adp_input_msg->bd_addr;
    MTKBMC(bmCB)(&parms);
}
#endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 


/*****************************************************************************
 * FUNCTION
 *  BTBMChangeScattnetRoleForSCO
 * DESCRIPTION
 *  To provide the check if the scatternet exists, if in scatternet state,
 *  we will try to do the role switch for one active link.(Not all link)
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  The current piconet state(FALSE) or scatternet state(TRUE)
 *****************************************************************************/
BOOL BTBMChangeScattnetRoleForSCO(BtRemoteDevice *remDev)
{
    BOOL result = FALSE;
    U8 i = 0;
    BtRemoteDevice *link = 0;
    U32 value = 0;  

    OS_GetSystemPropertyUint("sconocarescattnet", &value, 0);    
    if (value)
    {
        return result;        
    }    
    Report(("ScattnetRoleForSCO(): ptr=%04X,state=%d,switchstate=%d,role=%d",remDev,remDev->state,remDev->switchState,remDev->role));
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        Report(("ScattnetRoleForSCO()_2: ptr=%04X,state=%d,switchstate=%d,role=%d",link,link->state,link->switchState,link->role));
        if (link->state == BDS_CONNECTED)
        {
            if (link->switchState != 0)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_PREVIOUS_ROLE_SWITCH_IS_NOT_COMPLETED);
                /* goto done; */
                result = TRUE;
                break;

            }
            if (link->role == BCR_SLAVE)
            {
                /* Ignore link in pairing since it may not affect much and it's not possible to do role switch */
                if (link->authState > BAS_NOT_AUTHENTICATED && link->authState < BAS_AUTHENTICATED)
                {
                    Report(("ScattnetRoleForSCO(): Skip role switch for pairing slave: state=%d", link->authState));
                    continue;
                }

                bt_trace(TRACE_GROUP_1, BTLOG_DO_SCO_ROLE_SWITCH);
                BTBMDoRoleSwitch(link);
//                BTBMDoRoleSwitch(remDev);
                result = TRUE;
                /* goto done; */
                break;
            }
        }
    }
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGetScattnetStatus
 * DESCRIPTION
 *  
 * PARAMETERS
 *  piconet         [OUT]       
 *  scatternet      [OUT]       
 *  sco             [OUT]       
 *  esco            [OUT]       
 * RETURNS
 *  
 *****************************************************************************/
void BTBMGetScattnetStatus(U8 *piconet, U8 *scatternet, U8 *sco, U8 *esco, U8 *msco, U8 *ssco)
{
    U8 i = 0;
    BtRemoteDevice *link = 0;
    BtScoConnect *scocon;

    *piconet = 0;
    *scatternet = 0;
    *sco = 0;
    *esco = 0;
    *msco = 0;
    *ssco = 0;
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if (link->state == BDS_CONNECTED)
        {
            if (link->role == BCR_SLAVE)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_CON_HDLx04X_SLAVE , link->hciHandle);
                *scatternet = *scatternet + 1;
            }
            else
            {
                bt_trace(TRACE_GROUP_1, BTLOG_CON_HDLx04X_MASTER , link->hciHandle);
                *piconet = 1;
            }
        }
    }
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scocon = &(MEC(scoTable)[i]);
        switch (MEC(scoTable)[i].scostate)
        {
            case BDS_CONNECTED:
                bt_trace(TRACE_GROUP_1, ONE_SCO_IN_CONNECTED);
                if (scocon->scoLinkType == BLT_SCO)
                {
                    *sco = *sco + 1;
                }
                else
                {
                    *esco = *esco + 1;
                }
                if (scocon->scord->role == BCR_MASTER)
                {
                    *msco = *msco + 1;
                }
                else
                {
                    *ssco = *ssco + 1;
                }
                break;
        }
    }

    bt_trace(TRACE_GROUP_1, BTLOG_BTBM_PICONETNUM_SCATTERNETNUM, *piconet, *scatternet, *sco, *esco, *msco, *ssco);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMChangeAllLinkPolcy
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev          [IN]        
 *  disabled        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMChangeAllLinkPolcy(BtRemoteDevice *remDev, BOOL disabled)
{
    BtRemoteDevice *link = 0;
    U8 i = 0;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if ((link->state == BDS_CONNECTED) && (link != remDev))
        {
            bt_trace(TRACE_GROUP_1, BTLOG_LINKROLExD_DISBALEDxD , link->role, disabled);
            if (!((link->role == BCR_SLAVE) && (disabled))) /* we don't disable role switch if we are slave */
            {
                bt_trace(TRACE_GROUP_1, BTLOG_TRY_TO_MAKE_LINK_POLICY);
                Me_ChangeLinkPolicy(link, (disabled ? BT_OP_DISABLE : BT_OP_ENABLE), BT_OP_DONT_CARE);
            }
        }
    }
}

BOOL BTBMCheckRoleSwitchBlockList(BtRemoteDevice *remDev)
{
    U32 i = 0;
    U32 count = sizeof(s_roleSwitchBlackList) / sizeof(s_roleSwitchBlackList[0]);
    DEVICE_MODEL nameModel = GENERAL_DEVICE;   
    DEVICE_MODEL addrModel = GENERAL_DEVICE;     
    DEVICE_MODEL verModel = BTBMGetDeviceModelByVersion(remDev);  

    if (remDev)
    {
        nameModel = BTBMGetDeviceModel(&remDev->bdAddr);
        addrModel = BTBMGetDeviceModelByAddr(&remDev->bdAddr);
    }    
    
    for (i = 0; i < count; i++)
    {
        if (s_roleSwitchBlackList[i] == GENERAL_DEVICE)
        {
            continue;
        }
        if ((nameModel == s_roleSwitchBlackList[i]) || 
            (addrModel == s_roleSwitchBlackList[i]) ||
            (verModel  == s_roleSwitchBlackList[i]))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMDoRoleSwitch
 * DESCRIPTION
 *  To perform the role switch operation to be the master for a ACL link
 * PARAMETERS
 *  link        [IN]        The ACL link
 * RETURNS
 *  void
 *****************************************************************************/
BtStatus BTBMDoRoleSwitch(BtRemoteDevice *link)
{
    BtStatus status = BT_STATUS_FAILED;
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    U8 i=0;
#endif
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTDO_ROLE_SWITCH_WITH_FEATUREBT_DEBUGx02X, link->remoteFeature[0]);
    Me_WriteAudioSetting();
    if (link->scOutoConnectState == 1)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_IN_SCO_CONNECT_REQ_NO_ROLE_SWITCH);
        return status;
    }
    if((MTKBMC(scoData) != 0) && ((random() %3) == 0))
    {    
#ifdef __BT_DLIGHT1_DEBUG__
        Report(("autumn current state not allow to do the role switch operation"));
#endif 
        return status;
    }
    if (link->role == BCR_MASTER)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_ALREADY_MASTER_RETURN);
        return status;
    }

#if 0//defined (__BT_2_1_SIMPLE_PAIRING__)
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if ((MEC(devTable)[i].state == BDS_CONNECTED) && 
            (MEC(devTable)[i].authState != BAS_AUTHENTICATED))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTROLE_SWITCH_WAIT_AUTHENTICATED);
            return ;
        }
    }    
#endif

    if ((link->remoteFeature[0] & 0x20) == 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTROLE_SWITCH_NOT_SUPPORTED);
        return status;
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTROLE_SWITCH_SUPPORTED);
    }
    bt_trace(TRACE_GROUP_1, MTKBMCHID_BROADCOM_IOT_ROLE_SWITCHBT_DEBUGx02X, MTKBMC(hid_broadcom_iot_role_switch));
    if ((MTKBMC(hid_broadcom_iot_role_switch) == 1) && (link->remoteVersion[1] == 0x0F) &&
        (link->remoteVersion[2] == 0x00))
    {
        bt_trace(TRACE_GROUP_1, BROADCOM_CHIP_HID_EXIST_NO_ROLE_SWITCH);
        return status;
    }
    //if (BTBMGetDeviceModelByVersion(link) == DEVICE_TOSHIBA1296 ||
        //BTBMGetDeviceModel(&link->bdAddr) == DEVICE_PCM)
    //{
        //[MAUI_03243200] Toshiba Corp1296 has problem with local initiates encryption and
        //remote initiates sco connection
        //Report(("Case Toshiba Corp with SubVersNr 1296, we don't do role switch"));                
        //return status;
    //}
    if((U32)btGetDevCustvalueById(link->devId, CUSTID_NO_ROLE_SWITCH)||
        BTBMCheckRoleSwitchBlockList(link)){
        Report(("RemDev did not allow role switch."));
        return status;        
    }
    if (link->switchState != BM_ROLE_SWITCH_NONE)
    {
        bt_trace(TRACE_GROUP_1, ROLE_SWITCH_PERFORM_RETURN_DIRECTLY);
        return BT_STATUS_PENDING;
    }

    Report(("BTBMDoRoleSwitch: mode=0x%x, encryptState=0x%x, remoteFeature[5]=0x%x", 
        ME_GetCurrentMode(link), link->encryptState, link->remoteFeature[5]));
    if (ME_GetCurrentMode(link) == BLM_SNIFF_MODE)
    {
        status = ME_StopSniff(link);       
        if (status == BT_STATUS_PENDING)
        {
            link->switchState = BM_ROLE_SWITCH_EXIT_SNIFF;
        }
    }
    else if (link->encryptState != BES_ENCRYPTED || (link->remoteFeature[5] & 0x04))
    {
        if ((MTKBMC(bm_opactivity) == BM_OP_BONDING || MTKBMC(bm_opactivity) == BM_OP_PAIRING) && (MTKBMC(pin_bdc) != 0) && (MTKBMC(pin_bdc)->link == link))
        {
            bt_trace(TRACE_GROUP_1, DURING_PAIRING_NOT_ALLOW_ROLE_SWITCH);
            return status;
        }
        else
        {
            link->oldEncryptionMode = link->encryptState;
            link->switchState = BM_ROLE_SWITCH_NONE;
            status = ME_SwitchRole(link);
        }
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLINKENCRYPTSTATE_OLD_1BT_DEBUGx02X, link->encryptState);
        link->oldEncryptionMode = BES_ENCRYPTED;
        link->switchState = BM_ROLE_SWITCH_STOP_ENCRYPTION;
        status = SEC_SetLinkEncryption(0, link, BECM_ENCRYPT_DISABLE);
    }
    OS_Report("BTBMDoRoleSwitch: switchState=0x%x", link->switchState);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMHIDNoRoleSwitch
 * DESCRIPTION
 *  The HID profiles will set the value that if we connect with broadcom chip, we will not do the role switch operation.
 * PARAMETERS
 *  ON_OFF      [IN]        Set to on or off
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMHIDNoRoleSwitch(U8 ON_OFF)
{
    bt_trace(TRACE_GROUP_1, HID_SET_MODEBT_DEBUGx02X, ON_OFF);
    if (ON_OFF == 0)
    {
        MTKBMC(hid_broadcom_iot_role_switch) = 0;
    }
    else
    {
        MTKBMC(hid_broadcom_iot_role_switch) = 1;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMLocalAllowAllLink
 * DESCRIPTION
 *  It is used by SAP profile to set the btstack into ready to accept new connection.
 *  After that, we can accept new connection
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMLocalAllowAllLink(void)
{
    MTKBMC(power_on_off_mode) = BM_POWER_ON;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMReturnNormalLinkState
 * DESCRIPTION
 *  The function is set the BT stack as accept new link state or reject all link state. In which state depends on
 *  the previous state. Currently, only the SIM will use it.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReturnNormalLinkState(void)
{
    bt_trace(TRACE_GROUP_1, CURRENT_LINK_STATEBT_DEBUGx02X, MTKBMC(link_allow_state));
    switch (MTKBMC(link_allow_state))
    {
        case BM_LINK_DISALLOW:
            BTAccCtrlSet((BT_PSM_POLICY_NONE), BT_SERVICE_POLICY_NONE);
            break;
        case BM_LINK_ALLOW:
            BTAccCtrlUnset();
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAllowAllLink
 * DESCRIPTION
 *  The function is used by MMI power on procedure to ready to accept new connection.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAllowAllLink(void)
{
    BMCallbackParms parms;

    MTKBMC(power_on_off_mode) = BM_POWER_ON;
    MTKBMC(link_allow_state) = BM_LINK_ALLOW;
    BTBMReturnNormalLinkState();
    parms.event = BTBM_EVENT_ALLOW_ACL_LINK_RESULT;
    MTKBMC(bmCB)(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMDisconnectAllLink
 * DESCRIPTION
 *  The function provide the MMI to disconnect all active ACL link.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMDisconnectAllLink(void)
{
    BtDeviceContext *bdc = 0;
    BD_ADDR addr;
    BtScoConnect *scocon;
    BOOL aclexist = FALSE;
    U8 i = 0;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_DISCONNECT_ALL_LINK);

    MTKBMC(power_on_off_mode) = BM_POWER_OFF_PENDING;
    MTKBMC(link_allow_state) = BM_LINK_DISALLOW;
    BTBMReturnNormalLinkState();
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scocon = &(MEC(scoTable)[i]);
        /* Hunt for an outstanding connection */
        if ((MEC(scoTable)[i].scostate == BDS_OUT_CON2) || (MEC(scoTable)[i].scostate == BDS_IN_CON))
        {
            break;
        }
        else if (MEC(scoTable)[i].scostate == BDS_CONNECTED)
        {
            SCO_DisconnectLink(scocon);
        }
    }
    /* Clear "In Range" status for all devices. */
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
    {
        BTBMDumpBdAddr(bdc->addr.addr);

        if (bdc->link != 0)
        {
            aclexist = TRUE;
            bt_trace(TRACE_GROUP_1, BDCLINKSTATEBT_DEBUGx02X, bdc->link->state);
            if ((bdc->link->state == BDS_CONNECTED) || (bdc->link->state == BDS_IN_CON))
            {
                addr = bdc->addr;
                bdc->link->discFlag = TRUE;
                btbmDisconnectAclLinkWithoutForce(bdc->link, BEC_POWER_OFF, 1);
            }
            else if (bdc->link->state == BDS_OUT_CON)
            {
                btbmAddLinkCallback(bdc->link);
                bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x03);
                ME_CancelCreateLink(&linkbmHandler, bdc->link);
                btbmRemoveLinkCallback(bdc->link);
            }
        }
    }
    btbmHandlerCheckAllLinkPowerOffStatus();

}


/*****************************************************************************
 * FUNCTION
 *  BTBMCheckAllowScoConnectReq
 * DESCRIPTION
 *  The function is used to provide the ckeck for SCO can be connected or not.
 *  If the current power on off state is power on, the SCO can be accepted. Otherwise, the SCO will be rejected.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
U8 BTBMCheckAllowScoConnectReq(void)
{
    if (MTKBMC(power_on_off_mode) == BM_POWER_ON)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}


/*****************************************************************************
 * FUNCTION
 *  BTBMACLLinkAcceptMode
 * DESCRIPTION
 *  The MMI will set the bt stack can accept connection automatically or not.
 * PARAMETERS
 *  mode        [IN]        The MMI to BT SAP defined auto accept value or not.
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMACLLinkAcceptMode(U8 mode)
{
    MTKBMC(wait_conection_acp_rsp) = mode;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMACLLinkAcceptRsp
 * DESCRIPTION
 *  After the connect ind event to BTstack, the btstack will send a confirm message to MMI, MMI shall
 *  response the accept or not for that connection.
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMACLLinkAcceptRsp(bt_bm_link_connect_accept_rsp_struct *btbm_adp_input_msg)
{
    btbm_bd_addr_t *remoteaddr;
    BD_ADDR bdaddr;
    U8 piconet_no, scatternet_no, sco_no, esco_no, master_sco, slave_sco;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, remoteaddr);
    BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &master_sco, &slave_sco);
    bt_trace(TRACE_GROUP_1, BTLOG_MASTER_SCOxD , master_sco);
    if (MTKBMC(wait_conection_acp_rsp) != BM_LINK_ACCEPT_CMD_AUTO)
    {
        MTKBMC(wait_conection_acp_rsp) = BM_LINK_ACCEPT_CMD_NONE;
    }
    if ((MTKBMC(connect_acp_link) != 0) && (StrnCmp(MTKBMC(connect_acp_link)->bdAddr.addr, 6, bdaddr.addr, 6)))
    {
        if (btbm_adp_input_msg->result == BTBM_ADP_SUCCESS)
        {
//            if (esco_no != 0)
//            {
//                ME_RejectIncomingLink(MTKBMC(connect_acp_link), BEC_LIMITED_RESOURCE);
//            }
//            else if (master_sco == TRUE)
            if (master_sco > 0)
            {
                ME_AcceptIncomingLink(MTKBMC(connect_acp_link), BCR_MASTER);
            }
            else
            {
                ME_AcceptIncomingLink(MTKBMC(connect_acp_link), BCR_SLAVE);
            }
        }
        else
        {
            ME_RejectIncomingLink(MTKBMC(connect_acp_link), BEC_LIMITED_RESOURCE);
        }
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCONNECTION_IS_NOT_EXISTING_OR_BD_ADDR_IS_NOT_THE_SAME);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMCheckSpecialSlaveProcedure
 * DESCRIPTION
 *  Do special procedure for BT chip.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
BOOL BTBMCheckSpecialSlaveProcedure(void)
{
    /* Check all state to be slave or not */
    BtDeviceContext *bdc = 0;
    BOOL status = FALSE;

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBMSPECIALSLAVEPROCEDURE);
    bt_trace(TRACE_GROUP_1, HID_ROLE_STATEBT_DEBUGx02X, MTKBMC(hid_broadcom_iot_role_switch));
    /* Clear "In Range" status for all devices. */
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
    {
        BTBMDumpBdAddr(bdc->addr.addr);
        if (bdc->link != 0)
        {
            if (bdc->link->state == BDS_CONNECTED)
            {
                if ((bdc->link->role == BCR_SLAVE) && (bdc->link->mode == BLM_ACTIVE_MODE))
                {
                    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTDEVICE_IS_SLAVE_AND_CONNECTED);
                    if (bdc->link->roleSwitchCounter <= 0x02)
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTDO_ROLE_SWITCH);
                        if( BTBMDoRoleSwitch(bdc->link) != BT_STATUS_PENDING)
                            status = FALSE;
                        else
                            status = TRUE;
                    }
                    /*
                    else if (bdc->link->sniffCounter < 0x02)
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTDO_SNIFFER_REQUEST);
                        if (bdc->link->sniffCounter < 0xFF)
                        {
                            bdc->link->sniffCounter++;
                        }
                        ConnMgrSnifferTimeOutImmediately(bdc->link);
                        status = TRUE;
                    }
                    */
                    else
                    {
                        bdc->link->roleSwitchCounter = 0x00;
                        bdc->link->sniffCounter = 0x00;
                        status = FALSE;
                    }
                }
            }
            else if (bdc->link->state == BDS_IN_CON)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSTILL_WAIT_FOR_CONNECTION_COMPLETE_EVENT);
                status = TRUE;
            }
        }
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGetDeviceModel
 * DESCRIPTION
 *  The function is used to get remote device model (Limited listed device, not all devices)
 * PARAMETERS
 *  addr        [IN]        Remote device address
 * RETURNS
 *  Device Model enum
 *****************************************************************************/
DEVICE_MODEL BTBMGetDeviceModel(BD_ADDR *addr)
{
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &MTKBMC(DevRemNameTbl)[i];
        if ((entry->link != 0) && (OS_MemCmp(entry->link->bdAddr.addr, 6, addr->addr, 6)))
        {
            if (StrnCmp((const U8*)MOTO_HT820, OS_StrLen((char*)MOTO_HT820), (U8*) entry->dev_name, entry->dev_name_len))
            {
                return DEVICE_MOTO_HT820;
            }
            else if (StrnCmp((const U8*)HP_XXX, OS_StrLen((char*)HP_XXX), (U8*) entry->dev_name, entry->dev_name_len))
            {
                return DEVICE_HP;
            }
            else if (StrnCmp((const U8*)WIREVO_HSH200, OS_StrLen((char*)WIREVO_HSH200), (U8*) entry->dev_name, entry->dev_name_len))
            {
                return DEVICE_WIREVO_HSH200;
            }
            else if (StrnCmp((const U8*)NOKIA_BH_100, OS_StrLen((char*)NOKIA_BH_100), (U8*) entry->dev_name, entry->dev_name_len)) /* NOKIA_BH_100 */
            {
                return DEVICE_NOKIA_BH100;
            }
            else if (StrnCmp((const U8*)JABRA_BT3030, OS_StrLen((char*)JABRA_BT3030), (U8*) entry->dev_name, entry->dev_name_len)) 
            {
                return DEVICE_JABRA_BT3030;
            }
            else if (StrnCmp((const U8*)HBH_DS980, OS_StrLen((char*)HBH_DS980), (U8*) entry->dev_name, entry->dev_name_len)) 
            {
                return DEVICE_HBH_DS980;
            }
            else if (StrnCmp((const U8*)PCM, OS_StrLen((char*)PCM), (U8*) entry->dev_name, entry->dev_name_len)) 
            {
                return DEVICE_PCM;
            }
            
        }
    }
    return GENERAL_DEVICE;

}

DEVICE_MODEL BTBMGetDeviceModelByVersion(BtRemoteDevice *remDev){
    U32 count = sizeof(s_devVerModel)/sizeof(s_devVerModel[0]);
    U32 i = 0;
    
    if(remDev && !OS_MemCmp(s_nullVer, 5, remDev->remoteVersion, 5)){
        for(i = 0;i < count;i++){
            if(OS_MemCmp(s_devVerModel[i].ver, 5, remDev->remoteVersion, 5)){
                return s_devVerModel[i].model;
            }
        }
    }
    return GENERAL_DEVICE;
}

DEVICE_MODEL BTBMGetDeviceModelByAddr(BD_ADDR *addr)
{
    U32 count = sizeof(s_devAddrModel)/sizeof(s_devAddrModel[0]);
    U32 i = 0;
    
    if (addr)
    {
        for (i = 0; i < count; i++)
        {
            if (OS_MemCmp(s_devAddrModel[i].id, 3, addr->addr + 3, 3))
            {
                return s_devAddrModel[i].model;
            }
        }
    }
    return GENERAL_DEVICE;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMReturnScatternetState
 * DESCRIPTION
 *  
 * PARAMETERS
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReturnScatternetState(void)
{
    BMCallbackParms parms;
    U8 piconet = 0, scatternet = 0, sco = 0, esco = 0, msco = 0, ssco = 0;
    
    BTBMGetScattnetStatus(&piconet, &scatternet, &sco, &esco, &msco, &ssco);
    parms.event = BTBM_EVENT_REPORT_SCATTERNET_STATUS;
    parms.p.ReportScatternetStatus.piconet = piconet;
    parms.p.ReportScatternetStatus.scatternet = scatternet;
    parms.p.ReportScatternetStatus.sco = sco;
    parms.p.ReportScatternetStatus.esco = esco;
    MTKBMC(bmCB)(&parms);
}

/*****************************************************************************
 * FUNCTION
 *  BTBMReturnScatternetState
 * DESCRIPTION
 *  
 * PARAMETERS
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGetLinkKey(btbm_bd_addr_t *bd_addr)
{
    BMCallbackParms parms;
    BD_ADDR addr;
    BtDeviceRecord record;
    BtCodRecord cod;
    BtDeviceContext *bdc;

    parms.event = BTBM_EVENT_GET_LINK_KEY;
    parms.p.ReportLinkKeyResult.result = BTBM_ADP_SUCCESS;
    CONVERT_BDADDRSRC2ARRAY(addr.addr, bd_addr);
    if( DDB_FindRecord(&addr, &record) == BT_STATUS_SUCCESS ){
        if(DDB_COD_FindRecord(&addr, &cod) !=BT_STATUS_SUCCESS){
            //parms.p.ReportLinkKeyResult.result = BTBM_ADP_FAILED;
            bt_prompt_trace(MOD_BT, "[BTBM] BTBMGetLinkKey : Can not find cod");
        }
    }else 
    {
        parms.p.ReportLinkKeyResult.result = BTBM_ADP_FAILED;
        bt_prompt_trace(MOD_BT, "[BTBM] BTBMGetLinkKey : Can not find link key");
    }
    if(parms.p.ReportLinkKeyResult.result == BTBM_ADP_SUCCESS){
        bt_prompt_trace(MOD_BT, "[BTBM] BTBMGetLinkKey : find link key");
        parms.p.ReportLinkKeyResult.key_type = record.keyType;
        parms.p.ReportLinkKeyResult.bd_addr = *bd_addr;
        parms.p.ReportLinkKeyResult.cod = cod.cod;
        parms.p.ReportLinkKeyResult.key = record.linkKey;
        #ifdef __BT_4_0_BLE__
        parms.p.ReportLinkKeyResult.device_type = record.devType;
        #else
        parms.p.ReportLinkKeyResult.device_type = BT_DEV_TYPE_BR_EDR;
        #endif
        bdc = DS_FindDevice(&addr);
        if( bdc && bdc->link && 
            (bdc->link->state >= BDS_CONNECTED) &&
            bdc->link->pairingType == BPT_NOT_SAVED){
            parms.p.ReportLinkKeyResult.persist = 0;
        }else{
            parms.p.ReportLinkKeyResult.persist = 1;
        }
    }
    if(MTKBMC(bmCB)) MTKBMC(bmCB)(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMReturnLinkState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReturnLinkState(bt_bm_get_link_state_req_struct *ptr)
{
    BtRemoteDevice *link;
    BMCallbackParms parms;
    U8 status = BTBM_ADP_FAILED;
    U8 *bd_addr;
    BOOL is_master = FALSE;
    BOOL is_authenticated = FALSE;
    BOOL is_encrypted = FALSE;
    U8 i=0;

    
    bd_addr = (U8*) & ptr->bdaddr;
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if (link->state == BDS_CONNECTED)
        {
            if (OS_MemCmp(bd_addr, 6, link->bdAddr.addr, 6))
            {
                status = BTBM_ADP_SUCCESS;
                if (link->role == BCR_MASTER)
                {
                    is_master = TRUE;
                }
                if (link->authState == BAS_AUTHENTICATED)
                {
                    is_authenticated = TRUE;
                }
                if (link->encryptState == BES_ENCRYPTED)
                {
                    is_encrypted = TRUE;
                }
                break;
            }
        }
    }
    parms.event = BTBM_EVENT_REPORT_NETWORK_STATUS;
    parms.p.ReportNetworkStatus.result = status;
    parms.p.ReportNetworkStatus.is_master = is_master;
    parms.p.ReportNetworkStatus.is_authenticated = is_authenticated;
    parms.p.ReportNetworkStatus.is_encrypted = is_encrypted;    
    MTKBMC(bmCB)(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMCheckAclConnectRequestNotAddPendingCon
 * DESCRIPTION
 *  To check if there is any pending con waiting for MMI's accept.
 * PARAMETERS
 *  void
 * RETURNS
 *  TRUE if there are more than one pending connection waiting for accept. FALSE, no connection is waiting.
 *****************************************************************************/
BOOL BTBMCheckAclConnectRequestNotAddPendingCon(void)
{
    bt_trace(TRACE_GROUP_1, WAIT_CONECTION_ACP_RSPBT_DEBUGx02X, MTKBMC(wait_conection_acp_rsp));
    if (MTKBMC(wait_conection_acp_rsp) == BM_LINK_ACCEPT_CMD_WAIT)
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
 *  BTBMCheckBMStatusNotExistSnifferMode
 * DESCRIPTION
 *  Check no device in outgoing connection and inquiry procedure.
 * PARAMETERS
 *  void
 * RETURNS
 *  TRUE for in inquiry procedure or device in outgoing connection mode
 *****************************************************************************/
BOOL BTBMCheckBMStatusNotExistSnifferMode(void)
{
    U8 i = 0;
    BOOL status = FALSE;

    #ifndef __INDEPENDENT_INQUIRY_FLOW__
    if (MTKBMC(bm_opactivity) == BM_OP_INQUIRY)
    {
        status = TRUE;
    }
    else
    #endif
    {
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if ((MEC(devTable)[i].state == BDS_OUT_CON) || (MEC(devTable)[i].state == BDS_OUT_CON2))
            {
                status = TRUE;
                break;
            }
        }
    }
    return status;
}


#if BT_READ_RSSI == XA_ENABLED
/*****************************************************************************
 * FUNCTION
 *  BTBMReadRSSI
 * DESCRIPTION
 *  
 * PARAMETERS
 *  uap       [IN]
 *  nap       [IN]
 *  lap        [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus BTBMReadRSSI(U32 lap, U8 uap, U16 nap)
{
    U8 device_bt_addr[6];
    BtRemoteDevice* remDev = NULL;
    BtStatus status = BT_STATUS_FAILED;
    memset(device_bt_addr,0,6);
    CONVERT_BDADDR2ARRAY(device_bt_addr, lap, uap, nap);

    if (MTKBMC(rssi_op_ongoing) == TRUE)
    {
        return status;
    }
    
    remDev = ME_FindRemoteDeviceP(device_bt_addr);
    if (remDev != NULL)
    {
        status = ME_ReadRSSI(remDev);
        if (status == BT_STATUS_PENDING)
        {
            MTKBMC(rssi_op_ongoing) = TRUE;
        }
        bt_trace(TRACE_GROUP_1,BTLOG_BTBM_READRSSI_STATUS,status);
    }
    else
    {
        bt_trace(TRACE_GROUP_1,BTLOG_BTBM_READRSSI_NOT_FIND_REMDEV_WITH_ADDR);
    }
    return status;
}
#endif /* BT_READ_RSSI == XA_ENABLED */


#ifdef __BQE_TESTER_


/*****************************************************************************
 * FUNCTION
 *  BTBMGapLinkCreate
 * DESCRIPTION
 *  The function is used to GAP, SDP  testing to create ACL link
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapLinkCreate(bt_bm_acl_link_req_struct *btbm_adp_input_msg)
{
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t *remoteaddr;
    BtStatus status;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);

    CONVERT_BDADDRSRC2ARRAY(MTKBMC(remote_addr).addr, remoteaddr);

    MTKBMC(bm_opactivity) = BM_OP_NONE;
    if ((bdc = DS_FindDevice(&(MTKBMC(remote_addr)))) == 0)
    {
        status = BtbmAllocateFreeDevices((MTKBMC(remote_addr)), &bdc);
        if(status == BT_STATUS_SUCCESS){
            bdc->under_used |= BDS_CONNECTING;
            Report(("bdc->under_used=0x%x", bdc->under_used));
        }else{
            OS_Report("[BM][ERR] BTBMGapLinkCreate : BtbmAllocateFreeDevices failed");
            Assert(0);
            return;
        }
    }
    if (bdc->link == 0)
    {
        bt_trace(TRACE_GROUP_1, BTBM_CREATE_CONNECTION, 0x04);
        ME_CreateLink(&linkbmHandler, &(MTKBMC(remote_addr)), &(bdc->psi), &(bdc->link));
        btbmRemoveLinkCallback(bdc->link);
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapLinkClose
 * DESCRIPTION
 *  The function is used to GAP, SDP  testing to close ACL link
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapLinkClose(bt_bm_acl_link_close_struct *btbm_adp_input_msg)
{
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t *remoteaddr;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(MTKBMC(remote_addr).addr, remoteaddr);

    MTKBMC(bm_opactivity) = BM_OP_NONE;
    if ((bdc = DS_FindDevice(&(MTKBMC(remote_addr)))) == 0)
    {
        btbmAddLinkCallback(bdc->link);
        bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x01);
        ME_DisconnectLink(&linkbmHandler, bdc->link);
        btbmRemoveLinkCallback(bdc->link);
    }
    return;
}

#endif /* __BQE_TESTER_ */ 


/*****************************************************************************
 * FUNCTION
 *  btbmAddLinkCallback
 * DESCRIPTION
 *  for Btstack internal used. To register the callback function of that link
 * PARAMETERS
 *  link        [IN]        The callback function register to this link
 * RETURNS
 *  void
 *****************************************************************************/
void btbmAddLinkCallback(BtRemoteDevice *link)
{
    bt_trace(TRACE_GROUP_1, BTBM_ADD_LINK_CALLBACK);
    if (!IsNodeOnList(&(link->handlers), &(linkbmHandler.node)))
    {
        InsertTailList(&(link->handlers), &(linkbmHandler.node));
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmRemoveLinkCallback
 * DESCRIPTION
 *  for Btstack internal used. To deregister the callback function of that link
 * PARAMETERS
 *  link        [IN]        The callback function register to this link
 * RETURNS
 *  void
 *****************************************************************************/
void btbmRemoveLinkCallback(BtRemoteDevice *link)
{
    bt_trace(TRACE_GROUP_1, BTBM_REMOVE_LINK_CALLBACK);

    if (link == NULL)
    {
        #ifdef __LINUX_SUPPRESS_ERROR__
        #else
        bt_trace(TRACE_GROUP_1, BTBM_REMOVE_LINK_CALLBACK_LINK_NOT_EXISTED);
        #endif
        return;
    }
    if (IsNodeOnList(&(link->handlers), &(linkbmHandler.node)))
    {
        RemoveEntryList(&(linkbmHandler.node));
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmStartSpecialSlaveProcedure
 * DESCRIPTION
 *  Special procedure for reset the role switch count and sniff count for BT chip.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmStartSpecialSlaveProcedure(void)
{
    /* Check all state to be slave or not */
    BtDeviceContext *bdc = 0;

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBMSTARTSPECIALSLAVEPROCEDURE);

    /* Clear "In Range" status for all devices. */
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
    {
        BTBMDumpBdAddr(bdc->addr.addr);
        if (bdc->link != 0)
        {
            bdc->link->roleSwitchCounter = 0;
            bdc->link->sniffCounter = 0;
        }
    }
    if(!HCI_IsValidScoPacketLen())
    {
        MTKBMC(scoData) = 0xff;
    }    
}


/*****************************************************************************
 * FUNCTION
 *  BtbmAllocateFreeDevices
 * DESCRIPTION
 *  The function to use to allocate a new BtDeviceContext in Database
 * PARAMETERS
 *  bdAddr      [IN]        The Bluetooth address of the BtDeviceContext
 * RETURNS
 *  void
 *****************************************************************************/
BtStatus BtbmAllocateFreeDevices(BD_ADDR bdAddr, BtDeviceContext **NewDevice)
{
    BtStatus status;
    BtDeviceContext device;

    OS_MemSet((U8*) & device, 0, (U32) sizeof(BtDeviceContext));
    device.addr = bdAddr;
    device.classOfDevice = 0x00;
    status = DS_AddDevice(&device, NewDevice);
    if (status != BT_STATUS_SUCCESS){
        OS_Report("[BM][ERR] BtbmAllocateFreeDevices Failed : status = %d", status);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  btbmPutLinkAsMaster
 * DESCRIPTION
 *  The function is used for inquiry procedure to force all ACL link as one piconet. We are the master state.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmPutLinkAsMaster(void)
{
    U8 i=0;
    BtRemoteDevice *link;
    
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_LINK_AS_MASTER);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if ((link->state == BDS_CONNECTED) || (link->state == BDS_IN_CON))
        {
            if (link->role == BCR_SLAVE)
            {
                BTBMDoRoleSwitch(link);
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerLinkCreateCancel
 * DESCRIPTION
 *  To handle the Create Connection Cancel Completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerLinkCreateCancel(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtDeviceContext *list_bdc = 0;
    BtRemoteDevice *remDev = 0;

    remDev = Event->p.remDev;
    bdc = DS_FindDevice(&(remDev->bdAddr));
    if (Event->errCode != BEC_ACL_ALREADY_EXISTS)
    {
        bdc->link = 0x00;
        if (MTKBMC(power_on_off_mode) != BM_POWER_OFF)
        {
            while (DS_EnumDeviceList(&list_bdc) == BT_STATUS_SUCCESS)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_DISCONNECT_LINK);
                BTBMDumpBdAddr(list_bdc->addr.addr);
                if (list_bdc->link != 0)
                {
                    return;
                }
            }
            btbmHandlerCheckAllLinkPowerOffStatus();
        }
    }
    else
    {
        btbmAddLinkCallback(bdc->link);
        bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x0a);
        ME_DisconnectLink(&linkbmHandler, bdc->link);
        btbmRemoveLinkCallback(bdc->link);
        return;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerConnectInd
 * DESCRIPTION
 *  To handle the connect indication event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerConnectInd(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev = 0;
    BtDeviceRecord record;

    BTBMRemoteNameSetting_T *entry;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_EVENT_CONNECT_IND);

    remDev = Event->p.remDev;
    bdc = DS_FindDevice(&(remDev->bdAddr));
    if (bdc == NULL){ 
        /* DeviceContextmight be cleared due to user starting discovery or ME accept / reject 
            directly so no DeviceContext created.
        */
        OS_Report("[SH] Device Context is NULL");
        if( BT_STATUS_SUCCESS != BtbmAllocateFreeDevices(remDev->bdAddr, &bdc)){
            OS_Report("[BM][ERR] btbmHandlerConnectInd : BtbmAllocateFreeDevices failed");
            Assert(0);
            return;
        }
    }
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
    return;
  #else
    switch(ME_GetConnType(remDev)){
    case BT_CONN_TYPE_LE:
        bdc->devType = BT_DEV_TYPE_LE;
        break;
    case BT_CONN_TYPE_BR_EDR:
        bdc->devType = BT_DEV_TYPE_BR_EDR;
        break;
    default:
        OS_Report("btbmHandlerConnectCfm conn type unknown");
        break;
    }
    if (bdc->devType == BT_DEV_TYPE_LE)
    {
        return;
    }
  #endif
#endif
    if(!HCI_IsValidScoPacketLen())
    {
        MTKBMC(scoData) = 0xff;
    }
    if (Event->errCode == BEC_NO_ERROR)
    {
        bdc->link = remDev;
        entry = btbmUtilConnectedDeviceNameEntryAllocate(remDev);
        if (entry != 0)
        {
            entry->link = remDev;
        }

        switch (MTKBMC(bm_opactivity))
        {
            case BM_OP_BONDING:
                /* ALPS01043861 : Remote initiate connection, but bonding req from MMI received before 
                connection complete event received. Call btbmUtilCheckBondingState to continue bonding. */
                if (bdc == MTKBMC(pin_bdc))
                {
                    btbmUtilCheckBondingState(BM_BONDING_ACL_CREATED, Event->errCode);
                }
                break;        
            case BM_OP_PAIRING:
                if (bdc == MTKBMC(pin_bdc))
                {
                    btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_ACL_CREATED, Event->errCode);
                }
                break;
            default:
                if (MTKBMC(bm_security_mode) == BSM_SEC_LEVEL_3)
                {
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_EVENT_CONNECT_IND_LEVEL3);
                    if (SEC_FindDeviceRecord(&(remDev->bdAddr), &record) != BT_STATUS_SUCCESS)
                    {
                        btbmDisconnectAclLinkWithoutForce(bdc->link, BEC_USER_TERMINATED, 2);
                    }
                }
                /*
                 * check for remote device's name
                 * Remote Name Request will be issued here
                 * if one entry for this remote device is available, the name query is started
                 */
            #ifndef MTK_BB_LOOPBACK_MODE
                if (entry != 0)
                {
                    btbmRemoteNameRequestAtNoAuthEnabled(bdc);
                }
            #endif /* MTK_BB_LOOPBACK_MODE */ 
                /* MTKBMC(bm_opactivity) = BM_OP_NONE; */
                break;
        }
    #if defined (__BT_2_1_BLOCK_DEVICE__)
        if (BTBMCheckBlockList(bdc->link->bdAddr.addr) == TRUE)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_IN_BLOCK_LIST_DISCONNECT);
            btbmHandleForceDisconnectLink(&(bdc->link->bdAddr));
        }
    #endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_CONNECT_FAILED);
        switch (MTKBMC(bm_opactivity))
        {
            case BM_OP_PAIRING:
                if (bdc == MTKBMC(pin_bdc))
                {
                    btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_FAILED_COMPLETED, Event->errCode);
                    MTKBMC(bm_opactivity) = BM_OP_NONE;
                    bdc->under_used &= ~BDS_BONDING;
                    bdc->under_used &= ~BDS_CONNECTING;
                    Report(("bdc->under_used=0x%x", bdc->under_used));
                }
                break;
            default:
                break;
        }
    }
    if (MTKBMC(power_on_off_mode) != BM_POWER_ON)
    {
        btbmHandlerCheckAllLinkPowerOffStatus();
    }

}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerConnectCfm
 * DESCRIPTION
 *  To handle the connect confirm event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerConnectCfm(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev = 0;

    BTBMRemoteNameSetting_T *entry;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_EVENT_CONNECT_CNF);
    remDev = Event->p.remDev;
    bdc = DS_FindDevice(&(remDev->bdAddr));

#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
    return;
  #else
    /* Set the dev type according to the connection type */
    switch(ME_GetConnType(remDev)){
    case BT_CONN_TYPE_LE:
        bdc->devType = BT_DEV_TYPE_LE;
        break;
    case BT_CONN_TYPE_BR_EDR:
        bdc->devType = BT_DEV_TYPE_BR_EDR;
        break;
    default:
        OS_Report("btbmHandlerConnectCfm conn type unknown");
        break;
    }
    if (bdc->devType == BT_DEV_TYPE_LE)
    {
        return;
    }
  #endif
#endif

    if (Event->errCode == BEC_NO_ERROR)
    {
        bdc->link = remDev;
        entry = btbmUtilConnectedDeviceNameEntryAllocate(remDev);
        if (entry != 0)
        {
            entry->link = remDev;
        }

        switch (MTKBMC(bm_opactivity))
        {
            case BM_OP_BONDING:
                if (bdc == MTKBMC(pin_bdc))
                {
                    btbmUtilCheckBondingState(BM_BONDING_ACL_CREATED, Event->errCode);
                }
                break;
            case BM_OP_PAIRING:
                if (bdc == MTKBMC(pin_bdc))
                {
                    btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_ACL_CREATED, Event->errCode);
                }
                break;
            default:
                /* MTKBMC(bm_opactivity) = BM_OP_NONE; */
                if (MTKBMC(bonding_state) == BM_BONDING_CANCELING)
                {
                    bt_trace(TRACE_GROUP_1, BTBM_REMOVE_CONNECTION, 0x02);  /* bonding cancelled */
                    MTKBMC(bonding_state) = BM_BONDING_NONE;
                    btbmHandleForceDisconnectLink(&(remDev->bdAddr));
                }
                break;
        }
    #if defined (__BT_2_1_BLOCK_DEVICE__)
        if (BTBMCheckBlockList(remDev->bdAddr.addr) == TRUE)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_IN_BLOCK_LIST_DISCONNECT);
            btbmHandleForceDisconnectLink(&(remDev->bdAddr));
        }
    #endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 
        if (entry != 0)
        {
            btbmRemoteNameRequestAtNoAuthEnabled(bdc);
        }

    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_CONNECT_FAILED);
        switch (MTKBMC(bm_opactivity))
        {
            case BM_OP_BONDING:
                if (bdc == MTKBMC(pin_bdc))
                {
                    bdc->link = 0;
                    btbmUtilCheckBondingState(BM_BONDING_FAILED_COMPLETED, Event->errCode);
                    MTKBMC(bm_opactivity) = BM_OP_NONE;
                    bdc->under_used &= ~BDS_BONDING;
                    bdc->under_used &= ~BDS_CONNECTING;
                    Report(("bdc->under_used=0x%x", bdc->under_used));
                }
                break;
            case BM_OP_PAIRING:
                if (bdc == MTKBMC(pin_bdc))
                {
                    bdc->link = 0;
                    btbmUtilCheckPairingState(&remDev->bdAddr, BM_BONDING_FAILED_COMPLETED, Event->errCode);
                    bdc->under_used &= ~BDS_BONDING;
                    bdc->under_used &= ~BDS_CONNECTING;
                    Report(("bdc->under_used=0x%x", bdc->under_used));
                }
                break;
            default:
                if (MTKBMC(bonding_state) == BM_BONDING_CANCELING)
                {
                    MTKBMC(bonding_state) = BM_BONDING_NONE;
                }
                break;
        }
    }
    if (MTKBMC(power_on_off_mode) != BM_POWER_ON)
    {
        btbmHandlerCheckAllLinkPowerOffStatus();
    }

}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerDisconnectInd
 * DESCRIPTION
 *  To handle the Disconnect ind event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerDisconnectInd(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev = 0;

    remDev = Event->p.remDev;
    bdc = DS_FindDevice(&(remDev->bdAddr));
    btbmUtilConnectedDeviceNameEntryFree(&(remDev->bdAddr));
    if (bdc == MTKBMC(pin_bdc))
    {
        switch (MTKBMC(bm_opactivity))
        {
            case BM_OP_BONDING:
                if(MTKBMC(bonding_state) & BM_BONDING_ACL_CREATED){
                    btbmUtilCheckBondingState(BM_BONDING_FAILED_COMPLETED, Event->errCode);
                }else{
                    OS_Report("btbmHandlerDisconnectInd : receive disconnected ind before ACL created flag set");
                }
                break;
            case BM_OP_PAIRING:
                btbmUtilCheckPairingState(&bdc->addr, BM_BONDING_FAILED_COMPLETED, Event->errCode);
                break;
        }
    }
    bdc->under_used &= ~BDS_BONDING;
    bdc->under_used &= ~BDS_CONNECTING;
    Report(("bdc->under_used=0x%x", bdc->under_used));
    bdc->link = 0;

    if (MTKBMC(power_on_off_mode) != BM_POWER_ON)
    {
        btbmHandlerCheckAllLinkPowerOffStatus();
    }

}

/*****************************************************************************
 * FUNCTION
 *  btbmHandleModeCfm
 * DESCRIPTION
 *  To handle the enter/exit XXX mode completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleModeCfm(const BtEvent *Event)
{
    BtRemoteDevice *link;
    BtLinkMode mode;
    BtStatus status;

    link = Event->p.modeChange.remDev;
    mode = Event->p.modeChange.curMode;
    if (link == NULL)
    {
        return;
    }
    OS_Report("btbmHandleModeCfm switchState=0x%02x, curMode=0x%02x", link->switchState, mode);
    BTBMDumpBdAddr(link->bdAddr.addr);
    if (link->switchState != BM_ROLE_SWITCH_EXIT_SNIFF)
    {
        return;
    }
    if (mode == BLM_ACTIVE_MODE)   
    {                 
        if (link->role == BCR_SLAVE)
        {
            if (link->encryptState != BES_ENCRYPTED || (link->remoteFeature[5] & 0x04))
            {
                link->switchState = BM_ROLE_SWITCH_NONE;               
                if ((MTKBMC(bm_opactivity) == BM_OP_BONDING || MTKBMC(bm_opactivity) == BM_OP_PAIRING) && (MTKBMC(pin_bdc) != 0) && (MTKBMC(pin_bdc)->link == link))
                {
                    bt_trace(TRACE_GROUP_1, DURING_PAIRING_NOT_ALLOW_ROLE_SWITCH);
                    return;
                }
                else
                {
                    link->oldEncryptionMode = link->encryptState;
                    status = ME_SwitchRole(link);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_ROLE_SWITCH_STATUSBT_DEBUGx02X, status);                    
                }
            }
            else
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLINKENCRYPTSTATE_OLD_1BT_DEBUGx02X, link->encryptState);
                link->oldEncryptionMode = BES_ENCRYPTED;
                status = SEC_SetLinkEncryption(0, link, BECM_ENCRYPT_DISABLE);
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_SEC_SETLINKENCRYPTION_STATUSBT_DEBUGx02X, status);   
                if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING))
                {
                    link->switchState = BM_ROLE_SWITCH_STOP_ENCRYPTION;
                }
                else
                {
                    link->switchState = BM_ROLE_SWITCH_NONE;
                }
            }         
        }
        else
        {
            bt_trace(TRACE_GROUP_1, ALREADY_MASTER);
            link->switchState = BM_ROLE_SWITCH_NONE;
        }
    } 
    else 
    {
        link->switchState = BM_ROLE_SWITCH_NONE;
    }
}

/*****************************************************************************
 * FUNCTION
 *  btbmHandleEncryptionCfm
 * DESCRIPTION
 *  To handle the set encryption mode completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleEncryptionCfm(const BtEvent *Event)
{
    BtRemoteDevice *link;
    U8 mode;
    BtStatus status;

#ifdef __BT_4_0_BLE__
    if (ME_GetDeviceType(&(Event->p.encrypt.remDev->bdAddr)) == BT_DEV_TYPE_LE)
    {
        return;
    }
  #ifdef __BT_LE_STANDALONE__
    return;
  #endif
#endif
    link = Event->p.encrypt.remDev;
    mode = Event->p.encrypt.mode;
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBMHANDLEENCRYPTIONCFM_SWITCHSTATE_BT_DEBUGx02X, link->switchState);
    BTBMDumpBdAddr(link->bdAddr.addr);
    if(!HCI_IsValidScoPacketLen())
    {
        MTKBMC(scoData) = 0xff;
    }
    if (mode == 0x00)   /* Encryption mode OFF */
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_ENCRYPTION_CHANGE_TO_OFF);
        if (link->switchState == BM_ROLE_SWITCH_STOP_ENCRYPTION)
        {
            link->switchState = BM_ROLE_SWITCH_ROLE_SWITCH;
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_ROLE_SWITCH);
            if (link->role == BCR_SLAVE)
            {
                status = ME_SwitchRole(link);
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_ROLE_SWITCH_STATUSBT_DEBUGx02X, status);
                if (status == BT_STATUS_PENDING)
                {
                    link->switchState = BM_ROLE_SWITCH_ROLE_SWITCH;
                }
                else
                {
                    link->switchState = BM_ROLE_SWITCH_NONE;
                }
            }
            else
            {
                bt_trace(TRACE_GROUP_1, ALREADY_MASTER);
                if (link->oldEncryptionMode == BES_ENCRYPTED)
                {
                    status = SEC_SetLinkEncryption(0, link, BECM_ENCRYPT_ENABLE);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_SEC_SETLINKENCRYPTION_STATUSBT_DEBUGx02X, status);
                    if (status == BT_STATUS_PENDING)
                    {
                        link->switchState = BM_ROLE_SWITCH_START_ENCRYPTION;
                    }
                    else
                    {
                        link->switchState = BM_ROLE_SWITCH_NONE;
                    }
                }
            }
        }
    }
    else    /* Encryption mode ON */
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_ENCRYPTION_CHANGE_TO_ON);
        link->switchState = BM_ROLE_SWITCH_NONE;
        MeWriteSupervisionTimeout(link);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleRoleSwitchCfm
 * DESCRIPTION
 *  To handle the Mode change event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleRoleSwitchCfm(const BtEvent *Event)
{
    BtRemoteDevice *link;
    U8 role;
    BtStatus status;

    link = Event->p.roleChange.remDev;
    role = Event->p.roleChange.newRole;
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBM_HANDLEROLESWITCHCFM_SWITCHSTATE, link->switchState, role, link->hciHandle);
    BTBMDumpBdAddr(link->bdAddr.addr);
    
    if (role == BCR_MASTER) /* Encryption mode OFF */
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_MODE_AS_MASTER);
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLINKSWITCHSTATEBT_DEBUGx02X, link->switchState);

        if (link->switchState == BM_ROLE_SWITCH_ROLE_SWITCH)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLINKOLDENCRYPTIONMODE_2BT_DEBUGx02X, link->oldEncryptionMode);
            if (link->oldEncryptionMode == BES_ENCRYPTED)
            {
                status = SEC_SetLinkEncryption(0, link, BECM_ENCRYPT_ENABLE);
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_SEC_SETLINKENCRYPTION_STATUSBT_DEBUGx02X, status);
                if (status == BT_STATUS_PENDING)
                {
                    link->switchState = BM_ROLE_SWITCH_START_ENCRYPTION;
                }
                else
                {
                    link->switchState = BM_ROLE_SWITCH_NONE;
                }
            }
            else
            {
                MeWriteSupervisionTimeout(link);
            }
        }
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_MODE_AS_SLAVE);
        if (link->switchState == BM_ROLE_SWITCH_ROLE_SWITCH)
        {
            if (link->oldEncryptionMode == BES_ENCRYPTED)
            {
                status = SEC_SetLinkEncryption(0, link, BECM_ENCRYPT_ENABLE);
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBM_DO_SEC_SETLINKENCRYPTION_STATUSBT_DEBUGx02X, status);
                if (status == BT_STATUS_PENDING)
                {
                    link->switchState = BM_ROLE_SWITCH_START_ENCRYPTION;
                }
                else
                {
                    link->switchState = BM_ROLE_SWITCH_NONE;
                }
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerAuthenticatedCfm
 * DESCRIPTION
 *  To handle the Authentication link completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerAuthenticatedCfm(const BtEvent *Event)
{
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev = 0;

    bdc = DS_FindDevice(&(Event->p.remDev->bdAddr));
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_EVENT_AUTHENTICATION_CNF);
    bt_trace(TRACE_GROUP_1, BTLOG_MTKBMCPIN_BDCx08X, MTKBMC(pin_bdc));
    bt_trace(TRACE_GROUP_1, BTLOG_EVENTERRCODEx02X, Event->errCode);
    bt_trace(TRACE_GROUP_1, BTLOG_BDCx08X, bdc);
    bt_trace(TRACE_GROUP_1, BTLOG_MTKBMCBM_OPACTIVITYx02X, MTKBMC(bm_opactivity));
    if (Event->errCode != BEC_NO_ERROR)
    {    
        BtStatus status = BT_STATUS_FAILED;    
        
        status = SEC_DeleteDeviceRecord(&(Event->p.remDev->bdAddr));
        bt_trace(TRACE_GROUP_1, BT_LOG_BTBM_AUTH_CFM_DEL_DEV_RECORD_STATUS, status);
    }
    
    if ((MTKBMC(pin_bdc) == bdc) && (Event->errCode != BEC_NO_ERROR) && (MTKBMC(bonding_state) != BM_BONDING_CANCELED))
    {
        if (MTKBMC(bm_opactivity) == BM_OP_BONDING)
        {
            btbmUtilCheckBondingState(BM_BONDING_FAILED_COMPLETED, Event->errCode);
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
            if (MTKBMC(pin_bdc) != 0)
            {
                MTKBMC(pin_bdc)->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
            }
        #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        }
        else if (MTKBMC(bm_opactivity) == BM_OP_PAIRING)
        {
            remDev = Event->p.remDev;
            btbmUtilCheckPairingState(&(remDev->bdAddr), BM_BONDING_FAILED_COMPLETED, Event->errCode);
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
            if (MTKBMC(pin_bdc) != 0)
            {
                MTKBMC(pin_bdc)->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
            }
        #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        }
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        Event->p.remDev->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
    #endif 
    }
}

/*****************************************************************************
 * FUNCTION
 *  btbmHandlerLinkkeyDeleted
 * DESCRIPTION
 *  Handle linke key deleted by host
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerLinkkeyDeleted(const BtEvent *Event){
    OS_Report("[SEC] btbmHandlerLinkkeyDeleted");
    BTBMDumpBdAddr(Event->p.bdAddr.addr);
    btbmSendLinkkeyDelInd(BTBM_ADP_FAIL_LINK_KEY_DISMISSED, &Event->p.bdAddr);
}

/*****************************************************************************
 * FUNCTION
 *  btbmHandleLinkConnectReq
 * DESCRIPTION
 *  Handle remote link request.
 * PARAMETERS
 *  remDev : remote device which initiate connection request.
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleLinkConnectReq(BtRemoteDevice *remDev){
    BtDeviceContext *bdc = 0;
    U8 piconet_no, scatternet_no, sco_no, esco_no, master_sco, slave_sco;
    BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &master_sco, &slave_sco);
    U32 value = 0;  

    bdc = DS_FindDevice(&(remDev->bdAddr));
    if (bdc == NULL){ 
        /* DeviceContext should be created at BM layer. */
        if( BT_STATUS_SUCCESS != BtbmAllocateFreeDevices(remDev->bdAddr, &bdc)){
            OS_Report("[BM][ERR] btbmHandleLinkConnectReq : btbmHandleLinkConnectReq failed");
            Assert(0);
            return;
        }
    }
    bdc->under_used |= BDS_CONNECTING;
    bdc->link = remDev;
    if( (sco_no+esco_no) > 0 ){
        if (scatternet_no == 0){
            /* Not in scatternet, request as master to prevent scatternet */
            ME_AcceptIncomingLink(remDev, BCR_MASTER);
        }else{
            OS_GetSystemPropertyUint("sconocarescattnet", &value, 0);    
            if (value)        
            {
                ME_AcceptIncomingLink(remDev, BCR_SLAVE);
            }else{
                /* in scatternet, reject directly to guarantee SCO quality */
                ME_RejectIncomingLink(remDev, BEC_LIMITED_RESOURCE);
            }
        }
    }else{
        ME_AcceptIncomingLink(remDev, BCR_SLAVE);
    }
}

/*****************************************************************************
 * FUNCTION
 *  btbmHandleReportLinkState
 * DESCRIPTION
 *  Report the current active connection and dump the connection status in catcher log.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleReportLinkState(BtRemoteDevice *remDev)
{
    U8 active_no;
    BMCallbackParms parms;
    
    active_no = MEC(activeCons);
    btbmHandleCheckPiconetState();
    parms.event = BTBM_EVENT_REPORT_ACTIVE_LINK_NO;
    parms.p.ReportLinkStateResult.ActiveLinkNumber = active_no;
    CONVERT_ARRAY2BDADDR(&parms.p.ReportLinkStateResult.bd_addr, remDev->bdAddr.addr);
    MTKBMC(bmCB)(&parms); 
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleCheckPiconetState
 * DESCRIPTION
 *  Dump the connection status in catcher log.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
U8 btbmHandleCheckPiconetState(void)
{
    U8 master_numer = 0;
    U8 slave_number = 0;
    BtRemoteDevice *link;
    U8 i = 0;
    BtScoConnect *scocon;
    U8 link_no = 0;

    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scocon = &(MEC(scoTable)[i]);
        /* Hunt for an outstanding connection */
        switch (MEC(scoTable)[i].scostate)
        {
            case BDS_OUT_CON2:
                bt_trace(TRACE_GROUP_1, ONE_SCO_OUT_CON2);
                break;
            case BDS_IN_CON:
                bt_trace(TRACE_GROUP_1, ONE_SCO_IN_CON);
                break;
            case BDS_CONNECTED:
                bt_trace(TRACE_GROUP_1, ONE_SCO_IN_CONNECTED);
                break;
        }
    }
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if (link->state == BDS_CONNECTED)
        {
            if (link->role == BCR_SLAVE)
            {
                slave_number++;
            }
            else
            {
                master_numer++;
            }
        }
    }
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBM_MASTERNUM_SLAVENUM, master_numer, slave_number);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &(MEC(devTable)[i]);
        if (link->state != BDS_DISCONNECTED)
        {
            link_no++;
                bt_trace(
                    TRACE_GROUP_1,
                    BD_ADDRBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X_IN_STATE,
                    link->bdAddr.addr[0],
                    link->bdAddr.addr[1],
                    link->bdAddr.addr[2],
                    link->bdAddr.addr[3],
                    link->bdAddr.addr[4],
                    link->bdAddr.addr[5],
                    link->state);
        }
    }
    if ((slave_number == 0) && (master_numer == 0))
    {
        BTBMHIDNoRoleSwitch(0);
    }
    bt_trace(TRACE_GROUP_1, BTLOG_BTBM_LINK_NUMBER_NOT_IN_DISCONNECTED_STATEBT, link_no);
    return link_no;
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerCheckAllLinkPowerOffStatus
 * DESCRIPTION
 *  To check all link in disconnected mode, if not, force to disconnect the ACL link
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerCheckAllLinkPowerOffStatus(void)
{
    U8 i = 0;
    BtRemoteDevice *link = 0;
    BOOL aclexist = FALSE;
    BtDeviceContext *bdc = 0;
    BD_ADDR addr;
    BMCallbackParms parms;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        link = &MEC(devTable)[i];
        if((link->state != BDS_DISCONNECTED) && (link->state != BDS_OUT_DISC))
        {
            bt_trace(TRACE_GROUP_1, THE_LINK_STATEBT_DEBUGx02X, link->state);
            BTBMDumpBdAddr(link->bdAddr.addr);
            aclexist = TRUE;
        }
    }
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
    {
        if (bdc->link != 0)
        {
            aclexist = TRUE;
            bt_trace(TRACE_GROUP_1, BDCLINKSTATEBT_DEBUGx02X, bdc->link->state);
            BTBMDumpBdAddr(bdc->addr.addr);
            if (bdc->link->state == BDS_CONNECTED)
            {
                addr = bdc->addr;
                bdc->link->discFlag = TRUE;
                btbmDisconnectAclLinkWithoutForce(bdc->link, BEC_POWER_OFF, 3);
            }
        }
    }

    if (!aclexist)
    {
        if (MTKBMC(power_on_off_mode) != BM_POWER_OFF)
        {
            parms.event = BTBM_EVENT_DISALLOW_ACL_LINK_RESULT;
            MTKBMC(bmCB)(&parms);
            MTKBMC(power_on_off_mode) = BM_POWER_OFF;
        }
    }

}

#if defined (__BT_2_1_SIMPLE_PAIRING__) || defined (__BT_2_1_BLOCK_DEVICE__)


/*****************************************************************************
 * FUNCTION
 *  btbmHandleForceDisconnectLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleForceDisconnectLink(BD_ADDR *link)
{
    BtDeviceContext *bdc;

    bdc = DS_FindDevice(link);
    if ((bdc != 0) && (bdc->link != 0) && (bdc->link->state == BDS_CONNECTED))
    {
        btbmDisconnectAclLinkWithoutForce(bdc->link, BEC_USER_TERMINATED, 4);
    }
}
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) || defined (__BT_2_1_BLOCK_DEVICE__) */ 


/*****************************************************************************
 * FUNCTION
 *  btbmDisconnectAclLinkWithoutForce
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btbmDisconnectAclLinkWithoutForce(BtRemoteDevice *link, U8 reason, U8 index)
{
    bt_trace(TRACE_GROUP_1, ME_FORCEDISCONNECTLINKWITHREASONxD, index);
    btbmAddLinkCallback(link);
    ME_ForceDisconnectLinkWithReason(&linkbmHandler, link, reason, TRUE);
    btbmRemoveLinkCallback(link);
}

#if (BT_READ_RSSI == XA_ENABLED)
/*****************************************************************************
 * FUNCTION
 *  btbmReportReadRSSIResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btbmReportReadRSSIResult(const BtEvent* Event)
{
    BMCallbackParms parms;
    BtRemoteDevice* remDev = Event->p.rssi.remDev;
    
    S8 rssi_value = Event->p.rssi.RSSI;
    BtErrorCode errCode = Event->errCode;
    btbm_bd_addr_t dev_addr;
    memset((U8*)&dev_addr,0,sizeof(btbm_bd_addr_t));
    CONVERT_ARRAY2BDADDR(&dev_addr, remDev->bdAddr.addr);      
    bt_trace(TRACE_GROUP_1,BTLOG_BTBM_REPORT_READRSSI_RESULT,MTKBMC(rssi_op_ongoing),errCode,rssi_value,dev_addr.lap,dev_addr.uap,dev_addr.nap);
    if (MTKBMC(rssi_op_ongoing) == TRUE)
    {
        MTKBMC(rssi_op_ongoing) = FALSE;
        parms.event = BTBM_EVENT_REPORT_RSSI_VALUE;
        parms.p.ReportRSSIValue.error_code = errCode;
        parms.p.ReportRSSIValue.rssi_value = rssi_value;
        parms.p.ReportRSSIValue.bd_addr = dev_addr;
        MTKBMC(bmCB)(&parms);
    }  
}
#endif /* (BT_READ_RSSI == XA_ENABLED) */
