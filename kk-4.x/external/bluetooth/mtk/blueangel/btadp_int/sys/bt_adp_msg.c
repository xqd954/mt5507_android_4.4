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
*  permission of MediaTek Inc. (C) 2005
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
 * Bt_adp_msg.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Dlight Ting
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "osapi.h"
#include "bt_message.h"
#ifdef BTMTK_ON_WISE 
#include "bt.h" /* for bt_p */
#endif  /* BTMTK_ON_WISE */
#include "bttypes.h"

#ifdef __BT_3_0_HS__
extern void pal_bt_data_command_handler (ilm_struct *ilm_ptr);
#endif

#ifndef BTMTK_ON_WISE 
U8 BT_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size);
#endif  /* BTMTK_ON_WISE */

void BTCoreSendNotifySelfMessage(void)
{
#if BTMTK_ON_WISE
    if(bt_p->NotifyEVM == FALSE)
    {
#endif
    /* if the task is not been notified, send message to wakeup again. */
    BT_SendMessage(MSG_ID_BT_NOTIFY_EVM_IND, MOD_BT, NULL, 0);
#if BTMTK_ON_WISE
        bt_p->NotifyEVM = TRUE;
    }
#endif
}

#ifdef BTMTK_ON_WISE
#if 0
/*****************************************************************************
 * FUNCTION
 *  bt_send_msg
 * DESCRIPTION
 *  This is bt_send_msg function of BT module.
 *  send msg to other module
 * PARAMETERS
 *  msg             [IN]        To be sent
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void bt_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{

    ilm_struct *ilmPtr;

    if (!bt_p->is_factory_mode)
    {
        ilmPtr = allocate_ilm(MOD_BT);
        ilmPtr->msg_id = msg;
        ilmPtr->local_para_ptr = local_para;
        ilmPtr->peer_buff_ptr = peer_buff;
        ilmPtr->dest_mod_id = dstMod;
        ilmPtr->src_mod_id = MOD_BT;
        ilmPtr->sap_id = sap;
        msg_send_ext_queue(ilmPtr);
    }

    return;
}
#endif
#else   /* BTMTK_ON_WISE */
U8 BT_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
	ilm_struct ilm;
    U16 *field;
    U8 status = 0;
    
    //bt_prompt_trace(MOD_BT, "[BT] BT_SendMessage %d, ptr=%d, size=%d", msg_id, ptr, size);
    OS_MemSet((U8 *)&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.src_mod_id = MOD_BT;
    ilm.dest_mod_id = dest_mod;
    Assert (size <= MAX_ILM_BUFFER_SIZE);
	if(ptr != NULL)
    {   
        OS_MemCopy((U8 *)&ilm.ilm_data, ptr, size);
        free_local_para(ptr);
    }

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm.ilm_data;
    *field = 1;
    field = (U16 *)&ilm.ilm_data[2];
    *field = size;
    if(dest_mod != MOD_BT)
    bt_log_primitive(&ilm);
#ifdef BTMTK_ON_LINUX
       status = bt_enQueue_message(dest_mod, (void *)&ilm, (size+sizeof(ilm)-MAX_ILM_BUFFER_SIZE));
#endif
#ifdef __WIN_QUEUE__    
	win_enQueue_message(dest_mod, (void *)&ilm);
#endif
#ifdef __WIN_SHARED_MEMORY__
    win_send_message_from_bt((void *)&ilm, sizeof(ilm_struct));
#endif
    return status;
}

U8 BT_SendPalMessage(msg_type msg_id, void *ptr, U16 size)
{
    ilm_struct *ilm;
    U16 *field;
    U8 status = 0;

    if(ptr != NULL)
    {   
        ilm = (ilm_struct*)ptr;
        ilm->msg_id = msg_id;
        ilm->src_mod_id = MOD_BT;
        ilm->dest_mod_id = MOD_PAL;
        Assert (size <= 4*MAX_PAL_ILM_BUFFER_SIZE);
    }else{
        OS_Report("BT_SendPalMessage failed due to ptr is NULL");
        return 1;
    }

    /* LOCAL_PARA_HDR */
//    field = (U16 *)ilm->ilm_data;
//    *field = 1;
//    field = (U16 *)&ilm->ilm_data[2];
//    *field = 100;
//    bt_log_primitive((ilm_struct *)ilm);
    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    field = (U16 *)&ilm->ilm_data[2];
    *field = size;

#if !defined(__MTK_BT_EXTERNAL_PLATFORM__)
     // directly invoke api 
	if( ilm->msg_id == MSG_ID_PAL_BT_DATA_COMMAND ){
		pal_bt_data_command_handler(ilm);
		status = 0;
		return status;
	}
#endif	
	
    status = bt_enQueue_message(MOD_PAL, (void *)ilm, (size+sizeof(pal_ilm_struct)-MAX_PAL_ILM_BUFFER_SIZE));
    return status;
}


#endif  /* BTMTK_ON_WISE */

void btmtk_adp_check_msg_id_range(void)
{
#ifdef BTMTK_ON_WIN32
   if (MSG_ID_BT_START + MSG_ID_BT_RANGE <= MSG_ID_BT_END)
      Assert(0);

   if (MSG_ID_BT_GAP_GROUP_START + MSG_ID_BT_GAP_GROUP_RANGE <= MSG_ID_BT_GAP_GROUP_END)
      Assert(0);

   if (MSG_ID_BT_A2DP_GROUP_START + MSG_ID_BT_A2DP_GROUP_RANGE <= MSG_ID_BT_A2DP_GROUP_END)
      Assert(0);

   if (MSG_ID_BT_AVRCP_GROUP_START + MSG_ID_BT_AVRCP_GROUP_RANGE <= MSG_ID_BT_AVRCP_GROUP_END)
      Assert(0);

   if (MSG_ID_BT_HFG_GROUP_START + MSG_ID_BT_HFG_GROUP_RANGE <= MSG_ID_BT_HFG_GROUP_END)
      Assert(0);

   if (MSG_ID_BT_SPP_GROUP_START + MSG_ID_BT_SPP_GROUP_RANGE <= MSG_ID_BT_SPP_GROUP_END)
      Assert(0);

   if (MSG_ID_BT_CHN_GROUP_START + MSG_ID_BT_CHN_GROUP_RANGE <= MSG_ID_BT_CHN_GROUP_END)
      Assert(0);
#endif   
}
#ifdef BTMTK_ON_WISE 
void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
    ilm_struct *ilmPtr;

    if (!bt_p->is_factory_mode)
    {
        ilmPtr = allocate_ilm(MOD_EXTERNAL);
        
        ilmPtr->msg_id = msg_id;
        if (size != 0)
        {
            ilmPtr->local_para_ptr = construct_local_para(size, TD_UL);
            ((local_para_struct *)ptr)->msg_len = size;
            ((local_para_struct *)ptr)->ref_count = 1;
            OS_MemCopy((U8 *)ilmPtr->local_para_ptr, ptr, size);
        }
        else
        {
            ilmPtr->local_para_ptr = 0;
        }
//        ilmPtr->local_para_ptr = ptr;
        ilmPtr->peer_buff_ptr = NULL;
        ilmPtr->dest_mod_id = dest_mod;
        ilmPtr->src_mod_id = MOD_EXTERNAL;

        ilmPtr->sap_id = BT_APP_SAP;
        msg_send_ext_queue(ilmPtr);
    }
}
#endif  /* BTMTK_ON_WISE */



