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
 * Bt_adp_system.c
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

#ifndef _BT_SYS_C
#define _BT_SYS_C

#include "assert.h"
#include "bt_common.h"
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#include "win_service.h"
#elif defined(BTMTK_ON_LINUX)
#include <sys/time.h>
#include <errno.h>
#endif
#include "xatypes.h"
#include "osapi.h"
#include "bluetooth_struct.h"
#ifdef BTMTK_ON_WISE 
#include "bt.h"
#endif  /* BTMTK_ON_WISE */
#include "bt_uart_log_drv.h"
#include "bt_profile_init.h"
#include "bt_adp_system.h"
#include "eventmgr.h"
#include "bt_feature.h"
#ifdef ANDROID
#include <hardware_legacy/power.h>
#endif
#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"
#endif

#ifndef __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_drv.h"
#endif

#ifdef ANDROID
#include <cutils/properties.h>
#endif

typedef enum{
    LOAD_TYPE_UNKNOWN = 0,
    LOAD_TYPE_ENG = 1,
    LOAD_TYPE_USER = 2,
}LOAD_TYPE;

typedef struct {
    unsigned char os_init;
    unsigned char during_power_on_state;
    unsigned char during_power_off_state;
    unsigned char on_panic_reset_state;
    unsigned char notify_evm;
    OsTimerNotify func;
#if defined(__ENABLE_SLEEP_MODE__)
#ifdef ANDROID
    unsigned char wake_lock_acquired;
#endif
#endif /* __ENABLE_SLEEP_MODE__ */
#ifdef __BT_TEST_MODE_ENABLED__
    unsigned char bttm;
#endif
    U8 nvrm_addr_exist; /* indicate if addr is exist in NVRM */
    BD_ADDR bd_addr;    /* address in NVRM */
    LOAD_TYPE load_type;
    U32 fwdump;
}bt_adp_system_struct_t;

#ifndef BTMTK_ON_WISE 
bt_adp_system_struct_t BTAdpCoreCtx = {0};
#endif  /* BTMTK_ON_WISE */

extern int commPort;

void BTCoreCancelTimer(void)
{
#ifdef BTMTK_ON_WISE 
    if (bt_p->event_id != 0)
    {
        evshed_cancel_event(bt_p->evsh_sched, &(bt_p->event_id));
        bt_p->event_id = 0;
    }
#else   /* BTMTK_ON_WISE */
    #ifdef BTMTK_ON_LINUX
    {
        struct itimerval timeout;
        /* No periodic */
        timeout.it_interval.tv_sec = 0;
        timeout.it_interval.tv_usec = 0;
        timeout.it_value.tv_sec = 0;
        timeout.it_value.tv_usec = 0;
        //bt_prompt_trace(MOD_BT, "[TM] cancel timer");
        if ( setitimer(ITIMER_REAL, &timeout, NULL) < 0 )
            bt_prompt_trace(MOD_BT, "[TM] cancel failed %s, errno=%d", strerror(errno), errno);
        //else
        //    bt_prompt_trace(MOD_BT, "[TM] cancel timer success");
    }    
    #else
    BTMTK_DisabledTimer(MOD_BT);
    #endif
#endif  /* BTMTK_ON_WISE */
}

void BTCoreStartTimer(TimeT time, OsTimerNotify func)
{
    BTCoreCancelTimer();   
#ifdef BTMTK_ON_WISE 
    bt_p->event_id = evshed_set_event(bt_p->evsh_sched, (kal_timer_func_ptr) func, NULL, time);
#else   /* BTMTK_ON_WISE */
    BTAdpCoreCtx.func = func;
    #ifdef BTMTK_ON_LINUX
    {
        struct itimerval timeout;
        int ms = time*50;
        /* No periodic */
        timeout.it_interval.tv_sec = 0;
        timeout.it_interval.tv_usec = 0;
        timeout.it_value.tv_sec = ms/1000;
        timeout.it_value.tv_usec = (ms%1000)*1000;
        //bt_prompt_trace(MOD_BT, "[TM] set timer(sec=%d, usec=%d)", timeout.it_value.tv_sec, timeout.it_value.tv_usec);
        if ( setitimer(ITIMER_REAL, &timeout, NULL) < 0 )
            bt_prompt_trace(MOD_BT, "[TM] setitimer failed %s, errno=%d", strerror(errno), errno);
    }
    #else
    BTMTK_SetTimer(MOD_BT, time);
    #endif
#endif  /* BTMTK_ON_WISE */
}


TimeT BTCoreGetSystemTick(void)
{
    kal_uint32 sys_time;
    //bt_prompt_trace(MOD_BT, "[BT] +BTCoreGetSystemTick");
#if defined ( BTMTK_ON_WISE )
    kal_get_time(&sys_time);
#elif defined (BTMTK_ON_LINUX)
    #if 1
    struct timespec tp;
    if (0 != clock_gettime(CLOCK_MONOTONIC, &tp)){
        assert(0);
    }
    sys_time = tp.tv_sec*20+tp.tv_nsec/50000000;
    #else
    struct timeval tv;
    if (0 != gettimeofday(&tv, NULL))
    {
        //bt_prompt_trace(MOD_BT, "[BT] gettimeofday failed");
        assert(0);
    }
    //sys_time = ((tv.tv_sec*1000000+tv.tv_usec)/50000);
    sys_time = tv.tv_sec*20+tv.tv_usec/50000;
    #endif
#else   /* BTMTK_ON_WISE */
    sys_time = BTMTK_GetSystemTick();
#endif  /* BTMTK_ON_WISE */
    //bt_prompt_trace(MOD_BT, "[BT] -BTCoreGetSystemTick");
    return (sys_time);
}


