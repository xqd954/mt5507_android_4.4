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
*  permission of MediaTek Inc. (C) 2007
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * Appsdpdb.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * 
 *
 * Author:
 * -------
 * Dlight Ting
 *
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
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * Jun 27 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Apr 12 2008 mtk01411
 * [MAUI_00293219] Assert fail: bt_osapi.c 624 - BT in func SdpFindUuidInAttrib: Assert (i == len)
 * 
 *
 * Feb 22 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "bt_common.h"
#include "stdio.h"
#include "bluetooth_sdp_message.h"
#include "bluetooth_sdp_struct.h"
#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "bt_jsr82_setting.h"
#include "sdpdb.h"
#include "sdp.h"
#include "appsdpdb.h"

#if BT_JSR82_ENABLED

static BT_APP_SDPDB_CONTEXT_struct_t bt_app_sdpdb_context;

static void jsr82_bt_sendMsg(
                msg_type msg,
                module_type dstMod,
                sap_type sap,
                local_para_struct *local_para,
                peer_buff_struct *peer_buff);

//#define BTAPPSDPDB(f)  (bt_app_sdpdb_context.f)


/*****************************************************************************
 * FUNCTION
 *  BTAppSdpDbInit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppSdpDbInit(void)
{
    U8 i = 0;

    OS_MemSet((void*)&(bt_app_sdpdb_context), 0, sizeof(bt_app_sdpdb_context));
    for (i = 0; i < BT_SDPDB_MAX_SERVICE_RECORD_NO; i++)
    {
        bt_app_sdpdb_context.db[i].AppRecordHandler = 0x00010030 + i;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAppSdpDbDeactivate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppSdpDbDeactivate(void)
{
    U8 i = 0;
    U32 handle;

    for (i = 0; i < BT_SDPDB_MAX_SERVICE_RECORD_NO; i++)
    {
        if ((bt_app_sdpdb_context.db[i].used != 0))
        {
            handle = bt_app_sdpdb_context.db[i].AppRecordHandler;
            /* Find matched service record */
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SDPDBDEACTIVATE_FIND_INDEX_I_TO_UNREGISTERx02X, i);
            bt_app_sdpdb_context.db[i].used = 0;
#ifndef BTMTK_ON_LINUX  /* android no shared memory management yet */
            bt_app_sdpdb_context.db[i].attribute_value = 0;
