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
 *   bpp.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for BPP session management.
 *
 * Author:
 * -------
 *   Dexiong Xu, revised by Yufeng Chu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * 09 01 2010 sh.lai
 * NULL
 * Integration change into 1036OF.
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_BPP_PROFILE__

#include "osapi.h"

#ifdef __ON_MAUI__
#include "fat_fs.h"     /* FS */
#else
#include "bt_adp_fs.h"  /* FS APIs */
#include "bt_adp_fs_ucs2.h"  /* FS APIs */
#include "bt_osal.h"  /* str APIs */

#define  FHANDLE kal_int32 /* used in xml_def.h */
#endif

#include "xml_def.h"    /* XML_PARSER */

#ifdef    __DRM_SUPPORT__  
#include "drm_def.h"
#include "Drm_gprot.h"
#endif


#include "bttypes.h"

#include "bluetooth_trc.h"

#ifdef __ON_MAUI__
#include "bluetooth_bm_struct.h"        /* CONVERT_BDADDR2ARRAY */
#endif

#include "btbm_adp.h"   /* CONVERT_BDADDR2ARRAY */

#include "goep.h"


#include "bluetooth_bpp_common.h"

#include "bpp.h"
#include "bppi.h"


static const U8 BtBaseUUID[] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};

static const ObStoreFuncTable bpp_obsfunc_table = 
{
    bpp_obs_read_doc,
    bpp_obs_write_soaprsp,
    bpp_obs_get_objlen,
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
#error bpp not support
#endif 
#if OBEX_SESSION_SUPPORT == XA_ENABLED
#error bpp not support
#endif 
};/*Only used for the GET response for the OBEX*/

/* global variable */
bpp_session_context g_bpp_context;

/* macro defination */
#define BPP_GET_PRT_FROM_JOBCHN(goep_client_app)   ((bpp_printer_struct*)((char*)goep_client_app - ((U32)&((bpp_printer_struct*) 0)->job_channel)))
#define BPP_GET_STS_CHANNEL(goep_client_app)   ((bpp_status_channel_struct*)((char*)goep_client_app - ((U32)&((bpp_status_channel_struct*) 0)->channel)))

#define BPP_IS_RSP_STSCODE_SUCCESS(sts_idx) 	((sts_idx) < 3)


#define BPP_STATIC_FUNC_DECLARARTION


void bpp_job_channel_callback(GoepClientEvent *event);

static BtStatus bpp_obex_connect(GoepClientApp *client, 
										bpp_channel_num channel, 
										bt_bpp_obex_auth_resp *pauth);

static BtStatus bpp_simple_push_print(bpp_printer_struct *printer, bt_bpp_object *print_object);

static void bpp_init_goep_object(GoepObjectReq *pobjreq, bpp_oper_context_struct *pcontext);

static BtStatus bpp_disc_job_channel(bpp_printer_struct *printer, bpp_disc_type_enum disc_type);

static BtStatus bpp_connect_status_channel(bpp_printer_struct *printer);

static void bpp_disc_status_channel(bpp_printer_struct *printer);

void bpp_status_channel_callback(GoepClientEvent *event);

static void bpp_destory_status_channel(bpp_status_channel_struct *sts_chnl);

static BtStatus bpp_create_job(bpp_printer_struct *printer, bt_bpp_object *print_object);

static BtStatus bpp_pull(GoepClientApp * GoepClient, bpp_oper_context_struct *oper_context);

static kal_bool bpp_create_job_end(bpp_printer_struct *printer);

static kal_bool bpp_get_printer_attr_end(bpp_printer_struct *printer,
												 bt_bpp_printer_attributes * prtr_attr);

static kal_bool bpp_get_event(bpp_printer_struct *printer);

static BtStatus bpp_send_doc(bpp_printer_struct *printer);

static kal_bool bpp_get_job_status(bpp_printer_struct *printer, bt_bpp_job_status *prt_status);
static void bpp_clear_job_status_data(bpp_printer_struct *printer);

static bpp_printer_struct *bpp_new_printer(PRINTER_HANDLE handle);

static bpp_printer_struct *bpp_find_printer(PRINTER_HANDLE handle);

static bpp_oper_context_struct *bpp_malloc_oper_context(bpp_printer_struct *printer);


static void bpp_init_printer(bpp_printer_struct *printer);
static void bpp_deinit_printer(bpp_printer_struct *printer);


static void bpp_error_handle(bpp_printer_struct *printer, BPP_ERROR_CODE err_code);

static BtStatus bpp_exec_get_printer_attributes(bpp_printer_struct *printer, U32 attr_bitmask);

BtStatus bpp_exec_print_object(bpp_printer_struct *printer, bt_bpp_object *print_req);

static kal_bool bpp_printer_capability_gotten(bpp_printer_struct *printer);


/*----*/
kal_bool bpp_continue_to_read_data(bpp_printer_struct *printer);

static BtStatus bpp_cancel_job(bpp_printer_struct *printer);



/*****************************************************************************
 * FUNCTION
 *  bpp_init
 * DESCRIPTION
 *  This function is bpp simple push init function
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_bpp_context.printer_list = NULL;

	bpp_create_private_folder();
	
#ifdef __BPP_DEBUG__

	bpp_debug_x_init();

#endif
}


#define API___________FUNC

BtStatus bpp_register_callback(BppCallback cb_func)
{
	g_bpp_context.app_callback = cb_func;

	return BT_STATUS_SUCCESS;
}

BtStatus bpp_connect_printer(BD_ADDR *remAddr, kal_int32 prt_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = NULL;
    GoepClientApp *client = NULL;
    ObexTpAddr addr;
    BtStatus status = BT_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY1(BPP_CONNECT_ENTRY_PRINTER, prt_handle);
	
    printer = bpp_new_printer(prt_handle);
	
    if (printer)
    {
        OS_MemCopy((U8*)&printer->bt_addr, (U8*)(remAddr), sizeof(BD_ADDR));
        printer->handle = prt_handle;

		printer->user_abort_flag = 0;
    }
    else
    {
		return BT_STATUS_FAILED;
    }

	client = &printer->job_channel;

    client->type = GOEP_PROFILE_BPP;
    client->callback = (GoepClientCallback) bpp_job_channel_callback;
    client->connFlags = GOEP_NEW_CONN;
    client->connId = 0;

    status = GOEP_RegisterClient(client, &bpp_obsfunc_table);
	BT_BPP_TRACE_INFO_ARG1(BPP_GOEP_REGISTERCLIENT_RETURN, status);

	if (status == OB_STATUS_SUCCESS)
	{
	    addr.type = OBEX_TP_BLUETOOTH;
	    OS_MemCopy((U8*)&(addr.proto.bt.addr), (U8*)remAddr, sizeof(BD_ADDR));
	    addr.proto.bt.uuid = SC_DIRECT_PRINTING;

	    status = GOEP_TpConnect(client, &addr);
		BT_BPP_TRACE_INFO_ARG1(BPP_GOEP_TPCONNECT_RETURN, status);

		if (status == BT_STATUS_PENDING)
		{
			printer->state = BPP_STATE_TP_CONNECTING;
		}
	    else if (status == BT_STATUS_SUCCESS)
	    {
	    	/* TP connected, build OBEX connect */
			status = bpp_obex_connect(client, BPP_JOB_CHANNEL, NULL);
			
			printer->state = BPP_STATE_OBEX_CONNECTING;
	    }
		else
		{
			/* error */
	        bpp_error_handle(printer, BPP_ERROR);		
		}
	}

	return status;
}

