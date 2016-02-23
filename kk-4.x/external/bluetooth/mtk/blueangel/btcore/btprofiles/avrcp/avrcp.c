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
 * File:
 *     $Workfile:avrcp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains public API for the Audio/Video
 *     Remote Control Profile (AVRCP).
 *             
 * Created:     Mar 10, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 ****************************************************************************/
#ifdef __BT_AVRCP_PROFILE__

#include "sys/avrcpi.h"
#include "avrcp.h"
#include "btbm.h"
#include "btbm_int.h"
#include "bt_adp_avrcp.h"
#include "avrcp_inc.h"
#include "bluetooth_struct.h"
#include "bluetooth_avrcp_struct.h"

#if XA_CONTEXT_PTR == XA_ENABLED
/* The stack context is accessed through a pointer. Normally in this case
 * the stack context would be dynamically allocated. For testing purposes
 * the pointer points to a global structure.
 */
BtAvrcpContext  avTmp;
BtAvrcpContext *avrcpContext = &avTmp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
BtAvrcpContext avrcpContext;
#endif /* XA_CONTEXT_PTR */

AvrcpCallbackParms g_avrcp_fail_parms;
BD_ADDR g_avrcp_latest_a2dpaddr = {{0}};

extern avrcp_adp_context avrcp_cntx[];

static void AvrcpCmgrCallback(CmgrHandler *cHandler, 
                              CmgrEvent Event, 
                              BtStatus Status);
#if AVRCP_PANEL_SUBUNIT == XA_ENABLED
static BOOL AvrcpHandlePanelCmd(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
static BOOL AvrcpHandlePanelCmdControl(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
static void AvrcpPanelTimerExpired(EvmTimer *timer);
static void AvrcpHandlePanelInput(AvrcpChannel *chnl);
static BOOL AvrcpHandlePanelRsp(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */


void AvrcpAvctpCallback(AvctpChannel *Chnl, AvctpCallbackParms *Parms);

extern void AvrcpStartContinueTimer(AvrcpChannel *chnl);
extern void AvrcpCancelContinueTimer(AvrcpChannel *chnl);
extern void AVRCP_Connect_Timeout(EvmTimer *Timer); // avrcpsdp
BtStatus AVRCP_ConnectRetry(AvrcpChannel *chnl, BD_ADDR *addr);
extern void AVRCP_UitlInternalBrowseDisconnectReq(U8 chnl_num);

AvrcpChannel *AVRCP_UitlGetChannelByChnlNum(U8 chnl_num);

void AvrcpFFTimerSendRelease(AvrcpChannel *chnl);
BOOL AvrcpHandleFFCmd(AvrcpChannel *chnl, AvrcpCallbackParms *parms);

extern U8 A2dpGetState(void);
extern void AvrcpHandleCommandFrameInd(U8 chnl_num, U16 result, U8 seq_id, U8 profile_id, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len,const U8 *frame_data);

/*---------------------------------------------------------------------------
 * AVRCP_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the AVRCP SDK.  This function should only be called
 *            once, normally at initialization time.  The calling of this 
 *            function can be specified in overide.h using the XA_LOAD_LIST 
 *            macro (i.e. #define XA_LOAD_LIST XA_MODULE(AV)).
 *
 * Return:    (See header file)
 */
BOOL AVRCP_Init(void)
{
    BOOL status;

    /* Initialize Memory */
    status = AvrcpAlloc();
    if (status) {
        status = AVCTP_Init();
    }
#ifdef __BT_AVRCP_V14__
    OS_Report("[AVRCP] AVRCP_V14");
#endif
#ifdef __BT_AVRCP_V13__
    OS_Report("[AVRCP] AVRCP_V13");
#endif

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpContinueTimeout
 * DESCRIPTION
 *  Timeout handler for commands.
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpContinueTimeout(EvmTimer *Timer)
{
	AvctpChannel *Chnl = (AvctpChannel*) Timer->context;
	AvrcpChannel *chnl = ContainingRecord(Chnl, AvrcpChannel, chnl);
	
	AvctpCallbackParms parms;

    chnl->txTimer.context = 0;
    chnl->txCmdTimeout = TRUE;
	
	OS_MemSet((U8 *) &parms, 0, sizeof(AvctpCallbackParms));
    
    /* Pass to avctp callback and disconnect the transport*/
	parms.p.cmdFrame = (AvctpCmdFrame *) chnl->curQueueFrame;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONTINUE_TIMEOUT, chnl, chnl->curQueueFrame);
	parms.event = AVRCP_EVENT_TX_ERROR;

	AvrcpAvctpCallback(Chnl, &parms);
}


/*---------------------------------------------------------------------------
 * AvrcpAvctpCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback function for AVCTP.
 *
 * Return:    void
 */
void AvrcpAvctpCallback(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
    AvrcpChannel *chnl = ContainingRecord(Chnl, AvrcpChannel, chnl);
    AvrcpCallbackParms parms;
    AvrcpQueuePacketFrame *packetFrame;
    AvrcpCallbackParms *parms2;
    BtStatus status; 
    U8 bPassToUtilCallback = TRUE;
    BD_ADDR l_addr;
    BTBMRemoteNameSetting_T *deviceName;
    char property_value[16];
    U8 DoRejectCheck = 1;
    U8 a2dpState = 0;

    Report(("[AVRCP] avctpCB AVCTP_EVENT:%d", Parms->event));

    if (Parms->event == AVCTP_EVENT_DISCONNECT) {

#if 1
        // retry for MOTO
        Report(("[AVRCP] avctpCB state:%d retryCount:%d retryFlag:%d", chnl->chnl.state , chnl->chnl.retryCount, chnl->chnl.retryFlag));
        if( TRUE == AvrcpUtilIsRetryConnecting(chnl) ){
            if( chnl->chnl.retryCount < 2){ // First connection doesn't increase the retryCount. retry twice (0,1).
                chnl->chnl.retryCount++;

                // use a timer to do the retry connect
                if( NULL != chnl->cmgrHandler.remDev ){
                    if(chnl->conntimer.func!=0){
    		            EVM_CancelTimer(&chnl->conntimer);
                    }
    		        chnl->conntimer.func = AVRCP_Connect_Timeout;
    		        chnl->conntimer.context = chnl->cmgrHandler.remDev;
    		        EVM_StartTimer(&chnl->conntimer, 50);
                    OS_Report("[AVRCP] retry timer succcess! wait response ");
                    return;
                }
                
            }
        }
#endif
        
        /* Release link handler */
        Report(("AVRCP: removeDataLink in Callback"));
        (void)CMGR_RemoveDataLink(&chnl->cmgrHandler);
        chnl->cmgrHandler.remDev = 0;
        chnl->cmgrHandler.bdc = 0;        
//        (void)CMGR_DeregisterHandler(&chnl->cmgrHandler);
    } else if (Parms->event == AVCTP_EVENT_CONNECT_IND) {
    	
        deviceName = btbmUtilConnectedDeviceNameEntryLookUp(&(Parms->p.remDev->bdAddr));
        a2dpState = A2dpGetState();
        
    	// remote device is "PTS-", ignore the Reject check. if len is 0/not getname back yet, ignore the Reject check
    	if( deviceName != 0 ){
            if( deviceName->dev_name_len > 4 ){
                OS_Report("[AVRCP] Remote 0x%x 0x%x 0x%x 0x%x", deviceName->dev_name[0], deviceName->dev_name[1], deviceName->dev_name[2], deviceName->dev_name[3] );
            }
            if( deviceName->dev_name_len > 0 ){
        	    OS_Report("[AVRCP] Remote is (%s) len:%d a2dpState:%d", deviceName->dev_name, deviceName->dev_name_len, a2dpState);
            }else if( deviceName->dev_name_len == 0 ){
    	        DoRejectCheck = 0 ;
    	        OS_Report("[AVRCP] Remote name is not ready. skip reject-check "); // PTS AVRCP-only 
            }
    	    if( 0 == OS_StrnCmp(deviceName->dev_name, "PTS-", 4) ){
    	        DoRejectCheck = 0 ;
    	        OS_Report("[AVRCP] Remote is PTS. skip reject-check");
    	    }
        }else{

            OS_Report("[AVRCP] warning no remote dev_name");
        }
        
        if( 1 == DoRejectCheck ){
    	  
        /* for ALPS01208574 
         * it may cause IOT issue because some device will not connect avrcp anymore when rejected by us
        */
        if (A2dpGetState() <= 2 && deviceName && 
    	    !strncmp(deviceName->dev_name, "MW600", 5))
        {
    	    Report(("AVRCP: reject connect ind due to a2dp disconnected"));
            AVCTP_ConnectRsp(Chnl, FALSE);
            return;
        }
        }

        
        CMGR_CreateDataLink(&chnl->cmgrHandler, &(Parms->p.remDev->bdAddr));
        if(chnl->cmgrHandler.bdc == 0)
        {
            AVCTP_ConnectRsp(Chnl, FALSE);
            return;
        }
        // PTS TC_TG_CON_BV_02_C save address for TG to establishes the connection
        if( NULL != Parms && NULL != Parms->p.remDev){
            memcpy( &g_avrcp_latest_a2dpaddr, &Parms->p.remDev->bdAddr, sizeof(BD_ADDR));
            memcpy( &l_addr, &Parms->p.remDev->bdAddr, sizeof(BD_ADDR));            
            Report(("AVRCP: avrcp save connected addr 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", l_addr.addr[0], l_addr.addr[1], l_addr.addr[2], l_addr.addr[3], l_addr.addr[4], l_addr.addr[5] ));            
        }
        // (void)AVCTP_ConnectRsp(Chnl, TRUE); // auto accept
//        CMGR_ActivateSniffTimer(&chnl->cmgrHandler);        
    }

    parms.channel = chnl;
    parms.event = Parms->event;
    // AV13 record the current TransId of AVCTP for furthur usage
	if( NULL != Parms && NULL != Parms->channel )  {
		if( AVRCP_EVENT_COMMAND == parms.event ) {
			parms.transId = Parms->channel->avcChnl.acpTransId;
		}else{
			parms.transId = Parms->channel->avcChnl.intTransId;
		}
	}

	/* Assigns the pointer in the "p" union (whatever it is) */
	parms.p.remDev = Parms->p.remDev;

    switch(parms.event) {
        case AVRCP_EVENT_COMMAND:
            AvrcpHandleFFCmd(chnl, &parms);
            break;
    }


#if AVRCP_PANEL_SUBUNIT == XA_ENABLED
    /* Do special panel handling */
    switch(parms.event) {

    case AVRCP_EVENT_CONNECT:
    case AVRCP_EVENT_DISCONNECT:
        EVM_CancelTimer(&chnl->panelTimer);
        if(parms.event == AVRCP_EVENT_CONNECT)
            CMGR_SetDeviceAsMaster(&chnl->cmgrHandler);
        /* Initialize values as necessary */
        if (chnl->role == SC_AV_REMOTE_CONTROL) {
            chnl->panelState = AVRCP_PANEL_STATE_C_IDLE;
            chnl->panel.ct.opRead = 0;
            chnl->panel.ct.opWrite = 0;

            /* Prepare the cmd frame */
            chnl->panel.ct.cmd.ctype = AVCTP_CTYPE_CONTROL;
            chnl->panel.ct.cmd.subunitType = AVRCP_SUBUNIT_PANEL;
            chnl->panel.ct.cmd.subunitId = 0x0;
            chnl->panel.ct.cmd.opcode = AVRCP_OPCODE_PASS_THROUGH;
            chnl->panel.ct.cmd.operandLen = 2;
            chnl->panel.ct.cmd.operands = chnl->panel.ct.data;
            chnl->panel.ct.data[1] = 0;
        }
        else if (chnl->role == SC_AV_REMOTE_CONTROL_TARGET) {
            chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;
            chnl->panel.tg.curOp = AVRCP_POP_INVALID;
        } else {
            Assert(0);
        }

         break;

    case AVRCP_EVENT_RESPONSE:
        /* AV13 internal handle the Panel Cmd */
        if (AvrcpHandlePanelRsp(chnl, &parms)) return;

        /* Give the panel code a chance to send a new command */
        AvrcpHandlePanelInput(chnl);
        break;

    case AVRCP_EVENT_COMMAND:
        /* AV13 internal handle the Panel Cmd */
        if (AvrcpHandlePanelCmd(chnl, &parms)) return;
        break;

    case AVRCP_EVENT_TX_DONE:
        /* AV13 recycle the packet and send the next one
         * Pick the next AVRCP packet or 
         * continue to send the next piece of current packet
         */

        /* Do NOT indicate internal cmd/rsp to app, just quietly
         * consume. 
         */
        if ((parms.p.rspFrame == &chnl->panel.tg.rsp) || 
            (parms.p.cmdFrame == &chnl->panel.ct.cmd)){
            return;
		}
        break;
    }
    
#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */

	switch( parms.event ){
		case AVRCP_EVENT_CONNECT_IND:
			break;
		case AVRCP_EVENT_CONNECT:
    		{
    		    /* Init state */
				chnl->curTxState = AVRCP_TX_IDLE;
				chnl->txCmdTimeout = FALSE;
				chnl->startTimer = FALSE;
			    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UPDATE_TXSTATE , chnl->curTxState);
			    
				while( FALSE == IsListEmpty(&chnl->txPacketList) ){
					packetFrame = (AvrcpQueuePacketFrame *) RemoveHeadList( (ListEntry *)&chnl->txPacketList);
					Assert( NULL != packetFrame);

					/* recycle unsend packeted */
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONNECT_CHECK, packetFrame);
					AvrcpAssert(0);
                        
					parms2 = &g_avrcp_fail_parms;
					OS_MemSet((U8*)parms2, 0, sizeof(AvrcpCallbackParms));

        			parms2->event = AVRCP_EVENT_TX_ERROR;
        			parms2->channel = chnl;
					/* Assigns the pointer in the "p" union (whatever it is) */
					parms2->p.queueFrame = packetFrame;
                
        			chnl->callback(chnl, parms2); // pass it to upper level
				}

			}
			break;
        case AVRCP_EVENT_DISCONNECT:
			{
			    /* Stop Timer */
                AvrcpCancelContinueTimer(chnl);
			    
				if( NULL != chnl->curQueueFrame ){
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVCRCP_DISCONNECT_IN_FRAGEMENT, chnl->curQueueFrame);
					parms2 = &g_avrcp_fail_parms;
					OS_MemSet((U8*)parms2, 0, sizeof(AvrcpCallbackParms));

        			parms2->event = AVRCP_EVENT_TX_ERROR;
        			parms2->channel = chnl;
					/* Assigns the pointer in the "p" union (whatever it is) */
					parms2->p.queueFrame = chnl->curQueueFrame;

        			chnl->callback(chnl, parms2);
				}

				while( FALSE == IsListEmpty(&chnl->txPacketList) ){
					packetFrame = (AvrcpQueuePacketFrame *) RemoveHeadList( (ListEntry *)&chnl->txPacketList);
					Assert( NULL != packetFrame);

					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DISCONNECT_CHECK, packetFrame);
							
                        
					parms2 = &g_avrcp_fail_parms;
					OS_MemSet((U8*)parms2, 0, sizeof(AvrcpCallbackParms));

        			parms2->event = AVRCP_EVENT_TX_ERROR;
        			parms2->channel = chnl;
					/* Assigns the pointer in the "p" union (whatever it is) */
					parms2->p.queueFrame = packetFrame;
					chnl->curQueueFrame = packetFrame;

        			chnl->callback(chnl, parms2);

					chnl->curQueueFrame = NULL;
				}

			}
		break;
	}


    /* AV13 pass it to AVRCP user - AvrcpUtilCallback */
    chnl->callback(chnl, &parms);

    /* AV13 Release the pointer of current QueuePacket */
    switch( parms.event ){
        case AVRCP_EVENT_CONNECT:
			
        case AVRCP_EVENT_DISCONNECT:
			{
				chnl->discFlag = FALSE;
				chnl->connFlag = FALSE;
				chnl->bwsConnFlag = FALSE;
			}
		break;
        case AVRCP_EVENT_TX_ERROR:
        case AVRCP_EVENT_TX_DONE:
			{
							
				if( NULL == chnl->curQueueFrame ){
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TX_FINISH, chnl->curQueueFrame);
					chnl->curTxState = AVRCP_TX_IDLE;
			        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UPDATE_TXSTATE , chnl->curTxState);
			        AvrcpCancelContinueTimer(chnl);
                
					/* AV13 search for the next packet and send it */
					while( FALSE == IsListEmpty(&chnl->txPacketList) ){
						packetFrame = (AvrcpQueuePacketFrame *) RemoveHeadList( (ListEntry *)&chnl->txPacketList);
						Assert( NULL != packetFrame);
						status = AVRCP_SendQueuePacket( chnl, packetFrame);
						kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_NEXT_PACKET, packetFrame, status);
						if( BT_STATUS_PENDING == status ){
							chnl->curTxState = AVRCP_TX_SENDING;
			                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UPDATE_TXSTATE , chnl->curTxState);							
							break; // out of while loop
						}else{
							AvrcpCallbackParms *parms2;
                        
							// fail to send the next packets. this will cause that current packet cannot back to packet pool
        					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_FAIL_NEXT_PACKET, packetFrame);
							parms2 = &g_avrcp_fail_parms;
							OS_MemSet((U8*)parms2, 0, sizeof(AvrcpCallbackParms));

        					parms2->event = AVRCP_EVENT_TX_ERROR;
        					parms2->channel = chnl;
							/* Assigns the pointer in the "p" union (whatever it is) */
							parms2->p.queueFrame = packetFrame;
                        
        					chnl->callback(chnl, parms2);
						}
                    
					}
				}else{
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TX_FINISH_NOT_EMPTY, parms.event, chnl->curQueueFrame);
					if( chnl->txOffset == chnl->curQueueFrame->data_len ){
						AvrcpAssert(0);
					}
				}
			}
			break;

		
	}
}



