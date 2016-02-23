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

/***************************************************************************
 *
 * File:
 *     $Workfile:sim_server.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: Source file for SIM Server sample application.
 *
 * Created:     May 6, 2002
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifdef __BT_SIM_PROFILE__

/* if the bluetooth_struct.h is included in your C file, also the uart_sw.h must be included. */
#ifdef __SIMAP_MAUI__
#include "kal_release.h"        /* Basic data type */
#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h" /* enum for message IDs */
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"     /* type of system module components */
#include "task_config.h"        /* Task creation */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"
#include "uart_sw.h"
#include "kal_trace.h"  /* Trace Header Files */
//#include "bt_maui_utility.h"
#endif
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt.h"
#include "bluetooth_struct.h"
#include "bt_simi.h"
#include "bluetooth_trc.h"
#include "bt_os_api.h"
#include "bluetooth_gap_struct.h"


/* Flag describing the state of the SIM */
#define SIM_CARD_FLAG_INSERTED   0x01
#define SIM_CARD_FLAG_POWERED    0x02
#define SIM_CARD_FLAG_ACCESSIBLE 0x04

/* Callback function to Internel ADP layer */
SIMAPCallback SIMAPCB;

/* current RFCOMM channel */
RfChannel * cur_rfchannel = NULL;


/* Define in other files */
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);
extern void bt_fakesim_disconnect_req_hdlr(void);
extern void bt_fakesim_apdu_req_hdlr(U16 len, U8 *apdu_req_data);
extern void bt_fakesim_poweroff_req_hdlr(void);
extern void bt_fakesim_poweron_req_hdlr(void);
extern void bt_fakesim_set_transprotocol_type_req_hdlr(void);
extern void bt_fakesim_reset_req_hdlr(void);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
#ifdef __SIMAP_MAUI__
extern void bt_simap_stop_timer(eventid *timer_id_p);
#else
extern void bt_simap_stop_timer(void *timer_id_p);
#endif

extern BtStatus SimApCloseRFCommChnl(RfChannel *rfChannel);

/* Callback routine for SIM Access events */


/* Register callback function to Internel ADP layer */
void SIMAP_Register(SIMAPCallback Callback)
{
    SIMAPCB = Callback;
}


