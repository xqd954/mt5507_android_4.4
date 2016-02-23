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
 * jbt_session.c
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
 * 10 13 2010 bingyi.chen
 * [ALPS00021197] [CTS][E1K16] System pops NE during test "CtsBluetoothTestCases"
 * .
 *
 * Jul 11 2009 mtk02126
 * [MAUI_01703270] Modify traces
 * 
 *
 * Mar 11 2009 mtk02126
 * [MAUI_01644724] [JSR82] bt_jsr82_HandleSdpReq
 * 
 *
 * Dec 10 2008 mtk01411
 * [MAUI_01478777] JSR82_ Assert Fail: utils.c 331-BT
 * 
 *
 * Sep 24 2008 mtk01411
 * [MAUI_01243263] [JSR82] JBT Add reject cmds handling in VM Minimize mode
 * 
 *
 * Sep 22 2008 mtk01411
 * [MAUI_01241533] [JSR82] Enable ADM management in JBT
 * 
 *
 * Sep 4 2008 mtk01411
 * [MAUI_01233160] [JSR82] Change jbt_malloc to jvm_malloc directly in jbt files
 * 
 *
 * Aug 6 2008 mtk01411
 * [MAUI_00817738] [JSR82] Format JBT codes
 * 
 *
 * Mar 27 2008 mtk01411
 * [MAUI_00740020] Fatal Error (4): Data Abort (8a74968) - JDAEMON (Use a NULL subsession_entry ptr to
 * 
 *
 * Feb 22 2008 mtk01411
 * [MAUI_00622206] [Java] Support JSR-82 on Aplix soluiton
 * 
 *
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#if defined(__BT_JSR82__) || defined(__BT_JSR82_L2RF__)

/***************************************************************************** 
* Include
*****************************************************************************/
//#include "bt_common.h"
#include "bt_jbt_interface.h"
#include "bt_jbt_internal.h"
#include "memory.h"
//#include "bt_osal.h"
#include "bt_jsr82_api.h"
#include "bt_mmi_msg.h"
#include "bt_jsr82_hdl.h"
#include "bt_jsr82_sdp.h"
#include "ext_ring.h"
#include "bt_types.h"
#include "bt_ext_debug.h"
#include "bt_mmi.h"
#include "bt_config.h"

#include "session_api.h"
#include "session_stream.h"
#include "bt_session.h"

JBT_JSR82_CNTX jbt_jsr82 = {
				.init = FALSE, 
				.mutex = NULL};


SESSION_RESULT jbt_session_monitor_routine(Session_Event_PARM *parm);


/*****************************************************************************
 * FUNCTION
 *  jbt_l2cap_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_l2cap_init(JBT_L2CAP_CNTX *jbt_l2cap_context)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

#ifdef __JBT_SESSION_LINK_LIST__
    jbt_list *head = NULL, *tail = NULL;
#endif 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT l2cap init");
    memset((kal_uint8*) jbt_l2cap_context, 0, sizeof(jbt_l2cap_context));

#ifdef __JBT_SESSION_LINK_LIST__
    /* Initialization freeList and activeList */
    jbt_l2cap_context->freeList.Blink = &jbt_l2cap_context.freeList;
    jbt_l2cap_context->freeList.Flink = &jbt_l2cap_context.freeList;

    jbt_l2cap_context->activeList.Flink = &jbt_l2cap_context.activeList;
    jbt_l2cap_context->activeList.Blink = &jbt_l2cap_context.activeList;

    head = &jbt_l2cap_context.freeList;
    tail = head;
#endif /* __JBT_SESSION_LINK_LIST__ */ 

    for (i = 0; i < JBT_MAX_L2CAP_NO; i++)
    {
    #ifdef __JBT_SESSION_LINK_LIST__
        jbt_list *entry = &jbt_l2cap_context->l2cap[i].l2cap_session.node;
    #endif 
        kal_uint8 j = 0;
        jbt_session_info *session = &jbt_l2cap_context->l2cap[i].l2cap_session;

        for (j = 0; j < JBT_MAX_SUBSESSION_NO; j++)
        {
     //       session->subsession[j].is_tx_empty = KAL_TRUE;
            session->subsession[j].is_lower_tx_empty = KAL_TRUE;
            session->subsession[j].readyToRead = KAL_TRUE;
            session->subsession[j].notifyReadyToWrite = KAL_FALSE;
        }

    #ifdef __JBT_SESSION_LINK_LIST__
        tail->Flink = entry;
        entry->Blink = tail;
        entry->Flink = head;
        head->Blink = entry;
        tail = entry;
    #endif /* __JBT_SESSION_LINK_LIST__ */ 
    }

}

void jbt_l2cap_deinit(JBT_L2CAP_CNTX *jbt_l2cap_context)
{
	int i = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT l2cap deinit");   
	for (i = 0; i < JBT_MAX_L2CAP_NO; i++)
	{
  		jbt_session_info *session = &jbt_l2cap_context->l2cap[i].l2cap_session;
		if (session->used)
		{
        	jbt_reset_session_entry(session);
		}
		 
	}
    
}

/*****************************************************************************
 * FUNCTION
 *  jbt_spp_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_spp_init(JBT_SPP_CNTX *jbt_spp_context)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

#ifdef __JBT_SESSION_LINK_LIST__
    jbt_list *head = NULL, *tail = NULL;
#endif 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT Spp init");

    memset((kal_uint8*)jbt_spp_context, 0, sizeof(jbt_spp_context));

#ifdef __JBT_SESSION_LINK_LIST__
    /* Initialization freeList and activeList */
    jbt_spp_context->freeList.Blink = &jbt_spp_context.freeList;
    jbt_spp_context->freeList.Flink = &jbt_spp_context.freeList;

    jbt_spp_context->activeList.Flink = &jbt_spp_context.activeList;
    jbt_spp_context->activeList.Blink = &jbt_spp_context.activeList;

    head = &jbt_spp_context->freeList;
    tail = head;