/*---------------------------------------------------------------------------
 * AvrcpAvctpCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback function for AVCTP.
 *
 * Return:    void
 */
static void AvrcpBrowseAvctpCallback(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
    U8 is_internal = FALSE;
    AvrcpChannel *chnl = ContainingRecord(Chnl, AvrcpChannel, bws_chnl);
    AvrcpCallbackParms parms;
	AvrcpQueuePacketFrame *packetFrame;
    AvrcpCallbackParms *parms2;	

	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_AVCTP_CALLBACK, Parms->event );
	OS_MemSet( (U8 *) &parms, 0, sizeof(AvrcpCallbackParms));

    parms.channel = chnl;
    parms.event = Parms->event;
    // AV13 record the current TransId of AVCTP for furthur usage
    if( AVRCP_EVENT_COMMAND == parms.event ) {
        parms.transId = Parms->channel->avcChnl.acpTransId;
    }else{
        parms.transId = Parms->channel->avcChnl.intTransId;
    }

    /* Assigns the pointer in the "p" union (whatever it is) */
    parms.p.remDev = Parms->p.remDev;
    
    /* Do special panel handling */
    switch(parms.event) {
        case AVRCP_EVENT_CONNECT:
			{
				chnl->curBrowseTxState = AVRCP_TX_IDLE;
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_UPDATE_TXSTATE , chnl->curBrowseTxState);
				while( FALSE == IsListEmpty(&chnl->txBrowsePacketList) ){
					packetFrame = (AvrcpQueuePacketFrame *) RemoveHeadList( (ListEntry *)&chnl->txBrowsePacketList);
					Assert( NULL != packetFrame);

					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_CONNECT_CHECK, packetFrame);
					parms2 = &g_avrcp_fail_parms;
					OS_MemSet((U8*)parms2, 0, sizeof(AvrcpCallbackParms));

        			parms2->event = AVRCP_EVENT_TX_ERROR;
        			parms2->channel = chnl;
					/* Assigns the pointer in the "p" union (whatever it is) */
					parms2->p.queueFrame = packetFrame;
                
        			chnl->bws_callback(chnl, parms2);					
				}
			}
			break;
        case AVRCP_EVENT_DISCONNECT:
            {
    			chnl->curBrowseTxState = AVRCP_TX_IDLE;
                chnl->bwsConnFlag = FALSE;
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_UPDATE_TXSTATE , chnl->curBrowseTxState);
				
    			if( TRUE == chnl->discFlag ){
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DISC_FLAG_IS );
    			    AVCTP_Disconnect( &chnl->chnl );
    			}

			    // retrieve unsend packet 
				while( FALSE == IsListEmpty(&chnl->txBrowsePacketList) ){
					packetFrame = (AvrcpQueuePacketFrame *) RemoveHeadList( (ListEntry *)&chnl->txBrowsePacketList);
					Assert( NULL != packetFrame);

					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_AUTO_ACCEPT, packetFrame);
                        
					parms2 = &g_avrcp_fail_parms;
					OS_MemSet((U8*)parms2, 0, sizeof(AvrcpCallbackParms));

        			parms2->event = AVRCP_EVENT_TX_ERROR;
        			parms2->channel = chnl;
					/* Assigns the pointer in the "p" union (whatever it is) */
					parms2->p.queueFrame = packetFrame;
                
        			chnl->bws_callback(chnl, parms2);
				}
            }
			break;
        case AVRCP_EVENT_CONNECT_IND:
            is_internal = TRUE; // auto accept the incoming connection

            if( AVCTP_CHAN_STATE_IDLE != chnl->chnl.conn.state ){
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_AUTO_ACCEPT);
                AVCTP_ConnectRsp( &chnl->bws_chnl, TRUE);
            }else{
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_REJECT_CONNECTION, chnl->chnl.conn.state);
                AVCTP_ConnectRsp( &chnl->bws_chnl, TRUE);
            }
			break;
        case AVRCP_EVENT_OPERANDS:
            /// this should not happend ! no handle this case (ignore it)!
            is_internal = TRUE;
            
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_FRAGMENT_PACKET_WRN);
			break;
        case AVRCP_EVENT_COMMAND:
        	if( FALSE  != Parms->p.cmdFrame->more ){
                is_internal = TRUE;
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_IGNORE_FRAGMENT);
        	}
        	/// otherwise, pass it to user for handlng.
        	break;
        case AVRCP_EVENT_RESPONSE:
        case AVRCP_EVENT_REJECT:
        	if( FALSE != Parms->p.rspFrame->more ){
                is_internal = TRUE;
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_IGNORE_FRAGMENT);        	    
        	}
			/// otherwise, pass it to user for handlng.
			break;
        case AVRCP_EVENT_TX_DONE:
        case AVRCP_EVENT_TX_ERROR:
            /// pass it to user for handlng.
            {
		    chnl->curBrowseTxState = AVRCP_TX_IDLE;
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_UPDATE_TXSTATE , chnl->curBrowseTxState);
			}
			break;
    }

    /* AV13 pass it to AVRCP user - AvrcpUtilCallback */
    if( TRUE != is_internal && NULL != chnl->bws_callback ){
        chnl->bws_callback(chnl, &parms);	
    }

	switch(parms.event){
        case AVRCP_EVENT_TX_DONE:
        case AVRCP_EVENT_TX_ERROR:
            /// pass it to user for handlng.
            {
				AvrcpQueuePacketFrame *packetFrame;
				BtStatus status;

                /// select to next  packet
                if( NULL == chnl->curBrowseQueueFrame ){
                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_TX_FINISH, chnl->curBrowseQueueFrame);
                    chnl->curBrowseTxState = AVRCP_TX_IDLE;
                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_UPDATE_TXSTATE , chnl->curBrowseTxState);                    
                    
                    /* AV13 search for the next packet and send it */
                    while( FALSE == IsListEmpty(&chnl->txBrowsePacketList) ){
                        packetFrame = (AvrcpQueuePacketFrame *) RemoveHeadList( (ListEntry *)&chnl->txBrowsePacketList);
                        Assert( NULL != packetFrame);
                        status = AVRCP_SendBrowseQueuePacket( chnl, packetFrame);
                        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_NEXT_PACKET, packetFrame, status);
                        if( BT_STATUS_PENDING == status ){
                            chnl->curBrowseTxState = AVRCP_TX_SENDING;
            				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_UPDATE_TXSTATE, chnl->curBrowseTxState);                            
                            break; // out of while loop
                        }else{
                            AvrcpCallbackParms parms2;
                            
                            // fail to send the next packets. this will cause that current packet cannot back to packet pool
            				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_FAIL_NEXT, packetFrame);
            				parms2.event = AVRCP_EVENT_TX_ERROR;
            				parms2.channel = chnl;
                            /* Assigns the pointer in the "p" union (whatever it is) */
                            parms2.p.queueFrame = packetFrame;
                            
            				chnl->callback(chnl, &parms2);
                        }
                        
                    }
				}else{
				    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_NO_CLEANUP_PACKET, chnl->curBrowseQueueFrame);
				    AvrcpAssert(0);
				}
            }
            break;
	}
}    

