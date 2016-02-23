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
 * Bt_adp_hidh.c
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
 * Ting Zheng
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

#ifdef __BT_HIDH_PROFILE__

#include "stdio.h"              /* Basic C I/O functions */
#ifndef __LINUX_SUPPRESS_ERROR__
#ifndef BTMTK_ON_WISE
#include <Windows.h>
#endif
#endif /* __LINUX_SUPPRESS_ERROR__ */
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "bt.h"
#include "bt_os_api.h"
#include "osapi.h"
#include "hid.h"

#include "eventmgr.h"
//#include "kal_trace.h"  /* Trace Header Files */
//#include "bluetooth_trc.h"      /* bt trace definition */

#include "bluetooth_struct.h"
#include "btbm_adp.h"
#include "sdp_adp.h"
#include "l2cap_adp.h"
#include "bluetooth_hid_struct.h"

#include "bt_adp_hidh.h"
#include "sdap_devdb.h"
#include "ddb.h"


hidha_context_struct hidha_cntx[HID_MAX_DEV_NUM];
U8 hidha_reconnect_retry = 0;
static int hid_kbd_fd = -1;
static int device_count= 0;
static int keyboard_registered = 0;
static int nonkeyboard_registered = 0;



#define HID_HOST_RECONNECT_INTERVAL	20000
#define HID_HOST_RECONNECT_RETRY_MAX	5
#define HID_KBD_DEV_PATH	"/dev/hid-keyboard"


/***************************************************************************** 
* External functions
*****************************************************************************/
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);
extern S32 Hid_Interpreter(int fd,HidReportConstructor *hrc, U8 *data, U16 data_len,HidReportType reportType);
extern void Hid_Free_Mem();
static void hidha_handle_query_cnf(HidChannel *Channel, HidQueryRsp *queryRsp, U16 len, BtStatus result);
static void hidha_handle_connect_authorize_ind(HidChannel *Channel, BD_ADDR *bt_addr_p);
static void hidha_handle_connected(HidChannel *Channel, BD_ADDR *bt_addr_p);
static void hidha_handle_disconnected(HidChannel *Channel, BD_ADDR *bt_addr_p, U32 status);
static void hidha_handle_transaction_ind(HidChannel *Channel, HidTransaction *pTrans);       		
static void hidha_handle_transaction_rsp(HidChannel *Channel, HidTransaction *pTrans, BtStatus status);
static void hidha_handle_transaction_cmplt(HidChannel *Channel, HidTransaction *pTrans, BtStatus status);
static void hidha_handle_interrupt_ind(HidChannel *Channel, HidInterrupt *pIntr, BtStatus status);
static void hidha_handle_interrupt_cmplt(HidChannel *Channel, HidInterrupt *pIntr, BtStatus status);
static hidha_context_struct *hidha_find_free_context(void);
static hidha_context_struct *hidha_find_free_context_by_addr(BD_ADDR *bt_addr_p);
static hidha_context_struct *hidha_find_context_by_addr(BD_ADDR *bt_addr_p);
static hidha_context_struct *hidha_find_context_by_ch(HidChannel *channel);
static void hidha_send_msg(msg_type msg, module_type dstMod, sap_type sap,
                                                          local_para_struct *local_para, peer_buff_struct *peer_buff);
static void hidha_send_msg_ext(msg_type msg, module_type dstMod, sap_type sap, kal_uint8 result,
                                                              hidha_context_struct *cntx, BD_ADDR *bt_addr_p);
static void hidha_clear_connection(hidha_context_struct *cntx);
static void hidha_reconnect_start_timer(hidha_context_struct *cntx, U16 timer_ms);
static void hidha_reconnect_stop_timer(hidha_context_struct *cntx);



/*****************************************************************************
 * FUNCTION
 *  hidha_init
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  void       
 * RETURNS
 *  void
 *****************************************************************************/
void hidha_init(void)
{
    U8 i;
    
    hidha_reconnect_retry = 0;
	device_count= 0;
    keyboard_registered = 0;
    nonkeyboard_registered = 0;
	
	btmtk_os_memset((U8*) hidha_cntx, 0, sizeof(hidha_cntx));

    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        hidha_cntx[i].hidha_connection_id = HID_INVALID_CONN_ID;
    }
    
}


void hidh_handle_message(ilm_struct *ilm_ptr)
{
bt_prompt_trace(MOD_BT, "[BT_HID_INT]hidh_handle_message");
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_HIDH_ACTIVATE_REQ:
            btmtk_adp_hidh_handle_activate_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_DEACTIVATE_REQ:
            btmtk_adp_hidh_handle_deactivate_req(ilm_ptr);			
            break;

        case MSG_ID_BT_HIDH_GET_DESCLIST_REQ:
            btmtk_adp_hidh_handle_get_desclist_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_CONNECT_REQ:
            btmtk_adp_hidh_handle_connect_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_DISCONNECT_REQ:
            btmtk_adp_hidh_handle_disconnect_req(ilm_ptr);			
            break;

        case MSG_ID_BT_HIDH_RECONNECT_REQ:
            btmtk_adp_hidh_handle_reconnect_req(ilm_ptr);
            break;
			
        case MSG_ID_BT_HIDH_SEND_CONTROL_REQ:
            btmtk_adp_hidh_handle_control_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_SET_REPORT_REQ:
            btmtk_adp_hidh_handle_set_report_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_GET_REPORT_REQ:
            btmtk_adp_hidh_handle_get_report_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_SET_PROTOCOL_REQ:
            btmtk_adp_hidh_handle_set_protocol_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_GET_PROTOCOL_REQ:
            btmtk_adp_hidh_handle_get_protocol_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_SET_IDLE_RATE_REQ:
            btmtk_adp_hidh_handle_set_idlerate_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_GET_IDLE_RATE_REQ:
            btmtk_adp_hidh_handle_get_idlerate_req(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_SEND_OUTPUT_REPORT:
            btmtk_adp_hidh_handle_send_output_report(ilm_ptr);			
            break;
			
        case MSG_ID_BT_HIDH_CONNECTION_AUTHORIZE_RSP:
            btmtk_adp_hidh_handle_connect_authorize_rsp(ilm_ptr);			
            break;
			
        default:
            break;
    }
}

void hidha_app_callback(HidChannel *Channel, HidCallbackParms *Parms)
{
    switch (Parms->event) 
    {
        case HIDEVENT_QUERY_CNF:
            hidha_handle_query_cnf(Channel, Parms->ptrs.queryRsp, Parms->len, (BtStatus)Parms->status);			
            break;			

        case HIDEVENT_OPEN_IND:
            hidha_handle_connect_authorize_ind(Channel, &(Parms->ptrs.remDev->bdAddr));
            break;
    		
        case HIDEVENT_OPEN:
            kal_trace(BT_TRACE_G2_PROFILES,CONNECTION_OPENED);
            hidha_handle_connected(Channel, &(Parms->ptrs.remDev->bdAddr));
            break;
    		
        case HIDEVENT_CLOSE_IND:
            kal_trace(BT_TRACE_G2_PROFILES,CLOSE_INDICATION_RECEIVED);
            break;
    		
        case HIDEVENT_CLOSED:
            kal_trace(BT_TRACE_G2_PROFILES,CONNECTION_CLOSED);
            hidha_handle_disconnected(Channel, &(Parms->ptrs.remDev->bdAddr), Parms->status);
            break;
    		
        case HIDEVENT_TRANSACTION_IND:
            kal_trace(BT_TRACE_G2_PROFILES,TRANSACTION_RECEIVED);
            hidha_handle_transaction_ind(Channel, Parms->ptrs.trans);       		
            break;

        case HIDEVENT_TRANSACTION_RSP:
            hidha_handle_transaction_rsp(Channel, Parms->ptrs.trans, (BtStatus)Parms->status);			
            break;			
			
        case HIDEVENT_TRANSACTION_COMPLETE:
            kal_trace(BT_TRACE_G2_PROFILES,TRANSACTION_COMPLETE_STATUS__x0X2 , Parms->status);
            hidha_handle_transaction_cmplt(Channel, Parms->ptrs.trans, (BtStatus)Parms->status);
            break;
    		
        case HIDEVENT_INTERRUPT:
            // output report from remote device		
            //kal_trace(BT_TRACE_G2_PROFILES,INTERRUPT_RECEIVED);
            hidha_handle_interrupt_ind(Channel, Parms->ptrs.intr, (BtStatus)Parms->status);
            break;
    
        case HIDEVENT_INTERRUPT_COMPLETE:
            hidha_handle_interrupt_cmplt(Channel, Parms->ptrs.intr, (BtStatus)Parms->status);
            break;
    
        default:
            break;
    }
}

