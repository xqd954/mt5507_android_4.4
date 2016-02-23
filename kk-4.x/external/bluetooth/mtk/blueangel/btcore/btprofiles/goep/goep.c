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

/***************************************************************************
 *
 * File:
 *     $Workfile:goep.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the functions that comprise the Generic Object
 *     Exchange profile implementation.
 *
 * Created:
 *     September 4, 2000
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
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Mar 18 2009 mtk80029
 * [MAUI_01406911] [Bluetooth] Btadp folder restruct activity
 * 
 *
 * Dec 26 2008 mbj06038
 * [MAUI_01348800] [BT]When connect failed,the previous connection will disconnect
 * 
 *
 * Dec 5 2008 mbj06032
 * [MAUI_01347154] [BT OBEX] code revisement: assign value of server member in OBEX_ServerSendResponse
 * GOEP_ServerContinue : remove PRECOMPLETE state checking
 *
 * Dec 5 2008 mbj06032
 * [MAUI_01345132] [BT]Different behavior when have connection request on pairing screen
 * If user reject the RFCOMM Connect indication, Remove the ACL Link, and clear related variable
 *
 * Nov 26 2008 mbj06038
 * [MAUI_01342148] [New feature][SyncML] Device sync support
 * 
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106205] [BT GOEP] Send OPP Client SDP result (OPP server's supported format list) to MMI
 * 
 *
 * Aug 22 2008 mbj06032
 * [MAUI_01196499] MTK:Bluetooth OPP_[1] Fatal Error (431, 5c) - BT
 * copy target uuid , because connect_req.target will be freed in send_disconnect_ind_hdlr func
 *
 * Aug 15 2008 mbj06032
 * [MAUI_01101632] [BT BIP] revise thumbnail request code of BIP PUSH server
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_00792520] BT-device power off but can connectPBAP
 * notify server when  RFCOMM Open Ind to let user confirm:
 * 1. define "GOEP_EVENT_AUTHORIZE_IND"; 
 * 2. implement "GOEP_ServerAuthorizeResponse"
 *
 * Jul 2 2008 mbj06032
 * [MAUI_01171562] MTK:Bluetooth OBEX_it can't connect FTP succeed
 * accept OBEX authentication challenge
 *
 * Jun 25 2008 mbj06032
 * [MAUI_00791922] [1] Fatal Error (4, f169b188) - BT
 * memset secToken to 0
 *
 * Jun 11 2008 mbj06053
 * [MAUI_01070863] BPP revise check in
 * 
 *
 * Jun 6 2008 mbj06032
 * [MAUI_01063800] [BT]Long time display "Connecting" and connect failed.
 * 
 *
 * Jun 6 2008 mbj06032
 * [MAUI_01063800] [BT]Long time display "Connecting" and connect failed.
 * GOEP_ServerContinue check GOEP_EVENT_PRECOMPLETE event
 *
 * May 28 2008 mbj06032
 * [MAUI_00678189] [PBAP]Can't get the missed call wth Nokia BH-902
 * reserver goep_service type
 *
 * May 19 2008 mbj06032
 * [MAUI_00976686] MTK:Bluetooth BIP_[1] Fatal Error (207, 26f3990) - BT
 * remove compile warning
 *
 * May 16 2008 mbj06032
 * [MAUI_00976686] MTK:Bluetooth BIP_[1] Fatal Error (207, 26f3990) - BT
 * to reduce system stack size, use dynamic memory, not array
 *
 * May 16 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * revise code for Partial Content
 *
 * May 15 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * support Partial Content rsp code
 *
 * May 15 2008 mbj06032
 * [MAUI_01040129] [BT GOEP] Revise for PBAP
 * 
 *
 * May 13 2008 mbj06032
 * [MAUI_01038033] [BT]Can connect server when press button no on Server.
 * 
 *
 * May 13 2008 mbj06032
 * [MAUI_00093122] Screen always displays recieving X%, x/x.
 * revise code
 *
 * Apr 11 2008 mbj06032
 * [MAUI_00653702] [BT OBEX & GOEP ] add some trace info
 * 
 *
 * Apr 10 2008 mbj06032
 * [MAUI_00653223] [BT OBEX]modify trace info
 * 
 *
 * Apr 9 2008 mbj06032
 * [MAUI_00651340] [BT GOEP] Modify goep.c for PBAP server
 * modify GOEP_ServerRFCommOpenInd func
 *
 * Mar 20 2008 mbj06032
 * [MAUI_00638880] [BT GOEP] Remove GOEP modification for CR MAUI_00282255
 * Remove modification for CR MAUI_00282255, because BPP has reject the second connection
 *
 * Mar 19 2008 mbj06032
 * [MAUI_00282255] ASSERT Fail: cngrGabdker !=0 cibngr,c 740 - BT
 * in GOEP_TpConnect, check if a same service has connected
 *
 * Mar 13 2008 mbj06032
 * [MAUI_00281507] MTK OPP: it is abanormal when receive a OPP file
 * clear server.pullpush.name info only after using it
 *
 * Mar 10 2008 mbj06032
 * [MAUI_00630752] MTK BT profile supported on MT6611 download file from PC by FTP speed abnormal
 * Add goep_service_type member to ObexClientApp
 *
 * Mar 10 2008 mbj06032
 * [MAUI_00622409] Bluetooth-PTSy?UTIL_ Server _SETUP_FILES^У?CRESET
 * clear server.pushpull.nameLen info in time
 *
 * Mar 6 2008 mbj06032
 * [MAUI_00634019] [BT GOEP] remove MODIS compile warning
 * 
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 *
 ****************************************************************************/
#ifdef __BT_GOEP_PROFILE__

#include <sys/obxalloc.h>
#include <sys/obex_utl.h>


#include "goepext.h"
#include "obex.h"

#include "sec.h"
#include "goep.h"
#include "bluetooth_struct.h"
#include "goep_main.h"
#include "obexauth.h"
#include "bt_os_api.h"
#include "bt_ucs2.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

extern kal_uint8* global_goep_connection[GOEP_CONNECTION_NUM];
extern kal_int8 global_goep_conn_count;

/*
 * The GOEP layer relies on events deliverd as part of the OBEX packet
 * flow control option. Therefore it must be enabled when using the GOEP.
 * It also uses server header build and client header parse functions.
 */
#if OBEX_PACKET_FLOW_CONTROL == XA_DISABLED
#error "OBEX_PACKET_FLOW_CONTROL Must be enabled."
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_DISABLED */

#if OBEX_HEADER_BUILD == XA_DISABLED
#error "OBEX_HEADER_BUILD Must be enabled."
#endif /* OBEX_HEADER_BUILD == XA_DISABLED */

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#if OBEX_ROLE_CLIENT == XA_ENABLED
#if XA_CONTEXT_PTR == XA_ENABLED
static GoepClientData   tempClient;
GoepClientData         *GoeClient = &tempClient;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
GoepClientData          GoeClient;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
#if XA_CONTEXT_PTR == XA_ENABLED
static GoepServerData   tempServer;
GoepServerData         *GoeServer = &tempServer;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
GoepServerData          GoeServer;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Internal Function Prototypes
 *
 ****************************************************************************/
#if OBEX_ROLE_CLIENT == XA_ENABLED
static void GoepClntCallback(ObexClientCallbackParms *parms);
static void GoepClientProcessHeaders(ObexClientCallbackParms *parms);
static void NotifyCurrClient(ObexClientApp *clientApp, GoepEventType Event);
static void NotifyAllClients(ObexClientApp *clientApp, GoepEventType Event);
#if GOEP_ADDITIONAL_HEADERS > 0
static BOOL ClientBuildHeaders(GoepClientObexCons *client, U16 *more);
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
static U16  Strlen(const char *String);
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
static U16  Unilen(const U16  *String);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
static void GoepSrvrCallback(ObexServerCallbackParms *parms);
static void GoepServerProcessHeaders(ObexServerCallbackParms *parms);
static void NotifyCurrServer(ObexServerApp *serverApp, GoepEventType Event);
static void NotifyAllServers(ObexServerApp *serverApp, GoepEventType Event);
static void StartServerOperation(ObexServerApp *serverApp, GoepOperation Op);
static void AssociateServer(ObexServerApp *serverApp);
#if GOEP_ADDITIONAL_HEADERS > 0
static BOOL ServerBuildHeaders(GoepServerObexCons *server);
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
static U16  AsciiToUnicode(U16 *Unistring, const char *String);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
static void DoHeaderCopy(U8 *Dst, U16 *Len, U16 MaxLen, U8 *Src, U16 SrcLen);
static void DoUniHeaderCopy(GoepUniType *Dst, U16 *Len, U16 MaxLen, U8 *Src, U16 SrcLen);
static void DoUniNameTruncateCopy(GoepUniType *Dst, U16 *Len, U16 MaxLen, U8 *Src, U16 SrcLen, U16 RemainLen);
static BOOL GetFreeConnId(U8 *id, U8 type);
static BOOL GoepGetPreviousRegisterConnIdByProfile(U8 goep_service_type, U8 type, U8 *id);

#if GOEP_AUTH == XA_ENABLED

extern auth_chal_struct  auth_chal_profiles[MAX_AUTHCHAL_PROFILES_NUM];

kal_bool goep_need_obex_auth_challenge(kal_uint8 *uuid, kal_uint8 uuid_len, int *pIndex);
ObexAuthChallenge* goep_build_auth_challenge_data(int index);

#endif

#if (GOEP_ADDITIONAL_HEADERS > 0)

#define  CLEAR_QUEUEDHEADERS(obxc)   \
do { \
    int i;\
    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) \
        OS_MemSet((U8 *)&(obxc)->queuedHeaders[i], 0, sizeof((obxc)->queuedHeaders[i]));\
}while(0)

#else

#define  CLEAR_QUEUEDHEADERS(obxc)  

#endif

extern ObServerEvent ServerEventVerifier(ObServerEvent lastEvent, ObServerEvent event);
extern char *pServerEvent(ObServerEvent event);
extern char *pClientEvent(ObClientEvent event);
extern const char *pObexOpcode(U8 Opcode);
extern void ObProtRxChopper(ObexAppHandle *App, U8 *buff, U16 len);
#if (XA_ERROR_CHECK == XA_ENABLED) /*|| (XA_DEBUG == XA_ENABLED)*/
extern BOOL ObIsHeaderSpaceAvail(ObexAppHandle *AppHndl, U16 HeaderLen);
#endif
/****************************************************************************
 *
 * Public API Functions
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            GOEP_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the GOEP component.  This must be the first GOEP 
 *            function called by the application layer, or if multiple 
 *            GOEP applications exist, this function should be called
 *            at system startup (see XA_LOAD_LIST in config.h).  OBEX must 
 *            also be initialized separately.
 *
 * Return:    TRUE or FALSE
 *
 */
BOOL GOEP_Init(void)
{
    U8 i;

    OS_LockStack();

#if OBEX_ROLE_CLIENT == XA_ENABLED
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)GoeClient, 0, sizeof(GoepClientData));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&GoeClient, 0, sizeof(GoepClientData));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)GoeServer, 0, sizeof(GoepServerData));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&GoeServer, 0, sizeof(GoepServerData));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

    /* Setup a connection Id and connection Count for each client/server */
#if OBEX_ROLE_CLIENT == XA_ENABLED
    for (i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++) {
        /* Setup the connId */
        GOEC(clients)[i].connId = i;
        /* Setup the connCount */
        GOEC(clients)[i].connCount = 0;
    }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
    for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++) {
        /* Setup the connId */
        GOES(servers)[i].connId = i;
        /* Setup the connCount */
        GOES(servers)[i].connCount = 0;
    }
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

    /* GOEP Initialized */
#if OBEX_ROLE_CLIENT == XA_ENABLED
    GOEC(initialized) = TRUE;
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#if OBEX_ROLE_SERVER == XA_ENABLED
    GOES(initialized) = TRUE;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

    OS_MemSet( (U8 *) global_goep_connection, 0, sizeof(global_goep_connection));
    global_goep_conn_count = 0;

    OS_UnlockStack();
    return TRUE;

}
 
#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_RegisterServer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Server.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_RegisterServer(GoepServerApp *Service, 
                             const ObStoreFuncTable *obStoreFuncs)
{
    ObStatus  status = OB_STATUS_SUCCESS;
    U8        id;
#if OBEX_SERVER_CONS_SIZE > 0
    U8        i;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    int        goep_conn_id;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (GOES(initialized) != TRUE) {
        /* GOEP is not initialized */
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if ((!Service) || (Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) || 
#if OBEX_SERVER_CONS_SIZE > 0
        (Service->numTargets > OBEX_SERVER_CONS_SIZE) ||
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        ((Service->connFlags != GOEP_NEW_CONN) && 
        (Service->connFlags != GOEP_MUX_CONN))) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }

    if (Service->connFlags == GOEP_MUX_CONN) {
        /* This is an existing connection.  Make sure we aren't trying 
         * to add the same profile twice.
         */
        if (GOES(servers)[Service->connId].profiles[Service->type]) {
            status = OB_STATUS_INVALID_PARM;
            goto Error;
        }
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    Assert(Service && (Service->type <= GOEP_MAX_PROFILES) &&
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) && 
           ((Service->connFlags == GOEP_NEW_CONN) || 
           (Service->connFlags == GOEP_MUX_CONN)));

    /* Create an OBEX connection if available, and if the Server requests a 
     * new connection 
     */
    if ((GOES(connCount) < GOEP_NUM_OBEX_SERVER_CONS) && 
        (Service->connFlags == GOEP_NEW_CONN)) {
        /* Get a free server conn Id */
        if (!GetFreeConnId(&id, GOEP_SERVER)) {
            status = OB_STATUS_NO_RESOURCES;
            goto Error;
        }
        OBEX_InitAppHandle(&GOES(servers)[id].obs.handle, GOES(servers)[id].
                    headerBlock, (U16)GOEP_SERVER_HB_SIZE, obStoreFuncs);

        GOES(servers)[id].obs.goep_service_type = Service->bt_service;
       
        status = OBEX_ServerInit(&GOES(servers)[id].obs, GoepSrvrCallback, 
                                 OBEX_EnumTransports(), &Service->l2data );

        if (status == OB_STATUS_SUCCESS) {
            Service->connId = id;
            /* Increment the current number of Obex connections */
            GOES(connCount)++;
            //OS_Report("goep:id %d 0x%x %d",0, (Service->target[0]==0?0:Service->target[0]->target[0]), (Service->target[0]==0?0:Service->target[0]->targetLen));
            goep_conn_id = goep_find_free_goep_id( 0, (Service->target[0]==0?0:Service->target[0]->target), (Service->target[0]==0?0:Service->target[0]->targetLen));
            if(goep_conn_id < 0)
            {
                status = OB_STATUS_NO_RESOURCES;
                goto Error;
            }
            else
            {
                GOES(servers)[id].obs.goep_conn_id = (U8)goep_conn_id;
                GOES(servers)[id].obs.goep_buff_ptr = NULL;
                GOES(servers)[id].obs.goep_buff_size = 0;
                GOES(servers)[id].obs.goep_buff_wrote_len = 0;
                GOES(servers)[id].obs.goep_first_packet_received = KAL_FALSE;
                GOES(servers)[id].obs.goep_buff_rawheader = KAL_FALSE;
                global_goep_connection[goep_conn_id] = (U8*)Service;
                global_goep_conn_count++;
            }            
        }
    }
    else if ((GOES(connCount) >= GOEP_NUM_OBEX_SERVER_CONS) && 
             (Service->connFlags == GOEP_NEW_CONN)) {
        /* Not enough Obex connections allocated in GOEP to allocate a new 
         * Connection Id 
         */
        kal_trace(BT_TRACE_G6_OBEX, GOEP_OBEX_CONNECTION_NUM, GOES(connCount)); 
        status = OB_STATUS_NO_RESOURCES;
        goto Error;
    }

    if (status == OB_STATUS_SUCCESS) {
#if OBEX_SERVER_CONS_SIZE > 0
        GOEP_Report( "[GOEP][SERV] RegisterServer numTargets:%d", Service->numTargets );
        for (i = 0; i < Service->numTargets; i++) {
/* all target registered with type 0  */
/* Assert(Service->type != GOEP_PROFILE_OPUSH);*/
            status = OBEX_ServerRegisterTarget(&GOES(servers)[Service->connId].obs, 
                                               Service->target[i]);
            if (status != OB_STATUS_SUCCESS) {
#if OBEX_DEINIT_FUNCS == XA_ENABLED
                if (GOES(servers)[Service->connId].connCount == 0)
                    OBEX_ServerDeinit(&GOES(servers)[Service->connId].obs);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
                goto Error;
            }
        }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        /* Set this server to "in use" */
        GOES(servers)[Service->connId].connCount++;
        GOES(servers)[Service->connId].profiles[Service->type] = Service;
    }

Error:
    OS_UnlockStack();
    return status;
}

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_RegisterServerSecurityRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers a security record for the GOEP service.
 *
 * Return:    (See goep.h)
 */
BtStatus GOEP_RegisterServerSecurityRecord(GoepServerApp *Service, BtSecurityLevel Level)
{
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

    /* Register security for the GOEP service */
    Service->secRecord.id =  SEC_GOEP_ID;
    Service->secRecord.channel = (U32)Service;
    Service->secRecord.level = Level;
    status = SEC_Register(&Service->secRecord);

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_UnregisterServerSecurityRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Unregisters a security handler for the GOEP service.
 *
 * Return:    (See goep.h)
 */
BtStatus GOEP_UnregisterServerSecurityRecord(GoepServerApp *Service)
{
    BtStatus status;

    OS_LockStack();

    /* Unregister security for the GOEP Service */
    status = SEC_Unregister(&Service->secRecord);

    OS_UnlockStack();

    return status;
}
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            GOEP_GetObexServer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieves the OBEX Server pertaining to a specific GOEP Server.  
 *            This function is valid after GOEP_Init and GOEP_RegisterServer 
 *            have been called.
 *
 * Return:    ObexServerApp
 *
 */
ObexServerApp* GOEP_GetObexServer(GoepServerApp *Service)
{
    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Service) || (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS)) {
        OS_UnlockStack();
        return 0;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert(Service && (Service->connId < GOEP_NUM_OBEX_SERVER_CONS));

    OS_UnlockStack();
    return &(GOES(servers)[Service->connId].obs);
}

/*---------------------------------------------------------------------------
 * GOEP_SetPullObjectLength()
 *
 *     To set the length of the sending object for pull indication
 *     
 * Parameters:
 *     Service - A registered GoepServerApp structure. 
 *     obj_len - The length of sending object
 *
 * Returns: 
 *     none
 *
 */
void GOEP_SetPullObjectLength(GoepServerApp *Service, U32 obj_len)
{
    Assert(Service && (Service->connId < GOEP_NUM_OBEX_SERVER_CONS));

    GOES(servers)[Service->connId].currOp.info.pushpull.objectLen = obj_len;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_DeregisterServer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_DeregisterServer(GoepServerApp *Service)
{
    ObStatus    status;
    GoepServerObexCons *server;
#if OBEX_SERVER_CONS_SIZE > 0
    U8          i;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Service) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }

    if (GOES(initialized) != TRUE) {
        /* GOEP is not initialized, so there is nothing to deinit */
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert(Service);

    server = &GOES(servers)[Service->connId];
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) ||
#if OBEX_SERVER_CONS_SIZE > 0
        (Service->numTargets > OBEX_SERVER_CONS_SIZE) ||
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        (server->profiles[Service->type] != Service)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((Service->type < GOEP_MAX_PROFILES) &&
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Service->type] == Service));

    /* We had better have a connection open */
    if (GOES(connCount) == 0) {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    /* See if they are processing an operation */
    if ((server->currOp.oper != GOEP_OPER_NONE) &&
        (server->currOp.handler == Service)) {
        status = OB_STATUS_BUSY;
        goto Error;
    }
    server->profiles[Service->type] = 0;

#if OBEX_SERVER_CONS_SIZE > 0
    for (i = 0; i < Service->numTargets; i++) {
        OBEX_ServerDeregisterTarget(&server->obs, Service->target[i]);
    }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

    if (--(server->connCount) > 0) {
        status = OB_STATUS_SUCCESS;
        goto Error;
    }

    global_goep_connection[server->obs.goep_conn_id] = NULL;
    server->obs.goep_conn_id = 0xFF;
    global_goep_conn_count--;   
    --GOES(connCount);
    
    status = OBEX_ServerDeinit(&server->obs);

Error:
    OS_UnlockStack();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            GOEP_ServerAbort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Abort the current server operation.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ServerAbort(GoepServerApp *Service, ObexRespCode Resp)
{
    ObStatus    status;
    GoepServerObexCons *server;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Service) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert(Service);

    server = &GOES(servers)[Service->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_ENTER_SERVER_ABORT, Service, Resp);
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) ||
        (server->profiles[Service->type] != Service)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((Service->type < GOEP_MAX_PROFILES) &&
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Service->type] == Service));

    if (((Service != 0) && (Service != server->currOp.handler)) ||
        (server->currOp.oper == GOEP_OPER_NONE)) {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    status = OBEX_ServerAbort(&server->obs, Resp);

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVERABORT_STATUS__xD , status);
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_ServerAccept
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called to accept a Push or Pull request.
 *            It MUST be called during the GOEP_EVENT_PROVIDE_OBJECT indication for
 *            these operations. Failure to do so will abort the operation.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ServerAccept(GoepServerApp *Service, void *Obsh)
{
    ObStatus            status;
    GoepServerObexCons *server;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Service) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert(Service);

    kal_trace(BT_TRACE_G6_OBEX, GOEP_ENTER_SERVER_ACCEPT, Service);

    server = &GOES(servers)[Service->connId];
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) ||
        (server->profiles[Service->type] != Service)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((Service->type < GOEP_MAX_PROFILES) && 
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Service->type] == Service));

    if (server->currOp.handler != Service) {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (server->currOp.oper == GOEP_OPER_PULL && 
        server->currOp.info.pushpull.objectLen)
        OBEXH_Build4Byte(&server->obs.handle, OBEXH_LENGTH, 
                          server->currOp.info.pushpull.objectLen);

    server->object = Obsh;

    OBEX_ServerAccept(&server->obs, server->object);

    status = OB_STATUS_SUCCESS;

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVERACCEPT_STATUS__xD , status);
    return status;
}