TimeT BTCoreGetSystemTick1(void)
{
    kal_uint32 sys_time;
#if defined ( BTMTK_ON_WISE )
    kal_get_time(&sys_time);
#elif defined (BTMTK_ON_LINUX)
    struct timeval tv;
    if (0 != gettimeofday(&tv, NULL))
    {
        assert(0);
    }
    sys_time = ((tv.tv_sec*1000000+tv.tv_usec));
#else   /* BTMTK_ON_WISE */
    sys_time = BTMTK_GetSystemTick();
#endif  /* BTMTK_ON_WISE */
    return (sys_time);
}

struct timeval tv1;
U32 total_delta0 = 0;
U32 total_delta_count0 = 0;

void BTCoreGetSystemTick2(void)
{
    kal_uint32 sys_time;
    kal_uint32 sys_time1;
    struct timeval tv;	
    if (0 != gettimeofday(&tv, NULL))
    {
        assert(0);
    }
    sys_time = ((tv.tv_sec*1000000+tv.tv_usec));
    sys_time1 = ((tv1.tv_sec*1000000+tv1.tv_usec));    
    
//    Report(("delta:%d", sys_time - sys_time1));
    tv1 = tv;
    total_delta_count0++;
    total_delta0 += (sys_time - sys_time1);
//    Report(("daylong avg:%d", total_delta0/total_delta_count0));

}

U32 total_delta = 0;
U32 total_delta_count = 0;
struct timeval tv2;

void BTCoreGetSystemTick3_Reset(void){
	total_delta = 0;
	total_delta_count = 0;
	Report(("BTCoreGetSystemTick3_Reset 10 reset"));
}


void BTCoreGetSystemTick3(void)
{
    kal_uint32 sys_time;
    kal_uint32 sys_time1;
    struct timeval tv;
    if (0 != gettimeofday(&tv, NULL))
    {
        assert(0);
    }
    
//   Report(("daylong 11 sec:%d, usec:%d", tv.tv_sec, tv.tv_usec));
    tv2 = tv;

}  

void BTCoreGetSystemTick4(void)
{
    kal_uint32 sys_time;
    kal_uint32 sys_time1;
    struct timeval tv;
    if (0 != gettimeofday(&tv, NULL))
    {
        assert(0);
    }
    sys_time = ((tv.tv_sec*1000000+tv.tv_usec));
    sys_time1 = ((tv2.tv_sec*1000000+tv2.tv_usec));    
//    Report(("daylong 12 sec:%d, usec:%d", tv.tv_sec, tv.tv_usec));    
    Report(("daylong :%d", sys_time - sys_time1));
    total_delta_count++;
    total_delta += (sys_time - sys_time1);
    Report(("daylong avg:%d", total_delta/total_delta_count));
    tv2 = tv;
}

U32 total_delta1 = 0;
U32 total_delta_count1 = 0;
struct timeval tv3;

void BTCoreGetSystemTick5(void)
{
    kal_uint32 sys_time;
    kal_uint32 sys_time1;
    struct timeval tv;
    if (0 != gettimeofday(&tv, NULL))
    {
        assert(0);
    }
    tv3 = tv;

}  


void BTCoreSetSysInitState(U8 state)
{
    if(state == 1)
    {
    #ifdef BTMTK_ON_WISE 
        bt_p->os_init = TRUE;
    #else   /* BTMTK_ON_WISE */
        BTAdpCoreCtx.os_init = TRUE;
    #endif  /* BTMTK_ON_WISE */        
    }
    else
    {
    #ifdef BTMTK_ON_WISE 
        bt_p->os_init = FALSE;
    #else   /* BTMTK_ON_WISE */
        BTAdpCoreCtx.os_init = FALSE;
    #endif  /* BTMTK_ON_WISE */        
    }
}

void BTCoreGetSystemTick6(void)
{
    kal_uint32 sys_time;
    kal_uint32 sys_time1;
    struct timeval tv;
    if (0 != gettimeofday(&tv, NULL))
    {
        assert(0);
    }
    sys_time = ((tv.tv_sec*1000000+tv.tv_usec));
    sys_time1 = ((tv3.tv_sec*1000000+tv3.tv_usec));    
//    Report(("daylong 2:%d", sys_time - sys_time1));

    total_delta_count1++;
    total_delta1 += (sys_time - sys_time1);
//    Report(("daylong 2 avg:%d", total_delta1/total_delta_count1));
    tv3 = tv;
}



