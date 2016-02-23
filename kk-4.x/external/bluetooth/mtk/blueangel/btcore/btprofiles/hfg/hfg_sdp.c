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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfg_sdp.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the SDP code for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
 /*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
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
 * 04 25 2012 huirong.liao
 * [ALPS00270892] [BlueTooth][HFP]DUT Connected with PC "phone audio" will disconnected itself after a while "This issue also exist on MT6577"
 * .
 *
 * Jun 2 2009 mtk80029
 * [MAUI_01837169] [BT] remove kal_prompt_trace
 * add trace
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/
#ifdef __BT_HFG_PROFILE__

#include "stdio.h"
#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "hfg.h"
#include "sdpdb.h"
#include "sdp.h"
#include "sdap.h"
#include "ddb.h"
#include "btutils.h"

#define __BT_FILE__ BT_FILE_HFG_SDP

#if SDP_PARSING_FUNCS == XA_DISABLED
#error The Hands-free Gateway SDK requires the SDP parsing functions.
#endif /* SDP_PARSING_FUNCS == XA_DISABLED */

#define MAX_HFG_DB 20
#define MAX_HSG_DB 20

#define HFG_DB_NAME "HFGDB"
#define HSG_DB_NAME "HSGDB"

typedef struct _hfgSDPRecord{
    BD_ADDR bdAddr;
    U8 channelNo;
    U16 version;
    U32 features;
}hfgSDPRecord, hsgSDPRecord;

hfgSDPRecord g_hfgSDPRecord[MAX_HFG_DB];
hsgSDPRecord g_hsgSDPRecord[MAX_HSG_DB];

ddbStruct g_hfgDB;
ddbStruct g_hsgDB;

static U16 hfg_attribute_id_list[] = 
{
#ifndef HFG_NOT_CHECK_AG_RECORD    
    AID_SERVICE_CLASS_ID_LIST,
#endif    
    AID_PROTOCOL_DESC_LIST, 
    AID_BT_PROFILE_DESC_LIST, 
    AID_SUPPORTED_FEATURES,
};

static U16 hsag_attribute_id_list[] = 
{
#ifndef HFG_NOT_CHECK_AG_RECORD       
    AID_SERVICE_CLASS_ID_LIST,
#endif    
    AID_PROTOCOL_DESC_LIST, 
    AID_BT_PROFILE_DESC_LIST,
};

#ifndef HFG_NOT_CHECK_AG_RECORD
static void hfgQueryPreCallback(SdpQueryToken *sqt, 
                                U8             result, 
                                U8             attribute_index,
                                U8            *attribute_value );                                           
#endif

static void hfgQueryCallback(SdpQueryToken *sqt, 
                             U8             result, 
                             U8             attribute_index,
                             U8            *attribute_value );

extern void bt_hfg_mark_supported_feature(U32 feature_flag);
extern void bt_hfg_unmark_supported_feature(U32 feature_flag);

/* Internal function prototypes */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
U32 HfgGetUuid(U8 *attribute_value)
{
    U32 uuid = 0;
    
    if (attribute_value)
    {
        SdpDataElemSize size = SDP_GetElemSize(attribute_value[0]);
        switch (size)
        {
        case DESD_2BYTES:
            uuid = SDP_GetU16(&attribute_value[1]);
            break;
        case DESD_4BYTES:
            uuid = SDP_GetU32(&attribute_value[1]);            
            break; 
        default:
            break;
        }
        OS_Report("[HFG][SDP] HfgGetUuid size=0x%02x, uuid=0x%x", size, uuid);        
    }
    
    return uuid;
}

/*********************************************
*  0 : No cache
*  1 : Use cache when SDP query return "No Service"
*  2 : Use cache first
**********************************************/
U32 HfgGetSDPCacheType(HfgChannel *Channel){
    U32 cache_type = 1; /* Set default to 1 */
    #ifdef __BT_USE_CUST_DATA__
    if(Channel->cmgrHandler.remDev){
        cache_type = (U32)btGetDevCustvalueById(Channel->cmgrHandler.remDev->devId, CUSTID_HFP_SDP_CACHE);
    }else{
        cache_type = (U32)btGetCustvalue(CUSTID_HFP_SDP_CACHE);
    }
    #endif
    return cache_type;
}