/*---------------------------------------------------------------------------
 * AVRCP_Register()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers an application callback to create/receive AVRCP 
 *            connections.  This function must be called before any other 
 *            AVRCP functions.
 *
 * Return:    (See header file)
 */
BtStatus AVRCP_Register(AvrcpChannel *chnl, AvrcpCallback callback, 
                        AvrcpRole role)
{
    BtStatus status, bsResult;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, callback != 0);

    OS_Report("AVRCP register activeVersion:%d", chnl->activeVersion);

    OS_LockStack();
    chnl->conntimer.func =0;
    status = AVCTP_Register(&chnl->chnl, AvrcpAvctpCallback, BT_PSM_AVCTP);
    if (status == BT_STATUS_SUCCESS) {
        chnl->role = role;
        chnl->callback = callback;
        OS_MemSet((U8*)&chnl->cmgrHandler, 0, sizeof(CmgrHandler));
        /* Register with SDP */
        status = AvrcpRegisterSdp(chnl, chnl->activeVersion);
        if (status != BT_STATUS_SUCCESS) {
            AVCTP_Deregister(&chnl->chnl);
        }
        CMGR_RegisterHandler(&chnl->cmgrHandler, AvrcpCmgrCallback);

        bsResult = AVCTP_Register(&chnl->bws_chnl, AvrcpBrowseAvctpCallback, BT_PSM_AVCTP_BROWSING);
        if( BT_STATUS_SUCCESS != bsResult ){
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_REGISTER_FAIL, bsResult);
        }
        
        // init pack list
        InitializeListHead(&chnl->txPacketList);
        chnl->curQueueFrame = NULL;
        chnl->QueuetxState = 0;
        chnl->curTxState = AVRCP_TX_IDLE;
        chnl->curBrowseTxState = AVRCP_TX_IDLE;
        
        chnl->discFlag = FALSE;
        chnl->connFlag = FALSE;

        InitializeListHead(&chnl->txBrowsePacketList);

        chnl->txTimeout = 10000;
        chnl->startTimer = FALSE;

        chnl->ffKey = 0;
        chnl->ffInterceptRsp = 0;

#if AVRCP_PANEL_SUBUNIT == XA_ENABLED
        chnl->panelTimer.func = AvrcpPanelTimerExpired;
        chnl->panelTimer.context = chnl;
#endif
    }

    OS_UnlockStack();

    return status;
}

BtStatus AVRCP_RegisterBrowse(AvrcpChannel *chnl, AvrcpCallback callback, 
                        AvrcpRole role){
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, callback != 0);

    OS_LockStack();
    status = BT_STATUS_SUCCESS;

    chnl->bws_callback = callback;
    //status = AVCTP_Register(&chnl->bws_chnl, AvrcpBrowseAvctpCallback, BT_PSM_AVCTP_BROWSING);
    //if (status != BT_STATUS_SUCCESS) {
    //    chnl->bws_callback = NULL;    
    //}
    
    OS_UnlockStack();

    return status;

}
                        

/*---------------------------------------------------------------------------
 * AVRCP_Deregister()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  De-registers the AVRCP callback. After making this call
 *            successfully, the callback specified in AVRCP_Register will
 *            receive no further events.
 *
 * Return:    (See header file)
 */
BtStatus AVRCP_Deregister(AvrcpChannel *chnl)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();
    (void)CMGR_DeregisterHandler(&chnl->cmgrHandler);
    status = AVCTP_Deregister(&chnl->chnl);
    if (status == BT_STATUS_SUCCESS) {
        /* Deregister SDP */
        AvrcpDeregisterSdp(chnl);
		status = AVCTP_Deregister(&chnl->bws_chnl);
    }

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Connect to the remote device.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_Connect(AvrcpChannel *chnl, BD_ADDR *addr)
{
    BtStatus status;
    BD_ADDR l_localaddr = {{0}};
    
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();
    
    if( 0 == memcmp(addr, &l_localaddr, sizeof(BD_ADDR)) ){
        Report(("AVRCP: connect to latest addr 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5] ));
        memcpy( addr, &g_avrcp_latest_a2dpaddr, sizeof(BD_ADDR));
    }
 
    /* Create a link */
    status = AvrcpCreateAclLink(addr, &chnl->cmgrHandler, AvrcpCmgrCallback);
    if( BT_STATUS_PENDING == status ){
        chnl->connFlag = TRUE;
    }

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Connect to the remote device.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_ConnectRetry(AvrcpChannel *chnl, BD_ADDR *addr)
{
    BtStatus status = BT_STATUS_FAILED;
    BD_ADDR l_localaddr = {{0}};
    
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();
    if( NULL != chnl->cmgrHandler.remDev){
        status = AVCTP_Connect(&chnl->chnl, (BtRemoteDevice *)chnl->cmgrHandler.remDev);
    }
    OS_Report("[AVRCP] AVRCP_ConnectRetry status:%d remDev:0x%x", status, chnl->cmgrHandler.remDev);
    OS_UnlockStack();
 
    return status;
}    


/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Connect to the remote device.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_ConnectBrowse(AvrcpChannel *chnl)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    if( NULL == chnl->chnl.conn.remDev ){
        return BT_STATUS_FAILED;
    }

    OS_LockStack();
 
    /* Create a browse link */
	status = AVCTP_Connect(&chnl->bws_chnl, (BtRemoteDevice *) chnl->chnl.conn.remDev);

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Connect confirm to the remote device.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_ConnectRsp(AvrcpChannel *chnl, BOOL Accept)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    status = AVCTP_ConnectRsp(&chnl->chnl, Accept);

    OS_UnlockStack();

    return status;    
}

/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Connect confirm to the remote device.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_ConnectBrowseRsp(AvrcpChannel *chnl, BOOL Accept)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    status = AVCTP_ConnectRsp(&chnl->bws_chnl, Accept);

    OS_UnlockStack();

    return status;    
}

/*---------------------------------------------------------------------------
 * AVRCP_Disconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Disconnect from the remote device.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_Disconnect(AvrcpChannel *chnl)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if( AVCTP_STATE_DISCONNECTED != chnl->bws_chnl.conn.state ){
        chnl->discFlag = TRUE;
        status = AVCTP_Disconnect(&chnl->bws_chnl);
    }else{
        /* Release link handler */    
        status = AVCTP_Disconnect(&chnl->chnl);
    }

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVRCP_Disconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Disconnect the browse channel only
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_DisconnectBrowse(AvrcpChannel *chnl)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    /* Release link handler */
    status = AVCTP_Disconnect(&chnl->bws_chnl);
    
    OS_UnlockStack();

    return status;
}

