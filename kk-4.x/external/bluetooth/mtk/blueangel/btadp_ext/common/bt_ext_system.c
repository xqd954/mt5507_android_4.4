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
 * bt_ext_system.c
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
 * Autumn Li
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
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#endif
#include "MBTType.h"
#include "MBTDataType.h"

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"

#ifdef BTMTK_ON_WISE
#include "osapi.h"
extern int ws_debug_wiselog(int eKind, int nLevel, int nLineNo, char* pszFileName, char* pFmt, va_list varArgs);
#endif

#ifdef __BT_OPP_PROFILE__
extern void btmtk_deinit_opp_server(void);
extern void btmtk_deinit_opp_client(void);
#endif
#if defined(__BT_FTS_PROFILE__)
extern void btmtk_deinit_ftp_server(void);
#endif
#if defined(__BT_FTC_PROFILE__)
extern void btmtk_deinit_ftp_client(void);
#endif
#if defined(__BT_PBAP_PROFILE__)
extern void btmtk_init_pbap(void);
extern void btmtk_deinit_pbap(void);
#endif
#if defined(__BT_HIDD_PROFILE__)
extern void btmtk_deinit_hid_device(void);
#endif
#if defined(__BT_HIDH_PROFILE__)
extern void btmtk_deinit_hid_host(void);
#endif
#ifdef __BT_SIM_PROFILE__
extern MBT_VOID btmtk_simap_deinit(MBT_VOID);
#endif

#if defined BTMTK_ON_WISE || defined BTMTK_ON_WISESDK
    #ifdef __BT_BIPI_PROFILE__
        void mbt_bip_initiator_disable_i(void);
    #endif
    #ifdef __BT_BIPR_PROFILE__
        void mbt_bip_responder_disable_i(void);
    #endif
#endif
/*****************************************************************************
 * FUNCTION
 *  btmtk_ext_init
 * DESCRIPTION
 *  
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void btmtk_init(void)
{
    /* recommend naming: btmtk_init_xxx() */
    #if defined(__BT_PBAP_PROFILE__)
     btmtk_init_pbap();
    #endif
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_ext_deinit
 * DESCRIPTION
 *  
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void btmtk_deinit(void)
{
    /* recommend naming: btmtk_deinit_xxx() */
    #ifdef __BT_OPP_PROFILE__
    btmtk_deinit_opp_server();
    btmtk_deinit_opp_client();
    #endif
    #if defined(__BT_FTS_PROFILE__)
    btmtk_deinit_ftp_server();
    #endif
    #if defined(__BT_FTC_PROFILE__)
    btmtk_deinit_ftp_client();
    #endif
     #if defined(__BT_PBAP_PROFILE__)
     btmtk_deinit_pbap();
    #endif
    #if defined(__BT_HIDD_PROFILE__)
    btmtk_deinit_hid_device();
    #endif
    #if defined(__BT_HIDH_PROFILE__)
    btmtk_deinit_hid_host();
    #endif
    #ifdef __BT_SIM_PROFILE__
    btmtk_simap_deinit();
    #endif

    #if defined BTMTK_ON_WISE || defined BTMTK_ON_WISESDK
    #ifdef __BT_BIPI_PROFILE__
        mbt_bip_initiator_disable_i();
    #endif
    #ifdef __BT_BIPR_PROFILE__
        mbt_bip_responder_disable_i();
    #endif
    #endif
}

#ifdef BTMTK_ON_WISE   
/*****************************************************************************
 * FUNCTION
 *  btmtk_message_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  message         [IN]        
 * RETURNS
 *****************************************************************************/
