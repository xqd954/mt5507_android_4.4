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
 * bt_adp_prx.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth MMI OPP server
 *
 * Author:
 * -------
 * Daylong Chen
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
#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)

#include "bttypes.h"
#include "bt_common.h"

#include "bluetooth_struct.h"
#include "bluetooth_prx_struct.h"

#include "prxr.h"
#include "prxm.h"
#include "prxadapter.h"

PrxRApp g_prxrapp[PRX_MAX_REPORTER_NUM];
PrxMApp g_prxmapp[PRX_MAX_MONITOR_NUM];

void adpprxrcallback(PrxREvent *event, void *apdata);

/* */
void prx_send_msg(msg_type msg_id, local_para_struct *local_para,
		peer_buff_struct *peer_buff) {
	module_type dst_id = MOD_MMI;

	if (local_para != NULL) {
		BT_SendMessage(msg_id, dst_id, local_para, local_para->msg_len);
	} else {
		BT_SendMessage(msg_id, dst_id, NULL, 0);
	}
}

/**
 * @brief send the register result
 */
void send_prxr_register_cnf(U8 index, U8 rspcode) {
	bt_prxr_register_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxr_register_cnf_struct), TD_CTRL);

	cnf->index = index;
	cnf->rspcode = rspcode;

	prx_send_msg((U16) MSG_ID_BT_PRXR_REGISTER_CNF, (local_para_struct *) cnf,
			NULL );

}

void send_prxr_authorize_ind(U8 index, U8 *addr) {
	bt_prxr_authorize_ind_struct *ind;
	ind = construct_local_para(sizeof(bt_prxr_authorize_ind_struct), TD_CTRL);

	ind->index = index;
	memset(ind->addr, 0, 6);
	if( NULL != addr ){
		// copy address to
		memcpy(ind->addr, addr, 6);
	}

	prx_send_msg((U16) MSG_ID_BT_PRXR_AUTHORIZE_IND, (local_para_struct *) ind,
			NULL );
}

void send_prxr_connect_ind(U8 index, U8 *addr) {
	bt_prxr_connect_ind_struct *ind;
	ind = construct_local_para(sizeof(bt_prxr_connect_ind_struct), TD_CTRL);

	ind->index = index;
	memset(ind->addr, 0, 6);
	if( NULL != addr ){
		// copy address to
		memcpy(ind->addr, addr, 6);
	}

	prx_send_msg((U16) MSG_ID_BT_PRXR_CONNECT_IND, (local_para_struct *) ind,
			NULL );
}

void send_prxr_get_capability_ind(U8 index, U8 *addr) {
	// no such indication event!
}

void send_prxr_pathloss_ind(U8 index, U8 level) {
	bt_prxr_pathloss_ind_struct *ind;
	ind = construct_local_para(sizeof(bt_prxr_pathloss_ind_struct), TD_CTRL);

	ind->index = index;
	ind->level = level;

	prx_send_msg((U16) MSG_ID_BT_PRXR_PATHLOSS_IND, (local_para_struct *) ind,
			NULL );
}

void send_prxr_linkloss_ind(U8 index, U8 level) {
	bt_prxr_linkloss_ind_struct *ind;
	ind = construct_local_para(sizeof(bt_prxr_linkloss_ind_struct), TD_CTRL);

	ind->index = index;
	ind->level = level;

	prx_send_msg((U16) MSG_ID_BT_PRXR_LINKLOSS_IND, (local_para_struct *) ind,
			NULL );
}

void send_prxr_disconnect_ind(U8 index, U8 rspcode) {
	bt_prxr_disconnect_ind_struct *ind;
	ind = construct_local_para(sizeof(bt_prxr_disconnect_ind_struct), TD_CTRL);

	ind->index = index;
	ind->rspcode = rspcode;

	// TODO: check the current link loss level

	prx_send_msg((U16) MSG_ID_BT_PRXR_DISCONNECT_IND,
			(local_para_struct *) ind, NULL );
}

/**
 * @brief send the register result
 */
void send_prxr_deregister_cnf(U8 index, U8 rspcode) {
	bt_prxr_deregister_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxr_deregister_cnf_struct), TD_CTRL);

	cnf->index = index;
	cnf->rspcode = rspcode;

	prx_send_msg((U16) MSG_ID_BT_PRXR_DEREGISTER_CNF,
			(local_para_struct *) cnf, NULL );

}