/*****************************************************************************
 * FUNCTION
 *  SimSrvCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Server      [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimSrvCallback(SimServer *Server, SimCallbackParms *Parms)
{
    SimCardReaderStatus cardRdrStatus;
    SimCardStatus statusChange = SIM_CARD_STATUS_UNKNOWN;
    SimResultCode result = SIM_RESULT_NO_REASON;
    BtStatus status;
    U8 *ptr;
    U16 len;

    switch (Parms->event)
    {
        case SAEVENT_SERVER_OPEN_IND:
            /* When receiving this event, SimServer will send CONNECT_RSP event back to client */
            if ((status = SIM_ServerRespondOpenInd(Server, SIM_CONN_STATUS_OK)) != BT_STATUS_PENDING)
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_CON_NOT_ACCEPTED_IN_SRVCALLBACK, status);
                /* In this case means that sending of CONNECT_RSP is failed */
                /* Return value from RF_CloseChannel() may be BT_STATUS_IN_PROGRRESS(presenting rfchannel state is DLC_STATE_DISC_PENDING) */
                status = SimApCloseRFCommChnl(&(Server->rfChannel));
                if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS) && (status != BT_STATUS_SUCCESS))
                {
                    /* Report(("SERVER_OPEN_IND and RF_CloseChannel error, status=%d",status)); */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE, status);
                    Assert(0);
                }
            }
            else
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_CON_ACCEPTED_IN_SRVCALLBACK);
            }
            break;

        case SAEVENT_SERVER_OPEN:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_CON_OPEN_IN_SRVCALLBACK, Parms->status);
            if (Parms->status == BT_STATUS_SUCCESS)
            {
#if 0 // Reserved for MAUI
		  bt_simap_connect_ind_struct *bt_simap_connect_ind = NULL;
#endif
            	  SIMAPCallbackParms parms;

                /* Allocated a connection id for SIM Access Profile (SC_SIM_ACCESS: 0x112D) */
                SIM(connect_id)[0] = BTBMProfileAllocateConnIndex(SC_SIM_ACCESS, Parms->ptrs.remDev->bdAddr);
                SIM(allocated_connect_id)[0] = KAL_TRUE;
#if 0 // Reserved for MAUI
                bt_simap_connect_ind = construct_local_para(sizeof(bt_simap_connect_ind_struct), TD_UL);
                /* Reset lap,uap,nap to zero */
                bt_simap_connect_ind->lap = 0;
                bt_simap_connect_ind->uap = 0;
                bt_simap_connect_ind->nap = 0;

                memcpy(&(bt_simap_connect_ind->lap), &Parms->ptrs.remDev->bdAddr.addr[0], 3);
                bt_simap_connect_ind->uap = Parms->ptrs.remDev->bdAddr.addr[3];
                memcpy(&(bt_simap_connect_ind->nap), (U16*) & Parms->ptrs.remDev->bdAddr.addr[4], 2);

                bt_simap_connect_ind->cid = SIM(connect_id)[0];

                bt_send_msg(
                    MSG_ID_BT_SIMAP_CONNECT_IND,
                    MOD_MMI,
                    BT_APP_SAP,
                    (local_para_struct*) bt_simap_connect_ind,
                    NULL);
#endif
		  parms.event = SIMAP_EVENT_CONNECT_IND;
		  parms.dstMod = MOD_MMI;
		  parms.p.simap_connect_ind.cid = SIM(connect_id)[0];
		  parms.p.simap_connect_ind.lap = 0;
		  parms.p.simap_connect_ind.uap = 0;
		  parms.p.simap_connect_ind.nap = 0;
                btmtk_os_memcpy((U8 *)&(parms.p.simap_connect_ind.lap), &Parms->ptrs.remDev->bdAddr.addr[0], 3);
                parms.p.simap_connect_ind.uap = Parms->ptrs.remDev->bdAddr.addr[3];
                btmtk_os_memcpy((U8 *)&(parms.p.simap_connect_ind.nap), (U8 *) &Parms->ptrs.remDev->bdAddr.addr[4], 2);
                SIMAPCB(&parms);
            }
            else
            {
                /* Handle the failed OPEN Event */
                /* Return value from RF_CloseChannel() may be BT_STATUS_IN_PROGRRESS(presenting rfchannel state is DLC_STATE_DISC_PENDING) */
                status = SimApCloseRFCommChnl(&(Server->rfChannel));
                if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS) && (status != BT_STATUS_SUCCESS))
                {
                    /* Report(("SERVER_OPEN and RF_CloseChannel err, status=%d",status)); */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE, status);
                    Assert(0);
                }
            }
            break;

        case SAEVENT_SERVER_CLOSE_IND:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_CON_CLOSED_IND_IN_SRVCALLBACK, Parms->status);
            break;

        case SAEVENT_SERVER_CLOSED:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_CLOSED_IN_SRVCALLBACK, Parms->status);
            /* Determine that if is is necessary to notify SIM task to start recovery mechanism */
			Report(("[BT SIMAP_svr]SAEVENT_SERVER_CLOSED: substate=%d", SIM(simServer)[0].substate));

			//clear the RF channel
			cur_rfchannel = NULL;

            if (SIM(simServer)[0].substate & BT_REJECT_SECOND_CLIENT_SESSION)
            {
                SIM(simServer)[0].substate &= (~BT_REJECT_SECOND_CLIENT_SESSION);
                /* Report(("This closed event is due to SimServer is serving one client now, so reject another seesion")); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_CLOSED_EVENT_DUE_TO_REJECT_SECOND_CLIENT);
                return;
            }

            if ((SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT) &&
                (SIM(simServer)[0].substate & BT_SIM_CONNECT_CNF_RECV) && (SIM(simServer)[0].atr.dataLen == 0))
            {
                //Report(("CONNECT_CNF from SIM task is error, Send DISC_REQ to SIM task is not necessary when CLOSED in SimServer")); 
                //Report(("Case0:In SimSrvCallback():SimServer is available again!"));
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SRVCALLBACK_CASE0);
                SIM(simServer)[0].substate = 0;
                SIM(simServer)[0].available = TRUE;
            }
            else if (SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT)
            {
                /* Because the SIM task may notify other modules to enter remote SIM mode, it is necessary to recovery as normal mode */
                /* Through sending disconnect_req to SIM task to notify it */

                /* Check if timer for DISC_Graceful is still runing */
                if (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV)
                {
                    /* Report(("In SimSrvCallback() recv CLOSED, check timer for DISC_Graceful type")); */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SRVCALLBACK_RECV_CLOSED_CHECK_TIMER);
                    bt_simap_stop_timer(&SIM(simap_event_id));
                }

                /* Send BT_SIM_DISCONNECT_REQ message to SIM task */
                SIM(simServer)[0].substate |= BT_SIM_DISCONNECT_REQ_SENT;

            #if 0
