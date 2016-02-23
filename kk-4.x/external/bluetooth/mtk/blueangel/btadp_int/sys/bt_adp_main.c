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
 * bt_adp_main.c
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
 * Derek Cho
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
#ifdef BTMTK_ON_WISE 

/***************************************************************************** 
* Include
*****************************************************************************/
/* basic system service headers */
#include "kal_release.h"        /* Basic data type */
#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h" /* enum for message IDs */
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"     /* type of system module components */
#include "task_config.h"        /* Task creation */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "stdio.h"      /* Basic C I/O functions */
#include "gpio_sw.h"    /* for GPIO */
#include "app_buff_alloc.h"     /* get_ctel_buffer */

#include "nvram_struct.h"       /* for NVRAM read/write */
#include "Nvram_user_defs.h"
#include "bluetooth_trc.h"
/* internal headers */
#include "bt.h"

#include "bt_profile_init.h"
#include "btconfig.h"

#include "bt_jsr82_setting.h"

/* if the bluetooth_struct.h is included in your C file, also the uart_sw.h must be included. */
#include "uart_sw.h"
#include "bluetooth_struct.h"
#include "bt_test_cmd.h"
#include "bt_debug.h"   //for bt_trace


#ifdef __BT_BEFORE_PARTITION__ 
#include "bt_maui_utility.h"
#include "bluetooth_bm_struct.h"
#endif  /* __BT_BEFORE_PARTITION__ */

/* CSJ_PBAP */
#ifdef __BT_PBAP_PROFILE__
#include "bt_adp_pbap.h"
#endif 

#ifdef __OBEX_ADAP_SUPPORT__
#include "bt_adp_oapi.h"
#endif

#ifdef __BT_PAN_PROFILE__
#include "bt_adp_pan.h"
#endif

#if defined(__BT_GATTC_PROFILE__) || defined(__BT_GATTS_PROFILE__)
#include "bt_adp_gatt.h"
#endif

/***************************************************************************** 
* Define
*****************************************************************************/

/***************************************************************************** 
* Typedef 
*****************************************************************************/

/***************************************************************************** 
* Local Variable
*****************************************************************************/

/***************************************************************************** 
* Local Function
*****************************************************************************/
kal_bool bt_main(ilm_struct *ilm_ptr);


/***************************************************************************** 
* External Function
*****************************************************************************/
void bpp_adp_msg_hdlr(ilm_struct *ilm_ptr);


/***************************************************************************** 
* Global Variable
*****************************************************************************/
bt_context_struct bt_cntx;
bt_context_struct *bt_p = &bt_cntx;

/***************************************************************************** 
* Global Function
*****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  bt_start_base_timer
 * DESCRIPTION
 *  This is l4c_start_base_timer function of L4C module.
 *  provide the function pointer for create a event scheduler
 * PARAMETERS
 *  base_timer_ptr      [IN]        Pointer to timer
 *  time_out            [IN]        Time out value
 * RETURNS
 *  void
 *****************************************************************************/