#endif /* __JBT_SESSION_LINK_LIST__ */ 

    for (i = 0; i < JBT_MAX_SPP_NO; i++)
    {
    #ifdef __JBT_SESSION_LINK_LIST__
        jbt_list *entry = &jbt_spp_context->spp[i].spp_session.node;
    #endif 
        kal_uint8 j = 0;
        jbt_session_info *session = &jbt_spp_context->spp[i].spp_session;

        for (j = 0; j < JBT_MAX_SUBSESSION_NO; j++)
        {
         //   session->subsession[j].is_tx_empty = KAL_TRUE;
            session->subsession[j].is_lower_tx_empty = KAL_TRUE;
            session->subsession[j].readyToRead = KAL_TRUE;
            session->subsession[j].notifyReadyToWrite = KAL_FALSE;
        }

    #ifdef __JBT_SESSION_LINK_LIST__
        tail->Flink = entry;
        entry->Blink = tail;
        entry->Flink = head;
        head->Blink = entry;
        tail = entry;
    #endif /* __JBT_SESSION_LINK_LIST__ */ 

    }

}

void jbt_spp_deinit(JBT_SPP_CNTX *jbt_spp_context)
{
	int i = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT Spp deinit");


    for (i = 0; i < JBT_MAX_SPP_NO; i++)
    {
        jbt_session_info *session = &jbt_spp_context->spp[i].spp_session;
		if (session->used)
		{
        	jbt_reset_session_entry(session);
		}
    }
}


void jbt_ConvertArray2Bdaddr(bt_addr_struct *dest, U8 *src)
{
    dest->nap = (((U16) src[4])) | ((U16) src[5] << 8);
    dest->uap = (U8) src[3];
    dest->lap = (((U32) src[2]) << 16) | ((U32) src[1] << 8) | ((U32) src[0]);
}



/*****************************************************************************
 * FUNCTION
 *  insert_node_to_targetList_tail
 * DESCRIPTION
 *  
 * PARAMETERS
 *  target_list     [IN]        
 *  node            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void insert_node_to_targetList_tail(jbt_list *target_list, jbt_list *node)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_list *tail = target_list->Blink;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __JSR82_MAUI__
    kal_trace(TRACE_FUNC, JBT_INSERT_NODE_TO_TARGETLIST_TAIL);
#else
    bt_ext_log("[JSR82][JBT] JBT insert_node_to_targetList_tail");
#endif
    target_list->Blink = node;
    node->Flink = target_list;
    node->Blink = tail;
    tail->Flink = node;

}


/*****************************************************************************
 * FUNCTION
 *  remove_node_from_targetList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  target_list     [IN]        
 *  node            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void remove_node_from_targetList(jbt_list *target_list, jbt_list *node)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /* Remove node from target_list */
    jbt_list *previous_entry = NULL, *next_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __JSR82_MAUI__
    kal_trace(TRACE_FUNC, JBT_REMOVE_NODE_FROM_TARGETLIST);
#else
    bt_ext_log("[JSR82][JBT] JBT remove_node_from_targetList");
#endif

    previous_entry = node->Blink;
    next_entry = node->Flink;
    previous_entry->Flink = next_entry;
    next_entry->Blink = previous_entry;
}

kal_uint8 jbt_convert_list_type(kal_uint8 ps_type)
{
	switch(ps_type)
	{
	case JSR82_SESSION_PS_RFCOMM:
		return JBT_LIST_TYPE_SPP_SESSION;
		
	case JSR82_SESSION_PS_L2CAP:
		return JBT_LIST_TYPE_L2CAP_SESSION;
		
	default:
		return 0;
		
	}
}
kal_uint16 jbt_get_max_mtu(kal_uint8 ps_type)

{
	kal_uint16 max_mtu;
	if (ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        max_mtu = JSR82_SESSION_PS_RFCOMM_MTU;
    }
    else
    {
    	max_mtu = JSR82_SESSION_PS_L2CAP_MTU;
    }
	return max_mtu;
}


#ifdef __JBT_SESSION_LINK_LIST__


/*****************************************************************************
 * FUNCTION
 *  jbt_free_one_existing_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list_type       [IN]        
 *  node            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_free_one_existing_entry(kal_uint8 list_type, jbt_list *node)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (list_type == JBT_LIST_TYPE_L2CAP_SESSION)
    {
        /* Remove this entry from activeList */
        remove_node_from_targetList(&jbt_l2cap_context.activeList, node);
        /* Insert this entry to freeList */
        insert_node_to_targetList_tail(&jbt_l2cap_context.freeList, node);
    }
    else if (list_type == JBT_LIST_TYPE_SPP_SESSION)
    {
        /* Remove this entry from activeList */
        remove_node_from_targetList(&jbt_spp_context.activeList, node);
        /* Insert this entry to freeList */
        insert_node_to_targetList_tail(&jbt_spp_context.freeList, node);
    }
}
#else /* __JBT_SESSION_LINK_LIST__ */ 



/*****************************************************************************
 * FUNCTION
 *  jbt_free_one_existing_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list_type       [IN]        
 *  node            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_free_one_existing_entry(kal_uint8 list_type, jbt_list *node)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;
    int max_session_no = 0;
    jbt_session_info *session_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (list_type == JBT_LIST_TYPE_L2CAP_SESSION)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.l2cap.l2cap;
    }
    else if (list_type == JBT_LIST_TYPE_SPP_SESSION)
    {
        max_session_no = JBT_MAX_SPP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.spp.spp;
    }

    for (i = 0; i < max_session_no; i++)
    {
        if ((session_entry->used == 1) && (session_entry == (jbt_session_info*) node))
        {
            /* Find this entry which will be freed, reset its used and initiator fileds */
            session_entry->used = 0;
            session_entry->initiator = JBT_SESSION_INITIATOR_FALSE;
            session_entry->ps_type = 0;
            session_entry->psm_channel = 0;
            session_entry->transaction_id = 0;
            session_entry->index = 0xFF;
            session_entry->subsession_count = 0;
            session_entry->active_count = 0;
            session_entry->turn_on_op.operation_state = 0;
            session_entry->turn_on_op.transaction_id = 0;
            session_entry->con_req_op.operation_state = 0;
            session_entry->con_req_op.transaction_id = 0;
//            memset(session_entry->bd_addr, 0, 6);
            break;
        }
        else
        {
            session_entry += 1;
        }
    }
}
#endif /* __JBT_SESSION_LINK_LIST__ */ 

#ifdef __JBT_SESSION_LINK_LIST__