void btmtk_adp_hidh_handle_activate_req(ilm_struct *ilm_ptr)
{
    bt_hidh_activate_cnf_struct *msg_p;

    msg_p = (bt_hidh_activate_cnf_struct*) construct_local_para(sizeof(bt_hidh_activate_cnf_struct), TD_UL);

    if (HID_RegisterHost(hidha_app_callback) == BT_STATUS_FAILED) 
    {
#ifndef __LINUX_SUPPRESS_ERROR__    
#ifndef BTMTK_ON_WISE
        Sleep(2000);
#endif
#endif /* __LINUX_SUPPRESS_ERROR__ */
        msg_p->result = hidd_result_failed;
    }
    else
    {
        msg_p->result = hidd_result_ok;
    }

    hidha_send_msg(MSG_ID_BT_HIDH_ACTIVATE_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) msg_p, NULL);
}
			
void btmtk_adp_hidh_handle_deactivate_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_deactivate_cnf_struct *msg_p;
    U8 i;	
	
    msg_p = (bt_hidh_deactivate_cnf_struct*) construct_local_para(sizeof(bt_hidh_deactivate_cnf_struct), TD_UL);

    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        if (hidha_cntx[i].bReconnect)
        {
            hidha_reconnect_stop_timer(&hidha_cntx[i]);
        }
    }


    if (HID_DeregisterHost() == BT_STATUS_FAILED) 
    {
        msg_p->result = hidd_result_failed;
    }
    else
    {
        msg_p->result = hidd_result_ok;
    }

    hidha_send_msg(MSG_ID_BT_HIDH_DEACTIVATE_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) msg_p, NULL);
}

void btmtk_adp_hidh_handle_get_desclist_req(ilm_struct *ilm_ptr)
{
    bt_hidh_query_req_struct *req_p = (bt_hidh_query_req_struct *) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);
    BtStatus status = BT_STATUS_FAILED;

    if (cntx == NULL)
    {
        return;
    }

    cntx->flag |= HIDDA_TRANSA_FLAG_GET_DESCLIST;    
    status = HID_HostQueryDevice(&cntx->hidChannel, &req_p->bt_addr);
    if (status == BT_STATUS_SUCCESS)
    {
        cntx->flag &= ~HIDDA_TRANSA_FLAG_GET_DESCLIST;    
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_DESCINFO_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            cntx,
            &req_p->bt_addr);
    }
    else if (status != BT_STATUS_PENDING)
    {
        cntx->flag &= ~HIDDA_TRANSA_FLAG_GET_DESCLIST;      
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_DESCINFO_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_failed,
            cntx,
            &req_p->bt_addr);
    }    
}

void btmtk_adp_hidh_handle_connect_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_connect_req_struct *req_p = (bt_hidh_connect_req_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);    
    BtStatus status = BT_STATUS_FAILED;

    if(cntx != NULL){   
        bt_prompt_trace(MOD_BT,"[HID]This connection is reconnecting");
        return;
    }

    cntx = hidha_find_free_context_by_addr(&req_p->bt_addr);   

    if (cntx != NULL)
    {
        if (cntx->bReconnect)
        {
            // it is in reconnect retrying...
            hidha_reconnect_stop_timer(cntx);
        }
    }
    else
    {
        if ((cntx = hidha_find_free_context()) == NULL)
        {
            return;
        }
    }
    
    status = HID_HostQueryDevice(&cntx->hidChannel, &req_p->bt_addr);

    if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_SUCCESS))
    {
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_CONNECT_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_failed,
            cntx,
            &req_p->bt_addr);
        return;
    }

    cntx->bConnInitiator = TRUE;
    cntx->bConnecting = TRUE;
    cntx->hidha_bd_addr = req_p->bt_addr;
    cntx->in_use = TRUE;
}
	
void btmtk_adp_hidh_handle_disconnect_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_disconnect_req_struct *req_p = (bt_hidh_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);
    BtStatus status = BT_STATUS_FAILED;

    if (cntx == NULL)
    {
        return;
    }

    cntx->bConnTerminator = TRUE;       // it shall be set before close function returned, since callback function may be called
    status = HID_CloseConnection(cntx->hidChannel);
    if ((status == BT_STATUS_SUCCESS) ||
        (status == BT_STATUS_PENDING) || 
        (status == BT_STATUS_IN_PROGRESS))
    {
        return;		
    }
    else
    {
        cntx->bConnTerminator = FALSE;
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_DISCONNECT_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_failed,
            cntx,
            &req_p->bt_addr);
    }

}

void btmtk_adp_hidh_handle_reconnect_req(ilm_struct *ilm_ptr)
{
    bt_hidh_reconnect_req_struct *req_p = (bt_hidh_reconnect_req_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_free_context_by_addr(&req_p->bt_addr);    
    BtStatus status;

    if (cntx == NULL)
    {
        return;
    }

    //status = HID_OpenConnection(&cntx->hidChannel, &cntx->hidha_bd_addr);
    status =  HID_HostQueryDevice(&cntx->hidChannel, &cntx->hidha_bd_addr);
    if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING))
    {
        hidha_reconnect_start_timer(cntx, HID_HOST_RECONNECT_INTERVAL);
        return;
    }
    cntx->bConnInitiator = TRUE;
    cntx->in_use = TRUE;
}
			
void btmtk_adp_hidh_handle_control_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_control_req_struct *req_p = (bt_hidh_control_req_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_CONTROL))
    {
        return;
    }
	
    cntx->flag |= HIDDA_TRANSA_FLAG_CONTROL;
    cntx->gTrans[hidda_control].parm.control = (HidControl)req_p->ctrl_op;
    HID_SendControl(cntx->hidChannel, &cntx->gTrans[hidda_control]);
}
			
void btmtk_adp_hidh_handle_set_report_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_set_report_req_struct *req_p = (bt_hidh_set_report_req_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);
    BtStatus status = BT_STATUS_FAILED;

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_SET_REPORT))
    {
        //HID_MEM_FREEIF(req_p->data_ptr);
        return;
    }
	bt_prompt_trace(MOD_BT,"[HID]btmtk_adp_hidh_handle_set_report_req::req_p->report_type=%d",req_p->report_type);
    cntx->flag |= HIDDA_TRANSA_FLAG_SET_REPORT;

    cntx->gTrans[hidda_set_report].parm.report = HID_MEM_MALLOC(sizeof(HidReport));
   cntx->gTrans[hidda_set_report].parm.report->reportType = req_p->report_type;
    // cntx->gTrans[hidda_set_report].parm.report->reportType = HID_REPORT_INPUT;
    cntx->gTrans[hidda_set_report].parm.report->dataLen = req_p->data_len;
    cntx->gTrans[hidda_set_report].parm.report->data = HID_MEM_MALLOC(req_p->data_len);
    btmtk_os_memcpy(cntx->gTrans[hidda_set_report].parm.report->data, req_p->data_ptr, req_p->data_len);
#ifdef __NORDIC_VOICE_COMMAND__
    cntx->gTrans[hidda_set_report].parm.report->serviceInstance = 1;
#endif

    status = HID_HostSetReport(cntx->hidChannel, &cntx->gTrans[hidda_set_report]);
    if (status != BT_STATUS_PENDING)
    {
        hidha_handle_transaction_cmplt(cntx->hidChannel, &cntx->gTrans[hidda_set_report], status);     
    }
    //HID_MEM_FREEIF(req_p->data_ptr);
}
			
void btmtk_adp_hidh_handle_get_report_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_get_report_req_struct *req_p = (bt_hidh_get_report_req_struct *) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_GET_REPORT))
    {
        return;
    }

    cntx->flag |= HIDDA_TRANSA_FLAG_GET_REPORT;

    cntx->gTrans[hidda_get_report].parm.reportReq = HID_MEM_MALLOC(sizeof(HidReportReq));
    btmtk_os_memset((U8 *)cntx->gTrans[hidda_get_report].parm.reportReq, 0, sizeof(HidReportReq));
    cntx->gTrans[hidda_get_report].parm.reportReq->reportType = req_p->report_type;
    cntx->gTrans[hidda_get_report].parm.reportReq->useId = req_p->use_rpt_id;
    if (req_p->use_rpt_id)
    {
        cntx->gTrans[hidda_get_report].parm.reportReq->reportId = req_p->report_id;
    }
    cntx->gTrans[hidda_get_report].parm.reportReq->bufferSize = req_p->buffer_size;

    HID_HostGetReport(cntx->hidChannel, &cntx->gTrans[hidda_get_report]);
}
			