static U8 HfgComparator(const U8 *record, const void *key){
    hfgSDPRecord *hfgRecord = (hfgSDPRecord*)record;
    return OS_MemCmp(hfgRecord->bdAddr.addr, 6, (U8*)key, 6);
}
static U8 HsgComparator(const U8 *record, const void *key){
    hsgSDPRecord *hsgRecord = (hsgSDPRecord*)record;
    return OS_MemCmp(hsgRecord->bdAddr.addr, 6, (U8*)key, 6);
}

BtStatus HfgInitSDPDB(void){
    BtStatus status;
    DBG_PRINT_FLOW(("[DB] HfgInitSDPDB"));
    status = DDB_OpenDB(HFG_DB_NAME, &g_hfgDB, (U8*)g_hfgSDPRecord, sizeof(hfgSDPRecord), MAX_HFG_DB, HfgComparator);
    if(status != BT_STATUS_SUCCESS){
        DBG_PRINT_ERROR(("[DB] open HFG DB failed"));
    }
    status = DDB_OpenDB(HSG_DB_NAME, &g_hsgDB, (U8*)g_hsgSDPRecord, sizeof(hsgSDPRecord), MAX_HSG_DB, HsgComparator);
    if(status != BT_STATUS_SUCCESS){
        DBG_PRINT_ERROR(("[DB] open HSG DB failed"));
    }
    return status;
}

BtStatus HfgCloseSDPDB(void){
    BtStatus status;
    DBG_PRINT_FLOW(("[DB] HfgCloseSDPDB"));
    status = DDB_CloseDB(&g_hfgDB);
    if(status != BT_STATUS_SUCCESS){
        DBG_PRINT_ERROR(("[DB] close HFG DB failed"));
    }
    status = DDB_CloseDB(&g_hsgDB);
    if(status != BT_STATUS_SUCCESS){
        DBG_PRINT_ERROR(("[DB] close HFG DB failed"));
    }   
    return status;
}

static BtStatus HfgAddSDPRecord(HfgChannel *Channel){
    BtStatus status;
    hfgSDPRecord record;

    DBG_PRINT_FLOW(("[DB] HfgAddSDPRecord"));
    BTBMDumpBdAddr(Channel->bdAddr.addr);
    DBG_PRINT_FLOW(("[DB] channel=%u, version=0x%x, feature=0x%x", Channel->rfServerChannel, Channel->version, Channel->hfFeatures));

    OS_MemCopy(record.bdAddr.addr, Channel->bdAddr.addr, 6);
    record.channelNo = Channel->rfServerChannel;
    record.version = Channel->version;    
    if(Channel->type == AG_TYPE_HF){
        record.features = Channel->hfFeatures;
        status = DDB_AddDBRecord(&g_hfgDB, (U8*)&record, (void*)record.bdAddr.addr);
    }else{
        status = DDB_AddDBRecord(&g_hsgDB, (U8*)&record, (void*)record.bdAddr.addr);
    }
    if(status != BT_STATUS_SUCCESS){
        DBG_PRINT_ERROR(("[DB] HfgAddSDPRecord failed"));
    }else{
        status = DDB_FlushDB((Channel->type == AG_TYPE_HF) ? &g_hfgDB : &g_hsgDB);
        if(status != BT_STATUS_SUCCESS){
            DBG_PRINT_ERROR(("[DB] DDB_FlushDB failed"));
        }
    }
    return status;
}

static BtStatus HfgFindSDPRecord(HfgChannel *Channel, hfgSDPRecord *record){
    BtStatus status;
    DBG_PRINT_FLOW(("[DB] HfgFindSDPRecord"));
    BTBMDumpBdAddr(Channel->bdAddr.addr);
    if(Channel->type == AG_TYPE_HF){
        status = DDB_FindDBRecord(&g_hfgDB, (const void*)Channel->bdAddr.addr, (U8*)record);
    }else{
        status = DDB_FindDBRecord(&g_hsgDB, (const void*)Channel->bdAddr.addr, (U8*)record);
    }
    if(status == BT_STATUS_SUCCESS){
        DBG_PRINT_FLOW(("[DB] record find : channel=%u, version=0x%x, feature=0x%x", record->channelNo, record->version, record->features));
    }
    return status;
}

