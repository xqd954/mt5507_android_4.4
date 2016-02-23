/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "bttypes.h"
#include "l2cap.h"

#include "conmgr.h"
#include "osapi.h"

#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)

#include "bluetooth_prx_struct.h"
#include "prxm.h"
#include "prxr.h"
#include "prxadapter.h"
#include "gattcb.h"
#include "utils.h"  // for listentry
#include "attdef.h"
#include "sdap.h"
#include "sdpdb.h" // for register record

/***** *****/
ProximityServerInfo ProximityReporterDatabase[MAX_PROXIMITY_DATABASE];

BtGattHandler g_prx_adapter_gatthdl; /* gatt callback */
U8            g_reading_rssi = FALSE; 
void          *g_reading_rssi_owner = NULL; // monitor

U8            g_reading_txpower = FALSE;
void          *g_reading_txpower_owner = NULL; //reporter
EvmTimer      g_prx_sdp_timer; 

/***** Internal api *****/
void prxm_notifyConnectResult(PrxMContext* pCntx, U8 ok, U32 rspcode);
void prxm_notifyQueryResult(PrxMContext* pCntx, U8 ok, U32 rspcode);
void prxm_notifyLinkResult(PrxMContext* pCntx, U8 ok, U32 rspcode);
BOOL prxm_isConnecting(PrxMContext* pCntx);
void prxm_notifyGetRssiResult(PrxMContext* pCntx, U8 ok, S32 value);
void prxm_notifyDisconnectResult(PrxMContext* pCntx, U8 ok, U32 rspcode);
void prxm_notifyGetTxpowerResult(PrxMContext* pCntx, U8 ok, S32 value);
void prxm_notifyGetLinkLossResult(PrxMContext* pCntx, U8 ok, S32 value);
void prxm_notifyWriteRemoteIMAlert(PrxMContext* pCntx, U8 rspcode);
void prxm_notifyWriteRemoteLinkAlert(PrxMContext* pCntx, U8 rspcode);
void prxr_notifyGetTxPowerValue(PrxRContext* pCntx, U8 ok, U32 txpowerValue);
void prxr_notifyIncoming(PrxRContext* pCntx);
void prxr_notifyDisconnected(PrxRContext* pCntx);
void prxr_notifyPathLossValue(PrxRContext* pCntx, U8 ok, U8 pathlossValue);
void prxr_notifyLinkLossValue(PrxRContext* pCntx, U8 ok, U8 linklossValue);

/***** callback api *****/
void prxrServerCmgrConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
		BtStatus Status);
// ap callback for GATT
void ProximityCommonGattCallback(void *handler, BtGattEvent *event);

/***** static local api *****/
BtStatus PrxClientReadRSSI(BtRemoteDevice *remDev);
/* lost is that has connected before */
BtStatus static_monitor_removeCmgr(PrxMTransport *pChnl, U8 lost);

/* GATT functions */
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern BtStatus BT_GATT_WriteCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryCharacteristicByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);

/* gatt_utl.c */
extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);

/* prxattdb.c */
extern U8 PrxAttDB_Registration(void);
extern U8 PrxAttDB_Deregistration(void);

/* meif.c */
extern BtStatus ME_ReadRSSI(BtRemoteDevice *remDev);
extern BtStatus ME_ReadTxPower(BtRemoteDevice *remDev);

static void PrxmIASQuerySdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn);
static void PrxmTPSQuerySdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn);
void prx_start_query_timer(EvmTimer *timer_id_p, U32 timer_duration, EvmTimerNotify funct_ptr, void* hf_param);
static void prx_ias_serivce_Timeout(void *para);
static void prx_tps_serivce_Timeout(EvmTimer *ptimer);
BtStatus prxm_att_queryservice(PrxMTransport *chnl, U16 uuid, SdapCallBack callback);
BtStatus prx_GattDiscoveryCharDescription(PrxPacket *packet, U16 uuid, U16 start, U16 end);

/***** static internal *****/
static BOOL release_reporter(PrxRTransport *reporter);

/*****/
ListEntry g_prx_packetlist;
PrxPacket g_prx_packet[prx_MAX_PACKET_NUM];

U8            g_prx_reporterlist_init = FALSE;
ListEntry     g_prx_reporterlist;
PrxRTransport *g_prx_used_reporterarray[PRX_MAX_REPORTER_NUM];

// monitor related variables
U8            g_prx_monitorlist_init = FALSE;
ListEntry     g_prx_monitorlist;
PrxMTransport *g_prx_used_monitorarray[PRX_MAX_REPORTER_NUM] = {NULL};
PrxMTransport *g_prx_monitor = NULL;

 U16 prx_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST 
};

/* internal functions */
void prx_return_packet(PrxPacket *packet);
BtStatus prx_att_removedatalink(void *channel);

void prxClientReportRSSI(BtRemoteDevice *remDev, U8 errorCode);
void prxServerReportTxPower(BtRemoteDevice *remDev, U8 errorCode);
BtStatus prxServerReadTxPower(BtRemoteDevice *remDev);

BtStatus prx_GattSendDiscoveryPrimaryServiceByUUID(
                                PrxPacket *packet,
                                U16 uuid, 
                                U16 start, 
                                U16 end);

BtStatus prx_GattDiscoveryCharServicesByUUID(PrxPacket *packet, U16 uuid, U16 start, U16 end);
BtStatus prx_GattReadCharValue(PrxPacket *packet, U16 char_handle);
BtStatus prx_GattWriteCharValue(PrxPacket *packet, U16 char_handle);
BtStatus prx_GattWriteWithoutResponse(PrxPacket *packet, U16 char_handle, U8 value);

static void prxServerAllocateProximityDataBase(BtRemoteDevice *link);
static void prxServerReleaseProximityDataBase(BtRemoteDevice *link);
static void prxClientQueryResult(PrxPacket *packet, U8 *data, U8 len, U8 result);

BOOL release_monitor(PrxMTransport *chnl);
BOOL assign_monitor(PrxMTransport *chnl);
PrxMTransport *get_monitor_by_remdev(BtRemoteDevice *dev);



/**
 * @brief create a packetpool for prxr
 */
void prx_packet_pool_init() {
	U8 i;
	/* Initialize the Packet List */
	InitializeListHead(&g_prx_packetlist);

	/* Fill in the Packet List according to the number of
	 * available transmit packets.
	 */
	for (i = 0; i < prx_MAX_PACKET_NUM; i++) {
		g_prx_packet[i].checkid = PRX_CHECK_ID_VALUE;
		InsertTailList(&(g_prx_packetlist), &(g_prx_packet[i].node));
	}

	OS_Report("[PRX:] check g_prx_reporterlist_init:%d", g_prx_reporterlist_init);
	if( g_prx_reporterlist_init == TRUE ){
		g_prx_reporterlist_init = FALSE;
	}

	/* register the GATT callback to handle packet pool */
	BT_GattRegisterConnectedEvent(&g_prx_adapter_gatthdl, ProximityCommonGattCallback);
}

/**
 * @brief get a packet from pool
 */
PrxPacket *prx_get_packet(void *user) {
	PrxPacket *pkt = NULL;
	PrxMContext *pCntx;

	pCntx = (PrxMContext *) user;
	PRXASSERT( (pCntx->isuse == TRUE) );

	if (!IsListEmpty(&g_prx_packetlist)) {
		pkt = (PrxPacket *) RemoveHeadList((ListEntry *) &g_prx_packetlist);
		pkt->pUser = user;

		PRX_DBG1("[PRX:] get_packet 0x%x", pkt);
	}
	if (NULL == pkt) {
		PRX_DBG("[PRX:][ERR] get_packet fail! list is empty!");
	}
	return pkt;
}

/**
 * @brief return a packet to pool
 */
void prx_return_packet(PrxPacket *packet) {
	if (packet->checkid != PRX_CHECK_ID_VALUE) {
		PRX_DBG1("[PRX:][ERR] return wrong packet 0x%x", packet);
	} else {
		// return the packet
		PRX_DBG1("[PRX:] return_packet 0x%x", packet);
		InsertTailList(&(g_prx_packetlist), &packet->node);
	}
}

/**********************************************************************/
BtStatus prx_att_register(void *channel, void *callback, void *info) {
	PrxRTransport *chnl = (PrxRTransport *) channel;
	L2capPsm *psm = &chnl->psm;
	BtStatus status;
	U8 i;

	PRX_DBG2("[PRX:] prx_att_register channeldata:0x%x regFlag:0x%x", channel, chnl->regFlag);
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
    chnl->regFlag = 0;
	status = CMGR_RegisterHandler(&chnl->handler, prxrServerCmgrConnectionCallback);
	PRX_DBG1("[PRX:] CMGR_RegisterHandler status:%d", status);
	if( BT_STATUS_SUCCESS == status ){
		chnl->regFlag |= PRX_CMGR_REG;
	}
	
	// append Prxr service to linklist
	if( TRUE != g_prx_reporterlist_init ){
		g_prx_reporterlist_init = TRUE;
		InitializeListHead( &g_prx_reporterlist);
		for( i=0; i<PRX_MAX_REPORTER_NUM; i++){
			g_prx_used_reporterarray[i] = NULL;
		}
		OS_Report("[PRX:] InitializeListHead reporterlist");
	}
	InsertTailList(&(g_prx_reporterlist), &(chnl->node));
	PRX_DBG1("[PRX:] InsertTailList 0x%x to reportlist", &(chnl->node));

	return BT_STATUS_SUCCESS;
}

BtStatus prx_att_registerrecord(void *channel) {
	PrxRTransport *chnl = (PrxRTransport *) channel;
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	/* add the service record */
	// BR/EDR need to register 3 service record
	// 1. mandatory link loss record
	// 2. txpwer record
	// 3. imalert record
	
	PrxSdpDB_AddRecord(0, 0xff);
    PrxAttDB_Registration();
	
	return BT_STATUS_SUCCESS;
}

BtStatus prx_att_deregister(void *channel) {
	BtStatus status = BT_STATUS_SUCCESS;
	PrxRTransport *chnl = (PrxRTransport *) channel;
	L2capPsm *psm = &chnl->psm;

	PRX_DBG1(" prx_att_deregister channeldata:0x%x", channel);
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));

    status = CMGR_DeregisterHandler(&(chnl->handler));
 	PRX_DBG1("[PRX:] CMGR_DeregisterHandler status:%d", status);

	if( TRUE == g_prx_reporterlist_init ){
	 	PRX_DBG1("[PRX:] RemoveentryList 0x%x from reportlist", &(chnl->node));
		RemoveEntryList(&(chnl->node));
	}

	return BT_STATUS_SUCCESS;
}

BtStatus prx_att_deregisterrecord(void *channel) {
	PrxRTransport *chnl = (PrxRTransport *) channel;
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));

	/* remove the service record */
	PrxSdpDB_RemoveRecord(0, 0xff);
    PrxAttDB_Deregistration();
	
	return BT_STATUS_SUCCESS;
}