BtStatus bpp_get_printer_attribute(PRINTER_HANDLE printer_handle, U32 attr_bitmask)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);
	BtStatus status = BT_STATUS_FAILED;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_FUNC_ENTRY4(BPP_GET_PRINTER_ATTR, attr_bitmask, printer_handle, 
    				printer, printer ? printer->state : 0);

	if (printer == NULL || printer->state != BPP_STATE_CONNECTED)
	{
		return BT_STATUS_FAILED;
	}

	printer->getattr_invoker = BPP_GETATTR_INVOKER_APP;

	status = bpp_exec_get_printer_attributes(printer, attr_bitmask);
	
    return status;
}

BtStatus bpp_print_object(PRINTER_HANDLE printer_handle, bt_bpp_object *print_req)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);
	BtStatus status = BT_STATUS_FAILED;
#define BPP_DOCFMT_ATTR_BITMASK (BPP_DOCUMENT_FORMATS_SUPPORTED_MASK)

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_FUNC_ENTRY6(BPP_ENTRY_PRINT_OBJECT, 
    				print_req->print_model,print_req->mime_type,print_req->object_size,printer_handle, 
    				printer, printer ? printer->state : 0);

	if (printer == NULL || printer->state != BPP_STATE_CONNECTED)
	{
		return BT_STATUS_FAILED;
	}

	memcpy (&printer->print_object, print_req, sizeof(bt_bpp_object));

	if (bpp_printer_capability_gotten(printer))
	{
		/* print the object */
		status = bpp_exec_print_object(printer, &printer->print_object);
	}
	else
	{
		/* first GetPrinterAttributes, then print */
		status = bpp_exec_get_printer_attributes(printer, BPP_DOCFMT_ATTR_BITMASK);

		printer->getattr_invoker = BPP_GETATTR_INVOKER_OWN;
	}

	return status;
}

void bpp_progress_rsp_received(PRINTER_HANDLE printer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY3(BPP_PROGRESS_RSP_RECEIVED, printer_handle, printer, printer?printer->state:0);

	if (printer == NULL || printer->state == BPP_STATE_IDLE)
	{
		return ;
	}

	/* we can continue to send progress ind to app now. */
	printer->cont_prog_ind = KAL_TRUE;
}

BtStatus bpp_cancel(PRINTER_HANDLE printer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);
	BtStatus status = BT_STATUS_PENDING;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY3(BPP_CANCEL, printer_handle, printer, printer?printer->state:0);

	if (printer == NULL || printer->state == BPP_STATE_IDLE)
	{
		return BT_STATUS_FAILED;
	}

    switch (printer->state)
    {
        case BPP_STATE_CONNECTED:
			status = BT_STATUS_FAILED;
            break;
			
        case BPP_STATE_GETING_ATTRIBUTE:            
            status = GOEP_ClientAbort(&printer->job_channel); 
            break;
			
        case BPP_STATE_SENDING_DOC:
            if (printer->status_channel)
            {
            	if (printer->status_channel->state == BPP_STS_STATE_GETING_EVENT)
            	{
					status = GOEP_ClientAbort(&printer->job_channel);             
            	}

				/* disconnect status channel */			
				bpp_disc_status_channel(printer);
            }
            printer->user_abort_flag |= BPP_USER_CANCEL_FLAG;
            break;

		case BPP_STATE_PRINTING_JOB:

			/* CancelJob is sent on job-channel */
            status = bpp_cancel_job(printer);

			/* disconnect status channel */			
			bpp_disc_status_channel(printer);

            printer->user_abort_flag |= BPP_USER_CANCEL_FLAG;
			break;
			
        case BPP_STATE_CREATEING_JOB:
            status = GOEP_ClientAbort(&printer->job_channel); 
			break;

        default:
            break;

    }

	printer->state = BPP_STATE_USER_CANCELLING;
	
    return status;
}

BtStatus bpp_disconnect_printer(PRINTER_HANDLE printer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);
	BtStatus status = BT_STATUS_PENDING;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY3(BPP_DISCONNECT_PRINTER, printer_handle, printer, printer?printer->state:0);

	if (printer == NULL || printer->state == BPP_STATE_IDLE)
	{
		return BT_STATUS_FAILED;
	}

    switch (printer->state)
    {
        case BPP_STATE_CONNECTED:
            if (printer->status_channel)
            {
                bpp_disc_status_channel(printer);
            }
            else
            {
                status = GOEP_Disconnect(&printer->job_channel);
            }
            printer->user_abort_flag |= BPP_USER_DISCONNECT_FLAG;
            break;
			
        case BPP_STATE_GETING_ATTRIBUTE:            
            status = GOEP_ClientAbort(&printer->job_channel); 
            break;
			
        case BPP_STATE_SENDING_DOC:
        case BPP_STATE_PRINTING_JOB:
            if (printer->status_channel)
            {
                bpp_disc_status_channel(printer);
            }
            printer->user_abort_flag |= BPP_USER_DISCONNECT_FLAG;
            break;

        case BPP_STATE_CREATEING_JOB:
            status = GOEP_ClientAbort(&printer->job_channel); 
			break;

		case BPP_STATE_DISCONNECTING:
			break;
			
        default:
            if (printer->status_channel)
            {
                status = GOEP_TpDisconnect(&printer->status_channel->channel);
				BT_BPP_TRACE_INFO_ARG1(BPP_TPDISCONNECT_STSCHNL_RETURN, status);
	            printer->user_abort_flag |= BPP_USER_DISCONNECT_FLAG;
            }
            else
            {
                status = GOEP_TpDisconnect(&printer->job_channel);
				BT_BPP_TRACE_INFO_ARG1(BPP_TPDISCONNECT_JOBCHNL_RETURN, status);
            }
            break;

    }
	
	printer->state = BPP_STATE_DISCONNECTING;

	BT_BPP_TRACE_INFO_ARG1(BPP_DISCONNECT_PRINTER_RETURN, status);
	
    return status;
}


BtStatus bpp_accept_authentication(PRINTER_HANDLE printer_handle, bt_bpp_obex_auth_resp *auth_rsp)
{
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);
	BtStatus status;

	BT_BPP_FUNC_ENTRY3(BPP_ACCEPT_AUTHCHAL, printer_handle, printer, printer?printer->state:0);

	if (printer == NULL || printer->state != BPP_STATE_OBEX_CONNECTING)
	{
		return BT_STATUS_FAILED;
	}

	status = bpp_obex_connect(&printer->job_channel, BPP_JOB_CHANNEL, auth_rsp);

	printer->flags &= ~BPP_FLAGS_AUTHCHAL_RCVD;

	return status;
}

void bpp_reject_authentication(PRINTER_HANDLE printer_handle)
{
    bpp_printer_struct *printer = bpp_find_printer(printer_handle);

	BT_BPP_FUNC_ENTRY3(BPP_REJECT_AUTHCHAL, printer_handle, printer, printer?printer->state:0);

	if (printer == NULL 
		|| printer->state == BPP_STATE_DISCONNECTING)
	{
		return;
	}

	/* user cancel: just disconnect */
	bpp_disc_job_channel(printer, BPP_DISC_TP);
	
}


#define APP_CALLBACK________FUNC

void BppAppCallback(BppCallbackParms *parms)
{
	if (g_bpp_context.app_callback)
	{
		g_bpp_context.app_callback(parms);
	}

	if (parms->oper == BPP_OPER_USER_CANCEL)
	{
		bpp_printer_struct * printer = bpp_find_printer(parms->printer_handle);

		if (printer)
        	printer->user_abort_flag &= ~BPP_USER_CANCEL_FLAG;	
	}
}



