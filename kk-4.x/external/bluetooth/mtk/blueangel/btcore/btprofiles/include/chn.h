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

#ifndef __BT_CHN_H__
#define __BT_CHN_H__

#include "stdio.h"
#include "bttypes.h"
#include "conmgr.h"
#include "rfcomm_adp.h"

#define DBG_PRINT_API(s)            printf s;printf("\r\n")  /* Prefix : [HFG][API]*/
#define DBG_PRINT_CALLBACK(s)  printf s;printf("\r\n")  /* Prefix : [HFG][CALLBACK]*/
#define DBG_PRINT_EVENT(s)        printf s;printf("\r\n")  /* Prefix : [HFG][EVENT]*/
#define DBG_PRINT_FLOW(s)         printf s;printf("\r\n")  /* Prefix : [HFG][FLOW]*/
#define DBG_PRINT_AT(s)              printf s;printf("\r\n")  /* Prefix : [HFG][AT]*/
#define DBG_PRINT_ERROR(s)       printf s;printf("\r\n")  /* Prefix : [HFG][ERR]*/
#define DBG_PRINT_WARN(s)        printf s;printf("\r\n")  /* Prefix : [HFG][WARN]*/

/* Channel States */
#define CHN_STATE_CLOSED         0
#define CHN_STATE_CONN_PENDING   1
#define CHN_STATE_CONN_INCOMING  2
#define CHN_STATE_DISCONNECTING 	3
#define CHN_STATE_OPEN           		4
#define CHN_STATE_SLC				5	/* SLC is created successfully */

/*****************************************************
*  Definition of sub states
*****************************************************/
/* Sub states of state CHN_STATE_CLOSED */
#define CHN_SUBSTATE_CLOSED1   			(CHN_STATE_CLOSED)

/* Sub states of state CHN_STATE_CONN_PENDING */
#define CHN_SUBSTATE_CONN_OUT1                  (CHN_STATE_CONN_PENDING)
#define CHN_SUBSTATE_CONN_OUT2                  (CHN_STATE_CONN_PENDING | 0x10)
#define CHN_SUBSTATE_CONN_OUT3                  (CHN_STATE_CONN_PENDING | 0x20)

/* Sub states of state CHN_STATE_CONN_INCOMING */
#define CHN_SUBSTATE_CONN_IN1                     (CHN_STATE_CONN_INCOMING)
#define CHN_SUBSTATE_CONN_IN2   			(CHN_STATE_CONN_INCOMING | 0x10)

/* Sub states of state CHN_STATE_DISCONNECTING */
#define CHN_SUBSTATE_DISC1   				(CHN_STATE_DISCONNECTING)

/* Sub states of state CHN_STATE_OPEN */
#define CHN_SUBSTATE_OPEN1   				(CHN_STATE_OPEN)
#define CHN_SUBSTATE_OPEN2   				(CHN_STATE_OPEN | 0x10)

/* Sub states of state CHN_STATE_SLC */
#define CHN_SUBSTATE_SLC1   				(CHN_STATE_SLC)
#define CHN_SUBSTATE_SLC2   				(CHN_STATE_SLC | 0x10)


/*****************************************************
*  Definition of Link Flags (Value of HfgChannel.linkFlags)
*****************************************************/
/* DESC : ACL link created and associated. It is set/reset through CMGR_CreateDataLink 
 *            and CMGR_RemoveDataLink.
 *
 * SET : ACL link is associated.
 * UNSET : Not associated ACL link.
 */
#define CHN_LINK_ACL              			0x01
/* DESC : Indicate if SCO connection creation is ongoing.
 *
 * SET : SCO connection creation is ongoing.
 * UNSET : No SCO connection creation is ongoing.
 */
#define CHN_LINK_SCO_CONNECTING          0x02
/* DESC : Indicate if SCO connection creation is ongoing.
 *
 * SET : SCO connection removal is ongoing.
 * UNSET : No SCO connection removal is ongoing.
 */
#define CHN_LINK_SCO_DISCONNECTING    	0x04
/* DESC : Indicate if RFCOMM channel for HFG is created or not
 *
 * SET : RFCOMM channel for HFG is created.
 * UNSET : No RFCOMM channel for HFG is created.
 */
#define CHN_LINK_HANDSFREE        		0x08
/* DESC : Indicate if RFCOMM channel shall be removed after the SCO connection is removed. 
 * 		 It is used in disconnection procedure.
 *
 * SET : RFCOMM channel shall be removed after SCO is disconnected.
 * UNSET : RFCOMM channel shall not be removed after SCO is disconnected.
 */
#define CHN_LINK_REMOVE_HF        		0x10
/* DESC : Not used
 */
#define CHN_LINK_HS_CONNECTING    		0x20
/* DESC : Not used
 */
#define CHN_LINK_HS_DISCONNECTING 	0x40
/* DESC : Not used
 */
#define CHN_LINK_REM_DISC         			0x80


#define SDP_QUERY_FLAG_ONGOING  			0x80


typedef U8 ChnEvent;
#define CHN_EVENT_SERVICE_CONNECTED          	1
#define CHN_EVENT_SERVICE_DISCONNECTED       	2
#define CHN_EVENT_AUDIO_CONNECTED            		3
#define CHN_EVENT_AUDIO_DISCONNECTED         	4
#define CHN_EVENT_AUTH_REQ					5
#define CHN_EVENT_RX_DATA          				6
#define CHN_EVENT_RFCOMM_CONNECTED			7


