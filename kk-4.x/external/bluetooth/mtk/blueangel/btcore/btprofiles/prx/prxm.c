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
 * prxm.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth Proximity Profile
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
#ifdef __BT_PRXM_PROFILE__

#include "bttypes.h"
#include "bluetooth_prx_struct.h"
#include "prxm.h"
#include "prxadapter.h"
#include "me.h"

/* global variable used inside proximity profile */
PrxMContext g_prxm[PRX_MAX_MONITOR_NUM];

/*********************************/
BtStatus PRXM_Disconnect(PrxMApp *app);
void prxm_notify(PrxMContext* pCntx, PrxMEventType event, void *data);
void prxm_notifyDisconnectResult(PrxMContext* pCntx, U8 ok, U32 rspcode);
/*********************************/

static PrxMContext* static_getPRXMFreeContext() {
	U8 index = 0;
	PrxMContext *pCntx;

	/* check the cntx is inside list */
	for (index = 0; index < PRX_MAX_MONITOR_NUM; index++) {
		// found
		pCntx = &g_prxm[index];
		if (pCntx->isuse != TRUE) {
			pCntx->isuse = TRUE;
			PRX_DBG1(" static_getPRXMFreeContext free at %d", index);
			return pCntx;
		}
	}

	// error 1
	PRX_DBG(" error! static_getPRXMFreeContext! no any avaiable one");

	return NULL;
}

static void static_freePRXMFreeContext(PrxMContext *pCntx) {
	U8 index = 0;

	/* check the cntx is inside list */
	for (index = 0; index < PRX_MAX_MONITOR_NUM; index++) {
		if (pCntx == &g_prxm[index]) {
			// found
			if (pCntx->isuse == TRUE) {
				pCntx->isuse = FALSE;
				PRX_DBG1(" static_freePRXMFreeContext free at %d", index);
			} else {
				PRX_DBG1(" static_freePRXMFreeContext unable to free at %d",
						index);
			}
			return;
		}
	}

	// error 1
	PRX_DBG(" error! static_freePRXMFreeContext! not found ");
	PRXASSERT((FALSE));
}

void PRXM_ResetContext(){
    //
    U8 i;
    for (i = 0; i< PRX_MAX_MONITOR_NUM; i++){
        memset( &g_prxm[i], 0, sizeof(PrxMContext));
    }
}

/**
 * @brief system invoke this before using prxm
 */
BtStatus PRXM_Init(PrxMApp *app) {
	PrxMContext* pCntx;

	if (NULL == app) {
		return BT_STATUS_FAILED;
	}
	pCntx = static_getPRXMFreeContext();
	if (NULL == pCntx) {
		return BT_STATUS_FAILED;
	}
	app->prxContext = pCntx;
	pCntx->chnl.trantype = PRX_TRANSTYPE_CHECK;

	return BT_STATUS_SUCCESS;
}

BtStatus PRXM_Deinit(PrxMApp *app) {
	PrxMContext* pCntx;
	if (NULL == app) {
		return BT_STATUS_FAILED;
	}
	pCntx = app->prxContext;
	if (NULL == pCntx) {
		return BT_STATUS_FAILED;
	}

	PRXASSERT( (pCntx->chnl.trantype == PRX_TRANSTYPE_CHECK) );
	static_freePRXMFreeContext(pCntx);
	app->prxContext = NULL;

	return BT_STATUS_SUCCESS;
}

BtStatus prxm_QueryChannel(PrxMContext* pCntx) {
	BtStatus status;
	pCntx->state = PRXMS_QUERY;
    if (ME_GetDeviceType(&pCntx->chnl.handler.remDev->bdAddr) != BT_DEV_TYPE_LE)
    {
        status = ATT_QUERYSERVICE(&pCntx->chnl);
    }
    else
    {
        status = BT_STATUS_SUCCESS;
    }
	return status;
}

BtStatus prxm_OpenChannel(PrxMContext* pCntx) {
	BtStatus status;
	if (NULL == pCntx) {
		return BT_STATUS_FAILED;
	}
	pCntx->state = PRXMS_CONNECTING;
	status = ATT_CONNECTSERVICE(&pCntx->chnl);

	return status;
}

BtStatus prxm_GetCapability(PrxMContext* pCntx) {
	BtStatus status;
	if (NULL == pCntx) {
		return BT_STATUS_FAILED;
	}
	pCntx->state = PRXMS_GETCAP;
	status = BT_STATUS_SUCCESS;

	return status;
}

