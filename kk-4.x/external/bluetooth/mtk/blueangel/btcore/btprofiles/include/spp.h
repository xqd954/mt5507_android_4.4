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

#ifndef __SPP_H
#define __SPP_H
/***************************************************************************
 *
 * File:
 *     $Workfile:spp.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *      Private header file for the Serial Port Profile. 
 *      This file is meant to be included by modules in the portable
 *      SPP and the OS-specific portion of the SPP (SPPOS). It defines
 *      the portable API, data types, and defines presented and required by
 *      the portable code.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "utils.h"
#include "rfcomm_adp.h"
#include "ring.h"
#include "sdp_adp.h"
#include "conmgr.h"
#include "bt_common.h"
//#define CUSTOM_SPP XA_ENABLED
/*---------------------------------------------------------------------------
 * Serial Port Profile (SPP) layer
 *
 *     The Serial Port Profile (SPP) specification defines procedures
 *     and protocols for Bluetooth devices emulating RS232 or other serial
 *     connections.
 */

/****************************************************************************
 *
 * Section: Configuration Constants
 *
 * The following defines are configuration constants that allow
 * an implementer to include/exclude functionality from SPP.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
/* SPP_Open has not been called or SPP_Closed was last called */
#define DEVICE_STATE_CLOSED         0

/* SPP_Open has been called but the device is not connected. */
#define DEVICE_STATE_DISCONNECTED   1

/* The device is opened and connected to a remote device. */
#define DEVICE_STATE_CONNECTED      2

/* 
 * Bingyi 20091223: SPP_Closed has been called but the RFCOMM channel has not been closed (Server). 
 * De-register the RFCOMM channel after it has been closed.
 */
#define DEVICE_STATE_DEACTIVATING         3

/*---------------------------------------------------------------------------
 * SppPortType type
 * 
 *     Identifies the serial port as a client (outgoing) or server (incoming). 
 */
typedef U8 SppPortType;

#define SPP_SERVER_PORT      0x01   /* SPP/DUN/FAX server port */
#define SPP_CLIENT_PORT      0x02   /* SPP/DUN/FAX client port */

#define SPP_NO_SCO      0x00
#define SPP_WITH_SCO      0x01

/* End of SppPortType */

#if SPP_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SppService structure
 *
 *      Servers need this data type for registering with RFCOMM. A particular
 *      service may have one or more ports available. For example, a device
 *      might provide access to 2 modems. It is providing a single service
 *      (modem) via 2 serial ports.
 */
typedef struct _SppService
{
#ifdef MULTI_RF_CHANNEL
    RfService *rfService;      /* Required for registering service w/RFCOMM */    
#else
    RfService service;      /* Required for registering service w/RFCOMM */    
#endif
    const U8 *name;         /* name of service */
    U16 nameLen;            /* Length of name */
    U16 numPorts;           /* Number of ports in this service */
    SdpRecord *sdpRecord;   /* Description of the service */
#if (SPP_SCO_SERVER == XA_ENABLED)
    CmgrHandler cmgr_hdlr;
    CmgrCallback cmgrcallback;
#endif /* (SPP_SCO_SERVER == XA_ENABLED) */ 
} SppService;

#ifdef MULTI_RF_CHANNEL
typedef struct _RfServicePack
{
    U8 refCount;
    RfService service;    
} RfServicePack;
#endif

#endif /* SPP_SERVER == XA_ENABLED */

#if SPP_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SppClient structure
 *      Contains all fields unique to clients. SppClient is a data type in
 *      device structures (SppDev) for clients.
 */
typedef struct _SppClient
{
    /* === Internal use only === */
    BtRemoteDevice *remDev;
    U8 serverId;
    SdpQueryToken sdpToken;
    /* Add the following fields for SPP client to connect with others */
    CmgrHandler cmgr_hdlr;
    CmgrCallback cmgrcallback;
    kal_bool in_use;
    /* U8           state; */
    U32 flag;
} SppClient;
#endif /* SPP_CLIENT == XA_ENABLED */ 

/* Flag#1 */
#define RECV_SPPC_ACTIVATE_REQ   0x00000001
/* Flag#2 */
#define SEND_SPPC_ACTIVATE_CNF   0x00000002
/* Flag#3 CON_REQ from MMI */
#define RECV_SPPC_CONNECT_REQ    0x00000004
/* Flag#4 */
#define SEND_SPPC_ACL_LINK_CONNECT_REQ 0x00000080
/* Flag#5 */
#define RECV_SPPC_ACL_LINK_CONNECT_CNF 0x00000010
/* Flag#6 */
#define SEND_SPPC_SDP_REQ     0x00000020
/* Flag#7 */
#define RECV_SPPC_SDP_RSP     0x00000040
/* Flag#8 SPPC establishs RFCOMM channel */
#define SEND_SPPC_SERVICE_REQ 0x00000080
/* Flag#9 */
#define RECV_SPPC_SERVICE_RSP 0x00000100
/* Flag#10 CON_CNF to MMI */
#define SEND_SPPC_CONNECT_CNF    0x00000200
/* Flag#11 DISC_REQ from MMI */
#define RECV_SPPC_DISCONNECT_REQ 0x00000400
/* Flag#12 DISC_CNF to MMI */
#define SEND_SPPC_DISCONNECT_CNF 0x00000800
/* Flag#13 SPPC closes RFCOMM channel */
#define SEND_SPPC_SERVICE_DISC_REQ 0x00001000
/* Flag#14 SPPC receives RF_CLOSED event */
#define RECV_SPPC_SERVICE_DISC_CNF 0x00002000
/* Flag#15 */
#define RECV_SPPC_DEACTIVATE_REQ 0x00004000
/* Flag#16 */
#define SEND_SPPC_DEACTIVATE_CNF  0x00008000
/* Flag#17: DISC ACL LINK Due to error happen */
#define SEND_SPP_DISCLINK_REQ    0x00010000
/* Flag#18 */
#define RECV_SPP_DISCLINK_CNF    0x00020000

#define SCO_CMD_NONE    0x00
#define SCO_CMD_INIT    0x01

/* Currently: The following codes are not used, disabled them */
#if 0
#define __SPP_XONOFF_CHAR__
#define __SPP_LINE_STATUS__
#define __SPP_MSR__
#define __SPP_BREAKLEN__
#define __SPP_GET_REMOTE_MODEM_STATUS__
#endif /* 0 */ 

/*---------------------------------------------------------------------------
 * SppDev structure
 *      This structure defines an SPP device. A user of SPP must allocate
 *      one SppDev for each serial device. Members should not be accessed
 *      directly.
 */