/*****************************************************************************
 * FUNCTION
 *  jbt_allocate_one_available_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  freeList        [IN]        
 *  activeList      [IN]        
 *  list_type       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_list *jbt_allocate_one_available_entry(jbt_list *freeList, jbt_list *activeList, kal_uint8 list_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_list *current_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    current_entry = freeList->Flink;
    bt_ext_log("[JSR82][JBT] JBT jbt_allocate_one_available_entry");

    while (current_entry != freeList)
    {
        if (((jbt_session_info*) current_entry)->used == 0)
        {        	
            ((jbt_session_info*) current_entry)->used = 1;
            ((jbt_session_info*) current_entry)->subsession_count = 0;
            /* Remove current_entry from freeList */
            remove_node_from_targetList(freeList, current_entry);
            /* Insert current_entry to activeList */
            insert_node_to_targetList_tail(activeList, current_entry);
            ((jbt_session_info*) current_entry)->initiator = JBT_SESSION_INITIATOR_FALSE;
			bt_ext_log("[JSR82][JBT] JBT jbt_allocate_one_available_entry find one");
            return current_entry;
        }
        else
        {
            current_entry = current_entry->Flink;
        }
    }
    bt_ext_log("[JSR82][JBT] JBT jbt_allocate_one_available_entry find none");
    return NULL;

}
#else /* __JBT_SESSION_LINK_LIST__ */ 


/*****************************************************************************
 * FUNCTION
 *  jbt_allocate_one_available_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  freeList        [IN]        
 *  activeList      [IN]        
 *  list_type       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_list *jbt_allocate_one_available_entry(jbt_list *freeList, jbt_list *activeList, kal_uint8 list_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int i = 0;
    int max_session_no = 0;
    jbt_session_info *session_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (list_type == JBT_LIST_TYPE_L2CAP_SESSION)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.l2cap.l2cap;
    }
    else if (list_type == JBT_LIST_TYPE_SPP_SESSION)
    {
        max_session_no = JBT_MAX_SPP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.spp.spp;
    }

    for (i = 0; i < max_session_no; i++)
    {
        if (session_entry->used == 0)
        {
        	memset(session_entry, 0x0, sizeof(jbt_session_info));
            session_entry->used = 1;
            session_entry->subsession_count = 0;
            session_entry->initiator = JBT_SESSION_INITIATOR_FALSE;
            return (jbt_list*) session_entry;
        }
        else
        {
            session_entry += 1;
        }
    }
    return NULL;
}
#endif /* __JBT_SESSION_LINK_LIST__ */ 


/*****************************************************************************
 * FUNCTION
 *  jbt_check_already_connect_chnl_and_addr
 * DESCRIPTION
 *  This function is used to avoid duplicated session with same bd_addr, ps_type and psm_chnl_num
 * PARAMETERS
 *  bd_addr         [IN]        
 *  list_type       [IN]        
 *  ps_chnl_num     [IN]        
 * RETURNS
 *  
 *****************************************************************************/

kal_bool jbt_check_already_connect_chnl_and_addr(kal_uint8 *bd_addr, kal_uint8 list_type, kal_uint16 ps_chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;
    int max_session_no = 0;
    jbt_session_info *session_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	//TODO: check the remote device and psm channal
	return FALSE;
#if 0

    if (list_type == JBT_LIST_TYPE_L2CAP_SESSION)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.l2cap.l2cap;
    }
    else if (list_type == JBT_LIST_TYPE_SPP_SESSION)
    {
        max_session_no = JBT_MAX_SPP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.spp.spp;
    }

    for (i = 0; i < max_session_no; i++)
    {
        if (session_entry->used == 1)
        {
#ifdef __JSR82_MAUI__
            kal_trace(
                BT_TRACE_JSR82_GROUP,
                JBT_ADDRx02Xx02Xx02Xx02Xx02Xx02X,
                session_entry->bd_addr[0],
                session_entry->bd_addr[1],
                session_entry->bd_addr[2],
                session_entry->bd_addr[3],
                session_entry->bd_addr[4],
                session_entry->bd_addr[5]);
#else
		  bt_ext_log("[JSR82][JBT] JBT addr:%02x,%02x,%02x,%02x,%02x,%02x", 
                session_entry->bd_addr[0],
                session_entry->bd_addr[1],
                session_entry->bd_addr[2],
                session_entry->bd_addr[3],
                session_entry->bd_addr[4],
                session_entry->bd_addr[5]);
#endif
            if ((memcmp(session_entry->bd_addr, bd_addr, 6) == 0) && session_entry->psm_channel == ps_chnl_num 
            	&& (JBT_SESSION_INITIATOR_TRUE == session_entry->initiator))
            {
                return KAL_TRUE;
            }
        }
        session_entry += 1;
    }
    return KAL_FALSE;
#endif 
}

#ifdef __JBT_SESSION_LINK_LIST__


/*****************************************************************************
 * FUNCTION
 *  jbt_search_an_existing_entry
 * DESCRIPTION
 *  If the session is connected, the session_inx (i.e., con_id) is used to search an existing entry.
 *  (1) server side: turn on service, disable service, read/write operation, disc_request  (session_inx is obtained after receiveing enable service cfn)
 *  (2) client side : read/write operation, disc_request
 *  After 3rd party Java obtaining the session_inx (ie., con_id), it will passed this session_inx to do any operations of this session (e.g, read,write, disc_req)
 *  For Btstack, it will use this session_inx to determine this operation request from JBT is belonged to which session.
 *  Besides, the identify in operation request is used for 3rd party Java solution to do its callback
 * PARAMETERS
 *  targetList      [IN]        
 *  search_type     [IN]        
 *  value           [IN]        
 *  list_type       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_list *jbt_search_an_existing_entry(
            jbt_list *targetList,
            kal_uint8 search_type,
            kal_uint32 value,
            kal_uint8 list_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_list *current_entry = targetList->Flink;
    kal_uint32 existed_value = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT jbt_search_an_existing_entry");
    bt_ext_log("[JSR82][JBT] JBT search_type:%02x", search_type);
    bt_ext_log("[JSR82][JBT] JBT value:%08x", value);

    while (current_entry != targetList)
    {
        jbt_session_info *session_entry = (jbt_session_info*) (current_entry);

        if (search_type == JBT_LIST_SEARCH_TYPE_BY_INX)
        {
            existed_value = session_entry->index;
        }
        else if (search_type == JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID)
        {
            existed_value = session_entry->transaction_id;
        }
        else if (search_type == JBT_LIST_SEARCH_TYPE_BY_CHNL_NUM)
        {
            existed_value = session_entry->psm_channel;
        }
		else if (search_type == JBT_LIST_SEARCH_TYPE_BY_SESSION_ID)
        {
            existed_value = session_entry->session_id;
        }

        if ((existed_value == value) && (session_entry->used == 1))
        {
            return current_entry;
        }
        else
        {
            current_entry = current_entry->Flink;
        }
    }
    return NULL;
}
#else /* __JBT_SESSION_LINK_LIST__ */ 


