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
 * bt_spp_hdl.c
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
 * Bingyi Chen
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
#include "bt_types.h"
#include "bt_mmi.h"
#include "bluetooth_struct.h"
#include "bt_spp_api.h"
#include "bluetooth_spp_message.h"

#ifdef BTMTK_ON_LINUX
#include "bt_ext_debug.h"
#endif


/* 
 * bt_spp_hdl.c
 * 
 * This file is the External Adaptation handler of SPP profile used by Application.
 * Flow direction: APP <-- external ADP handler
 */

 
 
/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Local function prototypes
 *
 ****************************************************************************/


/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
extern BTMTK_EventCallBack g_SPP_MMI_Callback;

#if 0
/*****************************************************************************
 * FUNCTION
 *  btmtk_spp_activation_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  param1      [IN]        
 *  param2      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_spp_activation_req(U8 param1, U8 param2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}
#endif


void btmtk_spp_handle_connect_ind(ilm_struct *message)
{
    btmtk_spp_connect_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_connect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_connect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    printf("SPP CONNECT_IND port=%d, cid=%d\n", msg->port, msg->cid);
    port = msg->port;
#ifndef BTMTK_ON_WISE /* SPP_PORTING */
    btmtk_spp_uart_open_req(port);
#endif  /* BTMTK_ON_WISE */
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_CONNECT_IND, msg, sizeof(btmtk_spp_connect_ind_struct));
}


void btmtk_spp_handle_connect_ind_req(ilm_struct *message)
{
    btmtk_spp_connect_ind_req_struct *msg;
    U32 lap;
    U8 uap;
    U16 nap;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_connect_ind_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_connect_ind_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    port = msg->port;
    lap = msg->lap;
    uap = msg->uap;
    nap = msg->nap;
    printf("SPP CONNECT_IND_REQ port=%d, lap=0x%06lx, uap=0x%02x, nap=0x%04x\n", port, lap, uap, nap);
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_CONNECT_IND_REQ, msg, sizeof(btmtk_spp_connect_ind_req_struct));
}


void btmtk_spp_handle_disconnect_ind(ilm_struct *message)
{
    btmtk_spp_disconnect_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_disconnect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_disconnect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    printf("SPP DISCONNECT_IND port=%d\n", msg->port);
    port = msg->port;
    btmtk_spp_uart_close_req(port);
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_DISCONNECT_IND, msg, sizeof(btmtk_spp_disconnect_ind_struct));
}


void btmtk_spp_handle_activate_cnf(ilm_struct *message)
{
	btmtk_spp_activate_cnf_struct *msg;
	U8 port;
	
#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_activate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_activate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    port = msg->port;
    printf("SPP ACTIVATE_CNF: port=%d, result=%d\n", msg->port, msg->result);
    if (TRUE == msg->result)
    {
#ifndef BTMTK_ON_WISE /* SPP_PORTING */
    	btmtk_spp_uart_open_req(port);
#endif  /* BTMTK_ON_WISE */        
#ifdef __SPP_SHARED_MEMORY__
    	// Register getMemPtr of this port
    	btmtk_spp_uart_assign_buffer_req(port);
#endif	/* __SPP_SHARED_MEMORY__ */
    }
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_ACTIVATE_CNF, msg, sizeof(btmtk_spp_activate_cnf_struct));
}


void btmtk_spp_handle_deactivate_cnf(ilm_struct *message)
{
	btmtk_spp_deactivate_cnf_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_deactivate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_deactivate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

#ifdef __SPP_SHARED_MEMORY__
    if (TRUE == msg->result)
    {
    	// De-register buffer of this port
    	btmtk_spp_uart_release_buffer_req(msg->port);
    }
#endif	/* __SPP_SHARED_MEMORY__ */

    printf("SPP DEACTIVATE_CNF: port=%d, result=%d\n", msg->port, msg->result);
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_DEACTIVATE_CNF, msg, sizeof(btmtk_spp_deactivate_cnf_struct));
}


void btmtk_spp_handle_disconnect_cnf(ilm_struct *message)
{
    btmtk_spp_disconnect_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_disconnect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_disconnect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    printf("SPP DISCONNECT_CNF port=%d, cid=%d\n", msg->port, msg->cid);
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_DISCONNECT_CNF, msg, sizeof(btmtk_spp_disconnect_cnf_struct));
}


