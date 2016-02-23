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
 * Bt_adp_bpp.c
 *
 * Project:
 * --------
 *  
 *
 * Description:
 * ------------
 *   This file is for BPP external adapter service.
 *
 * Author:
 * -------
 * Yufeng Chu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifdef __ON_MAUI__
#include "kal_release.h"        /* Basic data type */

#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h"         /* enum for message IDs */
#include "app_ltlcom.h" /* Task message communiction(ILM) */
#include "uart_sw.h"    /* UART port for bluetooth_struct */

#include "bluetooth_trc.h"      /* bt trace definition */
#include "bluetooth_struct.h"
#include "bt_dm_struct.h"       /* BT_OBEX_BPPS_PROFILE_UUID */
#include "bttypes.h"

#else

#include "bt_types.h"
#include "bt_common.h"
#include "bt_os_api.h"

#include "btbm_adp.h"       /* BT_OBEX_BPPS_PROFILE_UUID */
#include "bt_adp_msg.h"

#endif

#include "bluetooth_bpp_common.h"
#include "bluetooth_bpp_struct.h"


#include "bpp.h"

#include "bt_adp_bpp.h"

#ifndef __ON_MAUI__
#include "bluetooth_bpp_message.h"
#endif

/* UUID */

#ifndef BT_BASIC_PRINTING
#define BT_BASIC_PRINTING 0x1122
#endif

/* globle variable */
bt_bpp_adp_cntx g_bpp_adp_cntx;

/* external APIs */
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);


#define FUNCTION_IMPLEMENT

/* function implementation */

void bpp_adp_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY(BPP_ADP_ENTRY_INIT);

	/* clear memory */
	OS_MemSet((U8*)&g_bpp_adp_cntx, 0 , sizeof(g_bpp_adp_cntx));

	g_bpp_adp_cntx.cm_conn_id = (U8)BPP_INVALID_CMID;

	/* register callback */
	bpp_register_callback(bpp_adp_event_callback);
}