typedef struct _SppDev
{
    /*
     * Pointer to platform-specific data. This pointer is passed to all
     * platform-specific functions (prefaced SPPOS_).
     */
    void *osDev;

    /* === Internal use only === */

    /* Server / Client elements */
    SppPortType portType;   /* SPP_SERVER_PORT or SPP_CLIENT_PORT */
#if ((SPP_SCO_SERVER == XA_ENABLED) || (SPP_SCO_CLIENT == XA_ENABLED))
    U8 scoType;
    U8 scoInitCmd;
    EvmTimer rssi_timer;
#endif /* ((SPP_SCO_SERVER == XA_ENABLED) || (SPP_SCO_CLIENT == XA_ENABLED)) */ 
    U8 state;               /* device state */
    RfChannel channel;

#ifdef __SPP_GET_REMOTE_MODEM_STATUS__
    RfModemStatus rModemStatus; /* remote modem status */
#endif 

#if defined (__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
    RfModemStatus lModemStatus; /* local modem status */
#endif 

#ifdef __SPP_LINE_STATUS__
    RfLineStatus lineStatus;
#endif 
    RfPortSettings portSettings;

#ifdef __SPP_XONOFF_CHAR__
    U8 xonChar;
    U8 xoffChar;
#endif /* __SPP_XONOFF_CHAR__ */ 

    S16 highWater;      /* when rRing.dataLen >= highWater,
                           flow off rx */
    U8 credit;          /* rx credits outstanding */
#ifdef __SPP_MSR__
    U8 msr;             /* Modem Status Register */
#endif 
#ifdef __SPP_BREAKLEN__
    S16 breakLen;
#endif 
    /* Transmit packet */
    ListEntry txPacketList;
#ifdef __SPP_SLEEP__ 
    /* To reduce TX rate for some slow PC */
    EvmTimer sppSleep;  /* Sleep timer for TX */
    U8 txCount;         /* TX counter */
#endif /* __SPP_SLEEP__    */    
    U8 con_id;
    U8 con_id_flag;    /* record con_id is allocated or not */
    /* Fields specific to clients and servers */
#if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)
    union
    {
    #if SPP_CLIENT == XA_ENABLED
        SppClient client;
    #endif 
    #if SPP_SERVER == XA_ENABLED
        SppService *sppService;
    #endif 
    } type;
#endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */ 
    /* For Server: This is the specific UUID used to register SDP record 
     * For Client: This is the remote Server UUID that is assigned through Client_Connect 
     */
    U16 uuid;
} SppDev;

/*
 * If no FAX service is existed: 2 SPP Server (sppOsDev[0],sppOsDev[1]), 1 DUN Server(sppOsDev[2]), 1 SPP Client(sppOsDev[3])
 * else 1 SPP server(sppOsDev[0]), 1 FAX server(sppOsDev[1]), 1 DUN server(sppOsDev[2]), 1 SPP Client(sppOsDev[3])
 */

/* All SPP_DEVS use the port's range: from "BT_START_OF_VIRTUAL_PORT" to "BT_START_OF_VIRTUAL_PORT + NUM_SPP_DEVS -1" */
/* And the hf_port(and hfg profile will not use the sppOsDev[] structure) is the port number = (BT_START_OF_VIRTUAL_PORT + NUM_SPP_DEVS) and hf_port <= BT_END_OF_VIRTUAL_PORT */
/* BT_START_OF_VIRTUAL_PORT and BT_END_OF_VIRTUAL_PORT can be defined in mcu\interface\hwdrv\uart_sw.h */

#define NUM_SPP_DEVS        NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS + NUM_FAX_SRVS + NUM_SPP_CLIS + NUM_DUN_CLIS + NUM_SPP_SCO_CLIS + NUM_FAX_CLIS

#ifdef __SPP_THROUGHPUT_TEST__
#define SPP_RX_CREDIT (24)
#else
#define SPP_RX_CREDIT (6)
#endif
/* After BT v2.1: new sap for MMI AUTHORIZE is applied */
#define BT_SPP_AUTHORIZE_BY_MMI 1
/* High water mark value (ie. RF_FRAME_SIZE*SPP_RX_CREDIT/2) must be larger than one RF_FRAME_SIZE */
#if (SPP_RX_CREDIT < 2)
#error For High Water Mark check, SPP_RX_CREDIT must be larger than 2
#endif 

#if 0
/* Define Error code for SPP Client connection */
#define SPPC_CONNECT_SUCCESS       0x0000
#define SPPC_CREATE_ACL_LINK_ERROR 0x0001
#define SPPC_SDP_QUERY_REQ_FAILED  0x0002
#define SPPC_SDP_QUERY_REQ_INVALID_PARM 0x0003
#define SPPC_SDP_QUERY_RSP_FAILED 0x0004
#define SPPC_SERVICE_LEVEL_CON_FAILED 0x0005
#define SPPC_NOT_CONNECTED_BUT_RECV_CLOSED 0x0006
#define SPPC_NO_AVAILABLE_CLI_DEVS 0x0007

/* Define Error code for SPP Client/Server disconnection: 2007-0711 */
#define SPP_DISCONNECT_SUCCESS 0x0000
#define NOT_SUCCESS_RETURN_FROM_SPP_DISC_FUNC 0x0001
#endif

#define SPP_RSSI_TIMER_PERIOD CUSTOM_SPP_SCO_READ_RSSI_TIMER_PERIOD_IN_MS
#ifdef __BT_SPP_GENERAL__
#define BT_START_OF_VIRTUAL_PORT (10)
#define	BT_END_OF_VIRTUAL_PORT (10+VIRTUAL_PORTS_NUM-1)
#else /* end of __BT_SPP_GENERAL__ */
#define BT_START_OF_VIRTUAL_PORT (start_of_virtual_port)
#define	BT_END_OF_VIRTUAL_PORT (end_of_virtual_port)
#endif /* end of __BT_SPP_GENERAL__ */
extern kal_uint8 MMI_DUN_CLIENT_TEST;

#define ASSERT(expr) {if(!(expr)) {} else {}};

/* Bingyi */
#define SPP_UART_GET_BYTE_BUFFER_LENGTH 1024

/*----------------------------------------------------------------------
 * SPP_DEV_OS type
 *      Serial Port Profile Device structure for Windows implementation.
 *      This structure contains all the fields required for a serial 
 *      device as implemented by the Serial Port Profile. The first member
 *      MUST be of type SppDev. Any other members are for use by 
 *      implementation or OS-specific code.
 */
typedef struct _SPP_DEV_OS
{
    SppDev sppDev;  /* MUST be the first member */

    /* OS-specific members */
    char *name;
    RingBuf *rRing;                 /* pointer to read buffer */
    RingBuf *wRing;                 /* pointer to write buffer */
    UART_PORT port;
    module_type owner;              /*  Owner of the port */
    kal_uint8 profile;              /* 0: SPP, 1: DUN, 2: HFG */
    kal_bool is_tx_empty;           /* True: tx empty */
    kal_bool notifyReadyToWrite;    /* This flag is only set by sender task: It regards the buffer is full from its view */
#if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
    kal_bool dcd_valid;
    kal_bool dcd;
#endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */ 
    kal_bool readyToRead;
    kal_bool set_auth_req_mode;     /* TRUE: This means that SPP_REQ should be allowed by upper layer: 2007-01-15 */
    kal_bool wait_first_received_data_in_openstate;
    kal_bool wait_plugout_cnf_msg;
    U16 wait_uart_data_available_ind_rsp_length;	// Record if UART_DATA_AVAILABLE_IND has been sent to MMI
#ifdef __SPP_SHARED_MEMORY__
    //U8 *putMemPtr;	/* Shared memory pointer of Put data */
    U8 *getMemPtr;	/* Shared memory pointer of Get data */
    U16 getMemSize;	/* Shared memory size of Get data */
#endif	/* __SPP_SHARED_MEMORY__ */
} SPP_DEV_OS;

/* End of SPP_DEV_OS */

