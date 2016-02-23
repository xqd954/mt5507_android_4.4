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
*  permission of MediaTek Inc. (C) 2007
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
 * jsr82_session.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * 
 *
 * Author:
 * -------
 * Dlight Ting
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 02 23 2012 dlight.ting
 * [ALPS00238652] [ICS][CTS Verifier]
 * .
 *
 * 03 04 2011 dlight.ting
 * [ALPS00033296] [Bluetooth] checkin BT3.0+HS
 * .
 *
 * 10 13 2010 bingyi.chen
 * [ALPS00021197] [CTS][E1K16] System pops NE during test "CtsBluetoothTestCases"
 * .
 *
 * Mar 11 2009 mtk02126
 * [MAUI_01644724] [JSR82] bt_jsr82_HandleSdpReq
 * 
 *
 * Dec 12 2008 mtk01411
 * [MAUI_01295384] [JSR82] Reset bdc and remDev to zero after CMGR_RemoveDataLink
 * 
 *
 * Dec 10 2008 mtk01411
 * [MAUI_01478777] JSR82_ Assert Fail: utils.c 331-BT
 * 
 *
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Nov 4 2008 mtk01411
 * [MAUI_01268352] [JSR82] Update JSR82 L2cap CON_IND with identify value as transaction id of turn on
 * 
 *
 * Nov 3 2008 mtk01411
 * [MAUI_01331610] [1]Assert Fail: IsListCircular(head) utils.c 331 - BT
 * 
 *
 * Oct 12 2008 mtk01411
 * [MAUI_01254027] [JSR82] Update JSR82 related codes
 * 
 *
 * Aug 6 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Aug 5 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * Apr 14 2008 mtk01411
 * [MAUI_00755198] [JSR82] Update JSR82 codes
 * 
 *
 * Mar 30 2008 mtk01411
 * [MAUI_00740866] [JSR82] Handle auth event with error in L2Cap to force delete paired reocrd and disc
 * 
 *
 * Mar 25 2008 mtk01411
 * [MAUI_00737864] [JSR82] Check restore state check in set_local_cod_result hdler and delay setup conn
 * 
 *
 * Mar 15 2008 mtk01411
 * [MAUI_00281663] Fatal Error (b03): Abnormal Reset (63f423) - BT
 * 
 *
 * Mar 14 2008 mtk01411
 * [MAUI_00731923] JSR82 _it can't load image
 * 
 *
 * Mar 2 2008 mtk01411
 * [MAUI_00278253] [JSR82]Java_always display " play wait "
 * 
 *
 * Feb 22 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "bt_feature.h"
#include "bt_common.h"
#include "bluetooth_trc.h"
#include "jsr82_session.h"
#include "bt_jsr82_setting.h"
#include "l2cap_i.h"
#include "rfcomm.h"
#include "bt_os_api.h"
#include "btalloc.h"
#include "bt_debug.h"
#include "bt_adp_mem.h"

#if defined(__BT_JSR82__)
#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
extern void BTAppSdpDbDeactivate(void);
extern void btbmHandlerAuthenticatedCfm(const BtEvent *Event);
extern U8 BT_JSR82QueryPowerStatus(void);
extern void bma_panic(void);
extern void BTBMAdpSearchAttributeRawComplete(kal_uint8 result, btbm_bd_addr_t addr);
extern void SDAP_ServiceSearchAttributeRaw(bt_bm_search_raw_req_struct *search_pattern);
extern BOOL BTCoreVerifySysInitState(void);
extern BOOL BTCoreIsDeinitializing(void);
static void bt_jsr82_event(BT_JSR82_SESSION_AP_Record_Context_struct_t *session, JSR82_event event, JSR82_SESSION_RESULT result);

BT_JSR82_SESSION_AP_CONTEXT_struct_t bt_jsr82_session_ap_context;

static BtHandler jsr82LinkHandler;
static BtHandler jsr82globalHandler;

static void jsr82_DummyLinkCallback(const BtEvent *Event);
static void Me2JSR82Callback(const BtEvent *Event);
void bt_jsr82_setHighWaterMark(RfChannel *chan, BT_JSR82_SESSION_Buffer_struct_t *session_buf_ptr);
void bt_jsr82_sendToL2Cap(U8 session_inx, U16 l2cap_id);

/* Callback function to Internel ADP layer */
JSR82Callback JSR82CB;


/* Register callback function to Internel ADP layer */
void JSR82_Register(JSR82Callback Callback)
{
    JSR82CB = Callback;
}



/* Add this function: 2007-1225 */


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_init_session_buffer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_buf     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_init_session_buffer(BT_JSR82_SESSION_Buffer_struct_t *session_buf)
{
    session_buf->mainRecord = NULL;
    btmtk_os_memset(session_buf->data, 0, sizeof(session_buf->data));
    btmtk_os_memset((U8*)&(session_buf->cmgrHandler), 0, sizeof(session_buf->cmgrHandler));
    btmtk_os_memset((U8*)&(session_buf->rfc), 0, sizeof(RfChannel));
    btmtk_os_memset((U8*)&(session_buf->l2capCtx), 0, sizeof(session_buf->l2capCtx));
	btmtk_os_memset((U8*)&(session_buf->sdpQueryToken), 0, sizeof(SdpQueryToken));
    session_buf->highWaterMarkValue = 0;
    session_buf->devCredit = 1;
}

void BTJSR82_SessionApRecordInit(BT_JSR82_SESSION_AP_Record_Context_struct_t *session_ap, U8 conn_id)
{
    U8 index = 0;

	if (NULL == session_ap)
	{
		return;
	}
	btmtk_os_memset((kal_uint8 *)session_ap, 0x0, sizeof(BT_JSR82_SESSION_AP_Record_Context_struct_t));
	InitializeListHead(&session_ap->ApTxQueue);
	session_ap->con_id = conn_id;
	for (index = 0; index < JSR82_SESSION_TX_PKT_QUEUE_SIZE; index++)
    {
        InsertTailList(&session_ap->ApTxQueue, &session_ap->txPacket[index].node);    
	}

	InitializeListHead(&session_ap->ApTxPktList);
	session_ap->txPktPool = NULL;
}



/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SessionApInit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SessionApInit(void)
{
    U8 i = 0;

    btmtk_os_memset((U8*) & bt_jsr82_session_ap_context, 0, sizeof(BT_JSR82_SESSION_AP_CONTEXT_struct_t));
    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
		BTJSR82_SessionApRecordInit(&BTJ82SEAP(session)[i], i);
    }
    BTJ82SEAP(l2cap_psm_usage_table) = 0;
    BTJ82SEAP(rfcomm_channel_usage_table) = 0;
    BTJ82SEAP(jsr82_disconnect_all_acl).func = 0;
    BTJ82SEAP(jsr82_con_req_timer).func = 0;
    BTJ82SEAP(jsr82_sdp_req_timer).func = 0;
    jsr82LinkHandler.callback = jsr82_DummyLinkCallback;
    BTJ82SEAP(jsr82_disconnect_all_acl).func = 0;
    BTJ82SEAP(jsr82_con_req_timer).func = 0;
    BTJ82SEAP(jsr82_sdp_req_timer).func = 0;
    BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) = 0;
    BTJ82SEAP(vm_disc_link_ptr) = NULL;
    BTJ82SEAP(dummyL2capPsm).callback = BTJSR82_L2CapDummyCallback;

    /* Initialization jsr82globalHandler */
    ME_InitHandler(&jsr82globalHandler);
    jsr82globalHandler.callback = Me2JSR82Callback;
    ME_RegisterGlobalHandler(&jsr82globalHandler);
    /* If the errCode of Authentication Complete Event is zero, the eventType BTEVENT_AUTHENTICATED will be reported via the registered global handler */
    ME_SetEventMask(&jsr82globalHandler, BEM_AUTHENTICATED);
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_DeInitAll
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_DeInitAll(void)
{
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLEJSR82DEACTIVATE);
    bt_jsr82_HandleJSR82Deactivate();
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleJSR82Deactivate
 * DESCRIPTION
 *  It do de-register service procedure and reset session context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleJSR82Deactivate(void)
{
    U8 i = 0;
    RfChannel *channel;
    RfService srv_chn;
    BtStatus status;
    U8 ps_type;

    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_DEACTIVATE_STATUD_CONID,
            BTJ82SEAP(session)[i].status,
            BTJ82SEAP(session)[i].con_id);
        /* Deactivation procedure: only for Server side */
        if (BTJ82SEAP(session)[i].status == BT_JSR82_SESSION_ACTIVE)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DEACTIVATE_FIND_CHNL_INX, i);
            Assert((BTJ82SEAP(session)[i].session_buffer) != 0);
            ps_type = BTJ82SEAP(session)[i].ps_type;
            if (BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_RFCOMM)
            {
        //        Assert(BTJ82SEAP(session)[i].channel_state == BT_JSR82_CHANNEL_STATE_IDLE);
                /* Only when this session is server role, it needs to de-register : 2007-1213 */
                if (BTJ82SEAP(session)[i].client_server == BT_JSR82_SESSION_SERVER)
                {
                    channel = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[i].session_buffer))->rfc;
                    srv_chn.serviceId = (U8) BTJ82SEAP(session)[i].channel;
                    status = RF_DeregisterServerChannel(channel, &srv_chn);

                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DEACTIVATE_DEREG_CHNL_STATUS, status);
                    bt_jsr82_SessionReleaseServChnIndex(srv_chn.serviceId);
                }
            }
            else
            {
                BT_JSR82_SESSION_Buffer_struct_t *temp_ptr =
                    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[i].session_buffer));

          //      Assert(temp_ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_IDLE);
                /*
                 * For L2cap client and server: They both need to free l2cap PSM index and de-register L2CAP PSM:2007-1216 
                 * But for L2CAP client: it must de-register PSM index via bt_jsr82_free_L2capPSMIndex when receiving L2EVENT_DISCONNECTED
                 */
                bt_jsr82_free_L2capPSMIndex(BTJ82SEAP(session)[i].channel, BTJ82SEAP(session)[i].security_value, BTJ82SEAP(session)[i].client_server);
                /* Only L2CAP server needs to call the function bt_jsr82_SessionReleasePSMIndex() */
                bt_jsr82_SessionReleasePSMIndex(BTJ82SEAP(session)[i].channel);
            }

			if (BTJ82SEAP(session)[i].channelTimer.context)
			{
				EVM_CancelTimer(&(BTJ82SEAP(session)[i].channelTimer));	
				BTJ82SEAP(session)[i].channelTimer.context = NULL;
			}

            /*
             * Add for RFCOMM and L2CAP server: 2007-1216
             * For RFCOMM and L2CAP client, their corresponding cmgrhdl will be de-registered when connection is disconnected
             */
            BTJ82SEAP(session)[i].status = BT_JSR82_SESSION_IDLE;
            if (BTJ82SEAP(session)[i].client_server == BT_JSR82_SESSION_SERVER)
            {
                CmgrHandler *cmgrhdl = NULL;

                cmgrhdl = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[i].session_buffer))->cmgrHandler;
                status = CMGR_DeregisterHandler(cmgrhdl);
				
                BTJ82SEAP(session)[i].channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
				
		bt_jsr82_event(&BTJ82SEAP(session)[i],  JSR82_EVENT_DISABLE_SERVICE_CNF, BT_JSR82_SESSION_DISABLED_SUCCESS);
				
		  
				// Free ring buffer
	 			jsr82_session_free_RWRingBuf(i);   
        		// Clear session context
        		bt_jsr82_ClearSessionContext(i);
                }
		else
		{
			bt_jsr82_event(&BTJ82SEAP(session)[i], JSR82_EVENT_DISCONNECT_IND, JSR82_SESSION_RESULT_SUCCESS);				
		}
		// Free ring buffer
	 	jsr82_session_free_RWRingBuf(i);   
        	// Clear session context
        	bt_jsr82_ClearSessionContext(i);
        }
    }
    btmtk_os_memset((U8*) & bt_jsr82_session_ap_context, 0, sizeof(BT_JSR82_SESSION_AP_CONTEXT_struct_t));
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_CheckVMDiscAllLinksReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 *  ps_type     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_CheckVMDiscAllLinksReq(BtRemoteDevice *link, U8 ps_type)
{
    if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) > 0)
    {
        BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no)--;
        if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) == 0)
        {

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VM_END_PEND_CON_REQ_PS_TYPE, ps_type);
            if (!IsNodeOnList(&(link->handlers), &(jsr82LinkHandler.node)))
            {
                InsertTailList(&(link->handlers), &(jsr82LinkHandler.node));
            }
            link->discFlag = TRUE;
            ME_ForceDisconnectLinkWithReason(&jsr82LinkHandler, link, BEC_POWER_OFF, TRUE);
            if (IsNodeOnList(&(link->handlers), &(jsr82LinkHandler.node)))
            {
                RemoveEntryList(&(jsr82LinkHandler.node));
            }

            BTJ82SEAP(jsr82_disconnect_all_acl).func = bt_jsr82_ACLCheckDisconnectTimer;
            EVM_StartTimer(&BTJ82SEAP(jsr82_disconnect_all_acl), 1000);
        }
    }

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_CheckSendVMDiscAllLinksCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  type        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_CheckSendVMDiscAllLinksCnf(U8 type)
{
    if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) > 0)
    {
        BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no)--;

        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_VM_DISC_ALL_LINK_PEND_CON_REQ_NO_TYPE,
            BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no),
            type);
    #if 0
        if(type == 0)
            bt_trace(BT_TRACE_JSR82_GROUP,BT_JSR82_VM_TERMINATE_PENDING_CON_REQ_CONNECTED_THEN_DISC_THIS_LINK);
        else if(type == 1)
            bt_trace(BT_TRACE_JSR82_GROUP,BT_JSR82_VM_TERMINATION_PENDING_CON_REQ_IS_ACL_LINK_SETUP_OKBUT_RFCOMM_IS_REJECTED);
        else if(type ==2)    
            bt_trace(BT_TRACE_JSR82_GROUP,BT_JSR82_VM_TERMINATION_PENDING_CON_REQ_IS_ACL_LINK_SETUP_OKBUT_L2CAP_IS_REJECTED);
    #endif /* 0 */ 
        if ((BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) == 0) &&
            (BTJ82SEAP(jsr82_disconnect_all_acl).func == 0))
        {
            JSR82CallbackParms parms;
            //bt_jsr82_SessionApDisconnectAllAclCfn();
	     parms.event = JSR82_EVENT_DISCONNECT_ALL_ACL_CNF;
	     parms.dstMod = MOD_MMI;
	     JSR82CB(&parms);
        }
    }

}


