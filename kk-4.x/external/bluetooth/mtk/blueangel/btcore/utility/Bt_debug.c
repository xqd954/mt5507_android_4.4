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

#include "bt_common.h"
#include "btconfig.h"
#include "xatypes.h"
#include "osapi.h"
#include <dlfcn.h>

#define LINE_WIDTH 8

/* +VSNIFF */
typedef int (*vsniffinit)();

static void* libsniff = NULL;
static vsniffinit g_pSniffinit = NULL;
static vsniffinit g_pSniffdeinit = NULL;
vsniff_write p_sniffwrite = NULL;
vsniff_read p_sniffread = NULL;

void initVirtualSniff(){
    libsniff = dlopen("/system/lib/libbtsniff.so", RTLD_LAZY);
    if (!libsniff) 
    {
        bt_prompt_trace(MOD_BT, "dlopen sniff failed : %s\n", dlerror());
    }else{
        g_pSniffinit = dlsym(libsniff, "init");
        if(g_pSniffinit == NULL){
            bt_prompt_trace(MOD_BT, "g_pSniffinit is NULL");
            dlclose(libsniff);
            libsniff = NULL;
        }else if(g_pSniffinit() >= 0){
            bt_prompt_trace(MOD_BT, "init sniff success");
            p_sniffwrite = dlsym(libsniff, "writePacket");
            p_sniffread = dlsym(libsniff, "readPacket");
            bt_prompt_trace(MOD_BT, "p_sniffwrite=%p, p_sniffread=%p", p_sniffwrite, p_sniffread);
        }else{
            bt_prompt_trace(MOD_BT, "init sniff failed");
            dlclose(libsniff);
            libsniff = NULL;
        }
    }
}

void deinitVirtualSniff(){
    libsniff = dlopen("/system/lib/libbtsniff.so", RTLD_LAZY);
    if (!libsniff) 
    {
        bt_prompt_trace(MOD_BT, "dlopen sniff failed : %s\n", dlerror());
    }else{
        g_pSniffdeinit = dlsym(libsniff, "deinit");
        if(g_pSniffdeinit == NULL){
            bt_prompt_trace(MOD_BT, "g_pSniffdeinit is NULL");
        }else if(g_pSniffdeinit() >= 0){
            bt_prompt_trace(MOD_BT, "deinit sniff success");
        }else{
            bt_prompt_trace(MOD_BT, "deinit sniff failed");
        }
		dlclose(libsniff);
        libsniff = NULL;
    }
}

/* -VSNIFF */

/*---------------------------------------------------------------------------
 * DisplayHex()
 *
 *     Debug output tool for displaying data bytes read from and written
 *     to the UART port.
 */

#if 0
/*****************************************************************************
 * FUNCTION
 *  DisplayHex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Source      [IN]        
 *  Buffer      [IN]        
 *  Len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void DisplayHex(const char *Source, const U8 *Buffer, U16 Len)
{
    const U8 *p = Buffer;
    char *o, output[(LINE_WIDTH * 4) + 20]; /* 12 bytes padding */
    int po;

    po = 0;
    while (p < Buffer + Len)
    {
        o = output;

        /* Append proper line advance spaces */
        o += sprintf(o, " %s   ", Source);

        /* Dumps the packet in basic ASCII dump format */
        po = 0;
        while (po < LINE_WIDTH)
        {
            if (p + po < Buffer + Len)
            {
                o += sprintf(o, "%02x ", p[po]);
            }
            else
            {
                o += sprintf(o, "   ");
            }
            if (++po == LINE_WIDTH / 2)
            {
                o += sprintf(o, "  ");
            }
        }
        o += sprintf(o, "    ");

        po = 0;
        while (po < LINE_WIDTH)
        {
            if (p + po < Buffer + Len)
            {
                o += sprintf(o, "%1c", ((p[po] >= ' ' && p[po] <= '~') ? p[po] : '.'));
            }
            else
            {
                break;
            }
            po++;
        }

        printf("%s\n", output);
        p += po;
    }
}
#endif