typedef enum
{
    /*APP <-- SPP*/
    SPP_EVENT_CONNECT_IND,
    SPP_EVENT_CONNECT_IND_REQ,
    SPP_EVENT_DISCONNECT_IND,
    SPP_EVENT_ACTIVATE_CNF,
    SPP_EVENT_DEACTIVATE_CNF,
    SPP_EVENT_DISCONNECT_CNF,
    SPP_EVENT_AUTH_REQ,
    SPP_EVENT_AUDIO_CONNECT_CNF,
    SPP_EVENT_AUDIO_CONNECT_IND,
    SPP_EVENT_AUDIO_DISCONNECT_CNF,
    SPP_EVENT_AUDIO_DISCONNECT_IND,
    SPP_EVENT_SCO_RSSI_IND,
    SPP_EVENT_CONNECT_CNF, 
    SPP_EVENT_UART_OWNER_IND,

    /*APP <-- DUN*/
    DUN_EVENT_CONNECT_IND,
    DUN_EVENT_CONNECT_IND_REQ,
    DUN_EVENT_DISCONNECT_IND,
    DUN_EVENT_ACTIVATE_CNF,
    DUN_EVENT_DEACTIVATE_CNF,
    DUN_EVENT_DISCONNECT_CNF,
    DUN_EVENT_CONNECT_CNF,

    /* APP <-- FAX */
    FAX_EVENT_CONNECT_IND, 
    FAX_EVENT_CONNECT_IND_REQ, 
    FAX_EVENT_DISCONNECT_IND, 
    FAX_EVENT_ACTIVATE_CNF, 
    FAX_EVENT_DEACTIVATE_CNF, 
    FAX_EVENT_DISCONNECT_CNF, 
    FAX_EVENT_CONNECT_CNF, 

    /* UART event */
    UART_EVENT_PLUGOUT_IND,
    UART_EVENT_READY_TO_READ_IND,
    UART_EVENT_READY_TO_WRITE_IND, 

    /* SW Partition */
    SPP_UART_EVENT_INITIALIZE_CNF,
    SPP_UART_EVENT_REGISTER_CALLBACK_CNF, 
    
    /* UART synchronous API event */
    SPP_UART_EVENT_PUT_BYTES_CNF, 
    SPP_UART_EVENT_GET_BYTES_CNF, 
    SPP_UART_EVENT_OPEN_CNF, 
    SPP_UART_EVENT_CLOSE_CNF, 
    SPP_UART_EVENT_GET_BYTES_AVAIL_CNF, 
    SPP_UART_EVENT_GET_TX_ROOM_LEFT_CNF, 
    SPP_UART_EVENT_SET_OWNER_CNF, 
    SPP_UART_EVENT_CTRL_DCD_CNF, 
    SPP_UART_EVENT_CTRL_BREAK_CNF, 
    SPP_UART_EVENT_GET_OWNER_ID_CNF, 
    SPP_UART_EVENT_CLR_RX_BUFFER_CNF, 
    SPP_UART_EVENT_CLR_TX_BUFFER_CNF,

#ifdef __SPP_SHARED_MEMORY__
    SPP_UART_EVENT_ASSIGN_BUFFER_CNF,
#endif	/* __SPP_SHARED_MEMORY__ */

    /* SPP <-- SPP */
    /* Event that is sent to profile itself to trigger some action */
    SPP_EVENT_PROFILE_SEND_DATA_REQ, 
    SPP_EVENT_PROFILE_GET_DATA_REQ, 

    /* Customization for WISE */
    SPP_EVENT_ENABLE_CNF, 
    SPP_EVENT_DISABLE_CNF, 
    SPP_UART_EVENT_DATA_AVAILABLE_IND
} spp_event;

typedef U8 SPPEvent;
typedef struct _SPP_event_connect_ind_struct
{        
    UART_PORT port;     /* virtual port number */
    kal_uint8 cid;      /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;   
} SPP_event_connect_ind_struct;

typedef struct _SPP_event_connect_ind_req_struct
{        
    UART_PORT port;     /* virtual port number */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;   
} SPP_event_connect_ind_req_struct;

typedef struct _SPP_event_disconnect_ind_struct
{        
    UART_PORT port; /* virtual port number */
} SPP_event_disconnect_ind_struct;

typedef struct _SPP_event_disconnect_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    kal_uint8 cid;      /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
    kal_uint8 result;   /* This is result field of this returned cnf msg */
} SPP_event_disconnect_cnf_struct;

typedef struct _SPP_event_auth_req_struct
{        
    UART_PORT port; /* virtual port number */
} SPP_event_auth_req_struct;

typedef struct _SPP_event_audio_connect_cnf_struct
{        
    UART_PORT port; /* virtual port number */
    kal_uint8 result;
} SPP_event_audio_connect_cnf_struct;

typedef struct _SPP_event_audio_connect_ind_struct
{        
    UART_PORT port; /* virtual port number */
} SPP_event_audio_connect_ind_struct;

typedef struct _SPP_event_audio_disconnect_cnf_struct
{        
    UART_PORT port; /* virtual port number */
    kal_uint8 result;
} SPP_event_audio_disconnect_cnf_struct;

typedef struct _SPP_event_audio_disconnect_ind_struct
{        
    UART_PORT port; /* virtual port number */
} SPP_event_audio_disconnect_ind_struct;

typedef struct _SPP_event_sco_rssi_ind_struct
{        
    kal_uint8 bd_address[6];
    kal_uint8 rssi_value;
} SPP_event_sco_rssi_ind_struct;

typedef struct _SPP_event_connect_cnf_struct
{        
    UART_PORT port;             /* virtual port number */
    kal_uint8 cid;              /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
    kal_uint8 result;           /* This is result field of this returned cnf msg */
    kal_uint8 server_chnl_num;  
} SPP_event_connect_cnf_struct;

typedef struct _SPP_event_uart_owner_ind_struct
{        
    UART_PORT port;
} SPP_event_uart_owner_ind_struct;

/*APP <-- DUN*/
typedef struct _DUN_event_connect_ind_struct
{        
    UART_PORT port; /* virtual port number */
    kal_uint8 cid;  /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} DUN_event_connect_ind_struct;

typedef struct _DUN_event_connect_ind_req_struct
{        
    UART_PORT port; /* virtual port number */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} DUN_event_connect_ind_req_struct;

typedef struct _DUN_event_disconnect_ind_struct
{        
    UART_PORT port; /* virtual port number */
} DUN_event_disconnect_ind_struct;

typedef struct _DUN_event_disconnect_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    kal_uint8 cid;      /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
    kal_uint8 result;   /* This is result field of this returned cnf msg */
} DUN_event_disconnect_cnf_struct;

typedef struct _DUN_event_connect_cnf_struct
{        
    UART_PORT port;             /* virtual port number */
    kal_uint8 cid;              /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
    kal_uint8 result;           /* This is result field of this returned cnf msg */
    kal_uint8 server_chnl_num;  
} DUN_event_connect_cnf_struct;

/* APP <-- FAX */
typedef struct _FAX_event_connect_ind_struct
{        
    UART_PORT port; /* virtual port number */
    kal_uint8 cid;  /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} FAX_event_connect_ind_struct;

typedef struct _FAX_event_connect_ind_req_struct
{        
    UART_PORT port; /* virtual port number */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} FAX_event_connect_ind_req_struct;

typedef struct _FAX_event_disconnect_ind_struct
{        
    UART_PORT port; /* virtual port number */
} FAX_event_disconnect_ind_struct;

typedef struct _FAX_event_disconnect_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    kal_uint8 cid;      /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
    kal_uint8 result;   /* This is result field of this returned cnf msg */
} FAX_event_disconnect_cnf_struct;

