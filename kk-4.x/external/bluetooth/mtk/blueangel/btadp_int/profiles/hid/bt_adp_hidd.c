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
 * bt_adp_hidd.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file handles HID message 
 *
 * Author:
 * -------
 * Elvis Lin
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * Jun 3 2009 mtk80035
 * [MAUI_01751667] [Bt]Desktop interface,the button "ALT+Tab" have not alignment
 * 
 *
 * Mar 12 2009 mtk80035
 * [MAUI_01406911] [Bluetooth] Btadp folder restruct activity
 * 
 *
 * Jan 7 2009 mbj06038
 * [MAUI_01544791] Screen always displays Connecting
 * 
 *
 * Nov 18 2008 mbj06038
 * [MAUI_01338633] [BT]The device name in HID Requeat screen disappeared
 * 
 *
 * Nov 3 2008 mbj06038
 * [MAUI_01256277] [BT HID] Disconnect HID from PC, MS won't display disconnect message and MS also can
 * 
 *
 * Jun 20 2008 mbj06038
 * [MAUI_01079127] [BT HID]Revise HID trace
 * 
 *
 * Apr 7 2008 Mbj06038
 * [MAUI_00651492] Compile warning
 * 
 *
 * Apr 5 2008 Mbj06038
 * [MAUI_00651492] Compile warning
 * 
 *
 * Mar 4 2008 Mbj06038
 * [MAUI_00633098] [HID] adapter release connection ID error
 * 
 *
 * Nov 2 2007 MBJ06017
 * [MAUI_00033624] "Connect failed" pop up appears on selecting Remote control.
 * when connection failed becasue SCO, it will tell user the detail reason
 *
 * Oct 8 2007 mtk00560
 * [MAUI_00556510] [BT]Shrinking BT code size
 * 
 *
 * Jul 12 2007 mtk00560
 * [MAUI_00407534] UPF_BT_PTS_HID Profile's failed in  TC_DEV_HID_BI_01_C, TC_DEV_HID_BI_02_C
 * 
 *
 * Jul 4 2007 mtk00560
 * [MAUI_00407534] UPF_BT_PTS_HID Profile's failed in  TC_DEV_HID_BI_01_C, TC_DEV_HID_BI_02_C
 * 
 *
 * Apr 9 2007 mtk00560
 * [MAUI_00379977] Bluetooth_The screen will pop up ¡§Device Busy ¡¨ when disconnect the Handsfree prof
 * 
 *
 * Mar 15 2007 mtk00560
 * [MAUI_00421706] [Bluetooth] Pop up "Disconnect Faild",but in fact MS have disconnected with HSK
 * 
 *
 * Mar 8 2007 mtk00560
 * [MAUI_00230545] Assert fail: 0 bt_adp_hidd.c 710 - BT
 * 
 *
 * Mar 5 2007 mtk00560
 * [MAUI_00369338] [1]Assert fail:(hidda_cntx_p->num_intr_queue <=5) bt_adp_hidd.c 955-BT
 * 
 *
 * Feb 26 2007 mtk00560
 * [MAUI_00367691] [BT][HID] HID new feature check-in
 * 
 *
 * Feb 16 2007 mtk00560
 * [MAUI_00367691] [BT][HID] HID new feature check-in
 *    
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifdef __BT_HIDD_PROFILE__

#include "stdio.h"              /* Basic C I/O functions */
#ifndef __LINUX_SUPPRESS_ERROR__
#ifndef BTMTK_ON_WISE
#include <Windows.h>
#endif
#endif
//#include "bt.h"
#include "bt_os_api.h"
//#include "osapi.h"
#include "hid.h"

//#include "kal_trace.h"  /* Trace Header Files */
//#include "bluetooth_trc.h"      /* bt trace definition */

#include "bluetooth_struct.h"
//#include "bluetooth_bm_struct.h"
#include "btbm_adp.h"
//#include "bt_dm_struct.h"
#include "sdp_adp.h"
#include "bluetooth_hid_struct.h"

#include "bt_adp_hidd.h"
#include "bt_adp_hidh.h"


/***************************************************************************** 
 * Definations
 ****************************************************************************/
#define NUM_INTR_STRUCTS 5
#define HIDDA_MOUSE_CTRL_DATA_TERMINATION  4
#define HIDDA_KBD_CTRL_DATA_TERMINATION  9


/***************************************************************************** 
* Globol Variable
*****************************************************************************/
typedef struct _hidda_context_struct
{
    U32                  flag;    
    BOOL	 hidda_active;
    BOOL 	bConnInitiator;
    BOOL 	bConnTerminator;	
		
    bt_hidd_state hidda_state;

    U32 hidda_connection_id;
    BD_ADDR   hidda_bd_addr;

    HidChannel           *hidChannel;
    ListEntry            intrPool;
    HidTransaction       gTrans[hidda_trasation_type_total];
    HidReport            report; 	// to queue the uncomplete report data in set report request
    HidReport            output_report;		// to queue the uncomplete output report data in interrupt channel
    /* Contains the current protocol */
    HidProtocol          protocol;
    /* Contains the idle rate */
    HidIdleRate          idleRate;
} hidda_context_struct;


hidda_context_struct hidda_cntx;
hidda_context_struct *hidda_cntx_p = &hidda_cntx;
const kal_uint8 mouse_ctrl_data_termination[HIDDA_MOUSE_CTRL_DATA_TERMINATION]={0x02, 0x00, 0x00, 0x00};
const kal_uint8 kbd_ctrl_data_termination[HIDDA_KBD_CTRL_DATA_TERMINATION]={0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/***************************************************************************** 
* External functions
*****************************************************************************/
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);