void btmtk_adp_hidh_handle_set_protocol_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_set_protocol_req_struct *req_p = (bt_hidh_set_protocol_req_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_SET_PROTO))
    {
        return;
    }
	
    cntx->flag |= HIDDA_TRANSA_FLAG_SET_PROTO;
    cntx->gTrans[hidda_set_protocol].parm.protocol = (HidProtocol)req_p->protocol_type;
    HID_HostSetProtocol(cntx->hidChannel, &cntx->gTrans[hidda_set_protocol]);
}
			
void btmtk_adp_hidh_handle_get_protocol_req(ilm_struct *ilm_ptr)		
{
    bt_hidh_get_protocol_req_struct *req_p = (bt_hidh_get_protocol_req_struct *) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_GET_PROTO))
    {
        return;
    }

    cntx->flag |= HIDDA_TRANSA_FLAG_GET_PROTO;
    HID_HostGetProtocol(cntx->hidChannel, &cntx->gTrans[hidda_get_protocol]);
}
			
void btmtk_adp_hidh_handle_set_idlerate_req(ilm_struct *ilm_ptr)	
{
    bt_hidh_set_idle_rate_req_struct *req_p = (bt_hidh_set_idle_rate_req_struct *) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);
	
    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_SET_IDLE_RATE))
    {
        return;
    }

    cntx->flag |= HIDDA_TRANSA_FLAG_SET_IDLE_RATE;
    cntx->gTrans[hidda_set_idle_rate].parm.idleRate = (HidIdleRate)req_p->idle_rate;
    HID_HostSetIdleRate(cntx->hidChannel, &cntx->gTrans[hidda_set_idle_rate]);
}
			
void btmtk_adp_hidh_handle_get_idlerate_req(ilm_struct *ilm_ptr)	
{
    bt_hidh_get_idle_rate_req_struct *req_p = (bt_hidh_get_idle_rate_req_struct *) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_GET_IDLE_RATE))
    {
        return;
    }

    cntx->flag |= HIDDA_TRANSA_FLAG_GET_IDLE_RATE;
    HID_HostGetIdleRate(cntx->hidChannel, &cntx->gTrans[hidda_get_idle_rate]);
}
			
void btmtk_adp_hidh_handle_send_output_report(ilm_struct *ilm_ptr)	
{
    bt_hidh_interrupt_data_req_struct *req_p = (bt_hidh_interrupt_data_req_struct *) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&req_p->bt_addr);
    HidInterrupt *pIntr;
    BtStatus status = BT_STATUS_FAILED;
    
    if (cntx == NULL)
    {
        //HID_MEM_FREEIF(req_p->data_ptr);
#ifdef __NORDIC_VOICE_COMMAND__
    {
        U16 i;
        for (i = 0; i < HID_MAX_DEV_NUM; i++)
        {
            if (hidha_cntx[i].in_use && hidha_cntx[i].hidha_active)
            {
                cntx = &hidha_cntx[i];
                break;
            }
        }
        if (cntx == NULL)
        {
            return;
        }
    }
#else
        return;
#endif
    }

    pIntr = (HidInterrupt *)HID_MEM_MALLOC(sizeof(HidInterrupt));
    btmtk_os_memset((U8 *)pIntr, 0, sizeof(HidInterrupt));

    pIntr->data = (U8 *)HID_MEM_MALLOC(req_p->data_len);
    btmtk_os_memcpy(pIntr->data, req_p->data_ptr, req_p->data_len);
    pIntr->dataLen = req_p->data_len;
    pIntr->reportType = req_p->report_type;
#ifdef __NORDIC_VOICE_COMMAND__
    pIntr->serviceInstance = 1;
#endif
	bt_prompt_trace(MOD_BT,"[HID]btmtk_adp_hidh_handle_send_output_report::pIntr->reportType=%d",pIntr->reportType);
	bt_prompt_trace(MOD_BT,"[HID]btmtk_adp_hidh_handle_send_output_report::req_p->data_ptr=%d",req_p->data_ptr[0]);

    status = HID_SendInterrupt(cntx->hidChannel, pIntr);
    if (status != BT_STATUS_PENDING)
    {
        hidha_handle_interrupt_cmplt(cntx->hidChannel, pIntr, status);     
    }

    //HID_MEM_FREEIF(req_p->data_ptr);
}
			
void btmtk_adp_hidh_handle_connect_authorize_rsp(ilm_struct *ilm_ptr)		
{
    bt_hidh_connection_authorize_rsp_struct *msg_p = (bt_hidh_connection_authorize_rsp_struct*) ilm_ptr->local_para_ptr;
    hidha_context_struct *cntx = hidha_find_context_by_addr(&msg_p->bt_addr);

    if (cntx == NULL)
    {
        return;
    }

    if(msg_p->result == hidd_connect_authorization_result_accepted)
    {
        BtStatus status = HID_AcceptConnection(cntx->hidChannel);
        if ((status != BT_STATUS_PENDING) &&
        	     (status != BT_STATUS_SUCCESS))
        {
            kal_trace(BT_TRACE_G2_PROFILES,COULD_NOT_ACCEPT_THE_INCOMING_CONNECTION, status);

            /* send connect cnf  */
            hidha_send_msg_ext(
                MSG_ID_BT_HIDH_CONNECT_IND,
                MOD_EXTERNAL,
                BT_APP_SAP,
                hidd_result_failed,
                cntx,
                &msg_p->bt_addr);
        } 
        else 
        {
            kal_trace(BT_TRACE_G2_PROFILES,OPEN_REQUEST_RECEIVED);
            /* wait for HID connected indication */
        }
    }
    else
    {
        /* check the result ? */
        HID_RejectConnection(cntx->hidChannel);
    }

}

HidDeviceType hidha_get_device_type(hidha_context_struct *cntx)
{
	U32 cod = 0;
    HidDeviceType DevType = 0;
    BtCodRecord record;

    if (DDB_COD_FindRecord((BD_ADDR *)cntx->hidha_bd_addr.addr, &record) == BT_STATUS_SUCCESS)
    {
        cod = record.cod;
//        bt_prompt_trace(MOD_BT, "[HID]cod = %x",cod);      
    }

	switch (cod&0x000000FF)
	{
		case COD_MINOR_PERIPH_KEYBOARD:
            DevType = HID_DEVICE_TYPE_KEYBOARD;
			break;
            
		case COD_MINOR_PERIPH_POINTING:
            DevType = HID_DEVICE_TYPE_MOUSE;
			break;

		case COD_MINOR_PERIPH_GAMEPAD:
            DevType = HID_DEVICE_TYPE_GAMEPAD;
			break;

		case COD_MINOR_PERIPH_JOYSTICK:
            DevType = HID_DEVICE_TYPE_JOYSTICK;
			break;
            
		default:
            bt_prompt_trace(MOD_BT, "[HID]DevType = %x:don't support this device type!",DevType);
			break;
	}

    return DevType;
    
}

static void hidha_open_driver(hidha_context_struct *cntx)
{
    BtStatus status = BT_STATUS_FAILED;
    SdapDevDBRecord sdapRecord;
    U32 vid_pid = 0;	
    HidDeviceType	deviceType;
    
    bt_prompt_trace(MOD_BT, "[HID] when open device_count=%d",device_count);
	
	if(cntx->hidChannel == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]cntx->hidChannel == NULL");
		return;
	}

	if(cntx->hidChannel->reportCons[0] == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]cntx->hidChannel->reportCons[0] == NULL");
		return;
	}

	//if(device_count == 0)
	//{
	if(hid_kbd_fd >= 0)
	{
		close(hid_kbd_fd);
		hid_kbd_fd = -1;
	}

    OS_MemSet((void*) &sdapRecord, 0, sizeof(SdapDevDBRecord));
    OS_MemCopy((void*) &sdapRecord.bdaddr, (const void*) &cntx->hidha_bd_addr, BD_ADDR_SIZE);
    status = SDAP_QueryDevDBRecord(&sdapRecord);
    if (status == BT_STATUS_SUCCESS) {
        vid_pid = ((U32) sdapRecord.vendorID) << 16 | (U32) sdapRecord.productID;
    }
    bt_prompt_trace(MOD_BT,"[HID]vid_pid: 0x%08x, status: %d", vid_pid, status);
    
	hid_kbd_fd = open(HID_KBD_DEV_PATH, O_WRONLY);
	if(hid_kbd_fd < 0)
    {
		bt_prompt_trace(MOD_BT,"[HID]open HID_KBD_DEV_PATH fail");
        return;
    }
    