U8 AVRCP_IsConnected(AvrcpChannel *chnl){

    U8 status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if( AVCTP_STATE_CONNECTED == chnl->chnl.conn.state ){
        status = TRUE;
    }else{
        status = FALSE;
    }    
    
    OS_UnlockStack();

    return status;
}

U8 AVRCP_GetConnectedAddr(AvrcpChannel *chnl, U8 *addr){

    U8 status = FALSE;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if( AVCTP_STATE_CONNECTED == chnl->chnl.conn.state ){
        if( NULL != addr && NULL != chnl  && NULL != chnl->cmgrHandler.remDev ){
            memcpy( addr, chnl->cmgrHandler.remDev->bdAddr.addr, sizeof(BD_ADDR) );
            status = TRUE;
        }            
    }else{
        status = FALSE;
    }    
    
    OS_UnlockStack();

    return status;
}



U8 AVRCP_IsBrowseExisted(AvrcpChannel *chnl){

    U8 status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if( AVCTP_STATE_CONNECTED == chnl->bws_chnl.conn.state ){
        status = TRUE;
    }else{
        status = FALSE;
    }    
    
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVRCP_SendCommand()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Send a command to the target.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_SendCommand(AvrcpChannel *chnl, AvrcpCmdFrame *cmdFrame)
{
    BtStatus status;

//    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (chnl != 0) &&
//        (chnl->role == SC_AV_REMOTE_CONTROL));

    OS_LockStack();

    status = AVCTP_SendCommand(&chnl->chnl, cmdFrame);

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVRCP_SendResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Send a response to the controller.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_SendResponse(AvrcpChannel *chnl, AvrcpRspFrame *rspFrame)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    status = AVCTP_SendResponse(&chnl->chnl, rspFrame);

    OS_UnlockStack();

    return status;
}

U16 AvrcpGetSinglePacketMtu(AvctpChannel *chnl, U8 type){
    U16 u2MtuPayload = 256;
    
    switch( type ){
        case AVRCP_CMD_FORMAT_PASS_THROUGH:
        case AVRCP_CMD_FORMAT_AVRCP_SPECIFIC:
            u2MtuPayload = AVCTP_GetSinglePacketMtu( chnl, TRUE);
            // fix size
            if( u2MtuPayload > 512 ){
                u2MtuPayload = 512; // AV/C header maximun is 512
            }

            // remove the header
            if( 0 == u2MtuPayload ){
                u2MtuPayload = 48 - 10;
            }else{
                u2MtuPayload -=  10; // avctp header + av/c header + avrcp header
            }
        break;
        case AVRCP_CMD_FORMAT_BROWSE:
            u2MtuPayload = AVCTP_GetSinglePacketMtu( chnl, FALSE);
            u2MtuPayload -= 3; //browse header
        break;
        default:
        break;
    }
    return u2MtuPayload;
}

/**
 * AV13 
 * Compose a avctp rsp frame by the content of QueuePacket
 * u2DataOffset is the data length that has been sent.
 * u2Len:
 *       The txOffset shift length
 */
U16 AvrcpComposeSingleAVCTPCmdFrame(AvrcpChannel *chnl, AvrcpQueuePacketFrame *qPacket, AvrcpCmdFrame *avctpCmd, U16 u2DataOffset){
    U16 u2Len = 0, u2MtuPayload;
    U8 ptype;
    
	if( NULL == qPacket || NULL == avctpCmd ){
		return 0;
	}
    Assert( (qPacket->data_len >= u2DataOffset) );
	
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_COMPOSE_SINGLE_CMD, qPacket->av_ctype, u2DataOffset, qPacket->cmd_type);

    //	AVCTP header
	avctpCmd->ctype = qPacket->av_ctype; // control(0) or status(1)
	avctpCmd->subunitType = 9; //PANEL AVRCP_SUBUNITTYPE_PANEL_VALUE
	avctpCmd->subunitId = 0;

    switch( qPacket->cmd_type ){
        case AVRCP_CMD_FORMAT_PASS_THROUGH:
            {
                u2MtuPayload = AvrcpGetSinglePacketMtu( &(chnl->chnl), AVRCP_CMD_FORMAT_PASS_THROUGH); // AVCTP channel always uses AV/C header
                if( 0 != u2DataOffset || (avctpCmd->operandLen-1) > u2MtuPayload ){
                    // error! pass through is large then a avctp packet
                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_PASSTHRU_SIZE, u2DataOffset, avctpCmd->operandLen, u2MtuPayload);
                }
                
                avctpCmd->opcode = qPacket->av_opcode;
                avctpCmd->operandLen = (U16) qPacket->data_len;
                avctpCmd->operands = &qPacket->data[0];
                avctpCmd->more = FALSE;
				Assert( ((avctpCmd->operandLen-1) <= u2MtuPayload) );

                u2Len = avctpCmd->operandLen; //1 is the opcode
            }
        break;
        case AVRCP_CMD_FORMAT_AVRCP_SPECIFIC:
            {
                // compose a start/continue/end packet for avrcp specific packet
                // if 0 == offset && len <= u2mtu -> avrcp single type
                // if 0 == offset && len > u2mtu -> avrcp start type
                // if 0 != offset && remain len > u2mtu -> avrcp continue end type
                // if 0 != offset && remain len <= u2mtu -> avrcp end type

                u2MtuPayload = AvrcpGetSinglePacketMtu( &(chnl->chnl), AVRCP_CMD_FORMAT_AVRCP_SPECIFIC); // AVCTP channel always uses AV/C header
                Assert( (10+u2MtuPayload) <= 512 );
                if( 0 == u2DataOffset ){
                    if( u2MtuPayload > (qPacket->data_len) ){
                        ptype = AVRCP_PACKET_TYPE_SINGLE;
						u2Len = qPacket->data_len;
                    }else{
                        ptype = AVRCP_PACKET_TYPE_START;
						u2Len = u2MtuPayload;
                    }
                }else{
                    if( (qPacket->data_len-u2DataOffset) > u2MtuPayload ){
                        ptype = AVRCP_PACKET_TYPE_CONTINUE;
						u2Len = u2MtuPayload;
                    }else{
                        ptype = AVRCP_PACKET_TYPE_END;
						u2Len = qPacket->data_len-u2DataOffset;
                    }
                }
                
                avctpCmd->opcode = qPacket->av_opcode; // vendor code
                avctpCmd->operands = chnl->txBuffer;
                /// copy the avctp specific header into operands
                avctpCmd->operands[0] = 0x00; //0x001958 BT SIG company id
                avctpCmd->operands[1] = 0x19;
                avctpCmd->operands[2] = 0x58;
                avctpCmd->operands[3] = qPacket->pdu_id;// PDU_ID
                avctpCmd->operands[4] = ptype; // ptype
                avctpCmd->operands[5] = (U8)((u2Len) & 0xff00)>>8;
                avctpCmd->operands[6] = (U8)((u2Len) & 0x00ff);
                OS_MemCopy( &avctpCmd->operands[7], &qPacket->data[u2DataOffset], u2Len);
                
                avctpCmd->operandLen = u2Len + 7;
                avctpCmd->more = FALSE;

           }
        break;
        case AVRCP_CMD_FORMAT_BROWSE:
            {
                Assert(0);
            }
        break;
        default:
            // 
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_QUEUE_PACKET_TYPE, qPacket->cmd_type);
        break;
    }
	
	
	return u2Len;
}

/**
 * AV13 
 * Compose a avctp rsp frame by the content of QueuePacket
 * u2DataOffset is the data length that has been sent.
 * 
 */
U16 AvrcpComposeSingleAVCTPRspFrame(AvrcpChannel *chnl, AvrcpQueuePacketFrame *qPacket, AvrcpRspFrame *avctpRsp, U16 u2DataOffset){
    U16 u2Len = 0, u2MtuPayload;
    U8 ptype;
    
	if( NULL == qPacket || NULL == avctpRsp ){
		return 0;
	}
    Assert( (qPacket->data_len >= u2DataOffset) );
	
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_COMPOSE_SINGLE_RSP, qPacket->av_ctype, u2DataOffset, qPacket->cmd_type);

    //	AVCTP header
	avctpRsp->response = qPacket->av_ctype; // control(0) or status(1)
	avctpRsp->subunitType = 9; //PANEL AVRCP_SUBUNITTYPE_PANEL_VALUE
	avctpRsp->subunitId = 0;

    switch( qPacket->cmd_type ){
        case AVRCP_CMD_FORMAT_PASS_THROUGH:
            {
                u2MtuPayload = AvrcpGetSinglePacketMtu( &(chnl->chnl), AVRCP_CMD_FORMAT_PASS_THROUGH); // AVCTP channel always uses AV/C header
                if( 0 != u2DataOffset || (avctpRsp->operandLen-1) > u2MtuPayload ){
                    // error! pass through is large then a avctp packet
                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_PASSTHRU_SIZE, u2DataOffset, avctpRsp->operandLen, u2MtuPayload);
                }
                
                avctpRsp->opcode = qPacket->av_opcode;
                avctpRsp->operandLen = (U16) qPacket->data_len;
                avctpRsp->operands = &qPacket->data[0];
                avctpRsp->more = FALSE;
				Assert( ((avctpRsp->operandLen-1) <= u2MtuPayload) );

                u2Len = avctpRsp->operandLen; // index 0 in array is the opcode
            }
        break;
        case AVRCP_CMD_FORMAT_AVRCP_SPECIFIC:
            {
                // compose a start/continue/end packet for avrcp specific packet
                // if 0 == offset && len <= u2mtu -> avrcp single type
                // if 0 == offset && len > u2mtu -> avrcp start type
                // if 0 != offset && remain len > u2mtu -> avrcp continue end type
                // if 0 != offset && remain len <= u2mtu -> avrcp end type

                u2MtuPayload = AvrcpGetSinglePacketMtu( &(chnl->chnl), AVRCP_CMD_FORMAT_AVRCP_SPECIFIC); // AVCTP channel always uses AV/C header
                Assert( (10+u2MtuPayload) <= 512 );
                if( 0 == u2DataOffset ){
                    if( u2MtuPayload > (qPacket->data_len) ){
                        ptype = AVRCP_PACKET_TYPE_SINGLE;
						u2Len = qPacket->data_len;
                    }else{
                        ptype = AVRCP_PACKET_TYPE_START;
						u2Len = u2MtuPayload;
                    }
                }else{
                    if( (qPacket->data_len-u2DataOffset) > u2MtuPayload ){
                        ptype = AVRCP_PACKET_TYPE_CONTINUE;
						u2Len = u2MtuPayload;
                    }else{
                        ptype = AVRCP_PACKET_TYPE_END;
						u2Len = qPacket->data_len-u2DataOffset;
                    }
                }
                
                avctpRsp->opcode = qPacket->av_opcode; // vendor code
                avctpRsp->operands = chnl->txBuffer;
                /// copy the avctp specific header into operands
                avctpRsp->operands[0] = 0x00; //0x001958 BT SIG company id
                avctpRsp->operands[1] = 0x19;
                avctpRsp->operands[2] = 0x58;
                avctpRsp->operands[3] = qPacket->pdu_id;// PDU_ID
                avctpRsp->operands[4] = ptype; // ptype
				StoreBE16( (U8 *) &(avctpRsp->operands[5]), u2Len);

                OS_MemCopy( &avctpRsp->operands[7], &qPacket->data[u2DataOffset], u2Len);
                
                avctpRsp->operandLen = u2Len + 7;
                avctpRsp->more = FALSE;

           }
        break;
        case AVRCP_CMD_FORMAT_BROWSE:
            {
                Assert(0);
#if 0                
                u2MtuPayload = AvrcpGetSinglePacketMtu( &(chnl->chnl), AVRCP_CMD_FORMAT_BROWSE); // AVCTP channel always uses AV/C header
                // browse header: PDU-ID(1B) Length(2B)
                avctpRsp->opcode = qPacket->av_opcode; // vendor code
                avctpRsp->operands = chnl->txBuffer;
                
                avctpRsp->operands[0] = qPacket->pdu_id;
				StoreBE16( (U8 *) &(avctpRsp->operands[1]), u2Len);

                OS_MemCopy( &avctpRsp->operands[3], &qPacket->data[1], qPacket->data_len-1);
                
                avctpRsp->operandLen = (U16)(3 + (qPacket->data_len - 1));
                
                Assert( u2MtuPayload >= (qPacket->data_len -1) );

				u2Len = (U16)qPacket->data_len + 1; // 1 is opcode
#endif				
            }
        break;
        default:
            // 
           	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_QUEUE_PACKET_TYPE, qPacket->cmd_type);
        break;
    }
	
	
	return u2Len;
}

