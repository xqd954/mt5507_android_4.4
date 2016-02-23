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
#if !defined(__PLATFORM_SUPPORTED_CATCHER__)

#include "bt_common.h"
#include "tst_ringbuffer.h"
#include "osapi.h"
#if defined(BTMTK_ON_LINUX)
#include <cutils/log.h>
#endif

unsigned char tst_buffer[TST_RB_SIZE];
unsigned int tst_last_pos = 0;
unsigned int tst_cur_pos = 0;
unsigned int tst_write_pos = 0;
unsigned int tst_full = 0;
unsigned int tst_lost_primitive = 0;
unsigned int tst_lost_trace = 0;
unsigned int tst_expand_bytes = 0;

#define LOST_FRAME_SIZE (15+sizeof(tst_index_trace_header_struct))

static tst_wakeup_fun wakeup_fun = NULL;
static tst_send_fun send_fun = NULL;
kal_bool tst_rb_init(tst_wakeup_fun wakeup, tst_send_fun send)
{
    bt_tst_log("tst_rb_init");
    if(!wakeup || !send)
        return KAL_FALSE;
    wakeup_fun = wakeup;
    send_fun = send;
    return KAL_TRUE;
}

kal_bool IsFrameLost(void)
{
    //bt_tst_log("IsFrameLost is %d\r\n", (tst_lost_primitive || tst_lost_trace)?1:0);
    return (tst_lost_primitive || tst_lost_trace);
}

unsigned char tstChecksum(void)
{
    unsigned int expandbytes = 0;
    unsigned int idx = tst_last_pos;
    unsigned char checksum = 0;

    while(idx != tst_write_pos)
    {
        checksum ^= tst_buffer[idx];
        if(tst_buffer[idx] == 0xA5 || tst_buffer[idx] == 0x5A)
            tst_expand_bytes++;
        idx = ((idx+1)&TST_RB_SIZE_MASK);
    }
    /* checksum also has to expand */
    if(checksum ==0xA5 || checksum == 0x5A){
        //bt_tst_err("tstChecksum : checksum=%d", checksum);
        tst_expand_bytes++;
    }
    
    bt_tst_log("tstChecksum, %d, %d, %d, %d", tst_write_pos, tst_last_pos, checksum, tst_expand_bytes);
    return checksum;
}
unsigned int g_nMaxSize = 0;
unsigned int g_nMinSize = 0x7FFFFFFF;
kal_bool tstChecksize(size_t size)
{
    size++; /* Add 1 for padding */
    if(tst_lost_primitive | tst_lost_trace)
        size += LOST_FRAME_SIZE+1; /* Plus 1 for padding  */
    if(size > g_nMaxSize) g_nMaxSize=size;
    if(size < g_nMinSize) g_nMinSize=size;

    if(size > GET_RB_FREE_SIZE() && wakeup_fun)
            wakeup_fun();
        
    return (size <= GET_RB_FREE_SIZE());
}

void tstmemcpy(const U8 *src, size_t length, kal_bool last)
{
    size_t  length2 = 0;
    bt_tst_log("tstmemcpy(%p, %d, %d) : %d, %d", src, length, last, tst_last_pos, tst_write_pos);
    if(length > (TST_RB_SIZE-tst_write_pos))
    {
        length2 = length - (TST_RB_SIZE-tst_write_pos);
        length -= length2;
        /* Copy second segment */
        OS_MemCopy(&tst_buffer[0], src+length, length2);
    }
    else
    {
        length2 = 0;
    }
    /* Copy first segment */
    OS_MemCopy(&tst_buffer[tst_write_pos], src, length);
    /* Update tst_write_pos */
    tst_write_pos = ((tst_write_pos+length)&TST_RB_SIZE_MASK)+length2;
    if(last)
    {
        if((tst_expand_bytes+(tst_write_pos+TST_RB_SIZE-tst_last_pos))&0x1)
        {
            tst_buffer[tst_write_pos] = 0;
            tst_write_pos = ((tst_write_pos+1)&TST_RB_SIZE_MASK);
        }
        tst_expand_bytes = 0;
        /* Update tst_last_pos */
        tst_last_pos = tst_write_pos;
        /* Wakeup tst thread */
        if(wakeup_fun)
            wakeup_fun();
    }
}

int tstFlushBuffer(void)
{
    int res = 0;
    unsigned int last = tst_last_pos;

    if(last > tst_cur_pos)
    {
        res = send_fun(&tst_buffer[tst_cur_pos], last-tst_cur_pos);
    }
    else if(last < tst_cur_pos)
    {
        res = send_fun(&tst_buffer[tst_cur_pos], TST_RB_SIZE-tst_cur_pos);
        if(res >= 0)
            res = send_fun(&tst_buffer[0], last);
    }
    bt_tst_log("tstFlushBuffer : tst_cur_pos=%u, last=%u, res=%d", tst_cur_pos, last, res);
    /* Update tst_cur_pos */
    if(res >= 0)
        tst_cur_pos = last;
    else
        bt_tst_err("tstFlushBuffer failed : res=%d", res);
    tst_full = 0;
    return res;
}

int g_tstWaitFlag = 0;
//int g_tstSendFlag = 0;
extern kal_bool g_tstStarted;
void dumpTstStatus(void)
{
    ALOGI("[DUMP] tst_buffer=%p, TST_RB_SIZE=0x%X, TST_RB_SIZE_MASK=0x%X", tst_buffer, TST_RB_SIZE, TST_RB_SIZE_MASK);
    ALOGI("[DUMP] tst_cur_pos=%d, tst_last_pos=%d, tst_write_pos=%d", tst_cur_pos, tst_last_pos, tst_write_pos);
    ALOGI("[DUMP] tst_lost_trace=%d, tst_lost_promitive=%d", tst_lost_trace, tst_lost_primitive);
    ALOGI("[DUMP] wakeup_fun=%p, send_fun=%p", wakeup_fun, send_fun);
    ALOGI("[DUMP] g_tstWaitFlag=%d", g_tstWaitFlag);
    //ALOGI("[DUMP] g_tstSendFlag=%d", g_tstSendFlag);
    ALOGI("[DUMP] g_nMaxSize=%d, g_nMinSize=%d", g_nMaxSize, g_nMinSize);
    ALOGI("[DUMP] g_tstStarted=%d", g_tstStarted);
}

#endif /* __PLATFORM_SUPPORTED_CATCHER__ */