#ifdef __BT_4_0_BLE__
	if (cntx->hidChannel->cmgrHandler.bdc->devType != BT_DEV_TYPE_BR_EDR)
        deviceType = cntx->hidChannel->reportCons[0]->deviceType;
    else
#endif //__BT_4_0_BLE__
        deviceType = hidha_get_device_type(cntx);
    
	bt_prompt_trace(MOD_BT,"[HID]deviceType = %x", deviceType);
	bt_prompt_trace(MOD_BT,"[HID]keyboard_registered = %d, nonkeyboard_registered = %d", keyboard_registered, nonkeyboard_registered);
		
	if(deviceType == HID_DEVICE_TYPE_KEYBOARD)
	{
		if(ioctl(hid_kbd_fd,HID_KETBOARD, &vid_pid)<0) 
		{
			bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
			return;
		}
		keyboard_registered++;
	}
	else
	{
		if(keyboard_registered == 0)
		{
			if(deviceType == HID_DEVICE_TYPE_MOUSE || deviceType == HID_DEVICE_TYPE_KEYPAD)
			{
				if(ioctl(hid_kbd_fd,HID_MOUSE, 0)<0) 
				{
					bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
					return;
				}
			}
			else if(deviceType == HID_DEVICE_TYPE_GAMEPAD ||
					deviceType == HID_DEVICE_TYPE_JOYSTICK)
			{
				if(ioctl(hid_kbd_fd,HID_GAMEPAD, 0)<0) 
				{
					bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
					return;
				}
			}
		}
		else
		{
			if(ioctl(hid_kbd_fd,HID_KETBOARD, &vid_pid)<0) 
			{
				bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
				return;
			}
		}
		nonkeyboard_registered++;
	}
	device_count++;

}

static void hidha_close_driver(hidha_context_struct *cntx)
{	
	U8 i;
	HidDeviceType	deviceType;
	
	bt_prompt_trace(MOD_BT, "[HID]when close device_count=%d",device_count);
	
	if(cntx->hidChannel == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]cntx->hidChannel == NULL");
		return;
	}

	if(cntx->hidChannel->reportCons[0] == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]cntx->hidChannel->reportCons[0] == NULL");
		return;
	}
	
	if(device_count == 0)
    {
		return;
    }   

#ifdef __BT_4_0_BLE__
	if (cntx->hidChannel->cmgrHandler.bdc->devType != BT_DEV_TYPE_BR_EDR)
        deviceType = cntx->hidChannel->reportCons[0]->deviceType;
    else
#endif //__BT_4_0_BLE__
        deviceType = hidha_get_device_type(cntx);

	bt_prompt_trace(MOD_BT,"[HID]keyboard_registered = %d, nonkeyboard_registered = %d", 
                    keyboard_registered, nonkeyboard_registered);
	device_count--;
    
	if(deviceType == HID_DEVICE_TYPE_KEYBOARD)
	{
		keyboard_registered--;
		if(keyboard_registered == 0 && nonkeyboard_registered != 0)
		{			
			if(hid_kbd_fd >= 0)
			{
				close(hid_kbd_fd);
				hid_kbd_fd = -1;
			}
			hid_kbd_fd = open(HID_KBD_DEV_PATH, O_WRONLY);
			if(hid_kbd_fd < 0)
            {
                bt_prompt_trace(MOD_BT,"[HID]open HID_KBD_DEV_PATH fail");
                return;
            }         
				
			for (i = 0; i < HID_MAX_DEV_NUM; i++)
			{
				if (hidha_cntx[i].in_use)
				{
			       	if(hidha_cntx[i].hidChannel->reportCons[0]->deviceType== HID_DEVICE_TYPE_MOUSE)
					{
						if(ioctl(hid_kbd_fd,HID_MOUSE, 0)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
							return;
						}
					}
					if(hidha_cntx[i].hidChannel->reportCons[0]->deviceType == HID_DEVICE_TYPE_GAMEPAD ||
						hidha_cntx[i].hidChannel->reportCons[0]->deviceType == HID_DEVICE_TYPE_JOYSTICK)
					{
						if(ioctl(hid_kbd_fd,HID_GAMEPAD, 0)<0) 
						{
							bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		nonkeyboard_registered--;
	}
    
	if(device_count == 0)	    
	{
		keyboard_registered = 0;
		nonkeyboard_registered = 0;
        if(hid_kbd_fd >= 0)
        {
            close(hid_kbd_fd);
            hid_kbd_fd = -1;
        }
	}

}

static void hidha_handle_query_cnf(HidChannel *Channel, HidQueryRsp *queryRsp, U16 len, BtStatus result)			
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);
    BtStatus status;

    if (cntx == NULL)
    {
        return;
    }

    /* Note: There might be two possibilities to enter into query cnf:
      * case 1. Local initiates the connection, and do sdp query first. hid connection is
              not established yet.
      * case 2. Local accepts the connect request, and hid connection is established first. 
      *     And do sdp query then.
      */
    if ((result == BT_STATUS_SUCCESS) &&
         (queryRsp->descriptorList != NULL) && 
         (queryRsp->descriptorLen > 0))
    {
        btmtk_os_memset(cntx->descStr, 0, sizeof(cntx->descStr));
        btmtk_os_memcpy(cntx->descStr, Channel->descStr, Channel->descLen); 
        cntx->descLen = (U16)Channel->descLen;
        bt_prompt_trace(MOD_BT,"[HID]cntx->descLen=%d",cntx->descLen);
        HidShowBytes(cntx->descStr, cntx->descLen);
        
		cntx->bSdpQueryDone = TRUE;
        //hidha_open_driver(cntx);
    }


    if (!(cntx->flag & HIDDA_TRANSA_FLAG_GET_DESCLIST)	)
    {
        /* case 1 */
        if (cntx->bConnInitiator)
        {
            if (result == BT_STATUS_SUCCESS)
            {
                status = HID_OpenConnection(&Channel, &cntx->hidha_bd_addr);
                /* send connect cnf */
                if (status ==  BT_STATUS_SUCCESS)
                {
                    hidha_uhid_open(cntx);
                    bt_prompt_trace(MOD_BT,"[HID]1");
                    hidha_send_msg_ext(
                        MSG_ID_BT_HIDH_CONNECT_CNF,
                        MOD_EXTERNAL,
                        BT_APP_SAP,
                        hidd_result_ok,
                        cntx,
                        &cntx->hidha_bd_addr);
                    
                    cntx->bConnInitiator = FALSE;       
                    cntx->hidha_active = TRUE;
                }
                else if (status != BT_STATUS_PENDING)
                {
                    if (status == BT_STATUS_SCO_REJECT)
                    {
                        hidha_send_msg_ext(
                            MSG_ID_BT_HIDH_CONNECT_CNF,
                            MOD_EXTERNAL,
                            BT_APP_SAP,
                            hidd_result_sco_reject,
                            cntx,
                            &cntx->hidha_bd_addr);
                        return;
                    }
                    else
                    {
                        hidha_send_msg_ext(
                            MSG_ID_BT_HIDH_CONNECT_CNF,
                            MOD_EXTERNAL,
                            BT_APP_SAP,
                            hidd_result_failed,
                            cntx,
                            &cntx->hidha_bd_addr);
                        return;
                    }
                }
            }
            else
            {
                hidha_send_msg_ext(
                    MSG_ID_BT_HIDH_CONNECT_CNF,
                    MOD_EXTERNAL,
                    BT_APP_SAP,
                    hidd_result_failed,
                    cntx,
                    &cntx->hidha_bd_addr);
                return;         
            }
        }
        else
        {
            /* For BLE, sdp query may be done before upper layer sends getting desclist req */
            if (TRUE == cntx->bSdpQueryDone)
            {
                hidha_uhid_open(cntx);
            }
        }
    }
    else 
    {
        /* case 2 */
        bt_hidh_query_cnf_struct *cnf_p = (bt_hidh_query_cnf_struct*) construct_local_para(sizeof(bt_hidh_query_cnf_struct), TD_UL);
        cnf_p->bt_addr = cntx->hidha_bd_addr;
        cnf_p->connection_id = cntx->hidha_connection_id;

        cntx->flag &= ~HIDDA_TRANSA_FLAG_GET_DESCLIST;
        if (result == BT_STATUS_SUCCESS)
        {
            cnf_p->result = hidd_result_ok;
            cnf_p->descLen = (U16)queryRsp->descriptorLen;
			cnf_p->descList = cntx->descStr;
			hidha_uhid_open(cntx);
            bt_prompt_trace(MOD_BT,"[HID]2");
            hidha_send_msg(MSG_ID_BT_HIDH_DESCINFO_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) cnf_p, NULL);
        }
        else
        {
            cnf_p->result = hidd_result_failed;
            hidha_send_msg(MSG_ID_BT_HIDH_DESCINFO_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) cnf_p, NULL);
            return;
        }
    }

    /* only update sdp attribute record to context in successful case */	
    cntx->virt_cable = queryRsp->virtualCable;
    cntx->reconnect_initiate = queryRsp->reconnect;
    cntx->norm_connectable = queryRsp->normConnectable;
    cntx->sdp_disable = queryRsp->sdpDisable;
    cntx->boot_dev = queryRsp->bootDevice;
    cntx->hidChannel = Channel;
    //cntx->reportCons = (HidReportConstructor*)HID_MEM_MALLOC(sizeof(HidReportConstructor)*HID_REPORT_ID_MAX);
    //btmtk_os_memcpy((U8*)cntx->reportCons, Channel->reportCons,sizeof(HidReportConstructor)*HID_REPORT_ID_MAX);
 
}