/**
 * AV13 
 * Qeueu a queue packet for sending out
 * 
 */
BtStatus AVRCP_SendQueuePacket(AvrcpChannel *chnl, AvrcpQueuePacketFrame *packetFrame){
    BtStatus status;
    U16 payload_mtu;
    U8 type;

    OS_LockStack();
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_QUEUE_PACKET, chnl->chnl.txState, packetFrame->data_len);

    if( packetFrame->cmd_type != 0 && packetFrame->cmd_type != 1 && packetFrame->cmd_type != 3  ){
        AvrcpAssert(0);
    }

    if( NULL == chnl->curQueueFrame && chnl->curTxState == AVRCP_TX_IDLE ){
        Assert( chnl->curQueueFrame == NULL );
        
        // convert to avctp packet and send it out
        chnl->curQueueFrame = packetFrame;
        chnl->txOffset = 0;
        payload_mtu = AvrcpGetSinglePacketMtu( &chnl->chnl, AVRCP_CMD_FORMAT_AVRCP_SPECIFIC); // AVCTP channel always uses AV/C header

        if( payload_mtu > packetFrame->data_len ){
            // single avrcp packet
            type = AVRCP_PACKET_TYPE_SINGLE;
        }else{
            // split to multi avrcp packets
            type = AVRCP_PACKET_TYPE_START;
        }

        if( packetFrame->is_cmd ){
            // check the tx packet's size
            if( AVRCP_PACKET_TYPE_SINGLE != type ){
                // exceed the max tx cmd size
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CMD_TYPE_NOT_SINGLE, type);
                status = BT_STATUS_FAILED;
                chnl->curQueueFrame = NULL;
            }else{
                chnl->txOffset += AvrcpComposeSingleAVCTPCmdFrame( chnl, chnl->curQueueFrame, &chnl->curTx.avrcpCmd , chnl->txOffset);
				chnl->chnl.avcChnl.intTransId = chnl->curQueueFrame->transId;
                status = AVCTP_SendCommand(&chnl->chnl, &chnl->curTx.avrcpCmd );        
            }
        }else{
            chnl->txOffset += AvrcpComposeSingleAVCTPRspFrame( chnl, chnl->curQueueFrame, &chnl->curTx.avrcpRsp , chnl->txOffset);
            chnl->chnl.avcChnl.acpTransId = chnl->curQueueFrame->transId;
            chnl->curTx.avrcpRsp.transId = chnl->curQueueFrame->transId;
            chnl->curTx.avrcpRsp.subunitType = chnl->curQueueFrame->av_addr >> 3;
            chnl->curTx.avrcpRsp.subunitId = chnl->curQueueFrame->av_addr & 0x7;   
            if( 0 == chnl->curTx.avrcpRsp.subunitType ){
                chnl->curTx.avrcpRsp.subunitType = 0x48 >> 3; // AVRCP uses PANEL subunittype
            }
            Report(("avrcp: sbunit type:%d id%d", chnl->curTx.avrcpRsp.subunitType, chnl->curTx.avrcpRsp.subunitId));
            status = AVCTP_SendResponse(&chnl->chnl, &chnl->curTx.avrcpRsp);
            
            if( AVRCP_PACKET_TYPE_SINGLE != type ){
                /* Start Timer to check the fragmented response */
                AvrcpStartContinueTimer(chnl);              
            }
        }

        if( BT_STATUS_PENDING == status ){
            chnl->curTxState = AVRCP_TX_SENDING;
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UPDATE_TXSTATE , chnl->curTxState);            
        }else{
            // FAIL
            chnl->curQueueFrame = NULL;
        }
    }else{
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PUT_PACKET_TO_QUEUELIST, packetFrame);
        // append queue the packet and return as pending
        InsertHeadList( &chnl->txPacketList, (ListEntry *) packetFrame);
        status = BT_STATUS_PENDING;
    }

    OS_UnlockStack();
    
    // return as error
    return status;
}


/**
 * AV13 
 * Qeueu a queue packet for sending out
 * 
 */
BtStatus AVRCP_ContinueQueuePacket(AvrcpChannel *chnl){
    BtStatus status;
    U16 payload_mtu;
    U8 type;
    AvrcpQueuePacketFrame *packetFrame;

    OS_LockStack();
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONTINUE_QUEUE_PACKET, chnl->chnl.txState);

    if( chnl->curTxState == AVRCP_TX_SENDING ){
        packetFrame = chnl->curQueueFrame;
        // convert to avctp packet and send it out
        payload_mtu = AVCTP_GetSinglePacketMtu( &chnl->chnl, TRUE); // AVCTP channel always uses AV/C header

        if( payload_mtu > (packetFrame->data_len - chnl->txOffset) ){
            // end packet
            type = AVRCP_PACKET_TYPE_END;
        }else{
            // continue packet
            type = AVRCP_PACKET_TYPE_CONTINUE;
        }

        if( packetFrame->is_cmd ){
            status = BT_STATUS_FAILED;
            // NOT support command is large then MTU - should not invoked this function
            AvrcpAssert(0);
        }else{
            chnl->txOffset += AvrcpComposeSingleAVCTPRspFrame( chnl, chnl->curQueueFrame, &chnl->curTx.avrcpRsp , chnl->txOffset);
            status = AVCTP_SendResponse(&chnl->chnl, &chnl->curTx.avrcpRsp);        
        }

        if( BT_STATUS_PENDING == status ){
            chnl->curTxState = AVRCP_TX_SENDING;
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UPDATE_TXSTATE , chnl->curTxState);
        }
    }else{
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONTINUE_QUEUE_FAIL, chnl->curTxState);
        status = BT_STATUS_FAILED;
    }

    OS_UnlockStack();
    
    // return as error
    return status;
}


/**
 * AV13 
 * Qeueu a queue packet for sending out
 * 
 */
BtStatus AVRCP_SendBrowseQueuePacket(AvrcpChannel *chnl, AvrcpQueuePacketFrame *packetFrame){
    BtStatus status;
    U16 payload_mtu;
    U8 type;

    OS_LockStack();
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_SEND_QUEUE_PACKET, chnl->curBrowseTxState);
    if( packetFrame->cmd_type != 2 ){
        AvrcpAssert(0);
    }	

    if( chnl->curBrowseTxState == AVRCP_TX_IDLE ){
        // convert to avctp packet and send it out
        chnl->curBrowseQueueFrame = packetFrame;

        payload_mtu = AVCTP_GetSinglePacketMtu( &chnl->bws_chnl, FALSE); // AVCTP channel always uses AV/C header

        if( payload_mtu > (packetFrame->data_len+3) ){
            // single avrcp packet
            type = AVRCP_PACKET_TYPE_SINGLE;

            // compose the chnl->curBrowseRawFrame by packetFrame's data
			chnl->curBrowseRawFrame.transaction_id = packetFrame->transId;
			chnl->curBrowseRawFrame.type           = packetFrame->is_cmd;
			chnl->curBrowseRawFrame.pid            = SC_AV_REMOTE_CONTROL;

			chnl->curBrowseRawFrame.length         = packetFrame->data_len + 3; // 3 are avrcp browse header
			if( chnl->curBrowseRawFrame.length > sizeof(chnl->txBrowseBuffer) ){
			    Assert(0);
			}
            chnl->curBrowseRawFrame.data           = chnl->txBrowseBuffer;
            OS_MemCopy( (U8 *) &chnl->curBrowseRawFrame.data[3], (const U8 *) packetFrame->data, packetFrame->data_len);
			chnl->curBrowseRawFrame.data[0] = packetFrame->pdu_id;
			StoreBE16( (U8 *) &(chnl->curBrowseRawFrame.data[1]), packetFrame->data_len);
			
            status = AVCTP_SendRawMessage( &chnl->bws_chnl, &(chnl->curBrowseRawFrame));
            if( BT_STATUS_PENDING == status ){
                chnl->curBrowseTxState = AVRCP_TX_SENDING;
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_UPDATE_TXSTATE , chnl->curBrowseTxState);               
            }else{
                chnl->curBrowseQueueFrame = NULL;
            }
        }else{
            // error! browse packet should be smaller then l2cap size
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_PACKET_TOO_LARGE, payload_mtu , packetFrame->data_len );
            status = BT_STATUS_INVALID_PARM;
        }

    }else{
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_PUT_PACKET_TO_QUEUELIST, packetFrame);
        // append queue the packet and return as pending
        InsertHeadList( &chnl->txBrowsePacketList, (ListEntry *) packetFrame);
        status = BT_STATUS_PENDING;
    }

    OS_UnlockStack();
    
    // return as error
    return status;
}