/***************************************************************************** 
* Internal functions
*****************************************************************************/
static void hidda_handle_connected(HidChannel *Channel, BD_ADDR *bt_addr_p);
static void hidda_handle_disconnected(BD_ADDR *bt_addr_p);
static void hidda_handle_transaction_ind(HidTransaction * pTrans, BtStatus status);
static void hidda_handle_control_ind(bt_hidd_ctrl_op_enum ctrl_op);
static void hidda_handle_set_report_ind(HidReport *report, BtStatus status);
static void hidda_handle_transaction_cmplt(HidTransaction *pTrans, BtStatus status);
static void hidda_handle_interrupt_ind(HidInterrupt *intr, BtStatus status);
static void hidda_handle_interrupt_cmplt(HidInterrupt *pIntr, BtStatus status);
static void hidda_handle_connect_authorize_ind(HidChannel *Channel, BD_ADDR *bt_addr_p);
static void hidda_get_device_name(kal_uint8 *name, kal_uint32 size, kal_uint8 *addr);
static void hidda_clear_connection(hidda_context_struct *cntx);

/*****************************************************************************
 * FUNCTION
 *  hidda_init
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  void       
 * RETURNS
 *  void
 *****************************************************************************/
void hidda_init(void)
{

    btmtk_os_memset((U8*) hidda_cntx_p, 0, sizeof(hidda_context_struct));
    hidda_cntx_p->hidda_connection_id = HID_INVALID_CONN_ID;
    
    #ifdef HID_BOOT_DEVICE
    hidda_cntx_p->protocol = HID_PROTOCOL_BOOT;
    #else
    hidda_cntx_p->protocol = HID_PROTOCOL_REPORT;
    #endif
    hidda_cntx_p->idleRate = 0;
    InitializeListHead(&hidda_cntx_p->intrPool);
}


void btmtk_adp_hid_handle_message(void *msg)
{
    ilm_struct *msg_p = (ilm_struct *)msg;

    if ((msg_p->msg_id >= MSG_ID_BT_HIDD_DEVICE_START) && (msg_p->msg_id <= MSG_ID_BT_HIDD_DEVICE_END))
    {
        hidd_handle_message(msg_p);
    }
    else if ((msg_p->msg_id >= MSG_ID_BT_HIDH_HOST_START) && (msg_p->msg_id <= MSG_ID_BT_HIDH_HOST_END))
    {
        hidh_handle_message(msg_p);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hidda_handler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef __LINUX_SUPPRESS_ERROR__
void hidd_handle_message(ilm_struct *ilm_ptr)
#else
static void hidd_handle_message(ilm_struct *ilm_ptr)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_HIDD_ACTIVATE_REQ:
            btmtk_adp_hidd_handle_activate_req(ilm_ptr);
            break;
			
        case MSG_ID_BT_HIDD_DEACTIVATE_REQ:
            btmtk_adp_hidd_handle_deactivate_req(ilm_ptr);
            break;
			
        case MSG_ID_BT_HIDD_CONNECT_REQ:
            btmtk_adp_hidd_handle_connect_req(ilm_ptr);
            break;
			
        case MSG_ID_BT_HIDD_DISCONNECT_REQ:
            btmtk_adp_hidd_handle_disconnect_req(ilm_ptr);
            break;
			
        case MSG_ID_BT_HIDD_UNPLUG_REQ:
            btmtk_adp_hidd_handle_unplug_req(ilm_ptr);
            break; 			

        case MSG_ID_BT_HIDD_INTERRUPT_DATA_REQ:
            btmtk_adp_hidd_handle_send_input_report(ilm_ptr);
            break;
			
        case MSG_ID_BT_HIDD_CONNECTION_AUTHORIZE_RSP:
            btmtk_adp_hidd_handle_connect_authorize_rsp(ilm_ptr);
            break;
			
        default:
            break;

    }

}


