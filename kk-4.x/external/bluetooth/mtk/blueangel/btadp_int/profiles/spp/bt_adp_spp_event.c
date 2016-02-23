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
 * bt_adp_spp_event.c
 * 
 * This file is the Internal Adaptation of SPP profile that profile uses to send event to external ADP.
 * Flow direction: external ADP API <-- message queue <-- internal ADP <-- profile API
 */


#include "bt_adp_spp_event.h"
#include "bluetooth_spp_struct.h"
#include "osapi.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"

 
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


/*****************************************************************************
 * FUNCTION
 *  SPPOS_sendMsg
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
#if 0
void SPPOS_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = msg;
    ilmPtr->local_para_ptr = local_para;
    ilmPtr->peer_buff_ptr = peer_buff;
    ilmPtr->dest_mod_id = dstMod;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = sap;
    msg_send_ext_queue(ilmPtr);
}
#endif


void SPPOS_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
#ifndef __SPP_THROUGHPUT_TEST__
    //ilm_struct *ilmPtr;

    if ((0 > (int)dstMod) || (END_OF_MOD_ID <= dstMod))
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
#endif
}


/* #ifdef CUSTOM_SPP */
void SPPAdpConnectInd(SPPCallbackParms *Parms)
{
    bt_spp_connect_ind_struct *msg = (bt_spp_connect_ind_struct*) construct_local_para(sizeof(bt_spp_connect_ind_struct), TD_UL);

    msg->port = Parms->p.spp_connect_ind.port;
    msg->cid = Parms->p.spp_connect_ind.cid;
    msg->lap = Parms->p.spp_connect_ind.lap;
    msg->uap = Parms->p.spp_connect_ind.uap;
    msg->nap = Parms->p.spp_connect_ind.nap;

    SPPOS_sendMsg(MSG_ID_BT_SPP_CONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}

void SPPAdpConnectIndReq(SPPCallbackParms *Parms)
{
    bt_spp_connect_ind_req_struct *spp_con_ind_req = construct_local_para(sizeof(bt_spp_connect_ind_req_struct), TD_UL);

    spp_con_ind_req->lap = 0;
    spp_con_ind_req->lap = Parms->p.spp_connect_ind_req.lap;
    spp_con_ind_req->uap = 0;
    spp_con_ind_req->uap = Parms->p.spp_connect_ind_req.uap;
    spp_con_ind_req->nap = 0;
    spp_con_ind_req->nap = Parms->p.spp_connect_ind_req.nap;
    spp_con_ind_req->port = Parms->p.spp_connect_ind_req.port;
    SPPOS_sendMsg(
        MSG_ID_BT_SPP_CONNECT_IND_REQ,
        Parms->dstMod,
        BT_APP_SAP,
        (local_para_struct*) spp_con_ind_req,
        NULL);
}

void SPPAdpDisconnectInd(SPPCallbackParms *Parms)
{
    bt_spp_disconnect_ind_struct *msg1 = 0;
    SPP_DEV_OS *vp_ptr = 0;
    
    Report(("SPPAdpDisconnectInd dstmod=%d", Parms->dstMod));    
    
    msg1 = (bt_spp_disconnect_ind_struct*) construct_local_para(sizeof(bt_spp_disconnect_ind_struct), TD_UL);
    msg1->port = Parms->p.spp_disconnect_ind.port;
    SPPOS_sendMsg(MSG_ID_BT_SPP_DISCONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg1, NULL);

    // Deactivate if this is a Client port
    vp_ptr = getDev(Parms->p.spp_disconnect_cnf.port);
    if (vp_ptr->sppDev.portType & SPP_CLIENT_PORT)
    {
    	btmtk_spp_deactivate_req_struct *msg =
    	(btmtk_spp_deactivate_req_struct*) construct_local_para(sizeof(btmtk_spp_deactivate_req_struct), TD_UL);
    	msg->port = Parms->p.spp_disconnect_cnf.port;
	SPPOS_sendMsg(MSG_ID_BT_SPP_DEACTIVATE_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);
    }
}        

void SPPAdpActivateCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_activate_cnf_struct *msg = 
		(btmtk_spp_activate_cnf_struct*) construct_local_para(sizeof(btmtk_spp_activate_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_activate_cnf.port;
	msg->result = Parms->p.spp_activate_cnf.result;
	
	SPPOS_sendMsg(MSG_ID_BT_SPP_ACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}

void SPPAdpDeactivateCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_deactivate_cnf_struct *msg = 
		(btmtk_spp_deactivate_cnf_struct*) construct_local_para(sizeof(btmtk_spp_deactivate_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_deactivate_cnf.port;
	msg->result = Parms->p.spp_activate_cnf.result;
	
    SPPOS_sendMsg(MSG_ID_BT_SPP_DEACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}       

void SPPAdpDisconnectCnf(SPPCallbackParms *Parms)
{
    bt_spp_disconnect_cnf_struct *bt_spp_disconnect_cnf;
    SPP_DEV_OS *vp_ptr = 0;

    bt_spp_disconnect_cnf =
        (bt_spp_disconnect_cnf_struct*) construct_local_para(sizeof(bt_spp_disconnect_cnf_struct), TD_UL);
    bt_spp_disconnect_cnf->cid = Parms->p.spp_disconnect_cnf.cid;
    bt_spp_disconnect_cnf->port = Parms->p.spp_disconnect_cnf.port;
    bt_spp_disconnect_cnf->lap = Parms->p.spp_disconnect_cnf.lap;
    bt_spp_disconnect_cnf->uap = Parms->p.spp_disconnect_cnf.uap;
    bt_spp_disconnect_cnf->nap = Parms->p.spp_disconnect_cnf.nap;
    bt_spp_disconnect_cnf->result = Parms->p.spp_disconnect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_SPP_DISCONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_disconnect_cnf, NULL);    

    // Deactivate if this is a Client port
    vp_ptr = getDev(Parms->p.spp_disconnect_cnf.port);
    if ((vp_ptr->sppDev.portType & SPP_CLIENT_PORT) && (SPP_DISCONNECT_SUCCESS == Parms->p.spp_disconnect_cnf.result))
    {
    	btmtk_spp_deactivate_req_struct *msg =
    	(btmtk_spp_deactivate_req_struct*) construct_local_para(sizeof(btmtk_spp_deactivate_req_struct), TD_UL);
    	msg->port = Parms->p.spp_disconnect_cnf.port;
	SPPOS_sendMsg(MSG_ID_BT_SPP_DEACTIVATE_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);
    }
}

void SPPAdpAuthReq(SPPCallbackParms *Parms)
{
    bt_spp_auth_req_struct *bt_spp_auth_req = construct_local_para(sizeof(bt_spp_auth_req_struct), TD_UL);

    bt_spp_auth_req->port = Parms->p.spp_auth_req.port;
    SPPOS_sendMsg(MSG_ID_BT_SPP_AUTH_REQ, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_auth_req, NULL);
   
}

void SPPAdpAudioConnectCnf(SPPCallbackParms *Parms)
{    
    bt_spp_audio_connect_cnf_struct *bt_spp_msg =
        (bt_spp_audio_connect_cnf_struct*) construct_local_para(
                                            sizeof(bt_spp_audio_connect_cnf_struct),
                                            TD_UL);
    bt_spp_msg->port = Parms->p.spp_audio_connect_cnf.port;
    bt_spp_msg->result = Parms->p.spp_audio_connect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_SPP_AUDIO_CONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_msg, NULL);            
}

void SPPAdpAudioConnectInd(SPPCallbackParms *Parms)
{
    bt_spp_audio_connect_ind_struct *bt_spp_msg =
        (bt_spp_audio_connect_ind_struct*) construct_local_para(
                                            sizeof(bt_spp_audio_connect_ind_struct),
                                            TD_UL);
    bt_spp_msg->port = Parms->p.spp_audio_connect_ind.port;
    SPPOS_sendMsg(MSG_ID_BT_SPP_AUDIO_CONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_msg, NULL);    
}

void SPPAdpAudioDisconnectCnf(SPPCallbackParms *Parms)
{
    bt_spp_audio_disconnect_cnf_struct *bt_spp_msg =
    (bt_spp_audio_disconnect_cnf_struct*) construct_local_para(
                                            sizeof(bt_spp_audio_disconnect_cnf_struct),
                                            TD_UL);
    bt_spp_msg->port = Parms->p.spp_audio_disconnect_cnf.port;
    bt_spp_msg->result = Parms->p.spp_audio_disconnect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_msg, NULL);
}

void SPPAdpAudioDisconnectInd(SPPCallbackParms *Parms)
{
    bt_spp_audio_disconnect_ind_struct *bt_spp_msg =
        (bt_spp_audio_disconnect_ind_struct*) construct_local_para(
                                                sizeof(bt_spp_audio_disconnect_ind_struct),
                                                TD_UL);    
    bt_spp_msg->port = Parms->p.spp_audio_disconnect_ind.port;
    SPPOS_sendMsg(MSG_ID_BT_SPP_AUDIO_DISCONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_msg, NULL);    
}

void SPPAdpScoRssiInd(SPPCallbackParms *Parms)
{
    bt_spp_sco_rssi_ind_struct *rssi_ind =
        (bt_spp_sco_rssi_ind_struct*) construct_local_para(sizeof(bt_spp_sco_rssi_ind_struct), TD_UL);

    btmtk_os_memcpy(rssi_ind->bd_address, Parms->p.spp_sco_rssi_ind.bd_address, 6);
    rssi_ind->rssi_value = Parms->p.spp_sco_rssi_ind.rssi_value;
    SPPOS_sendMsg(MSG_ID_BT_SPP_SCO_RSSI_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) rssi_ind, NULL);

}

void SPPAdpConnectCnf(SPPCallbackParms *Parms)
{
    bt_spp_connect_cnf_struct *bt_spp_connect_cnf;

    bt_spp_connect_cnf = (bt_spp_connect_cnf_struct*) construct_local_para(sizeof(bt_spp_connect_cnf_struct), TD_UL);
    bt_spp_connect_cnf->cid = Parms->p.spp_connect_cnf.cid;
    bt_spp_connect_cnf->port = Parms->p.spp_connect_cnf.port;
    bt_spp_connect_cnf->lap = Parms->p.spp_connect_cnf.lap;
    bt_spp_connect_cnf->uap = Parms->p.spp_connect_cnf.uap;
    bt_spp_connect_cnf->nap = Parms->p.spp_connect_cnf.nap;
    bt_spp_connect_cnf->server_chnl_num =  Parms->p.spp_connect_cnf.server_chnl_num;    
    bt_spp_connect_cnf->result = Parms->p.spp_connect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_SPP_CONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_spp_connect_cnf, NULL);    
}