#define APP_MSG_HANDLER
/*****************************************************************************
 * FUNCTION
 *  bpp_adp_sp_msg_hdlr
 * DESCRIPTION
 *  This function handle the msg which come from bpp app(mmi)
 * PARAMETERS
 *  ilm_ptr   
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_adp_msg_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    msg_type msg_id = ilm_ptr->msg_id;
	BtStatus status = BT_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY2(BPP_ADP_ENTRY_MSG_HDLR, g_bpp_adp_cntx.cur_req, g_bpp_adp_cntx.printer_handle);

	/* process msg */
    switch (msg_id)
    {
        case MSG_ID_BT_BPP_CONNECT_REQ:
        {
			bt_bpp_connect_req_struct *conn_req = (bt_bpp_connect_req_struct*)ilm_ptr->local_para_ptr;

			if (g_bpp_adp_cntx.cur_req != BPP_REQ_NONE)
			{
				/* only support one session */
	            bpp_adp_connect_confirm(BPP_ERROR);
				
				return;
			}
			
			g_bpp_adp_cntx.printer_handle = conn_req->hprinter;
				
			OS_MemCopy((U8*)&(g_bpp_adp_cntx.bt_addr), (U8*)&(conn_req->addr), sizeof(conn_req->addr));
				
			status = bpp_connect_printer((BD_ADDR*)&(conn_req->addr), conn_req->hprinter);
			
	        if (status == BT_STATUS_PENDING)
	        {
	            g_bpp_adp_cntx.cur_req = BPP_REQ_CONNECT;
	        }
	        else
	        {
				/* failed: notify app */
	            bpp_adp_connect_confirm(BPP_ERROR);
	        }
        }
			
			/* just return */
            return;
			
        case MSG_ID_BT_BPP_GET_PRINTER_ATTR_REQ:
		{
		    bt_bpp_get_printer_attr_req_struct *get_attr_req = (bt_bpp_get_printer_attr_req_struct*)ilm_ptr->local_para_ptr;

			//BPP_ASSERT(g_bpp_adp_cntx.printer_handle == get_attr_req->hprinter);
			if (g_bpp_adp_cntx.cur_req != BPP_REQ_NONE)
			{
				/* failed: notify app */
	            bpp_adp_get_attr_confirm(BPP_ERROR, NULL);
			}
			
			status = bpp_get_printer_attribute(g_bpp_adp_cntx.printer_handle, get_attr_req->attr_bitmask);

	        if (status == BT_STATUS_PENDING)
	        {
	            g_bpp_adp_cntx.cur_req = BPP_REQ_GETPRINTERATTR;
	        }
			else
	        {
				/* failed: notify app */
	            bpp_adp_get_attr_confirm(BPP_ERROR, NULL);
	        }
    	}

			/* just return */
            return;
			
        case MSG_ID_BT_BPP_PRINT_DOC_REQ:
		{
			bt_bpp_print_doc_req_struct *print_req = (bt_bpp_print_doc_req_struct *)ilm_ptr->local_para_ptr;

			//BPP_ASSERT(g_bpp_adp_cntx.printer_handle == print_req->hprinter);
			if (g_bpp_adp_cntx.cur_req != BPP_REQ_NONE)
			{
				/* failed: notify app */
	            bpp_adp_print_doc_confirm(BPP_ERROR);
			}
			
			status = bpp_print_object(g_bpp_adp_cntx.printer_handle, &print_req->print_object);

	        if (status == BT_STATUS_PENDING)
	        {
	            g_bpp_adp_cntx.cur_req = BPP_REQ_PRINT_DOC;
	        }
			else
	        {
				/* failed: notify app */
	            bpp_adp_print_doc_confirm(BPP_ERROR);
	        }
    	}

			/* just return */
            return;
			
        case MSG_ID_BT_BPP_AUTH_RSP:
			{
				bt_bpp_auth_rsp_struct *auth_rsp = (bt_bpp_auth_rsp_struct *)ilm_ptr->local_para_ptr;

				OS_MemCopy((U8*)g_bpp_adp_cntx.auth_rsp.passwd, (U8*)auth_rsp->auth_resp.passwd, auth_rsp->auth_resp.passwd_len);
				g_bpp_adp_cntx.auth_rsp.passwd_len = auth_rsp->auth_resp.passwd_len;
				OS_MemCopy((U8*)g_bpp_adp_cntx.auth_rsp.userid, (U8*)auth_rsp->auth_resp.userid, auth_rsp->auth_resp.userid_len);
				g_bpp_adp_cntx.auth_rsp.userid_len= auth_rsp->auth_resp.userid_len;

				if (auth_rsp->auth_resp.cancel)
				{
					bpp_reject_authentication(g_bpp_adp_cntx.printer_handle);
				}
				else
				{
					bpp_accept_authentication(g_bpp_adp_cntx.printer_handle, &g_bpp_adp_cntx.auth_rsp);
				}
        	}

			/* just return */
			return;

		case MSG_ID_BT_BPP_PROGRESS_RSP:
			
			bpp_progress_rsp_received(g_bpp_adp_cntx.printer_handle);

			return;

        case MSG_ID_BT_BPP_CANCEL_REQ:
			
			if (g_bpp_adp_cntx.cur_req == BPP_REQ_GETPRINTERATTR ||
				g_bpp_adp_cntx.cur_req == BPP_REQ_PRINT_DOC)
			{
				g_bpp_adp_cntx.cur_req = BPP_REQ_CANCEL;
				
			    status = bpp_cancel(g_bpp_adp_cntx.printer_handle);
			}
			else
			{
				/* failed: notify app */
				status = BT_STATUS_FAILED;
			}

			if (status == BT_STATUS_SUCCESS)
			{
            	bpp_adp_cancel_confirm(BPP_SUCCESS);
			}
			else if (status != BT_STATUS_PENDING)
			{
            	bpp_adp_cancel_confirm(BPP_ERROR);
			}
			
			return;

		case MSG_ID_BT_BPP_DISCONNECT_REQ:
			{
			//bt_bpp_disconnect_req_struct *disconn_req = (bt_bpp_disconnect_req_struct*)ilm_ptr->local_para_ptr;

			//BPP_ASSERT(g_bpp_adp_cntx.printer_handle == disconn_req->hprinter);
			}
			
			status = bpp_disconnect_printer(g_bpp_adp_cntx.printer_handle);

	        if (status == BT_STATUS_PENDING)
	        {
	            g_bpp_adp_cntx.cur_req = BPP_REQ_DISCONNECT;
	        }
			else if (status == BT_STATUS_SUCCESS)
	        {
	            bpp_adp_disconnect_confirm(BPP_SUCCESS);
			}
			else
	        {
				/* failed: notify app */
	            bpp_adp_disconnect_confirm(BPP_ERROR);
	        }

			return;		

        default:
            break;
    }

}