/*****************************************************************************
 * FUNCTION
 *  hidda_app_callback
 * DESCRIPTION
 *  application callback of hid device
 * PARAMETERS
 *  Channel     [IN]     
 *  Parms       [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void hidda_app_callback(HidChannel *Channel, HidCallbackParms *Parms)
{
    switch (Parms->event) 
    {
        case HIDEVENT_OPEN_IND:
            hidda_handle_connect_authorize_ind(Channel, &(Parms->ptrs.remDev->bdAddr));
            break;
    		
        case HIDEVENT_OPEN:
            kal_trace(BT_TRACE_G2_PROFILES,CONNECTION_OPENED);
            hidda_handle_connected(Channel, &(Parms->ptrs.remDev->bdAddr));
            break;
    		
        case HIDEVENT_CLOSE_IND:
            kal_trace(BT_TRACE_G2_PROFILES,CLOSE_INDICATION_RECEIVED);
            break;
    		
        case HIDEVENT_CLOSED:
            kal_trace(BT_TRACE_G2_PROFILES,CONNECTION_CLOSED);
            hidda_handle_disconnected(&(Parms->ptrs.remDev->bdAddr));
            break;
    		
        case HIDEVENT_TRANSACTION_IND:
            kal_trace(BT_TRACE_G2_PROFILES,TRANSACTION_RECEIVED);
            hidda_handle_transaction_ind(Parms->ptrs.trans, (BtStatus)Parms->status);       		
            break;
    		
        case HIDEVENT_TRANSACTION_COMPLETE:
            kal_trace(BT_TRACE_G2_PROFILES,TRANSACTION_COMPLETE_STATUS__x0X2 , Parms->status);
            hidda_handle_transaction_cmplt(Parms->ptrs.trans, (BtStatus)Parms->status);
            break;
    		
        case HIDEVENT_INTERRUPT:
            // output report from remote device		
            kal_trace(BT_TRACE_G2_PROFILES,INTERRUPT_RECEIVED);
            hidda_handle_interrupt_ind(Parms->ptrs.intr, (BtStatus)Parms->status);
            break;
    
        case HIDEVENT_INTERRUPT_COMPLETE:
            kal_trace(BT_TRACE_G2_PROFILES,BT_HIDD_ADP_SEND_COMPLETE_STATUS, Parms->status, 0);
            hidda_handle_interrupt_cmplt(Parms->ptrs.intr, (BtStatus)Parms->status);
            break;
    
        default:
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hidda_activate_req_hdler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_hidd_handle_activate_req(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_hidd_activate_cnf_struct *cnf_p;
    bt_hidd_activate_req_struct *req_p = (bt_hidd_activate_req_struct *)ilm_ptr->local_para_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    cnf_p = (bt_hidd_activate_cnf_struct*) construct_local_para(sizeof(bt_hidd_activate_cnf_struct), TD_UL);

    // set sdp attribute
    cnf_p->sdpAttr = req_p->sdpAttr;
    if (HID_RegisterDevice(hidda_app_callback) != BT_STATUS_SUCCESS) 
    {
       cnf_p->result = hidd_result_failed;
    }
    else
    {
       cnf_p->result = hidd_result_ok;				
    }

    hidda_send_msg(MSG_ID_BT_HIDD_ACTIVATE_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) cnf_p, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  hidda_deactivate_req_hdler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_hidd_handle_deactivate_req(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_hidd_deactivate_cnf_struct *msg_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);

    
    msg_p = (bt_hidd_deactivate_cnf_struct*) construct_local_para(sizeof(bt_hidd_deactivate_cnf_struct), TD_UL);
    if (HID_DeregisterDevice() != BT_STATUS_SUCCESS)
    {
        msg_p->result = hidd_result_failed;
    }
    else
    {
        msg_p->result = hidd_result_ok;
    }
    hidda_send_msg(MSG_ID_BT_HIDD_DEACTIVATE_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) msg_p, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  hidda_connect_req_hdler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_hidd_handle_connect_req(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status = BT_STATUS_FAILED;
    bt_hidd_connect_req_struct *msg_p = (bt_hidd_connect_req_struct*) ilm_ptr->local_para_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);
    
    
    status =  HID_OpenConnection(&hidda_cntx_p->hidChannel, &msg_p->bt_addr);
    
    if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
    {
        /* send connect cnf */
        if (status == BT_STATUS_SCO_REJECT)
        {
            hidda_send_msg_ext(
                MSG_ID_BT_HIDD_CONNECT_CNF,
                MOD_EXTERNAL,
                BT_APP_SAP,
                hidd_result_sco_reject,
                &msg_p->bt_addr);
            return;
        }
        else
        {
            hidda_send_msg_ext(
                MSG_ID_BT_HIDD_CONNECT_CNF,
                MOD_EXTERNAL,
                BT_APP_SAP,
                hidd_result_failed,
                &msg_p->bt_addr);
            return;
        }
    }

    hidda_cntx_p->bConnInitiator = TRUE;
    /* start a timer to guard rsp time for upper layer */
    //hidda_start_timer(&(hidda_cntx_p->hidda_request_guard_timer_id), CONNECT_REQ_TIMER_DURATION, hidda_connect_req_timeout_hdler);

}

/*****************************************************************************
 * FUNCTION
 *  hidda_disconnect_req_hdler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_hidd_handle_disconnect_req(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);

    HID_CloseConnection(hidda_cntx_p->hidChannel);

    hidda_cntx_p->bConnTerminator = TRUE;
    #if 0
    /* start a timer to guard rsp time for upper layer */
    hidda_start_timer(&(hidda_cntx_p->hidda_request_guard_timer_id), DISCONNECT_REQ_TIMER_DURATION, hidda_disconnect_req_timeout_hdler);
    #endif

}

void btmtk_adp_hidd_handle_unplug_req(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);

    if (!hidda_cntx_p->hidda_active)
    {
        return;
    }

    hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_CONTROL;
    hidda_cntx_p->gTrans[hidda_control].parm.control = HID_CTRL_VIRTUAL_CABLE_UNPLUG;

    HID_SendControl(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_control]);
	
}