U16 AVRCP_GetSinglePacketMtu(AvrcpChannel *chnl, BT_BOOL bBrowse){
    U16 u2Max;
    
    if( FALSE == bBrowse ){
        u2Max = AVCTP_GetSinglePacketMtu( &chnl->chnl, TRUE );
        if( u2Max > (512 - 10) ){
            u2Max = 512 - 10; // 10 is the avctp av/c avrcp specific header
        }
    }else{
        // browse channel is the avctp's max mtu value
        u2Max = AVCTP_GetSinglePacketMtu( &chnl->chnl, FALSE );
    }
    
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_GET_SINGLE_AVRCP_MTU, u2Max, bBrowse);

    return u2Max;
}

/*---------------------------------------------------------------------------
 * AvrcpAlloc()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes the global context for the AV profile.  This 
 *            function can be replaced if necessary.
 *
 * Return:    void
 */
BOOL AvrcpAlloc(void)
{
    U8* ptr; 

    /* Fill memory with 0. */
#if XA_CONTEXT_PTR == XA_ENABLED
    ptr = (U8*)avrcpContext;
#else
    ptr = (U8*)&avrcpContext;
#endif /* XA_CONTEXT_PTR */

    OS_MemSet(ptr, 0, (U32)sizeof(BtAvrcpContext));

    return TRUE;
}

/*---------------------------------------------------------------------------
 *            AvrcpReportFailedConnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Report a failed connection attempt
 *
 */
void AvrcpReportFailedConnect(AvrcpChannel *chnl)
{
    AvrcpCallbackParms  info;

    /* Release link handler */
    OS_Report("[AVRCP] remove link handler");
    /* Indicate the response to the application */
    info.event = AVRCP_EVENT_DISCONNECT;
    info.channel = chnl;
	info.p.remDev = chnl->cmgrHandler.remDev;
    (void)CMGR_RemoveDataLink(&chnl->cmgrHandler);
    chnl->callback(chnl, &info);
    chnl->cmgrHandler.remDev = 0;
    chnl->cmgrHandler.bdc = 0;     
}

/*---------------------------------------------------------------------------
 *            AvrcpCmgrCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by device manager with link state events.
 *
 */
static void AvrcpCmgrCallback(CmgrHandler *cHandler, 
                              CmgrEvent Event, 
                              BtStatus Status)
{
    AvrcpChannel   *chnl = ContainingRecord(cHandler, AvrcpChannel, cmgrHandler);

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CMGR_CALLBACK, Event);

    switch (Event) {
    
    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            /* ACL is connected, start service query */

#ifndef __BT_AVRCP_V14__
            if (AvrcpStartServiceQuery(chnl, BSQM_FIRST) == BT_STATUS_PENDING) {
                return;
            }
#else
			if (AvrcpStartMultiServiceQuery(chnl, BSQM_FIRST, chnl->role) == BT_STATUS_PENDING) { //chnl->role
                return;
            }
#endif

        }else{
            OS_Report("AvrcpCmgrCallback status:%d", Status);
        }

        /* Disconnect and call the application */
        AvrcpReportFailedConnect(chnl);
        break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_avrcp_get_bd_addr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src     [?]     
 *  dst     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_avrcp_get_bd_addr(bt_addr_struct *src, BD_ADDR *dst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemCopy((U8 *)&dst->addr[0], (U8 *)&src->lap, 3);
    dst->addr[3] = src->uap;
    OS_MemCopy((U8 *)&dst->addr[4], (U8 *)&src->nap, 2);
}

/*---------------------------------------------------------------------------
 *            AVRCP_HandleA2DPInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by a2dp when state is changed
 * algorithm:
 * a2dp connected event 0x00
 *      uncompare addr - do nothing
 *      same addr with avrcp - do nothing
 *      not same addr with avrcp - do disconnect
 * a2dp disconnected event 0x01
 *      uncompare addr - do disconnect
 *      same addr with avrcp - do disconnect
 *      not same addr with avrcp - do disconnect
 * a2dp pause stream event : 0x03 
 *      check the fast-forward pressed status and send the relesae
 */
 void AVRCP_HandleA2DPInfo(U32 info, U8 *data){
    bt_addr_struct l_btaddr = {0, 0, 0};
    U8 l_addr[sizeof(BD_ADDR)] = {0, 0, 0, 0, 0, 0};
    U8 l_a2dpaddr[sizeof(BD_ADDR)] = {0, 0, 0, 0, 0, 0};
    U8 chnl_num = 0;
    AvrcpChannel *chnl;
	U8 l_hasAvrcpAddr = FALSE;
    AvrcpChannel *avrcpChannel = 0;
        
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    chnl = &avrcp_cntx[chnl_num].rcp_channel;
    
    Report(("AVRCP: AVRCP_HandleA2DPInfo info:%d data:0x%x", info, data));
    switch(info){
        case 0: /* a2dp connected */
            /* update the latest a2dp addr */
            if( data != NULL ){
                bt_avrcp_get_bd_addr( (bt_addr_struct*)data, &g_avrcp_latest_a2dpaddr);
            }
            
            if( AVRCP_IsConnected(chnl) ){
                // avrcp is connected 
                l_hasAvrcpAddr = AVRCP_GetConnectedAddr(chnl, l_addr);
                if( NULL != data){
                    bt_avrcp_get_bd_addr( (bt_addr_struct*)data,(BD_ADDR *) l_a2dpaddr);
                }

                OS_Report("AVRCP:[addr] a %02x:%02x:%02x:%02x:%02x:%02x", l_addr[0], l_addr[1], l_addr[2], l_addr[3], l_addr[4], l_addr[5]);
                OS_Report("AVRCP:[addr] b %02x:%02x:%02x:%02x:%02x:%02x", l_a2dpaddr[0], l_a2dpaddr[1], l_a2dpaddr[2], l_a2dpaddr[3], l_a2dpaddr[4], l_a2dpaddr[5]);                
                if( TRUE != l_hasAvrcpAddr ){
					OS_Report("AVRCP: no avrcp addr. ignore compare");
				}else{
					if(  data != NULL && 0 != memcmp(l_addr, l_a2dpaddr, sizeof(BD_ADDR)) ){
						// "no a2dp addr" Take it as the same
						Report(("AVRCP: disconnect because a2dp is connected with other device"));
						AVRCP_UitlInternalDisconnectReq(0);
					}
				}
            }
        break;
        case 1: /* a2dp disconnected */
            if( AVRCP_IsConnected(chnl) ){
                // avrcp is connected                 
                AVRCP_GetConnectedAddr(chnl, l_addr);
                if( NULL != data){
                    bt_avrcp_get_bd_addr( (bt_addr_struct *)data, (BD_ADDR *)l_a2dpaddr);
                }

                OS_Report("AVRCP:[addr] a %02x:%02x:%02x:%02x:%02x:%02x", l_addr[0], l_addr[1], l_addr[2], l_addr[3], l_addr[4], l_addr[5]);
                OS_Report("AVRCP:[addr] b %02x:%02x:%02x:%02x:%02x:%02x", l_a2dpaddr[0], l_a2dpaddr[1], l_a2dpaddr[2], l_a2dpaddr[3], l_a2dpaddr[4], l_a2dpaddr[5]);                

                // address is the same. disconnect it.
                Report(("AVRCP: disconnect because a2dp is lost"));
                AVRCP_UitlInternalDisconnectReq(0);
            }
            if( AVRCP_IsBrowseExisted(chnl) ){
                AVRCP_UitlInternalBrowseDisconnectReq(chnl_num);
            }
            
        break;
        case 0x03: /*a2dp stream paused */
            avrcpChannel = AVRCP_UitlGetChannelByChnlNum(chnl_num);
            if( 0 != avrcpChannel ){
                if( avrcpChannel->ffPressed == 1 ){
                    AvrcpFFTimerSendRelease(avrcpChannel);
                }
            }
            break;
    }
}

void AvrcpFFTimerSendRelease(AvrcpChannel *chnl){
    
    U8 transId = 0xFF;
    U8 operands[3] = {0x7c, 0x49, 0x00};
    if( 0 != chnl ){
        OS_Report("avrcp: fftimer check ffKey:%d ffPressed:%d", chnl->ffKey, chnl->ffPressed);
        switch(chnl->ffKey){
            case AVRCP_POP_FAST_FORWARD:
            case AVRCP_POP_REWIND:
            case AVRCP_POP_FORWARD: // moto-s605 FF
            case AVRCP_POP_BACKWARD:
            {
            if( 0 != chnl->ffKey && 1 == chnl->ffPressed ){        
                // setup the ffintercept response
                chnl->ffInterceptRsp = 1;
                chnl->ffPressed = 0; //avoid timerout twice
                
                // send the ff -release indication to upper layer
                OS_Report("avrcp: fftimer expire to upper layer");
                operands[1] = chnl->ffKey | 0x80; //last bit is the release bit
                
                AvrcpHandleCommandFrameInd(
                    0, //chnl_num
                    BT_AVRCP_RESULT_SUCCESS,
                    transId,
                    0,
                    AVRCP_CTYPE_CONTROL, // ctype
                    9, // subunittype   
                    0, //subunitid
                    3, // operandLen
                    operands);            
            }else{
                OS_Report("avrcp: fftimer ffKey:%d ffPressed:%d", chnl->ffKey, chnl->ffPressed);
            }
            }
            break;
        }
    }
}


/*---------------------------------------------------------------------------
 *            AvrcpHandleFFCmd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle the timeout of release of Fast-forward or Rewind.
 *
 * Returns:   TRUE if the event indicated in "parms" was handled. FALSE
 *            if it could not be processed.
 */
