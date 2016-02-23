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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * tst_trace.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file defines the index trace of TST writer module.
 *
 * Author:
 * -------
 * Spancer 	(mtk00264)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime:   Jul 25 2005 12:36:04  $
 * $Log:   //mtkvs01/vmdata/Maui_sw/archives/mcu/tst/include/tst_trace.h-arc  $
 *
 * Mar 25 2009 mtk01988
 * [MAUI_01654764] [TST] Remove windows.h inclusion in tst_def.h to prevent MMI compile errors
 * 
 *
 * Mar 19 2009 mtk01988
 * [MAUI_01651132] [TST] Reduce memory usage
 * 
 *
 * Feb 19 2009 mtk01988
 * [MAUI_01632992] [TST] Add debug info for UT env
 * 
 *
 * Jan 15 2009 mtk01988
 * [MAUI_01316919] [TST] Support TST Ring buffrt flush function and enhance the memory copy speed on ts
 * 
 *
 * Oct 8 2008 mtk01988
 * [MAUI_01251803] [Change Feature][SST] Add a function to query type of register
 * 
 *
 * Aug 7 2008 mtk01988
 * [MAUI_00818518] [TST] Add reset command when system asserts and print traces after the redump action
 * 
 *
 * Apr 2 2007 mtk00730
 * [MAUI_00376666] [TST] Enable core dump in the feature:  __TST_WRITE_TO_FILE_ONLY__
 * 
 *
 * May 23 2006 mtk00730
 * [MAUI_00196720] [TST] Redump the tst ring buffer in the precise way
 * 
 *
 * May 22 2006 mtk00730
 * [MAUI_00195624] [TST] 1. indicate number of lost frames 2. redump tst ring buffer at assert/fatal er
 * 
 * 
 *    Rev 1.9   Jul 25 2005 15:40:10   mtk00730
 * Revise kal_buffer_trace interface
 * Resolution for 12074: [TST][Revision] Revise kal_buffer_trace interface to reduce code size of related traces
 * 
 *    Rev 1.8   Jul 19 2005 08:38:08   mtk00730
 * 1. tst_report_version, 2. kal_buffer_trace
 * Resolution for 11979: [TST][New Feature] kal_buffer_trace, report version, TRACE_PEER
 * 
 *    Rev 1.7   May 17 2005 03:01:34   BM_Trunk
 * Karlos:
 * add copyright and disclaimer statement
 * 
 *    Rev 1.6   Jan 18 2005 02:16:22   BM
 * append new line in W05.04
 * 
 *    Rev 1.5   Sep 24 2004 20:11:04   mtk00730
 * 1. Revise dumping for fatal error
 * 2. For cost down, enclosed kal_xxx_trace functions in compile option DEBUG_KAL
 * Resolution for 7682: [TST][Revise] Re-implement the mechanism of queuing messages to send
 * 
 *    Rev 1.4   Sep 15 2004 10:57:58   mtk00730
 * 1. Add new implementation of queuing messages to send (old implementation is enclosed by TST_BEFORE_COST_DOWN_ARCH)
 * 2. Open/close UART port in run time
 * Resolution for 7681: [TST][Add Feature] Open/close UART port in run time
 * Resolution for 7682: [TST][Revise] Re-implement the mechanism of queuing messages to send
 * 
 *    Rev 1.3   Jan 16 2003 11:48:26   mtk00264
 * Add Engineer mode and SIM Editor functionality
 * Resolution for 650: Engineer Mode and SIM Editor to TST module
 * 
 *    Rev 1.2   10 Dec 2002 12:00:26   mtk00264
 * Revise the file header to MTK format.
 * Resolution for 26: Remove CCWW relatived file header.
 * 
 *    Rev 1.1   06 Dec 2002 14:58:10   mtk00264
 * Modify file relatived headers.
 * Resolution for 26: Remove CCWW relatived file header.
 * 
 *    Rev 1.0   Nov 30 2002 19:54:24   admin
 * Initial revision.
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifndef _TST_TRACE_H
#define _TST_TRACE_H