/*****************************************************************************
 * FUNCTION
 *  hidda_send_interrupt_data_req_hdler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_hidd_handle_send_input_report(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    HidInterrupt    *pIntr;
    BtStatus status = BT_STATUS_SUCCESS;
    bt_hidd_interrupt_data_req_struct *msg_p = (bt_hidd_interrupt_data_req_struct*) ilm_ptr->local_para_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);

    if (!hidda_cntx_p->hidda_active)
    {
        return;
    }
    

    /* allocate buffer  */
    /* it shall be freed while receiving HIDEVENT_INTERRUPT_COMPLETE in hidda_app_callback */
    pIntr = (HidInterrupt*) HID_MEM_MALLOC(sizeof(HidInterrupt));
    pIntr->data =  msg_p->data_ptr;
    pIntr->dataLen =  msg_p->data_len;
    pIntr->reportType =  msg_p->report_type;

          
   /* Send the interrupt */
   /* The input report data shall be queued in profile level */
   status = HID_SendInterrupt(hidda_cntx_p->hidChannel, pIntr);
   kal_trace(BT_TRACE_G2_PROFILES,BT_HIDD_ADP_SEND_STATUS,status);
   if (status == BT_STATUS_PENDING) 
   {
       kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_INTERRUPT);
   } 
   else 
   {
       /* release control buffer */
       HID_MEM_FREEIF(pIntr->data);
       HID_MEM_FREEIF(pIntr);
       /* send connect cnf  */
       hidda_send_msg_ext(
           MSG_ID_BT_HIDD_INTERRUPT_DATA_CNF,
           MOD_EXTERNAL,
           BT_APP_SAP,
           hidd_result_failed,
           &hidda_cntx_p->hidda_bd_addr);
   }

}
/*****************************************************************************
 * FUNCTION
 *  hidda_connect_authorize_rsp_hdler
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  ilm_ptr       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_hidd_handle_connect_authorize_rsp(ilm_struct *ilm_ptr)
{
    bt_hidd_connection_authorize_rsp_struct *msg_p = (bt_hidd_connection_authorize_rsp_struct*) ilm_ptr->local_para_ptr;

    if (msg_p->result == hidd_connect_authorization_result_accepted)
    {
        BtStatus status = HID_AcceptConnection(hidda_cntx_p->hidChannel); 
        if ((status != BT_STATUS_PENDING) &&
		(status != BT_STATUS_SUCCESS))
        {
            kal_trace(BT_TRACE_G2_PROFILES,COULD_NOT_ACCEPT_THE_INCOMING_CONNECTION, status);

            /* send connect cnf  */
            hidda_send_msg_ext(
                MSG_ID_BT_HIDD_CONNECT_IND,
                MOD_EXTERNAL,
                BT_APP_SAP,
                hidd_result_failed,
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
        HID_RejectConnection(hidda_cntx_p->hidChannel);
    }

}

/*****************************************************************************
 * FUNCTION
 *  hidda_connected
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  bt_addr_p       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void hidda_handle_connected(HidChannel *Channel, BD_ADDR *bt_addr_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);
    

    if (!hidda_cntx_p->bConnInitiator)
    {
        hidda_cntx_p->hidChannel = Channel;
        /* send connect ind  */
        hidda_send_msg_ext(
            MSG_ID_BT_HIDD_CONNECT_IND,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            bt_addr_p);
    }
    else
    {
        hidda_cntx_p->bConnInitiator = FALSE;		
        /* send connect cnf */
        hidda_send_msg_ext(
            MSG_ID_BT_HIDD_CONNECT_CNF,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_ok,
            bt_addr_p);
    }

    hidda_cntx_p->hidda_active = TRUE;

}


/*****************************************************************************
 * FUNCTION
 *  hidda_disconnected
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  bt_addr_p       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void hidda_handle_disconnected(BD_ADDR *bt_addr_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(
        BT_TRACE_G2_PROFILES,
        BT_HIDD_ADP_STATUS,
        hidda_cntx_p->hidda_state,
        hidda_cntx_p->hidda_active,
        0,
        0);
#ifndef __LINUX_SUPPRESS_ERROR__    
#ifndef BTMTK_ON_WISE
            Sleep(2000);
#endif
#endif /* __LINUX_SUPPRESS_ERROR__ */
    if (!hidda_cntx_p->hidda_active)
    {
        msg_type type; 
        /* channel closed during connection establishing procedure */
        if (hidda_cntx_p->bConnInitiator)
        {
            type = MSG_ID_BT_HIDD_CONNECT_CNF;
        }
        else
        {
            type = MSG_ID_BT_HIDD_CONNECT_IND;
        }

        hidda_send_msg_ext(
            type,
            MOD_EXTERNAL,
            BT_APP_SAP,
            hidd_result_failed,
            bt_addr_p);
    }
    else
    {
	if (hidda_cntx_p->bConnTerminator)
	{
              hidda_cntx_p->bConnTerminator = FALSE;
              
              /* send disconnect cnf */
              hidda_send_msg_ext(
              	MSG_ID_BT_HIDD_DISCONNECT_CNF,
              	MOD_EXTERNAL,
              	BT_APP_SAP,
              	hidd_result_ok,
              	bt_addr_p);
	}
	else
	{
             /* send disconnect ind */
             hidda_send_msg_ext(
             	MSG_ID_BT_HIDD_DISCONNECT_IND,
             	MOD_EXTERNAL,
             	BT_APP_SAP,
             	hidd_result_ok,
             	bt_addr_p);
	}
    }

    hidda_clear_connection(hidda_cntx_p);	
}