void btmtk_message_handler(ilm_struct *ptr)
{
    if((ptr->msg_id >= MSG_ID_BT_GAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GAP_GROUP_END))
    {
        btmtk_gap_handle_message(ptr);
    }
    else if((ptr->msg_id >= MSG_ID_BT_HFG_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HFG_GROUP_END) )
    {
        btmtk_hfg_handle_message(ptr->msg_id, ptr->local_para_ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_SPP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_SPP_GROUP_END) )
    {
    		btmtk_spp_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_GOEPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GOEPC_GROUP_END) )
    {
    		btmtk_oppc_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_GOEPS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GOEPS_GROUP_END) )
    {
    		btmtk_opps_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_OPPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_OPPC_GROUP_END) )
    {
    		btmtk_oppc_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_OPPS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_OPPS_GROUP_END) )
    {
    		btmtk_opps_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_FTPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_FTPC_GROUP_END) )
    {
    		btmtk_ftpc_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_FTPS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_FTPS_GROUP_END) )
    {
    		btmtk_ftps_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_A2DP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_A2DP_GROUP_END) )
    {
    		btmtk_a2dp_handle_message(ptr);
    }
    else if ((ptr->msg_id >= MSG_ID_BT_AVRCP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_AVRCP_GROUP_END) )
    {
    		btmtk_avrcp_handle_message(ptr);
    }
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
    else if ((ptr->msg_id >= MSG_ID_BT_HID_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HID_GROUP_END) )
    {
        btmtk_hid_handle_message(NULL, ptr);
    }
#endif
#if defined(__BT_TESTING_PROFILE__)
    else if ((ptr->msg_id >= MSG_ID_BT_CHN_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_CHN_GROUP_END) )
    {
    		btmtk_chn_handle_message(ptr->msg_id, ptr->local_para_ptr);
    }
#endif

    else if ((ptr->msg_id >= MSG_ID_BT_BPP_GROUP_START) && (ptr->msg_id <= (MSG_ID_BT_BPP_GROUP_END)) )
    {
    		btmtk_bpp_handle_message(ptr);
    }
#if defined(__BT_PBAP_PROFILE__)
    else if ((ptr->msg_id >= MSG_ID_BT_PBAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PBAP_GROUP_END) )
    {
    		btmtk_pbap_handle_message(ptr);
    }
#endif
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
    else if ((ptr->msg_id >= MSG_ID_BT_MAP_GROUP_START) && (ptr->msg_id <= (MSG_ID_BT_MAP_GROUP_END)) )
    {
    		btmtk_map_handle_message(ptr);
    }
#endif
#if defined(__BT_SIM_PROFILE__)
    else if ((ptr->msg_id >= MSG_ID_BT_SIMAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_SIMAP_GROUP_END) )
    {
    		btmtk_simap_handle_message(ptr);
    }
#endif
#if defined(__BT_PAN_PROFILE__)
        else if ((ptr->msg_id >= MSG_ID_BT_PAN_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PAN_GROUP_END) )
        {
            btmtk_pan_handle_message(ptr);
        }
#endif

    free_ilm(ptr);
       
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_debug_wisebtlog
 * DESCRIPTION
 *  Dump MBT traces (similar to lg_oem_debug_wiselog)
 * PARAMETERS
 *  message         [IN]        
 * RETURNS
 *****************************************************************************/
int btmtk_debug_wisebtlog(int eKind, int nLineNo, char* pszFileName, char* pFmt, va_list varArgs)
{
	//LGOEM_NEEDTO_IMPLEMENT
	int retVal = 0;
//	Report ( ( "ENTER btmtk_debug_wisebtlog()" ) );
	retVal = ws_debug_btlog ( eKind, TRACE_INFO, nLineNo, pszFileName, pFmt, varArgs );
//	Report ( ( "EXIT btmtk_debug_wisebtlog()" ) );
	return retVal;
}

#else
void btmtk_assert(char* expr, char* file, kal_uint32 line)
{
   /* chop off dirname if present */
   char *f = strrchr(file, '\\');

   if (f == NULL)
      f = file;
   else
      ++f;

    printf("Assert fail: %s %d\n", f, line);
    printf("Expression: %s", expr);
//    OS_Report("Assert fail: %s %d\n", f, line);
//    OS_Report("Expression: %s", expr);
    assert(0);
}
#endif /* BTMTK_ON_WISE */