BtStatus prxm_disconnect(PrxMContext* pCntx) {
	BtStatus status;
	if (NULL == pCntx) {
		return BT_STATUS_FAILED;
	}
	pCntx->state = PRXMS_DISCONNECTING;
	status = ATT_MONITOR_DISCONNECT(&pCntx->chnl);

	return status;
}

BtStatus prxm_getlocalrssi(PrxMContext* pCntx) {
	BtStatus status;
	if (NULL == pCntx) {
		return BT_STATUS_FAILED;
	}
	status = ATT_GETRSSI(&pCntx->chnl);
	return status;
}

void prxm_notifyConnectFail(PrxMContext* pCntx, U8 rspcode) {
	U32 code = rspcode;
	if (pCntx->callback) {
		prxm_notify(pCntx, (PrxMEventType)PRXME_CONNECTFAIL, (void *)code);
	}
}

void prxm_notify(PrxMContext* pCntx, PrxMEventType event, void *data) {
	PrxMEvent l_event;
	// pass event object to ap levevl
	if (pCntx->callback) {
		l_event.event = event;
		l_event.data = data;
		pCntx->callback(&l_event, pCntx->app);
	} else {
		PRX_DBG2("prxm_notify ignore beacuse no callback ~ pCntx:0x%x event:%d", pCntx, event);
		PRXASSERT((FALSE));
	}
}

void prxm_notifyLinkResult(PrxMContext* pCntx, U8 ok, U32 rspcode) {
	BtStatus status = BT_STATUS_FAILED;

	if (PRXMS_IDLE == pCntx->state) {
		// ignore this
		PRX_DBG("prxm_notifyLinkResult ignore event");
		return;
	}
	if (TRUE == ok) {
		//ok
		if (PRXMS_DISCONNECTING != pCntx->state) {
			// go to next level
			switch (pCntx->state) {
			case PRXMS_LINK_CONNECT:
				status = prxm_QueryChannel(pCntx);
				if (BT_STATUS_SUCCESS == status) {
					prxm_notifyLinkResult(pCntx, TRUE, BT_STATUS_SUCCESS);
				} else if (BT_STATUS_PENDING == status) {
					// wait for response
				} else {
					PRX_DBG1("[RPX] prxm_notifyLinkResult LINK_CONNECT fail:%d", status);
					// disconnect it!
					status = ATT_REMOVELINK(&pCntx->chnl);
					OS_Report("[PRX] ATT_REMOVELINK status:%d", status);
					prxm_notifyConnectFail(pCntx, XA_STATUS_NO_LINK);
				}
				break;
			case PRXMS_QUERY:
				status = prxm_OpenChannel(pCntx);
				if (BT_STATUS_SUCCESS == status) {
					prxm_notifyLinkResult(pCntx, TRUE, BT_STATUS_SUCCESS);
				} else if (BT_STATUS_PENDING == status) {
					// wait for response
				} else {
					PRX_DBG1("[RPX] prxm_notifyLinkResult QUERY fail:%d", status);
					// disconnect it!
					status = ATT_REMOVELINK(&pCntx->chnl);
					if (BT_STATUS_SUCCESS == status) {
						prxm_notifyConnectFail(pCntx, BT_STATUS_NOT_FOUND);
					}

				}
				break;
			case PRXMS_CONNECTING:
				status = prxm_GetCapability(pCntx);
				if (BT_STATUS_SUCCESS == status) {
					// directly into next phrase
					pCntx->state = PRXMS_CONNECTED;
					prxm_notify(pCntx, PRXME_CONNECTED, NULL);
				} else if (BT_STATUS_PENDING == status) {
					// wait for callback
				} else {
					PRX_DBG1("[RPX] prxm_notifyLinkResult CONNECTING fail:%d", status);
					// fail! fail to next level
					pCntx->state = PRXMS_CONNECTED;
					// try to disconnect it~
					status = PRXM_Disconnect(pCntx->app);
				}
				break;
			case PRXMS_GETCAP:
				// TODO: unknow what is get-capability right now. just notify it ok.
				pCntx->state = PRXMS_CONNECTED;
				prxm_notify(pCntx, PRXME_CONNECTED, NULL);
				break;

			default:
				status = BT_STATUS_SUCCESS;
				break;
			}
		} else {
			// Abort the connection. disconnect it and wait for disconnect callback
			switch (pCntx->state) {
			case PRXMS_LINK_CONNECT:
				status = ATT_REMOVELINK(&pCntx->chnl);
				break;
			case PRXMS_QUERY:
				status = ATT_STOPQUERYSERVICE(&pCntx->chnl);
				if (BT_STATUS_FAILED == status) {
					status = ATT_REMOVELINK(&pCntx->chnl);
				}
				break;
			case PRXMS_CONNECTING:
			case PRXMS_GETCAP:
				pCntx->state = PRXMS_CONNECTED;
				status = PRXM_Disconnect(pCntx->app);
			default:
				status = BT_STATUS_SUCCESS;
				break;
			}

			if (status == BT_STATUS_PENDING) {
				// wait response
			} else if (status == BT_STATUS_SUCCESS) {
				// already disconnected
				pCntx->state = PRXMS_IDLE;
				prxm_notifyConnectFail(pCntx, BT_STATUS_CANCELLED);
			} else {
				// error ?
				PRX_DBG1("[PRX][err] prxm_notifyLinkResult removelink:%d", status );
				pCntx->state = PRXMS_IDLE;
				prxm_notifyConnectFail(pCntx, BT_STATUS_CANCELLED);
			}
		}
	} else {
		// fail to establish link. report as fail
		U8 state;
		state = pCntx->state;
		pCntx->state = PRXMS_IDLE;
		switch(state ){
			case PRXMS_CONNECTED:
			case PRXMS_GETCAP:
			case PRXMS_DISCONNECTING:
				prxm_notifyDisconnectResult(pCntx, TRUE, BT_STATUS_NO_CONNECTION);
				break;
			default:
				prxm_notifyConnectFail(pCntx, BT_STATUS_NO_CONNECTION);
				break;
		}
		
	}
}

