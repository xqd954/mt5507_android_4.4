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


/***************
Internal Globals 
****************/
/* MTK Bluetooth Manager Context */
extern BMCONTENT_T BMController;

#define MTKBMC(f)  (BMController.f)

extern void CONVERT_BDADDRSRC2ARRAY(U8 *dest, btbm_bd_addr_t *source);

static void btbmStartServiceSearchImmediatly(){
    sdap_service_search_all_struct search_pattern;
    OS_Report("[BM] btbmStartServiceSearchImmediatly");
    search_pattern.bdAddr = &MTKBMC(service_search_addr);
    search_pattern.sdap_len = MTKBMC(sdap_len);
    search_pattern.sdp_uuid = MTKBMC(sdap_uuid);
    SDAP_ServiceSearchAll(search_pattern);
}

void btbmHandleServiceSearchDelayTimeout(EvmTimer *timer){
    OS_Report("[BM] btbmHandleServiceSearchDelayTimeout");
    btbmStartServiceSearchImmediatly();
}

void btbmStartServiceSearch(void){
    EvmTimer *timer = &MTKBMC(service_search_delay_timer);
    BtRemoteDevice *remDev = ME_FindRemoteDevice(&MTKBMC(service_search_addr));
    U32 delay_timeout;
    if(remDev 
        /* && BTBMGetDeviceModelByVersion(remDev) == DEVICE_SE_XPERIA_RAY */
        && (delay_timeout = (U32)btGetDevCustvalueById(remDev->devId, CUSTID_SDP_DELAY_TIMEOUT)) > 0){
        OS_Report("[BM] btbmStartServiceSearch : delay %u ms", delay_timeout);
        if (timer->func != 0){
            EVM_CancelTimer(timer);
        }
        timer->func = btbmHandleServiceSearchDelayTimeout;
        EVM_StartTimer(timer, delay_timeout);
    }else{
        btbmStartServiceSearchImmediatly();
    }
}

/*****************************************************************************
 * FUNCTION
 *  BTBMSdapSearchallService
 * DESCRIPTION
 *  This function is used to search remote device supported services
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_service_search_req_struct
 *  bd_addr: Remote bluetooth address
 *  sdap_len: The sdap uuid number, the sdap_len must >0
 *  sdap_uuid: The UUID list of the SDAP operation
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMSdapSearchallService(bt_bm_service_search_req_struct *btbm_adp_input_msg)
{
    sdap_service_search_all_struct search_pattern;
    U8 i = 0;
    btbm_bd_addr_t *remoteaddr;
    BD_ADDR temp_addr;

#if 1
    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(MTKBMC(service_search_addr).addr, remoteaddr);
    MTKBMC(sdap_len) = btbm_adp_input_msg->sdap_len;
    for (i = 0; i < MTKBMC(sdap_len); i++)
    {
        MTKBMC(sdap_uuid)[i] = btbm_adp_input_msg->sdap_uuid[i];
    }
    btbmStartServiceSearch();
#else
    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(temp_addr.addr, remoteaddr);
    MTKBMC(sdap_len) = btbm_adp_input_msg->sdap_len;
    for (i = 0; i < MTKBMC(sdap_len); i++)
    {
        MTKBMC(sdap_uuid)[i] = btbm_adp_input_msg->sdap_uuid[i];
    }
    search_pattern.bdAddr = &temp_addr;
    search_pattern.sdap_len = MTKBMC(sdap_len);
    search_pattern.sdp_uuid = MTKBMC(sdap_uuid);
    SDAP_ServiceSearchAll(search_pattern);
#endif
}


/*****************************************************************************
 * FUNCTION
 *  BTBMSdapSearchAllCancel
 * DESCRIPTION
 *  This function is used to search remote device supported services
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_service_search_req_struct
 *  bd_addr: Remote bluetooth address
 *  sdap_len: The sdap uuid number, the sdap_len must >0
 *  sdap_uuid: The UUID list of the SDAP operation
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMSdapSearchAllCancel(bt_bm_service_search_cancel_req_struct *btbm_adp_input_msg)
{
    BD_ADDR temp_addr;

    CONVERT_BDADDRSRC2ARRAY(temp_addr.addr, &(btbm_adp_input_msg->bd_addr));
    SDAP_ServiceSearchAllCancel(&temp_addr);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMSdapSearchAttributeService
 * DESCRIPTION
 *  This function is used to search remote device supported services and attribute
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_search_attribute_req_struct
 *  bd_addr: Remote bluetooth address
 *  sdap_uuid: The sdap service uuid
 *  attribute_id: The attribute id
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMSdapSearchAttributeService(bt_bm_search_attribute_req_struct *btbm_adp_input_msg)
{
    sdap_service_search_single_attribute_struct search_pattern;
    btbm_bd_addr_t *remoteaddr;
    BD_ADDR temp_addr;

    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDRSRC2ARRAY(temp_addr.addr, remoteaddr);
    search_pattern.bdAddr = &temp_addr;
    search_pattern.attribute_id = btbm_adp_input_msg->attribute_id;
    if (btbm_adp_input_msg->type == BTBM_UUID_16)
    {
        search_pattern.uuid_len = 2;
        search_pattern.sdp_uuid = (U16)btbm_adp_input_msg->sdap_uuid_16;
    }
    else
    {
        search_pattern.uuid_len = 16;
        OS_MemCopy(search_pattern.sdp_uuid_128, btbm_adp_input_msg->sdap_uuid_128, 16);
    }
    SDAP_ServiceSearchSingleAttribute(search_pattern);
}

