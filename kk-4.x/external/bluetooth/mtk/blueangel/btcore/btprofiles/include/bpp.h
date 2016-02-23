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
 *  bpp.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for BPP profile header file.
 *
 * Author:
 * -------
 *  Dexiong Xu, revised by Yufeng Chu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/* enum */

/* BPP Event */

typedef enum
{
	BPP_EVENT_NONE = 0,
		
	BPP_EVENT_COMPLETE,

	BPP_EVENT_FAILED,
	BPP_EVENT_USER_ABORTED,
		
	BPP_EVENT_PROGRESS_IND,

	BPP_EVENT_PRINTER_STATUS_IND,
	
	BPP_EVENT_AUTHCHAL_IND,

	BPP_EVENT_TP_DISCONNECTED,
	
	BPP_EVENT_LAST
	
}bpp_event_type;


typedef enum
{
    BPP_OPER_NONE,
    BPP_OPER_CONNECT,
    BPP_OPER_GET_ATTRIBUTE,
    BPP_OPER_CREATE_JOB,
    BPP_OPER_PRINT_DOC,
    BPP_OPER_GET_EVENT,
    BPP_OPER_CANCEL_JOB,
    BPP_OPER_USER_CANCEL,
    BPP_OPER_DISCONNECT
} bpp_oper_enum;


/* data structure */


/* Forward reference to BppCallbackParms defined below */
typedef struct _BppCallbackParms BppCallbackParms;

/*---------------------------------------------------------------------------
 * BppCallback type
 *
 *  BPPEvents and data are passed to the application through a callback
 *  function of this type defined by the application.
 */
typedef void (*BppCallback)(BppCallbackParms *parms);
/* End of BppCallback */

struct _BppCallbackParms
{
    bpp_oper_enum		oper;       /* current request */
	
    bpp_event_type    	event;      /* Type of BPP event */

	U32				    failed_reason;

	PRINTER_HANDLE 		printer_handle; /*  */

	union
	{
		bt_bpp_printer_attributes	printer_attributes;/* for get_attr oper */

		bt_bpp_job_status 	 		job_status;    /* for PRINTER_STATUS_IND */

	    bt_bpp_progress_status 		prog_status;  /* for PROGRESS_IND */
		
		bt_bpp_obex_auth_chal_info  chal_info;
		
	}info;
	
};

/* APIs  */

/* callback */
BtStatus bpp_register_callback(BppCallback cb_func);

/* function call */
BtStatus bpp_connect_printer(BD_ADDR *remAddr, kal_int32 prt_handle);

BtStatus bpp_disconnect_printer(PRINTER_HANDLE prt_handle);

BtStatus bpp_get_printer_attribute(PRINTER_HANDLE prt_handle, U32 attr_bitmask);

BtStatus bpp_print_object(PRINTER_HANDLE prt_handle, bt_bpp_object *print_req);

void bpp_progress_rsp_received(PRINTER_HANDLE prt_handle);

BtStatus bpp_accept_authentication(PRINTER_HANDLE prt_handle, bt_bpp_obex_auth_resp *auth_rsp);

void bpp_reject_authentication(PRINTER_HANDLE prt_handle);

BtStatus bpp_cancel(PRINTER_HANDLE prt_handle);


 