#ifdef __SIMAP_TESTER_

            if(SIM(simServer)[0].test_mode == 1)
                bt_fakesim_disconnect_req_hdlr();
            else if(SIM(simServer)[0].test_mode == 2)
                bt_send_msg(MSG_ID_BT_SIM_DISCONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#endif
            #endif /* 0 */ 
                if (SIM(simServer)[0].exec_mode == 0)
                {
#ifdef __SIMAP_MAUI__
                    bt_send_msg(MSG_ID_BT_SIM_DISCONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#else
		      SIMAPCallbackParms parms;
		      parms.event = SIM_EVENT_DISCONNECT_REQ;
		      parms.dstMod = MOD_SIM;
		      SIMAPCB(&parms);
#endif
                }
            #ifdef __SIMAP_UT_
                else if (SIM(simServer)[0].exec_mode == 1)
                {
                    bt_fakesim_disconnect_req_hdlr();
                }
            #endif /* __SIMAP_UT_ */ 

            }
            else
            {
                /* Note: substate field has two bytes, if U8 is used the substate (e.g., 0x1000) will be narrowed to 0x00 : 2006-12-28 */
                U32 oldsubstate = SIM(simServer)[0].substate;

                kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSERVER_AVAILABLE_IN_SRVCALLBACK, oldsubstate);
                /* Report(("Case1:In SimSrvCallback():SimServer is available again, and Simserver substate=%d",oldsubstate)); */

                /*
                 * Check if this CLOSED Event is triggered due to receive MMI's DEACTIVATE_REQ in SimServer's CONNECT_PENDING state
                 * If SimServer has not yet send CONNECT_REQ to SIM task, this case is matched (No necessary to notify SIM task)
                 * The DEACTIVATE_CNF is returned here directly
                 * At that time, we call RF_CloseChannel() to trigger this event
                 */

                if (SIM(simServer)[0].substate & BT_APP_DEACTIVATE_REQ_RECV)
                {
#if 0 // Reserved for MAUI
                    bt_simap_deactivate_cnf_struct *bt_simap_deactivate_cnf = construct_local_para(sizeof(bt_simap_deactivate_cnf_struct), TD_UL);
#endif
		      SIMAPCallbackParms parms;

                    /* Using this func to deregister rfcomm server channel and sdp record of SIM Access Profile */
                    Assert(SIM_DeregisterServer(Server) == BT_STATUS_SUCCESS);
                    /* Report(("This case is SimServer closed then RFCOMM closed due to MMI Deactivate Req, return Deactivate_CNF with ok")); */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRV_CLOSED_RFCOMM_CLOSED_DUE_TO_DEACTIVATE_REQ);

                    SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;

                    /* This means that SIM Access Profile connection is closed, check if conneciton_id is still allocated */
                    if (SIM(allocated_connect_id)[0] == KAL_TRUE)
                    {
                        BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
                        SIM(allocated_connect_id)[0] = KAL_FALSE;
                        /* Report(("Release SIM Acces Profile connect_id in Case 1")); */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_SAP_CONID_CASE1);
                    }
#if 0 // Reserved for MAUI
		      bt_simap_deactivate_cnf->result = TRUE;
                    bt_send_msg(
                        MSG_ID_BT_SIMAP_DEACTIVATE_CNF,
                        MOD_MMI,
                        BT_APP_SAP,
                        (local_para_struct*) bt_simap_deactivate_cnf,
                        NULL);