/*****************************************************************************
 * FUNCTION
 *  jbt_search_an_existing_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  targetList      [IN]        
 *  search_type     [IN]        
 *  value           [IN]        
 *  list_type       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_list *jbt_search_an_existing_entry(
            jbt_list *targetList,
            kal_uint8 search_type,
            kal_uint32 value,
            kal_uint8 list_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 existed_value = 0;
    int i = 0;
    int max_session_no = 0;
    jbt_session_info *session_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (list_type == JBT_LIST_TYPE_L2CAP_SESSION)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.l2cap.l2cap;
    }
    else if (list_type == JBT_LIST_TYPE_SPP_SESSION)
    {
        max_session_no = JBT_MAX_SPP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.spp.spp;
    }

    for (i = 0; i < max_session_no; i++)
    {
        if (search_type == JBT_LIST_SEARCH_TYPE_BY_INX)
        {
            existed_value = session_entry->index;
        }
        else if (search_type == JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID)
        {
            existed_value = session_entry->transaction_id;
        }
        else if (search_type == JBT_LIST_SEARCH_TYPE_BY_CHNL_NUM)
        {
            existed_value = session_entry->psm_channel;
        }
		else if (search_type == JBT_LIST_SEARCH_TYPE_BY_SESSION_ID)
        {
            existed_value = session_entry->session_id;
        }

        if ((existed_value == value) && (session_entry->used == 1))
        {
            return (jbt_list*) session_entry;
        }
        else
        {
            session_entry += 1;
        }
    }
    return NULL;
}
#endif /* __JBT_SESSION_LINK_LIST__ */ 

#ifdef __JBT_SESSION_LINK_LIST__
/* Add by mtk01411: 2007-1103 */


/*****************************************************************************
 * FUNCTION
 *  jbt_search_existing_entry_with_psm_chnl_bdaddr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  targetList      [IN]        
 *  psm_channel     [IN]        
 *  bd_addr         [IN]        
 *  list_type       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_session_info *jbt_search_existing_entry_with_psm_chnl_bdaddr(
                    jbt_list *targetList,
                    kal_uint16 psm_channel,
                    kal_uint8 *bd_addr,
                    kal_uint8 list_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_list *current_entry = targetList->Flink;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (current_entry != targetList)
    {
        jbt_session_info *session_entry = (jbt_session_info*) (current_entry);

        if ((session_entry->psm_channel == psm_channel) /*&& (memcpy(session_entry->bd_addr, bd_addr, 6) == 0)*/)
        {
            return session_entry;
        }
        else
        {
            current_entry = current_entry->Flink;
        }
    }
    return NULL;
}
#else /* __JBT_SESSION_LINK_LIST__ */ 


/*****************************************************************************
 * FUNCTION
 *  jbt_search_existing_entry_with_psm_chnl_bdaddr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  targetList      [IN]        
 *  psm_channel     [IN]        
 *  bd_addr         [IN]        
 *  list_type       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_session_info *jbt_search_existing_entry_with_psm_chnl_bdaddr(
                    jbt_list *targetList,
                    kal_uint16 psm_channel,
                    kal_uint8 *bd_addr,
                    kal_uint8 list_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int i = 0;
    int max_session_no = 0;
    jbt_session_info *session_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (list_type == JBT_LIST_TYPE_L2CAP_SESSION)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.l2cap.l2cap;
    }
    else if (list_type == JBT_LIST_TYPE_SPP_SESSION)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.spp.spp;
    }

    for (i = 0; i < max_session_no; i++)
    {
        if ((session_entry->psm_channel == psm_channel)/* && (memcpy(session_entry->bd_addr, bd_addr, 6) == 0)*/ &&
            (session_entry->used == 1))
        {
            return session_entry;
        }
        else
        {
            session_entry += 1;
        }
    }
    return NULL;
}
#endif /* __JBT_SESSION_LINK_LIST__ */ 

jbt_session_info *jbt_search_existing_session_entry(kal_uint8 ps_type,
																kal_uint8 search_type,
                    											kal_uint32 value)
{
	jbt_list * existing_entry = NULL;
	
	if (ps_type == JSR82_SESSION_PS_RFCOMM)
	{
			existing_entry = jbt_search_an_existing_entry(
								&jbt_jsr82.spp.activeList,
								search_type,
								value,
								JBT_LIST_TYPE_SPP_SESSION);
	}
	else if (ps_type == JSR82_SESSION_PS_L2CAP)
	{
			existing_entry = jbt_search_an_existing_entry(
								&jbt_jsr82.l2cap.activeList,
								search_type,
								value,
								JBT_LIST_TYPE_L2CAP_SESSION);
	}
	else
	{
		ASSERT(0);
	}
	return (jbt_session_info *)existing_entry;

}


jbt_session_info *jbt_allocate_one_available_session_entry(kal_uint8 ps_type)
{	
	jbt_list *free_entry = NULL;
	if (ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        free_entry = jbt_allocate_one_available_entry(
                        &jbt_jsr82.spp.freeList,
                        &jbt_jsr82.spp.activeList,
                        JBT_LIST_TYPE_SPP_SESSION);
    }
    else if (ps_type == JSR82_SESSION_PS_L2CAP)
    {
        free_entry = jbt_allocate_one_available_entry(
                        &jbt_jsr82.l2cap.freeList,
                        &jbt_jsr82.l2cap.activeList,
                        JBT_LIST_TYPE_L2CAP_SESSION);
    }
    else
    {
        ASSERT(0);
    }

	return (jbt_session_info*) free_entry;
}


/*****************************************************************************
 * FUNCTION
 *  jbt_allocate_one_available_subsession_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_entry       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_subsession *jbt_allocate_one_available_subsession_entry(jbt_session_info *session_entry)
{
#if (JBT_MAX_SUBSESSION_NO > 1)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 i = 0;
#else /* (JBT_MAX_SUBSESSION_NO > 1) */
    jbt_subsession *subsession_entry = NULL;
#endif /* (JBT_MAX_SUBSESSION_NO > 1) */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if (JBT_MAX_SUBSESSION_NO > 1)
    for (i = 0; i < JBT_MAX_SUBSESSION_NO; i++)
    {
        if (session_entry->subsession[i].used == 0)
        {
            session_entry->subsession[i].used = 1;
            session_entry->subsession[i].subsession_id = 0;
            session_entry->subsession_count++;
            return &(session_entry->subsession[i]);
        }
    }
    return NULL;