void hidda_handle_transaction_ind(HidTransaction *pTrans, BtStatus status)
{
    BtStatus state = BT_STATUS_SUCCESS;

    switch (pTrans->type) 
    {
        //Add by stanley: 2007-0607
        case HID_HANDSHAKE:
            kal_trace(BT_TRACE_G2_PROFILES, SETUP_HID_HANDSHACK_PAYLOAD_PACKET);
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_HANDSHAKE));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_HANDSHAKE;
            hidda_cntx_p->gTrans[hidda_handshake].type = pTrans->type;
            hidda_cntx_p->gTrans[hidda_handshake].resultCode = pTrans->resultCode;
            //status = HID_DeviceSendHandShake(Channel,Parms->ptrs.trans);
            state = HID_DeviceSendHandShake(hidda_cntx_p->hidChannel,&hidda_cntx_p->gTrans[hidda_handshake]);
            if(state == BT_STATUS_PENDING)
                kal_trace(BT_TRACE_G2_PROFILES,SENDING_HANDSHAKE_PACKET);
            else 
                kal_trace(BT_TRACE_G2_PROFILES,SENDING_HANDSHAKE_PACKET_WIT_ERROR_STATUSxD ,state);
            break;
			
        case HID_TRANS_CONTROL:
            kal_trace(BT_TRACE_G2_PROFILES,CONTROL);
            hidda_handle_control_ind((bt_hidd_ctrl_op_enum)pTrans->parm.control);
            break;
			
        case HID_TRANS_GET_REPORT:
            kal_trace(BT_TRACE_G2_PROFILES,GET_REPORT);
            
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_GET_REPORT));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_GET_REPORT;
            
            //pTrans->parm.reportReq
            hidda_cntx_p->report.data = (U8*)kbd_ctrl_data_termination;
            hidda_cntx_p->report.dataLen = min(sizeof(kbd_ctrl_data_termination), pTrans->parm.reportReq->bufferSize);
            hidda_cntx_p->report.reportType = HID_REPORT_INPUT;
            hidda_cntx_p->gTrans[hidda_get_report].parm.report = &hidda_cntx_p->report;
            state = HID_DeviceGetReportRsp(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_get_report]);
            if (state == BT_STATUS_PENDING) 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_GET_REPORT_RESPONSE);
            } 
            else 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_COULD_NOT_SEND_GET_REPORT_RESPONSE_REASON_x02X , status);
            }
            break;
			
        case HID_TRANS_SET_REPORT:
            kal_trace(BT_TRACE_G2_PROFILES,SET_REPORT_REPORT_DATA_LEN__xD , pTrans->parm.report->dataLen);
            
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_SET_REPORT));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_SET_REPORT;

            hidda_handle_set_report_ind(pTrans->parm.report, status);
            break;
			
        case HID_TRANS_GET_PROTOCOL:
            kal_trace(BT_TRACE_G2_PROFILES,GET_PROTOCOL);
            
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_GET_PROTO));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_GET_PROTO;
            
            hidda_cntx_p->gTrans[hidda_get_protocol].parm.protocol = hidda_cntx_p->protocol;
            
            state = HID_DeviceGetProtocolRsp(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_get_protocol]);
            if (state == BT_STATUS_PENDING) 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_GET_PROTOCOL_RESPONSE);
            } 
            else 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_COULD_NOT_SEND_GET_PROTOCOL_RESPONSE_REASON_x02X , status); 
            }
            break;
			
        case HID_TRANS_SET_PROTOCOL:
            kal_trace(BT_TRACE_G2_PROFILES,SET_PROTOCOL_PROTOCOL__x02X , pTrans->parm.protocol);
            
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_SET_PROTO));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_SET_PROTO;
            hidda_cntx_p->protocol = pTrans->parm.protocol;
            
            hidda_cntx_p->gTrans[hidda_set_protocol].resultCode = HID_RESULT_SUCCESS;
            state = HID_DeviceSetProtocolRsp(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_set_protocol]);
            if (state == BT_STATUS_PENDING) 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_SET_PROTOCOL_RESPONSE);
            } 
            else 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_COULD_NOT_SEND_SET_PROTOCOL_RESPONSE_REASON_x02X , status);
            }
            break;
			
        case HID_TRANS_GET_IDLE_RATE:
            kal_trace(BT_TRACE_G2_PROFILES,GET_IDLE);
            
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_GET_IDLE_RATE));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_GET_IDLE_RATE;
            
            hidda_cntx_p->gTrans[hidda_get_idle_rate].parm.idleRate = hidda_cntx_p->idleRate;
            state = HID_DeviceGetIdleRsp(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_get_idle_rate]);
            if (state == BT_STATUS_PENDING) 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_GET_IDLE_RATE_RESPONSE);
            } 
            else 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_COULD_NOT_SEND_GET_IDLE_RATE_RESPONSE_REASON_x02X , status);
            }
            break;
			
        case HID_TRANS_SET_IDLE_RATE:
            kal_trace(BT_TRACE_G2_PROFILES,SET_IDLE_IDLE_RATE__xD , pTrans->parm.idleRate);
            
            Assert(!(hidda_cntx_p->flag & HIDDA_TRANSA_FLAG_SET_IDLE_RATE));
            hidda_cntx_p->flag |= HIDDA_TRANSA_FLAG_SET_IDLE_RATE;
            hidda_cntx_p->idleRate = pTrans->parm.idleRate;
            
            hidda_cntx_p->gTrans[hidda_set_idle_rate].resultCode = HID_RESULT_SUCCESS;
            state = HID_DeviceSetIdleRsp(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_set_idle_rate]);
            if (state == BT_STATUS_PENDING) 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_SET_IDLE_RATE_RESPONSE);
            } 
            else 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_COULD_NOT_SEND_SET_IDLE_RATE_RESPONSE_REASON_x02X , status); 
            }
            break;

        default:
            break;
    }
    
}