#endif
		      parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
		      parms.dstMod = MOD_MMI;
		      parms.p.simap_deactivate_cnf.result = TRUE;
		      SIMAPCB(&parms);
                }

                /* 
                 * [MAUI_02129303]: As the AUTH_REQ is not responded by MMI & remote device close the RFCOMM channel (timed-out), 
                 * we have to report this event to MMI to sync the status.
                 */
                else if (BT_MMI_AUTH_REQ_SENT == (SIM(simServer)[0].substate & BT_MMI_AUTH_REQ_SENT))
                {
                	SIMAPCallbackParms parms;
                     btbm_bd_addr_t bd_addr;

                     btmtk_os_memset((kal_uint8*) & bd_addr, 0, sizeof(btbm_bd_addr_t));
                     CONVERT_ARRAY2BDADDR(&bd_addr, Server->remDev->bdAddr.addr);

		       parms.event = SIMAP_EVENT_AUTH_REQ_TIMEOUT_RFCLOSED_IND;
		       parms.dstMod = MOD_MMI;
		       parms.p.simap_auth_req_timeout_rfclose_ind.lap = bd_addr.lap;
		       parms.p.simap_auth_req_timeout_rfclose_ind.uap = bd_addr.uap;
		       parms.p.simap_auth_req_timeout_rfclose_ind.nap = bd_addr.nap;
		       SIMAPCB(&parms);
                }

                SIM(simServer)[0].substate = 0;
                SIM(simServer)[0].available = TRUE;

                if (SIM_SERVER_STATUS_DEACTIVATING == SIM(simServer)[0].simserver_activate_status)
                {
                	/* Deactivate now.*/
                	bt_simap_deactivate_req_hdlr();
                }

            }

            break;

        case SAEVENT_SERVER_STATUS_REQ:

            if (Parms->status == BT_STATUS_SUCCESS)
            {
                if (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON)
                {
                    statusChange = SIM_CARD_STATUS_RESET;
                }
                else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                         (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
                {
                    statusChange = SIM_CARD_STATUS_INSERTED;
                }
                else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
                {
                    statusChange = SIM_CARD_STATUS_REMOVED;
                }

            }
            else if (Parms->status == BT_STATUS_FAILED)
            {
                statusChange = SIM_CARD_STATUS_NOT_ACCESSIBLE;
            }

            if ((status = SIM_ServerSendStatus(Server, statusChange)) != BT_STATUS_PENDING)
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_SEND_STATUS_IND_IN_SRVCALLBACK, status);
            }
            else
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_STATUS_IND_IN_SRVCALLBACK);
            }
            break;

        case SAEVENT_SERVER_ATR_REQ:
			//kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMCARDREADER_STATUS, SIM(simServer)[0].current_simcardreader_status);
			Report(("[BT SIMAP]current_simcardreader_status=0x%x", SIM(simServer)[0].current_simcardreader_status));
            if ((SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                result = SIM_RESULT_OK;
                ptr = SIM(simServer)[0].atr.data;
                len = SIM(simServer)[0].atr.dataLen;

            }
            else if (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT)
            {
                /* Alread off : This case is POWERED_OFF but CARD_PRESENT */
                result = SIM_RESULT_CARD_ALREADY_OFF;
                len = 0;
                ptr = NULL;
            }
            else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /* No card inserted */
                /* Note: it is impossible that POWERED_ON but CARD_NOT_PRESENT; so this is CARD_NOT_PRESENT case */
                result = SIM_RESULT_CARD_REMOVED;
                len = 0;
                ptr = NULL;
            }

            status = SIM_ServerAtrRsp(Server, len, ptr, result);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_SEND_ATR_RSP_IN_SRVCALLBACK, result, status);
            if (status != BT_STATUS_SUCCESS)
            {
                status = SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
                Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS));
            }
            break;

        case SAEVENT_SERVER_APDU_REQ:

            if ((SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {

                if (SIM(simServer)[0].exec_mode == 0)
                {
#ifdef __SIMAP_MAUI__
                    bt_sim_apdu_req_struct *apdu_req = NULL;
#else
		      SIMAPCallbackParms parms;
#endif
                    SIM(simServer)[0].substate |= BT_SIM_APDU_REQ_SENT;
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_APDU_TO_SIM_IN_SRVCALLBACK);
#ifdef __SIMAP_MAUI__
                    /* Send APDU request to SIM task */
                    apdu_req = construct_local_para(sizeof(bt_sim_apdu_req_struct), TD_UL);

                    apdu_req->apdu_req_len = Parms->ptrs.msg->parm.apdu.dataLen;
                    OS_MemSet(apdu_req->apdu_req, 0, APDU_REQ_MAX_LEN);
                    OS_MemCopy(apdu_req->apdu_req, Parms->ptrs.msg->parm.apdu.data, apdu_req->apdu_req_len);

                    apdu_req->transport_protocol_type = SIM(simServer)[0].current_transport_protocol_type;
                    bt_send_msg(MSG_ID_BT_SIM_APDU_REQ, MOD_SIM, PS_SIM_SAP, (local_para_struct*) apdu_req, NULL);
#else
		      parms.event = SIM_EVENT_APDU_REQ;
		      parms.dstMod = MOD_SIM;
		      parms.p.sim_apdu_req.apdu_req_len = Parms->ptrs.msg->parm.apdu.dataLen;
		      btmtk_os_memset(parms.p.sim_apdu_req.apdu_req, 0, APDU_REQ_MAX_LEN);
		      btmtk_os_memcpy(parms.p.sim_apdu_req.apdu_req, Parms->ptrs.msg->parm.apdu.data, parms.p.sim_apdu_req.apdu_req_len);
		      parms.p.sim_apdu_req.transport_protocol_type = SIM(simServer)[0].current_transport_protocol_type;
		      SIMAPCB(&parms);
#endif
                }
            #ifdef __SIMAP_UT_
                else if (SIM(simServer)[0].exec_mode == 1)
                {

                    bt_fakesim_apdu_req_hdlr(Parms->ptrs.msg->parm.apdu.dataLen, Parms->ptrs.msg->parm.apdu.data);
                }
            #endif /* __SIMAP_UT_ */ 
            }
            else if (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT)
            {
                /* Alread off : This case is POWERED_OFF but CARD_PRESENT */
                result = SIM_RESULT_CARD_ALREADY_OFF;
                ptr = NULL;
                len = 0;
                status = SIM_ServerApduRsp(&SIM(simServer)[0], len, ptr, result);
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_APDU_RSP_TO_CLIENT, result, status, 1);
                if (status != BT_STATUS_SUCCESS)
                {
                    status = SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
                    Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS));

                }
            }
            else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /* No card inserted */
                /* Note: it is impossible that POWERED_ON but CARD_NOT_PRESENT; so this is CARD_NOT_PRESENT case */
                result = SIM_RESULT_CARD_REMOVED;
                ptr = NULL;
                len = 0;
                status = SIM_ServerApduRsp(&SIM(simServer)[0], len, ptr, result);
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_APDU_RSP_TO_CLIENT, result, status, 2);
                if (status != BT_STATUS_SUCCESS)
                {
                    Assert(0);
                }
            }
            break;

        case SAEVENT_SERVER_SIM_OFF_REQ:

            if ((SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /* In this case: must SIM_CRS_CARD_POWERED_ON and SIM_CRS_CARD_PRESENT are satisfied at the same time */
                /* Send POWER_OFF_REQ to SIM task */
                kal_trace(TRACE_GROUP_1, BT_SIM_SEND_POWER_OFF_TO_SIM_IN_SRVCALLBACK);
                SIM(simServer)[0].substate |= BT_SIM_POWER_OFF_REQ_SENT;

                if (SIM(simServer)[0].exec_mode == 0)
                {
#ifdef __SIMAP_MAUI__
                    bt_send_msg(MSG_ID_BT_SIM_POWER_OFF_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#else
		      SIMAPCallbackParms parms;
		      parms.event = SIM_EVENT_POWER_OFF_REQ;
		      parms.dstMod = MOD_SIM;
		      SIMAPCB(&parms);
#endif
                }

            #ifdef __SIMAP_UT_
                else if (SIM(simServer)[0].exec_mode == 1)
                {
                    bt_fakesim_poweroff_req_hdlr();
                }
            #endif /* __SIMAP_UT_ */ 
                return;
            }
            else if (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT)
            {
                /* Alread off : This case is POWERED_OFF but CARD_PRESENT */
                result = SIM_RESULT_CARD_ALREADY_OFF;
            }
            else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /* No card inserted */
                /* Note: it is impossible that POWERED_ON but CARD_NOT_PRESENT; so this is CARD_NOT_PRESENT case */
                result = SIM_RESULT_CARD_REMOVED;
            }

            /* Send POWER_OFF_RSP to client */
            status = SIM_ServerSimOffRsp(Server, result);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_POWER_OFF_RSP_TO_CLIENT, result, status);
            if (status != BT_STATUS_SUCCESS)
            {
                status = SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
                Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS));

            }
            break;

        case SAEVENT_SERVER_SIM_ON_REQ:
            /* if current information is enough to return rsp, then sends rsp in this function */
            /* POWE_ON_REQ only let client to request T at 0 (T=0) */

            if ((SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                result = SIM_RESULT_CARD_ALREADY_ON;
            }
            else if (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT)
            {
                /*
                 * Because SIM card is already off, so current_transport_protocol_type is invalid
                 * Set T=0 directly and send BT_SIM_RESET_REQ to sim task!
                 */
                if (SIM(simServer)[0].supported_transport_protocol_type_capability == 2 ||
                    SIM(simServer)[0].supported_transport_protocol_type_capability == 0)
                {
                    SIM(simServer)[0].substate |= BT_SIM_RESET_REQ_SENT_POWER_ON;

                    if (SIM(simServer)[0].exec_mode == 0)
                    {
#ifdef __SIMAP_MAUI__
			   bt_sim_reset_req_struct *reset_req = NULL;
                        reset_req = construct_local_para(sizeof(bt_sim_reset_req_struct), TD_UL);
                        reset_req->transport_protocol_type = 0; /* default using T=0 transport protocol type */
                        bt_send_msg(
                            MSG_ID_BT_SIM_RESET_REQ,
                            MOD_SIM,
                            PS_SIM_SAP,
                            (local_para_struct*) reset_req,
                            NULL);
#else
			   SIMAPCallbackParms parms;
			   parms.event = SIM_EVENT_RESET_REQ;
			   parms.dstMod = MOD_SIM;
			   parms.p.sim_reset_req.transport_protocol_type = 0;
			   SIMAPCB(&parms);
#endif
                    }
                #ifdef __SIMAP_UT_
                    else if (SIM(simServer)[0].exec_mode == 1)
                    {
                        bt_fakesim_poweron_req_hdlr();
                    }
                #endif /* __SIMAP_UT_ */ 
                    return;
                }
                else if (SIM(simServer)[0].supported_transport_protocol_type_capability == 1)
                {
                    /* SIM Card only supports T=1: RESET Error! because not support T=0, RESET_REQ uses T=0! */
                    result = SIM_RESULT_CARD_NOT_ACCESSIBLE;
                }

            }
            else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /* Note: it is impossible that POWERED_ON but CARD_NOT_PRESENT; so this is CARD_NOT_PRESENT case */
                result = SIM_RESULT_CARD_REMOVED;
            }
            else
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_NO_RULES_PROCESS_SIM_ON_REQ_IN_SRVCALLBACK);
                Assert(0);
            }

            /* Send SIM_POWER_ON_RSP to sim client with result code */
            status = SIM_ServerSimOnRsp(Server, result);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_POWER_ON_RSP_TO_CLIENT_IN_SRVCALLBACK, result, status);
            if (status != BT_STATUS_SUCCESS)
            {
                status = SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
                Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS));

            }
            break;

        case SAEVENT_SERVER_SET_TRANS_PROT_REQ:

        #if SET_T_SUPPORTED == TRUE
            /* if current information is enough to return rsp, then sends rsp in this function */
            /* Set Transport Protocol Type Req will ask to reset the SIM Card */
            SIM(simServer)[0].client_requested_transport_protocol_type = Parms->ptrs.msg->parm.protocolId;
            /* Report(("In SimSrvCallback(): client requested T type=%d",SIM(simServer)[0].client_requested_transport_protocol_type)); */
            kal_trace(
                TRACE_GROUP_1,
                BT_SIMAP_CLIENT_REQ_T_TYPE,
                SIM(simServer)[0].client_requested_transport_protocol_type);
            if ((SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                if (SIM(simServer)[0].supported_transport_protocol_type_capability == 2)
                {
                    /* SIM Card supports both T=0 and T=1: Send BT_SIM_RESET_REQ msg to SIM task */

                    if (SIM(simServer)[0].current_transport_protocol_type != Parms->ptrs.msg->parm.protocolId)
                    {
                        Assert((SIM(simServer)[0].current_transport_protocol_type == 0) ||
                               (SIM(simServer)[0].current_transport_protocol_type == 1));
                        SIM(simServer)[0].substate |= BT_SIM_RESET_REQ_SENT_SET_T;

                        if (SIM(simServer)[0].exec_mode == 0)
                        {
#ifdef __SIMAP_MAUI__
				bt_sim_reset_req_struct *reset_req = NULL;
                            reset_req = construct_local_para(sizeof(bt_sim_reset_req_struct), TD_UL);
                            reset_req->transport_protocol_type = Parms->ptrs.msg->parm.protocolId;
                            bt_send_msg(
                                MSG_ID_BT_SIM_RESET_REQ,
                                MOD_SIM,
                                PS_SIM_SAP,
                                (local_para_struct*) reset_req,
                                NULL);
#else
				SIMAPCallbackParms parms;
				parms.event = SIM_EVENT_RESET_REQ;
				parms.dstMod = MOD_SIM;
				parms.p.sim_reset_req.transport_protocol_type = Parms->ptrs.msg->parm.protocolId;
				SIMAPCB(&parms);
#endif
                        }
                    #ifdef __SIMAP_UT_
                        else if (SIM(simServer)[0].exec_mode == 1)
                        {
                            bt_fakesim_set_transprotocol_type_req_hdlr();
                        }
                    #endif /* __SIMAP_UT_ */ 

                        return;
                    }
                    else
                    {
                        /* Requested T type is the same as current T type, send rsp to sim client directly */
                        SIM(simServer)[0].substate |= BT_SIM_RESET_REQ_SENT_SET_T;
                        result = SIM_RESULT_OK;
                    }
                }
                else if (SIM(simServer)[0].supported_transport_protocol_type_capability == 1 ||
                         SIM(simServer)[0].supported_transport_protocol_type_capability == 0)
                {
                    /* In this case: SIM card only supports T=0 or T=1 and current_transport_protocol_type is equal to supported_transport_protocol_type_capability */
                    if (SIM(simServer)[0].current_transport_protocol_type != Parms->ptrs.msg->parm.protocolId)
                    {
                        /* Send REST_RSP with error code: Can't accessible to SIM Client */
                        /*
                         * In this case: Send SET_T_RSP first 
                         * then in Open state PACKET_HANDLED EVENT: send STATUS_IND with card not accessible 
                         */
                        /* Report(("SET T=0/T=1 but only support T=1/T=0, Send SET_T_RSP first then STATUS_IND with card not accessible later")); */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_SET_T0_T1_NOT_SUPPORT_T1_T0);
                        SIM(simServer)[0].substate |= BT_SIM_RESET_REQ_SENT_SET_T;
                        result = SIM_RESULT_NOT_SUPPORTED;
                    }
                    else
                    {
                        /* Requested T type is the same as current T type, send rsp to sim client directly */
                        result = SIM_RESULT_OK;
                    }
                }
            }
            else if ((!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON)) &&
                     (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {

                result = SIM_RESULT_CARD_ALREADY_OFF;
            }
            else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                result = SIM_RESULT_CARD_REMOVED;
            }
        #else /* SET_T_SUPPORTED == TRUE */
            result = SIM_RESULT_NOT_SUPPORTED;
        #endif /* SET_T_SUPPORTED == TRUE */
            /* Send SIM_SET_TRANSPORT_PROTOCOL_TYPE_RSP to sim client with result code */
            status = SIM_ServerSetTransProtRsp(Server, result);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_SET_T_RSP_TO_CLIENT_IN_SRVCALLBACK, result, status);
            if (status != BT_STATUS_SUCCESS)
            {
                Assert(0);
            }
            break;

        case SAEVENT_SERVER_RESET_SIM_REQ:
            /* if current information is enough to return rsp, then sends rsp in this function */
            /* RESET_REQ only let client to request T at 0 (T=0) */

            if ((SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON) &&
                (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /*
                 * The original following state is wrong! 
                 * Because as long as one bit (SIM_CRS_CARD_POWERED_ON or SIM_CRS_CARD_PRESENT) is set in current_simcardreader_status, condition is true!
                 * if((SIM(simServer)[0].current_simcardreader_status & (SIM_CRS_CARD_POWERED_ON | SIM_CRS_CARD_PRESENT))) 
                 */

                /* In this case, must POWERED_ON and CARD_PRESERT are statisfied at the same time */
                if (SIM(simServer)[0].supported_transport_protocol_type_capability == 2)
                {
                    /* SIM Card supports both T=0 and T=1: Send BT_SIM_RESET_REQ msg to SIM task */
                    Assert((SIM(simServer)[0].current_transport_protocol_type == 0) ||
                           (SIM(simServer)[0].current_transport_protocol_type == 1));
                    SIM(simServer)[0].substate |= BT_SIM_RESET_REQ_SENT;

                    if (SIM(simServer)[0].exec_mode == 0)
                    {
#ifdef __SIMAP_MAUI__
			   bt_sim_reset_req_struct *reset_req = NULL;
                        reset_req = construct_local_para(sizeof(bt_sim_reset_req_struct), TD_UL);
                        reset_req->transport_protocol_type = 0; /* default using T=0 transport protocol type */
                        bt_send_msg(
                            MSG_ID_BT_SIM_RESET_REQ,
                            MOD_SIM,
                            PS_SIM_SAP,
                            (local_para_struct*) reset_req,
                            NULL);
#else
			   SIMAPCallbackParms parms;
			   parms.event = SIM_EVENT_RESET_REQ;
			   parms.dstMod = MOD_SIM;
			   parms.p.sim_reset_req.transport_protocol_type = 0;
			   SIMAPCB(&parms);
#endif
                    }
                #ifdef __SIMAP_UT_
                    else if (SIM(simServer)[0].exec_mode == 1)
                    {
                        bt_fakesim_reset_req_hdlr();
                    }
                #endif /* __SIMAP_UT_ */ 

                    return;
                }
                else if (SIM(simServer)[0].supported_transport_protocol_type_capability == 1)
                {
                    /* SIM Card only supports T=1: RESET Error! because not support T=0, RESET_REQ uses T=0! */
                    Assert(SIM(simServer)[0].supported_transport_protocol_type_capability == SIM(simServer)[0].current_transport_protocol_type);
                    /* Send REST_RSP with error code: Can't accessible to SIM Client */
                    result = SIM_RESULT_CARD_NOT_ACCESSIBLE;
                }
                else if (SIM(simServer)[0].supported_transport_protocol_type_capability == 0)
                {
                    /* SIM Card only supports T=0: Send BT_SIM_RESET_REQ msg to SIM task */
                    Assert(SIM(simServer)[0].supported_transport_protocol_type_capability == SIM(simServer)[0].current_transport_protocol_type);
                    SIM(simServer)[0].substate |= BT_SIM_RESET_REQ_SENT;

                    if (SIM(simServer)[0].exec_mode == 0)
                    {
#ifdef __SIMAP_MAUI__
			   bt_sim_reset_req_struct *reset_req = NULL;
                        reset_req = construct_local_para(sizeof(bt_sim_reset_req_struct), TD_UL);
                        reset_req->transport_protocol_type = 0; /* default using T=0 transport protocol type */
                        bt_send_msg(
                            MSG_ID_BT_SIM_RESET_REQ,
                            MOD_SIM,
                            PS_SIM_SAP,
                            (local_para_struct*) reset_req,
                            NULL);
#else
			   SIMAPCallbackParms parms;
			   parms.event = SIM_EVENT_RESET_REQ;
			   parms.dstMod = MOD_SIM;
			   parms.p.sim_reset_req.transport_protocol_type = 0;
			   SIMAPCB(&parms);
#endif
                    }
                #ifdef __SIMAP_UT_
                    else if (SIM(simServer)[0].exec_mode == 1)
                    {
                        bt_fakesim_reset_req_hdlr();
                    }
                #endif /* __SIMAP_UT_ */ 
                    return;
                }
            }
            else if (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT)
            {
                /* This case is SIM_CRS_CARD_PRESENT(0x40) with SIM_CRS_CARD_POWERED_OFF(0x00) */
                /* Send REST_RSP with error code: Card powered off to SIM Client */
                result = SIM_RESULT_CARD_ALREADY_OFF;
            }
            else if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_PRESENT))
            {
                /* Send REST_RSP with error code: Card Removed to SIM Client */
                /* Note: it is impossible that POWERED_ON but CRS_NOT_PRESENT case, so this is CARD_NOT_PRESENT case */
                result = SIM_RESULT_CARD_REMOVED;
            }
            else
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_NO_RULES_PROCESS_RESET_REQ_IN_SRVCALLBACK);
                Assert(0);
            }

            /* Send SIM_RESET_RSP to sim client with result code */
            status = SIM_ServerResetSimRsp(Server, result);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_RESET_RSP_TO_CLIENT_IN_SRVCALLBACK, result, status);
            if (status != BT_STATUS_SUCCESS)
            {
                status = SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
                Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS));
            }
            break;

        case SAEVENT_SERVER_CARD_READER_STATUS_REQ:
            cardRdrStatus = SIM(simServer)[0].current_simcardreader_status;
            status = SIM_ServerCardReaderStatusRsp(Server, cardRdrStatus, SIM_RESULT_OK);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_CARD_RDS_RSP_TO_CLIENT_IN_SRVCALLBACK, result, status);
            if (status != BT_STATUS_SUCCESS)
            {
                status = SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
                Assert((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS));
            }

            break;

        case SAEVENT_SERVER_STATUS_SENT:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_STATUS_IND_SENT_IN_SRVCALLBACK, Parms->status);
            /* Note that the STATUS_SENT Event may be failed, handle this case here */
            if (Parms->status != BT_STATUS_SUCCESS)
            {
                if (Server->state == SIM_SERVER_STATE_CONN_PENDING)
                {
                    /* In CONNECT_PENDING state, this means that STATUS_IND(with reset) is sent failed */
                    /* Close this SIM Access Profile connection */
                    /* Return value from RF_CloseChannel() may be BT_STATUS_IN_PROGRRESS(presenting rfchannel state is DLC_STATE_DISC_PENDING) */
                    status = SimApCloseRFCommChnl(&(Server->rfChannel));
                    if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS) && (status != BT_STATUS_SUCCESS))
                    {
                        /* Report(("SERVER_STATUS_SENT and RF_CloseChannel err, status=%d",status)); */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE, status);
                        Assert(0);
                    }
                }
                else if (Server->state == SIM_SERVER_STATE_OPEN)
                {   /* Ignore it */
                }
                else
                {
                    /* Send STATUS_IND in wrong SimServer's state and send this event fail */
                    Assert(0);
                }
            }
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimSrvSetGracefulDiscType
 * DESCRIPTION
 *  For PTS SIMAP Testing Usage
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SimSrvSetGracefulDiscType(void)
{
    SIM(simServer)[0].discType = SIM_DISCONNECT_GRACEFUL;
    kal_trace(TRACE_GROUP_1, BT_SIMAP_CURRENT_DISC_TYPE, SIM(simServer)[0].discType);

}