/*****************************************************************************
 * FUNCTION
 *  jsr82_DummyLinkCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void jsr82_DummyLinkCallback(const BtEvent *Event)
{
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleAuthenticationCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleAuthenticationCnf(const BtEvent *Event)
{
    U32 transaction_id = 0;
    U8 result = JSR82_SET_ACL_SECURITY_FAILED;
    BtRemoteDevice *remDev = Event->p.remDev;
    int i = 0, remDev_index = JSR82_MAX_ACL_NO;
    JSR82CallbackParms parms;

    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if ((BTJ82SEAP(active_link)[i].used == 1) &&
            ((BTJ82SEAP(active_link)[i].authentication_link_state == BT_JSR82_AUTHENTICATE_LINK_ONGOING) ||
             (BTJ82SEAP(active_link)[i].authentication_link_state == BT_JSR82_ENCRYPT_AUTHENTICATE_LINK_ONGOING)))
        {
            if (BTJ82SEAP(active_link)[i].link == remDev)
            {
                remDev_index = i;
                break;
            }
        }
    }

    /* This authentication cnf event is not belonged to JSR82 session */
    if (i == JSR82_MAX_ACL_NO)
    {
        return;
    }

    if (Event->errCode != 0)
    {
        /* Check bm_opactivity and cancel timer if PIN_CODE_RSP was sent to BTStack previously */
        BtStatus status = BT_STATUS_FAILED;

        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLE_AUTH_CNF_ERRCODE, Event->errCode);
        btbmHandlerAuthenticatedCfm(Event);
        status = SEC_DeleteDeviceRecord(&(remDev->bdAddr));
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLE_AUTH_CNF_DEL_DEV_RECORD_STATUS, status);
        result = JSR82_SET_ACL_SECURITY_FAILED;
    }
    else
    {
        result = JSR82_SET_ACL_SECURITY_SUCCESS;
    }

    transaction_id = BTJ82SEAP(active_link)[remDev_index].authentication_transaction_id;

    if (result == JSR82_SET_ACL_SECURITY_SUCCESS &&
        BTJ82SEAP(active_link)[remDev_index].authentication_link_state == BT_JSR82_ENCRYPT_AUTHENTICATE_LINK_ONGOING)
    {
        /* Complete Authentication, then start Encryption procedure */
        /* Check if there is any one encryption operation ongoing or not */
        int j = 0;
        BtStatus status = BT_STATUS_FAILED;

        for (j = 0; j < JSR82_MAX_ACL_NO; j++)
        {
            if (BTJ82SEAP(active_link)[j].encrypt_link_state == BT_JSR82_ENCRYPTION_LINK_ONGOING)
            {
                result = JSR82_SET_ACL_SECURITY_FAILED;

                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_AUTH_COMPLETE_BUT_ONE_ENCRYPT_ON_GOING);
                /* i: points to the target remote device index */
                BTJ82SEAP(active_link)[remDev_index].authentication_link_state = BT_JSR82_AUTHENTICATE_LINK_NONE;
                BTJ82SEAP(active_link)[remDev_index].authentication_transaction_id = 0;
                //bt_jsr82_SetACLSecurityCnf(result, transaction_id, remDev->bdAddr.addr);
	         parms.p.set_acl_security_cnf.result = result;
	         parms.p.set_acl_security_cnf.transaction_id = transaction_id;
	         parms.p.set_acl_security_cnf.addr = remDev->bdAddr.addr;
		  parms.event = JSR82_EVENT_SET_ACL_SECURITY_CNF;
		  parms.dstMod = MOD_MMI;
		  JSR82CB(&parms);
                return;
            }
        }

        /* Start encryption procedure */
        status = SEC_SetLinkEncryption(&jsr82globalHandler, BTJ82SEAP(active_link)[remDev_index].link, BECM_ENCRYPT_ENABLE);
        if (status == BT_STATUS_PENDING)
        {
            BTJ82SEAP(active_link)[remDev_index].authentication_link_state = BT_JSR82_AUTHENTICATE_LINK_NONE;
            BTJ82SEAP(active_link)[remDev_index].encrypt_link_state = BT_JSR82_ENCRYPTION_LINK_ONGOING;

        }
        else
        {

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SEC_LINK_ENCRYPT_STATUS, status);
            BTJ82SEAP(active_link)[remDev_index].authentication_link_state = BT_JSR82_AUTHENTICATE_LINK_NONE;
            //bt_jsr82_SetACLSecurityCnf(JSR82_SET_ACL_SECURITY_FAILED, transaction_id, remDev->bdAddr.addr);
	     parms.p.set_acl_security_cnf.result = JSR82_SET_ACL_SECURITY_FAILED;
	     parms.p.set_acl_security_cnf.transaction_id = transaction_id;
	     parms.p.set_acl_security_cnf.addr = remDev->bdAddr.addr;
	     parms.event = JSR82_EVENT_SET_ACL_SECURITY_CNF;
	     parms.dstMod = MOD_MMI;
	     JSR82CB(&parms);
            return;
        }

    }
    else
    {
        BTJ82SEAP(active_link)[i].authentication_link_state = BT_JSR82_AUTHENTICATE_LINK_NONE;
        BTJ82SEAP(active_link)[i].authentication_transaction_id = 0;
        //bt_jsr82_SetACLSecurityCnf(result, transaction_id, remDev->bdAddr.addr);
	 parms.p.set_acl_security_cnf.result = result;
	 parms.p.set_acl_security_cnf.transaction_id = transaction_id;
	 parms.p.set_acl_security_cnf.addr = remDev->bdAddr.addr;
	 parms.event = JSR82_EVENT_SET_ACL_SECURITY_CNF;
	 parms.dstMod = MOD_MMI;
	 JSR82CB(&parms);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleAuthenticated
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleAuthenticated(const BtEvent *Event)
{

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleEncryptCompleted
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleEncryptCompleted(const BtEvent *Event)
{
    U32 transaction_id = 0;
    U8 result = JSR82_SET_ACL_SECURITY_FAILED;
    BtRemoteDevice *remDev = Event->p.remDev;
    int i = 0, remDev_index = JSR82_MAX_ACL_NO;
    JSR82CallbackParms parms;

    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if ((BTJ82SEAP(active_link)[i].used == 1) &&
            (BTJ82SEAP(active_link)[i].encrypt_link_state == BT_JSR82_ENCRYPTION_LINK_ONGOING))
        {
            if (BTJ82SEAP(active_link)[i].link == remDev)
            {
                remDev_index = i;
                break;
            }
        }
    }

    /* This encryption cnf event is not belonged to JSR82 session */
    if (i == JSR82_MAX_ACL_NO)
    {
        return;
    }

    if (Event->errCode != 0)
    {
        result = JSR82_SET_ACL_SECURITY_FAILED;
    }
    else
    {
        result = JSR82_SET_ACL_SECURITY_SUCCESS;
    }
    transaction_id = BTJ82SEAP(active_link)[i].authentication_transaction_id;
    BTJ82SEAP(active_link)[i].encrypt_link_state = BT_JSR82_ENCRYPTION_LINK_NONE;
    BTJ82SEAP(active_link)[i].authentication_transaction_id = 0;
    //bt_jsr82_SetACLSecurityCnf(result, transaction_id, remDev->bdAddr.addr);
    parms.p.set_acl_security_cnf.result = result;
    parms.p.set_acl_security_cnf.transaction_id = transaction_id;
    parms.p.set_acl_security_cnf.addr = remDev->bdAddr.addr;
    parms.event = JSR82_EVENT_SET_ACL_SECURITY_CNF;
    parms.dstMod = MOD_MMI;
    JSR82CB(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  Me2JSR82Callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void Me2JSR82Callback(const BtEvent *Event)
{
    switch (Event->eType)
    {
        case BTEVENT_AUTHENTICATE_CNF:
            bt_jsr82_HandleAuthenticationCnf(Event);
            break;

        case BTEVENT_AUTHENTICATED:
            bt_jsr82_HandleAuthenticated(Event);
            break;

        case BTEVENT_ENCRYPT_COMPLETE:
            bt_jsr82_HandleEncryptCompleted(Event);
            break;
        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_GetSessionInx
 * DESCRIPTION
 *  
 * PARAMETERS
 *  identify        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 BTJSR82_GetSessionInx(U32 identify)
{
    int i = 0;

    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        if ((BTJ82SEAP(session)[i].status == BT_JSR82_SESSION_ACTIVE) && (identify == BTJ82SEAP(session)[i].identify))
        {

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_GET_SESSION_INX, i);
            break;
        }
    }
    return i;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SearchL2capContext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  l2cap_id        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 bt_jsr82_SearchL2capContext(U16 l2cap_id)
{
    U8 index = 0;

    BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr;
    BT_JSR82_SESSION_Buffer_struct_t *ptr1;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SEARCHL2CAPCONTEXT);

    for (index = 0; index < JSR82_MAX_SESSION_NO; index++)
    {
        ptr = (BT_JSR82_SESSION_AP_Record_Context_struct_t*) & (BTJ82SEAP(session)[index]);

        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_SEARCH_L2CAP_CNTX_DUMP_SESSION_INFO,
            index,
            ptr->status,
            ptr->ps_type,
            ptr->channel,
            ptr->client_server);
        ptr1 = (BT_JSR82_SESSION_Buffer_struct_t*) ptr->session_buffer;

	 if (BT_JSR82_SESSION_ACTIVE== BTJ82SEAP(session)[index].status)
	 {
	        bt_trace(
	            BT_TRACE_JSR82_GROUP,
	            BT_JSR82_SEARCH_L2CAP_CNTX_L2CAP_INFO,
	            ptr1->l2capCtx.l2cap_con_state,
	            ptr1->l2capCtx.l2capLocalCid);

	        if (((ptr1->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_OPEN ||
	              (ptr1->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL))) &&
	            (ptr1->l2capCtx.l2capLocalCid == l2cap_id))
	        {

	            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SEARCH_L2CAP_CNTX_FIND_EXISTED_L2CID, index, l2cap_id);
	            return index;
	        }

	 }
    }
    return JSR82_MAX_SESSION_NO;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_AddNewL2capToContext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr             [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus bt_jsr82_AddNewL2capToContext(BT_JSR82_SESSION_Buffer_struct_t *ptr, U16 l2cap_id)
{
    BtStatus status = BT_STATUS_FAILED;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ADD_NEW_L2CAP_TO_CNTX, l2cap_id, ptr->l2capCtx.l2cap_con_state);
    if (ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_IDLE)
    {
        ptr->l2capCtx.l2capLocalCid = l2cap_id;
        ptr->l2capCtx.l2cap_con_state = BT_JSR82_CHANNEL_STATE_OPEN;
        status = BT_STATUS_SUCCESS;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_AddCreateL2capToContext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr             [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus bt_jsr82_AddCreateL2capToContext(BT_JSR82_SESSION_Buffer_struct_t *ptr, U16 l2cap_id)
{
    BtStatus status = BT_STATUS_FAILED;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ADD_CREATE_L2CAP_TO_CNTX, l2cap_id, ptr->l2capCtx.l2cap_con_state);
    if (ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_IDLE)
    {
        ptr->l2capCtx.l2capLocalCid = l2cap_id;
        ptr->l2capCtx.l2cap_con_state = BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL;
        status = BT_STATUS_SUCCESS;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_ReleaseL2capFromContext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr             [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_ReleaseL2capFromContext(BT_JSR82_SESSION_Buffer_struct_t *ptr, U16 l2cap_id)
{

    if (((ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_OPEN) ||
         (ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL)) &&
        (ptr->l2capCtx.l2capLocalCid == l2cap_id))

    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FREE_L2CAP_CNTX, ptr->l2capCtx.l2cap_con_state, l2cap_id);
        ptr->l2capCtx.l2cap_con_state = BT_JSR82_CHANNEL_STATE_IDLE;
        ptr->l2capCtx.l2capLocalCid = 0;

        return;
    }
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_NO_L2CAP_CON_RELEASE_FIND_BY_ID, l2cap_id);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_L2capRegisterSecurity
 * DESCRIPTION
 *  
 * PARAMETERS
 *  l2capSecRecord      [IN]        
 *  sec_value           [IN]        
 *  channel             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_L2capRegisterSecurity(BtSecurityRecord *l2capSecRecord, U8 sec_value, U16 channel)
{
    InitializeListEntry(&l2capSecRecord->node);
    l2capSecRecord->id = SEC_L2CAP_ID;
    l2capSecRecord->channel = channel;
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REG_SEC_RECORD_ON_CHNL, l2capSecRecord->channel);
    l2capSecRecord->level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
    /*
     * 2007-1219: 
     * BT_SEC_LOW: it means that this chnl does not want to do authentication, depends on MMI setting
     * BT_SEC_LOW + MMI auth on : do authentication
     * BT_SEC_LOW + MMI auth off: not do authentication
     * BT_SEC_HIGH: it means that this chnl wants to do authentication, independt of MMI setting
     * BT_SEC_HIGH + MMI auth on: do authentication
     * BT_SEC_HIGH + MMI auth off: do authentication
     */
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    l2capSecRecord->level_type = BT_SEC_MED;
#else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    if (sec_value == 0x00)
    {
        l2capSecRecord->level_type = BT_SEC_LOW;
    }
    else
    {
        l2capSecRecord->level_type = BT_SEC_MED;
    }
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    /*
     * Replace the SEC_Register() to SEC_RegisterRecord(): 2007-1218 
     * If Security_Level2 is enabled, the invoking SEC_Register() will do nothing and return success directly
     */
    SEC_RegisterRecord(l2capSecRecord);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_get_L2capPSMIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  channel             [IN]        
 *  mtu                 [IN]        
 *  security_level      [IN]        
 *  client_server	[IN]
 * RETURNS
 *  
 *****************************************************************************/
U8 bt_jsr82_get_L2capPSMIndex(U16 channel, U16 mtu, U8 security_level, bt_jsr82_session_type client_server)
{
    U8 i = 0;
    L2capPsm *l2cap_psm = NULL;
    BtStatus status;
    BT_JSR82_L2CAP_PSM_struct_t *l2cap_psm_info = 0;

    for (i = 0; i < JSR82_MAX_PSM_NO; i++)
    {
        if ((BTJ82SEAP(l2cap_psm_info)[i].used == 1) && (BTJ82SEAP(l2cap_psm_info)[i].l2cap.psm == channel) 
        	&& (client_server == BTJ82SEAP(l2cap_psm_info)[i].client_server))
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_ALLOCATED_L2CAP_PSM_INX, channel, i);
            if (mtu < BTJ82SEAP(l2cap_psm_info)[i].l2cap.localMtu)
            {
                BTJ82SEAP(l2cap_psm_info)[i].l2cap.localMtu = mtu;
            }
            BTJ82SEAP(l2cap_psm_info)[i].used_no++;
            l2cap_psm = &BTJ82SEAP(l2cap_psm_info)[i].l2cap;
            /*
             * BTJ82SEAP(l2cap_psm_info)[i].security_level: for one specific PSM value, using this bit-mask table to record if some conn of this same PSM needs auth or not.
             * For example: Because security_level is only 0 or 1,
             * If for PSM=3, there are 3 conn connected to this same PSM:
             * For 1st conn, its security_level=0, then BTJ82SEAP(l2cap_psm_info)[i].security_level |= (0<<1) -> finale result 0 ==0: SEC_LOW
             * For 2nd conn, its security_level=1,then BTJ82SEAP(l2cap_psm_info)[i].security_level |= (1<<2) -> finale result 2 !=0: SEC_HIGH
             * For 3rd conn, its security_level=0,then BTJ82SEAP(l2cap_psm_info)[i].security_level |= (0<<3) -> finale result 2 !=0: SEC_HIGH 
             */
            BTJ82SEAP(l2cap_psm_info)[i].security_level |= (security_level << BTJ82SEAP(l2cap_psm_info)[i].used_no);
            SEC_Unregister(&BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord);
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
            bt_jsr82_L2capRegisterSecurity(&BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord, BT_SEC_MED, l2cap_psm->psm);
        #else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
            if (BTJ82SEAP(l2cap_psm_info)[i].security_level != 0)
            {
                bt_jsr82_L2capRegisterSecurity(
                    &BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord,
                    BT_SEC_MED,
                    l2cap_psm->psm);
            }
            else
            {
                bt_jsr82_L2capRegisterSecurity(
                    &BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord,
                    BT_SEC_LOW,
                    l2cap_psm->psm);
            }
        #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
            return i;
        }
    }
    for (i = 0; i < JSR82_MAX_PSM_NO; i++)
    {
        if (BTJ82SEAP(l2cap_psm_info)[i].used == 0)
        {
            l2cap_psm_info = &BTJ82SEAP(l2cap_psm_info)[i];
            l2cap_psm_info->used = 1;
            l2cap_psm_info->used_no++;
            l2cap_psm_info->client_server = client_server;
            l2cap_psm = &l2cap_psm_info->l2cap;
            l2cap_psm->psm = channel;
            l2cap_psm->localMtu = mtu;
            l2cap_psm->minRemoteMtu = L2CAP_MINIMUM_MTU;
            l2cap_psm->authorizedSetting = FALSE;
            l2cap_psm->callback = BTJSR82_L2capCallback;
#if L2CAP_FLOW_CONTROL == XA_ENABLED
            l2cap_psm->inLinkMode = l2cap_psm->outLinkMode = L2MODE_BASIC;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */

            BTJ82SEAP(l2cap_psm_info)[i].security_level = 0;
#ifdef __BT_2_1_ERTM__    
            l2cap_psm->lockStepNeeded = 0;
            l2cap_psm->highSpeedRequired = 0;
#endif

            status = L2CAP_RegisterPsm(l2cap_psm);
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_EMPTY_PSM_INX_REG_STATUS, i, status);
            if (status != BT_STATUS_SUCCESS)
            {
                return JSR82_MAX_PSM_NO;
            }
            else
            {
                BTJ82SEAP(l2cap_psm_info)[i].security_level |= security_level << BTJ82SEAP(l2cap_psm_info)[i].used_no;
            #if defined (__BT_2_1_SIMPLE_PAIRING__)
                bt_jsr82_L2capRegisterSecurity(
                    &(BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord),
                    BT_SEC_MED,
                    l2cap_psm->psm);
            #else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
                if (BTJ82SEAP(l2cap_psm_info)[i].security_level != 0)
                {
                    bt_jsr82_L2capRegisterSecurity(
                        &(BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord),
                        BT_SEC_MED,
                        l2cap_psm->psm);
                }
                else
                {
                    bt_jsr82_L2capRegisterSecurity(
                        &(BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord),
                        BT_SEC_LOW,
                        l2cap_psm->psm);
                }
            #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
                return i;
            }
        }
    }
    return JSR82_MAX_PSM_NO;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_free_L2capPSMIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  channel             [IN]        
 *  secruity_level      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_free_L2capPSMIndex(U32 channel, U8 secruity_level, bt_jsr82_session_type client_server)
{
    U8 i = 0;
    U8 j = 0;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FREE_L2CAP_PSM_INX, channel, secruity_level);

    for (i = 0; i < JSR82_MAX_PSM_NO; i++)
    {
        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_DUMP_L2CAP_PSM_INFO,
            i,
            BTJ82SEAP(l2cap_psm_info)[i].used,
            BTJ82SEAP(l2cap_psm_info)[i].l2cap.psm,
            BTJ82SEAP(l2cap_psm_info)[i].used_no);

        if ((BTJ82SEAP(l2cap_psm_info)[i].used == 1) && (BTJ82SEAP(l2cap_psm_info)[i].l2cap.psm == channel) && 
        	(client_server == BTJ82SEAP(l2cap_psm_info)[i].client_server))
        {

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_ONE_ALLOCATED, i);
            if (BTJ82SEAP(l2cap_psm_info)[i].used_no == 0)
            {
                Assert(0);
            }
            BTJ82SEAP(l2cap_psm_info)[i].used_no--;
            if (BTJ82SEAP(l2cap_psm_info)[i].used_no == 0)
            {
                BtStatus status = BT_STATUS_FAILED;

                status = L2CAP_DeregisterPsm(&BTJ82SEAP(l2cap_psm_info)[i].l2cap);
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_DEREGISTER_PSM_STATUS, status);
                if (status == BT_STATUS_BUSY)
                {
                    U8 p_inx = 0, c_inx = 0;
                    L2CAP_Channel *chnl = NULL;
                    L2capPsm *Protocol = &BTJ82SEAP(l2cap_psm_info)[i].l2cap;

                    btmtk_os_memcpy(
                        (U8*) (&BTJ82SEAP(dummyL2capPsm)),
                        (U8*) (&BTJ82SEAP(l2cap_psm_info)[i].l2cap),
                        sizeof(L2capPsm));
                    BTJ82SEAP(dummyL2capPsm).callback = BTJSR82_L2CapDummyCallback;

                    for (p_inx = 0; p_inx < L2CAP_NUM_PROTOCOLS; p_inx++)
                    {
                        if (L2C(protocols)[p_inx] == Protocol)
                        {
                            /* Check each chnl, because the same psm may be connected via different channels */
                            for (c_inx = 0; c_inx < L2CAP_NUM_CHANNELS; c_inx++)
                            {
                                if (L2C(channels)[c_inx].psmInfo == Protocol)
                                {
                                    chnl = &(L2C(channels)[c_inx]);
                                    /*
                                     * Due to VM disable service, the server's channel state and session_buf will be clean
                                     * If an ongoing connection request (ie., the chnl_state is not L2C_CLOSED), it will call back later
                                     * If this de-register service is due to VM Termination, the psm's callback will be reset to zero
                                     * In this way, event is callbacked to zero function, fatal happens!  
                                     * Replace chnl->psmInfo as a dummyL2capPsm due to de-register L2CAP PSM early 
                                     */
                                    chnl->psmInfo = &(BTJ82SEAP(dummyL2capPsm));
                                    bt_trace(
                                        BT_TRACE_JSR82_GROUP,
                                        BT_JSR82_L2CAP_DEREGISTER_PSM_CHNL_STATE,
                                        c_inx,
                                        chnl->state);
                                }
                            }
                            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FORCE_DEREGISTER_PSM_INX, p_inx);
                            L2C(protocols)[p_inx] = 0;
                        }
                    }

                }
                BTJ82SEAP(l2cap_psm_info)[i].used = 0;
                /*
                 * For L2CAP Server:It will un-register SecurityRecord when disable service or deactivation procedure
                 * For L2CAP Client: It will un-register SecurityRecord when L2EVENT_DISCONNETED
                 * If no security record is registered, no action will be done in SEC_Unregister()
                 */
                SEC_Unregister(&BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord);
                /* Clear l2cap_psm_info context. */
                btmtk_os_memset((unsigned char *)&BTJ82SEAP(l2cap_psm_info)[i], 0, sizeof(BT_JSR82_L2CAP_PSM_struct_t));
            }
            else
            {
                /*
                 * Because this conn of this PSM value's security_level is not zero,
                 * it must set bit-mask previously.
                 * Now, reset this bit in bit-mask table!
                 */
                if ((secruity_level != 0) && BTJ82SEAP(l2cap_psm_info)[i].security_level != 0)
                {
                    /* Modify 2007-1219: Maximum number of conns for same PSM supported in server side is 8 */
                    /* for(j=0;i<8;j++) */
                    for (j = 0; j < 8; j++)
                    {
                        if ((BTJ82SEAP(l2cap_psm_info)[i].security_level & (0x01 << j)) != 0)
                        {
                            break;
                        }
                    }
                    BTJ82SEAP(l2cap_psm_info)[i].security_level &= ~(0x01 << j);
                    bt_trace(
                        BT_TRACE_JSR82_GROUP,
                        BT_JSR82_FREE_L2CAP_PSM_NEW_SEC_VAL,
                        i,
                        BTJ82SEAP(l2cap_psm_info)[i].security_level);
                    if (BTJ82SEAP(l2cap_psm_info)[i].security_level == 0)
                    {
                        SEC_Unregister(&BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord);
                        /*
                         * For server side, this function is invoked at disable service and deactivation procedure
                         * If register record again, once this service is reigstered via enable service request again
                         * Assertion will happen due to List is not circular (the previous registered record's field may be reset due to disable service or deactivation procedure, and it is still in List)
                         */
                        bt_jsr82_L2capRegisterSecurity(
                            &BTJ82SEAP(l2cap_psm_info)[i].l2capSecRecord,
                            BT_SEC_LOW,
                            BTJ82SEAP(l2cap_psm_info)[i].l2cap.psm);
                    }
                }
            }
            return;
        }
    }
    Assert(0);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_find_L2capPSMInContext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  channel             [IN]        
 *  L2capChannelId      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 bt_jsr82_find_L2capPSMInContext(U16 channel, U16 L2capChannelId)
{
    U8 i = 0;
    BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr = 0;

    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        ptr = &BTJ82SEAP(session)[i];

	 if (ptr->status == BT_JSR82_SESSION_ACTIVE)
	 {
	        bt_trace(
	            BT_TRACE_JSR82_GROUP,
	            BT_JSR82_DUMP_FIND_L2CAP_PSM_CNTX,
	            i,
	            ptr->status,
	            ptr->ps_type,
	            ptr->channel,
	            ptr->client_server,
	            ((BT_JSR82_SESSION_Buffer_struct_t*) ptr->session_buffer)->l2capCtx.l2capLocalCid);
	        
	        if ((ptr->ps_type == JSR82_SESSION_PS_L2CAP) &&
	            (ptr->channel == channel) &&
	            (((BT_JSR82_SESSION_Buffer_struct_t*) ptr->session_buffer)->l2capCtx.l2capLocalCid == L2capChannelId))
	        {
	            break;
	        }
	 }
    }
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_L2CAP_PSM_CNTX_RESULT, i, channel, L2capChannelId);
    return i;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_find_L2capPSMListenInContext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  channel             [IN]        
 *  L2capChannelId      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 bt_jsr82_find_L2capPSMListenInContext(U16 channel, U16 L2capChannelId)
{
    U8 i = 0;
    BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr = 0;

    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        ptr = &BTJ82SEAP(session)[i];

	 if (BT_JSR82_SESSION_ACTIVE == ptr->status)
	 {
	        bt_trace(
	            BT_TRACE_JSR82_GROUP,
	            BT_JSR82_DUMP_FIND_L2CAP_PSM_LISTEN_CNTX,
	            i,
	            ptr->status,
	            ptr->ps_type,
	            ptr->channel,
	            ptr->client_server,
	            ((BT_JSR82_SESSION_Buffer_struct_t*) ptr->session_buffer)->l2capCtx.l2capLocalCid);

	        if ((ptr->ps_type == JSR82_SESSION_PS_L2CAP) &&
	            (ptr->channel == channel) && (ptr->client_server == BT_JSR82_SESSION_SERVER) &&
	            (((BT_JSR82_SESSION_Buffer_struct_t*) ptr->session_buffer)->l2capCtx.l2capLocalCid == 0))
	        {
	            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_L2CAP_PSM_LISTEN_CNTX_RESULT, i, channel, L2capChannelId);
	            return i;
	        }
	 }
    }

    return i;
}


