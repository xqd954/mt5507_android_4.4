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
 * prxr.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth proximity
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
#if defined(__BT_PRXR_PROFILE__)
#include "bttypes.h"
#include "bluetooth_prx_struct.h"
#include "prxr.h"
#include "prxm.h"
#include "prxadapter.h"

/* global variable used inside proximity profile */
PrxRContext g_prxr[PRX_MAX_REPORTER_NUM];

/***** internal function *****/
void prxr_notify(PrxRContext* pCntx, U8 eventid, void *data);
BT_BOOL PRXR_isConnected(U8 state);
BT_BOOL PRXR_raiseDisconnect(PrxRContext* pCntx);
BtStatus prxr_AddATTRecord(PrxRContext* pCntx);
BtStatus prxr_RemoveATTRecord(PrxRContext* pCntx);
/******************************/

static PrxRContext* static_getPRXRFreeContext() {
	U8 index = 0;
	PrxRContext *pCntx;

	/* check the cntx is inside list */
	for (index = 0; index < PRX_MAX_REPORTER_NUM; index++) {
		// found
		pCntx = &g_prxr[index];
		if (pCntx->isuse != TRUE) {
			pCntx->isuse = TRUE;
			PRX_DBG1(" static_getPRXRFreeContext get at %d", index);
			return pCntx;
		}
	}

	// error 1
	PRX_DBG(" error! static_getPRXRFreeContext! no any avaiable one");
	PRXASSERT((FALSE));
	return NULL;
}

static void static_freePRXRFreeContext(PrxRContext *pCntx) {
	U8 index = 0;

	/* check the cntx is inside list */
	for (index = 0; index < PRX_MAX_REPORTER_NUM; index++) {
		if (pCntx == &g_prxr[index]) {
			// found
			if (pCntx->isuse == TRUE) {
				pCntx->isuse = FALSE;
				PRX_DBG1(" static_freePRXRFreeContext free at %d", index);
			} else {
				PRX_DBG1(" static_freePRXRFreeContext unable to free at %d",
						index);
			}
			return;
		}
	}

	// error 1
	PRX_DBG(" error! static_freePRXRFreeContex! not found ");
	PRXASSERT((FALSE));
}

/**
 * @brief invoke by system before using prxr - poweron
 */
BT_BOOL PRXR_ResetContext() {
	U8 index = 0;
	PrxRContext *pCntx;

	PRX_DBG("PRXR_ResetContext all");
	/* check the cntx is inside list */
	for (index = 0; index < PRX_MAX_REPORTER_NUM; index++) {
		pCntx = &g_prxr[index];
		memset((void *) pCntx, 0, sizeof(PrxRContext));
	}
	return TRUE;
}

/**
 * @brief invoke by system When it dones't need prxr - poweroff
 */
BT_BOOL RPXR_Deinit() {
	return TRUE;
}

static void prxr_resetDataMember(PrxRContext* pCntx) {
	pCntx->state = PRXRS_LISTENING;
	// for check!
	pCntx->chnl.trantype = PRX_TRANSTYPE_CHECK;
}

/**
 * @brief Registe the specific channel and ATT database
 * @param apcallback used to notify ap
 */
BtStatus PRXR_Register(PrxRApp *app, PrxRTransport *chnl,
		BTMTKPRXRCALLBACK apcallback) {
	PrxRContext* pCntx = NULL;

	pCntx = static_getPRXRFreeContext();
	if (NULL != pCntx && NULL != apcallback) {
		// TODO: clear the ATT database !

		/* register the ATT channel info data*/
		prxr_resetDataMember(pCntx);

		// TODO: where to put the call-back function
		if (BT_STATUS_SUCCESS == ATT_REGISTER(&(pCntx->chnl), NULL, &app->id)) { // TODO: change the app->id to register infomation
			if (BT_STATUS_SUCCESS != ATT_REGISTERRECORD(&pCntx->chnl)) {
				PRX_DBG("[ERR] PRXR_Register ATT_REGISTERRECORD fail!");
				ATT_DEREGISTER(&pCntx->chnl);
				static_freePRXRFreeContext(pCntx);
				return BT_STATUS_FAILED;
			}

			pCntx->callback = apcallback;
			app->prxContext = (void *) pCntx;
			pCntx->app = app;

			return BT_STATUS_SUCCESS;
		} else {
			PRX_DBG("[ERR] PRXR_Register ATT_REGISTER fail!");
			static_freePRXRFreeContext(pCntx);
		}
	}

	// fail handle
	if (pCntx != NULL) {
		static_freePRXRFreeContext(pCntx);
	}
	return BT_STATUS_FAILED;
}

/**
 * @brief Register the specific channel and ATT database
 * @param apcallback used to notify AP
 */