void btmtk_spp_handle_auth_req(ilm_struct *message)
{
    btmtk_spp_auth_req_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_auth_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_auth_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("SPP AUTH_REQ port=%d\n", msg->port);
}


void btmtk_spp_handle_connect_cnf(ilm_struct *message)
{
    btmtk_spp_connect_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_connect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_connect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("SPP CONNECT_CNF port=%d, cid=%d, result=%d\n", msg->port, msg->cid, msg->result);

#ifndef BTMTK_ON_WISE /* SPP_PORTING */
    if (SPPC_CONNECT_SUCCESS == msg->result)
    {
    btmtk_spp_uart_open_req(msg->port);
    }
#endif  /* BTMTK_ON_WISE */            
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_CONNECT_CNF, msg, sizeof(btmtk_spp_connect_cnf_struct));
}


void btmtk_spp_handle_uart_owner_ind(ilm_struct *message)
{
    btmtk_spp_uart_owner_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_owner_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_owner_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    port = msg->port;
    if (SPP_PROFILE == msg->profile)
    {
	    printf("SPP UART_OWNER_IND port=%d\n", msg->port);
	    btmtk_spp_uart_owner_cnf(port);
    }
    else if (DUN_PROFILE == msg->profile)
    {
	    printf("DUN UART_OWNER_IND port=%d\n", msg->port);
	    btmtk_spp_uart_owner_cnf(port);
    }
    else if (FAX_PROFILE == msg->profile)
    {
	    printf("FAX UART_OWNER_IND port=%d\n", msg->port);
	    btmtk_spp_uart_owner_cnf(port);
    }
}


void btmtk_spp_handle_uart_plugout_ind(ilm_struct *message)
{
    btmtk_spp_uart_plugout_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_plugout_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_plugout_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    port = msg->port;
    if (SPP_PROFILE == msg->profile)
    {
	    printf("SPP UART_PLUGOUT_IND port=%d\n", msg->port);
	    btmtk_spp_uart_plugout_cnf(port);
    }
    else if (DUN_PROFILE == msg->profile)
    {
	    printf("DUN UART_PLUGOUT_IND port=%d\n", msg->port);
	    btmtk_spp_uart_plugout_cnf(port);
    }
    else if (FAX_PROFILE == msg->profile)
    {
	    printf("FAX UART_PLUGOUT_IND port=%d\n", msg->port);
	    btmtk_spp_uart_plugout_cnf(port);
    }
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_UART_PLUGOUT_IND, NULL, 0);
}


void btmtk_spp_handle_uart_ready_to_read_ind(ilm_struct *message)
{
    btmtk_spp_uart_ready_to_read_ind_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_ready_to_read_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_ready_to_read_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    if (SPP_PROFILE == msg->profile)
    {
	    printf("SPP UART_READY_TO_READ_IND port=%d\n", msg->port);
	    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_READY_TO_READ_IND, msg, sizeof(btmtk_spp_uart_ready_to_read_ind_struct));
    }
    else if (DUN_PROFILE == msg->profile)
    {
	    printf("DUN UART_READY_TO_READ_IND port=%d\n", msg->port);
	    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_READY_TO_READ_IND, msg, sizeof(btmtk_spp_uart_ready_to_read_ind_struct));
    }
    else if (FAX_PROFILE == msg->profile)
    {
	    printf("FAX UART_READY_TO_READ_IND port=%d\n", msg->port);
	    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_READY_TO_READ_IND, msg, sizeof(btmtk_spp_uart_ready_to_read_ind_struct));
    }
}


void btmtk_spp_handle_uart_ready_to_write_ind(ilm_struct *message)
{
    btmtk_spp_uart_ready_to_write_ind_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_ready_to_write_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_ready_to_write_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    if (SPP_PROFILE == msg->profile)
    {
	    printf("SPP UART_READY_TO_WRITE_IND port=%d\n", msg->port);
	    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_READY_TO_WRITE_IND, msg, sizeof(btmtk_spp_uart_ready_to_write_ind_struct));
    }
    else if (DUN_PROFILE == msg->profile)
    {
	    printf("DUN UART_READY_TO_WRITE_IND port=%d\n", msg->port);
	    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_READY_TO_WRITE_IND, msg, sizeof(btmtk_spp_uart_ready_to_write_ind_struct));
    }
    else if (FAX_PROFILE == msg->profile)
    {
	    printf("FAX UART_READY_TO_WRITE_IND port=%d\n", msg->port);
	    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_READY_TO_WRITE_IND, msg, sizeof(btmtk_spp_uart_ready_to_write_ind_struct));
    }
}