void bt_jsr82_ClearSessionContext(U8 session_inx)
{
	BTJ82SEAP(session)[session_inx].client_server = 0;
	btmtk_os_memset(BTJ82SEAP(session)[session_inx].bd_addr, 0, 6);
	BTJ82SEAP(session)[session_inx].ps_type = 0;
	BTJ82SEAP(session)[session_inx].channel = 0;
	BTJ82SEAP(session)[session_inx].identify = 0;
	//BTJ82SEAP(session)[session_inx].channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
	//BTJ82SEAP(session)[session_inx].status = BT_JSR82_CHANNEL_STATE_IDLE;
#ifdef __JSR82_SESSION_SHARED_MEMORY__
	jsr82_core_free_ashm(session_inx, JSR82_ASHM_TYPE_CHANNEL_BUFFER);		
#elif defined(__JSR82_SESSION_DYNAMIC_MEMORY__)
	if (BTJ82SEAP(session)[session_inx].session_buffer != NULL)
	{
		btmtk_jsr82_free(BTJ82SEAP(session)[session_inx].session_buffer );
	}
#endif
	BTJ82SEAP(session)[session_inx].session_buffer = NULL;
	BTJ82SEAP(session)[session_inx].authorize = 0;
	BTJ82SEAP(session)[session_inx].security_value = 0;
	BTJ82SEAP(session)[session_inx].mtu = 0;
	//BTJ82SEAP(session)[session_inx].link = NULL;
	BTJ82SEAP(session)[session_inx].is_tx_empty = KAL_FALSE;
	BTJ82SEAP(session)[session_inx].notifyReadyToWrite = KAL_FALSE;
	BTJ82SEAP(session)[session_inx].readyToRead = KAL_FALSE;
	BTJ82SEAP(session)[session_inx].wait_data_available_ind_rsp_length = 0;
	BTJ82SEAP(session)[session_inx].deliverBufPtr = NULL;
#ifdef __JSR82_SESSION_SHARED_MEMORY__
	jsr82_core_free_ashm(session_inx, JSR82_ASHM_TYPE_DELIVER_GET);
#endif
	BTJ82SEAP(session)[session_inx].deliverBufSize = 0;
	BTJ82SEAP(session)[session_inx].deliverPutBufPtr = NULL;
#ifdef __JSR82_SESSION_SHARED_MEMORY__
	jsr82_core_free_ashm(session_inx, JSR82_ASHM_TYPE_DELIVER_PUT);
#endif
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApDisconnectAllAcl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApDisconnectAllAcl(void)
{
    /*
     * This function is invoked due to JSR82 PowerOff to want to terminate VM: Add by mtk01411 2007-1127
     * Turn off All registration Services to reject all possible incoming connections during disconnecting link period
     */
    int i = 0;

    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        BTJ82SEAP(session)[i].authorize = BT_JSR82_AUTHORIZE_AUTO_REJECT;
    }

    if (BTJ82SEAP(jsr82_con_req_timer).func != 0)
    {
        bt_jsr82_connect_req_struct *ptr = &BTJ82SEAP(jsr82_con_req);
        JSR82CallbackParms parms;

        EVM_CancelTimer(&BTJ82SEAP(jsr82_con_req_timer));
		 parms.p.connect_cnf.bd_addr = (U8*) ptr->bd_addr;
		 parms.p.connect_cnf.ps_type = ptr->ps_type;
		 parms.p.connect_cnf.mtu = ptr->mtu;
		 parms.p.connect_cnf.in_mtu = 0;
		 parms.p.connect_cnf.channel = ptr->channel;
		 parms.p.connect_cnf.index = 0;
		 parms.p.connect_cnf.identify = ptr->identify;
		 parms.p.connect_cnf.l2cap_id = 0;
		 parms.p.connect_cnf.result = JSR82_SESSION_CONNECT_CLIENT_FAILED;
		 parms.event = JSR82_EVENT_CONNECT_CNF;
		 parms.dstMod = MOD_MMI;
		 JSR82CB(&parms);

        BTJ82SEAP(jsr82_con_req_timer).func = 0;
        btmtk_os_memset((U8*) & BTJ82SEAP(jsr82_con_req), 0, sizeof(bt_jsr82_connect_req_struct));
    }

    if (BTJ82SEAP(jsr82_sdp_req_timer).func != 0)
    {
        bt_bm_search_raw_req_struct *ptr = &BTJ82SEAP(jsr82_sdp_req);

        EVM_CancelTimer(&BTJ82SEAP(jsr82_sdp_req_timer));

        BTJ82SEAP(jsr82_sdp_req_timer).func = 0;
        btmtk_os_memset((U8*) & BTJ82SEAP(jsr82_sdp_req), 0, sizeof(bt_bm_search_raw_req_struct));
    }


    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TURN_OFF_ALL_SERVICES_DISC_ALL_LINKS);
    EVM_ResetTimer(&BTJ82SEAP(jsr82_disconnect_all_acl));
    bt_jsr82_ACLCheckAndDisconnectAclNo();
    BTJ82SEAP(jsr82_disconnect_all_acl).func = bt_jsr82_ACLCheckDisconnectTimer;
    EVM_StartTimer(&BTJ82SEAP(jsr82_disconnect_all_acl), 1000);

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_ACLAddLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_ACLAddLink(BtRemoteDevice *link)
{
    U8 i = 0;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ACL_ADD_LINK, link);
    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].link == link)
        {

            BTJ82SEAP(active_link)[i].used_no++;
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_A_ACTIVE_LINK_USED_NO, BTJ82SEAP(active_link)[i].used_no);
            return;

        }
    }
    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].used == 0)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_FIND_ONE_EMPTY_IN_ADD_LINK);
            BTJ82SEAP(active_link)[i].used = 1;
            BTJ82SEAP(active_link)[i].link = link;
            /* Add set discFlag as FALSE: 2008-0110 */
            BTJ82SEAP(active_link)[i].link->discFlag = FALSE;
            BTJ82SEAP(active_link)[i].used_no = 1;
            return;

        }
    }
    Assert(0);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_ACLRemoveLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_ACLRemoveLink(BtRemoteDevice *link)
{
    U8 i = 0;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REMOVE_ACL_LINK, link);
    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].link == link)
        {

            BTJ82SEAP(active_link)[i].used_no--;
            if (BTJ82SEAP(active_link)[i].used_no == 0)
            {
                BTJ82SEAP(active_link)[i].used = 0;
                /* Add by mtk01411: 2007-1126 */
                BTJ82SEAP(active_link)[i].link = NULL;
            }
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_REMOVE_LINK_USED_NO, BTJ82SEAP(active_link)[i].used_no);
            return;
        }
        else
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_NEXT_CHECK_IN_REMOVE_LINK, BTJ82SEAP(active_link)[i].link);
        }
    }
    Assert(0);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_ACLCheckAndDisconnectAclNo
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_ACLCheckAndDisconnectAclNo(void)
{
    U8 i = 0, current_active_link_num = 0;
    BtRemoteDevice *link = 0;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ACLCHECKANDDISCONNECTACLNO);
    BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) = 0;

    /* Check if there is any active link but it is not in BTJ82SEAP(active_link) */
    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        int j = 0;

        for (j = 0; j < JSR82_MAX_ACL_NO; j++)
        {
            if (BTJ82SEAP(active_link)[j].used == 1)
            {
                if (BTJ82SEAP(session)[i].link == BTJ82SEAP(active_link)[j].link)
                {
                    break;
                }
            }
        }

        if (j == JSR82_MAX_ACL_NO)
        {
            /*  In this case: it means that BTJ82SEAP(session)[i].link is not in BTJ82SEAP(active_link)[] */
            if ((BTJ82SEAP(session)[i].link != NULL) && (BTJ82SEAP(session)[i].link->state == BDS_CONNECTED))
            {
                if ((BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_RFCOMM) &&
                    (BTJ82SEAP(session)[i].channel_state == BT_JSR82_CHANNEL_STATE_OPEN))
                {
                    Assert(0);
                }
                else if ((BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_L2CAP) && (((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[i].session_buffer)->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_OPEN))
                {
                    Assert(0);
                }
            }
        }
    }

    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].used == 1)
        {
            current_active_link_num++;
            Assert(BTJ82SEAP(active_link)[i].link != 0);
            link = BTJ82SEAP(active_link)[i].link;
            if (!IsNodeOnList(&(link->handlers), &(jsr82LinkHandler.node)))
            {
                InsertTailList(&(link->handlers), &(jsr82LinkHandler.node));
            }
            link->discFlag = TRUE;
            ME_ForceDisconnectLinkWithReason(&jsr82LinkHandler, link, BEC_POWER_OFF, TRUE);
            if (IsNodeOnList(&(link->handlers), &(jsr82LinkHandler.node)))
            {
                RemoveEntryList(&(jsr82LinkHandler.node));
            }
        }
    }
    if (current_active_link_num != 0)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CHECK_DISC_ACL_NO, current_active_link_num);
        BTJ82SEAP(jsr82_disconnect_all_acl).func = bt_jsr82_ACLCheckDisconnectTimer;
        EVM_StartTimer(&BTJ82SEAP(jsr82_disconnect_all_acl), 1000);
    }

    /*
     * Check if any session entry is client and status in active (This means that one ongoing connection request) 
     * In this case, it must wait for link establishment then disc it : 2008-0110
     */
    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        if ((BTJ82SEAP(session)[i].client_server == BT_JSR82_SESSION_CLIENT) &&
            (BTJ82SEAP(session)[i].status == BT_JSR82_SESSION_ACTIVE))
        {
            if ((BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_RFCOMM) &&
                (BTJ82SEAP(session)[i].channel_state != BT_JSR82_CHANNEL_STATE_OPEN))
            {
                BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no)++;
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CHECK_DISC_ACL_RFCOMM_CON_REQ_ONGOING);
            }
            else if ((BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_L2CAP) && (((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[i].session_buffer)->l2capCtx.l2cap_con_state != BT_JSR82_CHANNEL_STATE_OPEN))
            {
                BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no)++;
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CHECK_DISC_ACL_L2CAP_CON_REQ_ONGOING);
            }

        }
    }

    /*
     * [Question] Does it need check if there is L2CAP Server with connect request ongoin?
     * [Answer]: Not necessary
     * [Reason]:
     * In this step, it may de-register L2CAP service due to VM's Termination procedure
     * Actually, we already handle this case in de-register:
     * If there is any chnl belongs to this PSM is not in L2C_CLOSED (e.g., wait for authentication result)
     * We replace chnl->psmInfo to dummyL2cappsm, in this way:
     * the L2EVENT_CONNECT_IND will be reported to BTJSR82_L2CapDummyCallback() and rejects it due to de-registered service
     * If it does not handle this case, the chnl->psmInfo->callback is 0 (due to context is reset of VM Termination procedure),
     * the abnormal reset fatal error will happen in BT Stack
     */

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_ACLCheckDisconnectTimer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_ACLCheckDisconnectTimer(EvmTimer *Timer)
{
    U8 i = 0;
    BtRemoteDevice *link = 0;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CHECK_DISC_TIMER);
    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].used == 1)
        {
            /* Check this active link is not yet disconnecting or it is a new setup link after the previous disc request */
            Assert(BTJ82SEAP(active_link)[i].link != 0);
            link = BTJ82SEAP(active_link)[i].link;
            if (link->discFlag == TRUE)
            {
                /* In this case: This link is still disconnecting */
                BTJ82SEAP(jsr82_disconnect_all_acl_counter)++;
                if (BTJ82SEAP(jsr82_disconnect_all_acl_counter) == 10)
                {
                    Assert(0);
                }
                EVM_StartTimer(&BTJ82SEAP(jsr82_disconnect_all_acl), 1000);
                return;
            }
            else
            {
                Assert(0);
            }
        }
    }
    BTJ82SEAP(jsr82_disconnect_all_acl_counter) = 0;
    EVM_ResetTimer(&BTJ82SEAP(jsr82_disconnect_all_acl));
    if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) == 0)
    {
        JSR82CallbackParms parms;
        //bt_jsr82_SessionApDisconnectAllAclCfn();
	 parms.event = JSR82_EVENT_DISCONNECT_ALL_ACL_CNF;
	 parms.dstMod = MOD_MMI;
	 JSR82CB(&parms);
    }
    else
    {
        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_DISC_TIMER_PEND_CON_REQ_NO,
            BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no));
    }
}

static void bt_jsr82_event(BT_JSR82_SESSION_AP_Record_Context_struct_t *session, JSR82_event event, JSR82_SESSION_RESULT result)
{
	JSR82CallbackParms parms;
	BT_JSR82_SESSION_Buffer_struct_t *session_buffer = (BT_JSR82_SESSION_Buffer_struct_t *)session->session_buffer;

	memset(&parms, 0x0, sizeof(JSR82CallbackParms));
	parms.dstMod = MOD_MMI;
	
	switch(event)
	{
	 case JSR82_EVENT_CONNECT_CNF:	 	
		parms.p.connect_cnf.bd_addr = (U8*) session->bd_addr;
		parms.p.connect_cnf.ps_type = session->ps_type;
		if (JSR82_SESSION_PS_RFCOMM == session->ps_type)
		{
			parms.p.connect_cnf.in_mtu = session->mtu;
			parms.p.connect_cnf.mtu = session_buffer->rfc.maxFrameSize;
		}
		else
		{
			if (result == JSR82_SESSION_RESULT_SUCCESS)
			{
				parms.p.connect_cnf.mtu = session_buffer->l2capCtx.channel->outMtu;
			}
			parms.p.connect_cnf.in_mtu = session->mtu; 
		}
		parms.p.connect_cnf.channel = session->channel;
		parms.p.connect_cnf.index = session->con_id;
		parms.p.connect_cnf.identify = session->identify;
		parms.p.connect_cnf.l2cap_id = 0;
		parms.p.connect_cnf.result = result;
		parms.event = JSR82_EVENT_CONNECT_CNF;
		parms.dstMod = MOD_MMI;
		break;

	case JSR82_EVENT_DISCONNECT_IND:
		parms.p.disconnect_ind.ps_type = session->ps_type;
		parms.p.disconnect_ind.con_id = session->con_id;
		parms.p.disconnect_ind.identify = session->identify;
		parms.p.disconnect_ind.l2cap_id = 0;
		break;

	case JSR82_EVENT_CONNECT_IND:
		parms.p.connect_ind.bd_addr = session->bd_addr;
		parms.p.connect_ind.ps_type = session->ps_type;
		parms.p.connect_ind.mtu = session_buffer->rfc.maxFrameSize;
		parms.p.connect_ind.in_mtu = session->mtu;
		parms.p.connect_ind.channel = session->channel;
		parms.p.connect_ind.index = session->con_id;
		parms.p.connect_ind.identify = session->identify;
		parms.p.connect_ind.l2cap_id = 0;
		break;

	case JSR82_EVENT_DISABLE_SERVICE_CNF:
		parms.p.disable_service_cnf.ps_type = session->ps_type;
		parms.p.disable_service_cnf.con_id = session->con_id;
		parms.p.disable_service_cnf.identify = session->identify;
		parms.p.disable_service_cnf.result = result;
		break;

	case JSR82_EVENT_DISCONNECT_ALL_ACL_CNF:
		break;
		
	default:
		return;
	}

	parms.event = event;
	JSR82CB(&parms);
}


void bt_jsr82_rfcomm_connect(BT_JSR82_SESSION_AP_Record_Context_struct_t *session)
{
	BT_JSR82_SESSION_Buffer_struct_t *session_buffer = NULL;
	RfChannel *channel = NULL;
    BtStatus ret_status = BT_STATUS_FAILED;

	OS_Report("[JSR82]bt_jsr82_rfcomm_connect");

	if (NULL == session)
	{
		OS_Report("[JSR82]invalid session context");
		return;
	}

	if (session->channel == 0 || session->channel > 32)
	{	
		OS_Report("[JSR82]invalid channle number %x", session->channel);		
		goto exit;
	}

	session_buffer = (BT_JSR82_SESSION_Buffer_struct_t*) session->session_buffer;	
	session_buffer->devCredit = 1;
    channel = &session_buffer->rfc;
    channel->callback = BTJSR82_RfCallbackFunc;
    channel->maxFrameSize = session->mtu;
    channel->priority = 63;
	
    if (session->security_value == 0x00)
    {
        RF_ChannelInit(channel, BT_SEC_LOW);
    }
    else
    {
        RF_ChannelInit(channel, BT_SEC_MED);
    }
	
	session->channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
	ret_status = RF_OpenClientChannel(
                            session_buffer->cmgrHandler.remDev,
                            (U8) (session->channel),
                            channel,
                            session_buffer->devCredit);
	if (ret_status != BT_STATUS_PENDING)
	{
		SEC_Unregister(&channel->secRecord);
		CMGR_RemoveDataLink(&session_buffer->cmgrHandler);
		CMGR_RemoveCmgrWithDevice(&session_buffer->cmgrHandler);
		CMGR_DeregisterHandler(&session_buffer->cmgrHandler);
		session->status = BT_JSR82_SESSION_IDLE;
		bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_CON_REQ_OPEN_RFCOMM_CHNL_FAILED);	
		bt_jsr82_event(session, JSR82_EVENT_CONNECT_CNF, JSR82_SESSION_RESULT_FAIL);
    }
	ret_status = BT_STATUS_PENDING;
	
exit:
	if (ret_status == BT_STATUS_FAILED)
		bt_jsr82_event(session, JSR82_EVENT_CONNECT_CNF, JSR82_SESSION_RESULT_FAIL);

	return;
}


