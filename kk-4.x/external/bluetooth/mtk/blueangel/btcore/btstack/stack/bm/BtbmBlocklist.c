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
 * btbm.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
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
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Aug 5 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jun 26 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 26 2008 mtk01239
 * [MAUI_00777619] [Bluetooth] Revise the BTBM code
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
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
#include "btbm.h"
#include "osapi.h"
#include "btbmblocklist.h"

#if defined (__BT_2_1_BLOCK_DEVICE__)
extern BMCONTENT_T BMController;

#define MTKBMC(f)  (BMController.f)

extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);
extern void btbmHandleForceDisconnectLink(BD_ADDR *link);


/*****************************************************************************
 * FUNCTION
 *  BTBMHandleUpdateBlockList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMHandleUpdateBlockList(bt_bm_block_list_update_req_struct *btbm_adp_input_msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;
    U8 no;
    BD_ADDR addr;
    btbm_bd_addr_t *remoteaddr;
    BMCallbackParms parms;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    no = btbm_adp_input_msg->num_block_list;
    if (no > BTBM_ADP_MAX_BLOCK_LIST_NO)
    {
        no = BTBM_ADP_MAX_BLOCK_LIST_NO;
    }
    MTKBMC(blockNo) = no;
    bt_trace(TRACE_GROUP_1, BTLOG_BLOCK_LIST_NOxD, no);
    for (i = 0; i < no; i++)
    {
        remoteaddr = &(btbm_adp_input_msg->block_list[i]);
        CONVERT_BDADDR2ARRAY(addr.addr, remoteaddr->lap, remoteaddr->uap, remoteaddr->nap);
        OS_MemCopy(&(MTKBMC(blockAddr[i * 6])), addr.addr, 6);
#ifndef BTMTK_ON_WISESDK
        btbmHandleForceDisconnectLink(&addr);
#endif
        BTBMBlockListDumpAddr(MTKBMC(blockAddr)+i * 6, i);
    }
    parms.event = BTBM_EVENT_BLOCKLIST_UPDATED_RESULT;
    MTKBMC(bmCB)(&parms);
    
}


/*****************************************************************************
 * FUNCTION
 *  BTBMCheckBlockList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  addr        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL BTBMCheckBlockList(U8 *addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BTBMBlockListDumpAddr(addr, 0xFF);
    for (i = 0; i < MTKBMC(blockNo); i++)
    {
        BTBMBlockListDumpAddr(MTKBMC(blockAddr)+i * 6, i);
        if (OS_MemCmp(&MTKBMC(blockAddr[i * 6]), 6, addr, 6) == TRUE)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMBlockListDumpAddr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  addr        [IN]        
 *  index       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMBlockListDumpAddr(U8 *addr, U8 index)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_trace(
        TRACE_GROUP_1,
        BTLOG_BLOCK_LIST_Ix02Xx02Xx02Xx02Xx02Xx02X,
        index,
        addr[0],
        addr[1],
        addr[2],
        addr[3],
        addr[4],
        addr[5]);
}

#endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 