BOOL BTCoreVerifySysInitState(void)
{
#ifdef BTMTK_ON_WISE 
    return bt_p->os_init;
#else   /* BTMTK_ON_WISE */
    return BTAdpCoreCtx.os_init;
#endif  /* BTMTK_ON_WISE */
}

#ifndef BTMTK_ON_WISE 
BOOL BTCoreVerifyNotifyEvm(void)
{
    //bt_prompt_trace(MOD_BT,"+BTCoreVerifyNotifyEvm : os_init=%d, notify_evm=%d", BTAdpCoreCtx.os_init, BTAdpCoreCtx.notify_evm);
    if(BTAdpCoreCtx.os_init == TRUE)
    {
        if(BTAdpCoreCtx.notify_evm == 0)
        {
            BTAdpCoreCtx.notify_evm = 1;
            //bt_prompt_trace(MOD_BT,"-BTCoreVerifyNotifyEvm : return TRUE");
            return TRUE;
        }
    }
    //bt_prompt_trace(MOD_BT,"-BTCoreVerifyNotifyEvm : return FALSE");
    return FALSE;
}
#endif  /* BTMTK_ON_WISE */

BOOL BTCoreIsInitializing(void)
{
#ifdef BTMTK_ON_WISE 
    return bt_p->is_poweron;    
#else   /* BTMTK_ON_WISE */
    return BTAdpCoreCtx.during_power_on_state;
#endif  /* BTMTK_ON_WISE */    
}
BOOL BTCoreIsDeinitializing(void)
{
#ifdef BTMTK_ON_WISE 
    return bt_p->entering_poweroff;    
#else   /* BTMTK_ON_WISE */
    return BTAdpCoreCtx.during_power_off_state;
#endif  /* BTMTK_ON_WISE */    
}

BOOL BTCoreReadyToPanic(void)
{
#ifdef BTMTK_ON_WISE 
    if ((bt_p->is_poweron == FALSE) && (bt_p->is_reset == FALSE))
#else   /* BTMTK_ON_WISE */
    if ((BTAdpCoreCtx.during_power_on_state == 0) && (BTAdpCoreCtx.on_panic_reset_state == FALSE))
#endif  /* BTMTK_ON_WISE */    
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

void BTCoreHandlePanic(void)
{
#ifdef BTMTK_ON_WISE 
    bt_send_msg(MSG_ID_BT_RESET_REQ_IND, MOD_MMI, BT_APP_SAP, NULL, NULL);
#ifdef __GEMINI__
    {
        module_type dst_mod = Get_DualSim_Uart_Default_Owner();

        bt_send_msg(MSG_ID_BT_RESET_REQ_IND, dst_mod, BT_APP_SAP, NULL, NULL);
    }
#else /* __GEMINI__ */ 
    bt_send_msg(MSG_ID_BT_RESET_REQ_IND, MOD_ATCI, BT_APP_SAP, NULL, NULL);
#endif /* __GEMINI__ */ 
    bt_p->is_reset = TRUE;
    bt_p->power_status = 0;
#elif defined (BTMTK_ON_LINUX)
    BT_SendMessage(MSG_ID_BT_RESET_REQ_IND, MOD_MMI, NULL, 0);
    BTAdpCoreCtx.on_panic_reset_state = 1;
#else   /* BTMTK_ON_WISE */    
    BTAdpCoreCtx.on_panic_reset_state = 1;
#endif  /* BTMTK_ON_WISE */
}



/*****************************************************************************
 * FUNCTION
 *  BTCoreIsPanic
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
kal_bool BTCoreIsPanic(void)
{
#ifdef BTMTK_ON_WISE 
    return bt_p->is_reset;
#else   /* BTMTK_ON_WISE */
    return BTAdpCoreCtx.on_panic_reset_state;
#endif  /* BTMTK_ON_WISE */    
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_adp_system_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_system_init(void)
{
    const char *fwdump = "fwdump";
    /* get if the load is eng load */
    BTCoreIsEngLoad();
    if( !OS_GetSystemPropertyUint(fwdump, &BTAdpCoreCtx.fwdump, 1) ){
        if(BTCoreIsEngLoad() || BTCoreIsInternalLoad()){
            BTAdpCoreCtx.fwdump = 0xFFFFFFFF;
        }else{
            BTAdpCoreCtx.fwdump = 0;
        }
        OS_SetSystemPropertyUint(fwdump, BTAdpCoreCtx.fwdump, 1);
    }
    OS_Report("BTAdpCoreCtx.fwdump == %u", BTAdpCoreCtx.fwdump);

    #ifdef __LINUX_SUPPRESS_ERROR__
    #else
    UART_Open(0, MOD_BT);  /* Logging */
    #endif
    btmtk_adp_check_msg_id_range();
}

/*****************************************************************************
 * FUNCTION
 *  BtSysAdpVerifyInPanicState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL BtSysAdpVerifyInPanicState(void)
{
    BOOL result;
    result = BTCoreIsPanic();
    if(result == KAL_TRUE)
        return TRUE;
    else 
        return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_adp_system_power_on_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_system_power_on_req(void)
{
#ifdef BTMTK_ON_WISE 
#ifdef __GEMINI__
    bt_poweron_req_struct *poweron_req = (bt_poweron_req_struct*) ilm_ptr->local_para_ptr;
#endif 
    if (bt_p->os_init == TRUE)
    {                
        bt_poweron_cnf_struct *msg;
        bt_trace(TRACE_GROUP_1, BTLOG_BT_ALREADY_POWERED_ON);
        /*
         * BT is already powered on.
         * We should not reset the state by calling BTSysAdpPowerOnCnf(FALSE)                
         * We just return the poweron confirm failed
         */
        
        msg = (bt_poweron_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_poweron_cnf_struct), TD_CTRL);
        msg->result = KAL_TRUE;
        bt_send_msg(MSG_ID_BT_POWERON_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
        return;
    }
    bt_p->is_poweron = TRUE;
    BTAdpCoreCtx.notify_evm =0;

    if (bt_p->bt_rftest_buffer != NULL)
    {
        free_ctrl_buffer(bt_p->bt_rftest_buffer);
        bt_p->bt_rftest_buffer = NULL;
    }
    bt_p->entering_poweroff = FALSE;
    if (OS_Init() == FALSE)
    {
        ASSERT(0);
    }