void bt_jsr82_Rfcomm_closed(EvmTimer *Timer)
{
	
	OS_Report("[JSR82]bt_jsr82_Rfcomm_closed, context addr is %x", Timer->context);

	BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr1 = Timer->context;
	if (ptr1->channel_state == BT_JSR82_CHANNEL_STATE_IDLE)
	{
		if (ptr1->client_server == BT_JSR82_SESSION_CLIENT)
		{
			//set channel state
			ptr1->status = BT_JSR82_SESSION_IDLE;
			bt_jsr82_event(ptr1, JSR82_EVENT_CONNECT_CNF, JSR82_SESSION_RESULT_FAIL);

			/* Check if VM termination procedure is ongoing or not */
			BTJSR82_CheckSendVMDiscAllLinksCnf(1);

		}
		else
		{
			bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFSRV_CLOSED);
		}
	}
 	else
	{
		ptr1->channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
        /* Before CLOSING:For both Client and Server's channel_stata is in BT_JSR82_CHANNEL_STATE_OPEN */        
        bt_jsr82_event(ptr1, JSR82_EVENT_DISCONNECT_IND, 0);

		if (BT_JSR82_SESSION_CLIENT == ptr1->client_server)
		{
			// Free ring buffer
			jsr82_session_free_RWRingBuf(ptr1->con_id);     

			// Clear session context
			bt_jsr82_ClearSessionContext(ptr1->con_id);
			ptr1->status = BT_JSR82_SESSION_IDLE;
		}
		else
		{
			// Clear ring buffer
		  	RING_BufFlush(&(BTJ82SEAP(session)[ptr1->con_id].ReadRingBuf));
		  	RING_BufFlush(&(BTJ82SEAP(session)[ptr1->con_id].WriteRingBuf));
		  	// Reset Ring buffer related context
			BTJ82SEAP(session)[ptr1->con_id].is_tx_empty = KAL_TRUE;
			BTJ82SEAP(session)[ptr1->con_id].notifyReadyToWrite = KAL_FALSE;
			BTJ82SEAP(session)[ptr1->con_id].readyToRead = KAL_TRUE;
			BTJ82SEAP(session)[ptr1->con_id].wait_data_available_ind_rsp_length = 0;
		  }
	}
	ptr1->channelTimer.context = NULL;
	ptr1->channelTimer.func = NULL;

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_RfCallbackFunc
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chan        [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_RfCallbackFunc(RfChannel *chan, RfCallbackParms *parms)
{
    BT_JSR82_SESSION_Buffer_struct_t *ptr = NULL;
    BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr1 = NULL;
    U16 subsession_id = 0;
	JSR82CallbackParms cbparms;

    ptr = ContainingRecord(chan, BT_JSR82_SESSION_Buffer_struct_t, rfc);
    ptr1 = (BT_JSR82_SESSION_AP_Record_Context_struct_t*) ptr->mainRecord;
    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_RFCALLBACK_INFO,
        ptr1->con_id,
        parms->event,
        chan,
        ptr,
        ptr1,
        parms->ptrs.remDev);

    switch (parms->event)
    {
        case RFEVENT_PACKET_HANDLED:

	     /* Return directly if this connection is already disconnected. */
	     if (BT_JSR82_CHANNEL_STATE_OPEN != ptr1->channel_state)
	     {
	     	  return ;
	     }
            /*
             * Add by mtk01411: 2007-0916
             * (1) Delete data carried in this packet from the WriteRingBuf: Need to add codes!
             */
            jsr82_session_ReturnBuf(
                JSR82_SESSION_PS_RFCOMM,
                ptr1->con_id,
                subsession_id,
                (char*)parms->ptrs.packet->data,
                parms->ptrs.packet->dataLen);

            InitializeListEntry(&parms->ptrs.packet->node);
            /*
             * Add by mtk01411: 2007-0916
             * (2) Return this txPacket to List 
             */
            InsertTailList(&ptr1->ApTxQueue, &parms->ptrs.packet->node);
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_TX_COMPLETED);

    #if 0   /* Disabled by mtk01411: 2007-0916 */
            bt_jsr82_SessionApTxCfn(ptr1->identify, 0, JSR82_SESSION_TX_ERROR);
            if(index == JSR82_MAX_SESSION_NO)
            {
                Report(("BT_JSR82_RFCOMM_CALLBACK jsr82 not find correct Channel"));
                return;
            }
    #endif /* 0 */ 
#ifdef __JSR82_MAUI__    
            if (BT_JSR82QueryPowerStatus() == 1)
#endif
            {
                /* (3) Check if there is any available data needed to be sent or not */
                bt_jsr82_sendToRfComm(ptr1->con_id);
            }

            break;
        case RFEVENT_CLOSE_IND:
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFEVENT_CLOSED_IND);
            break;
        case RFEVENT_CLOSED:
            if (ptr1->client_server == BT_JSR82_SESSION_CLIENT)
            {
                /* Check the status code to determine if this is authencation failure */
                if (parms->status == BT_STATUS_RESTRICTED)
                {
                    /* Check if the paired record is in DB, if it exists: It must be deleted */
                    BtStatus status = BT_STATUS_FAILED;

                    status = SEC_DeleteDeviceRecord(&(parms->ptrs.remDev->bdAddr));
                    bt_trace(
                        BT_TRACE_JSR82_GROUP,
                        BT_JSR82_RFCOMM_CLOSED_DUE_TO_ACCESS_DENIED_DEL_DEV_RECORD_STATUS,
                        status);
                }

                if (ptr1->channel_state != BT_JSR82_CHANNEL_STATE_IDLE)
                {
                    bt_jsr82_ACLRemoveLink(BTJ82SEAP(session)[ptr1->con_id].link);
                    BTJ82SEAP(session)[ptr1->con_id].link = 0;
                }
                CMGR_RemoveDataLink(&(ptr->cmgrHandler));
                CMGR_RemoveCmgrWithDevice(&(ptr->cmgrHandler));
                CMGR_DeregisterHandler(&(ptr->cmgrHandler));
            }
            else
            {
                /*
                 * Add 2007-1026: For SPP Server, it must reset devCredit and highWaterMarkValue 
                 * These two values will be re-caculated when receiving RFEVET_OPEN:
                 * Because the session entry will not be released, but for each connection, devCredit must reset to 1
                 */

                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFEVENT_CLOSED_RESET_SESSION_CREDIT);
                /* For Server: If the channel_state is not IDLE, it must remove LINK's use_no count: 2007-1112 */
                if (ptr1->channel_state != BT_JSR82_CHANNEL_STATE_IDLE)
                {
                    bt_jsr82_ACLRemoveLink(BTJ82SEAP(session)[ptr1->con_id].link);
                    BTJ82SEAP(session)[ptr1->con_id].link = 0;
                }
                ptr->devCredit = 1;
                ptr->highWaterMarkValue = 0;
                CMGR_RemoveDataLink(&(ptr->cmgrHandler));
                CMGR_RemoveCmgrWithDevice(&(ptr->cmgrHandler));
            }

			/*Delay 1s to notify MMI the disconnect indication*/
			EVM_ResetTimer(&(BTJ82SEAP(session)[ptr1->con_id].channelTimer));
			BTJ82SEAP(session)[ptr1->con_id].channelTimer.func = bt_jsr82_Rfcomm_closed;
			BTJ82SEAP(session)[ptr1->con_id].channelTimer.context = &(BTJ82SEAP(session)[ptr1->con_id]);
			OS_Report("[JSR82] context addr is %x", &BTJ82SEAP(session)[ptr1->con_id]);
			if(!BTCoreVerifySysInitState() || 
				BTCoreIsDeinitializing() ||
				(FALSE == jsr82_session_DataRemainInBuff(ptr1->ps_type, 
													ptr1->con_id,
													subsession_id)))
			{				
				OS_Report("[JSR82] bt is powering off or power off or no any data in buffer(tx/rx)");		
				bt_jsr82_Rfcomm_closed(&(BTJ82SEAP(session)[ptr1->con_id].channelTimer));
    		}
			else
			{
				EVM_StartTimer(&(BTJ82SEAP(session)[ptr1->con_id].channelTimer), 1000);
			}			
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCHNL_DISC_COMPLETED);
            break;
        case RFEVENT_DATA_IND:
        {
            kal_int8 rc = 0;

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_RX_IND_LEN, parms->dataLen);
            /* (0) Substract one from devCredit: Add by mtk01411: 2007-1022 */
            ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[ptr1->con_id].session_buffer))->devCredit--;

#ifdef __JSR82_MAUI__    
            if (BT_JSR82QueryPowerStatus() == 1)
#endif
            {
                /* (1) Put the received data to the corresponding ReadRingBuf  Add by mtk01411: 2007-0916 */
                rc = jsr82_session_DevRX(
                        JSR82_SESSION_PS_RFCOMM,
                        ptr1->con_id,
                        subsession_id,
                        (char*)parms->ptrs.data,
                        &(parms->dataLen));
                /* NOT in POWERON state:2007-1115 */
                if ((signed char) rc < 0)
                {
                    return;
                }

                /* (2) Update credit information */
                bt_jsr82_advanceCredit(ptr1->con_id, KAL_TRUE);
            }

        #if 0   /* Disabled by mtk01411: 2007-0916 */
            if( parms->dataLen <= JSR82_SESSION_MAX_RX_DATA)
            {
                OS_MemCopy(ptr->data, parms->ptrs.data, parms->dataLen);
                bt_jsr82_SessionApRxInd(ptr1->identify, 0, parms->dataLen, ptr->data);
            }
            else
                bt_trace(BT_TRACE_JSR82_GROUP,RX_DATA_EXCEED_ACCEPTABLE_VALUEx04X , parms->dataLen);
        #endif /* 0 */ 
        }
            break;
        case RFEVENT_OPEN_IND:
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_OPEN_IND_AUTHORIZE_SETTING, ptr1->authorize);
            /*
             * The first available session registered to RFCOMM will be choosed for next connection.
             * For example, if we register channel 15 twice and set AUTHORIZE_AUTO_REJECT to the first session, 
             * the connection from remote device to channel 15 will always be rejected.
             */
            switch (ptr1->authorize)
            {
                case BT_JSR82_AUTHORIZE_AUTO_REJECT:
                    RF_RejectChannel(chan);
                    break;
                case BT_JSR82_AUTHORIZE_AUTO_ACCEPT:
                    RF_AcceptChannel(chan);
                    break;

				//notify upper layer to decide it 
				case BT_JSR82_AUTHORIZE_REQUEST_ACCEPT:
					bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFSRV_OPEN);
					CMGR_CreateDataLink(&(ptr->cmgrHandler), &(parms->ptrs.remDev->bdAddr));
				//	ptr1->inmtu = RF_FrameSize(chan);
					btmtk_os_memcpy(ptr1->bd_addr, parms->ptrs.remDev->bdAddr.addr, 6);
				
					bt_jsr82_event(ptr1, JSR82_EVENT_CONNECT_IND, 0);
					break;
				
            }
            break;
        case RFEVENT_OPEN:
            if (parms->status == BT_STATUS_SUCCESS)
            {
                U8 rc = 0;

                /* Add by mtk01411: 2007-1112 */
                BTJ82SEAP(session)[ptr1->con_id].link = chan->remDev;
                bt_jsr82_ACLAddLink(BTJ82SEAP(session)[ptr1->con_id].link);

                /* Because the highWaterMark value is only used in BTstack, keeps this value in BTstack instead of JBT's context: 2007-1022 */
                bt_jsr82_setHighWaterMark(chan, ptr);

                /* Compute credit: Add by mtk01411: 2007-1022 */
                bt_jsr82_advanceCredit(ptr1->con_id, KAL_FALSE);

		//force to do role switch to avoid FW limitation: scatternet is not supported
		//TODO: remove it if this limitation is resolved
		CMGR_SetDeviceAsMaster(&(ptr->cmgrHandler));

		  // MTU (of remote device) should be updated for future data transmission
                BTJ82SEAP(session)[ptr1->con_id].mtu= ptr->rfc.maxFrameSize;

                if (ptr1->client_server == BT_JSR82_SESSION_CLIENT)
                {
                	rc = JSR82_SESSION_CONNECT_CLIENT_SUCCESS;
			      	bt_jsr82_event(&BTJ82SEAP(session)[ptr1->con_id], JSR82_EVENT_CONNECT_CNF,JSR82_SESSION_RESULT_SUCCESS);
                }
                else
                {
                }
                ptr1->channel_state = BT_JSR82_CHANNEL_STATE_OPEN;

                /*
                 * Because VM termination all link request may be issued when connecting (client con req or server accept conn)
                 * Now, channel is open, it is time to disconnect this link
                 */
                BTJSR82_CheckVMDiscAllLinksReq(BTJ82SEAP(session)[ptr1->con_id].link, JSR82_SESSION_PS_RFCOMM);

            }
            else
            {
                /* Note: Free mem space allocated for session's Read and Write RingBuf */
                /* Modify by mtk01411: 2007-1024 */
                /* jsr82_session_free_RWRingBuf_with_inx(JSR82_SESSION_PS_RFCOMM, ptr1->con_id); */
                Assert(0);
            }
            break;

    }
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_L2CapDummyCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  L2ChannelId     [IN]        
 *  info            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_L2CapDummyCallback(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{
    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_L2CAP_DUMMY_CALLBACK_CID_EVENT_STATUS,
        L2ChannelId,
        info->event,
        info->status);

    if ((info->event == L2EVENT_CONNECT_IND) && (info->status == BT_STATUS_SUCCESS))
    {
        /*
         * In this case, JSR82 L2CAP Service is already de-registered due to VM Termination
         * After sending con_rsp with result= reject_no_resources, then:
         * L2CAP_HciTxDone() -> L2Cap_StateMachine(chnl,LLC_TX_DONE) -> L2CapState_W4_CONN_RSP(chnl,Event)
         * -> L2Cap_NotifyUserAndCloseChannel(chnl,L2DISC_USER_REQUEST)
         * -> L2Cap_NotifyPsm(Chnl,L2EVENT_DISCONNECTED,status); status=L2DISC_USER_REQUEST(0x0001)
         */
        L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_L2capCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  L2ChannelId     [IN]        
 *  info            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_L2capCallback(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{
    BT_JSR82_SESSION_Buffer_struct_t *ptr = NULL;
    BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr1 = NULL;
    const L2capPsm *l2cap_psm = NULL;
    L2CAP_Channel *channel = NULL;
    U8 index = 0;
    U8 session_index = 0;
    BtStatus status = BT_STATUS_FAILED;

    l2cap_psm = info->owner.psm;
    // TODO: How to find the index with only PSM & without l2cap_id?
    // In L2EVENT_CONNECT_IND case, l2cap_id doesn't exist in context yet. (Server)
    session_index = bt_jsr82_find_L2capPSMInContext(l2cap_psm->psm, L2ChannelId);
    if (session_index < JSR82_MAX_SESSION_NO)
    {
        ptr1 = &BTJ82SEAP(session)[session_index];
        ptr = (BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[session_index].session_buffer);
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAPCALL_INFO, session_index, L2ChannelId, ptr1->con_id, info->event);
    }
    else
    {
	 bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAPCALL_INFO_NO_MATCHED_CONTEXT, L2ChannelId, info->event);
    }

    switch (info->event)
    {
        case L2EVENT_CONNECT_IND:
            channel = L2Cap_GetValidChannel(L2ChannelId);
            if (channel == 0)
            {
                L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
            }
            else
            {
                session_index = bt_jsr82_find_L2capPSMListenInContext(l2cap_psm->psm, L2ChannelId);
                if ((session_index != JSR82_MAX_SESSION_NO) &&
                    (BTJ82SEAP(session)[session_index].authorize != BT_JSR82_AUTHORIZE_AUTO_REJECT))
                {
                    status = bt_jsr82_AddNewL2capToContext(
                                (BT_JSR82_SESSION_Buffer_struct_t *) (BTJ82SEAP(session)[session_index].session_buffer),
                                L2ChannelId);
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CON_IND_STATUS, status);

                }
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_NOT_ADD_NEW_L2CAP_CNTX, session_index);
                if (status != BT_STATUS_SUCCESS)
                {
                    L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
                }
                else
                {
                    L2CAP_ConnectRsp(L2ChannelId, L2CONN_ACCEPTED, 0);
                }
            }
            break;
        case L2EVENT_DISCON_IND:
            /* Add DISCON_IND handler: 2008-0213 */
            index = bt_jsr82_SearchL2capContext(L2ChannelId);
            /* Report(("BT_JSR82_L2EVENT_DISCCON_IND: inx=%d,l2cap_cid=%d",index,L2ChannelId)); */
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2_DISCON_IND_INX_L2CID, index, L2ChannelId);
            break;
        case L2EVENT_DISCONNECTED:
            if (JSR82_MAX_SESSION_NO == session_index)
            {
            	  /* 
            	   * This event is generated after the CONNECT_IND has been rejected. (Server)
            	   * L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0); 
            	   * Nothing to do here.
            	   */
            	   kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CALLBACK_L2EVENT_DISCONNECTED_DUE_TO_REJECT);
            	   return ;
            }
            /* 
             * In this event, the channel state is L2C_CLOSED and return pointer will be NULL. 
             * Because this information is not necessary besides trace output, L2Cap_GetValidChannel() action should be removed.
             */
            //channel = L2Cap_GetValidChannel(L2ChannelId);
            //bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_DISCONNECTED_CHNL_LINK, channel, channel->link);
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_DISCONNECTED_CHNL_LINK, 0, BTJ82SEAP(session)[session_index].link);
            index = bt_jsr82_SearchL2capContext(L2ChannelId);
            if (index == JSR82_MAX_SESSION_NO)
            {
                break;
            }
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_DISCONNECTED_INX_CON_ID, index, ptr1->con_id);
            Assert(index == ptr1->con_id);
            index = ptr1->con_id;

            /* Check if this L2EVENT_DISCONNECTED is trigged due to Authentication failure or not */
            if (info->status == L2DISC_SECURITY_BLOCK)
            {
                BtStatus sec_status = BT_STATUS_FAILED;

                Assert(info->aclLink != NULL);
                sec_status = SEC_DeleteDeviceRecord(&(info->aclLink->bdAddr));
                bt_trace(
                    BT_TRACE_JSR82_GROUP,
                    BT_JSR82_L2CAP_DISCONNECTED_DUE_TO_SECURITY_BLOCK_DEL_DEV_RECORD_STATUS,
                    sec_status);
            }

            if (BTJ82SEAP(session)[index].client_server == BT_JSR82_SESSION_CLIENT)
            {
                JSR82CallbackParms parms;
                CMGR_RemoveDataLink(&(ptr->cmgrHandler));
                CMGR_RemoveCmgrWithDevice(&(ptr->cmgrHandler));
                CMGR_DeregisterHandler(&(ptr->cmgrHandler));

                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_DISCONNECTED_STATE, ptr->l2capCtx.l2cap_con_state);
                if ((ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_IDLE) ||
                    (ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL))
                {
                	bt_jsr82_event(ptr1, JSR82_EVENT_CONNECT_CNF, JSR82_SESSION_RESULT_FAIL);

                    if (ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL)
                    {
                        /* Check if VM termination procedure is ongoing or not */
                        BTJSR82_CheckSendVMDiscAllLinksCnf(2);

                    }
                }
                else
                {
                    bt_jsr82_ACLRemoveLink(BTJ82SEAP(session)[session_index].link);
                    BTJ82SEAP(session)[session_index].link = 0;
                    bt_jsr82_event(ptr1, JSR82_EVENT_DISCONNECT_IND, 0);
                }
                /* Add 2007-1214: to reset l2cap_con_state and ptr->l2capCtx.l2cap_con_state on Client side */
                bt_jsr82_ReleaseL2capFromContext(ptr, L2ChannelId);
                /* Because the L2CAP client will register L2CAP PSM at each time l2cap connection setup, it must free PSM index and de-register L2CAP PSM:2007-1216 */
                bt_jsr82_free_L2capPSMIndex(
                    BTJ82SEAP(session)[session_index].channel,
                    BTJ82SEAP(session)[session_index].security_value, 
                    BT_JSR82_SESSION_CLIENT);
                /* Only Cliet side needs reset session_status back to IDLE state: 2007-1112 */
                ptr1->status = BT_JSR82_SESSION_IDLE;

	        // Free ring buffer
		 jsr82_session_free_RWRingBuf(session_index);        
	        // Clear session context
	        bt_jsr82_ClearSessionContext(session_index);
            }
            else
            {
                /* Add 2007-1214: to reset l2cap_con_state and ptr->l2capCtx.l2cap_con_state on Server side */
                /* Check if this chnl is in Open State first: Add 2008-0213 */
                BtRemoteDevice *active_link = BTJ82SEAP(session)[index].link;

                if ((active_link != NULL) && (ptr->l2capCtx.l2cap_con_state == BT_JSR82_CHANNEL_STATE_OPEN))
                {
                    JSR82CallbackParms parms;
                    bt_jsr82_ReleaseL2capFromContext(ptr, L2ChannelId);
                    bt_jsr82_ACLRemoveLink(BTJ82SEAP(session)[session_index].link);
                    BTJ82SEAP(session)[session_index].link = 0;
                    CMGR_RemoveDataLink(&(ptr->cmgrHandler));
                    CMGR_RemoveCmgrWithDevice(&(ptr->cmgrHandler));
                    
                    bt_jsr82_event(ptr1, JSR82_EVENT_DISCONNECT_IND, 0);
                }
                else if (active_link == NULL)
                {
                    /*
                     * In this case: JSR82 L2CAP recvs L2EVENT_CON_IND and accepts it, but recvs DISC_CON_IND during configuration options, then recvs DISCONNECTED 
                     * Because when receiving DISC_IND, l2cap_con_state is reset to IDLE state and no CON_IND is reported to JBT
                     * It is not necessary to report DISC_IND to JBT --- note by mtk01411 2008-0213
                     * if L2ChannelID is valid and channel is NULL: This means that channel is in CLOSED state!
                     */
                    /* Check that the CID is in range */
                    /* Report(("JSR82_L2CAP_DISCONNECTED_Srv: Chnl is NULL, check if l2cap_cid is valid, chnl is in CLOSED state")); */
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2_DISCONNECTED_IND_SRV_CHNL_CLOSED_NEVER_OPENED);
                    if ((L2ChannelId >= BASE_DYNAMIC_CID) && (L2ChannelId <= LAST_DYNAMIC_CID) &&
                        (L2ChannelId != L2CID_INVALID))
                    {
                        /* Report(("JSR82_DISCONNECTED of Srv: Not necessary to notify JBT,l2cap_con_state=%d",ptr->l2capCtx.l2cap_con_state)); */
                        bt_trace(
                            BT_TRACE_JSR82_GROUP,
                            BT_JSR82_L2_DISCONNECTED_IND_NOT_NOTIFY_JBT_L2CON_STATE,
                            ptr->l2capCtx.l2cap_con_state);
                        bt_jsr82_ReleaseL2capFromContext(ptr, L2ChannelId);
                    }
                }

	  	  // Clear ring buffer
	  	  RING_BufFlush(&(BTJ82SEAP(session)[session_index].ReadRingBuf));
	  	  RING_BufFlush(&(BTJ82SEAP(session)[session_index].WriteRingBuf));
	  	  // Reset Ring buffer related context
		  BTJ82SEAP(session)[session_index].is_tx_empty = KAL_TRUE;
		  BTJ82SEAP(session)[session_index].notifyReadyToWrite = KAL_FALSE;
		  BTJ82SEAP(session)[session_index].readyToRead = KAL_TRUE;
		  BTJ82SEAP(session)[session_index].wait_data_available_ind_rsp_length = 0;
            }

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_DISC_COMPLETED);
            break;
        case L2EVENT_CONNECTED:
	    if (session_index >= JSR82_MAX_SESSION_NO)
            {
            	return ;
            }
            if (info->status == BT_STATUS_SUCCESS)
            {
                channel = L2Cap_GetValidChannel(L2ChannelId);
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CONNECTED_CHNL, channel);
                BTJ82SEAP(session)[session_index].link = channel->link;
                bt_jsr82_ACLAddLink(BTJ82SEAP(session)[session_index].link);
                // MTU (of remote device) should be updated for future data transmission
                BTJ82SEAP(session)[session_index].mtu = channel->outMtu;

                Assert(channel != 0);
                if (channel->authorization.incoming == FALSE)
                {
                    JSR82CallbackParms parms;
                    
                    /* L2CAP Client Side */
                    index = bt_jsr82_SearchL2capContext(L2ChannelId);
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CLIENT_CONNECTED_INX, index);
                    ptr->l2capCtx.l2cap_con_state = BT_JSR82_CHANNEL_STATE_OPEN;
					
					bt_jsr82_event(ptr1, JSR82_EVENT_CONNECT_CNF, JSR82_SESSION_RESULT_FAIL);
                }
                else
                {
                    /* L2CAP Server Side */
                    JSR82CallbackParms parms;
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CONNECTED_CON_ID, ptr1->con_id);
                    ptr->l2capCtx.l2cap_con_state = BT_JSR82_CHANNEL_STATE_OPEN;
                    CMGR_CreateDataLink(&(ptr->cmgrHandler), &(channel->link->bdAddr));

					bt_jsr82_event(ptr1, JSR82_EVENT_CONNECT_IND, JSR82_SESSION_RESULT_SUCCESS);
                }

                /*
                 * Because VM termination all link request may be issued when connecting (client con req or server accept conn)
                 * Now, channel is open, it is time to disconnect this link
                 */
                BTJSR82_CheckVMDiscAllLinksReq(BTJ82SEAP(session)[session_index].link, JSR82_SESSION_PS_L2CAP);

            }
            else
            {
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_OPEN_CHNL_FAILED);
            }

            break;
        case L2EVENT_PACKET_HANDLED:
            /* Add by mtk01411: Remove data from TX RingBuf: 2007-1104 */
            jsr82_session_ReturnBuf(
                JSR82_SESSION_PS_L2CAP,
                ptr1->con_id,
                L2ChannelId,
                (char*)info->ptrs.packet->data,
                info->ptrs.packet->dataLen);
            InitializeListEntry(&info->ptrs.packet->node);
            InsertTailList(&ptr1->ApTxQueue, &info->ptrs.packet->node);