void btmtk_spp_handle_initialize_cnf(ilm_struct *message)
{
    printf("SPP INITIALIZE_CNF\n");
}


void btmtk_spp_handle_register_callback_cnf(ilm_struct *message)
{
    btmtk_spp_register_callback_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_register_callback_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_register_callback_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    if (SPP_PROFILE == msg->profile)
    {
	    printf("SPP REGISTER_CALLBACK_CNF port=%d\n", msg->port);    
    }
    else if (DUN_PROFILE == msg->profile)
    {
	    printf("DUN REGISTER_CALLBACK_CNF port=%d\n", msg->port);    
    }
    else if (FAX_PROFILE == msg->profile)
    {
	    printf("FAX REGISTER_CALLBACK_CNF port=%d\n", msg->port);    
    }
}


void btmtk_spp_handle_uart_open_cnf(ilm_struct *message)
{
    btmtk_spp_uart_open_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_open_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
   msg = (btmtk_spp_uart_open_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("SPP UART_OPEN_CNF port=%d, result=%d\n", msg->port, msg->result);
}


void btmtk_spp_handle_uart_close_cnf(ilm_struct *message)
{
    btmtk_spp_uart_close_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_close_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_close_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("SPP UART_CLOSE_CNF port=%d\n", msg->port);
}


void btmtk_spp_handle_uart_get_bytes_cnf(ilm_struct *message)
{
    btmtk_spp_uart_get_bytes_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_get_bytes_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_get_bytes_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  

#ifdef __SPP_SHARED_MEMORY__
    if (msg->buffer != btmtk_get_shared_get_buffer_pointer(msg->port))
    {
#ifdef BTMTK_ON_WIN32
	    printf("SPP UART_GET_BYTES_CNF port=%d, Shared_Memory pointer doesn't sync!!\n", msg->port);
#endif	/* BTMTK_ON_WIN32 */
    }
#endif	/* __SPP_SHARED_MEMORY__ */

    if (SPP_PROFILE == msg->profile)
    {
#ifdef BTMTK_ON_WIN32
	    printf("SPP UART_GET_BYTES_CNF port=%d, length=%d\n", msg->port, msg->length);
#endif	/* BTMTK_ON_WIN32 */
	    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_GET_BYTES_CNF, msg, sizeof(btmtk_spp_uart_get_bytes_cnf_struct));
    }
    else if (DUN_PROFILE == msg->profile)
    {
#ifdef BTMTK_ON_WIN32
	    printf("DUN UART_GET_BYTES_CNF port=%d, length=%d\n", msg->port, msg->length);
#endif	/* BTMTK_ON_WIN32 */
	    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_GET_BYTES_CNF, msg, sizeof(btmtk_spp_uart_get_bytes_cnf_struct));
    }
    else if (FAX_PROFILE == msg->profile)
    {
#ifdef BTMTK_ON_WIN32
	    printf("FAX UART_GET_BYTES_CNF port=%d, length=%d\n", msg->port, msg->length);
#endif	/* BTMTK_ON_WIN32 */
	    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_GET_BYTES_CNF, msg, sizeof(btmtk_spp_uart_get_bytes_cnf_struct));
    }
}


void btmtk_spp_handle_uart_put_bytes_cnf(ilm_struct *message)
{
    btmtk_spp_uart_put_bytes_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_put_bytes_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_put_bytes_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    if (SPP_PROFILE == msg->profile)
    {
#ifdef BTMTK_ON_WIN32
	    printf("SPP UART_PUT_BYTES_CNF port=%d, length=%d\n", msg->port, msg->length);
#endif	/* BTMTK_ON_WIN32 */
	    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_PUT_BYTES_CNF, msg, sizeof(btmtk_spp_uart_put_bytes_cnf_struct));
    }
    else if (DUN_PROFILE == msg->profile)
    {
#ifdef BTMTK_ON_WIN32
	    printf("DUN UART_PUT_BYTES_CNF port=%d, length=%d\n", msg->port, msg->length);
#endif	/* BTMTK_ON_WIN32 */
	    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_PUT_BYTES_CNF, msg, sizeof(btmtk_spp_uart_put_bytes_cnf_struct));
    }
    else if (FAX_PROFILE == msg->profile)
    {
#ifdef BTMTK_ON_WIN32
	    printf("FAX UART_PUT_BYTES_CNF port=%d, length=%d\n", msg->port, msg->length);
#endif	/* BTMTK_ON_WIN32 */
	    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_PUT_BYTES_CNF, msg, sizeof(btmtk_spp_uart_put_bytes_cnf_struct));
    }
}


