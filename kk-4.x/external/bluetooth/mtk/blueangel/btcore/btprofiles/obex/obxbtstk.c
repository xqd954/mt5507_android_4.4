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

/****************************************************************************
 *
 * File:          obxbtstk.c
 *
 * Description:   This file contains the code for OBEX Bluetooth stack
 *                interface module.
 * 
 * Created:       October 21, 1999
 *
 * Version:       MTObex 3.4
 *
 * Copyright 1999-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 09 22 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integrate bluetooth code from //ALPS_SW_PERSONAL/sh.lai/10YW1040OF_CB/ into //ALPS_SW/TRUNK/ALPS/.
 *
 * Mar 18 2009 mtk80029
 * [MAUI_01406911] [Bluetooth] Btadp folder restruct activity
 * 
 *
 * Dec 26 2008 mbj06038
 * [MAUI_01348800] [BT]When connect failed,the previous connection will disconnect
 * 
 *
 * Dec 22 2008 mbj06032
 * [MAUI_01482962] ASSERT fail: utils.c 289 - BT
 * Undo modification
 *
 * Dec 12 2008 mbj06038
 * [MAUI_01477282] [1] ASSERT fail: conmgr.c 2734 - BT
 * 
 *
 * Dec 5 2008 mbj06032
 * [MAUI_01345132] [BT]Different behavior when have connection request on pairing screen
 * If user reject the RFCOMM Connect indication, Remove the ACL Link, and clear related variable
 *
 * Nov 26 2008 mbj06038
 * [MAUI_01342148] [New feature][SyncML] Device sync support
 * 
 *
 * Nov 10 2008 mbj06032
 * [MAUI_01335116] [BT OBEX] When RF_OpenClientChannel return failed, close the ACL link and notify app
 * 
 *
 * Oct 27 2008 mbj06032
 * [MAUI_01216572] MTK:Bluetooth_[1] ASSERT fail: conmgr.c 2729 - BT
 * check CMGR_CreateDataLink return value in RFEVENT_OPEN_IND event
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106205] [BT GOEP] Send OPP Client SDP result (OPP server's supported format list) to MMI
 * 
 *
 * Jul 24 2008 mbj06032
 * [MAUI_00875438] [1] ASSERT Fail: (*eid)-> data!=NULL event_shed.c 748 - BT
 * Tx Timer func use ObexPacket pointer as keyword to search the ObSendingPacket list 
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * check pointer
 *
 * Jul 15 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * start Tx timer after call RF_SendData; when timeout, disconnect and return the tx buffer
 *
 * Jul 4 2008 mbj06032
 * [MAUI_01082197] [BT]Long time display Connecting and can't connect server.
 * return the TxBuffer if not use it; use a buffer pointer to record the TxBuffer, if the TxBuffer has not been return when RF channel closed, then return it
 *
 * Jun 17 2008 mbj06032
 * [MAUI_01077981] [BT]ASSERT Fail: observer.c 1085 -BT
 * 
 *
 * Jun 6 2008 mbj06032
 * [MAUI_01063800] [BT]Long time display "Connecting" and connect failed.
 * add trace
 *
 * May 14 2008 mbj06032
 * [MAUI_01037856] BT V21 apdaptive revise
 * for BT V2.1 simple pairing feature
 *
 * Apr 18 2008 mbj06032
 * [MAUI_00664265] [Phonebook][PBAP] pbap profile new feature check in
 * 
 *
 * Apr 16 2008 mbj06032
 * [MAUI_00639839] [BT OBEX] modify trace info
 * 
 *
 * Apr 10 2008 mbj06032
 * [MAUI_00653223] [BT OBEX]modify trace info
 * 
 *
 * Mar 21 2008 mbj06032
 * [MAUI_00639839] [BT OBEX] modify trace info
 * 
 *
 * Mar 21 2008 mbj06032
 * [MAUI_00639839] [BT OBEX] modify trace info
 * 
 *
 * Mar 21 2008 mbj06032
 * [MAUI_00096220] BIP gets disconnected in the mentioned scenario.
 * modify trace info
 *
 * Mar 19 2008 mbj06032
 * [MAUI_00282255] ASSERT Fail: cngrGabdker !=0 cibngr,c 740 - BT
 * clear target uuid if need
 *
 * Mar 17 2008 mbj06032
 * [MAUI_00282265] [1] ASSERT fail: bt_osapi.c 613 - BT
 * 
 *
 * Mar 14 2008 mbj06032
 * [MAUI_00282265] [1] ASSERT fail: bt_osapi.c 613 - BT
 * before call CMGR_DeregisterHandler, check if CMGR_RemoveDataLink need to be called
 *
 * Mar 11 2008 mbj06032
 * [MAUI_00634780] [1] ASSERT Fail: 0 == "Contact MTK BT owner,  follow BT debug SOP" bt_osapi.c 613 -
 * in server's RFCOMM_OPEN_IND event, check the server's state, if it is not idle, then reject the connection
 *
 * Mar 10 2008 mbj06032
 * [MAUI_00630752] MTK BT profile supported on MT6611 download file from PC by FTP speed abnormal
 * For FTP profile, close the sniffer mode
 *
 * Mar 9 2008 mbj06032
 * [MAUI_00093193] [1] ASSERT Fail: (status == BT_STATUS_PENDING)|| (obxbtstk.c 236 - BT
 * remove ASSERT in BTSTACK_ClientDisconnect func, and add trace info
 *
 * Feb 29 2008 mbj06032
 * [MAUI_00725495] BT-Always in "Sending" screen
 * In the CMEVENT_DATA_LINK_DIS event, notify app disconnected if necessary
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "btconfig.h"
#include "btalloc.h"
#include "rfcomm.h"
#include <obex.h>
#include <sys/obxbtstk.h>
#include "bluetooth_trc.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#if BT_STACK == XA_ENABLED
#include <sys/hci.h>
#include <sys/debug.h>

#include "sdap.h"
#include "me.h"
#include "conmgr.h"
#include "bluetooth_struct.h"
#include "goep.h"
#include "bt_feature.h"
#include "l2cap.h"
#include "L2cap_AMP.h"
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
#if !defined(SDP_PARSING_FUNCS) || SDP_PARSING_FUNCS == XA_DISABLED
#error The OBEX Bluetooth transport requires the SDP parsing functions. 
#endif
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
static U16 obex_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST, 
    AID_SUPPORTED_FORMATS_LIST
};

static U16 obex_bpp_attribute_status_id_list[] = 
{
    AID_ADDITIONAL_PROT_DESC_LISTS
};

#ifdef __BT_GOEP_V2__
static U16 obexv15_attribute_id_list[] = 
{
    AID_GOEPL2CAP_PSM,
    AID_PROTOCOL_DESC_LIST, 
    AID_SUPPORTED_FORMATS_LIST
};
#endif

extern ObInternalData              ObexData;

#endif

extern U8 ObexSdpDB_GetServChn(U32 service_type, U8 instance);
extern U8 *BTBMGetLocalStoredName(U8 *bdAddr);
extern L2capExtendedFlowSpecOption *L2Cap_GetInExtendedFlowSpec(U16 l2cap_id);
extern U8 A2MP_GetActivatedControllerId(BtRemoteDevice *link);
#if BT_GOEP_V2 == XA_ENABLED
extern A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link);
#endif
extern BtStatus BTA2MP_LogicalLinkCreate(U16 L2CapId, U8 createOrAccept, BtRemoteDevice *link, U8 controllerId);
extern BtStatus BTA2MP_StartPhysicalLink(BtRemoteDevice *link, U16 psm, U16 local_cid);
extern BtStatus GOEP_ServerRFCommOpenInd(ObexServerApp * appHndl, goep_bd_addr_struct * bd_addr, char * dev_name);
BtStatus GOEP_ClientSDPSupportedFormatsInd(ObexClientApp * clientApp, kal_uint8 supported_list);
#define GOEP_USE_HIGHSPEED 1
//#define _DLIGHT_TEST_ 1

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/
#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
static void     BtServCallBack(RfChannel *Channel, RfCallbackParms *);
static void ObexServerConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status);
static void BTSTACK_RFOpenTimeout(void *para);
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
void ObexClientConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status);
static void     BtClientCallBack(RfChannel *Channel, RfCallbackParms *);
static void BtClientSdpCallback(
                                                        SdpQueryToken *sqt, 
                                                        U8 result, 
                                                        U8 attribute_index,
                                                        U8 *attribute_value
                                                        );
static BtStatus BtDisconnectClientLink(ObBtClientTransport *btxp);
static BtStatus BtStartServiceQuery(ObBtClientTransport *btxp);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/* Functions used in ObTransFuncTable */
static ObStatus BTSTACK_DisconnectReq(ObexTransportContext con);
static U16      BTSTACK_MaxTxSize(ObexTransportContext con);
static ObStatus BTSTACK_SendTxBuffer(ObexTransportContext con, U8 *buff, U16 len);

static BOOL     BTSTACK_GetTpConnInfo(ObexTransportContext context, 
                                      ObexTpConnInfo *tpConnInfo);
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
static BOOL     BTSTACK_IsRxFlowAvailable(ObexTransportContext context);
static void     BTSTACK_PauseRxFlow(ObexTransportContext context);
static void     BTSTACK_ResumeRxFlow(ObexTransportContext context);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

static BtStatus BtStartServiceRFCommQuery(ObBtClientTransport *btxp);
#if BT_GOEP_V2 == XA_ENABLED
static BtStatus BtStartServiceL2Query(ObBtClientTransport *btxp);
void BtClientL2CallBack(L2capChannelId L2ChannelId, L2capCallbackParms *Parms);
void BtServL2CallBack(L2capChannelId L2ChannelId, L2capCallbackParms *Parms);
#endif

#if BT_GOEP_V2 == XA_ENABLED
U16 OBEXL2GetLocalPsmValue(U32 profile_uuid, U8 bClient){
	U16 psmValue = 0;
	if( bClient == FALSE ){
		// Server psm
    	switch( profile_uuid){
            case SC_OBEX_FILE_TRANSFER:
    			psmValue = BT_FTP_GOEPV2_PSM; // psm channel value
    			break;
    		case SC_OBEX_OBJECT_PUSH:
    			psmValue = BT_OPP_GOEPV2_PSM;
    			break;
            case SC_DIRECT_PRINTING:
            case SC_PRINTING_STATUS:
            case SC_IMAGING_RESPONDER:
        #ifdef __BT_SUPPORT_SYNCML__  
            case SC_SYNCML_BT_SERVER:
            case SC_SYNCML_BT_CLIENT:
        #endif
    			psmValue = 0x1111;
                break;
    		default:
                ASSERT( FALSE );
    			break;
    	}
	}else{
    	switch( profile_uuid){
            case SC_OBEX_FILE_TRANSFER:
    			psmValue = 0x1207; // psm channel value
    			break;
    		case SC_OBEX_OBJECT_PUSH:
    			psmValue = 0x1205;
    			break;
            case SC_DIRECT_PRINTING:
            case SC_PRINTING_STATUS:
            case SC_IMAGING_RESPONDER:
        #ifdef __BT_SUPPORT_SYNCML__  
            case SC_SYNCML_BT_SERVER:
            case SC_SYNCML_BT_CLIENT:
        #endif
    			psmValue = 0x1212;
                break;
    		default:
                ASSERT( FALSE );
    			break;
    	}	
	}
  	return psmValue;
}
#endif

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Bluetooth Transport function call table
 */
static const ObTransFuncTable BtStkFuncTable = {
    BTSTACK_DisconnectReq,BTSTACK_MaxTxSize, 
    BTSTACK_SendTxBuffer, BTSTACK_GetTpConnInfo,
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    BTSTACK_IsRxFlowAvailable, BTSTACK_PauseRxFlow, BTSTACK_ResumeRxFlow,
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
};


/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)

U8 ClientCheck_Hishspeed_Exist(ObBtClientTransport  *btxp){

#if BT_GOEP_V2 == XA_ENABLED
    // high speed check. not allow to use high speed when high speed is used!
    {
        A2MP_MAIN_CONN *a2mpConn = NULL;
        
        if( NULL != btxp->remoteDevice ){
            a2mpConn = A2MP_FindMainChannel(btxp->remoteDevice);
            if( NULL == a2mpConn ){
                OS_Report("highspeed check. No a2mpConn");
            }else{
                OS_Report("highspeed check. a2mpConn.state:%d localctrid:%d phyid:%d", a2mpConn->state, a2mpConn->localControllerId , a2mpConn->physicalLinkHandler);
                if( A2MP_STATE_ALLOCATED == a2mpConn->state && 0 != a2mpConn->physicalLinkHandler ){
                    OS_Report("highspeed check. highspeed it already existed! change it");
                    return 1;
                }
            }
        }
    }                
#endif
    return 0;
}

/*---------------------------------------------------------------------------
 *            BTSTACK_ClientConnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Start a RFCOMM connection to an OBEX device.
 *
 * Return:    OB_STATUS_PENDING - operation is started successfully
 *            OB_STATUS_SUCCESS - operation is successful
 *            OB_STATUS_BUSY - operation failed because the client is busy.
 */
ObStatus BTSTACK_ClientConnect(ObexBtTarget         *Target, 
                               ObBtClientTransport  *btxp,
                               ObexTransport          **Trans)
{
    ObStatus status = OB_STATUS_BUSY;
	BtStatus ret;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_CLIENTCONNECT, btxp->client.state);

#if XA_ERROR_CHECK == XA_ENABLED
    if (Target == 0 || !IsValidBdAddr(&Target->addr)) {
        return OB_STATUS_FAILED;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Target && IsValidBdAddr(&Target->addr));

    switch (btxp->client.state) {
        case OCS_CONNECTED:
            ASSERT(btxp->remoteDevice);
            /* RFCOMM connection is up, just verify that we're
             * connected to the requested device.
             */
            if (AreBdAddrsEqual(&Target->addr, &btxp->remoteDevice->bdAddr)) {
                btxp->target_uuid = Target->uuid;
                status = OB_STATUS_SUCCESS;
            } else {
                /* We're already connected to a DIFFERENT device */
                status = OB_STATUS_BUSY;
            }
            break;

        case OCS_IDLE:
            /* We have no Connection. Bring up the ACL connection and setup
             * the SDP query with the information provided.
             */
            ASSERT(btxp->remoteDevice == 0);

            btxp->target_uuid = Target->uuid;
#if BT_GOEP_V2 == XA_ENABLED            
            btxp->client.bGoepL2cap = Target->tptype;
#endif
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_CREATEDATALINK);
            status =  CMGR_CreateDataLink(&(btxp->client.cmgr_hdl), &Target->addr);            
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CMGR_CREATEDATALINK_RETURN, status);
			
            if (status == BT_STATUS_PENDING) {
                btxp->client.state = OCS_LINK_CONNECT;
                break;
            }
                
            if (status != BT_STATUS_SUCCESS)
                break;
            btxp->remoteDevice = btxp->client.cmgr_hdl.bdc->link;
            /* The desired ACL was already Connected, bring up RFCOMM */
            ASSERT(btxp->remoteDevice);
            ASSERT(AreBdAddrsEqual(&Target->addr, &btxp->remoteDevice->bdAddr));

            /* Query peer for Remote Object Push DLC */
#if BT_GOEP_V2 == XA_ENABLED
            
            if( GOEP_TP_RFCOMM_ONLY == btxp->client.bGoepL2cap ){
                /* only query the goepl2cappsm */
            status = BtStartServiceQuery(btxp);
            }else{
                // update the l2cap buffer
                if( 1 == ClientCheck_Hishspeed_Exist(btxp) ){
                    OS_Report("OBEX: remote hs already exist. change to use rfcomm");
                    status = BtStartServiceQuery(btxp);
                }else{
                status = BtStartServiceL2Query(btxp);
                }
                // TODO: status = BtStartServiceQuery(btxp);
            }
#else
            status = BtStartServiceQuery(btxp);
#endif
         
            if (status != BT_STATUS_PENDING) {

                if (status == BT_STATUS_INVALID_TYPE)
                    status = OB_STATUS_INVALID_PARM;

                /* Keeps BtDisconnectClientLink() from indicating disconnect */
                btxp->client.state = OCS_IDLE;
                kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
                ret = CMGR_RemoveDataLink(&(btxp->client.cmgr_hdl));    
                kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, ret);    
                btxp->client.cmgr_hdl.remDev = 0;
                btxp->client.cmgr_hdl.bdc = 0;                    
                break;
            }                    
            /* Service query started */
            btxp->client.state = OCS_SERVICE_QUERY;
            break;
    }

    *Trans = &btxp->client.transport;
    return status;

}