/*****************************************************************************
 * FUNCTION
 *  hidda_cable_unplugged
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
static void hidda_handle_control_ind(bt_hidd_ctrl_op_enum ctrl_op)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_hidd_control_ind_struct *msg_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (!hidda_cntx_p->hidda_active)
    {
        return;
    }
	
    msg_p = (bt_hidd_control_ind_struct *)construct_local_para(sizeof(*msg_p), TD_UL);
    msg_p->connection_id = hidda_cntx_p->hidda_connection_id;
    msg_p->bt_addr = hidda_cntx_p->hidda_bd_addr;
    msg_p->ctrl_op = ctrl_op;
    hidda_send_msg(MSG_ID_BT_HIDD_CONTROL_IND, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct *)msg_p, NULL);
		
}

/*****************************************************************************
 * FUNCTION
 *  hidda_transaction_ind
 * DESCRIPTION
 *  This function is to notify SET_REPORT event
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
static void hidda_handle_set_report_ind(HidReport *report, BtStatus status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *data_ptr = hidda_cntx_p->report.data;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    hidda_cntx_p->report.reportType = report->reportType;

    if (report->dataLen > 0)
    {
        data_ptr += hidda_cntx_p->report.dataLen;
        data_ptr = (U8 *)HID_MEM_MALLOC(report->dataLen);
        btmtk_os_memcpy(data_ptr, report->data, report->dataLen);
        hidda_cntx_p->report.dataLen += report->dataLen;
        if (hidda_cntx_p->report.data == NULL)
        {
            hidda_cntx_p->report.data = data_ptr;
        }
    }

    if (status == BT_STATUS_SUCCESS)
    {
        bt_hidd_set_report_ind_struct *para;
    
        para = (bt_hidd_set_report_ind_struct *)construct_local_para(sizeof(*para), TD_UL);
        para->report_type = hidda_cntx_p->report.reportType;
        para->data_len = hidda_cntx_p->report.dataLen;
        para->data_ptr = (U8 *)HID_MEM_MALLOC(para->data_len);
        btmtk_os_memcpy(para->data_ptr, hidda_cntx_p->report.data, para->data_len);
    
        para->connection_id = hidda_cntx_p->hidda_connection_id;
        para->bt_addr = hidda_cntx_p->hidda_bd_addr;
    
        hidda_send_msg(
            MSG_ID_BT_HIDD_SET_REPORT_IND,
            MOD_EXTERNAL,
            BT_APP_SAP,
            (local_para_struct *)para,
            NULL);

        HID_MEM_FREEIF(hidda_cntx_p->report.data);
        btmtk_os_memset((U8 *)&hidda_cntx_p->report, 0, sizeof(hidda_cntx_p->report));


        /* To notify the remote host that SET_REPORT command is received, with HANDSHAKE response */
        hidda_cntx_p->gTrans[hidda_set_report].resultCode = HID_RESULT_SUCCESS;
        status = HID_DeviceSetReportRsp(hidda_cntx_p->hidChannel, &hidda_cntx_p->gTrans[hidda_set_report]);
        if (status == BT_STATUS_PENDING) 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_SENDING_SET_REPORT_RESPONSE);
        } 
        else 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_COULD_NOT_SEND_SET_REPORT_RESPONSE_REASON_x02X , status); 
        }
    }
    else if (status == BT_STATUS_PENDING)
    {
        /* if status is pending, report data shall be queued until completed */
    }

}

static void hidda_handle_transaction_cmplt(HidTransaction *pTrans, BtStatus status)
{
    kal_trace(BT_TRACE_G2_PROFILES,BT_HIDD_ADP_TRANSATION_COMPLETE , status, pTrans->type);
    
    switch (pTrans->type) 
    {
        case HID_TRANS_GET_REPORT_RSP:
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_GET_REPORT;
            break;
        case HID_TRANS_SET_REPORT_RSP:
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_SET_REPORT;
            break;
        case HID_TRANS_GET_PROTOCOL_RSP:
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_GET_PROTO;
            break;
        case HID_TRANS_SET_PROTOCOL_RSP:
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_SET_PROTO;
            break;
        case HID_TRANS_GET_IDLE_RATE_RSP:
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_GET_IDLE_RATE;
            break;
        case HID_TRANS_SET_IDLE_RATE_RSP:
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_SET_IDLE_RATE;
            break;
        //Add by stanley: 2007-0607    
        case HID_HANDSHAKE:
            kal_trace(BT_TRACE_G2_PROFILES,GET_THE_RETURNED_HID_TRANSACTION_HANDSHAKE_PACKET);
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_HANDSHAKE;
            break;
        case HID_TRANS_CONTROL:
        {
            bt_hidd_unplug_cnf_struct *msg_p;
            hidda_cntx_p->flag &= ~HIDDA_TRANSA_FLAG_CONTROL;

            // notify the result			
            msg_p = (bt_hidd_unplug_cnf_struct*)construct_local_para(sizeof(bt_hidd_unplug_cnf_struct), TD_UL);
            if (status == BT_STATUS_SUCCESS)
            {
                msg_p->result = hidd_result_ok;
            }
            else
            {
                msg_p->result = hidd_result_failed;
            }
            hidda_send_msg(MSG_ID_BT_HIDD_UNPLUG_CNF, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*)msg_p, NULL);
            break;			

        }
        default:
            break;
    }
	
    // unnecessary to send message to upper layer
}

