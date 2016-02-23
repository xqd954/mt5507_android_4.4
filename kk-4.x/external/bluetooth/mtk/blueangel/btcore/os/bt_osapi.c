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
 *     $Workfile:osapi.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     API abstraction layer for the Win32 API. This allows support for
 *     Windows 95, 98, NT, and 2000.
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

/*****************************************************************************
* Include
*****************************************************************************/
#define LOG_TAG "MTKBT"

#if defined(__FOOTPRINT_MEASUREMENT__)
void OS_Report(const char *format, ...)
{}
#endif

/* basic system service headers */
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#include "bt_common.h"
#include "osapi.h"
#include "eventmgr.h"
#include "bt.h"
#include "bt_adp_system.h"
#include "bt_adp_log.h"

#if defined(BTMTK_ON_LINUX)
#include "stdlib.h"
#include <cutils/log.h>
#include <cutils/properties.h>
#endif

//#define PRINT_TYPE_DEBUG
/****************************************************************************
 *
 * Prototypes
 *
 ****************************************************************************/


extern int rand(void);

extern void bt_profiles_reset(void);


/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Initializes the operating system.
 */


/*****************************************************************************
 * FUNCTION
 *  OS_Init
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
BOOL OS_Init(void)
{

    /*------------------------------------------------------------
     * Step three: initialize the event manager.
     */
    BTCoreSetSysInitState(1);
    if (EVM_Init() == FALSE)
    {
#ifndef BTMTK_ON_LINUX
        OS_Deinit();
#endif
        /* Make sure the os_init is false */
        BTCoreSetSysInitState(0);
        return FALSE;
    }
    BTCoreEnabledSleepMode(0);
    OS_NotifyEvm();

#ifdef BTMTK_ON_LINUX
    srand(time(0));
#endif

    return TRUE;

}

/*---------------------------------------------------------------------------
 * If de-initialization was necessary, this function could be filled out
 */


/*****************************************************************************
 * FUNCTION
 *  OS_Deinit
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void OS_Deinit(void)
{
    /* Cancel any running timers */
    OS_CancelTimer();

    /* Deinitialize the event manager */
    EVM_Deinit();

    BTCoreEnabledSleepMode(1);
    BTCoreSetSysInitState(0);


}


/*****************************************************************************
 * FUNCTION
 *  OS_GetSystemTime
 * DESCRIPTION
 *  Returns the system time in ticks. Called by stack.
 *  NOTE: By definition (prototype) this function returns ticks.
 *  Windows time functions return milliseconds rather than ticks.
 *  To accommodate this, MS_TO_TICKS() in config.h (overide.h) is defined
 *  such that 1 tick == 1 millisecond. This is perfectly acceptable in porting.
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
TimeT OS_GetSystemTime(void)
{
    TimeT sys_time;
    //bt_prompt_trace(MOD_BT, "[BT] +OS_GetSystemTime");
    sys_time = BTCoreGetSystemTick();
    //bt_prompt_trace(MOD_BT, "[BT] -OS_GetSystemTime");
    return (sys_time);

}

/*****************************************************************************
 * FUNCTION
 *  OS_StartTimer
 * DESCRIPTION
 *
 * PARAMETERS
 *  time        [IN]
 *  func        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void OS_StartTimer(TimeT time, OsTimerNotify func)
{
    if ((BTCoreVerifySysInitState() == TRUE) && (BTCoreIsDeinitializing() == FALSE))
    {
        BTCoreStartTimer(time, func);
    }

}


/*****************************************************************************
 * FUNCTION
 *  OS_CancelTimer
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void OS_CancelTimer(void)
{
    if (BTCoreVerifySysInitState() == TRUE)
    {
        BTCoreCancelTimer();
    }

}


/*****************************************************************************
 * FUNCTION
 *  OS_ResetTimerId
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void OS_ResetTimerId(void)
{
    BTCoreCancelTimer();
}


/*****************************************************************************
 * FUNCTION
 *  OS_Rand
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
U16 OS_Rand(void)
{
    #ifdef BTMTK_ON_LINUX
    return rand();
    #else
    return rand();
    #endif
}


/*****************************************************************************
 * FUNCTION
 *  OS_Rand32
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
S32 OS_Rand32(void)
{
    #ifdef BTMTK_ON_LINUX
    return rand();
    #else
    return rand();
    #endif
}


/*****************************************************************************
 * FUNCTION
 *  OS_MemCopy
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest            [IN]
 *  source          [IN]
 *  numBytes        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void OS_MemCopy(void *dest, const void *source, U32 numBytes)
{
    if (dest && source && numBytes)
    {
    memcpy(dest, source, numBytes);
    }
}

/*****************************************************************************
 * FUNCTION
 *  OS_MemCmp
 * DESCRIPTION
 *
 * PARAMETERS
 *  buffer1     [IN]
 *  len1        [IN]
 *  buffer2     [IN]
 *  len2        [IN]
 * RETURNS
 *
 *****************************************************************************/