BtStatus HfgDelSDPRecord(BD_ADDR *bd_addr){
    BtStatus status = BT_STATUS_NOT_FOUND;
    DBG_PRINT_FLOW(("[DB] HfgDelSDPRecord"));
    BTBMDumpBdAddr(bd_addr->addr);
    if( DDB_DeleteDBRecord(&g_hfgDB, (const void*)bd_addr->addr) == BT_STATUS_SUCCESS ){
        DBG_PRINT_FLOW(("[DB] HFG record deleted"));
        DDB_FlushDB(&g_hfgDB);
        status = BT_STATUS_SUCCESS;
    }
    if( DDB_DeleteDBRecord(&g_hsgDB, (const void*)bd_addr->addr) == BT_STATUS_SUCCESS ){
        DBG_PRINT_FLOW(("[DB] HSG record deleted"));
        DDB_FlushDB(&g_hsgDB);
        status = BT_STATUS_SUCCESS;
    }
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgRegisterSdpServices()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers the SDP services.
 *
 * Return:    See SDP_AddRecord().
 */
BtStatus HfgRegisterSdpServices(BOOL bHeadset)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 type = bHeadset ? AG_TYPE_HS : AG_TYPE_HF;

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_REGISTER_SDP, bHeadset ? 1 : 0);

    if(type == AG_TYPE_HF)
    {
#ifdef __BT_USE_CUST_DATA__
        if((U32)btGetCustvalue(CUSTID_HFP_VOICE_RECOGNITION))
        {
            bt_hfg_mark_supported_feature(HFG_FEATURE_VOICE_RECOGNITION);
        }
        else
        {
            bt_hfg_unmark_supported_feature(HFG_FEATURE_VOICE_RECOGNITION);
        }
#endif    
    
#ifdef __BT_USE_CUST_DATA__
        if(CMGR_IsEnhancedAudioSupported() && (U32)btGetCustvalue(CUSTID_HFP_MSBC_CODEC))
#else
        if(CMGR_IsEnhancedAudioSupported())
#endif        
        {
            bt_hfg_mark_supported_feature(HFG_FEATURE_CODEC_NEGOTIATION);
        }
        else
        {
            bt_hfg_unmark_supported_feature(HFG_FEATURE_CODEC_NEGOTIATION);
        } 
    }
   
    if(HFG(hfgSdpRecord)[type].num == 0)
    {
        U16 serv = bHeadset ? SC_HEADSET_AUDIO_GATEWAY : SC_HANDSFREE_AUDIO_GATEWAY;
        HFG(hfgSdpRecord)[type].num = SDPDB_GetLocalServiceAttribute(serv,
                                                                    &(HFG(hfgSdpRecord)[type].attribs),
                                                                    &(HFG(hfgService)[type].serviceId));
        if(HFG(hfgSdpRecord)[type].num > 0)
        {
            status = SDP_AddRecord(&(HFG(hfgSdpRecord)[type]));
            if(status != BT_STATUS_SUCCESS)
            {
                kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
            }
        }
        else
        {
            kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, HFG(hfgSdpRecord)[type].num);
            Assert(HFG(hfgSdpRecord)[type].num > 0);
            status = BT_STATUS_FAILED;
        }            
    }
    else
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN, __BT_FILE__, __LINE__);
    }
    
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgDeregisterSdpServices()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters the SDP services.
 *
 * Return:    See SDP_RemoveRecord().
 */