/*---------------------------------------------------------------------------
 *            BTSTACK_ClientDisconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the client's RFCOMM connection if it is up.
 *
 * Return:    OB_STATUS_PENDING - operation is started successfully
 *            OB_STATUS_FAILED - operation failed because Server is has a
 *            connection (only in no LMP Disconnect build). 
 *            OB_STATUS_NO_CONNECT - operation failed because there is not
 *            a client connection.
 *
 */
ObStatus BTSTACK_ClientDisconnect(ObBtClientTransport *btxp)
{
    ObexClientTransports *transport = ContainingRecord(btxp, ObexClientTransports, ObexClientBtTrans);
    ObexClientApp *ObexApp = (ObexClientApp*)ContainingRecord(transport, ObexClientApp, trans);
    ObStatus  status;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_CLIENTDISCONNECT, btxp->client.state);

    switch (btxp->client.state) {
    case OCS_CONNECTING:
        /* An RFCOMM channel may not exists */
	status = BTSTACK_DisconnectReq(&(btxp->client.channel));
        // btxp->client.state = OCS_DISCONNECTING; // if rf_closechannel success ,  it enters OCS_disconnecting
        if( status == BT_STATUS_SUCCESS ){
            BtDisconnectClientLink(btxp);
        }
		break;
		
    case OCS_CONNECTED:
#ifdef __BT_GOEP_V2__
        /* An RFCOMM channel exists */
        if( GOEP_TP_L2CAP_ONLY == btxp->client.bConnType || GOEP_TP_L2CAP_ONLY == btxp->client.bConnType){
            status = L2CAP_DisconnectReq( btxp->client.l2ChannelId );
            kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_DISCONNECT_REQ, btxp->client.l2ChannelId, status);
        }else{
            kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL, &btxp->client.channel, btxp);
            status = RF_CloseChannel(&btxp->client.channel);
            kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL_RETURN, status);
        }
#else
        kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL, &btxp->client.channel, btxp);
        status = RF_CloseChannel(&btxp->client.channel);
        kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL_RETURN, status);
#endif
		
		/*
        ASSERT((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS) ||
               ((status == BT_STATUS_FAILED) && (btxp->client.state) == OCS_CONNECTING));
        */       

        /* If the RF_CloseChannel() call failed. We must be in the process
         * of connecting. The RF_EVENT_OPEN code will detect that were
         * closing and force the disconnect then.
         */
        switch(status){
          case BT_STATUS_SUCCESS:
            OS_Report("OBEX: status is ok. invoke disconect client link");
            // pass success to indicate the service disconnected 
            status = BtDisconnectClientLink(btxp);
            status = BT_STATUS_SUCCESS; // restore the result.
            break;
          case BT_STATUS_PENDING:
          default:
        btxp->client.state = OCS_DISCONNECTING;
        status = OB_STATUS_PENDING;
        break;
        }
        break;

    case OCS_SERVICE_QUERY:
        /* Cannot cancel service query. Fall through to disconnect the ACL.
         * This will cause us to cleanup after the SDP response arrives.
         */
    case OCS_LINK_CONNECT:
        /* Set state to idle to avoid calling back the client.
         * Drop through to disconnect ACL.
         */
/*   For cancel connect
        btxp->client.state = OCS_IDLE;
*/
        btxp->client.state = OCS_DISCONNECTING;
        status = OB_STATUS_PENDING;
        break;

    case OCS_DISCONNECTING:
		/* just wait the disconnection finish */
        status = OB_STATUS_PENDING;
		break;

    case OCS_IDLE:
        /* If an ACL exists, disconnect it. */
        if (btxp->remoteDevice) {
            //Report(("[OBEX]BTSTACK_ClientDisconnect call BtDisconnectClientLink\n"));
            status = BtDisconnectClientLink(btxp);
            if (status != OB_STATUS_SUCCESS) {
                btxp->client.state = OCS_DISCONNECTING;
                status = OB_STATUS_PENDING;
            }
            break;
        }
        status = OB_STATUS_NO_CONNECT;
        break;

    default:        
        status = OB_STATUS_NO_CONNECT;
        break;
    }

    return status;
}

/****************************************************************************
 *
 * OBEX Transport - Used by OBEX parser and command interpreter.
 *
 ****************************************************************************/
#ifdef _DLIGHT_TEST_
EvmTimer dlight_timer1;
BtPacket dlight_tx_packet[30];
U8 dlight_tx_buffer[600];
ListEntry dlight_tx_list;
#endif

/*---------------------------------------------------------------------------
 *            BTSTACK_ClientInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *
 */
ObStatus BTSTACK_ClientInit(ObBtClientTransport *btxp)
{
    BtStatus        status;
    U8 i = 0;

    ObexClientTransports *transport = ContainingRecord(btxp, ObexClientTransports, ObexClientBtTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexClientApp, trans);
    ObexClientApp *ObexApp = (ObexClientApp*)appHndl;

    /* Initialize the Bluetooth client transport */
    InitBtClientTransport(btxp);

    ASSERT(btxp->client.state == OCS_UNINITIALIZED);
        
    btxp->client.app = appHndl;

    btxp->client.transport.funcTab = &BtStkFuncTable;
    btxp->client.transport.context = (ObexTransportContext)&btxp->client.channel;
    btxp->client.transport.connected = &btxp->client.state;
    btxp->client.transport.tpType = OBEX_TP_BLUETOOTH;

    /* Initialize ACL Link handle */
    btxp->remoteDevice = 0;
#ifdef _DLIGHT_TEST_
    InitializeListHead( &dlight_tx_list );
    OS_MemSet((U8 *)dlight_tx_buffer, 0xff, sizeof(dlight_tx_buffer));
    for (i = 0; i < 30 ; i++)
	{
		InitializeListEntry(&dlight_tx_packet[i].node);
		InsertTailList(&dlight_tx_list, &dlight_tx_packet[i].node);
	}        
#endif
	
    CMGR_RegisterHandler(&btxp->client.cmgr_hdl, ObexClientConnectionCallback);
    if(ObexApp->goep_service_type == GOEP_PROFILE_FTP)
    {
        /* modified for CR [MAUI_00630752] */
	    btxp->client.cmgr_hdl.sniffTimeout = 0xFFFFFFFF;
    }
    //Report(("[OBEX] Client CMGR_RegisterHandler: handler = 0X%x\n", &btxp->client.cmgr_hdl));

    /* Initialize the RFCOMM parameters */
    btxp->client.channel.callback = BtClientCallBack;
    btxp->client.channel.maxFrameSize = RF_MAX_FRAME_SIZE-1;
    btxp->client.channel.priority = RF_DEFAULT_PRIORITY;
#ifndef __BT_2_1_SIMPLE_PAIRING__    
    RF_ChannelInit(&(btxp->client.channel), BT_SEC_LOW);
#else
    if(ObexApp->goep_service_type == GOEP_PROFILE_OPUSH || ObexApp->goep_service_type == GOEP_PROFILE_BIP){
        RF_ChannelInit(&(btxp->client.channel), BT_SEC_LOW);
        OS_Report("obex: SEC_LOW");
    }else{
        RF_ChannelInit(&(btxp->client.channel), BT_SEC_MED);
    }
#endif
    /* We are able to deinitialize now */
    btxp->client.state = OCS_IDLE;
	btxp->target_uuid = 0;

    ME_ReadLocalBdAddr(&btxp->client.devAddr);
    
    appHndl->SendingRFPktNum = 0;
	appHndl->tpStatus = OB_TP_DISCONNECTED;
    
    status = BT_STATUS_SUCCESS;

    /* test */
    OS_Report("test: OBEX_RFCOMM_PACKET_TIMER_DUR:%d", OBEX_RFCOMM_PACKET_TIMER_DUR);

    return status;

}


ObStatus BTSTACK_ClientFlushChannel(ObBtClientTransport *btxp){
    ObStatus status = BT_STATUS_SUCCESS;
    U8 index = 0;
    U8 result = 1; //flush number
    L2capChannelId channel_id =0;
    
#ifdef __BT_3_0_HS__  
    if( btxp->client.bConnType == GOEP_TP_L2CAP_ONLY ){
        channel_id = btxp->client.l2ChannelId;
        while( 0 != result && index < 100 ){
            result = L2Cap_FlushOnePktTransmits(channel_id);
            index++;
        }    
        OS_Report("obex: l2ChannelId:%d flushnum:%d", channel_id, index);
    }else{
        OS_Report("obex: flush fail! connType:%d", btxp->client.bConnType);
    }
#endif    
    return status;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            BTSTACK_ClientDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Client Bluetooth transport.
 *
 * Return:    TRUE -  Client stack deinitialized.
 *            FALSE - Client cannot deinitialize.
 *
 */
BOOL BTSTACK_ClientDeinit(ObBtClientTransport *btxp)
{
    BtStatus    status;
    ObexClientTransports *transport = ContainingRecord(btxp, ObexClientTransports, ObexClientBtTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexClientApp, trans);

    if (btxp->client.state == OCS_UNINITIALIZED) {
        /* Already deinitialized */
        return TRUE;
    }
/*  For GOEP deinit
    if (btxp->client.state != OCS_IDLE) {
        return FALSE;
    }
*/
    Assert(btxp->client.app == appHndl);
/*  For GOEP deinit
    ASSERT(btxp->remoteDevice == 0);
*/
    if (btxp->remoteDevice)
    {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
	    status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
	    kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
	    if (status == BT_STATUS_SUCCESS) 
        {
	        btxp->remoteDevice = 0;
	        if (btxp->client.state != OCS_IDLE) 
            {
	            btxp->client.state = OCS_IDLE;
	            OBPROT_Disconnected(btxp->client.app, ODR_USER_REQUEST);
	        }
	    }
	    btxp->client.cmgr_hdl.remDev = 0;
	    btxp->client.cmgr_hdl.bdc = 0;       
    }

    status = CMGR_DeregisterHandler(&btxp->client.cmgr_hdl);
    if (status != BT_STATUS_SUCCESS)
		return FALSE;

    btxp->client.state = OCS_UNINITIALIZED;
    btxp->client.app = 0;
	btxp->target_uuid = 0;

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_RegisterSdpResultsCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers an SDP Query results callback for the OBEX Client.
 *            This callback allows layers above OBEX to parse the received 
 *            SDP attributes for additional profile specific data.  Such
 *            parsing should be in addition to the SDP parsing of the Protocol 
 *            Descriptor List attribute that OBEX performs automatically.
 *
 * Return:    ObStatus
 *
 */
ObStatus OBEX_RegisterSdpResultsCallback(ObBtClientTransport *Btxp, 
                                         ObSdpResultsCb Callback)
{
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Btxp || !Callback) {
        return OB_STATUS_INVALID_PARM;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    OS_LockObex();
    /* All SDP query results will be passed to this callback 
     * for further SDP parsing.
     */
    Btxp->sdpCallback = Callback;
    OS_UnlockObex();

    return OB_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            OBEX_DeregisterSdpResultsCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters an SDP Query results callback for the OBEX Client.
 *
 * Return:    ObStatus
 *
 */
ObStatus OBEX_DeregisterSdpResultsCallback(ObBtClientTransport *Btxp)
{
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Btxp) {
        return OB_STATUS_INVALID_PARM;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    OS_LockObex();
    /* All SDP query results will be passed to this callback 
     * for further SDP parsing.
     */
    Btxp->sdpCallback = 0;
    OS_UnlockObex();

    return OB_STATUS_SUCCESS;
}
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#ifdef _DLIGHT_TEST_
U32 OBEX_TX_COunt = 0;
#endif 

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            BTSTACK_ServerInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Server.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *
 */
ObStatus BTSTACK_ServerInit(ObBtServerTransport *btxp)
{
    BtStatus        status;
    U8              instance = 0;

    ObexServerTransports *transport = ContainingRecord(btxp, ObexServerTransports, ObexServerBtTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexServerApp, trans);
    ObexServerApp *ObexApp = (ObexServerApp*)appHndl;
    U32 obex_service_type = SC_SERVICE_DISCOVERY_SERVER;
        
    /* Initialize the Bluetooth server transport */
    InitBtServerTransport(btxp);

    ASSERT(btxp->server.state == OCS_UNINITIALIZED);

    btxp->server.app = appHndl;
    CMGR_RegisterHandler(&btxp->server.cmgr_hdl, ObexServerConnectionCallback);
    if(ObexApp->goep_service_type == GOEP_PROFILE_FTP)
    {
        /* modified for CR [MAUI_00630752] */
	    btxp->server.cmgr_hdl.sniffTimeout = 0xFFFFFFFF;
    }
    //Report(("[OBEX] Server CMGR_RegisterHandler: handler = 0X%x\n", &btxp->server.cmgr_hdl));
    
    /* Bind the server to the RFCOMM protocol stack */
    btxp->server.channel.callback = BtServCallBack;
    btxp->server.channel.maxFrameSize = RF_MAX_FRAME_SIZE;
    btxp->server.channel.priority = RF_DEFAULT_PRIORITY;
#ifndef __BT_2_1_SIMPLE_PAIRING__
    RF_ChannelInit(&(btxp->server.channel), BT_SEC_LOW);
#else
    if(ObexApp->goep_service_type == GOEP_PROFILE_OPUSH || ObexApp->goep_service_type == GOEP_PROFILE_BIP){
        RF_ChannelInit(&(btxp->server.channel), BT_SEC_LOW);
        OS_Report("obex: SEC_LOW");
    }else{
        RF_ChannelInit(&(btxp->server.channel), BT_SEC_MED);
    }
#endif

    /* Indicate a new service is being entered into RFCOMM.  OBEX 
     * doesn't know whether this is intended, or whether multiple
     * instances of the same service are needed.  Therefore, by default,
     * we present each new Obex server as registering a new RFCOMM service */
     switch(ObexApp->goep_service_type)
     {
         case GOEP_PROFILE_OPUSH:
            obex_service_type = SC_OBEX_OBJECT_PUSH;
            break;

         case GOEP_PROFILE_FTP:
            obex_service_type = SC_OBEX_FILE_TRANSFER;
            break;

         case GOEP_PROFILE_BIP:
            obex_service_type = SC_IMAGING_RESPONDER;
            break;

         case GOEP_PROFILE_PBAP:
            obex_service_type = SC_PBAP_SERVER;
            break;

#ifdef __BT_SUPPORT_SYNCML__
        case GOEP_PROFILE_SYNCML_CLIENT:
            obex_service_type = SC_SYNCML_BT_CLIENT;
            break;
        case GOEP_PROFILE_SYNCML_SERVER:
            obex_service_type = SC_SYNCML_BT_SERVER;
            break;
#endif /* __BT_SUPPORT_SYNCML__ */

         case GOEP_PROFILE_MAP_CLIENT:
            obex_service_type = SC_MAP_MNS_SERVER;
            break;

         case GOEP_PROFILE_MAP_SERVER1:
            instance = 0;
            obex_service_type = SC_MAP_SERVER;
            break;
         case GOEP_PROFILE_MAP_SERVER2:
            instance = 1;
            obex_service_type = SC_MAP_SERVER;
            break;
         case GOEP_PROFILE_MAP_SERVER3:
            instance = 2;
            obex_service_type = SC_MAP_SERVER;
            break;
         case GOEP_PROFILE_MAP_SERVER4:
            instance = 3;
            obex_service_type = SC_MAP_SERVER;
            break;

         default:
        ASSERT(0);
     }

    btxp->serverReg.serviceId = ObexSdpDB_GetServChn(obex_service_type, instance);
    kal_trace(BT_TRACE_G6_OBEX, OBEX_REG_RFCOMM_CHANNEL, btxp->serverReg.serviceId);
    if (RF_RegisterServerChannel(&btxp->server.channel, &btxp->serverReg, OBEX_PERFORMANCE_MULTIPLIER) != BT_STATUS_SUCCESS) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_RFCOMM_REGISTER_SERVER_CHANNEL_FAILED);
        return OB_STATUS_FAILED;
    }

    btxp->server.transport.funcTab = &BtStkFuncTable;
    btxp->server.transport.context = (ObexTransportContext)&btxp->server.channel;
    btxp->server.transport.connected = &btxp->server.state;
    btxp->server.transport.tpType = OBEX_TP_BLUETOOTH;
    
    /* We are able to deinitialize now */
    btxp->server.state = OCS_IDLE;

    ME_ReadLocalBdAddr(&btxp->server.devAddr);
    
    appHndl->SendingRFPktNum = 0;
#if BT_GOEP_V2 == XA_ENABLED
#else
	appHndl->tpStatus = OB_TP_DISCONNECTED;
#endif

#if BT_GOEP_V2 == XA_ENABLED    
    // TODO: check by only-l2cap only-rfcom

    /* Initialize the L2cap parameters */
    /* if status is success */
	btxp->server.bGoepL2cap = ObexApp->tptype;
    btxp->server.pl2Buffer = &ObexApp->l2buffer;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERV_INIT_L2CAP_BUF, btxp->server.bGoepL2cap, btxp->server.pl2Buffer);
	if( btxp->server.bGoepL2cap != 0 ){
        btxp->server.goepl2cap_psm.psm = 0; //default value. Before ConnectReq
        btxp->server.goepl2cap_psm.callback = BtServL2CallBack;
        btxp->server.goepl2cap_psm.localMtu = (L2CAP_MAXIMUM_MTU-3); //675; //L2CAP_MAXIMUM_MTU;
        btxp->server.goepl2cap_psm.minRemoteMtu = L2CAP_MINIMUM_MTU;
        btxp->server.goepl2cap_psm.authorizedSetting = FALSE;	    
        btxp->server.goepl2cap_psm.op_state = L2OPERATORSTATE_STATE1;

        btxp->server.goepl2cap_psm.inLinkMode = btxp->server.goepl2cap_psm.outLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION; //L2MODE_BASIC;
        if( NULL != btxp->server.pl2Buffer ){
            btxp->server.goepl2cap_psm.inTransmitMax = 100; //btxp->server.pl2Buffer->rxNum; // default value
        }else{
            kal_trace(BT_TRACE_G6_OBEX , OBEX_NO_L2CAP_BUF);
            btxp->server.goepl2cap_psm.inLinkMode = btxp->server.goepl2cap_psm.outLinkMode = L2MODE_BASIC;
            btxp->server.goepl2cap_psm.inTransmitMax = 0;
        }
        btxp->server.goepl2cap_psm.channelUsage = L2CAP_CHANNEL_DEDICATED;
        btxp->server.goepl2cap_psm.inBufferSize = (L2CAP_MAXIMUM_MTU + 7)*24;

        btxp->server.goepl2cap_psm.psm = OBEXL2GetLocalPsmValue( obex_service_type, FALSE);
#ifdef __BT_2_1_ERTM__    
        btxp->server.goepl2cap_psm.lockStepNeeded = GOEP_USE_HIGHSPEED;
        btxp->server.goepl2cap_psm.highSpeedRequired = GOEP_USE_HIGHSPEED;
        GOEP_Report("OBEX: S locakStepNeeded:%d highSpeedRequired:%d", btxp->server.goepl2cap_psm.lockStepNeeded, btxp->server.goepl2cap_psm.highSpeedRequired);
#endif

        status = L2CAP_RegisterPsm(&btxp->server.goepl2cap_psm) ;
        kal_trace(BT_TRACE_G6_OBEX, OBEX_REG_L2CAP_PSM_RETURN, status, btxp->server.goepl2cap_psm.psm);
        if ( status != BT_STATUS_SUCCESS){
            return status;
        }		
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
        InitializeListEntry(&(btxp->server.secRecord.node));
        btxp->server.secRecord.id = SEC_L2CAP_ID;
        btxp->server.secRecord.channel = btxp->server.goepl2cap_psm.psm;
        btxp->server.secRecord.level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
        btxp->server.secRecord.level_type = BT_SEC_LOW;
#else 
        btxp->server.secRecord.level_type = BT_SEC_MED;
#endif 
        GOEP_Report("OBEX: S SEC_RegisterRecord level_type:%d psm:0x%x", btxp->server.secRecord.level_type, btxp->server.secRecord.channel);
        SEC_RegisterRecord(&btxp->server.secRecord);
#endif

	}
#endif	
#endif
    
    status = BT_STATUS_SUCCESS;

    return status;
}