void SPPAdpUartOwnerInd(SPPCallbackParms *Parms)
{
    bt_spp_uart_owner_ind_struct *bt_spp_uart_owner_ind = NULL;

    bt_spp_uart_owner_ind = construct_local_para(sizeof(bt_spp_uart_owner_ind_struct), TD_UL);
    bt_spp_uart_owner_ind->port = Parms->p.spp_uart_owner_ind.port;    
    SPPOS_sendMsg(
        MSG_ID_BT_SPP_UART_OWNER_IND,
        Parms->dstMod,
        BT_APP_SAP,
        (local_para_struct*) bt_spp_uart_owner_ind,
        NULL);    
}

#if defined(__BT_DUN_PROFILE__)
/*APP <-- DUN*/
void DUNAdpConnectInd(SPPCallbackParms *Parms)
{
    bt_dun_connect_ind_struct *msg1 = 0;
    msg1 = (bt_dun_connect_ind_struct*) construct_local_para(sizeof(bt_dun_connect_ind_struct), TD_UL);

    msg1->port = Parms->p.dun_connect_ind.port;
    msg1->cid = Parms->p.dun_connect_ind.cid;
    msg1->lap = Parms->p.dun_connect_ind.lap;
    msg1->uap = Parms->p.dun_connect_ind.uap;
    msg1->nap = Parms->p.dun_connect_ind.nap;

    SPPOS_sendMsg(MSG_ID_BT_DUN_CONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg1, NULL);    
}