static void hidha_handle_connect_authorize_ind(HidChannel *Channel, BD_ADDR *bt_addr_p)
{
    hidha_context_struct *cntx = hidha_find_free_context_by_addr(bt_addr_p);
    bt_hidh_connection_authorize_ind_struct *msg_p;

    if (cntx != NULL)
    {
        if (cntx->bReconnect)
        {
            // it is in reconnect retrying...
            hidha_reconnect_stop_timer(cntx);
        }
	  
	  if(cntx->bConnecting == TRUE)
	  {
		  bt_prompt_trace(MOD_BT,"[HID]This device is connecting~");
		  HID_RejectConnection(Channel);
	  }
    }
    else
    {
        cntx = hidha_find_free_context();
    }

    if (cntx != NULL)
    {

        msg_p = (bt_hidh_connection_authorize_ind_struct*)construct_local_para(sizeof(bt_hidh_connection_authorize_ind_struct), TD_UL);
        msg_p->connection_id = HID_INVALID_CONN_ID;
        msg_p->bt_addr = *bt_addr_p;
        
        hidha_send_msg(MSG_ID_BT_HIDH_CONNECTION_AUTHORIZE_IND, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) msg_p, NULL);

        cntx->hidChannel = Channel;
        cntx->hidha_bd_addr = *bt_addr_p;
        cntx->in_use = TRUE;
	//add by zhangsg
    //cntx->reportCons = (HidReportConstructor*)HID_MEM_MALLOC(sizeof(HidReportConstructor)*HID_REPORT_ID_MAX);
    //btmtk_os_memcpy((U8*)cntx->reportCons, Channel->reportCons,sizeof(HidReportConstructor)*HID_REPORT_ID_MAX);
    }
    else
    {
        HID_RejectConnection(Channel);
    }

}

static void hidha_handle_connected(HidChannel *Channel, BD_ADDR *bt_addr_p)
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);

    if (cntx != NULL)
    {	
		if (cntx->bConnInitiator)
		{
			if(TRUE == cntx->bSdpQueryDone)
        	{
				hidha_uhid_open(cntx);
				bt_prompt_trace(MOD_BT,"[HID]3");
							
				/* connection is initiated by local host */
				/* send connect cnf */
				hidha_send_msg_ext(
					MSG_ID_BT_HIDH_CONNECT_CNF,
					MOD_EXTERNAL,
					BT_APP_SAP,
					hidd_result_ok,
					cntx,
					bt_addr_p);
						
				cntx->bConnInitiator = FALSE;		
				cntx->bConnecting = FALSE;
			}
		}
		else
		{
			/* connection is initiated by remote device */
			/* send connect ind  */
			hidha_send_msg_ext(
				MSG_ID_BT_HIDH_CONNECT_IND,
				MOD_EXTERNAL,
				BT_APP_SAP,
				hidd_result_ok,
				cntx,
				bt_addr_p);
		}

    }
    else
    {
        // no resource. reject connection?
        cntx = hidha_find_free_context();
        if (cntx == NULL)
        {
            return;
        }

        cntx->hidChannel = Channel;
        cntx->hidha_bd_addr = *bt_addr_p;
        cntx->in_use = TRUE;
        
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_CONNECT_IND,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            cntx,
            bt_addr_p);
        
    }

    cntx->hidha_active = TRUE;
    if (cntx->bReconnect)
    {
        hidha_reconnect_stop_timer(cntx);
    }
}
   		
static void hidha_handle_disconnected(HidChannel *Channel, BD_ADDR *bt_addr_p, U32 status)
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);
    L2capDiscReason discReason = (L2capDiscReason)(status>>16);

    if (cntx == NULL)
    {
        return;
    }
#ifndef __LINUX_SUPPRESS_ERROR__
#ifndef BTMTK_ON_WISE
            Sleep(2000);
#endif
#endif /* __LINUX_SUPPRESS_ERROR__ */
    if (cntx->bReconnect)
    {
        if (hidha_reconnect_retry < HID_HOST_RECONNECT_RETRY_MAX)
        {
            hidha_reconnect_start_timer(cntx, HID_HOST_RECONNECT_INTERVAL);
        }
        else
        {
            hidha_reconnect_stop_timer(cntx);
        }
        goto exit;
    }

    /* channel closed during connection establishing procedure */
    if (!cntx->hidha_active)
    {
        msg_type type; 
        if (cntx->bConnInitiator)
        {
            type = MSG_ID_BT_HIDH_CONNECT_CNF;
        }
        else
        {
            type = MSG_ID_BT_HIDH_CONNECT_IND;
        }
        hidha_send_msg_ext(
            type,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_failed,
            cntx,
            bt_addr_p);
		
        goto exit;	    		
    }

    if (discReason == L2DISC_LINK_DISCON)
    {
        cntx->bReconnect = TRUE;
        hidha_reconnect_start_timer(cntx, HID_HOST_RECONNECT_INTERVAL);
		
        if (!cntx->reconnect_initiate && cntx->norm_connectable)
        {
            // reconnect remote device
            // to be done
        }
        else
        {
            // keep connectable, but need to notify app the status
        }
    }

    if (cntx->bConnTerminator)
    {
        cntx->bConnTerminator = FALSE;
          
        /* send disconnect cnf */
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_DISCONNECT_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            cntx,
            bt_addr_p);
    }
    else
    {
        /* send disconnect ind */
        /* it may be caused by either remote device request or connection timeout... */
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_DISCONNECT_IND,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            cntx,
            bt_addr_p);
    }
	Hid_Free_Mem();

	hidha_uhid_close(cntx);

exit:
    hidha_clear_connection(cntx);	

}

static void hidha_handle_unplug_ind(hidha_context_struct *cntx)
{
    bt_hidh_control_ind_struct *msg_p;

    if (!cntx->hidha_active)
    {
        return;
    }
	
    msg_p = (bt_hidh_control_ind_struct *)construct_local_para(sizeof(*msg_p), TD_UL);
    msg_p->connection_id = cntx->hidha_connection_id;
    msg_p->bt_addr = cntx->hidha_bd_addr;
    msg_p->ctrl_op = hidd_ctrl_op_virtual_cable_unplug;
    hidha_send_msg(MSG_ID_BT_HIDH_UNPLUG_IND, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct *)msg_p, NULL);
}
    		
static void hidha_handle_transaction_ind(HidChannel *Channel, HidTransaction *pTrans)  		
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);

    if (cntx == NULL)
    {
        return;
    }

    switch (pTrans->type)
    {
        case HID_TRANS_CONTROL:
            kal_trace(BT_TRACE_G2_PROFILES,CONTROL);
            if (pTrans->parm.control == HID_CTRL_VIRTUAL_CABLE_UNPLUG)
            {
                hidha_handle_unplug_ind(cntx);
            }
            break;

        default:
            break; 			
    }
}