void btmtk_spp_handle_uart_set_owner_cnf(ilm_struct *message)
{
    btmtk_spp_uart_set_owner_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_set_owner_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_set_owner_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("SPP UART_SET_OWNER_CNF port=%d\n", msg->port);
}


void btmtk_spp_handle_enable_cnf(ilm_struct *message)
{
	printf("SPP Enable_CNF.\n");
	g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_ENABLE_CNF, NULL, 0);
}


void btmtk_spp_handle_disable_cnf(ilm_struct *message)
{
	printf("SPP Disable_CNF.\n");
	g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_DISABLE_CNF, NULL, 0);
}


void btmtk_spp_handle_uart_data_available_ind(ilm_struct *message)
{
    btmtk_spp_uart_data_available_ind_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_data_available_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_data_available_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  

#ifdef __SPP_SHARED_MEMORY__
    if (msg->buffer != btmtk_get_shared_get_buffer_pointer(msg->port))
    {
#ifdef BTMTK_ON_WIN32
	    printf("SPP UART_DATA_AVAILABLE_IND port=%d, Shared_Memory pointer doesn't sync!!\n", msg->port);
#endif	/* BTMTK_ON_WIN32 */
    }
#endif	/* __SPP_SHARED_MEMORY__ */

    if (SPP_PROFILE == msg->profile)
    {
	    printf("SPP UART_DATA_AVAILABLE_IND port=%d, length=%d\n", msg->port, msg->length);
	    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_DATA_AVAILABLE_IND, msg, sizeof(btmtk_spp_uart_data_available_ind_struct));
    }
    else if (DUN_PROFILE == msg->profile)
    {
	    printf("DUN UART_DATA_AVAILABLE_IND port=%d, length=%d\n", msg->port, msg->length);
	    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_DATA_AVAILABLE_IND, msg, sizeof(btmtk_spp_uart_data_available_ind_struct));
    }
    else if (FAX_PROFILE == msg->profile)
    {
	    printf("FAX UART_DATA_AVAILABLE_IND port=%d, length=%d\n", msg->port, msg->length);
	    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_DATA_AVAILABLE_IND, msg, sizeof(btmtk_spp_uart_data_available_ind_struct));
    }
}


void btmtk_dun_handle_connect_ind(ilm_struct *message)
{
    btmtk_dun_connect_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_connect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_connect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("DUN CONNECT_IND port=%d, cid=%d\n", msg->port, msg->cid);
    port = msg->port;
#ifndef BTMTK_ON_WISE /* SPP_PORTING */
    btmtk_spp_uart_open_req(port);
#endif  /* BTMTK_ON_WISE */                
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_CONNECT_IND, msg, sizeof(btmtk_dun_connect_ind_struct));
}


void btmtk_dun_handle_connect_ind_req(ilm_struct *message)
{
    btmtk_dun_connect_ind_req_struct *msg;
    U32 lap;
    U8 uap;
    U16 nap;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_connect_ind_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_connect_ind_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */      
    port = msg->port;
    lap = msg->lap;
    uap = msg->uap;
    nap = msg->nap;
    printf("DUN CONNECT_IND_REQ port=%d, lap=0x%06lx, uap=0x%02x, nap=0x%04x\n", port, lap, uap, nap);
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_CONNECT_IND_REQ, msg, sizeof(btmtk_dun_connect_ind_req_struct));
}


void btmtk_dun_handle_disconnect_ind(ilm_struct *message)
{
    btmtk_dun_disconnect_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_disconnect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_disconnect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */      
    printf("DUN DISCONNECT_IND port=%d\n", msg->port);
    port = msg->port;
    btmtk_spp_uart_close_req(port);
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_DISCONNECT_IND, msg, sizeof(btmtk_dun_disconnect_ind_struct));
}