#define PROFILE_EVENT_HANDLER

void bpp_adp_event_callback(BppCallbackParms *params)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY5(BPP_ADP_ENTRY_EVENT_CALLBACK,params->printer_handle, (kal_int32)params->event,(kal_int32)params->oper,
						g_bpp_adp_cntx.printer_handle,g_bpp_adp_cntx.cur_req);
	
	if (params->printer_handle != g_bpp_adp_cntx.printer_handle)
	{
		//ASSERT(0);
	}

	switch (params->event)
	{
		case BPP_EVENT_COMPLETE:
			switch (g_bpp_adp_cntx.cur_req)
			{
				case BPP_REQ_CONNECT:
					bpp_adp_connect_confirm(BPP_SUCCESS);
					break;

				case BPP_REQ_GETPRINTERATTR:
					bpp_adp_get_attr_confirm(BPP_SUCCESS, 
									&params->info.printer_attributes);
					break;

				case BPP_REQ_PRINT_DOC:
					bpp_adp_print_doc_confirm(BPP_SUCCESS);
					break;

				case BPP_REQ_CANCEL:
					bpp_adp_cancel_confirm(BPP_SUCCESS);
					break;
					
				default:
					break;
			}
			
			break;

		case BPP_EVENT_FAILED:
		case BPP_EVENT_USER_ABORTED:
			switch (g_bpp_adp_cntx.cur_req)
			{
				case BPP_REQ_CONNECT:
					bpp_adp_connect_confirm(BPP_ERROR);
					break;

				case BPP_REQ_GETPRINTERATTR:
					bpp_adp_get_attr_confirm(BPP_ERROR, NULL);
					break;

				case BPP_REQ_PRINT_DOC:
					bpp_adp_print_doc_confirm(BPP_ERROR);
					break;

				case BPP_REQ_DISCONNECT:
					bpp_adp_disconnect_confirm(BPP_ERROR);
					break;

				case BPP_REQ_CANCEL:
					bpp_adp_cancel_confirm(BPP_ERROR);
					break;

				default:
					break;
			}
			break;

		case BPP_EVENT_AUTHCHAL_IND:
			bpp_adp_auth_ind(&params->info.chal_info);
			break;

		case BPP_EVENT_PROGRESS_IND:
			bpp_adp_progress_ind(&params->info.prog_status);
			break;

		case BPP_EVENT_PRINTER_STATUS_IND:
			bpp_adp_printer_status_ind(&params->info.job_status);
			break;

		case BPP_EVENT_TP_DISCONNECTED:
			
			if (g_bpp_adp_cntx.cur_req == BPP_REQ_DISCONNECT)
			{
				bpp_adp_disconnect_confirm(BPP_SUCCESS);
			}
			else if (g_bpp_adp_cntx.cur_req == BPP_REQ_CONNECT)
			{
				bpp_adp_connect_confirm(BPP_ERROR);
			}
			else
			{
				bpp_adp_disconnect_ind();
			}
			break;

		default:
			break;
	}
}

#define UTIL_FUNC
/*****************************************************************************
 * FUNCTION
 *  bpp_adp_open_confirm
 * DESCRIPTION
 *  This function is to send open confirm msg to bpp app(MMI)
 * PARAMETERS
 *  printer         [?]         Address
 *  err_code        [IN]        Error code
 * RETURNS
 *****************************************************************************/