/*****************************************************************************
 * FUNCTION
 *  hidda_transaction_ind
 * DESCRIPTION
 *  This function is to notify SET_REPORT event
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
static void hidda_handle_interrupt_ind(HidInterrupt *intr, BtStatus status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *data_ptr = hidda_cntx_p->output_report.data;

    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* receiving output data from remote device */
    if (intr->dataLen > 0)
    {
        data_ptr += hidda_cntx_p->output_report.dataLen;
        data_ptr = (U8 *)HID_MEM_MALLOC(intr->dataLen);
        btmtk_os_memcpy(data_ptr, intr->data, intr->dataLen);
        hidda_cntx_p->output_report.dataLen += intr->dataLen;
        if (hidda_cntx_p->output_report.data == NULL)
        {
            hidda_cntx_p->output_report.data = data_ptr;
        }
    }

    if (status == BT_STATUS_SUCCESS)
    {
        bt_hidd_interrupt_data_ind_struct *ind;
        
        ind = (bt_hidd_interrupt_data_ind_struct *)construct_local_para(sizeof(*ind), TD_UL);
        ind->report_type = intr->reportType;
        ind->data_len = hidda_cntx_p->output_report.dataLen;
        ind->data_ptr = (U8 *)HID_MEM_MALLOC(ind->data_len);
        btmtk_os_memcpy(ind->data_ptr, hidda_cntx_p->output_report.data, ind->data_len);
    
        ind->connection_id = hidda_cntx_p->hidda_connection_id;
        ind->bt_addr = hidda_cntx_p->hidda_bd_addr;
    
        hidda_send_msg(
            MSG_ID_BT_HIDD_INTERRUPT_DATA_IND,
            MOD_EXTERNAL,
            BT_APP_SAP,
            (local_para_struct *)ind,
            NULL);

        HID_MEM_FREEIF(hidda_cntx_p->output_report.data);
        btmtk_os_memset((U8 *)&hidda_cntx_p->output_report, 0, sizeof(hidda_cntx_p->output_report));
    }
    else if (status == BT_STATUS_PENDING)
    {
        /* if status is pending, report data shall be queued until completed */
    }

    /* save the output report data */	

}

static void hidda_handle_interrupt_cmplt(HidInterrupt *pIntr, BtStatus status)
{
    if (status == BT_STATUS_SUCCESS)
    {
        kal_trace(BT_TRACE_G2_PROFILES,INTERRUPT_COMPLETE);
        
        /* release control buffer */
        HID_MEM_FREEIF(pIntr->data);
        HID_MEM_FREE(&pIntr->node);
        
        
        /* send connect cnf */
        hidda_send_msg_ext(
        	MSG_ID_BT_HIDD_INTERRUPT_DATA_CNF,
        	MOD_EXTERNAL,
        	BT_APP_SAP,
        	hidd_result_ok,
        	&(hidda_cntx_p->hidda_bd_addr));

    }
    else
    {
        
        /* ignore the sending data while sending failed */
        kal_trace(BT_TRACE_G2_PROFILES,INTERRUPT_COMPLETE_FAILED);
        
        /* release control buffer */
        HID_MEM_FREEIF(pIntr->data);
        HID_MEM_FREE(&pIntr->node);
        
        /* send connect cnf */
        hidda_send_msg_ext(
        	MSG_ID_BT_HIDD_INTERRUPT_DATA_CNF,
        	MOD_EXTERNAL,
        	BT_APP_SAP,
        	hidd_result_failed,
        	&(hidda_cntx_p->hidda_bd_addr));
        
    }

}