BOOL OS_MemCmp(const void *buffer1, U16 len1, const void *buffer2, U16 len2)
{
    if (len1 != len2)
    {
        return FALSE;
    }

    return (0 == memcmp(buffer1, buffer2, len1));

}

/*****************************************************************************
 * FUNCTION
 *  OS_MemSet
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [IN]
 *  byte        [IN]
 *  len         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void OS_MemSet(void *dest, U8 byte, U32 len)
{
    memset(dest, byte, len);

}


/*****************************************************************************
 * FUNCTION
 *  OS_NotifyEvm
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void OS_NotifyEvm(void)
{
#ifdef BTMTK_ON_WISE
    if ((BTCoreVerifySysInitState() == TRUE))
#else   /* BTMTK_ON_WISE */
    if ((BTCoreVerifyNotifyEvm() == TRUE))
#endif  /* BTMTK_ON_WISE */
    {
        //bt_prompt_trace(MOD_BT,"OS_NotifyEvm");
        BTCoreSendNotifySelfMessage();
    }
}


/*****************************************************************************
 * FUNCTION
 *  OS_StrCmp
 * DESCRIPTION
 *
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
U8 OS_StrCmp(const char *Str1, const char *Str2)
{
    while (*Str1 == *Str2)
    {
        if (*Str1 == 0 || *Str2 == 0)
        {
            break;
        }
        Str1++;
        Str2++;
    }

    /* Return zero on success, just like the ANSI strcmp() */
    if (*Str1 == *Str2)
    {
        return 0;
    }

    return 1;

}

char OS_ToLower(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
    {
        U8 distance = 'a' - 'A';
        ch += distance;
    }
    return ch;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_stricmp
 * DESCRIPTION
 *  Case-insensitive comparison
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
U8 OS_StriCmp(const char *str1, const char *str2)
{
    for (; OS_ToLower(*str1) == OS_ToLower(*str2); str1++, str2++)
    {
        /* Return zero on success, just like the ANSI strcmp() */
        if (*str1 == '\0')
            return 0;
    }
    return 1;
}

/*****************************************************************************
 * FUNCTION
 *  OS_StrnCmp
 * DESCRIPTION
 *
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
U8 OS_StrnCmp(const char *Str1, const char *Str2, U32 size)
{
    while (*Str1 == *Str2 && --size)
    {
        if (*Str1 == 0 || *Str2 == 0)
        {
            break;
        }
        Str1++;
        Str2++;
    }

    /* Return zero on success, just like the ANSI strcmp() */
    if (*Str1 == *Str2)
    {
        return 0;
    }

    return 1;

}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_strnicmp
 * DESCRIPTION
 *  Case-insensitive comparison
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
U8 OS_StrniCmp(const char *str1, const char *str2, U32 size)
{
    for (; --size > 0 && OS_ToLower(*str1) == OS_ToLower(*str2); str1++, str2++)
    {
        /* Return zero on success, just like the ANSI strcmp() */
        if (*str1 == '\0')
            return 0;
    }

    /* Return zero on success, just like the ANSI strcmp() */
    if (*str1 == *str2)
    {
        return 0;
    }

    return 1;
}