ObStatus BTSTACK_ServerFlushChannel(ObBtServerTransport *btxp){
    ObStatus status = BT_STATUS_SUCCESS;
    U8 index = 0;
    U8 result = 1; // flush number
    L2capChannelId channel_id =0;
    
#ifdef __BT_3_0_HS__  
    if( btxp->server.bConnType == GOEP_TP_L2CAP_ONLY ){
        channel_id = btxp->server.l2ChannelId;
        while( 0 != result && index < 100 ){
            result = L2Cap_FlushOnePktTransmits(channel_id);
            index++;
        }    
        OS_Report("obex: l2ChannelId:%d flushnum:%d", channel_id, index);
    }else{
        OS_Report("obex: flush fail! connType:%d", btxp->server.bConnType);
    }
#endif    
    return status;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            BTSTACK_ServerDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server Bluetooth transport.
 *
 * Return:    TRUE -  Server stack deinitialized.
 *            FALSE - Server cannot deinitialize.
 *
 */
BOOL BTSTACK_ServerDeinit(ObBtServerTransport *btxp)
{
    BtStatus    status;
    ObexServerTransports *transport = ContainingRecord(btxp, ObexServerTransports, ObexServerBtTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexServerApp, trans);

    if (btxp->server.state == OCS_UNINITIALIZED) {
        /* Already deinitialized */
        return TRUE;
    }
/*  To deregister RFCOMM and CMGR even if the state is not IDLE
    if (btxp->server.state != OCS_IDLE) {
        return FALSE;
    }
*/

    ASSERT(btxp->server.app == appHndl);
    
#if BT_GOEP_V2 == XA_ENABLED    
    if( btxp->server.bGoepL2cap != 0 ){
        status = L2CAP_DeregisterPsm(&btxp->server.goepl2cap_psm) ;
        kal_trace(BT_TRACE_G6_OBEX , OBEX_DEREG_L2CAP_PSM, status);
    }


#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    GOEP_Report("OBEX: S SEC_Unregister level_type:%d ", btxp->server.secRecord.level_type);
    SEC_Unregister(&btxp->server.secRecord);
#endif
#endif

    /* Unbind channel */
    status = RF_DeregisterService(&btxp->serverReg);
    status = CMGR_DeregisterHandler(&btxp->server.cmgr_hdl);
    if (status != BT_STATUS_SUCCESS) 
		return FALSE;

    btxp->server.state = OCS_UNINITIALIZED;
    btxp->server.app = 0;
    
    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/****************************************************************************
 *
 * OBEX Bluetooth Transport - Internal functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            BtServCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The OBEX Server's stack callback function. The stack calls
 *            this function with stack events.
 *
 * Return:    void
 *
 */
static void BtServCallBack(RfChannel *Channel, RfCallbackParms *parms)
{
    BtStatus    status;
    ObexPacket *obp;
    ObBtServerTransport *btxp;
    ObexServerApp *appHndl;
    U8 *addr;
    char *dev_name = 0;
    goep_bd_addr_struct bd_addr;
    

    OS_LockObex();

    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtServerTransport *)ContainingRecord(Channel, ObBtCommonTransport, channel);
    appHndl = (ObexServerApp *)btxp->server.app;

    switch (parms->event) {
    case RFEVENT_OPEN_IND:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_OPEN_IND, btxp->server.state, appHndl);
        if (btxp->server.state > OCS_IDLE)
        {
            /* we're busy: Reject the connection  */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_BUSY_REJECT_CONNECT);
            status = RF_RejectChannel(Channel);
            break;
        }
#if BT_GOEP_V2 == XA_ENABLED
        btxp->server.bConnType = GOEP_TP_RFCOMM_ONLY; //QDAY incoming at the same time !
#endif        
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_CREATEDATALINK);
        status = CMGR_CreateDataLink(&btxp->server.cmgr_hdl, &parms->ptrs.remDev->bdAddr);
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CMGR_CREATEDATALINK_RETURN, status);

        if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
        {
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_BUSY_REJECT_CONNECT);
            status = RF_RejectChannel(Channel);
            break;
        }
		
#if BT_GOEP_V2 == XA_ENABLED
         // QDAY: remove this. check in front of this function
        if( btxp->server.state == OCS_CONNECTED || btxp->server.state == OCS_CONNECTING ){
            if( btxp->server.bConnType == GOEP_TP_L2CAP_ONLY ){
                kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_CONNECTED_REJECT_INCOMING_RFCOMM);
                status = RF_RejectChannel(Channel);
                break;
            }
        }
#endif
		
        /* Accept the incoming connection to our server if it's idle. */
        if (OBPROT_LinkTransport(btxp->server.app, &btxp->server.transport)) {
#if BT_GOEP_V2 == XA_ENABLED
            btxp->server.bConnType = GOEP_TP_RFCOMM_ONLY;
#endif

            ASSERT(btxp->server.state != OCS_CONNECTED);
/* Get device address from the RFCALLBACK params */
/* Get device name from the Dlight's API */
            addr = parms->ptrs.remDev->bdAddr.addr;
            ((goep_bd_addr_struct *)&bd_addr)->nap = (((U16)((U8 *) addr)[4])) | ((U16)((U8 *) addr)[5]<<8);
            ((goep_bd_addr_struct *)&bd_addr)->uap = (U8)((U8 *) addr)[3];
            ((goep_bd_addr_struct *)&bd_addr)->lap =  (((U32)((U8 *) addr)[2]) << 16) | ((U32)((U8 *) addr)[1] <<8)|((U32)((U8 *) addr)[0]); 
            dev_name = (char *)BTBMGetLocalStoredName(addr) ;

    	    status = GOEP_ServerRFCommOpenInd(appHndl, &bd_addr, dev_name);
    	    obex_start_timer(
    	        &btxp->rfOpenTimer, 
    	        OBEX_RF_OPEN_TIMEOUT,
    	        BTSTACK_RFOpenTimeout,
    	        btxp);
			
            btxp->server.state = OCS_CONNECTING;
        } else {
            /* Reject the connection, we're busy */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_LINKTRANSPORTFAILED_REJECT_CONNECT);
            status = RF_RejectChannel(Channel);

            /* remove CMGR data link */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_REMOVE_DL);
            status = CMGR_RemoveDataLink(&btxp->server.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
            btxp->server.cmgr_hdl.remDev = 0;
            btxp->server.cmgr_hdl.bdc = 0;                        
        }            
        ASSERT(status == BT_STATUS_PENDING);
        break;

    case RFEVENT_OPEN:
#ifdef _DLIGHT_TEST_
        OBEX_TX_COunt = 0;
#endif
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_OPEN, btxp->server.state, appHndl);
        ASSERT(btxp->server.state == OCS_CONNECTING);

        obex_stop_timer(&btxp->rfOpenTimer);
        btxp->server.state = OCS_CONNECTED;

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
        btxp->server.credits = 0;
        btxp->server.flags &= ~BTF_FLOW;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
        /* Tell the application that the server has accepted a connection */
        OBPROT_Connected(btxp->server.app);

        SnifferRegisterEndpoint( SPI_RFCOMM, &Channel->dlci,
                                 RF_RemoteDevice(Channel), SPI_OBEX);
        //add by stanley 2006-10-16: Do role switch to change to be a master when ftp or opp is connected
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SET_MASTER);
        CMGR_SetDeviceAsMaster(&btxp->server.cmgr_hdl);                       
        break;

    case RFEVENT_DATA_IND:
        /* Process the received data */
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
        (btxp->server.credits)++;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_EVENT_RECEIVED_xD_BYTES , parms->dataLen);
        OBPROT_ParseRx(btxp->server.app, parms->ptrs.data, parms->dataLen);

        /* The packet has been consumed so advance credit */
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
        if ((btxp->server.flags & BTF_FLOW) == 0) {
            RF_AdvanceCredit(Channel, btxp->server.credits);
            btxp->server.credits = 0;
        }
#else
        RF_AdvanceCredit(Channel, 1);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
        break;

    case RFEVENT_CLOSED:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_EVENT_CLOSED, btxp->server.state,appHndl);
#if BT_GOEP_V2 == XA_ENABLED
		if (btxp->server.state > OCS_IDLE && btxp->server.bConnType == GOEP_TP_L2CAP_ONLY ){
			// ignore the reject of incoming RFComm when there is a L2CAP connection
#else			
		if (btxp->server.state <= OCS_IDLE ){	
#endif
			kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_IGNORED_RFCOMM_EVENT_xD);
		}else{
		
        if (btxp->server.state > OCS_IDLE) {
            status = CMGR_RemoveDataLink(&btxp->server.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);
            btxp->server.cmgr_hdl.remDev = 0;
            btxp->server.cmgr_hdl.bdc = 0;
            
            obex_stop_timer(&btxp->rfOpenTimer);
            btxp->server.state = OCS_IDLE;
#if 1
            //  MAUI w0946
#else
            {
                BOOL ret = OBPROT_ReturnTxBuffer_IfNeed(Channel);
                if (ret == TRUE)
			    {
                    kal_trace(BT_TRACE_G6_OBEX, OBEX_PROC_PENDING_RFPKT,ret);
                    OBPROT_GivePktToWaitingApp_IfNeed(btxp->server.app);
                }
		    }
#endif
	
            /* This call aborts the current operation if there
             * is one and reports the link status to the server.
             */
            OBPROT_Disconnected(btxp->server.app, ODR_UNKNOWN);
        }
        
        }
        break;
    
    case RFEVENT_PACKET_HANDLED: 
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_EVENT_TRANSMIT_COMPLETE,parms->status, parms->dataLen,appHndl);

        obp = ContainingRecord(parms->ptrs.packet, ObexPacket, btpkt);
        ASSERT(&obp->btpkt == parms->ptrs.packet);

        if (btxp->server.app)            
            btxp->server.app->SendingRFPktNum--;
    
        OBPROT_StopTxTimer(obp);
#if 1
        // MAUI w0946
        OBPROT_TxDone(btxp->server.app, obp);
#else    
        if (parms->status == BT_STATUS_SUCCESS)
        {
            OBPROT_TxDone(btxp->server.app, obp);
        }
        else 
        {
			OBPROT_ReturnTxBuffer(obp->buffer);
            OBPROT_GivePktToWaitingApp_IfNeed(btxp->server.app);
        }
#endif
        break;
    }

    OS_UnlockObex();
}
#if BT_GOEP_V2 == XA_ENABLED
/*---------------------------------------------------------------------------
 *            BtClientL2CallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The OBEX Client's stack callback function. The stack calls
 *            this function with stack events.
 *
 * Return:    void
 *
 */
void BtServL2CallBack(L2capChannelId L2ChannelId, L2capCallbackParms *Parms)
{
    ObBtServerTransport *btxp;
    ObexServerApp *appHndl;
	BtStatus status;
    ObexPacket *obp;
    U8 *addr;
    char *dev_name = 0;
    goep_bd_addr_struct bd_addr;
    
	OS_LockObex();
    
    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtServerTransport *)ContainingRecord( Parms->owner.psm, ObBtCommonTransport, goepl2cap_psm);
    appHndl = (ObexServerApp *)btxp->server.app;

    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_L2CALLBACK, L2ChannelId, Parms->event, Parms->owner.psm->psm);
