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

/***************************************************************************
 *
 * File:
 *     $Workfile:eventmgr.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains code for the protocol stack event manager.
 *
 * Created:
 *     July 20, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "bt_common.h"
#include "eventmgr.h"
#include "utils.h"

#if (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED)

#if BT_STACK == XA_ENABLED
#include "radiomgr.h"
#include "sys/mexp.h"
#endif /* BT_STACK == XA_ENABLED */ 

#include "btalloc.h"
#include "attdbmain.h"
#include "gatt.h"

#ifdef __BT_4_0_BLE__
extern BtStatus BT_ATT_Init(void);
#endif

/* Creates "extern BOOL MODULE_Init();" prototypes from XA_LOAD_LIST. */
#define XA_MODULE(a) extern BOOL a##_Init(void);
XA_LOAD_LIST
#undef XA_MODULE
    /* Creates "MODULE_Init() &&" conditional from XA_LOAD_LIST (used in EVM_Init()). */
#define XA_MODULE(a) a##_Init() &&
/***************************************************************************
 *
 * Local functions
 *
 ***************************************************************************/
    /* In non-multitasking systems, the OS timer is not necessary */
static void ResetOsTimer(TimeT curTime);

#if 0
#define OS_StopHardware()       (void)0
#define OS_ResumeHardware()     (void)0
#endif

/***************************************************************************
 *
 * Local variables
 *
 ***************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  EVM_Init
 * DESCRIPTION
 *  Initialize the Event Manager.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL EVM_Init(void)
{
#if XA_DEBUG == XA_ENABLED
    /* This is a debug-only test to verify that macros are compiled correctly. */
    VerifyMacros();
#endif /* XA_DEBUG == XA_ENABLED */

    /* Intialize the Bluetooth memory */
    if (!BtInitMemory())
    {
        return FALSE;
    }

    /* Initialize the timer list */
    InitializeListHead(&BTC(timerList));

    /* Initialize the Radio Manager */
    if (RMGR_RadioInit() == BT_STATUS_FAILED)
    {
        return FALSE;
    }

    /* Initialize the Bluetooth stack */
    if (ME_Init() == BT_STATUS_FAILED)
    {
        return FALSE;
    }
#ifdef __BT_4_0_BLE__
    BT_ATT_Init();
    GATT_Init();

    ATTDB_Init();
#endif

    /* XA_LOAD_LIST evaluates to series of module specific Init() function calls. */
    if ((XA_LOAD_LIST TRUE) != TRUE)
    {
        return FALSE;
    }

    /* ... other initialization here ... */

    return TRUE;

}


/*****************************************************************************
 * FUNCTION
 *  EVM_Deinit
 * DESCRIPTION
 *  Deinitialize the Event Manager.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void EVM_Deinit(void)
{
    OS_LockStack();

    /* Deinit the radio manager */
    /* RMGR_RadioDeinit(); */
    RMGR_RadioShutdown();

    /* Deinitialize the stack */
    ME_Deinit();

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  EvmResetTimer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [?]
 * RETURNS
 *  void
 *****************************************************************************/
void EvmResetTimer(EvmTimer *timer)
{
    if (timer->func != 0)
    {
        EVM_CancelTimer(timer);
    }
    timer->func = 0;
}


/*****************************************************************************
 * FUNCTION
 *  EvmCancelTimer
 * DESCRIPTION
 *  Cancel an Event Manager timer.
 * PARAMETERS
 *  timer               [IN]
 *  stopHardware        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void EvmCancelTimer(EvmTimer *timer, BOOL stopHardware)
{
    EvmTimer *curTimer;
    ListEntry *node;

    if (stopHardware)
    {
        OS_StopHardware();
    }

    /* Look for the timer in our active timer list and remove it. */
    if (!IsListEmpty(&BTC(timerList)))
    {

        node = GetHeadList(&BTC(timerList));
        while (node != &BTC(timerList))
        {
            curTimer = (EvmTimer*) node;
            node = node->Flink;

            if (curTimer == timer)
            {
                RemoveEntryList(&curTimer->node);

                /* Recalculates the correct OS timer period */
                ResetOsTimer(OS_GetSystemTime());
                break;
            }
        }
    }

    /*
     * If the timer was not found in our active list, simply assume it was
     * already cancelled.
     */
    if (stopHardware)
    {
        OS_ResumeHardware();
    }
}

#if XA_MULTITASKING == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  TimerFired
 * DESCRIPTION
 *  Called by the OS layer to indicate that the system timer fired.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void TimerFired(void)
{
    /* The EVM_Process thread looks for specific expired timers */
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTTIMEOUT_TIMERFIRED);
    OS_ResetTimerId();
    OS_NotifyEvm();
}