/**
 * @brief send packet by channel
 */
BtStatus prx_att_sendpacket(void *channel, void *packet) {
	PRX_DBG2(" prx_att_sendpacket chnlId:%d packet:0x%x", channel, packet );
	return BT_STATUS_SUCCESS;
}

BtStatus prx_att_acceptincoming(void *channel) {
	PrxRTransport *chnl = (PrxRTransport *) channel;
	BtStatus status;

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	status = BT_STATUS_SUCCESS;
	return status;
}

BtStatus prx_att_rejectincoming(void *channel) {
	PrxRTransport *chnl = (PrxRTransport *) channel;
	BtStatus status;

	PRX_DBG1("[PRX:] prx_att_rejectincoming id:%d ", chnl->id);
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));

	status = GattDisconnect(chnl->handler.remDev);
	PRX_DBG1("[PRX:] GattDisconnect status:%d", status);

	status = CMGR_RemoveDataLink(&(chnl->handler));
	PRX_DBG1("[PRX:] CMGR_RemoveDataLink status:%d", status);

    // clean up the record
    release_reporter(chnl);

	status = BT_STATUS_SUCCESS;

	return status;
}

/**
 * @brief ATT_REPORTER_DISCONNECT
 * reporter(server) disconnect
 * When acl is lost, release the context.
 * Keep the cmgr callback for next incoming connection.
 */
BtStatus prx_att_disconnect_by_reporter(void *channel) {
	PrxRTransport *chnl = (PrxRTransport *) channel;
	BtStatus status;

	PRX_DBG2(" prx_att_disconnect id:%d regFlag:0x%x", chnl->id, chnl->regFlag);
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));

	status = GattDisconnect(chnl->handler.remDev);
	PRX_DBG1("[PRX:] GattDisconnect status:%d", status);

	status = CMGR_RemoveDataLink(&(chnl->handler));
	PRX_DBG1("[PRX:] CMGR_RemoveDataLink status:%d", status);

	if( status == BT_STATUS_SUCCESS ){
		release_reporter(chnl);
	}
	return status;
}

/**
 * @brief ATT_MONITOR_DISCONNECT
 * When acl link is lost, remove all the cmgr & gatt callback.
 */
BtStatus prx_att_disconnect_by_monitor(void *channel) {
	PrxMTransport *chnl = (PrxMTransport *) channel;
	BtStatus status;

	PRX_DBG2("[PRX:] prx_att_disconnect_by_monitor id:%d regFlag:0x%x", chnl->id, chnl->regFlag);
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	
	status = GattDisconnect(chnl->handler.remDev);
	PRX_DBG1("[PRX:] GattDisconnect status:%d", status);

    if( status == BT_STATUS_SUCCESS ){ // no cmevent back
		static_monitor_removeCmgr(chnl, TRUE);
    }    

	return status;
}

BtStatus prx_get_rssi(void *channel) {
	PrxMTransport *pChnl;

	BtStatus status = BT_STATUS_FAILED;

	pChnl = (PrxMTransport *) channel;
	PRX_DBG1("[PRX:] prx_get_rssi chnl:0x%x", channel);
	PRXASSERT( (pChnl->trantype == PRX_TRANSTYPE_CHECK) );

	if( FALSE == g_reading_rssi ){
		g_reading_rssi = TRUE;
		g_reading_rssi_owner = (void *) pChnl;
		status = PrxClientReadRSSI(pChnl->handler.remDev);
		if( status != BT_STATUS_PENDING ){
	        PRX_DBG1("[PRX:] prx_get_rssi status:%d", status);		    
		}
	}else{
		status = BT_STATUS_BUSY;
	}

	return status;
}

BtStatus prx_get_local_txpower(void *channel) {
	PrxRTransport *pChnl; //NOTE! only reporter!

	BtStatus status = BT_STATUS_FAILED;

	pChnl = (PrxRTransport *) channel;
	PRX_DBG1("[PRX:] prx_get_local_txpower chnl:0x%x", channel);
	PRXASSERT( (pChnl->trantype == PRX_TRANSTYPE_CHECK) );

	/* invoke the ME api to get the txpower by dev*/
	if( FALSE == g_reading_txpower && NULL != pChnl->handler.remDev ){
		g_reading_txpower = TRUE;
		g_reading_txpower_owner = channel;
		status = prxServerReadTxPower(pChnl->handler.remDev);
		PRX_DBG1("[PRX:] status:%d", status);
	}else{
		status = BT_STATUS_BUSY;
	}	

	return status;
}

/* 
 * @brief Use flag to decide to invoke remove api
 * prx needs to use CMGR_RemoveDataLink to trigger prx disconnect.
 * Since there is no api for gatt disconnect api to trigger disconnection and
 * CMGR_RemoveDataLink cannot be removed twice, 
 * prx uses flag to check whether need to remove or not.
 *
 * 
 */
BtStatus static_monitor_removeCmgr(PrxMTransport *pChnl, U8 lost) {
	BtStatus status = BT_STATUS_SUCCESS;
	PRX_DBG3("[PRX:] static_monitor_removeCmgr PrxMTranport:0x%x lost:%d regFlag:0x%x", pChnl, lost, pChnl->regFlag);
	PRXASSERT( (pChnl->trantype == PRX_TRANSTYPE_CHECK) );

	if( 0 != (pChnl->regFlag & PRX_CMGR_CREATE_REG) ){
		status = CMGR_RemoveDataLink(&(pChnl->handler));
		PRX_DBG1("[PRX:] CMGR_RemoveDataLink status:%d", status);

		pChnl->regFlag = pChnl->regFlag & (~PRX_CMGR_CREATE_REG);
	}
	
	if( 0 != (pChnl->regFlag & PRX_CMGR_REG) ){
	    status = CMGR_DeregisterHandler(&(pChnl->handler));
	    PRX_DBG1("[PRX:] CMGR_DeregisterHandler status:%d", status);

		pChnl->regFlag = pChnl->regFlag & (~PRX_CMGR_REG);
	}

	if( TRUE == lost ){
		PRX_DBG2("[PRX:] static_monitor_removeCmgr no unregGATT %d regFlag:0x%x",lost, pChnl->regFlag);
	}
    
	return status;
}

void prxmClientCmgrConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
		BtStatus Status) {
	PrxMTransport *pChnl;
	PrxMContext *pCntx;
	BtStatus status = BT_STATUS_FAILED;

	// Traceback  psm -> PrxRTransport -> PrxRContext
	pChnl = (PrxMTransport *) ContainingRecord( Handler, PrxMTransport, handler);
	pCntx = (PrxMContext *) ContainingRecord( pChnl, PrxMContext, chnl);

	PRXASSERT( (pChnl->trantype == PRX_TRANSTYPE_CHECK) );

	PRX_DBG3("[PRX:] prxmClientCmgrConnectionCallback event:%d status:%d cntx-state:%d ", Event, Status, pCntx->state);
	switch (Event) {
        case CMEVENT_READ_TX_POWER_COMPLETED:
            break;            
        case CMEVENT_READ_RSSI_COMPLETED:
            prxClientReportRSSI(Handler->remDev, Handler->errCode);
            break;        
	case CMEVENT_DATA_LINK_CON_CNF:
		if (pCntx->state == PRXMS_LINK_CONNECT) {
			if (Status == BT_STATUS_SUCCESS) {
                PRX_DBG1("[PRX:] CMGR is equal connected 0x%x", &(pChnl->handler));
				prxm_notifyConnectResult(pCntx, TRUE, 0);
			} else {
				prxm_notifyConnectResult(pCntx, FALSE, Status);
				PRX_DBG1("[PRX:] goto prepare CMGR_RemoveDataLink 0x%x", &(pChnl->handler));
				static_monitor_removeCmgr(pChnl, FALSE);
			}
		} 
        else if(pCntx->state == PRXMS_DISCONNECTING) {
			// disconnect it
			if( Status == BT_STATUS_SUCCESS ){
				status = prx_att_removedatalink(&pCntx->chnl);
				PRX_DBG1("[PRX:] abort connection status:%d", status);
				// wait the final cmevent to get the notify
			}else{
			    // no more cmevent
				if(BT_STATUS_SUCCESS == status){
					prxm_notifyLinkResult(pCntx, FALSE, status);
				}
			}
		}else{
			//
			PRX_DBG1("[PRX:] error! state:%d", pCntx->state);
		}
		break;
	case CMEVENT_DATA_LINK_DIS:
		// local disconnect by lower level or upper level
		PRX_DBG2("[PRX:] CMEVENT_DATA_LINK_DIS handler 0x%x state:%d", &(pChnl->handler), pCntx->state);

		switch(pCntx->state){
			case PRXMS_QUERY:
			case PRXMS_CONNECTING:
				prxm_notifyConnectResult(pCntx, FALSE, status);							
				break;
			default:
				prxm_notifyLinkResult(pCntx, FALSE, status);			
			break;
		}

		// recycle the monitor
		g_prx_monitor = get_monitor_by_remdev(pChnl->handler.remDev);
		if( g_prx_monitor == pChnl ){
			//g_prx_monitor = NULL; // has been removed
			release_monitor(g_prx_monitor ); 
		}

		static_monitor_removeCmgr(pChnl, TRUE);
		
		break;
	}
}

static PrxRTransport *get_free_reporter(BtRemoteDevice *link){
    U8 i=0;
	U8 l_freecount = 0;
	U8 freeidx = PRX_MAX_REPORTER_NUM;
    
	PrxRTransport *reporter = NULL;
	PrxRTransport *reporter1 = NULL;
	if( TRUE != g_prx_reporterlist_init ){
		return NULL;
	}
	
    for(i=0;i<PRX_MAX_REPORTER_NUM; i++)
    {
        reporter1 = g_prx_used_reporterarray[i];
		if( NULL == reporter1 ){
			l_freecount++;
			continue;
		}
        if( NULL != reporter1 && OS_MemCmp(reporter1->bdaddr.addr,6, link->bdAddr.addr, 6) == TRUE)
        {
        	// has a used record
            return NULL;
        }
    }
	if( l_freecount == 0 ){
		OS_Report("[PRX:][WRN] No free reporterarry cell");
            return NULL;
	}else{
		// assign freeidx to a free position
		for(i=0;i<PRX_MAX_REPORTER_NUM; i++){
			if( NULL == g_prx_used_reporterarray[i] ){
				freeidx = i;
				break;
        }
    }
	}
	OS_Report("[PRX:] freeidx:%d", freeidx);
	if ( freeidx < PRX_MAX_REPORTER_NUM && !IsListEmpty(&g_prx_reporterlist)) {
		reporter = (PrxRTransport *) RemoveHeadList(&g_prx_reporterlist);
        g_prx_used_reporterarray[freeidx] = reporter;
        reporter->isUsed = TRUE;            
        memcpy(reporter->bdaddr.addr, link->bdAddr.addr, 6);         
	}else{
		OS_Report("[PRX:][WRN] no free reporter ");
	}
	return reporter;
}

