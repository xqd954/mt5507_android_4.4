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

/* 
 * simap_api.c
 * 
 * This file is part of the SIMAP profile that includes the interfaces for internal ADP.
 * Flow direction: internal ADP API --> SIMAP API --> SPP core
 */
#ifdef __BT_SIM_PROFILE__

#include "bt_common.h"
#include "bluetooth_struct.h"
#include "bt.h"
#include "btconfig.h"
#include "bt_simi.h"
#include "bluetooth_trc.h"
#include "sdpdb.h"
#include "config.h"
#include "btaccctrl.h"


extern SIMAPCallback SIMAPCB;

/* Definition in other files */
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);

#ifdef __SIMAP_MAUI__
extern bt_context_struct *bt_p;
#endif
extern void SimSrvCallback(SimServer *Server, SimCallbackParms *Parms);
extern void bt_fakesim_connect_req_hdlr(void);
extern void bt_fakesim_reset_req_hdlr(void);

extern void SetPacketListMsgSize(SimServer *Server, kal_uint16 msg_size);

extern BtStatus SimApCloseRFCommChnl(RfChannel *rfChannel);
extern void BTBMLocalAllowAllLink(void);
extern void SimSrvSimCardUnavailableSendStatusIndvoid(void);

#ifdef __SIMAP_REG_GLOBAL_HDLER__
static BtHandler SimAp_globalHandler;
#endif 

void Me2SimApCallback(const BtEvent *Event);

#ifdef __SIMAP_MAUI__
extern void bt_simap_stop_timer(eventid *timer_id_p);
#else
extern void bt_simap_stop_timer(void *timer_id_p);
#endif

extern void SIMAP_AcceptIncomingRequest(void);
extern void SIMAP_RejectIncomingRequest(void);



/*****************************************************************************
 * FUNCTION
 *  bt_simap_config_simserver
 * DESCRIPTION
 *  This function is invoked by bt_simap_init()
 *  In this function, simServer will be configured
 *  "available" field will be set to TRUE when receiving MMI's MSG_ID_BT_SIMAP_ACTIVATE_REQ 
 *  "simserver_activate_status" field will be set to SIM_SERVER_STATUS_ACTIVATED when receiving MMI's MSG_ID_BT_SIMAP_ACTIVATE_REQ
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_config_simserver(void)
{
    /* Add only one SIM card and its server's configuration : because our design only supports one card now! */

    SIM(simServer)[0].supported_by_sim = TRUE;
    SIM(simServer)[0].available = FALSE;
    SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;
    SIM(simServer)[0].current_simcardreader_status =
        (CARD_ID(0) | SIM_CRS_NOT_REMOVABLE | SIM_CRS_PRESENT | SIM_CRS_NOT_ID1_SIZE);
    SIM(simServer)[0].connect_mode = SLOW_CONNECT_MODE;

    SIM(simServer)[0].exec_mode = 0;    /* Default: Send msg to SIM task */
    SIM(simServer)[0].discType = SIM_DISCONNECT_IMMEDIATE; 
#ifdef __SIMAP_MAUI__
#else
    SIM(simap_event_id).context = NULL;
#endif
}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_allocated_one_simserver
 * DESCRIPTION
 *  
 * PARAMETERS
 *  CARD_ID     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