void prxm_notifyQueryResult(PrxMContext* pCntx, U8 ok, U32 rspcode) {
	prxm_notifyLinkResult(pCntx, ok, rspcode);
}

void prxm_notifyConnectResult(PrxMContext* pCntx, U8 ok, U32 rspcode) {
	prxm_notifyLinkResult(pCntx, ok, rspcode);
}

void prxm_notifyGetCapabilityResult(PrxMContext* pCntx, U8 ok, U32 rspcode) {
	/* get remote service's status. get capability */
	prxm_notifyLinkResult(pCntx, ok, rspcode);
}
void prxm_notifyDisconnectResult(PrxMContext* pCntx, U8 ok, U32 rspcode) {
	PRX_DBG1("[PRX] prxm_notifyDisconnectResult state:%d", pCntx->state );
	if( pCntx->state != PRXMS_IDLE ){
		pCntx->state = PRXMS_IDLE;
		prxm_notify(pCntx, PRXME_DISCONNECTED, NULL);
	}else{
	}
}

void prxm_notifyGetRssiResult(PrxMContext* pCntx, U8 ok, S32 value) {
	if (ok) {
		pCntx->rssi = value;
		prxm_notify(pCntx, PRXME_GETRSSI, (void *)value);
	} else {
		// fail
		pCntx->rssi = PRX_INVALID_RSSI;
		prxm_notify(pCntx, PRXME_GETRSSI, 0);
	}
}

void prxm_notifyGetTxpowerResult(PrxMContext* pCntx, U8 ok, S32 value) {
	if (ok) {
		pCntx->txPower = value;
	} else {
		// fail
		pCntx->txPower = PRX_INVALID_RSSI;
	}

	prxm_notify(pCntx, PRXME_GETTXPOWER, (void *)((S32)pCntx->txPower));
}

void prxm_notifyGetLinkLossResult(PrxMContext* pCntx, U8 ok, S32 value) {
	if (ok) {
		pCntx->linkloss = value;
	} else {
		// fail
		pCntx->linkloss = PRX_INVALID_LINKLOSS;
	}

	prxm_notify(pCntx, PRXME_GETLINKLOSS, (void *)(U32)pCntx->linkloss);
}


void prxm_notifyWriteRemoteIMAlert(PrxMContext* pCntx, U8 rspcode) {
	prxm_notify(pCntx, PRXME_SETPATHLOSS, (void *)((U32)rspcode));
}

void prxm_notifyWriteRemoteLinkAlert(PrxMContext* pCntx, U8 rspcode) {
	prxm_notify(pCntx, PRXME_SETLINKLOSS, (void *)((U32)rspcode));
}

/**
 * @brief Connect to report
 * @param addr remote address
 */