void DUNAdpConnectIndReq(SPPCallbackParms *Parms)
{
    bt_dun_connect_ind_req_struct *dun_con_ind_req = construct_local_para(sizeof(bt_dun_connect_ind_req_struct), TD_UL);

    dun_con_ind_req->lap = 0;
    dun_con_ind_req->lap = Parms->p.dun_connect_ind_req.lap;
    dun_con_ind_req->uap = 0;
    dun_con_ind_req->uap = Parms->p.dun_connect_ind_req.uap;
    dun_con_ind_req->nap = 0;
    dun_con_ind_req->nap = Parms->p.dun_connect_ind_req.nap;
    dun_con_ind_req->port = Parms->p.dun_connect_ind_req.port;
    SPPOS_sendMsg(
        MSG_ID_BT_DUN_CONNECT_IND_REQ,
        Parms->dstMod,
        BT_APP_SAP,
        (local_para_struct*) dun_con_ind_req,
        NULL);
}

void DUNAdpDisconnectInd(SPPCallbackParms *Parms)
{
    bt_dun_disconnect_ind_struct *msg2 = 
        (bt_dun_disconnect_ind_struct*) construct_local_para(sizeof(bt_dun_disconnect_ind_struct), TD_UL);
    SPP_DEV_OS *vp_ptr = 0;

    msg2->port = Parms->p.dun_disconnect_ind.port;

    SPPOS_sendMsg(MSG_ID_BT_DUN_DISCONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg2, NULL);

    // Deactivate if this is a Client port
    vp_ptr = getDev(Parms->p.dun_disconnect_cnf.port);
    if (vp_ptr->sppDev.portType & SPP_CLIENT_PORT)
    {
    	btmtk_dun_deactivate_req_struct *msg =
    	(btmtk_dun_deactivate_req_struct*) construct_local_para(sizeof(btmtk_dun_deactivate_req_struct), TD_UL);
    	msg->port = Parms->p.dun_disconnect_cnf.port;
	SPPOS_sendMsg(MSG_ID_BT_DUN_DEACTIVATE_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);
    }
}

void DUNAdpActivateCnf(SPPCallbackParms *Parms)
{
    btmtk_dun_activate_cnf_struct *msg;

    msg = (btmtk_dun_activate_cnf_struct*) construct_local_para(sizeof(btmtk_dun_activate_cnf_struct), TD_UL);
    msg->port = Parms->p.dun_activate_cnf.port;
    msg->result = Parms->p.dun_activate_cnf.result;

    SPPOS_sendMsg(MSG_ID_BT_DUN_ACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}

void DUNAdpDeactivateCnf(SPPCallbackParms *Parms)
{
    btmtk_dun_deactivate_cnf_struct *msg;

    msg = (btmtk_dun_deactivate_cnf_struct*) construct_local_para(sizeof(btmtk_dun_deactivate_cnf_struct), TD_UL);
    msg->port = Parms->p.dun_deactivate_cnf.port;
    msg->result = Parms->p.dun_deactivate_cnf.result;

    SPPOS_sendMsg(MSG_ID_BT_DUN_DEACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}

void DUNAdpDisconnectCnf(SPPCallbackParms *Parms)
{
    bt_dun_disconnect_cnf_struct *bt_dun_disconnect_cnf;
    SPP_DEV_OS *vp_ptr = 0;

    bt_dun_disconnect_cnf =
        (bt_dun_disconnect_cnf_struct*) construct_local_para(sizeof(bt_dun_disconnect_cnf_struct), TD_UL);
    bt_dun_disconnect_cnf->cid = Parms->p.dun_disconnect_cnf.cid;
    bt_dun_disconnect_cnf->port = Parms->p.dun_disconnect_cnf.port;
    bt_dun_disconnect_cnf->lap = Parms->p.dun_disconnect_cnf.lap;
    bt_dun_disconnect_cnf->uap = Parms->p.dun_disconnect_cnf.uap;
    bt_dun_disconnect_cnf->nap = Parms->p.dun_disconnect_cnf.nap;
    bt_dun_disconnect_cnf->result = Parms->p.dun_disconnect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_DUN_DISCONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_dun_disconnect_cnf, NULL);    

    // Deactivate if this is a Client port
    vp_ptr = getDev(Parms->p.dun_disconnect_cnf.port);
    if ((vp_ptr->sppDev.portType & SPP_CLIENT_PORT) && (SPP_DISCONNECT_SUCCESS == Parms->p.dun_disconnect_cnf.result))
    {
    	btmtk_dun_deactivate_req_struct *msg =
    	(btmtk_dun_deactivate_req_struct*) construct_local_para(sizeof(btmtk_dun_deactivate_req_struct), TD_UL);
    	msg->port = Parms->p.dun_disconnect_cnf.port;
	SPPOS_sendMsg(MSG_ID_BT_DUN_DEACTIVATE_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);
    }
}

void DUNAdpConnectCnf(SPPCallbackParms *Parms)
{
    bt_dun_connect_cnf_struct *bt_dun_connect_cnf;

    bt_dun_connect_cnf = (bt_dun_connect_cnf_struct*) construct_local_para(sizeof(bt_dun_connect_cnf_struct), TD_UL);
    bt_dun_connect_cnf->cid = Parms->p.dun_connect_cnf.cid;
    bt_dun_connect_cnf->port = Parms->p.dun_connect_cnf.port;
    bt_dun_connect_cnf->lap = Parms->p.dun_connect_cnf.lap;
    bt_dun_connect_cnf->uap = Parms->p.dun_connect_cnf.uap;
    bt_dun_connect_cnf->nap = Parms->p.dun_connect_cnf.nap;
    bt_dun_connect_cnf->server_chnl_num =  Parms->p.dun_connect_cnf.server_chnl_num;    
    bt_dun_connect_cnf->result = Parms->p.dun_connect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_DUN_CONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_dun_connect_cnf, NULL);    
}
#endif /* defined(__BT_DUN_PROFILE__) */

#if defined(__BT_FAX_PROFILE__)
/*APP <-- FAX*/
void FAXAdpConnectInd(SPPCallbackParms *Parms)
{
    bt_fax_connect_ind_struct *msg1 = 0;
    msg1 = (bt_fax_connect_ind_struct*) construct_local_para(sizeof(bt_fax_connect_ind_struct), TD_UL);

    msg1->port = Parms->p.fax_connect_ind.port;
    msg1->cid = Parms->p.fax_connect_ind.cid;
    msg1->lap = Parms->p.fax_connect_ind.lap;
    msg1->uap = Parms->p.fax_connect_ind.uap;
    msg1->nap = Parms->p.fax_connect_ind.nap;

    SPPOS_sendMsg(MSG_ID_BT_FAX_CONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg1, NULL);    
}