static BOOL release_reporter(PrxRTransport *reporter){
	U8 i;
	if( TRUE != g_prx_reporterlist_init ){
		return FALSE;
	}
	
	if(reporter && TRUE == reporter->isUsed){
		reporter->isUsed = FALSE;
		InsertTailList(&(g_prx_reporterlist), &(reporter->node));
		for(i = 0; i<PRX_MAX_REPORTER_NUM; i++ ){
			if( g_prx_used_reporterarray[i] == reporter ){
				g_prx_used_reporterarray[i] = NULL;
			}
		}
		return TRUE;
	}
	return FALSE;
}

static BOOL assign_reporter_by_remdev(PrxRTransport *reporter, BtRemoteDevice *dev){
	U8 i;
	if( TRUE == reporter->isUsed ){
		return FALSE;
	}

	for(i = 0; i<PRX_MAX_REPORTER_NUM; i++ ){
		if( NULL == g_prx_used_reporterarray[i] ){
			g_prx_used_reporterarray[i] = reporter;
			reporter->isUsed = TRUE;			
			memcpy(reporter->bdaddr.addr, dev->bdAddr.addr, 6);			
			break;
		}
	}
	if( TRUE == reporter->isUsed ){
		return TRUE;
	}
	return TRUE;
}

PrxRTransport *get_reporter_by_remdev(BtRemoteDevice *dev){
	PrxRTransport *reporter;
	U8 i;
	// reporter = g_prxr_chnl;
	for(i = 0; i<PRX_MAX_REPORTER_NUM; i++ ){
		if( NULL == g_prx_used_reporterarray[i] ){
			continue;
		}
		reporter = g_prx_used_reporterarray[i];
		if( reporter && reporter->isUsed == TRUE ){
			if( 0 == memcmp(dev->bdAddr.addr, reporter->bdaddr.addr, 6) ){
				return reporter;
			}else{
				OS_Report("[PRX:] different addres" );
			}
		}
	}
	OS_Report("[PRX:] cannot find assigned reporter");
	return NULL;
}

BOOL release_monitor(PrxMTransport *chnl){
	U8 i;
	for(i = 0; i<PRX_MAX_MONITOR_NUM; i++ )
	{
		if( chnl == g_prx_used_monitorarray[i] )
		{
			OS_Report("[PRX:] In release_monitor, find released index: %d", i);
			g_prx_used_monitorarray[i] = NULL;
			return TRUE;
		}
	}
	OS_Report("[PRX:] In release_monitor, find no index wanted");
	return FALSE;
}



BOOL assign_monitor(PrxMTransport *chnl){
	U8 i;
	for(i = 0; i<PRX_MAX_MONITOR_NUM; i++ )
	{
		if( NULL == g_prx_used_monitorarray[i] )
		{
			OS_Report("[PRX:] In assign_monitor, find free index: %d", i);
			g_prx_used_monitorarray[i] = chnl;
			return TRUE;
		}
	}
	OS_Report("[PRX:] In assign_monitor, find no free index");
	return FALSE;
}

PrxMTransport *get_monitor_by_remdev(BtRemoteDevice *dev){
	PrxMTransport *monitor;
	U8 i;
	if(dev == NULL){
		OS_Report("[PRX:]In get_monitor_by_remdev, invalid BtRemoteDevice");
		return NULL;
	}
	for(i = 0; i<PRX_MAX_MONITOR_NUM; i++ ){
		if( NULL == g_prx_used_monitorarray[i] ){
			continue;
		}
		monitor = g_prx_used_monitorarray[i];
    	if( 0 == memcmp(dev->bdAddr.addr, monitor->bdaddr.addr, 6) ){
		OS_Report("[PRX:]In get_monitor_by_remdev find assigned reporter in index %d",i);
	    	return monitor;
		}else{
			// OS_Report("[PRX:] different addres. find next" );
		}
	}
	OS_Report("[PRX:]In get_monitor_by_remdev cannot find assigned reporter");
	return NULL;
}


void prxrServerCmgrConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
		BtStatus Status) {
	PrxRTransport *chnl;
	PrxRContext *pChnl;

	chnl = (PrxRTransport *) ContainingRecord( Handler, PrxRTransport, handler);
	PRX_DBG2("[PRX:] prxrServerCmgrConnectionCallback event:%d status:%d cntx-state:%d ", Event, Status);
	switch (Event) {
        case CMEVENT_READ_RSSI_COMPLETED:
            break;
        case CMEVENT_READ_TX_POWER_COMPLETED:
            prxServerReportTxPower(Handler->remDev, Handler->errCode);
            break;            
		case CMEVENT_DATA_LINK_CON_CNF:
			// find a available reporter to accept the incoming connection
			break;
		case CMEVENT_DATA_LINK_CON_IND:
            // do nothing
			break;
		case CMEVENT_DATA_LINK_DIS:
			// find the reporter to handler
			chnl = get_reporter_by_remdev(Handler->remDev);
			if( chnl != NULL ){
				pChnl = (PrxRContext *) ContainingRecord( chnl, PrxRContext, chnl);
				prxr_notifyDisconnected(pChnl);

				// release the 
				release_reporter(chnl);

	            prxServerReleaseProximityDataBase(Handler->remDev);

				if( (PRX_CMGR_CREATE_REG & chnl->regFlag ) ){
    	        CMGR_RemoveDataLink(Handler);
					chnl->regFlag = chnl->regFlag & ~PRX_CMGR_CREATE_REG;
				}else{
					OS_Report("[PRX:] ignore removeDatalink regFlag:0x%x", chnl->regFlag);
				}
				
    	        
			}
			break;
	}
}

BtStatus prxr_att_serverhandleDataIncoming(BtRemoteDevice *link)
{
	PrxRTransport *chnl;
	PrxRContext *pChnl;
    
    // an incoming acl connection request has been connected
    chnl = get_free_reporter(link);
    Report(("[PRX:] prxr_att_serverhandleDataIncoming chnl:%08x",chnl));
    if( chnl != NULL )
    {
        prxServerAllocateProximityDataBase(link);
        prxServerReadTxPower(link);
        chnl->regFlag |= PRX_CMGR_CREATE_REG;
        CMGR_CreateDataLink(&chnl->handler, &link->bdAddr);

        pChnl = (PrxRContext *) ContainingRecord( chnl, PrxRContext, chnl);
		// copy address to cntx
		memcpy( chnl->bdaddr.addr, link->bdAddr.addr, sizeof(link->bdAddr));
        prxr_notifyIncoming(pChnl);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

BtStatus prx_att_createdatalink(void *channel, char *addr) {
	PrxMTransport *chnl = (PrxMTransport *) channel;
	BtStatus status;

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	PRX_DBG1(" prx_att_createdatalink chnl:0x%x", channel);

	// copy addr to chnl.
	chnl->bdaddr.addr[5] = addr[5];
	chnl->bdaddr.addr[4] = addr[4];
	chnl->bdaddr.addr[3] = addr[3];
	chnl->bdaddr.addr[2] = addr[2];
	chnl->bdaddr.addr[1] = addr[1];
	chnl->bdaddr.addr[0] = addr[0];

	PRX_DBG3("[PRX:] prx_att_createdatalink(1~3) 0x%x 0x%x 0x%x", chnl->bdaddr.addr[0], chnl->bdaddr.addr[1], chnl->bdaddr.addr[2]);
	PRX_DBG3("[PRX:] prx_att_createdatalink(4~6) 0x%x 0x%x 0x%x", chnl->bdaddr.addr[3], chnl->bdaddr.addr[4], chnl->bdaddr.addr[5]);

	PRX_DBG1("[PRX:] prx_att_createdatalink regFlag:0x%x", chnl->regFlag);

    chnl->regFlag = 0;
	memset(&chnl->handler, 0, sizeof(chnl->handler));

	status = CMGR_RegisterHandler(&chnl->handler, prxmClientCmgrConnectionCallback);
	PRX_DBG2("[PRX:] CMGR_RegisterHandler 0x%x status:%d", &chnl->handler, status);

	if( BT_STATUS_SUCCESS == status ){
		chnl->regFlag |= PRX_CMGR_REG;
	}


	if (BT_STATUS_SUCCESS == status) {
		PRX_DBG1("[PRX:] CMGR_CreateDataLink cmgr_handle:0x%x", &chnl->handler);
		status = CMGR_CreateDataLink(&chnl->handler, &chnl->bdaddr);
		chnl->regFlag |= PRX_CMGR_CREATE_REG;
	}

	if( BT_STATUS_SUCCESS != status && BT_STATUS_PENDING != status ){
		// deregister
		PRX_DBG1("[PRX:] CMGR_DeregisterHandler 0x%x", &chnl->handler);
		CMGR_DeregisterHandler(&(chnl->handler));

		chnl->regFlag = chnl->regFlag & (~PRX_CMGR_REG);
	}

	PRX_DBG2("[PRX:] prx_att_createdatalink end status:%d regFlag:0x%x", status, chnl->regFlag);
	return status;
}

/**
 * @brief monitor to remote the datalink
 */
BtStatus prx_att_removedatalink(void *channel) {
	PrxMTransport *chnl = (PrxMTransport *) channel;
	BtStatus status;

	PRX_DBG2("[PRX:] prx_att_removedatalink chnl:0x%x regFlag:0x%x", channel, chnl->regFlag);

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));

	status = CMGR_RemoveDataLink(&chnl->handler);
	PRX_DBG1("[PRX:] CMGR_RemoveDataLink 0x%x", &chnl->handler);
	chnl->regFlag = chnl->regFlag & ~PRX_CMGR_CREATE_REG;
	if( status == BT_STATUS_SUCCESS ){	
		static_monitor_removeCmgr(chnl, TRUE);
	}

	PRX_DBG1("[PRX:] prx_att_removedatalink end status:%d", status);

	return status;
}


