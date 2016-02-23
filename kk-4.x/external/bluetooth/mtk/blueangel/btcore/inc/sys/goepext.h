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

#ifndef __GOEPEXT_H
#define __GOEPEXT_H

/****************************************************************************
 *
 * File:
 *     $Workfile$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:
 *     Internal structures used by the GOEP layer and its EXTensions.
 * 
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if BT_STACK == XA_ENABLED
#include <medev.h>
#endif /* BT_STACK == XA_ENABLED */
#include <goep.h>

/*---------------------------------------------------------------------------
 * Generic Object Exchange Flags (for Client and Server)
 */
#define GOEF_ACTIVE                 0x01    /* Transport connection is up */
#define GOEF_CHALLENGE              0x02    /* Received an Auth Challenge */
#define GOEF_RESPONSE               0x04    /* Received an Auth Response */
#define GOEF_CLIENT_TP_INITIATED    0x08    /* Transport connection is client initiated */
#define GOEF_SERVER_TP_INITIATED    0x10    /* Transport connection is server initiated */

/*---------------------------------------------------------------------------
 * Client Transport Connection States
 */
#define CS_DISCONNECTED     0       /* No transport connection */
#define CS_CONNECTING       1       /* TpConnect() called */
#define CS_CONNECTED        2       /* Connection up and indicated */
#define CS_DISCONNECTING    3       /* Connection is disconnecting */

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 * GoepQueuedHeader structure
 * 
 *     This structure is used by the during Header indications.
 */
typedef struct _GoepQueuedHeader {
    /* Queued header type */
    ObexHeaderType type;

    /* Pointer to the start of the queued header */
    const U8      *buffer; 

    /* Length of the queued header.  Only in the case of a GET request 
     * sending a BODY header can this length span multiple OBEX packets.
     * In this one case, the length value will decrease as the 
     * header is sent.
     */        
    U16            len;
    
} GoepQueuedHeader;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP Client Obex connections 
 *
 *      Manages GOEP Client Obex connections and the multiplexing on each 
 *      connection.
 */
typedef struct _GoepClientObexCons {
    /* This field must be remain at the top of this structure */
    ObexClientApp      obc;                             /* Obex Client */
    GoepClientEvent    currOp;                          /* Current operation */
    U8                 flags;                           /* Connection and Authorization flags */
    U8                 connId;                          /* Connection ID for the client */
    U8                 connCount;                       /* Number of connections to the OBEX client */
    void               *object;                         /* Current Object's handle */
    void               *buf_ptr;                         /* Current buffer pointer */
    U32                 buf_len;                         /* Current buffer length */
    U8                 headerBlock[GOEP_CLIENT_HB_SIZE];/* Header construct */
#if OBEX_AUTHENTICATION == XA_ENABLED
    U8                 nonce[AUTH_NONCE_LEN];           /* Our nonce sent in chall */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    GoepClientApp      *profiles[GOEP_MAX_PROFILES];    /* Profiles per connection */
#if GOEP_ADDITIONAL_HEADERS > 0
    GoepQueuedHeader   queuedHeaders[GOEP_ADDITIONAL_HEADERS]; /* Queued additional client headers */
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
} GoepClientObexCons;

/*---------------------------------------------------------------------------
 * GOEP Client data 
 *
 *      Keeps track of internal GOEP Client information.
 */
typedef struct _GoepClientData {
    BOOL                initialized;                    
    U8                  connCount;                      /* Obex connections */
    GoepClientObexCons  clients[GOEP_NUM_OBEX_CLIENT_CONS];    /* Number of Obex connections */
#if BT_STACK == XA_ENABLED
    BtSelectDeviceToken deviceQuery;
	ObexTpAddr			tpAddr;
	GoepClientApp		*storedAppPtr;
#endif /* BT_STACK == XA_ENABLED */
} GoepClientData;
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP Server Obex connections 
 *
 *      Manages GOEP Server Obex connections and the multiplexing on each 
 *      connection.
 */
typedef struct _GoepServerObexCons {
    /* This field must remain at the top of this structure */
    ObexServerApp      obs;                             /* Obex Server */
    GoepServerEvent    currOp;                          /* Current operation */
    BOOL               oustandingResp;                  /* Status of an outstanding response */
    U8                 flags;                           /* Connection and Authorization flags */
    U8                 connId;                          /* Connection ID for the server */
    U8                 connCount;                       /* Number of connections to the OBEX server */
    void               *object;                         /* Current Object's handle */
    void               *buf_ptr;                         /* Current buffer pointer */
    U32                 buf_len;                         /* Current buffer length */
    GoepOperation      abortedOper;                     /* GOEP_OPER_xxx being aborted */
    U8                 headerBlock[GOEP_SERVER_HB_SIZE];/* Header construct */
#if OBEX_AUTHENTICATION == XA_ENABLED
    U8                 nonce[AUTH_NONCE_LEN];           /* Our nonce sent in chall */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    GoepServerApp      *profiles[GOEP_MAX_PROFILES];    /* One profile per connection */
#if GOEP_ADDITIONAL_HEADERS > 0
    GoepQueuedHeader   queuedHeaders[GOEP_ADDITIONAL_HEADERS]; /* Queued additional server headers */
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
//#if (XA_DEBUG == XA_ENABLED)
    ObServerEvent      lastEvent;
//#endif /* (XA_DEBUG == XA_ENABLED) */
} GoepServerObexCons;

/*---------------------------------------------------------------------------
 * GOEP Server data 
 *
 *      Keeps track of internal GOEP Server information.
 */
typedef struct _GoepServerData {
    BOOL                initialized;                    
    U8                  connCount;                      /* Obex connections */
    GoepServerObexCons  servers[GOEP_NUM_OBEX_SERVER_CONS];    /* Number of Obex connections */
#if BT_STACK == XA_ENABLED
    BtSelectDeviceToken deviceQuery;
	ObexTpAddr			tpAddr;
	GoepServerApp		*storedAppPtr;
#endif /* BT_STACK == XA_ENABLED */
} GoepServerData;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
#if OBEX_ROLE_CLIENT == XA_ENABLED
#if XA_CONTEXT_PTR == XA_ENABLED
#define GOEC(field)         (GoeClient->field)
extern GoepClientData       *GoeClient;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define GOEC(field)         (GoeClient.field)
extern GoepClientData       GoeClient;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
#if XA_CONTEXT_PTR == XA_ENABLED
#define GOES(field)         (GoeServer->field)
extern GoepServerData       *GoeServer;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define GOES(field)         (GoeServer.field)
extern GoepServerData       GoeServer;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#endif /* __GOEPEXT_H */