#if OBEX_BODYLESS_GET == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ServerAcceptNoObject
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accepts a GET without specifying a target object.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ServerAcceptNoObject(GoepServerApp *Service)
{
    ObStatus            status;
    GoepServerObexCons *server;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Service) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert(Service);

    server = &GOES(servers)[Service->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_ENTER_SERVER_ACCEPT_NO_OBJECT, Service);
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) ||
        (server->profiles[Service->type] != Service) ||
        (server->currOp.oper != GOEP_OPER_PULL)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((Service->type < GOEP_MAX_PROFILES) && 
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Service->type] == Service) &&
           (server->currOp.oper == GOEP_OPER_PULL));

    if ((server->currOp.handler != Service) 
        /* || (server->currOp.event != GOEP_EVENT_PROVIDE_OBJECT) */
        /* revise for PBAP */
        || (server->currOp.event != GOEP_EVENT_PROVIDE_OBJECT && server->currOp.event != GOEP_EVENT_CONTINUE)
        ) {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    server->object = 0;

    OBEX_ServerAcceptNoObject(&server->obs);

    status = OB_STATUS_SUCCESS;

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVERACCEPT_STATUS__xD , status);
    return status;
}
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */


/*---------------------------------------------------------------------------
 *            GOEP_ServerContinue
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called to keep data flowing between the client and the server.
 *            This function must be called once for every GOEP_EVENT_CONTINUE
 *            event received. It does not have to be called in the context
 *            of the callback. It can be deferred for flow control reasons.
 *
 * Return:    ObStatus
 */
ObStatus GOEP_ServerContinue(GoepServerApp *Service)
{
    ObStatus            status;
    GoepServerObexCons *server;
    U8 l_enable = 1;

    OS_LockStack();

    kal_trace(BT_TRACE_G6_OBEX, GOEP_ENTER_SERVER_CONTINUE, Service);

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Service) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert(Service);

    server = &GOES(servers)[Service->connId];
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) ||
        (server->profiles[Service->type] != Service)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((Service->type < GOEP_MAX_PROFILES) && 
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Service->type] == Service));
#ifdef __BT_GOEP_V2__
    if ((server->currOp.handler != Service)  ||
        ((server->currOp.event != GOEP_EVENT_CONTINUE) &&
        ((server->currOp.event != GOEP_EVENT_START) || (server->currOp.oper != GOEP_OPER_ABORT)))) {
#else
    if ((server->currOp.handler != Service) || (!server->oustandingResp) ||
        ((server->currOp.event != GOEP_EVENT_CONTINUE) &&
        ((server->currOp.event != GOEP_EVENT_START) || (server->currOp.oper != GOEP_OPER_ABORT)))) {
#endif

        kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_CONTINUE_FAILED,server->currOp.handler,server->oustandingResp,server->currOp.event,server->currOp.oper);
        status = OB_STATUS_FAILED;
        goto Error;
    }

#ifdef __BT_GOEP_V2__
    // ALPS00237212 ftpc pull + ftps push and cause ftpc disconnect
    if( TRUE == GOEP_ServerInternalPending(Service) ){
        OS_Report("[SERVER] already pending");
        status = OB_STATUS_PENDING;
        goto Error;
    } 

    if( server->obs.server.srm_server != OBSH_SRM_ENABLE ){
        if( (!server->oustandingResp) ){
            kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_CONTINUE_FAILED,server->currOp.handler,server->oustandingResp,server->currOp.event,server->currOp.oper);
            status = OB_STATUS_FAILED;
            goto Error;
        }
    }

	// check the srm state
	if( (server->obs.server.srm_flags&OBSF_SERVER_ENABLE)!= 0  && server->obs.server.srm_server == 0 ){
		GOEP_Report("[SRM][SERVER] srm_flags:0x%x", server->obs.server.srm_flags);
		if( (server->obs.server.srm_flags&OBSF_CLIENT_ENABLE)!= 0 ){
			GOEP_Report("[SRM][SERVER] enter srm mode from no");
			server->obs.server.srm_server = OBSH_SRM_ENABLE;
		}
		if( (server->obs.server.srm_flags&OBSF_CLIENT_WAITING)!= 0 ){
			server->obs.server.srm_server = OBSH_SRM_WAITENABLE;
		}else if( (server->obs.server.srm_flags&OBSF_SERVER_WAITING)!= 0 ){
			server->obs.server.srm_server = OBSH_SRM_WAITENABLE;
		}
	}
        // check the srmwait to srm state
	if( server->obs.server.srm_server == OBSH_SRM_WAITENABLE ){
		GOEP_Report("[SRM][SERVER] srm_flags:0x%x", server->obs.server.srm_flags);
		if( (server->obs.server.srm_flags&OBSF_CLIENT_WAITING)== 0 && (server->obs.server.srm_flags&OBSF_SERVER_WAITING)== 0){
			GOEP_Report("[SRM][SERVER] enter srm mode from waitenable");
			server->obs.server.srm_server = OBSH_SRM_ENABLE;
		}
	}

        if( (server->obs.server.srm_flags & OBSF_SERVER_WAITING)!= 0 ){
            if( FALSE == GOEP_ServerQueueHeader(Service, OBEXH_SRM_PARAM, &l_enable, 1) ){
                GOEP_Report("[SRM][ERR] no more space for srm wait. before:0x%x after:0x%x",server->obs.server.srm_flags , server->obs.server.srm_flags& (~OBSF_CLIENT_WAITING) );
                server->obs.server.srm_flags &= (~OBSF_SERVER_WAITING);
            }
        }
#endif

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ServerBuildHeaders(server) == FALSE) {
        OBEXH_FlushHeaders(&server->obs.handle);
        kal_trace(BT_TRACE_G6_OBEX, GOEP_SERVER_CONTINUE_BUILDHEADER_FAIL);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    server->oustandingResp = FALSE;
    status = OBEX_ServerSendResponse(&server->obs);

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVERCONTINUE_STATUS__xD , status);
    return status;
}

BOOL GOEP_ServerInternalPending(GoepServerApp *Service){
    GoepServerObexCons *server;
    server = &GOES(servers)[Service->connId];
    if( ( OBPF_PENDING & server->obs.handle.parser.flags ) == OBPF_PENDING){
//        GOEP_Report("[SRM] parser.flags:0x%x", server->obs.handle.parser.flags);
        return TRUE;
    }
    return FALSE;
}

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ServerAuthenticate
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Respond to an authentication challenge received from a client, 
 *            or send a challenge to the client.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ServerAuthenticate(GoepServerApp     *Service, 
                                 ObexAuthResponse  *Response,
                                 ObexAuthChallenge *Challenge )
{
    ObStatus    status;
    GoepServerObexCons *server;
    BOOL success = FALSE;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Service) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Service);

    server = &GOES(servers)[Service->connId];

    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVERAUTH, Service, Response, Challenge);

#if XA_ERROR_CHECK == XA_ENABLED
    if ((Challenge && Response) || (!Challenge && !Response) ||
        (Service->type >= GOEP_MAX_PROFILES) || 
        (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS) ||
        (server->profiles[Service->type] != Service)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Challenge || Response) && !(Challenge && Response) && 
           (Service->type < GOEP_MAX_PROFILES) && 
           (Service->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Service->type] == Service));

    if (server->currOp.handler != Service) {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    /* Build Challenge if one was provided. Otherwise build a
     * response, as long as we've received a challenge.
     */
    if (Challenge) {
        success = OBEXH_BuildAuthChallenge(&server->obs.handle, Challenge, 
                                            server->nonce);
        /*
         * Since we are challenging the client, the proper thing
         * to do is to respond with unauthorized.
         */
        status = OBEX_ServerAbort(&server->obs, OBRC_UNAUTHORIZED);
        if (status != OB_STATUS_SUCCESS) goto Error;
    } 
    else if (server->flags & GOEF_CHALLENGE) {
        success = OBEXH_BuildAuthResponse(&server->obs.handle, Response, 
                                           server->currOp.challenge.nonce);
        server->flags &= ~GOEF_CHALLENGE;
    }

    if (!success) {
        status = OB_STATUS_FAILED;
    } else {
        status = OB_STATUS_SUCCESS;
    }

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVERAUTH_STATUS__xD , status);
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_ServerVerifyAuthResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Verifies the received authentication response.
 *
 * Return:    TRUE if client's response is authenticated.
 *
 */
BOOL GOEP_ServerVerifyAuthResponse(GoepServerApp *Service, U8 *Password, 
                                   U8 PasswordLen)
{
    BOOL                    status;
    ObexAuthResponseVerify  verify;
    GoepServerObexCons     *server;

    OS_LockStack();

    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_VERIFYAUTHRESP, Service);

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Service) || (!Password)) {
        status = FALSE;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Service && Password);

    server = &GOES(servers)[Service->connId];
  
    if (server->flags & GOEF_RESPONSE) {
        /* Verify the authentication response */
        verify.nonce = server->nonce;
        verify.digest = server->currOp.response.digest;
        verify.password = Password;
        verify.passwordLen = PasswordLen;

        status = OBEXH_VerifyAuthResponse(&verify);
    } 
	else 
		status = FALSE;

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_VERIFYAUTHRESP_RETURN, status);
    return status;
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ServerGetTpConnInfo
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieves OBEX transport layer connection information.  This 
 *            function can be called when a transport connection is active to 
 *            retrieve connection specific information.   
 *
 * Return:    
 *     TRUE - The tpInfo structure was successfully completed.
 *     FALSE - The transport is not connected (XA_ERROR_CHECK only).
 *
 */
BOOL GOEP_ServerGetTpConnInfo(GoepServerApp *Service, 
                                   ObexTpConnInfo *tpInfo)
                                   
{
    GoepServerObexCons *server;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Service) || (!tpInfo)) {
        OS_UnlockStack();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Service && tpInfo);

    server = &GOES(servers)[Service->connId];
    
    OS_UnlockStack();
    return OBEX_GetTpConnInfo(&server->obs.handle, tpInfo);
}
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 *            GOEP_ServerQueueHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function queues a Byte Sequence, UNICODE, 1-byte, or 4-byte 
 *            OBEX header for transmission by the GOEP Server.
 *
 * Return:    True if the OBEX Header was queued successfully.
 */
BOOL GOEP_ServerQueueHeader(GoepServerApp *Server, ObexHeaderType Type,
                            const U8 *Value, U16 Len) {
    U8                  i;
    BOOL                status;
    GoepServerObexCons *server;

    OS_LockStack();

    kal_trace(BT_TRACE_G6_OBEX, GOEP_SERVER_QUEUEHEADER, Type, Len);

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) { 
        status = FALSE;
        goto Error;
    }

    /* Only allow certain headers to be queued for transmission */
    if ((Type == OBEXH_TYPE) || (Type == OBEXH_TARGET) || 
        (Type == OBEXH_WHO) || (Type == OBEXH_CONNID) ||
#if OBEX_SESSION_SUPPORT == XA_ENABLED
        (Type == OBEXH_SESSION_PARAMS) || (Type == OBEXH_SESSION_SEQ_NUM) ||
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        (Type == OBEXH_AUTH_CHAL) || (Type == OBEXH_AUTH_RESP) || (Type == OBEXH_WAN_UUID)) {
        status = FALSE;
        goto Error;
    }

#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
    /* Unicode headers must have an even length if we aren't
     * automatically performing the unicode conversions. 
     */
    if (OBEXH_IsUnicode(Type) && ((Len % 2) != 0)) {
        status = FALSE;
        goto Error;
    }
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED */
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Server);

    server = &GOES(servers)[Server->connId];
 
#if XA_ERROR_CHECK == XA_ENABLED
    /* Non-Body headers cannot exceed the available OBEX packet size */
    if ((Type != OBEXH_BODY) && (Len > OBEXH_GetAvailableTxHeaderLen(&server->obs.handle))) {
        status = FALSE;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) {
        if (server->queuedHeaders[i].type == 0) {
            /* Found an empty header to queue */
            server->queuedHeaders[i].type = Type;
            server->queuedHeaders[i].buffer = Value;
            server->queuedHeaders[i].len = Len;
            status = TRUE;
            goto Error;
        }
    }

    /* No empty headers available */
    status = FALSE;

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_ClearServerQueuedHeaders()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function clear the queued headers of the GOEP Server.
 *
 * Return:    True if the queued OBEX Headers was cleard successfully.
 */
BOOL GOEP_ClearServerQueuedHeaders(GoepServerApp *Server)
{
    U8                  i;
    BOOL                status;
    GoepServerObexCons *server;
   
    OS_LockStack();
    
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    { 
            status = FALSE;
            goto Error;
    }
#endif

    ASSERT(Server);

    server = &GOES(servers)[Server->connId];

    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) 
    {
        OS_MemSet((U8 *)&server->queuedHeaders[i], 0, sizeof(server->queuedHeaders[i]));
    }

    status = TRUE;

Error:
    OS_UnlockStack();
    return status;
}


#endif /* GOEP_ADDITIONAL_HEADERS > 0 */


#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ServerTpConnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Transport Connection from the GOEP Server.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ServerTpConnect(GoepServerApp *Server, ObexTpAddr *Target)
{
    ObStatus			status;
    GoepServerObexCons *server;
#if BT_STACK == XA_ENABLED
	U8					i;
	BOOL				targetAddr = FALSE;
#endif /* BT_STACK == XA_ENABLED */

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Server);

    server = &GOES(servers)[Server->connId];
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Server->type >= GOEP_MAX_PROFILES) ||
        (Server->connId >= GOEP_NUM_OBEX_SERVER_CONS) || !Target ||
        (server->profiles[Server->type] != Server)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Server->type < GOEP_MAX_PROFILES) && 
           (Server->connId < GOEP_NUM_OBEX_SERVER_CONS) && Target &&
           (server->profiles[Server->type] == Server));

    if (server->flags & GOEF_ACTIVE) {
        /* Already connected */
        status = OB_STATUS_SUCCESS;
        goto Error;
    }

#if BT_STACK == XA_ENABLED
	for (i = 0; i < BD_ADDR_SIZE; i++) {
		/* Check if a Bluetooth address has been provided by the app. If any
		 * byte in the 6-byte Bluetooth address is nonzero, we assume it to 
		 * be a valid Bluetooth address.
		 */
		if (Target->proto.bt.addr.addr[i] != 0x00) {
			targetAddr = TRUE;
			break;
		}
	}
	
    if ((Target->type == OBEX_TP_BLUETOOTH) && (!targetAddr)) {
		/* No address was provided, so we'll use the Device Manager 
		 * to allow the application to select a device.
		 */
		GOES(tpAddr).type = Target->type;
	}
#endif /* BT_STACK == XA_ENABLED */

    status = OBEX_ServerTpConnect(&server->obs, Target);
    
    if (status == OB_STATUS_SUCCESS) {
        /* The transport connection is up */
        server->currOp.oper = GOEP_OPER_NONE;
        server->flags |= (GOEF_ACTIVE|GOEF_SERVER_TP_INITIATED);
    } 
    else if (status == OB_STATUS_PENDING) {
        /* The transport connection is coming up */
        server->currOp.handler = Server;
        server->flags |= GOEF_SERVER_TP_INITIATED;
    }

Error:
    OS_UnlockStack();
    return status;
}
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ServerTpDisconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate destruction of an OBEX Transport Connection from the
 *            GOEP Server.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ServerTpDisconnect(GoepServerApp *Server)
{
    ObStatus    status;
    GoepServerObexCons *server;

    kal_trace(BT_TRACE_G6_OBEX, GOEP_ENTER_SERVER_TPDISCONNECT, Server);

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Server);

    server = &GOES(servers)[Server->connId];
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Server->type >= GOEP_MAX_PROFILES) ||
        (Server->connId >= GOEP_NUM_OBEX_SERVER_CONS) || 
        (server->profiles[Server->type] != Server)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Server->type < GOEP_MAX_PROFILES) && 
           (Server->connId < GOEP_NUM_OBEX_SERVER_CONS) &&
           (server->profiles[Server->type] == Server));


    if ((server->flags & GOEF_ACTIVE) == 0) {
        /* Already disconnected */
        kal_trace(BT_TRACE_G6_OBEX, GOEP_NO_ACTIVE_CONNECT);

        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }

    /* It's time to disconnect the transport connection */
    status = OBEX_ServerTpDisconnect(&server->obs, TRUE);

    if (status == OB_STATUS_SUCCESS) {
        /* The transport connection is down */
        server->currOp.oper = GOEP_OPER_NONE;
        server->flags &= ~GOEF_ACTIVE;
        server->flags &= ~GOEF_SERVER_TP_INITIATED;
    } 
    else if (status == OB_STATUS_PENDING) {
        /* The transport connection is going down */
        server->currOp.handler = Server;
    }

Error:
    OS_UnlockStack();
    kal_trace(BT_TRACE_G6_OBEX, GOEP_SERVER_TPDISCONNECT_RETURN, status);
    return status;
}
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */

#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_RegisterClient()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_RegisterClient(GoepClientApp *Client, 
                             const ObStoreFuncTable *obStoreFuncs)
{
    U8        id;
    ObStatus    status = OB_STATUS_SUCCESS;
    int goep_conn_id;
#ifdef __BT_GOEP_V2__
    U8 idx;
#endif

    kal_trace(BT_TRACE_G6_OBEX, GOEP_REGISTER_CLIENT, Client);

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (GOEC(initialized) != TRUE) {
        /* GOEP is not initialized */
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if ((!Client) || (Client->type >= GOEP_MAX_PROFILES) || 
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || 
        ((Client->connFlags != GOEP_NEW_CONN) && 
        (Client->connFlags != GOEP_MUX_CONN))) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }

    if (Client->connFlags == GOEP_MUX_CONN) {
        /* This is an existing connection.  Make sure we aren't trying 
         * to add the same profile twice.
         */
        if (GOEC(clients)[Client->connId].profiles[Client->type]) {
            // Maybe usesless Client->connId is updated later
            status = OB_STATUS_INVALID_PARM;
            goto Error;
        }
    }
    
    if( 0 != Client){
        /* Check previous connected Profile exited or not*/
        if( GoepGetPreviousRegisterConnIdByProfile(Client->type, GOEP_CLIENT, &id) ){
            OS_Report("[GOEP] error ! has a privious registered service_type:%d id:%d client", Client->type, id);
            goto Error;
        }
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client && (Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && 
           ((Client->connFlags == GOEP_NEW_CONN) || 
           (Client->connFlags == GOEP_MUX_CONN)));

    /* Create an OBEX connection if available, and if the Client requests 
     * a new connection 
     */
    if ((GOEC(connCount) < GOEP_NUM_OBEX_CLIENT_CONS) && 
        (Client->connFlags == GOEP_NEW_CONN)) {
        /* Get a free client conn Id */
        if (!GetFreeConnId(&id, GOEP_CLIENT)) {
            status = OB_STATUS_NO_RESOURCES;
            goto Error;
        }
        /* Init the variable. MAUI_03127760 */
        OS_Report("[GOEP] get a free id:%d Client->type:%d", id, Client->type);
        GOEC(clients)[id].flags = 0;
        GOEC(clients)[id].obc.goep_service_type = Client->type;

        OBEX_InitAppHandle(&GOEC(clients)[id].obc.handle, 
                GOEC(clients)[id].headerBlock, (U16)GOEP_CLIENT_HB_SIZE, obStoreFuncs);

        status = OBEX_ClientInit(&GOEC(clients)[id].obc, GoepClntCallback, 
                        OBEX_EnumTransports());

#ifdef __BT_GOEP_V2__
        GOEC(clients)[id].obc.l2buffer.rxNum = 0;
        if( Client->l2capRawDataNum > 0 ){
            GOEC(clients)[id].obc.l2buffer.rxNum = Client->l2capRawDataNum;
            GOEP_Report("[GOEP] RegisterClient l2capRawDataNum is %d ", Client->l2capRawDataNum);
            for( idx = 0; idx< Client->l2capRawDataNum && idx < GOEPL2_MAX_RETRANS_NUM ; idx++){
                kal_trace(BT_TRACE_G6_OBEX, GOEP_L2CAP_RAWDATA_BUFF, Client->pl2capRawData[idx]);
                ASSERT( Client->l2capRawData[idx] != NULL );
                GOEC(clients)[id].obc.l2buffer.rfcRxPackets[idx].data = Client->pl2capRawData[idx];
                InsertTailList( &GOEC(clients)[id].obc.l2buffer.rxPacketsBufferList, &GOEC(clients)[id].obc.l2buffer.rfcRxPackets[idx].node);
                GOEC(clients)[id].obc.l2buffer.rxNum = idx+1;
            }
        }
#endif
        if (status == OB_STATUS_SUCCESS) {
            Client->connId = id;
            /* Increment the current number of Obex connections */
            GOEC(connCount)++;

            goep_conn_id = goep_find_free_goep_id(1, 0, 0);
            if(goep_conn_id < 0)
            {
                status = OB_STATUS_NO_RESOURCES;
                goto Error;
            }
            else
            {
                GOEC(clients)[id].obc.goep_conn_id = (U8)goep_conn_id;
                GOEC(clients)[id].obc.req_id = 0xFF;
                GOEC(clients)[id].obc.goep_buff_ptr = NULL;
                GOEC(clients)[id].obc.goep_buff_size = 0;
                GOEC(clients)[id].obc.goep_buff_wrote_len = 0;
                GOEC(clients)[id].obc.goep_first_packet_received = KAL_FALSE;
                GOEC(clients)[id].obc.goep_auth_use = KAL_FALSE;
                global_goep_connection[goep_conn_id] = (U8*)Client;
                global_goep_conn_count++;

                /* clear oper infomation */
                OS_MemSet((U8 *)&GOEC(clients)[id].currOp, 0, sizeof(GOEC(clients)[id].currOp));
            }   
        }
    }
    else if ((GOEC(connCount) >= GOEP_NUM_OBEX_CLIENT_CONS) && 
             (Client->connFlags == GOEP_NEW_CONN)) {
        /* Not enough Obex connections allocated in GOEP to allocate 
         * a new Connection Id 
         */
        kal_trace(BT_TRACE_G6_OBEX, GOEP_OBEX_CONNECTION_NUM, GOEC(connCount)); 
        status = OB_STATUS_NO_RESOURCES;
        goto Error;
    }

    if (status == OB_STATUS_SUCCESS) {
        /* Set this client to "in use" */
        GOEC(clients)[Client->connId].connCount++;
        GOEC(clients)[Client->connId].profiles[Client->type] = Client;
        Client->connState = CS_DISCONNECTED;
        Client->obexConnId = OBEX_INVALID_CONNID;
        
        if( GOEC(clients)[Client->connId].connCount > 1 ){
            OS_Report("GOEP: connId:%d has connCount:%d ", Client->connId, GOEC(clients)[Client->connId].connCount);
        }
    }

Error:
    OS_UnlockStack();
    return status;
}

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_RegisterClientSecurityRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers a security record for the GOEP client.
 *
 * Return:    (See goep.h)
 */
BtStatus GOEP_RegisterClientSecurityRecord(GoepClientApp *Client, BtSecurityLevel Level)
{
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

    /* Register security for the GOEP service */
    Client->secRecord.id =  SEC_GOEP_ID;
    Client->secRecord.channel = (U32)Client;
    Client->secRecord.level = Level;
    status = SEC_Register(&Client->secRecord);

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_UnregisterClientSecurityRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Unregisters a security handler for the GOEP client.
 *
 * Return:    (See goep.h)
 */
BtStatus GOEP_UnregisterClientSecurityRecord(GoepClientApp *Client)
{
    BtStatus status;

    OS_LockStack();

    /* Unregister security for the GOEP Client */
    status = SEC_Unregister(&Client->secRecord);

    OS_UnlockStack();

    return status;
}
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            GOEP_GetObexClient()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieves the OBEX Client pertaining to a specific GOEP Client.  
 *            This function is valid after GOEP_Init and GOEP_RegisterClient 
 *            have been called.
 *
 * Return:    ObexClientApp
 *
 */
ObexClientApp* GOEP_GetObexClient(GoepClientApp *Client)
{
    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Client) || (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS)) {
        OS_UnlockStack();
        return 0;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client && (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS));
    
    OS_UnlockStack();
    return &(GOEC(clients)[Client->connId].obc);
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_DeregisterClient()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Client.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_DeregisterClient(GoepClientApp *Client)
{   
    ObStatus    status;
    GoepClientObexCons *client;

    kal_trace(BT_TRACE_G6_OBEX, GOEP_DEREGISTER_CLIENT, Client);

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }

    if (GOEC(initialized) != TRUE) {
        /* GOEP is not initialized, so there is nothing to deinit */
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) || 
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT((Client->type < GOEP_MAX_PROFILES) &&
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) &&
           (client->profiles[Client->type] == Client));

    /* See if it is connected */
/*
    if (Client->connState != CS_DISCONNECTED) {
        status = OB_STATUS_BUSY;
        goto Error;
    }
*/
    client->profiles[Client->type] = 0;

    ASSERT(GOEC(connCount) > 0);

    if (--(client->connCount) > 0) {
        status = OB_STATUS_SUCCESS;
        goto Error;
    }

    global_goep_connection[client->obc.goep_conn_id] = NULL;
    global_goep_conn_count--;
    ASSERT(global_goep_conn_count>=0);

    --GOEC(connCount);

    client->obc.goep_conn_id = 0xFF;
    client->obc.req_id = 0xFF;

    status = OBEX_ClientDeinit(&client->obc);

Error:
    OS_UnlockStack();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            GOEP_Connect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issue an OBEX Connect Request. If the Connect parameter is not
 *            used, set it to zero.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_Connect(GoepClientApp *Client, GoepConnectReq *Connect)
{
#if GOEP_ADDITIONAL_HEADERS > 0
    U16                 more;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
    ObStatus            status;
    GoepClientObexCons *client;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    BOOL                success;
    ObexTpConnInfo      tpInfo;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_CONNECT_ENTERING, Client, client, Connect->mru);
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) || 
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || 
        (client->profiles[Client->type] != Client)) {
        //Report(("[GOEP] GOEP_Connect..OB_STATUS_INVALID_PARM.\n"));
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) &&
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CONNECT_STATUS_BUSY);
        status = OB_STATUS_BUSY;
        goto Error;
    }

    if (Connect) {
        /* Build OBEX Connect headers */
        if (Connect->target)
            OBEXH_BuildByteSeq(&client->obc.handle, OBEXH_TARGET, 
                Connect->target, Connect->targetLen);
        if (Connect->who)
            OBEXH_BuildByteSeq(&client->obc.handle, OBEXH_WHO, 
                Connect->who, Connect->whoLen);
#if OBEX_AUTHENTICATION == XA_ENABLED
        Client->goep_auth_resp_sent = FALSE;			
        if (Connect->response) {
            if ((client->flags & GOEF_CHALLENGE) == 0) {
				kal_trace(BT_TRACE_G6_OBEX, GOEP_CONNECT_NO_CHALLENGE);	
                status = OB_STATUS_INVALID_PARM;
                goto Error;
            }
            OBEXH_BuildAuthResponse(&client->obc.handle, Connect->response, 
                client->currOp.challenge.nonce);

            Client->goep_auth_resp_sent = TRUE;			
        }
#if GOEP_AUTH == XA_ENABLED
        Client->goep_auth_chal_server= FALSE;
        Client->goep_auth_verify_fail = TRUE;
#endif
        if (Connect->challenge)
        {
            OBEXH_BuildAuthChallenge(&client->obc.handle, Connect->challenge, 
                client->nonce);

#if GOEP_AUTH == XA_ENABLED
            Client->goep_auth_chal_server = TRUE;
#endif
        }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

	    client->obc.handle.mru = Connect->mru;
    }
	else
	{
		client->obc.handle.mru = 0;
    }
    GOEP_Report("[OBEX] connect mru:%d", client->obc.handle.mru);
    client->currOp.oper = GOEP_OPER_CONNECT;
    /* IOT for FTP client with HTCTytnII */
    if( Connect->target && Connect->targetLen == 16 ){
        // && Connect->target[6] == 0x11 && Connect->target[7] == 0xd2 ){
        client->obc.client.idFlag = 1 ;
    }else{
        client->obc.client.idFlag = 0 ;
    }
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CONNID_VALUE, client->obc.client.idFlag);
    

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        ASSERT(more == 0);
        OBEXH_FlushHeaders(&client->obc.handle);
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CONNECT_BUILDHEADERS_FAILED);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

#if BT_SECURITY == XA_ENABLED
    /* Check security for this GoepClientApp */
    OS_MemSet((U8*)&(Client->secToken), 0, sizeof(Client->secToken) );

    Client->secToken.id = SEC_GOEP_ID;

    if (client->flags & GOEF_CLIENT_TP_INITIATED) {
        /* Client initiated transport connection */
        Client->secToken.remDev = client->obc.trans.ObexClientBtTrans.remoteDevice;
    } 
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    else {
        /* Must be server initiated, check the transport connection information */
        success = OBEX_GetTpConnInfo(&client->obc.handle, &tpInfo);
        ASSERT(success);
        Client->secToken.remDev = tpInfo.remDev;
    }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    Client->secToken.channel = (U32)Client;
    Client->secToken.incoming = FALSE;
    status = SEC_AccessRequest(&(Client->secToken));
    if (status == BT_STATUS_SUCCESS) {
        status = OBEX_ClientConnect(&client->obc);
    } else if (status != BT_STATUS_PENDING) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CONNECT_STATUS_RESTRICTED);
        status = OB_STATUS_RESTRICTED;
    }
#else /* BT_SECURITY == XA_ENABLED */
    status = OBEX_ClientConnect(&client->obc);
#endif /* BT_SECURITY == XA_ENABLED */

    if (status == OB_STATUS_PENDING) {
        client->currOp.handler = Client;
        /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
        client->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    } else {
        client->currOp.oper = GOEP_OPER_NONE;
    }

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issue an OBEX Disconnect Request.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_Disconnect(GoepClientApp *Client)
{
#if GOEP_ADDITIONAL_HEADERS > 0
    U16         more;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
    ObStatus    status;
    GoepClientObexCons *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_DISCONNECT_ENTERING, Client, client);
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || 
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && 
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_IS_OPERATING);
        status = OB_STATUS_BUSY;
        goto Error;
    }

    if (Client->obexConnId != OBEX_INVALID_CONNID)
        OBEXH_Build4Byte(&client->obc.handle, OBEXH_CONNID, 
                Client->obexConnId);

    client->currOp.oper = GOEP_OPER_DISCONNECT;

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        ASSERT(more == 0);
        OBEXH_FlushHeaders(&client->obc.handle);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_OBEX_CLIENTDISCONNECT);

    status = OBEX_ClientDisconnect(&client->obc);
    if (status == OB_STATUS_PENDING) {
        client->currOp.handler = Client;
        /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
        client->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
        Client->obexConnId = OBEX_INVALID_CONNID;
    } else {
        client->currOp.oper = GOEP_OPER_NONE;
    }

Error:
    OS_UnlockStack();
    return status;
}

#ifdef __BT_GOEP_V2__

BOOL OBEXH_BuildSRM(ObexAppHandle *AppHndl){
    /* Assume SRM is 1 byte */
    OS_LockObex();
    SRM_FLOW("[SRM] OBEXH_BuildSRM apphndl:0x%x", AppHndl);
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !ObIsHeaderSpaceAvail(AppHndl, 2)) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && AppHndl->buffer != 0);
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, 2));
    
    AppHndl->buffer[AppHndl->txLength++] = OBEXH_SRM; // TODO: Type;
    AppHndl->buffer[AppHndl->txLength++] = 1; // TODO: Value;

    OS_UnlockObex();
    return TRUE;
}

BOOL OBEXH_BuildSRMWait(ObexAppHandle *AppHndl){
    /* Assume SRMP Header is 1 byte */
    OS_LockObex();
    SRM_FLOW("[SRM] OBEXH_BuildSRMWait apphndl:0x%x", AppHndl);
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !ObIsHeaderSpaceAvail(AppHndl, 2)) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && AppHndl->buffer != 0);
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, 2));
    
    AppHndl->buffer[AppHndl->txLength++] = OBEXH_SRM_PARAM; // TODO: Type;
    AppHndl->buffer[AppHndl->txLength++] = 1; //Value;

    OS_UnlockObex();
    return TRUE;
}

BOOL OBEX_UpdateSRMFlag(ObexAppHandle *AppHndl, U8 Role){
    SRM_FLOW("[SRM] OBEX_UpdateSRMFlag apphndl:0x%x Role:%d", AppHndl, Role);
    AppHndl->SRMFlag |= Role;
	return TRUE;
}

BOOL OBEX_UpdateSRMWaitFlag(ObexAppHandle *AppHndl, U8 Role){
    SRM_FLOW("[SRM] OBEX_UpdateSRMWaitFlag apphndl:0x%x Role:%d", AppHndl, Role);
    AppHndl->SRMWaitFlag |= Role;
	return TRUE;
}

BOOL OBEX_ClearSRMFlag(ObexAppHandle *AppHndl){
    SRM_FLOW("[SRM] OBEX_ClearSRMFlag apphndl:0x%x", AppHndl);
    AppHndl->SRMFlag = 0;
	return TRUE;
}

BOOL OBEX_ClearSRMWaitFlag(ObexAppHandle *AppHndl){
    SRM_FLOW("[SRM] OBEX_ClearSRMWaitFlag apphndl:0x%x", AppHndl);
    AppHndl->SRMWaitFlag = 0;
	return TRUE;
}

BOOL OBEX_IsSRMMode(ObexAppHandle *AppHndl){
    BT_BOOL ret = FALSE;
    if( (GOEP_SERVER & AppHndl->SRMFlag) && (GOEP_CLIENT & AppHndl->SRMFlag) ){
        ret = TRUE;
    }

	return TRUE;
}

BOOL OBEX_IsSRMWait(ObexAppHandle *AppHndl){
    BT_BOOL ret = FALSE;
    if( (GOEP_SERVER & AppHndl->SRMFlag) || (GOEP_CLIENT & AppHndl->SRMFlag) ){
        ret = TRUE;
    }
    SRM_FLOW("[SRM] OBEX_IsSRMWait SRMWaitFlag:%d ret:%d", AppHndl->SRMWaitFlag, ret);
	return TRUE;
}
#endif

extern BOOL OBEXH_BuildEmptyEndBody(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 *            GOEP_Push
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issue an OBEX Put. To delete an object, set the Object Store
 *            Handle in the GoepObjectReq to zero.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_Push(GoepClientApp *Client, GoepObjectReq *Object)
{
#if GOEP_ADDITIONAL_HEADERS > 0
    U16         more;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
    U16         len;
    ObStatus    status;
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U16         uniName[GOEP_MAX_UNICODE_LEN];
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    GoepClientObexCons *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_PUSH_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS)|| (!Object) ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && Object &&
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_IS_OPERATING);
        status = OB_STATUS_BUSY;
        goto Error;
    }

#if OBEX_PUT_DELETE == XA_DISABLED
    if (Object->object == 0) {
        status = OB_STATUS_INVALID_HANDLE;
        goto Error;
    }
#endif /* OBEX_PUT_DELETE == XA_DISABLED */

    if (Client->obexConnId != OBEX_INVALID_CONNID)
        OBEXH_Build4Byte(&client->obc.handle, OBEXH_CONNID, 
            Client->obexConnId);

    if (Object->name) {
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
        /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the name header
         * in Unicode form, to prevent memory corruption problems
         */
        ASSERT(GOEP_MAX_UNICODE_LEN >= (Strlen(Object->name) * 2 + 2)); 
        len = AsciiToUnicode(uniName, Object->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, (U8 *)uniName, 
                len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
        len = Unilen(Object->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 
                (U8 *)Object->name, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    }

    if (Object->type) {
        len = Strlen((const char*)Object->type) + 1;
        if (len > 1)
            OBEXH_BuildByteSeq(&client->obc.handle, OBEXH_TYPE, Object->type, 
                len);
    }

    if (Object->objectLen) 
        OBEXH_Build4Byte(&client->obc.handle, OBEXH_LENGTH, Object->objectLen);

    
    client->obc.client.srm_client = 0;
    client->obc.client.srm_flags = 0;
    Client->lastprogress = 0;
    SRM_Report("[SRM][CLIENT] reset srm_client:0x%x srm_flags:0x%x", client->obc.client.srm_client, client->obc.client.srm_flags);
    SRM_Report("[SRM][CLIENT] pushobject_req srm_flags:0x%x", Object->srm_flags );
    /* append client enable flag */
    if( (Object->srm_flags & OBSF_CLIENT_ENABLE) == OBSF_CLIENT_ENABLE ){
        OBEXH_Build1Byte(&client->obc.handle, OBEXH_SRM, OBSH_SRM_ENABLE);
        client->obc.client.srm_flags |= OBSF_CLIENT_ENABLE;
    }

	/* append client srmwait flag */
    if( (Object->srm_flags & OBSF_CLIENT_WAITING) == OBSF_CLIENT_WAITING ){
        OBEXH_Build1Byte(&client->obc.handle, OBEXH_SRM_PARAM, OBSH_SRM_ENABLE);
        client->obc.client.srm_flags |= OBSF_CLIENT_WAITING;
    }

    client->object = Object->object;
    client->currOp.oper = GOEP_OPER_PUSH;

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        ASSERT(more == 0);
        OBEXH_FlushHeaders(&client->obc.handle);
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_BUILDHEADERS_FAILED);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    /* <!-- added by yufeng for BIP put-empty */    
    if (Object->objectLen == 0) 
    {
        /* objectLen == 0: maybe PUT-DELETE or CREATE-EMPTY */
        if (Object->object == 0)
        {
            /* PUT-DELETE: no BODY or END-OF-BODY header */
        }
        else
        {
            /* CREATE-EMPTY: with an empty END-OF-BODY header */
            OBEXH_BuildEmptyEndBody(&client->obc.handle);
        }
    }
    /*  added by yufeng --> */    

    /* Put or Put Delete based on the value of Object->object */
    if(client->obc.goep_buff_ptr != NULL)
        client->obc.client.objLen = Object->objectLen;
    status = OBEX_ClientPut(&client->obc, Object->object); 

    if (status == OB_STATUS_PENDING) {

#if OBEX_PUT_DELETE == XA_ENABLED
        if (Object->object == 0)
            client->currOp.oper = GOEP_OPER_DELETE;
#endif /* OBEX_PUT_DELETE == XA_ENABLED */
        client->currOp.handler = Client;
        /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
        client->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    } else {
        client->currOp.oper = GOEP_OPER_NONE;
    }

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_Pull
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issue an OBEX Get.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_Pull(GoepClientApp *Client, GoepObjectReq *Object, BOOL More)
{
    ObStatus    status;
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U16         uniName[GOEP_MAX_UNICODE_LEN];
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    GoepClientObexCons *client;
    U16         len, more = 0;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_PULL_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS)|| (!Object) ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && Object &&
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_IS_OPERATING);
        status = OB_STATUS_BUSY;
        goto Error;
    }