#ifdef __GEMINI__
    if (poweron_req->dual_sim_uart_setting == 0)
    {
        Set_DualSim_Uart_Setting(0);
    }
    else if (poweron_req->dual_sim_uart_setting == 1)
    {
        Set_DualSim_Uart_Setting(1);
    }
    else
    {
        ASSERT(0);
    }
#endif /* __GEMINI__ */ 
    /* Adaptor and profile init */
    bt_profiles_init();
    if (bt_p->is_factory_mode)
    {
        HciFactoryMode();
    }
    bt_adp_profiles_init();   
#else   /* BTMTK_ON_WISE */
    if(BTAdpCoreCtx.during_power_on_state == TRUE)
    {
        assert(0);
    }
    if(BTAdpCoreCtx.os_init == TRUE)
    {
#ifdef BTMTK_ON_LINUX
         void BTCorePowerOnCnf(kal_bool result);
         bt_prompt_trace(MOD_BT, "BT already turn on before");
         BTCorePowerOnCnf(KAL_TRUE);
         return;
#endif
    }
    BTAdpCoreCtx.notify_evm = 0;
    BTAdpCoreCtx.during_power_on_state = TRUE;
    if (OS_Init() == FALSE)
    {
#ifdef BTMTK_ON_LINUX
         void BTCorePowerOnCnf(kal_bool result);
         bt_prompt_trace(MOD_BT, "OS_Init failed");
         BTCorePowerOnCnf(KAL_FALSE);
         return;
#else
         assert(0);
#endif
    }
    /* Adaptor and profile init */
    bt_profiles_init();
    bt_adp_profiles_init();   

#if defined(__BTMODULE_MT6611__)
    bt_prompt_trace(MOD_BT, "__BTMODULE_MT6611__ power on");
#elif defined(__BTMODULE_MT6612__)
    bt_prompt_trace(MOD_BT, "__BTMODULE_MT6612__ power on");
#elif defined(__BTMODULE_MT6616__)
    bt_prompt_trace(MOD_BT, "__BTMODULE_MT6616__ power on");
#elif defined(__BTMODULE_MT6620__)
    bt_prompt_trace(MOD_BT, "__BTMODULE_MT6620__ power on");
#elif defined(__BT_BTWIFI_COMBO_CHIP__)
    bt_prompt_trace(MOD_BT, "__BT_BTWIFI_COMBO_CHIP__ power on");

#endif /* __BTMODULE_MTXXXX__ */    
#endif  /* BTMTK_ON_WISE */   
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_adp_system_power_off_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_system_power_off_req(void)
{
#ifdef __ANDROID_EMULATOR__
    ME_SetAccessibleModeNC(0);
    BTCorePowerOffCnf(KAL_TRUE);
    return;
#endif
#ifdef BTMTK_ON_WISE 
    bt_p->entering_poweroff = TRUE;
    OS_Deinit();
    bt_p->power_status = 0;

    /* Adaptor and profile deinit */
    bt_adp_profiles_deinit();
#ifdef __WIFI_SUPPORT__
    wndrv_PTA_isOFF();
#endif 
#else   /* BTMTK_ON_WISE */
    if(!BTCoreVerifySysInitState()){
        BTCorePowerOffCnf(KAL_TRUE);
    }else if(BTAdpCoreCtx.during_power_off_state == TRUE){
        bt_prompt_trace(MOD_BT, "[SH] power off is on going");
    }else{
    BTAdpCoreCtx.during_power_off_state = TRUE;
    OS_Deinit();
    bt_adp_profiles_deinit();
    bt_profiles_deinit();
    }
#endif  /* BTMTK_ON_WISE */   
}

#ifndef BTMTK_ON_WISE 
void btmtk_adp_system_main_loop(void)
{
    if(BTAdpCoreCtx.os_init == TRUE)
    {
        BTAdpCoreCtx.notify_evm =0;
        EVM_Process();
    }
}
#endif  /* BTMTK_ON_WISE */