void send_prxr_disconnect_cnf(U8 index, U8 rspcode) {
	bt_prxr_disconnect_ind_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxr_disconnect_ind_struct), TD_CTRL);

	cnf->index = index;

	prx_send_msg((U16) MSG_ID_BT_PRXR_DISCONNECT_IND,
			(local_para_struct *) cnf, NULL );
}

void send_prxr_txpower_cnf(U8 index, U8 rspcode, S8 txpower) {
	bt_prxr_update_txpower_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxr_update_txpower_cnf_struct),
			TD_CTRL);

	cnf->index = index;
	cnf->txpower = txpower;

	prx_send_msg((U16) MSG_ID_BT_PRXR_UPDATE_TXPOWER_CNF,
			(local_para_struct *) cnf, NULL );
}

/* */
void prxr_register_req_hndl(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_prxr_register_req_struct *req;
	req = (bt_prxr_register_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= PRX_MAX_REPORTER_NUM) {
		send_prxr_register_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	if (g_prxrapp[req->index].isReg == 1) {
		send_prxr_register_cnf(req->index, BT_STATUS_IN_USE); // The register has been success

		// error handling
		PRX_DBG("[PRX][WRN] has been register!");
		status = PRXR_Disconnect(&g_prxrapp[req->index]);
		PRX_DBG1("[PRX][WRN] has been register disconnect it status:%d", status);
		return;
	}

	// prepare the data
	g_prxrapp[req->index].id = req->index;
	g_prxrapp[req->index].isReg = 1;

	/* use global data to regist ther proximity channel */
	if (BT_STATUS_SUCCESS == PRXR_Register(&g_prxrapp[req->index], NULL,
			adpprxrcallback)) {
		// ok
		send_prxr_register_cnf(req->index, BT_STATUS_SUCCESS);
	} else {
		// fail
		send_prxr_register_cnf(req->index, BT_STATUS_FAILED);
		g_prxrapp[req->index].isReg = 0; // back to unreg state
	}
}

void prxr_deregister_req_hndl(ilm_struct *ilm_ptr) {
	bt_prxr_deregister_req_struct *req;
	BtStatus status;
	req = (bt_prxr_deregister_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= PRX_MAX_REPORTER_NUM) {
		send_prxr_deregister_cnf(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	if (g_prxrapp[req->index].isReg != 1) {
		send_prxr_deregister_cnf(req->index, BT_STATUS_NOSERVICES);
		return;
	}

	/* use global data to regist ther proximity channel */
	status = PRXR_Deregister(&g_prxrapp[req->index]);
	if (BT_STATUS_SUCCESS == status) {
		// ok
		send_prxr_deregister_cnf(req->index, BT_STATUS_SUCCESS);
	} else {
		// fail
		send_prxr_deregister_cnf(req->index, BT_STATUS_FAILED);
		PRX_DBG1("[PRX] deregister fail! status:%d", status);
	}

	// anyway, back to unreg state
	g_prxrapp[req->index].isReg = 0;
}

void prxr_authorize_rsp_hndl(ilm_struct *ilm_ptr) {
	bt_prxr_authorize_rsp_struct *rsp;
	BtStatus status;
	rsp = (bt_prxr_authorize_rsp_struct *) ilm_ptr->ilm_data;
	if (rsp->index >= PRX_MAX_REPORTER_NUM) {
		PRX_DBG1("[PRX] bt_prxr_authorize_rsp_struct ignore %d", rsp->index);
		return;
	}

	/* use global data to regist ther proximity channel */
	status = PRXR_AcceptIncoming(&g_prxrapp[rsp->index], rsp->rspcode);
	switch( status ) {
		case BT_STATUS_SUCCESS:
			send_prxr_connect_ind(g_prxrapp[rsp->index].id, ((PrxRContext *)g_prxrapp[rsp->index].prxContext)->addr );
			break;
		case BT_STATUS_PENDING:
		// ok. wait for the connected event
			break;
		default:
			// fail. disconnect it
			PRX_DBG1("[PRX][ERR] fail to accept it status:%d", status);
			PRXR_Disconnect(&g_prxrapp[rsp->index]);
		break;
	}
	/* no back message */
}

void prxr_disconnect_req_hndl(ilm_struct *ilm_ptr) {
	bt_prxr_disconnect_req_struct *req;
	BtStatus status;

	req = (bt_prxr_disconnect_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= PRX_MAX_REPORTER_NUM) {
		send_prxr_disconnect_cnf(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	if (g_prxrapp[req->index].isReg != 1) {
		send_prxr_disconnect_cnf(req->index, BT_STATUS_NOSERVICES);
		return;
	}

	PRX_DBG1("prxr_disconnect_req_hndl ap:0x%x", &g_prxrapp[req->index]);

	if( BT_STATUS_SUCCESS == PRXR_IsDisconnecting(&g_prxrapp[req->index])){
		PRX_DBG("[PRX][WRN] isDisconnecting ignore it!");
		return;
	}

	status = PRXR_Disconnect( &g_prxrapp[req->index] );

	switch(status){
		case BT_STATUS_SUCCESS:
			send_prxr_disconnect_ind(req->index, BT_STATUS_SUCCESS);
			//TODO: recycle the apdata
			break;
		case BT_STATUS_PENDING:
			// wait for callback
			break;
		default:
			send_prxr_disconnect_ind(req->index, status);
			break;
	}
}

void prxr_update_txpower_req_hndl(ilm_struct *ilm_ptr) {
	bt_prxr_update_txpower_req_struct *req;
	PrxRApp *apdata;
	BtStatus status;

	req = (bt_prxr_update_txpower_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= PRX_MAX_REPORTER_NUM) {
		send_prxr_txpower_cnf(req->index, BT_STATUS_NOT_FOUND, PRX_INVALID_TXPOWER); // param1 is value
		return;
	}

	if (g_prxrapp[req->index].isReg != 1) {
		send_prxr_txpower_cnf(req->index, BT_STATUS_NOSERVICES, PRX_INVALID_TXPOWER); // param1 is value
		return;
	}

	apdata = &g_prxrapp[req->index];
	status = PRXR_UpdateTxPower(apdata, 0);
	if (BT_STATUS_SUCCESS == status) {
		send_prxr_txpower_cnf(req->index, status, PRXR_GetCurrentTxPower(apdata));
	} else if (BT_STATUS_PENDING == status) {
		// wait callback
	} else {
		send_prxr_txpower_cnf(req->index, status, PRX_INVALID_TXPOWER);
	}
}

/**
 * @brief prxr will use to notify result
 * @param apdata the PrxrApp data
 */
void adpprxrcallback(PrxREvent *event, void *apdata) {
	PrxRApp *app = (PrxRApp *) apdata;
	S32 data = (S32)event->data;
	/* this is a call back from prxr level */
	PRX_DBG3("[PRX] adpprxrcallback event:%d apdata:0x%x idx:%d", event->event, apdata, app->id );

	switch(event->event){
	case PRXRE_DISCONNECTED:
		send_prxr_disconnect_ind(app->id, data);
		break;
	case PRXRE_INCOMING:
		send_prxr_authorize_ind(app->id, (U8 *)data);
		break;
	case PRXRE_CONNECTED:
		send_prxr_connect_ind(app->id, (U8 *)data);
		break;
	case PRXRE_GETCAPABILITY:
		send_prxr_get_capability_ind(app->id, (U8 *)data);
		break;
	case PRXRE_GETTXPOWER:
		// update the sdp record
		// TODO: replace with
		PRX_DBG1("[PRX] PRXRE_GETTXPOWER level change. update record level:%d", data);
		send_prxr_txpower_cnf(app->id, BT_STATUS_SUCCESS, PRXR_GetCurrentTxPower(app));
		break;
	case PRXRE_SETPATHLOSS:
		// immediate alert level has been change! notify upper level
		send_prxr_pathloss_ind(app->id, data);
		break;
	case PRXRE_SETLINKLOSS:
		//
		PRX_DBG1("[PRX] linkloss level change. ignore notify level:%d", data);
		send_prxr_linkloss_ind(app->id, data);
		break;
	case PRXRE_GETRSSI:
		PRX_DBG("[PRX][ERR] not implement yet");
		PRXASSERT((FALSE)); //not implement yet
		break;
	default:
	case PRXRE_REGISTER:
	case PRXRE_DEREGISTER:
		PRX_DBG("[PRX][ERR] unkown event!!!");
		break;
	}
}
/********************************************************/
void prxm_connect_req_hndl(ilm_struct *ilm_ptr);
void prxm_disconnect_req_hndl(ilm_struct *ilm_ptr);
void prxm_get_capability_req_hndl(ilm_struct *ilm_ptr);
void prxm_get_remote_txpower_hndl(ilm_struct *ilm_ptr);
void prxm_set_pathloss_req_hndl(ilm_struct *ilm_ptr);
void prxm_set_linkloss_req_hndl(ilm_struct *ilm_ptr);

void adpprxmCallback(PrxMEvent *event, void *apdata);

typedef struct {
    U8 isUsed;
    U8 internalIndex;
    U8 MMIIndex;
} PRXMInternalMapping;
PRXMInternalMapping g_prxmInternal[PRX_MAX_MONITOR_NUM] = {{0,0,0},{0,0,0},{0,0,0}};

void resetPRXMfreeIndex(){
    U32 i = 0;
    for(i = 0 ; i < PRX_MAX_MONITOR_NUM; i++){
        memset(&g_prxmInternal[i], 0, sizeof(PRXMInternalMapping) );
    }
    OS_Report("[PRX]mmi resetPRXMfreeIndex");
}

/* 
 * @return the index of internal. if no enough index it will be PRX_MAX_MONITOR_NUM
 */
U8 getPRXMfreeIndex(U8 mmiIndex){
    U32 i = 0;    
    for(i = 0 ; i < PRX_MAX_MONITOR_NUM; i++){
        if( 0 == g_prxmInternal[i].isUsed ){
            g_prxmInternal[i].MMIIndex = mmiIndex;
            g_prxmInternal[i].isUsed = 1 ;            
            g_prxmInternal[i].internalIndex = i ;
            OS_Report("[PRX]mmi getPRXMfreeIndex %d return %d", mmiIndex, i);
            return i;
        }
    }
    OS_Report("[PRX][ERR][WRN]mmi getPRXMInternalIndex_getfreeIndex too large %d", mmiIndex);
    return PRX_MAX_MONITOR_NUM;
}

/*
 */
U8 freePRXMInternalIndex(U8 internal){
    U32 i = 0;
    for(i = 0 ; i < PRX_MAX_MONITOR_NUM; i++){
        if( 1 == g_prxmInternal[i].isUsed && i == internal ){
            g_prxmInternal[i].MMIIndex = 0xff;
            g_prxmInternal[i].isUsed = 0 ;            
            g_prxmInternal[i].internalIndex = i ;
            OS_Report("[PRX]mmi freePRXMInternalIndex i:%d Internal %d", i, internal);            
            return TRUE;
        }
    }

    OS_Report("[PRX][ERR][WRN]mmi freePRXMInternalIndex fail !");
    return PRX_MAX_MONITOR_NUM;
}

/* from PRXM's Indexnal to get the MMI index */
U8 getMMIIndex(U8 internal){
    U32 i = 0;
    /* */
    for(i = 0 ; i < PRX_MAX_MONITOR_NUM; i++){
        if( 1 == g_prxmInternal[i].isUsed && i == internal ){
            OS_Report("[PRX]mmi find MMI at:%d internal:%d MMI:%d", i, internal, g_prxmInternal[i].MMIIndex);
            return g_prxmInternal[i].MMIIndex;
        }
    }
    OS_Report("[PRX][WRN]mmi cannot find MMI index:%d", internal);
    return internal;
}

/* from PRXM's Indexnal to get the MMI index */
U8 getPRXMInternalIndex(U8 mmiIndex){
    U32 i = 0;    
    for(i = 0 ; i < PRX_MAX_MONITOR_NUM; i++){
        if( 1 == g_prxmInternal[i].isUsed && g_prxmInternal[i].MMIIndex == mmiIndex ){
            OS_Report("[PRX]mmi getPRXMInternalIndex %d return %d", mmiIndex, i);
            return i;
        }
    }
    OS_Report("[PRX][ERR][WRN]mmi getPRXMInternalIndex too large mmiIndex:%d", mmiIndex);
    return mmiIndex;
}


void send_prxm_connect_cnf(U8 index, U8 rspcode) {
	bt_prxm_connect_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_connect_cnf_struct), TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;

    if( cnf->rspcode != 0 ){
        freePRXMInternalIndex(index);
    }
	prx_send_msg((U16) MSG_ID_BT_PRXM_CONNECT_CNF, (local_para_struct *) cnf,
			NULL );
}

void send_prxm_disconnect_ind(U8 index, U8 rspcode) {
	bt_prxm_disconnect_ind_struct *ind;
	ind = construct_local_para(sizeof(bt_prxm_disconnect_ind_struct), TD_CTRL);

	ind->index = getMMIIndex(index);
	ind->rspcode = rspcode;

    freePRXMInternalIndex(index);

	prx_send_msg((U16) MSG_ID_BT_PRXM_DISCONNECT_IND,
			(local_para_struct *) ind, NULL );
}

void send_prxm_get_rssi_cnf(U8 index, U8 rspcode, S32 value) {
	bt_prxm_get_rssi_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_get_rssi_cnf_struct), TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;
	cnf->rssi = value;

	prx_send_msg((U16) MSG_ID_BT_PRXM_GET_RSSI_CNF, (local_para_struct *) cnf,
			NULL );

}

void send_prxm_get_capability_cnf(U8 index, U8 rspcode, S32 capability) {
	bt_prxm_get_capability_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_get_capability_cnf_struct),
			TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;
	cnf->capability = capability;

    if( cnf->rspcode != 0 ){
        cnf->rspcode = 0;
       	cnf->capability = 0xFF;
    }
    OS_Report("[PRX][WRN] send_prxm_get_capability_cnf return support !");

	prx_send_msg((U16) MSG_ID_BT_PRXM_GET_CAPABILITY_CNF,
			(local_para_struct *) cnf, NULL );

}

void send_prxm_get_txpower_cnf(U8 index, U8 rspcode, S32 value) {
	bt_prxm_get_remote_txpower_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_get_remote_txpower_cnf_struct),
			TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;
	cnf->txpower = value;

	prx_send_msg((U16) MSG_ID_BT_PRXM_GET_REMOTE_TXPOWER_CNF,
			(local_para_struct *) cnf, NULL );

}

void send_prxm_set_linkloss_cnf(U8 index, U8 rspcode) {
	bt_prxm_set_linkloss_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_set_linkloss_cnf_struct), TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;

	prx_send_msg((U16) MSG_ID_BT_PRXM_SET_LINKLOSS_CNF,
			(local_para_struct *) cnf, NULL );

}