#endif
            SDP_RemoveRecord(&(bt_app_sdpdb_context.db[i].AppSdpRecord));
            /* Remove break statement: Because each entry must be reset: 2008-0102 */
        }
    }
    OS_MemSet((void*)&(bt_app_sdpdb_context), 0, sizeof(bt_app_sdpdb_context));

    /* The following are codes of AppSdpDBInit() */
    for (i = 0; i < BT_SDPDB_MAX_SERVICE_RECORD_NO; i++)
    {
        bt_app_sdpdb_context.db[i].AppRecordHandler = 0x00010030 + i;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleGetAvailableRecordHandler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleGetAvailableRecordHandler(bt_app_sdpdb_get_handle_req_struct *req)
{
    U8 i = 0;
    U32 handle = 0;
	I32 identify = req->identify;

    for (i = 0; i < BT_SDPDB_MAX_SERVICE_RECORD_NO; i++)
    {
        /* Check free and available service record */
        if (bt_app_sdpdb_context.db[i].used == 0)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_GETAVAILABLERECORDHANDLER_FIND_INDEX_I_AVAILABLEx02X, i);
            kal_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_GETAVAILABLERECORDHANDLER_RECORD_HANDLERx08X,
                bt_app_sdpdb_context.db[i].AppRecordHandler);
            bt_app_sdpdb_context.db[i].used = 1;
            handle = bt_app_sdpdb_context.db[i].AppRecordHandler;
            break;
        }
    }
    if (i == BT_SDPDB_MAX_SERVICE_RECORD_NO)
    {
        BTAppHandleGetRecordHandleResult(BTSDPDBAPP_SDP_DDB_FULL, identify, 0);
    }
    else
    {
        BTAppHandleGetRecordHandleResult(BTSDPDBAPP_SUCCESS, identify, handle);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleRegisterRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleRegisterRecord(bt_app_sdpdb_register_req_struct *ptr)
{
    U32 handle = 0;
    U8 type = 0;
    U8 *record_raw = NULL;
    U16 record_raw_length = 0;
    U8 *attribs_buffer = NULL;
    U16 attribs_buffer_size = 0;
    U8 record_index = 0, i = 0;
    U8 attribute_no = 0;
    U8 *record_attribute_ptr = 0;
    BtStatus status = BT_STATUS_FAILED;
    SdpAttribute *attribs_ptr = NULL;
    SdpRecord *service_record = 0;
    BT_APP_ATTRIBUTE_struct_t attribute_buffer[BT_APP_MAX_ATTRIBUTE_SIZE];

    OS_MemSet((U8*) attribute_buffer, 0, sizeof(BT_APP_ATTRIBUTE_struct_t) * BT_APP_MAX_ATTRIBUTE_SIZE);

    handle = ptr->handle;
    type = ptr->type;
#ifndef BTMTK_ON_LINUX 
    record_raw = ptr->record_raw;
    record_raw_length = ptr->record_raw_length;
    /* ptr->attribs: points to the memory space is allocated by JBT and this buffer space is used to registration SDP record */
	/* android no shared memory management yet */
    attribs_buffer = ptr->attribs_buffer;
    attribs_buffer_size = ptr->attribs_buffer_size;
#endif

    for (i = 0; i < BT_SDPDB_MAX_SERVICE_RECORD_NO; i++)
    {
        /* Check free and available service record */
        /*
         * bt_app_sdpdb_context.db[i].AppRecordHandler: It is just for Java Application to identify
         * Before java application registration its sdp record, it must obtain this AppRecordHandler first to ensure one available entry can be used to register to native BT SDPDB
         * The actual service record handle published to other device is determined by SDP_AddRecord()'s SDPS(nextHandle) in native BT stack
         */
        if ((bt_app_sdpdb_context.db[i].used != 0) && (bt_app_sdpdb_context.db[i].AppRecordHandler == handle))
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_FIND_INDEX_I_AVAILABLEx02X, i);
            record_index = i;
#ifdef BTMTK_ON_LINUX  /* android no shared memory management yet */
            attribs_buffer = bt_app_sdpdb_context.db[i].attribute_value;
            attribs_buffer_size = BT_APP_MAX_RECORD_SIZE;
            memcpy(bt_app_sdpdb_context.db[i].raw_data, ptr->record_raw, ptr->record_raw_length);
            record_raw = bt_app_sdpdb_context.db[i].raw_data;
            record_raw_length = ptr->record_raw_length;
#endif
            break;
        }
    }
    if (i == BT_SDPDB_MAX_SERVICE_RECORD_NO)
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_NO_AVAILABLE_SERVICE_RECORD);
        /* No available service record  */
        BTAppHandleRegisterRecordResult(BTSDPDBAPP_INVALID_HANDLE, handle);
        return;
    }
    service_record = &bt_app_sdpdb_context.db[i].AppSdpRecord;
    Assert(service_record != 0);

    if (record_raw_length > BT_APP_MAX_RECORD_SIZE)
    {
        BTAppHandleRegisterRecordResult(BTSDPDBAPP_SDP_RECORD_TOO_LARGE, handle);
        return;
    }
    if (BTAppVerifyApplicationRecord(record_raw, record_raw_length, &attribute_no, (U8*) & attribute_buffer))
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_THE_ATTRIBUTE_NO_ISx02X, attribute_no);
        if (attribs_buffer_size < sizeof(SdpAttribute) * attribute_no)
        {
            BTAppHandleRegisterRecordResult(BTSDPDBAPP_SDP_RECORD_ATTRIBUTE_BUFFER_TOO_SMALL, handle);
            return;
        }
        if (type == BT_APP_UPDATE_RECORD)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_UPDATE_THE_RECORD);
            status = SDP_RemoveRecord(&(bt_app_sdpdb_context.db[i].AppSdpRecord));
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_REMOVE_STATUSxD, status);
        }
        /* points to memory space allocated in JBT and this buffer will not freed until to deregister this record */
