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
 * Bt_ext_timer.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to expose common api for profile&gap
 *
 * Author:
 * -------
 * Liang Cheng
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
#define LOG_TAG "bt_ext_timer.c"

#include "utils/Log.h"

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>

#include "bt_ext_timer.h"


#define RESTVAL(t) \
    ((t).it_value.tv_sec * SECTICKS + (t).it_value.tv_usec / TICKUNIT + \
     ((((t).it_value.tv_usec % TICKUNIT) >= (TICKUNIT >> 1)) ? 1 : 0))

static struct btTimer *TimerList = NULL, *ExpiredList = NULL;

static void StopTimerNoBlock(struct btTimer *);

static const char *
tState2Nam(u_int state)
{
  static const char * const StateNames[] = { "stopped", "running", "expired" };

  if (state >= sizeof StateNames / sizeof StateNames[0])
    return "unknown";
  return StateNames[state];
}

void
timer_Stop(struct btTimer *tp)
{
  sigset_t mask, omask;

  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  sigprocmask(SIG_BLOCK, &mask, &omask);
  StopTimerNoBlock(tp);
  sigprocmask(SIG_SETMASK, &omask, NULL);
}

void
timer_Start(struct btTimer *tp)
{
  struct itimerval itimer;
  struct btTimer *t, *pt;
  u_long ticks = 0;
  sigset_t mask, omask;

  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  sigprocmask(SIG_BLOCK, &mask, &omask);

  if (tp->state != TIMER_STOPPED)
    StopTimerNoBlock(tp);

  if (tp->load == 0) {
  	ALOGI("%s timer[%p] has 0 load!\n", tp->name, tp);
    sigprocmask(SIG_SETMASK, &omask, NULL);
    return;
  }

  /*
   * We just need to insert tp in the correct relative place.  We don't
   * need to adjust TimerList->rest (yet).
   */
  if (TimerList && getitimer(ITIMER_REAL, &itimer) == 0)
    ticks = RESTVAL(itimer) - TimerList->rest;

  pt = NULL;
  for (t = TimerList; t; t = t->next) {
    if (ticks + t->rest >= tp->load)
      break;
    ticks += t->rest;
    pt = t;
  }

  tp->state = TIMER_RUNNING;
  tp->rest = tp->load - ticks;

  if (t)
  	ALOGI("timer_Start: Inserting %s timer[%p] before %s "
              "timer[%p], delta = %ld\n", tp->name, tp, t->name, t, tp->rest);
  else
  	ALOGI("timer_Start: Inserting %s timer[%p]\n", tp->name, tp);

  /* Insert given *tp just before *t */
  tp->next = t;
  if (pt) {
    pt->next = tp;
  } else {
    TimerList = tp;
    timer_InitService(t != NULL);	/* [re]Start the Timer Service */
  }
  if (t)
    t->rest -= tp->rest;

  sigprocmask(SIG_SETMASK, &omask, NULL);
}

static void
StopTimerNoBlock(struct btTimer *tp)
{
  struct itimerval itimer;
  struct btTimer *t, *pt;

  /*
   * A RUNNING timer must be removed from TimerList (->next list).
   * A STOPPED timer isn't in any list, but may have a bogus [e]next field.
   * An EXPIRED timer is in the ->enext list.
   */

  if (tp->state == TIMER_STOPPED)
    return;

  pt = NULL;
  for (t = TimerList; t != tp && t != NULL; t = t->next)
    pt = t;

  if (t) {
    if (pt)
      pt->next = t->next;
    else {
      TimerList = t->next;
      if (TimerList == NULL)	/* Last one ? */
	timer_TermService();	/* Terminate Timer Service */
    }
    if (t->next) {
      if (!pt && getitimer(ITIMER_REAL, &itimer) == 0)
        t->next->rest += RESTVAL(itimer); /* t (tp) was the first in the list */
      else
        t->next->rest += t->rest;
      if (!pt && t->next->rest > 0)   /* t->next is now the first in the list */
        timer_InitService(1);
    }
  } else {
    /* Search for any pending expired timers */
    pt = NULL;
    for (t = ExpiredList; t != tp && t != NULL; t = t->enext)
      pt = t;

    if (t) {
      if (pt)
        pt->enext = t->enext;
      else
        ExpiredList = t->enext;
    } else if (tp->state == TIMER_RUNNING)
    	ALOGE("Oops, %s timer not found!!\n", tp->name);
  }

  tp->next = tp->enext = NULL;
  tp->state = TIMER_STOPPED;
}

static void
TimerService(int data)
{
  struct btTimer *tp, *exp, *next;

  tp = TimerList;
  if (tp) {
    tp->rest = 0;

    /* Multiple timers might expire at once. Create a list of expired timers */
    exp = NULL;
    do {
      tp->state = TIMER_EXPIRED;
      next = tp->next;
      tp->enext = exp;
      exp = tp;
      tp = next;
    } while (tp && tp->rest == 0);

    TimerList = tp;
    if (TimerList != NULL)	/* Any timers remaining ? */
      timer_InitService(1);	/* Restart the Timer Service */
    else
      timer_TermService();	/* Stop the Timer Service */

    /* Process all expired timers */
    while (exp) {
      ExpiredList = exp->enext;
      exp->enext = NULL;
      if (exp->func)
        (*exp->func)(exp->arg);
      exp = ExpiredList;
    }
  }
}

void
timer_Show()
{
  struct itimerval itimer;
  struct btTimer *pt;
  long rest;

  /*
   * Adjust the base time so that the deltas reflect what's really
   * happening.  Changing TimerList->rest might cause it to become zero
   * (if getitimer() returns a value close to zero), and the
   * timer_InitService() call will call setitimer() with zero it_value,
   * stopping the itimer... so be careful!
   */
  if (TimerList && getitimer(ITIMER_REAL, &itimer) == 0)
    rest = RESTVAL(itimer) - TimerList->rest;
  else
    rest = 0;

#define SECS(val)	((val) / SECTICKS)
#define HSECS(val)	(((val) % SECTICKS) * 100 / SECTICKS)
#define DISP								\
  "%s timer[%p]: freq = %ld.%02lds, next = %lu.%02lus, state = %s\n",	\
  pt->name, pt, SECS(pt->load), HSECS(pt->load), SECS(rest),		\
  HSECS(rest), tState2Nam(pt->state)

  ALOGI("---- Begin of Timer Service List---\n");

  for (pt = TimerList; pt; pt = pt->next) {
    rest += pt->rest;
    ALOGI(DISP);
  }

  ALOGI("---- End of Timer Service List ---\n");
}


void
timer_InitService(int restart)
{
  struct itimerval itimer;

  if (TimerList) {
    if (!restart)
      signal(SIGALRM, TimerService);
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = TimerList->rest / SECTICKS;
    itimer.it_value.tv_usec = (TimerList->rest % SECTICKS) * TICKUNIT;
    if (setitimer(ITIMER_REAL, &itimer, NULL) == -1)
    	ALOGE("Unable to set itimer (%s)\n", strerror(errno));
  }
}

void
timer_TermService(void)
{
  struct itimerval itimer;

  itimer.it_interval.tv_usec = itimer.it_interval.tv_sec = 0;
  itimer.it_value.tv_usec = itimer.it_value.tv_sec = 0;
  if (setitimer(ITIMER_REAL, &itimer, NULL) == -1)
  	ALOGE("Unable to set itimer (%s)\n", strerror(errno));
  signal(SIGALRM, SIG_IGN);
}