/*
    if (Object->object == 0) {
        status = OB_STATUS_INVALID_HANDLE;
        goto Error;
    }
*/
    if (Client->obexConnId != OBEX_INVALID_CONNID)
        OBEXH_Build4Byte(&client->obc.handle, OBEXH_CONNID, 
            Client->obexConnId);

    /* reset flags */
    client->obc.client.srm_client = 0;
    client->obc.client.srm_flags = 0;
    Client->lastprogress = 0;    
    SRM_Report("[SRM][CLIENT] reset srm_client:%x srm_flags:%x", client->obc.client.srm_client, client->obc.client.srm_flags);

    /* append client enable flag */
    if( (Object->srm_flags & OBSF_CLIENT_ENABLE) == OBSF_CLIENT_ENABLE ){
        OBEXH_Build1Byte(&client->obc.handle, OBEXH_SRM, OBSH_SRM_ENABLE);
        client->obc.client.srm_flags |= OBSF_CLIENT_ENABLE;
    }

	/* append client srmwait flag */
    if( (Object->srm_flags & OBSF_CLIENT_WAITING) == OBSF_CLIENT_WAITING ){
        OBEXH_Build1Byte(&client->obc.handle, OBEXH_SRM_PARAM, OBSH_SRM_ENABLE);
        client->obc.client.srm_flags |= OBSF_CLIENT_WAITING;
    }

	SRM_Report("[SRM] GOEP_Pull client.srm_flags:0x%x", client->obc.client.srm_flags);

    if (Object->name) {
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
        /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the name header
         * in Unicode form, to prevent memory corruption problems
         */
        ASSERT(GOEP_MAX_UNICODE_LEN >= (Strlen(Object->name) * 2 + 2)); 
        len = AsciiToUnicode(uniName, Object->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, (U8 *)uniName, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
        len = Unilen(Object->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, 
                    OBEXH_NAME, (U8 *)Object->name, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    }

    if (Object->type) {
        len = Strlen((const char*)Object->type) + 1;
        if (len > 1)
            OBEXH_BuildByteSeq(&client->obc.handle, OBEXH_TYPE, 
                Object->type, len);
    }

    client->currOp.oper = GOEP_OPER_PULL;
    client->currOp.info.pull.objectLen = 0;/* clear objectLen */

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        OBEXH_FlushHeaders(&client->obc.handle);
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_BUILDHEADERS_FAILED);
        
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    client->object = Object->object;

    status = OBEX_ClientGet(&client->obc, client->object, (((more > 0) || More)? TRUE : FALSE));
    if (status == OB_STATUS_PENDING) {
        client->currOp.handler = Client;
        /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
        client->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    } else {
        client->currOp.oper = GOEP_OPER_NONE;
    }

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_SetFolder
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issue an OBEX SetPath command.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_SetFolder(GoepClientApp *Client, GoepSetFolderReq *Folder)
{
#if GOEP_ADDITIONAL_HEADERS > 0
    U16         more;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
    U16         len;
    ObStatus    status;
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U16         uniName[GOEP_MAX_UNICODE_LEN];
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    GoepClientObexCons *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_SETFOLDER_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || (!Folder) ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) &&
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && Folder &&
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_IS_OPERATING);
        status = OB_STATUS_BUSY;
        goto Error;
    }

    if (Client->obexConnId != OBEX_INVALID_CONNID)
        OBEXH_Build4Byte(&client->obc.handle, 
                OBEXH_CONNID, Client->obexConnId);

    if (Folder->name) {
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
        /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the name header
         * in Unicode form, to prevent memory corruption problems
         */
        ASSERT(GOEP_MAX_UNICODE_LEN >= (Strlen(Folder->name) * 2 + 2)); 
        len = AsciiToUnicode(uniName, Folder->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, (U8 *)uniName, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
        len = Unilen(Folder->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 
                (U8 *)Folder->name, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 
                0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    } else if (Folder->reset) {
        /* Path reset requires an empty name header to be formed */
        OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
    }

    client->currOp.oper = GOEP_OPER_SETFOLDER;

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        ASSERT(more == 0);
        OBEXH_FlushHeaders(&client->obc.handle);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    status = OBEX_ClientSetPath(&client->obc, Folder->flags);
    if (status == OB_STATUS_PENDING) {
        client->currOp.handler = Client;
        /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
        client->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    } else {
        client->currOp.oper = GOEP_OPER_NONE;    
    }

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_PerformAction
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issue an OBEX ACTION command.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_PerformAction(GoepClientApp *Client, GoepActionReq *actionReq)
{
    //
#if GOEP_ADDITIONAL_HEADERS > 0
    U16         more;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
    U16         len;
    ObStatus    status;
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U16         uniName[GOEP_MAX_UNICODE_LEN];
    U16         unidstName[GOEP_MAX_UNICODE_LEN];
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    GoepClientObexCons *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_SETFOLDER_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || (!actionReq) ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) &&
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && actionReq &&
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_IS_OPERATING);
        status = OB_STATUS_BUSY;
        goto Error;
    }

    if (Client->obexConnId != OBEX_INVALID_CONNID)
        OBEXH_Build4Byte(&client->obc.handle, 
                OBEXH_CONNID, Client->obexConnId);

    OBEXH_Build1Byte(&client->obc.handle, OBEXH_ACTIONID, actionReq->id);

    if (actionReq->name) {
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
        /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the name header
         * in Unicode form, to prevent memory corruption problems
         */
        ASSERT(GOEP_MAX_UNICODE_LEN >= (Strlen(actionReq->name) * 2 + 2)); 
        len = AsciiToUnicode(uniName, actionReq->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, (U8 *)uniName, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
        len = Unilen(actionReq->name) + 2;
        if (len > 2)
            OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 
                (U8 *)actionReq->name, len);
        else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_NAME, 
                0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
	}

    if( OBAP_SETPARAMISSION == actionReq->id ){
        OBEXH_Build4Byte(&client->obc.handle, OBEXH_PERMISSION, actionReq->permission);        
    }else{
        if (actionReq->dstname) {
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
            /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the name header
             * in Unicode form, to prevent memory corruption problems
             */
            ASSERT(GOEP_MAX_UNICODE_LEN >= (Strlen(actionReq->dstname) * 2 + 2)); 
            len = AsciiToUnicode(unidstName, actionReq->dstname) + 2;
            if (len > 2)
                OBEXH_BuildUnicode(&client->obc.handle, OBEXH_DSTNAME, (U8 *)unidstName, len);
            else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_DSTNAME, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
            len = Unilen(actionReq->dstname) + 2;
            if (len > 2)
                OBEXH_BuildUnicode(&client->obc.handle, OBEXH_DSTNAME, 
                    (U8 *)actionReq->dstname, len);
            else OBEXH_BuildUnicode(&client->obc.handle, OBEXH_DSTNAME, 
                    0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
        }
    } 

    client->currOp.oper = GOEP_OPER_ACTION;

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        ASSERT(more == 0);
        OBEXH_FlushHeaders(&client->obc.handle);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    status = OBEX_ClientAction(&client->obc, actionReq->flags);
    if (status == OB_STATUS_PENDING) {
        client->currOp.handler = Client;
        /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
        client->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    } else {
        client->currOp.oper = GOEP_OPER_NONE;    
    }

Error:
    OS_UnlockStack();
    return status;
}


/*---------------------------------------------------------------------------
 *            GOEP_ClientAbort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Abort the current operation.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_ClientAbort(GoepClientApp *Client)
{
    GoepClientObexCons *client;
    ObStatus status = OB_STATUS_FAILED;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENTABORT_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) &&
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) &&
           (client->profiles[Client->type] == Client));

    /* to avoid app send 2 ABORT request continuously */
    if(client->obc.client.status == OBRC_USER_ABORT)
    {   
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENTABORT_ALREADY_PENDING);
        /* previous request is ABORT request also */
        status = OB_STATUS_BUSY;
        goto Error;
    }


    if (Client->obexConnId != OBEX_INVALID_CONNID)
        OBEXH_Build4Byte(&client->obc.handle, OBEXH_CONNID, 
            Client->obexConnId);

    if (Client == client->currOp.handler)
    {
        status = OBEX_ClientAbort(&client->obc);
    }
    else
    {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_HANDLE_ERROR, client->currOp.handler);
    }

Error:
    OS_UnlockStack();
    return status;
}

BOOL GOEP_ClientInternalPending(GoepClientApp *Client){
    GoepClientObexCons *client;
    client = &GOEC(clients)[Client->connId];
    if( ( OBPF_PENDING & client->obc.handle.parser.flags ) == OBPF_PENDING){
//        GOEP_Report("[SRM] parser.flags:0x%x", server->obs.handle.parser.flags);
        return TRUE;
    }
    return FALSE;
}

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ClientVerifyAuthResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Verifies the received authentication response.
 *
 * Return:    TRUE if server's response is authenticated.
 *
 */
BOOL GOEP_ClientVerifyAuthResponse(GoepClientApp *Client, 
                                   U8 *Password, 
                                   U8 PasswordLen)
{
    BOOL                    status;
    ObexAuthResponseVerify  verify;
    GoepClientObexCons     *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Client) || (!Password)) {
        status = FALSE;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client && Password);

    client = &GOEC(clients)[Client->connId];
    
    if (client->flags & GOEF_RESPONSE) {
        /* Verify the authenticaiton response */
        verify.nonce = client->nonce;
        verify.digest = client->currOp.response.digest;
        verify.password = Password;
        verify.passwordLen = PasswordLen;

        status = OBEXH_VerifyAuthResponse(&verify);
        goto Error;
    }

    status = FALSE;

Error:
    OS_UnlockStack();
    return status;
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ClientGetTpConnInfo
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieves OBEX transport layer connection information.  This 
 *            function can be called when a transport connection is active to 
 *            retrieve connection specific information.   
 *
 * Return:    
 *     TRUE - The tpInfo structure was successfully completed.
 *     FALSE - The transport is not connected (XA_ERROR_CHECK only).
 *
 */
BOOL GOEP_ClientGetTpConnInfo(GoepClientApp *Client, 
                                   ObexTpConnInfo *tpInfo)
                                   
{
    GoepClientObexCons *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Client) || (!tpInfo)) {
        OS_UnlockStack();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client && tpInfo);

    client = &GOEC(clients)[Client->connId];
        
    OS_UnlockStack();
    return OBEX_GetTpConnInfo(&client->obc.handle, tpInfo);
}
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            GOEP_TpConnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Transport Connection.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_TpConnect(GoepClientApp *Client, ObexTpAddr *Target)
{
    ObStatus			status;
    GoepClientObexCons *client;
#if BT_STACK == XA_ENABLED
	U8					i;
	BOOL				targetAddr = FALSE;
#endif /* BT_STACK == XA_ENABLED */

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_TPCONNECT_ENTERING, Client, client);
    
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || !Target ||
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) && Target &&
           (client->profiles[Client->type] == Client));

    if (client->currOp.handler) {
        kal_trace(BT_TRACE_G6_OBEX, GOEP_TPCONNECT_STATUS_BUSY);
        status = OB_STATUS_BUSY;
        goto Error;
    }

    if (client->flags & GOEF_ACTIVE) {
        Client->connState = CS_CONNECTED;
        status = OB_STATUS_SUCCESS;
        kal_trace(BT_TRACE_G6_OBEX, GOEP_TPCONNECT_ACTIVE);
        goto Error;
    }

#if BT_STACK == XA_ENABLED
	for (i = 0; i < BD_ADDR_SIZE; i++) {
		/* Check if a Bluetooth address has been provided by the app. If any
		 * byte in the 6-byte Bluetooth address is nonzero, we assume it to 
		 * be a valid Bluetooth address.
		 */
		if (Target->proto.bt.addr.addr[i] != 0x00) {
			targetAddr = TRUE;
			break;
		}
	}
	
	if ((Target->type == OBEX_TP_BLUETOOTH) && (!targetAddr) && 
		(Client->connState != CS_CONNECTING)) {
		/* No address was provided, so we'll use the Device Manager 
		 * to allow the application to select a device.
		 */
		GOEC(tpAddr).type = Target->type;
		GOEC(storedAppPtr) = Client;
	        Client->connState = CS_CONNECTING;
	}
#endif /* BT_STACK == XA_ENABLED */

#ifdef __BT_GOEP_V2__
    Target->proto.bt.tptype = Client->target_type; /* Copy the target's level from ObexClientApp */
#endif

    status = OBEX_ClientTpConnect(&client->obc, Target);
    
    if (status == OB_STATUS_SUCCESS) {
        /* The transport connection is up */
        client->currOp.oper = GOEP_OPER_NONE;
        Client->connState = CS_CONNECTED;
        client->flags |= (GOEF_ACTIVE|GOEF_CLIENT_TP_INITIATED);
    } 
    else if (status == OB_STATUS_PENDING) {
        /* The transport connection is coming up */
        Client->connState = CS_CONNECTING;
        client->currOp.handler = Client;
        client->flags |= GOEF_CLIENT_TP_INITIATED;
    }

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_TpDisconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate destruction of an OBEX Transport Connection.
 *
 * Return:    ObStatus
 *
 */
ObStatus GOEP_TpDisconnect(GoepClientApp *Client)
{
    ObStatus    status;
    GoepClientObexCons *client;
    I8          i, inUse = 0;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
    
    kal_trace(BT_TRACE_G6_OBEX, GOEP_TPDISCONNECT_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || 
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) &&
           (client->profiles[Client->type] == Client));
/* For cancel connect
    if (client->currOp.handler) {
        status = OB_STATUS_BUSY;
        goto Error;
    }

    if (Client->connState != CS_CONNECTED) {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
*/
    for (i = 0; i < GOEP_MAX_PROFILES; i++) {
        if ((client->profiles[i]) &&
            (client->profiles[i]->connState == CS_CONNECTED)) {
            inUse++;
        }
    }

    if (inUse > 1) {
        /* Tell the client its disconnected */
        Client->connState = CS_DISCONNECTED;
        status = OB_STATUS_SUCCESS;
        kal_trace(BT_TRACE_G6_OBEX, GOEP_TPDISCONNECT_INUSE);
        goto Error;
    }

    kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_OBEX_CLIENTDISCONNECT);

    /* It's time to disconnect the transport connection */
    status = OBEX_ClientTpDisconnect(&client->obc, TRUE);

    kal_trace(BT_TRACE_G6_OBEX, GOEP_TPDISCONNECT_STATUS, status);

    if (status == OB_STATUS_SUCCESS) {
        /* The transport connection is down */
        client->currOp.oper = GOEP_OPER_NONE;
        Client->connState = CS_DISCONNECTED;
        client->flags &= ~GOEF_ACTIVE;
        client->flags &= ~GOEF_CLIENT_TP_INITIATED;
    } 
    else if (status == OB_STATUS_PENDING) {
        /* The transport connection is going down */
        client->currOp.handler = Client;
    }

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_ClientContinue
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called to keep data flowing between the client and the server.
 *            This function must be called once for every GOEP_EVENT_CONTINUE
 *            event received. It does not have to be called in the context
 *            of the callback. It can be deferred for flow control reasons.
 *
 * Return:    ObStatus
 */
ObStatus GOEP_ClientContinue(GoepClientApp *Client)
{
#if GOEP_ADDITIONAL_HEADERS > 0
    U16                 more;
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
    ObStatus            status = OB_STATUS_FAILED;
    GoepClientObexCons *client;
    U8 l_enable = 1;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
       status = OB_STATUS_INVALID_PARM;
       goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];
        
    kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENTCONTINUE_ENTERING, Client, client);
#if XA_ERROR_CHECK == XA_ENABLED
    /* Verify Client Conn Id */
    if ((Client->type >= GOEP_MAX_PROFILES) ||
        (Client->connId >= GOEP_NUM_OBEX_CLIENT_CONS) || 
        (client->profiles[Client->type] != Client)) {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT((Client->type < GOEP_MAX_PROFILES) && 
           (Client->connId < GOEP_NUM_OBEX_CLIENT_CONS) &&
           (client->profiles[Client->type] == Client));

    if ((client->currOp.handler != Client) ||
        (client->currOp.event != GOEP_EVENT_CONTINUE)) {
        status = OB_STATUS_FAILED;
        goto Error;
    }

// IOT FIX. if there is a conection id, then send it out
// APPEND: CONNID
    if ( client->obc.client.idFlag == 1 && client->obc.client.obConnId != OBEX_INVALID_CONNID ){        
        if( client->currOp.oper == GOEP_OPER_PULL ){
        	kal_trace(BT_TRACE_G6_OBEX, OBEX_APPEND_CONNID_QUEUEHEADER);
        	OBEXH_Build4Byte(&client->obc.handle, 
                OBEXH_CONNID, Client->obexConnId);
        }
    }
// endif // APPEND: CONNID  

#ifdef __BT_GOEP_V2__
    if( (client->obc.client.srm_flags & OBSF_CLIENT_WAITING)!= 0 ){
        SRM_Report("[SRM] build client wait parameter");
        if( FALSE == GOEP_ClientQueueHeader(Client, OBEXH_SRM_PARAM, &l_enable, 1) ){
            GOEP_Report("[SRM][ERR] no more space for srm wait. before:0x%x after:0x%x",client->obc.client.srm_flags , client->obc.client.srm_flags& (~OBSF_CLIENT_WAITING) );
            client->obc.client.srm_flags &= (~OBSF_CLIENT_WAITING);
        }
    }
#endif
    // has been pending 
    if( GOEP_ClientInternalPending(Client) ){
        GOEP_Report("[SRM] has been pending inside");
        goto Error; 
    }

#ifdef __BT_GOEP_V2__
    /* remove server's wait flag */
    if( (client->obc.client.srm_flags & OBSF_SERVER_WAITING) != 0 ){
        SRM_Report("[SRM] Remove server's wait flag");
        client->obc.client.srm_flags &= (~OBSF_SERVER_WAITING);
    }
#endif

#if GOEP_ADDITIONAL_HEADERS > 0
    if (ClientBuildHeaders(client, &more) == FALSE) {
        OBEXH_FlushHeaders(&client->obc.handle);
        kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_BUILDHEADERS_FAILED);
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

    if( TRUE == GOEP_ClientInternalPending(Client) ){
        GOEP_Report("[SRM] no more free packet!");
        goto Error;
    }

    status = OBEX_ClientSendCommand(&client->obc);

Error:
    OS_UnlockStack();
    return status;
}

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 *            GOEP_ClientQueueHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function queues a Byte Sequence, UNICODE, 1-byte, or 4-byte 
 *            OBEX header for transmission by the GOEP Client.
 *
 * Return:    True if the OBEX Header was queued successfully.
 */
BOOL GOEP_ClientQueueHeader(GoepClientApp *Client, ObexHeaderType Type,
                                   const U8 *Value, U16 Len) {
    U8                  i;
    BOOL                status;
    GoepClientObexCons *client;

    kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_QUEUEHEADER, Type, Len);
    
    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client) {
        status = FALSE;
        goto Error;
    }
   
    /* NAME and TYPE headers are not recommended, since they are typically built by GOEP
     * when initiating Push, Pull, or SetPath operations. If header ordering is critical, 
     * these headers can be queued in a different order.  If this is done, be sure not to 
     * add these headers a second time when the Push, Pull, or SetPath operation is initiated.
     */

    /* Only allow certain headers to be queued for transmission */
    if ((Type == OBEXH_TARGET) || (Type == OBEXH_WHO) || (Type == OBEXH_CONNID) ||
#if OBEX_SESSION_SUPPORT == XA_ENABLED
        (Type == OBEXH_SESSION_PARAMS) || (Type == OBEXH_SESSION_SEQ_NUM) ||
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        (Type == OBEXH_AUTH_CHAL) || (Type == OBEXH_AUTH_RESP) || (Type == OBEXH_WAN_UUID)) {
        status = FALSE;
        goto Error;
    }

#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
    /* Unicode headers must have an even length if we aren't
     * automatically performing the unicode conversions. 
     */
    if (OBEXH_IsUnicode(Type) && ((Len % 2) != 0)) {
        status = FALSE;
        //Report(("[GOEP]GOEP_ClientQueueHeader, Unicode parameter error!.\n"));
        goto Error;
    }
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED */
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];

#if XA_ERROR_CHECK == XA_ENABLED
#ifdef __BT_GOEP_V2__
	if( Type == OBEXH_SRM || Type == OBEXH_SRM_PARAM ){
		// skip check ! allow to add it
	}else{
#else
    {
#endif
    /* Non-Body headers cannot exceed the available OBEX packet size */
    if ((Type != OBEXH_BODY) && (Len > OBEXH_GetAvailableTxHeaderLen(&client->obc.handle))) {
        status = FALSE;
        kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_QUEUEHEADER_LENTOOLARGE, Len, OBEXH_GetAvailableTxHeaderLen(&client->obc.handle));
        goto Error;
    }
	}
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) {
        if (client->queuedHeaders[i].type == 0) {
            /* Found an empty header to queue */
            client->queuedHeaders[i].type = Type;
            client->queuedHeaders[i].buffer = Value;
            client->queuedHeaders[i].len = Len;
            status = TRUE;
            goto Error;
        }
    }

    /* No empty headers available */
    //Report(("[GOEP]GOEP_ClientQueueHeader, No empty headers available!.\n"));
    status = FALSE;

Error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            GOEP_ClearClientQueuedHeaders()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function clear the queued headers of the GOEP Client.
 *
 * Return:    True if the queued OBEX Headers was cleard successfully.
 */
BOOL GOEP_ClearClientQueuedHeaders(GoepClientApp *Client)
{
    U8                  i;
    BOOL                status;
    GoepClientObexCons *client;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
        if (!Client) 
        {
            status = FALSE;
            goto Error;
        }
#endif        

    ASSERT(Client);

    client = &GOEC(clients)[Client->connId];

    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) 
    {
        OS_MemSet((U8 *)&client->queuedHeaders[i], 0, sizeof(client->queuedHeaders[i]));
    }

    status = TRUE;

Error:
    OS_UnlockStack();
    return status;
}

#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            GOEP_GetConnectionId
 *---------------------------------------------------------------------------
 *
 *     
 * Synopsis:  This function call facilitates the use of the OBEX client/server
 *            multiplexing capabilities of the GOEP layer, by retrieving a 
 *            connection Id (if one exists) for an OBEX client/server 
 *            connection.  A connection Id will be returned if an OBEX client/
 *            server connection exists and if no instances of the requested 
 *            profile are currently registered.  This is due to the fact that 
 *            only one instance of a specific profile is allowed over the same 
 *            OBEX connection.  
 *
 * Return:    Connection Id - 0-254, 255 (No connection Id exists)
 */
