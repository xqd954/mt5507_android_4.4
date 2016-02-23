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

#ifdef __BT_MCAP__

#ifndef __MCAP_H
#define __MCAP_H

#include <btconfig.h>
#include <bttypes.h>
#include <eventmgr.h>
#include <l2cap.h>
#include <sec.h>
#include <me.h>

/*---------------------------------------------------------------------------
 * Multi-Channel Adaptation Protocol (MCAP) layer
 *
 *     MCAP defines a simple protocol for communicating with devices as if they were
 *     connected over a locally attached cable. This is a versatile
 *     L2CAP-based protocol that provides a Control Channel to create
 *     and manage a plurality of Data Channels.
 *     
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * NUM_MCAP_MCLS constant
 * 
 *     Defines the number of concurrent MCL connections supported by this 
 *     device. Multiple concurrent connections would typically be configured
 *
 *     The default value is 1, and the maximum is limited to 255.
 */
#ifndef NUM_MCAP_MCLS
#define NUM_MCAP_MCLS 7
#endif

#if NUM_MCAP_MCLS < 1
#error NUM_MCAP_MCLS must be greater than 0
#endif

#if NUM_MCAP_MCLS > 255
#error NUM_MCAP_MCLS must be less than 256
#endif


/*---------------------------------------------------------------------------
 * MCAP_NUM_TIMERS constant
 *
 *     Defines the number of timers available for signaling commands (Control
 *     Response Msg.)
 *
 *     The default value is 3 timers.
 */
#ifndef MCAP_NUM_TIMERS
#define MCAP_NUM_TIMERS     3
#endif

#if MCAP_NUM_TIMERS < 1
#error MCAP_NUM_TIMERS must be at least 1.
#endif

/*---------------------------------------------------------------------------
 * MCAP_CONTROL_TIMEOUT constant
 *
 *     Defines the response timeout value for signaling commands (Control
 *     Response Msg), in seconds. If the timer elapses without a response
 *     from the remote device, a timeout event is generated.
 *
 *     The timeout value may range from 1 to 30 seconds. The default value
 *     is 20 seconds.
 */
#ifndef MCAP_CONTROL_TIMEOUT
#define MCAP_CONTROL_TIMEOUT     20000
#endif

#if MCAP_CONTROL_TIMEOUT < 1 || MCAP_CONTROL_TIMEOUT > 30000
#error MCAP_CONTROL_TIMEOUT must be between 1 and 30 seconds.
#endif

/*---------------------------------------------------------------------------
 * Check the Maximum L2CAP MTU size.
 * 
 */
#if L2CAP_MTU < 1691
#error MCAP requires maximum L2CAP_MTU be at least 1691 bytes
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * McapUpperProfile type
 *
 *     The profile id that use MCAP. Used in McapRegister struct
 */

typedef U8 McapUpperProfile;

#define MCAPPROFILE_HDP		1;

/*---------------------------------------------------------------------------
 * MCAPEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type McapCallback. The "McapCallbackParms.event" field will
 *     contain one of the event types below.
 */
typedef U8 McapEvent;

#define MCAPEVENT_MCLOPENED              	1
#define MCAPEVENT_MCLOPEN_IND               2 
#define MCAPEVENT_MCLCLOSED             	3
#define MCAPEVENT_MDLOPENED_CNF           	4
#define MCAPEVENT_MDLOPENED_IND           	5
#define MCAPEVENT_MDLCONFIG_IND           	6
#define MCAPEVENT_MDLCONFIG_CNF           	7
#define MCAPEVENT_MDLCLOSED       			8
#define MCAPEVENT_ABORTED    				9
#define MCAPEVENT_ABORT_IND    				10
#define MCAPEVENT_PACKET_HANDLED    		11
#define MCAPEVENT_PACKET_IND    			12
/* End of McapEvent */

/*---------------------------------------------------------------------------
* MCAPState type
*/
typedef U8 McapMclState;
	
#define MCAP_MCL_STATE_IDLE           	0
#define MCAP_MCL_STATE_CONNECTED        1
#define MCAP_MCL_STATE_ACTIVE           2
#define MCAP_MCL_STATE_PENDING          3
#define MCAP_MCL_STATE_DISCONNECTING    4
	
typedef U8 McapMdlState;
	
#define MCAP_MDL_STATE_CLOSED           	0
#define MCAP_MDL_STATE_OPEN        			1
#define MCAP_MDL_STATE_CONN_PENDING         2
#define MCAP_MDL_STATE_DISC_PENDING         3
/* End of MCAPState */
	