#ifdef __JSR82_MAUI__    
            if (BT_JSR82QueryPowerStatus() == 1)
#endif
            {
                /* Continuous to check if there is any available data can be sent */
                bt_jsr82_sendToL2Cap(ptr1->con_id, L2ChannelId);
            }

        #if 0
            bt_jsr82_SessionApTxCfn(ptr1->identify, L2ChannelId, JSR82_SESSION_TX_ERROR);          
        #endif /* 0 */ 
            break;
        case L2EVENT_DATA_IND:
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_RX_IND_LEN, info->dataLen);
#ifdef __JSR82_MAUI__    
            if (BT_JSR82QueryPowerStatus() == 1)
#endif
            {
                kal_int8 rc = 0;

                /* Add by mtk01411: Put Data to RX RingBuf: 2007-1104 */
                rc = jsr82_session_DevRX(
                        JSR82_SESSION_PS_L2CAP,
                        ptr1->con_id,
                        L2ChannelId,
                        (char*)info->ptrs.data,
                        &(info->dataLen));
                /* NOT in POWERON state, ignore it: 2007-1115 */
                if ((signed char) rc < 0)
                {
                    return;
                }
            }
        #if 0
            if( info->dataLen <= JSR82_SESSION_MAX_RX_DATA)
            {
                OS_MemCopy(ptr->data, info->ptrs.data, info->dataLen);
                bt_jsr82_SessionApRxInd(ptr1->con_id, L2ChannelId, info->dataLen, ptr->data);
            }
            else
                bt_trace(BT_TRACE_JSR82_GROUP,RX_DATA_EXCEED_ACCEPTABLE_VALUEx04X , info->dataLen);
        #endif /* 0 */ 
        }
            break;
        case L2EVENT_PING_COMPLETE:
            break;
        case L2EVENT_INFO_COMPLETE:
            break;
    }
}






/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_CmgrClientCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_CmgrClientCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status)
{
    BtStatus ret_Status = BT_STATUS_FAILED;
    BT_JSR82_SESSION_Buffer_struct_t *ptr;
    BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr1;

    U16 l2capLocalCid;
    U16 mtu;
    U8 psm_index = 0;

    if (Handler &&
		Handler->remDev &&
		Handler->bdc &&
		!StrnCmp(Handler->remDev->bdAddr.addr, 6, Handler->bdc->addr.addr, 6))
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGRCLICALLBACK_NOT_ITS_BD_ADDR);
        return;
    }

    ptr = ContainingRecord(Handler, BT_JSR82_SESSION_Buffer_struct_t, cmgrHandler);
    ptr1 = (BT_JSR82_SESSION_AP_Record_Context_struct_t*) ptr->mainRecord;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CLI_CMGR_CALLBACK_CON_ID_EVENT_STATUS, ptr1->con_id, Event, Status);
	OS_Report("[JSR82] session buffer [%p], session [%p], uuid[%x][%x]", ptr, ptr1, ptr1->uuid_128[0], ptr1->uuid_128[1]);

    if (ptr1->status == BT_JSR82_SESSION_IDLE)
    {
        /* Report(("BT_JSR82_CLIENT_CMGR_CALLBACK:ptr1 status idle, return directly")); */
        return;
    }
    switch (Event)
    {
        case CMEVENT_DATA_LINK_CON_CNF:
            if (Status == BT_STATUS_SUCCESS)
            {
                if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) > 0)
                {
                    BtRemoteDevice *link = NULL;

                    link = Handler->remDev;
                    /* VM Termination procedure: VM request to disc all acl link */
                    bt_trace(
                        BT_TRACE_JSR82_GROUP,
                        BT_JSR82_LINK_COF_CHECK_VM_DISC_PEND_CON_NO,
                        BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no));
                    /* Disconnect this link due to VM termination */
                    if (!IsNodeOnList(&(link->handlers), &(jsr82LinkHandler.node)))
                    {
                        InsertTailList(&(link->handlers), &(jsr82LinkHandler.node));
                    }
                    link->discFlag = TRUE;
                    ME_ForceDisconnectLinkWithReason(&jsr82LinkHandler, link, BEC_POWER_OFF, TRUE);
                    if (IsNodeOnList(&(link->handlers), &(jsr82LinkHandler.node)))
                    {
                        RemoveEntryList(&(jsr82LinkHandler.node));
                    }

                    BTJ82SEAP(vm_disc_link_ptr) = link;
                    return;
                }

                if (ptr1->ps_type == JSR82_SESSION_PS_RFCOMM)
                {
                	ret_Status = bt_jsr82_sdp_query(ptr, ptr1->uuid_128);
                }
                else
                {
                    psm_index = bt_jsr82_get_L2capPSMIndex((U16)ptr1->channel, ptr1->mtu, ptr1->security_value, BT_JSR82_SESSION_CLIENT);
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_LINK_CON_CNF_GET_L2CAP_PSM_INX, psm_index);
                    if (psm_index != JSR82_MAX_PSM_NO)
                    {
                        ret_Status = L2CAP_ConnectReq(
                                        (const L2capPsm*)&BTJ82SEAP(l2cap_psm_info)[psm_index].l2cap,
                                        (U16)ptr1->channel,
                                        Handler->remDev,
                                        0,
                                        &l2capLocalCid);
                    }
                    if (ret_Status == BT_STATUS_PENDING)
                    {
                        bt_jsr82_AddCreateL2capToContext(
                            (BT_JSR82_SESSION_Buffer_struct_t*) ptr1->session_buffer,
                            l2capLocalCid);
                        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_LINK_CON_CNF_TRY_OPEN_L2CAP_CHNL_CID, l2capLocalCid);
                    }

                }
            }

            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_LINK_CON_CNF_RET_STATUS, ret_Status);
            if (ret_Status != BT_STATUS_PENDING)
            {
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_OPEN_CHNL_FAILED_LINK_CON_CNF_STATUS, Status);
                CMGR_RemoveDataLink(&(ptr->cmgrHandler));
                CMGR_RemoveCmgrWithDevice(&(ptr->cmgrHandler));
                CMGR_DeregisterHandler(&(ptr->cmgrHandler));
                ptr1->status = BT_JSR82_SESSION_IDLE;
                if (ptr1->ps_type == JSR82_SESSION_PS_RFCOMM)
                {
                    mtu = ptr->rfc.maxFrameSize;
                }
                else
                {
                    mtu = ptr1->mtu;
                }
                
				bt_jsr82_event(ptr1, JSR82_EVENT_CONNECT_CNF,JSR82_SESSION_RESULT_FAIL);

                /* Check if VM termination procedure is ongoing or not */
                if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) > 0)
                {
                    BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no)--;
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_VM_END_ACL_SETUP_OK_BUT_PS_TYPE_REJECTED, ptr1->ps_type);
                    if ((BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) == 0) &&
                        (BTJ82SEAP(jsr82_disconnect_all_acl).func == 0))
                    {
			   			bt_jsr82_event(ptr1, JSR82_EVENT_DISCONNECT_ALL_ACL_CNF, 0);
                    }
                }
            }
            break;
			
        case CMEVENT_DATA_LINK_DIS:
        {
            if (BTJ82SEAP(vm_request_disc_all_link_but_pending_conreq_no) > 0)
            {
                if (BTJ82SEAP(vm_disc_link_ptr) == Handler->remDev)
                {
                    BTJ82SEAP(vm_disc_link_ptr) = NULL;
                    CMGR_RemoveDataLink(&(ptr->cmgrHandler));
                    CMGR_RemoveCmgrWithDevice(&(ptr->cmgrHandler));
                    CMGR_DeregisterHandler(&(ptr->cmgrHandler));

                    /*
                     * Reset status from ACTIVE to BT_JSR82_SESSION_IDLE : 2008-0114
                     * Because after returning MSG_ID_BT_JSR82_RESTROE_ACL_CNF, VM will send MSG_ID_BT_JSR_DEACTIVE_REQ to BTStack later:
                     * While processing this msg, it will check status (in ACTIVE mode) and do something:
                     * For L2CAP, it will freePSMIndex, but in this case, no PSMIndex is reserved: 
                     * Because this link is created then terminated immediately (Not yet callback to l2cap level, so no PSM is allocated actually),
                     * But if status is not reset to IDLE for client role, it will assert in case that no found PSM index while processing DEACTIVE_REQ !
                     */
                    if (ptr1->client_server == BT_JSR82_SESSION_CLIENT)
                    {
                        ptr1->status = BT_JSR82_SESSION_IDLE;
                    }

                    BTJSR82_CheckSendVMDiscAllLinksCnf(0);
                }
            }
        }
            break;
        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_CmgrServerCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_CmgrServerCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status)
{
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SRV_CMGR_CALLBACK_EVENT_STATUS, Event, Status);
    switch (Event)
    {
        case CMEVENT_DATA_LINK_CON_CNF:
        case CMEVENT_DATA_LINK_DIS:
            break;
        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApEnableService
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApEnableService(bt_jsr82_enable_service_req_struct *ptr)
{
    U8 index = 0;
    U8 security_value;
    U16 mtu;
    BtStatus status = BT_STATUS_SUCCESS;
    RfChannel *rfcomm_channel;
    CmgrHandler *cmgrhdl;
    RfService srv_chn;
    JSR82CallbackParms parms;
	U16 channel = 0;
	

    for (index = 0; index < JSR82_MAX_SESSION_NO; index++)
    {
        if (BTJ82SEAP(session)[index].status == BT_JSR82_SESSION_IDLE)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ENABLE_SERVICE_FIND_NO_CHNL, 1);
            break;
        }
    }
    if (index == JSR82_MAX_SESSION_NO)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ENABLE_SERVICE_FIND_NO_CHNL, 2);
		status = BT_STATUS_FAILED;
        goto exit;
    }

    // Assign ring buffer
    jsr82_session_allocate_RWRingBuf(ptr->ps_type, index);
    BTJ82SEAP(session)[index].is_tx_empty = KAL_TRUE;
    BTJ82SEAP(session)[index].notifyReadyToWrite = KAL_FALSE;
    BTJ82SEAP(session)[index].readyToRead = KAL_TRUE;
    BTJ82SEAP(session)[index].wait_data_available_ind_rsp_length = 0;
    BTJ82SEAP(session)[index].deliverBufPtr = NULL;
    BTJ82SEAP(session)[index].deliverBufSize = 0;
	BTJ82SEAP(session)[index].deliverPutBufPtr = NULL;
    
    security_value = ptr->security_value;
    BTJ82SEAP(session)[index].client_server = BT_JSR82_SESSION_SERVER;
#ifdef __JSR82_SESSION_SHARED_MEMORY__
	bt_prompt_trace(MOD_BT, "[JSR82] ptr->memName=%s", ptr->memName);
    // Get memory pointer by name
    BTJ82SEAP(session)[index].session_buffer = (U8*)jsr82_core_malloc_ashm(ptr->memName, index, JSR82_ASHM_TYPE_CHANNEL_BUFFER);
#elif defined(__JSR82_SESSION_DYNAMIC_MEMORY__)
	BTJ82SEAP(session)[index].session_buffer = btmtk_jsr82_malloc(sizeof(BT_JSR82_SESSION_Buffer_struct_t));
#else
	BTJ82SEAP(session)[index].session_buffer = ptr->channel_context;	