U8 GOEP_GetConnectionId(GoepRole role, GoepProfile profileType)
{
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ROLE_SERVER == XA_ENABLED)
    U8 i;
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ROLE_SERVER == XA_ENABLED) */

    OS_LockStack();

    if (role == GOEP_CLIENT) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
        ASSERT(GOEC(connCount) <= GOEP_NUM_OBEX_CLIENT_CONS);
        for (i = 0; i < GOEC(connCount); i++) {
            if (!GOEC(clients)[i].profiles[profileType]) {
                OS_UnlockStack();
                return GOEC(clients)[i].connId;
            }
        }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */    
    }
    else if (role == GOEP_SERVER) {
#if OBEX_ROLE_SERVER == XA_ENABLED
        ASSERT(GOES(connCount) <= GOEP_NUM_OBEX_SERVER_CONS);
        for (i = 0; i < GOES(connCount); i++) {
            if (!GOES(servers)[i].profiles[profileType]) {
                OS_UnlockStack();
                return GOES(servers)[i].connId;
            }
        }
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */    
    }

    /* No Connection ID found */
    OS_UnlockStack();
    return 0xFF;
}

/****************************************************************************
 *
 * Internal Functions
 *
 ****************************************************************************/

#if OBEX_ROLE_SERVER == XA_ENABLED

/*---------------------------------------------------------------------------
 *            GoepSrvrCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX Server protocol events.
 *
 * Return:    void
 *
 */
static void GoepSrvrCallback(ObexServerCallbackParms *parms)
{
#if BT_SECURITY == XA_ENABLED
    BtStatus            status;
    BOOL                success;
    ObexTpConnInfo      tpInfo;
#endif /* BT_SECURITY == XA_ENABLED */
    GoepServerObexCons *server = (GoepServerObexCons*)parms->server;

//#if XA_DEBUG == XA_ENABLED
    server->lastEvent = ServerEventVerifier(server->lastEvent, parms->event);
//#endif /* XA_DEBUG == XA_ENABLED */
    ASSERT(IsObexLocked());

    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_CALLBACK,parms->event, server);

    switch ( parms->event ) {
    case OBSE_SEND_RESPONSE:
        server->oustandingResp = TRUE;

#if BT_SECURITY == XA_ENABLED
        if (parms->opcode == OB_OPCODE_CONNECT) {
            /* Make sure the Server has been assigned */
            if ((server->currOp.event == GOEP_EVENT_NONE) &&
                (server->currOp.handler == 0)) {
                /* The Server hasn't been assigned yet. Indicate the START
                 * event early to make sure the server is assigned.
                 */
                NotifyCurrServer(parms->server, GOEP_EVENT_START);
            }

            if (server->currOp.handler) {
                /* Check security for this GoepServerApp */

                OS_MemSet((U8*)&(server->currOp.handler->secToken), 0, sizeof(server->currOp.handler->secToken) );
                
                if ((server->flags & GOEF_SERVER_TP_INITIATED) == 0) {
                    /* Client initiated transport connection, check the 
                     * transport connection information 
                     */
                    success = OBEX_GetTpConnInfo(&server->obs.handle, &tpInfo);
                    ASSERT(success);
                    server->currOp.handler->secToken.remDev = tpInfo.remDev;
                } 
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
                else {
                    /* Must be server initiated */
                    server->currOp.handler->secToken.remDev = 
                        server->obs.trans.ObexClientBtTrans.remoteDevice;
                }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

                server->currOp.handler->secToken.id = SEC_GOEP_ID;
                server->currOp.handler->secToken.channel = (U32)server->currOp.handler;
                server->currOp.handler->secToken.incoming = TRUE;
                status = SEC_AccessRequest(&(server->currOp.handler->secToken));
                if (status == BT_STATUS_SUCCESS) {
                    NotifyCurrServer(parms->server, GOEP_EVENT_CONTINUE);
                } else if (status != BT_STATUS_PENDING) {
                    /* Abort the connection operation */
                    status = OBEX_ServerAbort(&server->obs, OBRC_UNAUTHORIZED);
                    ASSERT(status == OB_STATUS_SUCCESS);
                    NotifyCurrServer(parms->server, GOEP_EVENT_CONTINUE);
                }
            } else {
                /* No server existed to perform security with */
                NotifyCurrServer(parms->server, GOEP_EVENT_CONTINUE);
            }
        } else {
            /* Not processing a connect operation, proceed as normal */
            NotifyCurrServer(parms->server, GOEP_EVENT_CONTINUE);
        }
#else /* BT_SECURITY == XA_ENABLED */
        NotifyCurrServer(parms->server, GOEP_EVENT_CONTINUE);
#endif /* BT_SECURITY == XA_ENABLED */
        break;

    case OBSE_PRECOMPLETE:
        /* Remove this event indication to avoid to access OBEX data at the same time */
        /* --add this event ind for Partial Content rsp code */
        NotifyCurrServer(parms->server, GOEP_EVENT_PRECOMPLETE);
        break;

    case OBSE_CONNECTED:
        //Report(("[GOEP]GoepSrvrCallback get event: OBSE_CONNECTED\n"));
        server->flags |= GOEF_ACTIVE;

        server->currOp.oper = GOEP_OPER_NONE;
        NotifyAllServers(parms->server, GOEP_EVENT_TP_CONNECTED);
        break;

    case OBSE_DISCONNECT:
        //Report(("[GOEP]GoepSrvrCallback get event: OBSE_DISCONNECT\n"));
        server->flags &= ~GOEF_ACTIVE;
        server->flags &= ~GOEF_SERVER_TP_INITIATED;
        server->currOp.oper = GOEP_OPER_NONE;

        CLEAR_QUEUEDHEADERS(server);

        NotifyAllServers(parms->server, GOEP_EVENT_TP_DISCONNECTED);
        break;

    case OBSE_HEADER_RX:
        GoepServerProcessHeaders(parms);
        break;

    case OBSE_DELETE_OBJECT:
        /* Change operation to delete, drop into START state notification */
        server->currOp.oper = GOEP_OPER_DELETE;
        NotifyCurrServer(parms->server, GOEP_EVENT_DELETE_OBJECT);
        break;

    case OBSE_CREATE_EMPTY_OBJECT:
        server->currOp.oper = GOEP_OPER_CREATEEMPTY;
        NotifyCurrServer(parms->server, GOEP_EVENT_CREATE_EMPTY_OBJECT);
	break;
  
    case OBSE_PROVIDE_OBJECT:
        NotifyCurrServer(parms->server, GOEP_EVENT_PROVIDE_OBJECT);
        break;
        
    case OBSE_ABORTED:
       // Report(("[GOEP]GoepSrvrCallback get event: OBSE_ABORTED\n"));

        CLEAR_QUEUEDHEADERS(server);
        
        server->currOp.abortReason = parms->u.abortReason;
        
        if (server->currOp.event != GOEP_EVENT_NONE) {
            server->currOp.reason = parms->u.abortReason;
            GOEP_Report("[GOEP] OBSE_Aborted abortedoper:%d curop:%d", server->abortedOper, server->currOp.oper);
            if (server->currOp.oper == GOEP_OPER_PULL 
                || server->currOp.oper == GOEP_OPER_PUSH)
            {
                /* Remove srm */
                GOEP_Report("[SRM] OBSE_Aborted clean up srm flags");
                server->obs.server.srm_flags = server->obs.server.srm_server = 0;
            }
        
            /* Makes sure that the currOp.oper is correct during Aborts. */
            if (server->abortedOper != GOEP_OPER_NONE) {
                server->currOp.oper = server->abortedOper;
                server->abortedOper = GOEP_OPER_NONE;

                GOEP_Report("[SRM] OBSE_Aborted abortedOp clean up srm flags");
                server->obs.server.srm_flags = server->obs.server.srm_server = 0;
                
                NotifyCurrServer(parms->server, GOEP_EVENT_ABORTED);
            }
            /* revise for Partial Content response code */
            else if (server->currOp.oper == GOEP_OPER_PULL 
                || server->currOp.oper == GOEP_OPER_PUSH)
            {
                NotifyCurrServer(parms->server, GOEP_EVENT_ABORTED);
            }
        }
        server->currOp.oper = GOEP_OPER_NONE;
        break;

    case OBSE_COMPLETE:
        NotifyCurrServer(parms->server, GOEP_EVENT_COMPLETE);
        server->currOp.oper = GOEP_OPER_NONE;
        break;

    case OBSE_PUT_START:
        SRM_Report("[SRM][SERVER] OBSE_PUT_START clean up srm flags");
        server->obs.server.srm_flags = server->obs.server.srm_server = 0;        
        StartServerOperation(parms->server, GOEP_OPER_PUSH);
        break;

    case OBSE_GET_START:
        SRM_Report("[SRM][SERVER] OBSE_GET_START clean up srm flags");
        server->obs.server.srm_flags = server->obs.server.srm_server = 0;        
        StartServerOperation(parms->server, GOEP_OPER_PULL);
        break;
    
    case OBSE_SET_PATH_START:
        StartServerOperation(parms->server, GOEP_OPER_SETFOLDER);
        server->currOp.info.setfolder.flags = parms->u.setPathFlags;
        break;
    case OBSE_ACTION_START:
        StartServerOperation(parms->server, GOEP_OPER_ACTION);
        server->currOp.info.action.action_id = 0;
        server->currOp.info.action.permission = 0;
        server->currOp.info.action.dstnameLen = 0;
        server->currOp.info.action.nameLen = 0;
        break;
        
    case OBSE_CONNECT_START:
        StartServerOperation(parms->server, GOEP_OPER_CONNECT);
        break;
        
    case OBSE_DISCONNECT_START:

        CLEAR_QUEUEDHEADERS(server);

        StartServerOperation(parms->server, GOEP_OPER_DISCONNECT);
        break;
            
    case OBSE_ABORT_START:
        server->abortedOper = server->currOp.oper;
        StartServerOperation(parms->server, GOEP_OPER_ABORT);
        /* Inform the application that an Abort operation is
         * starting.  If an outstanding response exists, it must 
         * be sent via GOEP_ServerContinue to allow the Abort 
         * operation to complete.
         */
        ASSERT(server->currOp.handler == 0);
        AssociateServer(parms->server);

        CLEAR_QUEUEDHEADERS(server);

        NotifyCurrServer(parms->server, GOEP_EVENT_START);
        break;
    }
}


/*---------------------------------------------------------------------------
 *            GoepServerProcessHeaders
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process the incoming headers based on the current operation.
 *            Note that ConnId and Target are processed by the OBEX layer.
 *
 * Return:    void
 *
 */
static void GoepServerProcessHeaders(ObexServerCallbackParms *parms)
{
    GoepServerObexCons *server = (GoepServerObexCons*)parms->server;
	static U8 b_truncate = 0;

    switch (server->currOp.oper) {
    case GOEP_OPER_CONNECT:
#if GOEP_MAX_WHO_LEN > 0
        if (parms->u.headerRx.type == OBEXH_WHO) {
            DoHeaderCopy(server->currOp.info.connect.who, 
                &server->currOp.info.connect.whoLen, GOEP_MAX_WHO_LEN, 
                parms->u.headerRx.buff, parms->u.headerRx.currLen);
            return;
        }
#endif /* GOEP_MAX_WHO_LEN > 0 */
        break;

    case GOEP_OPER_PUSH:
    case GOEP_OPER_PULL:
        //GOEP_Report("[GOEP] RxHeader type:%d len:%d",parms->u.headerRx.type, parms->u.headerRx.currLen );
        switch (parms->u.headerRx.type) {
        case OBEXH_NAME:

            if (parms->u.headerRx.currLen > 0)
            {
				kal_trace(BT_TRACE_G6_OBEX,GOEP_PROCESS_NAME_HEADER,
				        server->currOp.info.pushpull.nameLen, 
				        parms->u.headerRx.currLen,
				        parms->u.headerRx.remainLen,
				        b_truncate);

            /* modified by yufeng 2007.9.5 for CR MAUI_00031499 */
            if (server->currOp.oper == GOEP_OPER_PUSH 
                && ((parms->u.headerRx.currLen > (GOEP_MAX_UNICODE_LEN * sizeof(GoepUniType) - server->currOp.info.pushpull.nameLen)) || (b_truncate == 1)))
            {
                /* truncate the filename */
                DoUniNameTruncateCopy(server->currOp.info.pushpull.name, 
                    &server->currOp.info.pushpull.nameLen, GOEP_MAX_UNICODE_LEN, 
                    parms->u.headerRx.buff, parms->u.headerRx.currLen, parms->u.headerRx.remainLen);                

				if (parms->u.headerRx.remainLen > 0)
				{
					/* when next packet comes, "DoUniNameTruncateCopy" will be called again */
					b_truncate = 1; 
				}
				else
				{
					b_truncate = 0;
				}
            }
            else
            {
            DoUniHeaderCopy(server->currOp.info.pushpull.name, 
                &server->currOp.info.pushpull.nameLen, GOEP_MAX_UNICODE_LEN, 
                parms->u.headerRx.buff, parms->u.headerRx.currLen);                
            }
            }
            return;

#if GOEP_MAX_TYPE_LEN > 0
        case OBEXH_TYPE:
            DoHeaderCopy(server->currOp.info.pushpull.type, 
                &server->currOp.info.pushpull.typeLen, GOEP_MAX_TYPE_LEN, 
                parms->u.headerRx.buff, parms->u.headerRx.currLen);                
            return;
#endif /* GOEP_MAX_WHO_LEN > 0 */
        case OBEXH_LENGTH:
            server->currOp.info.pushpull.objectLen = parms->u.headerRx.value;
        }
        break;

    case GOEP_OPER_SETFOLDER:
        if (parms->u.headerRx.type == OBEXH_NAME) {
            if (parms->u.headerRx.totalLen == 0) 
                server->currOp.info.setfolder.reset = TRUE;
            else server->currOp.info.setfolder.reset = FALSE;

            DoUniHeaderCopy(server->currOp.info.setfolder.name, 
                &server->currOp.info.setfolder.nameLen, GOEP_MAX_UNICODE_LEN,
                parms->u.headerRx.buff, parms->u.headerRx.currLen);                
            return;
        }
        break;
    case GOEP_OPER_ACTION:
        if (parms->u.headerRx.type == OBEXH_NAME) {
            DoUniHeaderCopy(server->currOp.info.action.name, 
                &server->currOp.info.action.nameLen, GOEP_MAX_UNICODE_LEN,
                parms->u.headerRx.buff, parms->u.headerRx.currLen);                
            return;
        }
        if (parms->u.headerRx.type == OBEXH_DSTNAME) {
            DoUniHeaderCopy(server->currOp.info.action.dstname, 
                &server->currOp.info.action.dstnameLen, GOEP_MAX_UNICODE_LEN,
                parms->u.headerRx.buff, parms->u.headerRx.currLen);                
            return;
        }
        if (parms->u.headerRx.type == OBEXH_ACTIONID) {
            server->currOp.info.action.action_id = parms->u.headerRx.value;
            return;
        }
        if (parms->u.headerRx.type == OBEXH_PERMISSION ) {
            server->currOp.info.action.permission = parms->u.headerRx.value;
            return;
        }
        
        break;

    case GOEP_OPER_DISCONNECT:
        break;
    }

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* We accept authentication headers with any operation. */
    switch (parms->u.headerRx.type) {
    case OBEXH_AUTH_CHAL:
        if (OBEXH_ParseAuthChallenge(&parms->server->handle, 
                    &server->currOp.challenge)) {
            /* Full auth challenge has been received, indicate it. */
            kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_RECEIVE_AUTHCHAL_HEADER);
            server->flags |= GOEF_CHALLENGE;
            NotifyCurrServer(parms->server, GOEP_EVENT_AUTH_CHALLENGE);
        }
        return;

    case OBEXH_AUTH_RESP:
        if (OBEXH_ParseAuthResponse(&parms->server->handle, 
                    &server->currOp.response)) {
            /* Full auth response has been received, indicate it. */
            server->flags |= GOEF_RESPONSE;
            NotifyCurrServer(parms->server, GOEP_EVENT_AUTH_RESPONSE);
        }
        return;
    }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

    if (/* parms->u.headerRx.type == OBEXH_TARGET || */
        parms->u.headerRx.type == OBEXH_CONNID) {
        /* We do not pass up Target or ConnId headers, in order 
         * for Directed connections and operations to setup properly.
         */
        /* pass up Target header -- Yufeng */
        return;
    }

    /* If we have not processed the OBEX header already we will accept it now, regardless
     * the current operation. 
     */
    server->currOp.header.type = parms->u.headerRx.type;
    if (OBEXH_Is1Byte(server->currOp.header.type) || 
        OBEXH_Is4Byte(server->currOp.header.type)) {
        /* 1-byte or 4-byte*/
        server->currOp.header.value = parms->u.headerRx.value;
    } else if (OBEXH_IsUnicode(server->currOp.header.type)) {
        /* Unicode */
#ifdef __GOEP_USE_ARRAY_HEADER_BUFFER__
        DoUniHeaderCopy(server->currOp.header.unicodeBuffer, &server->currOp.header.len, 
            GOEP_MAX_UNICODE_LEN, parms->u.headerRx.buff, parms->u.headerRx.currLen);                
        /* Don't indicate the Unicode header until it is completely copied */
        if (parms->u.headerRx.remainLen > 0) 
            return;
        /* Point the header buffer pointer to the unicode buffer */
        server->currOp.header.buffer = (U8 *)server->currOp.header.unicodeBuffer;

#else
        if (server->currOp.header.buffer == NULL)
        {
            server->currOp.header.buffer = get_ctrl_buffer(GOEP_MAX_UNICODE_LEN * sizeof(GoepUniType));
        }        
        DoUniHeaderCopy((GoepUniType*)server->currOp.header.buffer, &server->currOp.header.len, 
            GOEP_MAX_UNICODE_LEN, parms->u.headerRx.buff, parms->u.headerRx.currLen);                
        /* Don't indicate the Unicode header until it is completely copied */
        if (parms->u.headerRx.remainLen > 0) 
            return;
        
#endif        
    } else {
        /* Byte Sequence */
        ASSERT(OBEXH_IsByteSeq(server->currOp.header.type));
        server->currOp.header.totalLen = parms->u.headerRx.totalLen;
        server->currOp.header.remainLen = parms->u.headerRx.remainLen;
        server->currOp.header.len = parms->u.headerRx.currLen;
        server->currOp.header.buffer = parms->u.headerRx.buff;
    }
    NotifyCurrServer(parms->server, GOEP_EVENT_HEADER_RX);

#ifndef __GOEP_USE_ARRAY_HEADER_BUFFER__
    if (OBEXH_IsUnicode(server->currOp.header.type))
    {
        if (server->currOp.header.buffer != NULL)
            free_ctrl_buffer(server->currOp.header.buffer);
        server->currOp.header.buffer = NULL;
    }
#endif
    /* Clear out the header information in preparation for a new header */
    OS_MemSet((U8 *)&server->currOp.header, 0, sizeof(server->currOp.header));
}

/*---------------------------------------------------------------------------
 *            AssociateServer
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Associate a specific profile server based on the target header
 *            for this particular Obex Server.
 *
 * Return:    void
 *
 */
static void AssociateServer(ObexServerApp *serverApp)
{
#if OBEX_SERVER_CONS_SIZE > 0
    U8 i,j;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    GoepServerObexCons *server = (GoepServerObexCons*)serverApp;

    ASSERT(server->connId < GOEP_NUM_OBEX_SERVER_CONS);

    /* Try to associate the operation with a server */
#if OBEX_SERVER_CONS_SIZE > 0
        for (i = 0; i < GOEP_MAX_PROFILES; i++) {
            if (server->profiles[i]) {
                ASSERT(server->profiles[i]->numTargets <= OBEX_SERVER_CONS_SIZE);
                for (j = 0; j < server->profiles[i]->numTargets; j++) {
                    if (server->profiles[i]->target[j] == OBEX_ServerGetConnInfo(serverApp)) {
                        /* We have found a matching target header for the active connection
                         * ID of the current operation.
                         */
                        server->currOp.handler = server->profiles[i];
                        return;
                    }
                }
                if (!server->profiles[i]->numTargets && !OBEX_ServerGetConnInfo(serverApp)) {
                    /* We do not have active connection ID for this operation and this 
                     * profile has no target headers; therefore, set this profile as our handler.
                     */
                    server->currOp.handler = server->profiles[i];
                    return;
                }
            }
        }
        /* If current operation has no connection id => assign it to the first registered server */
        if (server->currOp.handler == 0)
        {
            for (i = 0; i < GOEP_MAX_PROFILES; i++)
            {
                if (server->profiles[i] != NULL)
                {
                    server->currOp.handler = server->profiles[i];
                    return;
                }
            }
            server->currOp.handler = NULL;
        }
#else /* OBEX_SERVER_CONS_SIZE > 0 */
    /* Assign Object Push Server */
    server->currOp.handler = server->profiles[GOEP_PROFILE_OPUSH];
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
}

/*---------------------------------------------------------------------------
 *            StartServerOperation
 *---------------------------------------------------------------------------
 *
 */