/*****************************************************************************
 * FUNCTION
 *  OS_StrLen
 * DESCRIPTION
 *
 * PARAMETERS
 *  Str     [IN]
 * RETURNS
 *
 *****************************************************************************/
U16 OS_StrLen(const char *Str)
{

    const char *cp = Str;

    Assert(Str);

    while (*cp != 0)
    {
        cp++;
    }

    return (U16) (cp - Str);

}

/*****************************************************************************
 * FUNCTION
 *  OS_StrCpy
 * DESCRIPTION
 *  OS strlen
 * PARAMETERS
 *  void
 * RETURNS
 *  string
 *****************************************************************************/
char* OS_StrCpy(char *dest, const char *source)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
      char *s = dest;

      while ((*dest++ = *source++) != 0)
        ;

      return s;
}

/*****************************************************************************
 * FUNCTION
 *  OS_StrnCpy
 * DESCRIPTION
 *
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
char *OS_StrnCpy(char *dest, const char *source, U32 numBytes)
{
    U32 i = 0;
    char *to = dest;

    while (*source && i < numBytes)
    {
        *to = *source;
        to++;
        source++;
        i++;
    }

    if (i < numBytes)
    {
        *to = '\0';
    }
    return dest;
}

/*****************************************************************************
 * FUNCTION
 *  OS_StrCat
 * DESCRIPTION
 *
 * PARAMETERS
 *  destintation        [IN]
 *  source              [IN]
 * RETURNS
 *
 *****************************************************************************/
char *OS_StrCat(char *dest, const char *source)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return strcat(dest, source);
}


/********************************************************************************
 * OS_SetSystemPropertyUint()
 *
 *     Set system properties with type unsigned int.
 *
 * Parameters:
 *     name - system property name        [IN]
 *     val - value of the system property   [IN]
 *
 * RETURNS:
 *     0 : failed
 *     other :success
 *******************************************************************************/
U32 OS_SetSystemPropertyUint(const char *name, U32 val, U8 persist){
#ifdef ANDROID
    char property_name[64];
    char property_value[PATH_MAX];
    if(name){
        int ret;
        if(persist){
            sprintf(property_name, "persist.bt.%s", name);
        }else{
        sprintf(property_name, "bt.%s", name);
        }
        sprintf(property_value, "%lu", val);
        if( property_set(property_name, property_value) == 0 ){
            OS_Report("set property %s to %u(%s) success", property_name, val, property_value);
            ret = property_get(property_name, property_value, NULL);
            if(ret <=0 ){
                OS_Report("get property %s failed : %d", property_name, ret);
            }else{
                OS_Report("get property %s success : %s", property_name, property_value);
            }
            return 1;
        }else{
            OS_Report("set property %s to %u fail", property_name, val);
        }
    }else{
        OS_Report("set property with null name");
    }
#endif
    return 0;
}

/********************************************************************************
 * OS_GetSystemPropertyUint()
 *
 *     Get system properties with type unsigned int.
 *
 * Parameters:
 *     name - system property name        [IN]
 *     val - value of the system property   [IN]
 *
 * RETURNS:
 *     0 : failed
 *     other :success
 *******************************************************************************/
U32 OS_GetSystemPropertyUint(const char *name, U32 *val, U8 persist){
#ifdef ANDROID
    char property_name[64];
    char property_value[PATH_MAX];
    if(name && val){
        int ret;
        if(persist){
            sprintf(property_name, "persist.bt.%s", name);
        }else{
        sprintf(property_name, "bt.%s", name);
        }
        ret = property_get(property_name, property_value, NULL);
        if(ret <=0 ){
            OS_Report("get property %s failed : %d", property_name, ret);
        }else{
            OS_Report("get property %s success : %s", property_name, property_value);
            *val = strtoul(property_value, NULL, 10);
            return 1;                
        }
    }else{
        OS_Report("set property with null name or NULL val");
    }
#endif
    return 0;
}