#ifdef __BT_2_1_ERTM__    
    switch(Parms->event){
        case L2EVENT_CONNECT_IND:
            /* server only */
           // kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_OPEN_IND, btxp->server.state, appHndl);
           kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_CURRENT_STATE,  btxp->server.state, appHndl);
            if (btxp->server.state > OCS_IDLE)
            {
                /* we're busy: Reject the connection  */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_BUSY_REJECT_CONNECT);
                status = L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, NULL);
                break;
            }

            /* Setup the l2 status */
            btxp->server.l2ChannelId = L2ChannelId;
            btxp->server.bConnType = GOEP_TP_L2CAP_ONLY;            
            
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_CREATEDATALINK);
            status = CMGR_CreateDataLink(&btxp->server.cmgr_hdl, &Parms->aclLink->bdAddr);
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CMGR_CREATEDATALINK_RETURN, status);

            if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
            {
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_BUSY_REJECT_CONNECT);
                status = L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, NULL);
                break;
            }

            // reject the second connection
            if ( btxp->server.state == OCS_CONNECTED || btxp->server.state == OCS_CONNECTING )
            {
                kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_REJECT_SECOND_INCOMING, btxp->server.state, L2ChannelId);                
                status = L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, NULL);
                break;
            }
            
            /* Accept the incoming connection to our server if it's idle. */
            if (OBPROT_LinkTransport(btxp->server.app, &btxp->server.transport)) {
               
                ASSERT(btxp->server.state != OCS_CONNECTED);
				/* Get device address from the L2CAPCALLBACK params */
                addr = Parms->aclLink->bdAddr.addr;
                ((goep_bd_addr_struct *)&bd_addr)->nap = (((U16)((U8 *) addr)[4])) | ((U16)((U8 *) addr)[5]<<8);
                ((goep_bd_addr_struct *)&bd_addr)->uap = (U8)((U8 *) addr)[3];
                ((goep_bd_addr_struct *)&bd_addr)->lap =  (((U32)((U8 *) addr)[2]) << 16) | ((U32)((U8 *) addr)[1] <<8)|((U32)((U8 *) addr)[0]); 
                dev_name = (char *)BTBMGetLocalStoredName(addr) ;

        	    status = GOEP_ServerRFCommOpenInd(appHndl, &bd_addr, dev_name); //QDAY !
        	    // start timer for auto reject obex_start_timer
				obex_start_timer(
								&btxp->rfOpenTimer, 
								OBEX_RF_OPEN_TIMEOUT,
								BTSTACK_RFOpenTimeout,
								btxp);

                btxp->server.state = OCS_CONNECTING;
            } else {
                /* Reject the connection, we're busy */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_LINKTRANSPORTFAILED_REJECT_CONNECT);
                status = L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, NULL);

                /* remove CMGR data link */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_REMOVE_DL);
                status = CMGR_RemoveDataLink(&btxp->server.cmgr_hdl);
                kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
                btxp->server.cmgr_hdl.remDev = 0;
                btxp->server.cmgr_hdl.bdc = 0;                        
            }            
            ASSERT(status == BT_STATUS_PENDING);
            break;
        case L2EVENT_CONNECTED:
             /* The RFCOMM connection is now up. Call the application and
             * indicate that data can be written.
             */
            btxp->server.bConnType = GOEP_TP_L2CAP_ONLY;
             
            //kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RFCOMM_OPEN, btxp->server.state, appHndl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_L2CAP_OPEN,  btxp->server.state, appHndl);            
            ASSERT(btxp->server.state == OCS_CONNECTING);

            obex_stop_timer(&btxp->rfOpenTimer);
            btxp->server.state = OCS_CONNECTED;

    #if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            btxp->server.credits = 0;
            btxp->server.flags &= ~BTF_FLOW;
    #endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            /* Tell the application that the server has accepted a connection */
            OBPROT_Connected(btxp->server.app);

            // TODO: why need switch 
            /*SnifferRegisterEndpoint( SPI_RFCOMM, &Channel->dlci,
                                     RF_RemoteDevice(Channel), SPI_OBEX);
            //add by stanley 2006-10-16: Do role switch to change to be a master when ftp or opp is connected
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SET_MASTER);
            CMGR_SetDeviceAsMaster(&btxp->server.cmgr_hdl);*/
        
            break;
        case L2EVENT_DISCON_IND:
            /* L2cap is going to disconnect */
            break;
        case L2EVENT_DISCONNECTED:
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_L2CAP_DISCONNECTED,  btxp->server.state, appHndl);            
            
			if (btxp->server.state > OCS_IDLE && btxp->server.bConnType == GOEP_TP_RFCOMM_ONLY ){
				// ignore the l2cap disconnect
				kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_IGNORED_L2CAP_EVENT_xD);
			}else{

            if( NULL != btxp ){
                OS_Report("[OBEX:] cur l2channelId:0x%x", btxp->server.l2ChannelId);
            }

            if( L2ChannelId == btxp->server.l2ChannelId ){

            if (btxp->server.state > OCS_IDLE) {
                status = CMGR_RemoveDataLink(&btxp->server.cmgr_hdl);
                kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);
                btxp->server.cmgr_hdl.remDev = 0;
                btxp->server.cmgr_hdl.bdc = 0;
                
                obex_stop_timer(&btxp->rfOpenTimer);
                btxp->server.state = OCS_IDLE;
    	
                /* This call aborts the current operation if there
                 * is one and reports the link status to the server.
                 */
                OBPROT_Disconnected(btxp->server.app, ODR_UNKNOWN);
            }
            
            /* Deinit the status of transport */
            btxp->server.bConnType = GOEP_TP_RFCOMM_ONLY;
            
            }else{
                /* CR: ALPS00094896 Two ftp clients test */
                OS_Report("[OBEX:][Wrn] ignore l2event_disc from channel id 0x%x", L2ChannelId);
            }
            
            }
            break;
        case L2EVENT_DATA_IND:
            // "OBEX: OBEX L2cap should not receive basic mode data !");
            OS_Report("[OBEX:][Err] Should not use basic mode !!!");
	    status = L2CAP_DisconnectReq( L2ChannelId );

            break;
        case L2EVENT_PACKET_HANDLED:
            /* Continue to send data */
#ifdef _DLIGHT_TEST_
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_L2CAP_PACKET_HANDLED,  btxp->server.state, appHndl);                        
            OBEX_TX_COunt--;
            Report(("Dlight L2EVENT_PACKET_HANDLED OBEX_TX_COunt:%d",OBEX_TX_COunt));
            Report(("Dlight L2EVENT_PACKET_HANDLED %08x", Parms->ptrs.packet));            
#endif

            obp = ContainingRecord(Parms->ptrs.packet, ObexPacket, btpkt);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_L2CAP_PACKET_HANDLED,  btxp->server.state, appHndl, Parms->ptrs.packet, (ObexData.packetnum+1) );

            ASSERT(&obp->btpkt == Parms->ptrs.packet);

            if (btxp->server.app)            
                btxp->server.app->SendingRFPktNum--;
        
            // TODO: OBPROT_StopTxTimer(obp);

            if (Parms->status == BT_STATUS_SUCCESS)
            {
                OBPROT_TxDone(btxp->server.app, obp);
            }
            else 
            {
    			OBPROT_ReturnTxBuffer(obp->buffer);
                // OBPROT_GivePktToWaitingApp_IfNeed(btxp->server.app); //QDAY:
            }
            
            break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        case L2EVENT_FLOW_DATA_IND:
            {
                BtRxPacket *rxPacket;
            
                appHndl = (ObexServerApp *)btxp->server.app;
                if(IsListEmpty(&appHndl->l2buffer.receivedPacketsList) )
                {
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_FLOW_DATA_IND_ERR);
                    return;
                }
                
                rxPacket = (BtRxPacket *)RemoveHeadList(&(appHndl->l2buffer.receivedPacketsList) );
#ifdef _DLIGHT_TEST_
                if(rxPacket->data[0] == 0xff)
                {
                    Report(("Dlight rx test packet"));
                    InsertTailList(&appHndl->l2buffer.rxPacketsBufferList, &rxPacket->node);
                    L2CAP_UpdateSduDataBufferSeq( L2ChannelId );
                    break;
                }
#endif
		        /* Process the received data */
		        kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_FLOW_DATA_IND,  btxp->server.state, appHndl, rxPacket->dataLen);
                OBPROT_ParseRx(btxp->server.app, rxPacket->data, rxPacket->dataLen);

                InsertTailList(&appHndl->l2buffer.rxPacketsBufferList, &rxPacket->node);
                L2CAP_UpdateSduDataBufferSeq( L2ChannelId );
            }
			break;
#endif      

#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
        case L2EVENT_LOCKSTEP_CONFIG_REQ:
            //            L2CAP_SendLockStepConfigRsp(L2ChannelId, BT_STATUS_SUCCESS);
            status = BTA2MP_LogicalLinkCreate(L2ChannelId,
                                    Parms->ptrs.lockstepConfigReq.createChannelnitiator,
                                    Parms->aclLink,
                                    Parms->aclLink->remoteWifiAMP.remoteControllerId);
            Report(("Lock Step config create logical link status:%d", status));
            if(status == BT_STATUS_PENDING)
            {
                /* Wait for LogicalLInk create complete */
            }
            else if(status == BT_STATUS_SUCCESS)
            {
                L2CAP_SendLockStepConfigRsp(L2ChannelId, BT_STATUS_SUCCESS);    
            }
            break;
#endif
        case L2EVENT_NOTIFY_PSM_CHECK_MOVE:
            L2CAP_MoveChannelReq(Parms->aclLink,
                L2ChannelId,
                A2MP_GetActivatedControllerId(Parms->aclLink),
                Parms->aclLink->remoteWifiAMP.remoteControllerId);
            break;
        case L2EVENT_NOTIFY_PSM_MOVE_BACK_TO_BR:
            L2CAP_MoveChannelReq(Parms->aclLink,
                L2ChannelId,
                0x00,
                0x00);
            break;
        case L2EVENT_MOVE_CHANNEL_REQ:
            /* L2C(callbackParms).ptrs.moveChannelReq.controllerId is our controller Id */
            {
                U8 localControllerId;
                U8 tempStatus;
                localControllerId = A2MP_GetActivatedControllerId(Parms->aclLink);
                OS_Report("oldctrid:%d reqctrid:%d", Parms->ptrs.moveChannelReq.oldControllerId, Parms->ptrs.moveChannelReq.controllerId);
                if(Parms->ptrs.moveChannelReq.oldControllerId ==
                    Parms->ptrs.moveChannelReq.controllerId)
                {
                    OS_Report("movechannel server return IS_OLD");
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_CONTROLER_IS_OLD, 0);
                    break;
                }
                if((Parms->ptrs.moveChannelReq.controllerId != 0) &&
                    (localControllerId != Parms->ptrs.moveChannelReq.controllerId))
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_CONTROLER_NOT_SUPPORTED, 0);
                    break;
                }
                tempStatus = BTA2MP_LogicalLinkCreate(L2ChannelId,
                                                BT_AMP_COMMAND_ACCEPT_LOGICAL_LINK,
                                                Parms->aclLink,
                                                Parms->ptrs.moveChannelReq.controllerId);
                if (tempStatus == BT_STATUS_SUCCESS)
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_ACCEPTED, Parms->ptrs.moveChannelReq.controllerId);
                }
                else if (tempStatus == BT_STATUS_PENDING)
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_PENDING, 0);
                }
                else
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_CONTROLER_NOT_ALLOWED, 0);
                }
            }
            break;
        case L2EVENT_MOVE_CHANNEL_RSP:
            Report(("Move channel response:%d", Parms->ptrs.moveChannelRsp.result));
            if(Parms->ptrs.moveChannelRsp.result == L2MOVE_PENDING)
            {
                BtStatus tempStatus;

                BTA2MP_LogicalLinkCreate(L2ChannelId,
                                        BT_AMP_COMMAND_CREATE_LOGICAL_LINK,
                                        Parms->aclLink,
                                        L2C(callbackParms).ptrs.moveChannelRsp.controllerId);
            }
            else if (Parms->ptrs.moveChannelRsp.result == L2MOVE_ACCEPTED)
            {
                L2capExtendedFlowSpecOption *inExtFlowSpec;

                inExtFlowSpec = (L2capExtendedFlowSpecOption *)L2Cap_GetInExtendedFlowSpec(L2ChannelId);
                if(inExtFlowSpec->serviceType <= 0x01)
                {
                    Report(("in flow service type:%d",inExtFlowSpec->serviceType));
                    BTA2MP_LogicalLinkCreate(L2ChannelId,
                                            0,
                                            Parms->aclLink,
                                            L2C(callbackParms).ptrs.moveChannelRsp.controllerId);
                }
                /* The logical link shall created done. */
                L2CAP_MoveChannelCnfReq(Parms->aclLink, L2ChannelId, L2MOVE_CNF_RESULT_SUCCESS);
            }
            else if (Parms->ptrs.moveChannelRsp.result != L2MOVE_CONTROLER_MOVE_COLLISION)
            {
                OS_Report("Move fail result:%d", Parms->ptrs.moveChannelRsp.result );
                L2CAP_MoveChannelCnfReq(Parms->aclLink, L2ChannelId, L2MOVE_CNF_RESULT_FAILED);
            }
            break;
        case L2EVENT_MOVE_CHANNEL_CNF_REQ:
            if(Parms->ptrs.moveChannelCnfReq.result == L2MOVE_CNF_RESULT_SUCCESS)
            {
                L2CAP_MoveChannelCnfRsp(Parms->aclLink, L2ChannelId);
            }
            else
            {
                L2CAP_MoveChannelCnfRsp(Parms->aclLink, L2ChannelId);
            }
            break;
#if 0
        case L2EVENT_CREATE_CHANNEL_REQ:
            /* Request from a new machine.  A new multiplexer must be added */
            if (RfAllocMux(Parms->aclLink, L2ChannelId, &muxId) != BT_STATUS_SUCCESS)
            {
                /* Unable to allocate mux, reject the L2CAP connect indication */
                AssertEval(L2CAP_CreateChannelRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0) == BT_STATUS_PENDING);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_SEND_CONNECT_REJECT, muxId);
            }
            else
            {
                /* Accept the connection */
                setting.reasmBuffer = (ListEntry *)&MUX(muxId).rxPacketsBufferList;
                setting.receivedBuffer = (ListEntry *)&MUX(muxId).receivedPacketsList;
                setting.rxWindowSize = RF_NUM_RETRANSMISSION_WINDOW_SIZE;
                OS_MemSet((U8 *)MUX(muxId).txMaxCountPerIFrame, 0, 128);
                OS_MemSet((U8 *)MUX(muxId).txSrejList, 0, sizeof(MUX(muxId).txSrejList));
                setting.txMaxTransmitCount = (U8 *)MUX(muxId).txMaxCountPerIFrame;
                setting.txSrejList = (U8 *)MUX(muxId).txSrejList;
                setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;
                if(Parms->aclLink->remoteWifiAMP.physical_link_sate != BDS_CONNECTED)
                {
                    if (L2CAP_CreateChannelRsp(L2ChannelId, L2CONN_REJECT_SECURITY_BLOCK, 0, &setting) != BT_STATUS_PENDING)
                    {
                        RfFreeMux(muxId);
                        return;
                    }
                }
                else
                {
                    if (L2CAP_CreateChannelRsp(L2ChannelId, L2CONN_ACCEPTED, 0, &setting) != BT_STATUS_PENDING)
                    {
                        RfFreeMux(muxId);
                        return;
                    }
//                  A2MPHandleCreateServerChannelReq(info->aclLink, L2ChannelId, info->ptrs.createChannelReq.psm, info->ptrs.createChannelReq.controllerId);
                    MUX(muxId).state = MUX_STATE_CONN_INCOMING;
                }
            }
            break;
#endif
        case L2EVENT_READY_TO_MOVE:
            L2CAP_MoveChannelReq(Parms->aclLink,
                                L2ChannelId,
                                A2MP_GetActivatedControllerId(Parms->aclLink),
                                Parms->aclLink->remoteWifiAMP.remoteControllerId);
            break;
        case L2EVENT_LOCK_STEP_DONE:
            if(Parms->aclLink->remoteWifiAMP.physical_link_sate == BDS_DISCONNECTED)
            {
                BTA2MP_StartPhysicalLink(Parms->aclLink, 0x0003, L2ChannelId);
            }
            else
            {
                L2CAP_MoveChannelReq(Parms->aclLink,
                    L2ChannelId,
                    A2MP_GetActivatedControllerId(Parms->aclLink),
                    Parms->aclLink->remoteWifiAMP.remoteControllerId);
            }
            break;
        case L2EVENT_CONNECTED_START_CREATE_PHYSICAL_LINK:
            L2Cap_StartLockStepConfig(L2ChannelId);
            break;
        default:
            break;
    }
#endif // __BT_2_1_ERTM__    

    OS_UnlockObex();
}
#endif

/*---------------------------------------------------------------------------
 *            BtServerGeneralCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Processes events from the Management Entity and SDP. Only the
 *            server receives events via this handler.
 *
 * Return:    void
 *
 */

void ObexServerConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status) 
{
    ObBtServerTransport *btxp;
	BtStatus status;

    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtServerTransport *)ContainingRecord(Handler, ObBtCommonTransport, cmgr_hdl);

    kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_CONN_CB, Event, Status, btxp->server.state);

    switch (Event) {
    case CMEVENT_DATA_LINK_CON_CNF:
        //Report(("[OBEX]: ObexServerConnectionCallback : CMEVENT_DATA_LINK_CON_CNF.\n"));
        break;
    case CMEVENT_DATA_LINK_DIS:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CMEVENT_DATA_LINK_DIS, btxp->server.state);
        if(btxp->server.cmgr_hdl.bdc !=0)
        {
            if (!StrnCmp(btxp->server.cmgr_hdl.bdc->addr.addr, 6, Handler->bdc->addr.addr,6))
            {
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_BDADDR_ERR);
                return;
            }
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_REMOVE_DL);
            status = CMGR_RemoveDataLink(&btxp->server.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);
            btxp->server.cmgr_hdl.remDev = 0;
            btxp->server.cmgr_hdl.bdc = 0;
        }
        break;
    default:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_IGNORED_EVENT_xD , Event);
        break;
    }

}

