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
 * Bt_bpp_hdl.c
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
 * Tina Shen
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
#include "stdio.h"
#include "string.h"
#include "stdarg.h"


#include "bt_mmi.h"
#ifdef BTMTK_ON_LINUX
#include "bt_bpp_porting.h"
#else
#include "bt_bpp_api.h"
#endif
#include "bluetooth_bpp_struct.h"

#include "xatypes.h"

#ifdef BTMTK_ON_WISE
#include "bt_debug.h"
#endif


extern void bt_bpp_connect_cnf(BPP_ERROR_CODE bpp_conn_result);
extern void bt_bpp_getprinterattribute_cnf(BPP_ERROR_CODE bpp_getatt_result, bt_bpp_printer_attributes* p_attr);
extern void bt_bpp_print_cnf(BPP_ERROR_CODE bpp_print_result);
extern void bt_bpp_disconnect_cnf(BPP_ERROR_CODE bpp_disc_result);
extern void bt_bpp_disconnect_ind(void);

extern void bt_bpp_authentication_ind(bt_bpp_obex_auth_chal_info *chal_info);
extern void bt_bpp_print_progress_ind(U32 sent_len, U32 total_len);
extern void bt_bpp_job_status_ind(bt_bpp_job_status * job_sts);


void btmtk_bpp_handle_connect_cnf(ilm_struct *message);
void btmtk_bpp_handle_getprinterattribute_cnf(ilm_struct *message);
void btmtk_bpp_handle_print_cnf(ilm_struct *message);
void btmtk_bpp_handle_auth_ind(ilm_struct *message);
void btmtk_bpp_handle_progress_ind(ilm_struct *message);
void btmtk_bpp_handle_job_status_ind(ilm_struct *message);
void btmtk_bpp_handle_disconnect_cnf(ilm_struct *message);
void btmtk_bpp_handle_disconnect_ind(ilm_struct *message);


void btmtk_bpp_handle_message(ilm_struct *message)
{
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_HDLE_MSG, message->msg_id);
	
    switch(message->msg_id)
    {
        case MSG_ID_BT_BPP_CONNECT_CNF:
            btmtk_bpp_handle_connect_cnf(message);
            break;
			
        case MSG_ID_BT_BPP_GET_PRINTER_ATTR_CNF:
            btmtk_bpp_handle_getprinterattribute_cnf(message);
            break;
			
        case MSG_ID_BT_BPP_PRINT_DOC_CNF:
            btmtk_bpp_handle_print_cnf(message);
            break;
			
        case MSG_ID_BT_BPP_AUTH_IND:
            btmtk_bpp_handle_auth_ind(message);
            break;
			
        case MSG_ID_BT_BPP_PROGRESS_IND:
            btmtk_bpp_handle_progress_ind(message);
            break;
			
        case MSG_ID_BT_BPP_JOB_STATUS_IND:
            btmtk_bpp_handle_job_status_ind(message);
            break;
			
        case MSG_ID_BT_BPP_DISCONNECT_CNF:
            btmtk_bpp_handle_disconnect_cnf(message);
            break;
			
        case MSG_ID_BT_BPP_DISCONNECT_IND:
            btmtk_bpp_handle_disconnect_ind(message);
            break;
			
        default:
            return;
    }

}

void btmtk_bpp_handle_connect_cnf(ilm_struct *message)
{
    bt_bpp_connect_cnf_struct *msg;
    
#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_connect_cnf_struct *)message->ilm_data;
#else
    msg = (bt_bpp_connect_cnf_struct *)message->local_para_ptr;
#endif
    
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_CONNECT_CNF, msg->hprinter, msg->cnf_code);    

    bt_bpp_connect_cnf((BPP_ERROR_CODE)msg->cnf_code);    
}

void btmtk_bpp_handle_getprinterattribute_cnf(ilm_struct *message)
{
    bt_bpp_get_printer_attr_cnf_struct *msg;     
    
#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_get_printer_attr_cnf_struct *)message->ilm_data;
#else
    msg = (bt_bpp_get_printer_attr_cnf_struct *)message->local_para_ptr;
#endif


    bt_bpp_getprinterattribute_cnf((BPP_ERROR_CODE)msg->cnf_code, &(msg->printer_attributes));

}

void btmtk_bpp_handle_print_cnf(ilm_struct *message)
{
    bt_bpp_print_doc_cnf_struct *msg;
    
#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_print_doc_cnf_struct *)message->ilm_data;
#else
    msg = (bt_bpp_print_doc_cnf_struct *)message->local_para_ptr;
#endif

    bt_bpp_print_cnf((BPP_ERROR_CODE)msg->cnf_code);
}

void btmtk_bpp_handle_auth_ind(ilm_struct *message)
{
    bt_bpp_auth_ind_struct *msg;
#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_auth_ind_struct *)message->ilm_data;
#else
    msg = (bt_bpp_auth_ind_struct *)message->local_para_ptr;
#endif

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_AUTH_IND, msg->hprinter);

	bt_bpp_authentication_ind(&msg->chal_info);
}

void btmtk_bpp_handle_progress_ind(ilm_struct *message)
{
    bt_bpp_progress_ind_struct *msg;

#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_progress_ind_struct *)message->ilm_data;
#else
    msg = (bt_bpp_progress_ind_struct *)message->local_para_ptr;
#endif

    bt_trace(TRACE_FUNC, BT_BPP_EXT_PROGRESS_IND, msg->hprinter, 
    	msg->progress_status.sent_data_len, msg->progress_status.total_data_len);


	bt_bpp_print_progress_ind(msg->progress_status.sent_data_len,
						msg->progress_status.total_data_len);
	
}

void btmtk_bpp_handle_job_status_ind(ilm_struct *message)
{
    bt_bpp_job_status_ind_struct *msg;

#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_job_status_ind_struct *)message->ilm_data;
#else
    msg = (bt_bpp_job_status_ind_struct *)message->local_para_ptr;
#endif
    
    bt_trace(TRACE_FUNC, BT_BPP_EXT_JOB_STATUS_IND, msg->hprinter, 
    	msg->job_status.job_state, msg->job_status.printer_state, msg->job_status.state_reason);


	bt_bpp_job_status_ind(&(msg->job_status));
}

void btmtk_bpp_handle_disconnect_cnf(ilm_struct *message)
{
    bt_bpp_disconnect_cnf_struct *msg;

#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_disconnect_cnf_struct *)message->ilm_data;
#else
    msg = (bt_bpp_disconnect_cnf_struct *)message->local_para_ptr;
#endif

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISCONNECT_CNF, msg->hprinter, msg->cnf_code);    

    bt_bpp_disconnect_cnf((BPP_ERROR_CODE)msg->cnf_code);    
}

void btmtk_bpp_handle_disconnect_ind(ilm_struct *message)
{
    bt_bpp_disconnect_ind_struct *msg;
    
#ifndef BTMTK_ON_WISE 
    msg = (bt_bpp_disconnect_ind_struct *)message->ilm_data;
#else
    msg = (bt_bpp_disconnect_ind_struct *)message->local_para_ptr;
#endif

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISCONNECT_IND, msg->hprinter);    

    bt_bpp_disconnect_ind();
}