#ifdef BTMTK_ON_WISE 
/*****************************************************************************
 * FUNCTION
 *  BTPowerOff
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTPowerOff(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_POWEROFF_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_BT;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);
}
#endif  /* BTMTK_ON_WISE */


/*****************************************************************************
 * FUNCTION
 *  BTCorePowerOnCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#if defined(__SPP_THROUGHPUT_TEST__)
extern int g_bSppServer;
#endif
void BTCorePowerOnCnf(kal_bool result)
{
#ifdef __BT_SINGLE_ANTENNA__
    if(result == TRUE)
    {
        BT_WMT_Stack_Send_Status(1);
    }
#endif

#ifdef BTMTK_ON_WISE 
    // set the value only in the success case
    if (result == KAL_TRUE)
    {
        bt_p->power_status = 1;
        BTPowerOnSetUartRxSpeed();
    }
    if (bt_p->is_engineering_mode == 1)
    {
        bt_engineer_mode_poweron_cnf_struct *msg;

        msg =
            (bt_engineer_mode_poweron_cnf_struct*) construct_local_para(
                                                    (kal_uint16) sizeof(bt_engineer_mode_poweron_cnf_struct),
                                                    TD_CTRL);
        msg->result = result;
        bt_send_msg(MSG_ID_BT_ENGINEER_MODE_POWERON_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
        bt_p->is_poweron = FALSE;
        if (result == KAL_TRUE)
        {
            bt_send_msg(MSG_ID_BT_READY_IND, MOD_MMI, BT_APP_SAP, NULL, NULL);
        }
        return;
    }
    if (bt_p->bt_test_mode == 1)
    {
        // If power on failed, we reset bt_test_mode but not enter test mode.
        if (result == KAL_TRUE) 
        {            
            BtRadio_EnterTestMode();
        }
        bt_p->bt_test_mode = 0;
    }
    if (bt_p->bt_read_addr == 1)
    {
        if (result == KAL_TRUE)
        {
            bt_get_bd_addr_req_hdlr(TRUE);
        }
        else
        {
            bt_get_bd_addr_req_hdlr(FALSE);
        }
        bt_p->bt_read_addr = 0;        
    }
    if (bt_p->is_poweron == TRUE)
    {
        bt_poweron_cnf_struct *msg;

        msg = (bt_poweron_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_poweron_cnf_struct), TD_CTRL);

        msg->result = result;

        bt_send_msg(MSG_ID_BT_POWERON_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
        bt_p->is_poweron = FALSE;

        if (result == KAL_TRUE)
        {
            bt_send_msg(MSG_ID_BT_READY_IND, MOD_MMI, BT_APP_SAP, NULL, NULL);
        }
    }
    else if (bt_p->is_reset == TRUE)
    {
        bt_mmi_reset_cnf_struct *msg;

        msg = (bt_mmi_reset_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_mmi_reset_cnf_struct), TD_CTRL);

        msg->result = result;

        bt_send_msg(MSG_ID_BT_MMI_RESET_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
        bt_send_msg(MSG_ID_BT_L4C_RESET_CNF, MOD_MMI, BT_APP_SAP, NULL, NULL);
        bt_p->is_reset = FALSE;

    }
    else
    {
        ASSERT(0);

    }
#else   /* BTMTK_ON_WISE */
    bt_poweron_cnf_struct *msg = (bt_poweron_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_poweron_cnf_struct), TD_CTRL);
    
    BTAdpCoreCtx.during_power_on_state = FALSE;
    BTAdpCoreCtx.on_panic_reset_state = FALSE;
    msg->result = result;
    BT_SendMessage(MSG_ID_BT_POWERON_CNF, MOD_EXTERNAL, msg, sizeof(bt_poweron_cnf_struct));
#endif  /* BTMTK_ON_WISE */    
#if defined(__SPP_THROUGHPUT_TEST__)
	spp_throughput_test_start(g_bSppServer);
#endif    
}


/*****************************************************************************
 * FUNCTION
 *  BTCorePowerOffCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTCorePowerOffCnf(kal_bool result)
{
#ifdef __BT_SINGLE_ANTENNA__
    if(result == TRUE)
    {
        BT_WMT_Stack_Send_Status(0);
    }
#endif

#ifdef BTMTK_ON_WISE 
    bt_poweroff_cnf_struct *msg;

    /*
     * In BT_PowerOffToJsr82():
     * * (1) BTJSR82_DeInitAll(): Reset JSR82 session context and APP SDPDB context kept in BTstack
     * * (2) Notify JBT 
     */
#ifdef MTK_UPF_TESTING
    UPF_SETADDR = 0;
#endif 
    BT_PowerOffToJsr82();
    if (bt_p->is_reset == FALSE)
    {
        msg = (bt_poweroff_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_poweroff_cnf_struct), TD_CTRL);
        msg->result = result;
        bt_send_msg(MSG_ID_BT_POWEROFF_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
    }
    else
    {
        bt_mmi_reset_cnf_struct *msg;

        msg = (bt_mmi_reset_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_mmi_reset_cnf_struct), TD_CTRL);

        msg->result = result;

        bt_send_msg(MSG_ID_BT_MMI_RESET_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
        bt_send_msg(MSG_ID_BT_L4C_RESET_CNF, MOD_MMI, BT_APP_SAP, NULL, NULL);
        bt_p->is_reset = FALSE;

    }