void btmtk_dun_handle_activate_cnf(ilm_struct *message)
{
	btmtk_dun_activate_cnf_struct *msg;
	U8 port;
	
#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_activate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_activate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    port = msg->port;

    printf("DUN ACTIVATE_CNF: port=%d, result=%d\n", msg->port, msg->result);
    if (TRUE == msg->result)
    {
    #ifndef BTMTK_ON_WISE /* SPP_PORTING */       
    	btmtk_spp_uart_open_req(port);
    #endif  /* BTMTK_ON_WISE */                        
#ifdef __SPP_SHARED_MEMORY__
    	// Register getMemPtr of this port
    	btmtk_spp_uart_assign_buffer_req(port);
#endif	/* __SPP_SHARED_MEMORY__ */
    }
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_ACTIVATE_CNF, msg, sizeof(btmtk_dun_activate_cnf_struct));
}


void btmtk_dun_handle_deactivate_cnf(ilm_struct *message)
{
	btmtk_dun_deactivate_cnf_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_deactivate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_deactivate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

#ifdef __SPP_SHARED_MEMORY__
    if (TRUE == msg->result)
    {
    	// De-register getMemPtr of this port
    	btmtk_spp_uart_release_buffer_req(msg->port);
    }
#endif	/* __SPP_SHARED_MEMORY__ */

    printf("DUN DEACTIVATE_CNF: port=%d, result=%d\n", msg->port, msg->result);
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_DEACTIVATE_CNF, msg, sizeof(btmtk_dun_deactivate_cnf_struct));
}


void btmtk_dun_handle_disconnect_cnf(ilm_struct *message)
{
    btmtk_dun_disconnect_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_disconnect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_disconnect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("DUN DISCONNECT_CNF port=%d, cid=%d\n", msg->port, msg->cid);
    btmtk_spp_uart_close_req(msg->port);
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_DISCONNECT_CNF, msg, sizeof(btmtk_dun_disconnect_cnf_struct));
}


void btmtk_dun_handle_auth_req(ilm_struct *message)
{
    btmtk_dun_auth_req_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_auth_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_auth_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("DUN AUTH_REQ port=%d\n", msg->port);
}


void btmtk_dun_handle_connect_cnf(ilm_struct *message)
{
    btmtk_dun_connect_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_dun_connect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_dun_connect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("DUN CONNECT_CNF port=%d, cid=%d, result=%d\n", msg->port, msg->cid, msg->result);
    g_SPP_MMI_Callback(NULL, EVENT_DUN_MMI_CONNECT_CNF, msg, sizeof(btmtk_dun_connect_cnf_struct));
}


void btmtk_fax_handle_connect_ind(ilm_struct *message)
{
    btmtk_fax_connect_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_connect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_connect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */  
    printf("FAX CONNECT_IND port=%d, cid=%d\n", msg->port, msg->cid);
    port = msg->port;
    btmtk_spp_uart_open_req(port);
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_CONNECT_IND, msg, sizeof(btmtk_fax_connect_ind_struct));
}


void btmtk_fax_handle_connect_ind_req(ilm_struct *message)
{
    btmtk_fax_connect_ind_req_struct *msg;
    U32 lap;
    U8 uap;
    U16 nap;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_connect_ind_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_connect_ind_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */      
    port = msg->port;
    lap = msg->lap;
    uap = msg->uap;
    nap = msg->nap;
    printf("FAX CONNECT_IND_REQ port=%d, lap=0x%06lx, uap=0x%02x, nap=0x%04x\n", port, lap, uap, nap);
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_CONNECT_IND_REQ, msg, sizeof(btmtk_fax_connect_ind_req_struct));
}

void btmtk_fax_handle_disconnect_ind(ilm_struct *message)
{
    btmtk_fax_disconnect_ind_struct *msg;
    U8 port;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_disconnect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_disconnect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */      
    printf("FAX DISCONNECT_IND port=%d\n", msg->port);
    port = msg->port;
    btmtk_spp_uart_close_req(port);
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_DISCONNECT_IND, msg, sizeof(btmtk_fax_disconnect_ind_struct));
}


void btmtk_fax_handle_activate_cnf(ilm_struct *message)
{
	btmtk_fax_activate_cnf_struct *msg;
	U8 port;
	
#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_activate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_activate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */
    port = msg->port;

    printf("FAX ACTIVATE_CNF: port=%d, result=%d\n", msg->port, msg->result);
    if (TRUE == msg->result)
    {
    	btmtk_spp_uart_open_req(port);
#ifdef __SPP_SHARED_MEMORY__
    	// Register getMemPtr of this port
    	btmtk_spp_uart_assign_buffer_req(port);
#endif	/* __SPP_SHARED_MEMORY__ */
    }
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_ACTIVATE_CNF, msg, sizeof(btmtk_fax_activate_cnf_struct));
}