/*****************************************************************************
 * FUNCTION
 *  SimSrvSetImmediateDiscType
 * DESCRIPTION
 *  For PTS SIMAP Testing Usage
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SimSrvSetImmediateDiscType(void)
{
    SIM(simServer)[0].discType = SIM_DISCONNECT_IMMEDIATE;
    kal_trace(TRACE_GROUP_1, BT_SIMAP_CURRENT_DISC_TYPE, SIM(simServer)[0].discType);

}


/*****************************************************************************
 * FUNCTION
 *  SimSrvSimCardUnavailableSendStatusIndvoid
 * DESCRIPTION
 *  For PTS SIMAP Testing Usage
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SimSrvSimCardUnavailableSendStatusIndvoid(void)
{
    /*
     * Add Send Card Status with Card Not Accessible to Client 2008-0614
     * Note that: STATUS_IND is only sent in Server's OPEN or CON_PENDING state in SIM_ServerSendStatus() 
     */
    if (SIM(simServer)[0].simserver_activate_status != SIM_SERVER_STATUS_ACTIVATED)
    {
#ifdef __SIMAP_MAUI__
        Assert(0);
#else
	 return ;
#endif
    }
    kal_trace(TRACE_GROUP_1, BT_SIMAP_MAKE_SIMCARD_UNAVAILABLE_THEN_SEND_STATUS_IND);
    SIM(simServer)[0].flags |= SIM_SERVER_FLAG_STATUS;
    SimServerAppCallback(&SIM(simServer)[0], SAEVENT_SERVER_STATUS_REQ, BT_STATUS_FAILED, 0, 0);
}