typedef struct _FAX_event_connect_cnf_struct
{        
    UART_PORT port;             /* virtual port number */
    kal_uint8 cid;              /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
    kal_uint8 result;           /* This is result field of this returned cnf msg */
    kal_uint8 server_chnl_num;  
} FAX_event_connect_cnf_struct;

/* UART */
typedef struct _UART_event_plugout_struct
{        
	UART_PORT 	port;
	U8 profile;
} UART_event_plugout_struct;

typedef struct _UART_event_ready_to_read_struct
{        
	UART_PORT 	port;
	U8 profile;
} UART_event_ready_to_read_struct;

typedef struct _UART_event_ready_to_writ_struct
{        
	UART_PORT 	port;
	U8 profile;
} UART_event_ready_to_write_struct;

/* SW Partition */
typedef struct _SPP_event_initialize_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_event_initialize_cnf_struct;

typedef struct _SPP_event_register_callback_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_event_register_callback_cnf_struct;

/* UART synchronous API event */
typedef struct _SPP_UART_event_put_bytes_cnf_struct
{
	UART_PORT	port;
	U16 length;
	U8 profile;
}SPP_UART_event_put_bytes_cnf_struct;

typedef struct _SPP_UART_event_get_bytes_cnf_struct
{
	UART_PORT	port;
	U16 length;
#ifdef __SPP_SHARED_MEMORY__
	U8 *buffer;
#else
	U8 buffer[SPP_UART_GET_BYTE_BUFFER_LENGTH];
#endif	/* __SPP_SHARED_MEMORY__ */
	U8 profile;
}SPP_UART_event_get_bytes_cnf_struct;

typedef struct _SPP_UART_event_open_cnf_struct
{
	UART_PORT	port;
	BOOL result;	
	U8 profile;
}SPP_UART_event_open_cnf_struct;

typedef struct _SPP_UART_event_close_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_UART_event_close_cnf_struct;

typedef struct _SPP_UART_event_get_bytes_avail_cnf_struct
{
	UART_PORT	port;
	U16 length;
	U8 profile;
}SPP_UART_event_get_bytes_avail_cnf_struct;

typedef struct _SPP_UART_event_get_tx_room_left_cnf_struct
{
	UART_PORT	port;
	U16 length;
	U8 profile;
}SPP_UART_event_get_tx_room_left_cnf_struct;

typedef struct _SPP_UART_event_set_owner_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_UART_event_set_owner_cnf_struct;

typedef struct _SPP_UART_event_ctrl_dcd_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_UART_event_ctrl_dcd_cnf_struct;

typedef struct _SPP_UART_event_ctrl_break_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_UART_event_ctrl_break_cnf_struct;

typedef struct _SPP_UART_event_get_owner_id_cnf_struct
{
	UART_PORT	port;
	module_type owner;
	U8 profile;
}SPP_UART_event_get_owner_id_cnf_struct;

typedef struct _SPP_UART_event_clr_rx_buffer_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_UART_event_clr_rx_buffer_cnf_struct;

typedef struct _SPP_UART_event_clr_tx_buffer_cnf_struct
{
	UART_PORT	port;
	U8 profile;
}SPP_UART_event_clr_tx_buffer_cnf_struct;

typedef struct _SPP_UART_event_data_available_ind_struct
{
	UART_PORT	port;
	U16 length;
#ifdef __SPP_SHARED_MEMORY__
	U8 *buffer;
#else
	U8 buffer[SPP_UART_GET_BYTE_BUFFER_LENGTH];
#endif	/* __SPP_SHARED_MEMORY__ */
	U8 profile;
}SPP_UART_event_data_available_ind_struct;

typedef struct _SPP_event_profile_send_data_req_struct
{
	UART_PORT	port;
}SPP_event_profile_send_data_req_struct;

typedef struct _SPP_event_profile_get_data_req_struct
{
	UART_PORT	port;
}SPP_event_profile_get_data_req_struct;

// Only for MAUI
typedef struct
{
    UART_PORT port;
} SPP_event_hf_send_data_req_struct;

typedef struct _SPP_event_activate_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    BT_BOOL result;
    
} SPP_event_activate_cnf_struct;

typedef struct _SPP_event_deactivate_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    BT_BOOL result;
} SPP_event_deactivate_cnf_struct;

typedef struct _DUN_event_activate_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    BT_BOOL result;
    
} DUN_event_activate_cnf_struct;

typedef struct _DUN_event_deactivate_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    BT_BOOL result;
} DUN_event_deactivate_cnf_struct;

typedef struct _FAX_event_activate_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    BT_BOOL result;
    
} FAX_event_activate_cnf_struct;

typedef struct _FAX_event_deactivate_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
    BT_BOOL result;
} FAX_event_deactivate_cnf_struct;

#ifdef __SPP_SHARED_MEMORY__
typedef struct _SPP_UART_event_assign_buffer_cnf_struct
{        
    UART_PORT port;     /* virtual port number */
} SPP_UART_event_assign_buffer_cnf_struct;
#endif	/* __SPP_SHARED_MEMORY__ */


typedef struct _SPPCallbackParms {
    SPPEvent    event;   /* Event associated with the callback       */
    module_type dstMod;    
    union {
        SPP_event_connect_ind_struct            spp_connect_ind;
        SPP_event_connect_ind_req_struct        spp_connect_ind_req;
        SPP_event_disconnect_ind_struct         spp_disconnect_ind;
        SPP_event_disconnect_cnf_struct         spp_disconnect_cnf;
        SPP_event_auth_req_struct               spp_auth_req;
        SPP_event_audio_connect_cnf_struct      spp_audio_connect_cnf;
        SPP_event_audio_connect_ind_struct      spp_audio_connect_ind;
        SPP_event_audio_disconnect_cnf_struct   spp_audio_disconnect_cnf;
        SPP_event_audio_disconnect_ind_struct   spp_audio_disconnect_ind;
        SPP_event_sco_rssi_ind_struct           spp_sco_rssi_ind;
        SPP_event_connect_cnf_struct            spp_connect_cnf;
        SPP_event_uart_owner_ind_struct         spp_uart_owner_ind; 
        /* DUN -> APP */
        DUN_event_connect_ind_struct            dun_connect_ind;
        DUN_event_connect_ind_req_struct        dun_connect_ind_req;
        DUN_event_disconnect_ind_struct         dun_disconnect_ind;
        DUN_event_disconnect_cnf_struct         dun_disconnect_cnf;
        DUN_event_connect_cnf_struct            dun_connect_cnf;
        /* FAX -> APP */
        FAX_event_connect_ind_struct            fax_connect_ind;
        FAX_event_connect_ind_req_struct        fax_connect_ind_req;
        FAX_event_disconnect_ind_struct         fax_disconnect_ind;
        FAX_event_disconnect_cnf_struct         fax_disconnect_cnf;
        FAX_event_connect_cnf_struct            fax_connect_cnf;
        /* UART */
        UART_event_plugout_struct               uart_plugout_ind;
        UART_event_ready_to_read_struct         uart_ready_to_read;
        UART_event_ready_to_write_struct        uart_ready_to_write;
        /* SW Partition */
        SPP_event_initialize_cnf_struct spp_initialize_cnf;
        SPP_event_register_callback_cnf_struct spp_register_callback_cnf;
	 /* UART synchronous API event */
	 SPP_UART_event_put_bytes_cnf_struct spp_uart_put_bytes_cnf;
	 SPP_UART_event_get_bytes_cnf_struct spp_uart_get_bytes_cnf;
	 SPP_UART_event_open_cnf_struct spp_uart_open_cnf;
	 SPP_UART_event_close_cnf_struct spp_uart_close_cnf;
	 SPP_UART_event_get_bytes_avail_cnf_struct spp_uart_get_bytes_avail_cnf;
	 SPP_UART_event_get_tx_room_left_cnf_struct spp_uart_get_tx_room_left_cnf;
	 SPP_UART_event_set_owner_cnf_struct spp_uart_set_owner_cnf;
	 SPP_UART_event_ctrl_dcd_cnf_struct spp_uart_ctrl_dcd_cnf;
	 SPP_UART_event_ctrl_break_cnf_struct spp_uart_ctrl_break_cnf;
	 SPP_UART_event_get_owner_id_cnf_struct spp_uart_get_owner_id_cnf;
	 SPP_UART_event_clr_rx_buffer_cnf_struct spp_uart_clr_rx_buffer_cnf;
	 SPP_UART_event_clr_tx_buffer_cnf_struct spp_uart_clr_tx_buffer_cnf;
	 SPP_UART_event_data_available_ind_struct spp_uart_data_available_ind;
	 /* SPP <-- SPP */
	 SPP_event_profile_send_data_req_struct spp_profile_send_data_req;
	 SPP_event_profile_get_data_req_struct spp_profile_get_data_req;
	 SPP_event_hf_send_data_req_struct hf_send_data_req;	// only in MAUI
	 
	 SPP_event_activate_cnf_struct spp_activate_cnf;
	 SPP_event_deactivate_cnf_struct spp_deactivate_cnf;
	 DUN_event_activate_cnf_struct dun_activate_cnf;
	 DUN_event_deactivate_cnf_struct dun_deactivate_cnf;
	 FAX_event_activate_cnf_struct fax_activate_cnf;
	 FAX_event_deactivate_cnf_struct fax_deactivate_cnf;

#ifdef __SPP_SHARED_MEMORY__
	 SPP_UART_event_assign_buffer_cnf_struct spp_uart_assign_buffer_cnf;
#endif	/* __SPP_SHARED_MEMORY__ */
    } p;
} SPPCallbackParms;

