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
 * bt_adp_simap_event.c
 * 
 * This file is the Internal Adaptation of SAP profile that profile uses to send event to external ADP.
 * Flow direction: external ADP API <-- message queue <-- internal ADP <-- profile API
 */

#include "bt_common.h"
#include "bt_sim.h"
#include "bt_os_api.h"

void SIMAP_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    //ilm_struct *ilmPtr;

    if (/* (0 > dstMod) || */ (END_OF_MOD_ID < dstMod))
    {
        return;
    }
	if (NULL != local_para)
	{
		BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
		//free_local_para(local_para);
	}
	else
	{
		BT_SendMessage(msg, dstMod, NULL, 0);
	}
}


void SIMAPAdpActivateCnf(SIMAPCallbackParms *Parms)
{
    bt_simap_activate_cnf_struct *msg = (bt_simap_activate_cnf_struct*) construct_local_para(sizeof(bt_simap_activate_cnf_struct), TD_UL);

    msg->result = Parms->p.simap_activate_cnf.result;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_ACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAPAdpDeactivateCnf(SIMAPCallbackParms *Parms)
{
    bt_simap_deactivate_cnf_struct *msg = (bt_simap_deactivate_cnf_struct*) construct_local_para(sizeof(bt_simap_deactivate_cnf_struct), TD_UL);

    msg->result = Parms->p.simap_deactivate_cnf.result;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_DEACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAPAdpConnectInd(SIMAPCallbackParms *Parms)
{
    bt_simap_connect_ind_struct *msg = (bt_simap_connect_ind_struct*) construct_local_para(sizeof(bt_simap_connect_ind_struct), TD_UL);

    msg->cid = Parms->p.simap_connect_ind.cid;
    msg->lap = Parms->p.simap_connect_ind.lap;
    msg->uap = Parms->p.simap_connect_ind.uap;
    msg->nap = Parms->p.simap_connect_ind.nap;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_CONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAPAdpDisconnectInd(SIMAPCallbackParms *Parms)
{
    bt_simap_disconnect_ind_struct *msg = (bt_simap_disconnect_ind_struct*) construct_local_para(sizeof(bt_simap_disconnect_ind_struct), TD_UL);

    msg->cid = Parms->p.simap_disconnect_ind.cid;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_DISCONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAPAdpDisconnectCnf(SIMAPCallbackParms *Parms)
{
    bt_simap_disconnect_cnf_struct *msg = (bt_simap_disconnect_cnf_struct*) construct_local_para(sizeof(bt_simap_disconnect_cnf_struct), TD_UL);

    msg->result = Parms->p.simap_disconnect_cnf.result;
    msg->cid = Parms->p.simap_disconnect_cnf.cid;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_DISCONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAPAdpAuthReq(SIMAPCallbackParms *Parms)
{
    bt_simap_auth_req_struct *msg = (bt_simap_auth_req_struct*) construct_local_para(sizeof(bt_simap_auth_req_struct), TD_UL);

    msg->lap = Parms->p.simap_auth_req.lap;
    msg->uap = Parms->p.simap_auth_req.uap;
    msg->nap = Parms->p.simap_auth_req.nap;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_AUTH_REQ, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAPAdpAuthReqTimeoutRfcloseInd(SIMAPCallbackParms *Parms)
{
    bt_simap_auth_req_timeout_rfclose_ind_struct *msg = (bt_simap_auth_req_timeout_rfclose_ind_struct*) construct_local_para(sizeof(bt_simap_auth_req_timeout_rfclose_ind_struct), TD_UL);

    msg->lap = Parms->p.simap_auth_req_timeout_rfclose_ind.lap;
    msg->uap = Parms->p.simap_auth_req_timeout_rfclose_ind.uap;
    msg->nap = Parms->p.simap_auth_req_timeout_rfclose_ind.nap;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_AUTH_REQ_TIMEOUT_RFCLOSE_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SIMAdpAtrRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_ATR_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpApduRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_APDU_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpSimOnRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_SIM_ON_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpSimOffRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_SIM_OFF_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpSimResetRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_SIM_RESET_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpCardStatusIndSentInd(SIMAPCallbackParms *Parms)
{
    bt_simap_card_status_ind_sent_ind_struct *msg = (bt_simap_card_status_ind_sent_ind_struct*) construct_local_para(sizeof(bt_simap_card_status_ind_sent_ind_struct), TD_UL);

    msg->status = Parms->p.simap_card_status_ind_sent_ind.status;

    SIMAP_sendMsg(MSG_ID_BT_SIMAP_CARD_STATUS_IND_SENT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
    
}


void SIMAdpCardReaderStatusRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_CARD_READER_STATUS_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpSetTransportProtocolRspSentInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpSimConnectFailedInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_SIM_CONNECT_FAILED_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}

void SIMAdpBTResetInd(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIMAP_BT_RESET_IND, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SIMAdpConnectReq(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIM_CONNECT_REQ, Parms->dstMod, PS_SIM_SAP, NULL, NULL);
}


void SIMAdpDisconnectReq(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIM_DISCONNECT_REQ, Parms->dstMod, PS_SIM_SAP, NULL, NULL);
}


void SIMAdpApduReq(SIMAPCallbackParms *Parms)
{
    bt_sim_apdu_req_struct *msg = (bt_sim_apdu_req_struct*) construct_local_para(sizeof(bt_sim_apdu_req_struct), TD_UL);

    msg->apdu_req_len = Parms->p.sim_apdu_req.apdu_req_len;
    msg->transport_protocol_type = Parms->p.sim_apdu_req.transport_protocol_type;
    btmtk_os_memset(msg->apdu_req, 0, APDU_REQ_MAX_LEN);
    btmtk_os_memcpy(msg->apdu_req, Parms->p.sim_apdu_req.apdu_req, msg->apdu_req_len);

    SIMAP_sendMsg(MSG_ID_BT_SIM_APDU_REQ, Parms->dstMod, PS_SIM_SAP, (local_para_struct*) msg, NULL);
}


void SIMAdpPowerOffReq(SIMAPCallbackParms *Parms)
{
    SIMAP_sendMsg(MSG_ID_BT_SIM_POWER_OFF_REQ, Parms->dstMod, PS_SIM_SAP, NULL, NULL);
}


void SIMAdpResetReq(SIMAPCallbackParms *Parms)
{
    bt_sim_reset_req_struct *msg = (bt_sim_reset_req_struct*) construct_local_para(sizeof(bt_sim_reset_req_struct), TD_UL);

    msg->transport_protocol_type = Parms->p.sim_reset_req.transport_protocol_type;

    SIMAP_sendMsg(MSG_ID_BT_SIM_RESET_REQ, Parms->dstMod, PS_SIM_SAP, (local_para_struct*) msg, NULL);
}


void btmtk_adp_simap_int_event_callback(SIMAPCallbackParms *Parms)
{
	Report(("[BT SIMAP]btmtk_adp_simap_int_event_callback, event=%d",Parms->event));
	
    switch (Parms->event)        
    {
    	 /* SIMAP --> MMI */
        case SIMAP_EVENT_ACTIVATE_CNF:
            SIMAPAdpActivateCnf(Parms);
            break;
        case SIMAP_EVENT_DEACTIVATE_CNF:
            SIMAPAdpDeactivateCnf(Parms);
            break;
        case SIMAP_EVENT_CONNECT_IND:
            SIMAPAdpConnectInd(Parms);
            break;
        case SIMAP_EVENT_DISCONNECT_IND:
            SIMAPAdpDisconnectInd(Parms);
            break;
        case SIMAP_EVENT_DISCONNECT_CNF:
            SIMAPAdpDisconnectCnf(Parms);
            break;
        case SIMAP_EVENT_AUTH_REQ:
            SIMAPAdpAuthReq(Parms);
            break;
        case SIMAP_EVENT_ATR_RSP_SENT_IND:
            SIMAdpAtrRspSentInd(Parms);
            break;
        case SIMAP_EVENT_APDU_RSP_SENT_IND:
            SIMAdpApduRspSentInd(Parms);
            break;
        case SIMAP_EVENT_SIM_ON_RSP_SENT_IND:
            SIMAdpSimOnRspSentInd(Parms);
            break;
        case SIMAP_EVENT_SIM_OFF_RSP_SENT_IND:
            SIMAdpSimOffRspSentInd(Parms);
            break;
        case SIMAP_EVENT_SIM_RESET_RSP_SENT_IND:
            SIMAdpSimResetRspSentInd(Parms);
            break;
        case SIMAP_EVENT_CARD_STATUS_IND_SENT_IND:
            SIMAdpCardStatusIndSentInd(Parms);
            break;
        case SIMAP_EVENT_CARD_READER_STATUS_RSP_SENT_IND:
            SIMAdpCardReaderStatusRspSentInd(Parms);
            break;
        case SIMAP_EVENT_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND:
            SIMAdpSetTransportProtocolRspSentInd(Parms);
            break;
        case SIMAP_EVENT_AUTH_REQ_TIMEOUT_RFCLOSED_IND:
            SIMAPAdpAuthReqTimeoutRfcloseInd(Parms);
            break;
        case SIMAP_EVENT_SIM_CONNECT_FAILED_IND:
            SIMAdpSimConnectFailedInd(Parms);
            break;

		case SIMAP_EVENT_BT_RESET_IND:
			SIMAdpBTResetInd(Parms);
			break;

	 /* SIMAP --> SIM task */
	 case SIM_EVENT_CONNECT_REQ:
	     SIMAdpConnectReq(Parms);
	     break;
	 case SIM_EVENT_DISCONNECT_REQ:
	     SIMAdpDisconnectReq(Parms);
	     break;
	 case SIM_EVENT_APDU_REQ:
	     SIMAdpApduReq(Parms);
	     break;
	 case SIM_EVENT_POWER_OFF_REQ:
	     SIMAdpPowerOffReq(Parms);
	     break;
	 case SIM_EVENT_RESET_REQ:
	     SIMAdpResetReq(Parms);
	     break;
	     
        default:
            Assert(0);
    }
}