static void PrxmQuerySdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
		PrxMTransport *chnl;
		PrxMContext *pCntx;
		BtStatus status = BT_STATUS_FAILED; 
		OS_Report("[PRX:] PrxmQuerySdpCallback result:%d index:%d", result, serv_chn);
	
		if( NULL == sqt ){
			return;
		}
	
		chnl = (PrxMTransport *) ContainingRecord( sqt, PrxMTransport, sdpQueryToken);
		pCntx = (PrxMContext *) ContainingRecord( chnl, PrxMContext, chnl);
	
		if( chnl->trantype != PRX_TRANSTYPE_CHECK){
			PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
			OS_Report("[PRX:][ERR] trantype is not %d", PRX_TRANSTYPE_CHECK);
			return;
		}
		
		switch (result) 
		{
			case BT_STATUS_SUCCESS:
				// find it
				prxm_notifyQueryResult(pCntx, TRUE, BT_STATUS_SUCCESS);

				status = BT_STATUS_SUCCESS;
			break;
		}
		if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS )
		{

			// fail result
			status = GattDisconnect(chnl->handler.remDev);
			PRX_DBG1("[PRX:] GattDisconnect status:%d", status);			

			if( 0 != (chnl->regFlag & PRX_CMGR_CREATE_REG) ){
				status = CMGR_RemoveDataLink(&(chnl->handler));
				PRX_DBG1("[PRX:] CMGR_RemoveDataLink status:%d", status);			
				chnl->regFlag = chnl->regFlag & ~PRX_CMGR_CREATE_REG;
			}

			
			if( status == BT_STATUS_SUCCESS ){
				prxm_notifyQueryResult(pCntx, FALSE, BT_STATUS_SUCCESS);
				static_monitor_removeCmgr(chnl, TRUE);
			}else{
				// wait for CMGR disc event
			}
		}
}

/**
 * @brief Get other optional sdp record
 * Use timer to get the other optional sdp record
 * Since sdp api cannot be invoked in sdpcallback,
 * prx have to setup another timer to trigger the optional query.
 */
void prx_start_query_timer(EvmTimer *timer_id_p, U32 timer_duration, EvmTimerNotify funct_ptr, void* hf_param)
{
	EvmTimer *p_timer;
	p_timer = (EvmTimer *) timer_id_p;
	p_timer->context = hf_param;
	p_timer->func = (EvmTimerNotify) funct_ptr;
	EVM_StartTimer(p_timer, (timer_duration) );
}

static void prx_ias_serivce_Timeout(void *para){

	BtStatus status = BT_STATUS_PENDING;
    PrxMTransport *chnl;
	EvmTimer *ptimer;

	ptimer = (EvmTimer *)para;
	chnl = (PrxMTransport *)ptimer->context;
	
	status = prxm_att_queryservice(chnl, BT_UUID_GATT_SERVICE_ALERT, PrxmIASQuerySdpCallback);
}

static void prx_tps_serivce_Timeout(EvmTimer *ptimer){

	BtStatus status = BT_STATUS_PENDING;
    PrxMTransport *chnl;
	//EvmTimer *ptimer;

	//ptimer = (EvmTimer *)para;
	chnl = (PrxMTransport *)ptimer->context;
	
	status = prxm_att_queryservice(chnl, BT_UUID_GATT_SERVICE_TXPOWER, PrxmTPSQuerySdpCallback);
}


static void PrxmIASQuerySdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
	PrxMTransport *chnl;
	PrxMContext *pCntx;
	BtStatus status = BT_STATUS_FAILED; 
	OS_Report("[PRX] PrxmIASQuerySdpCallback result:%d index:%d", result, 0);

	if( NULL == sqt ){
		return;
	}

	chnl = (PrxMTransport *) ContainingRecord( sqt, PrxMTransport, sdpQueryToken);
	pCntx = (PrxMContext *) ContainingRecord( chnl, PrxMContext, chnl);

	if( chnl->trantype != PRX_TRANSTYPE_CHECK){
		PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
		OS_Report("[PRX:][ERR] trantype is not %d", PRX_TRANSTYPE_CHECK);
		return;
	}
	
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
        prx_start_query_timer(&g_prx_sdp_timer, 100, prx_tps_serivce_Timeout, chnl);
        break;
    }
}


static void PrxmTPSQuerySdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
	PrxMTransport *chnl;
	PrxMContext *pCntx;
	BtStatus status = BT_STATUS_FAILED; 
	OS_Report("[PRX] PrxmTPSQuerySdpCallback result:%d index:%d", result, 0);

	if( NULL == sqt ){
		return;
	}

	chnl = (PrxMTransport *) ContainingRecord( sqt, PrxMTransport, sdpQueryToken);
	pCntx = (PrxMContext *) ContainingRecord( chnl, PrxMContext, chnl);

	if( chnl->trantype != PRX_TRANSTYPE_CHECK){
		PRXASSERT( chnl->trantype == PRX_TRANSTYPE_CHECK );
		OS_Report("[PRX][ERR] trantype is not %d", PRX_TRANSTYPE_CHECK);
		return;
	}
	
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
        // done!
					
        break;
    }  
}

BtStatus prxm_att_queryservice(PrxMTransport *chnl, U16 uuid, SdapCallBack callback){
	BtStatus status = BT_STATUS_FAILED;
	sdap_service_search_struct search_pattern;

	if( chnl && chnl->handler.remDev ){
		chnl->sdpQueryToken.uuid = uuid;
		chnl->sdpQueryToken.mode = BSPM_BEGINNING;
		search_pattern.rm = (chnl->handler.remDev);
		search_pattern.sqt = &chnl->sdpQueryToken;
		search_pattern.uuid = uuid;
		
		search_pattern.callback = callback;  

		status = SDAP_ServiceSearch(search_pattern);
	}
	return status;
}


BtStatus prx_att_queryservice(void *channel) {
	PrxMTransport *chnl = (PrxMTransport *) channel;
	BtStatus status = BT_STATUS_FAILED;

	PRX_DBG1("[PRX:] prx_att_queryservice chnl:0x%x", channel);

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	
	status = BT_STATUS_SUCCESS;
	// start a sdp query
	{
#if 0	
		sdap_service_search_multi_attribute_struct search_pattern;
		
		chnl->sdpQueryToken.rm = chnl->handler.remDev;
		/* Reset the query flag */
		search_pattern.rm = chnl->handler.remDev;
		search_pattern.sqt = &chnl->sdpQueryToken;
		search_pattern.callback = PrxMSdpCallback;
		search_pattern.uuid = (U16) SC_LINKLOSS_ALERT; //TODO: UUID of link loss 
		search_pattern.attribute_id = prx_attribute_id_list;
		
		status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);
#endif
#if 0
		sdap_service_search_struct search_pattern;
		chnl->sdpQueryToken.uuid = SC_LINKLOSS_ALERT;
		chnl->sdpQueryToken.mode = BSPM_BEGINNING;
		search_pattern.rm = (chnl->handler.remDev);
		search_pattern.sqt = &chnl->sdpQueryToken;
		search_pattern.uuid = SC_LINKLOSS_ALERT;
		
		search_pattern.callback = PrxmQuerySdpCallback;  

		status = SDAP_ServiceSearch(search_pattern);
#endif
		status = prxm_att_queryservice(chnl, BT_UUID_GATT_SERVICE_LINKLOSS, PrxmQuerySdpCallback);
	}

	return status;
}

BtStatus prx_att_stopqueryservice(void *channel) {
	PrxMTransport *chnl = (PrxMTransport *) channel;
	BtStatus status = BT_STATUS_FAILED;

	PRX_DBG1("[PRX:] prx_att_stopqueryservice chnl:0x%x", channel);

	PRX_DBG("[PRX:] prx_att_stopqueryservice");
	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));

	// unable to stop the query
	return status;
}

/**
 * @brief Monitor(client) connect to remote serivce.
 */
BtStatus prx_att_connectservice(void *channel) {
	PrxMTransport *chnl = (PrxMTransport *) channel;
	BtStatus status = BT_STATUS_FAILED;

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	PRX_DBG2("[PRX:] prx_att_connectservice channel:0x%x regFlag:0x%x", channel, chnl->regFlag);

	// if cmgr is connect, just return true
	if( chnl->handler.remDev ){

        //g_prx_monitor = chnl;
        if(!assign_monitor(chnl)){
		return BT_STATUS_NO_RESOURCES;
	}
		status = GattClientConnect(chnl->handler.remDev); // wait BT_GATT_APP_EVENT_CONNECTED event
        PRX_DBG1("[PRX:] GattClientConnect status:%d", status);
	}else{
		PRX_DBG("[PRX:][ERR] chnl->handler.remDev is NULL ! ");
		status = BT_STATUS_INVALID_PARM;
	}

	return status;
}

#ifdef _AAA_
/**
 * @brief Read the local ATT table
 * Sync code
 */
BtStatus prx_att_writelocalvalue(U32 id, U32 type, void *value) {
	BtStatus status = BT_STATUS_FAILED;
	PRX_DBG1("[PRX:] prx_att_writelocalvalue id:%d", id);


	if (NULL == value) {
		PRX_DBG("[PRX:][ERR] NULL parameter");
		return status;
	}

	PRX_DBG2("[PRX:][ERR] PRX_BY_GATT not implement yet readlocalvalue id:%d type:%d", id, type);
	return status;
}


/**
 * @brief Read the local ATT table
 * Packet format:
 * 1 (B) magic value 0x5f
 * 1 (B) length
 * 1 (B) seqid
 * 1 (B) type of command
 * (0 for update, 1 for read, 2 for write, 0x10 for update response
 * 0x11 for read response, 0x22 for write response)
 * 4 (B) attribute id
 * 1~4 (B) attribute value
 * total length: 9~13(0x9~0xd)
 */
BtStatus prx_att_readremotevalue(void *packet, U8 seqid, U32 id, U32 type) {
	// use 
	PrxPacket *prxp;
	BtStatus status = BT_STATUS_FAILED;
	prxp = (PrxPacket *) packet;
	PRXASSERT( (prxp->checkid == PRX_CHECK_ID_VALUE) );

	/* save some history record */
	prxp->seqid = seqid;
	prxp->cmd = id;
	prxp->type = type;
	prxp->value = 0;

	PRX_DBG1("[PRX:] prx_att_readremotevalue id:%d", id);

	if (GATT_TYPE_S8 != type) {
		PRX_DBG1("[PRX:][ERR] prx_att_readremotevalue wrong type:%d", type);
		return status;
	}

	{
		PrxMTransport *pChnl;
		PrxMContext *pCntx;

		pCntx = (PrxMContext *) prxp->pUser;
		pChnl = (PrxMTransport *) &pCntx->chnl;

		//gattTesterTestCase0(pChnl->handler.remDev);
		status = BT_STATUS_SUCCESS;
	}
	return status;
}
#endif


static void prxServerAllocateProximityDataBase(BtRemoteDevice *link)
{
    U8 i=0;
    
    for (i=0; i<MAX_PROXIMITY_DATABASE; i++)
    {
        if (ProximityReporterDatabase[i].used == 0x00)
        {
            ProximityReporterDatabase[i].used = 1;
            ProximityReporterDatabase[i].link = link;
            break;
        }
    }
}

/**
 * @brief Proximity Reporter's GATT database
 */
static void prxServerReleaseProximityDataBase(BtRemoteDevice *link)
{
    U8 i=0;
    
    for (i=0; i<MAX_PROXIMITY_DATABASE; i++)
    {
        if ((ProximityReporterDatabase[i].used == 0x01) &&
            (ProximityReporterDatabase[i].link == link))
        {
            ProximityReporterDatabase[i].used = 0;
            ProximityReporterDatabase[i].link = 0;
            break;
        }
    }
}