SimServer *bt_simap_allocated_one_simserver(U8 CARD_ID)
{
    if (SIM(simServer)[CARD_ID].supported_by_sim)
    {
        return (&SIM(simServer)[CARD_ID]);
    }
    else
    {
        return NULL;    /* CARD is already used by one remote SIM Client */
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_activate_simserver_status
 * DESCRIPTION
 *  This function is invoked by bt_simap_actviate_req_hdlr() when receiving MMI's MSG_ID_BT_SIMAP_ACTIVATE_REQ
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_activate_simserver_status(void)
{
    
    SIM(simServer)[0].available = TRUE;
    SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_ACTIVATED;

}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_deactivate_simserver_status
 * DESCRIPTION
 *  This function is invoked by bt_simap_deactviate_req_hdlr() when receiving MMI's MSG_ID_BT_SIMAP_DEACTIVATE_REQ
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_deactivate_simserver_status(void)
{
    
    SIM(simServer)[0].available = FALSE;
    SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;
    bt_simap_config_simserver();

}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_activate_req_hdlr
 * DESCRIPTION
 *  In this function: it will to activate simServer's status: let this server be available and in activate status
 *  Also, invoking SIM_RegisterServer() in this activate hdlr, the related actions contains:
 *  Registration rfcomm server channel
 *  Registration sdp records
 *  Registraion CMGR hdlr...etc.
 *  The above three things are done in function SIM_RegisterServer()
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_activate_req_hdlr(void)
{
    SimServer *simServer = NULL;
    BtStatus status = BT_STATUS_FAILED;
    SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
    bt_simap_activate_cnf_struct *bt_simap_activate_cnf = NULL;
    bt_simap_activate_cnf = construct_local_para(sizeof(bt_simap_activate_cnf_struct), TD_UL);
    bt_simap_activate_cnf->result = TRUE;
#endif

    if (SIM(simServer)[0].simserver_activate_status != SIM_SERVER_STATUS_DEACTIVATED)
    {
	 parms.event = SIMAP_EVENT_ACTIVATE_CNF;
	 parms.dstMod = MOD_MMI;
	 parms.p.simap_activate_cnf.result = FALSE;
	 SIMAPCB(&parms);
#if 0 // Reserved for MAUI __SIMAP_MAUI__
        Assert(0);
#endif
	 return ;
    }

    /* Set the activate_status to SIM_SERVER_STATUS_ACTIVATED for the simServer supports ID_0 SIM CARD: 2006-11-05 */
    bt_simap_activate_simserver_status();
    SimInitStateMachine();

#if SIM_SERVER == XA_ENABLED
    /*
     * if our design supports multiple cards, then each sim card is in each server, MAX_NUM_SIM_SERVER is 7:
     * * so serverList is possible to have more than one nodes!
     */

    if (MAX_NUM_SIM_SERVER == 1)
    {
        /* Report(("Only one SIM card is supported in our current design, Select ID_0")); */

        simServer = bt_simap_allocated_one_simserver(CARD_ID(0));

        if (simServer == NULL)
        {
            Assert(0);
        }

        //If state is not reset to zero, and recv DEACTIVATE_REQ immediately,
        //Maybe the initial value of simServer->rfChannel.state !=0, goto the aseert condition!
        simServer->rfChannel.state = 0;
    }

    /* Before register simServer to serverList, initialize the SIM(serverList) first */
    InitializeListHead(&SIM(serverList));

    status = SIM_RegisterServer(simServer, SimSrvCallback);
    if (status != BT_STATUS_SUCCESS)
    {
        /* Do Something here 2006-11-01 */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SIM_REGSERVER, status);
        Assert(0);
    }

    /*
     * SIM Access Profile Activate Procedure:
     * * 1. MMI sends LINK_DISALLOW_REQ to BM, so BM  changes tp power_off mode to reject all acl links
     * * 2. In SIM_RegisterServer(): Using BTAccCtrlSet() to set new access policy: only allowing SDP and SIM Access Profile
     * * 3. Using BTBMLocalAllowAllLink() to let BM change to power_on mode to allow acl link establishment
     */
    BTBMLocalAllowAllLink();

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
    InitializeListHead(&SIM(clientList));
#endif 
    /* Report(("SIM Access Profile is activated completely")); */

    /* Send ACTIVATE_CNF msg to APP/MMI */
#if 0 // Reserved for MAUI
    bt_send_msg(MSG_ID_BT_SIMAP_ACTIVATE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) bt_simap_activate_cnf, NULL);
#endif
    parms.event = SIMAP_EVENT_ACTIVATE_CNF;
    parms.dstMod = MOD_MMI;
    parms.p.simap_activate_cnf.result = TRUE;
    SIMAPCB(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_deactivate_req_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_deactivate_req_hdlr(void)
{

    /* Close SIM Server Connect 2006-10-16: using immediate type? */
    BtStatus status = BT_STATUS_FAILED;
    bt_simap_deactivate_cnf_struct *bt_simap_deactivate_cnf = NULL;

    /* Check current state of SIM Access Profile Server first! */

    if ((SIM(simServer)[0].simserver_activate_status != SIM_SERVER_STATUS_ACTIVATED) && (SIM_SERVER_STATUS_DEACTIVATING != SIM(simServer)[0].simserver_activate_status))
    {
	 SIMAPCallbackParms parms;
	 parms.p.simap_deactivate_cnf.result = FALSE;
	 parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
#if 0 // Reserved for MAUI  __SIMAP_MAUI__
        Assert(0);
#endif
	 return ;
    }

    kal_trace(TRACE_GROUP_1, BT_SIMAP_SRVSTATE_RCV_DEACTIVATE, SIM(simServer)[0].state, SIM(simServer)[0].substate,  SIM(simServer)[0].rfChannel.state);

    if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CLOSED) &&
        (SIM(simServer)[0].substate & BT_SIM_DISCONNECT_REQ_SENT))
    {
        /*
         * [Scenario#1]
         * * During POWER_OFF Procedure: MMI sends LINK_DISALLOWED to BT, the sequences is:
         * * 1. SimServer receives CLOSED event (Now: in SIM_SERVER_STATE_CLOSED state), and sends DISC_REQ to SIM task
         * * 2. After all connections are terminated, MMI sends DEACTIVATE_REQ msg to SIM Access Profile
         * * 3. But, in this case: DISC_CNF from SIM task is not received:
         * *    DO NOT return DEACTIVATE_CNF msg to MMI here!  
         */

        /*
         * [Scenario#2] 
         * * Something wrong happens, and link is disconnected
         * * 1. SimServer receives CLOSED event (Now: in SIM_SERVER_STATE_CLOSED state), and sends DISC_REQ to SIM task
         * * 2. At this time, user press MMI's button(i.e. Turn Off Remote SIM Access) to sends DEACTIVATE_REQ msg to SIM Access Profile
         * * 3. But, in this case: DISC_CNF from SIM task is not received:
         * *    DO NOT return DEACTIVATE_CNF msg to MMI here!
         */

        SIM(simServer)[0].substate |= BT_APP_DEACTIVATE_REQ_RECV;
        /* Report(("Recv Deactive_req, although SimServer is closed, but it waits for SIM task's DISC_CNF")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SRV_CLOSESTATE_DISC_SENT);
    }
    else if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CLOSED) && (SIM(simServer)[0].substate == 0) &&
             (SIM(simServer)[0].available))
    {
        /* Connection is already closed, clear and deregister the related contexts */
        /* The substate=0 and available = TRUE are already set to init values in bt_disconnect_cnf_hdlr() after DISC_CNF returned from SIM task */

        /*
         * Exception case: If the RFCOMM's channel is used and its state is in CON_PENDING
         * * Although SimServer is in CLOSED state and not notified, but the SIM_DeregisterServer() will be failed
         */

        if (SIM(simServer)[0].rfChannel.state == 0)
        {
            /* This case:  RFCOMM channel is in DLC_STATE_DISCONNECTED state */
	     SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
            bt_simap_deactivate_cnf = construct_local_para(sizeof(bt_simap_deactivate_cnf_struct), TD_UL);
            bt_simap_deactivate_cnf->result = TRUE;
#endif
	     parms.p.simap_deactivate_cnf.result = TRUE;
            SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;
            /* Report(("Recv Deactive_req, the RFCOMM and SimServer are already in CLOSED state")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RFCOMM_SIMSRV_BOTHCLOSED_RECV_DEACTIVE_REQ);

            if (SIM_DeregisterServer(&SIM(simServer)[0]) != BT_STATUS_SUCCESS)
            {
                Assert(0);
            }
            if (SIM(allocated_connect_id)[0] == KAL_TRUE)
            {
                BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
                SIM(allocated_connect_id)[0] = KAL_FALSE;
                /* Report(("Release SIM Access Profile connect_id in bt_simap_deactivate_req_hdlr")); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_IN_DEACTIVE_REQ_HDLR);

            }
            /* Send DEACTIVATE_CNF msg to APP/MMI */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DEACTIVATE_CNF,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_simap_deactivate_cnf,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else
        {
            BtStatus status = BT_STATUS_NO_CONNECTION;

            status = RF_CloseChannel(&(SIM(simServer)[0].rfChannel));
            if (status == BT_STATUS_PENDING)
            {

                /*
                 * Because the RFCOMM' server channel is disconnected when it is not transited to OPEN state,
                 * * RfDisconnectDLC()->RfSendCommand(DISC,..)->RfDlcStateDiscPending()->RfDisconnectL2CAP()->RfL2Callback()->RfMuxStateMachine()->RfFreeMux()
                 * * So, the RF_CloseChannel() is not generated the RFCLOSE_EVENT to SimServer
                 * * Return the Deactive CNF with OK here!
                 */
                SIM(simServer)[0].substate |= BT_APP_DEACTIVATE_REQ_RECV;
                SIM(simServer)[0].available = FALSE;

            }
			else if (status == BT_STATUS_SUCCESS)
			{
				OS_Report("[SIMAP][Handle RF_CloseChannel Return BT_STATUS_SUCCESS]---"); 
				SIMAPCallbackParms parms;
				
	     		parms.p.simap_deactivate_cnf.result = TRUE;
            	SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;

	            if (SIM_DeregisterServer(&SIM(simServer)[0]) != BT_STATUS_SUCCESS)
	            {
	                Assert(0);
	            }
	            if (SIM(allocated_connect_id)[0] == KAL_TRUE)
	            {
	                BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
	                SIM(allocated_connect_id)[0] = KAL_FALSE;
	                /* Report(("Release SIM Access Profile connect_id in bt_simap_deactivate_req_hdlr")); */
	                kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_IN_DEACTIVE_REQ_HDLR);

	            }
	            /* Send DEACTIVATE_CNF msg to APP/MMI */
			    parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
			    parms.dstMod = MOD_MMI;
			    SIMAPCB(&parms);
			}
            else
            {
                /*
                 * If status == BT_STATUS_IN_PROGRESS, this means that RFCOMM's state is in DISC_CON_PENDING,
                 * * In this case, SimServer is not possible in CLOSED state, so assert it
                 * * OR, if status == BT_STATUS_FAILED: assert it
                 */
                Assert(0);
            }
        }

    }
    else if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CLOSED) && (SIM(simServer)[0].substate == 0) &&
             (FALSE == SIM(simServer)[0].available))
    {
	/* 
	 * [MAUI_01811752]
	 * Bingyi 2009.12.16: use SIM_SERVER_STATUS_DEACTIVATING the following case that 
	 * 1. AUTH_REQ is rejected
	 * 2. DEACTIVATE is call by MMI
	 * 3. RFCOMM channel has not been closed
	 * ==> the DEACTIVATE will be delayed after RFCOMM channel has been closed.
	 */
	 SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATING;
    }
    else if (SIM(simServer)[0].state >= SIM_SERVER_STATE_CONN_PENDING)
    {
        /* SimServer's state >= CONNECT_PENDIND : close rfchannel */
        SIM(simServer)[0].substate |= BT_APP_DEACTIVATE_REQ_RECV;

    #ifdef __SIMAP_UT_

        /* Add this for private test : 2007-0118 */
        if (SIM(simServer)[0].exec_mode == 2)
        {
#ifndef __SIMAP_MAUI__
	     SIMAPCallbackParms parms;
#endif
            /* Simulate the SimServer in Open state and send disc req to SIM task */
            SIM(simServer)[0].state = SIM_SERVER_STATE_CLOSED;
            SIM(simServer)[0].substate |= BT_SIM_DISCONNECT_REQ_SENT;
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIM_DISCONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#else
	     parms.event = SIM_EVENT_DISCONNECT_REQ;
	     parms.dstMod = MOD_SIM;
	     SIMAPCB(&parms);
#endif
            return;

        }

    #endif /* __SIMAP_UT_ */ 

        /* Check if SimServer has sent DISC with Graceful type and its timer is still running or not */
        if (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV)
        {
            /* Report(("Recv Deactivate Req, check disc with graceful timer is running or not!")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RECV_DEACTIVE_CHECK_DISCGRACE_TIMER);
            bt_simap_stop_timer(&SIM(simap_event_id));
        }

	 /* Bingyi [MAUI_01817449]:  If in SIM_SERVER_STATE_DISC_PENDING state, do not call SIM_ServerClose() again. */
	 if (SIM_SERVER_STATE_DISC_PENDING == SIM(simServer)[0].state)
	 {
	 	kal_trace(TRACE_GROUP_1, BT_SIMAP_DO_NOTHING_IN_DISC_PENDING_STATE_IN_DEACTIVE_REQ);
	 	return ;
	 }
	 else if (SIM_SERVER_STATE_CONN_PENDING == SIM(simServer)[0].state)
	 {
	 	/* Server is waiting for ATR_REQ from Client side. Close RFCOMM channel first. */
	 	/* Similar case with ATR timeout in bt_simap_wait_atr_in_connect_procedure_timeout_handler() */
	 	kal_trace(TRACE_GROUP_1, BT_SIMAP_CLOSE_RFCOMM_IN_CONN_PENDING_STATE_IN_DEACTIVE_REQ);
	 	SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
	 	return ;
	 }

        /* The following is modified */
        status = SIM_ServerClose(&SIM(simServer)[0], SIM_DISCONNECT_IMMEDIATE);
        if (SIM(simServer)[0].state == SIM_SERVER_STATE_OPEN)
        {
            /* Report(("Using SimServerSendDiscInd() first to do deactive request")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_USE_DISCIND_FOR_DEACTIVE_REQ);
            /* Using SimServerSendDiscInd():to send DISC with immediate type to SimClient to close this connection */

            if (status != BT_STATUS_PENDING)
            {
                Assert(0);
            }
            /* If status == BT_STATUS_PENDING: the DEACTIVATE_CNF will be pended util the DISC_CNF from SIM task! */
        }
        else
        {
            /* Report(("Using RF_CloseChannel() first to do deactive request")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_USE_RFCLOSECHNL_FOR_DEACTIVE_REQ);
            /* Using RF_CloseChannel() to close this connection */
            if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS))
            {
                //Report(("In bt_simap_deactiveate_req_hdlr(): RF_CloseChannel failed, status=%d",status));
                //kal_trace(TRACE_GROUP_1,BT_SIMAP_DEACTIVE_REQ_HDLR_RFCLOSECHNL_FAILED,status);
                kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_STATUS, status);
                Assert(0);
            }

            /* If status == BT_STATUS_PENDING or status == BT_STATUS_IN_PROGRESS: the DEACTIVATE_CNF will be pended util the DISC_CNF from SIM task! */
        }

    }
    else
    {
        Assert(0);
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_auth_rsp_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
#if 0
void bt_simap_auth_rsp_hdlr(ilm_struct *ilm_ptr)
{
    bt_simap_auth_rsp_struct *bt_simap_auth_rsp = (bt_simap_auth_rsp_struct*) ilm_ptr->local_para_ptr;
    SimConnStatus connStatus;
    U8 maxMsgSize[2];
    BtStatus status;

    /* In this case: it means that sim access profile session was already closed before this auth_rsp is sent from MMI via user's confirmation */
    if ((SIM(simServer)[0].substate == 0) && (SIM(simServer)[0].available == TRUE))
    {
		Report(("bt_simap_auth_rsp_hdlr: SIMAP session maybe already CLOSED! SIM(simServer)[0].substate=%d", SIM(simServer)[0].substate));
        return;
    }
    else if ((SIM(simServer)[0].substate & BT_MMI_AUTH_REQ_SENT) != BT_MMI_AUTH_REQ_SENT)
    {
		Report(("bt_simap_auth_rsp_hdlr: NOT BT_MMI_AUTH_REQ_SENT! SIM(simServer)[0].substate=%d", SIM(simServer)[0].substate));
#ifdef __SIMAP_MAUI__
        Assert(0);
#else
	 return ;
#endif
    }

    SIM(simServer)[0].substate &= (~BT_MMI_AUTH_REQ_SENT);
    /* Report(("bt_simap_auth_rsp->result =%d",bt_simap_auth_rsp->result)); */

    if (bt_simap_auth_rsp->result == KAL_TRUE)
    {
        /* Report(("MMI accepts this SAP connection in bt_simap_auth_rsp_hdlr")); */

        /*
         * Set the message size for the connection. 
         * * In this case: this MaxMsgSize is accepted by SIM Server 
         */

        /* kal_trace(TRACE_GROUP_1,BT_SIMAP_RCV_CON_REQ_IN_CLOSE,SIM(simServer)[0].simPacket.maxMsgSize); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_CON_REQ_IN_CLOSE, SIM(simServer)[0].simPacketList[0].maxMsgSize);

        SIM(simServer)[0].state = SIM_SERVER_STATE_CONN_PENDING;

        /* 2006-10-18: Send BT_SIM_CONNECT_REQ msg to SIM task to notify it that SIM Acces Profile connection is incoming */
        SIM(simServer)[0].substate |= BT_SIM_CONNECT_REQ_SENT;

        if (SIM(simServer)[0].exec_mode == 0)
        {
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIM_CONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#else
	     SIMAPCallbackParms parms;
	     parms.event = SIM_EVENT_CONNECT_REQ;
	     parms.dstMod = MOD_SIM;
	     SIMAPCB(&parms);
#endif
        }
    #ifdef __SIMAP_UT_
        else if (SIM(simServer)[0].exec_mode == 1)
        {
            bt_fakesim_connect_req_hdlr();
        }
    #endif /* __SIMAP_UT_ */ 

    }
    else
    {
        SimServer *Server = &(SIM(simServer)[0]);

        /* Report(("MMI rejects this SAP connection in bt_simap_auth_rsp_hdlr")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_MMI_REJECT_SAP_CON_IN_AUTH_RSP_HDLR);

        /* if SAP connection is rejected by MMI, reset maxMsgSize to our default value */
        SIM(simServer)[0].simPacket.maxMsgSize = SIM_MAX_MSG_SIZE;
        SIM(simServer)[0].discPacket.maxMsgSize = SIM_MAX_MSG_SIZE;
        SIM(simServer)[0].discGracePacket.maxMsgSize = SIM_MAX_MSG_SIZE;

        /* Add of this on 2007-0117 */
        SetPacketListMsgSize(&SIM(simServer)[0], SIM_MAX_MSG_SIZE);

        /* Send Connect_RSP with error to SIM Client! */
        connStatus = SIM_CONN_STATUS_SERVER_UNABLE_ESTABLISH_CONNECTION;
        StoreBE16(maxMsgSize, SIM_MAX_MSG_SIZE);
        /* When simServer is serving one client, simServer->simPacket has already maxMsgSize value */
        status = SimServerSendConnRspErr(Server, &connStatus, maxMsgSize);
        if (status != BT_STATUS_PENDING)
        {
            //Report(("Status of SimServerSendConnRspErr =%d",status));
            //kal_trace(TRACE_GROUP_1,BT_SIMAP_CONRSPERR_STATUS,status);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_STATUS, status);
            Assert(0);
        }
    }

}
#else  //Yufeng
void bt_simap_auth_rsp_hdlr(ilm_struct *ilm_ptr)
{
    bt_simap_auth_rsp_struct *bt_simap_auth_rsp = (bt_simap_auth_rsp_struct*) ilm_ptr->local_para_ptr;
	
    /* In this case: it means that sim access profile session was already closed before this auth_rsp is sent from MMI via user's confirmation */
	if ((SIM(simServer)[0].substate & BT_MMI_AUTH_REQ_SENT) != BT_MMI_AUTH_REQ_SENT)
	{
		Report(("bt_simap_auth_rsp_hdlr: NOT BT_MMI_AUTH_REQ_SENT! SIM(simServer)[0].substate=%d", SIM(simServer)[0].substate));
	 	return ;
	}

    SIM(simServer)[0].substate &= (~BT_MMI_AUTH_REQ_SENT);

    if (bt_simap_auth_rsp->result == KAL_TRUE)
    {
        /* Report(("MMI accepts this SAP connection in bt_simap_auth_rsp_hdlr")); */
		SIMAP_AcceptIncomingRequest();
    }
	else
	{
		SIMAP_RejectIncomingRequest();
	}
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_simap_disconnect_req_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_disconnect_req_hdlr(ilm_struct *ilm_ptr)
{
    bt_simap_disconnect_req_struct *bt_simap_disconnect_req = (bt_simap_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    
    /* Receiving the disconnect request from MMI: The default type is graceful disconnection */
    if (SIM(simServer)[0].state == SIM_SERVER_STATE_OPEN)
    {
        BtStatus status;

	 if (SIM_DISCONNECT_GRACEFUL == bt_simap_disconnect_req->discType)
	 {
	 	SimSrvSetGracefulDiscType();
	 }
	 else if (SIM_DISCONNECT_IMMEDIATE == bt_simap_disconnect_req->discType)
	 {
	 	SimSrvSetImmediateDiscType();
	 }

        /* This means that the interactions between sim Client and SIM cards may already be going. Using graceful disconnection */
        SIM(simServer)[0].substate |= BT_APP_DISCONNECT_REQ_RECV;

    #ifdef __SIMAP_UT_

        /* Add this for private Test :2007-0118 */
        if (SIM(simServer)[0].exec_mode == 2)
        {
#ifndef __SIMAP_MAUI__
	     SIMAPCallbackParms parms;
#endif
            SIM(simServer)[0].state = SIM_SERVER_STATE_CLOSED;
            SIM(simServer)[0].substate |= BT_SIM_DISCONNECT_REQ_SENT;
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIM_DISCONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#else
	     parms.event = SIM_EVENT_DISCONNECT_REQ;
	     parms.dstMod = MOD_SIM;
	     SIMAPCB(&parms);
#endif
            return;
        }

    #endif /* __SIMAP_UT_ */ 
        kal_trace(TRACE_GROUP_1,BT_SIMAP_CURRENT_DISC_TYPE,SIM(simServer)[0].discType);
        /* Send DISCONNECT_REQ packet to sim client: Because server in open state, using SIM_ServerClose() to disc */
        if ((status = SIM_ServerClose(&SIM(simServer)[0], SIM(simServer)[0].discType)) != BT_STATUS_PENDING)
        {
            kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_CLOSE_CON_WITH_TYPE, status, SIM(simServer)[0].discType);
            /* If SIM_ServerClose() failed, try RF_CloseChannel() */
            SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));

        }
        else
        {
            /* kal_trace(TRACE_GROUP_1,BT_SIMAP_CLOSE_CON_WITH_GRACEFUL); */
            Report(("Recv SIMAP_DISC_REQ, using Immdediaye Disc Type"));
            /* Start a timer to avoid the client from sending nothing and occupying server's resource */

        }

    }
    else if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CONN_PENDING) &&
             (SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT))
    {
        /*
         * In SIM_SERVER_STATE_CONN_PENDING: 
         * * Because server is not in open state, so it is not allowed to call SIM_ServerClose() even with Immd Disconnection type 
         */
        SIM(simServer)[0].substate |= BT_APP_DISCONNECT_REQ_RECV;

    }
    else if (SIM(simServer)[0].state == SIM_SERVER_STATE_CONN_PENDING)
    {
        /* Report(("In bt_simap_disconnect_req_hdlr():Error! MMI has no entry to disconnect this SAP")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_REQ_HDLR_NO_ENTRY_TO_DISC);
        Assert(0);
    }
    else if (SIM(simServer)[0].state == SIM_SERVER_STATE_DISC_PENDING)
    {
        SIM(simServer)[0].substate |= BT_APP_DISCONNECT_REQ_RECV;
        /* When this connection is closed, it will return DISC_CNF msg back to MMI */
        /* Report(("In bt_simap_disconnect_req_hdlr():Already disc pending, ignore this request")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_REQ_HDLR_ALREADY_DISC_PENDING);

    }
    else if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CLOSED) &&
             (SIM(simServer)[0].substate & BT_SIM_DISCONNECT_REQ_SENT))
    {
        /*
         * The scenario of this case is described as follows:
         * * 1. In SimServer's open state: receiving a DISC Req from client
         * * 2. SIM Access Profile is disconnected, and chages its state to CLOSED. So DISC_Req is sent to SIM task
         * * 3. At this time, user presses DISC button: MMI sends this DISC_Req to BT
         */
        SIM(simServer)[0].substate |= BT_APP_DISCONNECT_REQ_RECV;
        /* Report(("Recv Disc_req from MMI, although SimServer is closed, but it waits for SIM task's DISC_CNF")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_RECV_DISC_REQ_SIMSRV_CLOSED_WAIT_DISCCNF);
    }

    else
    {
	 SIMAPCallbackParms parms;

        kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSERVER_STATE, SIM(simServer)[0].state);
#ifdef __SIMAP_MAUI__
        Assert(0);
#endif
	 parms.event = SIMAP_EVENT_DISCONNECT_CNF;
	 parms.dstMod = MOD_SIM;
	 parms.p.simap_disconnect_cnf.result = FALSE;
	 SIMAPCB(&parms);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_connect_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_connect_cnf_hdlr(ilm_struct *ilm_ptr)
{

    bt_sim_connect_cnf_struct *bt_sim_connect_cnf = (bt_sim_connect_cnf_struct*) ilm_ptr->local_para_ptr;

    kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSERVER_STATE_SUBSTATE, SIM(simServer)[0].state, SIM(simServer)[0].substate);
    /* Only one SIM Card ID-0 is used */
    if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CONN_PENDING) &&
        (SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT) &&
        (!(SIM(simServer)[0].substate & BT_SIM_CONNECT_CNF_RECV)))
    {
        /* 2006-10-19: check result value with Nick, temporary value is used here */
        if (bt_sim_connect_cnf->result == SIM_RESULT_OK)
        {

            SIM(simServer)[0].substate |= BT_SIM_CONNECT_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_REQ_SENT;
            /* Record the related information: current_transport_protocol_type and atr from this BT_SIM_CONNECT_CNF */
            SIM(simServer)[0].current_transport_protocol_type = bt_sim_connect_cnf->current_transport_protocol_type;
            SIM(simServer)[0].supported_transport_protocol_type_capability =
                bt_sim_connect_cnf->supported_transport_protocol_type_capability;
            SIM(simServer)[0].atr.dataLen = bt_sim_connect_cnf->atr_len;
            OS_MemSet(SIM(simServer)[0].atr.data, 0, SIM_MAX_ATR);
            OS_MemCopy(SIM(simServer)[0].atr.data, bt_sim_connect_cnf->atr, SIM(simServer)[0].atr.dataLen);

            /* From the result field, to update current_simcardreader_status */
            SIM(simServer)[0].current_simcardreader_status |= (SIM_CRS_CARD_POWERED_ON | SIM_CRS_CARD_PRESENT);

            /* Enter to RESET_CNF stage successfully: 2006-11-29 modify */
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_PROCEDURE_COMPLETED;

        #ifdef __SIMAP_UT_
            /* Report(("SimServer's test mode=%d",SIM(simServer)[0].exec_mode)); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRV_MODE, SIM(simServer)[0].exec_mode);

            /* Add this for private test : 2008-0118 */
            if (SIM(simServer)[0].exec_mode == 2)
            {
                SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
                bt_simap_connect_ind_struct *bt_simap_connect_ind = NULL;
		  bt_simap_connect_ind = construct_local_para(sizeof(bt_simap_connect_ind_struct), TD_UL);
#endif

                SIM(simServer)[0].state = SIM_SERVER_STATE_OPEN;

                /* Allocated a connection id for SIM Access Profile (SC_SIM_ACCESS: 0x112D) */
                SIM(connect_id)[0] = 255;
                SIM(allocated_connect_id)[0] = KAL_TRUE;

#if 0 // Reserved for MAUI
                /* Reset lap,uap,nap to zero */
                bt_simap_connect_ind->lap = 0;
                bt_simap_connect_ind->uap = 0;
                bt_simap_connect_ind->nap = 0;

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
		  /* No BD_ADDR ??? */
		  SIMAPCB(&parms);
                return;
            }
        #endif /* __SIMAP_UT_ */ 
            /* Only RESET or CARD_NOT_ACCESSIBLE case can report to upper layer */
            SimServerAppCallback(
                &SIM(simServer)[0],
                SAEVENT_SERVER_OPEN_IND,
                BT_STATUS_SUCCESS,
                (&SIM(simServer)[0])->remDev,
                sizeof(BtRemoteDevice));

        }
        else
        {
            SimConnStatus connStatus;
            SimServer *Server = (&SIM(simServer)[0]);
            U8 maxMsgSize[2];
            BtStatus status;
            SIMAPCallbackParms parms;

            kal_trace(TRACE_GROUP_1, BT_SIMAP_CON_CNF_ERROR_FROM_SIM);

        #ifdef __SIMAP_UT_

            /* Add this for private test : 2007-0118 */
            if (SIM(simServer)[0].exec_mode == 2)
            {
                Report(("In unit test mode: Check the sim card is inserted"));
                SIM(simServer)[0].state = 0;
                SIM(simServer)[0].substate = 0;
                SIM(simServer)[0].available = TRUE;
                return;
            }
        #endif /* __SIMAP_UT_ */ 

            SIM(simServer)[0].substate |= BT_SIM_CONNECT_CNF_RECV;
            SIM(simServer)[0].atr.dataLen = 0;  /* This means that CNF from SIM task is error */

            /* Can not Reset substate to zero here! Must left to close event for SIMAP reported to upper layer */
            /* Send Connect_RSP with error to SIM Client! */
            connStatus = SIM_CONN_STATUS_SERVER_UNABLE_ESTABLISH_CONNECTION;
            /* StoreBE16(maxMsgSize, SIM(simServer)[0].simPacket.maxMsgSize); */
            StoreBE16(maxMsgSize, SIM(simServer)[0].simPacketList[0].maxMsgSize);
            status = SimServerSendConnRspErr(Server, &connStatus, maxMsgSize);
            if (status != BT_STATUS_PENDING)
            {
                //Report(("SimServerSendConnRspErr() failed, status=%d",status));
                //kal_trace(TRACE_GROUP_1,BT_SIMAP_CONRSPERR_STATUS,status);
                kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_STATUS, status);
                Assert(0);
            }

            /* Bingyi 2010.01.06.: Report that connection could not be made with SIM task to MMI (sync context flag and state). */
	    parms.event = SIMAP_EVENT_SIM_CONNECT_FAILED_IND;
	    parms.dstMod = MOD_MMI;
	    SIMAPCB(&parms);
        }
    }
    else if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CLOSED) &&
             (SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT) &&
             (!(SIM(simServer)[0].substate & BT_SIM_CONNECT_CNF_RECV)) &&
             (SIM(simServer)[0].substate & BT_SIM_DISCONNECT_REQ_SENT))

    {
        /*
         * This scenario of this case is described as follows:
         * * 1. Car Kit connects to our SimServer and CONNECT_REQ is sent to SIM task
         * * 2. MTK handset receives LINK  DISCONNECT Event
         * * 3. SIM Access Profile is disconnected (SimServer is changed to CLOSED state) , so DISC_REQ is sent to SIM
         * * 4. At this time, CONNECT_CNF is received.
         */

        if (bt_sim_connect_cnf->result == SIM_RESULT_OK)
        {
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_REQ_SENT;
            /* Enter to RESET_CNF stage successfully: 2006-12-28 modify */
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_PROCEDURE_COMPLETED;
        }
        else
        {
            Assert(0);
        }

    }
    else if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CLOSED) &&
             (SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT) &&
             (!(SIM(simServer)[0].substate & BT_SIM_CONNECT_CNF_RECV)) &&
             (SIM(simServer)[0].substate & BT_SIM_DISCONNECT_REQ_SENT) &&
             (SIM(simServer)[0].substate & BT_APP_DEACTIVATE_REQ_RECV))
    {
        /*
         * The scenario of this case is described as follows:
         * * 1. Car Kit connects to our SimServer and CONNECT_REQ is sent to SIM task
         * * 2. MMI press POWER OFF BT, so disconnect all link 
         * * 3. SIM Access Profile is disconnected (SimServer is changed to CLOSED state) , so DISC_REQ is sent to SIM
         * * 4. At this time, CONNECT_CNF is received.
         */

        /*
         * Another scenario : 2006-12-28
         * * 1. Car Kit sends CON_REQ, SimServer accepts it.
         * * 2. SimServer sends CON_REQ to SIM task
         * * 3. At this time, MMI sends DEACTIVATE_REQ to BT SimServer
         * * 4. Using RF_CloseChannel() to let SimServer to be in CLOSED state, so DISC_REQ sends to SIM task
         * * 5. CON_CNF with ok is returned from SIM task
         * * 6. Later, SimServer will receive DISC_CNF with ok from SIM task
         */

        if (bt_sim_connect_cnf->result == SIM_RESULT_OK)
        {
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_REQ_SENT;
            /* Enter to RESET_CNF stage successfully: 2006-12-28 modify */
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_PROCEDURE_COMPLETED;
        }
        else
        {
            Assert(0);
        }

    }
    else
    {
        /* Report(("In bt_sim_connect_cnf_hdlr():Not Expect to receive BT_SIM_CONNECT_CNF")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_CON_CNF_HDLR_NOT_EXPECT_SIM_CON_CNF);
        Assert(0);
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_openstate_reset_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_openstate_reset_cnf_hdlr(ilm_struct *ilm_ptr)
{
    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = (bt_sim_reset_cnf_struct*) ilm_ptr->local_para_ptr;

    SimResultCode result = SIM_RESULT_NO_REASON;
    BtStatus status = BT_STATUS_FAILED;

    if (bt_sim_reset_cnf->result == SIM_RESULT_OK)
    {
        /*
         * If this RESET_RSP is due to RESET_REQ/POWER ON, T must be zero (T=0)
         * * else if this rsp is due to "Set Transport Protocol", T may be zero or one !
         */

        SIM(simServer)[0].current_transport_protocol_type = bt_sim_reset_cnf->current_transport_protocol_type;

        /* Update ATR rsp sent from SIM Task module */
        SIM(simServer)[0].atr.dataLen = bt_sim_reset_cnf->atr_len;
        OS_MemSet(SIM(simServer)[0].atr.data, 0, SIM_MAX_ATR);
        OS_MemCopy(SIM(simServer)[0].atr.data, bt_sim_reset_cnf->atr, SIM(simServer)[0].atr.dataLen);

        /* Determine if this is CARD_NOT_ACCESSIBLE case or not */
        if (bt_sim_reset_cnf->current_transport_protocol_type == 1)
        {
            result = SIM_RESULT_CARD_NOT_ACCESSIBLE;
        }
        else
        {
            result = SIM_RESULT_OK;
        }
        /* Send SIM_RESET_RSP with OK to SIM client */

    }
    else if (bt_sim_reset_cnf->result == SIM_RESULT_CARD_ALREADY_OFF)
    {
        /* In this case: BT maintains cardstatus is on, so RESET_REQ is sent to SIM task, but card is off by other module */
        result = SIM_RESULT_CARD_ALREADY_OFF;

        SIM(simServer)[0].current_simcardreader_status &= (~SIM_CRS_CARD_POWERED_ON);
        /* Because the result is not 0x00, clear this flag BT_SIM_RESET_REQ_SENT here */
        SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT);
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_CNF_ERROR_FROM_SIM, bt_sim_reset_cnf->result);
        /* According result to set error code: Send SIM_RESET_RSP with error code to SIM client */
        /* using bt_sim_reset_cnf->result to determine how to process: Need to discuss with Nick */
        /* From the result field, to update current_simcardreader_status */

        /* For our device now, it is not possible to receive CRAD_REMOVED in open state. */

        if (bt_sim_reset_cnf->result == SIM_RESULT_NO_REASON)
        {
            Assert(0);
        }

        if (bt_sim_reset_cnf->result == SIM_RESULT_NO_REASON)
        {
            result = SIM_RESULT_NO_REASON;
        }
        else
        {
            result = SIM_RESULT_NO_REASON;
        }

        /* Because the result is not 0x00, clear this flag BT_SIM_RESET_REQ_SENT here */
        SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT);
    }

    /* Send ResetRsp to Sim client */
    if ((status = SIM_ServerResetSimRsp(&SIM(simServer)[0], result)) != BT_STATUS_SUCCESS)
    {
        /* Because sent err is happen, also clear this flag BT_SIM_RESET_REQ_SENT here */
        if (result == SIM_RESULT_OK)
        {
            SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT);
        }

        /* Because the conenction is closed before RESET_RSP returned from SIM, so SIM_ServerResetSimRsp() failed is ok */
        if (SIM(simServer)[0].rfChannel.state == 0)
        {
            //Report(("RFChannel is closed, so SIM_ServerResetSimRsp() failed is ok"));
            //kal_trace(TRACE_GROUP_1,BT_SIMAP_RFCHNL_CLOSED_RESET_SIM_RSP_FAILED_OK);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RFCHNLCLOSED_SEND_RSP_FAILED_OK);
            return;
        }

        SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));

    }

    kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_RSP_TO_CLIENT, result, status);

}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_openstate_power_on_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_openstate_power_on_cnf_hdlr(ilm_struct *ilm_ptr)
{
    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = (bt_sim_reset_cnf_struct*) ilm_ptr->local_para_ptr;

    SimResultCode result = SIM_RESULT_NO_REASON;
    BtStatus status = BT_STATUS_FAILED;

    if (bt_sim_reset_cnf->result == SIM_RESULT_OK)
    {
        /*
         * If this RESET_RSP is due to RESET_REQ/POWER ON, T must be zero (T=0)
         * * else if this rsp is due to "Set Transport Protocol", T may be zero or one !
         */

        SIM(simServer)[0].current_transport_protocol_type = bt_sim_reset_cnf->current_transport_protocol_type;

        /* Update ATR rsp sent from SIM Task module */
        SIM(simServer)[0].atr.dataLen = bt_sim_reset_cnf->atr_len;
        OS_MemSet(SIM(simServer)[0].atr.data, 0, SIM_MAX_ATR);
        OS_MemCopy(SIM(simServer)[0].atr.data, bt_sim_reset_cnf->atr, SIM(simServer)[0].atr.dataLen);

        /* Determine if this is CARD_NOT_ACCESSIBLE case or not */
        if (bt_sim_reset_cnf->current_transport_protocol_type == 1)
        {
            result = SIM_RESULT_CARD_NOT_ACCESSIBLE;
        }
        else
        {
            result = SIM_RESULT_OK;
        }

        /* Send SIM_RESET_RSP with OK to SIM client */

    }
    else if (bt_sim_reset_cnf->result == SIM_RESULT_CARD_ALREADY_ON)
    {
        /* In this case: BT maintains cardstatus is off, so POWER_ON_REQ is still sent to SIM task, but card is powered on by other module */
        SIM(simServer)[0].current_simcardreader_status |= SIM_CRS_CARD_POWERED_ON;
        /* Because RESULT is not 0x00, so clear the flag BT_SIM_RESET_REQ_SENT_POWER_ON :2006-12-04 */
        SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT_POWER_ON);
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_POWER_ON_ERROR_FROM_SIM, bt_sim_reset_cnf->result);
        /* According result to set error code: Send SIM_RESET_RSP with error code to SIM client */
        /* using bt_sim_reset_cnf->result to determine how to process: Need to discuss with Nick */
        /* From the result field, to update current_simcardreader_status */

        /* For our device now, it is not possible to receive CRAD_REMOVED in open state. */

        if (bt_sim_reset_cnf->result == SIM_RESULT_NO_REASON)
        {
            Assert(0);
        }

        if (bt_sim_reset_cnf->result == SIM_RESULT_NO_REASON)
        {
            result = SIM_RESULT_NO_REASON;
        }
        else
        {
            result = SIM_RESULT_NO_REASON;
        }

        /* Because RESULT is not 0x00, so clear the flag BT_SIM_RESET_REQ_SENT_POWER_ON :2006-12-04 */
        SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT_POWER_ON);
    }

    /* Send SimOnRsp to Sim client */
    if ((status = SIM_ServerSimOnRsp(&SIM(simServer)[0], result)) != BT_STATUS_SUCCESS)
    {
        /* Report(("In bt_sim_reset_cnf_hdlr(): SIM_SERVER_STATE_OPEN:Cound not send SIM POWER ON response, status:%d",status)); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_OPENSTATE_SEND_POWERON_RSP_FAILED_STATUS, status);

        /* Because sent err for 0x00, also clear this flag BT_SIM_RESET_REQ_SENT_POWER_ON */
        if (result == SIM_RESULT_OK)
        {
            SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT_POWER_ON);
        }

        /* Because the conenction is closed before POWERON_RSP returned from SIM, so SIM_ServerSimOnRsp() failed is ok */
        if (SIM(simServer)[0].rfChannel.state == 0)
        {
            //Report(("RFChannel is closed, so SIM_ServerSimOnRsp() failed is ok"));
            //kal_trace(TRACE_GROUP_1,BT_SIMAP_RFCHNLCLOSED_SEND_SIMONRSP_FAILED_OK);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RFCHNLCLOSED_SEND_RSP_FAILED_OK);
            return;
        }

        SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_POWER_ON_RSP_TO_CLIENT, result);
        if (result == SIM_RESULT_OK)
        {
            /* Updata current_simcardreader_status */
            /* RESET_REQ sent to SIM task due to POWER_ON: need to update status from POWER_OFF to POWER_ON! */
            /* When SimPacket is handled in Open state, the flag BT_SIM_RESET_REQ_SENT_POWER_ON is cleared */
            if (!(SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON))
            {
                SIM(simServer)[0].current_simcardreader_status |= SIM_CRS_CARD_POWERED_ON;
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_openstate_set_transport_protocol_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_openstate_set_transport_protocol_cnf_hdlr(ilm_struct *ilm_ptr)
{
    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = (bt_sim_reset_cnf_struct*) ilm_ptr->local_para_ptr;

    SimResultCode result = SIM_RESULT_NO_REASON;
    BtStatus status = BT_STATUS_FAILED;

    if (bt_sim_reset_cnf->result == SIM_RESULT_OK)
    {
        /*
         * If this RESET_RSP is due to RESET_REQ/POWER ON, T must be zero (T=0)
         * * else if this rsp is due to "Set Transport Protocol", T may be zero or one !
         */

        SIM(simServer)[0].current_transport_protocol_type = bt_sim_reset_cnf->current_transport_protocol_type;

        /* Update ATR rsp sent from SIM Task module */
        SIM(simServer)[0].atr.dataLen = bt_sim_reset_cnf->atr_len;
        OS_MemSet(SIM(simServer)[0].atr.data, 0, SIM_MAX_ATR);
        OS_MemCopy(SIM(simServer)[0].atr.data, bt_sim_reset_cnf->atr, SIM(simServer)[0].atr.dataLen);

        /* Send SIM_RESET_RSP with OK to SIM client */
        result = SIM_RESULT_OK;
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SET_T_ERROR_FROM_SIM, bt_sim_reset_cnf->result);
        /* According result to set error code: Send SIM_RESET_RSP with error code to SIM client */
        /* using bt_sim_reset_cnf->result to determine how to process: Need to discuss with Nick */
        /* From the result field, to update current_simcardreader_status */

        if ((bt_sim_reset_cnf->result == SIM_RESULT_CARD_REMOVED) ||
            (bt_sim_reset_cnf->result == SIM_RESULT_CARD_NOT_ACCESSIBLE))
        {
            Assert(0);
        }

        if (bt_sim_reset_cnf->result == SIM_RESULT_NO_REASON)
        {
            result = SIM_RESULT_NO_REASON;
        }
        else
        {
            result = SIM_RESULT_NO_REASON;
        }

        /* Because the result is not 0x00, so clear the flag BT_SIM_RESET_REQ_SENT_SET_T here */
        SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT_SET_T);
    }

    /* Send SetTransProtRsp to Sim client */
    if ((status = SIM_ServerSetTransProtRsp(&SIM(simServer)[0], result)) != BT_STATUS_SUCCESS)
    {

        /* Report(("In bt_sim_reset_cnf_hdlr(): SIM_SERVER_STATE_OPEN:Cound not send SIM Set Transport Protocol Type response, status:%d",status)); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_OPENSTATE_SETT_CNF_HDLR_STATUS, status);
        /* Becaus sent err is happen, also clear this flag BT_SIM_RESET_REQ_SENT_SET_T */
        SIM(simServer)[0].substate &= (~BT_SIM_RESET_REQ_SENT_SET_T);

        /* Because the conenction is closed before SetT_RSP returned from SIM, so SIM_ServerSetTransProtRsp() failed is ok */
        if (SIM(simServer)[0].rfChannel.state == 0)
        {
            //Report(("RFChannel is closed, so SIM_ServerSetTransProtRsp() failed is ok"));
            //kal_trace(TRACE_GROUP_1,BT_SIMAP_RFCHNLCLOSED_SEND_SETT_RSP_FAILED_OK);         
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RFCHNLCLOSED_SEND_RSP_FAILED_OK);
            return;
        }

        SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_SET_T_RSP_TO_CLIENT, result);
        /* RESET_REQ sent to SIM task due to SET_TRANSPORT_PROTOCOL_TYPE: already powered on! */
        /* if status is changed, STATUS_IND is needed be sent to client ! */

        /* When SimPacket is handled in Open State for 0x00, the flag BT_SIM_RESET_REQ_SENT_SET_T is cleared. */
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_reset_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]   
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_reset_cnf_hdlr(ilm_struct *ilm_ptr)
{

    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = (bt_sim_reset_cnf_struct*) ilm_ptr->local_para_ptr;

    /* Only one SIM Card ID-0 is used */
    if (SIM(simServer)[0].state == SIM_SERVER_STATE_CONN_PENDING)
    {
        if ((SIM(simServer)[0].substate & BT_SIM_CONNECT_CNF_RECV) &&
            (SIM(simServer)[0].substate & BT_SIM_CONNECT_RESET_REQ_SENT) &&
            (!(SIM(simServer)[0].substate & BT_SIM_CONNECT_RESET_CNF_RECV)))
        {
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_RESET_CNF_RECV;
            SIM(simServer)[0].substate |= BT_SIM_CONNECT_PROCEDURE_COMPLETED;

            /* Need to check with Nick : It must retrieve CardReaderStatus from result */
            if (bt_sim_reset_cnf->result == SIM_RESULT_OK)
            {
                /* Record the related information from the BT_SIM_RESET_CNF */
                SIM(simServer)[0].atr.dataLen = bt_sim_reset_cnf->atr_len;
                OS_MemSet(SIM(simServer)[0].atr.data, 0, SIM_MAX_ATR);
                OS_MemCopy(SIM(simServer)[0].atr.data, bt_sim_reset_cnf->atr, SIM(simServer)[0].atr.dataLen);

                /*
                 * Need to record the transport protocol types supported by the SIM Card
                 * * Although the result is ok, but this only means that driver sets SIM card on T=0 or T=1 successfully
                 * * It is not promised that T=0 is used for this SIM card now!
                 */
                /* Report(("In bt_sim_reset_cnf_hdlr(), CT=%d",bt_sim_reset_cnf->current_transport_protocol_type)); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_CNF_HDLR_CT, bt_sim_reset_cnf->current_transport_protocol_type);
                SIM(simServer)[0].current_transport_protocol_type = bt_sim_reset_cnf->current_transport_protocol_type;

                /* From the result field, to update current_simcardreader_status */
                SIM(simServer)[0].current_simcardreader_status |= (SIM_CRS_CARD_POWERED_ON | SIM_CRS_CARD_PRESENT);

                /* Only RESET or CARD_NOT_ACCESSIBLE case can report to upper layer */
                SimServerAppCallback(
                    &SIM(simServer)[0],
                    SAEVENT_SERVER_OPEN_IND,
                    BT_STATUS_SUCCESS,
                    (&SIM(simServer)[0])->remDev,
                    sizeof(BtRemoteDevice));
            }

        #ifdef __SIM_SUPPORT_RETURN_CARD_NOT_ACCESSIBLE__
            else if (bt_sim_reset_cnf->result == SIM_RESULT_CARD_NOT_ACCESSIBLE)
            {
                /* 2006-11-29 discuss with Nick : SIM task does not return this result code */
                // Notify upper layer to send STATUS_IND with card not accessible
                // Record the related information from the BT_SIM_RESET_CNF
                SIM(simServer)[0].atr.dataLen = bt_sim_reset_cnf->atr_len;
                OS_MemSet(SIM(simServer)[0].atr.data, 0, SIM_MAX_ATR);
                OS_MemCopy(SIM(simServer)[0].atr.data, bt_sim_reset_cnf->atr, SIM(simServer)[0].atr.dataLen);

                /* Need to record the transport protocol types supported by the SIM Card? */
                SIM(simServer)[0].current_transport_protocol_type = 1;  /* using T=1: because Card not accessible, so T=1 is used instead the default T=0 */

                /* From the result field, to update current_simcardreader_status */
                SIM(simServer)[0].current_simcardreader_status |= (SIM_CRS_CARD_POWERED_ON | SIM_CRS_CARD_PRESENT);

                SimServerAppCallback(
                    &SIM(simServer)[0],
                    SAEVENT_SERVER_OPEN_IND,
                    BT_STATUS_SUCCESS,
                    (&SIM(simServer)[0])->remDev,
                    sizeof(BtRemoteDevice));
            }
        #endif /* __SIM_SUPPORT_RETURN_CARD_NOT_ACCESSIBLE__ */ 

            else if ((bt_sim_reset_cnf->result == SIM_RESULT_NO_REASON) ||
                     (bt_sim_reset_cnf->result == SIM_RESULT_CARD_REMOVED))
            {
                SimConnStatus connStatus;
                U8 maxMsgSize[2];
                BtStatus status;
                SimServer *Server = &SIM(simServer)[0];

                /* From the result field, to update current_simcardreader_status */

                if (bt_sim_reset_cnf->result == SIM_RESULT_CARD_REMOVED)
                {
                    SIM(simServer)[0].current_simcardreader_status &= (~SIM_CRS_CARD_POWERED_ON);
                    SIM(simServer)[0].current_simcardreader_status &= (~SIM_CRS_CARD_PRESENT);
                }
                else
                {
                    /* Not reset successfully */
                    SIM(simServer)[0].current_simcardreader_status &= (~SIM_CRS_CARD_POWERED_ON);
                    SIM(simServer)[0].current_simcardreader_status |= SIM_CRS_CARD_PRESENT;
                }
                kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_CNF_ERR_RFOM_SIM_IN_CONPENDING, bt_sim_reset_cnf->result);
                /* Reset substate to zero here */
                SIM(simServer)[0].substate = 0;
                /* Send Connect_RSP with error to SIM Client! */
                connStatus = SIM_CONN_STATUS_SERVER_UNABLE_ESTABLISH_CONNECTION;
                /* StoreBE16(maxMsgSize, SIM(simServer)[0].simPacket.maxMsgSize); */
                StoreBE16(maxMsgSize, SIM(simServer)[0].simPacketList[0].maxMsgSize);
                status = SimServerSendConnRspErr(Server, &connStatus, maxMsgSize);
                if (status != BT_STATUS_PENDING)
                {
                    /* Report(("In bt_sim_reset_cnf_hdlr: SimServerSendConnRspErr failed, status=%d",status)); */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_STATUS, status);
                    Assert(0);
                }

            }
            else
            {
                Assert(0);
            }

        }
        else
        {
            /* Report(("Error substate=%d of CONNECT_PENDING in bt_sim_reset_cnf_hdlr()",SIM(simServer)[0].substate)); */
            Assert(0);
        }
    }
    else if (SIM(simServer)[0].state == SIM_SERVER_STATE_OPEN)
    {

        if (SIM(simServer)[0].substate & BT_SIM_RESET_REQ_SENT)
        {
            bt_sim_openstate_reset_cnf_hdlr(ilm_ptr);
        }
        else if (SIM(simServer)[0].substate & BT_SIM_RESET_REQ_SENT_POWER_ON)
        {
            bt_sim_openstate_power_on_cnf_hdlr(ilm_ptr);
        }
        else if (SIM(simServer)[0].substate & BT_SIM_RESET_REQ_SENT_SET_T)
        {
            bt_sim_openstate_set_transport_protocol_cnf_hdlr(ilm_ptr);
        }
        else
        {
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_RSP_NOT_EXPECTED_IN_OPEN);
            Assert(0);
        }
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_RESET_RSP_NOT_IN_CONPENDING_OR_OPEN);
        Assert(0);
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_apdu_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_apdu_cnf_hdlr(ilm_struct *ilm_ptr)
{
    bt_sim_apdu_cnf_struct *bt_apdu_cnf = (bt_sim_apdu_cnf_struct*) ilm_ptr->local_para_ptr;
    BtStatus status;
    SimResultCode result = 0;
    U16 len = 0;
    U8 *ptr = NULL;

    /* Only one SIM Card ID-0 is used */

    if (SIM(simServer)[0].substate & BT_SIM_APDU_REQ_SENT)
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_APDU_RSP_FROM_SIM, bt_apdu_cnf->result);
        if (bt_apdu_cnf->result == SIM_RESULT_OK)
        {
            /* Get APDU_RSP with result from SIM task */
            len = bt_apdu_cnf->apdu_rsp_len;
            ptr = bt_apdu_cnf->apdu_rsp;
            result = SIM_RESULT_OK;
        }
        else if (bt_apdu_cnf->result == SIM_RESULT_CARD_NOT_ACCESSIBLE)
        {
            /* In this case, means that: driver does not get result, this belongs to physical err */
            /* Report(("APDU resp from SIM task is not accessible (physical error)")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_APDU_RSP_FROM_SIM_NOT_ACCESSIBLE);
            len = 0;
            ptr = NULL;
            /* Because result is not 0x00, so clear the flag BT_SIM_APDU_REQ_SENT here */
            SIM(simServer)[0].substate &= (~BT_SIM_APDU_REQ_SENT);
            result = SIM_RESULT_CARD_NOT_ACCESSIBLE;
        }
        else
        {
            /* Check the error code and update current_simcardreader_status */
            len = 0;
            ptr = NULL;
            SIM(simServer)[0].substate &= (~BT_SIM_APDU_REQ_SENT);
            result = SIM_RESULT_NO_REASON;
            /*
             * Determine the error code here
             * * result = bt_apdu_cnf->result;
             * * From the result field, to update current_simcardreader_status: if status is changed, STATUS_IND is needed be sent to client
             */

            /* Because result is not 0x00, so clear the flag BT_SIM_APDU_REQ_SENT here */
            SIM(simServer)[0].substate &= (~BT_SIM_APDU_REQ_SENT);
        }

        /* Send SIM_APDU_RSP msg to client with result: No matter success or error */
        if ((status = SIM_ServerApduRsp(&SIM(simServer)[0], len, ptr, result)) != BT_STATUS_SUCCESS)
        {
            /* Because sent err is happen for 0x00, also clear this flag BT_SIM_APDU_REQ_SENT */
            if (result == SIM_RESULT_OK)
            {
                SIM(simServer)[0].substate &= (~BT_SIM_APDU_REQ_SENT);
            }

            /* Because the conenction is closed before APDU_RSP returned from SIM, so SIM_ServerApduRsp() failed is ok */
            if (SIM(simServer)[0].rfChannel.state == 0)
            {
                //Report(("RFChannel is closed, so SIM_ServerApduRsp() failed is ok"));
                //kal_trace(TRACE_GROUP_1,RFCHNLCLOSED_SEND_RSP_FAILED_OK);
                kal_trace(TRACE_GROUP_1, BT_SIMAP_RFCHNLCLOSED_SEND_RSP_FAILED_OK);
                return;
            }

            SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
        }

        kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_APDU_RSP_TO_CLIENT, result, status, 3);

    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_NOT_EXPECTED_RCV_APDU_RSP_FROM_SIM);
        //Assert(0);
    }

}