/*---------------------------------------------------------------------------
* McapOpCode type
*
*		MCAP Standard Op Codes
*
*/
typedef U8 McapOpCode;
	
#define MCAP_ERROR_RSP				0x00
#define MCAP_MD_CREATE_MDL_REQ		0x01
#define MCAP_MD_CREATE_MDL_RSP		0x02
#define MCAP_MD_RECONNECT_MDL_REQ	0x03
#define MCAP_MD_RECONNECT_MDL_RSP	0x04
#define MCAP_MD_ABORT_MDL_REQ		0x05
#define MCAP_MD_ABORT_MDL_RSP		0x06
#define MCAP_MD_DELETE_MDL_REQ		0x07
#define MCAP_MD_DELETE_MDL_RSP		0x08
#define MCAP_MD_SYNC_CAP_REQ			0x11
#define MCAP_MD_SYNC_CAP_RSP			0x12
#define MCAP_MD_SYNC_SET_REQ			0x13
#define MCAP_MD_SYNC_SET_RSP			0x14
#define MCAP_MD_SYNC_INF_IND			0x15
#define MCAP_MD_SYNC_RESERVED		0x20

	
/* End of McapOpCode */
	
/*---------------------------------------------------------------------------
* McapResponseCode type
*
*	MCAP Response codes
*
*/
typedef U8 McapResponseCode;
	
#define MCAP_SUCCESS				0x00
#define MCAP_INVALID_OP_CODE		0x01
#define MCAP_INVALID_PARAM_VALUE	0x02
#define MCAP_INVALID_MDEP			0x03
#define MCAP_MDEP_BUSY				0x04
#define MCAP_INVALID_MDL			0x05
#define MCAP_MDL_BUSY				0x06
#define MCAP_INVALID_OPERATION		0x07
#define MCAP_RESOURCE_UNAVAILABLE	0x08
#define MCAP_UNSPECIFIED_ERROR		0x09
#define MCAP_REQUEST_NOT_SUPPORTED	0x0A
#define MCAP_CONFIGURATION_REJECTED	0x0B
	
/* MDL IDs */
typedef U16 McapMdlId;

#define MCAP_MDLID_RESERVED		0x0000
#define MCAP_MDLID_INITIAL		0x0001
#define MCAP_MDLID_FINAL		0xFEFF
#define MCAP_ALL_MDLIDS			0xFFFF

/*MCL role*/
typedef U8 McapMclRole;

#define MCAP_MCL_ROLE_INITIATOR		0
#define MCAP_MCL_ROLE_ACCEPTOR		1


/* Forward reference to McapCallbackParms defined below */
typedef struct _McapCallbackParms McapCallbackParms;

/*---------------------------------------------------------------------------
 * McapCallback type
 *
 *     McapEvent and data are passed to the application through a callback
 *     function of this type defined by the application.
 */
typedef void (*McapCallback)(McapCallbackParms *parms);

/* End of McapCallback */


#define MCAP_RETRANSMISSION_ENABLE 0x01
#define MCAP_RETRANSMISSION_TIMES 0x01
#define MCAP_MAX_RETRANS_NUM	3


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
typedef struct
{
	LOCAL_PARA_HDR
	U16 mclPsm;
	U16 mdlPsm;
    BD_ADDR remoteAddr;
} bt_mcap_open_struct;

/*---------------------------------------------------------------------------
 * McapRegister structure
 *
 *      Used for profile who will register to MCAP
 */
typedef struct _McapRegister {
    McapUpperProfile	profile;		/*Operation code*/
	L2capPsmValue		mclPsm;			/*PSM for this MVL connection*/
	L2capPsmValue		mdlPsm;			/*PSM for this MDL connection*/
	McapCallback 		callback;		/*Callback for profile*/
}   McapRegister;


/*---------------------------------------------------------------------------
 * McapControlPacket structure
 *
 *      Describe the data packet that HDP send through MCL channel
 */
typedef struct _McapControlPacket {
	ListEntry			node;
    McapOpCode			opCode;			/*Operation code*/
    McapMdlId			mdlId;    		/* MDL ID*/
	McapResponseCode	responseCode;	/*Response code, valid in the RSP packet*/
	U8					configuration;	/*L2cap configuration in MD_CREATE_MDL_REQ*/
	U8					mdepId;		/*MDEP ID*/
}   McapControlPacket;

/*---------------------------------------------------------------------------
 * McapDataPacket structure
 *
 *      Describe the data packet that HDP send through MDL channel
 */
typedef struct _McapDataPacket {
    U8*       	data;       /* The pointer to the data */
    U16         dataLen;    /* Data length */
}   McapDataPacket;