void SimServerAuthorize(RfChannel *Channel, BD_ADDR bdAddr)
{
	SIMAPCallbackParms parms;
	btbm_bd_addr_t bd_addr;

	/* record the current channel*/
	cur_rfchannel = Channel;

	SIM(simServer)[0].substate |= BT_MMI_AUTH_REQ_SENT;
		
	btmtk_os_memset((kal_uint8*) & bd_addr, 0, sizeof(btbm_bd_addr_t));
	CONVERT_ARRAY2BDADDR(&bd_addr, bdAddr.addr);
	
	
#if 0 // Reserved for MAUI
	bt_simap_auth_req->lap = 0;
	bt_simap_auth_req->uap = 0;
	bt_simap_auth_req->nap = 0;
	bt_simap_auth_req->lap = bd_addr.lap;
	bt_simap_auth_req->uap = bd_addr.uap;
	bt_simap_auth_req->nap = bd_addr.nap;
#endif
	parms.p.simap_auth_req.lap = 0;
	parms.p.simap_auth_req.uap = 0;
	parms.p.simap_auth_req.nap = 0;
	parms.p.simap_auth_req.lap = bd_addr.lap;
	parms.p.simap_auth_req.uap = bd_addr.uap;
	parms.p.simap_auth_req.nap = bd_addr.nap;

	/* Send AUTH_REQ msg to MMI */
#if 0 // Reserved for MAUI
	bt_send_msg(
		MSG_ID_BT_SIMAP_AUTH_REQ,
		MOD_MMI,
		BT_APP_SAP,
		(local_para_struct*) bt_simap_auth_req,
		NULL);
#endif

	parms.event = SIMAP_EVENT_AUTH_REQ;
	parms.dstMod = MOD_MMI;
	SIMAPCB(&parms);

}

void SIMAP_AcceptIncomingRequest(void)
{
	if (cur_rfchannel != NULL)
		RF_AcceptChannel(cur_rfchannel);
}


void SIMAP_RejectIncomingRequest(void)
{
	if (cur_rfchannel != NULL)
		RF_RejectChannel(cur_rfchannel);

	cur_rfchannel = NULL;
}

#endif /* __BT_SIM_PROFILE__ */