/* This function handles the disconnect_cnf from SIM task */


/*****************************************************************************
 * FUNCTION
 *  bt_sim_disconnect_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]    
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_disconnect_cnf_hdlr(ilm_struct *ilm_ptr)
{
    if (SIM(simServer)[0].substate & BT_SIM_DISCONNECT_REQ_SENT)
    {
        /* Report(("disconnect_cnf_hdlr(): cardstatus=%02X",SIM(simServer)[0].current_simcardreader_status)); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_HDLR_CARDSTATUS, SIM(simServer)[0].current_simcardreader_status);
        if ((SIM(simServer)[0].substate & BT_SIM_CONNECT_PROCEDURE_COMPLETED) &&
            (SIM(simServer)[0].substate & BT_APP_DEACTIVATE_REQ_RECV) &&
            (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV))
        {

            /* In this case, recive MMI's DISC_REQ then DEACTIVATE_REQ */
            SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
            bt_simap_disconnect_cnf_struct *bt_simap_disconnect_cnf = construct_local_para(sizeof(bt_simap_disconnect_cnf_struct), TD_UL);
            bt_simap_deactivate_cnf_struct *bt_simap_deactivate_cnf = construct_local_para(sizeof(bt_simap_deactivate_cnf_struct), TD_UL);
            bt_simap_disconnect_cnf->result = TRUE;
            bt_simap_disconnect_cnf->cid = SIM(connect_id)[0];