void btmtk_fax_handle_deactivate_cnf(ilm_struct *message)
{
	btmtk_fax_deactivate_cnf_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_deactivate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_deactivate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

#ifdef __SPP_SHARED_MEMORY__
    if (TRUE == msg->result)
    {
    	// De-register getMemPtr of this port
    	btmtk_spp_uart_release_buffer_req(msg->port);
    }
#endif	/* __SPP_SHARED_MEMORY__ */

    printf("FAX DEACTIVATE_CNF: port=%d, result=%d\n", msg->port, msg->result);
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_DEACTIVATE_CNF, NULL, 0);
}


void btmtk_fax_handle_disconnect_cnf(ilm_struct *message)
{
    btmtk_fax_disconnect_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_disconnect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_disconnect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("FAX DISCONNECT_CNF port=%d, cid=%d\n", msg->port, msg->cid);
    btmtk_spp_uart_close_req(msg->port);
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_DISCONNECT_CNF, msg, sizeof(btmtk_fax_disconnect_cnf_struct));
}


void btmtk_fax_handle_auth_req(ilm_struct *message)
{
    btmtk_fax_auth_req_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_auth_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_auth_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("FAX AUTH_REQ port=%d\n", msg->port);
}


void btmtk_fax_handle_connect_cnf(ilm_struct *message)
{
    btmtk_fax_connect_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_fax_connect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_fax_connect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("FAX CONNECT_CNF port=%d, cid=%d, result=%d\n", msg->port, msg->cid, msg->result);
    g_SPP_MMI_Callback(NULL, EVENT_FAX_MMI_CONNECT_CNF, msg, sizeof(btmtk_fax_connect_cnf_struct));
}