void send_prxm_set_pathloss_cnf(U8 index, U8 rspcode) {
	bt_prxm_set_pathloss_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_set_pathloss_cnf_struct), TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;

	prx_send_msg((U16) MSG_ID_BT_PRXM_SET_PATHLOSS_CNF,
			(local_para_struct *) cnf, NULL );

}

void send_prxm_get_linkloss_cnf(U8 index, U8 rspcode, U8 level) {
	bt_prxm_get_linkloss_cnf_struct *cnf;
	cnf = construct_local_para(sizeof(bt_prxm_get_linkloss_cnf_struct), TD_CTRL);

	cnf->index = getMMIIndex(index);
	cnf->rspcode = rspcode;
	cnf->level = level;

	prx_send_msg((U16) MSG_ID_BT_PRXM_GET_LINKLOSS_CNF,
			(local_para_struct *) cnf, NULL );

}

/*******************************************************************************************************/

void prxm_connect_req_hndl(ilm_struct *ilm_ptr) {
	bt_prxm_connect_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
	req = (bt_prxm_connect_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMfreeIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_connect_cnf(req->index, BT_STATUS_NOT_FOUND); //1 is false
		return;
	}

	if( g_prxmapp[internalIndex].state != 0 ){
		PRX_DBG2("[PRX][ERR] In use state:%d (0x%x)", g_prxmapp[internalIndex].state, &g_prxmapp[internalIndex].state);

		// error handling
		PRX_DBG1("[PRX][WRN] receive twice connect. Disconnect the previous one ! state:%d", g_prxmapp[internalIndex].state);
		if( g_prxmapp[internalIndex].state == PRXMS_CONNECTED ){
		status = PRXM_Disconnect(&g_prxmapp[internalIndex]);
		}
	    return;
	}
	
	PRX_DBG2("prxm_connect_req_hndl idx:%d ap:0x%x", internalIndex, &g_prxrapp[internalIndex]);

	g_prxmapp[internalIndex].id = internalIndex;
	g_prxmapp[internalIndex].state = 1; //inuse
	     
	status = PRXM_Init( (PrxMApp *)&g_prxmapp[internalIndex]);
	if( BT_STATUS_SUCCESS == status ){
		status = PRXM_Connect(&g_prxmapp[internalIndex], req->addr, adpprxmCallback);
	}else{
		g_prxmapp[internalIndex].state = 0; //out of inuse	
		send_prxm_connect_cnf(internalIndex, BT_STATUS_BUSY); //1 is false
		return;
	}
	if (BT_STATUS_SUCCESS == status) {
		send_prxm_connect_cnf(internalIndex, 0);
		//1 is false
	} else if (BT_STATUS_PENDING == status) {
		// wait for response
	} else {
		g_prxmapp[internalIndex].state = 0; //out of inuse	
        
		// recycle the
		status = PRXM_Deinit(&g_prxmapp[internalIndex]);

		send_prxm_connect_cnf(internalIndex, BT_STATUS_FAILED); //1 is false
	}
}