/********************************************************************************
 * OS_GetBitsValue()
 *
 *     Get value of bits in specified memory address
 *
 * Parameters:
 *     addr - address of the mask bits     [IN]
 *     start_pos - start bit                     [IN]
 *     bits - number of bits to get           [IN]
 * RETURNS:
 *     value in the specified bits
 *
 *******************************************************************************/
U32 OS_GetBitsValue(const U8 *addr, U32 start_pos, U32 bits){
    U32 ret = 0;
    if(addr && bits){
        start_pos += (bits-1);
        addr = (addr+(start_pos>>3));
        start_pos &= 0x7;
        while(bits){
            //printf("start_pos=%u, bits=%u, ret=0x%X\n", start_pos, bits, ret);
            ret = ret*2;
            if((*addr) & (0x1<<start_pos))
                ret += 1;
            bits--;
            if(start_pos == 0){
                addr -= 1;
                start_pos = 7;
            }else{
                start_pos--;
            }
        }
    }
    return ret;
}

U32 OS_TestBit(U32 nr, const U8 *addr){
    return ((0x1 << (nr % 8)) & (addr[nr / 8])) != 0;
}

void OS_SetBit(U32 nr, U8 *addr){
    addr[nr / 8] |= (0x1 << (nr % 8));
}

/*****************************************************************************
 * FUNCTION
 *  OS_ResetInd
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void OS_ResetInd(void)
{
    if (BTCoreReadyToPanic())
    {
        //bt_profiles_reset();
        BTCoreHandlePanic();
    }

}

/*****************************************************************************
 * FUNCTION
 *  MTK_BT_Assert
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MTK_BT_Assert(void)
{
    Assert(0 == "Contact MTK BT owner, follow BT debug SOP");
}

#if XA_DEBUG == XA_ENABLED

/*---------------------------------------------------------------------------
 * Called to indicate a failed condition
 */


/*****************************************************************************
 * FUNCTION
 *  OS_Assert
 * DESCRIPTION
 *
 * PARAMETERS
 *  expression      [IN]
 *  file            [IN]
 *  line            [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void OS_Assert(const char *expression, const char *file, U16 line)
{
    char buffer[50];

    sprintf(buffer, "File: %s, Line: %d   ", file, line);
    kal_prompt_trace(MOD_BT, buffer);
    Assert(0);
}

#endif /* XA_DEBUG == XA_ENABLED */

#ifndef __USE_CATCHER_LOG__
static char msg_buffer[500] = "[MTKBT]";   /* Output buffer */
#endif
/*****************************************************************************
 * FUNCTION
 *  OS_Report
 * DESCRIPTION
 *
 * PARAMETERS
 *  format      [IN]
 * RETURNS
 *  void
 *****************************************************************************/
#if !defined(__FOOTPRINT_MEASUREMENT__)
void OS_Report(const char *format, ...)
{
    #ifndef __USE_CATCHER_LOG__
    //char msg_buffer[500] = "[MTKBT]";   /* Output buffer */
    va_list args;
    msg_buffer[7] = 0;
    va_start(args, format);
    vsnprintf(msg_buffer+7, 200, format, args);
    va_end(args);

    ALOGI(msg_buffer);
    #else
    char buffer[200];   /* Output buffer */
    va_list args;

    va_start(args, format);
    #ifdef BTMTK_ON_LINUX
    vsnprintf(buffer, 200, format, args);
    #else
    _vsnprintf(buffer, 200, format, args);
    #endif
    va_end(args);

    bt_prompt_trace(MOD_BT, buffer);
    #endif
}
#endif