/* @brief Read RSSI by dev
 * D: down invok 
 * Use ME_ api to get the rssi value by Device
 * The result will be got from CMGR callback.
 */
BtStatus PrxClientReadRSSI(BtRemoteDevice *remDev)
{
    return ME_ReadRSSI(remDev);
}


/* Up: RSSI */
void prxClientReportRSSI(BtRemoteDevice *remDev, U8 errorCode)
{
	PrxMContext *pCntx = NULL;
    S8 RSSI = -128;
    if( 0 != errorCode ){
	Report(("[PRX:] RSSIdone remDev:0x%x error code:%d", remDev, errorCode));
    Report(("[PRX:] Client Read RSSIdone, error code:%d", errorCode));
    }
    Report(("[PRX:] Client Read RSSIdone, remDev->rssi:%d", remDev->rssi));

    if(errorCode == 0x00)
    {
        RSSI = remDev->rssi;
    }
	g_reading_rssi = FALSE;
	// report to upper layer
	if( g_reading_rssi_owner != NULL ){
		pCntx = (PrxMContext *) ContainingRecord( g_reading_rssi_owner, PrxMContext, chnl);
		if( 0x00 == errorCode ){
			prxm_notifyGetRssiResult( pCntx, TRUE, (S32) RSSI );
		}else{
			prxm_notifyGetRssiResult( pCntx, FALSE, (S32) RSSI );		
		}
	}
}

/**
 * @breif Use ME_ api to get the TxPower value
 * Regarding ot firmware team, the TxPower is fixed value.
 * We coudl read it once or a few times but it has the same value.
 * The result will be got from CMGR callback.
 */
BtStatus prxServerReadTxPower(BtRemoteDevice *remDev)
{
    return ME_ReadTxPower(remDev);
}


void prxServerReportTxPower(BtRemoteDevice *remDev, U8 errorCode)
{
    S8 txPower = 0;
	PrxRContext *pCntx = NULL;

    U8 i=0;
    Report(("[PRX:] Server Read Tx Power done, error code:%d", errorCode));
    Report(("Server Read Tx Power done, remDev->txPower:%02x", remDev->txPower));    
    if(errorCode == 0x00)
    {
        txPower = remDev->txPower;
        for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
        {
            if(remDev == ProximityReporterDatabase[i].link)
            {
            	// update the txPower value inside GATT table
                ProximityReporterDatabase[i].txPower = txPower;
                /* Read TX power done. */
                
            }
        }
    }
	g_reading_txpower = FALSE;
	if( g_reading_txpower_owner != NULL){
		pCntx = (PrxRContext *) ContainingRecord( g_reading_txpower_owner, PrxRContext, chnl);
		g_reading_txpower_owner = NULL;
		if( 0x00 == errorCode ){
			prxr_notifyGetTxPowerValue( pCntx, TRUE, (S32) txPower );
		}else{
			prxr_notifyGetTxPowerValue( pCntx, FALSE, (S32) txPower );		
		}
		
	}
}

void BtPrxServerUpdateAlertLevel(U8 alertLevel, BtRemoteDevice *remDev)
{
    U8 i=0;

    for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
    {
        if(remDev == ProximityReporterDatabase[i].link)
        {
            ProximityReporterDatabase[i].alertLevel = alertLevel;
        }
    }

}


U8 prxr_getDatabaseIndex(U8 *addr)
{
    U8 i=0;

    for (i=0;i<MAX_PROXIMITY_DATABASE;i++)
    {
        if((ProximityReporterDatabase[i].link !=0) &&
            (OS_MemCmp(ProximityReporterDatabase[i].link->bdAddr.addr, 6, addr, 6) == TRUE))
        {
            return i;
        }
    }
    return MAX_PROXIMITY_DATABASE;
}

/* D: server down invok */
/* for server */
BtStatus prxr_readlocalvalue(void *channel, U32 id, U32 type, void *value) {
	BtStatus status = BT_STATUS_FAILED;
	S8 *s8value = (S8 *) value;
    U8 i=0;
    
	PrxRTransport *chnl = (PrxRTransport *) channel;

	PRXASSERT( (chnl->trantype == PRX_TRANSTYPE_CHECK) );

	PRX_DBG1("[PRX:] prx_att_readlocalvalue id:%d", id);
	// read characterictic GATT_CHARACTER_VALUE
	if (GATT_TYPE_S8 == type) {
		switch (id) {
		case GATT_PRX_TXPOWER:
			*s8value = 0;
            status = prxServerReadTxPower(chnl->handler.bdc->link);
			PRX_DBG3("[PRX:] try to read tx power id:%d value:%d status:%d", id, *s8value, status);
			break;
		case GATT_PRX_IMMEDIATE_SERVICE:
            i = prxr_getDatabaseIndex(chnl->bdaddr.addr);
			if( i < MAX_PROXIMITY_DATABASE ){
            *s8value = ProximityReporterDatabase[i].immAlertLevel;
			PRX_DBG2("[PRX:] g_prx_imalert id:%d value:%d", id, *s8value);
            status = BT_STATUS_SUCCESS;
			}else{
				PRX_DBG1("[PRX:] g_prx_imalert i:%d", i);
            	status = BT_STATUS_FAILED;
            }
			break;
		case GATT_PRX_LINKLOSS_SERVICE:
            i = prxr_getDatabaseIndex(chnl->bdaddr.addr);
			if( i < MAX_PROXIMITY_DATABASE ){
            *s8value = ProximityReporterDatabase[i].alertLevel;
			PRX_DBG2("[PRX:] x g_prx_lossalert id:%d value:%d", id, *s8value);
            status = BT_STATUS_SUCCESS;
			}else{
				PRX_DBG1("[PRX:] g_prx_imalert i:%d", i);
				status = BT_STATUS_FAILED;
			}

			break;
		default:
			status = BT_STATUS_FAILED;
			break;
		}
	}
	return status;
}

/* D: server down invok */
/* for server */
BtStatus prxr_writelocalvalue(void *channel, U32 id, U32 type, void *value) {
	BtStatus status = BT_STATUS_FAILED;
    U8 i=0;
	PrxRTransport *chnl = (PrxRTransport *) channel;

	PRXASSERT( (chnl->trantype == PRX_TRANSTYPE_CHECK) );
	
	PRX_DBG1("[PRX:] prx_att_writelocalvalue id:%d", id);

	if (NULL == value) {
		PRX_DBG("[PRX:][ERR] NULL parameter");
		return status;
	}

	// read characterictic GATT_CHARACTER_VALUE
	if (GATT_TYPE_S8 == type) {
		status = BT_STATUS_SUCCESS;
		switch (id) {
		case GATT_PRX_TXPOWER:
            i = prxr_getDatabaseIndex(chnl->bdaddr.addr);
            if(i == MAX_PROXIMITY_DATABASE)
                return BT_STATUS_FAILED;
            ProximityReporterDatabase[i].txPower = *((S8 *) value);
            PRX_DBG2("[PRX:] g_prx_txpower id:%d value:%d", id, ProximityReporterDatabase[i].txPower);
			break;
		case GATT_PRX_IMMEDIATE_SERVICE:
            i = prxr_getDatabaseIndex(chnl->bdaddr.addr);
            if(i == MAX_PROXIMITY_DATABASE)
                return BT_STATUS_FAILED;
            ProximityReporterDatabase[i].immAlertLevel = *((S8 *) value);
			PRX_DBG2("[PRX:] g_prx_imalert id:%d value:%d", id, ProximityReporterDatabase[i].immAlertLevel);
			break;
		case GATT_PRX_LINKLOSS_SERVICE:
            i = prxr_getDatabaseIndex(chnl->bdaddr.addr);
            if(i == MAX_PROXIMITY_DATABASE)
                return BT_STATUS_FAILED;
            ProximityReporterDatabase[i].alertLevel = *((S8 *) value);
			PRX_DBG2("[PRX:] g_prx_lossalert id:%d value:%d", id, ProximityReporterDatabase[i].alertLevel);
			break;
		default:
			status = BT_STATUS_FAILED;
			break;
		}
	} else {
		PRX_DBG2("[PRX:][ERR] wrong type prx_att_writelocalvalue id:%d type:%d", id, type);
	}
	return status;
}

/* D: client down invok */
BtStatus prxm_writeremotevalue(
        void *channel,
        void *packet, 
        U8 seqid, 
        U32 id, 
        U32 type,
		void *value) 
{
    PrxPacket *prxp;
    BtStatus status = BT_STATUS_FAILED;
	PrxMTransport *chnl = (PrxMTransport *) channel;

    prxp = (PrxPacket *) packet;

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
    PRXASSERT( (prxp->checkid == PRX_CHECK_ID_VALUE) );

	/* save some history record */
	prxp->seqid = seqid;
	prxp->cmd = id;
	prxp->type = type;
	prxp->value = *((U8 *) value); // assume the type is u8
    prxp->chnl = channel;

	PRX_DBG3("[PRX:] prx_att_writeremotevalue seqid:%d id:%d type:%d", seqid, id, type);
	PRX_DBG1("[PRX:] prx_att_writeremotevalue value:%d", prxp->value);

	if (GATT_TYPE_S8 != type) 
    {
		PRX_DBG1("[PRX:][ERR] prx_att_writeremotevalue wrong type:%d", type);
		return status;
	}

	switch(id) {
		case GATT_PRX_WRITE_TXPOWER:
    		// NO write remote Txpower !!!
    		status = BT_STATUS_FAILED;
            return status;
		case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
            prxp->buff[0] = *((U8 *) value);
            prxp->size = 1;
            status = prx_GattSendDiscoveryPrimaryServiceByUUID(
                                            prxp,
                                            BT_UUID_GATT_SERVICE_ALERT, 
                                            0x0001, 
                                            0xFFFF);
			PRX_DBG1("[PRX:] status:%d", status );
    		break;
		case GATT_PRX_WRITE_LINKLOSS_SERVICE:
            prxp->buff[0] = *((U8 *) value);
            prxp->size = 1;
            status = prx_GattSendDiscoveryPrimaryServiceByUUID(
                                            prxp,
                                            BT_UUID_GATT_SERVICE_LINKLOSS, 
                                            0x0001, 
                                            0xFFFF);
			PRX_DBG1("[PRX:] status:%d", status );			
    		break;
		default:
    		break;
	}
	return status;
}