static void hidha_handle_get_report_cnf(hidha_context_struct *cntx, HidReport *report, BtStatus status)
{
    U8 *data_ptr = cntx->report.data;
    cntx->report.reportType = report->reportType;

    if (report->dataLen > 0)
    {
        data_ptr += cntx->report.dataLen;
        data_ptr = (U8 *)HID_MEM_MALLOC(report->dataLen);
        btmtk_os_memcpy(data_ptr, report->data, report->dataLen);
        cntx->report.dataLen += report->dataLen;
        if (cntx->report.data == NULL)
        {
            cntx->report.data = data_ptr;
        }
    }
    
    if (status == BT_STATUS_SUCCESS)
    {
        bt_hidh_get_report_cnf_struct *msg_p;
        
        msg_p = (bt_hidh_get_report_cnf_struct *)construct_local_para(sizeof(*msg_p), TD_UL);
        msg_p->bt_addr = cntx->hidha_bd_addr;
        msg_p->connection_id = cntx->hidha_connection_id;
        msg_p->report_type = cntx->report.reportType;
        msg_p->data_len = cntx->report.dataLen;
        msg_p->data_ptr = (U8 *)HID_MEM_MALLOC(msg_p->data_len);
        btmtk_os_memcpy(msg_p->data_ptr, cntx->report.data, msg_p->data_len);

        hidha_send_msg(MSG_ID_BT_HIDH_GET_REPORT_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct *)msg_p, NULL);

        HID_MEM_FREEIF(cntx->report.data);
        btmtk_os_memset((U8 *)&cntx->report, 0, sizeof(cntx->report));
    }
    else if (status == BT_STATUS_PENDING)
    {
        /* if status is pending, report data shall be queued until completed */
    }
	
}

static void hidha_handle_get_protocol_cnf(hidha_context_struct *cntx, HidProtocol protocol, BtStatus status)
{
    bt_hidh_get_protocol_cnf_struct *msg_p;

    if (status == BT_STATUS_SUCCESS)
    {
        msg_p = (bt_hidh_get_protocol_cnf_struct *)construct_local_para(sizeof(bt_hidh_get_protocol_cnf_struct), TD_UL);
	 bt_prompt_trace(MOD_BT,"[HID]hidha_handle_get_protocol_cnf:sizeof(bt_hidh_get_protocol_cnf_struct)=%d",sizeof(bt_hidh_get_protocol_cnf_struct));
	 bt_prompt_trace(MOD_BT,"[HID]hidha_handle_get_protocol_cnf:sizeof(*msg_p)=%d",sizeof(*msg_p));

	 msg_p->result = hidd_result_ok;
        msg_p->bt_addr = cntx->hidha_bd_addr;
        msg_p->connection_id = cntx->hidha_connection_id;
        msg_p->protocol_type = (bt_hidd_protocol_type_enum)protocol;
		
        hidha_send_msg(MSG_ID_BT_HIDH_GET_PROTOCOL_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct *)msg_p, NULL);
    }
}

static void hidha_handle_get_idle_rate_cnf(hidha_context_struct *cntx, HidIdleRate idleRate, BtStatus status)
{
    bt_hidh_get_idle_rate_cnf_struct *msg_p;

    if (status == BT_STATUS_SUCCESS)
    {
        msg_p = (bt_hidh_get_idle_rate_cnf_struct *)construct_local_para(sizeof(*msg_p), TD_UL);
        msg_p->bt_addr = cntx->hidha_bd_addr;
        msg_p->connection_id = cntx->hidha_connection_id;
        msg_p->idle_rate = (bt_hidd_protocol_type_enum)idleRate;
		
        hidha_send_msg(MSG_ID_BT_HIDH_GET_IDLE_RATE_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct *)msg_p, NULL);
    }
}
    		
/*****************************************************************************
 * FUNCTION
 *  hidha_handle_transaction_rsp
 * DESCRIPTION
 *  Data has been received from remote device. If there is more data coming, 
 *  the received data shall be queued. 
 * PARAMETERS
 *  Channel             [IN]        
 *  pTrans          [IN]        
 *  status             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void hidha_handle_transaction_rsp(HidChannel *Channel, HidTransaction *pTrans, BtStatus status)  		
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);

    if (cntx == NULL)
    {
        return;
    }

    switch (pTrans->type)
    {
        case HID_TRANS_GET_REPORT_RSP:
            hidha_handle_get_report_cnf(cntx, pTrans->parm.report, status);			
            break;

        case HID_TRANS_GET_PROTOCOL_RSP:
            hidha_handle_get_protocol_cnf(cntx, pTrans->parm.protocol, status);			
            break;

        case HID_TRANS_GET_IDLE_RATE_RSP:
            hidha_handle_get_idle_rate_cnf(cntx, pTrans->parm.idleRate, status);			
            break;
			
        default:
            break; 			
    }
}

/*****************************************************************************
 * FUNCTION
 *  hidha_handle_transaction_cmplt
 * DESCRIPTION
 *  All data (DATA, DATC, SET_xxx rsp or GET_xxx rsp)has been received from remote device, 
 *  then notify that transaction is complete.
 * PARAMETERS
 *  Channel             [IN]        
 *  pTrans          [IN]        
 *  status             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void hidha_handle_transaction_cmplt(HidChannel *Channel, HidTransaction *pTrans, BtStatus status)
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);
    bt_hidh_set_cmd_cnf_struct *msg_p = NULL;
    msg_type msg;	

    if (cntx == NULL)
    {
        return;
    }
	
	
    switch (pTrans->type)
    {
        case HID_TRANS_CONTROL:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_CONTROL;
            msg = MSG_ID_BT_HIDH_SEND_CONTROL_CNF;			
            break;

        case HID_TRANS_SET_REPORT:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_SET_REPORT;
            HID_MEM_FREEIF(pTrans->parm.report->data);
            HID_MEM_FREEIF(pTrans->parm.report);
            msg = MSG_ID_BT_HIDH_SET_REPORT_CNF;			
            break;

        case HID_TRANS_SET_PROTOCOL:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_SET_PROTO;
            msg = MSG_ID_BT_HIDH_SET_PROTOCOL_CNF;			
            break;

        case HID_TRANS_SET_IDLE_RATE:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_SET_IDLE_RATE;
            msg = MSG_ID_BT_HIDH_SET_IDLE_RATE_CNF;			
            break;

        case HID_TRANS_GET_REPORT:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_GET_REPORT;
            HID_MEM_FREEIF(pTrans->parm.reportReq);
            return;
			
        case HID_TRANS_GET_PROTOCOL:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_GET_PROTO;
            return;
			
        case HID_TRANS_GET_IDLE_RATE:
            cntx->flag &= ~HIDDA_TRANSA_FLAG_GET_IDLE_RATE;
            return;
			 
        default:
            return; 			
    }

    msg_p = (bt_hidh_set_cmd_cnf_struct *)construct_local_para(sizeof(*msg_p), TD_UL);
    msg_p->bt_addr = cntx->hidha_bd_addr;
    msg_p->data = *((U32*)&pTrans->parm);
    msg_p->connection_id = cntx->hidha_connection_id;
    if (status == BT_STATUS_SUCCESS)
    {
        msg_p->result = hidd_result_ok;
    }
    else
    {
        msg_p->result = hidd_result_failed;
    }
	
    hidha_send_msg(msg, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct *)msg_p, NULL);
}
    		
/*****************************************************************************
 * FUNCTION
 *  hidha_handle_interrupt_ind
 * DESCRIPTION
 *  Data(Input Report) has been received from remote device. If there is more data coming, 
 *  the received data shall be queued. 
 * PARAMETERS
 *  Channel             [IN]        
 *  pIntr          [IN]        
 *  status             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void hidha_handle_interrupt_ind(HidChannel *Channel, HidInterrupt *pIntr, BtStatus status)
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);
    U8 *data_ptr = NULL;
	int i ;
    if (cntx == NULL)
    {
        return;
    }

    data_ptr = cntx->input_report.data;
	
    /* receiving input data from remote device */
    if (pIntr->dataLen > 0)
    {
        data_ptr += cntx->input_report.dataLen;
        data_ptr = (U8 *)HID_MEM_MALLOC(pIntr->dataLen);
       // data_ptr = (U8 *)malloc(pIntr->dataLen);
        btmtk_os_memcpy(data_ptr, pIntr->data, pIntr->dataLen);
        cntx->input_report.dataLen += pIntr->dataLen;
        if (cntx->input_report.data == NULL)
        {
            cntx->input_report.data = data_ptr;
        }
    }

    if (status == BT_STATUS_SUCCESS)
    {
        //add by zhangsg
        S32	parsedUsage;
        //U16 dataLen = cntx->input_report.dataLen;
        //U8* tmpData = (U8*)malloc(dataLen);
        //btmtk_os_memcpy(tmpData, cntx->input_report.data, dataLen);
#ifdef __BT_4_0_BLE__
        //bt_prompt_trace(MOD_BT,"[HID]pIntr->serviceInstance = %d",pIntr->serviceInstance);
        //S32	parsedUsage = Hid_Interpreter(hid_kbd_fd,cntx->reportCons,cntx->input_report.data,dataLen,pIntr->reportType);
        //parsedUsage = Hid_Interpreter(hid_kbd_fd,cntx->hidChannel->reportCons[pIntr->serviceInstance],
            //cntx->input_report.data, cntx->input_report.dataLen, pIntr->reportType);
#else
        //parsedUsage = Hid_Interpreter(hid_kbd_fd,cntx->hidChannel->reportCons[0],
            //cntx->input_report.data, cntx->input_report.dataLen, pIntr->reportType);
#endif

        hidha_uhid_input(cntx->fd, cntx->input_report.data, cntx->input_report.dataLen);

        if(parsedUsage == -1)
            bt_prompt_trace(MOD_BT,"[HID]Hid_Interpreter fail");
        //free(tmpData);
        //add end		
        //bt_prompt_trace(MOD_BT,"[HID]Hid_Interpreter parsed usage=%d",parsedUsage);
        if(parsedUsage==KEY_NUM_LOCK_USAGE||parsedUsage==KEY_CAPS_LOCK_USAGE||parsedUsage==KEY_SCROLL_LOCK_USAGE)
        {
            bt_hidh_interrupt_data_ind_struct *ind;

            ind = (bt_hidh_interrupt_data_ind_struct *)construct_local_para(sizeof(*ind), TD_UL);
            ind->report_type = pIntr->reportType;
            ind->data_len = cntx->input_report.dataLen;
            ind->data = parsedUsage;        

            ind->connection_id = cntx->hidha_connection_id;
            ind->bt_addr = cntx->hidha_bd_addr;

            hidha_send_msg(
                MSG_ID_BT_HIDH_INTERRUPT_DATA_IND,
                MOD_EXTERNAL,
                BT_APP_SAP,
                (local_para_struct *)ind,
                NULL);
        }	
#ifdef __NORDIC_VOICE_COMMAND__
        else if (parsedUsage == 0xff)
        {
            bt_hidh_disconnect_req_struct *req;
            req = (bt_hidh_disconnect_req_struct *)construct_local_para(sizeof(bt_hidh_disconnect_req_struct), TD_UL);
            req->bt_addr = cntx->hidha_bd_addr;
            hidha_send_msg(
                MSG_ID_BT_HIDH_DISCONNECT_REQ,
                MOD_BT,
                BT_APP_SAP,
                (local_para_struct *)req,
                NULL);
        }
#endif
        /*
            for  (  i  =   0 ; i  <  cntx->input_report.dataLen+15; i ++ )
            {
            bt_prompt_trace(MOD_BT, "[HID][%4d] %8x: %2x\n ",i,cntx->input_report.data+i-8,*(cntx->input_report.data+i-8));
            if  ((i  +   1 )  %   4   ==   0 )
            {
            bt_prompt_trace(MOD_BT, " ----------------------\n " );
            } 
            } 
            */
        HID_MEM_FREEIF(cntx->input_report.data);
        // free(cntx->input_report.data);
        btmtk_os_memset((U8 *)&cntx->input_report, 0, sizeof(cntx->input_report));
    }
    else if (status == BT_STATUS_PENDING)
    {
        /* if status is pending, report data shall be queued until completed */
    }
}
    