#ifndef BTMTK_ON_LINUX  /* android no shared memory management yet */
        bt_app_sdpdb_context.db[i].attribute_value = attribs_buffer;
#endif
        record_attribute_ptr = bt_app_sdpdb_context.db[i].attribute_value;

        /* Once return from BTAppVerifyApplicationRecord() successfully, the atrribute_buffer stores the valid syntax attribute id and its corresponding atrribute value */
        /* The attribute_no will not include the aid = 0x0000, because the service_record_handle attribute will not obtain from app */
        for (i = 0; i < attribute_no; i++)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_INDEXxD, i);
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_ATTRIBUTE_IDxD, attribute_buffer[i].attribute_id);
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_ATTRIBUTE_SIZExD, attribute_buffer[i].attribute_size);
            attribs_ptr = (SdpAttribute*) (record_attribute_ptr + i * sizeof(SdpAttribute));
            attribs_ptr->id = attribute_buffer[i].attribute_id;
            attribs_ptr->len = attribute_buffer[i].attribute_size;
            attribs_ptr->value = attribute_buffer[i].attribute_value;
            attribs_ptr->flags = 0x0000;
        }
        bt_app_sdpdb_context.db[record_index].AppSdpRecord.num = attribute_no;
        bt_app_sdpdb_context.db[record_index].AppSdpRecord.attribs =
            (SdpAttribute*) bt_app_sdpdb_context.db[record_index].attribute_value;
        status = SDP_AddRecord(&(bt_app_sdpdb_context.db[record_index].AppSdpRecord));
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_SDP_ADDRECORD_STATUSx02X, status);
        if (status == BT_STATUS_SUCCESS)
        {
            BTAppHandleRegisterRecordResult(BTSDPDBAPP_SUCCESS, handle);
        }
        else
        {
            BTAppHandleRegisterRecordResult(BTSDPDBAPP_SDP_REGISTER_FAILED, handle);
        }
    }
    else
    {
        BTAppHandleRegisterRecordResult(BTSDPDBAPP_SDP_RECORD_SYNTAX_ERROR, handle);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_RECORD_RECORD_PARSE_ERROR);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleDeRegisterRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleDeRegisterRecord(bt_app_sdpdb_deregister_req_struct *ptr)
{
    U8 i = 0;
    BtStatus status = BT_STATUS_FAILED;
    U32 handle = 0;

    handle = ptr->handle;

    for (i = 0; i < BT_SDPDB_MAX_SERVICE_RECORD_NO; i++)
    {
        if ((bt_app_sdpdb_context.db[i].used != 0) && (bt_app_sdpdb_context.db[i].AppRecordHandler == handle))
        {
            /* Find matched service record */
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DEREG_RECORD_FIND_INDEX_I_TO_UNREGISTERx02X, i);

            bt_app_sdpdb_context.db[i].used = 0;
#ifdef BTMTK_ON_LINUX  /* android no shared memory management yet */
            OS_MemSet(bt_app_sdpdb_context.db[i].attribute_value, 0, BT_APP_MAX_RECORD_SIZE);
            OS_MemSet(bt_app_sdpdb_context.db[i].raw_data, 0, BT_APP_MAX_RECORD_SIZE);
#else
            bt_app_sdpdb_context.db[i].attribute_value = 0;
#endif
            status = SDP_RemoveRecord(&(bt_app_sdpdb_context.db[i].AppSdpRecord));
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DEREG_RECORD_STATUSx02X, status);
            break;
        }
    }
    if (i == BT_SDPDB_MAX_SERVICE_RECORD_NO)
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DEREG_RECORD_NO_REGISTERED_SERVICE_RECORD);
        BTAppHandleDeRegisterRecordResult(BTSDPDBAPP_INVALID_HANDLE, handle);
        return;
    }
    if (status == BT_STATUS_INVALID_PARM)
    {
        BTAppHandleDeRegisterRecordResult(BTSDPDBAPP_FAILED, handle);
    }
    else if (status == BT_STATUS_FAILED)
    {
        BTAppHandleDeRegisterRecordResult(BTSDPDBAPP_SDP_DEREGISTER_FAILED, handle);
    }
    else
    {
        BTAppHandleDeRegisterRecordResult(BTSDPDBAPP_SUCCESS, handle);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAppVerifyApplicationRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  raw_data            [IN]        
 *  raw_data_length     [IN]        
 *  attribute_no        [IN]        
 *  ptr                 [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL BTAppVerifyApplicationRecord(U8 *raw_data, U16 raw_data_length, U8 *attribute_no, U8 *ptr)
{
    U16 attr_id = 0;
    U16 offset = 0;
    U16 element_size = 0;
    U16 element_header_size = 0;
    U8 *data = 0;
    BOOL result = TRUE;
    U16 i = 0;
    BOOL attribute_data_element = FALSE;
    U16 attribute_len = 0;
    U8 attribute_type = 0;
    U8 size_descriptor = 0;
    U8 attribute_index = 0;
    BT_APP_ATTRIBUTE_struct_t *attribute_buffer = NULL;

    attribute_index = 0;
    *attribute_no = 0;
    kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_RECORD_LENGTHx04X, raw_data_length);
    for (i = 0; i < raw_data_length; i += 8)
    {
        kal_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_VERIFY_RECORD_ITS_DATAx02Xx02Xx02Xx02Xx02Xx02Xx02Xx02X,
            raw_data[i],
            raw_data[i + 1],
            raw_data[i + 2],
            raw_data[i + 3],
            raw_data[i + 4],
            raw_data[i + 5],
            raw_data[i + 6],
            raw_data[i + 7]);
    }

    /* attribute_len: attribute value's length; +2: attribute ID's length is two bytes */
    for (offset = 0; offset < raw_data_length; offset += (attribute_len + 2))
    {
        if (attribute_index > BT_APP_MAX_ATTRIBUTE_SIZE)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_ATTRIBUTE_INDEX_xD_TOO_LARGE, attribute_index);
        }
        data = (U8*) (raw_data + offset);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_DATA0x02X_DATA1x02X, data[0], data[1]);
        attr_id = ((((U16) data[0]) << 8) | ((U16) data[1]));
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_THE_ATTRIBUTE_ID_xD, attr_id);
        data += 2;
        /* Retrieve attribute id */
        (*attribute_no)++;
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_THE_ELEMENT_TYPE_x02X, data[0]);
        attribute_data_element = FALSE;
        /* Retrieve attribute value size index and type descriptor */
        size_descriptor = SDP_GetElemSize(data[0]);
        attribute_type = SDP_GetElemType(data[0]);

        switch (attribute_type)
        {
            case DETD_NIL:
                element_header_size = 1;
                if (size_descriptor != DESD_1BYTE)
                {
                    result = FALSE;
                }
                else
                {
                    element_size = 0;
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_A_THE_ELEMENT_SIZE_x02X, element_size);
                break;
            case DETD_BOOL:
                element_header_size = 1;
                if (size_descriptor != DESD_1BYTE)
                {
                    result = FALSE;
                }
                else
                {
                    element_size = BTAppSdpGetValidSize(size_descriptor);
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_A_THE_ELEMENT_SIZE_x02X, element_size);
                break;
            case DETD_UINT:
            case DETD_SINT:
                element_header_size = 1;
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_B_THE_ELEMENT_SIZE_x02X, element_size);
                if (size_descriptor > DESD_16BYTES)
                {
                    result = FALSE;
                }
                else
                {
                    element_size = BTAppSdpGetValidSize(size_descriptor);
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_B_THE_ELEMENT_SIZE_x02X, element_size);
                break;
            case DETD_UUID:
                element_header_size = 1;
                if ((size_descriptor != DESD_2BYTES) && (size_descriptor != DESD_4BYTES) &&
                    (size_descriptor != DESD_16BYTES))
                {
                    result = FALSE;
                }
                else
                {
                    element_size = BTAppSdpGetValidSize(size_descriptor);
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_C_THE_ELEMENT_SIZE_x02X, element_size);
                break;
            case DETD_SEQ:  /* Data Element Sequence. Valid Size descriptor: 5,6,7 */
            case DETD_ALT:  /* Data Alternative. Valid Size descriptor: 5,6,7 */
            case DETD_TEXT: /* Text string. Valid Size 5,6,7 */
            case DETD_URL:  /* Uniform Resource Locator. Valid size 5,6,7 */
                element_header_size = SDP_GetElemSize(data[0]);
                kal_trace(
                    BT_TRACE_JSR82_GROUP,
                    BT_JSR82_VERIFY_RECORD_TYPEx02X_SIZE_INDEXx02X,
                    attribute_type,
                    element_header_size);
                if (element_header_size < DESD_ADD_8BITS)
                {
                    result = FALSE;
                }
                else
                {
                    if ((attribute_type == DETD_SEQ) || (attribute_type == DETD_ALT))
                    {
                        attribute_data_element = TRUE;
                    }

                    /* If size descriptor=5,6,7 then its additional size_bytes=1(5-4),2(6-4),4(7-4+1) followed (Type descriptor:5-bits size descriptor:3-bits) */
                    /* element_header_size -= 4; */
                    if (element_header_size == DESD_ADD_8BITS)
                    {
                        element_header_size = 1;
                    }
                    else if (element_header_size == DESD_ADD_16BITS)
                    {
                        element_header_size = 2;
                    }
                    else if (element_header_size == DESD_ADD_32BITS)
                    {
                        element_header_size = 4;
                    }
                    else
                    {
                        kal_trace(
                            BT_TRACE_JSR82_GROUP,
                            BT_JSR82_VERIFY_RECORD_INVALID_SIZE_DESCRIPTOR_VALUEx02X,
                            element_header_size);
                        result = FALSE;
                        return result;
                    }
                    element_size = 0;

                    /*
                     * The for loop is used to obtain the actual data size and stores this actual data size to element_size 
                     * data[0]: Type descriptor(5-bits) + size descriptor(3-bits)
                     * data[1] or data[1]-data[2] or data[1]-data[4] are actual data size
                     */
                    for (i = 0; i < element_header_size; i++)
                    {
                        element_size = (element_size << 8) | (data[1 + i]);
                    }

                    /*
                     * 5-bits (Type descriptor) + 3-bits (size descriptor) + 1 or 2 or 4 bytes (actual data size)
                     * +1: it means that this one byte: 5-bits (Type descriptor) + 3-bits (size descriptor)
                     */
                    element_header_size += 1;

                    if ((element_header_size + element_size) > (raw_data_length - offset))
                    {
                        result = FALSE;
                        break;
                    }

                    kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_E_THE_ELEMENT_SIZE_x02X, element_size);
                }
                break;
            default:
                result = FALSE;
                break;
        }
        if (result)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_ATTRIBUTE_VALUE_ELEMENT_SIZE_x04X, element_size);
            kal_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_VERIFY_RECORD_ATTRIBUTE_VALUE_ELEMENT_HEADER_SIZE_x04X,
                element_header_size);
            if ((attribute_data_element) && (element_size > 0) && (element_size < BT_APP_MAX_DATA_ELEMENT_SIZE))
            {
                /* Process the attribute_value part and its data type belongs to Data Element Sequence (Because attribute_data_element is TRUE) */
                if (!BTAppVerifyApplicationDataElement(data + element_header_size, element_size, 0))
                {
                    result = FALSE;
                    return FALSE;
                }

            }
            attribute_data_element = FALSE;
        }
        /* The BTstack will automatically assign the record handle of the record in SDP_AddRecord()'s SDPS(nextHandle) */
        /* From SdpMarkAttribute(): It can be known that: aid= 0x0000 and aid= 0x0002 are assigned by BTStack */
        if (attr_id != AID_SERVICE_RECORD_HANDLE && attr_id != AID_SERVICE_RECORD_STATE)
        {
            attribute_buffer =
                (BT_APP_ATTRIBUTE_struct_t*) (ptr + sizeof(BT_APP_ATTRIBUTE_struct_t) * attribute_index);
            attribute_buffer->attribute_id = attr_id;
            attribute_buffer->attribute_value = data;
            attribute_buffer->attribute_size = element_size + element_header_size;
            attribute_index++;
        }
        attribute_len = (element_size + element_header_size);
    }
    if (offset != raw_data_length)
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_VERIFY_FAILED);
        return FALSE;
    }
    else
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_RECORD_FINALLY_ATTRIBUTE_INDEXx02X, attribute_index);
        BTAppSdpCreateSortingAttributes(ptr, attribute_index);
        /* Note 2008-0410: actual stored attribute no is equal to attribute_index (aid=0x0000 is not stored) */
        (*attribute_no) = attribute_index;
        return TRUE;

    }

}