BtStatus PRXM_Connect(PrxMApp *app, U8 addr[6], BTMTKPRXMCALLBACK callback) {
	PrxMContext* pCntx = app->prxContext;
	BtStatus status;

	if (NULL != pCntx) {
		// connect to remote
		pCntx->state = PRXMS_LINK_CONNECT;
		pCntx->callback = callback;
		pCntx->app = app;

		//memcpy(pCntx->addr, addr, sizeof(addr));

		PRXASSERT( (pCntx->chnl.trantype == PRX_TRANSTYPE_CHECK) );
 
		// acl link
		status = ATT_CREATELINK(&pCntx->chnl, (char *)addr);
		if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS) {
			//static_freePRXMFreeContext(pCntx); // to deinit
			return BT_STATUS_FAILED;
		}

		// query
		if (status == BT_STATUS_SUCCESS) {
			status = prxm_QueryChannel(app->prxContext);
			if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS) {
				//static_freePRXMFreeContext(pCntx); // to deinit
                // ALPS00287001 quickly test
                OS_Report("[PRX:]found1 status:%d", status);
				ATT_REMOVELINK(&pCntx->chnl);
				return BT_STATUS_FAILED;
			}
		}

		// connect
		if (status == BT_STATUS_SUCCESS) {
			status = prxm_OpenChannel(app->prxContext); 
			if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS) {
				//static_freePRXMFreeContext(pCntx); // to deinit
				OS_Report("[PRX:]found2 status:%d", status);
				ATT_REMOVELINK(&pCntx->chnl);
				return BT_STATUS_FAILED;
			}
			if( BT_STATUS_SUCCESS == status ){
			    pCntx->state = PRXMS_CONNECTED; //wcncr00000855 connect to a ble device which is cmgr connected.
			}
		}

		// success
		if (status == BT_STATUS_SUCCESS) {
			PRX_DBG("PRXM_Connect ok");
			return BT_STATUS_SUCCESS;
		}
	} else {
		status = BT_STATUS_FAILED;
	}
	return status;
}

BtStatus PRXM_Disconnect(PrxMApp *app) {
	PrxMContext* pCntx = app->prxContext;
	BtStatus status;

	/* if it is connecting, raise up the disconnect flag */
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	if (PRXMS_CONNECTING == pCntx->state) {
		pCntx->state = PRXMS_DISCONNECTING;
	}
	if (PRXMS_IDLE == pCntx->state) {
		return BT_STATUS_SUCCESS;
	}

	status = prxm_disconnect(pCntx);

	// recycle the prxm context

	return status;
}

BtStatus PRXM_IsDisconnecting(PrxMApp *app) {
	PrxMContext* pCntx = app->prxContext;
	BtStatus status = BT_STATUS_FAILED;
	if(pCntx && pCntx->state == PRXMS_DISCONNECTING){
		return BT_STATUS_SUCCESS;
	}
	return status;
}

/*
 * @brief callback the capability
 *
 */
BtStatus RPXM_GetCapability(PrxMApp *app) {
	// TODO: unknow how to get the remote's capability
	// assume that a sync procedure will be done
	PrxMContext* pCntx = app->prxContext;
	BtStatus status = BT_STATUS_FAILED;

	if (pCntx == NULL || pCntx->state != PRXMS_CONNECTED) {
		return status;
	}
	status = BT_STATUS_SUCCESS;
	return status;
}

S32 PRXM_GetCurrentCapabilityValue(PrxMApp *app) {
	// TODO: temp solution
	return 0xff;
}

static BtStatus prxm_writeRemoteLevel(PrxMApp *app, U32 attid, U8 level,
		U8 seqid) {
	PrxMContext* pCntx = app->prxContext;
	BtStatus status = BT_STATUS_FAILED;
	void *packet;

	if (pCntx == NULL || pCntx->state != PRXMS_CONNECTED) {
	if (pCntx != NULL) { 
		OS_Report("prxm state:%d", pCntx->state);
        }
		return BT_STATUS_FAILED; 
	}

	// get a available packet
	packet = (void *)ATT_GETPACKET(pCntx);

	if (NULL != packet) {
		switch(attid){
			case GATT_PRX_TXPOWER:
				attid = GATT_PRX_WRITE_TXPOWER;
				break;
			case GATT_PRX_IMMEDIATE_SERVICE:
				attid = GATT_PRX_WRITE_IMMEDIATE_SERVICE;
				break;
			case GATT_PRX_LINKLOSS_SERVICE:
				attid = GATT_PRX_WRITE_LINKLOSS_SERVICE;
				break;
			default:
				OS_Report("attid:%d", attid);
				break;
		}
		// write the remote data and use packet
		status = ATT_WRITEREMOTEVALUE( &pCntx->chnl, packet, seqid, attid, GATT_TYPE_S8,
				&level); //void *packet, U8 seqid, U32 id, U32 type, void *value

		// check response code
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			PRX_DBG1("RPXM_WriteLinkLossLevel fail status:%d", status);
			// return the packet. since it doens't send out
			ATT_RETURNPACKET(packet);
		}

		if( status == BT_STATUS_SUCCESS ){
			ATT_RETURNPACKET(packet);
		}
	}
	return status;
}

