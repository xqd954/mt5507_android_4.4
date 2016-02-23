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
*  permission of MediaTek Inc. (C) 2008
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
 * Meeir.c
 *
 * Project:
 * --------
 *   Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions provide the service to MMI or JAVA to 
 *   make the operation of command and event
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
 *******************************************************************************/

#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "me.h"
#include "sdp.h"
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)


/*****************************************************************************
 * FUNCTION
 *  ME_AddEIRServiceClass
 * DESCRIPTION
 *  
 * PARAMETERS
 *  service_class       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ME_AddEIRServiceClass(U16 service_class)
{
    U8 index;
    BOOL record_exist = FALSE;
    U8 i = 0;

    bt_trace(TRACE_GROUP_1, BTLOG_EIR_REGISTRATION_SDP_FOR_UPPER_LAYER_SERVICE_CLASSx04X, service_class);
    record_exist = FALSE;
    for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
    {
        if (MEC(eirSdpCtx)[index].used != 0)
        {
            if (MEC(eirSdpCtx)[index].service_class == service_class)
            {
                bt_trace(
                    TRACE_GROUP_1,
                    BTLOG_RECORD_INDEXx0D_SERVICE_CLASSx04X,
                    index,
                    MEC(eirSdpCtx)[index].service_class);

                record_exist = TRUE;
                MEC(eirSdpCtx)[index].counter++;
                break;
            }
        }
    }
    bt_trace(TRACE_GROUP_1, BTLOG_RECORD_EXISTxD, record_exist);
    if (record_exist == FALSE)
    {
        for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
        {
            if (MEC(eirSdpCtx)[index].used == 0)
            {
                MEC(eirSdpCtx)[index].used = 1;
                MEC(eirSdpCtx)[index].service_class = service_class;
                MEC(eirSdpCtx)[index].counter = 1;
                MEC(eirRecordUpdated) = 1;
                break;
            }
        }
        if (index == MAX_EIR_SDP_SIZE)
        {
            for (i = 0; i < MAX_EIR_SDP_SIZE; i++)
            {
                bt_trace(
                    TRACE_GROUP_1,
                    BTLOG_xD_USEDxD_SERVICE_CLASSx04X,
                    i,
                    MEC(eirSdpCtx)[i].used,
                    MEC(eirSdpCtx)[i].service_class);
            }
            Assert(0);
        }

    }
    {
        U8 k = 0;
        U8 total_count = 0;

        for (k = 0; k < MAX_EIR_SDP_SIZE; k++)
            if (MEC(eirSdpCtx)[index].used == 1)
            {
                total_count++;
            }
        if (total_count > 7)
        {
            ME_StartWriteEIR();
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  ME_AddEIRDeviceName
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void ME_AddEIRDeviceName(void)
{
    MEC(eirRecordUpdated) = 1;
    ME_StartWriteEIR();
}


/*****************************************************************************
 * FUNCTION
 *  ME_RemoveEIRServiceClass
 * DESCRIPTION
 *  
 * PARAMETERS
 *  service_class       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ME_RemoveEIRServiceClass(U16 service_class)
{
    U8 index;

    bt_trace(TRACE_GROUP_1, BTLOG_EIR_REGISTRATION_SDP_FOR_UPPER_LAYER_SERVICE_CLASSx04X, service_class);
    for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
    {
        if (MEC(eirSdpCtx)[index].used != 0)
        {
            if (MEC(eirSdpCtx)[index].service_class == service_class)
            {
                bt_trace(
                    TRACE_GROUP_1,
                    BTLOG_RECORD_INDEXx0D_SERVICE_CLASSx04X,
                    index,
                    MEC(eirSdpCtx)[index].service_class);
                if(MEC(eirSdpCtx)[index].counter == 0)
                {
                    Assert(0);
                }
                MEC(eirSdpCtx)[index].counter--;
                if (MEC(eirSdpCtx)[index].counter == 0)
                {
                    MEC(eirSdpCtx)[index].service_class = 0;
                    MEC(eirSdpCtx)[index].used = 0;
                    MEC(eirRecordUpdated) = 1;
                }
                break;
            }
        }
    }
    ME_StartWriteEIR();

}

U8 meUtilWriteEirName(U8 *buf, U8 bufLen, const U8 *name, U8 nameLen)
{
    int offset = 0;

    if(name && buf && bufLen > 2){
	nameLen = min(nameLen, OS_StrLen((const char*)name));
	if (nameLen)
	{
	    if (nameLen > bufLen - 2)
	    {
                buf[offset] = bufLen - 1;
		offset++;
		buf[offset++] = BT_EIR_NAME_SHORTENED;
		OS_MemCopy(buf + offset, name, bufLen - offset);
		offset = bufLen;
            }
	    else
	    {
                buf[offset++] = nameLen + 1;
                buf[offset++] = BT_EIR_NAME_COMPLETE;
                OS_MemCopy(buf + offset, name, nameLen);
                offset += nameLen;
            }
        }
    }
    Assert (offset <= bufLen);
    return offset;
}

#ifdef __BT_4_0_BLE__
U8 meUtilWriteEirBLEServiceList(U8 *buf, U8 bufLen)
{
    U8 offset = 0, recNum = 0, index;
    U8 uuid_list[MAX_EIR_SDP_SIZE * 2], len = 0;
        
    if (bufLen >= 4)
    {
        for (index = 0; index < MAX_EIR_SDP_SIZE; index++)
        {
            if (MEC(eirSdpCtx)[index].used != 0)
            {
                OS_Report("UUID=0x%x",MEC(eirSdpCtx)[index].service_class);
                if(SdpIsBLEService(MEC(eirSdpCtx)[index].service_class))
                {
                    recNum++;
                    StoreLE16(&uuid_list[len], MEC(eirSdpCtx)[index].service_class);
                    len += 2;
                }
            }
        }

        if (recNum)
        {
            if (len + 2 > bufLen)
            {
                /* EIR UUID is not completed */
                len = bufLen - 2;
                recNum = len / 2;
                buf[1] = BT_EIR_SERVICE_CLASS_16UUID_MORE;
            }
            else
            {
                /* EIR UUID is completed */
                buf[1] = BT_EIR_SERVICE_CLASS_16UUID_COMPLETE;
            }
            bt_trace(TRACE_GROUP_1, BTLOG_EIRRECORDNUMxD, recNum);
            buf[0] = len + 1;
            offset += 2;
            OS_MemCopy(buf + offset, uuid_list, len);
            offset += len;
        }
    }
    Assert (offset <= bufLen);
    return offset;
}
#endif

#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