BtStatus PRXR_Deregister(PrxRApp *app) {
	PrxRContext* pCntx = NULL;
	BtStatus status;

	if (NULL != app->prxContext) {
		pCntx = (PrxRContext *) app->prxContext;

		if (PRXRS_LISTENING == pCntx->state) {
			// TODO: is there a ATT table object ?
			prxr_RemoveATTRecord(pCntx);

			// unregister the table
			status = ATT_DEREGISTER(&pCntx->chnl);
			if (BT_STATUS_SUCCESS != status) {
				PRX_DBG1("PRXR_Unregister wrong status:%d", status );
				PRXASSERT((FALSE));
			}
			/* return the PRXR context */
			static_freePRXRFreeContext(pCntx);
			app->prxContext = NULL;

			return BT_STATUS_SUCCESS; 
		} else {
			// uanble to unregister
			PRX_DBG1("PRXR_Unregister wrong state:%d", pCntx->state );
		}
	}
	return BT_STATUS_FAILED;
}

/**
 * @brief disconnect the connecting
 */
BtStatus PRXR_Disconnect(PrxRApp *app) {
	/* check the status */
	PrxRContext* pCntx = NULL;
	BtStatus status;
	pCntx = (PrxRContext*) app->prxContext;

	// TODO: check pCntx is real.
	if(pCntx && pCntx->state == PRXRS_CONNECTING){
		status = PRXR_AcceptIncoming(app, BT_STATUS_FAILED);
		return status;
	}
	
	if (pCntx && PRXR_isConnected(pCntx->state)) {
		status = ATT_REPORTER_DISCONNECT(&pCntx->chnl);
		if (BT_STATUS_SUCCESS == status) {
			/* disconnect immediatly */
			pCntx->state = PRXRS_LISTENING;

			return BT_STATUS_SUCCESS;
		} else if (BT_STATUS_PENDING == status) {
			/* wait for callback */
			pCntx->state = PRXRS_DISCONNECTING;
			return BT_STATUS_PENDING;
		} else {
			if (PRXR_raiseDisconnect(pCntx)) {
				/* unable to disconnect it. wait for callback */
				return BT_STATUS_PENDING;
			}
		}
	}
	return BT_STATUS_FAILED;
}

/**
 * @brief accept or reject the incoming connection
 * @param rspcode BtStatus type to accept or reject the incoming connection
 */
BtStatus PRXR_AcceptIncoming(PrxRApp *app, BtStatus rspcode) {
	PrxRContext* pCntx = NULL;
	BtStatus status = BT_STATUS_FAILED;
	pCntx = (PrxRContext*) app->prxContext;

	if (pCntx && (PRXRS_CONNECTING == pCntx->state || PRXRS_CONNECTED == pCntx->state) ) {
		if (rspcode == BT_STATUS_SUCCESS) {
			status = ATT_ACCEPT(&pCntx->chnl);
			if( status == BT_STATUS_SUCCESS ){
				// change to disconnected status
				pCntx->state = PRXRS_CONNECTED;
			}			
		} else {
			status = ATT_REJECT(&pCntx->chnl);
			if( status == BT_STATUS_SUCCESS ){
				// change to disconnected status
				pCntx->state = PRXRS_IDLE;
			}
		}
		PRX_DBG3(" PRXR_AcceptIncoming app:0x%x status:%d state:%d", app, status, pCntx->state );
		return status;
	}
	return BT_STATUS_FAILED;
}

/**
 * @brief Update the txpower in ATT record
 * Async code
 */
BtStatus PRXR_UpdateTxPower(PrxRApp *app, U32 txpower) {
	// TODO: get the TxPower from hci and update it to sdp record
	// USE HCI_LE_Read_Advertising_Channel_Tx_Power to get the Txpower
	PrxRContext* pCntx = NULL;
	BtStatus status = BT_STATUS_FAILED;
	pCntx = (PrxRContext*) app->prxContext;

	status = ATT_GETTXPOWER(&pCntx->chnl);

	return status;
}

S8 PRXR_GetCurrentTxPower(PrxRApp *app) {
	PrxRContext* pCntx = NULL;
	BtStatus status;
	pCntx = (PrxRContext*) app->prxContext;

	// check the current TxPower
	if (pCntx) {
		return pCntx->txPower;
	}
	return PRX_INVALID_TXPOWER;
}

