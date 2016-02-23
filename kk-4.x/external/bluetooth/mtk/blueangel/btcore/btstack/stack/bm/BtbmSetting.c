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
#include "bt_adp_system.h"

#if defined (__BT_2_1_BLOCK_DEVICE__)
#include "btbmblocklist.h"
#endif 

#ifdef MTK_BB_TESTING
#include "BtBBtesting.h"
#endif 

extern void bm_app_callback(BMCallbackParms *Parms);
/***************
Internal Globals 
****************/
/* MTK Bluetooth Manager Context */
extern BMCONTENT_T BMController;

#define MTKBMC(f)  (BMController.f)

extern BtHandler globalbtbmHandler;

/*****************************************************************************
 * FUNCTION
 *  BTBMReadLocalBDAddress
 * DESCRIPTION
 *  This function is used to read the local bluetooth address.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReadLocalBDAddress(void)
{
    btbm_bd_addr_t bd_addr;
    BMCallbackParms parms;
    BD_ADDR addr;

    parms.event = BTBM_EVENT_READ_LOCAL_BLUETOOTH_ADDR_RESULT;
    parms.p.ReadLocalAddrResult.result = BTBM_ADP_SUCCESS;
    if( BTCoreVerifySysInitState() ){
        CONVERT_ARRAY2BDADDR(&bd_addr, MEC(bdaddr).addr);
    }else if(BTCoreCheckNVRMBDAddr() && BTCoreGetNVRMBDAddr(&addr)){
        CONVERT_ARRAY2BDADDR(&bd_addr, addr.addr);
    }else{
        parms.p.ReadLocalAddrResult.result = BTBM_ADP_FAILED;
    }
    parms.p.ReadLocalAddrResult.bd_addr = bd_addr;
    if(MTKBMC(bmCB)){
        MTKBMC(bmCB)(&parms);
    }else{
        bm_app_callback(&parms);
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMReadLocalDeviceClass
 * DESCRIPTION
 *  This function is used to read the local bluetooth class of device.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReadLocalDeviceClass(void)
{
    BMCallbackParms parms;

    parms.event = BTBM_EVENT_READ_CLASS_OF_DEVICE_RESULT;
    parms.p.ReadCodResult.result = BTBM_ADP_SUCCESS;
    if(BTCoreVerifySysInitState()){
        parms.p.ReadCodResult.cod = MEC(classOfDevice);
    }else{
        parms.p.ReadCodResult.cod = BT_DEFAULT_COD;
    }
    if(MTKBMC(bmCB)){
        MTKBMC(bmCB)(&parms);
    }else{
        bm_app_callback(&parms);
    }
    return;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMReadLocalDeviceUuidList
 * DESCRIPTION
 *  This function is used to read the local bluetooth list of uuid.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReadLocalDeviceUuidList(void)
{
    BMCallbackParms parms;
    U8 index;
    U16 uuid_16;
    U32 service_list[6];

    OS_MemSet((U8*) service_list, 0, sizeof(service_list));
    if(BTCoreVerifySysInitState()){
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
        for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
        {
            if (MEC(eirSdpCtx)[index].used != 0)
            {
                uuid_16 = MEC(eirSdpCtx)[index].service_class;

                if ((uuid_16 < 0x1120) && (uuid_16 >= 0x1100))
                {
                    service_list[0] |= ((U32) 1 << (uuid_16 - 0x1100));
                }
                else if ((uuid_16 < 0x1140) && (uuid_16 >= 0x1120))
                {
                    service_list[1] |= ((U32) 1 << (uuid_16 - 0x1120));
                }
                else if ((uuid_16 < 0x1220) && (uuid_16 >= 0x1200))
                {
                    service_list[2] |= ((U32) 1 << (uuid_16 - 0x1200));
                }
                else if ((uuid_16 < 0x1320) && (uuid_16 >= 0x1300))
                {
                    service_list[3] |= ((U32) 1 << (uuid_16 - 0x1300));
                }
                else if ((uuid_16 < 0x1420) && (uuid_16 >= 0x1400))
                {
                    service_list[4] |= ((U32) 1 << (uuid_16 - 0x1400));
                }
            }
        }
        parms.p.ReadCodResult.result = BTBM_ADP_SUCCESS;
#else
        parms.p.ReadCodResult.result = BTBM_ADP_FAILED;
#endif
    }else{
        parms.p.ReadCodResult.result = BTBM_ADP_SUCCESS;
    }
    parms.event = BTBM_EVENT_READ_UUID_LIST_RESULT;
    parms.p.ReadLocalUuidListResult.uuid_list = service_list;
    if(MTKBMC(bmCB)){
        MTKBMC(bmCB)(&parms);
    }else{
        bm_app_callback(&parms);
    }
    return;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMWriteLocalDeviceClass
 * DESCRIPTION
 *  The function is used to write local cod
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  cod : The input message to write the cod.
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMWriteLocalDeviceClass(bt_bm_write_local_cod_req_struct *btbm_adp_input_msg)
{
    U32 cod = 0;
    U32 new_cod = 0;
    btbm_write_cod_type type;
    BMCallbackParms parms;
    
    cod = btbm_adp_input_msg->cod;
    type = btbm_adp_input_msg->write_type;
    bt_trace(TRACE_GROUP_1, WRITE_BT_CODx08X, cod);
    switch (type)
    {
        case BTBM_WRITE_COD_RESTORE:
            new_cod = BT_DEFAULT_COD;
            break;
        case BTBM_WRITE_COD_MASK:
            /* Modify from (MEC(classOfDevice) || cod) to (MEC(classOfDevice) | cod): 2007-1206 */
            new_cod = MEC(classOfDevice) | cod;
            break;
        case BTBM_WRITE_COD_NEW:
            new_cod = cod;
            break;
    }
    /* Using the final decision new_cod to set: Modify from ME_SetClassOfDevice(cod) to new_cod: 2007-1206 */
    bt_trace(TRACE_GROUP_1, WRITE_BT_CODx08X, new_cod);
    ME_SetClassOfDevice(new_cod);
    parms.event = BTBM_EVENT_WRITE_LOCAl_CLASS_OF_DEVICE_RESULT;
    parms.p.result = BTBM_ADP_SUCCESS;
    MTKBMC(bmCB)(&parms);
    return;

}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapReadScanEnabled
 * DESCRIPTION
 *  This function is used to read the visible mode setting.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapReadScanEnabled(void)
{
    BMCallbackParms parms;

    parms.event = BTBM_EVENT_READ_SCAN_MODE_RESULT;
    parms.p.ReadScanModeResult = MEC(accModeExpValue);
    MTKBMC(bmCB)(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapWriteScanEnabled
 * DESCRIPTION
 *  This function is used to set the Bluetooth Writescan enable mode.
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_write_scanenable_mode_req_struct
 *  mode; writescan enabled mode
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapWriteScanEnabled(bt_bm_write_scanenable_mode_req_struct *btbm_adp_input_msg)
{
    BMCallbackParms parms;
    
#ifdef __BT_4_0_BLE__
#ifdef __BT_LE_STANDALONE__
    if (MEC(localNameLen) > 0)
    {
        ME_LE_SetAccessible((BtAccessibleMode) ((btbm_adp_input_msg->mode >> 8) & 0xFF), NULL);
        parms.event = BTBM_EVENT_WRITE_SCAN_MODE_RESULT;
        parms.p.result = BTBM_ADP_SUCCESS;
        MTKBMC(bmCB)(&parms);
        MTKBMC(bm_setting).writescan_mode = (btbm_adp_input_msg->mode & 0xFF);
        MTKBMC(bm_setting).le_writescan_mode = ((btbm_adp_input_msg->mode >> 8) & 0xFF);
    }
    else    /* Wait for name config */
    {
        MEC(accModeCur) = btbm_adp_input_msg->mode;
        MEC(accModeExpValue) = btbm_adp_input_msg->mode;
    }
    return;    
#else
    #if 0
    if (MEC(localNameLen) > 0)
    {
        ME_LE_SetAccessible(((btbm_adp_input_msg->mode >> 8) & 0xFF), NULL);  /* Wait for name config */
    }
    #endif
#endif
#endif
    U32 lescanable = 0;
    if( OS_GetSystemPropertyUint("lescanable", &lescanable, 1) == 0 ){
        OS_Report("Property lescanable not set");
    }
    if(lescanable){
        MTKBMC(bm_setting).writescan_mode = btbm_adp_input_msg->mode;
        MTKBMC(bm_setting).le_writescan_mode = btbm_adp_input_msg->mode;
    }else{
        MTKBMC(bm_setting).writescan_mode = (btbm_adp_input_msg->mode & 0xFF);
        MTKBMC(bm_setting).le_writescan_mode = ((btbm_adp_input_msg->mode >> 8) & 0xFF);
    }
    BTBMLocalWriteScanEnabled(MTKBMC(bm_scan_mode));
    
    parms.event = BTBM_EVENT_WRITE_SCAN_MODE_RESULT;
    parms.p.result = BTBM_ADP_SUCCESS;
    MTKBMC(bmCB)(&parms);
}



/*****************************************************************************
 * FUNCTION
 *  BTBMGapSaveWriteScanModeValue
 * DESCRIPTION
 *  
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus BTBMGapSaveWriteScanModeValue(bt_bm_write_scanenable_mode_req_struct *btbm_adp_input_msg)
{
    MTKBMC(bm_setting).writescan_mode = btbm_adp_input_msg->mode;
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapWriteLocalName
 * DESCRIPTION
 *  This function is used to make the write the local device name
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_write_local_name_req_struct
 *  name_len: device name length
 *  name: device name
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapWriteLocalName(bt_bm_write_local_name_req_struct *btbm_adp_input_msg)
{
    U8 length = 0;

    length = min(btbm_adp_input_msg->name_len, BTBM_ADP_MAX_NAME_LEN);

    OS_MemSet(MTKBMC(bm_setting).local_name, 0, sizeof(MTKBMC(bm_setting).local_name));
    OS_MemCopy(MTKBMC(bm_setting).local_name, btbm_adp_input_msg->name, length);
    MTKBMC(bm_setting).len = length;
    ME_SetLocalDeviceName(MTKBMC(bm_setting).local_name, (U8)(length + 1));
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapReadLocalName
 * DESCRIPTION
 *  This function is used to Read local device name
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapReadLocalName(void)
{
    BMCallbackParms parms;

    parms.event = BTBM_EVENT_READ_LOCAL_DEVICE_NAME_RESULT;
    parms.p.ReadLocalNameResult.name = MTKBMC(bm_setting).local_name;
    parms.p.ReadLocalNameResult.length = MTKBMC(bm_setting).len;
    MTKBMC(bmCB)(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapCopyLocalNameWithoutCFM
 * DESCRIPTION
 *  
 * PARAMETERS
 *  name        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 BTBMGapCopyLocalNameWithoutCFM(U8 *name)
{
    if (MTKBMC(bm_setting).len > BTBM_ADP_MAX_NAME_LEN)
    {
        MTKBMC(bm_setting).len = BTBM_ADP_MAX_NAME_LEN;
    }
    OS_MemSet(name, 0, MTKBMC(bm_setting).len);
    OS_MemCopy(name, MTKBMC(bm_setting).local_name, MTKBMC(bm_setting).len);
    return MTKBMC(bm_setting).len;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapGetAuthenticationMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 BTBMGapGetAuthenticationMode()
{
    return MTKBMC(bm_security_mode);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapSetAuthenticationMode
 * DESCRIPTION
 *  This function is used to set the device into authentication enable/disable mode
 * PARAMETERS
 *  reported                [IN]        
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_write_authentication_mode_req_struct
 *  mode: The authentication enable/disable mode setting
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapSetAuthenticationMode(BOOL reported, bt_bm_write_authentication_mode_req_struct *btbm_adp_input_msg)
{
    BtStatus status;
    BMCallbackParms parms;
    
    /* Add by mtk01411 2007-0629 for GAP PTW70 test: TC_SEC_AUT_BV_01_C and TC_SEC_SEM_BV_03_C */
    if (btbm_adp_input_msg->mode == BTBM_AUTHENTICATION_ENABLE_LEVEL3_ENCRYPT_OFF)
    {
        bt_trace(TRACE_GROUP_1, IN_BTBMGAPSETAUTHENTICATION_FORCE_TO_LEVEL_3_WITH_ENCRYPTION_OFF);
        /* For GAP PTW test case: disable encryption mode */
        status = SEC_EnableSecurityMode3(&globalbtbmHandler, FALSE);
        MTKBMC(bm_security_mode) = BSM_SEC_LEVEL_3;
        MEC(secLevel) = BSM_SEC_LEVEL_3;
        goto end_security_check;
    }
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    if (btbm_adp_input_msg->mode == BTBM_AUTHENTICATION_DISABLE)
    {
        status = SEC_DisableSecurityMode3(&globalbtbmHandler);
        MTKBMC(bm_security_mode) = BSM_SEC_LEVEL_1;
        MEC(secLevel) = BSM_SEC_LEVEL_1;
    }
    else
    {
        status = SEC_DisableSecurityMode3(&globalbtbmHandler);
        MTKBMC(bm_security_mode) = BSM_SEC_LEVEL_2;
        MEC(secLevel) = BSM_SEC_LEVEL_2;
    }
#else /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
    if (btbm_adp_input_msg->mode == BTBM_AUTHENTICATION_DISABLE)
    {
        status = SEC_DisableSecurityMode3(&globalbtbmHandler);
        MTKBMC(bm_security_mode) = BSM_SEC_LEVEL_1;
        MEC(secLevel) = BSM_SEC_LEVEL_1;
    }
    else
    {
        status = SEC_EnableSecurityMode3(&globalbtbmHandler, TRUE);
        MTKBMC(bm_security_mode) = BSM_SEC_LEVEL_3;
        MEC(secLevel) = BSM_SEC_LEVEL_3;
    }
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */

  end_security_check:

    bt_trace(TRACE_GROUP_1, THE_AUTHENTICATION_STATUSBT_DEBUGx02X, status);
    if (reported)
    {
        parms.event = BTBM_EVENT_SET_AUTHENTICATION_RESULT;

        switch (status)
        {
            case BT_STATUS_SUCCESS:
                MTKBMC(write_authentication_state) = BM_WRITE_AUTHENTICATION_NONE;
                parms.p.result = BTBM_ADP_SUCCESS;
                MTKBMC(bmCB)(&parms);                
                break;
            case BT_STATUS_PENDING:
                MTKBMC(write_authentication_state) = BM_WRITE_AUTHENTICATION_PROCESSING_NEED_REPORT;
                break;
            default:
                MTKBMC(write_authentication_state) = BM_WRITE_AUTHENTICATION_NONE;
                parms.p.result = BTBM_ADP_FAILED;
                MTKBMC(bmCB)(&parms);                
                return;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapSetEncryptionOff
 * DESCRIPTION
 *  The function is used to turn the encryption to OFF.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapSetEncryptionOff(void)
{
    SecSetEncryptMode(&globalbtbmHandler, FALSE);
}

#if L2CAP_NUM_GROUPS > 0
/*****************************************************************************
 * FUNCTION
 *  BTBMGapSetEncryptionOn
 * DESCRIPTION
 *  The function is used to turn the encryption to OFF.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapSetEncryptionOn(void)
{
    SecSetEncryptMode(&globalbtbmHandler, TRUE);
}
#endif

/*****************************************************************************
 * FUNCTION
 *  BTBMLocalWriteScanEnabled
 * DESCRIPTION
 *  The function is used by BT stack internally. When we connect, pairing, bonding, inquiry, disconnect,
 *  we will turn the discovery mode off or on. The function is used by internal stack to do the procedure.
 *  When the special scenario is finished, the discovery mode will be set to the original mode.
 * PARAMETERS
 *  mode        [IN]        The discovery mode shall be turn off or restore to origial value
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMLocalWriteScanEnabled(U8 mode)
{
    U8 scan_enabled_mode = 0;
    U8 le_scan_enabled_mode = 0;

    scan_enabled_mode = MTKBMC(bm_setting).writescan_mode;
    le_scan_enabled_mode = MTKBMC(bm_setting).le_writescan_mode;
    Report(("BTBMLocalWriteScanEnabled:%02x,%02x,%02x,%02x,", mode, scan_enabled_mode, le_scan_enabled_mode, MTKBMC(bm_opactivity)));
    if (mode == BM_SCAN_MODE_SET_UNPAGEABLE)
    {
        MTKBMC(bm_scan_mode) = BM_SCAN_MODE_SET_UNPAGEABLE;
        scan_enabled_mode &= BAM_DISCOVERABLE_ONLY;
        le_scan_enabled_mode &= BAM_DISCOVERABLE_ONLY;
    }
    else
    {
        switch (MTKBMC(bm_opactivity))
        {
            #ifndef __INDEPENDENT_INQUIRY_FLOW__
            case BM_OP_INQUIRY:
            case BM_OP_NAMEREQ:
            case BM_OP_INQCANCEL:
            #endif
            case BM_OP_BONDING:
            case BM_OP_PAIRING:
                bt_trace(
                    BT_TRACE_G1_PROTOCOL,
                    BTLOG_BTMTKBMCBM_OPACTIVITYxD_CAN_NOT_CHANGE_TO_NORMAL,
                    MTKBMC(bm_opactivity));
                return;
            default:
                MTKBMC(bm_scan_mode) = BM_SCAN_MODE_SET_NORMAL;
                break;
        }
    }
#ifdef __BT_4_0_BLE__
    ME_LE_SetAccessible(le_scan_enabled_mode, NULL);
    /*
    if (MTKBMC(bm_scan_mode) == BM_SCAN_MODE_SET_NORMAL)
    {
        ME_LE_SetAccessible(BAM_GENERAL_ACCESSIBLE, NULL);
    }
    else
    {
        ME_LE_SetAccessible(BAM_NOT_ACCESSIBLE, NULL);
    }
    */
#ifdef __BT_LE_STANDALONE__
    return;    
#endif
#endif
    ME_SetAccessibleModeNC(scan_enabled_mode);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGetLocalStoredName
 * DESCRIPTION
 *  The function is used to get connected device name
 * PARAMETERS
 *  bdAddr      [IN]        The remote device Bluetooth Address
 * RETURNS
 *  The pointer points to the connected device name.
 *****************************************************************************/
U8 *BTBMGetLocalStoredName(U8 *bdAddr)
{
    int i = 0;
    BTBMRemoteNameSetting_T *entry = 0;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &MTKBMC(DevRemNameTbl)[i];
        if (entry->link != 0)
        {
            if (OS_MemCmp(MTKBMC(DevRemNameTbl)[i].link->bdAddr.addr, 6, bdAddr, 6))
            {
                return (U8*) (MTKBMC(DevRemNameTbl)[i].dev_name);
            }
        }
    }
    return 0;
}


#ifdef __BT_4_0_BLE__
/*****************************************************************************
 * FUNCTION
 *  BTBMGapWriteAppearance
 * DESCRIPTION
 *  This function is used to make the write the appearance value
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapWriteAppearance(bt_bm_write_appearance_req_struct *btbm_adp_input_msg)
{
    StoreLE16(MTKBMC(bm_setting).appearance, btbm_adp_input_msg->appearance);
}
#endif


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerAccessModeChange
 * DESCRIPTION
 *  To handle the Write Scan enabled completed
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerAccessModeChange(const BtEvent *Event)
{
    BMCallbackParms parms;

    if (MTKBMC(bm_scan_opactivity) == BM_OP_SETSCANMODE)
    {
        MTKBMC(bm_scan_opactivity) = BM_OP_NONE;
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWRITESCAN_MODE_OPERATION_FAILEDBT_DEBUGx02X, Event->errCode);
        parms.event = BTBM_EVENT_WRITE_SCAN_MODE_RESULT;
        if (Event->errCode == 0x0C)
        {
            parms.p.result = BTBM_ADP_FAIL_NOT_ALLOW;
            MTKBMC(bmCB)(&parms);
        }
        else
        {
            parms.p.result = BTBM_ADP_SUCCESS;
            MTKBMC(bmCB)(&parms);
            if (MEC(accModeCur) != (MTKBMC(bm_setting).writescan_mode))
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWRITESCAN_MODE_OPERATION_IS_NOT_MATCH_REDO_AGAIN);
                BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
            }
        }
    }
    else
    {
        if (MTKBMC(bm_scan_mode) == BM_SCAN_MODE_SET_UNPAGEABLE)
        {
            if (((MEC(accModeCur) & 0x01) != (MTKBMC(bm_setting).writescan_mode & 0x01)) &&
                (Event->errCode != 0x0C))
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWRITESCAN_MODE_OPERATION_IS_NOT_MATCH_REDO_AGAIN);
                BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerChangeLocalName
 * DESCRIPTION
 *  To handle the write local device name completed
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerChangeLocalName(const BtEvent *Event)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_WRITE_LOCAL_DEVICE_NAME_RESULT;
    parms.p.result = BTBM_ADP_SUCCESS;
    MTKBMC(bmCB)(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerSecurityChange
 * DESCRIPTION
 *  To handle the write authentication mode completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerSecurityChange(const BtEvent *Event)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_SET_AUTHENTICATION_RESULT;

    if (MTKBMC(write_authentication_state) == BM_WRITE_AUTHENTICATION_PROCESSING_NEED_REPORT)
    {
        parms.p.result = BTBM_ADP_SUCCESS;
        MTKBMC(bmCB)(&parms);                
    }
    MTKBMC(write_authentication_state) = BM_WRITE_AUTHENTICATION_NONE;
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleGetSecurityLevel
 * DESCRIPTION
 *  To Get the current BT security level
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
btbm_security_level_mode btbmHandleGetSecurityLevel(void)
{
    U8 level = 0;

    switch (MEC(secLevel))
    {
        case BSM_SEC_LEVEL_1:
            level = BTBM_SECURITY_LEVEL1;
            break;
        case BSM_SEC_LEVEL_2:
            level = BTBM_SECURITY_LEVEL2;
            break;
        case BSM_SEC_LEVEL_3:
            level = BTBM_SECURITY_LEVEL3;
            break;

    }
    return level;
}