#else   /* BTMTK_ON_WISE */   
    bt_poweroff_cnf_struct *msg = (bt_poweroff_cnf_struct*) construct_local_para((kal_uint16) sizeof(bt_poweroff_cnf_struct), TD_CTRL);

    BTAdpCoreCtx.during_power_off_state = FALSE;
    msg->result = result;
#ifdef __BT_TEST_MODE_ENABLED__
    BTAdpCoreCtx.bttm = 0;
#endif
    BT_SendMessage(MSG_ID_BT_POWEROFF_CNF, MOD_EXTERNAL, msg, sizeof(bt_poweroff_cnf_struct));
#endif  /* BTMTK_ON_WISE */

}


void BTCoreEnabledSleepMode(U8 state)
{
#if defined(__ENABLE_SLEEP_MODE__)
#ifdef BTMTK_ON_WISE 
    if(state == 1)
    {
        L1SM_SleepEnable(bt_p->l1_sleep_handle);

    }
    else
    {
        L1SM_SleepDisable(bt_p->l1_sleep_handle);
    }
#elif BTMTK_ON_LINUX
    if(state == 1)
    {
        #ifdef ANDROID
        if(BTAdpCoreCtx.wake_lock_acquired){
            bt_prompt_trace(MOD_BT, "[PWR] release wake lock");
            release_wake_lock("mtkbt");
            BTAdpCoreCtx.wake_lock_acquired = 0;
        }
        #endif
    }else {
        #ifdef ANDROID
        if(!BTAdpCoreCtx.wake_lock_acquired){
            bt_prompt_trace(MOD_BT, "[PWR] acquire wake lock");
            acquire_wake_lock(PARTIAL_WAKE_LOCK, "mtkbt");
            BTAdpCoreCtx.wake_lock_acquired = 1;
        }
        #endif
    }
#endif  /* BTMTK_ON_WISE */    
#endif /* __ENABLE_SLEEP_MODE__ */
}


/*****************************************************************************
 * FUNCTION
 *  BTCoreHostWakeUpInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTCoreHostWakeUpInd(void)
{
#ifdef BTMTK_ON_WISE 
    if (!bt_p->is_factory_mode)
    {
        ilm_struct *ilmPtr;
        ilmPtr = allocate_ilm(MOD_EINT_HISR);
        ilmPtr->msg_id = MSG_ID_BT_HOST_WAKE_UP_IND;
        ilmPtr->local_para_ptr = NULL;
        ilmPtr->peer_buff_ptr = NULL;
        ilmPtr->dest_mod_id = MOD_BT;
        ilmPtr->src_mod_id = MOD_EINT_HISR;
        ilmPtr->sap_id = BT_APP_SAP;
        msg_send_ext_queue(ilmPtr);
    }    
#endif  /* BTMTK_ON_WISE */    
}


/*****************************************************************************
 * FUNCTION
 *  BTCoreHostSleep
 * DESCRIPTION
 *  
 * PARAMETERS
 *  sleep       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTCoreHostSleep(kal_uint8 sleep)
{
#if defined(__ENABLE_SLEEP_MODE__)
#if defined(BTMTK_ON_WISE)
    if (sleep == TRUE)
    {
        Radio_hostSleep();
        L1SM_SleepEnable(bt_p->l1_sleep_handle);
    }
    else
    {
        L1SM_SleepDisable(bt_p->l1_sleep_handle);
    }
#elif defined(BTMTK_ON_LINUX)
    if (sleep == TRUE)
    {
        /* unmask BT EINT */
        Radio_hostSleep();
#ifdef ANDROID
        if(BTAdpCoreCtx.wake_lock_acquired){
            bt_prompt_trace(MOD_BT, "[PWR] release wake lock");
            release_wake_lock("mtkbt");
            BTAdpCoreCtx.wake_lock_acquired = 0;
        }
#endif
    }
    else
    {
#ifdef ANDROID
        if(!BTAdpCoreCtx.wake_lock_acquired){
            bt_prompt_trace(MOD_BT, "[PWR] acquire wake lock");
            acquire_wake_lock(PARTIAL_WAKE_LOCK, "mtkbt");
            BTAdpCoreCtx.wake_lock_acquired = 1;
        }
#endif
    }
#endif  /* BTMTK_ON_WISE */
#endif /* __ENABLE_SLEEP_MODE__ */
}

/*****************************************************************************
 * FUNCTION
 *  BTCoreCheckNotTestMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL BTCoreCheckNotTestMode(void)
{
#ifdef BTMTK_ON_WISE 
    if (bt_p->bt_test_mode == 1)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
#else   /* BTMTK_ON_WISE */
    /* Always not in test mode */
    return TRUE;
#endif  /* BTMTK_ON_WISE */
}