void FAXAdpConnectIndReq(SPPCallbackParms *Parms)
{
    bt_fax_connect_ind_req_struct *fax_con_ind_req = construct_local_para(sizeof(bt_fax_connect_ind_req_struct), TD_UL);

    fax_con_ind_req->lap = 0;
    fax_con_ind_req->lap = Parms->p.fax_connect_ind_req.lap;
    fax_con_ind_req->uap = 0;
    fax_con_ind_req->uap = Parms->p.fax_connect_ind_req.uap;
    fax_con_ind_req->nap = 0;
    fax_con_ind_req->nap = Parms->p.fax_connect_ind_req.nap;
    fax_con_ind_req->port = Parms->p.fax_connect_ind_req.port;
    SPPOS_sendMsg(
        MSG_ID_BT_FAX_CONNECT_IND_REQ,
        Parms->dstMod,
        BT_APP_SAP,
        (local_para_struct*) fax_con_ind_req,
        NULL);
}

void FAXAdpDisconnectInd(SPPCallbackParms *Parms)
{
    bt_fax_disconnect_ind_struct *msg2 = 
        (bt_fax_disconnect_ind_struct*) construct_local_para(sizeof(bt_fax_disconnect_ind_struct), TD_UL);
    SPP_DEV_OS *vp_ptr = 0;
    
    msg2->port = Parms->p.fax_disconnect_ind.port;

    SPPOS_sendMsg(MSG_ID_BT_FAX_DISCONNECT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg2, NULL);

    // Deactivate if this is a Client port
    vp_ptr = getDev(Parms->p.fax_disconnect_cnf.port);
    if (vp_ptr->sppDev.portType & SPP_CLIENT_PORT)
    {
    	btmtk_fax_deactivate_req_struct *msg =
    	(btmtk_fax_deactivate_req_struct*) construct_local_para(sizeof(btmtk_fax_deactivate_req_struct), TD_UL);
    	msg->port = Parms->p.fax_disconnect_cnf.port;
	SPPOS_sendMsg(MSG_ID_BT_FAX_DEACTIVATE_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);
    }
}

void FAXAdpActivateCnf(SPPCallbackParms *Parms)
{
    btmtk_fax_activate_cnf_struct *msg;

    msg = (btmtk_fax_activate_cnf_struct*) construct_local_para(sizeof(btmtk_fax_activate_cnf_struct), TD_UL);
    msg->port = Parms->p.fax_activate_cnf.port;
    msg->result = Parms->p.fax_activate_cnf.result;

    SPPOS_sendMsg(MSG_ID_BT_FAX_ACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}

void FAXAdpDeactivateCnf(SPPCallbackParms *Parms)
{
    btmtk_fax_deactivate_cnf_struct *msg;

    msg = (btmtk_fax_deactivate_cnf_struct*) construct_local_para(sizeof(btmtk_fax_deactivate_cnf_struct), TD_UL);
    msg->port = Parms->p.fax_deactivate_cnf.port;
    msg->result = Parms->p.fax_deactivate_cnf.result;

    SPPOS_sendMsg(MSG_ID_BT_FAX_DEACTIVATE_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);
}

void FAXAdpDisconnectCnf(SPPCallbackParms *Parms)
{
    bt_fax_disconnect_cnf_struct *bt_fax_disconnect_cnf;
    SPP_DEV_OS *vp_ptr = 0;

    bt_fax_disconnect_cnf =
        (bt_fax_disconnect_cnf_struct*) construct_local_para(sizeof(bt_fax_disconnect_cnf_struct), TD_UL);
    bt_fax_disconnect_cnf->cid = Parms->p.fax_disconnect_cnf.cid;
    bt_fax_disconnect_cnf->port = Parms->p.fax_disconnect_cnf.port;
    bt_fax_disconnect_cnf->lap = Parms->p.fax_disconnect_cnf.lap;
    bt_fax_disconnect_cnf->uap = Parms->p.fax_disconnect_cnf.uap;
    bt_fax_disconnect_cnf->nap = Parms->p.fax_disconnect_cnf.nap;
    bt_fax_disconnect_cnf->result = Parms->p.fax_disconnect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_FAX_DISCONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_fax_disconnect_cnf, NULL);    

    // Deactivate if this is a Client port
    vp_ptr = getDev(Parms->p.fax_disconnect_cnf.port);
    if ((vp_ptr->sppDev.portType & SPP_CLIENT_PORT) && (SPP_DISCONNECT_SUCCESS == Parms->p.fax_disconnect_cnf.result))
    {
    	btmtk_fax_deactivate_req_struct *msg =
    	(btmtk_fax_deactivate_req_struct*) construct_local_para(sizeof(btmtk_fax_deactivate_req_struct), TD_UL);
    	msg->port = Parms->p.fax_disconnect_cnf.port;
	SPPOS_sendMsg(MSG_ID_BT_FAX_DEACTIVATE_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);
    }
}

void FAXAdpConnectCnf(SPPCallbackParms *Parms)
{
    bt_fax_connect_cnf_struct *bt_fax_connect_cnf;

    bt_fax_connect_cnf = (bt_fax_connect_cnf_struct*) construct_local_para(sizeof(bt_fax_connect_cnf_struct), TD_UL);
    bt_fax_connect_cnf->cid = Parms->p.fax_connect_cnf.cid;
    bt_fax_connect_cnf->port = Parms->p.fax_connect_cnf.port;
    bt_fax_connect_cnf->lap = Parms->p.fax_connect_cnf.lap;
    bt_fax_connect_cnf->uap = Parms->p.fax_connect_cnf.uap;
    bt_fax_connect_cnf->nap = Parms->p.fax_connect_cnf.nap;
    bt_fax_connect_cnf->server_chnl_num =  Parms->p.fax_connect_cnf.server_chnl_num;    
    bt_fax_connect_cnf->result = Parms->p.fax_connect_cnf.result;
    SPPOS_sendMsg(MSG_ID_BT_FAX_CONNECT_CNF, Parms->dstMod, BT_APP_SAP, (local_para_struct*) bt_fax_connect_cnf, NULL);    
}
#endif /* defined(__BT_FAX_PROFILE__) */

