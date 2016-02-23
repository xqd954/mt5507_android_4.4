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


/*****************************************************************************
 * FUNCTION
 *  BTBMProfileAllocateConnIndex
 * DESCRIPTION
 *  All profiles want to get a unique index by UUID and address. The BTBM will assign a unique index for them.
 *  If no connection is available, the system will assert.
 * PARAMETERS
 *  uuid        [IN]        The request profile's UUID
 *  addr        [IN]        The request connection address.
 * RETURNS
 *  U8 unique index.
 *****************************************************************************/
U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr)
{
    U8 i = 0;
    BtDeviceContext *bdc = 0;

    for (i = 0; i < NUM_SUPPORTED_PROFILE_CONN; i++)
    {
        if (MTKBMC(conn)[i].used == 0x00)
        {
            bdc = DS_FindDevice(&(addr));
            MTKBMC(conn)[i].bdc = bdc;
            MTKBMC(conn)[i].uuid = (U16) uuid;
            MTKBMC(conn)[i].used = 0x01;
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTGIVE_CON_IDxD, i);
            return i;
        }
    }
    Assert(0);
    return 0xFF;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMProfileReleaseConnIndex
 * DESCRIPTION
 *  The function is used to return the unique index allocated by btbm
 * PARAMETERS
 *  index       [IN]        The allocated index
 * RETURNS
 *  The result of the release operation.
 *****************************************************************************/
BOOL BTBMProfileReleaseConnIndex(U8 index)
{
    if (index < NUM_SUPPORTED_PROFILE_CONN)
    {
        if (MTKBMC(conn)[index].used == 0x01)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_CON_ALLOCATED_RELEASE);
            MTKBMC(conn)[index].used = 0x00;
            MTKBMC(conn)[index].bdc = 0x00;
            MTKBMC(conn)[index].uuid = 0x00;
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTRETURN_CON_IDxD, index);
            return TRUE;
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_CON_NOT_ALLOCATED_RELEASE);
            return FALSE;
        }
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_CON_RELEASE_VALUE_LARGE);
        return FALSE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMProfileSearchConnIndex
 * DESCRIPTION
 *  The function is used to search the unique index by UUID and address
 * PARAMETERS
 *  uuid        [IN]        The request profile's UUID
 *  addr        [IN]        The request connection address.
 * RETURNS
 *  The searched unque index.
 *****************************************************************************/
U8 BTBMProfileSearchConnIndex(U32 uuid, BD_ADDR addr)
{
    U8 index = 0;

    for (index = 0; index < NUM_SUPPORTED_PROFILE_CONN; index++)
    {
        if (MTKBMC(conn)[index].used == 0x01)
        {
            if ((MTKBMC(conn)[index].uuid == uuid) &&
                (OS_MemCmp((U8*) addr.addr, 6, (U8*) MTKBMC(conn)[index].bdc->addr.addr, 6) == TRUE))
            {
                break;
            }
        }
    }
    if (index == NUM_SUPPORTED_PROFILE_CONN)
    {
        index = 0xFF;
    }
    return index;
}