BtStatus HfgDeregisterSdpServices( BOOL bHeadset )
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 type = bHeadset ? AG_TYPE_HS : AG_TYPE_HF;
    
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DEREGISTER_SDP, bHeadset ? 1 : 0);
    /* Remove the Hands-free entry */
    if(HFG(hfgSdpRecord)[type].num > 0)
    {
        status = SDP_RemoveRecord(&(HFG(hfgSdpRecord)[type]));
        if(status != BT_STATUS_SUCCESS)
        {       
            kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
        }
    }
    /* As a mark that the SDP record is not registered  */
    /* num and serviceId are all get through HfgRegisterSdpService, so we clear these to 0 */
    HFG(hfgSdpRecord)[type].num = 0;
    HFG(hfgService)[type].serviceId = 0;
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate the SDP service query.
 *
 * Return:    See SDP_Query().
 */
BtStatus HfgStartServiceQuery(HfgChannel *Channel, SdpQueryMode mode)
{
    BtStatus status;
    BOOL bHeadset;
    sdap_service_search_multi_attribute_struct search_pattern;
    U32 cache_type = 2;
    hfgSDPRecord record;
        
    bHeadset = (Channel->type == AG_TYPE_HS);

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_START_SERVICE_QUERY, Channel, mode, bHeadset ? 1 : 0);

    #ifdef __ENABLE_HFG_SDP_CACHE__
    if(HfgGetSDPCacheType(Channel) == 2){
        status = HfgFindSDPRecord(Channel, &record);
        if( status == BT_STATUS_SUCCESS){
            DBG_PRINT_FLOW(("[HFG][FLOW] Find SDP record. Use cached record"));
            Channel->rfServerChannel = record.channelNo;
            Channel->version = record.version;
            Channel->hfFeatures = record.features;
            goto done;
        }
    }else{
        DBG_PRINT_FLOW(("[HFG][FLOW] cache type=%u. Start SDP query", cache_type));
    }   
    #endif
    /* Reset the query flag */
    Channel->queryFlags = 0x00;
    search_pattern.rm = Channel->cmgrHandler.remDev;
    search_pattern.sqt = &Channel->sdpQueryToken;
    search_pattern.uuid = bHeadset ? SC_HEADSET : SC_HANDSFREE;
#ifndef HFG_NOT_CHECK_AG_RECORD
    search_pattern.callback = hfgQueryPreCallback;
#else
    search_pattern.callback = hfgQueryCallback;
#endif
    if(bHeadset)
    {
        search_pattern.attribute_num = sizeof(hsag_attribute_id_list)/sizeof(hsag_attribute_id_list[0]);
        search_pattern.attribute_id = hsag_attribute_id_list;
    }
    else
    {
        search_pattern.attribute_num = sizeof(hfg_attribute_id_list)/sizeof(hfg_attribute_id_list[0]);
        search_pattern.attribute_id = hfg_attribute_id_list;        
    }
    status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);    

    DBG_PRINT_FLOW(("[HFG][FLOW] Start SDP query : %d", status));

    if(status == BT_STATUS_PENDING)
    {
        Channel->queryFlags |= SDP_QUERY_FLAG_ONGOING;
    }
    else
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);
    }

done:
    return status;
} 