void prxm_disconnect_req_hndl(ilm_struct *ilm_ptr) {
	bt_prxm_disconnect_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
   
	req = (bt_prxm_disconnect_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_disconnect_ind(internalIndex, BT_STATUS_NOT_FOUND);
		return;
	}

	PRX_DBG2("prxm_disconnect_req_hndl idx:%d ap:0x%x", internalIndex, &g_prxmapp[internalIndex]);
	if( BT_STATUS_SUCCESS == PRXM_IsDisconnecting(&g_prxmapp[internalIndex])){
		PRX_DBG("[PRX][WRN] isDisconnecting ignore it!");
		return;
	}

	status = PRXM_Disconnect( &g_prxmapp[internalIndex] );

	switch(status){
		case BT_STATUS_SUCCESS:
			send_prxm_disconnect_ind(internalIndex, BT_STATUS_SUCCESS);

			//TODO: recycle the apdata
			status = PRXM_Deinit(&g_prxmapp[internalIndex]);
			g_prxmapp[internalIndex].state = 0;
			break;
		case BT_STATUS_PENDING:
			// wait for callback
			break;
		default:
		    PRX_DBG1("[PRX][WRN] disconnect with result:%d", status);
			send_prxm_disconnect_ind(internalIndex, status);
			g_prxmapp[internalIndex].state = 0;
			break;
	}
}