#else /* (JBT_MAX_SUBSESSION_NO > 1) */
    if (session_entry->subsession_count != 0)
    {
        ASSERT(0);
    }
    subsession_entry = &(session_entry->subsession[0]);
    subsession_entry->used = 1;
    subsession_entry->subsession_id = 0;
    session_entry->subsession_count++;
    return subsession_entry;
#endif /* (JBT_MAX_SUBSESSION_NO > 1) */

}


/*****************************************************************************
 * FUNCTION
 *  jbt_search_existing_subsession_entry
 * DESCRIPTION
 *  For searching an already connected or connect_request cases
 * PARAMETERS
 *  session_entry       [IN]       
 *  search_type         [IN]        
 *  value               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_subsession *jbt_search_existing_subsession_entry(
                    jbt_session_info *session_entry,
                    kal_uint8 search_type,
                    kal_uint32 value)
{
#if (JBT_MAX_SUBSESSION_NO > 1)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 i = 0;
    kal_uint32 existed_value;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < JBT_MAX_SUBSESSION_NO; i++)
    {
        if (session_entry->subsession[i].used == 1)
        {
            if (search_type == JBT_LIST_SEARCH_TYPE_BY_INX)
            {
                existed_value = session_entry->subsession[i].l2cap_id;
            }
            else if (search_type == JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID)
            {
                existed_value = session_entry->subsession[i].transaction_id;
            }
			else if (search_type == JBT_LIST_SEARCH_TYPE_BY_SESSION_ID)
            {
                existed_value = session_entry->subsession[i].subsession_id;
            }

            if (existed_value == value)
            {
                if (search_type == JBT_LIST_SEARCH_TYPE_BY_INX)
                {
                    if (session_entry->subsession[i].subsession_state != JBT_SESSION_STATE_CONNECTED)
                    {
                    	   // TODO: What if in CONNECT_CNF & DISCONNECT_IND cases?
                        //ASSERT(0);
                    }
                }
                return &(session_entry->subsession[i]);
            }
        }
    }
    return NULL;
#else /* (JBT_MAX_SUBSESSION_NO > 1) */
    return (&session_entry->subsession[0]);
#endif /* (JBT_MAX_SUBSESSION_NO > 1) */
}


/*****************************************************************************
 * FUNCTION
 *  jbt_server_find_one_subsession_entry
 * DESCRIPTION
 *  This function is only invoked in server side
 * PARAMETERS
 *  session_entry       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
jbt_subsession *jbt_server_find_one_subsession_entry(jbt_session_info *session_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /* Find one subsession_entry and its used =1 (means allocates RXTX buf ) and subsession_state = IDLE */
    int i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < JBT_MAX_SUBSESSION_NO; i++)
    {
        if (session_entry->subsession[i].used == 1 &&
            session_entry->subsession[i].subsession_state == JBT_SESSION_STATE_IDLE)
        {
            return &(session_entry->subsession[i]);
        }
    }
    return NULL;
}
jbt_subsession *jbt_find_subsession_entry(kal_uint8 session_type,
            U8 session_inx,
            kal_uint16 subsession_id)
{
	/*----------------------------------------------------------------*/
		/* Local Variables												  */
		/*----------------------------------------------------------------*/
		/* Find one subsession_entry and its used =1 (means allocates RXTX buf ) and subsession_state = IDLE */
		int i = 0;
	 	jbt_list *existing_entry = NULL;
    	jbt_session_info *session_entry = NULL;
    	jbt_subsession *subsession_entry = NULL;
	
		/*----------------------------------------------------------------*/
		/* Code Body													  */
		/*----------------------------------------------------------------*/

   		if (session_type == JBT_SESSION_TYPE_SPP)
		{
			existing_entry = jbt_search_an_existing_entry(
								&jbt_jsr82.spp.activeList,
								JBT_LIST_SEARCH_TYPE_BY_INX,
								session_inx,
								JBT_LIST_TYPE_SPP_SESSION);
		}
		else if (session_type == JBT_SESSION_TYPE_L2CAP)
		{
			existing_entry = jbt_search_an_existing_entry(
								&jbt_jsr82.l2cap.activeList,
								JBT_LIST_SEARCH_TYPE_BY_INX,
								session_inx,
								JBT_LIST_TYPE_L2CAP_SESSION);
		}
		else
		{
			return NULL;
		}
	
		if (existing_entry == NULL)
		{
			bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, session_inx);
			return NULL;
		}
		session_entry = (jbt_session_info*) existing_entry;
	
		subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_INX, subsession_id);
#if (JBT_MAX_SUBSESSION_NO > 1)
		if (subsession_entry == NULL)
		{
			ASSERT(0);
		}
#endif /* (JBT_MAX_SUBSESSION_NO > 1) */
	ASSERT(subsession_id == subsession_entry->subsession_id);
	return subsession_entry;
	
}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_check_this_psm_chnl_nun_existed
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ps_type                 [IN]        
 *  existing_chnl_num       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_bool jbt_session_check_this_psm_chnl_nun_existed(kal_uint8 ps_type, kal_uint16 existing_chnl_num)
{
    kal_uint32 existed_value = 0;
    int i = 0;
    int max_session_no = 0;
    jbt_session_info *session_entry = NULL;


    if (existing_chnl_num == 0)
    {
        return KAL_FALSE;
    }
#if 0
    else if (ps_type == JSR82_SESSION_PS_L2CAP)
    {
        return KAL_FALSE;
    }
#endif

    /* Only Server role should be checked. */
    if (JSR82_SESSION_PS_L2CAP == ps_type)
    {
        max_session_no = JBT_MAX_L2CAP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.l2cap.l2cap;
    }
    else if (JSR82_SESSION_PS_RFCOMM == ps_type)
    {
        max_session_no = JBT_MAX_SPP_NO;
        session_entry = (jbt_session_info*) jbt_jsr82.spp.spp;
    }
    else
    {
    	ASSERT(0);
    }

    for (i = 0; i < max_session_no; i++)
    {
        if ((existing_chnl_num == session_entry->psm_channel) && (1 == session_entry->used) 
        	&& (JBT_SESSION_INITIATOR_FALSE == session_entry->initiator))
        {
            return KAL_TRUE;
        }
        else
        {
            session_entry += 1;
        }
    }

    if (max_session_no == i)
    {
    	bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", 
		 			JBT_LIST_SEARCH_TYPE_BY_CHNL_NUM, existing_chnl_num);
        return KAL_FALSE;
    }
    else
    {
        return KAL_TRUE;
    }
}