#define JOB_CHANNEL_GOEP_CALLBACK
/*****************************************************************************
 * FUNCTION
 *  bpp_job_channel_callback
 * DESCRIPTION
 *  This function handle job channel event which come from GOEP.
 * PARAMETERS
 *  event     
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_job_channel_callback(GoepClientEvent *event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = BPP_GET_PRT_FROM_JOBCHN(event->handler);
    kal_uint16 event_id = event->event;
	GoepClientApp *GoepClient;
	BppCallbackParms parms;
	BtStatus status = OB_STATUS_PENDING;
	kal_bool ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY6(BPP_JOBCHNL_CALLBACK, (kal_int32)event->handler,(kal_int32)event->event,
					(kal_int32)printer, printer?printer->state:0, printer->status_channel, 
					printer->status_channel ? printer->status_channel->state : 0);

    if (printer == NULL)
    {
        ASSERT(0);
        return;
    }

	GoepClient = &printer->job_channel;
	
	parms.printer_handle = printer->handle;
	
    switch (event_id)
    {
    case GOEP_EVENT_TP_CONNECTED:

		if (printer->state == BPP_STATE_DISCONNECTING)
		{
			GOEP_TpDisconnect(GoepClient);
			break;
		}		
		
		/* tp connected: build OBEX connect */
		bpp_obex_connect(GoepClient, BPP_JOB_CHANNEL, NULL);
		
		printer->state = BPP_STATE_OBEX_CONNECTING;

        break;

    case GOEP_EVENT_TP_DISCONNECTED:
		/* clear data */
		
		/* notify app */
		parms.event = BPP_EVENT_TP_DISCONNECTED;
		BppAppCallback(&parms);

		printer->state = BPP_STATE_IDLE;

		bpp_deinit_printer(printer);
		
		printer = NULL;
		
        break;

		
    case GOEP_EVENT_COMPLETE:
		/*operation complete: connect, get, put, abort, disconnect */
		
		if (printer->state == BPP_STATE_DISCONNECTING)
		{
			if (printer->status_channel)
			{
				/* wait for status channel disconnected */
				BT_BPP_TRACE_INFO(BPP_WAIT_FOR_STSCHNL_DISCONNECTED);
				break;
			}
			if (event->oper == GOEP_OPER_DISCONNECT)
			{
				/* OBEX Disconnect is complete, disconnect link. */
				status = GOEP_TpDisconnect(GoepClient);
			}
			else
			{
				GOEP_Disconnect(GoepClient);
			}
			break;
		}

        switch (event->oper) 
		{
        case GOEP_OPER_CONNECT:
            /* OBEX connection is established. */
			printer->state = BPP_STATE_CONNECTED;

            printer->printer_mru = Goep_get_mru(&printer->job_channel);
	
			/* notify app */
			parms.event = BPP_EVENT_COMPLETE;
			parms.oper = BPP_OPER_CONNECT;
			BppAppCallback(&parms);
			
			break;

        case GOEP_OPER_PUSH:

            bpp_destory_oper_context(printer->current_oper);
            printer->current_oper = NULL;
			
            /* OBEX put is complete. */
            if (printer->print_object.print_model == BPP_MODEL_JOB_BASE && printer->status_channel)
            {
				/* progress ind */
				bpp_progress_ind(printer);

				/* doc has been sent out, waiting for printing */
				printer->state = BPP_STATE_PRINTING_JOB;
            }
			else
			{
				/* notify app*/
				parms.event = BPP_EVENT_COMPLETE;			
				parms.oper = BPP_OPER_PRINT_DOC;
	            BppAppCallback(&parms);

				printer->state = BPP_STATE_CONNECTED;
			}

			Goep_init_send_buf(&printer->job_channel, NULL, 0);
			
			break;

        case GOEP_OPER_PULL:

            switch (printer->current_oper->current_req) 
			{
            case BPP_OPER_GET_ATTRIBUTE:
					
				ret = bpp_get_printer_attr_end(printer, &parms.info.printer_attributes);

				printer->state = BPP_STATE_CONNECTED;

				if (ret == KAL_TRUE)
					parms.event = BPP_EVENT_COMPLETE;
				else
					parms.event = BPP_EVENT_FAILED;

				if (printer->getattr_invoker == BPP_GETATTR_INVOKER_OWN && parms.event == BPP_EVENT_COMPLETE)
				{
					/* then print the object */
					status = bpp_exec_print_object(printer, &printer->print_object);
				}
			
				if (printer->getattr_invoker == BPP_GETATTR_INVOKER_APP)
				{
					/* app request to GetPrinterAttributes
					   notify it the result */
					parms.oper = BPP_OPER_GET_ATTRIBUTE;
					BppAppCallback(&parms);
				}
				else if (parms.event == BPP_EVENT_FAILED || status != OB_STATUS_PENDING)
				{
					/* Notify app: print failed */
					parms.event = BPP_EVENT_FAILED;
					parms.oper = BPP_OPER_PRINT_DOC;
					BppAppCallback(&parms);
				}
				
				break;
				
            case BPP_OPER_CREATE_JOB:

				bpp_create_job_end(printer);

				printer->state = BPP_STATE_CONNECTED;

	            if (printer->job_id > 0)
	            {
	            	if (printer->status_channel != NULL)
	            	{
						BT_BPP_TRACE_INFO_ARG1(BPP_CREATEJOBEND_STATE_ERROR,printer->status_channel->state);
	            		bpp_disc_status_channel(printer);	
						BPP_ASSERT(0);
	            	}

            		/* connect Status Channel after get a valid job ID */
					bpp_connect_status_channel(printer);

					/* send document */
					status = bpp_send_doc(printer);
	            }
				else
				{
					status = BT_STATUS_FAILED;
				}

				if (status == BT_STATUS_PENDING)
				{
					printer->state = BPP_STATE_SENDING_DOC;
				}
				else
				{
					/* if failed, notify app */
					parms.event = BPP_EVENT_FAILED;
					parms.oper = BPP_OPER_CREATE_JOB;
					BppAppCallback(&parms);
				}
				
				break;
				
            case BPP_OPER_CANCEL_JOB:
				parms.event = BPP_EVENT_COMPLETE;
				parms.oper = BPP_OPER_USER_CANCEL;
				BppAppCallback(&parms);

				/*Clean the current oper field*/
			    bpp_destory_oper_context(printer->current_oper);
				printer->current_oper = NULL;

				printer->state = BPP_STATE_CONNECTED;
				break;
				
			default:
				break;
            }
			
			break;

        case GOEP_OPER_DISCONNECT:
            /* OBEX Disconnect is complete, disconnect link. */
            status = GOEP_TpDisconnect(GoepClient);
            printer->state = BPP_STATE_DISCONNECTING;
			break;

        case GOEP_OPER_ABORT:
			if (printer->state == BPP_STATE_USER_CANCELLING)
			{
				parms.event = BPP_EVENT_COMPLETE;
				parms.oper = BPP_OPER_USER_CANCEL;
				BppAppCallback(&parms);
			}
			
			printer->state = BPP_STATE_CONNECTED;
			
			break;

        default:
			break;
			
        }
		break;
			
    case GOEP_EVENT_CONTINUE:
		
		if (printer->user_abort_flag)
		{
        	/* user abort */
            GOEP_ClientAbort(&printer->job_channel);
			break;
		}
		
        switch (event->oper) 
		{
        case GOEP_OPER_PUSH:
			/* continue to push */
			
			if (printer->cont_prog_ind)
			{
				bpp_progress_ind(printer);
			}
			
            ret = bpp_continue_to_read_data(printer);

			if (ret == KAL_TRUE)
			{
			}
            else
            {	
            	/* data read error */
                GOEP_ClientAbort(&printer->job_channel);
            }
			
			break;

        case GOEP_OPER_PULL:
			
            GOEP_ClientContinue(&printer->job_channel);
			
			break;
        }
		
		break;
		
    case GOEP_EVENT_ABORTED:
		if (printer->state == BPP_STATE_DISCONNECTING)
		{
			if (printer->status_channel)
			{
				/* wait for status channel disconnected */
				BT_BPP_TRACE_INFO(BPP_WAIT_FOR_STSCHNL_DISCONNECTED);
				break;
			}

			GOEP_TpDisconnect(GoepClient);
			break;
		}		
		else 