/* UART */
void UARTAdpPlugoutInd(SPPCallbackParms *Parms)
{
#ifdef BTMTK_ON_WISE     /* SPP_PORTING */ 
    uart_plugout_ind_struct *msg = 
        (uart_plugout_ind_struct*) construct_local_para(sizeof(uart_plugout_ind_struct), TD_UL);
    msg->port = Parms->p.uart_plugout_ind.port;

    SPPOS_sendMsg(MSG_ID_UART_PLUGOUT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
#else   /* BTMTK_ON_WISE */    
    
    btmtk_spp_uart_plugout_ind_struct *msg = 
        (btmtk_spp_uart_plugout_ind_struct*) construct_local_para(sizeof(btmtk_spp_uart_plugout_ind_struct), TD_UL);
    msg->port = Parms->p.uart_plugout_ind.port;
    msg->profile = Parms->p.uart_plugout_ind.profile;

    SPPOS_sendMsg(MSG_ID_BT_SPP_UART_PLUGOUT_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
#endif  /* BTMTK_ON_WISE */    
}

void UARTAdpReadyToReadInd(SPPCallbackParms *Parms)
{
#ifdef BTMTK_ON_WISE     /* SPP_PORTING */ 
    uart_ready_to_read_ind_struct *msg = 
        (uart_ready_to_read_ind_struct*) construct_local_para(sizeof(uart_ready_to_read_ind_struct), TD_UL);
    msg->port = Parms->p.uart_ready_to_read.port;
    
    SPPOS_sendMsg(MSG_ID_UART_READY_TO_READ_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
#else   /* BTMTK_ON_WISE */    
    btmtk_spp_uart_ready_to_read_ind_struct *msg = 
        (btmtk_spp_uart_ready_to_read_ind_struct*) construct_local_para(sizeof(btmtk_spp_uart_ready_to_read_ind_struct), TD_UL);
    msg->port = Parms->p.uart_ready_to_read.port;
    msg->profile = Parms->p.uart_ready_to_read.profile;

    SPPOS_sendMsg(MSG_ID_BT_SPP_UART_READY_TO_READ_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
#endif  /* BTMTK_ON_WISE */    
}

void UARTAdpReadyToWriteInd(SPPCallbackParms *Parms)
{
#ifdef BTMTK_ON_WISE     /* SPP_PORTING2 */     
    uart_ready_to_write_ind_struct *msg = 
        (uart_ready_to_write_ind_struct*) construct_local_para(sizeof(uart_ready_to_write_ind_struct), TD_UL);
    msg->port = Parms->p.uart_ready_to_write.port;

    SPPOS_sendMsg(MSG_ID_UART_READY_TO_WRITE_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
#else   /* BTMTK_ON_WISE */    
    btmtk_spp_uart_ready_to_write_ind_struct *msg = 
        (btmtk_spp_uart_ready_to_write_ind_struct*) construct_local_para(sizeof(btmtk_spp_uart_ready_to_write_ind_struct), TD_UL);
    msg->port = Parms->p.uart_ready_to_write.port;
    msg->profile = Parms->p.uart_ready_to_write.profile;

    SPPOS_sendMsg(MSG_ID_BT_SPP_UART_READY_TO_WRITE_IND, Parms->dstMod, BT_APP_SAP, (local_para_struct*) msg, NULL);    
#endif  /* BTMTK_ON_WISE */        
}


void SPPAdpInitializeCnf(SPPCallbackParms *Parms)
{
	SPPOS_sendMsg(MSG_ID_BT_SPP_INITIALIZE_CNF, MOD_MMI, BT_APP_SAP, NULL, NULL);    
}


#if 0
void SPPAdpRegisterCallbackCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_register_callback_req_struct *msg = 
		(btmtk_spp_register_callback_req_struct*) construct_local_para(sizeof(btmtk_spp_register_callback_req_struct), TD_UL);

	msg->port = Parms->p.spp_register_callback_cnf.port;
	msg->result= Parms->p.spp_register_callback_cnf.result;

	SPPOS_sendMsg(MSG_ID_BT_SPP_REGISTER_CALLBACK_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}
#endif


void SPPUARTPutBytesCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_put_bytes_cnf_struct *msg = 
		(btmtk_spp_uart_put_bytes_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_put_bytes_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_put_bytes_cnf.port;
	msg->length= Parms->p.spp_uart_put_bytes_cnf.length;
	msg->profile= Parms->p.spp_uart_put_bytes_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_PUT_BYTES_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTGetBytesCnfSend(SPPCallbackParms *Parms, U16 lengthSent, U16 lengthToSend)
{
	btmtk_spp_uart_get_bytes_cnf_struct *msg = 
		(btmtk_spp_uart_get_bytes_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_get_bytes_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_get_bytes_cnf.port;
	msg->profile = Parms->p.spp_uart_get_bytes_cnf.profile;
	msg->length = lengthToSend;
#ifdef __SPP_SHARED_MEMORY__
	msg->buffer = Parms->p.spp_uart_get_bytes_cnf.buffer;
#else
	btmtk_os_memcpy(msg->buffer, (Parms->p.spp_uart_get_bytes_cnf.buffer + lengthSent), lengthToSend);
#endif	/* __SPP_SHARED_MEMORY__ */
	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_GET_BYTES_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


void SPPUARTGetBytesCnf(SPPCallbackParms *Parms)
{
	U16 lengthSent;
	U16 lengthToSend;
	U16 maxBufferLength = 0;

	lengthSent = 0;
	lengthToSend = 0;

#ifdef __SPP_SHARED_MEMORY__
	 maxBufferLength = SPP_GET_DATA_SHARED_BUFFER_SIZE;
#else
	 maxBufferLength = SPP_ADP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef __SPP_SHARED_MEMORY__
	if (Parms->p.spp_uart_get_bytes_cnf.length > 0)
#else
	while ((Parms->p.spp_uart_get_bytes_cnf.length - lengthSent) > 0)
#endif	/* __SPP_SHARED_MEMORY__ */
	{
		if ((Parms->p.spp_uart_get_bytes_cnf.length - lengthSent) > maxBufferLength)
		{
			lengthToSend = maxBufferLength;
		}
		else
		{
			lengthToSend = Parms->p.spp_uart_get_bytes_cnf.length - lengthSent;
		}
		SPPUARTGetBytesCnfSend(Parms, lengthSent, lengthToSend);
		lengthSent += lengthToSend;
	}
}


void SPPUARTOpenCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_open_cnf_struct *msg = 
		(btmtk_spp_uart_open_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_open_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_open_cnf.port;
	msg->result= Parms->p.spp_uart_open_cnf.result;
	//msg->profile= Parms->p.spp_uart_open_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_OPEN_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTCloseCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_close_cnf_struct *msg = 
		(btmtk_spp_uart_close_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_close_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_close_cnf.port;
	//msg->profile = Parms->p.spp_uart_close_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_CLOSE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTGetBytesAvailCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_get_bytes_avail_cnf_struct *msg = 
		(btmtk_spp_uart_get_bytes_avail_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_get_bytes_avail_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_get_bytes_avail_cnf.port;
	msg->length= Parms->p.spp_uart_get_bytes_avail_cnf.length;
	//msg->profile= Parms->p.spp_uart_get_bytes_avail_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_GET_BYTES_AVAIL_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTTxRoomLeftCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_get_tx_room_left_cnf_struct *msg = 
		(btmtk_spp_uart_get_tx_room_left_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_get_tx_room_left_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_get_tx_room_left_cnf.port;
	msg->length= Parms->p.spp_uart_get_tx_room_left_cnf.length;
	//msg->profile= Parms->p.spp_uart_get_tx_room_left_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_GET_TX_ROOM_LEFT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTSetOwnerCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_set_owner_cnf_struct *msg = 
		(btmtk_spp_uart_set_owner_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_set_owner_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_set_owner_cnf.port;
	//msg->profile = Parms->p.spp_uart_set_owner_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_SET_OWNER_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTCtrlDcdCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_ctrl_dcd_cnf_struct *msg = 
		(btmtk_spp_uart_ctrl_dcd_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_ctrl_dcd_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_ctrl_dcd_cnf.port;
	//msg->profile = Parms->p.spp_uart_ctrl_dcd_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_CTRL_DCD_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTCtrlBreakCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_ctrl_dcd_cnf_struct *msg = 
		(btmtk_spp_uart_ctrl_dcd_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_ctrl_dcd_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_ctrl_break_cnf.port;
	//msg->profile = Parms->p.spp_uart_ctrl_break_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_CTRL_BREAK_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTGetOwnerIDCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_get_owner_id_cnf_struct *msg = 
		(btmtk_spp_uart_get_owner_id_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_get_owner_id_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_get_owner_id_cnf.port;
	//msg->owner= Parms->p.spp_uart_get_owner_id_cnf.owner;
	//msg->profile = Parms->p.spp_uart_get_owner_id_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_GET_OWNER_ID_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTClrRxBufferCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_clr_rx_buffer_cnf_struct *msg = 
		(btmtk_spp_uart_clr_rx_buffer_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_clr_rx_buffer_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_clr_rx_buffer_cnf.port;
	//msg->profile= Parms->p.spp_uart_clr_rx_buffer_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_CLR_RX_BUFFER_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTClrTxBufferCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_clr_tx_buffer_cnf_struct *msg = 
		(btmtk_spp_uart_clr_tx_buffer_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_clr_tx_buffer_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_clr_tx_buffer_cnf.port;
	//msg->profile = Parms->p.spp_uart_clr_tx_buffer_cnf.profile;

	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_CLR_TX_BUFFER_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);    
}


void SPPUARTDataAvailableIndSend(SPPCallbackParms *Parms, U16 lengthSent, U16 lengthToSend)
{
	btmtk_spp_uart_data_available_ind_struct *msg = 
		(btmtk_spp_uart_data_available_ind_struct*) construct_local_para(sizeof(btmtk_spp_uart_data_available_ind_struct), TD_UL);

	msg->port = Parms->p.spp_uart_data_available_ind.port;
	msg->profile = Parms->p.spp_uart_data_available_ind.profile;
	msg->length = lengthToSend;
#ifdef __SPP_SHARED_MEMORY__
	msg->buffer = Parms->p.spp_uart_data_available_ind.buffer;
#else
	btmtk_os_memcpy(msg->buffer, (Parms->p.spp_uart_data_available_ind.buffer + lengthSent), lengthToSend);
#endif	/* __SPP_SHARED_MEMORY__ */
	SPPOS_sendMsg(MSG_ID_BT_SPP_UART_DATA_AVAILABLE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
	
}


void SPPUARTDataAvailableInd(SPPCallbackParms *Parms)
{
	U16 lengthSent;
	U16 lengthToSend;
	U16 maxBufferLength = 0;

	lengthSent = 0;
	lengthToSend = 0;

#ifdef __SPP_SHARED_MEMORY__
	 maxBufferLength = SPP_GET_DATA_SHARED_BUFFER_SIZE;
#else
	 maxBufferLength = SPP_ADP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef __SPP_SHARED_MEMORY__
	if (Parms->p.spp_uart_data_available_ind.length > 0)
#else
	while ((Parms->p.spp_uart_data_available_ind.length - lengthSent) > 0)
#endif	/* __SPP_SHARED_MEMORY__ */
	{
		if ((Parms->p.spp_uart_data_available_ind.length - lengthSent) > maxBufferLength)
		{
			lengthToSend = maxBufferLength;
		}
		else
		{
			lengthToSend = Parms->p.spp_uart_data_available_ind.length - lengthSent;
		}
		SPPUARTDataAvailableIndSend(Parms, lengthSent, lengthToSend);
		lengthSent += lengthToSend;
	}
	
}


/*****************************************************************************
 * FUNCTION
 *  sppa_sendData
 * DESCRIPTION
 *  send data to SPP
 * PARAMETERS
 *  owner       [IN]        
 *  port        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
void sppa_sendData(module_type owner, UART_PORT port)
{
    ilm_struct *ilm;
    bt_spp_send_data_req_struct *pLocal;

    ilm = allocate_ilm(owner);
    pLocal = (bt_spp_send_data_req_struct*) construct_local_para(sizeof(bt_spp_send_data_req_struct), TD_UL);

    pLocal->port = port;

    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->src_mod_id = owner;
    ilm->msg_id = MSG_ID_BT_SPP_SEND_DATA_REQ;
    ilm->peer_buff_ptr = NULL;
    ilm->local_para_ptr = (local_para_struct*) pLocal;
    msg_send_ext_queue(ilm);    
}
#else   /* BTMTK_ON_WISE */
void sppa_sendData(SPPCallbackParms *Parms)
{
	btmtk_spp_send_data_req_struct *msg=
		(btmtk_spp_send_data_req_struct*) construct_local_para(sizeof(btmtk_spp_send_data_req_struct), TD_UL);
		
	msg->port = Parms->p.spp_profile_send_data_req.port;
	SPPOS_sendMsg(MSG_ID_BT_SPP_SEND_DATA_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);    
}
#endif  /* BTMTK_ON_WISE */        

/*****************************************************************************
 * FUNCTION
 *  sppa_getData
 * DESCRIPTION
 *  get data from SPP
 * PARAMETERS
 *  owner       [IN]        
 *  port        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
void sppa_getData(module_type owner, UART_PORT port)
{
    ilm_struct *ilm;
    bt_spp_get_data_req_struct *pLocal;

    ilm = allocate_ilm(owner);
    pLocal = (bt_spp_get_data_req_struct*) construct_local_para(sizeof(bt_spp_get_data_req_struct), TD_UL);

    pLocal->port = port;

    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->src_mod_id = owner;
    ilm->msg_id = MSG_ID_BT_SPP_GET_DATA_REQ;
    ilm->peer_buff_ptr = NULL;
    ilm->local_para_ptr = (local_para_struct*) pLocal;
    msg_send_ext_queue(ilm);

}
#else   /* BTMTK_ON_WISE */
void sppa_getData(SPPCallbackParms *Parms)
{
	btmtk_spp_get_data_req_struct *msg=
		(btmtk_spp_get_data_req_struct*) construct_local_para(sizeof(btmtk_spp_get_data_req_struct), TD_UL);
		
	msg->port = Parms->p.spp_profile_get_data_req.port;
	SPPOS_sendMsg(MSG_ID_BT_SPP_GET_DATA_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);    
}
#endif  /* BTMTK_ON_WISE */


#ifdef __BT_HFG_PROFILE__
/*****************************************************************************
 * FUNCTION
 *  hfa_sendData
 * DESCRIPTION
 *  send data to SPP
 * PARAMETERS
 *  owner       [IN]        
 *  port        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void hfa_sendData(SPPCallbackParms *Parms)
{
	bt_hf_send_data_req_struct *msg=
		(bt_hf_send_data_req_struct*) construct_local_para(sizeof(bt_hf_send_data_req_struct), TD_UL);
		
	msg->port = Parms->p.hf_send_data_req.port;
	SPPOS_sendMsg(MSG_ID_BT_HFG_SEND_DATA_REQ, MOD_BT, 0, (local_para_struct*) msg, NULL);    
}


/*---------------------------------------------------------------------------
 * if last 2 bytes are 0x0D, 0x0A, it is completed string
 * 
 *
 * Context: 
 *    Args: ring - pointer to RingBuf struct
 *    Return: TRUE - completed string 
 */

/*****************************************************************************
 * FUNCTION
 *  hf_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ring        [IN]     
 * RETURNS
 *  
 *****************************************************************************/
kal_bool hf_string(RingBuf *ring)
{
    kal_bool ret = KAL_FALSE;

    if (ring->pWrite >= (ring->pStart + 2))
    {
        if ((*(ring->pWrite - 2) == 0x0D) && (*(ring->pWrite - 1) == 0x0A))
        {
            ret = KAL_TRUE;
        }
    }
    else if (ring->pWrite == (ring->pStart + 1))
    {
        if ((*(ring->pEnd - 1) == 0x0D) && (*ring->pStart == 0x0A))
        {
            ret = KAL_TRUE;
        }
    }
    else if (ring->pWrite == ring->pStart)
    {
        if ((*(ring->pEnd - 2) == 0x0D) && (*(ring->pEnd - 1) == 0x0A))
        {
            ret = KAL_TRUE;
        }
    }
    else
    {
        ret = KAL_FALSE;
    }
    return ret;
}

#endif /* __BT_HFG_PROFILE__ */ 


void SPPAdpEnableCnf(SPPCallbackParms *Parms)
{
    SPPOS_sendMsg(MSG_ID_BT_SPP_ENABLE_CNF, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


void SPPAdpDisableCnf(SPPCallbackParms *Parms)
{
    SPPOS_sendMsg(MSG_ID_BT_SPP_DISABLE_CNF, Parms->dstMod, BT_APP_SAP, NULL, NULL);
}


#ifdef __SPP_SHARED_MEMORY__
void SPPUARTAssignBufferCnf(SPPCallbackParms *Parms)
{
	btmtk_spp_uart_assign_buffer_cnf_struct *msg = 
		(btmtk_spp_uart_assign_buffer_cnf_struct*) construct_local_para(sizeof(btmtk_spp_uart_assign_buffer_cnf_struct), TD_UL);

	msg->port = Parms->p.spp_uart_assign_buffer_cnf.port;
	
	SPPOS_sendMsg(MSG_ID_BT_UART_ASSIGN_BUFFER_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);
}
#endif	/* __SPP_SHARED_MEMORY__ */


void btmtk_adp_spp_int_event_callback(SPPCallbackParms *Parms)
{
    switch (Parms->event)        
    {
        case SPP_EVENT_CONNECT_IND:            
            SPPAdpConnectInd(Parms);
            break;
        case SPP_EVENT_CONNECT_IND_REQ:
            SPPAdpConnectIndReq(Parms);
            break;         
        case SPP_EVENT_DISCONNECT_IND:
            Report(("spp_app_callback dstmod=%d", Parms->dstMod));
            SPPAdpDisconnectInd(Parms);
            break;          
        case SPP_EVENT_ACTIVATE_CNF:
            SPPAdpActivateCnf(Parms);
            break;          
        case SPP_EVENT_DEACTIVATE_CNF:
            SPPAdpDeactivateCnf(Parms);
            break;        
        case SPP_EVENT_DISCONNECT_CNF:
            SPPAdpDisconnectCnf(Parms);
            break;          
        case SPP_EVENT_AUTH_REQ:
            SPPAdpAuthReq(Parms);
            break;          
        case SPP_EVENT_AUDIO_CONNECT_CNF:
            SPPAdpAudioConnectCnf(Parms);
            break;          
        case SPP_EVENT_AUDIO_CONNECT_IND:
            SPPAdpAudioConnectInd(Parms);
            break;          
        case SPP_EVENT_AUDIO_DISCONNECT_CNF:
            SPPAdpAudioDisconnectCnf(Parms);
            break;          
        case SPP_EVENT_AUDIO_DISCONNECT_IND:
            SPPAdpAudioDisconnectInd(Parms);
            break;          
        case SPP_EVENT_SCO_RSSI_IND:
            SPPAdpScoRssiInd(Parms);
            break;          
        case SPP_EVENT_CONNECT_CNF:
            SPPAdpConnectCnf(Parms);
            break;          
        case SPP_EVENT_UART_OWNER_IND:
            SPPAdpUartOwnerInd(Parms);
            break;      

#if defined(__BT_DUN_PROFILE__)
        /*APP <-- DUN*/
        case DUN_EVENT_CONNECT_IND:
            DUNAdpConnectInd(Parms);
            break;
        case DUN_EVENT_CONNECT_IND_REQ:
            DUNAdpConnectIndReq(Parms);
            break;
        case DUN_EVENT_DISCONNECT_IND:
            DUNAdpDisconnectInd(Parms);
            break;
        case DUN_EVENT_ACTIVATE_CNF:
            DUNAdpActivateCnf(Parms);
            break;
        case DUN_EVENT_DEACTIVATE_CNF:
            DUNAdpDeactivateCnf(Parms);
            break;
        case DUN_EVENT_DISCONNECT_CNF:
            DUNAdpDisconnectCnf(Parms);
            break;   
        case DUN_EVENT_CONNECT_CNF:
            DUNAdpConnectCnf(Parms);
            break;                      
#endif /* __BT_DUN_PROFILE__ */

#if defined(__BT_FAX_PROFILE__)
        /*APP <-- FAX*/
        case FAX_EVENT_CONNECT_IND:
            FAXAdpConnectInd(Parms);
            break;
        case FAX_EVENT_CONNECT_IND_REQ:
            FAXAdpConnectIndReq(Parms);
            break;
        case FAX_EVENT_DISCONNECT_IND:
            FAXAdpDisconnectInd(Parms);
            break;
        case FAX_EVENT_ACTIVATE_CNF:
            FAXAdpActivateCnf(Parms);
            break;
        case FAX_EVENT_DEACTIVATE_CNF:
            FAXAdpDeactivateCnf(Parms);
            break;
        case FAX_EVENT_DISCONNECT_CNF:
            FAXAdpDisconnectCnf(Parms);
            break;   
        case FAX_EVENT_CONNECT_CNF:
            FAXAdpConnectCnf(Parms);
            break;                      
#endif /* __BT_FAX_PROFILE__ */

        /* UART */
        case UART_EVENT_PLUGOUT_IND:
            UARTAdpPlugoutInd(Parms);
            break;
        case UART_EVENT_READY_TO_READ_IND:
            UARTAdpReadyToReadInd(Parms);
            break;
        case UART_EVENT_READY_TO_WRITE_IND:
            UARTAdpReadyToWriteInd(Parms);
            break;    

	 /* SW Partition*/
	 case SPP_UART_EVENT_INITIALIZE_CNF:
		SPPAdpInitializeCnf(Parms);
	     break;
#if 0	     
	 case SPP_UART_EVENT_REGISTER_CALLBACK_CNF:
		SPPAdpRegisterCallbackCnf(Parms);
	     break;
#endif

	 /* UART synchronous API event */
	 case SPP_UART_EVENT_PUT_BYTES_CNF:
		SPPUARTPutBytesCnf(Parms);
	     break;
	 case SPP_UART_EVENT_GET_BYTES_CNF:
		SPPUARTGetBytesCnf(Parms);
	     break;
	 case SPP_UART_EVENT_OPEN_CNF:
		SPPUARTOpenCnf(Parms);
	     break;
	 case SPP_UART_EVENT_CLOSE_CNF:
		SPPUARTCloseCnf(Parms);
	     break;
	 case SPP_UART_EVENT_GET_BYTES_AVAIL_CNF:
		SPPUARTGetBytesAvailCnf(Parms);
	     break;
	 case SPP_UART_EVENT_GET_TX_ROOM_LEFT_CNF:
		SPPUARTTxRoomLeftCnf(Parms);
	     break;
	 case SPP_UART_EVENT_SET_OWNER_CNF:
		SPPUARTSetOwnerCnf(Parms);
	     break;
	 case SPP_UART_EVENT_CTRL_DCD_CNF:
		SPPUARTCtrlDcdCnf(Parms);
	     break;
	 case SPP_UART_EVENT_CTRL_BREAK_CNF:
		SPPUARTCtrlBreakCnf(Parms);
	     break;
	 case SPP_UART_EVENT_GET_OWNER_ID_CNF:
		SPPUARTGetOwnerIDCnf(Parms);
	     break;
	 case SPP_UART_EVENT_CLR_RX_BUFFER_CNF:
		SPPUARTClrRxBufferCnf(Parms);
	     break;
	 case SPP_UART_EVENT_CLR_TX_BUFFER_CNF:
		SPPUARTClrTxBufferCnf(Parms);
	     break;

	/* SPP <-- SPP */
    #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined (BTMTK_ON_LINUX)/*SPP_PORTING*/   
	 case SPP_EVENT_PROFILE_SEND_DATA_REQ:
	 	sppa_sendData(Parms);
	 	break;
	 case SPP_EVENT_PROFILE_GET_DATA_REQ:
	 	sppa_getData(Parms);
	 	break;
    #endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */        

	 /* Customization for WISE */
	 case SPP_EVENT_ENABLE_CNF:
	 	SPPAdpEnableCnf(Parms);
	 	break;
	 case SPP_EVENT_DISABLE_CNF:
	 	SPPAdpDisableCnf(Parms);
	 	break;
	 case SPP_UART_EVENT_DATA_AVAILABLE_IND:
	 	SPPUARTDataAvailableInd(Parms);
	 	break;
#ifdef __SPP_SHARED_MEMORY__
	 case SPP_UART_EVENT_ASSIGN_BUFFER_CNF:
	 	SPPUARTAssignBufferCnf(Parms);
	 	break;
#endif	/* __SPP_SHARED_MEMORY__ */
        default:
            Assert(0);
    }
}