static void StartServerOperation(ObexServerApp *serverApp, GoepOperation Op)
{
    GoepServerObexCons *server = (GoepServerObexCons*)serverApp;

    OS_MemSet((U8 *)&server->currOp, 0, sizeof(server->currOp));

    /* Set event to NONE so we can track when the START event is delivered. */
    if (Op == GOEP_OPER_ABORT) {
        /* We will generate the GOEP_EVENT_START automatically when 
         * OBSE_ABORT_START is received, so there is no need to set 
         * GOEP_EVENT_NONE to generate it again.
         */
        server->currOp.event = GOEP_EVENT_START;
    } else server->currOp.event = GOEP_EVENT_NONE;
    server->currOp.oper = Op;

    /* Clear authentication flags */
#if OBEX_AUTHENTICATION == XA_ENABLED
    server->flags &= ~(GOEF_CHALLENGE|GOEF_RESPONSE);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
}


/*---------------------------------------------------------------------------
 *            NotifyAllServers
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Notify all profile servers for this Obex Server of the
 *            GOEP event.
 *
 * Return:    void
 *
 */
static void NotifyAllServers(ObexServerApp *serverApp, GoepEventType Event)
{
    U8  i;
    GoepServerObexCons *server = (GoepServerObexCons*)serverApp;
    GoepServerApp *Service = (GoepServerApp *)goep_find_server_with_goep_conn_id(serverApp->goep_conn_id);
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len = 0;

//    ASSERT(Service != NULL)


    ASSERT(server->connId < GOEP_NUM_OBEX_SERVER_CONS);

    server->currOp.event = Event; 
    for (i = 0; i < GOEP_MAX_PROFILES; i++) {
        if (server->profiles[i]) {

			server->currOp.handler = server->profiles[i];
			
            if(Service != NULL)
            {
	            if(Service->callback == NULL)
	            {
                    if(Service->target[0])
                    {
                        uuid_len = Service->target[0]->targetLen;
                        btmtk_os_memcpy(uuid, Service->target[0]->target, uuid_len);
                    }
                    else
                    {
                        uuid_len = 0;
                        uuid[0] = 0;
                    }

	                if(Event == GOEP_EVENT_TP_DISCONNECTED)
                    {
                        goep_send_disconnect_ind_msg(0xFF, serverApp->goep_conn_id, uuid, uuid_len, 0); //TODO reason
	                }
                    else if(Event == GOEP_EVENT_TP_CONNECTED)
                    {
                    /* Change server/clinet state to connected */
                    }
                }
                else
                {
                    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_APP_CALLBACK, server->currOp.event);                
                    Service->callback(&server->currOp);
                }
            }
            else /* callback */
            {
                kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_APP_CALLBACK, server->currOp.event);
                server->profiles[i]->callback(&server->currOp);
            }
        }
    }
}

/*---------------------------------------------------------------------------
 *            NotifyCurrServer
 *---------------------------------------------------------------------------
 *   Only Notify the current OPP / FTP 
 */
static void NotifyCurrServer(ObexServerApp *serverApp, GoepEventType Event)
{
    ObStatus    status;
    GoepServerObexCons *server = (GoepServerObexCons*)serverApp;
    GoepServerApp *Service = (GoepServerApp *)goep_find_server_with_goep_conn_id(serverApp->goep_conn_id);
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len = 0;

//    ASSERT(Service != NULL);
	if (Service == NULL)
	{
            GOEP_Report("[GOEP][ERR] Service is Null !");
            ASSERT(Service != NULL);
		return;
	}

    if (server->currOp.oper == GOEP_OPER_PUSH || 
        server->currOp.oper == GOEP_OPER_PULL) {
        /* Indicate the status of the final bit in the current OBEX packet */
        server->currOp.info.pushpull.finalBit = 
            (BOOL)((serverApp->handle.parser.opcode & 0x80) ? TRUE : FALSE);
    }

    /* If this is the first event for this operation, find the correct
     * server and make sure we always indicate a START event first.
     */
    if (server->currOp.event == GOEP_EVENT_NONE) {
        ASSERT(server->currOp.handler == 0);
        AssociateServer(serverApp);
        /* Be sure to indicate a start event before any other event. */
        if ( ((Service != NULL) && (Service->callback != NULL)) || (Service == NULL))
        {
            if (Event != GOEP_EVENT_START) 
            {
                if (server->currOp.handler) 
                {
                    server->currOp.event = GOEP_EVENT_START;
                    kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_APP_CALLBACK, server->currOp.event);                
                    server->currOp.handler->callback(&server->currOp);
                }
            }
        }
    }

    server->currOp.event = Event;

    /* If we found a server, indicate the event now. */
    if (server->currOp.handler) 
    {
        /* if app has callback, then call it. */
        if (server->currOp.handler->callback != NULL)
        {
            kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_APP_CALLBACK, server->currOp.event);                
            server->currOp.handler->callback(&server->currOp);

            return;
        }
 
        /* no callback, send msg. */
		
	    if(Service != 0 )
	    {
		    if(Service->target[0])
		    {
		        uuid_len = Service->target[0]->targetLen;
		        btmtk_os_memcpy(uuid, Service->target[0]->target, uuid_len);
		    }
		    else
		    {
		        uuid[0]=0;
		        uuid_len =0;
		    }
	    }
		
        if(server->currOp.event == GOEP_EVENT_CONTINUE)
        {
            if(server->currOp.oper == GOEP_OPER_PUSH)
            {
                if(serverApp->goep_first_packet_received == KAL_FALSE)
                {
                    if(serverApp->server.resp == (OBRC_SUCCESS | OB_FINAL_BIT))
                    goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_SINGLE_PKT, GOEP_PUT_NORMAL, 
                                            server->currOp.info.pushpull.objectLen, 
                                            (kal_uint8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                            (kal_uint8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                            serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                            uuid, uuid_len);
                    else
                    {
                        goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FIRST_PKT, GOEP_PUT_NORMAL, 
                                                server->currOp.info.pushpull.objectLen, 
                                                (kal_uint8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                                (kal_uint8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                                serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                                uuid, uuid_len);
                        serverApp->goep_first_packet_received = KAL_TRUE;
                        GOEP_Report("[OBEX] clearn first_packet_received:%d", serverApp->goep_first_packet_received);                            
                         
                    }
                }
                else
                {
                    if(serverApp->server.resp == (OBRC_SUCCESS | OB_FINAL_BIT))
                    {
                        goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FINAL_PKT, 
                                                GOEP_PUT_NORMAL, 0, NULL, 0, NULL, 0, 
                                                serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                                uuid, uuid_len);
                        serverApp->goep_first_packet_received = KAL_FALSE;
                        GOEP_Report("[OBEX] clearn first_packet_received:%d", serverApp->goep_first_packet_received);                            
                        
                    }
                    else
                        goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_NORMAL_PKT, GOEP_PUT_NORMAL,
                                            0, NULL, 0, NULL, 0, 
                                            serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                            uuid, uuid_len);
                }
            }
            else if(server->currOp.oper == GOEP_OPER_PULL)
            {
                goep_send_pull_ind_msg(serverApp->goep_conn_id, GOEP_NORMAL_PKT, server->currOp.info.pushpull.type, (kal_uint8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen, uuid, uuid_len);
            }
            else if(server->currOp.oper == GOEP_OPER_CONNECT)
            {
#if GOEP_AUTH == XA_ENABLED
                int index;
                if (!(server->flags & GOEF_RESPONSE)  
					&& goep_need_obex_auth_challenge(uuid, uuid_len, &index))
                {
		            Service->challenge = goep_build_auth_challenge_data(index);
		            Service->password = get_ctrl_buffer(GOEP_MAX_PASSWD_SIZE);
                    btmtk_os_memcpy(Service->password, auth_chal_profiles[index].passwd, GOEP_MAX_PASSWD_SIZE);		       
		            Service->passwordLen = auth_chal_profiles[index].passwd_len;
   
		            GOEP_ServerAuthenticate(Service, NULL, Service->challenge);
			   
			        free_ctrl_buffer(Service->challenge);
			        Service->challenge = NULL;
                
	                GOEP_ServerAbort(Service, OBRC_UNAUTHORIZED);
	                GOEP_ServerContinue(Service);
                }
			    else
				{
				    //send CONNECT_IND and AUTH_IND msg 
				    goep_send_connect_ind_msg(serverApp->goep_conn_id, (kal_uint16)(serverApp->handle.parser.maxTxPacket-6), uuid, uuid_len);
				 
			        Service->bAuthChallenged = FALSE;
				    if(server->flags & GOEF_CHALLENGE)
				    {
				        Service->bAuthChallenged = TRUE;
                        /* send auth_ind msg in connect_res_hdlr */
				    }				   	
				}
#else            
                if(server->flags & GOEF_CHALLENGE)
                    goep_send_auth_ind_msg(serverApp->goep_conn_id, server->currOp.challenge.realm, server->currOp.challenge.realmLen, uuid, uuid_len, server->currOp.challenge.options);
                else
                    goep_send_connect_ind_msg(serverApp->goep_conn_id, (kal_uint16)(serverApp->handle.parser.maxTxPacket-6), uuid, uuid_len, 0, 0);
#endif				
            }
            else if(server->currOp.oper == GOEP_OPER_SETFOLDER)
            {
                if(server->currOp.info.setfolder.reset == KAL_FALSE)
                {
                    if(server->currOp.info.setfolder.flags & OSPF_BACKUP)
                    {                        
                        goep_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_BACK_FOLDER, NULL, 0, uuid, uuid_len);
                    }
                    else if(server->currOp.info.setfolder.nameLen)
                    {
                        if(server->currOp.info.setfolder.flags & OSPF_DONT_CREATE)
                            goep_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_FORWARD_FOLDER, (kal_uint8*)server->currOp.info.setfolder.name, server->currOp.info.setfolder.nameLen, uuid, uuid_len);
                        else
                            goep_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_CREATE_FOLDER, (kal_uint8*)server->currOp.info.setfolder.name, server->currOp.info.setfolder.nameLen, uuid, uuid_len);
                    }
                    else
                    {
                        /* Error Handling */
                        goto CANCEL_OPER;
                    }
                }
                else
                {
                    goep_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_ROOT_FOLDER, NULL, 0, uuid, uuid_len);
                }
            }
            else if(server->currOp.oper == GOEP_OPER_ACTION)
            {
                goep_send_action_ind_msg(serverApp->goep_conn_id, server->currOp.info.action.action_id, server->currOp.info.action.permission, (U8*)server->currOp.info.action.name, server->currOp.info.action.nameLen, (U8*)server->currOp.info.action.dstname, server->currOp.info.action.dstnameLen, uuid, uuid_len);
            }
            else if(server->currOp.oper == GOEP_OPER_DELETE)
            {
                /* server->currOp.oper == GOEP_OPER_DELETE */
                if(server->currOp.info.pushpull.objectLen == 0)
                    goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FIRST_PKT, 
                                            GOEP_PUT_DELETE, server->currOp.info.pushpull.objectLen, (kal_uint8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                            (kal_uint8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                            NULL, 0,
                                            uuid, uuid_len);
                else
                {
                     goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FINAL_PKT, 
                                            GOEP_PUT_NORMAL, 0, NULL, 0, NULL, 0,
                                            serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                            uuid, uuid_len);
                     serverApp->goep_first_packet_received = KAL_FALSE;
                     OS_Report("[OBEX] clearn first_packet_received:%d", serverApp->goep_first_packet_received);                            
                     
                }
            }
            else if(server->currOp.oper == GOEP_OPER_CREATEEMPTY)
			{
                goep_send_push_ind_msg(serverApp->goep_conn_id, GOEP_SINGLE_PKT, 
                                        GOEP_CREATEEMPTY, server->currOp.info.pushpull.objectLen, (kal_uint8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                        (kal_uint8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                        NULL, 0,
                                        uuid, uuid_len);
			}
            else if(server->currOp.oper == GOEP_OPER_DISCONNECT)
            {
                GOEP_ServerContinue(Service);
            }
            else if(server->currOp.oper == GOEP_OPER_ABORT)
            {
                goep_send_abort_ind_msg(serverApp->goep_conn_id, uuid, uuid_len);
            }
            else
            {
                /* Error Handling */
                goto CANCEL_OPER;
            }
        }
        else if(server->currOp.event == GOEP_EVENT_COMPLETE)
        {
            if(server->currOp.oper == GOEP_OPER_PUSH)
            {
                serverApp->goep_buff_wrote_len = 0;
            }
        }
        else if(server->currOp.event == GOEP_EVENT_START)
        {
            if(server->currOp.oper == OBSE_PUT_START)
            {
                serverApp->goep_first_packet_received = KAL_FALSE;
                serverApp->goep_buff_wrote_len = 0;
                GOEP_Report("[OBEX] clearn first_packet_received:%d", serverApp->goep_first_packet_received);                            
                
            }
        }
        else if(server->currOp.event == GOEP_EVENT_AUTH_RESPONSE)
        {
            if(Service && GOEP_ServerVerifyAuthResponse(Service, Service->password, Service->passwordLen))
            {
#if GOEP_AUTH == XA_ENABLED
                //do nothing here. wait SEND_RESPONSE evet to send response 
#else
                GOEP_ServerContinue(Service);
#endif
                goep_send_auth_cnf_msg(serverApp->goep_conn_id, GOEP_STATUS_SUCCESS, uuid, uuid_len);
            }
            else
            {
                GOEP_ServerAbort(Service, OBRC_UNAUTHORIZED);
                GOEP_ServerContinue(Service);
                goep_send_auth_cnf_msg(serverApp->goep_conn_id, GOEP_STATUS_FAILED, uuid, uuid_len);
            }
        }

        return;
    }

    /* We did not find a suitable server, handle operation rejection. */
    if (Event == GOEP_EVENT_CONTINUE) {
CANCEL_OPER:
        status = OBEX_ServerAbort(serverApp, OBRC_SERVICE_UNAVAILABLE);
        ASSERT(status == OB_STATUS_SUCCESS);
        OBEX_ServerSendResponse(&server->obs);
        return;
    }
}
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED

/*---------------------------------------------------------------------------
 *            GoepClntCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX client protocol events.
 *
 * Return:    void
 *
 */
static void GoepClntCallback(ObexClientCallbackParms *parms)
{
    GoepClientObexCons *client = (GoepClientObexCons*)parms->client;

    ASSERT(IsObexLocked());

    kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_CALLBACK,parms->event);
    
    switch (parms->event) {
    case OBCE_SEND_COMMAND:
        ASSERT(client->currOp.event != GOEP_EVENT_NONE);
        NotifyCurrClient(parms->client, GOEP_EVENT_CONTINUE);
        break;

    case OBCE_CONNECTED:
        kal_trace(BT_TRACE_G6_OBEX, GOEP_OBCE_CONNECTED, client);
        /* A Transport Layer Connection has been established. */
        ASSERT((client->flags & GOEF_ACTIVE) == 0);
        client->flags |= GOEF_ACTIVE;
        NotifyAllClients(parms->client, GOEP_EVENT_TP_CONNECTED);
        break;

    case OBCE_DISCONNECT:
        kal_trace(BT_TRACE_G6_OBEX,GOEP_OBCE_DISCONNECT, client);
        client->flags &= ~GOEF_ACTIVE;
        client->flags &= ~GOEF_CLIENT_TP_INITIATED;

        CLEAR_QUEUEDHEADERS(client);

        NotifyAllClients(parms->client, GOEP_EVENT_TP_DISCONNECTED);
        break;
        
    case OBCE_COMPLETE:
        /* The requested operation has completed. */
        client->currOp.reason = OBRC_SUCCESS;
        NotifyCurrClient(parms->client, GOEP_EVENT_COMPLETE);
        break;
        
    case OBCE_ABORTED:
        kal_trace(BT_TRACE_G6_OBEX,GOEP_OBCE_ABORTED, client, parms->u.abortReason);
        /* The requested operation was aborted. */
        client->currOp.reason = parms->u.abortReason;

        CLEAR_QUEUEDHEADERS(client);

        if (client->currOp.reason == OBRC_LINK_DISCONNECT) {
            client->flags &= ~GOEF_ACTIVE;
            client->flags &= ~GOEF_CLIENT_TP_INITIATED;
            NotifyCurrClient(parms->client, GOEP_EVENT_ABORTED);
            kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_TP_DISCONNECTED);
            NotifyAllClients(parms->client, GOEP_EVENT_TP_DISCONNECTED);
        } else {
            NotifyCurrClient(parms->client, GOEP_EVENT_ABORTED);
        }
        break;
        
    case OBCE_NO_SERVICE_FOUND:
        ASSERT((client->flags & GOEF_ACTIVE) == 0);
        client->flags &= ~GOEF_CLIENT_TP_INITIATED;
        NotifyCurrClient(parms->client, GOEP_EVENT_NO_SERVICE_FOUND);
        break;

    case OBCE_DISCOVERY_FAILED:
        ASSERT((client->flags & GOEF_ACTIVE) == 0);
        client->flags &= ~GOEF_CLIENT_TP_INITIATED;
        NotifyCurrClient(parms->client, GOEP_EVENT_DISCOVERY_FAILED);
        break;

    case OBCE_HEADER_RX:
        GoepClientProcessHeaders(parms);
        break;

    case OBCE_SCO_REJECT:
        NotifyCurrClient(parms->client, GOEP_EVENT_SCO_REJECT);
        break;

    case OBCE_BTCHIP_REJECT:
        NotifyCurrClient(parms->client, GOEP_EVENT_BTCHIP_REJECT);
        break;

    default:
        ASSERT(0);
        break;
    }
}

/*---------------------------------------------------------------------------
 *            GoepClientProcessHeaders
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process the incoming headers based on the current operation.
 *
 * Return:    void
 *
 */
void GoepClientProcessHeaders(ObexClientCallbackParms *parms)
{
    GoepClientObexCons *client = (GoepClientObexCons*)parms->client;

    switch (client->currOp.oper) {

    case GOEP_OPER_CONNECT:
#if GOEP_MAX_WHO_LEN > 0
        if (parms->u.headerRx.type == OBEXH_WHO) {
            DoHeaderCopy(client->currOp.info.connect.who, 
                &client->currOp.info.connect.whoLen, GOEP_MAX_WHO_LEN, 
                parms->u.headerRx.buff, parms->u.headerRx.currLen);
            return;
        }
#endif /* GOEP_MAX_WHO_LEN > 0 */
        if (parms->u.headerRx.type == OBEXH_CONNID) {
            client->currOp.handler->obexConnId = parms->u.headerRx.value;
            client->obc.client.obConnId = parms->u.headerRx.value;
            return;
        }
        break;
       
    case GOEP_OPER_PULL:
        if (parms->u.headerRx.type == OBEXH_LENGTH) {
            /* Set the incoming object length so the progress meter works. */
            client->currOp.info.pull.objectLen = parms->u.headerRx.value;
        }
        break;
    }

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* We accept authentication headers with any operation */
    switch (parms->u.headerRx.type) {
    case OBEXH_AUTH_CHAL:
        if (OBEXH_ParseAuthChallenge(&parms->client->handle, 
                &client->currOp.challenge)) {
            /* Full auth challenge has been received, indicate it. */
            client->flags |= GOEF_CHALLENGE;
            NotifyCurrClient(parms->client, GOEP_EVENT_AUTH_CHALLENGE);
        }
        return;

    case OBEXH_AUTH_RESP:
        if (OBEXH_ParseAuthResponse(&parms->client->handle, 
                &client->currOp.response)) {
            /* Full auth response has been received, indicate it. */
            client->flags |= GOEF_RESPONSE;
            NotifyCurrClient(parms->client, GOEP_EVENT_AUTH_RESPONSE);
        }
        return;
    }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

    if (parms->u.headerRx.type == OBEXH_TARGET ||
        parms->u.headerRx.type == OBEXH_CONNID) {
        /* We do not pass up Target or ConnId headers, in order 
         * for Directed connections and operations to setup properly.
         */
        return;
    }

    /* If we have not processed the OBEX header already we will accept it now, regardless
     * the current operation. 
     */
    client->currOp.header.type = parms->u.headerRx.type;
    if (OBEXH_Is1Byte(client->currOp.header.type) || 
        OBEXH_Is4Byte(client->currOp.header.type)) {
        /* 1-byte or 4-byte */
        client->currOp.header.value = parms->u.headerRx.value;
    } else if (OBEXH_IsUnicode(client->currOp.header.type)) {
        /* Unicode */
#ifdef __GOEP_USE_ARRAY_HEADER_BUFFER__
        DoUniHeaderCopy(client->currOp.header.unicodeBuffer, &client->currOp.header.len,
            GOEP_MAX_UNICODE_LEN, parms->u.headerRx.buff, parms->u.headerRx.currLen);                
        /* Don't indicate the Unicode header until it is completely copied */
        if (parms->u.headerRx.remainLen > 0) 
            return;
        /* Point the header buffer pointer to the unicode buffer */
        client->currOp.header.buffer = (U8 *)client->currOp.header.unicodeBuffer;
#else
        if (client->currOp.header.buffer == NULL)
        {
            client->currOp.header.buffer = get_ctrl_buffer(GOEP_MAX_UNICODE_LEN * sizeof(GoepUniType));
        }
        DoUniHeaderCopy((GoepUniType*)client->currOp.header.buffer, &client->currOp.header.len, 
            GOEP_MAX_UNICODE_LEN, parms->u.headerRx.buff, parms->u.headerRx.currLen);                
        /* Don't indicate the Unicode header until it is completely copied */
        if (parms->u.headerRx.remainLen > 0) 
            return;
        
#endif
    } else {
        /* Byte Sequence */
        ASSERT(OBEXH_IsByteSeq(client->currOp.header.type));
        client->currOp.header.totalLen = parms->u.headerRx.totalLen;
        client->currOp.header.remainLen = parms->u.headerRx.remainLen;
        client->currOp.header.len = parms->u.headerRx.currLen;
        client->currOp.header.buffer = parms->u.headerRx.buff;
    }
    NotifyCurrClient(parms->client, GOEP_EVENT_HEADER_RX);

#ifndef __GOEP_USE_ARRAY_HEADER_BUFFER__
    if (OBEXH_IsUnicode(client->currOp.header.type))
    {
        if (client->currOp.header.buffer != NULL)
            free_ctrl_buffer(client->currOp.header.buffer);
        client->currOp.header.buffer = NULL;
    }
#endif
    /* Clear out the header information in preparation for a new header */
    OS_MemSet((U8 *)&client->currOp.header, 0, sizeof(client->currOp.header));
}