#endif

	if (NULL == BTJ82SEAP(session)[index].session_buffer) 
	{
		OS_Report("[JSR82]session buffer is NULL");
        status = BT_STATUS_FAILED;
        goto exit;
	}
    /* Add 2007-1225 */
    bt_jsr82_init_session_buffer((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer);
    BTJ82SEAP(session)[index].ps_type = ptr->ps_type;
#if 0
    /* Add by mtk01411: 2007-0917 */
    BTJ82SEAP(session)[index].session_inx = index;
#endif /* 0 */ 

    BTJ82SEAP(session)[index].identify = ptr->identify;
    BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_ACTIVE;
    BTJ82SEAP(session)[index].channel_state = BT_JSR82_CHANNEL_STATE_IDLE;

	//TODO: switch authorize from upper layer
//    BTJ82SEAP(session)[index].authorize = BT_JSR82_AUTHORIZE_AUTO_REJECT;
	BTJ82SEAP(session)[index].authorize = BT_JSR82_AUTHORIZE_REQUEST_ACCEPT;
    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->mainRecord =
        (U8*) & (BTJ82SEAP(session)[index]);
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ENABLE_SERVICE_PS_TYPEx02X, BTJ82SEAP(session)[index].ps_type);
    if (BTJ82SEAP(session)[index].ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        if (ptr->mtu > RF_MAX_FRAME_SIZE)
        {
            mtu = RF_MAX_FRAME_SIZE;
        }
        else
        {
            mtu = ptr->mtu;
        }

        if (ptr->existing_psm_chnl_num == 0)
            /* This is new service and its registered channel num depends on current available channel number: 2007-1106 */
        {
            BTJ82SEAP(session)[index].channel = bt_jsr82_SessionGetFreeServChnIndex();
        }
        else
        {
            /* In this case: register an existing channel number again to support same channel connected by different clients on different MUXs */
            /* Only RFCOMM can support this feature */
            BTJ82SEAP(session)[index].channel = ptr->existing_psm_chnl_num;
        }
        if (BTJ82SEAP(session)[index].channel == JSR82_NO_AVAILABLE_SESSION)
        {
			OS_Report("[JSR82]no channel is available");
            BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
			status = BT_STATUS_FAILED;
        	goto exit;

        }

        BTJ82SEAP(session)[index].mtu = mtu;
        /*
         * Initalization devCredit and highWaterMarkValue here: 
         * These two fileds will be updated when receiving RFEVENT_OPEN
         * Add by mtk01411: 2007-1022 
         */
        ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->devCredit = 1;
        ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->highWaterMarkValue = 0;

        rfcomm_channel = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->rfc;
        rfcomm_channel->callback = BTJSR82_RfCallbackFunc;
        rfcomm_channel->maxFrameSize = mtu;
        rfcomm_channel->priority = 63;
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        RF_ChannelInit(rfcomm_channel, BT_SEC_MED);
//    #else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        if (security_value == 0x00)
        {
            RF_ChannelInit(rfcomm_channel, BT_SEC_LOW);
        }
        else
        {
            RF_ChannelInit(rfcomm_channel, BT_SEC_MED);
        }
    #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        srv_chn.serviceId = (U8) BTJ82SEAP(session)[index].channel;
        status = RF_RegisterServerChannel(rfcomm_channel, &srv_chn, 1);
        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_HANDLESESSIONAPENABLESERVICEREGISTRATION_ON_SERV_CHNL__xD_AND_STATUSx02X,
            srv_chn.serviceId,
            status);
        if (status != BT_STATUS_SUCCESS)
        {
            bt_jsr82_SessionReleaseServChnIndex(BTJ82SEAP(session)[index].channel);
        }

    }
    else
    {
        if (ptr->mtu > L2CAP_MTU)
        {
            mtu = L2CAP_MTU;
        }
        else
        {
            mtu = ptr->mtu;
        }
        if (0 == ptr->existing_psm_chnl_num)
        {
        	BTJ82SEAP(session)[index].channel = bt_jsr82_SessionGetFreePSMIndex();
        }
        else
        {
        	BTJ82SEAP(session)[index].channel = ptr->existing_psm_chnl_num;
        }
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ENABLE_SERVICE_L2CAP_PSMx08X, BTJ82SEAP(session)[index].channel);

        if (BTJ82SEAP(session)[index].channel == JSR82_NO_AVAILABLE_SESSION)
        {
            BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
            status = BT_STATUS_FAILED;
        	goto exit;
        }
        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_ENABLE_SERVICE_AVAILABLE_CHANNELx08X,
            BTJ82SEAP(session)[index].channel);
        ((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer)->l2capCtx.l2capLocalCid = 0;
        /* Add reset l2capCtx.l2cap_con_state to BT_JSR82_CHANNEL_STATE_IDLE 2007-1222 */
        ((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer)->l2capCtx.l2cap_con_state =
            BT_JSR82_CHANNEL_STATE_IDLE;

        BTJ82SEAP(session)[index].mtu = mtu;
        BTJ82SEAP(session)[index].security_value = security_value;
        if (bt_jsr82_get_L2capPSMIndex((U16) BTJ82SEAP(session)[index].channel, mtu, security_value, BT_JSR82_SESSION_SERVER) ==
            JSR82_MAX_PSM_NO)
        {
            bt_jsr82_SessionReleasePSMIndex(BTJ82SEAP(session)[index].channel);
            status = BT_STATUS_FAILED;
        }
        else
        {
            BTJ82SEAP(session)[index].security_value = security_value;
            status = BT_STATUS_SUCCESS;
        }
    }
    if (status == BT_STATUS_SUCCESS)
    {
        cmgrhdl = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->cmgrHandler;
        status = CMGR_RegisterHandler(cmgrhdl, bt_jsr82_CmgrServerCallback);
        bt_trace(
            BT_TRACE_JSR82_GROUP,
            BT_JSR82_ENABLE_SERVICE_HANDLESESSIONAPENABLESERVICEREGISTRATION_CMGR_HANDLER_STATUSx02X,
            status);        
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ENABLE_SERVICE_CON_IDx02X, BTJ82SEAP(session)[index].con_id);
		channel = (U16) BTJ82SEAP(session)[index].channel;
		goto exit;
    }
    else
    {
        BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
        goto exit;
    }

exit:
	if (status == BT_STATUS_SUCCESS)
	{
		parms.p.enable_service_cnf.result = JSR82_SESSION_REGISTRARION_SUCCESS;
	}
	else
	{
		parms.p.enable_service_cnf.result = JSR82_SESSION_REGISTRARION_FAILED;
	}
	parms.p.enable_service_cnf.ps_type = ptr->ps_type;
    parms.p.enable_service_cnf.channel = channel;
    parms.p.enable_service_cnf.index = index;
    parms.p.enable_service_cnf.identify = ptr->identify;
	parms.event = JSR82_EVENT_ENABLE_SERVICE_CNF;
	parms.dstMod = MOD_MMI;
	JSR82CB(&parms);
	
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApTurnOnService
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApTurnOnService(bt_jsr82_turnon_service_req_struct *ptr)
{
    U8 index = 0;
    U32 transaction_id;
    JSR82CallbackParms parms;
	U8 result = JSR82_SESSION_TURNON_SUCCESS;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLESESSIONAPTURNONSERVICE);
    index = ptr->index;
    transaction_id = ptr->identify;

    // TODO: Check "index" validity
    if ((BTJ82SEAP(session)[index].status != BT_JSR82_SESSION_ACTIVE) ||
        (BTJ82SEAP(session)[index].ps_type != ptr->ps_type))
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TURN_ON_SERVICE_EMPTY_CHANNEL_IDLE_WRONG_STATE);
        result = JSR82_SESSION_TURNON_FAILED;
    }
    else
    {
        BTJ82SEAP(session)[index].authorize = BT_JSR82_AUTHORIZE_AUTO_ACCEPT;
        BTJ82SEAP(session)[index].identify = ptr->identify;        
    }
	parms.p.turn_on_service_cnf.result = result;
	parms.p.turn_on_service_cnf.ps_type = ptr->ps_type;
	parms.p.turn_on_service_cnf.con_id = index;
	parms.p.turn_on_service_cnf.identify = transaction_id;
	parms.event = JSR82_EVENT_TURN_ON_SERVICE_CNF;
	parms.dstMod = MOD_MMI;
	JSR82CB(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApTurnOffService
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApTurnOffService(bt_jsr82_turnoff_service_req_struct *ptr)
{
    U8 index = 0;
    U32 transaction_id;
    JSR82CallbackParms parms;
	U8 result = JSR82_SESSION_TURNOFF_SUCCESS;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLESESSIONAPTURNOFFSERVICE);
    index = ptr->index;
    transaction_id = ptr->identify;
    if ((BTJ82SEAP(session)[index].status != BT_JSR82_SESSION_ACTIVE) ||
        (BTJ82SEAP(session)[index].ps_type != ptr->ps_type))
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TURN_OFF_SERVICE_EMPTY_CHANNEL_IDLE_WRONG_STATE);
        result = JSR82_SESSION_TURNOFF_FAILED;
    }
    else
    {
        /* BTJ82SEAP(session)[index].authorize = BT_JSR82_AUTHORIZE_AUTO_ACCEPT; */
        /* Modify authorize to BT_JSR82_AUTHORIZE_AUTO_REJECT by mtk01411: 2007-1127 */
        BTJ82SEAP(session)[index].authorize = BT_JSR82_AUTHORIZE_AUTO_REJECT;
        BTJ82SEAP(session)[index].identify = ptr->identify;        
    }
	parms.p.turn_off_service_cnf.result = result;
	parms.p.turn_off_service_cnf.ps_type = ptr->ps_type;
	parms.p.turn_off_service_cnf.con_id = index;
	parms.p.turn_off_service_cnf.identify = transaction_id;
	parms.event = JSR82_EVENT_TURN_OFF_SERVICE_CNF;
	parms.dstMod = MOD_MMI;
	JSR82CB(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleConReqTimeoutHdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleConReqTimeoutHdlr(EvmTimer *timer)
{
    bt_jsr82_connect_req_struct *ptr = &BTJ82SEAP(jsr82_con_req);
	if (ptr->ps_type == JSR82_SESSION_PS_RFCOMM)
	{
		bt_jsr82_HandleSessionApRfcommConnectReq(ptr);
	}
	else
	{
		bt_jsr82_HandleSessionApL2capConnectReq(ptr);
	}
    BTJ82SEAP(jsr82_con_req_timer).func = 0;
    btmtk_os_memset((U8*) & BTJ82SEAP(jsr82_con_req), 0, sizeof(bt_jsr82_connect_req_struct));

}

/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSdpReqTimeoutHdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSdpReqTimeoutHdlr(EvmTimer *timer)
{
#ifdef __JSR82_MAUI__        
    bt_bm_search_raw_req_struct *ptr = &BTJ82SEAP(jsr82_sdp_req);
    U8 bt_power_status = BT_JSR82QueryPowerStatus();

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SDP_REQ_TIMEOUT_HDLR, bt_power_status);
    if (bt_power_status == 1)
    {
        SDAP_ServiceSearchAttributeRaw(ptr);
    }
    else
    {
        BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, ptr->bd_addr);
    }
    BTJ82SEAP(jsr82_sdp_req_timer).func = 0;
    btmtk_os_memset((U8*) & BTJ82SEAP(jsr82_sdp_req), 0, sizeof(bt_bm_search_raw_req_struct));

#endif

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSdpReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSdpReq(bt_bm_search_raw_req_struct *ptr)
{
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLESESSIONAPSDPREQ);
    if (BTJ82SEAP(jsr82_sdp_req_timer).func == 0)
    {
        BTJ82SEAP(jsr82_sdp_req_timer).func = bt_jsr82_HandleSdpReqTimeoutHdlr;
        btmtk_os_memcpy((U8*) & BTJ82SEAP(jsr82_sdp_req), (U8*) ptr, sizeof(bt_bm_search_raw_req_struct));
        /*
         * This timer is used to delay SDAP query for 2 reasons.
         * 1. Some JSR82 midlets issue SDAP queries in a very short time.
         *    In this case, the later queries will be rejected.
         *    Therefore, we start a timer to space the adjacent SDAP queries.
         * 2. BT chip can't handle the connect and disconnect at the same time.
         *    It's possilbe that midlet try to do SDAP query 
         *    while the L2CAP channel for previous SDAP query is still disconnecting, 
         *    there is a 500ms timer for L2CAP disconnetion.
         *    For this reason, we start a timer to separate connection and disconnectino.
         */
        EVM_StartTimer(&BTJ82SEAP(jsr82_sdp_req_timer), 500);
    }
    else
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SDP_REQ_PREVIOUS_ONGOING);
        /* Previous one sdp request is ongoing */
#ifdef __JSR82_MAUI__    
        BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, ptr->bd_addr);
#endif
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApConnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApConnectReq(bt_jsr82_connect_req_struct *ptr)
{
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLESESSIONAPCONNECTREQ);
    btmtk_os_memcpy((U8*) & BTJ82SEAP(jsr82_con_req), (U8*) ptr, sizeof(bt_jsr82_connect_req_struct));
    bt_jsr82_HandleConReqTimeoutHdlr(NULL);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApRfcommConnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApRfcommConnectReq(bt_jsr82_connect_req_struct *ptr)
{
    U8 index = 0;
    U8 security_value;
    U16 mtu = 0;
    BtStatus status = BT_STATUS_SUCCESS;
    RfChannel *channel;
    CmgrHandler *cmgrhdl;
    BD_ADDR addr;
    U8 session_devCredit = 0;
	JSR82CallbackParms parms;

    for (index = 0; index < JSR82_MAX_SESSION_NO; index++)
    {
        if (BTJ82SEAP(session)[index].status == BT_JSR82_SESSION_IDLE)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_CON_REQ_EMPTY_CHANNEL_FIND);
            break;
        }
    }
    if (index == JSR82_MAX_SESSION_NO)
    {
        
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_CON_REQ_NO_EMPTY_CHANNEL);
		status = BT_STATUS_FAILED;
		goto exit;
	 
    }

    // Assign ring buffer
    jsr82_session_allocate_RWRingBuf(ptr->ps_type, index);
    BTJ82SEAP(session)[index].is_tx_empty = KAL_TRUE;
    BTJ82SEAP(session)[index].notifyReadyToWrite = KAL_FALSE;
    BTJ82SEAP(session)[index].readyToRead = KAL_TRUE;
    BTJ82SEAP(session)[index].wait_data_available_ind_rsp_length = 0;
    BTJ82SEAP(session)[index].deliverBufPtr = NULL;
    BTJ82SEAP(session)[index].deliverBufSize = 0;
    BTJ82SEAP(session)[index].deliverPutBufPtr = NULL;

    if (ptr->mtu > RF_MAX_FRAME_SIZE)
    {
        mtu = RF_MAX_FRAME_SIZE;
    }
    else
    {
        mtu = ptr->mtu;
    }
    security_value = ptr->security_value;

    BTJ82SEAP(session)[index].client_server = BT_JSR82_SESSION_CLIENT;
#ifdef __JSR82_SESSION_SHARED_MEMORY__
    // Get memory pointer by name
    BTJ82SEAP(session)[index].session_buffer = (U8*)jsr82_core_malloc_ashm(ptr->memName, index, JSR82_ASHM_TYPE_CHANNEL_BUFFER);
#elif defined(__JSR82_SESSION_DYNAMIC_MEMORY__)
	BTJ82SEAP(session)[index].session_buffer = btmtk_jsr82_malloc(sizeof(BT_JSR82_SESSION_Buffer_struct_t));
#else
	BTJ82SEAP(session)[index].session_buffer = ptr->channel_context;
	
#endif
    /* Add 2007-1225 */
	if (NULL == BTJ82SEAP(session)[index].session_buffer)
	{
		status = BT_STATUS_FAILED;
		goto exit;		
	}
	
    bt_jsr82_init_session_buffer((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer);
    BTJ82SEAP(session)[index].ps_type = ptr->ps_type;
    BTJ82SEAP(session)[index].channel = ptr->channel;
    /* Add by mtk01411: 2007-0917 */
    /* BTJ82SEAP(session)[index].session_inx = index; */

    BTJ82SEAP(session)[index].identify = ptr->identify;
    BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_ACTIVE;
    BTJ82SEAP(session)[index].security_value = security_value;
    BTJ82SEAP(session)[index].mtu = mtu;

    OS_MemCopy((U8*)(BTJ82SEAP(session)[index].uuid_128), ptr->uuid_128, 16);
	OS_MemCopy((U8*)(BTJ82SEAP(session)[index].bd_addr), ptr->bd_addr, 6);
    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->mainRecord =
        (U8*) & (BTJ82SEAP(session)[index]);
    /*
     * For SPP client, it must set devCredit=1 for 1st credit given to Server side
     * The left available credit value is given when the connection is opend
     * Once the connection is opend, Client will give 5(using highWaterMark to compute) -1 credits to Server
     * Note 2007-1110
     */
    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->devCredit = 1;
    session_devCredit = ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->devCredit;
    channel = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->rfc;
  
    cmgrhdl = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->cmgrHandler;
    status = CMGR_RegisterHandler(cmgrhdl, bt_jsr82_CmgrClientCallback);
    if (status == BT_STATUS_SUCCESS)
    {
        OS_MemCopy((U8*) addr.addr, ptr->bd_addr, 6);
        status = CMGR_CreateDataLink(cmgrhdl, &addr);
		OS_Report("[JSR82]CMGR_CreateDataLink %d", status);
        if (status == BT_STATUS_SUCCESS)
        {
            BtStatus ret_status = BT_STATUS_FAILED;

            BTJ82SEAP(session)[index].channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
			ret_status = bt_jsr82_sdp_query((BT_JSR82_SESSION_Buffer_struct_t*)BTJ82SEAP(session)[index].session_buffer, ptr->uuid_128);
			if (ret_status != BT_STATUS_PENDING)
			{
                SEC_Unregister(&channel->secRecord);
                CMGR_RemoveDataLink(cmgrhdl);
                CMGR_RemoveCmgrWithDevice(cmgrhdl);
                CMGR_DeregisterHandler(cmgrhdl);
                BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RFCOMM_CON_REQ_OPEN_RFCOMM_CHNL_FAILED);		  
		  		status = BT_STATUS_FAILED;
				goto exit;		
            }
        }
        else
        {
            /* Add check of RFCOMM CMGR_CreatDataLink() case: 2008-0302 */
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 1, status);
            if (status != BT_STATUS_PENDING)
            {
                /* Note that: ME_FindRemoteDevice() will return a valid BtRemoteDevice if its state is not in BDS_DISCONNECTED */
                BtRemoteDevice* me_remDev = ME_FindRemoteDevice(&addr); 
                BtHandler *rm_handler = &(cmgrhdl->btHandler);
                /* For MAUI_01331610: Returned status of CMGR_CreateDataLink() is BT_STATUS_FAILED due to PAIRING state */
                /* In this case, this dev is in BDS_DISCONNECTED and not allowed to create link */
                
                if (me_remDev != NULL)
                {
                    if((me_remDev->disconnectACLCheckCreatACLTimer.func != 0)
                        && (me_remDev->disconnectACLCheckCreatACLAlwaysForce != 1)
                        && (!IsNodeOnList(&(me_remDev->handlers), &(rm_handler->node))))
                    {
                        /* 
                          * handler is already removed from remDev->handlers list due to ME_CreateLink() failure
                          * The failure reason of ME_CreateLink: Because this link was already requested to disconnect but
                          * its disconnection action is blocked by a ongoing connection setup
                          * In this case, its btHandler will be removed from remDev->handler list in ME_CreateLink()
                          * -> If it invokes CMGR_RemoveDataLink() in this case:
                          * Because me_remDev->state is BDS_CONNECTED, it will invoke ME_DisconnectLink()
                          * Then: it will assert in ME_ForceDisconnectLinkWithReason() due to 
                          * handler is already removed from remDev->handlers list
                          */
                        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 3, status);
                    }
                    else
                    {
                        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 5, status);
                        CMGR_RemoveCmgrWithDevice(cmgrhdl);
                    }
                }
                else
                {
                    /* me_remDev is NULL: it does not create link and state is in BDS_DISCONNECTED */
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 7, status);
                    CMGR_RemoveCmgrWithDevice(cmgrhdl);
                }
                CMGR_DeregisterHandler(cmgrhdl);
                BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
				status = BT_STATUS_FAILED;
				goto exit;			  
            }
        }
    }
    else
    {
        JSR82CallbackParms parms;
        
        CMGR_DeregisterHandler(cmgrhdl);
        BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
		status = BT_STATUS_FAILED;
		goto exit;	
    }

exit:
	if (index < JSR82_MAX_SESSION_NO  &&
		BT_JSR82_SESSION_IDLE == BTJ82SEAP(session)[index].status)
	{
		jsr82_session_free_RWRingBuf(index);
	}


	//TODO: if no context is found, how to callback?????
	if (BT_STATUS_PENDING != status &&
		BT_STATUS_SUCCESS != status)
	{	
		bt_jsr82_event(&BTJ82SEAP(session)[index], JSR82_EVENT_CONNECT_CNF,JSR82_SESSION_RESULT_FAIL);
	}
	
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApL2capConnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApL2capConnectReq(bt_jsr82_connect_req_struct *ptr)
{
    U8 index = 0;
    U8 security_value = 0;
    BtStatus ret_Status = BT_STATUS_FAILED;
    BtStatus status = BT_STATUS_SUCCESS;
    CmgrHandler *cmgrhdl = NULL;
    BD_ADDR addr;
    BT_JSR82_SESSION_Buffer_struct_t *buffer = NULL;
    U16 l2cap_cid = 0;
    U8 psm_index = 0;
    U16 mtu = 0;

    for (index = 0; index < JSR82_MAX_SESSION_NO; index++)
    {
        if (BTJ82SEAP(session)[index].status == BT_JSR82_SESSION_IDLE)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CON_REQ_EMPTY_CHANNEL_FIND);
            break;
        }
    }
    if (index == JSR82_MAX_SESSION_NO)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CON_REQ_NO_EMPTY_CHANNEL);
		status = BT_STATUS_FAILED;
		goto exit;
    }

    // Assign ring buffer
    jsr82_session_allocate_RWRingBuf(ptr->ps_type, index);
    BTJ82SEAP(session)[index].is_tx_empty = KAL_TRUE;
    BTJ82SEAP(session)[index].notifyReadyToWrite = KAL_FALSE;
    BTJ82SEAP(session)[index].readyToRead = KAL_TRUE;
    BTJ82SEAP(session)[index].wait_data_available_ind_rsp_length = 0;
    BTJ82SEAP(session)[index].deliverBufPtr = NULL;
    BTJ82SEAP(session)[index].deliverBufSize = 0;
    BTJ82SEAP(session)[index].deliverPutBufPtr = NULL;

    if (ptr->mtu > L2CAP_MTU)
    {
        mtu = L2CAP_MTU;
    }
    else
    {
        mtu = ptr->mtu;
    }

    security_value = ptr->security_value;

    BTJ82SEAP(session)[index].client_server = BT_JSR82_SESSION_CLIENT;