#ifndef HFG_NOT_CHECK_AG_RECORD
/*************************************************************************
* FUNCTION                                                            
*    hfgQueryPreCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*
* PARAMETERS
*    sqt  : SdpQueryToken contains information such as remote device information. uuid...
*    result  : The service search result.
*      attribute_index: the current request attribute list index
*      attribute_value: points to the search result
* RETURNS
*    None
* LOCAL AFFECTED
*   
*************************************************************************/                                           
static void hfgQueryPreCallback(SdpQueryToken *sqt, 
                                U8             result, 
                                U8             attribute_index,
                                U8            *attribute_value)                                           
{
    HfgChannel     *Channel = ContainingRecord( sqt, HfgChannel, sdpQueryToken );
    BOOL bHeadset = (Channel->type==AG_TYPE_HS); 
    U8 attrNum = bHeadset ? 
                 sizeof(hsag_attribute_id_list)/sizeof(hsag_attribute_id_list[0]) : 
                 sizeof(hfg_attribute_id_list)/sizeof(hfg_attribute_id_list[0]); 
    U32 clsId;
    static U8 hfIdx;
    static BtStatus status;
    
    kal_trace(TRACE_GROUP_5, BT_HFG_SDPQUERY_PRECALLBACK, attribute_index, result);
    if(result == BT_STATUS_SUCCESS)
    {
        clsId = bHeadset ? SC_HEADSET : SC_HANDSFREE;       
        if(sqt->attribId == AID_SERVICE_CLASS_ID_LIST)
        {
            hfIdx = 0;
            while(result == BT_STATUS_SUCCESS && HfgGetUuid(attribute_value) != clsId)
            {
                sqt->mode = BSPM_RESUME;
                result = SDP_ParseAttributes(sqt);
                attribute_value = sqt->valueBuff;
                hfIdx++;
            }
            status = result;
            kal_trace(TRACE_GROUP_5, BT_HFG_SDPQUERY_HF_RECORD_FOUND, hfIdx, result);
        }
        else
        {
            if(status == BT_STATUS_SUCCESS)
            {
                /* If status is success, we keep process next attribute or just pass and callback at final attribute get */
                U8 idx = hfIdx;
                while(idx > 0 && result == BT_STATUS_SUCCESS)
                {
                    sqt->mode = BSPM_RESUME;
                    result = SDP_ParseAttributes(sqt);
                    attribute_value = sqt->valueBuff;
                    idx--;
                }
                status = result;
            }
            kal_trace(TRACE_GROUP_5, BT_HFG_SDPQUERY_HF_ATTRIB_FOUND, status, result);
            /* If status is success or the final attribute is get, callback to HFG */
            if(status == BT_STATUS_SUCCESS || (attribute_index+1) == attrNum)
            {
                /* Attribute found */
                if(status == BT_STATUS_SUCCESS)
                {
                    hfgQueryCallback(sqt, status, attribute_index, attribute_value);
                }
                else
                {
                    hfgQueryCallback(sqt, BT_STATUS_FAILED, attribute_index, attribute_value);
                }
            }
        }
    }
    else
    {
        hfgQueryCallback(sqt, result, attribute_index, attribute_value);
    }
}
#endif

/*************************************************************************
* FUNCTION                                                            
*    hfgQueryCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*   Modified by MTK Dlight mtk01239
*
* PARAMETERS
*    sqt  : SdpQueryToken contains information such as remote device information. uuid...
*    result  : The service search result.
*      attribute_index: the current request attribute list index
*      attribute_value: points to the search result
* RETURNS
*    None
* LOCAL AFFECTED
*   
*************************************************************************/
static void hfgQueryCallback(SdpQueryToken *sqt, 
                             U8             result, 
                             U8             attribute_index,
                             U8            *attribute_value )
{
    BtStatus status;
    HfgChannel     *Channel = ContainingRecord( sqt, HfgChannel, sdpQueryToken );
    BOOL bHeadset = (Channel->type==AG_TYPE_HS);
    U8 completeFlags = bHeadset ? SDP_QUERY_ALL_HS_FLAGS : SDP_QUERY_ALL_HF_FLAGS;
    U16 attrib;

    attrib = bHeadset ? hsag_attribute_id_list[attribute_index] : hfg_attribute_id_list[attribute_index];

    kal_trace(TRACE_GROUP_5, BT_HFG_CBK_SDP_QUERY, sqt, result, attribute_index, attribute_value);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);
    
    switch (result) 
    {
    case BT_STATUS_SUCCESS:
        switch (attrib)
        {
        case AID_PROTOCOL_DESC_LIST:
            Channel->rfServerChannel = attribute_value[0];                    
            Channel->queryFlags |= SDP_QUERY_FLAG_PROTOCOL;
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : AID_PROTOCOL_DESC_LIST, server channel=%d", 
                Channel->rfServerChannel));
            break;
        case AID_BT_PROFILE_DESC_LIST:
            Channel->version = SDP_GetU16(attribute_value);
            Channel->queryFlags |= SDP_QUERY_FLAG_PROFILE;
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : AID_BT_PROFILE_DESC_LIST, version=0x%X", 
                Channel->version));
            break;
        case AID_SUPPORTED_FEATURES:
            /* SDP result parsing completed */
            Channel->queryFlags |= SDP_QUERY_FLAG_FEATURES;
            Channel->hfFeatures = SDP_GetU16(attribute_value);
            if (!(Channel->flags & CHANNEL_FLAG_OUTGOING))
            {
                HfgReportHfFeatures(Channel);
            }                    
            Channel->hfFeatures = SDP_GetU16(attribute_value);
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : AID_SUPPORTED_FEATURES, HF features=0x%X", 
                Channel->hfFeatures));
            break;
        case AID_REMOTE_AUDIO_VOL_CONTROL:
            Channel->bRemoteVolControl = attribute_value[0];
            Channel->queryFlags |= SDP_QUERY_FLAG_VOL_CONTROL;
            break;
        default:
            kal_trace(TRACE_GROUP_5, BT_HFG_WARN_UNRECOGNIZED_SDP_ATTR, attrib);
            break;
        }
        break;
    case BT_STATUS_CONNECTION_FAILED:
        DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : BT_STATUS_CONNECTION_FAILED"));
        break;
    case BT_STATUS_FAILED:
        DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : BT_STATUS_FAILED"));
        break;
    case BT_STATUS_NOSERVICES:
        DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : BT_STATUS_NOSERVICES"));
        break; 
    default:
        DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : unknown result : %d", result));
        break;
    }