/*****************************************************************************
 * FUNCTION
 *  BTAppVerifyApplicationDataElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  raw_data            [IN]        
 *  raw_data_length     [IN]        
 *  deepth              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL BTAppVerifyApplicationDataElement(U8 *raw_data, U16 raw_data_length, U8 deepth)
{
    U16 current_len = 0;
    U16 attribute_len = 0;
    U16 element_size = 0;
    U16 element_header_size = 1;
    BOOL result = TRUE;
    U8 *data = 0;
    U8 i = 0;
    U8 size_descriptor;
    U8 attribute_type;

    if (deepth > BT_APP_MAX_DEEPTH_SIZE)
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_DATA_ELEMENT_TOO_LARGE_NOT_CHECK);
        return TRUE;
    }

    for (current_len = 0; current_len < raw_data_length; current_len += attribute_len)
    {
        if (current_len > raw_data_length)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_INVALID_PARAMETERS);
            result = FALSE;
            break;
        }
        data = (U8*) & (raw_data[current_len]);
        size_descriptor = SDP_GetElemSize(data[0]);
        attribute_type = SDP_GetElemType(data[0]);

        switch (attribute_type)
        {
            case DETD_NIL:
            case DETD_BOOL:
                if (size_descriptor != DESD_1BYTE)
                {
                    result = FALSE;
                }
                else
                {
                    element_size = BTAppSdpGetValidSize(size_descriptor);
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_X1_THE_ELEMENT_SIZE_x02X, element_size);
                element_header_size = 1;
                break;
            case DETD_UINT:
            case DETD_SINT:
                if (size_descriptor > DESD_16BYTES)
                {
                    result = FALSE;
                }
                else
                {
                    element_size = BTAppSdpGetValidSize(size_descriptor);
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_X2_THE_ELEMENT_SIZE_x02X, element_size);
                element_header_size = 1;
                break;
            case DETD_UUID:
                if ((size_descriptor != DESD_2BYTES) && (size_descriptor != DESD_4BYTES) &&
                    (size_descriptor != DESD_16BYTES))
                {
                    result = FALSE;
                }
                else
                {
                    element_size = BTAppSdpGetValidSize(size_descriptor);
                }
                kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_X3_THE_ELEMENT_SIZE_x02X, element_size);
                element_header_size = 1;
                break;
            case DETD_SEQ:
            case DETD_ALT:
            case DETD_TEXT: /* Text string. Valid Size 5,6,7 */
            case DETD_URL:  /* Uniform Resource Locator. Valid size 5,6,7 */
                element_header_size = SDP_GetElemSize(data[0]);
                kal_trace(
                    BT_TRACE_JSR82_GROUP,
                    BT_JSR82_VERIFY_DATA_ELEMENT_TYPEx02X_SIZE_INDEXx02X,
                    attribute_type,
                    element_header_size);
                if (element_header_size < DESD_ADD_8BITS)
                {
                    result = FALSE;
                }
                else
                {

                    /* If size descriptor=5,6,7 then its additional size_bytes=1(5-4),2(6-4),4 (7-4+1) followed (Type descriptor:5-bits size descriptor:3-bits) */
                    /* element_header_size -= 4; */
                    if (element_header_size == DESD_ADD_8BITS)
                    {
                        element_header_size = 1;
                    }
                    else if (element_header_size == DESD_ADD_16BITS)
                    {
                        element_header_size = 2;
                    }
                    else if (element_header_size == DESD_ADD_32BITS)
                    {
                        element_header_size = 4;
                    }
                    else
                    {
                        kal_trace(
                            BT_TRACE_JSR82_GROUP,
                            BT_JSR82_VERIFY_DATA_ELEMENT_INVALID_SIZE_DESCRIPTOR_VALUEx02X,
                            element_header_size);
                        result = FALSE;
                        return result;
                    }
                    element_size = 0;
                    for (i = 0; i < element_header_size; i++)
                    {
                        element_size = (element_size << 8) | (data[1 + i]);
                    }

                    /* Currently, element_header_size is number of bytes to store actual data length, +1: 5-bits Type descriptor + 3-bits Size descriptor */
                    element_header_size += 1;

                    if ((element_header_size + element_size) > (raw_data_length - current_len))
                    {
                        result = FALSE;
                        break;
                    }
                    kal_trace(
                        BT_TRACE_JSR82_GROUP,
                        BT_JSR82_VERIFY_DATA_ELEMENT_X5_THE_ELEMENT_SIZE_x02X,
                        element_size);
                    if (((attribute_type == DETD_SEQ) || (attribute_type == DETD_ALT)) && (element_size > 0))
                    {
                        if (!BTAppVerifyApplicationDataElement((U8*) & data[element_header_size], element_size, (U8) (deepth + 1)))
                        {
                            result = FALSE;
                        }
                    }
                }
                break;
            default:
                result = FALSE;
                break;
        }
        if (!result)
        {
            break;
        }
        if (result)
        {
            kal_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_VERIFY_DATA_ELEMENT_ATTRIBUTE_VALUE_ELEMENT_SIZE_x02X,
                element_size);
            attribute_len = (element_header_size + element_size);
        }
        if (!result)
        {
            kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_VERIFY_FAILED);
            return FALSE;
        }
    }
    if (current_len != raw_data_length)
    {
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VERIFY_DATA_ELEMENT_PEASE_DATA_ELEMENT_FAILED);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTAppSdpGetValidSize
 * DESCRIPTION
 *  
 * PARAMETERS
 *  size_descriptor     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 BTAppSdpGetValidSize(U8 size_descriptor)
{
    U8 element_size = 0;

    switch (size_descriptor)
    {
        case DESD_1BYTE:
            element_size = 1;
            break;
        case DESD_2BYTES:
            element_size = 2;
            break;
        case DESD_4BYTES:
            element_size = 4;
            break;
        case DESD_8BYTES:
            element_size = 8;
            break;
        case DESD_16BYTES:
            element_size = 16;
            break;
    }
    return element_size;

}


