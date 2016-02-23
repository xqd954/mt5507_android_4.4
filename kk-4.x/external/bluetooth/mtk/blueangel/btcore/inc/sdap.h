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
 * Sdap.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains sdap functions prototype and variable definiation of SDAP services
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * Mar 12 2009 mtk01411
 * [MAUI_01645202] [Bluetooth] Btadp folder restruct
 * 
 *
 * Feb 18 2009 mtk01239
 * [MAUI_01631727] [Bluetooth] Checkin HCI Retransmission design
 * 
 *
 * Dec 3 2008 mtk02126
 * [MAUI_01288486] chech in SPP/DUN new architecture
 * 
 *
 * Dec 3 2008 mtk02126
 * [MAUI_01288486] chech in SPP/DUN new architecture
 * 
 *
 * Dec 2 2008 mtk02126
 * [MAUI_01288486] chech in SPP/DUN new architecture
 * 
 *
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Aug 12 2008 mtk01411
 * [MAUI_00820444] [Bluetooth] SyncML feature supported in BT
 * 
 *
 * Jul 2 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Feb 28 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Feb 19 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * May 7 2007 mtk01239
 * [MAUI_00389758] [Bluetooth] Checkin JSR82 code of BTStack
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Dec 27 2006 mtk01239
 * [MAUI_00354182] [1] Assert Fail: SDAP(sdap_current_multi_req_index) <= SDAP_MAX_MULTIPLE_REQ sdap.c
 * 
 *
 * Dec 7 2006 mtk01239
 * [MAUI_00349361] [1]Assert fail:attribute_num<=SDAP_MAX_ATTRIBUTE_NUM sdap.c 592-BT
 * 
 *
 * Jun 19 2006 mtk01239
 * [MAUI_00203552] [Bluetooth][ESI] update bt code
 * 
 *
 * Apr 3 2006 mtk00758
 * [MAUI_00184485] [New feature] Add task to simulate UART driver for unitest
 * 
 *
 * Mar 11 2006 MTK00303
 * [MAUI_00178684] [Bluetooth]ESI bluetooth protocol stack
 * add bluetooth files
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#ifndef _SDAP_H
#define _SDAP_H

#include "bttypes.h"
#include "sdp_adp.h"
#include "bt_adp_bm.h"
#include "bt_jsr82_setting.h"
#include "medev_adp.h"
#include "utils.h"

/***************
Type Definitions 
****************/
#define SDAP_NUM_RECORD_SEARCH (0x20)

#define SDAP_MAX_ATTRIBUTE_NUM               (0x10)

#ifdef __SDAP_REQUEST_QUEUE__
#define SDAP_MAX_MULTIPLE_REQ       (0x05)

#define SDAP_REQUEST_ALL					0x00
#define SDAP_REQUEST_ALL_2					0x01
#define SDAP_REQUEST_MULTIPLE_ATTRIBUTE		0x02
#define SDAP_REQUEST_MULTIPLE_ATTRIBUTE_2	0x03
#define SDAP_REQUEST_RAW					0x04

#define SDAP_QUEUED_REQUEST_TIMEOUT			5000
#else
#define SDAP_MAX_MULTIPLE_REQ       (0x02)
#endif

/* Slip state machine states. */
enum sdap_state_enum
{
    SDAP_PROGRESS_NONE = 0,
    SDAP_PROGRESS_ONGOING,
    SDAP_PROGRESS_MULTIPLE_ONGOING
};

typedef enum sdap_state_enum sdap_state;

typedef void (*SdapCallBack) (SdpQueryToken *sqt, U8 result, U8 serv_chn);
typedef void (*SdapMultiAttributeCallBack) (SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value);

typedef struct sdap_service_search_struct_t
{
    BtRemoteDevice *rm;
    SdpQueryToken *sqt;
    U16 uuid;
    SdapCallBack callback;
} sdap_service_search_struct;

typedef struct sdap_service_search_multi_attribute_struct_t
{
    BtRemoteDevice *rm;
    SdpQueryToken *sqt;
    U16 uuid;
//#ifdef __BT_SUPPORT_SYNCML__
    U8 uuid128_array[16];
//#endif
    SdapMultiAttributeCallBack callback;
    U8 attribute_num;
    U16 *attribute_id;
} sdap_service_search_multi_attribute_struct;

typedef struct sdap_service_search_all_struct_t
{
    BD_ADDR *bdAddr;
    U8 sdap_len;
    U32 *sdp_uuid;
} sdap_service_search_all_struct;

typedef struct sdap_service_search_single_attribute_struct_t
{
    BD_ADDR *bdAddr;
    U8 uuid_len;
    U16 sdp_uuid;
    U8 sdp_uuid_128[16];
    U16 attribute_id;
} sdap_service_search_single_attribute_struct;

typedef struct sdap_service_search_raw_struct_t
{
    BD_ADDR bdAddr;
    U8 search_type;
    U8 search_pattern[150];
    U8 search_pattern_size;
} sdap_service_search_raw_struct;