#ifdef __ENABLE_HFG_SDP_CACHE__
    if(HfgGetSDPCacheType(Channel) != 0){
        if( result == BT_STATUS_SUCCESS && (Channel->queryFlags & completeFlags) == completeFlags ){
            hfgSDPRecord record;
            
            if( BT_STATUS_SUCCESS != HfgAddSDPRecord(Channel) ){
                DBG_PRINT_ERROR(("HfgAddSDPRecord failed"));
            }
        }

        /* If return no service, use cached record */
        if(result == BT_STATUS_NOSERVICES){
            hfgSDPRecord record;
            if( BT_STATUS_SUCCESS == HfgFindSDPRecord(Channel, &record) ){
                result = BT_STATUS_SUCCESS;
                Channel->queryFlags = completeFlags;
                Channel->rfServerChannel = record.channelNo;
                Channel->version = record.version;
                if(Channel->type == AG_TYPE_HF)
                    Channel->hfFeatures = record.features;
            }
        }
    }
#endif
    if( result != BT_STATUS_SUCCESS || (Channel->queryFlags & completeFlags) == completeFlags )
    {
        /* SDP query is over then clear ongoing bit  */
        Channel->queryFlags &= ~SDP_QUERY_FLAG_ONGOING;
        kal_trace(TRACE_GROUP_5, BT_HFG_INFO_SDP_QUERY_COMPLETE, Channel->rfServerChannel);
    }

    if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CONN_OUT_W4SDP)
    {
        if(result == BT_STATUS_SUCCESS)
        {
            if( (Channel->queryFlags & completeFlags) == completeFlags )
            {
                /* All attributes are returned */
                status = HfgOpenRfChannel(Channel);
            }
        }
        else
        {
            Channel->reason = HFG_DISCREASON_SDP;
            HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
        }
    }
    /* WCNCR00000458: VW polo only supports HFP 0.99, if VW initiates HFP connection, 
       AG will trigger SDP query and the HFG state will go wrong. Merge HFG code from
       wise_sp */
    else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN_W4SDP)
    {
        if( result != BT_STATUS_SUCCESS || (Channel->queryFlags & completeFlags) == completeFlags ){
            HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN);
        }
    }
    else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN){
        /* Do nothing. Only waiting for negotioation done */
    }
    else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC || HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC_W4SCO)
    {
        /* if SDP query in incoming connection, the negotiation may complete before SDP query completed */
    }
    else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC)
    {
        HfgDisconnecting(Channel);
    }
    else
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, Channel, HfgGetSubstate(Channel));
    }    
}
#endif /* __BT_HFG_PROFILE__ */