/*****************************************************************************
 * FUNCTION
 *  ResetOsTimer
 * DESCRIPTION
 *  Resets the actual OS timer based on the shortest remaining
 *  timer.
 * PARAMETERS
 *  curTime     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ResetOsTimer(TimeT curTime)
{
    EvmTimer *timer;
    ListEntry *node;
    TimeT timeElapsed;
    TimeT minWait = (TimeT) (-1);

    /* Look for timers scheduled in the future */
    if (!IsListEmpty(&BTC(timerList)))
    {

        node = GetHeadList(&BTC(timerList));
        while (node != &BTC(timerList))
        {
            timer = (EvmTimer*) node;
            node = node->Flink;
            timeElapsed = curTime - timer->startTime;

            /* If the timer must elapse sooner than minWait, adjust minWait */
            if (timeElapsed < timer->time)
            {
                minWait = min(minWait, timer->time - timeElapsed);
            }
            else
            {
                OS_Report("[TIMER] Timer(%x) timeout (%u, %u) when reset OS timer. Just Fire timer", timer, timeElapsed, timer->time);
                TimerFired();
                return;
            }
        }
    }

    /* Are any timers in the future? */
    if (minWait < (TimeT) (-1))
    {
        OS_StartTimer(minWait, TimerFired);
    }
    else
    {
        OS_CancelTimer();
    }
}

#endif /* XA_MULTITASKING == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  EvmStartTimer
 * DESCRIPTION
 *  Start an Event Manager timer.
 * PARAMETERS
 *  timer               [IN] 
 *  ticks               [IN]        
 *  stopHardware        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void EvmStartTimer(EvmTimer *timer, TimeT ticks, BOOL stopHardware)
{
    /* Assert(timer->func != 0); */

    if (stopHardware)
    {
        OS_StopHardware();
    }

    /* Configure the timer's internals */
    timer->startTime = OS_GetSystemTime();
    timer->time = ticks;

    /* If the timer is already on the list, don't add it again. */
    if (!IsNodeOnList(&BTC(timerList), &timer->node))
    {
        InsertHeadList(&BTC(timerList), &timer->node);
    }

    /* Reset the time amount for the OS timer */
    ResetOsTimer(timer->startTime);

    if (stopHardware)
    {
        OS_ResumeHardware();
    }
}


/*****************************************************************************
 * FUNCTION
 *  CheckTimers
 * DESCRIPTION
 *  Look for expired timers and call their notify functions
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void CheckTimers(void)
{
    EvmTimer *timer;
    ListEntry *node;
    TimeT curTime;
    TimeT timeElapsed;
    U8 timerFired;

    //bt_prompt_trace(MOD_BT, "[BT] +CheckTimers");
    OS_StopHardware();

    timerFired = FALSE;

    /* See if we have any expired timers */
    if (!IsListEmpty(&BTC(timerList)))
    {

        curTime = OS_GetSystemTime();

        /* Look through all active timers */
        node = GetHeadList(&BTC(timerList));
        while (node != &BTC(timerList))
        {
            timer = (EvmTimer*) node;
            node = node->Flink;
            /* Calculate elapsed time */
            if(curTime >= timer->startTime){
                timeElapsed = curTime - timer->startTime;
            }else{
                bt_prompt_trace(MOD_BT, "[TIMER] timer overrun : curTime=%u, timer->startTime=%u", curTime, timer->startTime);
                timeElapsed = curTime + (0xFFFFFFFF - timer->startTime);
            }
            //bt_prompt_trace(MOD_BT, "[BT] timeElapsed=%d, wait time=%d",timeElapsed, timer->time);
            /* If its time is complete, remove the timer and fire it. */
            if (timeElapsed >= timer->time)
            {
                RemoveEntryList(&timer->node);
                OS_ResumeHardware();
                //bt_prompt_trace(MOD_BT, "[BT] timer->func(timer) : 0x%x", (U32)timer->func);
                timer->func(timer);
                //bt_prompt_trace(MOD_BT, "[BT] timer->func(timer) success");
                timerFired = TRUE;

                OS_StopHardware();

                /*
                 * Start looking back at the beginning of the list
                 * This is necessary because the contents of the list
                 * might have been modified during timer->func.
                 */
                node = GetHeadList(&BTC(timerList));
                curTime = OS_GetSystemTime();
            }
        }

        /*
         * If any timer fired, we need to reset the OS timer and
         * re-schedule the stack task.
         */
        if (timerFired)
        {
            OS_NotifyEvm();
            ResetOsTimer(curTime);
        }
    }

    OS_ResumeHardware();
    //bt_prompt_trace(MOD_BT, "[BT] -CheckTimers");
}


/*****************************************************************************
 * FUNCTION
 *  EVM_Process
 * DESCRIPTION
 *  Process all outstanding events.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void EVM_Process(void)
{

    /* Do not permit stack API calls to occur until we're done */
    OS_LockStack();

    /* Let HCI process any events */
    HCI_Process();

    /* Look for timers that might have fired */
    CheckTimers();

    /* Permit stack API calls again */
    OS_UnlockStack();
}

#endif /* (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED) */ /* (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED) */