U8 BTCoreGetNVRMBDAddr(BD_ADDR *addr){
    OS_Report("BTCoreSetNVRMBDAddr (%p), nvrm_addr_exist=%u", addr, BTAdpCoreCtx.nvrm_addr_exist);
    if(addr && BTAdpCoreCtx.nvrm_addr_exist){
        OS_MemCopy(addr->addr, BTAdpCoreCtx.bd_addr.addr, 6);
        return 1;
    }
    return 0;
}

U8 BTCoreSetNVRMBDAddr(U8 *addr){
    OS_Report("BTCoreSetNVRMBDAddr (%p)", addr);
    if(addr){
        BTAdpCoreCtx.nvrm_addr_exist = 1;
        BTAdpCoreCtx.bd_addr.addr[0] = addr[5];
        BTAdpCoreCtx.bd_addr.addr[1] = addr[4];
        BTAdpCoreCtx.bd_addr.addr[2] = addr[3];
        BTAdpCoreCtx.bd_addr.addr[3] = addr[2];
        BTAdpCoreCtx.bd_addr.addr[4] = addr[1];
        BTAdpCoreCtx.bd_addr.addr[5] = addr[0];
        return 1;
    }
    return 0;
}

U8 BTCoreCheckNVRMBDAddr(){
#ifdef __MTK_BT_EXTERNAL_PLATFORM__
    return 1;  /* TODO */
#else
    int ret = 0;
    BT_REQ req = {BT_COLD_OP_GET_ADDR, {0}};
    BT_RESULT result;
    if(!BTAdpCoreCtx.nvrm_addr_exist){
        /* check address in NVRM */
        mtk_bt_op(req, &result);
        if( result.status == 0 || result.param.addr[0] == 0 ){
            OS_Report("No nvrm address exist. enable / disable driver to make address wrote to NVRM");
            ret = mtk_bt_enable(0, NULL);
            if(ret >= 0){
                mtk_bt_disable(ret);
                mtk_bt_op(req, &result);
                if( result.status == 0 || result.param.addr[0] == 0 ){
                    OS_Report("mtk_bt_op return status %u, isdefault addr=%u", result.status, result.param.addr[0]);
                }else{
                    return BTCoreSetNVRMBDAddr(&result.param.addr[1]);
                }
            }else{
                OS_Report("mtk_bt_enable failed");
            }
        }else{
            return BTCoreSetNVRMBDAddr(&result.param.addr[1]);
        }        
    }else{
        return 1;
    }
    return 0;
#endif
}


/*****************************************************************************
 * FUNCTION
 *  BTSysAdpRFTestResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTCoreRFTestResult(U8 result)
{
#ifdef BTMTK_ON_WISE 
    if (bt_p->is_engineering_mode == 1)
    {
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, EM_OPERATIONBT_DEBUGx02XBT_DEBUGx02X, bt_p->bt_em_operation, result);
    #endif 
        if (result == 0)
        {
            /* Success */
            switch (bt_p->bt_em_operation)
            {
                case BT_RFTEST_OP_ENABLED_WHITEN:
                    Assert(bt_p->bt_rftest_buffer != 0);
            #ifdef MTK_BT_DEBUG_TRACE
                    kal_trace(TRACE_GROUP_1, BT_RFTEST_OP_ENABLED_WHITEN_COMPLETED);
            #endif 
                    bt_p->bt_em_state |= BT_RFTEST_WHITEN_ENABLED;
                    bt_p->bt_em_operation = BT_RFTEST_OP_TXRX_TEST;

                    /* 
                     * Example:  
                     * BtRadio_EMEnabledTxRxTest(0x01,0x00,0x00,0x00,0x02,0x04,27);
                     */

                    Report(("%02x", ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->pattern));
                    Report(("%02x",
                            ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->channel_hopping));
                    Report(("%02x", ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->tx_freq));
                    Report(("%02x", ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->rx_freq));
                    Report(("%02x", ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->poll_period));
                    Report(("%02x", ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->packet_type));
                    Report(("%04x",
                            ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->packet_length));

                    BtRadio_EMEnabledTxRxTest(
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->pattern,
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->channel_hopping,
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->tx_freq,
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->rx_freq,
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->poll_period,
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->packet_type,
                        ((bt_engineer_mode_txrx_test_req_struct*) (bt_p->bt_rftest_buffer))->packet_length);

                    if (bt_p->bt_rftest_buffer != NULL)
                    {
                        free_ctrl_buffer(bt_p->bt_rftest_buffer);
                    }
                    bt_p->bt_rftest_buffer = NULL;
                    break;
                case BT_RFTEST_OP_TXRX_TEST:
                #ifdef MTK_BT_DEBUG_TRACE
                    kal_trace(TRACE_GROUP_1, BT_RFTEST_OP_TXRX_TEST_TRC);
                #endif 
                    bt_p->bt_em_state |= BT_RFTEST_TXRX_ENABLED;
                    bt_p->bt_em_operation = BT_RFTEST_OP_NONE;
                    bt_engineer_mode_cnf_handler(MSG_ID_BT_ENGINEER_MODE_TXRX_TEST_CNF, KAL_TRUE);
                    break;
                case BT_RFTEST_OP_POWER_TEST:
                #ifdef MTK_BT_DEBUG_TRACE
                    kal_trace(TRACE_GROUP_1, BT_RFTEST_OP_POWER_TEST_TRC);
                #endif 
                    bt_p->bt_em_operation = BT_RFTEST_OP_NONE;
                    bt_engineer_mode_cnf_handler(MSG_ID_BT_ENGINEER_MODE_POWER_TEST_CNF, KAL_TRUE);
                    break;
            }
        }
        else
        {
            /* Failed */
            switch (bt_p->bt_em_operation)
            {
                case BT_RFTEST_OP_ENABLED_WHITEN:
                    if (bt_p->bt_rftest_buffer != NULL)
                    {
                        free_ctrl_buffer(bt_p->bt_rftest_buffer);
                    }
                    bt_p->bt_rftest_buffer = NULL;
                case BT_RFTEST_OP_TXRX_TEST:
                    bt_engineer_mode_cnf_handler(MSG_ID_BT_ENGINEER_MODE_TXRX_TEST_CNF, KAL_FALSE);
                    break;
                case BT_RFTEST_OP_POWER_TEST:
                    bt_engineer_mode_cnf_handler(MSG_ID_BT_ENGINEER_MODE_POWER_TEST_CNF, KAL_FALSE);
                    break;
            }
            bt_p->bt_em_operation = BT_RFTEST_OP_NONE;
        }
    }