#endif
	     parms.p.simap_disconnect_cnf.result = TRUE;
	     parms.p.simap_disconnect_cnf.cid = SIM(connect_id)[0];
            
            BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
            SIM(allocated_connect_id)[0] = KAL_FALSE;
            /* Report(("Release SIM Access Profile connect_id in bt_sim_disconnect_cnf()_00")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_DISC_CNF_HDLR00);
            /* Using this func to deregister rfcomm server channel and sdp record of SIM Access Profile */

            if (SIM_DeregisterServer(&SIM(simServer)[0]) != BT_STATUS_SUCCESS)
            {
                Assert(0);
            }

#if 0 // Reserved for MAUI
            bt_simap_deactivate_cnf->result = TRUE;
#endif
	     parms.p.simap_deactivate_cnf.result = TRUE;

            /* Report(("DISC_CNF case0")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE0);
            SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;
            /* Send DEACTIVATE_CNF msg to APP/MMI */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DISCONNECT_CNF,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_simap_disconnect_cnf,
                NULL);
            bt_send_msg(
                MSG_ID_BT_SIMAP_DEACTIVATE_CNF,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_simap_deactivate_cnf,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DISCONNECT_CNF;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);

	     parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else if ((SIM(simServer)[0].substate & BT_SIM_CONNECT_PROCEDURE_COMPLETED) &&
                 (SIM(simServer)[0].substate & BT_APP_DEACTIVATE_REQ_RECV))
        {
            SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
            bt_simap_deactivate_cnf_struct *bt_simap_deactivate_cnf = construct_local_para(sizeof(bt_simap_deactivate_cnf_struct), TD_UL);
            bt_simap_disconnect_ind_struct *bt_disconnect_ind = construct_local_para(sizeof(bt_simap_disconnect_ind_struct), TD_UL);
            bt_disconnect_ind->cid = SIM(connect_id)[0];
#endif
	     parms.p.simap_disconnect_ind.cid = SIM(connect_id)[0];

            /* If the substate is set "BT_SIM_CONNECT_PROCEDURE_COMPLETED" this flag, meaning that SAP had been connected */
            BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
            SIM(allocated_connect_id)[0] = KAL_FALSE;
            /* Report(("Release SIM Access Profile connect_id in bt_sim_disconnect_cnf()_01")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_DISC_CNF_HDLR01);

            /* Using this func to deregister rfcomm server channel and sdp record of SIM Access Profile */
            SIM_DeregisterServer(&SIM(simServer)[0]);

#if 0 // Reserved for MAUI
            bt_simap_deactivate_cnf->result = TRUE;
#endif
            parms.p.simap_deactivate_cnf.result = TRUE;
            
            /* Report(("DISC_CNF case1")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE1);
            SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;

            /* Send DISCONNECT_IND msg to MMI: Is connect_id in disconnect_cnf msg? */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DISCONNECT_IND,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_disconnect_ind,
                NULL);
            bt_send_msg(
                MSG_ID_BT_SIMAP_DEACTIVATE_CNF,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_simap_deactivate_cnf,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DISCONNECT_IND;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);

	     parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else if ((SIM(simServer)[0].substate & BT_SIM_CONNECT_PROCEDURE_COMPLETED) &&
                 (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV))
        {

            /* This disconnect_req is issued by MMI */
            SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
            bt_simap_disconnect_cnf_struct *disconnect_cnf = construct_local_para(sizeof(bt_simap_disconnect_cnf_struct), TD_UL);
            disconnect_cnf->result = TRUE;
            disconnect_cnf->cid = SIM(connect_id)[0];
#endif
	     parms.p.simap_disconnect_cnf.result = TRUE;
	     parms.p.simap_disconnect_cnf.cid = SIM(connect_id)[0];

            BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
            SIM(allocated_connect_id)[0] = KAL_FALSE;
            /* Report(("Release SIM Access Profile connect_id in bt_sim_disconnect_cnf()_02")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_DISC_CNF_HDLR02);

            kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_GRACEFUL_DISC_PREVIOUSLY);
            SIM(simServer)[0].substate &= (~(BT_SIM_DISCONNECT_REQ_SENT | BT_APP_DISCONNECT_REQ_RECV));
            /* Report(("DISC_CNF case2")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE2);
            /* Send DISCONNECT_CNF msg back to MMI */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DISCONNECT_CNF,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) disconnect_cnf,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DISCONNECT_CNF;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else if (SIM(simServer)[0].substate & BT_SIM_CONNECT_PROCEDURE_COMPLETED)
        {
#if 0 // Reserved for MAUI
            bt_simap_disconnect_ind_struct *bt_disconnect_ind = construct_local_para(sizeof(bt_simap_disconnect_ind_struct), TD_UL);
#endif
	     SIMAPCallbackParms parms;

            kal_trace(TRACE_GROUP_1, BT_SIMAP_ERROR_AND_NOTIFY_SIM_TO_RECOVERY, SIM(simServer)[0].substate);
            SIM(simServer)[0].substate &= (~BT_SIM_DISCONNECT_REQ_SENT);
#if 0 // Reserved for MAUI
            bt_disconnect_ind->cid = SIM(connect_id)[0];
#endif
	     parms.p.simap_disconnect_ind.cid = SIM(connect_id)[0];

            BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
            SIM(allocated_connect_id)[0] = KAL_FALSE;
            /* Report(("Release SIM Access Profile connect_id in bt_sim_disconnect_cnf()_03")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_DISC_CNF_HDLR03);
            /* Report(("DISC_CNF case3: Receive SIM_CONNECT_CNF previously and DISC happens")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE3);
            /* Send DISCONNECT_IND msg to MMI: Is connect_id in disconnect_cnf msg? */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DISCONNECT_IND,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_disconnect_ind,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DISCONNECT_IND;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else if ((SIM(simServer)[0].substate & BT_SIM_CONNECT_REQ_SENT) &&
                 (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV))
        {
            SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
            bt_simap_deactivate_cnf_struct *bt_simap_deactivate_cnf = construct_local_para(sizeof(bt_simap_deactivate_cnf_struct), TD_UL);
            bt_simap_disconnect_ind_struct *bt_disconnect_ind = construct_local_para(sizeof(bt_simap_disconnect_ind_struct), TD_UL);
            bt_disconnect_ind->cid = SIM(connect_id)[0];
#endif
	     parms.p.simap_disconnect_ind.cid = SIM(connect_id)[0];
            
            /* If the substate is set "BT_SIM_CONNECT_PROCEDURE_COMPLETED" this flag, meaning that SAP had been connected */
            BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
            SIM(allocated_connect_id)[0] = KAL_FALSE;
            /* Report(("Release SIM Access Profile connect_id in bt_sim_disconnect_cnf()_04")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_DISC_CNF_HDLR04);
            /* Using this func to deregister rfcomm server channel and sdp record of SIM Access Profile */
            SIM_DeregisterServer(&SIM(simServer)[0]);

#if 0 // Reserved for MAUI
            bt_simap_deactivate_cnf->result = TRUE;
#endif
            parms.p.simap_deactivate_cnf.result = TRUE;

            /* Report(("DISC_CNF case4")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE4);
            SIM(simServer)[0].simserver_activate_status = SIM_SERVER_STATUS_DEACTIVATED;

            /* Send DISCONNECT_IND msg to MMI: Is connect_id in disconnect_cnf msg? */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DISCONNECT_IND,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_disconnect_ind,
                NULL);
            bt_send_msg(
                MSG_ID_BT_SIMAP_DEACTIVATE_CNF,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_simap_deactivate_cnf,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DISCONNECT_IND;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);

	     parms.event = SIMAP_EVENT_DEACTIVATE_CNF;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else if (SIM(simServer)[0].substate & (BT_SIM_CONNECT_REQ_SENT))
        {
            /*
             * This means that this disconnect_cnf is reponse to disconnect_req due to receive RFEVENT_CLOSED
             * * Because if the rfcomm channel is needed to close under some conditions(e.g., client requests to disconnect or server disconnects),
             * *  and it is necessary to notify SIM task to recovery after closing rfcomm channel (if connection procedure is completedl previously)
             */
            SIMAPCallbackParms parms;
#if 0 // Reserved for MAUI
            bt_simap_disconnect_ind_struct *bt_disconnect_ind = construct_local_para(sizeof(bt_simap_disconnect_ind_struct), TD_UL);
            bt_disconnect_ind->cid = SIM(connect_id)[0];
#endif
	     parms.p.simap_disconnect_ind.cid = SIM(connect_id)[0];

            kal_trace(TRACE_GROUP_1, BT_SIMAP_ERROR_AND_NOTIFY_SIM_TO_RECOVERY, SIM(simServer)[0].substate);
            BTBMProfileReleaseConnIndex(SIM(connect_id)[0]);
            SIM(allocated_connect_id)[0] = KAL_FALSE;
            /* Report(("Release SIM Access Profile connect_id in bt_sim_disconnect_cnf()_05")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RELEASE_CONID_DISC_CNF_HDLR05);

            SIM(simServer)[0].substate &= (~BT_SIM_DISCONNECT_REQ_SENT);
            /* Report(("DISC_CNF case5: Not yet receive SIM_CONNECT_CNF and DISC happens")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE5);
            /* Send DISCONNECT_IND msg to MMI */
#if 0 // Reserved for MAUI
            bt_send_msg(
                MSG_ID_BT_SIMAP_DISCONNECT_IND,
                MOD_MMI,
                BT_APP_SAP,
                (local_para_struct*) bt_disconnect_ind,
                NULL);
#endif
	     parms.event = SIMAP_EVENT_DISCONNECT_IND;
	     parms.dstMod = MOD_MMI;
	     SIMAPCB(&parms);
        }
        else
        {
            /* Report(("DISC_CNF case6")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISC_CNF_CASE6);
            kal_trace(TRACE_GROUP_1, BT_SIMAP_ERR_NO_CON_REQ_SENT_TO_SIM);
            Assert(0);
        }
        /* Clear related contx of this connection */
        /* Reset substate of SIM server, it is close now */
        /* Report(("bt_sim_disconnect_cnf_hdlr():SIMServer is avaliable again")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRV_AVAILABLE_AGAIN);
        SIM(simServer)[0].substate = 0;
        SIM(simServer)[0].available = TRUE;
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_ERR_NO_DISC_REQ_SENT_TO_SIM);
        /* This case happens in the following scenario: SIMAP is connected, but MMI requests to exit test mode: */
        /* All active link is disconnected first, but SIM DISC CNF from the SIM card is lower than MMI's BT POWER ON Req */
        /* In this way, SIM(simServer)[0].substate is reset to zero due to POWER OFF/POWER ON */
        /* Assert(0); */
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_sim_power_off_cnf_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]    
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sim_power_off_cnf_hdlr(ilm_struct *ilm_ptr)
{
    bt_sim_power_off_cnf_struct *bt_power_off_cnf = (bt_sim_power_off_cnf_struct*) ilm_ptr->local_para_ptr;
    SimResultCode result = SIM_RESULT_NO_REASON;
    BtStatus status = BT_STATUS_FAILED;

    if (SIM(simServer)[0].substate & BT_SIM_POWER_OFF_REQ_SENT)
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_POWER_OFF_CNF_FROM_SIM, bt_power_off_cnf->result);
        /* Report(("power_off_cnf_hdlr(): cardstatus=%02X",SIM(simServer)[0].current_simcardreader_status)); */
        kal_trace(
            TRACE_GROUP_1,
            BT_SIMAP_CARDSTATUS_IN_POWER_OFF_HDLER,
            SIM(simServer)[0].current_simcardreader_status);

        if (bt_power_off_cnf->result == SIM_RESULT_OK)
        {
            result = SIM_RESULT_OK;
        }
        else if (bt_power_off_cnf->result == SIM_RESULT_CARD_ALREADY_OFF)
        {
            /* Report(("Org cardstatus is on, so POWER_OFF is still sent to SIM, but actually card is off by other module")); */
            result = SIM_RESULT_CARD_ALREADY_OFF;
            SIM(simServer)[0].current_simcardreader_status &= (~SIM_CRS_CARD_POWERED_ON);

            /* Because result is not 0x00, so clear the flag BT_SIM_POWER_OFF_REQ_SENT */
            SIM(simServer)[0].substate &= (~BT_SIM_POWER_OFF_REQ_SENT);
        }
        else
        {
            /* Determine error code and */
            result = SIM_RESULT_NO_REASON;  /* or SIM_RESULT_NO_REASON */
            /* From the result field, to update current_simcardreader_status */

            /* Because result is not 0x00, so clear the flag BT_SIM_POWER_OFF_REQ_SENT */
            SIM(simServer)[0].substate &= (~BT_SIM_POWER_OFF_REQ_SENT);
        }

        /* Send SIM_POWER_OFF_RSP with result to client */
        if ((status = SIM_ServerSimOffRsp(&SIM(simServer)[0], result)) != BT_STATUS_SUCCESS)
        {
            /* Because sent err is happened for 0x00, also clear this flag BT_SIM_POWER_OFF_REQ_SENT */
            if (result == SIM_RESULT_OK)
            {
                SIM(simServer)[0].substate &= (~BT_SIM_POWER_OFF_REQ_SENT);
            }

            /* Because the conenction is closed before PowerOff_RSP returned from SIM, so SIM_ServerSimOffRsp() failed is ok */
            if (SIM(simServer)[0].rfChannel.state == 0)
            {
                /* Report(("RFChannel is closed, so SIM_ServerSimOffRsp() failed is ok")); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_RFCHNLCLOSED_SEND_RSP_FAILED_OK);
                return;
            }

            SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
        }
        else
        {
            if ((result == SIM_RESULT_OK) && (SIM(simServer)[0].current_simcardreader_status & SIM_CRS_CARD_POWERED_ON))
            {
                /*
                 * Updata current_simcardreader_status:
                 * * But the substate is updated (i.e., clearing the bit BT_SIM_POWER_OFF_REQ_SENT) 
                 * * util the simPacket payloaded for POWER_OFF_RSP is returned!(i.e, SIM_PACKET_HANDLED Event)
                 */

                /* When SimPacket is handled in Open state, the flag BT_SIM_RESET_REQ_SENT_POWER_OFF is cleared */

                SIM(simServer)[0].current_simcardreader_status &= (~SIM_CRS_CARD_POWERED_ON);
                SIM(simServer)[0].current_simcardreader_status |= SIM_CRS_CARD_POWERED_OFF;
                /* Report(("power_off_cnf_hdlr() and set off: cardstatus=%02X",SIM(simServer)[0].current_simcardreader_status)); */
                kal_trace(
                    TRACE_GROUP_1,
                    BT_SIMAP_CARDSTATUS_IN_POWER_OFF_HDLER,
                    SIM(simServer)[0].current_simcardreader_status);

            }
        }
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_POWER_OFF_RSP_TO_CLIENT, result, status);

    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_NOT_EXPECTED_RCV_POWER_OFF_RSP_FROM_SIM);
        Assert(0);
    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_init
 * DESCRIPTION
 *  When receiveing MSG_ID_BT_POWERON_REQ from MMI, bt_profiles_init()-> bt_simap_init()
 *  Init function to do initialization contexts for SIM Access Profile, e.g.
 *  bt_simap_config_simserver(): config the simServer supported which type SIM CARD 
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_init(void)
{
#if (defined(__BT_SIM_PROFILE__)) && (SIM_SERVER == XA_ENABLED)

    /*
     * 2006-10-16: Set Security mode and Encrypyion mode: But in current mode, encryption is combined with Security Level 3 
     * //Register the Global Handler (for operation modes)
     * ME_InitHandler((&SimAp_globalHandler));
     * SimAp_globalHandler.callback = Me2SimApCallback;
     * ME_RegisterGlobalHandler((&SimAp_globalHandler));
     * ME_SetEventMask((&SimAp_globalHandler), BEM_ALL_EVENTS);
     * //SIM Server requires authentication & encryption
     * SEC_EnableSecurityMode3((&SimAp_globalHandler),TRUE);
     * 
     */

    /*
     * 2006-10-16: Set Class of Device : in btProperty.h, BT_DEFAULT_COD is already defined, Can be multiple major device classes  defined?
     * // Set the Class of Device
     * //From Spec definition (and this is ESI simv1.0 version): ME_SetClassOfDevice(COD_MAJOR_PERIPHERAL | COD_MINOR_PERIPH_CARD_RDR);
     * //ESI simv1.1 version: ME_SetClassOfDevice(COD_MAJOR_PHONE | COD_MINOR_PHONE_UNCLASSIFIED);
     */

    /* Register a RfComm Server Channel and register a SDP record in the function SIM_RegisterServer() */
    /*
     * The callback procedure:
     * * 1. SimServerRfCallback(), i.e., Server->rfChannel.callback()
     * * 2. According to SIM server's state, invoke the corresponding function: SIM(serverState)[server->state](,)
     * * 3. Each state function invokes SimServerAppCallback() --> SimSrvCallback (i.e.,Server->callback()) 
     */

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8*) simContext, 0, sizeof(BtSimContext));
#else 
    OS_MemSet((U8*) & simContext, 0, sizeof(BtSimContext));