#if BT_GOEP_V2 == XA_ENABLED
static void BTSTACK_AUTO_GOEPL2SDP_Timeout(void *para)
{
	// run start timer
	BtStatus status = BT_STATUS_PENDING;
    ObBtClientTransport *btxp;
	EvmTimer *ptimer;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_GOEPL2SDP_TIMEOUT);
	ptimer = (EvmTimer *)para;
    btxp = (ObBtClientTransport *)ptimer->context;
	
	if( btxp->target_uuid == 0 ){
		kal_trace(BT_TRACE_G6_OBEX,OBEX_L2TIMEOUT_IGNORE);
		return;
	}

    status = BtStartServiceQuery(btxp);
	// TODO: check the acl is connected !
    if( status != BT_STATUS_PENDING ){
        kal_trace(BT_TRACE_G6_OBEX,OBEX_START_SERVICE_QUERY_FAIL, status);
        OS_LockObex();
        BtDisconnectClientLink(btxp);
        OS_UnlockObex();                
    }
	// wait for sdp callback
	btxp->client.state = OCS_SERVICE_QUERY;
}
#endif

static void BTSTACK_RFOpenTimeout(void *para)
{
    ObBtServerTransport *btxp;

    btxp = (ObBtServerTransport *)((EvmTimer *)para)->context;

    kal_trace(
        BT_TRACE_G6_OBEX,
        OBEX_RFOPEN_TIMEOUT,
        btxp, btxp->server.state,
        btxp->rfOpenTimer);

    if (btxp->server.state == OCS_CONNECTING && btxp->rfOpenTimer.time != 0)
    {
        btxp->rfOpenTimer.time = 0;
        BTSTACK_RejectRFChnConn(btxp);
    }
}


#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)*/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            BtClientCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The OBEX Client's stack callback function. The stack calls
 *            this function with stack events.
 *
 * Return:    void
 *
 */
#ifdef _DLIGHT_TEST_
U16 l2cap_id_test;
extern U8 IsListEmpty(ListEntry *list);

static void obexclienttimer1(EvmTimer *timer)
{
    BtPacket *packet;
    BtStatus status;
    Report(("obexclienttimer1 try to send data"));
    while(!IsListEmpty(&dlight_tx_list))
	{
        packet = (BtPacket *)RemoveHeadList(&dlight_tx_list);
        packet->data = dlight_tx_buffer;
        packet->dataLen = 600;
        packet->headerLen = 0;
        packet->priority_type = BTP_FLAG_LOW_PRIPORTY;
        OBEX_TX_COunt++;
        Report(("Dlight OBEX_TX_Count:%d",OBEX_TX_COunt));
        Report(("Dlight L2CAP_SendData:%08x",(packet)));
        status = L2CAP_SendData(l2cap_id_test, packet );
        kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_SENDDATA, status, 600);
        if(status == BT_STATUS_FAILED)
        {
            Report(("Dlight tx failed"));
            InsertTailList(&dlight_tx_list, packet);
            break;
        }
	} 

        
        
}
#endif

void BtClientCallBack(RfChannel *Channel, RfCallbackParms* parms)
{
    BtStatus status;
    ObexPacket *obp;
    ObBtClientTransport *btxp;

    OS_LockObex();

    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtClientTransport *)ContainingRecord(Channel, ObBtCommonTransport, channel);

    switch (parms->event) {
    case RFEVENT_OPEN:
#ifdef _DLIGHT_TEST_
        OBEX_TX_COunt = 0;
#endif
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RFCOMM_OPEN, btxp->client.state);
        /* The RFCOMM connection is now up. Call the application and
         * indicate that data can be written.
         */
        if (btxp->client.state == OCS_CONNECTING || btxp->client.state == OCS_SERVICE_QUERY) {
            btxp->client.state = OCS_CONNECTED;

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            btxp->client.credits = 0;
            btxp->client.flags &= ~BTF_FLOW;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            OBPROT_Connected(btxp->client.app);

            SnifferRegisterEndpoint(SPI_RFCOMM, &Channel->dlci,
                                    RF_RemoteDevice(Channel), SPI_OBEX);
                                    
            //add by stanley 2006-10-16: Do role switch to change to be a master when ftp or opp is connected
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SET_MASTER);
        		CMGR_SetDeviceAsMaster(&btxp->client.cmgr_hdl);                        
        } else {
            /* We must be disconnecting */
            ASSERT(btxp->client.state == OCS_DISCONNECTING);
            status = RF_CloseChannel(Channel);
	        kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL_RETURN, status);
            if (status != BT_STATUS_PENDING) {
                BtDisconnectClientLink(btxp);
            }            
        }
        break;

    case RFEVENT_CLOSED:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RFCOMM_CLOSED, btxp, Channel, btxp->client.state);
        
        if ((btxp->client.state == OCS_CONNECTING) && (btxp->index < OBEX_RF_SERVICE_CHANNELS)) {
            /* Try another possible RFCOMM service channel if this last one failed during a 
             * connection.  This channel may be in use by another device, since we don't
             * register multiple channels on the same serviceId.
             * RFCOMM channel should large then 0
             */
            if( 0 != btxp->remoteRfcServer[btxp->index] ){
                kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_OPEN_RFCHANNEL, &btxp->client.channel,btxp->target_uuid, btxp->serv_chn);
                status = RF_OpenClientChannel(btxp->remoteDevice,btxp->remoteRfcServer[btxp->index++],
                                          &btxp->client.channel, OBEX_PERFORMANCE_MULTIPLIER);
			
                kal_trace(BT_TRACE_G6_OBEX,OBEX_OPEN_RFCHANNEL_RETURN, status);
            }else{
                status = BT_STATUS_FAILED;
            }
            if (status != BT_STATUS_PENDING) {
                BtDisconnectClientLink(btxp);
            }
        }
        else if (btxp->client.state > OCS_IDLE) {
            //kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RFCOMM_EVENT_CLOSED);
#if 1
            // MAUI w0946 
#else
            {
                BOOL ret = OBPROT_ReturnTxBuffer_IfNeed(Channel);
                if (ret == TRUE)
			    {
                    kal_trace(BT_TRACE_G6_OBEX, OBEX_PROC_PENDING_RFPKT,ret);
                    OBPROT_GivePktToWaitingApp_IfNeed(btxp->client.app);
                }
			}
#endif
            BtDisconnectClientLink(btxp);
        }
                
        break;

    case RFEVENT_DATA_IND:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RFCOMM_DATA_IND,parms->dataLen);
        
        /* Process the received data */
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
        (btxp->client.credits)++;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
        //kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RFCOMM_EVENT_RECEIVED_xD_BYTES , parms->dataLen);
        OBPROT_ParseRx(btxp->client.app, parms->ptrs.data, parms->dataLen);

        /* The packet has been consumed so advance credit */
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
        if ((btxp->client.flags & BTF_FLOW) == 0) {
            RF_AdvanceCredit(Channel, btxp->client.credits);
            btxp->client.credits = 0;
        }
#else
        RF_AdvanceCredit(Channel, 1);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
        break;

    case RFEVENT_PACKET_HANDLED:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RFCOMM_EVENT_TRANSMIT_COMPLETE, parms->status, parms->dataLen);

        obp = ContainingRecord(parms->ptrs.packet, ObexPacket, btpkt);
        ASSERT(&obp->btpkt == parms->ptrs.packet);

        if (btxp->client.app != NULL)
            btxp->client.app->SendingRFPktNum--;
    
        OBPROT_StopTxTimer(obp);
#if 1
        // MAUI w0946
        OBPROT_TxDone(btxp->client.app, obp);
#else
    
        if (parms->status == BT_STATUS_SUCCESS)
        {
            OBPROT_TxDone(btxp->client.app, obp);
        }
        else 
        {
			OBPROT_ReturnTxBuffer(obp->buffer);
            OBPROT_GivePktToWaitingApp_IfNeed(btxp->client.app);
        }
#endif
        break;
    }

    OS_UnlockObex();
}

#if BT_GOEP_V2 == XA_ENABLED
/*---------------------------------------------------------------------------
 *            BtClientL2CallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The OBEX Client's stack callback function. The stack calls
 *            this function with stack events.
 *
 * Return:    void
 *
 */
void BtClientL2CallBack(L2capChannelId L2ChannelId, L2capCallbackParms *Parms)
{
    ObBtClientTransport *btxp;
    ObexClientApp *appHndl;
	BtStatus status;
	ObexPacket *obp;

	OS_LockObex();
    
    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtClientTransport *)ContainingRecord( Parms->owner.psm, ObBtCommonTransport, goepl2cap_psm);

    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_L2CALLBACK, L2ChannelId, Parms->event, Parms->owner.psm->psm);
#ifdef __BT_2_1_ERTM__    
    switch(Parms->event){

        case L2EVENT_CONNECT_IND:
            /* server only */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_L2CAP_OPEN, btxp->client.state);
            
            break;
        case L2EVENT_CONNECTED:
            // report to upper layer
             /* The RFCOMM connection is now up. Call the application and
             * indicate that data can be written.
             */
            if (btxp->client.state == OCS_CONNECTING) {
                btxp->client.state = OCS_CONNECTED;

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
                btxp->client.credits = 0;
                btxp->client.flags &= ~BTF_FLOW;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

                /* Record the CurrentTpType */
                btxp->client.l2ChannelId = L2ChannelId;
                btxp->client.bConnType = GOEP_TP_L2CAP_ONLY;
#ifdef _DLIGHT_TEST_
                OBEX_TX_COunt = 0;
                EVM_ResetTimer(&dlight_timer1);
                l2cap_id_test = L2ChannelId;
                dlight_timer1.func = obexclienttimer1;
                Report(("start timer"));
                EVM_StartTimer(&dlight_timer1, 10000);
#endif
                OBPROT_Connected(btxp->client.app);

                // TODO: check this. after switch to l2cap, should we request to as master  ?
#if 0 
                SnifferRegisterEndpoint(SPI_RFCOMM, &Channel->dlci,
                                        RF_RemoteDevice(Channel), SPI_OBEX);
                                        
                //add by stanley 2006-10-16: Do role switch to change to be a master when ftp or opp is connected
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SET_MASTER);
            	CMGR_SetDeviceAsMaster(&btxp->client.cmgr_hdl);                        
#endif            		
            } else {
                /* We must be disconnecting */
                ASSERT(btxp->client.state == OCS_DISCONNECTING);
                status = L2CAP_DisconnectReq( L2ChannelId );
    	        kal_trace(BT_TRACE_G6_OBEX, OBEX_L2CAP_DISCONNECT_REQ, L2ChannelId, status);
            }
            break;
        case L2EVENT_DISCON_IND:
            /* L2cap is going to disconnect */
            break;
        case L2EVENT_DISCONNECTED:
            if ((btxp->client.state == OCS_CONNECTING) && (btxp->index < OBEX_RF_SERVICE_CHANNELS)) {
                if( btxp->client.state == OCS_CONNECTING && Parms->status == L2DISC_CONN_PSM_UNSUPP){
                // fail to connect the l2cap, try to search rfcomm. Only when its reject reason is PSM unsupport ALPS00241840
                obex_start_timer(
        	        &btxp->rfSDPTimer, 
        	        10,
        	        BTSTACK_AUTO_GOEPL2SDP_Timeout,
        	        btxp);
        	    }else{
                /* connect fail notify disconnect */
                BtDisconnectClientLink(btxp);
            }
            }
            else if (btxp->client.state > OCS_IDLE) {
                /* return the sinding pkg but no return from RFComm module */
				/*                
				BOOL ret = OBPROT_ReturnTxBuffer_IfNeed(Channel);
                if (ret == TRUE)
			    {
                    kal_trace(BT_TRACE_G6_OBEX, OBEX_PROC_PENDING_RFPKT,ret);
                    OBPROT_GivePktToWaitingApp_IfNeed(btxp->client.app);
                }
				*/
                /* notify disconnect */
    			BtDisconnectClientLink(btxp);
            }

            /* Deinit the status of transport */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_UNREGEISTER_PSM, btxp->client.bConnType, btxp->client.l2ChannelId);
			L2CAP_DeregisterPsm(&btxp->client.goepl2cap_psm);
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)       
            SEC_Unregister(&btxp->client.secRecord);
#endif

            btxp->client.bConnType = GOEP_TP_RFCOMM_ONLY;
            btxp->client.l2ChannelId = 0;
            
            break;
        case L2EVENT_DATA_IND:
            
            
            break;
        case L2EVENT_PACKET_HANDLED:
            /* Continue to send data */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_L2CAP_EVENT_TRANSMIT_COMPLETE, Parms->status, Parms->dataLen);
#ifdef _DLIGHT_TEST_
            OBEX_TX_COunt--;
            Report(("Dlight L2EVENT_PACKET_HANDLED OBEX_TX_COunt:%d",OBEX_TX_COunt));
            Report(("Dlight L2EVENT_PACKET_HANDLED %08x", Parms->ptrs.packet));            
            if(Parms->ptrs.packet->data[0] == 0xFF)
            {
                Parms->ptrs.packet->data = dlight_tx_buffer;
                Parms->ptrs.packet->dataLen = 600;
                Parms->ptrs.packet->headerLen = 0;
                Parms->ptrs.packet->priority_type = BTP_FLAG_LOW_PRIPORTY;
                OBEX_TX_COunt++;
                Report(("Dlight OBEX_TX_Count1:%d",OBEX_TX_COunt));
                Report(("Dlight L2CAP_SendData1:%08x",(Parms->ptrs.packet)));
                status = L2CAP_SendData(l2cap_id_test, Parms->ptrs.packet );
                kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_SENDDATA, status, 600);
                if(status == BT_STATUS_FAILED)
                {
                    Report(("Dlight tx failed"));
                    InsertTailList(&dlight_tx_list, Parms->ptrs.packet);
                    break;
                }
                break;
            }
#endif
            obp = ContainingRecord(Parms->ptrs.packet, ObexPacket, btpkt);
            ASSERT(&obp->btpkt == Parms->ptrs.packet);
            
            if (btxp->client.app != NULL)
                btxp->client.app->SendingRFPktNum--;
        
            OBPROT_StopTxTimer(obp);
        
#if 1
            // MAUI w0946  QDAY:
            OBPROT_TxDone(btxp->client.app, obp);
#else
            if (Parms->status == BT_STATUS_SUCCESS)
            {
                OBPROT_TxDone(btxp->client.app, obp);
            }
            else 
            {
    			OBPROT_ReturnTxBuffer(obp->buffer);
                     // OBPROT_GivePktToWaitingApp_IfNeed(btxp->client.app); //QDAY:
            }
#endif
            break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        case L2EVENT_FLOW_DATA_IND:
            {
                BtRxPacket *rxPacket;
                //kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_FLOW_DATA_IND, Parms->status, Parms->dataLen);

                appHndl = (ObexClientApp *)btxp->client.app;
                if(IsListEmpty(&appHndl->l2buffer.receivedPacketsList) )
                {
                    Report(("Data Flow ind, rx buffer shall not be empty"));
                    Assert(0);
                }
                rxPacket = (BtRxPacket *)RemoveHeadList(&(appHndl->l2buffer.receivedPacketsList) );
                kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_FLOW_DATA_IND, Parms->status, appHndl, rxPacket->dataLen);                
                
		        /* Process the received data */
                OBPROT_ParseRx(btxp->client.app, rxPacket->data, rxPacket->dataLen);

                InsertTailList(&appHndl->l2buffer.rxPacketsBufferList, &rxPacket->node);
                L2CAP_UpdateSduDataBufferSeq( L2ChannelId );
            }
			break;
#endif     
        
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
        case L2EVENT_LOCKSTEP_CONFIG_REQ:
            //            L2CAP_SendLockStepConfigRsp(L2ChannelId, BT_STATUS_SUCCESS);
            status = BTA2MP_LogicalLinkCreate(L2ChannelId,
                                    Parms->ptrs.lockstepConfigReq.createChannelnitiator,
                                    Parms->aclLink,
                                    Parms->aclLink->remoteWifiAMP.remoteControllerId);
            Report(("Lock Step config create logical link status:%d", status));
            if(status == BT_STATUS_PENDING)
            {
            }
            else
            {
                L2CAP_SendLockStepConfigRsp(L2ChannelId, BT_STATUS_SUCCESS);    
            }