#ifdef BTMTK_ON_WISE
#ifndef GEN_FOR_PC
   #ifndef _STACK_CONFIG_H
   #error "stack_config.h should be included before tst_config.h"
   #endif

#else
   #include "kal_trace.h"
#endif /* GEN_FOR_PC */


#ifndef _KAL_TRACE_H
   #error "kal_trace.h should be included before tst_trace.h"
#endif

#else /* BTMTK_ON_WISE */
#include "bt_adp_log.h"
#endif  /* BTMTK_ON_WISE */



BEGIN_TRACE_MAP(MOD_TST)
   TRC_MSG(TST_MSG1, "System lost %d primtives and %d traces and %d EM datas")
   TRC_MSG(TST_MSG2, "System lost %d traces, %d primtives and %d others")
   TRC_MSG(TR_MSG1, "Lost Primitive [%d / %d] (32 %d, 64 %d, 128 %d, 256 %d, 512 %d, 1024 %d, 2048 %d)")
   TRC_MSG(TR_MSG2, "Lost Trace [%d / %d] (32 %d, 64 %d, 128 %d, 256 %d, 512 %d, 1024 %d, 2048 %d)")
   TRC_MSG(TR_MSG3, "Lost EM [%d / %d] (32 %d, 64 %d, 128 %d, 256 %d, 512 %d, 1024 %d, 2048 %d)")
   TRC_MSG(DROP_IN_DUMP, "Lost one message not filled completely (length is %d)")
   TRC_MSG(TR_TRACE_PEER_MSG, "Message class: TRACE_PEER")
   TRC_MSG(TR_BUFFER_TRACE, "Buffer to trace by kal_buffer_trace")   
   TRC_MSG(TR_REDUMP_WARN, "Caution: To re-dump TST ring buffer!")
   TRC_MSG(TR_REDUMP_END_WARN, "Caution: Finish re-dumping TST ring buffer!")
   TRC_MSG(TR_ONLY_LOG_TO_FILE_START, "Caution: Start to only log to file!")
   TRC_MSG(TR_ONLY_LOG_TO_FILE_STOP, "Caution: Resume to log to UART/USB! (%d messeages are in-between)")
   TRC_MSG(TST_LGA_EMB_LOGS_MSG, "MCU_BUS read_index  = %d, duration = %d, bus_ccnt = %d, trans_all_cnt = %d, trans_cnt = %d, word_all_cnt = %d, word_cnt = %d, busy_all_cnt = %d, busy_cnt = %d")
   TRC_MSG(TST_LGA_EMB_LOGS_BUF_LACK, "The buffer for LOGACC EMI LOG BUF is overflow")
   TRC_MSG(TST_LGA_EMB_COUNTER_OVERRUN, "The counter of EMI BUS is overflow for %d times")
   TRC_MSG(TST_CLEAN_BUF_DATA, "The TST ring buffer is erased, %d PS logs bytes are dropped")
   TRC_MSG(TR_CMD_SET_TRAP_FILTER, "[TR] The SAP trap settings are applied: SAP type:%Msap_type, is_trap: %d")   
   TRC_MSG(TR_CMD_SET_EMI_MASTER_MONITOR, "The result of monitored masters setting: %d. Set value (%d, %d, %d)")
   TRC_MSG(TR_CMD_SET_BW_LIMIT, "The result of EMI bandwidth limit cmd is %d")
   TRC_MSG(TR_CMD_SET_CPU_LIMIT, "The result of Task execution time limit setting is is %d.")
   TRC_MSG(TR_SET_CATCHER_FILTER_NVRAM_NOT_SAVE, "[TST Reader] Filters are set successfully but not saved to NVRAM!!")
   TRC_MSG(TR_GET_ERROR_UT_PRIMITIVE, "[TST Reader] TR recevied one incorrect UT primitive")
   TRC_MSG(TR_GET_ERROR_COMMAND, "[TST Reader] TR recevied one incorrect command from PC.")
   TRC_MSG(TR_DETECT_MODULE_NOT_EXIST, "[TST Reader] %Mmodule_type doesn't exist")
   TRC_MSG(TR_RECEIVE_UNEXPECTED_CHAR, "[TST Reader] TR: Discards %d chars.")   
   TRC_MSG(TR_LOST_ONE_CMD, "[TST Reader] tst lost one command !!")
   TRC_MSG(TR_SET_CATCHER_FILTER_NVRAM_SAVE_FAIL, "[TST Reader] TST Save Filters to Flash Error !! result = %d")
   TRC_MSG(TR_QUERY_NON_SUPPORT_AREA, "[TST Reader] Don't support read memory range of DSP-DMA (0x6000-0000 ~ 0x6FFF-FFFF) !!")
   TRC_MSG(TST_TEST_KAL_TRACE0, "[TST] kal_trace0")
   TRC_MSG(TST_TEST_KAL_TRACE1, "[TST] kal_trace1: %c")
   TRC_MSG(TST_TEST_KAL_TRACEM21, "[TST] kal_traceM21: %c %d %c %d %c %d %c %d %c")
   TRC_MSG(TST_TEST_KAL_TRACE31, "[TST] kal_trace31: %d %d %d %d %d %d %d %c %c %c")
   TRC_MSG(TST_TEST_KAL_TRACE32, "[TST] kal_trace32: %d %d %d %d %d %d %d %d ")
   TRC_MSG(TST_TEST_KAL_TRACE33, "[TST] kal_trace33: %d %d %d %d %d %d %d %d %c ")
   TRC_MSG(TST_TEST_KAL_TRACE126, "[TST] kal_trace126: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %c %c")
   TRC_MSG(TST_TEST_KAL_TRACE129, "[TST] kal_trace129: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %c")
   TRC_MSG(TST_TEST_KAL_DEV_TRACE0, "[TST] kal_dev_trace0")
   TRC_MSG(TST_TEST_KAL_ASSERT_TRACE0, "[TST] kal_assert_trace0")
   TRC_MSG(TST_TEST_KAL_TRACE_for_3rd_party0, "[TST] kal_trace_for_3rd_party0")
   TRC_MSG(TST_TEST_KAL_TRACE_for_3rd_partyStr, "[TST] kal_trace_for_3rd_party: %d, %s")
   TRC_MSG(TR_UT_RECEIVE_INJECT_MSG, "[TST Reader][UT] Receive one inject UT message. Src_id=%Mmodule_type, dst_id=%Mmodule_type, msg_id=%Mmsg_type")
   TRC_MSG(TR_UT_ALLOCATE_UT_LOCAL_BUF, "[TST Reader][UT] Allocate a local buffer. Addr: 0x%X, Size: 0x%X")
   TRC_MSG(TR_UT_ALLOCATE_UT_LOCAL_BUF_BUFS, "[TST Reader][UT] Allocate a buf for local parameter. Addr: 0x%X, Size: 0x%X")
   TRC_MSG(TR_UT_ALLOCATE_UT_PEER_BUF, "[TST Reader][UT] Allocate a peer buffer. Addr: 0x%X, Size: 0x%X")   
   TRC_MSG(TR_UT_MSG_REF_COUNT_INCORRECT, "[TST Reader][UT] Incorrect reference count for the msg:0x%X")   
   TRC_MSG(TR_FS_OPEN_FILE_FAILED, "[TST Reader] Fail to open files for write. Error code: 0x%X")   
   TRC_MSG(TR_FS_FILE_SEEK_FAILED, "[TST Reader] Fail to seek the file to 0x%x. Error code: 0x%X")   
   TRC_MSG(TR_FS_WRITE_LENGTH_LESS_THAN_EXPECTATION, "[TST Reader] The length being written is less than expected one. Actrual write length 0x%x. Expected write length: 0x%X")   
END_TRACE_MAP(MOD_TST)
#endif /* _TST_TRACE_H */