void bt_start_base_timer(void *base_timer_ptr, unsigned int time_out)
{
    stack_start_timer((stack_timer_struct*) base_timer_ptr, 0, time_out);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  bt_stop_base_timer
 * DESCRIPTION
 *  This is l4c_stop_base_timer function of L4C module.
 *  provide the function pointer for create a event scheduler
 * PARAMETERS
 *  base_timer_ptr      [IN]        Pointer to timer
 * RETURNS
 *  void
 *****************************************************************************/
void bt_stop_base_timer(void *base_timer_ptr)
{
    stack_stop_timer((stack_timer_struct*) base_timer_ptr);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  bt_init
 * DESCRIPTION
 *  Init function if BT task
 * PARAMETERS
 *  task_index      [IN]        Index of the taks
 * RETURNS
 *  TRUE if reset successfully
 *****************************************************************************/
kal_bool bt_init(task_indx_type task_index)
{
    /* Stack init */

    bt_p->os_init = FALSE;
    bt_p->NotifyEVM = FALSE;

    /*
     * Timer initialization: we need to determine the maximum timer 
     * * amount.
     */
    stack_init_timer(&bt_p->base_timer, "BT_Timer", MOD_BT);

    bt_p->evsh_sched = new_evshed(
                        &bt_p->base_timer,
                        bt_start_base_timer,
                        bt_stop_base_timer,
                        0,
                        kal_evshed_get_mem,
                        kal_evshed_free_mem,
                        0);

    bt_p->l1_sleep_handle = L1SM_GetHandle();
    bt_p->reset_count = 0;
    bt_p->is_reset = FALSE;
    bt_p->is_poweron = FALSE;
    bt_p->entering_poweroff = FALSE;
    bt_p->bt_test_mode = 0;
    bt_p->bt_read_addr = 0;
    bt_p->is_engineering_mode = 0;
    bt_p->bt_em_operation = 0;
    bt_p->bt_em_state = 0;
    bt_p->bt_rftest_buffer = NULL;
    bt_p->power_status = 0;
    bt_p->bt_uart_rx_speed = kal_create_mutex("BT_UART_SPEED");
    bt_p->uart_rx_slow = 0;
    bt_p->uart_rx_slow_msg = 0;
    bt_p->uart_rx_current_setting = 0;

    if (stack_query_boot_mode() == FACTORY_BOOT)
    {
        bt_p->is_factory_mode = TRUE;
    }
    else
    {
        bt_p->is_factory_mode = FALSE;
    }
    return KAL_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_reset
 * DESCRIPTION
 *  Reset function if BT task
 * PARAMETERS
 *  task_index      [IN]        Index of the taks
 * RETURNS
 *  TRUE if reset successfully
 *****************************************************************************/
kal_bool bt_reset(task_indx_type task_index)
{
    return KAL_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_task_main
 * DESCRIPTION
 *  This function is the main function of BT task
 * PARAMETERS
 *  task_entry_ptr      [IN]        Taks entry of BT
 * RETURNS
 *  void
 *****************************************************************************/
void bt_task_main(task_entry_struct *task_entry_ptr)
{
    ilm_struct current_ilm;

    kal_uint32 my_index;

    kal_get_my_task_index(&my_index);
    stack_set_active_module_id(my_index, MOD_BT);

    /* Init Coexistence Structure */
    BTInitWifiCoexistence();

    while (1)
    {
        receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);

        bt_main((void*)&current_ilm);

        free_ilm(&current_ilm);

    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_create
 * DESCRIPTION
 *  This function creats BT task
 * PARAMETERS
 *  handle      [OUT]       Pointer to the table of BT task structure.
 * RETURNS
 *  Always TRUE since the creation shall not be failed.
 *****************************************************************************/
kal_bool bt_create(comptask_handler_struct **handle)
{
    static const comptask_handler_struct bt_handler_info = 
    {
        bt_task_main,   /* task entry function */
        bt_init,        /* task initialization function */
        NULL,           /* task configuration function */
        bt_reset,       /* task reset handler */
        NULL,           /* task termination handler */
    };

    *handle = (comptask_handler_struct*) & bt_handler_info;
    return KAL_TRUE;
}

#elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /* BTMTK_ON_WISE */

#include <stdio.h>
#include <windows.h> /* For the CreateThread prototype */
#include "win_service.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_msg.h"
#include "bt_adp_system.h"
#include "bluetooth_gap_struct.h"
#include "bt_jsr82_setting.h"

#elif defined (BTMTK_ON_LINUX) /* #elif defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
#include <stdio.h>
//#include "win_service.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_msg.h"
#include "bt_adp_system.h"
#include "bluetooth_gap_struct.h"

#ifdef __BT_SINGLE_ANTENNA__
#include "bluetooth_bwcs.h"
#include "wmt_struct.h"
#include "bt_wmt_api.h"
#endif

#include "bt_feature.h"
#include "eventmgr.h"

#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
#include "bt_adp_jsr82.h"
#endif

#ifdef __BT_PAN_PROFILE__
#include "bt_adp_pan.h"
#endif

#if defined(__BT_GATTC_PROFILE__) || defined(__BT_GATTS_PROFILE__)
#include "bt_adp_gatt.h"
#endif

#include "btalloc.h"

/* External functions */
extern int logToFile(char *name);
extern void bt_em_handler(ilm_struct *ilm_ptr);

#ifdef __BT_SINGLE_ANTENNA__
extern void write_wifi_setting(bt_wifi_set_chnl_req_struct *ptr); 
#endif

#ifdef __BT_HS_30__
extern void a2mp_adp_message_handler(ilm_struct *ilm_ptr);
#endif

#endif /* #elif defined (BTMTK_ON_LINUX) */


#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)
extern void btmtk_adp_prx_handle_message(ilm_struct *ilm_ptr);
#endif

#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
extern void btmtk_adp_jsr82_handle_message(ilm_struct *ilm_ptr);
#endif

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__    
extern void BTSetSCOAcceptSetting(U8 setting);
extern void BTSetDualPCMSetting(U8 *value, U16 len);
extern void BTSetDualPCMSwitch(U8 *value, U16 len);
#endif

extern int switchPort(int bttm);

#ifdef __BT_TEST_MODE_ENABLED__
extern void testModeCnf(kal_bool result, kal_bool query);
extern U8 MeEnterTestMode(void);
static int readTmpAddr(unsigned char *addr){
    int ret= -1;
    FILE *fp = fopen("/data/BT_Addr", "r");
    unsigned char ori_addr[6];
    bt_android_log("[TM] readTmpAddr(%p)", addr);
    bt_prompt_trace(MOD_BT, "[TM] readTmpAddr(%u)", addr);
    if(fp){
        if(addr){
            ret = fread(addr, 6, 1, fp);
            if( ret == 1){
                ret = 1;
            }else{
                bt_android_log("[TM][ERR] fread failed : ret=%d, ferror=%d", ret, ferror(fp));
                bt_prompt_trace(MOD_BT, "[TM][ERR] fread failed : ret=%d, ferror=%d", ret, ferror(fp));
            }
        }
        fclose(fp);
    }else{
        bt_android_log("[TM][ERR] open /data/BT_Addr failed : ferror=%d", ferror(fp));
        bt_prompt_trace(MOD_BT, "[TM][ERR] open /data/BT_Addr failed : ferror=%d", ferror(fp));
    }
    return ret;
}
#endif
/*****************************************************************************
 * FUNCTION
 *  bt_main
 * DESCRIPTION
 *  Init function if BT task
 * PARAMETERS
 *  ilm_ptr      [IN]
 * RETURNS
 *  TRUE if reset successfully
 *****************************************************************************/
kal_bool bt_main(ilm_struct *ptr)
{
    //bt_prompt_trace(MOD_BT, "bt_main(0x%x), msg_id=%d", ptr, ptr->msg_id);
#ifdef BTMTK_ON_WISE 
    ptr->local_para_ptr = (void *)ptr->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    ptr->local_para_ptr = (void *)ptr->ilm_data;
#endif  /* BTMTK_ON_WISE */       

    /* Handle the message */
    switch(ptr->msg_id)
    {
#ifdef BTMTK_ON_WISE             
        case MSG_ID_TIMER_EXPIRY:
            /* Check if the base timer is stopped or not */
            if (stack_is_time_out_valid(&bt_p->base_timer))
            {
                /* Execute event's timeout handler */
                evshed_timer_handler(bt_p->evsh_sched);
            }
            /* Shoulbe be paired with stack_is_time_out_valid() */
            stack_process_time_out(&bt_p->base_timer);
            break;
        case MSG_ID_UART_READY_TO_READ_IND:
        case MSG_ID_BT_DATA_TO_READ_IND:
        {
            if (bt_p->os_init == TRUE)
            {                                           /* avoid BT send garbage */
                bt_p->UartDataReady = FALSE;
                bt_p->UartCallback(UE_DATA_TO_READ);    /* uartReadData */
            }
            break;
        }
        case MSG_ID_UART_READY_TO_WRITE_IND:
        {
            bt_p->UartCallback(UE_WRITE_COMPLETE);  /* uartTransmitData */
            break;
        }                
        case MSG_ID_BT_NOTIFY_EVM_IND:
            bt_p->NotifyEVM = FALSE;
            EVM_Process();
            break;
#ifdef __BQE_TESTER_
        case MSG_ID_TST_INJECT_STRING:
            bt_inject_string_hdlr(ptr->local_para_ptr, ptr->peer_buff_ptr);
            break;
#endif /* __BQE_TESTER_ */ 
        case MSG_ID_BT_HOST_WAKE_UP_IND:
        {
            /* call radio host wakeup function */
        #ifdef MTK_BT_DEBUG_TRACE
            bt_trace(TRACE_GROUP_1, HCI_PROCESS_CALL_RADIO_HOST_WAKEUP_FUNCTION);
        #endif 
#ifdef __ENABLE_SLEEP_MODE__
            Radio_hostWakeup();
            BTSysAdpHostSleep(0);
            /* Start x seconds verify system shall sleep timer */
            StartVerifyHostSleepTimer();
#else
            Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */
            break;
        }
        case MSG_ID_BT_GET_BT_VERSION_REQ:
            if (bt_p->os_init == TRUE)
            {
                BTGetBTVersionResult(KAL_TRUE);
            }
            else
            {
                BTGetBTVersionResult(KAL_FALSE);
            }
            break;
#else  /* BTMTK_ON_WISE */ 
#ifdef __EXTRA_DEBUG_FEATURE__
    case MSG_ID_BT_DBG_QUERY_CONTEXT_REQ:
         bt_dbg_query_req_hdlr(ptr);
         break;
#endif /* __EXTRA_DEBUG_FEATURE__ */

     case MSG_ID_BT_DBG_MANUAL_DUMP_REQ:
          bt_dbg_manual_dump_req_hdlr(ptr);
          break;
          
	case MSG_ID_BT_ENGINEER_MODE_LOG_MASK_REQ:
	{
		bt_engineer_mode_log_mask_cnf_struct *cnf_p = NULL;
		bt_engineer_mode_log_mask_req_struct *req_p = (bt_engineer_mode_log_mask_req_struct*)ptr->ilm_data;
		bt_log_setting(req_p->trace, req_p->primitive, req_p->prompt);
		cnf_p = construct_local_para(sizeof(bt_engineer_mode_log_mask_cnf_struct), TD_UL);
		cnf_p->result = KAL_TRUE;
		BT_SendMessage(MSG_ID_BT_ENGINEER_MODE_LOG_MASK_CNF, MOD_MMI, cnf_p, sizeof(bt_engineer_mode_log_mask_cnf_struct));
	}
		break;
        case MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_REQ:
        {
		bt_engineer_mode_log_to_file_cnf_struct *cnf_p = NULL;
		bt_engineer_mode_log_to_file_req_struct *req_p = (bt_engineer_mode_log_to_file_req_struct*)ptr->ilm_data;
               cnf_p = construct_local_para(sizeof(bt_engineer_mode_log_to_file_req_struct), TD_UL);
            #if !defined(__PLATFORM_SUPPORTED_CATCHER__)               
            cnf_p->result = (logToFile((S8*) req_p->name)<0) ? 1 : 0;
            #else
            cnf_p->result = 1; 
            #endif
            BT_SendMessage(MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_CNF, MOD_MMI, cnf_p, sizeof(bt_engineer_mode_log_to_file_cnf_struct));
        }
            break;
        case MSG_ID_TIMER_EXPIRY:
            bt_prompt_trace(MOD_BT, "MSG_ID_TIMER_EXPIRY");
            btmtk_adp_system_timer_handler();
            break;
        case MSG_ID_BT_NOTIFY_EVM_IND:
            //bt_prompt_trace(MOD_BT, "MSG_ID_BT_NOTIFY_EVM_IND");
            btmtk_adp_system_main_loop();
            break;
#if defined(__ENABLE_SLEEP_MODE__)
        case MSG_ID_BT_HOST_WAKE_UP_IND:
        {
            /* call radio host wakeup function */
            #ifdef MTK_BT_DEBUG_TRACE
            bt_trace(TRACE_GROUP_1, HCI_PROCESS_CALL_RADIO_HOST_WAKEUP_FUNCTION);
            #endif 
            Radio_hostWakeup();
            BTSysAdpHostSleep(0);
            /* Start x seconds verify system shall sleep timer */
            StartVerifyHostSleepTimer();
            break;
        }
#endif /* __ENABLE_SLEEP_MODE__ */

        //case MSG_ID_BT_WIFI_SET_CHNL_REQ:
#ifdef __BT_SINGLE_ANTENNA__
        case MSG_ID_WMT_BT_SET_BWCS_REQ:
        {
            bt_wifi_set_chnl_req_struct *msg_ptr;

            msg_ptr = (bt_wifi_set_chnl_req_struct*) (ptr->ilm_data);
            write_wifi_setting(msg_ptr);
            //if (bt_p->is_poweron)
            if (BTCoreVerifySysInitState() )
            {
                EVM_Process();
            }
        }
        break;
        case MSG_ID_WMT_BT_QUERY_STATE_REQ:
        {
            wmt_bt_query_state_req_struct *msg_ptr;
            msg_ptr = (wmt_bt_query_state_req_struct*) (ptr->ilm_data);
            BT_WMT_Query_State_handler();
        }
        break;
        case MSG_ID_WMT_BT_QUERY_RSSI_REQ:
            {
                wmt_bt_query_rssi_req_struct *msg_ptr;
                msg_ptr = (wmt_bt_query_rssi_req_struct*) (ptr->ilm_data);
                BT_WMT_Query_RSSI_handler(msg_ptr->bt_conn_handle);
            }
            break;
        case MSG_ID_WMT_BT_SET_RX_RANGE_REQ:
            {
                wmt_bt_set_rx_range_req_struct *msg_ptr;

                msg_ptr = (wmt_bt_set_rx_range_req_struct*) (ptr->ilm_data);
                BT_WMT_Set_Range_handler(msg_ptr->higher_boundary, msg_ptr->lower_boundary);
            }
            break;
        case MSG_ID_WMT_BT_SET_DEFAULT_TX_POWER_REQ:
            {
                wmt_bt_set_default_tx_power_req_struct *msg_ptr;

                msg_ptr = (wmt_bt_set_default_tx_power_req_struct*) (ptr->ilm_data);
                BT_WMT_Set_Default_Tx_Power_handler(msg_ptr->ucPower);
            }
            break;
        case MSG_ID_WMT_BT_UPDATE_CONN_TX_POWER_REQ:
            {
                wmt_bt_update_conn_tx_power_req_struct *msg_ptr;

                msg_ptr = (wmt_bt_update_conn_tx_power_req_struct*) (ptr->ilm_data);
                BT_WMT_Set_Update_Tx_Power_handler(msg_ptr->conn_handle, msg_ptr->ucPower);
            }
            break;          
#endif

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__    
        case MSG_ID_BT_SET_SCO_ACCEPT_REQ:
            BTSetSCOAcceptSetting(((bt_set_sco_accept_req_struct*)(ptr->ilm_data))->value);
            break;
        case MSG_ID_BT_SET_DUAL_PCM_SETTING_REQ:
            BTSetDualPCMSetting(((bt_set_dual_pcm_setting_req_struct*)(ptr->ilm_data))->value,
                                ((bt_set_dual_pcm_setting_req_struct*)(ptr->ilm_data))->val_len);
            break;
        case MSG_ID_BT_SET_DUAL_PCM_SWITCH_REQ:
            BTSetDualPCMSwitch(((bt_set_dual_pcm_switch_req_struct*)(ptr->ilm_data))->value,
                               ((bt_set_dual_pcm_switch_req_struct*)(ptr->ilm_data))->val_len);
            break;            
#endif

        case MSG_ID_BT_BTTM_SWITCH_PORT_REQ:
            {
                int ret = 1;
                bt_bttm_switch_port_cnf_struct *cnf_p = NULL;
                bt_bttm_switch_port_req_struct *req_p;
                req_p = (bt_bttm_switch_port_req_struct*) (ptr->ilm_data);
                ret = switchPort(req_p->bttm ? 1 : 0);
                cnf_p = construct_local_para(sizeof(bt_bttm_switch_port_cnf_struct), TD_UL);
                cnf_p->result = (ret < 0) ? KAL_FALSE : KAL_TRUE;
                BT_SendMessage(MSG_ID_BT_BTTM_SWITCH_PORT_CNF, MOD_MMI, cnf_p, sizeof(bt_bttm_switch_port_cnf_struct));
            }
            break;

        case MSG_ID_BT_TEST_MODE_REQ:
#ifdef __BT_TEST_MODE_ENABLED__
        {
            bt_test_mode_req_struct *req_p;
            req_p = (bt_test_mode_req_struct*) (ptr->ilm_data);
            if(req_p->mode == 0){
                testModeCnf(KAL_FALSE, KAL_TRUE);
            }else{
                if(readTmpAddr(MEC(bttmAddr).addr) < 0 
                    || MeEnterTestMode() == 0){
                    testModeCnf(KAL_FALSE, KAL_FALSE);
                }
            }
        }
#endif
            break;

#endif  /* BTMTK_ON_WISE */

        default:
            if((ptr->msg_id >= MSG_ID_BT_GAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GAP_GROUP_END))
            {
                switch(ptr->msg_id)
                {
                    case MSG_ID_BT_POWERON_REQ:
                        btmtk_adp_system_power_on_req();
                        break;
                    case MSG_ID_BT_POWEROFF_REQ:
                        btmtk_adp_system_power_off_req();
                        break;
                    default:
                        bm_adp_message_handler(ptr);
                        break;
                }
            }
#ifdef __BT_HS_30__
            else if((ptr->msg_id >= MSG_ID_PAL_MSG_CODE_BEGIN) && (ptr->msg_id <= MSG_TAG_PAL_BT_LAST_REQ_MSG))
            {
                a2mp_adp_message_handler(ptr);
                BTCoreSendNotifySelfMessage();
            }
#endif
#if defined(__BT_A2DP_PROFILE__)
#ifndef BTMTK_ON_LINUX
            else if((ptr->msg_id >= MSG_ID_BT_A2DP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_A2DP_GROUP_END))
            {
#else
            else if( ((ptr->msg_id >= MSG_ID_BT_A2DP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_A2DP_GROUP_END)) ||
            	           ((ptr->msg_id >= MSG_ID_BT_A2DP_DATA_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_A2DP_DATA_GROUP_END))
            	           #ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
						   ||((ptr->msg_id >= MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_GROUP_END))
						   #endif
						   )
            {
#endif
                /* A2DP handler */
                a2dp_adp_message_handler(ptr);
            }
#endif
#if defined(__BT_AVRCP_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_AVRCP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_AVRCP_GROUP_END))
            {
                avrcp_adp_message_handler(ptr);
            }
#endif
#if defined(__BT_HFG_PROFILE__)
            else if((ptr->msg_id >= MSG_ID_BT_HFG_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HFG_GROUP_END))
            {
                /* HFG handler */
                hfga_handler(ptr);
            }
#endif
#ifdef __BT_SPP_PROFILE__
            else if ((ptr->msg_id >= MSG_ID_BT_SPP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_SPP_GROUP_END))
            {
                /* SPP handler */
                btmtk_adp_spp_handle_message(ptr);
            }
#endif
    #if defined(__BT_OPP_PROFILE__) 
            else if( (ptr->msg_id >= MSG_ID_BT_GOEPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GOEPS_GROUP_END))
            {
                btmtk_adp_opp_handle_message(ptr);
            }
            else if( (ptr->msg_id >= MSG_ID_BT_OPPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_OPPS_GROUP_END) )
            {
                btmtk_adp_opp_handle_message(ptr);
            }
    #endif
    #if defined(__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__)   
                 else if((ptr->msg_id >= MSG_ID_BT_FTPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_FTPS_GROUP_END))
            {
                btmtk_adp_ftp_handle_message(ptr);
            }
    #endif
    #if defined(__BT_PRXR_PROFILE__)
            else if((ptr->msg_id >= MSG_ID_BT_PRXR_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PRXR_GROUP_END))
            {
                btmtk_adp_prx_handle_message(ptr);
            }
    #endif			
    #if defined(__BT_PRXM_PROFILE__)	
            else if((ptr->msg_id >= MSG_ID_BT_PRXM_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PRXM_GROUP_END))
            {
                btmtk_adp_prx_handle_message(ptr);
            }
    #endif    
    #if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_HID_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HID_GROUP_END))
            {
                btmtk_adp_hid_handle_message(ptr);
            }
    #endif
    #if defined(__BT_TESTING_PROFILE__)
            else if((ptr->msg_id >= MSG_ID_BT_CHN_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_CHN_GROUP_END))
            {
                chn_handler(ptr);
            }
    #endif
    #if defined (__BT_BIPI_PROFILES__) || defined (__BT_BIPR_PROFILE__)
         else if ((ptr->msg_id >= MSG_ID_BT_BIPI_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_BIPR_GROUP_END))
         {
             bip_adp_msg_hdlr(ptr);
         }
    #endif
    #if defined(__BT_PBAP_PROFILE__) 
            else if( (ptr->msg_id >= MSG_ID_BT_PBAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PBAP_GROUP_END))
            {
                bt_adp_pbap_handle_message(ptr);
            }
    #endif
    #if defined(__BT_SIM_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_SIMAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_SIMAP_GROUP_END))
            {
            	/* SIMAP handler */
                btmtk_adp_simap_handle_message(ptr);
            }
    #endif
            else if ((ptr->msg_id >= MSG_ID_BT_SDP_GROUP_START) && (ptr->msg_id <=MSG_ID_BT_SDP_GROUP_END))
            {
                sdpdba_handler(ptr);
            }

    #ifdef __BT_BPP_PROFILE__
            else if((ptr->msg_id >= MSG_ID_BT_BPP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_BPP_GROUP_END))
            {
                bpp_adp_msg_hdlr(ptr);
            }
    #endif
    #if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
            else if( (ptr->msg_id >= MSG_ID_BT_MAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_MAP_GROUP_END))
            {
                map_adp_message_handler(ptr);
            }
    #endif
    #if defined(__BT_JSR82__)
    #if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
            else if ((ptr->msg_id >= MSG_ID_BT_JSR82_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_JSR82_GROUP_END))
            {
            	/* JSR82 handler */
                btmtk_adp_jsr82_handle_message(ptr);
            }
    #endif
    #endif /* __BT_JSR82__ */
    #if defined(__BT_PAN_PROFILE__)
            else if ( (ptr->msg_id >= MSG_ID_BT_PAN_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PAN_GROUP_END)) 
            {
                btmtk_adp_pan_handle_message(ptr);
            }
    #endif
	#if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__)
			else if ((ptr->msg_id >= MSG_ID_BT_TIMEC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_TIMES_GROUP_END))
			{
				btmtk_adp_time_handle_message(ptr);
			}
	#endif
	#if defined(__SDAP_TESTER__)
			else if ((ptr->msg_id >= MSG_ID_BT_SDAP_TESTER_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_SDAP_TESTER_GROUP_END))
			{
				btmtk_adp_sdap_tester_handle_message(ptr);
			}
	#endif
	#if defined(__BT_HDP_PROFILE__)
			else if ((ptr->msg_id >= MSG_ID_BT_HDP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HDP_GROUP_END))
			{
				btmtk_adp_hdp_handle_message(ptr);
			}
	#endif
#if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)
            else if ( (ptr->msg_id >= MSG_ID_BT_HTP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HTP_GROUP_END)) 
            {
                btmtk_adp_htp_handle_message(ptr);
            }
#endif
#if defined(__BT_FMPL_PROFILE__) || defined(__BT_FMPT_PROFILE__)
            else if ( (ptr->msg_id >= MSG_ID_BT_FMP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_FMP_GROUP_END)) 
            {
                btmtk_adp_fmp_handle_message(ptr);
            }
#endif
#if defined(__BT_GATTC_PROFILE__)
            else if((ptr->msg_id >= MSG_ID_BT_GATTC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GATTC_GROUP_END)){
               btmtk_adp_gattc_handle_message(ptr);
            }
#endif
#if defined(__BT_GATTS_PROFILE__)
            else if((ptr->msg_id >= MSG_ID_BT_GATTS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GATTS_GROUP_END)){
               btmtk_adp_gatts_handle_message(ptr);
            }
#endif
            else if (ptr->msg_id == MSG_ID_BT_SSP_DEBUG_MODE_REQ)
            {
                bt_em_handler(ptr);
            }
            else if (ptr->msg_id == MSG_ID_BT_TEST_CMD_REQ)
            {
                bt_em_test_cmd_handler(ptr);
            }            
			else 
			{
				bt_prompt_trace(MOD_BT, "bt_main(): Unknown msg %d", ptr->msg_id);
			}
    }
    //free_ilm(ptr);
    //ptr = 0;

    return KAL_TRUE;
}