BOOL AvrcpHandleFFCmd(AvrcpChannel *chnl, AvrcpCallbackParms *parms)
{
 AvctpCmdFrame *cmdFrame = 0;
    U8 keyCode = 0; // 
    //OS_Report("avrcp: fftimer handleFFCmd");
        
    if (parms->p.cmdFrame->subunitType == AVRCP_SUBUNIT_PANEL)
    {
        if ((parms->p.cmdFrame->ctype == AVCTP_CTYPE_CONTROL) &&
            (parms->p.cmdFrame->opcode == AVCTP_OPCODE_PASS_THROUGH)) {
            cmdFrame = parms->p.cmdFrame;
            
            if( cmdFrame->operandLen > 1 ){
                keyCode = cmdFrame->operands[0] & 0x7F ;

                switch(keyCode){
                    case AVRCP_POP_FAST_FORWARD:
                    case AVRCP_POP_REWIND:
                    case AVRCP_POP_FORWARD: // moto-s605 FF
                    case AVRCP_POP_BACKWARD:
                    {
                        if( 0x80 == (cmdFrame->operands[0]&0x80) ){ // last bit is the relase bit
                            chnl->ffPressed = 0;
                            chnl->ffKey = 0;
                            // OS_Report("avrcp: fftimer handleFFCmd-release ");

                        }else{
                            chnl->ffPressed = 1;
                            chnl->ffKey = cmdFrame->operands[0];
                            // OS_Report("avrcp: fftimer handleFFCmd-press ffKey:0x%x transId:%d ", chnl->ffKey, parms->transId);                        
                        }
                    }
        break;
                    default:
                         OS_Report("avrcp: fftimer keyCode:0x%x ", keyCode);
                    break;
                        
    }
            
            }

        }         
    }   
    return TRUE;
}

#if AVRCP_PANEL_SUBUNIT == XA_ENABLED

/*---------------------------------------------------------------------------
 * AVRCP_SetPanelKey()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Queues a key operation to be sent.
 *
 * Return:   (See header file)
 */
BtStatus AVRCP_SetPanelKey(AvrcpChannel *chnl, AvrcpPanelOperation op,
                                  BOOL press)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    U8 qLen, qFree, oldWrite;
    AvrcpPanelOperation lastOp = 0;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    if (chnl->role != SC_AV_REMOTE_CONTROL) {
        return BT_STATUS_FAILED;
    }

    OS_LockStack();  

    op = op & ~AVRCP_POP_RELEASE;
    qLen = AVRCP_OpQueueLen(chnl);
    qFree = AVRCP_OpQueueFree(chnl);
    oldWrite = chnl->panel.ct.opWrite;

    if (qLen) {
        lastOp = AVRCP_OpQueueGet(chnl, qLen - 1);
    }

    /* If this is a new op and the previous key was not released, we must
     * automatically release it. */
    if ( (qLen) &&
         ( (lastOp & AVRCP_POP_RELEASE) == 0) &&
         ( (op != (lastOp & ~AVRCP_POP_RELEASE)) ||
           ( (press) && (op == (lastOp & ~AVRCP_POP_RELEASE)) ) ) ) {
        if (!qFree) {
            goto error;
        }
        lastOp = lastOp | AVRCP_POP_RELEASE;
        AVRCP_OpQueuePush(chnl, lastOp);
        qFree--;
        qLen++;
    }

    /* If this is a release request and the previous item was not a press
     * for the same operation, automatically add a "press"
     */
    if ( (!press) &&        
         ( (!qLen) ||
           (qLen && (lastOp != op) ) ) ) {
        if (!qFree) {
            goto error;
        }
        AVRCP_OpQueuePush(chnl, op);
        qFree--;
        qLen++;            
    }

    /* Finally, add this very operation, if there is room */
    if (!qFree) {
        goto error;
    }
    AVRCP_OpQueuePush(chnl, op | (press ? 0 : AVRCP_POP_RELEASE));
    AvrcpHandlePanelInput(chnl);
    status = BT_STATUS_PENDING;
    goto done;

error:
    chnl->panel.ct.opWrite = oldWrite;
done:
    OS_UnlockStack();

    return status;
}


/*---------------------------------------------------------------------------
 *            AvrcpPanelCnf()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deliver a cnf event to the channel application
 *
 */
static void AvrcpPanelCnf(AvrcpChannel *chnl,
                          AvrcpResponse rsp, AvrcpPanelOperation op)
{
    AvrcpCallbackParms parms;

    parms.channel = chnl;
    parms.p.panelCnf.operation = op & ~AVRCP_POP_RELEASE;
    parms.p.panelCnf.press = ((op & AVRCP_POP_RELEASE) ? FALSE : TRUE);
    parms.p.panelCnf.response = rsp;
    parms.event = AVRCP_EVENT_PANEL_CNF;
    chnl->callback(chnl, &parms);

    /* After notifying application, more keystrokes might have appeared. */
    AvrcpHandlePanelInput(chnl);
}


/*---------------------------------------------------------------------------
 *            AvrcpHandlePanelInput()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles case in which a panel subunit controller should check
 *            to see if its input can be delivered.
 */
static void AvrcpHandlePanelInput(AvrcpChannel *chnl)
{
    AvrcpPanelOperation op;
    BtStatus status;
    U16 opLen;

    /* Make sure there is something to do */
    opLen = AVRCP_OpQueueLen(chnl);
    if (!opLen) return;

    op = AVRCP_OpQueueGet(chnl, 0);

    switch(chnl->panelState) {
    case AVRCP_PANEL_STATE_C_IDLE:
        Assert(0 == (op & AVRCP_POP_RELEASE));
        chnl->panel.ct.data[0] = op;
        status = AVCTP_SendCommand(&chnl->chnl, &chnl->panel.ct.cmd);

        if (BT_STATUS_PENDING == status ) {
            chnl->panelState = AVRCP_PANEL_STATE_C_PRESS_R;
        } else if (BT_STATUS_BUSY != status) {
            Report(("AVRCP: Couldn't send command.\n"));
            AVRCP_Disconnect(chnl);
        }
        break;

    case AVRCP_PANEL_STATE_C_PRESS:
        /* If there are at least two items then we know the current
         * op was released.
         */
        if (opLen >= 2) {

            /* Cancel the holding timer */
            EVM_CancelTimer(&chnl->panelTimer);

            /* Indicate release to target */
            chnl->panel.ct.data[0] = op | AVRCP_POP_RELEASE;
            if (BT_STATUS_PENDING !=
                AVCTP_SendCommand(&chnl->chnl, &chnl->panel.ct.cmd)) {
                Report(("AVRCP: Couldn't send command.\n"));
                AVRCP_Disconnect(chnl);
            }
            chnl->panelState = AVRCP_PANEL_STATE_C_RELEASE;
        }
        break;

    case AVRCP_PANEL_STATE_C_SKIP:
        /* If there are at least two items then we know the current
         * op was released.
         */
        if (opLen >= 2) {
            /* Indicate that the release of the op was skipped */
            AVRCP_OpQueueAdvance(chnl, 2);
            chnl->panelState = AVRCP_PANEL_STATE_C_IDLE;
            AvrcpPanelCnf(chnl, AVRCP_RESPONSE_SKIPPED, (AvrcpResponse)(op | AVRCP_POP_RELEASE));
        }        
        break;
    }

    return;
}

/*---------------------------------------------------------------------------
 *            AvrcpHandlePanelRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Dispatch for responses directed to a Panel Subunit controller.
 *
 * Returns:   TRUE if the event indicated in "parms" was handled. FALSE
 *            if it could not be processed.
 */
static BOOL AvrcpHandlePanelRsp(AvrcpChannel *chnl, AvrcpCallbackParms *parms)
{
    AvrcpResponse rsp = parms->p.rspFrame->response;
    AvrcpPanelOperation op;

    if ((rsp != AVRCP_RESPONSE_ACCEPTED) &&
        (rsp != AVRCP_RESPONSE_REJECTED) &&
        (rsp != AVRCP_RESPONSE_NOT_IMPLEMENTED))
    {
        Report(("AVRCP: Invalid response code %d, changing to REJECTED\n"));
        rsp = AVRCP_RESPONSE_REJECTED;
    }

    switch(chnl->panelState) {
    case AVRCP_PANEL_STATE_C_PRESS_R:

        /* We could verify the response contents but for now just assume it is correct.
         */
        if (parms->p.rspFrame->response == AVRCP_RESPONSE_ACCEPTED) {
            EVM_StartTimer(&chnl->panelTimer, AVRCP_PANEL_HOLDUPDATE_TIME);
            chnl->panelState = AVRCP_PANEL_STATE_C_PRESS;
        } else {
            chnl->panelState = AVRCP_PANEL_STATE_C_SKIP;
        }
        AvrcpPanelCnf(chnl, rsp, AVRCP_OpQueueGet(chnl, 0));

        return TRUE;

    case AVRCP_PANEL_STATE_C_RELEASE:
        /* On release response, advance the opqueue and signal the app */
        Assert(AVRCP_OpQueueLen(chnl) >= 2);
        op = AVRCP_OpQueueGet(chnl, 1);
        AVRCP_OpQueueAdvance(chnl, 2);
        chnl->panelState = AVRCP_PANEL_STATE_C_IDLE;
        AvrcpPanelCnf(chnl, rsp, op);
        return TRUE;

    default:
        /* We are simply not expecting responses in other states. */
        return FALSE;
    }
}

/*---------------------------------------------------------------------------
 *            AvrcpHandlePanelCmd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Dispatch for commands directed to a Panel Subunit target.
 *
 * Returns:   TRUE if the event indicated in "parms" was handled. FALSE
 *            if it could not be processed.
 */