/*---------------------------------------------------------------------------
 * MdlL2Configuration structure
 *
 *      Maintains the configuration parameters of the Data channel.
 */
typedef struct _MdlL2Configuration {
	U16			  minRemoteMtu;	
    L2capLinkMode inLinkMode;
}   MdlL2Configuration;

/*---------------------------------------------------------------------------
 * McapMdl structure
 *
 *      Maintains connection information for a MDL channel.
 */
typedef struct _McapMdl {
	ListEntry			node;
    McapMdlState       	mdlState;       /* The state of a MDL */
    L2capChannelId     	l2ChannelId;    /* L2CAP channel ID for connection */
	McapMdlId			mdlId;			/*The MDL ID*/
	U8					mdepId;			/*The MDEP ID*/
	L2capPsmValue		mdlPsm;			/*PSM for this MDL connection*/
	BOOL				delFlag;		/*Whether this MDL should be deleted*/
	BOOL				connFlag;		/*Whether this MDL initiate MDL connection*/
	BOOL				resetFlag;		/*Whether this data channel connection should be reset*/
	MdlL2Configuration 	l2config;		/*The configuration parameters of the Data channel*/
	
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
	ListEntry rxPacketsBufferList;
	ListEntry receivedPacketsList;
	BtRxPacket rfcRxPackets[MCAP_MAX_RETRANS_NUM]; //RF_NUM_RETRANSMISSION_WINDOW_SIZE];
	U8 l2capRawData[MCAP_MAX_RETRANS_NUM][L2CAP_MAXIMUM_MTU+20]; //RF_NUM_RETRANSMISSION_WINDOW_SIZE][RF_MAX_FRAME_SIZE+20];
	U8 txMaxCountPerIFrame[128];
	U32 txSrejList[RF_L2CAP_MAX_SREJ_SIZE];
#endif    

}   McapMdl;



/*---------------------------------------------------------------------------
 * McapMcl structure
 *
 *      Maintains connection information for a MCL channel.
 */
typedef struct _McapMcl {
	ListEntry			node;
    McapMclState       	mclState;       /* The state of a MCL */
    L2capChannelId      l2ChannelId;    /* L2CAP channel ID for connection */
    ListEntry       	mcapMdlList;    /* One or more MDLs for this MCL */
    BtRemoteDevice *	remDev;         /* Remote device of connection */
	U8					currentOp;		/*Current operation*/
	U8					currentCmd;		/*Current command*/
	U8					mclRole;		/*Initiator or acceptor of this MCL*/
	McapMdlId			nextMdlId;		/*MDL ID for the next MDL*/
	McapMdl*			pendingMdl;		/*The extra MDL that triiger MCL into PENDING state*/
	ListEntry			cmdQueue;			/*Queue the MCL commands*/
	BOOL			delAllMdlFlag;		/*Whether all MDL in this MCL should be delete*/	
	BOOL			delOneMdlFlag;		/*Whether all MDL in this MCL should be delete*/
		
    EvmTimer        	timer[MCAP_NUM_TIMERS]; /* Timers for Control commands */
    EvmTimer        	delTimer; /* Timer for disconnect Control channel */
	BOOL		DelTimerStart;
	
	BOOL				reopen;
	L2capPsmValue		remPsm;
	
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
	ListEntry rxPacketsBufferList;
	ListEntry receivedPacketsList;
	BtRxPacket rfcRxPackets[MCAP_MAX_RETRANS_NUM]; //RF_NUM_RETRANSMISSION_WINDOW_SIZE];
	U8 l2capRawData[MCAP_MAX_RETRANS_NUM][L2CAP_MAXIMUM_MTU+20]; //RF_NUM_RETRANSMISSION_WINDOW_SIZE][RF_MAX_FRAME_SIZE+20];
	U8 txMaxCountPerIFrame[128];
	U32 txSrejList[RF_L2CAP_MAX_SREJ_SIZE];
#endif    
}   McapMcl;

/*---------------------------------------------------------------------------
 * McapCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Fields in the BnepPanu structure are valid depending on the "event"
 *     field.
 */
struct _McapCallbackParms
{

    U16             status;

    McapEvent       event;      /* Type of MCAP event */

    /* Group: The following fields are used for connection management. */
    McapMcl			*mcl;       	
    McapMdl			*mdl;       
    BtRemoteDevice 	*remDev;     	

    /* Points to the sent BtPacket. Valid on MCAPEVENT_PACKET_HANDLED
     * events.
     */
    BtPacket       *txPkt;
	
    /* Packet of received MCAP packet. Valid on MCAPEVENT_PACKET_IND events.
     */
    McapDataPacket	*rxPkt;
	