#define CHN_TX_BUFFER_POW					10
#define CHN_TX_BUFFER_SIZE					(1<<10)
#define CHN_TX_BUFFER_MODE				(CHN_TX_BUFFER_SIZE-1)

#define ChnGetState(c)		(c->state & 0x0F)
#define ChnGetSubstate(c)		(c->state)

typedef struct _RfBuffer
{
	U16	len;
	U8 	*buf;
}RfBuffer;

typedef struct _ChnCallbackParms 
{
	ChnEvent    	event;   /* Event associated with the callback       */

	//BtStatus    status;  /* Status of the callback event             */
	I8			status;
	//BtErrorCode errCode; /* Error code (reason) on disconnect events */
	U8			errCode;

	/* For certain events, a single member of this union will be valid.
	 * See HfgEvent documentation for more information.
	*/
    	union {
		void                        	*ptr;
		BD_ADDR                 	*addr;
		RfBuffer				*rxBuf;
    } p;
}ChnCallbackParms;

typedef void (*ChnCallback)(void *context, ChnCallbackParms *Parms);

typedef struct _ChnChannel 
{
	/* Registration variables */
	ListEntry            	node;  
	ChnCallback          	callback;         	/* Application callback function    */

	U16				svc_type;			/* Service UUID */
	U16				remote_svc_type;	/* Remote Service UUID */
	
	RfService               chnService;
	RfChannel           	*rfChannel;        	/* RFCOMM channel used for audio
                                            			* gateway connection.
                                            			*/

	/* Connection State Variables */
	U8                   	state;            	/* Current connection state         */
	U16                  	flags;            	/* Current connection flags         */
	U8                   	linkFlags;        	/* Levels of service connected      */
    
	/* SDP variables for client */
	SdpRecord             chnSdpRecord;
	SdpQueryToken    	sdpQueryToken;  	/* Used to query the service        */
	U8                  	queryFlags;        	/* Defines which SDP entries were
									* parsed from the gateway.
									*/
	U8                  	rfServerChannel;   /* When connecting AG client        */

	/* Channel Resources */
	CmgrHandler         cmgrHandler;
	#if BT_SECURITY == XA_ENABLED
	BtSecurityRecord    chnSecRec;
	#endif	
	BtRemoteDevice 	*remDev;		/* Keep the remDevice returned in callback function, soit can be used when HFG need to callback to ADP */
	kal_bool			bTxInProgress;
	BtPacket            	txPacket;
	XaBufferDesc        	txBuffer;
	U8                  	txData[CHN_TX_BUFFER_SIZE];
}ChnChannel;

typedef struct _BtChnContext 
{
    /* List of registered channels */
    ListEntry       		channelList;

    RfChannel         		rfChannels[NUM_BT_DEVICES * 2 + 2];
    ListEntry         		freeRfChannelList;
} BtChnContext;

extern BtChnContext *chnContext;

/***********************************************************
*  Channel API
***********************************************************/
BOOL ChnAlloc(void);
BOOL chn_init(void);
BtStatus chn_register(ChnChannel *Channel, ChnCallback Callback, U16 serv_type, U16 remote_svc);
BtStatus chn_deregister(ChnChannel *Channel);
BtStatus chn_create_service_link(ChnChannel *Channel, BD_ADDR *Addr);
BtStatus chn_disconnect_service_link(ChnChannel *Channel);
BtStatus chn_create_audio_link(ChnChannel *Channel);
BtStatus chn_disconnect_audio_link(ChnChannel *Channel);
BtStatus chn_accept_connect(ChnChannel *Channel);
BtStatus chn_reject_connect(ChnChannel *Channel);
BtStatus chn_send_data(ChnChannel *Channel, U8* data, U16 len);
BtStatus ChnDisconnecting(ChnChannel *Channel);
BtStatus ChnRegisterServerChannel(ChnChannel *Channel);
BtStatus ChnDeregisterServerChannel(ChnChannel *Channel);
BtStatus ChnOpenChannel(ChnChannel *Channel, BD_ADDR *Addr);
BtStatus ChnCloseChannel(ChnChannel *Channel);
RfChannel *ChnAllocRfChannel(void);
void ChnFreeRfChannel(RfChannel *rfChannel);
BtStatus ChnRegisterSdpServices(ChnChannel *Channel, U16 svc);
BtStatus ChnDeregisterSdpServices( ChnChannel *Channel );
BtStatus ChnStartServiceQuery(ChnChannel *Channel, SdpQueryMode mode);
ChnChannel * ChnFindRegisteredChannel(RfChannel *rfChannel);
BtStatus ChnChangeSubstate(ChnChannel *Channel, U8 newState);
void ChnAppCallback(ChnChannel *Channel, ChnEvent Event, BtStatus Status, U32 data);
void ChnServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
void ChnRfCallback(RfChannel *rfChannel, RfCallbackParms *Parms);
U16 ChnRemainTxData(ChnChannel *Channel);
U16 ChnEmptyTxSize(ChnChannel *Channel);
void ChnResetTxBuffer(ChnChannel *Channel);
U16 ChnWriteTxBuffer(ChnChannel *Channel, U8 *buf, U16 len);
BtStatus ChnSendTxBuffer(ChnChannel *Channel);
void ChnReportServiceConnected(ChnChannel *Channel);
U8 HfphfSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn);
#endif