/*---------------------------------------------------------------------------
 *            NotifyAllClients
 *---------------------------------------------------------------------------
 *
 */
static void NotifyAllClients(ObexClientApp *clientApp, GoepEventType Event)
{
    GoepClientEvent     tempOp;
    U8                  i;
    GoepClientObexCons  *client = (GoepClientObexCons*)clientApp;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    kal_uint8 reason;

    reason = client->obc.handle.parser.rsp;    

    ASSERT(client->connId < GOEP_NUM_OBEX_CLIENT_CONS);

    ASSERT((Event == GOEP_EVENT_TP_CONNECTED) ||
           (Event == GOEP_EVENT_TP_DISCONNECTED));

    client->currOp.event = Event;
    client->currOp.handler = 0;
    client->currOp.oper = GOEP_OPER_NONE;

    tempOp = client->currOp;

    for (i = 0; i < GOEP_MAX_PROFILES; i++) {
        if (client->profiles[i]) {
            client->profiles[i]->obexConnId = OBEX_INVALID_CONNID;
            client->obc.client.obConnId = OBEX_INVALID_CONNID;
            tempOp.handler = client->profiles[i];

            kal_trace(BT_TRACE_G6_OBEX,GOEP_NOTIFY_ALLCLIENT, i, client->profiles[i]->connState);
            
            if (Event == GOEP_EVENT_TP_DISCONNECTED)
            {

                if(client->profiles[i]->callback)
                {
                    client->profiles[i]->callback(&tempOp);
                    /* removed by will */
                    /* BPP will reset the cntx while callbacking for GOEP_EVENT_TP_DISCONNECTED EVENT */
                    //client->profiles[i]->connState = CS_DISCONNECTED;
                }
                else
                {
                    /* send msg to MMI */
                    if(client->profiles[i]->connect_req.target)
                    {
                        uuid_len = client->profiles[i]->connect_req.targetLen;
                        btmtk_os_memcpy(uuid, client->profiles[i]->connect_req.target, uuid_len);
                    }
                    else
                    {
                        uuid_len = 0;
                        uuid[0] = 0;
                    }                  
                
                    /* Deliver disconnect event */
                    if(client->profiles[i]->connState == CS_CONNECTING)
                    {
                        goep_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, uuid, uuid_len);
                    }
                    else if(client->profiles[i]->connState == CS_DISCONNECTING)
                    {
                        goep_send_disconnect_ind_msg(clientApp->req_id, 0xFF, uuid, uuid_len, 0); //TODO reason
                    }
                    else if(client->profiles[i]->connState == CS_CONNECTED)
                    {
                        if(client->profiles[i]->obex_conn_is_connected)
                        {
                            goep_send_disconnect_ind_msg(clientApp->req_id, clientApp->goep_conn_id, uuid, uuid_len, 0); //TODO reason
                        }
                        else
                        {
                            goep_send_disconnect_ind_msg(clientApp->req_id, 0xFF, uuid, uuid_len, 0); //TODO reason
                        }
                    }
                }
            }
            else if ((Event == GOEP_EVENT_TP_CONNECTED) && 
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
                     ((client->profiles[i]->connState == CS_CONNECTING)
                     || (client->profiles[i]->connState == CS_DISCONNECTED))) {
#else /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
                     (client->profiles[i]->connState == CS_CONNECTING)) {
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
                /* Deliver connect event */
                client->profiles[i]->connState = CS_CONNECTED;
                if(client->profiles[i]->callback)
                {
                    client->profiles[i]->callback(&tempOp);
                }
                else
                {
                    if(clientApp->goep_auth_use)
                    {
                        goep_send_connect_cnf_msg(clientApp->req_id, clientApp->goep_conn_id, OB_STATUS_PENDING, 0, client->profiles[i]->connect_req.target, client->profiles[i]->connect_req.targetLen);
                        kal_trace(BT_TRACE_G6_OBEX,GOEP_WAITFOR_AUTH_REQ);
                    }
                    else
                    {
                        GOEP_Connect(client->profiles[i], &(client->profiles[i]->connect_req));
                    }
                }
            }
        }
    }
}
/*---------------------------------------------------------------------------
 *            NotifyCurrClient
 *---------------------------------------------------------------------------
 *
 */
static void NotifyCurrClient(ObexClientApp *clientApp, GoepEventType Event)
{
    GoepClientEvent  tempOp;
    GoepClientObexCons  *client = (GoepClientObexCons*)clientApp;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;
    U8 reason;

    ASSERT(client->currOp.handler);

    reason = client->obc.handle.parser.rsp;
    if(Event == GOEP_EVENT_ABORTED)
    {
        if (client->currOp.reason == OBRC_LINK_DISCONNECT)
        {
            reason = GOEP_STATUS_DISCONNECT;
        }
    }
    
    client->currOp.event = Event;
    client->currOp.failed_rsp_code = reason;
    tempOp = client->currOp;

    if (Event >= GOEP_EVENT_COMPLETE) {
        ASSERT((Event == GOEP_EVENT_TP_CONNECTED) || 
               (Event == GOEP_EVENT_TP_DISCONNECTED) || 
               (Event == GOEP_EVENT_ABORTED) || 
               (Event == GOEP_EVENT_DISCOVERY_FAILED) ||
               (Event == GOEP_EVENT_NO_SERVICE_FOUND) ||
               (Event == GOEP_EVENT_SCO_REJECT) ||
               (Event == GOEP_EVENT_COMPLETE) ||
               (Event == GOEP_EVENT_BTCHIP_REJECT));
        if ((Event == GOEP_EVENT_DISCOVERY_FAILED) || 
            (Event == GOEP_EVENT_NO_SERVICE_FOUND) ||
            (Event == GOEP_EVENT_SCO_REJECT)||
            (Event == GOEP_EVENT_BTCHIP_REJECT)) {
            /* Reset the connection state for this profile */
            client->currOp.handler->connState = CS_DISCONNECTED;
        }
        /* Were done, remove the handler before indicating the event. */
        client->currOp.handler = 0;
    }

    /* Handle SRM */
#if 0    
    if( GOEP_EVENT_CONTINUE == Event && ( 0 != client->obc.client.srm_client) ){
            GOEP_Report("[OBEX] Event:%d srm-client:%d", Event, client->obc.client.srm_client);
    }
#endif

    /* Indicate the event */
    tempOp.event = Event;
    if(tempOp.handler->callback)
    {
        kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_CALL_TEMPOP_HANDLER_CALLBACK, Event);    
        tempOp.handler->callback(&tempOp);
        return;
    }
	
    goep_find_obex_uuid_with_bt_uuid(&(client->obc.trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
	
    if(Event == GOEP_EVENT_ABORTED)
    {
        kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_ABORTED, reason, client->currOp.oper, clientApp->client.status);    
        
        if(client->currOp.oper == GOEP_OPER_CONNECT)
        {
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
            /* if authentication response has be sent, and the verification fails, then send CONNECT_RSP with fail */
            if(goep_client && goep_client->goep_auth_resp_sent) // remote doesn't have response
            {
#if GOEP_AUTH == XA_ENABLED
                goep_client->goep_auth_verify_fail = TRUE;
#endif				
                goep_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, uuid, uuid_len);				

                if (client->currOp.reason != OBRC_LINK_DISCONNECT)
                {
 		        GOEP_TpDisconnect(goep_client);
                }
            }
            else
             /* this abort shall be ignored if the server challenge us */
            if ((client->flags & GOEF_CHALLENGE) == 0 && (client->currOp.reason != OBRC_LINK_DISCONNECT))
            {
                /* in case of no challenge and the abort reason is not TPDISCONNECTED */
                GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
                /* Disconnect RFCOMM */
                GOEP_TpDisconnect(goep_client);
            }
        }
        else if((client->currOp.oper == GOEP_OPER_PUSH)||(client->currOp.oper == GOEP_OPER_PULL))
        {
            if(clientApp->client.status == OBRC_USER_ABORT)
            {
                goep_send_abort_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, uuid, uuid_len);
            }
            else if(client->currOp.oper == GOEP_OPER_PUSH)
            {
                goep_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
            }
            else if(client->currOp.oper == GOEP_OPER_PULL)
            {
                goep_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, 0,
                                        NULL, 0, NULL, 0, uuid, uuid_len);
            }
        }
        else if(client->currOp.oper == GOEP_OPER_SETFOLDER)
        {
            goep_send_set_folder_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_ACTION)
        {
            goep_send_action_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
        }        
        else if(client->currOp.oper == GOEP_OPER_DELETE)
        {
            goep_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_DISCONNECT)
        {
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
            GOEP_TpDisconnect(goep_client);
        }
        else
        {
            /*error handling */
            GOEP_Report("[GOEP][WRN] aborted no handle oper:%d", client->currOp.oper);
        }
    }
    else if(Event == GOEP_EVENT_CONTINUE)
    {
        if(client->currOp.oper == GOEP_OPER_PUSH)
        {
            goep_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_PULL)
        {
            if(clientApp->goep_first_packet_received == KAL_FALSE)
            {
                goep_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_FIRST_PKT, 
                                        NULL, client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                        client->obc.goep_buff_wrote_len, uuid, uuid_len);
                clientApp->goep_first_packet_received = KAL_TRUE;
                GOEP_Report("[OBEX] clearn first_packet_received:%d", clientApp->goep_first_packet_received);                            
                
            }
            else
            {
                goep_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_NORMAL_PKT, 
                                        NULL, client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                        client->obc.goep_buff_wrote_len, uuid, uuid_len);
            }
        }
        else
        {
            /*error handling */
        }
    }
    else if(Event == GOEP_EVENT_COMPLETE)
    {
        if(client->currOp.oper == GOEP_OPER_CONNECT)
        {
#if GOEP_AUTH == XA_ENABLED
        GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);

        if ( TRUE == goep_client->goep_auth_chal_server && TRUE == goep_client->goep_auth_verify_fail)
        {   
            goep_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, uuid, uuid_len);

		    GOEP_TpDisconnect(goep_client);
        }
	    else
#endif			
#ifdef __BT_GOEP_V2__
        {
            ObexTpConnInfo tpInfo;
            OBEX_GetTpConnInfo(&clientApp->handle, &tpInfo);
            GOEP_Report("[OBEX] Connect transport maxPduSize is %d", tpInfo.maxPduSize);
        }
#endif
            goep_send_connect_cnf_msg(clientApp->req_id, clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, (U16)(clientApp->handle.parser.maxTxPacket - ( 3 + OBEXH_HI_BODY_SIZE)), uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_SETFOLDER)
        {
            goep_send_set_folder_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_ACTION)
        {
            goep_send_action_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }        
        else if((client->currOp.oper == GOEP_OPER_PUSH)||(client->currOp.oper == GOEP_OPER_DELETE))
        {
            goep_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_PULL)
        {
            if(clientApp->goep_first_packet_received == KAL_FALSE)
                goep_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_SINGLE_PKT, NULL, 
                                       client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                       client->obc.goep_buff_wrote_len, uuid, uuid_len);
            else
                goep_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_FINAL_PKT, NULL, 
                                       client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                       client->obc.goep_buff_wrote_len, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_DISCONNECT)
        {
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
            GOEP_TpDisconnect(goep_client);
        }
        else
        {
            /*error handling */
        }
    }
    else if((Event == GOEP_EVENT_DISCOVERY_FAILED) || 
            (Event == GOEP_EVENT_NO_SERVICE_FOUND))
    {
        goep_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, uuid, uuid_len);
    }
    else if (Event == GOEP_EVENT_SCO_REJECT)
    {
        goep_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_SCO_REJECT, 0, uuid, uuid_len);
    }
    else if (Event == GOEP_EVENT_BTCHIP_REJECT)
    {
        goep_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_BUSY, 0, uuid, uuid_len);
    }
    else if(Event == GOEP_EVENT_AUTH_CHALLENGE)
    {
        /* Received authentication challenge from the server */
        goep_send_auth_ind_msg(clientApp->goep_conn_id, client->currOp.challenge.realm, client->currOp.challenge.realmLen, uuid, uuid_len, client->currOp.challenge.options);
        kal_trace(BT_TRACE_G6_OBEX,GOEP_RECV_AUTH_CHAL);
    }
    else if(Event == GOEP_EVENT_AUTH_RESPONSE)
    {
        GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
		if (goep_client == NULL)
		{
                        GOEP_Report("[GOEP][ERR] no client !");
			return;
		}
		
        if(GOEP_ClientVerifyAuthResponse(goep_client, goep_client->connect_req.password, goep_client->connect_req.passwordLen))
        {
#if GOEP_AUTH == XA_ENABLED
            if (goep_client->goep_auth_chal_server)
            {
                //don't send connect_rsp here! send it in case of EVENT_COMPLETE and oper==CONNECT
                //goep_send_connect_cnf_msg(clientApp->req_id, clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, uuid, uuid_len);
		        goep_client->goep_auth_chal_server = FALSE;		
			 goep_client->goep_auth_verify_fail = FALSE;
            }
	     else
#endif
            goep_send_auth_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }
        else
        {
#if GOEP_AUTH == XA_ENABLED
            if (goep_client->goep_auth_chal_server)
            {
			 goep_client->goep_auth_verify_fail = TRUE;
		        goep_client->goep_auth_chal_server = FALSE;		
            }
	     else
#endif
            goep_send_auth_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, uuid, uuid_len);
        }
        kal_trace(BT_TRACE_G6_OBEX,GOEP_RECV_AUTH_RESP);
    }
}

/*---------------------------------------------------------------------------
 *            Strlen
 *---------------------------------------------------------------------------
 *
 */
static U16 Strlen(const char *String)
{
   const char  *cp = String;

   ASSERT(String);

   while (*cp != 0) cp++;

   return (U16)(cp - String);
}

#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
/*---------------------------------------------------------------------------
 *            Unilen
 *---------------------------------------------------------------------------
 *
 */
static U16 Unilen(const U16 *Unistr)
{
   const U16  *cp = Unistr;

   ASSERT(Unistr);

   while (*cp != 0) cp++;

   return (U16)((U8*)cp - (U8*)Unistr);
}
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            AsciiToUnicode
 *---------------------------------------------------------------------------
 *
 */
static U16 AsciiToUnicode(U16 *Unistr, const char *String)
{
   const char  *src = String;
   char        *dst = (char *)Unistr;

   while (*src != 0) {
        *dst++ = 0;
        *dst++ = *src++;
   }

   /* Add null terminator but don't advance ptr */
   *(dst+0) = 0;
   *(dst+1) = 0;

   return (dst - (char *)Unistr);
}
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */

#define LEN_UNICODE_ODD_FLAG    0x8000
#define LEN_UNICODE_MASK        0x7FFF
/*---------------------------------------------------------------------------
 *            DoUniHeaderCopy
 *---------------------------------------------------------------------------
 *
 */
static void DoUniHeaderCopy(GoepUniType *Dst, U16 *Len, U16 MaxLen, 
                            U8 *Src, U16 SrcLen)
{
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U8  *src;
    U16  toCopy;

    src = Src;
    toCopy = SrcLen;

    /* If we have run out of space to write data, return */
    if ((*Len & LEN_UNICODE_MASK) == MaxLen) {
        *Len &= LEN_UNICODE_MASK;
        return;
    }
    
    /* Advance destination ptr past written data. */
    Dst += (*Len & LEN_UNICODE_MASK);

    /* Convert to ASCII as we copy */
    while (toCopy) {
        if (*Len & LEN_UNICODE_ODD_FLAG) {    /* Unicode Odd */
            /* This is the byte to store */
            *Dst++ = *src;
            *Len &= ~LEN_UNICODE_ODD_FLAG;
            (*Len)++;
        } else {
            *Len |= LEN_UNICODE_ODD_FLAG;
        }
        src++;
        toCopy--;

        if ((*Len & LEN_UNICODE_MASK) == MaxLen) {
            *Len &= LEN_UNICODE_MASK;
            break;
        }
    }
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    /* Since we are copying the bytes one at a time, we must make sure
     * that MaxLen reflects a larger type that GoepUniType may have 
     * referenced (i.e. U16).  Doing so will make sure that we can 
     * copy every byte within the GoepUniType buffer provided.
     */
    DoHeaderCopy((U8 *)Dst, Len, (U16)(MaxLen*sizeof(GoepUniType)), Src, SrcLen);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
}

/*---------------------------------------------------------------------------
 *            DoHeaderCopy
 *---------------------------------------------------------------------------
 *
 */
static void DoHeaderCopy(U8 *Dst, U16 *DstLen, U16 MaxLen, U8 *Src, U16 SrcLen)
{
    U16 toCopy;

    /* Advance destination ptr past written data. */
    Dst += *DstLen;

    toCopy = min((MaxLen - *DstLen), SrcLen);
    OS_MemCopy(Dst, Src, toCopy);
    *DstLen += toCopy;
}

/*---------------------------------------------------------------------------
 *            DoUniNameTruncateCopy
 *---------------------------------------------------------------------------
 *
 */
static void DoUniNameTruncateCopy(GoepUniType *Dst_name, U16 *DstLen, U16 MaxLen, 
                            U8 *Src, U16 SrcLen, U16 RemainLen)
{
#define GOEP_POSTFIX_LEN (6 * 2)  //".wbmp\0" 
    U16 dst_max_len = MaxLen * sizeof(GoepUniType);
    U8 *p_postfix = Src + SrcLen - 1;
    U8 postfix_len = 1;
    U16 toCopy;
    U16 space_to_remain = 0;
	U8 *Dst = (U8*)Dst_name;

    /* Advance destination ptr past written data. */
    Dst += *DstLen;
    
    while (*p_postfix != '.' && p_postfix > Src)
    {
        p_postfix--;
        postfix_len++;
    }

    if (*p_postfix == '.')
    {
        p_postfix--; // net UNICODE: "0062"
        postfix_len++;

        if (postfix_len > GOEP_POSTFIX_LEN)
            postfix_len = GOEP_POSTFIX_LEN;

    }
    else
    {
        postfix_len = 0;
    }
    
    if (RemainLen > 0)
    {
       space_to_remain = GOEP_POSTFIX_LEN - postfix_len;
    }

    /* copy truncated filename */
    toCopy = dst_max_len - *DstLen;
	ASSERT(toCopy >= (postfix_len + space_to_remain));
    toCopy -= (postfix_len + space_to_remain);
    
    if (toCopy > 0)
    {
        OS_MemCopy(Dst, Src, toCopy);
    }
    *DstLen += toCopy;

    /* copy postfix */
    if (postfix_len > 0)
    {
        Dst += toCopy;
        OS_MemCopy(Dst, p_postfix, postfix_len);
        *DstLen += postfix_len;
    }    
}

/*---------------------------------------------------------------------------
 *            GetFreeConnId
 *---------------------------------------------------------------------------
 *
 */
static BOOL GetFreeConnId(U8 *id, U8 type)
{
    U8 i;

    if (type == GOEP_SERVER) {
#if OBEX_ROLE_SERVER == XA_ENABLED 
        for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++) {
            if (GOES(servers)[i].connCount == 0) {
                /* Free server */
                *id = i;
                return TRUE;
            }
        }
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
    }
    else if (type == GOEP_CLIENT) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
        for (i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++) {
            if (GOEC(clients)[i].connCount == 0) {
                /* Free client */
                *id = i;
                return TRUE;
            }
        }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
    }

    return FALSE;
}