/*****************************************************************************
 * FUNCTION
 *  hidda_connect_authorize_indication
 * DESCRIPTION
 *  This function is to
 * PARAMETERS
 *  bt_addr_p       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void hidda_handle_connect_authorize_ind(HidChannel *Channel, BD_ADDR *bt_addr_p)
{
    bt_hidd_connection_authorize_ind_struct *msg_p;
    BD_ADDR bt_addr;

    /* if the connection is already established, or it is 
          initiated by local device, reject the connect request.
     */
    if (hidda_cntx_p->bConnInitiator || hidda_cntx_p->hidda_active)
    {
        HID_RejectConnection(Channel);
        return;		
    }
    
    hidda_cntx_p->hidChannel = Channel;

    if (bt_addr_p != NULL)
    {
        btmtk_os_memcpy((U8 *)&bt_addr, (U8 *)bt_addr_p, sizeof(BD_ADDR));
        //hidda_bt_address_bytestring_2_struct_converer(bt_addr, &bd_struct_addr);
    }

    msg_p = (bt_hidd_connection_authorize_ind_struct*)construct_local_para(sizeof(bt_hidd_connection_authorize_ind_struct), TD_UL);
    msg_p->connection_id = HID_INVALID_CONN_ID;
    msg_p->bt_addr = bt_addr;
    hidda_get_device_name(
        msg_p->dev_name,
        sizeof(msg_p->dev_name),
        bt_addr_p->addr);
    
    hidda_send_msg(MSG_ID_BT_HIDD_CONNECTION_AUTHORIZE_IND, MOD_EXTERNAL, BT_APP_SAP, (local_para_struct*) msg_p, NULL);

}
/*****************************************************************************
 * FUNCTION
 *  hidda_send_msg
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
void hidda_send_msg(
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
 *  hidda_send_msg_ext
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
void hidda_send_msg_ext(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        kal_uint8 result,
        BD_ADDR *bt_addr_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_hid_addr_struct bd_struct_addr;
    BD_ADDR bt_addr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bt_addr_p != NULL)
    {
        btmtk_os_memcpy((U8 *)&bt_addr, (U8 *)bt_addr_p, sizeof(BD_ADDR));
        //hidda_bt_address_bytestring_2_struct_converer(bt_addr, &bd_struct_addr);
    }

    
    switch (msg)
    {
        case MSG_ID_BT_HIDD_CONNECT_CNF:
        {
            /* send connect cnf to MMI */
            bt_hidd_connect_cnf_struct *msg_p;

            msg_p = (bt_hidd_connect_cnf_struct*) construct_local_para(sizeof(bt_hidd_connect_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                hidda_cntx_p->hidda_connection_id = (U32) BTBMProfileAllocateConnIndex(SC_HUMAN_INTERFACE_DEVICE, bt_addr);
                hidda_cntx_p->hidda_bd_addr = bt_addr;
                msg_p->connection_id = hidda_cntx_p->hidda_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = bt_addr;
            hidda_send_msg(MSG_ID_BT_HIDD_CONNECT_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
        }
            break;
        case MSG_ID_BT_HIDD_CONNECT_IND:
        {
            bt_hidd_connect_ind_struct *msg_p;

            msg_p = (bt_hidd_connect_ind_struct*) construct_local_para(sizeof(bt_hidd_connect_ind_struct), TD_UL);
            if (result == hidd_result_ok)
            {
                hidda_cntx_p->hidda_connection_id = (U32) BTBMProfileAllocateConnIndex(SC_HUMAN_INTERFACE_DEVICE, bt_addr);
                hidda_cntx_p->hidda_bd_addr = bt_addr;
                msg_p->connection_id = hidda_cntx_p->hidda_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = bt_addr;
            msg_p->result = result;
            hidda_send_msg(MSG_ID_BT_HIDD_CONNECT_IND, dstMod, sap, (local_para_struct*) msg_p, NULL);

        }
            break;
        case MSG_ID_BT_HIDD_DISCONNECT_CNF:
        {
            bt_hidd_disconnect_cnf_struct *msg_p;

            msg_p = (bt_hidd_disconnect_cnf_struct*) construct_local_para(sizeof(bt_hidd_disconnect_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                msg_p->connection_id = hidda_cntx_p->hidda_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = bt_addr;
            hidda_send_msg(MSG_ID_BT_HIDD_DISCONNECT_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
        }
            break;
        case MSG_ID_BT_HIDD_DISCONNECT_IND:
        {

            bt_hidd_disconnect_ind_struct *msg_p;

            msg_p = (bt_hidd_disconnect_ind_struct*) construct_local_para(sizeof(bt_hidd_disconnect_ind_struct), TD_UL);
            msg_p->connection_id = hidda_cntx_p->hidda_connection_id;
            msg_p->bt_addr = bt_addr;
            hidda_send_msg(MSG_ID_BT_HIDD_DISCONNECT_IND, dstMod, sap, (local_para_struct*) msg_p, NULL);
        }
            break;

        case MSG_ID_BT_HIDD_INTERRUPT_DATA_CNF:
        {
            bt_hidd_interrupt_data_cnf_struct *msg_p;

            msg_p = (bt_hidd_interrupt_data_cnf_struct*) construct_local_para(sizeof(bt_hidd_interrupt_data_cnf_struct), TD_UL);
            msg_p->result = result;
            if (result == hidd_result_ok)
            {
                msg_p->connection_id = hidda_cntx_p->hidda_connection_id;
            }
            else
            {
                msg_p->connection_id = HID_INVALID_CONN_ID;
            }
            msg_p->bt_addr = bt_addr;
            //msg_p->report_type = hidd_report_input;            
            
            hidda_send_msg(MSG_ID_BT_HIDD_INTERRUPT_DATA_CNF, dstMod, sap, (local_para_struct*) msg_p, NULL);
            break;

        }
            
        default:
            break;
            //Assert(0);
    }

}

/*****************************************************************************
 * FUNCTION
 *  hidda_get_device_name
 * DESCRIPTION
 *  This function is to get device name.
 * PARAMETERS
 *  name    [OUT]
 *  size    [IN]
 *  addr    [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void hidda_get_device_name(kal_uint8 *name, kal_uint32 size, kal_uint8 *addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 *lname;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    lname = BTBMGetLocalStoredName(addr);

    if (lname == NULL)
    {
        name[0] = '\0';
    }
    else
    {
        name[size-1] = '\0';
        btmtk_os_memcpy((U8 *)name, (U8 *)lname, size-1);
    }
}

static void hidda_clear_connection(hidda_context_struct *cntx)
{
    /* The outgong data buffer shall be release in HIDEVENT_TRANSACTION_COMPLETE
          or HIDEVENT_INTERRUPT_COMPLETE event. Refer to HidClearConnection().
          Only free the memory used to queue incoming data here.
    */
    HID_MEM_FREEIF(cntx->report.data);
    HID_MEM_FREEIF(cntx->output_report.data);

    if (cntx->hidda_connection_id != HID_INVALID_CONN_ID)
    {
        BTBMProfileReleaseConnIndex((U8) cntx->hidda_connection_id);
    }

    btmtk_os_memset((U8*)cntx, 0, sizeof(hidda_context_struct));
    cntx->hidda_connection_id = HID_INVALID_CONN_ID;
}

#endif /* __BT_HIDD_PROFILE__ */