/*****************************************************************************
 * FUNCTION
 *  hidha_handle_interrupt_cmplt
 * DESCRIPTION
 *  All data(Output Report) has been sent to remote device. 
 *  Data sending can be refered to btmtk_adp_hidh_handle_send_output_report
 * PARAMETERS
 *  Channel             [IN]        
 *  pIntr          [IN]        
 *  status             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void hidha_handle_interrupt_cmplt(HidChannel *Channel, HidInterrupt *pIntr, BtStatus status)
{
    hidha_context_struct *cntx = hidha_find_context_by_ch(Channel);

    /* release control buffer */
    HID_MEM_FREEIF(pIntr->data);
    HID_MEM_FREE(&pIntr->node);

    if (cntx == NULL)
    {
        return;
    }

    if (status == BT_STATUS_SUCCESS)
    {
        kal_trace(BT_TRACE_G2_PROFILES,INTERRUPT_COMPLETE);
            
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_INTERRUPT_DATA_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            cntx,
            &(cntx->hidha_bd_addr));
    }
    else
    {
        /* ignore the sending data while sending failed */
        kal_trace(BT_TRACE_G2_PROFILES,INTERRUPT_COMPLETE_FAILED);
        
        hidha_send_msg_ext(
            MSG_ID_BT_HIDH_INTERRUPT_DATA_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_failed,
            cntx,
            &(cntx->hidha_bd_addr));
    }
}

static hidha_context_struct *hidha_find_free_context(void)
{
    hidha_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        if (!hidha_cntx[i].in_use)
        {
            cntx = &hidha_cntx[i];
            break;
        }
    }
    return cntx;	
}

static hidha_context_struct *hidha_find_free_context_by_addr(BD_ADDR *bt_addr_p)
{
    hidha_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        //if (HID_DEVICE_ADDR_EQUAL(&hidha_cntx[i].hidha_bd_addr, bt_addr_p) && !hidha_cntx[i].in_use)
        if(hidha_cntx[i].hidha_bd_addr.addr[0] == bt_addr_p->addr[0] && \
			hidha_cntx[i].hidha_bd_addr.addr[1] == bt_addr_p->addr[1] && \
			hidha_cntx[i].hidha_bd_addr.addr[2] == bt_addr_p->addr[2] && \
			hidha_cntx[i].hidha_bd_addr.addr[3] == bt_addr_p->addr[3] && \
			hidha_cntx[i].hidha_bd_addr.addr[4] == bt_addr_p->addr[4] && \
			hidha_cntx[i].hidha_bd_addr.addr[5] == bt_addr_p->addr[5] )
        {
            cntx = &hidha_cntx[i];
            break;
        }
    }
    return cntx;	
}

static hidha_context_struct *hidha_find_context_by_addr(BD_ADDR *bt_addr_p)
{
    hidha_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        //if (HID_DEVICE_ADDR_EQUAL(&hidha_cntx[i].hidha_bd_addr, bt_addr_p) && hidha_cntx[i].in_use)
        if(hidha_cntx[i].hidha_bd_addr.addr[0] == bt_addr_p->addr[0] && \
			hidha_cntx[i].hidha_bd_addr.addr[1] == bt_addr_p->addr[1] && \
			hidha_cntx[i].hidha_bd_addr.addr[2] == bt_addr_p->addr[2] && \
			hidha_cntx[i].hidha_bd_addr.addr[3] == bt_addr_p->addr[3] && \
			hidha_cntx[i].hidha_bd_addr.addr[4] == bt_addr_p->addr[4] && \
			hidha_cntx[i].hidha_bd_addr.addr[5] == bt_addr_p->addr[5] && \
			hidha_cntx[i].in_use)
        {
            cntx = &hidha_cntx[i];
            break;
        }
    }
    return cntx;	
}

static hidha_context_struct *hidha_find_context_by_ch(HidChannel *channel)
{
    hidha_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        if ((hidha_cntx[i].hidChannel == channel) && 
             hidha_cntx[i].in_use)
        {
            cntx = &hidha_cntx[i];
            break;
        }
    }
    return cntx;	
}