	/*The configuration parameters of the Data channel. Valid on MCAPEVENT_MDLCONFIG_IND event*/
	U8				l2config;		
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MCAP_Init()
 *
 *     Initialize MCAP. This should be the first function called for MCAP.
 *
 *
 * Returns:
 *     TRUE - MCAP was successfully initialized.
 *
 *     FALSE  - MCAP failed to initialize.  
 */
BOOL MCAP_Init(void);

/*---------------------------------------------------------------------------
 * MCAP_DeInit()
 *
 *     De-initializes the MCAP layer.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - MCAP was successfully deinitialized.
 * 
 *     BT_STATUS_FAILED  - MCAP was not previously initialized
 *         (XA_ERROR_CHECK only).
 */
BtStatus MCAP_DeInit(void);

/*---------------------------------------------------------------------------
 * MCAP_Register()
 *
 *      Registers an application callback to receive MCAP events. This 
 *      function must be called before any other MCAP functions.
 *
 * Parameters:
 *      register - Identifies the application register struct contains a callback function
 *                   that will be called with MCAP events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The MCAP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_FAILED  - MCAP callback failed to register.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *      BT_STATUS_NO_RESOURCES - The MCAP callback was already registered. 
 */
BtStatus MCAP_Register(McapRegister* profileRegister);

/*---------------------------------------------------------------------------
 * MCAP_DeRegister()
 *
 *      De-registers the MCAP callback. After making this call
 *      successfully, the callback specified in MCAP_Register will
 *      receive no further events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The MCAP callback was successfully deregistered.
 * 
 *      BT_STATUS_FAILED  - A MCAP callback was not previously registered
 *         (XA_ERROR_CHECK only).
 */
BtStatus MCAP_DeRegister(McapUpperProfile profile);

/*---------------------------------------------------------------------------
 * MCAP_OpenMcl()
 *
 *      Opens a MCAP MCL with a remote device. After getting
 *      a response from the remote device, the callback function will receive
 *      a MCAPEVENT_MCLOPENED event.
 *
 * Parameters:
 *      remDev - Identifies the remote device. 
 *      mclPsm - Remote device MCL PSM used to L2CAP connection.
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to start the MCAP MCL 
 *         service was successful.
 *
 *      BT_STATUS_FAILED  - Either MCAP_Register() was not called first or
 *         the MCAP MCL has already been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_OpenMcl(BtRemoteDevice *remDev, L2capPsmValue mclPsm);

/*---------------------------------------------------------------------------
 * MCAP_CloseMcl()
 *
 *      CLose a MCAP MCL with a remote device. 
 *
 * Parameters:
 *      mcl - Identifies the MCL. 
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to close the MCAP MCL 
 *         service was successful.
 *
 *      BT_STATUS_FAILED  - Fail to close the MCL
 *
 */
BtStatus MCAP_CloseMcl(McapMcl* mcl);


/*---------------------------------------------------------------------------
 * MCAP_Config()
 *
 *      Config a MCAP MDL L2CAP parameters in the given MCL. This will trigger MCL send MD_CREATE_MDL_REQ
 *      packet in the control channel. After getting
 *      a response from the remote device, the callback function will receive
 *      a MCAPEVENT_MDLCONFIG_CNF event.
 *
 * Parameters:
 *      mcl - Identifies the MCL. 
 *	  mdepId - MDEP ID
 *      configuration - The configuration parameters for L2CAP
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to start the MCAP MDL config
 *         service was successful.
 *
 *      BT_STATUS_FAILED  - Either MCAP_Register() was not called first or
 *         the MCAP MCL has not been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_Config(McapMcl *mcl, U8 mdepId, U8 configuration);


/*---------------------------------------------------------------------------
 * MCAP_OpenMdl()
 *
 *      Opens a MCAP MDL in the given MCL. This function will be called when
 *      configuration has been done. After getting
 *      a response from the remote device, the callback function will receive
 *      a MCAPEVENT_MDLOPENED event.
 *
 * Parameters:
 *      mdl - Identifies the MDL. 
 *      mdlPsm - Remote device MDL PSM used to L2CAP connection.
 *	  l2config - Configuration for open a L2CAP connection
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to start the MCAP MDL 
 *         service was successful.
 *
 *      BT_STATUS_FAILED  - Either MCAP_Register() was not called first or
 *         the MCAP MCL has not been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_OpenMdl(McapMdl *mdl, L2capPsmValue mdlPsm, BtRemoteDevice *remDev, MdlL2Configuration* l2config);

/*---------------------------------------------------------------------------
 * MCAP_CloseMdl()
 *
 *      Close a MCAP MDL in the given MCL. This will trigger MCL send MD_DELETE_MDL_REQ
 *      packet in the control channel. After getting
 *      a response from the remote device, the callback function will receive
 *      a MCAPEVENT_MDLCLOSED event.

 *
 * Parameters:
 *      mcl - Identifies the MCL. 
 *      mdl - Identifies the MDL that will be closed. 
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to close the MCAP MDL 
 *         connection was successful.
 *
 *      BT_STATUS_FAILED  - The MCAP MDL has not been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_CloseMdl(McapMcl *mcl, McapMdl *mdl);


/*---------------------------------------------------------------------------
 * MCAP_AcceptMcl()
 *
 *      Accepts a MCAP MCL connection from a remote device in response to a
 *      MCAPEVENT_MCLOPEN_IND event. After completing the open, the callback
 *      function will receive a MCAPEVENT_MCLOPENED event.
 *
 * Parameters:
 *      mcl - A pointer to a new MCL structure that will be used to
 *            maintain this MCL connection. 
 *      result - Accept or reject the connection request.
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to accept the MCAP MCL 
 *         connection was successful.
 *
 *      BT_STATUS_FAILED  - Either MCAP_Register() was not called first or
 *         the MCAP MCL has already been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_AcceptMcl(McapMcl *mcl, BOOL result);

/*---------------------------------------------------------------------------
 * MCAP_AcceptMdl()
 *
 *      Accepts a MCAP MDL connection from a remote device in response to a
 *      MCAPEVENT_MDLCONFIG_IND event. 
 *
 * Parameters:
 *      mcl - A pointer to a MCL structure that contains the MDL
 *	   mdl - A pointer to a new MDL structure that will be used to
 *            maintain this MDL connection. 
 *      result - Accept or reject the connection request.
 *	  configuration - The handled L2cap configuration
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to accept the MCAP MDL 
 *         connection was successful.
 *
 *      BT_STATUS_FAILED  - Either MCAP_Register() was not called first or
 *         the MCAP MDL has already been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_AcceptMdl(McapMcl *mcl, McapMdl *mdl, BOOL result, U8 configuration);


/*---------------------------------------------------------------------------
 * MCAP_AbortMdl()
 *
 *		The connection of an MDL is a two-step operation (the Create or 
 *		Reconnect operation, followed by the actual opening of the Data 
 *		Channel), and the Abort operation is provided for the special case 
 *		when this two-step operation should be cancelled after the first
 *		half has completed successfully.
 *
 * Parameters:
  *      mcl - A pointer to a MCL structure that contains the MDL
 *	   mdl - A pointer to a MDL structure whose connection will be abort
 *
 * Returns:
 *      BT_STATUS_PENDING - The process aborting the MCAP 
 *         MDL was successful.
 *
 *      BT_STATUS_FAILED  - MCL is not in pending state. 
 
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_AbortMdl(McapMcl *mcl, McapMdl *mdl);

/*---------------------------------------------------------------------------
 * MCAP_ReconnectMdl()
 *
 *		Reconnect the MDL without configuration first after a unintentionally broken
 *
 * Parameters:
  *      mcl - A pointer to a MCL structure that contains the MDL
 *	   mdl - A pointer to a MDL structure whose connection will be reconnect
 *
 * Returns:
 *      BT_STATUS_PENDING - The process aborting the MCAP 
 *         MDL was successful.
 *
 *      BT_STATUS_FAILED  - The configuration of MDL does not exost 
 
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus MCAP_ReconnectMdl(McapMcl *mcl, McapMdl *mdl);


/*---------------------------------------------------------------------------
 * MCAP_SendData()
 *
 *      Send an Ethernet packet to the remote device. The etherPkt parameter
 *      points to a valid ethernet packet to be encapsulated in the MCAP
 *      packet. 

 *
 * Parameters:
 *      mdl - Identifies the MCAP MDL channel that owns the packet.
 *      btPacket - A pointer to the BtPacket used to send the Ethernet packet
 *                 through L2CAP.
 *      txPkt - A pointer to the packet to send.
 *
 * Returns:    
 *      BT_STATUS_PENDING - The send packet process has been successfully 
 *          started.
 *
 *      BT_STATUS_INVALID_PARM - An invalid parameter was used.
 *
 *      BT_STATUS_FAILED - L2CAP_Send failure.
 */
BtStatus MCAP_SendData(McapMdl *mdl,
                           BtPacket *btPacket,
                           McapDataPacket *txPkt);


#endif /* __MCAP_H */

#endif /*__BT_MCAP__*/