typedef void (*SPPCallback)(SPPCallbackParms *Parms);


/****************************************************************************
 *
 * Section: Driver Initialization Functions
 *
 * These APIs are called typically when the driver is initialized or loaded.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPP_InitDevice()
 *
 *      Initializes a Serial Port Profile device structure. Call this API
 *      for each SPP device before using it for any other SPP API. In most
 *      cases this would be during driver initialization. 
 *     
 * Parameters:
 *      dev -   pointer to existing device structure
 *      packets - pointer to an array of TX packets used when queueing data to
 *          RFCOMM.
 *      numPackets - number of packets pointed to by 'packets'. This number
 *          can be 1 or more. If too few packets are provided, SPP may
 *          run out and be unable to queue more data to RFCOMM until packets
 *          in use are returned to SPP.
 */
void SPP_InitDevice(SppDev *dev, BtPacket *packets, U16 numPackets);

/****************************************************************************
 *
 * Section: Serial Port Profile I/O Functions
 *
 * These APIs are provided to upper layers. Normally, they are called
 * as the result of a corresponding call to the system API. For example,
 * in a POSIX interface open() would call SPP_Open().
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPP_Open()
 *
 *      Opens a serial device for reading and writing. Requires an
 *      existing ACL connection with the remote device for client devices.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      btDevice -  for client port this is a pointer to an existing 
 *               RemoteDevice structure describing the remote device to 
 *               which a service connection will be made. Server ports
 *               ignore this field.
 * Returns:
 *      BT_STATUS_SUCCESS - device opened successfully
 *      BT_STATUS_INVALID_PARM - invalid parameter; usually this is because
 *                               btDevice is not valid or no ACL exists to
 *                               the device
 *      BT_STATUS_FAILED - device is not in a closed state
 */
BtStatus SPP_Open(SppDev *dev, BtRemoteDevice *btDevice);

/*---------------------------------------------------------------------------
 * SPP_Close() 
 *     
 *     Close the serial device. Requires the device to have been opened 
 *     previously by SPP_Open(). This function does not verify that the 
 *     calling thread or process is the same one that opened it. The calling
 *     layer may optionally do any such checks.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - closed successfully
 *      BT_STATUS_FAILED - device was not opened
 */
BtStatus SPP_Close(SppDev *dev);

BtStatus SPP_Disconnect(SppDev *dev);

/*---------------------------------------------------------------------------
 * SPP_Read() 
 *      Read from the serial device. Requires the device to have been 
 *      successfully opened by SPP_Open().
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      buffer - allocated buffer to receive bytes
 *      maxBytes - on input: maximum bytes to read; on successful completion:
 *                 number of bytes actually read
 *
 * Returns:
 *      BT_STATUS_SUCCESS - read was successful
 *      BT_STATUS_FAILED - device is not opened.
 */
BtStatus SPP_Read(SppDev *dev);

/*---------------------------------------------------------------------------
 * SPP_Write()
 *      Write to the serial device. Requires the device to have been  
 *      successfully opened by SPP_Open().
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      buffer - buffer containing characters to write
 *      nBytes - on input: pointer to number of bytes in buffer; actual
 *               bytes written are returned in nBytes if it returns success
 *
 * Returns:
 *      BT_STATUS_SUCCESS - write was successful
 *      BT_STATUS_FAILED - device is not open
 */
BtStatus SPP_Write(SppDev *dev);

/*---------------------------------------------------------------------------
 * SPP_Ioctl()
 *      Perform an I/O control function on the serial device.
 *      Requires the device to have been successfully opened by the caller.
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      function - I/O control function as allowed by OS
 *      arg - argument peculiar to function; many ioctl functions alter
 *            the contents of *arg upon success (this is totally platform
 *            dependent and under the control of SPPOS_Ioctl)
 *
 * Returns:
 *      This function returns the status of SPPOS_Ioctl.
 */
BtStatus SPP_Ioctl(SppDev *dev, U16 function, void *arg);

/****************************************************************************
 * Section: SPP Lower-layer API's
 *
 * The following APIs are primarily for I/O control operations. That is,
 * the serial driver could call these functions for assistance in carrying
 * out a particular I/O request. For example, SPPOS_Ioctl() might use
 * these functions to obtain information about the device.
 *
 * Error checking (for example, making sure the SppDev is open and in the
 * right state, protecting from race conditions, etc.) is assumed handled
 * prior to making these calls.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPP_GetDataFormat() 
 *      Retrieve the current data format for the specified device.
 *     
 * Parameters:
 *      dev - pointer to existing device structure
 *
 * Returns:
 *      Current data format (data bits, stop bits and parity). The value
 *      returned is a superset of that for each individual setting (data bits,
 *      stop bits and parity - see APIs below).
 */
RfDataFormat SPP_GetDataFormat(SppDev *dev);

#define SPP_GetDataFormat(dev)        dev->portSettings.dataFormat

/*---------------------------------------------------------------------------
 * SPP_GetDataBits() 
 *      Get the number of data bits in effect for the specified device.
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current setting for number of data bits as defined by RfDataFormat.
 */
RfDataFormat SPP_GetDataBits(SppDev *dev);

#define SPP_GetDataBits(dev)        (RfDataFormat)(dev->portSettings.dataFormat \
                                     & RF_DATA_BITS_MASK)

/*---------------------------------------------------------------------------
 * SPP_GetParity()
 *      Get the current parity setting for the specified device. 
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current parity setting (see RfDataFormat).
 */