static void bpp_adp_connect_confirm(BPP_ERROR_CODE cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    bt_bpp_connect_cnf_struct *open_cnf =
        (bt_bpp_connect_cnf_struct*) construct_local_para(sizeof(bt_bpp_connect_cnf_struct), TD_CTRL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_CONNECT_CONFIRM,cnf_code);

    if (cnf_code == BPP_SUCCESS)
    {
        g_bpp_adp_cntx.cm_conn_id =  BTBMProfileAllocateConnIndex(BT_BASIC_PRINTING, g_bpp_adp_cntx.bt_addr);

        open_cnf->cm_conn_id = g_bpp_adp_cntx.cm_conn_id;

		BT_BPP_TRACE_INFO_ARG1(BPP_ADP_ALLOC_CM_CONNID, g_bpp_adp_cntx.cm_conn_id);
    }

    open_cnf->hprinter = g_bpp_adp_cntx.printer_handle;
    open_cnf->cnf_code = cnf_code;

	if (cnf_code != BPP_SUCCESS)
	{
		g_bpp_adp_cntx.printer_handle = BPP_INVALID_PRINTER_HANDLE;
	}
	
	g_bpp_adp_cntx.cur_req = BPP_REQ_NONE;
	
    bpp_adp_send_msg2app(MSG_ID_BT_BPP_CONNECT_CNF, (local_para_struct*) open_cnf);

}