/*****************************************************************************
 * FUNCTION
 *  hidha_send_msg
 * DESCRIPTION
 *  send message to external task.
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
static void hidha_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
#if 0
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilmPtr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = msg;
    ilmPtr->local_para_ptr = local_para;
    ilmPtr->peer_buff_ptr = peer_buff;
    ilmPtr->dest_mod_id = dstMod;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = sap;
    msg_send_ext_queue(ilmPtr);
#endif


    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hidha_send_msg_ext
 * DESCRIPTION
 *  This function is to change the hidda state
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  result          [IN]        
 *  bt_addr_p       [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void hidha_send_msg_ext(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        kal_uint8 result,
        hidha_context_struct *cntx,
        BD_ADDR *bt_addr_p)
{
    if(NULL == bt_addr_p)
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_send_msg_ext: NULL == bt_addr_p");
        return ;
    }
    
    switch (msg)
    {
        case MSG_ID_BT_HIDH_DESCINFO_CNF:
        {
            bt_hidh_query_cnf_struct *msg_p;

            msg_p = (bt_hidh_query_cnf_struct*) construct_local_para(sizeof(bt_hidh_query_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                msg_p->connection_id = cntx->hidha_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = *bt_addr_p;                    
            hidha_send_msg(MSG_ID_BT_HIDH_DESCINFO_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;
        }
        
        case MSG_ID_BT_HIDH_CONNECT_CNF:
        {
            /* send connect cnf to MMI */
            bt_hidh_connect_cnf_struct *msg_p;

            msg_p = (bt_hidh_connect_cnf_struct*) construct_local_para(sizeof(bt_hidh_connect_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                cntx->hidha_connection_id = (U32) BTBMProfileAllocateConnIndex(SC_HUMAN_INTERFACE_DEVICE, *bt_addr_p);
                cntx->hidha_bd_addr = *bt_addr_p;
                msg_p->connection_id = cntx->hidha_connection_id;
                msg_p->descList = cntx->descStr;
                msg_p->descLen = cntx->descLen;
            }
            else
            {
                //hidha_close_driver(cntx);
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = *bt_addr_p;
			
            hidha_send_msg(MSG_ID_BT_HIDH_CONNECT_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;
        }

        case MSG_ID_BT_HIDH_CONNECT_IND:
        {
            bt_hidh_connect_ind_struct *msg_p;

            msg_p = (bt_hidh_connect_ind_struct*) construct_local_para(sizeof(bt_hidh_connect_ind_struct), TD_UL);
            if (result == hidd_result_ok)
            {
                cntx->hidha_connection_id = (U32) BTBMProfileAllocateConnIndex(SC_HUMAN_INTERFACE_DEVICE, *bt_addr_p);
                cntx->hidha_bd_addr = *bt_addr_p;
                msg_p->connection_id = cntx->hidha_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = *bt_addr_p;
            msg_p->result = result;
            hidha_send_msg(MSG_ID_BT_HIDH_CONNECT_IND, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;
        }

        case MSG_ID_BT_HIDH_DISCONNECT_CNF:
        {
            bt_hidh_disconnect_cnf_struct *msg_p;

            msg_p = (bt_hidh_disconnect_cnf_struct*) construct_local_para(sizeof(bt_hidh_disconnect_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                msg_p->connection_id = cntx->hidha_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = *bt_addr_p;
            hidha_send_msg(MSG_ID_BT_HIDH_DISCONNECT_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;
        }

        case MSG_ID_BT_HIDH_DISCONNECT_IND:
        {
            bt_hidh_disconnect_ind_struct *msg_p;

            msg_p = (bt_hidh_disconnect_ind_struct*) construct_local_para(sizeof(bt_hidh_disconnect_ind_struct), TD_UL);
            msg_p->connection_id = cntx->hidha_connection_id;
            msg_p->bt_addr = *bt_addr_p;
            hidha_send_msg(MSG_ID_BT_HIDH_DISCONNECT_IND, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;
        }

        case MSG_ID_BT_HIDH_INTERRUPT_DATA_CNF:
        {
            bt_hidh_interrupt_data_cnf_struct *msg_p;

            msg_p = (bt_hidh_interrupt_data_cnf_struct*) construct_local_para(sizeof(bt_hidh_interrupt_data_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                msg_p->connection_id = cntx->hidha_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = *bt_addr_p;
            //msg_p->report_type = hidd_report_input;            
            
            hidha_send_msg(MSG_ID_BT_HIDH_INTERRUPT_DATA_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;
        }
            
        default:
            break;
            //Assert(0);
    }

}

extern void hid_virtual_cable_unplug_callback(BD_ADDR *hidha_bd_addr);
static void hidha_clear_connection(hidha_context_struct *cntx)
{
    if(TRUE == cntx->bVirtCableFlag)
    {
        cntx->bVirtCableFlag = FALSE;
        hid_virtual_cable_unplug_callback(&cntx->hidha_bd_addr);
        bt_prompt_trace(MOD_BT, "[HID] hid_virtual_cable_unplug_callback");
    }
    /* The outgong data buffer shall be release in HIDEVENT_TRANSACTION_COMPLETE
          or HIDEVENT_INTERRUPT_COMPLETE event. Refer to HidClearConnection().
          Only free the memory used to queue incoming data here.
    */
    HID_MEM_FREEIF(cntx->report.data);
    HID_MEM_FREEIF(cntx->input_report.data);
    //add by zhangsg
    //HID_MEM_FREEIF(cntx->reportCons);
    //cntx->reportCons = NULL;
	

    if (cntx->hidha_connection_id != HID_INVALID_CONN_ID)
    {
        BTBMProfileReleaseConnIndex((U8) cntx->hidha_connection_id);
    }


    if (!cntx->bReconnect)
    {
        btmtk_os_memset((U8*)cntx, 0, sizeof(hidha_context_struct));
    }
    else
    {
        // If reconnect is necessary, do not reset sdp attribute here. And keep the device in use state.
        cntx->flag = 0;
        cntx->hidChannel = NULL;
        btmtk_os_memset((U8*)&cntx->report, 0, sizeof(HidReport));
        btmtk_os_memset((U8*)&cntx->input_report, 0, sizeof(HidReport));
        btmtk_os_memset((U8*)&cntx->gTrans, 0, sizeof(cntx->gTrans));
        cntx->bConnInitiator = FALSE;
        cntx->bConnTerminator = FALSE;
        cntx->hidha_active = FALSE;
		cntx->bSdpQueryDone = FALSE;
        cntx->in_use = FALSE;
    }

    cntx->hidha_connection_id = HID_INVALID_CONN_ID;

}

static void hidha_reconnect(EvmTimer *timer)
{
    bt_hidh_reconnect_req_struct *req_p = (bt_hidh_reconnect_req_struct*) construct_local_para(sizeof(bt_hidh_reconnect_req_struct), TD_UL);
    hidha_context_struct *cntx = timer->context;
bt_prompt_trace(MOD_BT, "[BT] hidha_reconnect");
    req_p->bt_addr = cntx->hidha_bd_addr;
    req_p->connection_id = cntx->hidha_connection_id;
    hidha_send_msg(MSG_ID_BT_HIDH_RECONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct *) req_p, NULL);
}

static void hidha_reconnect_start_timer(hidha_context_struct *cntx, U16 timer_ms)
{
    cntx->reconnect_timer.func = hidha_reconnect;
    cntx->reconnect_timer.context = (void *)cntx;
    EVM_StartTimer(&(cntx->reconnect_timer), timer_ms);

    hidha_reconnect_retry++;
}

static void hidha_reconnect_stop_timer(hidha_context_struct *cntx)
{
    EVM_CancelTimer(&(cntx->reconnect_timer));
    cntx->bReconnect = FALSE;
    hidha_reconnect_retry = 0;
}

BtStatus btmtk_adp_hidh_virtual_cable_unplug_req(BD_ADDR *bt_addr)		
{
    BtStatus        status = BT_STATUS_FAILED;
	
    hidha_context_struct *cntx = hidha_find_context_by_addr(bt_addr);

    if ((cntx == NULL) || (cntx->flag & HIDDA_TRANSA_FLAG_CONTROL) || (TRUE == cntx->bVirtCableFlag))
    {
        return status;
    }

    /* send unplug command to remote device */
    cntx->flag |= HIDDA_TRANSA_FLAG_CONTROL;
    cntx->gTrans[hidda_control].parm.control = (HidControl)hidd_ctrl_op_virtual_cable_unplug;
    status = HID_SendControl(cntx->hidChannel, &cntx->gTrans[hidda_control]);

    bt_prompt_trace(MOD_BT, "[HID]btmtk_adp_hidh_virtual_cable_unplug_req:status = %d",status);

    if (status == BT_STATUS_SUCCESS || status == BT_STATUS_PENDING)
    {
        cntx->bVirtCableFlag = TRUE;
    }
    else
    {
        cntx->bVirtCableFlag = FALSE;
    }

    return status;
}

void hidha_event_leds(hidha_context_struct *cntx, U16 OutputEventCode)
{
    bt_hidh_interrupt_data_ind_struct *ind;
    
    ind = (bt_hidh_interrupt_data_ind_struct *)construct_local_para(sizeof(*ind), TD_UL);
    //ind->report_type = pIntr->reportType;
    ind->data_len = cntx->input_report.dataLen;
    ind->connection_id = cntx->hidha_connection_id;
    ind->bt_addr = cntx->hidha_bd_addr;

    switch (OutputEventCode) 
    {
        case 1:
            ind->data = KEY_CAPS_LOCK_USAGE;
            break;
        
        default:
            bt_prompt_trace(MOD_BT, "[HID]hidha_event_leds: for to do !");
    }

    hidha_send_msg(
        MSG_ID_BT_HIDH_INTERRUPT_DATA_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct *)ind,
        NULL);

}

#endif