RfDataFormat SPP_GetParity(SppDev *dev);

#define SPP_GetParity(dev)        (RfDataFormat)(dev->portSettings.dataFormat \
                                   & (RF_PARITY_TYPE_MASK | RF_PARITY_MASK))

/*---------------------------------------------------------------------------
 * SPP_GetStopbits()
 *      Retrieve the number of stop bits for the specified device. 
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current setting for number of stop bits as defined by RfDataFormat.
 */
RfDataFormat SPP_GetStopBits(SppDev *dev);

#define SPP_GetStopBits(dev)        (RfDataFormat)(dev->portSettings.dataFormat \
                                     & RF_STOP_BITS_MASK)

/*---------------------------------------------------------------------------
 * SPP_GetFlowControl() 
 *      Retrieve the current flow control setting for the specified device.
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Flow control setting (see RfFlowControl).
 */
RfFlowControl SPP_GetFlowControl(SppDev *dev);

#define SPP_GetFlowControl(dev)        dev->portSettings.flowControl

/*---------------------------------------------------------------------------
 * SPP_GetModemStatus()
 *      Retrieve the current modem status signals for the specified device. 
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Modem status signals (see RfSignals).
 */
RfSignals SPP_GetModemStatus(SppDev *dev);

#define SPP_GetModemStatus(dev)        dev->rModemStatus.signals

/*---------------------------------------------------------------------------
 * SPP_GetBaud() 
 *      Retrieve the current baud rate setting for the specified device.
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Baud rate (see RfBaudRate).
 */
RfBaudRate SPP_GetBaud(SppDev *dev);

#define SPP_GetBaud(dev)        dev->portSettings.baudRate

/*---------------------------------------------------------------------------
 * SPP_SendPortSettings()
 *      Transmit the port settings (RfPortSettings) to the remote device.
 *      When a port setting changes on the local system, that setting should
 *      be communicated to the remote device. 
 *      
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The processing of sending to the remote device 
 *               has been started or RF_SEND_CONTROL is disabled
 *      BT_STATUS_FAILED
 */
BtStatus SPP_SendPortSettings(SppDev *dev);

/*---------------------------------------------------------------------------
 * SPP_SetModemControl()
 *      Set the modem control signals for the specified device. 
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      signals - modem control signals to be set
 *
 * Returns:
 *      The modem control signals are set in the device structure. If
 *      RF_SEND_CONTROL is enabled, the settings are sent to the remote
 *      device.
 *
 *      BT_STATUS_SUCCESS - RF_SEND_CONTROL is disabled or
 *               RF_SEND_CONTROL is enabled and the transmission
 *               was begun.
 *
 *      Other returns - See RF_SetModemStatus().
 */

#if defined (__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
BtStatus SPP_SetModemControl(SppDev *dev, RfSignals signals);
#endif 

/*---------------------------------------------------------------------------
 * SPP_SetBaud()
 *      Set the baud rate for the specified device. Baud rate for the
 *      device is informational only. 
 *      
 * Parameters:
 *      dev -    pointer to existing device structure
 *      baud -   current baud rate
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'baud'. Note that SPP_SendPortSettings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to 
 *      be set.
 */
void SPP_SetBaud(SppDev *dev, U8 baud);

/*---------------------------------------------------------------------------
 * SPP_SetDataFormat() 
 *      Set the data format values for the specified device.
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      value -    data format (see RfDataFormat)
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'value'. Note that SPP_SendPortSettings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to 
 *      be set.
 */
void SPP_SetDataFormat(SppDev *dev, U8 value);

/*---------------------------------------------------------------------------
 * SPP_SetFlowControl() 
 *      Set the flow control value for the specified device.
 *      
 * Parameters:
 *      dev -    pointer to existing device structure
 *      value -    Flow control value (see RfFlowControl).
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'value'. Note that SPP_SendPortSettings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to 
 *      be set.
 */
void SPP_SetFlowControl(SppDev *dev, U8 value);

/****************************************************************************
 * 
 * Section: OS-Provided APIs
 *
 * The following functions are OS-specific in how they are implemented. 
 * The Serial Port Profile calls them in order to carry out its tasks.
 * The implementer needs to port these functions to the platform.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPPOS_LockDev() 
 *      Locks a serial device structure to prevent re-entrancy and data
 *      access problems. Called in task context. 
 * 
 *      Must allow for nested calls from the same thread. Calling this 
 *      function more than once without an intermediate call to SPPOS_UnlockDev()
 *      should have the same result as calling it once. That is, after
 *      the first call, subsequent calls should be a no-op.
 *
 *      On a non-multitasking platform this could be a no-op.
 *     
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 */
void SPPOS_LockDev(void *mutex);

/*---------------------------------------------------------------------------
 * SPPOS_UnlockDev() 
 *      Unlocks a serial device structure. This is the counterpart for 
 *      SPPOS_LockDev. When this function exits, another device pending
 *      on the object is allowed to obtain that object. If more than one
 *      call to SPPOS_LockDev() had been made by the same thread without
 *      an intervening call to SPPOS_UnlockDev(), the first call to this
 *      function should unlock the device.     
 *     
 *      On a non-multitasking platform this could be a no-op.
 *
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 */
void SPPOS_UnlockDev(void *mutex);

/*---------------------------------------------------------------------------
 * SPPOS_Wait()
 *      Causes the caller to block. If another task is ready to run, that
 *      task should be given context by the operating systems scheduler.
 *     
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 *
 *      ms    - number of milliseconds to pend; note that need not be at 
 *              all accurate. Currently, this parameter is used to avoid
 *              a wait-forever condition that might arise from an
 *              implementation bug.
 *
 *      On a non-multitasking platform this could be a no-op.
 *
 * Returns:
 *      BT_STATUS_SUCCESS      SPPOS_Resume was called before timeout
 *      BT_STATUS_FAILED   timeout or other error occurred
 */
BtStatus SPPOS_Wait(void *mutex, U16 ms);

/*---------------------------------------------------------------------------
 * SPPOS_Resume()
 *      Allow a blocked task to resume execution. This is the counterpart
 *      of SPPOS_Wait. When this function exits, the task waiting on the 
 *      object is allowed to run. It is operating system dependent as to 
 *      when the task is scheduled. 
 *      
 *      This function must obviously be callable by a task other that the
 *      one that called SPPOS_Wait.
 *
 *      On a non-multitasking platform this could be a no-op.
 *     
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 */
void SPPOS_Resume(void *mutex);

/*---------------------------------------------------------------------------
 * SPPOS_ReadBuf() 
 *      The SPP Driver calls this function when the caller is 
 *      reading the serial device (RFCOMM).
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done. Typically, this function extracts
 *      bytes from a ring buffer. If your OS routine handles only a single
 *      character, then this function should call it iteratively for each
 *      byte returned.
 *     
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - allocated buffer to receive bytes
 *      len  - pointer to max number of characters to be put in buf; on
 *             successful return this is set to number of bytes actually read
 *
 * Returns:
 *      BT_STATUS_SUCCESS - completed successfully
 *      BT_STATUS_FAILED - error as determined by platform implementation
 */
BtStatus SPPOS_ReadBuf(void *dev, char buf[], U16 *len);