/* D: client down invok */
BtStatus prxm_readremotevalue(
                    void *channel,
                    void *packet, 
                    U8 seqid, 
                    U32 id, 
                    U32 type) 
{
	// use 
	PrxPacket *prxp;
	BtStatus status = BT_STATUS_FAILED;
	PrxMTransport *chnl = (PrxMTransport *) channel;
	prxp = (PrxPacket *) packet;

	PRXASSERT(( chnl->trantype == PRX_TRANSTYPE_CHECK ));
	PRXASSERT( (prxp->checkid == PRX_CHECK_ID_VALUE) );

	/* save some history record */
	prxp->seqid = seqid;
	prxp->cmd = id;
	prxp->type = type;
	prxp->value = 0;
    prxp->chnl = chnl;
    
	PRX_DBG1("[PRX:] prx_att_readremotevalue id:%d", id);

	if (GATT_TYPE_S8 != type) 
    {
		PRX_DBG1("[PRX:][ERR] prx_att_readremotevalue wrong type:%d", type);
		return status;
	}

	switch(id) 
    {
		case GATT_PRX_TXPOWER:
            status = prx_GattSendDiscoveryPrimaryServiceByUUID(
                                            prxp,
                                            BT_UUID_GATT_SERVICE_TXPOWER, 
                                            0x0001, 
                                            0xFFFF);
			PRX_DBG1("[PRX:] status:%d", status );
    		break;
		case GATT_PRX_IMMEDIATE_SERVICE:
            status = prx_GattSendDiscoveryPrimaryServiceByUUID(
                                            prxp,
                                            BT_UUID_GATT_SERVICE_ALERT, 
                                            0x0001, 
                                            0xFFFF);
			PRX_DBG1("[PRX:] status:%d", status );			
    		break;
		case GATT_PRX_LINKLOSS_SERVICE:
            status = prx_GattSendDiscoveryPrimaryServiceByUUID(
                                            prxp,
                                            BT_UUID_GATT_SERVICE_LINKLOSS, 
                                            0x0001, 
                                            0xFFFF);
			PRX_DBG1("[PRX:] status:%d", status );			
    		break;
		default:
        	break;
	}
	return status;
}

void prx_DiscoveryAllPrimaryService(BtGattEvent *event)
{
    BtGattDiscoveryAllPrimaryServiceResultEvent *ptr;
          PrxMTransport *chnl;
    PrxPacket *packet = NULL;
	BtStatus status;
    U16 uuid = 0;
    
    ptr = (BtGattDiscoveryAllPrimaryServiceResultEvent *) event->parms;
    packet = ContainingRecord(event->pGattOp, PrxPacket, op);
    chnl = packet->chnl;
    Report(("[PRX:][GATT]BT_GATTAppHandleDiscoveryAllPrimaryService"));
    Report(("[PRX:][GATT]ptr->flag:%d", ptr->flag));
    if(chnl == 0)
    {
        Assert(0);
    }
    Report(("[PRX:]Channel address:%08x", chnl));
    switch (packet->cmd)
    {
        case GATT_PRX_WRITE_TXPOWER:
        case GATT_PRX_TXPOWER:
            uuid = BT_UUID_PROXIMITY_TXPOWER_LEVEL;
            break;
        case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
        case GATT_PRX_IMMEDIATE_SERVICE:
            uuid = BT_UUID_PROXIMITY_ALERT_LEVEL;
            break;
        case GATT_PRX_WRITE_LINKLOSS_SERVICE:
        case GATT_PRX_LINKLOSS_SERVICE:
            uuid = BT_UUID_PROXIMITY_ALERT_LEVEL;
            break;
    }
    switch(ptr->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            Report(("[PRX:][GATT]startHandle:%d", ptr->startHandle));
            Report(("[PRX:][GATT]endHandle:%d", ptr->endHandle));
            Report(("[PRX:][GATT]type:%d", ptr->type));       
            Report(("[PRX:][GATT]uuid2:%d", ptr->uuid2));    
            packet->startHandle = ptr->startHandle;
            packet->endHandle = ptr->endHandle;
            packet->operationSuccessFlag = 1;

            break;
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            Report(("[GATT]BT_GATTAppHandleDiscoveryAllPrimaryService BT_GATT_EVENT_RESULT_DONE_COMPLETED success:%d", packet->operationSuccessFlag));
            if(packet->operationSuccessFlag == 1)
            {
                status = prx_GattDiscoveryCharServicesByUUID(packet, uuid, packet->startHandle, packet->endHandle);
				OS_Report("[PRX:] status:%d", status);
				if( status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS ){
					prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);					
				}
            }else{
	            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            }
            break;
        case BT_GATT_EVENT_RESULT_ERROR:
            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            break;
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            break;            
          
    }
}

void prx_DiscoveryCharByUUID(BtGattEvent *event)
{
    BtGattDiscoveryCharacteristicByUUIDResultEvent *ptr;
	PrxMTransport *chnl;
    PrxPacket *packet;
    U16 uuid;
	BtStatus status = BT_STATUS_FAILED;
	PrxMContext * pCntx;
	
    
    ptr = (BtGattDiscoveryCharacteristicByUUIDResultEvent *) event->parms;
    packet = ContainingRecord(event->pGattOp, PrxPacket, op);
    chnl = packet->chnl;
    
    Report(("[PRX:][GATT]BT_GATTAppHandleDiscoveryCharByUUID"));
    Report(("[PRX:][GATT]ptr->flag:%d", ptr->flag));
    Report(("Channel address:%08x", chnl));
    if(chnl == 0)
    {
        Assert(0);
    }

    switch(ptr->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            Report(("[PRX:][GATT]attributeHandle:%d", ptr->attributeHandle)); //0x0008 characteristic's handle
            Report(("[PRX:][GATT]charProperty:%d", ptr->charProperty)); //0x0a
            Report(("[PRX:][GATT]charValueAttributeHandle:%d", ptr->charValueAttributeHandle )); //0x0009 value's handle
            Report(("[PRX:][GATT]type:%d", ptr->type));
            Report(("[PRX:][GATT]uuid2:%d", ptr->charUUID));    
            packet->startHandle = ptr->charValueAttributeHandle;
            packet->operationSuccessFlag = 1;

            break;
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            Report(("[GATT]BT_GATTAppHandleDiscoveryCharByUUID BT_GATT_EVENT_RESULT_DONE_COMPLETED %d", packet->operationSuccessFlag));
            if(packet->operationSuccessFlag == 1)
            {
                switch (packet->cmd)
                {
                    case GATT_PRX_TXPOWER:
                    case GATT_PRX_IMMEDIATE_SERVICE:
                    case GATT_PRX_LINKLOSS_SERVICE:     
                        status = prx_GattReadCharValue(packet, packet->startHandle);
					    Report(("[GATT] prx_GattReadCharValue status:%d", status));
						if( BT_STATUS_PENDING != status ){
							prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
						}
                        break;
                    case GATT_PRX_WRITE_TXPOWER:
                    case GATT_PRX_WRITE_LINKLOSS_SERVICE:                    
                        status = prx_GattWriteCharValue(packet, packet->startHandle);
					    Report(("[GATT] prx_GattWriteCharValue status:%d", status));						
						if( BT_STATUS_PENDING != status ){
							prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
						}
                        break;
				    case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
                        //status = prx_GattWriteCharValue(packet, packet->startHandle);						
				   	    status = prx_GattWriteWithoutResponse(packet, packet->startHandle, packet->buff[0]);
					    Report(("[GATT] BT_GATT_WriteWithoutResponse status:%d", status));
						pCntx = (PrxMContext *) ContainingRecord( packet->chnl, PrxMContext, chnl);						
						if( status == BT_STATUS_PENDING ){
							// wait the callback
						}else if( status == BT_STATUS_SUCCESS ){
							// ok
							prxm_notifyWriteRemoteIMAlert( pCntx, BT_STATUS_SUCCESS);
						}else{
							// fail
							prxm_notifyWriteRemoteIMAlert( pCntx, BT_STATUS_FAILED);
						}
						break;
					default:
						prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
						break;
                }
            }else{
            	// Complete with fail result
            	prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            }
            break;
        case BT_GATT_EVENT_RESULT_ERROR:
            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            break;
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            break;            
          
    }
}


void prx_ReadCharacteristic(BtGattEvent *event)
{
    BtGattReadCharacteristicValueResultEvent *ptr;
	PrxMTransport *chnl;
    PrxPacket *packet;
    U16 uuid;
    
    ptr = (BtGattReadCharacteristicValueResultEvent *) event->parms;
    packet = ContainingRecord(event->pGattOp, PrxPacket, op);
    chnl = packet->chnl;
    
    if(chnl == 0)
    {
        Assert(0);
    }

    Report(("[PRX:][GATT]BT_GATTAppHandleReadCharValue"));
    Report(("[PRX:][GATT]ptr->flag:%d", ptr->flag));
    Report(("Channel address:%08x", chnl));
    switch (packet->cmd)
    {
            case GATT_PRX_TXPOWER:
                uuid = BT_UUID_GATT_SERVICE_TXPOWER;
                break;
            case GATT_PRX_IMMEDIATE_SERVICE:
                uuid = BT_UUID_GATT_SERVICE_ALERT;
                break;
            case GATT_PRX_LINKLOSS_SERVICE:
                uuid = BT_UUID_GATT_SERVICE_LINKLOSS;
                break;
    }
    switch(ptr->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            Report(("[PRX:][GATT]length:%d", ptr->length));
            Report(("[PRX:][GATT]BT_GATTAppHandleReadCharValue BT_GATT_EVENT_RESULT_DONE_COMPLETED"));
            if( ptr->length == 1 ){
                Report(("[PRX:][GATT] data[0]:%d", ptr->data[0]));
            }else if( ptr->length > 1 ){
                Report(("[PRX:][GATT] data[0]:%d data[1]:%d", ptr->data[0], ptr->data[1]));
            }
            prxClientQueryResult(packet, ptr->data, ptr->length, BT_STATUS_SUCCESS);
            break;
        case BT_GATT_EVENT_RESULT_ERROR:
            Report(("[PRX:][GATT]BT_GATT_EVENT_RESULT_ERROR"));
            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);			
            break;
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            Report(("[PRX:][GATT]BT_GATT_EVENT_RESULT_DISCONNECT"));
            prxClientQueryResult(packet, 0, 0, BT_STATUS_FAILED);
            break;            
          
    }
}

