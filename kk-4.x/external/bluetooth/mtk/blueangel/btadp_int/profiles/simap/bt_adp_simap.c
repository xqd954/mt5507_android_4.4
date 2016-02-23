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



/* basic system service headers */
#ifdef __SIMAP_MAUI__
#include "kal_release.h"        /* Basic data type */
#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h" /* enum for message IDs */
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"     /* type of system module components */
#include "task_config.h"        /* Task creation */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "kal_trace.h"  /* Trace Header Files */
#include "uart_sw.h"
//#include "bt_maui_utility.h"
#endif
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt.h"
#include "btconfig.h"
#include "bt_simi.h"
#include "bt_adp_simap.h"
#include "bluetooth_struct.h"
#include "bluetooth_trc.h"
#include "sdpdb.h"
#include "config.h"
#include "btaccctrl.h"
#include "bt_sim.h"
#include "bt_adp_simap_event.h"
#include "bluetooth_simap_message.h"


/*****************************************************************************
 * FUNCTION
 *  bt_simap_start_timer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer_id_p          [IN]        
 *  timer_duration      [IN]        
 *  funct_ptr       32
**********************************************************/
#ifdef __SIMAP_MAUI__
void bt_simap_start_timer(eventid *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr)
{
    eventid timer_evt_id = *timer_id_p;

    kal_trace(TRACE_GROUP_1, BT_SIMAP_START_TIMER, timer_duration);
    if (bt_p->os_init == TRUE)
    {
        if (timer_evt_id != 0)
        {
            evshed_cancel_event(bt_p->evsh_sched, timer_id_p);
            *timer_id_p = 0;
        }

        *timer_id_p = evshed_set_event(bt_p->evsh_sched, funct_ptr, NULL, MS_TO_TICKS(timer_duration));
    }
}
#else
void bt_simap_start_timer(void *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr)
{
	EvmTimer *p_timer;

	p_timer = (EvmTimer *) timer_id_p;
	p_timer->context = NULL;
	p_timer->func = (EvmTimerNotify)funct_ptr;
	
	EVM_StartTimer(p_timer, timer_duration);
}
#endif


/*****************************************************************************
 * FUNCTION
 *  bt_simap_stop_timer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer_id_p      [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef __SIMAP_MAUI__
void bt_simap_stop_timer(eventid *timer_id_p)
{
    eventid timer_evt_id = *timer_id_p;

    kal_trace(TRACE_GROUP_1, BT_SIMAP_STOP_TIMER);
    if (bt_p->os_init == TRUE)
    {
        if (timer_evt_id != 0)
        {
            evshed_cancel_event(bt_p->evsh_sched, timer_id_p);
            *timer_id_p = 0;
        }
    }
}
#else
void bt_simap_stop_timer(void *timer_id_p)
{
    EVM_CancelTimer((EvmTimer *)timer_id_p);
}
#endif


void simap_adp_init(void)
{
	kal_trace(TRACE_GROUP_1, BT_SIMAP_ADP_INIT);
	SIMAP_Register(btmtk_adp_simap_int_event_callback);
	bt_simap_init();
}

void simap_adp_deinit(void)
{
	kal_trace(TRACE_GROUP_1, BT_SIMAP_ADP_DEINIT);
	bt_simap_deinit();
}

void simap_adp_reset(void)
{
	kal_trace(TRACE_GROUP_1, BT_SIMAP_ADP_RESET);
	bt_simap_reset();
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_adp_simap_handle_message
 * DESCRIPTION
 *  This function is used to process the msg request dispatched from the function bt_main()
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_simap_handle_message(ilm_struct *ilm_ptr)
{
    if (ilm_ptr == NULL)
    {
        Assert(0);
    }
	kal_trace(TRACE_GROUP_1, BT_SIMAP_ADP_HDL_MSG, ilm_ptr->msg_id);

    switch (ilm_ptr->msg_id)
    {
            /* from APP --> BT SIM Access Profile */
        case MSG_ID_BT_SIMAP_ACTIVATE_REQ:
            bt_simap_activate_req_hdlr();
            break;

        case MSG_ID_BT_SIMAP_DEACTIVATE_REQ:
            bt_simap_deactivate_req_hdlr();
            break;

        case MSG_ID_BT_SIMAP_AUTH_RSP:
            bt_simap_auth_rsp_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_SIMAP_DISCONNECT_REQ:
            bt_simap_disconnect_req_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_SIMAP_SEND_STATUS_IND_WITH_UNAVAILABLE_REQ:
            SimSrvSimCardUnavailableSendStatusIndvoid();
            break;

            /* from SIM task --> BT SIM Access Profile */
        case MSG_ID_BT_SIM_CONNECT_CNF:
            bt_sim_connect_cnf_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_SIM_RESET_CNF:
            bt_sim_reset_cnf_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_SIM_APDU_CNF:
            bt_sim_apdu_cnf_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_SIM_DISCONNECT_CNF:
            bt_sim_disconnect_cnf_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_SIM_POWER_OFF_CNF:
            bt_sim_power_off_cnf_hdlr(ilm_ptr);
            break;

#ifdef __SIMAP_UT_
        case MSG_ID_BT_SIMAP_INJECT_MESSAGE:
            simaptester_handler(ilm_ptr);
            break;
#endif

        default:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_NO_DEFINED_MSG_ID);
            Assert(0);
            break;

    }

}