/*---------------------------------------------------------------------------
 * SPPOS_WriteBuf() 
 *      Process characters to be transmitted using the serial device. 
 *      The SPP Driver calls this function when the caller is 
 *      writing bytes to the serial device (RFCOMM).
 *      Typically, this function places bytes in a ring buffer. 
 *      If your OS routine handles only a single character, then this 
 *      function should call it iteratively for each byte in the buffer.
 *     
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - buffer of characters received from RFCOMM
 *      len  - pointer to number of characters in buf; on successful
 *             return this is set to number of bytes actually written
 *
 * Returns:
 *      BT_STATUS_SUCCESS - len contains number of bytes in buf
 *      BT_STATUS_FAILED
 */
BtStatus SPPOS_WriteBuf(void *dev, char buf[], U16 *len);

/*---------------------------------------------------------------------------
 * SPPOS_DevRx() 
 *      Process characters received from the serial device. 
 *      This function is called by SPP for when characters are received
 *      from RFCOMM. Contrast this to SPPOS_ReadBuf(), which is called 
 *      when the application layer reads from the device itself.
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done. Typically, this function places
 *      bytes in a ring buffer. If your OS routine handles only a single
 *      character, then this function should call it iteratively for each
 *      byte in the buffer.
 *     
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - buffer of characters received from RFCOMM
 *      len  - number of characters in buf; on successful
 *             return this is set to number of bytes in buf
 *
 * Returns:
 *      BT_STATUS_SUCCESS    - *len is the number of bytes written
 *      BT_STATUS_FAILED - 
 */
BtStatus SPPOS_DevRx(void *dev, char buf[], U16 *len);

/*---------------------------------------------------------------------------
 * SPPOS_DevTx() 
 *      Get the next characters to be transmitted via RFCOMM.
 *      This function is called by SPP to retrieve bytes to be sent
 *      via RFCOMM. Typically, this routine removes characters from
 *      a ring buffer. Contrast this function to SPPOS_WriteBuf(), which
 *      queues data to be transmitted. This API retrieves that same data
 *      for actual transmission.
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done.
 *     
 * Parameters:
 *      dev  - pointer to SppDev
 *
 *      buf  - On output, filled with a char pointer full of data. If "len"
 *             is 0, a NULL pointer may be provided. If non-NULL, this pointer
 *             is owned by the SPP layer until SPPOS_ReturnBuf is called.
 *              
 *      len  - On output, contains the actual number of bytes in "buf".
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Indicates that "buf" contains bytes for transmission.
 *
 *      BT_STATUS_FAILED  - no more characters to send
 */
BtStatus SPPOS_DevTx(void *dev, char **buf, U16 *len);

/*---------------------------------------------------------------------------
 * SPPOS_Ioctl() 
 *      This routine implements the entire I/O control for the device.
 *      It is highly platform-specific which ioctl functions are supported.
 *      The implementer should consider which functions to support based
 *      on the needs of higher-layer software. 
 *     
 * Parameters:
 *      dev  - pointer to SppDev
 *      function - I/O control function code as defined by the platform
 *      arg - a function-specific argument; on successful return
 *            this is set to output of the function if appropriate for
 *            the particular function; implementation specific
 *
 * Returns:
 *      implementation and function specific
 */
BtStatus SPPOS_Ioctl(void *dev, U16 function, void *arg);

/*---------------------------------------------------------------------------
 * SPPOS_RxBytes() 
 *     Determine the number of bytes available for reading. The SPP driver
 *     calls this function. It may optionally be called in response
 *     to an IOCTL call.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *
 * Returns:
 *      max number of bytes to read
 */
U16 SPPOS_RxBytes(void *dev);

/*---------------------------------------------------------------------------
 * SPPOS_RxFree() 
 *     Determine the number of bytes available for receiving bytes from 
 *     the device. Called by the SPP driver and optionally in response
 *     to an IOCTL call.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *
 * Returns:
 *      max number of bytes to read
 */
U16 SPPOS_RxFree(void *dev);

/*---------------------------------------------------------------------------
 * SPPOS_FlushRx() 
 *      Flushes all the unread bytes received on the device
 *
 * Parameters:
 *      dev  - pointer to SppDev
 */
void SPPOS_FlushRx(void *dev);

/*---------------------------------------------------------------------------
 * SPPOS_FlushTx() 
 *      Flushes all the bytes that have not been transmitted on the device
 *
 * Parameters:
 *      dev  - pointer to SppDev
 */
void SPPOS_FlushTx(void *dev);

/*---------------------------------------------------------------------------
 * SPPOS_ReturnBuf() 
 *      Notify the OS-specific portion of the driver that a buffer is no
 *      longer needed and may be reused or unallocated. The buffer being 
 *      freed is one that is allocated and/or returned in SPPOS_DevTx.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - buffer being returned
 *      len  - length of buf
 */
void SPPOS_ReturnBuf(void *dev, char *buf, U16 len);

void SPPOS_ConnectInd(void *dev, U8 cid, U32 lap, U8 uap, U16 nap);

void SPPOS_DisConnectInd(void *dev);

void SPPOS_DevTxEmpty(void *dev);


/* 
 * spp_api.c related function declaration
 */

#if SPP_CLIENT == XA_ENABLED
void SPPHandleInternalConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Server_chnl_num, msg_type Msg_id, U16 remoteUUID);
#define SPPHandleConnectReq(Lap, Uap, Nap, Server_chnl_num, remoteUUID) SPPHandleInternalConnectReq(Lap, Uap, Nap, Server_chnl_num, MSG_ID_BT_SPP_CONNECT_REQ, remoteUUID)
#define SPPSCOHandleConnectReq(Lap, Uap, Nap, Server_chnl_num, remoteUUID) SPPHandleInternalConnectReq(Lap, Uap, Nap, Server_chnl_num, MSG_ID_BT_SPP_SCO_CONNECT_REQ, remoteUUID)
#if DUN_CLIENT == XA_ENABLED
#define DUNHandleConnectReq(Lap, Uap, Nap, Server_chnl_num) SPPHandleInternalConnectReq(Lap, Uap, Nap, Server_chnl_num, MSG_ID_BT_DUN_CONNECT_REQ, SC_DIALUP_NETWORKING)
#define FAXHandleConnectReq(Lap, Uap, Nap, Server_chnl_num) SPPHandleInternalConnectReq(Lap, Uap, Nap, Server_chnl_num, MSG_ID_BT_FAX_CONNECT_REQ, SC_FAX)
#endif /* #if DUN_CLIENT == XA_ENABLED */
#endif /* #if SPP_CLIENT == XA_ENABLED */

//#if BT_SPP_AUTHORIZE_BY_MMI
void SPPHandleInternalConnectIndRes(UART_PORT Port, kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Result);
#define SPPHandleConnectIndRes(port, lap, uap, nap, result) SPPHandleInternalConnectIndRes(port, lap, uap, nap, result)
#define DUNHandleConnectIndRes(port, lap, uap, nap, result) SPPHandleInternalConnectIndRes(port, lap, uap, nap, result)
#define FAXHandleConnectIndRes(port, lap, uap, nap, result) SPPHandleInternalConnectIndRes(port, lap, uap, nap, result)
//#endif /* #if BT_SPP_AUTHORIZE_BY_MMI */