void prxm_get_capability_req_hndl( ilm_struct *ilm_ptr ) {
	bt_prxm_get_capability_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
    
	req = (bt_prxm_get_capability_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_get_capability_cnf(req->index, BT_STATUS_NOT_FOUND, 0);
		return;
	}

	PRX_DBG1("prxm_get_capability_req_hndl ap:0x%x ", &g_prxmapp[internalIndex]);
	status = RPXM_GetCapability( &g_prxmapp[internalIndex]);

	switch(status){
		case BT_STATUS_SUCCESS:
		send_prxm_get_capability_cnf(internalIndex, status, PRXM_GetCurrentCapabilityValue(&g_prxmapp[internalIndex]) );
			break;
		case BT_STATUS_PENDING:
			// wait for callback
			break;
		default:
			send_prxm_get_capability_cnf(internalIndex, status, 0);
			break;
	}
}

void prxm_get_remote_txpower_hndl( ilm_struct *ilm_ptr ) {
	bt_prxm_get_remote_txpower_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
	req = (bt_prxm_get_remote_txpower_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_get_txpower_cnf(req->index, BT_STATUS_NOT_FOUND, 0);
		return;
	}

	PRX_DBG1("prxm_get_remote_txpower_hndl ap:0x%x ", &g_prxmapp[internalIndex]);
	status = RPXM_ReadRemoteTxPower( &g_prxmapp[internalIndex], 1 );

	switch(status){
		case BT_STATUS_SUCCESS:
			send_prxm_get_txpower_cnf(internalIndex, status, 0);
			break;
		case BT_STATUS_PENDING:
			// wait for callback
			break;
		default:
			send_prxm_get_txpower_cnf(internalIndex, status, 0);
			break;
	}
}