#if OBEX_AUTHENTICATION == XA_ENABLED    
        if (event->oper == GOEP_OPER_CONNECT &&
			(event->reason == OBRC_UNAUTHORIZED) &&
            (printer->flags & BPP_FLAGS_AUTHCHAL_RCVD)) 
        {
			parms.event = BPP_EVENT_AUTHCHAL_IND;
			OS_MemCopy((U8*)&(parms.info.chal_info), (U8*)&(printer->chal_info), sizeof(parms.info.chal_info));
			
			BppAppCallback(&parms);

			break;
        }
		else
#endif	
		if (printer->state == BPP_STATE_USER_CANCELLING)
		{
			parms.event = BPP_EVENT_COMPLETE;
			parms.oper = BPP_OPER_USER_CANCEL;
			BppAppCallback(&parms);
		}
		else
		{
			parms.event = BPP_EVENT_FAILED;
			if (printer->current_oper)
			{
				parms.oper = printer->current_oper->current_req;
			}
			else
			{
				parms.oper = BPP_OPER_NONE;
			}
			parms.failed_reason = event->failed_rsp_code;
			BppAppCallback(&parms);
		}

		Goep_init_send_buf(&printer->job_channel, NULL, 0);

		printer->state = BPP_STATE_CONNECTED;
		
		/* if status channel exists, disconnect it */
		if (printer->status_channel != NULL)
		{
		    bpp_disc_status_channel(printer);
		}
		
		break;

    case GOEP_EVENT_HEADER_RX:
        break;
		
#if OBEX_AUTHENTICATION == XA_ENABLED

    case GOEP_EVENT_AUTH_CHALLENGE:
		{
		U8 copy_len;
		/* receive authentication challenge : record the info */
		printer->flags |= BPP_FLAGS_AUTHCHAL_RCVD;
		printer->chal_info.options = event->challenge.options;
		copy_len = event->challenge.realmLen < BPP_MAX_REALM_LEN - 1 ? event->challenge.realmLen : BPP_MAX_REALM_LEN - 1;
		OS_MemCopy(printer->chal_info.realm, event->challenge.realm, copy_len);
		printer->chal_info.realm[copy_len] = 0;
		printer->chal_info.realm_len = copy_len;		
    	}
		break;

    case GOEP_EVENT_AUTH_RESPONSE:
		break;

#endif 


    case GOEP_EVENT_DISCOVERY_FAILED:
    case GOEP_EVENT_NO_SERVICE_FOUND:
    case GOEP_EVENT_BTCHIP_REJECT:
    case GOEP_EVENT_SCO_REJECT:
		parms.event = BPP_EVENT_FAILED;
		parms.oper = BPP_OPER_CONNECT;
		parms.failed_reason = event_id;
		BppAppCallback(&parms);

		bpp_deinit_printer(printer);		
		printer = NULL;
		
		break;

    default:

        break;
    }

}

#define STATUS_CHANNEL_FUNC