#endif
        case L2EVENT_NOTIFY_PSM_CHECK_MOVE:
            L2CAP_MoveChannelReq(Parms->aclLink,
                L2ChannelId,
                A2MP_GetActivatedControllerId(Parms->aclLink),
                Parms->aclLink->remoteWifiAMP.remoteControllerId);
            break;
        case L2EVENT_NOTIFY_PSM_MOVE_BACK_TO_BR:
            L2CAP_MoveChannelReq(Parms->aclLink,
                L2ChannelId,
                0x00,
                0x00);
            break;
        case L2EVENT_MOVE_CHANNEL_REQ:
            /* L2C(callbackParms).ptrs.moveChannelReq.controllerId is our controller Id */
            {
                U8 localControllerId;
                U8 tempStatus;
                localControllerId = A2MP_GetActivatedControllerId(Parms->aclLink);
                OS_Report("oldctrid:%d reqctrid:%d", Parms->ptrs.moveChannelReq.oldControllerId, Parms->ptrs.moveChannelReq.controllerId);
                if(Parms->ptrs.moveChannelReq.oldControllerId ==
                    Parms->ptrs.moveChannelReq.controllerId)
                {
                    OS_Report("movechannel client return IS_OLD");
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_CONTROLER_IS_OLD, 0);
                    break;
                }
                if((Parms->ptrs.moveChannelReq.controllerId != 0) &&
                    (localControllerId != Parms->ptrs.moveChannelReq.controllerId))
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_CONTROLER_NOT_SUPPORTED, 0);
                    break;
                }
                tempStatus = BTA2MP_LogicalLinkCreate(L2ChannelId,
                                                BT_AMP_COMMAND_ACCEPT_LOGICAL_LINK,
                                                Parms->aclLink,
                                                Parms->ptrs.moveChannelReq.controllerId);
                if (tempStatus == BT_STATUS_SUCCESS)
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_ACCEPTED, Parms->ptrs.moveChannelReq.controllerId);
                }
                else if (tempStatus == BT_STATUS_PENDING)
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_PENDING, 0);
                }
                else
                {
                    L2CAP_MoveChannelRsp(L2ChannelId, L2MOVE_CONTROLER_NOT_ALLOWED, 0);
                }
            }
            break;
        case L2EVENT_MOVE_CHANNEL_RSP:
            Report(("Move channel response:%d", Parms->ptrs.moveChannelRsp.result));
            if(Parms->ptrs.moveChannelRsp.result == L2MOVE_PENDING)
            {
                BtStatus tempStatus;

                BTA2MP_LogicalLinkCreate(L2ChannelId,
                                        BT_AMP_COMMAND_CREATE_LOGICAL_LINK,
                                        Parms->aclLink,
                                        L2C(callbackParms).ptrs.moveChannelRsp.controllerId);
            }
            else if (Parms->ptrs.moveChannelRsp.result == L2MOVE_ACCEPTED)
            {
                L2capExtendedFlowSpecOption *inExtFlowSpec;

                inExtFlowSpec = (L2capExtendedFlowSpecOption *)L2Cap_GetInExtendedFlowSpec(L2ChannelId);
                if(inExtFlowSpec->serviceType <= 0x01)
                {
                    Report(("in flow service type:%d",inExtFlowSpec->serviceType));
                    BTA2MP_LogicalLinkCreate(L2ChannelId,
                                            0,
                                            Parms->aclLink,
                                            L2C(callbackParms).ptrs.moveChannelRsp.controllerId);
                }
                /* The logical link shall created done. */
                L2CAP_MoveChannelCnfReq(Parms->aclLink, L2ChannelId, L2MOVE_CNF_RESULT_SUCCESS);
            }
            else if (Parms->ptrs.moveChannelRsp.result != L2MOVE_CONTROLER_MOVE_COLLISION)
            {
                OS_Report("Move fail result:%d", Parms->ptrs.moveChannelRsp.result );
                L2CAP_MoveChannelCnfReq(Parms->aclLink, L2ChannelId, L2MOVE_CNF_RESULT_FAILED);
            }
            break;
        case L2EVENT_MOVE_CHANNEL_CNF_REQ:
            if(Parms->ptrs.moveChannelCnfReq.result == L2MOVE_CNF_RESULT_SUCCESS)
            {
                L2CAP_MoveChannelCnfRsp(Parms->aclLink, L2ChannelId);
            }
            else
            {
                L2CAP_MoveChannelCnfRsp(Parms->aclLink, L2ChannelId);
            }
            break;
        case L2EVENT_READY_TO_MOVE:
            L2CAP_MoveChannelReq(Parms->aclLink,
                                L2ChannelId,
                                A2MP_GetActivatedControllerId(Parms->aclLink),
                                Parms->aclLink->remoteWifiAMP.remoteControllerId);
            break;
        case L2EVENT_LOCK_STEP_DONE:
            if(Parms->aclLink->remoteWifiAMP.physical_link_sate == BDS_DISCONNECTED)
            {
                BTA2MP_StartPhysicalLink(Parms->aclLink, 0x0003, L2ChannelId);
            }
            else
            {
                L2CAP_MoveChannelReq(Parms->aclLink,
                    L2ChannelId,
                    A2MP_GetActivatedControllerId(Parms->aclLink),
                    Parms->aclLink->remoteWifiAMP.remoteControllerId);
            }
            break;
        case L2EVENT_CONNECTED_START_CREATE_PHYSICAL_LINK:
            L2Cap_StartLockStepConfig(L2ChannelId);
            break;

        default:
            break;
    }
#endif    

    OS_UnlockObex();
}
#endif


/*---------------------------------------------------------------------------
 *            ObexClientConnectionCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Processes events from the CMGR. Only the
 *            client receives events via this handler.
 *
 * Return:    void
 *
 */
void ObexClientConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status) 
{
    ObBtClientTransport *btxp;
    BtStatus status;
    kal_uint8 ucDiscReason;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_CONN_CB, Event, Status);

    if ((Handler->bdc != NULL) && (!StrnCmp(Handler->remDev->bdAddr.addr, 6, Handler->bdc->addr.addr,6)))
    {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_BDADDR_ERR);
        return;
    }
    
    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtClientTransport *)ContainingRecord(Handler, ObBtCommonTransport, cmgr_hdl);

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_STATE, btxp->client.state);

    // Remove the Disconnecting state handle. CMEVENT_READ_RSSI will broadcast
    //    if(btxp->client.state == OCS_DISCONNECTING)


    switch (Event) {
    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
        	ASSERT(Handler->bdc);
            btxp->remoteDevice = Handler->bdc->link;        
			if( OCS_UNINITIALIZED == btxp->client.state ){
				GOEP_Report("[OBEX] CMEVENT_DATA_LINK_CON_CNF in uninit state");
				if( 0 != btxp->client.cmgr_hdl.remDev || 0 != btxp->client.cmgr_hdl.bdc ){
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
				    status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
                    kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);   
				}
                btxp->client.cmgr_hdl.remDev = 0;
                btxp->client.cmgr_hdl.bdc = 0; 
				return;
			}
            OS_Report("obex check the client stat:%d", btxp->client.state); 
            // ALPS00269678
            if(btxp->client.state == OCS_DISCONNECTING)
            {
                // during CM connecting. upper layer abot the connectino
				GOEP_Report("[OBEX] CMEVENT_DATA_LINK_CON_CNF in disconnecting state");
                OS_LockObex();
                BtDisconnectClientLink(btxp);
                OS_UnlockObex();
                return;
            }

#if BT_GOEP_V2 == XA_ENABLED
            if( GOEP_TP_RFCOMM_ONLY == btxp->client.bGoepL2cap ){
                /* only query the goepl2cappsm */
                Status = BtStartServiceQuery(btxp);
            }else{
                Status = BtStartServiceL2Query(btxp);
            }
			
			if( Status == BT_STATUS_PENDING)
#else
            if ((Status = BtStartServiceQuery(btxp)) ==  BT_STATUS_PENDING) 
#endif                
            {
                btxp->client.state = OCS_SERVICE_QUERY;            
                return;
            }
            else
            {
                kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
                status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
                kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);
                if (status == BT_STATUS_SUCCESS) 
                {
                    btxp->remoteDevice = 0;
                    if (btxp->client.state != OCS_IDLE) 
                    {
                        btxp->client.state = OCS_IDLE;
                        OS_LockObex();
                        OBPROT_Disconnected(btxp->client.app, ODR_UNKNOWN);
                        OS_UnlockObex();
                    }
                }
                btxp->client.cmgr_hdl.remDev = 0;
                btxp->client.cmgr_hdl.bdc = 0;  
                return;
            }
        }
        else if (Status == BT_STATUS_SCO_REJECT)
        {   
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_SCO_REJECT);
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
            status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
            if (status == BT_STATUS_SUCCESS) 
            {
                btxp->remoteDevice = 0;
                if (btxp->client.state != OCS_IDLE) 
                {
                    btxp->client.state = OCS_IDLE;
                    OS_LockObex();
                    OBPROT_Disconnected(btxp->client.app, ODR_SCO_REJECT);
                    OS_UnlockObex();
                }
            }
            btxp->client.cmgr_hdl.remDev = 0;
            btxp->client.cmgr_hdl.bdc = 0;              
        }
        else if (Status == BT_STATUS_CHIP_REASON)
        {   
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_BTCHIP_REASON);
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
            status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
            if (status == BT_STATUS_SUCCESS) 
            {
                btxp->remoteDevice = 0;
                if (btxp->client.state != OCS_IDLE) 
                {
                    btxp->client.state = OCS_IDLE;
                    OS_LockObex();
                    OBPROT_Disconnected(btxp->client.app, ODR_BTCHIP_REJECT);
                    OS_UnlockObex();
                }
            }
            btxp->client.cmgr_hdl.remDev = 0;
            btxp->client.cmgr_hdl.bdc = 0;              
        }
        else
        {   
/*         Status == BT_STATUS_SCO_REJECT, shall handle SCO not allow */
            /* Connect Error */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
            status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
            if (status == BT_STATUS_SUCCESS) 
            {
                btxp->remoteDevice = 0;
		// convert BT_STATUS_CHIP_REASON to ODR_UNKNOWN
                if (btxp->client.state != OCS_IDLE) 
                {
                    btxp->client.state = OCS_IDLE;
                    OS_LockObex();	
					switch( Status ){
						case BT_STATUS_CHIP_REASON:
							ucDiscReason = ODR_USER_REQUEST;
							break;
						case BT_STATUS_NOSERVICES: 
							ucDiscReason = ODR_NO_SERVICE_FOUND;
								break;
						case BT_STATUS_NOT_FOUND:
							ucDiscReason = ODR_DISCOVERY_FAILED;
							break;
						default:
							ucDiscReason = ODR_UNKNOWN;
							break;
					}
					OBPROT_Disconnected(btxp->client.app, ucDiscReason);
                    OS_UnlockObex();
                }
            }
            btxp->client.cmgr_hdl.remDev = 0;
            btxp->client.cmgr_hdl.bdc = 0;              
        }
        break;
    case CMEVENT_DATA_LINK_DIS:
        // CMEVENT_READ_RSSI will broadcast so move this to CMEVENT_DATA_LINK_DISC
        if(btxp->client.state == OCS_DISCONNECTING)
        {
            kal_trace(BT_TRACE_G6_OBEX,OBEXCLIENTCONNECTIONCALLBACK_BTXPCLIENTSTATE__OCS_DISCONNECTING);    
            kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
            status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
            if (status == BT_STATUS_SUCCESS) 
            {
                btxp->remoteDevice = 0;
                if (btxp->client.state != OCS_IDLE) 
                {
                    btxp->client.state = OCS_IDLE;
                    OS_LockObex();
                    OBPROT_Disconnected(btxp->client.app, ODR_UNKNOWN);
                    OS_UnlockObex();
                }
            }
            btxp->client.cmgr_hdl.remDev = 0;
            btxp->client.cmgr_hdl.bdc = 0;             
            return;
        }
        
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_CMEVENT_DATA_LINK_DIS_REPORTS_LINK_DISCONNECT);
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
        status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
        kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
        if (status == BT_STATUS_SUCCESS) 
        {
            btxp->remoteDevice = 0;
            if (btxp->client.state != OCS_IDLE) 
            {
                btxp->client.state = OCS_IDLE;
                OS_LockObex();
                OBPROT_Disconnected(btxp->client.app, ODR_UNKNOWN);
                OS_UnlockObex();
            }
        }
        btxp->client.cmgr_hdl.remDev = 0;
        btxp->client.cmgr_hdl.bdc = 0;           
        break;
    default:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_IGNORED_EVENT_xD , Event);
        break;
    }

}

static void BtClientSdpCallback(
                                                        SdpQueryToken *sqt, 
                                                        U8 result, 
                                                        U8 attribute_index,
                                                        U8 *attribute_value
                                                        )
{
    BtStatus status = BT_STATUS_PENDING;
    ObBtClientTransport *btxp;
    SdpQueryToken *token;
    ObexClientApp *appHndl;
    BtStatus notify_rst;
	ObexConnState old_state;
	U16 psmValue;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SDPCALLBACK, result, attribute_index);

    btxp = ContainingRecord(sqt, ObBtClientTransport, sdpQueryToken);

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_STATE, btxp->client.state);

    token  = sqt;
	
    if(btxp->client.state == OCS_IDLE)
    {
        /* maybe has be deinited */
        return;
    }

    if(btxp->client.state == OCS_DISCONNECTING)
    {
        kal_trace(BT_TRACE_G6_OBEX,BTCLIENTSDPCALLBACK_BTXPCLIENTSTATE__OCS_DISCONNECTING);
        OS_LockObex();
        BtDisconnectClientLink(btxp);
        OS_UnlockObex();
        return;
    }
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
            switch (obex_attribute_id_list[attribute_index])
            {
                case AID_PROTOCOL_DESC_LIST:
#if BT_GOEP_V2 == XA_ENABLED
					if(  btxp->client.state ==  OCS_CONNECTING || btxp->client.state == OCS_CONNECTED ) {
						kal_trace(BT_TRACE_G6_OBEX,OBEX_SDPCALLBACK_IGNORE, obex_attribute_id_list[attribute_index], btxp->client.state);
						return;
					}
#endif
                    old_state = btxp->client.state;
                    btxp->client.state = OCS_CONNECTING;
                    btxp->serv_chn = attribute_value[0];
                    switch (btxp->target_uuid)
                    {
                        case SC_OBEX_FILE_TRANSFER:
                        case SC_DIRECT_PRINTING:
                        case SC_PRINTING_STATUS:
                        case SC_IMAGING_RESPONDER:
                    #ifdef __BT_SUPPORT_SYNCML__  
                        case SC_SYNCML_BT_SERVER:
                        case SC_SYNCML_BT_CLIENT:
                    #endif    
                        case SC_MAP_SERVER:
                        case SC_MAP_MNS_SERVER:
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_OPEN_RFCHANNEL, &btxp->client.channel,btxp->target_uuid, btxp->serv_chn);
                            status = RF_OpenClientChannel(btxp->remoteDevice,
                                                                btxp->serv_chn, 
                                                                &btxp->client.channel, 
                                                                OBEX_PERFORMANCE_MULTIPLIER);
							switch( status ){
							    case BT_STATUS_PENDING:
									btxp->client.state = OCS_CONNECTING;
								break;
							    case BT_STATUS_SUCCESS:
								    btxp->client.state = OCS_CONNECTING;
								break;
								default:
									btxp->client.state = OCS_CONNECTING;
									GOEP_Report("[OBEX] no handle ret:%d status:%d", status, btxp->client.state);
									break;
							}
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_OPEN_RFCHANNEL_RETURN, status);
                            break;
                    }
                break;
                case AID_ADDITIONAL_PROT_DESC_LISTS:
                    btxp->client.state = OCS_CONNECTING;
                    btxp->serv_chn = attribute_value[0];
                    switch (btxp->target_uuid)
                    {
                        case SC_PRINTING_STATUS:
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_OPEN_RFCHANNEL, &btxp->client.channel,btxp->target_uuid, btxp->serv_chn);
                            status = RF_OpenClientChannel(btxp->remoteDevice,
                                                                btxp->serv_chn, 
                                                                &btxp->client.channel, 
                                                                OBEX_PERFORMANCE_MULTIPLIER);
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_OPEN_RFCHANNEL_RETURN, status);
                            break;
                        default:
                            ASSERT(0);
                            break;
                    }
                break;
                case AID_SUPPORTED_FORMATS_LIST:
                    /* SDP result parsing completed */
                    /* supported_list is used only by OPP */
                    btxp->supported_list = attribute_value[1];
                    
					Report(("[OBEX]-- SDP supported format: attribute_value[0]=0x%x, attribute_value[1]=0x%x",attribute_value[0],attribute_value[1]));
                    /* notify the client app */
                    appHndl = (ObexClientApp *)btxp->client.app;
                    notify_rst = GOEP_ClientSDPSupportedFormatsInd(appHndl, btxp->supported_list);

                    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_OPEN_RFCHANNEL, &btxp->client.channel,btxp->target_uuid, btxp->serv_chn);
                    status = RF_OpenClientChannel(btxp->remoteDevice,
                                                            btxp->serv_chn, 
                                                            &btxp->client.channel, 
                                                            OBEX_PERFORMANCE_MULTIPLIER);                    
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_OPEN_RFCHANNEL_RETURN, status);
            break;
            }
            break;
        case BT_STATUS_CONNECTION_FAILED:            
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:
            if(obex_attribute_id_list[attribute_index] == AID_PROTOCOL_DESC_LIST)
            {
                OS_LockObex();
                BtDisconnectClientLink(btxp);
                OS_UnlockObex();
            }
            else
            {
                kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_OPEN_RFCHANNEL, &btxp->client.channel,btxp->target_uuid, btxp->serv_chn);
                status = RF_OpenClientChannel(btxp->remoteDevice,
                                                        btxp->serv_chn, 
                                                        &btxp->client.channel, 
                                                        OBEX_PERFORMANCE_MULTIPLIER);            
                kal_trace(BT_TRACE_G6_OBEX,OBEX_OPEN_RFCHANNEL_RETURN, status);
            }
            break;
    }

    /* If RF_OpenClientChannel return FAILED, then disconnect ACL link and notify app. */
    if (status != BT_STATUS_PENDING)
    {
        OS_LockObex();
        BtDisconnectClientLink(btxp);
        OS_UnlockObex();
    }
}