#if SPP_CLIENT == XA_ENABLED
SPP_DEV_OS *SearchSppClientOsDevBySdpTkn(SdpQueryToken *sqt);
SPP_DEV_OS *SearchSppClientOsDevBySPPDev(SppDev *dev);
#endif /* SPP_CLIENT == XA_ENABLED */
kal_bool IsThisPortAsSPPServer(UART_PORT port);
#if (SPP_SCO_SUPPORT == XA_ENABLED)
void SPP_RetrieveRSSI_Timeout(EvmTimer *Timer);
#endif /* (SPP_SCO_SUPPORT == XA_ENABLED) */
BOOL SPPDRV_Init(void);
void initVp(SPP_DEV_OS *dev);
void setDevOwner(UART_PORT port, module_type owner);
SPP_DEV_OS *getDev(UART_PORT port);
#if SPP_CLIENT == XA_ENABLED
void bt_sppclient_cmgr_link_close(SPP_DEV_OS *sppclient_ptr);
void bt_sppclient_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status);
#endif /* SPP_CLIENT == XA_ENABLED */
#if (SPP_SCO_SERVER == XA_ENABLED)
void bt_sppserver_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status);
#endif /* (SPP_SCO_SERVER == XA_ENABLED) */
void sppa_send_connect_cnf_success(
        UART_PORT port,
        kal_uint8 con_id,
        U32 lap,
        U8 uap,
        U16 nap,
        kal_uint8 server_chnl_number
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
        , kal_uint8 profile
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
        );
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
void sppa_send_connect_cnf_failed(U32 lap, U8 uap, U16 nap, kal_uint8 result, kal_uint8 profile);
void sppa_send_disconnect_cnf_result(UART_PORT port, U8 con_id, U32 lap, U8 uap, U16 nap, U8 result, U8 profile);
#else  /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
void sppa_send_connect_cnf_failed(U32 lap, U8 uap, U16 nap, kal_uint8 result);
void sppa_send_disconnect_cnf_result(UART_PORT port, U8 con_id, U32 lap, U8 uap, U16 nap, U8 result);
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
void sppa_send_audio_command_result(UART_PORT port, kal_uint8 result, msg_type msg);
#if SPP_SCO_SUPPORT == XA_ENABLED
void sppa_send_rssi_result(UART_PORT port, kal_uint8 *addr, kal_uint8 rssi_value);
#endif /* SPP_SCO_SUPPORT == XA_ENABLED */ 
void SPP_Register(SPPCallback Callback);
#if BT_SPP_AUTHORIZE_BY_MMI
void SPPHandleInternalConnectIndRes(UART_PORT Port, kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Result);
#else /* #if BT_SPP_AUTHORIZE_BY_MMI */
void SPPHandleAuthRsp(UART_PORT Port, kal_bool Result);
#endif /* #if BT_SPP_AUTHORIZE_BY_MMI */
void SPPHandleUartOwnerCnf(UART_PORT Port);
void UartHandlePlugoutCnf(UART_PORT Port);
#if SPP_CLIENT == XA_ENABLED
void SPPHandleInternalConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Server_chnl_num, msg_type Msg_id, U16 remoteUUID);
#endif /* #if SPP_CLIENT == XA_ENABLED */
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
kal_uint16 SPPUartPutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);
kal_uint16 SPPUartGetBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid);
kal_bool SPPUartOpen(UART_PORT port, module_type owner);
kal_uint16 SPPUartGetBytesAvail(UART_PORT port);
kal_uint16 SPPUartGetTxRoomLeft(UART_PORT port);
kal_uint16 SppUartPutISRBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);
kal_uint16 SppUartGetTxISRRoomLeft(UART_PORT port);
void SppUartPurge(UART_PORT port, UART_buffer dir, module_type ownerid);
void SppUartSetFlowCtrl(UART_PORT port, kal_bool XON, module_type ownerid);
void SppUartConfigEscape(UART_PORT port, kal_uint8 EscChar, kal_uint16 ESCGuardtime, module_type ownerid);
void SppUartSetDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config, module_type ownerid);
void SppUartSetBaudRate(UART_PORT port, UART_baudrate baudrate, module_type ownerid);
kal_uint16 SppUartSendISRData(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 mode, kal_uint8 escape_char, module_type ownerid);
kal_uint16 SppUartSendData(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 mode, kal_uint8 escape_char, module_type ownerid);
void SppUartSetAutoBaud_Div(UART_PORT port, module_type ownerid);
void SppUartRegister_TX_cb(UART_PORT port, module_type owner, UART_TX_FUNC func);
void SppUartRegister_RX_cb(UART_PORT port, module_type owner, UART_RX_FUNC func);
kal_uint8 SppUartGetUARTByte(UART_PORT port);
void SppUartPutUARTByte(UART_PORT port, kal_uint8 data);
void SppUartPutUARTBytes(UART_PORT port, kal_uint8 *data, kal_uint16 len);
module_type SPPUartGetOwnerID(UART_PORT port);
#else   /* BTMTK_ON_WISE */
#ifdef __SPP_THROUGHPUT_TEST__
U16 SPPUartPutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);
#else	/* __SPP_THROUGHPUT_TEST__ */
void SPPUartPutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);
#endif	/* __SPP_THROUGHPUT_TEST__ */
void SPPUartGetBytes(UART_PORT port, kal_uint16 Length, kal_uint8 *status, module_type ownerid);
void SPPUartOpen(UART_PORT port, module_type owner);
void SPPUartGetBytesAvail(UART_PORT port);
void SPPUartGetTxRoomLeft(UART_PORT port);
void SPPUartGetOwnerID(UART_PORT port);
#endif  /* BTMTK_ON_WISE */
void SPPHandleActivateReq(SppPortType portType, char *svcName, U16 svcUUID);
#if (DUN_SERVER == XA_ENABLED) || (DUN_CLIENT == XA_ENABLED)
void DUNHandleActivateReq(SppPortType portType);
void DUNHandleDeactivateReq(UART_PORT port);
#endif /* #if (SPP_CLIENT == XA_ENABLED) || (DUN_CLIENT == XA_ENABLED) */
#if FAX_SERVER == XA_ENABLED
void FAXHandleActivateReq(SppPortType portType);
void FAXHandleDeactivateReq(UART_PORT port);
void FAXHandleDisconnectReq(UART_PORT Port);
#endif /* #if FAX_SERVER == XA_ENABLED */
void SPPHandleSendDataReq(UART_PORT Port);
void SPPHandleGetDataReq(UART_PORT Port);
void SPPHandleDeactivateReq(UART_PORT port);
void SPPHandleDisconnectReq(UART_PORT Port);
#if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED
void DUNHandleDisconnectReq(UART_PORT Port);
#endif /* #if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED */
void SPPHandleAudioConnectReq(UART_PORT Port);
void SPPHandleAudioDisconnectReq(UART_PORT Port);

void SPPUartClose(UART_PORT port, module_type ownerid);
void SPPUartSetOwner(UART_PORT port, module_type owner);
void SPPCtrlDCD(UART_PORT port, IO_level SDCD, module_type ownerid);
void SPPUartCtrlBreak(UART_PORT port, IO_level SBREAK, module_type ownerid);
void SPPUartClrRxBuffer(UART_PORT port, module_type ownerid);
void SPPUartClrTxBuffer(UART_PORT port, module_type ownerid);
void SPPEnableReq(void);
void SPPDisableReq(void);
void SPPUartDataAvailableIndRsp(UART_PORT port, kal_uint16 Length);
#ifdef __SPP_SHARED_MEMORY__
void SPPAssignBufferReq(UART_PORT port, U8 *getBufPtr, U16 getBufSize);
#endif	/* __SPP_SHARED_MEMORY__ */

void SPPOS_SendUARTDataAvailableInd(UART_PORT port);

#endif /* __SPP_H */ 

