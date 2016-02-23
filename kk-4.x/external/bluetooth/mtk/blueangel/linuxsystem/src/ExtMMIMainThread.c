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
 * ExtMMIMainThread.c
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
#include <stdio.h>
#include <windows.h> /* For the CreateThread prototype */
#include <assert.h>

#include "bt_common.h"
#include "bt_ext_system.h"
#include "bluetooth_gap_struct.h"

HANDLE hBTCmdThread; 

DWORD WINAPI BTCMD_Script( LPVOID lpParam ) 
{ 
    ilm_struct *ptr = 0;
    printf("BT wise command\n");
#ifdef __WIN_PIPE__
    BTCreatePipeClient(2);
#endif

    while(1)
    {
#ifdef __WIN_QUEUE__    
        if(ptr == 0x00)
            SuspendThread(hBTCmdThread);
        ptr = (ilm_struct *) win_deQueue_message(MOD_EXTERNAL);
        if( NULL != ptr ){
             ptr->local_para_ptr = (void *)ptr->ilm_data;
        }
#endif        
#ifdef __WIN_PIPE__
        ptr = (ilm_struct *)win_dePipe_message_ap();
#endif
#ifdef __WIN_SHARED_MEMORY__
        if(ptr == 0x00)
            Sleep(100);
        ptr = (ilm_struct *)win_ap_receive_message();
#endif

        if(ptr !=0)
        {
            if((ptr->msg_id >= MSG_ID_BT_GAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GAP_GROUP_END))
            {
                btmtk_gap_handle_message(ptr);
            }
#if defined(__BT_HFG_PROFILE__)
            else if((ptr->msg_id >= MSG_ID_BT_HFG_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HFG_GROUP_END) )
            {
                btmtk_hfg_handle_message(ptr->msg_id, ptr->ilm_data);
            }
#endif
#ifdef __BT_SPP_PROFILE__
            else if ((ptr->msg_id >= MSG_ID_BT_SPP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_SPP_GROUP_END ) )
            {
            		btmtk_spp_handle_message(ptr);
            }
#endif
#if defined(__BT_GOEP_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_GOEPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GOEPC_GROUP_END) )
            {
            		btmtk_oppc_handle_message(ptr);
            }
            else if ((ptr->msg_id >= MSG_ID_BT_GOEPS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_GOEPS_GROUP_END) )
            {
            		btmtk_opps_handle_message(ptr);
            }
#if defined(__BT_OPP_PROFILE__)            
            else if ((ptr->msg_id >= MSG_ID_BT_OPPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_OPPC_GROUP_END) )
            {
            		btmtk_oppc_handle_message(ptr);
            }
            else if ((ptr->msg_id >= MSG_ID_BT_OPPS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_OPPS_GROUP_END) )
            {
            		btmtk_opps_handle_message(ptr);
            }
#endif            
#if defined(__BT_FTC_PROFILE__)            
            else if ((ptr->msg_id >= MSG_ID_BT_FTPC_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_FTPC_GROUP_END) )
            {
            		btmtk_ftpc_handle_message(ptr);
            }
#endif            
#if defined(__BT_FTS_PROFILE__)            
            else if ((ptr->msg_id >= MSG_ID_BT_FTPS_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_FTPS_GROUP_END) )
            {
            		btmtk_ftps_handle_message(ptr);
            }
#endif            

#if defined __BT_BIPI_PROFILE__ || defined __BT_BIPR_PROFILE__
	        else if ((ptr->msg_id >= MSG_ID_BT_BIPI_GROUP_START) && (ptr->msg_id <= (MSG_ID_BT_BIPR_GROUP_END)) )
	        {
		        btmtk_bip_handle_message(ptr);
	        }
#endif
            
#if defined(__BT_BPP_PROFILE__)            
            else if ((ptr->msg_id >= MSG_ID_BT_BPP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_BPP_GROUP_END) )
            {
            		btmtk_bpp_handle_message(ptr);
            }
#endif            
#endif //#if defined(__BT_GOEP_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_A2DP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_A2DP_GROUP_END) )
            {
            		btmtk_a2dp_handle_message(ptr);
            }
            else if ((ptr->msg_id >= MSG_ID_BT_AVRCP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_AVRCP_GROUP_END) )
            {
            		btmtk_avrcp_handle_message(ptr);
            }
#if defined(__BT_TESTING_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_CHN_GROUP_START) && (ptr->msg_id <=MSG_ID_BT_CHN_GROUP_END) )
            {
            		btmtk_chn_handle_message(ptr->msg_id, ptr->ilm_data);
            }
#endif
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_HID_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_HID_GROUP_END) )
   	   {
    			btmtk_hid_handle_message(NULL, ptr);
   	   }
#endif
            else if (ptr->msg_id == MSG_ID_BT_SDPDB_REGISTER_CNF ||
                ptr->msg_id == MSG_ID_BT_SDPDB_DEREGISTER_CNF)
            {
#ifdef __BT_BIPR_PROFILE__
                if (((bt_sdpdb_register_cnf_struct*)ptr->local_para_ptr)->uuid == 0x111B)
                {
                    btmtk_bip_handle_message(ptr);
                }
#endif
            }
#if defined(__BT_PBAP_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_PBAP_GROUP_START) && (ptr->msg_id <= MSG_ID_BT_PBAP_GROUP_END) )
            {
            		btmtk_pbap_handle_message(ptr);
            }
#endif
#ifdef __BT_SIM_PROFILE__
            else if ((ptr->msg_id >= MSG_ID_BT_SIMAP_GROUP_START) && (ptr->msg_id <= (MSG_ID_BT_SIMAP_GROUP_END)) )
            {
            		btmtk_simap_handle_message(ptr);
            }
#endif
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
            else if ((ptr->msg_id >= MSG_ID_BT_MAP_GROUP_START) && (ptr->msg_id <= (MSG_ID_BT_MAP_GROUP_END)) )
            {
            		btmtk_map_handle_message(ptr);
            }
#endif
#if defined(__BT_JSR82__) || defined(__BT_JSR82_L2RF__)
            else if ((ptr->msg_id >= MSG_ID_BT_JSR82_GROUP_START) && (ptr->msg_id <= (MSG_ID_BT_JSR82_GROUP_END)) )
            {
            		btmtk_jsr82_handle_message(ptr);
            }
#endif
            free_ilm(ptr);
               
		}
	}
	return 0;
} 

void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
    ilm_struct ilm;
    U16 *field;

    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.src_mod_id = MOD_EXTERNAL;
    ilm.dest_mod_id = dest_mod;

    assert (size <= MAX_ILM_BUFFER_SIZE);

    if(ptr != NULL)
        memcpy(ilm.ilm_data, ptr, size);

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm.ilm_data;
    *field = 1;
    field = (U16 *)&ilm.ilm_data[2];
    *field = size;
    
    bt_log_primitive(&ilm);
#ifdef __WIN_QUEUE__    
    win_enQueue_message(dest_mod, (void *)&ilm);
#endif
#ifdef __WIN_PIPE__    
    win_enPipe_message_from_ap((void *)&ilm, sizeof(ilm));
#endif
#ifdef __WIN_SHARED_MEMORY__    
    win_send_message_from_ap((void *)&ilm, sizeof(ilm));
#endif
}