#if BT_GOEP_V2 == XA_ENABLED

BtStatus BtClientL2CAPConnect(ObBtClientTransport *btxp, U16 psmValue){
	BtStatus status = BT_STATUS_FAILED;
	ObexClientApp *appHndl;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_OPEN_L2Channel, &btxp->client.channel,btxp->target_uuid, btxp->serv_chn, psmValue);
    
	// configure local psm value
	btxp->client.goepl2cap_psm.psm = OBEXL2GetLocalPsmValue(btxp->target_uuid, 1);
	btxp->client.goepl2cap_psm.callback = BtClientL2CallBack;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
	// register psm to get the protocol
	if( btxp->client.bGoepL2cap != 0 ){
		btxp->client.goepl2cap_psm.localMtu = (L2CAP_MAXIMUM_MTU-3);//675; //L2CAP_MAXIMUM_MTU;
		btxp->client.goepl2cap_psm.minRemoteMtu = L2CAP_MINIMUM_MTU;
		btxp->client.goepl2cap_psm.authorizedSetting = FALSE;	    
        btxp->client.goepl2cap_psm.op_state = L2OPERATORSTATE_STATE1;
        
		btxp->client.goepl2cap_psm.inLinkMode = btxp->client.goepl2cap_psm.outLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION; //L2MODE_BASIC;
		if( NULL != btxp->client.pl2Buffer && btxp->client.pl2Buffer->rxNum > 0 ){
			btxp->client.goepl2cap_psm.inTransmitMax = 100; //btxp->client.pl2Buffer->rxNum; // default value
		}else{
			return BT_STATUS_FAILED;
		}
		btxp->client.goepl2cap_psm.channelUsage = L2CAP_CHANNEL_DEDICATED;
		btxp->client.goepl2cap_psm.inBufferSize = (L2CAP_MAXIMUM_MTU + 7)*24;
		kal_trace(BT_TRACE_G6_OBEX, OBEX_REG_L2CAP_PSM, &btxp->client.goepl2cap_psm , btxp->client.goepl2cap_psm.inLinkMode);
        
#ifdef __BT_2_1_ERTM__    
        btxp->client.goepl2cap_psm.lockStepNeeded = GOEP_USE_HIGHSPEED;
        btxp->client.goepl2cap_psm.highSpeedRequired = GOEP_USE_HIGHSPEED;
        GOEP_Report("OBEX: C locakStepNeeded:%d highSpeedRequired:%d", btxp->client.goepl2cap_psm.lockStepNeeded, btxp->client.goepl2cap_psm.highSpeedRequired);
#endif

        // 110125
        btxp->client.goepl2cap_psm.l2capExtFlowSpec.serviceType = L2QOS_OPTION_BEST_EFFORT;
        btxp->client.goepl2cap_psm.l2capExtFlowSpec.maxSduSize = 670;
        btxp->client.goepl2cap_psm.l2capExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
        btxp->client.goepl2cap_psm.l2capExtFlowSpec.accessLatency = 0xFFFFFFFF;
        btxp->client.goepl2cap_psm.l2capExtFlowSpec.flushTimeout = 0xFFFFFFFF;
        GOEP_Report("OBEX: config l2capExtFlowSpec.serviceType:%d ", btxp->client.goepl2cap_psm.l2capExtFlowSpec.serviceType);
        

		status = L2CAP_RegisterPsm(&btxp->client.goepl2cap_psm) ;
		if ( status != BT_STATUS_SUCCESS){
			kal_trace(BT_TRACE_G6_OBEX, OBEX_REG_L2CAP_PSM_RETURN, status, btxp->client.goepl2cap_psm.psm);
			return status;
		}
        
        // setup the secRecord
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
        InitializeListEntry(&(btxp->client.secRecord.node));
        btxp->client.secRecord.id = SEC_L2CAP_ID;
        btxp->client.secRecord.channel = btxp->client.goepl2cap_psm.psm;
        btxp->client.secRecord.level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
        btxp->client.secRecord.level_type = BT_SEC_LOW;
#else 
        btxp->client.secRecord.level_type = BT_SEC_MED;
#endif 
        GOEP_Report("OBEX: SEC_RegisterRecord level_type:%d psm:0x%x", btxp->client.secRecord.level_type, btxp->client.secRecord.channel);
        SEC_RegisterRecord(&btxp->client.secRecord);
#endif

    }
#endif

#if 1
    status = L2CAP_ConnectReq(&btxp->client.goepl2cap_psm, psmValue, btxp->remoteDevice, 0, &btxp->client.l2ChannelId);
#else
    status = L2CAP_CreateChannelReq(&btxp->client.goepl2cap_psm, psmValue, btxp->remoteDevice, 0, &btxp->client.l2ChannelId);
    GOEP_Report("OBEX: L2CAP_CreateChannelReq status:%d", status );
#endif

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if( BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status )
    {
		L2capConnSettings Setting;

        appHndl = (ObexClientApp *)btxp->client.app;
        Setting.reasmBuffer = (ListEntry *)&(appHndl->l2buffer.rxPacketsBufferList);
        Setting.receivedBuffer = (ListEntry *)&(appHndl->l2buffer.receivedPacketsList);
        Setting.rxWindowSize = appHndl->l2buffer.rxNum;  
        OS_MemSet((U8 *)appHndl->l2buffer.txMaxCountPerIFrame, 0, 128); // Always 128 
        OS_MemSet((U8 *)appHndl->l2buffer.txSrejList, 0, sizeof(appHndl->l2buffer.txSrejList) ); 
        Setting.txMaxTransmitCount = (U8 *)appHndl->l2buffer.txMaxCountPerIFrame;
        Setting.txSrejList = (U8 *)appHndl->l2buffer.txSrejList;
        Setting.maxSrejNo = OBEXL2_L2CAP_MAX_SREJ_SIZE;

        L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, btxp->client.l2ChannelId);
    }else{
		// unregister psm
		L2CAP_DeregisterPsm(&btxp->client.goepl2cap_psm) ;
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)        
        SEC_Unregister(&btxp->client.secRecord);
#endif
	}
#endif                                
    kal_trace(BT_TRACE_G6_OBEX,OBEX_OPEN_L2CHANNEL_RETURN, status);
	return status;
}

static void BtClientL2SdpCallback(
                                                        SdpQueryToken *sqt, 
                                                        U8 result, 
                                                        U8 attribute_index,
                                                        U8 *attribute_value
                                                        )
{
    BtStatus status = BT_STATUS_PENDING;
    ObBtClientTransport *btxp;
    SdpQueryToken *token;
    ObexClientApp *appHndl;
    BtStatus notify_rst;
    U16 psmValue = 0x0001;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_L2SDPCALLBACK, result, attribute_index);

    btxp = ContainingRecord(sqt, ObBtClientTransport, sdpQueryToken);

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_STATE, btxp->client.state);

    token  = sqt;
    if(btxp->client.state == OCS_DISCONNECTING)
    {
        kal_trace(BT_TRACE_G6_OBEX,BTCLIENTSDPCALLBACK_BTXPCLIENTSTATE_L2CAP_OCS_DISCONNECTING);
        OS_LockObex();
        BtDisconnectClientLink(btxp);
        OS_UnlockObex();
        return;
    }
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
            switch (obexv15_attribute_id_list[attribute_index])
            {
                case AID_GOEPL2CAP_PSM:
                    btxp->client.state = OCS_CONNECTING;
                    btxp->serv_chn = attribute_value[0];
                    switch (btxp->target_uuid)
                    {
						case SC_OBEX_OBJECT_PUSH:
                        case SC_OBEX_FILE_TRANSFER:
                        case SC_DIRECT_PRINTING:
                        case SC_PRINTING_STATUS:
                        case SC_IMAGING_RESPONDER:
                    #ifdef __BT_SUPPORT_SYNCML__  
                        case SC_SYNCML_BT_SERVER:
                        case SC_SYNCML_BT_CLIENT:
                    #endif    
                            psmValue = (attribute_value[0] * 0x100)  + attribute_value[1];
							status = BtClientL2CAPConnect(btxp, psmValue);
							if( BT_STATUS_PENDING == status ){
								btxp->client.state == OCS_CONNECTING;
							}

							if( BT_STATUS_SUCCESS != status && BT_STATUS_PENDING != status ){
								
							}
                            break;
                    }
                break;
            }
            break;
        case BT_STATUS_CONNECTION_FAILED:            
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:
            if(obexv15_attribute_id_list[attribute_index] == AID_GOEPL2CAP_PSM)
            {
                /* cannot find the l2cap psm */

                if( btxp->client.bGoepL2cap == 0 || btxp->client.bGoepL2cap == 2 ) { // TODO: day no l2cap found - continue to find rfcomm
                    obex_start_timer(
        	        &btxp->rfSDPTimer, 
        	        10,
        	        BTSTACK_AUTO_GOEPL2SDP_Timeout,
        	        btxp);
                }else{
                    OS_LockObex();
                    BtDisconnectClientLink(btxp);
                    OS_UnlockObex();
                }
            }
            else
            {
                /* others' fail*/
                OS_LockObex();
                BtDisconnectClientLink(btxp);
                OS_UnlockObex();
            }
            break;
    }

    /* If RF_OpenClientChannel return FAILED, then disconnect ACL link and notify app. */
    if (status != BT_STATUS_PENDING)
    {
        OS_LockObex();
        BtDisconnectClientLink(btxp);
        OS_UnlockObex();
    }
}
#endif

/*---------------------------------------------------------------------------
 *            BtStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate the SDP service query.
 *
 * Return:    
 *
 */
static BtStatus BtStartServiceQuery(ObBtClientTransport *btxp)
{
    return BtStartServiceRFCommQuery(btxp);
}

#if BT_GOEP_V2 == XA_ENABLED
/**
 * @brief Search the goepl2cap psm record of specific profile
 */
static BtStatus BtStartServiceL2Query(ObBtClientTransport *btxp)
{
    ObStatus status;
    sdap_service_search_multi_attribute_struct search_pattern;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_START_L2CAP_SERVICE_QUERY);

    btxp->sdpQueryToken.rm = btxp->remoteDevice;
    /* Reset the query flag */
    search_pattern.rm = btxp->remoteDevice;
    search_pattern.sqt = &btxp->sdpQueryToken;
    search_pattern.callback = BtClientL2SdpCallback;
    search_pattern.uuid = (U16) btxp->target_uuid;
    search_pattern.attribute_id = obexv15_attribute_id_list; /* search for different AID */   
    switch(btxp->target_uuid)
    {
        case SC_OBEX_OBJECT_PUSH:
            search_pattern.attribute_num = 0x01;
            break;
        case SC_OBEX_FILE_TRANSFER:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_IMAGING_RESPONDER:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_DIRECT_PRINTING:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_PRINTING_STATUS:
            search_pattern.attribute_num = 0x01;
            break;
#ifdef __BT_SUPPORT_SYNCML__
        case SC_SYNCML_BT_SERVER:
            search_pattern.attribute_num = 0x01;
            break;
        case SC_SYNCML_BT_CLIENT:
            search_pattern.attribute_num = 0x01;
            break;	
#endif
        default:
            search_pattern.attribute_num = 0x01;
            break;
            
    }
    status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);   
    return status;
}
#endif

/**
 * @brief Search the RFComm record of specific profile
 */
static BtStatus BtStartServiceRFCommQuery(ObBtClientTransport *btxp)
{
    ObStatus status;
    sdap_service_search_multi_attribute_struct search_pattern;
#ifdef __BT_SUPPORT_SYNCML__    
    /* For SyncML SDP Search Request */
    U8 syncs_uuid128_array[] = {0x00,0x00,0x00,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x02,0xEE,0x00,0x00,0x02};
    U8 syncc_uuid128_array[] = {0x00,0x00,0x00,0x02,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x02,0xEE,0x00,0x00,0x02};
#endif 

    kal_trace(BT_TRACE_G6_OBEX,OBEX_START_SERVICE_QUERY);

    btxp->sdpQueryToken.rm = btxp->remoteDevice;
    /* Reset the query flag */
    search_pattern.rm = btxp->remoteDevice;
    search_pattern.sqt = &btxp->sdpQueryToken;
    search_pattern.callback = BtClientSdpCallback;
    search_pattern.uuid = (U16) btxp->target_uuid;
    search_pattern.attribute_id = obex_attribute_id_list;
    search_pattern.attribute_num = 0x01;
    switch(btxp->target_uuid)
    {
        case SC_OBEX_OBJECT_PUSH:
            search_pattern.attribute_num = 0x02;       
            break;
        case SC_OBEX_FILE_TRANSFER:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_IMAGING_RESPONDER:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_DIRECT_PRINTING:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_PRINTING_STATUS:
            search_pattern.attribute_num = 0x01;
            search_pattern.attribute_id = obex_bpp_attribute_status_id_list;        
            break;
#ifdef __BT_SUPPORT_SYNCML__
        case SC_SYNCML_BT_SERVER:
            search_pattern.attribute_num = 0x01;
            search_pattern.uuid = SC_SYNCML_BT_SERVER;
            OS_MemCopy(search_pattern.uuid128_array, syncs_uuid128_array, 16);
            break;
        case SC_SYNCML_BT_CLIENT:
            search_pattern.attribute_num = 0x01;
            search_pattern.uuid = SC_SYNCML_BT_CLIENT;
            OS_MemCopy(search_pattern.uuid128_array, syncc_uuid128_array, 16);
            break;	
#endif
        case SC_MAP_SERVER:
            search_pattern.attribute_num = 0x01;       
            break;
        case SC_MAP_MNS_SERVER:
            search_pattern.attribute_num = 0x01;       
            break;

        default:
			kal_trace(BT_TRACE_G6_OBEX,OBEX_RFCOMM_QUERY_UNKNOW, btxp->target_uuid);
			break;
    }
    status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);   
    return status;
}


/*---------------------------------------------------------------------------
 *            BtDisconnectClientLink()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect clients ACL.
 *
 * Return:    void
 *
 */
static BtStatus BtDisconnectClientLink(ObBtClientTransport *btxp)
{
    BtStatus        status;
    ObexDiscReason  reason;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_DISCONNECT_CLIENT_LINK);

    if (btxp->client.state == OCS_DISCONNECTING)
        reason = ODR_USER_REQUEST;
    else 
		reason = ODR_UNKNOWN;

    if (!btxp->remoteDevice) {
        /* We already disconnected, so don't bother failing ME_DisconnectLink */
        
        ASSERT(btxp->client.state <= OCS_IDLE);
        return BT_STATUS_SUCCESS;
    }

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_CMGR_REMOVEDATALINK);
    status = CMGR_RemoveDataLink(&btxp->client.cmgr_hdl);
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
    if (status == BT_STATUS_SUCCESS) {
        btxp->remoteDevice = 0;
        if (btxp->client.state != OCS_IDLE) {

            btxp->client.state = OCS_IDLE;
            OBPROT_Disconnected(btxp->client.app, reason);
        }
    }
    btxp->client.cmgr_hdl.remDev = 0;
    btxp->client.cmgr_hdl.bdc = 0;       

    Assert(status == BT_STATUS_SUCCESS || status == BT_STATUS_IN_PROGRESS || status == BT_STATUS_PENDING);
    return status;
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Stack functions required by parser and command interpreter.
 * These functions are accessed via the ObTransFuncTable.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            BTSTACK_SendTxBuffer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a buffer over the RFCOMM Connection.
 *
 * Return:    Status of the RF_SendData() operation.
 *
 */