/*****************************************************************************
 * FUNCTION
 *  jbt_session_general_reset
 * DESCRIPTION
 *    
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL jbt_session_general_reset()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct ilm;
    bt_jsr82_reset_req_struct *req = (bt_jsr82_reset_req_struct *)ilm.ilm_data;
   	BtStatus status;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   
    status = JSR82_SendMessage(MSG_ID_BT_JSR82_RESET_REQ, MOD_BT, req, sizeof(bt_jsr82_reset_req_struct));
  
    return (BT_STATUS_SUCCESS == status);
}



/*****************************************************************************
 * FUNCTION
 *  jbt_session_general_service_registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ps_type                 [IN]        
 *  mtu                     [IN]        
 *  security                [IN]        
 *  transaction_id          [IN]        
 *  list_entry              [IN]        
 *  subsession_entry        [IN]        
 *  list_type               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL jbt_session_general_service_registration(jbt_session_info *session_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct ilm;
    bt_jsr82_enable_service_req_struct *req = (bt_jsr82_enable_service_req_struct *)ilm.ilm_data;
   	BtStatus status;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   
   	req->identify = session_entry->transaction_id;
    req->mtu = session_entry->mtu;
    req->ps_type = session_entry->ps_type;
    req->security_value = session_entry->security_value;
    req->existing_psm_chnl_num = session_entry->psm_channel;
	req->sessionid = session_entry->session_id;
    status = JSR82_SendMessage(MSG_ID_BT_JSR82_ENABLE_SERVICE_REQ, MOD_BT, req, sizeof(bt_jsr82_enable_service_req_struct));

    return (BT_STATUS_SUCCESS == status);

}

BT_BOOL jbt_session_general_service_deregistration(jbt_session_info *session_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_jsr82_disable_service_req_struct req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req.index = session_entry->index;
    req.identify = session_entry->transaction_id;
    JSR82_SendMessage(MSG_ID_BT_JSR82_DISABLE_SERVICE_REQ, MOD_BT, &req, sizeof(bt_jsr82_disable_service_req_struct));

    return TRUE;
}
BT_BOOL jbt_session_turn_on_service(jbt_session_info *session_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_jsr82_turnon_service_req_struct req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	if (session_entry->turn_on_op.operation_state != JBT_OPERATION_STATE_NONE)
    {
        return FALSE;
    }
    else
    {
        session_entry->turn_on_op.operation_state = JBT_OPERATION_STATE_ONGOING;
    }
	memset(&req, 0x0, sizeof(bt_jsr82_turnon_service_req_struct));
	req.ps_type = session_entry->ps_type;
	req.index = session_entry->index;
	req.identify = session_entry->transaction_id;
	JSR82_SendMessage(MSG_ID_BT_JSR82_TURNON_SERVICE_REQ, MOD_BT, &req, sizeof(bt_jsr82_turnon_service_req_struct));
	return TRUE;
}
BT_BOOL jbt_session_turn_off_service(jbt_session_info *session_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_jsr82_turnoff_service_req_struct req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	if (session_entry->turn_on_op.operation_state != JBT_OPERATION_STATE_NONE)
    {
        return FALSE;
    }
    else
    {
        session_entry->turn_on_op.operation_state = JBT_OPERATION_STATE_ONGOING;
    }

	memset(&req, 0x0, sizeof(bt_jsr82_turnoff_service_req_struct));
	req.ps_type = session_entry->ps_type;
	req.index = session_entry->index;
	req.identify = session_entry->transaction_id;
	JSR82_SendMessage(MSG_ID_BT_JSR82_TURNOFF_SERVICE_REQ, MOD_BT, &req, sizeof(bt_jsr82_turnoff_service_req_struct));
	return TRUE;
}
BT_BOOL jbt_session_general_connect(jbt_session_info *session_entry, jbt_subsession *subsession_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
   	bt_jsr82_connect_req_struct req;
	BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/	
	memset(&req, 0x0, sizeof(bt_jsr82_connect_req_struct));
    req.ps_type = session_entry->ps_type;
    req.mtu = session_entry->mtu;
    req.channel = session_entry->psm_channel;
    req.identify = session_entry->transaction_id;
    req.security_value = session_entry->security_value;
	memcpy(req.bd_addr, subsession_entry->bd_addr, 6);
	memcpy(req.uuid_128, session_entry->uuid_128, 16);
    	
	req.sessionid = session_entry->session_id;
    status = JSR82_SendMessage(MSG_ID_BT_JSR82_CONNECT_REQ, MOD_BT, &req, sizeof(bt_jsr82_connect_req_struct));
    return (BT_STATUS_SUCCESS == status);
}
BT_BOOL jbt_session_general_disconnect(jbt_session_info *session_entry, jbt_subsession *subsession_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
   	bt_jsr82_disconnect_req_struct req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/	
    req.identify = session_entry->transaction_id;
    req.index = session_entry->index;
    req.l2cap_id = subsession_entry->l2cap_id;
    req.ps_type = session_entry->ps_type;
    JSR82_SendMessage(MSG_ID_BT_JSR82_DISCONNECT_REQ, MOD_BT, &req, sizeof(bt_jsr82_disconnect_req_struct));
	return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  jbt_reset_subsession_entry_except_used
 * DESCRIPTION
 *  
 * PARAMETERS
 *  subsession_entry        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_reset_subsession_entry_except_used(jbt_subsession *subsession_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    subsession_entry->transaction_id = 0;
    subsession_entry->subsession_id = 0;
  //  subsession_entry->is_tx_empty = KAL_TRUE;
//	subsession_entry->is_lower_tx_empty = KAL_TRUE;
    subsession_entry->notifyReadyToWrite = KAL_FALSE;
    subsession_entry->readyToRead = KAL_TRUE;
    subsession_entry->subsession_state = JBT_SESSION_STATE_IDLE;

}


/*****************************************************************************
 * FUNCTION
 *  jbt_reset_subsession_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  subsession_entry        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_reset_subsession_entry(jbt_subsession *subsession_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    subsession_entry->used = 0;
	subsession_entry->subsession_state = JBT_SESSION_STATE_IDLE;
	memset(subsession_entry->bd_addr, 0, 6);
	#ifdef __USE_BT_RING_BUF_API__
        ext_RING_BufFlush(&(subsession_entry->ReadRingBuf));
        ext_RING_BufFlush(&(subsession_entry->WriteRingBuf));
    #else /* __USE_BT_RING_BUF_API__ */ 
        JBTRING_BufFlush(&(subsession_entry->ReadRingBuf));
        JBTRING_BufFlush(&(subsession_entry->WriteRingBuf));
    #endif /* __USE_BT_RING_BUF_API__ */ 

	if (0 != subsession_entry->subsession_id)
	{
		bt_session_lock(subsession_entry->subsession_id, FALSE);
		jbt_session_detach(subsession_entry->subsession_id);
	}
	
	jbt_session_free_RWRingBuf(subsession_entry);
    jbt_reset_subsession_entry_except_used(subsession_entry);
}