#endif 

    bt_simap_config_simserver();
#endif /* (defined(__BT_SIM_PROFILE__)) && (SIM_SERVER == XA_ENABLED) */

}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_deinit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_deinit(void)
{
#if (defined(__BT_SIM_PROFILE__)) && (SIM_SERVER == XA_ENABLED)
/* ALPS00118836 : Sometimes the SIMAP deactivate message will received after power off. In that case
                  clear context will make registered link of stack broken.
 */
/*
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8*) simContext, 0, sizeof(BtSimContext));
#else 
    OS_MemSet((U8*) & simContext, 0, sizeof(BtSimContext));
#endif 
*/
#endif /* (defined(__BT_SIM_PROFILE__)) && (SIM_SERVER == XA_ENABLED) */

}

/*****************************************************************************
 * FUNCTION
 *  bt_simap_reset
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_reset(void)
{
    kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_IND, SIM(simServer)[0].state);
	
	//check if need notify app
	if (SIM(simServer)[0].state != SIM_SERVER_STATE_CLOSED)
	{
		/* Need notify app */
		SIMAPCallbackParms parms;
		
		kal_trace(TRACE_GROUP_1, BT_SIMAP_NOTIFY_APP_RESET_IND);

		parms.event = SIMAP_EVENT_BT_RESET_IND;
		parms.dstMod = MOD_MMI;

		SIMAPCB(&parms);
	}

	//clear memory
	bt_simap_deinit();
}