/*****************************************************************************
 * FUNCTION
 *  bpp_status_channel_callback
 * DESCRIPTION
 *  This function is a status chanel callback function be registed to GOEP
 * PARAMETERS
 *  event      
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_status_channel_callback(GoepClientEvent *event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_status_channel_struct* sts_chn = BPP_GET_STS_CHANNEL(event->handler);
    bpp_printer_struct *printer = sts_chn? sts_chn->printer:NULL;

    bpp_status_channel_state state = (bpp_status_channel_state)(sts_chn? sts_chn->state : 0);
	kal_bool soap_complete, sts;
	BppCallbackParms parms;
	bpp_event_type evt;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY5(BPP_STATUSCHNL_CALLBACK, (kal_int32)event->handler,(kal_int32)event->event,
					(kal_int32)printer, printer?printer->state:0, state);
	
    if (printer == NULL)
    {
        switch (event->event)
        {
            case GOEP_EVENT_TP_DISCONNECTED:

				bpp_destory_oper_context(sts_chn->get_event_oper);
	            sts_chn->get_event_oper = NULL;

				GOEP_DeregisterClient(&sts_chn->channel);
	            bpp_mem_free(sts_chn);
				
#ifdef __BPP_DEBUG__
                if(g_bpp_context.printer_list == NULL)
                {
                    bpp_debug_x_mem_fh_summary();
                }
#endif
                break;

            default:
                GOEP_TpDisconnect(&sts_chn->channel);
                break;
        }
        return;
    }

	/* disconnecting */
	if (state == BPP_STS_STATE_DISCONNECTING && 
		event->event != GOEP_EVENT_TP_DISCONNECTED)
	{
        GOEP_TpDisconnect(&sts_chn->channel);
		return;
	}

    switch (event->event)
    {
        case GOEP_EVENT_TP_CONNECTED:
            switch (state)
            {
                case BPP_STS_STATE_TP_CONNECTING:
                    bpp_obex_connect(&sts_chn->channel, BPP_STATUS_CHANNEL, NULL);
                    sts_chn->state = BPP_STS_STATE_OBEX_CONNECTING;
                    break;
					
                default:
                    BPP_ASSERT(0);
                    break;
            }
            break;

        case GOEP_EVENT_SCO_REJECT:
        case GOEP_EVENT_NO_SERVICE_FOUND:
        case GOEP_EVENT_AUTH_CHALLENGE:
            bpp_mem_free(sts_chn);
            printer->status_channel = NULL;
            BPP_ASSERT(0);
            break;

        case GOEP_EVENT_TP_DISCONNECTED:
            bpp_destory_status_channel(sts_chn);
		    printer->status_channel = NULL;

			if ((printer->user_abort_flag & BPP_USER_DISCONNECT_FLAG))
			{
			    /* If the disconnection is caused by user disconnect request, 
			     * then disconnect job channel */
				bpp_disc_job_channel(printer, BPP_DISC_TP);
			}

			break;

        case GOEP_EVENT_COMPLETE:
            switch (state)
            {
                case BPP_STS_STATE_OBEX_CONNECTING:
                    bpp_get_event(printer);
                    sts_chn->state = BPP_STS_STATE_GETING_EVENT;
                    break;

				case BPP_STS_STATE_ABORT:
					sts_chn->state = BPP_STS_STATE_CONNECTED;
					break;
					
                default:
                    break;
            }
            break;

        case GOEP_EVENT_ABORTED:
			{
            	GOEP_TpDisconnect(&sts_chn->channel);
				sts_chn->state = BPP_STS_STATE_DISCONNECTING;
			}
            break;

        case GOEP_EVENT_CONTINUE:
			
            switch (state)
				
                case BPP_STS_STATE_GETING_EVENT:
				{					
					evt = BPP_EVENT_PRINTER_STATUS_IND;

           			soap_complete = bpp_is_soap_complete(sts_chn->get_event_oper);

					if(soap_complete == KAL_TRUE) 
		            {
		                if (printer->state == BPP_STATE_PRINTING_JOB
							|| printer->state == BPP_STATE_SENDING_DOC)
		               	{
			                sts = bpp_get_job_status(printer, &parms.info.job_status);

							if (sts == KAL_TRUE)
							{
				                switch (parms.info.job_status.job_state)
				                {
				                    case BPP_JOB_STATE_WAITING:
				                    case BPP_JOB_STATE_STOPPED:
										evt = BPP_EVENT_PRINTER_STATUS_IND;
				                        break;

				                    case BPP_JOB_STATE_COMPLETED:
										bpp_disc_status_channel(printer);
										evt = BPP_EVENT_COMPLETE;
				                        break;

				                    case BPP_JOB_STATE_ABORTED:
				                    case BPP_JOB_STATE_CANCELED:
							            bpp_disc_status_channel(printer);
										evt = BPP_EVENT_USER_ABORTED;
				                        break;
										
				                    case BPP_JOB_STATE_PRINTING:
				                    default:
										evt = BPP_EVENT_PRINTER_STATUS_IND;
				                        break;
				                }

								if (evt == BPP_EVENT_COMPLETE|| evt == BPP_EVENT_USER_ABORTED)
								{	
									/* reset the status */
									printer->state = BPP_STATE_CONNECTED;
									sts_chn->state = BPP_STS_STATE_CONNECTED;
							 	}
								 
								parms.event = evt;
								parms.oper = BPP_OPER_PRINT_DOC;
								parms.printer_handle = printer->handle;

								BppAppCallback(&parms);
							}
						}
					   	else
					   	{
					   		/* clear the current data */
					   		bpp_clear_job_status_data(printer);
					   	}
		            }
				
					if (evt == BPP_EVENT_COMPLETE|| evt == BPP_EVENT_USER_ABORTED)
					{	
					}
					else
					{
                    	GOEP_ClientContinue(&sts_chn->channel);
					}

					break;
					
                default:
                    BPP_ASSERT(0);
                    break;
            }
            break;

        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_status_channel_init
 * DESCRIPTION
 *  This function init status channel, If turn on the g_enable_pts_test, there would be no status channel.
 * PARAMETERS
 *  printer    
 * RETURNS
 *  void
 *****************************************************************************/
static BtStatus bpp_connect_status_channel(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	GoepClientApp *client;
    ObexTpAddr addr;
	BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_TRACE_INFO(BPP_CONNECT_STATUS_CHANNEL);
	
    printer->status_channel = (bpp_status_channel_struct*) bpp_mem_alloc(sizeof(bpp_status_channel_struct));
    if (printer->status_channel == NULL)
    {
    	return BT_STATUS_FAILED;
    }

    OS_MemSet((U8*) printer->status_channel, 0, sizeof(bpp_status_channel_struct));
    printer->status_channel->printer = printer;

	client = &printer->status_channel->channel;
		
    client->type = GOEP_PROFILE_BPP;
    client->callback = (GoepClientCallback) bpp_status_channel_callback;
    client->connFlags = GOEP_NEW_CONN;
    client->connId = 0;

    status = GOEP_RegisterClient(client, &bpp_obsfunc_table);

    addr.type = OBEX_TP_BLUETOOTH;
    addr.proto.bt.uuid = SC_PRINTING_STATUS;

    OS_MemCopy((U8*)&(addr.proto.bt.addr), (U8*)&printer->bt_addr, sizeof(BD_ADDR));

    status = GOEP_TpConnect(client, &addr);

	if (status == OB_STATUS_PENDING)
	{
          printer->status_channel->state = BPP_STS_STATE_TP_CONNECTING;
	}
    else if (status == OB_STATUS_SUCCESS)
    {
    	/* TP connected, build OBEX connect */
        status = bpp_obex_connect(client, BPP_STATUS_CHANNEL, NULL);
        printer->status_channel->state = BPP_STS_STATE_OBEX_CONNECTING;
    }

	return status;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_status_channel_disc
 * DESCRIPTION
 *  This function is to disc printer status channel
 * PARAMETERS
 *  printer        prienter instance
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_disc_status_channel(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY2(BPP_DISC_STATUS_CHANNEL, printer->status_channel, printer->status_channel?printer->status_channel->state:0);

    if (printer->status_channel)
    {
        switch (printer->status_channel->state)
        {
            case BPP_STS_STATE_INIT:
                break;

            case BPP_STS_STATE_TP_CONNECTING:
            case BPP_STS_STATE_OBEX_CONNECTING:
                printer->status_channel->state = BPP_STS_STATE_DISCONNECTING;
                break;

            case BPP_STS_STATE_CONNECTED:
                GOEP_TpDisconnect(&printer->status_channel->channel);
                break;

            case BPP_STS_STATE_GETING_EVENT:
                //GOEP_ClientAbort(&printer->status_channel->channel);
                //printer->status_channel->state = BPP_STS_STATE_DISCONNECTING;
                //in order to pass PTS test, use TpDisconnect instead of ClientAbort
                GOEP_TpDisconnect(&printer->status_channel->channel);
                break;

            case BPP_STS_STATE_DISCONNECTING:
                break;

            default:
                break;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_destory_status_channel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  printer   
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_destory_status_channel(bpp_status_channel_struct *status_channel)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY(BPP_DESTORY_STATUSCHNL);

	BPP_ASSERT(status_channel);

    bpp_destory_oper_context(status_channel->get_event_oper);
    status_channel->get_event_oper = NULL;

	GOEP_DeregisterClient(&status_channel->channel);

	bpp_mem_free(status_channel);
	
}

#define INTERNAL____FUNC



/*****************************************************************************
 * FUNCTION
 *  bpp_obex_connect
 * DESCRIPTION
 *  This function establish obex connection
 * PARAMETERS
 *  client     
 *  channel     [IN]        
 *  pauth   
 * RETURNS
 *  void
 *****************************************************************************/
static BtStatus bpp_obex_connect(GoepClientApp *client, 
										bpp_channel_num channel, 
										bt_bpp_obex_auth_resp *pauth)
{

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 bpp_uuid[TARGET_UUID_LEN];
    ObexAuthResponse auth_rsp;
	BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY2(BPP_ENTRY_OBEX_CONNECT, channel, pauth);

    OS_MemCopy(bpp_uuid, BtBaseUUID, 16);

    if (channel == BPP_JOB_CHANNEL)
    {
        StoreBE16(bpp_uuid + 2, BPPSVC_DPS);
    }
    else
    {
        StoreBE16(bpp_uuid + 2, BPPSVC_STS);
    }
    
    OS_MemSet((U8*)&client->connect_req,0,sizeof(GoepConnectReq));
    client->connect_req.target = bpp_uuid;
    client->connect_req.targetLen = TARGET_UUID_LEN;
    client->connect_req.mru = BPP_MRU;
	
    if (pauth)
    {
        auth_rsp.password = (U8*)pauth->passwd;
        auth_rsp.passwordLen = pauth->passwd_len;
        auth_rsp.userId = (U8*)pauth->userid;
        auth_rsp.userIdLen = pauth->userid_len;
        client->connect_req.response = &auth_rsp;
    }

    status = GOEP_Connect(client, &client->connect_req);
	BT_BPP_TRACE_INFO_ARG1(BPP_GOEP_CONNECT_RETURN, status);

	if (status != OB_STATUS_PENDING)
	{
        GOEP_TpDisconnect(client);
	}

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_print_doc_handle
 * DESCRIPTION
 *  This function is print doc req handler
 * PARAMETERS
 *  printer     
 *  print_object
 * RETURNS
 *  void
 *****************************************************************************/
static BtStatus bpp_simple_push_print(bpp_printer_struct *printer, bt_bpp_object *print_object)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_oper_context_struct *pcontext;
	BtStatus status = OB_STATUS_FAILED;
	kal_bool ret = KAL_FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_FUNC_ENTRY(BPP_SIMPLEPUSH_PRINT);

	pcontext = bpp_malloc_oper_context(printer);

    if (pcontext)
    {
        printer->current_oper = pcontext;
        ret = bpp_init_send_doc_context(pcontext, print_object, printer->printfile_flag);
    }
	
	if (ret == KAL_TRUE)
	{
		status = bpp_send_doc(printer);
	}

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_init_goep_object
 * DESCRIPTION
 *  This function init goep object
 * PARAMETERS
 *  pobjreq       
 *  pcontext    
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_init_goep_object(GoepObjectReq *pobjreq, bpp_oper_context_struct *pcontext)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_MemSet(pobjreq, 0, sizeof(GoepObjectReq));
    pobjreq->name = NULL;
    if (pcontext->current_req == BPP_OPER_PRINT_DOC)
    {
        pobjreq->object = pcontext;
        pobjreq->objectLen = pcontext->data_len;
        pobjreq->type = (U8*) pcontext->object.send_obj.mime_str;
		pobjreq->name = pcontext->object.send_obj.pfile_obj->file_name;
        pobjreq->buf_ptr = NULL;
    }
    else
    {
        pobjreq->type = (U8*) pcontext->object.get_obj.mime_type;
        pobjreq->object = pcontext;
        pobjreq->buf_ptr = NULL;
        pobjreq->objectLen = 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_disc_job_channel
 * DESCRIPTION
 *  This function is to disconnect the job channel
 * PARAMETERS
 *  printer              printer instance
 *  disc_type       [IN] Disc obex or disc tp layer
 * RETURNS
 *  void
 *****************************************************************************/
static BtStatus bpp_disc_job_channel(bpp_printer_struct *printer, bpp_disc_type_enum disc_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus status;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_FUNC_ENTRY2(BPP_DISC_JOG_CHANNEL, disc_type, printer->state);

    switch (disc_type)
    {
        case BPP_DISC_OBEX:
            bpp_destory_oper_context(printer->next_oper);
            bpp_destory_oper_context(printer->current_oper);
            printer->next_oper = NULL;
            printer->current_oper = NULL;
            status = GOEP_Disconnect(&printer->job_channel);
            break;

        case BPP_DISC_TP:
            status = GOEP_TpDisconnect(&printer->job_channel);
            break;
			
        default:
			status = BT_STATUS_FAILED;
            break;
    }
	
	BT_BPP_TRACE_INFO_ARG1(BPP_DISC_JOBCHNL_RETURN, status);

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_create_job
 * DESCRIPTION
 *  This function is create job req handler
 * PARAMETERS
 *  printer  
 *  print_object       
 * RETURNS
 *  void
 *****************************************************************************/
static BtStatus bpp_create_job(bpp_printer_struct *printer, bt_bpp_object *print_object)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_oper_context_struct *send_doc_context = bpp_malloc_oper_context(printer);
    kal_bool ret = KAL_FALSE;
	BtStatus status = BT_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_FUNC_ENTRY(BPP_ENTRY_CREATE_JOB);

    if (send_doc_context)
    {
    	/* Use the next oper to record the send_doc context */
        printer->next_oper = send_doc_context;
		
        ret = bpp_init_send_doc_context(send_doc_context, print_object, printer->printfile_flag);
        if (ret == KAL_TRUE)
        {
            bpp_oper_context_struct *create_job_oper_context = bpp_malloc_oper_context(printer);

			printer->current_oper = create_job_oper_context;
			
            if(create_job_oper_context)
            {
            	/*Use the current oper field*/
                ret = bpp_init_create_job_context(create_job_oper_context, print_object);
            }
        }

        if (ret == KAL_TRUE)
        {
            BPP_DB_SOAP_OUT((kal_char*)"\r\nBPP_CREATE_JOB_START:\r\n", 25);
			
            status = bpp_pull(&printer->job_channel, printer->current_oper);
        }
    }
	
    return status;
}


static BtStatus bpp_pull(GoepClientApp * GoepClient, bpp_oper_context_struct *oper_context)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	GoepObjectReq objreq;
	bpp_head_buff_struct *pdu_out;
	kal_uint8 *pbuf;
    kal_uint16 buf_len = 0;
	BtStatus status = BT_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY(BPP_ENTRY_PULL);

    pdu_out = oper_context->pdu_out;

    pbuf = bpp_get_pdu_ptr(pdu_out, &buf_len);

    oper_context->pdu_out = NULL;
	
    bpp_init_goep_object(&objreq, oper_context);

	BPP_DB_SOAP_OUT((kal_char*) pbuf, buf_len);
    BPP_DB_SOAP_OUT((kal_char*) "\r\n\r\n", 4);

    if (GOEP_ClientQueueHeader(GoepClient, OBEXH_BODY, pbuf, buf_len) == KAL_TRUE)
    {
        if (GOEP_ClientQueueHeader(GoepClient, OBEXH_END_BODY, 0, 0) == KAL_TRUE)
        {
            status = GOEP_Pull(GoepClient, &objreq, FALSE);
			BT_BPP_TRACE_INFO_ARG1(BPP_GOEP_PULL_RETURN, status);
        }
    }

	/* init data */
	/* pdu_out will be use to store soap data which recv from pinter */
    bpp_clear_pdu_data(pdu_out);  
	
    oper_context->object.get_obj.pdu_in = pdu_out;
    oper_context->data_len = 0;

	return status;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_create_job_end
 * DESCRIPTION
 *  This function parse the create job response data whic come from the printer
 * PARAMETERS
 *  printer   
 * RETURNS
 *  void
 *****************************************************************************/
static kal_bool bpp_create_job_end(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bpp_create_job_rst create_job_rst;
    kal_bool ret = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO(BPP_CREATE_JOB_END);

    create_job_rst.job_id = -1;
	printer->job_id = -1;

	ret = bpp_parse_create_job_rsp_soap(
			&printer->current_oper->object.get_obj, 
			&create_job_rst);
#if 1

	if (ret && BPP_IS_RSP_STSCODE_SUCCESS(create_job_rst.status_idx))
	{
        printer->job_id = create_job_rst.job_id;
	}
	
#else	

    if (ret &&
        create_job_rst.ret < 3 && create_job_rst.job_id > 0)
    {
        printer->job_id = create_job_rst.job_id;
	
        if(g_enable_pts_test==0)
        {
            /* connect to status channel */
            bpp_status_channel_init(printer);
        }
    }
    else
    {
        ret = create_job_rst.ret;
    }
#endif		
	
    BT_BPP_TRACE_INFO_ARG2(BPP_CREATE_JOB_INFO, create_job_rst.job_id, create_job_rst.status_idx);

	/*Clean the current oper field*/
    bpp_destory_oper_context(printer->current_oper);
	
	/*Let the extra oper field be the current oper field*/
    printer->current_oper = printer->next_oper;
    printer->next_oper = NULL;

	if (printer->job_id > 0)
	{
		ret = KAL_TRUE;
	}
	else
	{
		ret = KAL_FALSE;
	}
	
    return ret;
}

static kal_bool bpp_get_printer_attr_end(bpp_printer_struct *printer,
												 bt_bpp_printer_attributes * prt_attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bpp_get_printer_attr_resp attr_rsp;
	kal_bool ret = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_TRACE_INFO(BPP_GET_PRINTER_ATTR_END);
	
	attr_rsp.p_attr = prt_attr;
	OS_MemSet((U8 *)prt_attr, 0, sizeof(bt_bpp_printer_attributes));
	
	ret = bpp_parse_get_attr_rsp_soap(
			&printer->current_oper->object.get_obj, 
			&attr_rsp);

	if (ret == KAL_TRUE)
	{
	    /* check operation status */
		if (!BPP_IS_RSP_STSCODE_SUCCESS(attr_rsp.status_idx))
		{
			/* operation status: failed */
			ret = KAL_FALSE;
		}
	}

	if (printer->current_oper)
    {
        bpp_destory_oper_context(printer->current_oper);
        printer->current_oper = NULL;
    }

	if (ret == KAL_TRUE)
	{
		printer->docfmt_num = attr_rsp.p_attr->capability.docfmt_num;
		memcpy(printer->doc_format_supported, 
				attr_rsp.p_attr->capability.doc_format_supported,
				sizeof(printer->doc_format_supported));
	}
	
	return ret;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_get_event
 * DESCRIPTION
 *  This function send get event req
 * PARAMETERS
 *  printer     
 * RETURNS
 *  void
 *****************************************************************************/
static kal_bool bpp_get_event(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_bool ret = KAL_FALSE;
	BtStatus rst;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO(BPP_START_GET_EVENT);
	
    if (printer && printer->status_channel)
    {
        bpp_oper_context_struct *get_event_context = bpp_malloc_oper_context(printer);

        printer->status_channel->get_event_oper = get_event_context;

        ret = bpp_init_get_event_context(get_event_context, printer->job_id);
		
        BPP_DB_SOAP_OUT((kal_char*)"\r\nBPP_GETEVENT_START:\r\n", 23);

        if (ret == KAL_TRUE)
        {
            rst = bpp_pull(&printer->status_channel->channel, get_event_context);
			
			if (rst != OB_STATUS_PENDING)
			{
			    ret = KAL_FALSE;
			}
			else
			{
				printer->status_channel->state = BPP_STS_STATE_GETING_EVENT;
			}
        }
    }

    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_send_doc
 * DESCRIPTION
 *  This function send send_doc req
 * PARAMETERS
 *  printer  
 * RETURNS
 *  void
 *****************************************************************************/
static BtStatus bpp_send_doc(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_oper_context_struct *pcontext = printer->current_oper;
    bpp_send_obj_struct *psend_obj = pcontext ? &pcontext->object.send_obj : NULL;
	bpp_file_obj_struct *pfile_obj = psend_obj ? psend_obj->pfile_obj : NULL;
	GoepObjectReq objreq;
	BtStatus status = BT_STATUS_FAILED;
	kal_char name[BPP_MAX_FILE_NAME_LEN];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_BPP_FUNC_ENTRY(BPP_ENTRY_SEND_DOC);

    if (pcontext == NULL || psend_obj == NULL || pfile_obj == NULL)
    {
    	return BT_STATUS_FAILED;
    }

    bpp_init_goep_object(&objreq, pcontext);

	if (printer->print_object.print_model == BPP_MODEL_JOB_BASE)
    {
        U8 ap[6];

		BPP_ASSERT(printer->job_id > 0);
		
        ap[0] = BPPAPTAG_JOBID;
        ap[1] = 4;
        StoreBE32(ap + 2, printer->job_id);
        GOEP_ClientQueueHeader(&printer->job_channel, OBEXH_APP_PARAMS, ap, 6);
    }

	/* document name */
    bpp_get_doc_name(pfile_obj->file_name, name, BPP_MAX_FILE_NAME_LEN); 
	objreq.name = (U16*)name;
	
	/* document format */
    objreq.type = (U8*)bpp_get_doc_format(psend_obj->mime_id);

	status = GOEP_Push(&printer->job_channel, &objreq);
	BT_BPP_TRACE_INFO_ARG2(BPP_GOEP_PUSH_RETURN, objreq.objectLen, status);

    return status;
}

static void bpp_clear_job_status_data(bpp_printer_struct *printer)
{
	
    bpp_clear_pdu_data(printer->status_channel->get_event_oper->object.get_obj.pdu_in);
	
    printer->status_channel->get_event_oper->data_len = 0;
}

/*****************************************************************************
 * FUNCTION
 *  bpp_get_job_status
 * DESCRIPTION
 *  This function get job status
 * PARAMETERS
 *  printer  
 *  pret  
 * RETURNS
 *  void
 *****************************************************************************/
static kal_bool bpp_get_job_status(bpp_printer_struct *printer, bt_bpp_job_status *prt_status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bpp_get_job_status_resp job_sts_rsp;
	
    kal_bool ret = KAL_FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	job_sts_rsp.p_jobstatus = prt_status;

	OS_MemSet((U8 *)prt_status, 0, sizeof(bt_bpp_job_status));

    ret = bpp_parse_get_event_rsp_soap(&printer->status_channel->get_event_oper->object.get_obj, &job_sts_rsp);
	
    BT_BPP_TRACE_INFO_ARG4(BPP_GETEVENT_RESP, ret, prt_status->printer_state, prt_status->job_state, prt_status->state_reason);
    
	bpp_clear_job_status_data(printer);

    if (ret == KAL_TRUE && job_sts_rsp.status_idx < 3)
    {
    }

	return ret;
}

static BtStatus bpp_cancel_job(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_bool ret = KAL_FALSE;
	BtStatus status = BT_STATUS_FAILED;
    bpp_oper_context_struct *cancel_job_context = bpp_malloc_oper_context(printer);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cancel_job_context)
    {
    	if (printer->current_oper != NULL)
    	{
			/*Clean the current oper field*/
		    bpp_destory_oper_context(printer->current_oper);
    	}
		
        printer->current_oper = cancel_job_context;
        ret = bpp_init_cancel_job_context(cancel_job_context, printer->job_id);
        BPP_DB_SOAP_OUT((kal_char*)"\r\nBPP_CANCEL_JOB_START:\r\n", 25);
        if (ret == KAL_TRUE)
        {
            status = bpp_pull(&printer->job_channel, cancel_job_context);
        }
        else
        {
            bpp_disc_status_channel(printer);
        }
    }

	return status;
	
}

static BtStatus bpp_exec_get_printer_attributes(bpp_printer_struct *printer, U32 attr_bitmask)
{
    bpp_oper_context_struct *get_attr_context = bpp_malloc_oper_context(printer);
    kal_bool ret = KAL_FALSE;
	BtStatus status = BT_STATUS_FAILED;

    BT_BPP_FUNC_ENTRY3(BPP_EXEC_GET_PRINTER_ATTR, attr_bitmask, printer, printer->state);
	
	if (get_attr_context)
    {
        /*Assemble the soap of GetAttribute operation*/
        printer->current_oper = get_attr_context;
		
        ret = bpp_init_get_attr_context(get_attr_context, attr_bitmask);        

		if (ret == KAL_TRUE)
		{
			status = bpp_pull(&printer->job_channel, get_attr_context);
		}
    }

	if (status == BT_STATUS_PENDING)
	{
		printer->state = BPP_STATE_GETING_ATTRIBUTE;
	}
	printer->user_abort_flag = 0;

	return status;
}


BtStatus bpp_exec_print_object(bpp_printer_struct *printer, bt_bpp_object *print_req)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus status = BT_STATUS_FAILED;
	int ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_FUNC_ENTRY4(BPP_ENTRY_EXEC_PRINT_OBJECT, 
    				print_req->print_model, print_req->mime_type,print_req->object_size,
    				printer->state);

	/* Already gotten GetPrinterAttributes response */

	/* pre-process data: compose xhtml if need */
	ret = bpp_pre_process_object_data(printer, print_req);
	if (ret <= 0)
	{
		BT_BPP_TRACE_ERROR_ARG1(BPP_PRE_PRECESS_OBJECTDATA_FAILED, ret);
		return BT_STATUS_FAILED;
	}


	/* init the job ID to invalid value */
	printer->job_id = -1;
	
	if (print_req->print_model == BPP_MODEL_SIMPLE_PUSH)
	{
		/* Simple Push print */
		status = bpp_simple_push_print(printer, print_req);

		if (status == BT_STATUS_PENDING)
		{
			printer->state = BPP_STATE_SENDING_DOC;
		}
	}
	else
	{
		/* Job-base print */

		/* first create job */
		status = bpp_create_job(printer, print_req);
		
		if (status == BT_STATUS_PENDING)
		{
			printer->state = BPP_STATE_CREATEING_JOB;
		}
	}
	
	printer->cont_prog_ind = KAL_TRUE;
	printer->user_abort_flag = 0;

	return status;
}

static kal_bool bpp_printer_capability_gotten(bpp_printer_struct *printer)
{
	kal_bool ret;

	if (printer->docfmt_num > 0)
		ret = KAL_TRUE;
	else
		ret = KAL_FALSE;

	BT_BPP_TRACE_INFO_ARG1(BPP_PRINTER_CAPA_GOTTEN_RETURN, ret);

	return ret;
}
	
BT_BOOL bpp_printer_support_doctype(bpp_printer_struct *printer, bt_bpp_mime_type mime_type)
{
	BT_BOOL spt = FALSE;
#if 0 	
	int i;
	for (i = 0; i < printer->docfmt_num; i++)
	{
		if (mime_type == printer->doc_format_supported[i])
		{
			spt = TRUE;
			break;
		}
	}

	if (spt)
	{
		BT_BPP_TRACE_INFO_ARG1(BPP_PRINTER_SUPPORT_MIMETYPE, mime_type);
	}
	else
	{
		BT_BPP_TRACE_INFO_ARG1(BPP_PRINTER_NOT_SUPPORT_MIMETYPE, mime_type);
	}
#endif	
	return spt;
}



#define UNTIL_FUNC


/*****************************************************************************
 * FUNCTION
 *  bpp_init_printer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  printer    
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_init_printer(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemSet((U8*)printer,0,sizeof(bpp_printer_struct));

    printer->handle = BPP_INVALID_PRINTER_HANDLE;
    printer->job_id = -1;
    printer->state = BPP_STATE_IDLE;
}



/*****************************************************************************
 * FUNCTION
 *  bpp_new_printer
 * DESCRIPTION
 *  This function new printer instance
 * PARAMETERS
 *  handle      [IN]        
 *  bt_addr     
 * RETURNS
 *  void
 *****************************************************************************/
static bpp_printer_struct *bpp_new_printer(PRINTER_HANDLE handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = bpp_find_printer(handle);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (printer == NULL)
    {
        bpp_printer_struct *pt = g_bpp_context.printer_list;
        bpp_printer_struct *prev = pt;
        U32 count = 0;

        while (pt)
        {
            prev = pt;
            pt = pt->pnext;
            count++;
        }

		BT_BPP_TRACE_INFO_ARG2(BPP_MAX_PRINTER_NUM_REACHED, count, BPP_MAX_PRINTER_NUM);

        if (count < BPP_MAX_PRINTER_NUM)
        {
            printer = (bpp_printer_struct*) bpp_mem_alloc(sizeof(bpp_printer_struct));
        }

        if (printer)
        {
            bpp_init_printer(printer);
            if (prev == NULL)
            {
                g_bpp_context.printer_list = printer;
            }
            else
            {
                prev->pnext = printer;
            }
        }
        return printer;
    }

    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_find_printer
 * DESCRIPTION
 *  This function  find printer instance
 * PARAMETERS
 *  handle      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static bpp_printer_struct *bpp_find_printer(PRINTER_HANDLE handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *printer = g_bpp_context.printer_list;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    handle &= 0x7FFFFFFF;
    while (printer)
    {
        if ((printer->handle & 0x7FFFFFFF) == handle)
        {
            return printer;
        }
        printer = printer->pnext;
    }

    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_malloc_oper_context
 * DESCRIPTION
 *  This function malloc operation context
 * PARAMETERS
 *  printer     
 * RETURNS
 *  void
 *****************************************************************************/
static bpp_oper_context_struct *bpp_malloc_oper_context(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_oper_context_struct *oper_context = (bpp_oper_context_struct*) bpp_mem_alloc(sizeof(bpp_oper_context_struct));

    oper_context->printer = printer;
	
    return oper_context;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_find_printer_with_parser
 * DESCRIPTION
 *  This function find printer with xml parser 
 * PARAMETERS
 *  xml_parser_p   xml            
 *  current_req    oper code
 * RETURNS
 *  bpp_printer_struct* pritner struct
 *****************************************************************************/
bpp_oper_context_struct *bpp_find_oper_context(XML_PARSER_STRUCT *xml_parser_p ,bpp_oper_enum current_req)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    bpp_printer_struct *printer = g_bpp_context.printer_list;
    bpp_oper_context_struct *current_oper =NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(xml_parser_p)
    {
        BPP_ASSERT(current_req == BPP_OPER_GET_ATTRIBUTE || 
            current_req == BPP_OPER_CREATE_JOB ||
            current_req == BPP_OPER_GET_EVENT ||
            current_req == BPP_OPER_CANCEL_JOB);
        
        while(printer)
        {
            if(current_req == BPP_OPER_GET_EVENT)
            {
                if(printer->status_channel && 
                    printer->status_channel->state == BPP_STS_STATE_GETING_EVENT)
                {
                    current_oper = printer->status_channel->get_event_oper;
                }
            }
            else
            {
                current_oper = printer->current_oper;
            }

            if(current_oper && current_oper->current_req == current_req)
            {
                if(current_oper->object.get_obj.parser_param && 
                    current_oper->object.get_obj.parser_param->xml_parser_p==xml_parser_p)
                {
                    return current_oper;
                }
            }
            
            printer = printer->pnext;
        }
    }
    
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_destory_oper_context
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pcontext         
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_destory_oper_context(bpp_oper_context_struct *pcontext)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pcontext)
    {
        if (pcontext->pdu_out)
        {
            bpp_free_head_buf(pcontext->pdu_out);
        }
        if (pcontext->current_req == BPP_OPER_PRINT_DOC)
        {
            bpp_send_obj_struct *psend_obj = &pcontext->object.send_obj;

            bpp_deinit_send_obj(psend_obj);
        }
        else
        {
            bpp_get_obj_struct *get_obj = &pcontext->object.get_obj;

            if (get_obj->pfile_obj)
            {
                bpp_destory_file_obj(get_obj->pfile_obj);
                get_obj->pfile_obj = NULL;
            }

            if (get_obj->pdu_in)
            {
                bpp_free_head_buf(get_obj->pdu_in);
                get_obj->pdu_in = NULL;
            }
            if (get_obj->parser_param)
            {
                bpp_mem_free(get_obj->parser_param);
            }
        }

        bpp_mem_free(pcontext);
    }
}

static void bpp_deinit_printer(bpp_printer_struct *printer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_printer_struct *prt = g_bpp_context.printer_list;
#ifdef __BPP_DEBUG__
    kal_bool need_check_mem = KAL_TRUE;
#endif
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BPP_TRACE_INFO_ARG1(BPP_DEINIT_PRINTER, printer);
 
    ASSERT(prt);

	if (printer == NULL)
		return;

    if(printer->status_channel)
    {
		BT_BPP_TRACE_INFO_ARG1(BPP_DEINIT_PRINTER_WITH_STSCHNL,printer->status_channel->state);	
        bpp_disc_status_channel(printer);
        printer->status_channel->printer = NULL;
		
#ifdef __BPP_DEBUG__
        need_check_mem = KAL_FALSE;
#endif
    }
    
    GOEP_DeregisterClient(&printer->job_channel);
	
    if (printer->current_oper)
    {
        bpp_destory_oper_context(printer->current_oper);
    }
    if (printer->next_oper)
    {
        bpp_destory_oper_context(printer->next_oper);
    }

	/* remove the printer from the list */
    if (prt == printer)
    {
        g_bpp_context.printer_list = printer->pnext;
        bpp_mem_free(printer);
    }
    else
    {
        while (prt)
        {
            if (prt->pnext == printer)
            {
                prt->pnext = printer->pnext;
                bpp_mem_free(printer);
                break;
            }
            prt = prt->pnext;
        }
    }
	
#ifdef __BPP_DEBUG__
    if(g_bpp_context.printer_list==NULL && need_check_mem==KAL_TRUE )
    {
        bpp_debug_x_mem_fh_summary();
    }
#endif

}

static void bpp_error_handle(bpp_printer_struct *printer, BPP_ERROR_CODE err_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (err_code)
    {
        case BPP_ERROR_LINK_DISC:
            break;
        case BPP_ERROR_INVALID_PARAM:
            break;
			
        default:
            break;
    }

    /* notify app ?*/
	
	/* deinit the printer */
    bpp_deinit_printer(printer);
}

#endif /* __BT_BPP_PROFILE__ */ 