BtStatus PRXR_IsDisconnecting(PrxRApp *app) {
	PrxRContext* pCntx = NULL;
	BtStatus status;
	pCntx = (PrxRContext*) app->prxContext;

	// check the current disconnecting
	if(pCntx && pCntx->state == PRXRS_DISCONNECTING){
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}


/**
 *@brief add service record to att service record
 */
BtStatus prxr_AddATTRecord(PrxRContext* pCntx) {

	BtStatus status;

	// TODO: update the table information by table variable

	ATT_REGISTERRECORD(&pCntx->chnl);

	return BT_STATUS_SUCCESS;
}

/**
 *@brief add service record to att service record
 */
BtStatus prxr_RemoveATTRecord(PrxRContext* pCntx) {
	BtStatus status;

	// TODO: update the table information by table variable
	ATT_DEREGISTERRECORD(&pCntx->chnl);

	return BT_STATUS_SUCCESS;
}

/**
 * @brief read data the form Local ATT alert service record
 * sync code
 */
BtStatus PRXR_ReadAlertLevel(PrxRApp *app, U8 *value) {
	BtStatus status;
	PrxRContext* pCntx = (PrxRContext*) app->prxContext;
	
	status = ATT_READLOCALVALUE(&pCntx->chnl, GATT_PRX_TXPOWER, GATT_TYPE_S8, value);

	return status;
}

/**
 * @brief read data the form Local ATT alert service record
 * sync code
 */
BtStatus PRXR_ReadImmediateAlertLevel(PrxRApp *app, U8 *value) {
	BtStatus status;
	PrxRContext* pCntx = (PrxRContext*) app->prxContext;

	status = ATT_READLOCALVALUE(&pCntx->chnl, GATT_PRX_IMMEDIATE_SERVICE, GATT_TYPE_S8, value);

	return status;
}

void prxr_notify(PrxRContext* pCntx, U8 eventid, void *data) {
	PrxREvent event;
	if (pCntx->callback) {
		event.event = eventid;
		event.data = data;
		pCntx->callback(&event, pCntx->app);
	} else {
		// disconnect it !
		PRX_DBG1("PRXR_Deregister wrong status:%d", pCntx->state );

		if (pCntx->state != PRXRS_DISCONNECTING && pCntx->state != PRXRS_IDLE) {
			// disconnect it !
			PRXR_Disconnect(pCntx->app);
		}
	}
}

/**
 * @brief lower level notify disconnected
 */
void prxr_notifyDisconnected(PrxRContext* pCntx) {
	S8 level = 0;
	BtStatus status;
	pCntx->state = PRXRS_LISTENING;

	// before disconnect. check the linkloss level
	status = ATT_READLOCALVALUE(&pCntx->chnl, GATT_PRX_LINKLOSS_SERVICE, GATT_TYPE_S8, &level);
	if ( status == BT_STATUS_SUCCESS ) {
		prxr_notify(pCntx, PRXRE_SETLINKLOSS, (void *)((S32)level));

		// clear it to 0
		level = 0;
		ATT_WRITELOCALVALUE( &pCntx->chnl, GATT_PRX_LINKLOSS_SERVICE, GATT_TYPE_S8, &level);
	}

	prxr_notify(pCntx, PRXRE_DISCONNECTED, NULL);
}
/**
 * @brief lower level notify the incoming
 */
void prxr_notifyIncoming(PrxRContext* pCntx) {
	/* reject the incoming call */

	if (PRXRS_DISCONNECTING == pCntx->state) {
		//TODO: add dbg msg
		PRXR_AcceptIncoming(pCntx->app, BT_STATUS_FAILED);
	} else {
		pCntx->state = PRXRS_CONNECTING;
		// copy addr from transport to context
		memcpy( pCntx->addr, pCntx->chnl.bdaddr.addr, sizeof(pCntx->addr));
		prxr_notify(pCntx, PRXRE_INCOMING, pCntx->addr); // the parameter is 6 byte addr
	}
}

/**
 * @brief lower level notify disconnected
 */
void prxr_notifyConnected(PrxRContext* pCntx) {
	if (PRXRS_DISCONNECTING == pCntx->state) {
		// disconnect it
		PRXR_Disconnect(pCntx->app);
	} else {
		pCntx->state = PRXRS_CONNECTED;
		prxr_notify(pCntx, PRXRE_CONNECTED, NULL);
	}
}
/**
 * @brief lower level notify disconnected
 */
void prxr_notifyGetTxPowerValue(PrxRContext* pCntx, U8 ok, U32 txpowerValue) {
	if( ok ){
		pCntx->txPower = txpowerValue;
	}else{
		pCntx->txPower = PRX_INVALID_TXPOWER;
	}
	prxr_notify(pCntx, PRXRE_GETTXPOWER, (void *)txpowerValue);
}
/**
 * @brief lower level notify disconnected
 */
void prxr_notifyPathLossValue(PrxRContext* pCntx, U8 ok, U8 pathlossValue) {
	// this is a immediate alert. just pass it to upper level
	prxr_notify(pCntx, PRXRE_SETPATHLOSS, (void *)((U32)pathlossValue));


}
/**
 * @brief lower level notify disconnected
 */
void prxr_notifyLinkLossValue(PrxRContext* pCntx, U8 ok, U8 linklossValue) {
	// prxr_notify(pCntx, PRXRE_SETLINKLOSS, linklossValue);
	pCntx->linkloss = linklossValue;
	// the LinkLoss has been changed!
	// response the value ?
}
/**
 * @brief lower level notify disconnected
 */
void prxr_notifyRssiValue(PrxRContext* pCntx, U8 ok, U32 rssiValue) {
	prxr_notify(pCntx, PRXRE_GETRSSI, NULL);
}

/***********************************************************/
BT_BOOL PRXR_isConnected(U8 state) {
	switch (state) {
	case PRXRS_IDLE:
	case PRXRS_LISTENING:
	case PRXRS_DISCONNECTING:
		return FALSE;
		break;
	case PRXRS_CONNECTING:
	case PRXRS_CONNECTED:
		return TRUE;
		break;
	}
	return FALSE;
}

BT_BOOL PRXR_raiseDisconnect(PrxRContext* pCntx) {
	pCntx->state = PRXRS_DISCONNECTING; // change it to disconnecting flag
	return TRUE;
}

#endif