/*
 * @brief write the pathloss level 
 * Aysnc code
 */
BtStatus RPXM_WritePathLossLevel(PrxMApp *app, U8 level, U8 seqid) {
	return prxm_writeRemoteLevel(app, GATT_PRX_IMMEDIATE_SERVICE, level, seqid);
}

/*
 * @brief write the linkloss level
 * Aysnc code
 */
BtStatus RPXM_WriteLinkLossLevel(PrxMApp *app, U8 level, U8 seqid) {
	// use seq id to recognize the request
	return prxm_writeRemoteLevel(app, GATT_PRX_LINKLOSS_SERVICE, level, seqid);
}

/*
 * @brief write the linkloss level
 * Aysnc code
 */
BtStatus RPXM_ReadRemoteTxPower(PrxMApp *app, U8 seqid) {
	// use seq id to recognize the request
	PrxMContext* pCntx = app->prxContext;
	BtStatus status = BT_STATUS_FAILED;
	PrxPacket *packet;

	if (pCntx == NULL || pCntx->state != PRXMS_CONNECTED) {
		return BT_STATUS_FAILED;
	}

	// get a available packet
	packet = (PrxPacket *) ATT_GETPACKET(pCntx);

	if (NULL != packet) {
		// write the remote data and use packet
		status = ATT_READREMOTEVALUE( &pCntx->chnl, packet, seqid, GATT_PRX_TXPOWER,
				GATT_TYPE_S8);

		// check response code
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			PRX_DBG1("RPXM_ReadRemoteTxPower fail status:%d", status);
			ATT_RETURNPACKET(packet);
		}

		if (status == BT_STATUS_SUCCESS){
			ATT_RETURNPACKET(packet);
		}
	} else {
		// fail to get packet
	}
	return status;
}

/*
 * @brief write the linkloss level
 * Aysnc code
 */
BtStatus RPXM_ReadRemoteLinkLoss(PrxMApp *app, U8 seqid) {
	// use seq id to recognize the request
	PrxMContext* pCntx = app->prxContext;
	BtStatus status = BT_STATUS_FAILED;
	PrxPacket *packet;

	if (pCntx == NULL || pCntx->state != PRXMS_CONNECTED) {
		return BT_STATUS_FAILED;
	}

	// get a available packet
	packet = (PrxPacket *) ATT_GETPACKET(pCntx);

	if (NULL != packet) {
		// write the remote data and use packet
		status = ATT_READREMOTEVALUE( &pCntx->chnl, packet, seqid, GATT_PRX_LINKLOSS_SERVICE,
				GATT_TYPE_S8);

		// check response code
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			PRX_DBG1("RPXM_ReadRemoteLinkLoss fail status:%d", status);
			ATT_RETURNPACKET(packet);
		}

		if (status == BT_STATUS_SUCCESS){
			ATT_RETURNPACKET(packet);
		}
	} else {
		// fail to get packet
	}
	return status;
}


/*
 * @brief Get the local rssi
 * Aysnc code
 */
BtStatus RPXM_GetLocalRssi(PrxMApp *app) {
	/* get the local rssi and report it to top level */
	PrxMContext* pCntx = app->prxContext;
	BtStatus status;

	if (pCntx == NULL || pCntx->state != PRXMS_CONNECTED) {
	    if (pCntx != NULL){
	        PRX_DBG1("[PRX] state:%d", pCntx->state);
	    }
		return BT_STATUS_FAILED;
	}

	status = prxm_getlocalrssi(app->prxContext);
	return status;
}

BOOL prxm_isConnecting(PrxMContext* pCntx) {
	PRX_DBG1("[PRX] prxm_isConnecting state:%d", pCntx->state );
	if (NULL != pCntx && pCntx->state == PRXMS_CONNECTING) {
		return TRUE;
	}
	return FALSE;
}

#endif