/*****************************************************************************
 * FUNCTION
 *  BTAppSdpCreateSortingAttributes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr                 [IN]        
 *  attribute_no        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppSdpCreateSortingAttributes(U8 *ptr, U8 attribute_no)
{
    U8 i = 0;
    U8 j = 0;
    BT_APP_ATTRIBUTE_struct_t temp_buffer;
    BT_APP_ATTRIBUTE_struct_t *item1;
    BT_APP_ATTRIBUTE_struct_t *item2;

    for (i = 0; i < attribute_no; i++)
    {
        item1 = (BT_APP_ATTRIBUTE_struct_t*) (ptr + sizeof(BT_APP_ATTRIBUTE_struct_t) * i);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_INDEXxD, i);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_ATTRIBUTE_IDxD, item1->attribute_id);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_ATTRIBUTE_SIZExD, item1->attribute_size);
    }
    for (i = 0; i < attribute_no; i++)
    {
        for (j = i; j < attribute_no; j++)
        {
            item1 = (BT_APP_ATTRIBUTE_struct_t*) (ptr + sizeof(BT_APP_ATTRIBUTE_struct_t) * i);
            item2 = (BT_APP_ATTRIBUTE_struct_t*) (ptr + sizeof(BT_APP_ATTRIBUTE_struct_t) * j);
            if (item1->attribute_id > item2->attribute_id)
            {
                OS_MemSet((U8*) & temp_buffer, 0, sizeof(temp_buffer));
                OS_MemCopy((U8*) & temp_buffer, (U8*) item1, sizeof(BT_APP_ATTRIBUTE_struct_t));
                OS_MemCopy((U8*) item1, (U8*) item2, sizeof(BT_APP_ATTRIBUTE_struct_t));
                OS_MemCopy((U8*) item2, (U8*) & temp_buffer, sizeof(BT_APP_ATTRIBUTE_struct_t));
            }
        }
    }
    kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_AFTER_SORTING);
    for (i = 0; i < attribute_no; i++)
    {
        item1 = (BT_APP_ATTRIBUTE_struct_t*) (ptr + sizeof(BT_APP_ATTRIBUTE_struct_t) * i);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_INDEXxD, i);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_ATTRIBUTE_IDxD, item1->attribute_id);
        kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SORT_ATTIBS_ATTRIBUTE_SIZExD, item1->attribute_size);
    }

}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleGetRecordHandleResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  handle      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleGetRecordHandleResult(kal_uint8 result,kal_uint32 identify,  kal_uint32 handle)
{
    bt_app_sdpdb_get_handle_cnf_struct *ptr;

    ptr =
        (bt_app_sdpdb_get_handle_cnf_struct*) construct_local_para(sizeof(bt_app_sdpdb_get_handle_cnf_struct), TD_UL);
    ptr->result = result;
    ptr->handle = handle;
	ptr->identify = identify;
    jsr82_bt_sendMsg(MSG_ID_BT_APP_SDPDB_GET_HANDLE_CNF, MOD_JASYN, BT_APP_SAP, (local_para_struct*) ptr, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleRegisterRecordResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  handle      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleRegisterRecordResult(kal_uint8 result, kal_uint32 handle)
{
    bt_app_sdpdb_register_cnf_struct *ptr;

    ptr = (bt_app_sdpdb_register_cnf_struct*) construct_local_para(sizeof(bt_app_sdpdb_register_cnf_struct), TD_UL);
    ptr->result = result;
    ptr->handle = handle;
    jsr82_bt_sendMsg(MSG_ID_BT_APP_SDPDB_REGISTER_CNF, MOD_JASYN, BT_APP_SAP, (local_para_struct*) ptr, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleRetrieveRecordResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 *  handle          [IN]        
 *  data            [IN]        
 *  data_size       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleRetrieveRecordResult(kal_uint8 result, kal_uint32 handle, kal_uint8 *data, kal_uint16 data_size)
{
    bt_app_sdpdb_retrieve_record_cnf_struct *ptr;

    ptr =
        (bt_app_sdpdb_retrieve_record_cnf_struct*)
        construct_local_para(sizeof(bt_app_sdpdb_retrieve_record_cnf_struct), TD_UL);
    ptr->result = result;
    ptr->handle = handle;
    ptr->record = data;
    ptr->record_size = data_size;
    jsr82_bt_sendMsg(MSG_ID_BT_APP_SDPDB_RETRIEVE_RECOED_CNF, MOD_JASYN, BT_APP_SAP, (local_para_struct*) ptr, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleDeRegisterRecordResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  handle      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleDeRegisterRecordResult(kal_uint8 result, kal_uint32 handle)
{
    bt_app_sdpdb_deregister_cnf_struct *ptr;

    ptr =
        (bt_app_sdpdb_deregister_cnf_struct*) construct_local_para(sizeof(bt_app_sdpdb_deregister_cnf_struct), TD_UL);
    ptr->result = result;
    ptr->handle = handle;
    jsr82_bt_sendMsg(MSG_ID_BT_APP_SDPDB_DEREGISTER_CNF, MOD_JASYN, BT_APP_SAP, (local_para_struct*) ptr, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTAppHandleRetrieveRecordCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTAppHandleRetrieveRecordCnf(bt_app_sdpdb_retrieve_record_cnf_struct *ptr)
{
    U16 i = 0;

    kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RETRIEVERECORDCNF_RESULTx02X, ptr->result);
    kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RETRIEVERECORDCNF_HANDLEx08X, ptr->handle);
    kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RETRIEVERECORDCNF_RECORD_SIZExDX, ptr->record_size);
    for (i = 0; i < ptr->record_size; i += 4)
    {
        kal_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_RETRIEVERECORDCNF_RAW_DATAx02Xx02X_x02Xx02X,
            ptr->record[i],
            ptr->record[i + 1],
            ptr->record[i + 2],
            ptr->record[i + 3]);
    }
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_bt_sendMsg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]        
 *  peer_buff       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void jsr82_bt_sendMsg(
                msg_type msg,
                module_type dstMod,
                sap_type sap,
                local_para_struct *local_para,
                peer_buff_struct *peer_buff)
{
    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

#endif /* BT_JSR82_ENABLED */ 