#ifdef __JSR82_SESSION_SHARED_MEMORY__
		// Get memory pointer by name
		BTJ82SEAP(session)[index].session_buffer = (U8*)jsr82_core_malloc_ashm(ptr->memName, index, JSR82_ASHM_TYPE_CHANNEL_BUFFER);
#elif defined(__JSR82_SESSION_DYNAMIC_MEMORY__)
		BTJ82SEAP(session)[index].session_buffer = btmtk_jsr82_malloc(sizeof(BT_JSR82_SESSION_Buffer_struct_t));
#else
		BTJ82SEAP(session)[index].session_buffer = ptr->channel_context;
		
#endif


    /* Add 2007-1225 */
	if (NULL == BTJ82SEAP(session)[index].session_buffer)
	{				
		status = BT_STATUS_FAILED;
		goto exit;
	}

    bt_jsr82_init_session_buffer((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer);
    buffer = (BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer;
    buffer->mainRecord = (U8*) & BTJ82SEAP(session)[index];
    BTJ82SEAP(session)[index].ps_type = ptr->ps_type;
    BTJ82SEAP(session)[index].channel = ptr->channel;
    BTJ82SEAP(session)[index].identify = ptr->identify;
    BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_ACTIVE;
    BTJ82SEAP(session)[index].security_value = security_value;
    BTJ82SEAP(session)[index].mtu = mtu;
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CON_REQ_INIT_CON_IDx02X, BTJ82SEAP(session)[index].con_id);
    OS_MemCopy((U8*) & (BTJ82SEAP(session)[index].bd_addr), ptr->bd_addr, 6);
    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->mainRecord =
        (U8*) & (BTJ82SEAP(session)[index]);
    /* Add codes to reset l2capCtx.l2cap_con_state and l2capLocalCid :2007-1225 */
    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->l2capCtx.l2cap_con_state =
        BT_JSR82_CHANNEL_STATE_IDLE;
    ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->l2capCtx.l2capLocalCid = 0;

    cmgrhdl = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->cmgrHandler;
    status = CMGR_RegisterHandler(cmgrhdl, bt_jsr82_CmgrClientCallback);
    if (status == BT_STATUS_SUCCESS)
    {
        OS_MemCopy((U8*) addr.addr, ptr->bd_addr, 6);
        status = CMGR_CreateDataLink(cmgrhdl, &addr);
        if (status == BT_STATUS_SUCCESS)
        {
            BTJ82SEAP(session)[index].channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
            psm_index = bt_jsr82_get_L2capPSMIndex((U16)ptr->channel, mtu, ptr->security_value, BT_JSR82_SESSION_CLIENT);
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CON_REQ_L2CAP_PSM_INDEXx02X, psm_index);
            if (psm_index != JSR82_MAX_PSM_NO)
            {
                ret_Status = L2CAP_ConnectReq(
                                (const L2capPsm*)&BTJ82SEAP(l2cap_psm_info)[psm_index].l2cap,
                                ptr->channel,
                                cmgrhdl->remDev,
                                0,
                                &l2cap_cid);
            }
            if (ret_Status != BT_STATUS_PENDING)
            {
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_L2CAP_CON_REQOPEN_CHANNEL_FAILED);
                bt_jsr82_free_L2capPSMIndex(ptr->channel, ptr->security_value, BT_JSR82_SESSION_CLIENT);
                CMGR_RemoveDataLink(cmgrhdl);
                CMGR_RemoveCmgrWithDevice(cmgrhdl);
                CMGR_DeregisterHandler(cmgrhdl);
                BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;

		  		status = BT_STATUS_FAILED;
				goto exit;
            }
            else
            {
                /*
                 * In this function: it will set:
                 * ptr->l2capCtx.l2cap_con_state = BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL; 
                 */
                bt_jsr82_AddCreateL2capToContext(buffer, l2cap_cid);
            }

        }
        else
        {
            /* Add check of L2CAP CMGR_CreatDataLink() case: 2008-0302 */
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 2, status);
            if (status != BT_STATUS_PENDING)
            {
                /* Note that: ME_FindRemoteDevice() will return a valid BtRemoteDevice if its state is not in BDS_DISCONNECTED */
                BtRemoteDevice* me_remDev = ME_FindRemoteDevice(&addr); 
                BtHandler *rm_handler = &(cmgrhdl->btHandler);
                /* For MAUI_01331610: Returned status of CMGR_CreateDataLink() is BT_STATUS_FAILED due to PAIRING state */
                /* In this case, this dev is in BDS_DISCONNECTED and not allowed to create link */
                
                if (me_remDev != NULL)
                {
                    if((me_remDev->disconnectACLCheckCreatACLTimer.func != 0)
                        && (me_remDev->disconnectACLCheckCreatACLAlwaysForce != 1)
                        && (!IsNodeOnList(&(me_remDev->handlers), &(rm_handler->node))))
                    {
                        /* 
                          * handler is already removed from remDev->handlers list due to ME_CreateLink() failure
                          * The failure reason of ME_CreateLink: Because this link was already requested to disconnect but
                          * its disconnection action is blocked by a ongoing connection setup
                          * In this case, its btHandler will be removed from remDev->handler list in ME_CreateLink()
                          * -> If it invokes CMGR_RemoveDataLink() in this case:
                          * Because me_remDev->state is BDS_CONNECTED, it will invoke ME_DisconnectLink()
                          * Then: it will assert in ME_ForceDisconnectLinkWithReason() due to 
                          * handler is already removed from remDev->handlers list
                          */
                        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 4, status);
                    }
                    else
                    {
                        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 6, status);
                        CMGR_RemoveCmgrWithDevice(cmgrhdl);
                    }
                }
                else
                {
                    /* me_remDev is NULL: it does not create link and state is in BDS_DISCONNECTED */
                    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CMGR_CREATEDATALINK_STATUS, 8, status);
                    CMGR_RemoveCmgrWithDevice(cmgrhdl);
                }
                CMGR_DeregisterHandler(cmgrhdl);
                BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;

			  	status = BT_STATUS_FAILED;
				goto exit;
            }
        }
    }
    else
    {
        CMGR_DeregisterHandler(cmgrhdl);
        BTJ82SEAP(session)[index].status = BT_JSR82_SESSION_IDLE;
		status = BT_STATUS_FAILED;
		goto exit;
    }

exit:
	if (index < JSR82_MAX_SESSION_NO  &&
		BT_JSR82_SESSION_IDLE == BTJ82SEAP(session)[index].status)
	{
		jsr82_session_free_RWRingBuf(index);
	}
	if (BT_STATUS_SUCCESS != status)
	{
		JSR82CallbackParms parms;
		parms.p.connect_cnf.bd_addr = (U8*) ptr->bd_addr;
		parms.p.connect_cnf.ps_type = ptr->ps_type;
		parms.p.connect_cnf.mtu = mtu;
		parms.p.connect_cnf.in_mtu = 0;
		parms.p.connect_cnf.channel = ptr->channel;
		parms.p.connect_cnf.index = 0;
		parms.p.connect_cnf.identify = ptr->identify;
		parms.p.connect_cnf.l2cap_id = 0;
		parms.p.connect_cnf.result = JSR82_SESSION_CONNECT_CLIENT_FAILED;
		parms.event = JSR82_EVENT_CONNECT_CNF;
		parms.dstMod = MOD_MMI;
		JSR82CB(&parms);	
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApConnectRsp
 * DESCRIPTION
 *  This function is used to process the message MSG_ID_BT_JSR82_CONNECT_RSP
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApConnectRsp(bt_jsr82_connect_rsp_struct *ptr)
{
    U8 index = 0;

    for (index = 0; index < JSR82_MAX_SESSION_NO; index++)
    {
        if ((BTJ82SEAP(session)[index].status == BT_JSR82_SESSION_ACTIVE) &&
            (ptr->ps_type == BTJ82SEAP(session)[index].ps_type) && (ptr->channel == BTJ82SEAP(session)[index].channel)
            && (ptr->identify == BTJ82SEAP(session)[index].identify))

        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CON_RSP_FIND_JSR82_CHANNEL_x02X, index);
            break;
        }
    }
    if (index == JSR82_MAX_SESSION_NO)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CON_RSP_NO_EMPTY_CHANNEL);
        return;
    }
    Assert((BTJ82SEAP(session)[index].session_buffer) != 0);
    if (BTJ82SEAP(session)[index].ps_type == JSR82_SESSION_PS_L2CAP)
    {
        if (ptr->result == JSR82_SESSION_CONNECT_ACCEPT)
        {
            L2CAP_ConnectRsp(ptr->l2cap_id, L2CONN_ACCEPTED, 0);
        }

        else
        {
            L2CAP_ConnectRsp(ptr->l2cap_id, L2CONN_REJECT_NO_RESOURCES, 0);
        }
    }
    else
    {
        if (ptr->result == JSR82_SESSION_CONNECT_ACCEPT)
        {
            RF_AcceptChannel(&((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer)->rfc);
        }
        else
        {
            RF_RejectChannel(&((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer)->rfc);
        }
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApRxCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApRxCfn(bt_jsr82_rx_data_rsp_struct *ptr)
{
    U8 index = 0;

    for (index = 0; index < JSR82_MAX_SESSION_NO; index++)
    {
        if ((BTJ82SEAP(session)[index].status == BT_JSR82_SESSION_ACTIVE) &&
            (ptr->index == BTJ82SEAP(session)[index].con_id))

        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RX_CNF_FIND_JSR82_CHANNEL_x02X, index);
            break;
        }
    }
    if (index == JSR82_MAX_SESSION_NO)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RX_CNF_NO_EMPTY_CHANNEL);
        return;
    }
    Assert((BTJ82SEAP(session)[index].session_buffer) != 0);
    if (BTJ82SEAP(session)[index].ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        RF_AdvanceCredit(&((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[index].session_buffer)->rfc, 1);
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApTxReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApTxReq(bt_jsr82_tx_data_req_struct *ptr)
{
	bt_jsr82_send(ptr->index, ptr->l2cap_id);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApDisConnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApDisConnectReq(bt_jsr82_disconnect_req_struct *ptr)
{
    U8 i = 0;

    if (ptr->ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
        {
            if ((BTJ82SEAP(session)[i].status == BT_JSR82_SESSION_ACTIVE) &&
                (ptr->index == BTJ82SEAP(session)[i].con_id))

            {
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DISC_REQ_FIND_JSR82_CHANNEL_x02X, i);
                break;
            }
        }
        if (i == JSR82_MAX_SESSION_NO)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DISC_REQ_NO_EMPTY_CHANNEL);
            return;
        }
        Assert((BTJ82SEAP(session)[i].session_buffer) != 0);
        /* Add by mtk01411: 2007-1113 */
        BTJ82SEAP(session)[i].identify = ptr->identify;
        RF_CloseChannel(&((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[i].session_buffer)->rfc);
    }
    else
    {
        L2CAP_DisconnectReq(ptr->l2cap_id);
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApDisableService
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApDisableService(bt_jsr82_disable_service_req_struct *ptr)
{
    U8 i = 0;
    RfChannel *channel;
    RfService srv_chn;
    BtStatus status = BT_STATUS_FAILED;
    CmgrHandler *cmgrhdl;
    U8 ps_type = 0;
    JSR82CallbackParms parms;

    i = ptr->index;
	if (BTJ82SEAP(session)[i].status == BT_JSR82_SESSION_IDLE)
	{
		goto exit;
	}
    Assert((BTJ82SEAP(session)[i].session_buffer) != 0);
    ps_type = BTJ82SEAP(session)[i].ps_type;
    if (BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        /* Because this channel is still in active state, it must terminate this RFCOMM connection before deregistration service */
        if (BTJ82SEAP(session)[i].channel_state != BT_JSR82_CHANNEL_STATE_IDLE)
        {
	    	goto exit;
        }
        channel = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[i].session_buffer))->rfc;
        srv_chn.serviceId = (U8) BTJ82SEAP(session)[i].channel;
        status = RF_DeregisterServerChannel(channel, &srv_chn);
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DISABLE_SERVICE_DEREGISTER_CHANNEL_x02X, status);
        bt_jsr82_SessionReleaseServChnIndex(srv_chn.serviceId);
    }
    else
    {
        BT_JSR82_SESSION_Buffer_struct_t *temp_ptr =
            ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[i].session_buffer));

        /* Because this L2CAP connection is still in active state, it must terminate this L2CAP connection before deregistration this L2CAP service */
        if (temp_ptr->l2capCtx.l2cap_con_state != BT_JSR82_CHANNEL_STATE_IDLE)
        {
            bt_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_DISABLE_SERVICE_L2CAP_CON_INDEX_xD_IS_NOT_IDLE_xD,
                i,
                temp_ptr->l2capCtx.l2cap_con_state);
			goto exit;
        }
        bt_jsr82_free_L2capPSMIndex(BTJ82SEAP(session)[i].channel, BTJ82SEAP(session)[i].security_value, BT_JSR82_SESSION_SERVER);
        bt_jsr82_SessionReleasePSMIndex(BTJ82SEAP(session)[i].channel);
        status = BT_STATUS_SUCCESS;
    }
    if (status == BT_STATUS_SUCCESS)
    {
        cmgrhdl = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[i].session_buffer))->cmgrHandler;
        status = CMGR_DeregisterHandler(cmgrhdl);
        BTJ82SEAP(session)[i].status = BT_JSR82_SESSION_IDLE;
		
#ifdef __JSR82_SESSION_SHARED_MEMORY__
        jsr82_core_free_ashm(i, JSR82_ASHM_TYPE_CHANNEL_BUFFER);		
#elif defined(__JSR82_SESSION_DYNAMIC_MEMORY__)
		if (NULL != BTJ82SEAP(session)[i].session_buffer)
		{
			btmtk_jsr82_free(BTJ82SEAP(session)[i].session_buffer);
		}
#endif
        BTJ82SEAP(session)[i].session_buffer = 0x00;
        BTJ82SEAP(session)[i].channel_state = BT_JSR82_CHANNEL_STATE_IDLE;
        // Free ring buffer
	 	jsr82_session_free_RWRingBuf(i);        
        // Clear session context
        bt_jsr82_ClearSessionContext(i);
    }

