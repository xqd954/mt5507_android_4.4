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

#ifndef __TST_RINGBUFFER__
#define __TST_RINGBUFFER__

#ifdef __EXTRA_DEBUG_FEATURE__
#define TST_RB_SIZE (64*1024)
#else
#define TST_RB_SIZE (64*1024)
#endif
#define TST_RB_SIZE_MASK (TST_RB_SIZE-1)
extern unsigned int tst_last_pos;
extern unsigned int tst_cur_pos;
extern unsigned int tst_lost_primitive;
extern unsigned int tst_lost_trace;

typedef void (*tst_wakeup_fun)(void);
typedef int (*tst_send_fun)(U8 *buf, size_t size);

#define IS_RB_EMPTY()   (tst_last_pos == tst_cur_pos)
#define GET_RB_SIZE()   ((TST_RB_SIZE+tst_last_pos-tst_cur_pos)&TST_RB_SIZE_MASK)
#define GET_RB_FREE_SIZE()   (((TST_RB_SIZE+tst_cur_pos-tst_last_pos)&TST_RB_SIZE_MASK)-1)
//#define GET_RB_CONTIGUOUS_FREE_SIZE()   (((TST_RB_SIZE+tst_cur_pos-tst_last_pos)&TST_RB_SIZE_MASK)-1)
kal_bool IsFrameLost(void);
kal_bool tst_rb_init(tst_wakeup_fun wakeup, tst_send_fun send);
unsigned char tstChecksum(void);
kal_bool tstChecksize(size_t size);
void tstmemcpy(const U8 *src, size_t length, kal_bool last);
int tstFlushBuffer(void);
#endif