void prxm_set_pathloss_req_hndl( ilm_struct *ilm_ptr ) {
	bt_prxm_set_pathloss_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
	req = (bt_prxm_set_pathloss_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);    

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_set_pathloss_cnf(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	PRX_DBG1("prxm_set_pathloss_req_hndl ap:0x%x ", &g_prxmapp[internalIndex]);
	// TODO: seqid
	status = RPXM_WritePathLossLevel( &g_prxmapp[internalIndex], req->level, 4 );

	switch(status){
		case BT_STATUS_SUCCESS:
			send_prxm_set_pathloss_cnf(internalIndex, status);
			break;
		case BT_STATUS_PENDING:
			// wait for callback
			break;
		default:
			send_prxm_set_pathloss_cnf(internalIndex, status);
			break;
	}
}

void prxm_get_linkloss_req_hndl(ilm_struct *ilm_ptr) {
	bt_prxm_get_linkloss_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
	req = (bt_prxm_get_linkloss_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_get_linkloss_cnf(internalIndex, BT_STATUS_NOT_FOUND, 0);
		return;
	}

	PRX_DBG1("prxm_get_remote_linkloss_hndl ap:0x%x ", &g_prxmapp[internalIndex]);
	status = RPXM_ReadRemoteLinkLoss(&g_prxmapp[internalIndex], 1);

	switch (status) {
	case BT_STATUS_SUCCESS:
		send_prxm_get_linkloss_cnf(internalIndex, status, 0);
		break;
	case BT_STATUS_PENDING:
		// wait for callback
		break;
	default:
		send_prxm_get_linkloss_cnf(internalIndex, status, 0);
		break;
	}
}


void prxm_set_linkloss_req_hndl( ilm_struct *ilm_ptr ) {
	bt_prxm_set_linkloss_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
	req = (bt_prxm_set_linkloss_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_set_linkloss_cnf(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	PRX_DBG1("prxm_set_linkloss_req_hndl ap:0x%x ", &g_prxmapp[internalIndex]);
	// TODO: seqid
	status = RPXM_WriteLinkLossLevel( &g_prxmapp[internalIndex], req->level , 3);

	switch(status){
		case BT_STATUS_SUCCESS:
			send_prxm_set_linkloss_cnf(internalIndex, status);
			break;
		case BT_STATUS_PENDING:
			// wait for callback
			break;
		default:
			send_prxm_set_linkloss_cnf(internalIndex, status);
			break;
	}
}

void prxm_get_rssi_req_hndl(ilm_struct *ilm_ptr){
	bt_prxm_get_rssi_req_struct *req;
	BtStatus status;
    U8 internalIndex = 0;
	req = (bt_prxm_get_rssi_req_struct *) ilm_ptr->ilm_data;

    internalIndex = getPRXMInternalIndex(req->index);

	if (internalIndex >= PRX_MAX_MONITOR_NUM) {
		send_prxm_get_rssi_cnf(req->index, BT_STATUS_NOT_FOUND, 0);
		return;
	}

	PRX_DBG1("prxm_get_rssi_req_hndl ap:0x%x", &g_prxmapp[internalIndex]);
	status = RPXM_GetLocalRssi( &g_prxmapp[internalIndex] );
	if( status != BT_STATUS_PENDING ){
		send_prxm_get_rssi_cnf(internalIndex, BT_STATUS_FAILED, 0);
		return;
	}
}

void adpprxmCallback(PrxMEvent *event, void *apdata){
	PrxMApp *app;
	S32 value;
	BtStatus status;
	PRX_DBG3("adpprxmCallback event:0x%x data:0x%x ap:0x%x", event->event, event->data, apdata);
	app = (PrxMApp *) apdata;

	value = (S32)event->data;
	switch( event->event ){
	case PRXME_DISCONNECTED:
		send_prxm_disconnect_ind(app->id, BT_STATUS_SUCCESS);

		// recycle the AP's PRX context
		status = PRXM_Deinit(app);
		app->id = 0;
		app->state = 0;
		break;
	case PRXME_CONNECTED:
		send_prxm_connect_cnf(app->id, BT_STATUS_SUCCESS);
		break;
	case PRXME_GETCAPABILITY:
		send_prxm_get_capability_cnf(app->id, BT_STATUS_SUCCESS, value);
		break;
	case PRXME_GETTXPOWER:
		if ( ((S32)event->data) != PRX_INVALID_RSSI) {
			send_prxm_get_txpower_cnf(app->id, 0, value);
		}else{
			send_prxm_get_txpower_cnf(app->id, BT_STATUS_FAILED, 0);
		}
		break;
	case PRXME_SETPATHLOSS:
		send_prxm_set_pathloss_cnf(app->id, (U8)((U32) event->data));
		break;
	case PRXME_SETLINKLOSS:
		send_prxm_set_linkloss_cnf(app->id, (U8)((U32) event->data));
		break;
	case PRXME_GETRSSI:
		send_prxm_get_rssi_cnf(app->id, BT_STATUS_SUCCESS, (S32) event->data);
		break;
	case PRXME_GETLINKLOSS:
		send_prxm_get_linkloss_cnf(app->id, BT_STATUS_SUCCESS, (S32) event->data);
		break;
	case PRXME_CONNECTFAIL:
		send_prxm_connect_cnf(app->id, (U8)((U32) event->data) );

		// recycle the AP's PRX context
		status = PRXM_Deinit(app);
		app->id = 0;
		app->state = 0;
		break;
	default:
		PRX_DBG1("[PRX] adpprxmCallback NO handle for event:%d", event->event);
		break;
	}
    PRX_DBG2("[PRX] app->id:%d app->state:%d", app->id, app->state);
}
/********************************************************/

void btmtk_adp_prx_handle_message(ilm_struct *ilm_ptr) {
	OS_Report("[PRX] btmtk_prx_handle_message ilm_ptr:%d", ilm_ptr->msg_id);

	switch (ilm_ptr->msg_id) {
#if defined(__BT_PRXM_PROFILE__) 		
	case MSG_ID_BT_PRXM_CONNECT_REQ:
		prxm_connect_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXM_DISCONNECT_REQ:
		prxm_disconnect_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXM_GET_CAPABILITY_REQ:
		prxm_get_capability_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXM_GET_REMOTE_TXPOWER_REQ:
		prxm_get_remote_txpower_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXM_SET_PATHLOSS_REQ:
		prxm_set_pathloss_req_hndl(ilm_ptr);
		break;
       case MSG_ID_BT_PRXM_GET_LINKLOSS_REQ:
		prxm_get_linkloss_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXM_SET_LINKLOSS_REQ:
		prxm_set_linkloss_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXM_GET_RSSI_REQ:
		prxm_get_rssi_req_hndl(ilm_ptr);
		break;
#endif
#if defined(__BT_PRXR_PROFILE__)
	case MSG_ID_BT_PRXR_REGISTER_REQ:
		prxr_register_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXR_DEREGISTER_REQ:
		prxr_deregister_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXR_AUTHORIZE_RSP:
		prxr_authorize_rsp_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXR_DISCONNECT_REQ:
		prxr_disconnect_req_hndl(ilm_ptr);
		break;
	case MSG_ID_BT_PRXR_UPDATE_TXPOWER_REQ:
		prxr_update_txpower_req_hndl(ilm_ptr);
		break;
#endif		
	default:
		//MTK_BT_Assert(FALSE);
		break;
	}
}

void prx_adp_init() {
	U8 i = 0;
	OS_Report("[PRX] prx_adp_init %d %d", PRX_MAX_REPORTER_NUM,
			PRX_MAX_MONITOR_NUM);
	ATT_PACKETPOOL_INIT();
	// fake_packet_pool_init();
	
#if defined(__BT_PRXR_PROFILE__) 	
	for (i = 0; i < PRX_MAX_REPORTER_NUM; i++) {
		OS_MemSet((U8 *)&g_prxrapp[i], 0, sizeof(PrxRApp));		
	}
#endif	
#if defined(__BT_PRXM_PROFILE__) 
    resetPRXMfreeIndex();

	for (i = 0; i < PRX_MAX_MONITOR_NUM; i++) {
		OS_MemSet((U8 *)&g_prxmapp[i], 0, sizeof(PrxMApp));
	}
#endif

	// re-inital the context
#if defined(__BT_PRXR_PROFILE__) 
	PRXR_ResetContext();
#endif

#if defined(__BT_PRXM_PROFILE__) 
	PRXM_ResetContext();
#endif

}

void prxr_adp_reset(){
	// reset the prxr. free the allocated mem
#if defined(__BT_PRXR_PROFILE__) 
	U8 i;
	for (i = 0; i < PRX_MAX_REPORTER_NUM; i++) {
		OS_MemSet((U8 *)&g_prxrapp[i], 0, sizeof(PrxRApp));		
	}
#endif	
}

void prxm_adp_reset(){
	// reset the prxm. free the allocated mem
#if defined(__BT_PRXM_PROFILE__) 	
	U8 i;
	for (i = 0; i < PRX_MAX_MONITOR_NUM; i++) {
		OS_MemSet((U8 *)&g_prxmapp[i], 0, sizeof(PrxMApp));
	}
#endif		
}

#endif