void prx_WriteCharacteristic(BtGattEvent *event)
{
    BtGattWriteCharValueResultEvent *ptr;
	PrxMTransport *chnl;
    PrxPacket *packet;
    U16 uuid;
    
    ptr = (BtGattWriteCharValueResultEvent *) event->parms;

    packet = ContainingRecord(event->pGattOp, PrxPacket, op);
    chnl = packet->chnl;
    
    if(chnl == 0)
    {
        Assert(0);
    }

    Report(("[GATT]BT_GATTAppHandleWriteCharValue"));
    Report(("[GATT]ptr->flag:%d", ptr->flag));
    Report(("Channel address:%08x", chnl));
    switch (packet->cmd)
    {
            case GATT_PRX_WRITE_TXPOWER:
                uuid = BT_UUID_GATT_SERVICE_TXPOWER;
                break;
            case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
                uuid = BT_UUID_GATT_SERVICE_ALERT;
                break;
            case GATT_PRX_WRITE_LINKLOSS_SERVICE:
                uuid = BT_UUID_GATT_SERVICE_LINKLOSS;
                break;
    }
    switch(ptr->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            Report(("[PRX:][GATT]flag:%d", ptr->flag));
            Report(("[PRX:][GATT]BT_GATTAppHandleWriteCharValue BT_GATT_EVENT_RESULT_DONE_COMPLETED"));
            break;
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            Report(("[PRX:][GATT]BT_GATTAppHandleWriteCharValue BT_GATT_EVENT_RESULT_DONE_COMPLETED"));
            prxClientQueryResult(packet, NULL, 0, BT_STATUS_SUCCESS);
            break;
        case BT_GATT_EVENT_RESULT_ERROR:
	    	prxClientQueryResult(packet, NULL, 0, BT_STATUS_FAILED);
            break;
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            prxClientQueryResult(packet, NULL, 0, BT_STATUS_FAILED);
            break; 
          
    }
}

void prx_WriteCharacteristicWithoutResponse(BtGattEvent *event)
{
    BtGattWriteWithoutResponseResultEvent *ptr;
	PrxMTransport *chnl;
    PrxPacket *packet;
    U16 uuid;
    
    ptr = (BtGattWriteWithoutResponseResultEvent *) event->parms;

    packet = ContainingRecord(event->pGattOp, PrxPacket, op);
    chnl = packet->chnl;
    
    if(chnl == 0)
    {
        Assert(0);
    }

    Report(("[GATT]prx_WriteCharacteristicWithoutResponse"));
    Report(("[GATT]ptr->flag:%d", ptr->flag));
    Report(("Channel address:%08x", chnl));
    switch (packet->cmd)
    {
            case GATT_PRX_WRITE_TXPOWER:
                uuid = BT_UUID_GATT_SERVICE_TXPOWER;
                break;
            case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
                uuid = BT_UUID_GATT_SERVICE_ALERT;
                break;
            case GATT_PRX_WRITE_LINKLOSS_SERVICE:
                uuid = BT_UUID_GATT_SERVICE_LINKLOSS;
                break;
    }
    switch(ptr->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            Report(("[PRX:][GATT]flag:%d", ptr->flag));
            Report(("[PRX:][GATT]prx_WriteCharacteristicWithoutResponse BT_GATT_EVENT_RESULT_DONE_COMPLETED"));
            break;
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            Report(("[PRX:][GATT]prx_WriteCharacteristicWithoutResponse BT_GATT_EVENT_RESULT_DONE_COMPLETED"));
            prxClientQueryResult(packet, NULL, 0, BT_STATUS_SUCCESS);
            break;
        case BT_GATT_EVENT_RESULT_ERROR:
	    	prxClientQueryResult(packet, NULL, 0, BT_STATUS_FAILED);
            break;
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            prxClientQueryResult(packet, NULL, 0, BT_STATUS_FAILED);
            break; 
    }
}



void prxGattAPCallBack(void *userdata, BtGattEvent *event)
{
    Report(("[PRX:][GATT]prxGattAPCallBack with event:%d", event->eType));
    PrxPacket *packet = 0;
    U16 uuid = 0;
    
    event->pGattOp = userdata;
    if( userdata != 0 ){
        packet = ContainingRecord(event->pGattOp, PrxPacket, op);
    }
    Report(("[PRX:][GATT][t] prxGattAPCallBack packet:0x%x", packet));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
            prx_DiscoveryAllPrimaryService(event);
            break;
        case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
            prx_DiscoveryCharByUUID(event);
            break;
        case BT_GATT_OPERATOR_READ_CHAR_VALUE:
            prx_ReadCharacteristic(event);

            Report(("[PRX:][GATT] prxGattAPCallBack handler:0x%x, 0x%x", packet->startHandle, packet->endHandle));

            switch (packet->cmd)
            {
                case GATT_PRX_WRITE_TXPOWER:
                case GATT_PRX_TXPOWER:
                    uuid = BT_UUID_PROXIMITY_TXPOWER_LEVEL;
                    break;
                case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
                case GATT_PRX_IMMEDIATE_SERVICE:
                    uuid = BT_UUID_PROXIMITY_ALERT_LEVEL;
                    break;
                case GATT_PRX_WRITE_LINKLOSS_SERVICE:
                case GATT_PRX_LINKLOSS_SERVICE:
                    uuid = BT_UUID_PROXIMITY_ALERT_LEVEL;
                    break;
            }
            prx_GattDiscoveryCharDescription(packet, uuid, packet->startHandle - 2, packet->endHandle);
            break;
        case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
            prx_WriteCharacteristic(event);
            break;
		case BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE:
			prx_WriteCharacteristicWithoutResponse(event);
			break;
        case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR:
            Report(("[PRX:][GATT][Yi] prxGattAPCallBack discover all char"));
            break;
		default:
            if( 0 != userdata ){
              	prx_return_packet(packet);
            }
			Report(("[PRX:][GATT][WRN] unknow eType:%d userdata:0x%x", event->eType, userdata));
			break;
    }
}


BtStatus prx_GattSendDiscoveryPrimaryServiceByUUID(
                                PrxPacket *packet,
                                U16 uuid, 
                                U16 start, 
                                U16 end)
{
	PrxMTransport *chnl;
    BtGattOp *gattOp;
    
    chnl = packet->chnl;
    gattOp = &packet->op;
    memset(gattOp, 0, sizeof(BtGattOp));
	InitializeListEntry(&gattOp->op.node);
    packet->operationSuccessFlag = 0;	
	gattOp->apCb = prxGattAPCallBack;
    packet->parameter.endHandle = end;
    packet->parameter.startHandle = start;
    packet->parameter.type = 0x02;
    packet->parameter.uuid16[0] = uuid & 0xff; /* low  order*/
    packet->parameter.uuid16[1] = uuid >> 8; /* high order*/ /*alert  serivice uuid */
	gattOp->parms.p.primaryServiceByUUID = &packet->parameter;
	return BT_GATT_DiscoveryPrimaryServiceByUUID(chnl->handler.bdc->link, gattOp);
}


BtStatus prx_GattDiscoveryCharServicesByUUID(PrxPacket *packet, U16 uuid, U16 start, U16 end)
{
	PrxMTransport *chnl;
    BtGattOp *gattOp;
    
    chnl = packet->chnl;
    gattOp = &packet->op;
    memset(gattOp, 0, sizeof(BtGattOp));
	InitializeListEntry(&gattOp->op.node);
    packet->operationSuccessFlag = 0;	
	gattOp->apCb = prxGattAPCallBack;

    packet->parameter1.endHandle = end;
	packet->parameter1.startHandle = start;
	packet->parameter1.type = 0x02;
	packet->parameter1.uuid16[0] = uuid & 0xff; /* low order*/
	packet->parameter1.uuid16[1] = uuid >> 8 ; /* high order*/ /* alert level characteristic UUID */
	gattOp->parms.p.charByUUID = &packet->parameter1;

	return BT_GATT_DiscoveryCharacteristicByUUID(chnl->handler.bdc->link, gattOp);
}

BtStatus prx_GattDiscoveryCharDescription(PrxPacket *packet, U16 uuid, U16 start, U16 end)
{
    PrxMTransport *chnl;
    BtGattOp *gattOp;
    
    chnl = packet->chnl;
    gattOp = &packet->op;
    memset(gattOp, 0, sizeof(BtGattOp));
    InitializeListEntry(&gattOp->op.node);
    packet->operationSuccessFlag = 0;	
    gattOp->apCb = prxGattAPCallBack;
	
    packet->parameter1.endHandle = end;
    packet->parameter1.startHandle = start;
    packet->parameter1.type = 0x02;
    packet->parameter1.uuid16[0] = uuid & 0xff; /* low order*/
    packet->parameter1.uuid16[1] = uuid >> 8 ; /* high order*/ /* alert level characteristic UUID */
    gattOp->parms.p.charByUUID = &packet->parameter1;
	
    return BT_GATT_DiscoveryAllCharacteristicDescriptors(chnl->handler.bdc->link, gattOp);
}

BtStatus prx_GattReadCharValue(PrxPacket *packet, U16 char_handle)
{
	PrxMTransport *chnl;
    BtGattOp *gattOp;
    
    chnl = packet->chnl;
    gattOp = &packet->op;
    memset(gattOp, 0, sizeof(BtGattOp));
	InitializeListEntry(&gattOp->op.node);
    packet->operationSuccessFlag = 0;	
	gattOp->apCb = prxGattAPCallBack;
	packet->parameter2.handle = char_handle; /* from found result of ??? */
	gattOp->parms.p.readCharValue = &packet->parameter2;

	return BT_GATT_ReadCharacteristicValue(chnl->handler.bdc->link, gattOp);
}


BtStatus prx_GattWriteCharValue(PrxPacket *packet, U16 char_handle)
{
	PrxMTransport *chnl;
    BtGattOp *gattOp;
    
    chnl = packet->chnl;
    gattOp = &packet->op;
    memset(gattOp, 0, sizeof(BtGattOp));
    packet->operationSuccessFlag = 0;
	InitializeListEntry(&gattOp->op.node);
	
	gattOp->apCb = prxGattAPCallBack;
	packet->parameter3.handle = char_handle;

	packet->parameter3.data = packet->buff;
	packet->parameter3.size = packet->size;	
	gattOp->parms.p.writeRequest = &packet->parameter3;

	return BT_GATT_WriteCharacteristicValue(chnl->handler.bdc->link, gattOp);
}


BtStatus prx_GattWriteWithoutResponse(PrxPacket *packet, U16 char_handle, U8 value)
{
	U8 i=0;
	PrxMTransport *chnl = NULL;
    BtGattOp *gattOp = NULL;
	BtStatus status = BT_STATUS_FAILED;

    chnl = packet->chnl;
    gattOp = &packet->op;
    memset(gattOp, 0, sizeof(BtGattOp));
	InitializeListEntry(&gattOp->op.node);
    packet->operationSuccessFlag = 0;	
	gattOp->apCb = prxGattAPCallBack;


	packet->parameter18.handle = char_handle;

	packet->parameter18.data = packet->buff;
	packet->parameter18.size = 1;
	gattOp->parms.p.writeWithoutResponse = &packet->parameter18;

	status = BT_GATT_WriteWithoutResponse(chnl->handler.bdc->link, gattOp);
	Report(("[PRX:] status %d", status)); 

	Report(("[PRX:][T] BtGattOp %d %d %d", sizeof(gattOp), sizeof(*gattOp), sizeof(BtGattOp)));	
	return status;
}

/**
 * @param handler
 */