static BOOL AvrcpHandlePanelCmd(AvrcpChannel *chnl, AvrcpCallbackParms *parms)
{
    /* Intercept commands destined for the panel subunit target */
    if (parms->p.cmdFrame->subunitType == AVRCP_SUBUNIT_PANEL)
    {
        if ((parms->p.cmdFrame->ctype == AVCTP_CTYPE_CONTROL) &&
            (parms->p.cmdFrame->opcode == AVCTP_OPCODE_PASS_THROUGH)) {

            if ((parms->p.cmdFrame->operandLen >= 2) &&
                ((parms->p.cmdFrame->operands[0] & 0x7F) != AVRCP_POP_VENDOR_UNIQUE)) {

                while (!AvrcpHandlePanelCmdControl(chnl, parms));

                return TRUE;
            }
        }         
    } else if ((parms->p.cmdFrame->subunitType == AVRCP_SUBUNIT_UNIT) &&
        (parms->p.cmdFrame->ctype == AVRCP_CTYPE_STATUS) &&
        (parms->p.cmdFrame->opcode == AVRCP_OPCODE_UNIT_INFO)) {

        /* Handle a UNIT_INFO command */

        /* Send back a canned response indicating PANEL */
        chnl->panel.tg.rsp.response = AVCTP_RESPONSE_IMPLEMENTED_STABLE;
        chnl->panel.tg.rsp.subunitType = AVRCP_SUBUNIT_UNIT;
        chnl->panel.tg.rsp.subunitId = 0x7; /* Ignore */
        chnl->panel.tg.rsp.opcode = AVRCP_OPCODE_UNIT_INFO;
        chnl->panel.tg.rsp.operandLen = 5;
        chnl->panel.tg.rsp.operands = chnl->panel.tg.data;
        chnl->panel.tg.data[0] = 0x07; /* As per specification */
        chnl->panel.tg.data[1] = (AVRCP_SUBUNIT_PANEL << 3) + 0; /* Unit_type of Panel */
        OS_MemCopy((U8 *)chnl->panel.tg.data + 2, (U8 *)AVRCP_PANEL_COMPANY_ID, 3);
        if (BT_STATUS_PENDING != AVCTP_SendResponse(&chnl->chnl, &chnl->panel.tg.rsp)) {
            Report(("AVRCP: Couldn't send response.\n"));
            AVRCP_Disconnect(chnl);
        } 
        return TRUE;
    } else if ((parms->p.cmdFrame->subunitType == AVRCP_SUBUNIT_UNIT) &&
        (parms->p.cmdFrame->ctype == AVRCP_CTYPE_STATUS) &&
        (parms->p.cmdFrame->opcode == AVRCP_OPCODE_SUBUNIT_INFO)) {

        /* Handle a SUBUNIT_INFO command */

        /* Send back a canned response indicating PANEL */
        chnl->panel.tg.rsp.response = AVCTP_RESPONSE_IMPLEMENTED_STABLE;
        chnl->panel.tg.rsp.subunitType = AVRCP_SUBUNIT_UNIT;
        chnl->panel.tg.rsp.subunitId = 0x7; /* Ignore */
        chnl->panel.tg.rsp.opcode = AVRCP_OPCODE_SUBUNIT_INFO;
        chnl->panel.tg.rsp.operandLen = 5;
        chnl->panel.tg.rsp.operands = chnl->panel.tg.data;
        chnl->panel.tg.data[0] = 0x07; /* As per specification */
        chnl->panel.tg.data[1] = (AVRCP_SUBUNIT_PANEL << 3) + 0; /* Unit_type of Panel */
        chnl->panel.tg.data[2] = 0xFF;
        chnl->panel.tg.data[3] = 0xFF;
        chnl->panel.tg.data[4] = 0xFF;
        if (BT_STATUS_PENDING != AVCTP_SendResponse(&chnl->chnl, &chnl->panel.tg.rsp)) {
            Report(("AVRCP: Couldn't send response.\n"));
            AVRCP_Disconnect(chnl);
        } 
        return TRUE;           
    }
    return FALSE;
}

/*---------------------------------------------------------------------------
 *            AvrcpPanelInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deliver a PANEL_IND event to the application.
 *
 */
static void AvrcpPanelInd(AvrcpChannel *chnl,
                          AvrcpEvent event)
{
    AvrcpCallbackParms parms;

    parms.channel = chnl;
    parms.p.panelInd.operation = chnl->panel.tg.curOp;
    parms.event = event;
    chnl->callback(chnl, &parms);

    /* Reset the response byte in case it was altered */
    if ((chnl->panel.tg.curRsp != AVCTP_RESPONSE_REJECTED) &&
        (chnl->panel.tg.curRsp != AVCTP_RESPONSE_NOT_IMPLEMENTED)) {
        chnl->panel.tg.curRsp = AVCTP_RESPONSE_ACCEPTED;
    }

    /* After indicating a release be sure to reset the curOp */
    if (event == AVRCP_EVENT_PANEL_RELEASE)
    {
        chnl->panel.tg.curOp = AVRCP_POP_INVALID;
    }
}

/*---------------------------------------------------------------------------
 *            AvrcpHandlePanelCmdControl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Dispatch for commands directed to a Panel Subunit target.
 *
 * Returns:   TRUE if the event indicated in "parms" was handled. FALSE
 *            if it could not be processed.
 */
static BOOL AvrcpHandlePanelCmdControl(AvrcpChannel *chnl, AvrcpCallbackParms *parms)
{
    BOOL released = (parms->p.cmdFrame->operands[0] & 0x80) == 0x80;
    AvrcpPanelOperation op = parms->p.cmdFrame->operands[0] & 0x7F;

    /* Prepare a response frame from cmd data */
    OS_MemCopy((U8 *)&chnl->panel.tg.rsp,
        (U8 *)parms->p.cmdFrame, sizeof(AvctpCmdFrame));
    chnl->panel.tg.rsp.response = AVCTP_RESPONSE_ACCEPTED;
    chnl->panel.tg.rsp.operands = chnl->panel.tg.data;
    chnl->panel.tg.rsp.operandLen = 2;
    chnl->panel.tg.data[0] = parms->p.cmdFrame->operands[0];
    chnl->panel.tg.data[1] = 0;

    switch(chnl->panelState) {

    case AVRCP_PANEL_STATE_T_IDLE:
        if (!released) {

            /* Accept by default */
            chnl->panel.tg.curRsp = AVCTP_RESPONSE_ACCEPTED;

            /* Notify the application */
            chnl->panel.tg.curOp = parms->p.cmdFrame->operands[0] & 0x7F;
            AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_PRESS);

            chnl->panel.tg.rsp.response = chnl->panel.tg.curRsp;
            
            if (chnl->panel.tg.curRsp == AVCTP_RESPONSE_ACCEPTED) {
                chnl->panelState = AVRCP_PANEL_STATE_T_PRESS;
                EVM_StartTimer(&chnl->panelTimer, AVRCP_PANEL_PRESSHOLD_TIME);
            }
        } else {

            if (op == chnl->panel.tg.curOp) {
                /* Finally received a certain outstanding op. Don't
                 * notify app.
                 */
                chnl->panel.tg.curOp = AVRCP_POP_INVALID;

                /* Respond with standing response type */
                chnl->panel.tg.rsp.response = chnl->panel.tg.curRsp;

            } else {
                /* Accept by default */
                chnl->panel.tg.curRsp = AVCTP_RESPONSE_ACCEPTED;
                chnl->panel.tg.curOp = op;
                /* Indicate this new op */
                AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_PRESS);

                chnl->panel.tg.rsp.response = chnl->panel.tg.curRsp;
            
                if (chnl->panel.tg.curRsp == AVCTP_RESPONSE_ACCEPTED) {
                    /* Indicate the release as well */
                    AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_RELEASE);
                }
            }
        }

        if (BT_STATUS_PENDING != AVCTP_SendResponse(&chnl->chnl, &chnl->panel.tg.rsp)) {
            Report(("AVRCP: Couldn't send response.\n"));
            AVRCP_Disconnect(chnl);
        }

        break;

    case AVRCP_PANEL_STATE_T_PRESS:
        if (op == chnl->panel.tg.curOp) {

            if (released) {
                /* Notify the application */
                AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_RELEASE);

                /* Cancel the timer and go back to IDLE */
                EVM_CancelTimer(&chnl->panelTimer);
                chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;
            }

            /* Deliver a response in either case */
            if (BT_STATUS_PENDING != AVCTP_SendResponse(&chnl->chnl, &chnl->panel.tg.rsp)) {
                Report(("AVRCP: Couldn't send response.\n"));
                AVRCP_Disconnect(chnl);
            }
        } else {
            /* Release existing key (notify app) */
            AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_RELEASE);

            /* Reset to IDLE */
            EVM_CancelTimer(&chnl->panelTimer);
            chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;

            return FALSE;
        }
        break;

    case AVRCP_PANEL_STATE_T_HOLD:
        if (op == chnl->panel.tg.curOp) {
            if (released) {
                /* Notify the application */
                AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_RELEASE);

                chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;
                EVM_CancelTimer(&chnl->panelTimer);

            } else {
                /* Pressing and holding - no app notification */

                /* Reset timer */
                EVM_StartTimer(&chnl->panelTimer, AVRCP_PANEL_AUTORELEASE_TIME);
            }

            /* Deliver a response */
            if (BT_STATUS_PENDING != AVCTP_SendResponse(&chnl->chnl, &chnl->panel.tg.rsp)) {
                Report(("AVRCP: Couldn't send response.\n"));
                AVRCP_Disconnect(chnl);
            }
        } else {
            /* Release existing key (notify app) */
            AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_RELEASE);
            
            /* Reset to IDLE */
            chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;
            EVM_CancelTimer(&chnl->panelTimer);
    
            return FALSE;
        }
        break;

    default:
        Assert(0);
        chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;
        return FALSE;
    }
    return TRUE;
}

static void AvrcpPanelTimerExpired(EvmTimer *timer)
{
    AvrcpChannel *chnl = (AvrcpChannel *)timer->context;
    AvrcpPanelOperation op = chnl->panel.tg.curOp;
    bt_prompt_trace(MOD_BT, "[BT] AvrcpPanelTimerExpired"); 
    switch(chnl->panelState) {
    case AVRCP_PANEL_STATE_T_PRESS:
        /* Timer going off during PRESS indicates a HOLD */
        AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_HOLD);
        chnl->panelState = AVRCP_PANEL_STATE_T_HOLD;

        /* Now reset the timer to detect a "lost" release button */
        EVM_StartTimer(timer, AVRCP_PANEL_AUTORELEASE_TIME - AVRCP_PANEL_PRESSHOLD_TIME);
        break;

    case AVRCP_PANEL_STATE_T_HOLD:
        /* Timer going off during HOLD indicates a RELEASE */
        AvrcpPanelInd(chnl, AVRCP_EVENT_PANEL_RELEASE);

        /* Reset current operation back to previous value so that
         * corresponding (late) release command will be ignored
         */
        chnl->panel.tg.curOp = op;

        chnl->panelState = AVRCP_PANEL_STATE_T_IDLE;
        break;

    case AVRCP_PANEL_STATE_C_PRESS:
        /* Send a press update and move back into PRESS_R */
        if (BT_STATUS_PENDING != 
            AVCTP_SendCommand(&chnl->chnl, &chnl->panel.ct.cmd)) {
            Report(("AVRCP: Couldn't send command.\n"));
            AVRCP_Disconnect(chnl);
        }
        chnl->panelState = AVRCP_PANEL_STATE_C_PRESS_R;
        break;

    default:
        /* No timer should be active in other states */
        Assert(0);
        break;
    }
}

#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */

#endif /* __BT_AVRCP_PROFILE__ */