typedef struct sdap_context_struct_t
{
    BMCallback bmCB;
    sdap_state progress;            /* SDAP progress */
    U8 current_attribute_index;
    U8 attribute_num;
    U8 multiple_attribute_group_size;
    U8 sdap_len;
	U8 sdap_cur;
    U8 searched_uuid;
    U8 *multiple_attribute_group;
    U16 attribute_offset;
    U16 uuid;
    U16 attribute_id[SDAP_MAX_ATTRIBUTE_NUM];
    U32 sdap_uuid[SDAP_NUM_RECORD_SEARCH];
    U8 sdap_uuid_128[16];
    U8 sdap_uuid_len;
#if BT_JSR82_ENABLED
    U8 search_pattern_size;
    U8 search_type;
    U8 jsr82_sdp_client_contState_Len;
    U8 jsr82_sdp_client_contState_Data[17];
#endif /* BT_JSR82_ENABLED */ 
    sdap_service_search_multi_attribute_struct sdap_multi_req[SDAP_MAX_MULTIPLE_REQ - 1];
    U8 sdap_multi_req_index;
    U8 sdap_current_multi_req_index;
    SdpQueryToken local_sqt;
    SdapCallBack singlecallback;    /* Profile call back function */
    SdapMultiAttributeCallBack multicallback;
    SdpQueryToken *cur_sqt;
    BtDeviceContext *bdc;
    EvmTimer sdap_multi_req_timer;
    EvmTimer sdap_search_raw_retry_timer;
    U16 sdap_search_raw_retry_count;
#ifdef __SDAP_REQUEST_QUEUE__
	ListEntry sdap_request_queue;
#endif
	U32 record_handle;
} sdp_context_struct;

#ifdef __SDAP_REQUEST_QUEUE__
typedef struct {
	ListEntry node;
	BD_ADDR bdAddr;
	U8 req_type;
	U8 reserve1;
	EvmTimer req_timer;
	SdpQueryToken local_sqt;
	union {
		sdap_service_search_all_struct all;
		sdap_service_search_multi_attribute_struct multi_attr;
		sdap_service_search_single_attribute_struct single_attr;
	} params;
} SdapQueuedRequest;
#endif

/*************************************************************************
* FUNCTION                                                            
*  SDAP_Init
* DESCRIPTION                                                           
*   This function is used to call by initialize procedure. This function will initial the context of SDAP
*
* PARAMETERS
*  None
* RETURNS
*  init result
* GLOBALS AFFECTED
*   
*************************************************************************/
BOOL SDAP_Init(void);

/*************************************************************************
* FUNCTION                                                            
*  SDAP_ServiceSearch
* DESCRIPTION                                                           
*   This function is used to search peer device service by SdpQueryToken and uuid
*   The HSP, DUN, SPP, LAP, FAX, AVRCP can use the function to request the service.
*
* PARAMETERS
*      search_pattern: include the information such as 
*      bdAddr : The Bluetooth Address of remote device
*  sqt  : SdpQueryToken contains information such as remote device information. uuid...
*  uuid  : Connect peer device with Profiles == uuid.
*      callback: Profile callback function of search result
* RETURNS
*  U8 : The result status
* GLOBALS AFFECTED
*   
*************************************************************************/
BtStatus SDAP_ServiceSearch(sdap_service_search_struct search_pattern);

/*************************************************************************
* FUNCTION                                                            
*  SDAP_ServiceSearchMultipleAttribute
* DESCRIPTION                                                           
*   The function provides to the profiles  multiple attribute request.
*   The HFP, A2DP can use the function to request the service.
*
* PARAMETERS
*      search_pattern: include the information such as 
*      rm : The Bluetooth remote device
*  sqt  : SdpQueryToken contains information such as remote device information. uuid...
*  uuid  : Connect peer device with Profiles == uuid.
*      callback: Profile callback function of search result
*      attribute_num: The list number of profile interesting
*      attribute_id: The list of attribute id which profile interesting.
* RETURNS
*  U8 : The result status
* GLOBALS AFFECTED
*   
*************************************************************************/
BtStatus SDAP_ServiceSearchMultipleAttribute(sdap_service_search_multi_attribute_struct search_pattern);

BtStatus SDAP_ServiceSearchMultipleAttribute2(sdap_service_search_multi_attribute_struct search_pattern);

/*************************************************************************
* FUNCTION                                                            
*  SDAP_ServiceSearchAll
* DESCRIPTION                                                           
*   The function provides to the MMI to search the remote device supported services.
*
* PARAMETERS
*      search_pattern: include the information such as 
*      rm : The Bluetooth remote device
*  sdap_len  : the total uuid number
*  sdp_uuid  : The search service uuid.
* RETURNS
*  U8 : The result status
* GLOBALS AFFECTED
*   
*************************************************************************/
BtStatus SDAP_ServiceSearchAll(sdap_service_search_all_struct search_pattern);

/*************************************************************************
* FUNCTION                                                            
*  SDAP_ServiceSearchSingleAttribute
* DESCRIPTION                                                           
*   The function provides to the MMI or Java to search the remote device supported services by attribute.
*
* PARAMETERS
*      search_pattern: include the information such as 
*      bdAddr : The Bluetooth Address of remote device
*  sdp_uuid  : The service uuid
*  attribute_id  : The service search attribute.
* RETURNS
*  U8 : The result status
* GLOBALS AFFECTED
*   
*************************************************************************/

BtStatus SDAP_ServiceSearchSingleAttribute(sdap_service_search_single_attribute_struct search_pattern);

/*************************************************************************
* FUNCTION                                                            
*  SDAP_ServiceSearchAllCancel
* DESCRIPTION                                                           
*   The function provides to the MMI to cancel remote device service searching.
*
* PARAMETERS
*  bdAddr: remote device
* RETURNS
*  U8 : The result status
* GLOBALS AFFECTED
*   
*************************************************************************/
BtStatus SDAP_ServiceSearchAllCancel(BD_ADDR *bdAddr);

#endif /* _SDAP_H */ 