#endif  /* BTMTK_ON_WISE */
}

#ifdef BTMTK_ON_WISE 
/*****************************************************************************
 * FUNCTION
 *  BTSysAdpHostWakeUpIndbyHISR
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTSysAdpHostWakeUpIndbyHISR(void)
{
    BTCoreHostWakeUpInd();
    /* 
        Send a message to task and do the following procedure
        HCI_TransportSleep(0);
        Radio_hostWakeup();
        BTSysAdpHostSleep(0);
    */
}
#endif  /* BTMTK_ON_WISE */


#ifndef BTMTK_ON_WISE 
/*
void Radio_hostWakeup(void)
{
}
*/
void btmtk_adp_system_timer_handler(void)
{
    if(BTAdpCoreCtx.func !=0)
        BTAdpCoreCtx.func();
}
#endif  /* BTMTK_ON_WISE */

#ifdef __BT_TEST_MODE_ENABLED__
/* If just query tm status, do not set BTAdpCoreCtx->bttm */
void testModeCnf(kal_bool result, kal_bool query){
    bt_test_mode_cnf_struct *cnf_p = NULL;
    cnf_p = construct_local_para(sizeof(bt_test_mode_cnf_struct), TD_UL);
    if(query){
        cnf_p->result = BTAdpCoreCtx.bttm ? KAL_TRUE : KAL_FALSE;
    }else{
        cnf_p->result = result;
        BTAdpCoreCtx.bttm = result ? 1 : 0;
    }
    BT_SendMessage(MSG_ID_BT_TEST_MODE_CNF, MOD_MMI, cnf_p, sizeof(bt_test_mode_cnf_struct));
}
#endif /* __BT_TEST_MODE_ENABLED__ */

#ifdef ANDROID
/* 0 : not, others : yes */
U8 BTCoreIsEngLoad(void){
    if( BTAdpCoreCtx.load_type == LOAD_TYPE_UNKNOWN){
        char type[PATH_MAX] = {0};
        int typelen = property_get("ro.build.type", type, NULL);
        /* Set to default USER */
        BTAdpCoreCtx.load_type = LOAD_TYPE_USER;
        if(typelen > 0){
            OS_Report("ro.build.type == %s", type);
            if( !OS_StrCmp(type, "eng") ){
                /* It is eng load */
                BTAdpCoreCtx.load_type = LOAD_TYPE_ENG;
            }
        }else{
            OS_Report("ro.build.type not found");
        }
    }
    OS_Report("BTCoreIsEngLoad : type == %d", BTAdpCoreCtx.load_type);
    return (BTAdpCoreCtx.load_type == LOAD_TYPE_ENG) ? 1 : 0;
}
/* 0 : not internal load, others : yes */
U8 BTCoreIsInternalLoad(void){
    #ifdef __INTERNAL_LOAD__
    return 1;
    #else
    return 0;
    #endif
}
#endif

U32 BTCoreGetFWDump(void){
    OS_Report("BTCoreGetFWDump : %u", BTAdpCoreCtx.fwdump);
    return BTAdpCoreCtx.fwdump;
}

/* 0 : failed, others : success */
U32 BTCoreFWDump(kal_uint32 panic){
#ifdef __MTK_BT_EXTERNAL_PLATFORM__
    return 1;
#else
    BT_REQ req = {BT_HOT_OP_SET_FWASSERT, {0}};
    BT_RESULT result;

    if(commPort < 0){
        OS_Report("BTCoreFWDump failed due to commPort < 0");
        return 0;
    }
    req.param.assert.fd = commPort;
    req.param.assert.reason = (int)panic;
    mtk_bt_op(req, &result);
    OS_Report("BTCoreFWDump : status=%u", result.status);
    return result.status;
#endif    
}

#endif