#ifdef __SPP_SHARED_MEMORY__
void btmtk_spp_handle_uart_assign_buffer_cnf(ilm_struct *message)
{
    btmtk_spp_uart_assign_buffer_cnf_struct *msg;

#ifdef BTMTK_ON_WISE 
    msg = (btmtk_spp_uart_assign_buffer_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (btmtk_spp_uart_assign_buffer_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */          
    printf("SPP UART ASSIGN_BUFFER_CNF port=%d\n", msg->port);

#ifdef BTMTK_ON_LINUX
    bt_ext_log("[SPP][ADP] assign buffer cnf");
    g_SPP_MMI_Callback(NULL, EVENT_SPP_MMI_ASSIGN_BUFFER_CNF, msg, sizeof(btmtk_spp_uart_assign_buffer_cnf_struct));
#endif
}
#endif	/* __SPP_SHARED_MEMORY__ */


void btmtk_spp_handle_message(ilm_struct *message)
{
	switch (message->msg_id)
	{
		case MSG_ID_BT_SPP_CONNECT_IND:
			btmtk_spp_handle_connect_ind(message);
			break;
		case MSG_ID_BT_SPP_CONNECT_IND_REQ:
			btmtk_spp_handle_connect_ind_req(message);
			break;
		case MSG_ID_BT_SPP_DISCONNECT_IND:
			btmtk_spp_handle_disconnect_ind(message);
			break;
		case MSG_ID_BT_SPP_ACTIVATE_CNF:
			btmtk_spp_handle_activate_cnf(message);
			break;
		case MSG_ID_BT_SPP_DEACTIVATE_CNF:
			btmtk_spp_handle_deactivate_cnf(message);
			break;
		case MSG_ID_BT_SPP_DISCONNECT_CNF:
			btmtk_spp_handle_disconnect_cnf(message);
			break;
		case MSG_ID_BT_SPP_AUTH_REQ:
			btmtk_spp_handle_auth_req(message);
			break;
		case MSG_ID_BT_SPP_CONNECT_CNF:
			btmtk_spp_handle_connect_cnf(message);
			break;
		case MSG_ID_BT_SPP_UART_OWNER_IND:
			btmtk_spp_handle_uart_owner_ind(message);
			break;
		case MSG_ID_BT_SPP_UART_PLUGOUT_IND:
			btmtk_spp_handle_uart_plugout_ind(message);
			break;
		case MSG_ID_BT_SPP_UART_READY_TO_READ_IND:
			btmtk_spp_handle_uart_ready_to_read_ind(message);
			break;
		case MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND:
			btmtk_spp_handle_uart_ready_to_write_ind(message);
			break;
		case MSG_ID_BT_SPP_INITIALIZE_CNF:
			btmtk_spp_handle_initialize_cnf(message);
			break;
		case MSG_ID_BT_SPP_REGISTER_CALLBACK_CNF:
			btmtk_spp_handle_register_callback_cnf(message);
			break;
		case MSG_ID_BT_SPP_UART_OPEN_CNF:
			btmtk_spp_handle_uart_open_cnf(message);
			break;
		case MSG_ID_BT_SPP_UART_CLOSE_CNF:
			btmtk_spp_handle_uart_close_cnf(message);
			break;
		case MSG_ID_BT_SPP_UART_GET_BYTES_CNF:
			btmtk_spp_handle_uart_get_bytes_cnf(message);
			break;
		case MSG_ID_BT_SPP_UART_PUT_BYTES_CNF:
			btmtk_spp_handle_uart_put_bytes_cnf(message);
			break;
		case MSG_ID_BT_SPP_UART_SET_OWNER_CNF:
			btmtk_spp_handle_uart_set_owner_cnf(message);
		case MSG_ID_BT_SPP_UART_DATA_AVAILABLE_IND:
			btmtk_spp_handle_uart_data_available_ind(message);
			break;
		case MSG_ID_BT_SPP_ENABLE_CNF:
			btmtk_spp_handle_enable_cnf(message);
			break;
		case MSG_ID_BT_SPP_DISABLE_CNF:
			btmtk_spp_handle_disable_cnf(message);
			break;
		/* DUN  */			
		case MSG_ID_BT_DUN_CONNECT_IND:
			btmtk_dun_handle_connect_ind(message);
			break;
		case MSG_ID_BT_DUN_CONNECT_IND_REQ:
			btmtk_dun_handle_connect_ind_req(message);
			break;
		case MSG_ID_BT_DUN_DISCONNECT_IND:
			btmtk_dun_handle_disconnect_ind(message);
			break;
		case MSG_ID_BT_DUN_ACTIVATE_CNF:
			btmtk_dun_handle_activate_cnf(message);
			break;
		case MSG_ID_BT_DUN_DEACTIVATE_CNF:
			btmtk_dun_handle_deactivate_cnf(message);
			break;
		case MSG_ID_BT_DUN_DISCONNECT_CNF:
			btmtk_dun_handle_disconnect_cnf(message);
			break;
		case MSG_ID_BT_DUN_AUTH_REQ:
			btmtk_dun_handle_auth_req(message);
			break;
		case MSG_ID_BT_DUN_CONNECT_CNF:
			btmtk_dun_handle_connect_cnf(message);
			break;
		/* FAX  */			
		case MSG_ID_BT_FAX_CONNECT_IND:
			btmtk_fax_handle_connect_ind(message);
			break;
		case MSG_ID_BT_FAX_CONNECT_IND_REQ:
			btmtk_fax_handle_connect_ind_req(message);
			break;
		case MSG_ID_BT_FAX_DISCONNECT_IND:
			btmtk_fax_handle_disconnect_ind(message);
			break;
		case MSG_ID_BT_FAX_ACTIVATE_CNF:
			btmtk_fax_handle_activate_cnf(message);
			break;
		case MSG_ID_BT_FAX_DEACTIVATE_CNF:
			btmtk_fax_handle_deactivate_cnf(message);
			break;
		case MSG_ID_BT_FAX_DISCONNECT_CNF:
			btmtk_fax_handle_disconnect_cnf(message);
			break;
		case MSG_ID_BT_FAX_AUTH_REQ:
			btmtk_fax_handle_auth_req(message);
			break;
		case MSG_ID_BT_FAX_CONNECT_CNF:
			btmtk_fax_handle_connect_cnf(message);
#ifdef __SPP_SHARED_MEMORY__
		case MSG_ID_BT_UART_ASSIGN_BUFFER_CNF:
			btmtk_spp_handle_uart_assign_buffer_cnf(message);
			break;
#endif	/* __SPP_SHARED_MEMORY__ */
		default:
			break;
	}
}