#ifdef __SIMAP_REG_GLOBAL_HDLER__


/*****************************************************************************
 * FUNCTION
 *  Me2SimApCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void Me2SimApCallback(const BtEvent *Event)
{
    /* Reference Me2BMCallback() : Specify which events will be handled in this function */
    switch (Event->eType)
    {

    }

}
#endif /* __SIMAP_REG_GLOBAL_HDLER__ */ 


/*****************************************************************************
 * FUNCTION
 *  bt_simap_wait_atr_in_connect_procedure_timeout_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timeout_cb_input        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_wait_atr_in_connect_procedure_timeout_handler(void *timeout_cb_input)
{
    kal_trace(TRACE_GROUP_1, BT_SIMAP_ATR_TIMEOUT_HDLR);

#ifdef __SIMAP_MAUI__
    SIM(simap_event_id) = 0;

    if ((bt_p->os_init == FALSE) || (bt_p->entering_poweroff == TRUE))
    {
        /* Report(("In atr_timeout:Not in POWER ON state")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_ATR_TIMEOUT_NOT_IN_POWERON_STATE);
        return;
    }
#endif

    /* Assert((SIM(simServer)[0].state == SIM_SERVER_STATE_CONN_PENDING) && (SIM(simServer)[0].flags & SIM_SERVER_FLAG_ATR)); */
    if ((SIM(simServer)[0].state == SIM_SERVER_STATE_CONN_PENDING) && (SIM(simServer)[0].flags & SIM_SERVER_FLAG_ATR))
    {
        /* In connection procedure, waits for ATR_REQ from SIM Client timeout, close RFCOMM Channel */
        /* Return value from RF_CloseChannel() may be BT_STATUS_IN_PROGRRESS(presenting rfchannel state is DLC_STATE_DISC_PENDING) */
        SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
    }
    else
    {
        /* Report(("atr_timeout,simServerState=%d,flags=%02x",SIM(simServer)[0].state,SIM(simServer)[0].flags)); */
        kal_trace(
            TRACE_GROUP_1,
            BT_SIMAP_ATRTIMEOUT_SRVSTATE_AND_FLAG,
            SIM(simServer)[0].state,
            SIM(simServer)[0].flags);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_simap_wait_req_in_graceful_disconnect_procedure_timeout_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timeout_cb_input        [IN]    
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_wait_req_in_graceful_disconnect_procedure_timeout_handler(void *timeout_cb_input)
{
    BtStatus status;

    kal_trace(TRACE_GROUP_1, BT_SIMAP_GRACEFUL_TIMEOUT_HDLR);

#ifdef __SIMAP_MAUI__
    SIM(simap_event_id) = 0;

    if ((bt_p->os_init == FALSE) || (bt_p->entering_poweroff == TRUE))
    {
        /* Report(("In graceful_disc_timeout:Not in POWER ON state")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_GRACEFUL_TIMEOUT_NOT_IN_POWEROFF_STATE);
        return;
    }
#endif

    /* Assert((SIM(simServer)[0].state == SIM_SERVER_STATE_OPEN) && (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV)); */
    /* In connection procedure, waits for APDU cmd or DISC_REQ from SIM Client timeout, invoke SIM_ServerClose() with Immd disconnection type */

    /* Send DISCONNECT_REQ packet to sim client */
    if ((SIM(simServer)[0].state == SIM_SERVER_STATE_OPEN) && (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV))
    {
        if ((status = SIM_ServerClose(&SIM(simServer)[0], SIM_DISCONNECT_IMMEDIATE)) != BT_STATUS_PENDING)
        {
            kal_trace(TRACE_GROUP_1, BT_SIMAP_GRACEFUL_TIMEOUT_HDLR_CAN_NOT_CLOSE, status);
            /* If SIM_ServerClose() failed, using RF_CloseChannel */

            SimApCloseRFCommChnl(&(SIM(simServer)[0].rfChannel));
        }
        else
        {
            /* Report(("In bt_simap_wait_req_in_graceful_disconnect_procedure_timeout_handler():Closing the connection with immd type")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_GRACEFUL_DISC_TIMEOUT_CLOSE_CON_WITH_IMMED_TYPE);
        }
    }
    else
    {
        /* Report(("graceful_disc_timeout,simServerState=%d,substate=%02x",SIM(simServer)[0].state,SIM(simServer)[0].substate)); */
        kal_trace(
            TRACE_GROUP_1,
            BT_SIMAP_GRACEFUL_DISC_TIMEOUT_SRVSTATE_AND_SUBSTATE,
            SIM(simServer)[0].state,
            SIM(simServer)[0].substate);
    }
}

#endif /* __BT_SIM_PROFILE__ */