/*****************************************************************************
 * FUNCTION
 *  bpp_adp_get_attr_confirm
 * DESCRIPTION
 *  This function send get attr confirm to bpp app (mmi)
 * PARAMETERS
 *  printer         [?]         [?]
 *  err_code        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_get_attr_confirm(BPP_ERROR_CODE err_code, bt_bpp_printer_attributes *attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_get_printer_attr_cnf_struct *get_attr_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_GET_ATTR_CONFIRM,err_code);

    get_attr_cnf =
        (bt_bpp_get_printer_attr_cnf_struct*) construct_local_para(sizeof(bt_bpp_get_printer_attr_cnf_struct), TD_CTRL);

    OS_MemSet((U8*)&get_attr_cnf->printer_attributes, 0, sizeof(get_attr_cnf->printer_attributes));
    get_attr_cnf->hprinter = g_bpp_adp_cntx.printer_handle;
    get_attr_cnf->cnf_code = err_code;
	
	if (attr != NULL)
	{
		get_attr_cnf->printer_attributes.printer_state = attr->printer_state;
		get_attr_cnf->printer_attributes.state_reason = attr->state_reason;
	}
	
	if (err_code == BPP_SUCCESS)
	{		
		OS_MemCopy((U8*)&get_attr_cnf->printer_attributes.capability, (U8*)&attr->capability, sizeof(bt_bpp_printer_capability));
	}

	if (g_bpp_adp_cntx.cur_req == BPP_REQ_GETPRINTERATTR)
    {
		g_bpp_adp_cntx.cur_req = BPP_REQ_NONE;
    }

    bpp_adp_send_msg2app(MSG_ID_BT_BPP_GET_PRINTER_ATTR_CNF, (local_para_struct*) get_attr_cnf);
	
}

static void bpp_adp_auth_ind(bt_bpp_obex_auth_chal_info * chal_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_auth_ind_struct *auth_ind =
        (bt_bpp_auth_ind_struct*) construct_local_para(sizeof(bt_bpp_auth_ind_struct), TD_CTRL);
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    auth_ind->hprinter = g_bpp_adp_cntx.printer_handle;
	OS_MemCopy((U8*)&(auth_ind->chal_info), (U8*)chal_info, sizeof(auth_ind->chal_info));

    bpp_adp_send_msg2app(MSG_ID_BT_BPP_AUTH_IND, (local_para_struct*) auth_ind);
}

/*****************************************************************************
 * FUNCTION
 *  bpp_adp_status_ind
 * DESCRIPTION
 *  This function send printer status msg to bpp app(mmi)
 * PARAMETERS
 *  printer         [?]     [?]
 *  prt_status      [?]     
 *  status_ind      [?](?)
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_status_ind(bt_bpp_job_status *prt_status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO(BPP_ADP_STATUS_IND);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (prt_status)
    {
        bt_bpp_job_status_ind_struct *status_ind =
            (bt_bpp_job_status_ind_struct*) construct_local_para(sizeof(bt_bpp_job_status_ind_struct), TD_CTRL);

		status_ind->hprinter = g_bpp_adp_cntx.printer_handle;
        status_ind->job_status.printer_state = prt_status->printer_state;
        status_ind->job_status.job_state = prt_status->job_state;
        status_ind->job_status.state_reason = prt_status->state_reason;
		
        bpp_adp_send_msg2app(MSG_ID_BT_BPP_JOB_STATUS_IND, (local_para_struct*) status_ind);
    }
}

static void bpp_adp_printer_status_ind(bt_bpp_job_status *prt_status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO(BPP_ADP_STATUS_IND);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch (prt_status->job_state)
    {
        case BPP_JOB_STATE_WAITING:
        case BPP_JOB_STATE_STOPPED:
            bpp_adp_status_ind(prt_status);
            break;

        case BPP_JOB_STATE_COMPLETED:
            bpp_adp_print_doc_confirm(BPP_SUCCESS);
            break;

        case BPP_JOB_STATE_ABORTED:
        case BPP_JOB_STATE_CANCELED:
            bpp_adp_print_doc_confirm(BPP_ERROR_USER_ABORT);
            break;

        case BPP_JOB_STATE_PRINTING:
        default:
            break;
    }

}

/*****************************************************************************
 * FUNCTION
 *  bpp_adp_progress_ind
 * DESCRIPTION
 *  This function send progress to bpp app(mmi)
 * PARAMETERS
 *  printer     [?]     [?]
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_progress_ind(bt_bpp_progress_status *prog_status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //float precent = 0.0;
    bt_bpp_progress_ind_struct *progress_ind =
        (bt_bpp_progress_ind_struct*) construct_local_para(sizeof(bt_bpp_progress_ind_struct), TD_CTRL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    progress_ind->hprinter = g_bpp_adp_cntx.printer_handle;

	progress_ind->progress_status.sent_data_len = prog_status->sent_data_len;
	progress_ind->progress_status.total_data_len = prog_status->total_data_len;


    bpp_adp_send_msg2app(MSG_ID_BT_BPP_PROGRESS_IND, (local_para_struct*) progress_ind);

}



/*****************************************************************************
 * FUNCTION
 *  bpp_adp_print_doc_confirm
 * DESCRIPTION
 *  This function send print doc result to bpp app(mmi)
 * PARAMETERS
 *  ph              [IN]        
 *  err_code        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_print_doc_confirm(BPP_ERROR_CODE err_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_print_doc_cnf_struct *print_cnf =
        (bt_bpp_print_doc_cnf_struct*) construct_local_para(sizeof(bt_bpp_print_doc_cnf_struct), TD_CTRL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_PRINT_DOC_CONFIRM, err_code);

    print_cnf->hprinter = g_bpp_adp_cntx.printer_handle;
    print_cnf->cnf_code = err_code;

	if (g_bpp_adp_cntx.cur_req == BPP_REQ_PRINT_DOC)
    {
		g_bpp_adp_cntx.cur_req = BPP_REQ_NONE;
    }
	
    bpp_adp_send_msg2app(MSG_ID_BT_BPP_PRINT_DOC_CNF, (local_para_struct*) print_cnf);

}


/*****************************************************************************
 * FUNCTION
 *  bpp_adp_cancel_confirm
 * DESCRIPTION
 *  This function is send close confirm msg to bpp app
 * PARAMETERS
 *  printer         [?]         [?]
 *  err_code        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_cancel_confirm(BPP_ERROR_CODE err_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_cancel_cnf_struct *cancel_cnf =
        (bt_bpp_cancel_cnf_struct*) construct_local_para(sizeof(bt_bpp_cancel_cnf_struct), TD_CTRL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_CANCEL_CONFIRM, err_code);

    cancel_cnf->hprinter = g_bpp_adp_cntx.printer_handle;
    cancel_cnf->cnf_code = err_code;
    		
	if (err_code == BPP_SUCCESS)
	{
		g_bpp_adp_cntx.cur_req = BPP_REQ_NONE;
	}

    bpp_adp_send_msg2app(MSG_ID_BT_BPP_CANCEL_CNF, (local_para_struct*) cancel_cnf);
    
}


/*****************************************************************************
 * FUNCTION
 *  bpp_adp_disconnect_confirm
 * DESCRIPTION
 *  This function is send close confirm msg to bpp app
 * PARAMETERS
 *  printer         [?]         [?]
 *  err_code        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_disconnect_confirm(BPP_ERROR_CODE err_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_disconnect_cnf_struct *close_cnf =
        (bt_bpp_disconnect_cnf_struct*) construct_local_para(sizeof(bt_bpp_disconnect_cnf_struct), TD_CTRL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_DISC_CONFIRM, err_code);

	if (err_code == BPP_SUCCESS)
	{
	    if(g_bpp_adp_cntx.cm_conn_id != BPP_INVALID_CMID)
	    {
			BT_BPP_TRACE_INFO_ARG1(BPP_ADP_RELEASE_CM_CONNID, g_bpp_adp_cntx.cm_conn_id);

			BTBMProfileReleaseConnIndex(g_bpp_adp_cntx.cm_conn_id);

			g_bpp_adp_cntx.cm_conn_id = (U8)BPP_INVALID_CMID;
	    }
	}

    close_cnf->hprinter = g_bpp_adp_cntx.printer_handle;
    close_cnf->cnf_code = err_code;
    		
	if (err_code == BPP_SUCCESS)
	{
		g_bpp_adp_cntx.printer_handle = BPP_INVALID_PRINTER_HANDLE;
		g_bpp_adp_cntx.cur_req = BPP_REQ_NONE;
	}

    bpp_adp_send_msg2app(MSG_ID_BT_BPP_DISCONNECT_CNF, (local_para_struct*) close_cnf);
    
}

static void bpp_adp_disconnect_ind(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_disconnect_ind_struct *disc_ind;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO(BPP_ADP_DISC_IND);

    if(g_bpp_adp_cntx.cm_conn_id != BPP_INVALID_CMID)
    {
		BT_BPP_TRACE_INFO_ARG1(BPP_ADP_RELEASE_CM_CONNID, g_bpp_adp_cntx.cm_conn_id);

		BTBMProfileReleaseConnIndex(g_bpp_adp_cntx.cm_conn_id);

		g_bpp_adp_cntx.cm_conn_id = (U8)BPP_INVALID_CMID;
    }

	if (g_bpp_adp_cntx.cur_req == BPP_REQ_CANCEL)
	{
		bpp_adp_cancel_confirm(BPP_SUCCESS);
	}
	
	disc_ind =
        (bt_bpp_disconnect_ind_struct*) construct_local_para(sizeof(bt_bpp_disconnect_ind_struct), TD_CTRL);

    disc_ind->hprinter = g_bpp_adp_cntx.printer_handle;
	//disc_ind->cm_conn_id = g_bpp_adp_cntx.cm_conn_id;

	g_bpp_adp_cntx.printer_handle = BPP_INVALID_PRINTER_HANDLE;
	g_bpp_adp_cntx.cur_req = BPP_REQ_NONE;
		
    bpp_adp_send_msg2app(MSG_ID_BT_BPP_DISCONNECT_IND, (local_para_struct*) disc_ind);

}


#define SEND_MSG

/*****************************************************************************
 * FUNCTION
 *  bpp_adp_send_msg2app
 * DESCRIPTION
 *  This function send msg to bpp app(mmi)
 * PARAMETERS
 *  msg_id      [IN]        
 *  wparam      [?]         [?]
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_adp_send_msg2app(kal_uint16 msg_id, local_para_struct *wparam)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifdef __ON_MAUI__
    
    ilm_struct *ilm_send = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_SEND_MSG2APP, msg_id);

    ilm_send = allocate_ilm(MOD_BT);
    ilm_send->src_mod_id = MOD_BT;
    ilm_send->dest_mod_id = MOD_MMI;
    ilm_send->sap_id = BT_BPP_SAP;
    ilm_send->msg_id = (kal_uint16) msg_id;
    ilm_send->local_para_ptr = (local_para_struct*) wparam;
    ilm_send->peer_buff_ptr = NULL;
	
    msg_send_ext_queue(ilm_send);

#else

	U16 msg_len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_ADP_SEND_MSG2APP, msg_id);
    
    msg_len = wparam ? ((local_para_struct*)wparam)->msg_len : 0;
	
    BT_SendMessage(msg_id, MOD_MMI, wparam, msg_len);

#endif
}