static ObStatus BTSTACK_SendTxBuffer(ObexTransportContext con, U8* buff, U16 len)
{
    ObStatus    status;
    ObexPacket *txPacket;
#if BT_GOEP_V2 == XA_ENABLED
    ObBtCommonTransport *btxp;
#endif

    /* check error */
    if (len > BTSTACK_MaxTxSize(con))
    {
        kal_trace(BT_TRACE_G6_OBEX, OBEX_SENDTXBUFFER_LEN_ERROR,len,BTSTACK_MaxTxSize(con));
        return BT_STATUS_FAILED;
    }

    ASSERT(len <= BTSTACK_MaxTxSize(con));

    txPacket = ContainingRecord(buff, ObexPacket, buffer);
    txPacket->btpkt.data = buff;
    txPacket->btpkt.dataLen = len;
    txPacket->btpkt.headerLen = 0;
    txPacket->btpkt.tailLen = 0;
    txPacket->btpkt.tail = 0;
    txPacket->btpkt.priority_type = BTP_FLAG_LOW_PRIPORTY;

#if BT_GOEP_V2 == XA_ENABLED
    btxp = ContainingRecord((RfChannel*)con, ObBtCommonTransport, channel);

    /* Send to L2cap */
    if( btxp->bConnType == GOEP_TP_L2CAP_ONLY ){
        // current connection is l2cap 
#ifdef _DLIGHT_TEST_
        OBEX_TX_COunt++;
        Report(("Dlight OBEX_TX_Count:%d",OBEX_TX_COunt));
        Report(("Dlight L2CAP_SendData:%08x",(txPacket->btpkt)));
#endif
        status = L2CAP_SendData(btxp->l2ChannelId, &(txPacket->btpkt) );
        kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_SENDDATA, status, len, &(txPacket->btpkt), (ObexData.packetnum));
		if( status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS ){
			OBPROT_ReturnTxBuffer(buff);
		}
        return status;
    }
#endif

    
    
    status = RF_SendData((RfChannel *)con, &(txPacket->btpkt));
    if (status != BT_STATUS_PENDING) {
        ASSERT(status != BT_STATUS_SUCCESS);
		kal_trace(BT_TRACE_G6_OBEX, OBEX_RFCOMM_SENDDATA_FAILED, status);
        /* Tx Failed! Put the packet back into the queue. */
        OBPROT_ReturnTxBuffer(buff);
    }

#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED
    if (status == BT_STATUS_PENDING) 
    {
        /* start timer */
        OBPROT_StartTxTimer(con, txPacket);
    }
#endif
    
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CALL_RF_SENDDATA_RETURN, len, status);
    return status;

}

/*---------------------------------------------------------------------------
 *            BTSTACK_DisconnectReq()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the underlying RFCOMM connection.
 *
 * Return:    OB_STATUS_PENDING
 */
static ObStatus BTSTACK_DisconnectReq(ObexTransportContext con)
{
    BtStatus status;
	ObBtCommonTransport *btxp;
	
#if BT_GOEP_V2 == XA_ENABLED

    /* Get the local device address for the transport being used */
    btxp = ContainingRecord((RfChannel*)con, ObBtCommonTransport, channel);
	
    if( btxp->bConnType == GOEP_TP_L2CAP_ONLY ){
		
        status = L2CAP_DisconnectReq(btxp->l2ChannelId);
        kal_trace(BT_TRACE_G6_OBEX, OBEX_L2CAP_DISCONNECT_REQ, btxp->l2ChannelId, status);
		return status;
    }
#endif  	
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CALL_RF_CLOSECHANNEL);


    btxp = ContainingRecord((ObexTransportContext*)con, ObBtCommonTransport, channel);
	if( OCS_DISCONNECTING == btxp->state ){
		return BT_STATUS_PENDING;
	}
	if( OCS_LINK_CONNECT == btxp->state || OCS_SERVICE_QUERY == btxp->state || OCS_CONNECTING == btxp->state){
		btxp->state = OCS_DISCONNECTING;
		return BT_STATUS_PENDING;
	}

    status = RF_CloseChannel((RfChannel *)con);
    kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL_RETURN, status);
    
    return status;
}

/*---------------------------------------------------------------------------
 *            BTSTACK_MaxTxSize()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the maximum transport protocol data unit that can be
 *            sent over the referenced transport connection.
 *
 * Return:    U16
 *            
 */
static U16 BTSTACK_MaxTxSize(ObexTransportContext con)
{
    U32 maxsize;
#if BT_GOEP_V2 == XA_ENABLED
    ObBtCommonTransport *btxp;
    /* Get the local device address for the transport being used */
    btxp = ContainingRecord((RfChannel*)con, ObBtCommonTransport, channel);
    if( btxp->bConnType == GOEP_TP_L2CAP_ONLY ){
        maxsize = min(OBEX_TX_BUFF_SIZE, L2CAP_GetTxMtu(btxp->l2ChannelId) ); // TODO: check this  
        if( maxsize < 40 ){
            GOEP_Report("OBEX: WRN l2cap txmtu:%d", maxsize);
            // 0 is when l2cap is closed !
        }
        return maxsize;
            
    }
#endif    
    // When the rfcomm is closed, the RF_FrameSize is 0.
    if(  RF_FrameSize((RfChannel *)con) <  BT_PACKET_HEADER_LEN ){
	maxsize = 0;
    }else{
        maxsize = min(OBEX_TX_BUFF_SIZE, (RF_FrameSize((RfChannel *)con) - BT_PACKET_HEADER_LEN) );
    }
#if BT_GOEP_V2 == XA_ENABLED
    if( 0 == maxsize ){
        GOEP_Report("OBEX: bConnType:%d maxsize:%d", btxp->bConnType, maxsize);
    }
#endif    
    return maxsize;
}

/*---------------------------------------------------------------------------
 *            BTSTACK_GetTpConnInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieves OBEX transport layer connection information. This
 *            function can be called when a transport connection is active
 *            to retrieve connection specific information. It should be used
 *            in conjunction with the receive flow control API to retrieve 
 *            the minimum amount of application storage space (low water mark)
 *            used when deciding to pause and resume data flow.
 *
 * Return:    TRUE  - The structure was successfully completed.
 *            FALSE - The transport is not connected.
 */
static BOOL BTSTACK_GetTpConnInfo(ObexTransportContext  context,
                                  ObexTpConnInfo   *tpConnInfo)
{
    ObBtCommonTransport *btxp;

#if BT_GOEP_V2 == XA_ENABLED
    /* Get the local device address for the transport being used */
    btxp = ContainingRecord((RfChannel*)context, ObBtCommonTransport, channel);

    if( btxp->bConnType == GOEP_TP_L2CAP_ONLY ){
		
        tpConnInfo->tpType = OBEX_TP_BLUETOOTH;
        tpConnInfo->minRxBuffs = GOEPL2_MAX_RETRANS_NUM;
        tpConnInfo->maxPduSize = L2CAP_GetTxMtu(  btxp->l2ChannelId );
        if( NULL != btxp->cmgr_hdl.bdc ){
            tpConnInfo->remDev = btxp->cmgr_hdl.bdc->link;
        }else{
            tpConnInfo->remDev = NULL;
        }
        if (btxp->state != OCS_CONNECTED) return FALSE;
        tpConnInfo->devAddr = btxp->devAddr.addr;
		return TRUE;
    }
#endif

    tpConnInfo->tpType = OBEX_TP_BLUETOOTH;
    tpConnInfo->minRxBuffs = OBEX_PERFORMANCE_MULTIPLIER;
    tpConnInfo->maxPduSize = RF_FrameSize((RfChannel *)context);
    tpConnInfo->remDev = RF_RemoteDevice((RfChannel *)context); 

    if (tpConnInfo->maxPduSize == 0) {
        /* Estimate the max PDU size when the call fails. */
        tpConnInfo->maxPduSize = RF_MAX_FRAME_SIZE;
    }

    /* Get the local device address for the transport being used */
    btxp = ContainingRecord((RfChannel*)context, ObBtCommonTransport, channel);
    /* We do not have a transport connection */
    if (btxp->state != OCS_CONNECTED) return FALSE;
    tpConnInfo->devAddr = btxp->devAddr.addr;
    return TRUE;
}

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            BTSTACK_IsRxFlowAvailable()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allows the application to determine if a credit based 
 *            receive flow control is available.
 *
 * Return:    TRUE -  Receive flow control is available.
 *            FALSE - Receive flow control is not available.
 */
static BOOL BTSTACK_IsRxFlowAvailable(ObexTransportContext context)
{
#if BT_GOEP_V2 == XA_ENABLED
	ObBtCommonTransport *btxp;
    /* Get the local device address for the transport being used */
    btxp = ContainingRecord((RfChannel*)context, ObBtCommonTransport, channel);
	
    if( btxp->bConnType == GOEP_TP_L2CAP_ONLY ){
        return FALSE; //L2CAP has flow control
    }        
#endif
    return RF_CreditFlowEnabled((RfChannel *)context);
}

/*---------------------------------------------------------------------------
 *            BTSTACK_PauseRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Tells the transport layer to stop the remote device from
 *            sending data by refusing to advance transport credit. Once
 *            this function has been called, no more than the number of 
 *            bytes or packets reported by OBEX_GetTpConnInfo() will be
 *            received. This amount includes any currently indicated data 
 *            (if called during a data indication). Flow can be resumed
 *            with OBEX_ResumeRxFlow().
 *               
 * Return:    void
 *
 */
static void BTSTACK_PauseRxFlow(ObexTransportContext context)
{
    ObBtCommonTransport *btxp;

    btxp = ContainingRecord((RfChannel*)context, ObBtCommonTransport, channel);
    ASSERT(btxp->state == OCS_CONNECTED);
    btxp->flags |= BTF_FLOW;
}

/*---------------------------------------------------------------------------
 *            BTSTACK_ResumeRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens the receive window after flow control was exerted.
 *
 * Return:    void
 *
 */
static void BTSTACK_ResumeRxFlow(ObexTransportContext context)
{
    ObBtCommonTransport *btxp;

    btxp = ContainingRecord((RfChannel*)context, ObBtCommonTransport, channel);
    ASSERT(btxp->state == OCS_CONNECTED);
#if BT_GOEP_V2 == XA_ENABLED
    if( btxp->bConnType == GOEP_TP_L2CAP_ONLY ){
		ASSERT( FALSE );
	}
#endif

    btxp->flags &= ~BTF_FLOW;
    RF_AdvanceCredit(&btxp->channel, btxp->credits);
    btxp->credits = 0;
}
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED

void BTSTACK_TxTimeout(void *con)
{
    BtStatus status;
    ObBtClientTransport *btxp;

    OS_LockObex();

    /* Get the correct Bluetooth transport structure this callback is for */
    btxp = (ObBtClientTransport *)ContainingRecord(con, ObBtCommonTransport, channel);
    
#if BT_GOEP_V2 == XA_ENABLED
    if (btxp->client.state == OCS_CONNECTED && btxp->client.bConnType == GOEP_TP_L2CAP_ONLY )
    {
		
		status = L2CAP_DisconnectReq( btxp->client.l2ChannelId );
		kal_trace(BT_TRACE_G6_OBEX,OBEX_L2CAP_DISCONNECT_REQ, btxp->client.l2ChannelId, status);
		btxp->client.state = OCS_DISCONNECTING;

	}else if(btxp->client.state == OCS_CONNECTED)
	{
#else
    if (btxp->client.state == OCS_CONNECTED)
    {
#endif
        kal_trace(BT_TRACE_G6_OBEX, OBEX_BTSTACK_TXTIMEOUT, con);
    
        kal_trace(BT_TRACE_G6_OBEX, OBEX_CALL_RF_CLOSECHANNEL);

        status = RF_CloseChannel((RfChannel *)con);
        kal_trace(BT_TRACE_G6_OBEX,OBEX_BTSTACK_RFCLOSECHANNEL_RETURN, status);

        btxp->client.state = OCS_DISCONNECTING;
        if (status != BT_STATUS_PENDING) {
            BtDisconnectClientLink(btxp); 
        }

    }

    OS_UnlockObex();
}

#endif /* OBEX_SUPPORT_RFPKT_SEND_TIMER  == XA_ENABLED */


BtStatus BTSTACK_AcceptRFChnConn(ObBtServerTransport *btxp)
{
    BtStatus status;
    RfChannel * Channel;
#if BT_GOEP_V2 == XA_ENABLED
	L2capConnSettings Setting;
#endif

	kal_trace(BT_TRACE_G6_OBEX,	OBEX_ACCEPT_RFCHNL, btxp);

    /* This function is invoked from GOEP module */
#if BT_GOEP_V2 == XA_ENABLED
    
    if( btxp->server.bConnType == GOEP_TP_L2CAP_ONLY ){

#ifdef __BT_2_1_ERTM__  
        btxp->server.goepl2cap_psm.l2capExtFlowSpec.serviceType = L2QOS_OPTION_BEST_EFFORT;
        btxp->server.goepl2cap_psm.l2capExtFlowSpec.maxSduSize = 670;
        btxp->server.goepl2cap_psm.l2capExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
        btxp->server.goepl2cap_psm.l2capExtFlowSpec.accessLatency = 0xFFFFFFFF;
        btxp->server.goepl2cap_psm.l2capExtFlowSpec.flushTimeout = 0xFFFFFFFF;
        GOEP_Report("OBEX: accept serviceType:%d", btxp->server.goepl2cap_psm.l2capExtFlowSpec.serviceType);
#endif
        
        if( NULL == btxp->server.pl2Buffer ){
		 status = L2CAP_ConnectRsp(btxp->server.l2ChannelId, L2CONN_REJECT_NO_RESOURCES, NULL);
		    kal_trace(BT_TRACE_G6_OBEX, OBEX_L2CAP_CONNECT_RSP, btxp->server.l2ChannelId, L2CONN_REJECT_NO_RESOURCES, status );
        }else{
			Setting.reasmBuffer = (ListEntry *)&btxp->server.pl2Buffer->rxPacketsBufferList;
			Setting.receivedBuffer = (ListEntry *)&btxp->server.pl2Buffer->receivedPacketsList;
			Setting.rxWindowSize = btxp->server.pl2Buffer->rxNum;  
			OS_MemSet((U8 *)btxp->server.pl2Buffer->txMaxCountPerIFrame, 0, 128); /* Always 128 */
			OS_MemSet((U8 *)btxp->server.pl2Buffer->txSrejList, 0, sizeof(btxp->server.pl2Buffer->txSrejList) ); 
			Setting.txMaxTransmitCount = (U8 *)btxp->server.pl2Buffer->txMaxCountPerIFrame;
			Setting.txSrejList = (U8 *)btxp->server.pl2Buffer->txSrejList;         
			Setting.maxSrejNo = OBEXL2_L2CAP_MAX_SREJ_SIZE;

			status = L2CAP_ConnectRsp(btxp->server.l2ChannelId, L2CONN_ACCEPTED, &Setting);
			kal_trace(BT_TRACE_G6_OBEX, OBEX_L2CAP_CONNECT_RSP, btxp->server.l2ChannelId, L2CONN_ACCEPTED, status );
		}
		
		return status;
	}
#endif

	
    Channel = &(btxp->server.channel);
    
    status = RF_AcceptChannel(Channel);
    kal_trace(BT_TRACE_G6_OBEX,	OBEX_ACCEPT_RFCHNL_RETURN, status);

    obex_stop_timer(&btxp->rfOpenTimer);
    
	return status;
}

BtStatus BTSTACK_RejectRFChnConn(ObBtServerTransport *btxp)
{
    BtStatus status;
    RfChannel * Channel;

	kal_trace(BT_TRACE_G6_OBEX,	OBEX_REJECT_RFCHNL, btxp);

#if BT_GOEP_V2 == XA_ENABLED
	L2capConnSettings Setting;
#endif

#if BT_GOEP_V2 == XA_ENABLED
    if( btxp->server.bConnType == GOEP_TP_L2CAP_ONLY ){
		status = L2CAP_ConnectRsp(btxp->server.l2ChannelId, L2CONN_REJECT_NO_RESOURCES, NULL); // TODO: which reject code
		return status;
	}
#endif

    Channel = &(btxp->server.channel);

    status = RF_RejectChannel(Channel);

    obex_stop_timer(&btxp->rfOpenTimer);
        
    /* remove CMGR data link */
    kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_REMOVE_DL);
    status = CMGR_RemoveDataLink(&btxp->server.cmgr_hdl);
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CMGR_REMOVEDATALINK_RETURN, status);    
    btxp->server.cmgr_hdl.remDev = 0;
    btxp->server.cmgr_hdl.bdc = 0;                        

	return status;
}

#endif /* BT_STACK == XA_ENABLED */