/*****************************************************************************
 * FUNCTION
 *  jbt_reset_session_entry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list_type               [IN]        
 *  existing_entry          [IN]        
 *  subsession_entry        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_reset_session_entry(jbt_session_info *existing_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_session_info *session_entry = existing_entry;
	jbt_subsession *subsession_entry = NULL;
	int i;
	kal_uint8 list_type = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT: jbt_reset_session_entry()");
  
	for (i = 0; i < JBT_MAX_SUBSESSION_NO; i++)
	{
		subsession_entry = &session_entry->subsession[i];
		if (1 == subsession_entry->used)
		{
			bt_ext_log("[JSR82][JBT] JBT UT: Deregister service success: release rw ring buffer");
			jbt_session_free_RWRingBuf(subsession_entry);
			jbt_reset_subsession_entry(subsession_entry);
		}
	}
	jbt_session_detach(session_entry->session_id);
	if (session_entry->twin_session_id)
	{
		jbt_session_detach(session_entry->twin_session_id);
	}
	list_type = jbt_convert_list_type(session_entry->ps_type);
	jbt_free_one_existing_entry(list_type, (jbt_list *)existing_entry);
	session_entry->initiator = JBT_SESSION_INITIATOR_FALSE;
	session_entry->used = 0;
	session_entry->psm_channel = 0;
	session_entry->subsession_count = 0;
	/* Clear transaction_id in context */
	session_entry->transaction_id = 0;   
	session_entry->session_id = 0;
	session_entry->twin_session_id = 0;
}




/*****************************************************************************
 * FUNCTION
 *  jbt_session_allocate_RWRingBuf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type            [IN]        
 *  subsession_entry        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int8 jbt_session_allocate_RWRingBuf(kal_uint8 session_type, jbt_subsession *subsession_entry, U8 index)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 rc = 0;    /* rc=0 : Means successful */
    kal_uint16 rx_size = 0, tx_size = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rx_size = JSR82_GET_DATA_SHARED_BUFFER_SIZE;
    tx_size = JSR82_PUT_DATA_SHARED_BUFFER_SIZE;

//    subsession_entry->ReadRingBuf.pStart = (kal_uint8*) btmtk_jsr82_get_shared_get_buffer_pointer(index);

#ifdef __USE_BT_RING_BUF_API__
	ext_RING_BufInit(&subsession_entry->ReadRingBuf, (char *)subsession_entry->rxbuf, rx_size);
	ext_RING_BufInit(&subsession_entry->WriteRingBuf, (char *)subsession_entry->txbuf, tx_size);
#else 
	JBTRING_BufInit(&subsession_entry->ReadRingBuf, (char *)subsession_entry->rxbuf, rx_size);
	JBTRING_BufInit(&subsession_entry->WriteRingBuf, (char *)subsession_entry->txbuf, tx_size);
#endif 
    
    if (subsession_entry->ReadRingBuf.pStart == NULL)
    {
        /* Failed of jvm_malloc() */
        rc = JBT_SESSION_FAILED_ALLOCATE_RINGBUF;   /* Failed to allocate memory space */
		bt_ext_log("[JSR82][JBT] JBT allocate for ReadRingBuf failed, size=%d", rx_size);
        return rc;
    }
    else
    {
        /* Allocate memory space successfully, assign pEnd and init pRead and pWrite */
        subsession_entry->ReadRingBuf.pEnd = subsession_entry->ReadRingBuf.pStart + rx_size;
    #ifdef __USE_BT_RING_BUF_API__
        ext_RING_BufFlush(&(subsession_entry->ReadRingBuf));
    #else 
        JBTRING_BufFlush(&(subsession_entry->ReadRingBuf));
    #endif 
    }

  //  subsession_entry->WriteRingBuf.pStart = (kal_uint8*) btmtk_jsr82_get_shared_put_buffer_pointer(index);
    
    if (subsession_entry->WriteRingBuf.pStart == NULL)
    {
        /* Failed of jvm_malloc() */
        rc = JBT_SESSION_FAILED_ALLOCATE_RINGBUF;   /* Failed to allocate memory space */
		bt_ext_log("[JSR82][JBT] JBT allocate for WriteRingBuf failed, size=%d", tx_size);
        return rc;
    }
    else
    {
        /* Allocate memory space successfully, assign pEnd and init pRead and pWrite */
        subsession_entry->WriteRingBuf.pEnd = subsession_entry->WriteRingBuf.pStart + tx_size;
    #ifdef __USE_BT_RING_BUF_API__
        ext_RING_BufFlush(&(subsession_entry->WriteRingBuf));
    #else 
        JBTRING_BufFlush(&(subsession_entry->WriteRingBuf));
    #endif 
    }
    return rc;

}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_free_RWRingBuf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  subsession_entry        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint8 jbt_session_free_RWRingBuf(jbt_subsession *subsession_entry)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 rc = 0x00;    /* 0x00: means successful */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (subsession_entry->ReadRingBuf.pStart != NULL)
    {
        subsession_entry->ReadRingBuf.pStart = NULL;
        subsession_entry->ReadRingBuf.pEnd = NULL;
        subsession_entry->ReadRingBuf.pRead = NULL;
        subsession_entry->ReadRingBuf.pWrite = NULL;
    }
    if (subsession_entry->WriteRingBuf.pStart != NULL)
    {
        subsession_entry->WriteRingBuf.pStart = NULL;
        subsession_entry->WriteRingBuf.pEnd = NULL;
        subsession_entry->WriteRingBuf.pRead = NULL;
        subsession_entry->WriteRingBuf.pWrite = NULL;
    }
    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_free_RWRingBuf_with_inx
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int8 jbt_session_free_RWRingBuf_with_inx(kal_uint8 session_type, U8 session_inx)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    kal_uint8 rc = 0x00;
    jbt_subsession *subsession_entry = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	session_entry = jbt_search_existing_session_entry(session_type, JBT_LIST_SEARCH_TYPE_BY_INX, session_inx);
	if (session_entry == NULL)
	{
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, session_inx);
		return JBT_SESSION_FIND_NO_ENTRY;
	}
	
    /* Find the corresponding subsession_entry via trasnaction_id */
    subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_INX, session_inx);