/**
* @brief Get the privous registered ConnId by profile enum id
* OPP and FTP client only allow one registed application
* If there is an existed registed ConnId, donot register the second one
*/
BOOL GoepGetPreviousRegisterConnIdByProfile(U8 goep_service_type, U8 role_type, U8 *id)
{
    U8 i;
    if (role_type == GOEP_CLIENT) {
        if( goep_service_type == GOEP_PROFILE_FTP || goep_service_type == GOEP_PROFILE_OPUSH){
            OS_Report("GOEP: enter check service_type:%d role_type:%d", goep_service_type, role_type);
#if OBEX_ROLE_CLIENT == XA_ENABLED
            for (i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++) {
                OS_Report("GOEP: enter loop i:%d connCount:%d goep_service_type:%d", i, GOEC(clients)[i].connCount, GOEC(clients)[i].obc.goep_service_type);
                if (GOEC(clients)[i].connCount != 0 && goep_service_type == GOEC(clients)[i].obc.goep_service_type ){
                    /* Free client */
                    *id = i;
                    return TRUE;
                }
            }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
        }else{
            OS_Report("GOEP: skip privous register check service_type:%d", goep_service_type);
        }
    }

    return FALSE;
}

#if GOEP_ADDITIONAL_HEADERS > 0
#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ServerBuildHeaders()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function builds Byte Sequence, UNICODE, 1-byte, and 4-byte 
 *            OBEX headers for transmission by the GOEP Server.  
 *
 * Return:    True if the OBEX Headers were built successfully. False indicates 
 *            that the headers could not be built successfully.
 */
static BOOL ServerBuildHeaders(GoepServerObexCons *server) {
    U8              i;
    U32             value;
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U16             len;
    U16             uniName[GOEP_MAX_UNICODE_LEN];
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    BOOL            status = TRUE;

    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) {
        if (server->queuedHeaders[i].type != 0) {
            switch (server->queuedHeaders[i].type & 0xC0) {
            case 0x00:
                /* Unicode */
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
                /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the header
                 * in Unicode form to prevent memory corruption problems.
                 */
                if ((server->queuedHeaders[i].len * 2) > GOEP_MAX_UNICODE_LEN) {
                    status = FALSE;
                    break;
                }
            
                len = AsciiToUnicode(uniName, server->queuedHeaders[i].buffer) + 2;
                if (len > 2)
                    status = OBEXH_BuildUnicode(&server->obs.handle, server->queuedHeaders[i].type, 
                        (char *)uniName, len);
                else status = OBEXH_BuildUnicode(&server->obs.handle, server->queuedHeaders[i].type, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
                if (server->queuedHeaders[i].len > 2)
                    status = OBEXH_BuildUnicode(&server->obs.handle, server->queuedHeaders[i].type, 
                        server->queuedHeaders[i].buffer, server->queuedHeaders[i].len);
                else status = OBEXH_BuildUnicode(&server->obs.handle, server->queuedHeaders[i].type, 0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
                break;

            case 0x40:
                /* Byte Sequence */
                status = OBEXH_BuildByteSeq(&server->obs.handle, server->queuedHeaders[i].type,
                            server->queuedHeaders[i].buffer, server->queuedHeaders[i].len);
                break;

            case 0x80:
                /* 1-byte */
                status = OBEXH_Build1Byte(&server->obs.handle, server->queuedHeaders[i].type,
                            server->queuedHeaders[i].buffer[0]);
                break;

            case 0xC0:
                /* 4-byte */
                value = BEtoHost32(server->queuedHeaders[i].buffer);
                status = OBEXH_Build4Byte(&server->obs.handle, server->queuedHeaders[i].type,
                            value);
                break;
            }

            if (status == FALSE) {
                /* Clear the unsuccessful header */
                OS_MemSet((U8 *)&server->queuedHeaders[i], 0, sizeof(server->queuedHeaders[i]));
                return FALSE;
            }
        }
    }

    if (status == TRUE) {
        /* Clear all of the built headers, since they have been added successfully */
        for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) 
            OS_MemSet((U8 *)&server->queuedHeaders[i], 0, sizeof(server->queuedHeaders[i]));
    }

    return status;
}
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ClientBuildHeaders()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function builds Byte Sequence, UNICODE, 1-byte, and 4-byte 
 *            OBEX headers for transmission by the GOEP Client. Only a GET
 *            request can have BODY headers that span multiple OBEX packets.
 *            This function will queue as much header data as possible in the 
 *            current OBEX packet, with the remainder of the BODY header 
 *            information being sent in subsequent packets.  However, if a 
 *            BODY header exists, it will be the only header sent in the GET
 *            request.  This ensures that any headers sent after the BODY 
 *            header (i.e. End Of Body) will fit into one GET request packet.
 *
 * Return:    True if the OBEX Headers were built successfully. False indicates 
 *            that the headers could not be built successfully.
 */
static BOOL ClientBuildHeaders(GoepClientObexCons *client, U16 *more) {
    U8              i, index;
    U32             value;
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
    U16             len;
    U16             uniName[GOEP_MAX_UNICODE_LEN];
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
    U16             transmitLen = 0;
    BOOL            status = TRUE;

    /* Assume no more header information to send */
    *more = 0;

    for (i = 0; i < GOEP_ADDITIONAL_HEADERS; i++) {
        if (client->queuedHeaders[i].type != 0) {
            //Report(("[GOEP]ClientBuildHeaders... i = %d, type=%d\n", i, client->queuedHeaders[i].type));
            switch (client->queuedHeaders[i].type & 0xC0) {
            case 0x00:
                /* Unicode */
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
                /* Make sure GOEP_MAX_UNICODE_LEN is large enough for the header
                 * in Unicode form to prevent memory corruption problems.
                 */
                if ((client->queuedHeaders[i].len * 2) > GOEP_MAX_UNICODE_LEN) {
                    status = FALSE;
                    break;
                }
            
                len = AsciiToUnicode(uniName, client->queuedHeaders[i].buffer) + 2;
                if (len > 2)
                    status = OBEXH_BuildUnicode(&client->obc.handle, client->queuedHeaders[i].type, 
                        (char *)uniName, len);
                else status = OBEXH_BuildUnicode(&client->obc.handle, client->queuedHeaders[i].type, 0, 0);
#else /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
                if (client->queuedHeaders[i].len > 2)
                    status = OBEXH_BuildUnicode(&client->obc.handle, client->queuedHeaders[i].type, 
                        client->queuedHeaders[i].buffer, client->queuedHeaders[i].len);
                else status = OBEXH_BuildUnicode(&client->obc.handle, client->queuedHeaders[i].type, 0, 0);
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */
                break;

            case 0x40:                
                /* Byte Sequence */
                if ((client->currOp.oper == GOEP_OPER_PULL) && 
                    (client->queuedHeaders[i].type == OBEXH_BODY)) {
                    /* Calculate how much Body header data can fit into an OBEX packet */
                    transmitLen = min(client->queuedHeaders[i].len, 
                                      (OBEXH_GetAvailableTxHeaderLen(&client->obc.handle) - 3));
                } else {
                    /* The entire header must fit in one OBEX packet, if not we should not add the header */
                    transmitLen = client->queuedHeaders[i].len;
                }

                /* Byte Sequence */
                status = OBEXH_BuildByteSeq(&client->obc.handle, client->queuedHeaders[i].type,
                             client->queuedHeaders[i].buffer, transmitLen);

                if (status == TRUE) {
                    if ((client->currOp.oper == GOEP_OPER_PULL) && 
                        (client->queuedHeaders[i].type == OBEXH_BODY)) {
                        if ((client->queuedHeaders[i].len - transmitLen) > 0) {
                            /* Update the length and buffer, since additional data needs 
                             * to be sent in our next packet.
                             */
                            client->queuedHeaders[i].len -= transmitLen;
                            client->queuedHeaders[i].buffer += transmitLen;
                            *more = client->queuedHeaders[i].len;
                            //Report(("[GOEP]ClientBuildHeaders: need next packet to send GET BODY data! \n"));
                            goto Done;
                        }

                        /* Other headers may exist after the BODY header.  However, since we don't know
                         * how much space is available in the packet after the BODY is complete, we'll 
                         * wait to send any remaining headers in our next packet.  All remaining headers
                         * after the BODY must fit into one packet.
                         */
                        *more = client->queuedHeaders[i].len;
                        /* Clear the complete BODY header. */
                        i++;
                        goto Done;
                    }
                }
                break;

            case 0x80:
                /* 1-byte */
                status = OBEXH_Build1Byte(&client->obc.handle, client->queuedHeaders[i].type,
                            client->queuedHeaders[i].buffer[0]);
                break;
                
            case 0xC0:
                /* 4-byte */
                value = BEtoHost32(client->queuedHeaders[i].buffer);
                status = OBEXH_Build4Byte(&client->obc.handle, client->queuedHeaders[i].type, value);
                break;
            }

            if (status == FALSE) {
                /* Clear the failed queued header. */
                //Report(("[GOEP] ClientBuildHeaders, status = false, clear queuedHeader[%d]!\n", i));
                OS_MemSet((U8 *)&client->queuedHeaders[i], 0, sizeof(client->queuedHeaders[i]));
                return FALSE;
            }
        }
    }

Done:
    if (status == TRUE) {
        /* Clear all of the built headers, since they have been added successfully */
        for (index = 0; index < i; index++) 
        {
            OS_MemSet((U8 *)&client->queuedHeaders[index], 0, sizeof(client->queuedHeaders[index]));
        }
    }
    
    return status;
}
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GoepSecCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback for security check.
 */
void GoepSecCallback(const BtEvent *Event)
{
    ObStatus             status;
#if OBEX_ROLE_CLIENT == XA_ENABLED
    GoepClientObexCons  *client = 0;
    GoepClientApp       *clientApp = 0;
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#if OBEX_ROLE_SERVER == XA_ENABLED 
    GoepServerObexCons  *server = 0;
    GoepServerApp       *serverApp = 0;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

    if (Event->p.secToken->incoming == FALSE) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
        /* This is an outgoing connection (GOEP Client) */
        clientApp = (GoepClientApp *)Event->p.secToken->channel;
        client = &GOEC(clients)[clientApp->connId];
        if (Event->eType == BTEVENT_ACCESS_APPROVED) {
            /* Security approved, now send the OBEX Connect operation */
            status = OBEX_ClientConnect(&client->obc);
        } else {
            /* Security failed */
            client->currOp.reason = OBRC_UNAUTHORIZED;
            NotifyCurrClient(&client->obc, GOEP_EVENT_ABORTED);
        }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
    } 
#if OBEX_ROLE_SERVER == XA_ENABLED
    else {
        /* This is an incoming connection (GOEP Server) */
        serverApp = (GoepServerApp *)Event->p.secToken->channel;
        server = &GOES(servers)[serverApp->connId];
        if (Event->eType == BTEVENT_ACCESS_APPROVED) {
            /* Security approved - now pass up the CONTINUE event */
            NotifyCurrServer(&server->obs, GOEP_EVENT_CONTINUE);
        } else {
            /* Security failed - reject the operation, and pass up the CONTINUE event */
            status = OBEX_ServerAbort(&server->obs, OBRC_UNAUTHORIZED);
            ASSERT(status == OB_STATUS_SUCCESS);
            NotifyCurrServer(&server->obs, GOEP_EVENT_CONTINUE);
        }
    }
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
}
#endif /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------
 *
 * OBEX File Transfer UUID for Directed connections
 */
static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };

BtStatus GOEP_ServerRFCommOpenInd(ObexServerApp * serverApp, goep_bd_addr_struct * bd_addr, char * dev_name)
{
    GoepServerApp *Service = (GoepServerApp *)goep_find_server_with_goep_conn_id(serverApp->goep_conn_id);
    ObBtCommonTransport *serverTr;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len = 0;
    RfChannel *Channel;
    BtStatus status = BT_STATUS_PENDING;
    GoepServerObexCons *server;
    GoepServerApp *HandlerRec = NULL;


    if (Service == 0)
	{
	   /* target service is not active */
	    ASSERT(0);
	}

    server = &GOES(servers)[Service->connId];

    serverTr = &(server->obs.trans.ObexServerBtTrans.server);
    Channel = &(serverTr->channel);
    
    if(Service->target[0])
    {
        uuid_len = Service->target[0]->targetLen;
        btmtk_os_memcpy(uuid, Service->target[0]->target, uuid_len);
    }
    else
    {
        uuid[0] = 0;
        uuid_len = 0;
    }

    /* for OPP & FTP, send AUTHORIZE_IND msg */
    if ((uuid_len == 0)
#ifdef __BT_GOEP_V2__
#else
        || (bt_strncmp( (const char*)OBEX_FTP_UUID, (const char*)uuid, 16 ) == 0)
#endif
        )
    {
        /* send authorize ind msg */
        goep_send_authorize_ind_msg( serverApp->goep_conn_id, bd_addr, (kal_uint8 *)dev_name);
    }
    else
    {
        server->currOp.event = GOEP_EVENT_AUTHORIZE_IND;

        /* If we found a server, indicate the event now. */

        /* if app has callback, then call it. */
        if (Service->callback != NULL)
        {
            HandlerRec = server->currOp.handler;
            server->currOp.handler = Service;
            
            kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVER_APP_CALLBACK, server->currOp.event);                
            Service->callback(&server->currOp);

            server->currOp.handler = HandlerRec;
            
            return status;
        }
        else
        {
            kal_trace(BT_TRACE_G6_OBEX, GOEP_RFCOMM_OPENIND_ACCEPT);
            /* accept directly */
            GOEP_ServerAuthorizeResponse(Service, GOEP_STATUS_SUCCESS);
        }
    }
    
    return status;    
}

BtStatus GOEP_ClientSDPSupportedFormatsInd(ObexClientApp * clientApp, kal_uint8 supported_list)
{
    GoepClientApp *Client = NULL;
    BtStatus status = BT_STATUS_SUCCESS;

    if (clientApp)
    {
        Client = (GoepClientApp *)goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
    }
	else
	{
	    kal_trace(BT_TRACE_G6_OBEX, GOEP_SDPIND_CLIENTAPP_ISNULL);
    }

    if (Client && Client->type == GOEP_PROFILE_OPUSH)
    {
        goep_send_opp_supported_formats_ind_msg(clientApp->goep_conn_id, supported_list);
    }

    return status;    
}


void GOEP_ServerAuthorizeResponse(GoepServerApp *Service, kal_uint8 rsp_code)
{
    ObBtServerTransport *ObBtServerTrans;

	kal_trace(BT_TRACE_G6_OBEX, GOEP_SERVER_AUTHORIZE_RES,Service, rsp_code);
    
    if(Service != NULL)
    {
        ObBtServerTrans = &(GOES(servers)[Service->connId].obs.trans.ObexServerBtTrans);
        
        if (rsp_code == GOEP_STATUS_SUCCESS)
        {
            BTSTACK_AcceptRFChnConn(ObBtServerTrans);
        }
        else
        {
            BTSTACK_RejectRFChnConn(ObBtServerTrans);
        }
    }
}

void GOEP_ServerResetPushNameInfo(GoepServerApp *Service)
{
    GoepServerObexCons *server;

    OS_LockStack();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Service) || (Service->connId >= GOEP_NUM_OBEX_SERVER_CONS)) {
        OS_UnlockStack();
        return;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(Service && (Service->connId < GOEP_NUM_OBEX_SERVER_CONS));

    OS_UnlockStack();

	server = &GOES(servers)[Service->connId];
	
	server->currOp.info.pushpull.nameLen = 0;
}


#ifdef __BT_BPP_PROFILE__
ObStatus Goep_init_send_buf(GoepClientApp *Client,kal_char* pbuf, kal_uint32 size)
{
    GoepClientObexCons *client;
    ObexClientApp*      pobex_client = NULL;
    ObClientContext*    pclient_context = NULL;
#if XA_CONTEXT_PTR == XA_ENABLED    
    client = &GoeClient->clients[Client->connId];
#else
    client = &GoeClient.clients[Client->connId];
#endif
    if(client)
    {
        pobex_client = &client->obc;
        pobex_client->goep_buff_ptr = (U8*)pbuf;
        pobex_client->goep_buff_size = (U16) size;
        pclient_context = &pobex_client->client;
        pobex_client->goep_buff_wrote_len = 0;
    }
    
    if(pclient_context)
    {
        if(pbuf && size != 0)
        {
            pclient_context->objLen = size+1;
        }
        else
        {
            pclient_context->objLen = size;
        }
    }
    return XA_STATUS_SUCCESS;
}


kal_uint32  Goep_get_mru(GoepClientApp *Client)
{
    GoepClientObexCons *client;
    ObexClientApp*      pobex_client = NULL;

    GOEP_FLOW("[GOEPFLOW] Goep_get_mru");
        
#if XA_CONTEXT_PTR == XA_ENABLED    
    client = &GoeClient->clients[Client->connId];
#else
    client = &GoeClient.clients[Client->connId];
#endif
    if(client)
    {
        pobex_client = &client->obc;
        return pobex_client->handle.parser.maxTxPacket;
    }
    return 0;
}

#endif

#ifdef __BT_PBAP_PROFILE__
ObStatus GOEP_ServerInitSendBuf(GoepServerApp *Service, kal_char* pbuf, kal_uint32 size)
{
    GoepServerObexCons *server;
    ObexServerApp*      obexServer = NULL;

    server = &GOES(servers)[Service->connId];

    if(server)
    {
        obexServer = &server->obs;
        obexServer->goep_buff_ptr = (U8*)pbuf;
        obexServer->goep_buff_size = (U16) size;
        obexServer->goep_buff_wrote_len = 0;

        if(pbuf && size != 0)
        {
                            obexServer->server.objLen = size + 1;
        }
        else
        {
            obexServer->server.objLen = size;
        }
    }
    
    return XA_STATUS_SUCCESS;
}


kal_uint32  GOEP_ServerGetRemoteMRU(GoepServerApp *Service)
{
    GoepServerObexCons *server;

    server = &GOES(servers)[Service->connId];

    if(server)
    {
        return server->obs.handle.parser.maxTxPacket - 50;
    }
         
    return 0;
}

#endif /* __BT_PBAP_PROFILE__ */


BOOL GOEP_ClientDisableWait(GoepClientApp *Client){
    GoepClientObexCons *client;

    Assert(Client);
    client = &GoeClient.clients[Client->connId];

    // for test. auto disable wait after one req/rsp pattern
	if( (client->obc.client.srm_flags & OBSF_CLIENT_WAITING) == OBSF_CLIENT_WAITING ){
	    client->obc.client.srm_flags &= ~OBSF_CLIENT_WAITING;
	    GOEP_Report("[SRM][CLIENT] has a client wait parameter. disable it. after_flag:0x%x", client->obc.client.srm_flags);
		return TRUE;
    }    
    return FALSE;
}


BOOL GOEP_ServerDisableWait(GoepServerApp *Service){
    GoepServerObexCons *server;

    Assert(Service);
    server = &GoeServer.servers[Service->connId];

    // for test. auto disable wait after one req/rsp pattern
	if( (server->obs.server.srm_flags & OBSF_SERVER_WAITING) == OBSF_SERVER_WAITING ){
	    server->obs.server.srm_flags &= ~OBSF_SERVER_WAITING;
	    GOEP_Report("[SRM][SERVER] has a server wait parameter. disable it. after_flag:0x%x", server->obs.server.srm_flags);
		return TRUE;
    } 
	return FALSE;
}


U8   GOEP_ClientGetRemoteVersion(GoepClientApp *Client){
    GoepClientObexCons *client;
    
    Assert(Client);
    client = &GoeClient.clients[Client->connId]; 

    return client->obc.handle.parser.obexVersion;
}


U8   GOEP_ServerGetRemoteVersion(GoepServerApp *Service){
    GoepServerObexCons *server;
    
    Assert(Service);
    server = &GoeServer.servers[Service->connId];    

    return server->obs.handle.parser.obexVersion;
}


U8   GOEP_ClientGetTpType(GoepClientApp *Client){
#ifdef __BT_GOEP_V2__
    GoepClientObexCons *client;
    
    Assert(Client);
    client = &GoeClient.clients[Client->connId];    

    return client->obc.trans.ObexClientBtTrans.client.bConnType;    
#else
    return 0;
#endif
}


U8   GOEP_ServerGetTpType(GoepServerApp *Service){
#ifdef __BT_GOEP_V2__
    GoepServerObexCons *server;
    
    Assert(Service);
    server = &GoeServer.servers[Service->connId];    

    return server->obs.trans.ObexServerBtTrans.server.bConnType;
#else
        return 0;
#endif
}


BOOL GOEP_ClientSRMMode(GoepClientApp *Client){
#ifdef __BT_GOEP_V2__
    GoepClientObexCons *client;
    
    Assert(Client);
    client = &GoeClient.clients[Client->connId];    
    if( client->obc.client.srm_client == OBSH_SRM_ENABLE ){
        return TRUE;
    }
    return FALSE;
#else
    return FALSE;
#endif
}


BOOL GOEP_ServerSRMMode(GoepServerApp *Server){
#ifdef __BT_GOEP_V2__    
    GoepServerObexCons *server;
    
    Assert(Server);
    server = &GoeServer.servers[Server->connId];    
    if( server->obs.server.srm_server == OBSH_SRM_ENABLE ){
        return TRUE;
    }
    return FALSE;
#else
    return FALSE;
#endif
}
#endif /* __BT_GOEP_PROFILE__ */