exit:
	parms.p.disable_service_cnf.ps_type = ps_type;
	parms.p.disable_service_cnf.con_id = ptr->index;
	parms.p.disable_service_cnf.identify = ptr->identify;
	parms.p.disable_service_cnf.result = (status == BT_STATUS_SUCCESS) ? BT_JSR82_SESSION_DISABLED_SUCCESS : BT_JSR82_SESSION_DISABLED_FAILED;
	parms.event = JSR82_EVENT_DISABLE_SERVICE_CNF;
	parms.dstMod = MOD_MMI;
	JSR82CB(&parms);	
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_setHighWaterMark
 * DESCRIPTION
 *  This function is only invoked when receiving RFEVENT_OPEN: At that time, it can determine the highWaterMark value
 * PARAMETERS
 *  chan                [IN]        
 *  session_buf_ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_setHighWaterMark(RfChannel *chan, BT_JSR82_SESSION_Buffer_struct_t *session_buf_ptr)
{
    /* Add by mtk01411: 2007-0916 */
    U16 rx_free_size = 0, rfc_size = 0;

#if 0
    U16 subsession_id = 0;
#endif /* 0 */ 
    BT_JSR82_SESSION_AP_Record_Context_struct_t *session_ap_record_ptr =
        (BT_JSR82_SESSION_AP_Record_Context_struct_t*) session_buf_ptr->mainRecord;

    /* For both Client and Server : Calculate and Set HighWaterMark value: 2007-0916 */
    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_SET_HIGH_WATER_MARK_SESSION_AP_RECORD_PTRCOD_IDx02X_TRANSACTION_IDx04X,
        session_ap_record_ptr->con_id,
        session_ap_record_ptr->identify);
    /* Using predefined Buffer size: Only query this value! 2007-1113 */
    rx_free_size = jsr82_session_BufSize(JBT_SESSION_TYPE_SPP);

    if (rx_free_size == 0)
    {
        Assert(0);
    }

    if (RF_CreditFlowEnabled(chan) == TRUE)
    {
        rfc_size = RF_FrameSize(chan);
        session_buf_ptr->highWaterMarkValue = rx_free_size - rfc_size;
    }
    else
    {
        session_buf_ptr->highWaterMarkValue = rx_free_size / 2;
    }
    Assert(session_buf_ptr->highWaterMarkValue >= rfc_size);

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_getCurrentCredit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_inx     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 bt_jsr82_getCurrentCredit(U8 session_inx)
{
    return ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[session_inx].session_buffer))->devCredit;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_advanceCredit
 * DESCRIPTION
 *  This function is similar to advanceCredit()
 * PARAMETERS
 *  session_inx         [IN]        
 *  check_rx_bytes      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_advanceCredit(U8 session_inx, kal_bool check_rx_bytes)
{
    BtStatus status;
    S16 intCredit;
    U8 credit, current_assign_outgoing_credit;
    S16 highWater = 0, rx_bytes = 0, rfc_size = 0;
    RfChannel *channel = NULL;
    U16 subsession_id = 0;

	/*If RFCOMM channel is not active, directly return*/
	if (BT_JSR82_SESSION_IDLE == BTJ82SEAP(session)[session_inx].status) 
	{
		return;
	}

    /* Using identify to find corresponding inx */
    channel = &((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[session_inx].session_buffer))->rfc;
    /* rx_bytes = jbt_spp_RxBytes(session_inx); */
    if (check_rx_bytes == KAL_TRUE)
    {
        rx_bytes = jsr82_session_RxBytes(JSR82_SESSION_PS_RFCOMM, session_inx, subsession_id);
        /* Due to PowerOff period, ignore the following actions:2007-1115 */
        if (rx_bytes < 0)
        {
            return;
        }
    }
    else
    {
        rx_bytes = 0;
    }

    Assert(channel);
    rfc_size = RF_FrameSize(channel);
    /* Check if rfc_size =0 is due to channel->state is in DISCONNECTED : Add 2008-0102 */
    if (rfc_size == 0)
    {
        if (channel->state != DLC_STATE_CONNECTED)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ADVANCECREDIT_CHNL_STATExD_NOT_IN_CONNECTED, channel->state);
            return;
        }
        else
        {
            Assert(0);
        }
    }
    highWater =
        ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[session_inx].session_buffer))->highWaterMarkValue;
    current_assign_outgoing_credit =
        ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[session_inx].session_buffer))->devCredit;

    /* Calculate total amount of credit. */
    intCredit = (S8) ((S16) ((highWater - rx_bytes) / rfc_size));
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HIGHWATERxD_SPPOS_RXBYTESxD_RF_FRAMESIZExD, highWater, rx_bytes, rfc_size);

    /* Check for U8 overflow. */
    if (intCredit > 0xff)
    {
        credit = 0xff;
    }
    else
    {
        credit = (U8) intCredit;
    }
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CREDIT_xD_DEVCREDIT_xD, credit, current_assign_outgoing_credit);

    if (credit > current_assign_outgoing_credit)
    {
        /* Subtract credit already outstanding. */
        credit -= current_assign_outgoing_credit;
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_ADVANCECREDITCALL_RF_ADVANCECREDIT);
        status = RF_AdvanceCredit(channel, credit);

        if (status == BT_STATUS_SUCCESS)
        {
            current_assign_outgoing_credit += credit;
            /* Update current_outgoing_credit to its corresponding entry's credit field: Add by mtk01411: 2007-1022 */
            ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[session_inx].session_buffer))->devCredit =
                current_assign_outgoing_credit;
        }

        /* For JSR92: Currently, there is no interface to configure port settings */
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSessionApGetSppDataReq
 * DESCRIPTION
 *  This function is similar to function SPP_Read()
 *  If this credit kept for this SPP connection  is less than half of SPP_RX_CREDIT, this means that:
 *  The buffer allocated for storing incoming data has more space now due to application already read data from this buffer.
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSessionApGetSppDataReq(bt_jsr82_spp_get_data_req_struct *ptr)
{
    /* Call bt_jsr82_advanceCredit() to check credit information here */
#ifdef __JSR82_MAUI__    
    if (BT_JSR82QueryPowerStatus() == 1)
#endif
    {
        bt_jsr82_advanceCredit(ptr->index, KAL_TRUE);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_HandleSetACLSecurityReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_HandleSetACLSecurityReq(bt_jsr82_set_acl_security_req_struct *ptr)
{
    kal_bool enable_authentication_link = KAL_TRUE;
    kal_bool enable_encryption_link = KAL_TRUE;
    kal_bool need_authentication_link = KAL_FALSE;
    kal_bool need_encryption_link = KAL_FALSE;
    int i = 0, remDev_index = JSR82_MAX_ACL_NO;
    JSR82CallbackParms parms;
	BtStatus status = BT_STATUS_SUCCESS;

    /* ptr->security_mode = 2; */
    /* Check if this link is already authenticated or encrypted or not */
    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].used == 1)
        {
            if (TRUE == btmtk_os_memcmp(BTJ82SEAP(active_link)[i].link->bdAddr.addr, 6, ptr->bd_addr, 6))
            {
                remDev_index = i;
                bt_trace(
                    BT_TRACE_JSR82_GROUP,
                    BT_JSR82_HANDLESETACLSECURITYREQ_CURRENT_LINK_AUTHSTATExDENCRYPTSTATExD,
                    BTJ82SEAP(active_link)[i].link->authState,
                    BTJ82SEAP(active_link)[i].link->encryptState);
                if ((ptr->security_mode == 1) && (BTJ82SEAP(active_link)[i].link->authState == BAS_AUTHENTICATED))
                {                    
					status = BT_STATUS_SUCCESS;
					goto exit;
                }
                else if ((ptr->security_mode == 1) &&
                         (BTJ82SEAP(active_link)[i].link->authState == BAS_NOT_AUTHENTICATED))
                {
                    need_authentication_link = KAL_TRUE;
                    break;
                }
                else if ((ptr->security_mode == 2) &&
                         (BTJ82SEAP(active_link)[i].link->authState == BAS_NOT_AUTHENTICATED))
                {
                    need_authentication_link = KAL_TRUE;
                    /* Do authentication first  */
                    enable_encryption_link = KAL_FALSE;
                    break;
                }
                else if ((ptr->security_mode == 2) && (BTJ82SEAP(active_link)[i].link->authState == BAS_AUTHENTICATED)
                         && (BTJ82SEAP(active_link)[i].link->encryptState == BES_NOT_ENCRYPTED))
                {
                    need_encryption_link = KAL_TRUE;
                    /* Only do encryption link */
                    enable_authentication_link = KAL_FALSE;
                    break;
                }
                else if ((ptr->security_mode == 2) && (BTJ82SEAP(active_link)[i].link->authState == BAS_AUTHENTICATED)
                         && (BTJ82SEAP(active_link)[i].link->encryptState == BES_ENCRYPTED))
                {
					status = BT_STATUS_SUCCESS;
					goto exit;
                }
                else
                {
                    bt_trace(
                        BT_TRACE_JSR82_GROUP,
                        BT_JSR82_HANDLESETACLSECURITYREQTHIS_LINKS_AUTH_OR_ENCRYPT_ALREADY_ONGOING);
					status = BT_STATUS_FAILED;
					goto exit;
                }

            }
        }

    }

    if (remDev_index == JSR82_MAX_ACL_NO)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLESETACLSECURITYREQREMOTE_LINK_IS_NOT_EXISTED);
       	status = BT_STATUS_FAILED;
		goto exit;
    }

    for (i = 0; i < JSR82_MAX_ACL_NO; i++)
    {
        if (BTJ82SEAP(active_link)[i].used == 1)
        {
            /* 1. Check if any authentication command for one acl link is ongoing */
            /* 2. Check if any authentication command is ongoing for native BT stack */
            if ((need_authentication_link == KAL_TRUE) &&
                ((BTJ82SEAP(active_link)[i].authentication_link_state == BT_JSR82_AUTHENTICATE_LINK_ONGOING) ||
                 (BTJ82SEAP(active_link)[i].authentication_link_state == BT_JSR82_ENCRYPT_AUTHENTICATE_LINK_ONGOING)))
            {
                enable_authentication_link = KAL_FALSE;
                break;
            }
            else if ((need_encryption_link == KAL_TRUE) &&
                     (BTJ82SEAP(active_link)[i].encrypt_link_state == BT_JSR82_ENCRYPTION_LINK_ONGOING))
            {
                enable_encryption_link = KAL_FALSE;

            }
        }
    }

    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_HANDLESETACLSECURITYREQ_REMDEV_INDEXxDENABLE_AUTHENTICATION_LINKxD_ENABLE_ENCRYPTxD,
        remDev_index,
        enable_authentication_link,
        enable_encryption_link);
    if (enable_authentication_link == KAL_TRUE && (ptr->security_mode >= 1))
    {
        /*
         * The authHandler of this remDev is jsr82globalHandler
         * If the errCode of Authentication Complete Event is not zero, the BTEVENT_AUTHENTICATION_CNF will be reported to remDev's authHandler
         */
        if (BTJ82SEAP(active_link)[remDev_index].link->authState == BAS_NOT_AUTHENTICATED)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_HANDLESETACLSECURITYREQTRY_TO_AUTHENTICATE_ACL_LINK);
            status = SEC_AuthenticateLink(&jsr82globalHandler, BTJ82SEAP(active_link)[remDev_index].link);
            if (status == BT_STATUS_PENDING)
            {
                if (ptr->security_mode == 1)
                {
                    BTJ82SEAP(active_link)[remDev_index].authentication_link_state = BT_JSR82_AUTHENTICATE_LINK_ONGOING;
                }
                else
                {
                    BTJ82SEAP(active_link)[remDev_index].authentication_link_state =
                        BT_JSR82_ENCRYPT_AUTHENTICATE_LINK_ONGOING;
                }

                BTJ82SEAP(active_link)[remDev_index].authentication_transaction_id = ptr->transaction_id;
                return;
            }
            else
            {
                bt_trace(
                    BT_TRACE_JSR82_GROUP,
                    BT_JSR82_HANDLESETACLSECURITYREQ_RETURN_FROM_SEC_AUTHENTICATELINK_ERRxD,
                    status);
		  		status = BT_STATUS_FAILED;
				goto exit;
            }
        }

    }
    else if (enable_encryption_link == KAL_TRUE && (ptr->security_mode == 2))
    {
        BtStatus status = BT_STATUS_FAILED;

        status = SEC_SetLinkEncryption(&jsr82globalHandler, BTJ82SEAP(active_link)[remDev_index].link, BECM_ENCRYPT_ENABLE);
        if (status == BT_STATUS_PENDING)
        {
            BTJ82SEAP(active_link)[remDev_index].encrypt_link_state = BT_JSR82_ENCRYPTION_LINK_ONGOING;
            BTJ82SEAP(active_link)[remDev_index].authentication_transaction_id = ptr->transaction_id;
        }
        else
        {
            bt_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_HANDLESETACLSECURITYREQ_RETURN_FROM_SEC_SETLINKENCRYPTION_ERRxD,
                status);
            status = BT_STATUS_FAILED;
			goto exit;
        }
    }
    else
    {
        /* Report SetACLSecurityCnf with Error to JBT: Reject this request */
		status = BT_STATUS_FAILED;
		goto exit;
    }
exit:
	if (status == BT_STATUS_SUCCESS)
	{
		parms.p.set_acl_security_cnf.result = JSR82_SET_ACL_SECURITY_FAILED;
	}
	else
	{
		parms.p.set_acl_security_cnf.result = JSR82_SET_ACL_SECURITY_SUCCESS;
	}
	parms.p.set_acl_security_cnf.transaction_id = ptr->transaction_id;
	parms.p.set_acl_security_cnf.addr = ptr->bd_addr;
	parms.event = JSR82_EVENT_SET_ACL_SECURITY_CNF;
	parms.dstMod = MOD_MMI;
	JSR82CB(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionGetFreePSMIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U32 bt_jsr82_SessionGetFreePSMIndex(void)
{
    U32 i = 0;
    U32 mask = 0x01;
    U32 index = 0xFFFFFFFF;

    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_GET_FREE_PSM_INX_CURRENT_L2CAP_PSM_USAGE_TABLEx08X,
        BTJ82SEAP(l2cap_psm_usage_table));
    for (i = 0; i < JSR82_MAX_PSM_NO; i++)
    {
        mask = (0x01 << i);
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_GET_FREE_PSM_INX_CURRENT_MASKx08X, mask);
        if ((mask & BTJ82SEAP(l2cap_psm_usage_table)) == 0)
        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_GET_FREE_PSM_INX_GET_FREE_PSM_xD, i);
            index = i;
            BTJ82SEAP(l2cap_psm_usage_table) |= mask;
            break;
        }
    }
    if (i == JSR82_MAX_PSM_NO)
    {
        Assert("BT_JSR82_GET_FREE_PSM_INX All PSM allocated");
        return JSR82_NO_AVAILABLE_SESSION;
    }
    else
    {
        return (i * 2) + BT_JSR82_DYNAMIC_PSM;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionReleasePSMIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  psm_id      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionReleasePSMIndex(U32 psm_id)
{
    U32 mask = 0x01;

    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_RELEASE_PSM_INX_CURRENT_L2CAP_PSM_USAGE_TABLEx08X,
        BTJ82SEAP(l2cap_psm_usage_table));
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RELEASE_PSM_INX_RETURN_PSM_IDxD, psm_id);

    Assert(psm_id >= BT_JSR82_DYNAMIC_PSM);
    Assert(((psm_id - BT_JSR82_DYNAMIC_PSM) % 2) == 0);
    mask = 1 << (((psm_id - BT_JSR82_DYNAMIC_PSM) / 2));
    if ((mask & BTJ82SEAP(l2cap_psm_usage_table)) == 0)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RELEASE_PSM_INX_PSM_ERROR_NOT_REGISTERED);
    }
    else
    {
        mask = ~mask;
        BTJ82SEAP(l2cap_psm_usage_table) &= mask;
    }

    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionGetFreeServChnIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U32 bt_jsr82_SessionGetFreeServChnIndex(void)
{
    U32 i = 0;
    U32 mask = 0x01;
    U32 index = 0xFFFFFFFF;

    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_GET_FREE_SRVCHNL_INX_CURRENT_RFCOMM_CHANNEL_USAGE_TABLEx08X,
        BTJ82SEAP(rfcomm_channel_usage_table));
    for (i = 0; i < JSR82_MAX_RFCOMM_NO; i++)
    {
        mask = (0x01 << i);
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_GET_FREE_SRVCHNL_INX_CURRENT_MASKx08X, mask);
        if ((mask & BTJ82SEAP(rfcomm_channel_usage_table)) == 0)
        {
            bt_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_GET_FREE_SRVCHNL_INX_GET_FREE_CHANNEL_xD,
                i + RFCOMMTESTER_RFCOMM_SRV_CHN);
            index = i;
            BTJ82SEAP(rfcomm_channel_usage_table) |= mask;
            break;
        }
    }
    if (i == JSR82_MAX_RFCOMM_NO)
    {
        Assert("BT_JSR82_GET_FREE_SRVCHNL_INX All Channel allocated");
        return JSR82_NO_AVAILABLE_SESSION;
    }
    else
    {
        return (i + RFCOMMTESTER_RFCOMM_SRV_CHN);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionReleaseServChnIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionReleaseServChnIndex(U32 channel)
{
    U32 mask = 0x01;

    bt_trace(
        BT_TRACE_JSR82_GROUP,
        BT_JSR82_RELEASE_SRVCHNL_INX_CURRENT_RFCOMM_CHANNEL_USAGE_TABLEx08X,
        BTJ82SEAP(rfcomm_channel_usage_table));

    Assert(channel < 32);
    Assert(channel >= RFCOMMTESTER_RFCOMM_SRV_CHN);
    mask = 1 << (channel - RFCOMMTESTER_RFCOMM_SRV_CHN);
    if ((mask & BTJ82SEAP(rfcomm_channel_usage_table)) == 0)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_RELEASE_SRVCHNL_INX_CHANNEL_ERROR_NOT_REGISTERED);
    }
    else
    {
        /* Add by mtk01411: 2007-1128 */
        if (RFC(servers)[channel - 1].flags == 0)
        {
            /* All registered server channel corresponding to the same channel number are already de-registration */
            mask = ~mask;
            BTJ82SEAP(rfcomm_channel_usage_table) &= mask;
        }
        else
        {
            bt_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_RELEASE_SRVCHNL_INX_SERVER_CHNL_NUMxD_STILL_HAS_OTHER_REIGSTERED_ONES);
        }
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_sendToRfComm
 * DESCRIPTION
 *  The purpose of this function is similar to that of the function sendToRfComm()
 * PARAMETERS
 *  session_inx     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_sendToRfComm(U8 session_inx)
{
    BtPacket *pkt = NULL;
    U16 rfc_size = 0;
    U16 bytes = 0;
    BtStatus status = BT_STATUS_SUCCESS;
    U16 subsession_id = 0;
    kal_int8 rc = 0;

	bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SENDTORFCOMM_REMOVE_A_FREE_PKT_TO_SEND_DATA);

	while (TRUE)
	{
	    /* Check if there is available txPacket or not */
	    if (IsListEmpty(&BTJ82SEAP(session)[session_inx].ApTxQueue))
	    {
	        break;
	    }

	    pkt = (BtPacket*) RemoveHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue);
	    rfc_size = RF_FrameSize(&((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[session_inx].session_buffer)->rfc);
	    /* The default sent bytes number is rfc_size: Add 2007-1025 */
	    bytes = rfc_size;

	    rc = jsr82_session_DevTX(JSR82_SESSION_PS_RFCOMM, session_inx, subsession_id, (char **)&pkt->data, &bytes);
	    if ((signed char) rc < 0)
	    {
	        /* Return this avaliable txPacket back to List: Due to error */
	        InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
	        break;
	    }

	    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SENDTORFCOMM_AFTER_JBT_SESSION_DEVTX_GET_BYTESxD, bytes);
	    if (bytes > 0)
	    {
	        pkt->flags = BTP_FLAG_INUSE;
	        pkt->dataLen = bytes;
	        pkt->headerLen = 0;
	        pkt->priority_type = BTP_FLAG_LOW_PRIPORTY;
			status = RF_SendData(
	                        &((BT_JSR82_SESSION_Buffer_struct_t*) BTJ82SEAP(session)[session_inx].session_buffer)->rfc,
	                        pkt);
	        if (status != BT_STATUS_PENDING)
	        {
	        	OS_Report("[JSR82] RF send status %d", status);
	        	jsr82_session_ReturnBuf(
	                JSR82_SESSION_PS_RFCOMM,
	                session_inx,
	                subsession_id,
	                (char *)pkt->data,
	                pkt->dataLen);
	        	/* Return this avaliable txPacket back to List: Due to error */
	            InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
	        }
	    }
	    else
	    {
	        /*
	         * Need to add codes: mtk01411 2007-0831
	         * WriteRingBuf is empty: It is not possible to call this function  due to receive RFEVENT_PACKET_HANDLED
	         * In this case: If it wants to send data again, it should trigger the send data request from JBT 
	         * set is_tx_empty flag as TRUE: This can make a send_data_req from JBT to BT 
	         */
	        U16 subsession_id = 0;

			jsr82_session_ReturnBuf(
	                JSR82_SESSION_PS_RFCOMM,
	                session_inx,
	                subsession_id,
	                (char *)pkt->data,
	                pkt->dataLen);

	        jsr82_session_DevTxEmpty(JSR82_SESSION_PS_RFCOMM, session_inx, subsession_id);
	        /* Return this avaliable txPacket back to List: Due to no available data in WriteRingBuf  */
	        InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
			break;
	    }
		pkt = NULL;
	}

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_sendToL2Cap
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_inx     [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_sendToL2Cap(U8 session_inx, U16 l2cap_id)
{
    BtPacket *pkt = NULL;
    U16 bytes = 0;
    BtStatus status;
    U16 subsession_id = l2cap_id;
    kal_int8 rc = 0;

    /* Check if there is available txPacket or not */
    if (IsListEmpty(&BTJ82SEAP(session)[session_inx].ApTxQueue))
    {
        return;
    }

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SENDTOL2CAP_REMOVE_A_FREE_PKT_TO_SEND_DATA);
    pkt = (BtPacket*) RemoveHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue);

    /* The default sent bytes number is L2CAP_MTU: Add 2007-1025 */
    bytes = BTJ82SEAP(session)[session_inx].mtu;

    /* jbt_spp_DevTX(session_inx,(char **)&pkt->data, &bytes); */
    rc = jsr82_session_DevTX(JSR82_SESSION_PS_L2CAP, session_inx, subsession_id, (char **)&pkt->data, &bytes);
    /* In error state, eg., POWEROFF state or session is not in CONNECTED state: NOT to send data 2007=1115 */
    if ((signed char) rc < 0)
    {
        /* Return this avaliable txPacket back to List: Due to error */
        InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
        return;
    }

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SENDTOL2CAP_AFTER_JBT_SESSION_DEVTX_WITH_PS_L2CAP_GET_BYTESxD, bytes);
    if (bytes > 0)
    {
        pkt->flags = BTP_FLAG_INUSE;
        pkt->dataLen = bytes;
        pkt->headerLen = 0;
        pkt->priority_type = BTP_FLAG_LOW_PRIPORTY;
#ifdef __JSR82_MAUI__    
        if (BT_JSR82QueryPowerStatus() == 1)
#endif
        {
            status = L2CAP_SendData(l2cap_id, pkt);
            if (status != BT_STATUS_PENDING)
            {
                /* Return this avaliable txPacket back to List: Due to error */
                InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
            }
        }
#ifdef __JSR82_MAUI__    
        else
        {
            /* Return this avaliable txPacket back to List: Due to error */
            InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
        }
#endif
    }
    else
    {
        /*
         * Need to add codes: mtk01411 2007-0831
         * WriteRingBuf is empty: It is not possible to call this function  due to receive RFEVENT_PACKET_HANDLED
         * In this case: If it wants to send data again, it should trigger the send data request from JBT 
         * set is_tx_empty flag as TRUE: This can make a send_data_req from JBT to BT 
         */
        U16 subsession_id = 0;

        jsr82_session_DevTxEmpty(JSR82_SESSION_PS_L2CAP, session_inx, subsession_id);
        /* Return this avaliable txPacket back to List: Due to no available data in WriteRingBuf  */
        InsertHeadList(&BTJ82SEAP(session)[session_inx].ApTxQueue, &pkt->node);
    }
}

void bt_jsr82_send(U8 index, U16 l2cap_id)
{
    U8 i = 0;

    for (i = 0; i < JSR82_MAX_SESSION_NO; i++)
    {
        if ((BTJ82SEAP(session)[i].status == BT_JSR82_SESSION_ACTIVE) && (index == BTJ82SEAP(session)[i].con_id))

        {
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TX_REQ_FIND_JSR82_CHANNEL_x02X, i);
            break;
        }
    }
    if (i == JSR82_MAX_SESSION_NO)
    {
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TX_REQ_NO_EMPTY_CHANNEL);
        return;
    }

    Assert((BTJ82SEAP(session)[i].session_buffer) != 0);
    if (BTJ82SEAP(session)[i].ps_type == JSR82_SESSION_PS_RFCOMM)
    {
        bt_jsr82_sendToRfComm(index);
    }
    else
    {
        bt_jsr82_sendToL2Cap(index, l2cap_id);
    }
}


#endif /* BT_JSR82_ENABLED || defined(__BT_JSR82_L2RF__) */ 
#endif /* __BT_JSR82__ */