#if (JBT_MAX_SUBSESSION_NO > 1)
    if (subsession_entry == NULL)
    {
        ASSERT(0);
    }
#endif /* (JBT_MAX_SUBSESSION_NO > 1) */

    rc = jbt_session_free_RWRingBuf(subsession_entry);
    return rc;

}




/*****************************************************************************
 * FUNCTION
 *  jbt_ConvertBdAddrOrder
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [IN]        
 *  src         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_ConvertBdAddrOrder(kal_uint8 *dest, kal_uint8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    dest[0] = src[5];
    dest[1] = src[4];
    dest[2] = src[3];
    dest[3] = src[2];
    dest[4] = src[1];
    dest[5] = src[0];
}




U16 jbt_context_to_id(int ctxindex)
{
	if (ctxindex > (PROFILE_JSR82_DATA_RANGE - PROFILE_JSR82_DATA))
	{
		return 0xFF;
	}
	return (PROFILE_JSR82_DATA + ctxindex);
}

SESSION_RESULT jbt_iternal_session_attach(U32 ids[2], int fds[2])
{
	SESSION_RESULT sret = SR_NOERROR;
	int fd = -1;
	U32 index = 0;

	sret = bt_make_internal_session(ids);
	if (SR_NOERROR != sret)
	{
		goto exit;
	}

	for (index = 0; index < 2; index ++)
	{
		sret = bt_session_get_fd(ids[index], &fd);
		if (sret == SR_NOERROR)
		{		
			bt_ext_log("[JSR82][EXT]jbt_iternal_session_attach fd:%d", fd);
			fds[index] = fd;
		}
	}

	//the ownership of fd[0] has been transferred to app, so do not operate on this fd
	sret = bt_session_lock(ids[0], TRUE);
	if (sret != SR_NOERROR)
	{
		bt_ext_log("[JSR82][EXT]bt_session_lock fail %d", sret);
	}
exit:
	return sret;
}



SESSION_RESULT jbt_session_attach(int contextid, U32 *sessionid, int *sessionfd)
{
	SESSION_RESULT sret = SR_NOERROR;
	int fd = -1;
	U16 id = 0;
	U32 index = 0;

	id = jbt_context_to_id(contextid);
	sret = bt_session_connect(id ,&index);
	if (SR_NOERROR != sret)
	{
		goto exit;
	}
	
	sret = bt_session_get_fd(index, &fd);
	if (sret == SR_NOERROR)
	{
		*sessionid = index;
		*sessionfd = fd;
		bt_ext_log("[JSR82][EXT]jbt_session_attach fd:%d", fd);
	}

	//the ownership of this fd has been transferred to app, so do not operate on this fd
	sret = bt_session_lock(index, TRUE);
	if (sret != SR_NOERROR)
	{
		bt_ext_log("[JSR82][EXT]bt_session_lock fail %d", sret);
	}
exit:
	return sret;
}
void jbt_session_detach(U32 sessionid)
{
	bt_session_destroy(sessionid);
}

SESSION_RESULT jbt_session_start_monitor(U16 *monitorid)
{
	SESSION_RESULT sret = SR_NOERROR;
	sret = bt_session_register_handler(INTERNAL_RESERVED, 
									   INTERNAL_RESERVED_END, 
									   jbt_session_monitor_routine, 
									   monitorid);	
	return sret;
}
SESSION_RESULT jbt_session_stop_monitor(U16 monitorid)
{
	SESSION_RESULT sret = SR_NOERROR;
	sret = bt_session_unregister_handler(monitorid);	
	return sret;
}




void jbt_session_event_detached(U32 index)
{
	//find session context and subsession context
	jbt_session_info *session_entry = NULL;

	bt_ext_log("[JSR82]jbt_session_event_detached: %x", index);

	session_entry = jbt_search_existing_session_entry(JSR82_SESSION_PS_RFCOMM, JBT_LIST_SEARCH_TYPE_BY_SESSION_ID, index);
	if (session_entry != NULL)
	{
		goto exit;
	}

	session_entry = jbt_search_existing_session_entry(JSR82_SESSION_PS_L2CAP, JBT_LIST_SEARCH_TYPE_BY_SESSION_ID, index);
	if (session_entry != NULL)
	{
		goto exit;
	}

exit:
	if (session_entry)
	{
		//server
		if(session_entry->initiator == JBT_SESSION_INITIATOR_FALSE)
		{
			//sdp deregister
			btmtk_jsr82_sdp_deregistration(&session_entry->sdpinfo);
			
			jbt_session_general_service_deregistration(session_entry);
		}
	}
}


SESSION_RESULT jbt_session_monitor_routine(Session_Event_PARM *parm)
{
	if (parm == NULL || parm->event == SESSION_EVENT_INVALID)
	{
		return SR_E_INVALID_PARAM;
	}
	
	if (parm->event & SESSION_EVENT_DESTROYED)
	{
		jbt_session_event_detached(parm->index);		
	}	
	return SR_NOERROR;
}
void jbt_jsr82_session_registered(jbt_session_info *session)
{
	int length; 
	int channel;

	ASSERT(session);
	channel = session->psm_channel;
	
	bt_ext_log("[JSR82] jbt_jsr82_session_registered: %d", channel);
	
	length = bt_session_write(session->session_id, &channel, sizeof(int));
}


void jbt_jsr82_channel_connected(jbt_session_info *session, jbt_subsession *subsession)
{
	int length; 

	ASSERT(session);

	//send signal to external layer	
	bt_ext_log("[JSR82] jbt_jsr82_channel_connected: channel: %d, session fd[%d], subsessionfd[%d]", 
		session->psm_channel,
		session->fd,
		subsession->fd);

	connection_signal cs;
    cs.size = sizeof(cs);
	memcpy(cs.bd_addr.address, subsession->bd_addr, 6);
    cs.channel = session->psm_channel;
    cs.status = 0; //0 means success

	bt_session_write_control(session->session_id, &subsession->fd, sizeof(int), &cs, sizeof(cs));
}




#endif /* __BT_JSR82__ || __BT_JSR82_L2RF__ */ 