void ProximityCommonGattCallback(void *handler, BtGattEvent *event)
{
	PrxMTransport *pChnl;
	PrxMContext *pCntx;
	
	PrxRTransport *pRChnl;
	PrxRContext *pRCntx;	
    BtGattHandler *gatthandler = handler;	
    U8 i = 0;

	OS_Report("[PRX:] ProximityCommonGattCallback eType:%d", event->eType);
	switch (event->eType)
	{
		case BT_GATT_APP_EVENT_CONNECTED:
			Report(("[PRX:][GATT]BT_GATT_APP_EVENT_CONNECTED"));
			Report(("[PRX:] BT_GATT_APP_EVENT_CONNECTED"));
#if defined(__BT_PRXM_PROFILE__)
			// what is the address of the connected
			g_prx_monitor = get_monitor_by_remdev(event->link);
			if( NULL == event->link ){
			    //
			    Report(("[PRX:][WRN] no address ignore it!"));
			}else if( g_prx_monitor == NULL ){
			    Report(("[PRX:][WRN] no monitor ignore it!"));			
			}else{
	    	    pCntx = (PrxMContext *) ContainingRecord( g_prx_monitor, PrxMContext, chnl);
    			prxm_notifyConnectResult(pCntx, TRUE, 0);
			}
#endif			
			break;
		case BT_GATT_APP_EVENT_DISCONNECTED:
			Report(("[PRX:][GATT]BT_GATT_APP_EVENT_DISCONNECTED"));
#if defined(__BT_PRXM_PROFILE__)			
			g_prx_monitor = get_monitor_by_remdev(event->link);
			if( NULL == event->link ){
			    //
			    Report(("[PRX:][WRN] no address ignore it!"));
			}else if( g_prx_monitor == NULL ){
			    Report(("[PRX:][WRN] no monitor ignore it!"));			
			}else{
	    	    pCntx = (PrxMContext *) ContainingRecord( g_prx_monitor, PrxMContext, chnl);
				pChnl = &pCntx->chnl;

    			prxm_notifyDisconnectResult(pCntx, TRUE, 0);	

				// cannot use monitor anymore
				static_monitor_removeCmgr(pChnl, TRUE);
				//g_prx_monitor = NULL;
				release_monitor(g_prx_monitor);
			}
#endif			
#if defined(__BT_PRXR_PROFILE__)
			// find the reporter to handler
			pRChnl = get_reporter_by_remdev(event->link);
			Report(("[PRX:] find reporter 0x%x", pRChnl));
			if( pRChnl != NULL ){
				pRCntx = (PrxRContext *) ContainingRecord( pRChnl, PrxRContext, chnl);
				prxr_notifyDisconnected(pRCntx);				

				if( (PRX_CMGR_CREATE_REG & pRChnl->regFlag ) ){
					CMGR_RemoveDataLink(&pRChnl->handler);
					pRChnl->regFlag = pRChnl->regFlag & ~PRX_CMGR_CREATE_REG;
				}else{
					OS_Report("[PRX:] ignore removeDatalink regFlag:0x%x", pRChnl->regFlag);
				}

				// release the 
				release_reporter(pRChnl);

				prxServerReleaseProximityDataBase(event->link);
			}			
#endif
			break;
        case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
            Report(("[PRX:][GATT]BT_GATT_APP_EVENT_VALUE_NOTIFICATION"));
            break;
        case BT_GATT_APP_EVENT_VALUE_INDICATION:
            Report(("[PRX:][GATT]BT_GATT_APP_EVENT_VALUE_INDICATION"));
            break;            
        case BT_GATT_APP_EVENT_SERVER:
#ifdef __BT_PRXR_PROFILE__

            prxr_att_serverhandleDataIncoming(event->link);
#endif
            break;            
	}
}


static void prxClientQueryResult(PrxPacket *packet, U8 *data, U8 len, U8 result)
{
	S32 s32value = 0;
	BOOL okRet = FALSE;
	// return the packet
	PrxMContext * pCntx;
	
	if( packet == NULL ){
		OS_Report("[PRX:][ERR] prxClientQueryResult NULL packet");
		return;
	}

	if( packet->chnl == NULL ){
		OS_Report("[PRX:][ERR] prxClientQueryResult NULL Cntx packet:0x%x", packet);
		return;
	}
	pCntx = (PrxMContext *) ContainingRecord( packet->chnl, PrxMContext, chnl);
	
	prx_return_packet(packet);

    switch (packet->cmd)
    {
        case GATT_PRX_TXPOWER:
            if(result == BT_STATUS_SUCCESS)
            {
                Report(("[PRX:] Read TX Power is:%02x",data[0]));
				okRet = TRUE;
				s32value = data[0];
            }else{
				Report(("[PRX:] Read TX Power Fail result:%d", result));
			}
			prxm_notifyGetTxpowerResult(pCntx, TRUE, s32value);
            break;
        case GATT_PRX_IMMEDIATE_SERVICE:
            if(result == BT_STATUS_SUCCESS)
            {
                Report(("[PRX:] Read GATT_PRX_IMMEDIATE_SERVICE is:%02x",data[0]));
				okRet = TRUE;
				s32value = data[0];
            }else{
				Report(("[PRX:] Read Imalert Fail result:%d", result));
			}

            break;
        case GATT_PRX_LINKLOSS_SERVICE:
            if(result == BT_STATUS_SUCCESS)
            {
                Report(("[PRX:] Read GATT_PRX_LINKLOSS_SERVICE is:%02x",data[0]));
				okRet = TRUE;
				s32value = data[0];
				prxm_notifyGetLinkLossResult( pCntx, TRUE, (U8)s32value);
            }else{
				Report(("[PRX] Read linkloss Fail result:%d", result));
				prxm_notifyGetLinkLossResult( pCntx, FALSE, 0);
			}
            break;
        case GATT_PRX_WRITE_TXPOWER:
            if(result == BT_STATUS_SUCCESS)
            {
                Report(("[PRX:] write GATT_PRX_WRITE_TXPOWER done"));
				okRet = TRUE;
				s32value = data[0];
            }else{
				Report(("[PRX:] write txpower Fail result:%d", result));
			}
            break;
        case GATT_PRX_WRITE_IMMEDIATE_SERVICE:
            if(result == BT_STATUS_SUCCESS)
            {
                Report(("[PRX:] write GATT_PRX_WRITE_IMMEDIATE_SERVICE done"));
				okRet = TRUE;
            }else{
				Report(("[PRX:] write Im alert Fail result:%d", result));
			}
			prxm_notifyWriteRemoteIMAlert( pCntx, result);
	    
            break;
        case GATT_PRX_WRITE_LINKLOSS_SERVICE:
            if(result == BT_STATUS_SUCCESS)
            {
                Report(("[PRX:] write GATT_PRX_WRITE_LINKLOSS_SERVICE done"));
				okRet = TRUE;
            }else{
				Report(("[PRX:] Write LinkLoss Fail result:%d", result));
			}
			prxm_notifyWriteRemoteLinkAlert( pCntx, result);
            break;
    }
}

#endif

#if 1
U8 ProximityImmAlertLevel(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    U8 i=0;
    U8 status = BT_STATUS_FAILED;
#if defined(__BT_PRXR_PROFILE__)
	PrxRTransport *reporter;
	PrxRContext *pCntx;

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            /* Read */
            for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
            {
                if(link == ProximityReporterDatabase[i].link)
                {
                    *len = 0x01;
                    *raw = (U8 *)&ProximityReporterDatabase[i].immAlertLevel;
                    status = BT_STATUS_SUCCESS;
					OS_Report("[PRX:] read imalert %d", ProximityReporterDatabase[i].immAlertLevel);
                    break;
                }
            }
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            /* Write */
            for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
            {
                if(link == ProximityReporterDatabase[i].link)
                {
                    OS_Report("[PRX:] write imalert %d",  *raw[0]);
                    status = BT_STATUS_SUCCESS;
					if( 2 < (U8) *raw[0] ){
						OS_Report("[PRX:] reject it !");
						status = BT_STATUS_FAILED;
					}else{
						ProximityReporterDatabase[i].immAlertLevel = *raw[0];
						
						// find the reporter and notify it
						reporter = get_reporter_by_remdev(link);
						if( NULL != reporter ){
							pCntx = (PrxRContext *) ContainingRecord( reporter, PrxRContext, chnl);
							prxr_notifyPathLossValue(pCntx, TRUE, (U8)ProximityReporterDatabase[i].immAlertLevel);
						}
					}
                    break;
                }
            }
            break;
    }
#endif	
    return status;

}

U8 BtPrxLocalTxPowerDatabase(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{

    U8 i=0;
    U8 status = BT_STATUS_FAILED;
#if defined(__BT_PRXR_PROFILE__)

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            /* Read */
            for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
            {
                if(link == ProximityReporterDatabase[i].link)
                {
                    *len = 0x01;
                    *raw = (U8 *) &ProximityReporterDatabase[i].txPower;
                    status = BT_STATUS_SUCCESS;
					OS_Report("[PRX:] read txPower %d", ProximityReporterDatabase[i].txPower);
                    /* Read Local Tx Power done */
                    break;                    
                }
            }
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            /* Write */
            for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
            {
                if(link == ProximityReporterDatabase[i].link)
                {
					OS_Report("[PRX:] write txpower ! reject it !");
					status = BT_STATUS_FAILED;					
                    break;                    
                }
            }
            break;
    }
#endif

    return status;
}

U8 ProximityLinkLossAlert(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    U8 i=0;
    U8 status = BT_STATUS_FAILED;
#if defined(__BT_PRXR_PROFILE__)
	PrxRTransport *reporter;
	PrxRContext *pCntx;

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            /* Read */
            for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
            {
                if(link == ProximityReporterDatabase[i].link)
                {
                    *len = 0x01;
                    *raw = (U8 *)&ProximityReporterDatabase[i].alertLevel;
                    status = BT_STATUS_SUCCESS;
		            OS_Report("[PRX:] read txPower %d", ProximityReporterDatabase[i].alertLevel);
                    break;
                }
            }
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            /* Write */
            for(i=0;i<MAX_PROXIMITY_DATABASE; i++)
            {
                if(link == ProximityReporterDatabase[i].link)
                {
                    OS_Report("[PRX:] write linkloss %d",  *raw[0]);
                    status = BT_STATUS_SUCCESS;
					if( 2 < (U8) *raw[0] ){
						OS_Report("[PRX:] reject it !");
						status = BT_STATUS_FAILED;
					}else{
						ProximityReporterDatabase[i].alertLevel = *raw[0];
						
						// find the reporter and notify it
						reporter = get_reporter_by_remdev(link);
						if( NULL != reporter ){
							pCntx = (PrxRContext *) ContainingRecord( reporter, PrxRContext, chnl);
							prxr_notifyLinkLossValue(pCntx, TRUE, (U8) ProximityReporterDatabase[i].immAlertLevel);
						}
					}

                    break;
                }
            }
            break;
    }
#endif	
    return status;

}

#endif

