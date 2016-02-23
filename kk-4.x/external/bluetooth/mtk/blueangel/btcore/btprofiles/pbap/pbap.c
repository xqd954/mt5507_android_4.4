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
*     $Workfile:pbap.c$ for iAnywhere Blue SDK, PBAP Version 1.1.2
*     $Revision: #1 $
*
* Description:
*     This file contains the functions that comprise the Phonebook Access
*     Profile implementation.
*
* $Project:XTNDAccess Blue SDK$
*
* Copyright 2005-2006 Extended Systems, Inc.
* Portions copyright 2006 iAnywhere Solutions, Inc.
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
****************************************************************************
*============================================================================
* HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Log$
*
* 
*
* 
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
****************************************************************************/
/* Keep this header file outside of the compile option to avoid compile warning */

#ifdef __BT_PBAP_PROFILE__

#include "osapi.h"
#if BT_STACK == XA_ENABLED
#include "bttypes.h"
#endif /* BT_STACK == XA_ENABLED */
#include "pbap.h"
#include "pbapobs.h"


extern U8 ObexSdpDB_Registration(U32 service_type);
extern U8 ObexSdpDB_DeRegistration(U32 service_type);
extern void PBAPDeviceSdpDB_SupportedRepositories(U8 support_value);
/* Object Store Function call tables */
static const ObStoreFuncTable PhonebookStoreFuncTable = 
{
    PBAPOBS_Read,
    NULL, //PBAPOBS_Write,
    PBAPOBS_GetObjectLen,
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
   //NULL,PBAPOBS_ReadFlex,
   //PBAPOBS_WriteFlex,
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
};

static const U8 PbapUuid[] = 
{
    0x79, 0x61, 0x35, 0xf0, 0xf0, 0xc5, 0x11, 0xd8,
    0x09, 0x66, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 
};

#if PBAP_NUM_SERVERS > 0
static void SetOperation(const U8 *Type);
#endif /* PBAP_NUM_SERVERS > 0 */

static void PBAP_SetDefaultHeader(void);

#if XA_CONTEXT_PTR == XA_ENABLED
static PbapContext   temp;
PbapContext         *Pbap = &temp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
PbapContext          Pbap;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

#define PBAP_SERVER_INIT
/*****************************************************************************
* FUNCTION
*  PBAP_Init
* DESCRIPTION
*  Initialize the PBAP component.  This must be the first PBAP function called 
*  by the application layer, or if multiple PBAP applications exist, this function
*  should be called at system startup (see XA_LOAD_LIST in config.h).  GOEP and 
*  OBEX must also be initialized separately.
* PARAMETERS
*  void
* RETURNS
*  BT_BOOL
*****************************************************************************/
BT_BOOL PBAP_Init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL status = TRUE;  
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)Pbap, 0, sizeof(PbapContext)); 
#else
    OS_MemSet((U8 *)&Pbap, 0, sizeof(PbapContext)); 
#endif
	
    if (PBAPOBS_Init() == FALSE) 
    {	
	 Report(("PBAP Profile Initial FAILED\n"));			
        status = FALSE;
        goto Done;
    }
	
    PBAP(initialized) = TRUE;
	
Done:
    OS_UnlockObex();
    return status;
}


#if PBAP_NUM_SERVERS > 0 
/*****************************************************************************
* FUNCTION
*  PBAP_RegisterServer
* DESCRIPTION
*  PBAP server register function (now not multiplexor GOEP server request)
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_RegisterServer(PbapServerSession *Server, PbapServerCallback Callback)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if BT_SECURITY == XA_ENABLED
    BtSecurityLevel savedSecLevel;
#endif
    U8   saveSupportRepos;
    U8 i, j = PBAP_NUM_SERVERS;
    ObStatus status = OB_STATUS_BUSY;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server || !Callback) 
    {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
	
    if (PBAP(initialized) != TRUE) 
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif
	ASSERT(Server && Callback);
#if BT_SECURITY == XA_ENABLED
    savedSecLevel = Server->secLevel;
#endif
    saveSupportRepos = Server->suppored_repositories;
    OS_MemSet((U8 *)Server, 0, sizeof(PbapServerSession));
	
#if BT_SECURITY == XA_ENABLED
    Server->secLevel = savedSecLevel;
#endif
     Server->suppored_repositories = saveSupportRepos;

    if (PBAP(serverCallback) == 0 || PBAP(serverCallback) == Callback)
    {
        PBAP(serverCallback) = Callback;
        
        for (i = PBAP_NUM_SERVERS; i > 0; i--) 
        {
            if (PBAP(server)[i-1] == Server) 
            {
                status = BT_STATUS_IN_USE;
                goto Error;
            } 
            else if (PBAP(server)[i-1] == 0) 
            {
                j = i - 1;
            }
        }
    }
    else 
    {
        status = BT_STATUS_INVALID_PARM;
        goto Error;
    }
	
    Server->obexConn.target = (U8*)PbapUuid;
    Server->obexConn.targetLen = 16;
#if OBEX_SERVER_CONS_SIZE > 0
    Server->sApp.numTargets = 1;
    Server->sApp.target[0] = &Server->obexConn;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    Server->sApp.type = GOEP_PROFILE_PBAP;
    Server->sApp.bt_service = GOEP_PROFILE_PBAP;
    Server->sApp.callback = PbapGoepServerCallback;
    Server->sApp.connFlags = GOEP_NEW_CONN;
    Server->sApp.connId = 0;
 
	
    if (j < PBAP_NUM_SERVERS)
    {
        PBAP(server)[j] = Server;
        status = GOEP_RegisterServer(&Server->sApp, &PhonebookStoreFuncTable);
        if (status != OB_STATUS_SUCCESS)
        {
            goto Error;
        }
		PBAPDeviceSdpDB_SupportedRepositories(Server->suppored_repositories);
		ObexSdpDB_Registration(0x112F);
		Report(("PBAP Registered On GOEP\n"));
    }
    else 
    {
        status = BT_STATUS_NO_RESOURCES;
        goto Error;
    }
    
#if BT_SECURITY == XA_ENABLED
    /* Add Phonebook Access Bluetooth security record */
    if (Server->secLevel != BSL_NO_SECURITY) {
        /* Security record to enforce authentication/authorization/encryption */ 
        status = GOEP_RegisterServerSecurityRecord(&Server->sApp, Server->secLevel);
        ASSERT(status == XA_STATUS_SUCCESS);
    }
#endif /* BT_SECURITY == XA_ENABLED */
    
#if OBEX_DEINIT_FUNCS == XA_ENABLED
    if (status != OB_STATUS_SUCCESS) {
        GOEP_DeregisterServer(&Server->sApp);
		ObexSdpDB_DeRegistration(0x112f);
        goto Error;
    }
#else /* OBEX_DEINIT_FUNCS == XA_ENABLED */
    ASSERT(status == OB_STATUS_SUCCESS);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
	
Error:
    OS_UnlockObex();
    return status;
}


#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  PBAP_DeregisterServer
* DESCRIPTION
*  deregister PBAP server from GOEP
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_DeregisterServer(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, numActiveServers;
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server)
    {
        goto Error;
    }
	
    if (PBAP(initialized) != TRUE)
    {
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    numActiveServers = 0;
    for (i = 0; i < PBAP_NUM_SERVERS; i++)
    {
        if (PBAP(server)[i] == Server) 
        {
            PBAP(server)[i] = 0;
            status = BT_STATUS_SUCCESS;
        }
        if (PBAP(server)[i] != 0)
        {
            numActiveServers++;
        }
    }
	
    if (numActiveServers == 0) 
    {
        PBAP(serverCallback) = 0;
    }
	
    status = GOEP_DeregisterServer(&Server->sApp);
    ObexSdpDB_DeRegistration(0x112f);
	/* kal_trace(TRACE_GROUP_5, BT_PBAP_DEREGISTERED);*/ 
	Report(("PBAP Deregistered On GOEP\n"));
	
    if (status == OB_STATUS_SUCCESS)
    {
#if BT_SECURITY == XA_ENABLED
        if (Server->secLevel != BSL_NO_SECURITY)
        {
            GOEP_UnregisterServerSecurityRecord(&Server->sApp);
			ASSERT(status == XA_STATUS_SUCCESS);
        }
#endif /* BT_SECURITY == XA_ENABLED */
    }
	
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */


#define PBAP_SERVER_PROCESS_FUNCTION
/*****************************************************************************
* FUNCTION
*  PBAP_ServerAbort
* DESCRIPTION
*  when the server find any error , then call this function to send error info to
*  client.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerAbort(PbapServerSession *Server, PbapRespCode Resp)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex(); 
    Report(("PBAP Server Abort\n"));
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    status = GOEP_ServerAbort(&Server->sApp, Resp);
	
Error:
    OS_UnlockObex();
    return status;
}


/*****************************************************************************
* FUNCTION
*  PBAP_ServerContinue
* DESCRIPTION
* Synopsis: This function is called by the server in response to a received
*           PBAP_EVENT_CONTINUE event. It must be called once for every
*           CONTINUE event received. It may or may not be called in the 
*           context of the callback and can be deferred for flow control 
*           purposes.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerContinue(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    status = GOEP_ServerContinue(&Server->sApp);
	
Error:
    OS_UnlockObex();
    return status;
}


#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED 
/*****************************************************************************
* FUNCTION
*  PBAP_ServerTpDisconnect
* DESCRIPTION
* Synopsis: This function is used by the server to issue a transport 
*           disconnection.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerTpDisconnect(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server)
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    status = GOEP_ServerTpDisconnect(&Server->sApp);
	
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */


#define PBAP_SERVER_AUTH_FUNCTION
#if OBEX_AUTHENTICATION == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  PBAP_ServerAuthenticate
* DESCRIPTION
* Synopsis: This function is called by the server to initiate OBEX 
*           authentication on the upcoming Phonebook Access OBEX connection.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerAuthenticate(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    TimeT       time;
    U16         passwordLen;
    ObStatus    status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    if (Server->flags & PBAP_FLAGS_AUTHENTICATED) {
        /* We are already authenticated */
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
	
    Server->flags |= PBAP_FLAGS_AUTHENTICATION_REQ;
    OS_MemSet((U8 *)&Server->chal, 0, sizeof(Server->chal));
	
    passwordLen = OS_StrLen((const char *)Server->password);
    time = OS_GetSystemTime();
    Server->chalStr[0] = (U8)(time);
    Server->chalStr[1] = (U8)(time >> 8);
    Server->chalStr[2] = (U8)(time >> 16);
    Server->chalStr[3] = (U8)(time >> 24);
    Server->chalStr[4] = ':';
    OS_MemCopy(Server->chalStr+5, Server->password, passwordLen);
	
    Server->chal.challenge = Server->chalStr;
    Server->chal.challengeLen = 5 + passwordLen;
    Server->chal.options = 0x01;  /* Must send the UserId */
    Server->chal.realm = Server->realm;
    Server->chal.realmLen = (U8)OS_StrLen((const char *)Server->realm);
	
    Server->flags |= PBAP_FLAGS_CHAL_REQ;
	
    status = OB_STATUS_SUCCESS;
	
Error:
    OS_UnlockObex();
    return status;
}


/*****************************************************************************
* FUNCTION
*  PBAP_ServerAuthCheck
* DESCRIPTION
* Synopsis: This function is called by the server to set the proper username, 
*           password, and challenge realm information for the upcoming 
*           Phonebook Access OBEX connection. 
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerSetAuthInfo(PbapServerSession *Server, PbapAuthInfo *Info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus    status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server || !Info)
    {
        goto Error;
    }
	
    if (Info->passwordLen >= PBAP_MAX_PASSWORD_LEN ||
        Info->userIdLen >= PBAP_MAX_USERID_LEN ||
        Info->realmLen >= PBAP_MAX_REALM_LEN)
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    /* Clear the password, userId, and realm to make sure null-termination
	* always exists, as we always ensure that one byte is unused.
	*/
    OS_MemSet(Server->password, 0, PBAP_MAX_PASSWORD_LEN);
    OS_MemSet(Server->userId, 0, PBAP_MAX_USERID_LEN);
    OS_MemSet(Server->realm, 0, PBAP_MAX_REALM_LEN);
	
    /* Set the authentication information */
    OS_MemCopy(Server->password, Info->password, Info->passwordLen);
    OS_MemCopy(Server->userId, Info->userId, Info->userIdLen);
    OS_MemCopy(Server->realm, Info->realm, Info->realmLen);
	
    status = OB_STATUS_SUCCESS;
	
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */



#define PBAP_SERVER_HANDLE_GOEP_EVENT_CALLBACK 
/*****************************************************************************
* FUNCTION
*  PbapGoepServerCallback
* DESCRIPTION
*  This function processes GOEP Server protocol events.
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
void PbapGoepServerCallback(GoepServerEvent *Event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                                                         */
    /*----------------------------------------------------------------*/
    U8 *p, i, j;
    PbapServerSession *server;
    ObexRespCode rcode;
    PbapOp savedOper;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                                                               */
    /*----------------------------------------------------------------*/
    kal_trace(TRACE_GROUP_5, BT_PBAP_SERVER_GOEP_CALLBACK, Event->event, Event->oper);
	
    server = FindPbapServer(Event->handler);
    
    if (server == 0) 
    {
	Report(("PBAP Server Not Found\n"));
        return;
    }
	
    savedOper = PBAP(serverParms).oper;
    PBAP(serverParms).oper = savedOper;
    PBAP(serverParms).server = server;
	
    if ((Event->oper == GOEP_OPER_PUSH) || (Event->oper == GOEP_OPER_DELETE))
    {
        /* This is an illegal Phonebook Access command. Abort it and hide it from the app. */
        if (Event->event == GOEP_EVENT_START)
        {
            GOEP_ServerAbort(Event->handler, OBRC_BAD_REQUEST);            
            GOEP_ServerContinue(Event->handler);
        }
		
        if (Event->event == GOEP_EVENT_CONTINUE)
        {
            GOEP_ServerContinue(Event->handler);
        }
        return;
    }
	
    switch (Event->event)
    {
	case GOEP_EVENT_AUTHORIZE_IND:
		PBAP(serverParms).event = PBAP_EVENT_AUTHRIZE_IND;
		PBAP(serverCallback)(&PBAP(serverParms));
		break;
		
	case GOEP_EVENT_START:
		OS_MemSet((U8 *)&PBAP(serverParms), 0, sizeof(PbapServerCallbackParms));
		switch (Event->oper)
		{
		case GOEP_OPER_CONNECT:
			PBAP(serverParms).oper = PBAPOP_CONNECT;
			break;
			
		case GOEP_OPER_DISCONNECT:
			PBAP(serverParms).oper = PBAPOP_DISCONNECT;
			break;
			
		case GOEP_OPER_PULL:
			/* for fix no header client */
			PBAP_SetDefaultHeader();                   
			server->object = PBAPOBS_New(&server->sApp);
		 	ASSERT(server->object);
			OS_MemSet((U8 *)&server->appParms, 0, PBAP_MAX_APP_PARMS_LEN);
			PBAP(serverParms).oper = PBAPOP_PULL;
			break;
			
		case GOEP_OPER_SETFOLDER:
			PBAP(serverParms).oper = PBAPOP_SET_PHONEBOOK;
			break;
		}
		
		PBAP(serverParms).event = PBAP_EVENT_START;
		PBAP(serverCallback)(&PBAP(serverParms));
		break;
		
        case GOEP_EVENT_HEADER_RX:
			
			if(Event->header.type == OBEXH_APP_PARAMS
				&& Event->header.len >0 
				&& PBAP(serverParms).oper == PBAPOP_PULL)
			{
				if (PBAP(serverParms).oper == PBAPOP_PULL)
				{
					SetOperation(Event->info.pushpull.type);
				}
				PBAP(serverParms).u.paramsRx.type = (U8 *)Event->info.pushpull.type;
				
                               if( (server->appParms[0] + Event->header.len + 1) >= PBAP_MAX_APP_PARMS_LEN){
                                   bt_prompt_trace(MOD_BT, "[PBAP] out of boundary of server->appParms");
                                   break;
                               }
				p = server->appParms + server->appParms[0] + 1;
				OS_MemCopy(p, Event->header.buffer, Event->header.len);
				server->appParms[0] += Event->header.len;
				
				if (server->appParms[0] == Event->header.totalLen)
				{
					p = server->appParms + 1;
					while (p - (server->appParms + 1) < Event->header.totalLen)
					{
						switch (p[0])
						{
						case PBAP_TAG_FILTER:
							j = 2;
							for (i = PBAP_FILTER_SIZE; i > 0; i--)
							{
								PBAP(serverParms).u.paramsRx.filter.byte[i-1] = p[j++];
							}
							break;
							
						case PBAP_TAG_FORMAT:
							PBAP(serverParms).u.paramsRx.format = p[2];
							break;
							
						case PBAP_TAG_ORDER:
							PBAP(serverParms).u.paramsRx.order = p[2];
							break;
							
						case PBAP_TAG_SEARCH_VALUE:
							PBAP(serverParms).u.paramsRx.searchValue = p + 2;
							PBAP(serverParms).u.paramsRx.searchValueLen = p[1];
							break;
							
						case PBAP_TAG_SEARCH_ATTRIB:
							PBAP(serverParms).u.paramsRx.searchAttrib = p[2];
							break;
							
						case PBAP_TAG_MAX_LIST_COUNT:
							PBAP(serverParms).u.paramsRx.maxListCount = BEtoHost16(p + 2);
							break;
							
						case PBAP_TAG_LIST_OFFSET:
							PBAP(serverParms).u.paramsRx.listStartOffset = BEtoHost16(p + 2);
							break;
						}
						
						p += 1 + 1 + p[1];
					}
				}				
				
				if (Event->info.pushpull.nameLen != 0)
				{
					PBAP(serverParms).u.paramsRx.name = (U8 *)Event->info.pushpull.name;
				}			
			}   
			break;
			
		case GOEP_EVENT_PROVIDE_OBJECT:
				if (PBAP(serverParms).oper == PBAPOP_PULL)
				{
					SetOperation(Event->info.pushpull.type);
				}   
				
				PBAP(serverParms).u.paramsRx.type = (U8 *)Event->info.pushpull.type;
				
				if ((Event->info.pushpull.nameLen != 0)
					&&(PBAP(serverParms).oper == PBAPOP_PULL_PHONEBOOK
					|| PBAP(serverParms).oper == PBAPOP_PULL_VCARD_LISTING
					|| PBAP(serverParms).oper == PBAPOP_PULL_VCARD_ENTRY))
				{
					PBAP(serverParms).u.paramsRx.name = (U8 *)Event->info.pushpull.name;
				}
				break;
				
		case GOEP_EVENT_CONTINUE:
				switch(Event->oper) 
				{
					case GOEP_OPER_DISCONNECT:
					case GOEP_OPER_ABORT:
						if (server->object)
						{
							PBAPOBS_Close(&server->object);
						}
						GOEP_ServerContinue(&server->sApp);
						return;

					 case GOEP_OPER_CONNECT:                    
					 	PBAP(serverParms).event = PBAP_EVENT_TP_CONNECTED;
						PBAP(serverParms).oper = PBAPOP_CONNECT;
						break;
						
					case GOEP_OPER_SETFOLDER:
						PBAP(serverParms).u.info.setPb.name = (const U8 *)Event->info.setfolder.name;
						PBAP(serverParms).u.info.setPb.flags = Event->info.setfolder.flags;
						PBAP(serverParms).u.info.setPb.reset = Event->info.setfolder.reset;
						PBAP(serverParms).oper = PBAPOP_SET_PHONEBOOK;
						break;
						
					case GOEP_OPER_PULL:
						{

						}
						break;
					default:
						break;
				}				
				PBAP(serverParms).event = PBAP_EVENT_CONTINUE;
				PBAP(serverCallback)(&PBAP(serverParms));
				break;
				
			case GOEP_EVENT_PRECOMPLETE:
				PBAP(serverParms).event = PBAP_EVENT_PRECOMPLETE;
				PBAP(serverCallback)(&PBAP(serverParms));
				break;
					
			case GOEP_EVENT_COMPLETE:
					switch (Event->oper)
					{
					case GOEP_OPER_DISCONNECT:
						PBAP(serverParms).oper = PBAPOP_DISCONNECT;
						server->flags = 0; /* Reset flags */
						break;
						
					case GOEP_OPER_PULL:
						if (server->object)
						{
							rcode = PBAPOBS_Close(&server->object);
							if (rcode != OBRC_SUCCESS)
							{
								GOEP_ServerAbort(&server->sApp, rcode);
							}
						} 
						break;
						
					default:
						break;
					}
					
#if OBEX_AUTHENTICATION == XA_ENABLED
					if ((server->flags & PBAP_FLAGS_AUTHENTICATION_REQ) && 
						!(server->flags & PBAP_FLAGS_RESP_RCVD))
					{
						PBAP(serverParms).u.auth.result = FALSE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_NO_RCVD_AUTH_RESP;
						PBAP(serverParms).event = PBAP_EVENT_AUTH_RESULT;
						PBAP(serverCallback)(&PBAP(serverParms));
					}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					
					kal_trace(TRACE_GROUP_5, BT_PBAP_SERVER_OPER_COMPLETE, Event->oper);					
					PBAP(serverParms).event = PBAP_EVENT_COMPLETE;
					PBAP(serverCallback)(&PBAP(serverParms));					
					
#if OBEX_AUTHENTICATION == XA_ENABLED
					/* Clear authentication response flag if one was received */
					server->flags &= ~PBAP_FLAGS_RESP_RCVD;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					break;
					
				case GOEP_EVENT_TP_CONNECTED:
					break;
						
				case GOEP_EVENT_TP_DISCONNECTED:
					if (server->object)
					{
						PBAPOBS_Close(&server->object);
					}						
					server->flags = 0;
					PBAP(serverParms).event = PBAP_EVENT_TP_DISCONNECTED;
					PBAP(serverParms).oper = PBAPOP_NONE;
					PBAP(serverCallback)(&PBAP(serverParms));
					break;
						
				case GOEP_EVENT_ABORTED:
					if (server->object)
					{
						PBAPOBS_Close(&server->object);
					}
						
#if OBEX_AUTHENTICATION == XA_ENABLED
					if ((server->flags & PBAP_FLAGS_AUTHENTICATION_REQ) && 
							!(server->flags & PBAP_FLAGS_RESP_RCVD))
					{
						PBAP(serverParms).u.auth.result = FALSE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_NO_RCVD_AUTH_RESP;
						PBAP(serverParms).event = PBAP_EVENT_AUTH_RESULT;
						PBAP(serverCallback)(&PBAP(serverParms));
					}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					PBAP(serverParms).event = PBAP_EVENT_ABORTED;
					PBAP(serverCallback)(&PBAP(serverParms));
					PBAP(serverParms).oper = PBAPOP_NONE;
						
#if OBEX_AUTHENTICATION == XA_ENABLED
					/* Clear authentication response flag if one was received */
					server->flags &= ~PBAP_FLAGS_RESP_RCVD;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					break;						
		  				
#if OBEX_AUTHENTICATION == XA_ENABLED
				case GOEP_EVENT_AUTH_CHALLENGE:
					/* Authentication challenge has been received.
					* Prepare authentication response to go out.*/
					OS_MemSet((U8 *)&server->resp, 0, sizeof(server->resp));
					OS_MemSet((U8 *)&server->challengeInfo, 0, sizeof(server->challengeInfo));
					server->flags |= PBAP_FLAGS_CHAL_RCVD;
					server->challengeInfo.options = Event ->challenge.options;
					server->challengeInfo.realmLen =  Event ->challenge.realmLen;
					OS_MemCopy(server->challengeInfo.realm, Event ->challenge.realm, server->challengeInfo.realmLen);
					OS_MemCopy(server->challengeInfo.nonce, Event ->challenge.nonce, AUTH_NONCE_LEN);
					break;
					
				case GOEP_EVENT_AUTH_RESPONSE:
					/* Authentication response has been received */
					if (GOEP_ServerVerifyAuthResponse(Event->handler, server->password, 
							(U8)OS_StrLen((const char *)(server->password))))
					{
						server->flags |= PBAP_FLAGS_AUTHENTICATED;
						PBAP(serverParms).u.auth.result = TRUE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_NO_FAILURE;
					}
					else 
					{
						GOEP_ServerAbort(Event->handler, OBRC_UNAUTHORIZED);
						PBAP(serverParms).u.auth.result = FALSE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_INVALID_CREDENTIALS;
					}
					PBAP(serverParms).event = PBAP_EVENT_AUTH_RESULT;
					PBAP(serverCallback)(&PBAP(serverParms));
					server->flags |= PBAP_FLAGS_RESP_RCVD;
					break;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
				default:
					break;
			} 
}


#define PBAP_SERVER_UTIL_FUNCTION
/*****************************************************************************
* FUNCTION
*  PBAP_SetDefaultHeader
* DESCRIPTION
*  if the get command have no header , it will use default header to get data
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
static void PBAP_SetDefaultHeader(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemSet(PBAP(serverParms).u.paramsRx.filter.byte, 0, PBAP_FILTER_SIZE);
    PBAP(serverParms).u.paramsRx.format = VCARD_FORMAT_21;
    PBAP(serverParms).u.paramsRx.order = PBAP_SORT_ORDER_INDEXED;
    PBAP(serverParms).u.paramsRx.searchValue = NULL;
    PBAP(serverParms).u.paramsRx.searchValueLen = 0;
    PBAP(serverParms).u.paramsRx.searchAttrib = PBAP_SEARCH_ATTRIB_NAME;
    PBAP(serverParms).u.paramsRx.listStartOffset = 0;
    PBAP(serverParms).u.paramsRx.maxListCount = PBAP_INVALID_COUNT;
}


/*****************************************************************************
* FUNCTION
*  PBAP_ServerAuthCheck
* DESCRIPTION
*  check the server wether has received the auth challeng request
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerAuthCheck(PbapServerSession *Server, U8 cancel)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    if (cancel == 1)
    {
        GOEP_ServerAbort(&Server->sApp, OBRC_UNAUTHORIZED);
        goto Error;
    }
    
#if OBEX_AUTHENTICATION == XA_ENABLED
    if (Server->flags & PBAP_FLAGS_CHAL_REQ)
    {
        Server->flags &= ~PBAP_FLAGS_CHAL_REQ;
        /* Issue an OBEX Authentication challenge header */
        status = GOEP_ServerAuthenticate(&Server->sApp, 0, &Server->chal);
        if (status != OB_STATUS_SUCCESS)
        {
            GOEP_ServerAbort(&Server->sApp, OBRC_UNAUTHORIZED);
        }
    }
    else if (Server->flags & PBAP_FLAGS_CHAL_RCVD)
    {
        Server->flags &= ~PBAP_FLAGS_CHAL_RCVD;
        /* Issue an OBEX Authentication response header */
        status = GOEP_ServerAuthenticate(&Server->sApp, &Server->resp, 0);
        if (status != OB_STATUS_SUCCESS)
        {
            GOEP_ServerAbort(&Server->sApp, OBRC_UNAUTHORIZED);
        }
    }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
	
Error:
    OS_UnlockObex();
    return status;
}

/*****************************************************************************
* FUNCTION
*  SetOperation
* DESCRIPTION
* 
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
static void SetOperation(const U8 *Type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16     len;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    len = OS_StrLen((const char *)Type);
	
    if (OS_MemCmp(Type, len, (const U8 *)PULL_PHONEBOOK_TYPE, OS_StrLen((const char *)PULL_PHONEBOOK_TYPE)))
    {
        PBAP(serverParms).oper = PBAPOP_PULL_PHONEBOOK;
    } 
    else if (OS_MemCmp(Type, len, (const U8 *)VCARD_LISTING_OBJECT_TYPE, OS_StrLen((const char *)VCARD_LISTING_OBJECT_TYPE)))
    {
        PBAP(serverParms).oper = PBAPOP_PULL_VCARD_LISTING;
    } 
    else if (OS_MemCmp(Type, len, (const U8  *)VCARD_OBJECT_TYPE, OS_StrLen((const char *)VCARD_OBJECT_TYPE)))
    {
        PBAP(serverParms).oper = PBAPOP_PULL_VCARD_ENTRY;
    }
    
}


/*****************************************************************************
* FUNCTION
*  FindPbapServer
* DESCRIPTION
*  if the pbap server support multiple client, then this function is to find the
*  right server to handle the client request. return PBAP server session pointer.
* PARAMETERS
*  
* RETURNS
*  PbapServerSession
*****************************************************************************/
PbapServerSession *FindPbapServer(GoepServerApp *app)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < PBAP_NUM_SERVERS; i++)
    {
        if (PBAP(server)[i] && &PBAP(server)[i]->sApp == app)
        {
            return PBAP(server)[i];
        }
    }	
    return 0;
}

#endif /* PBAP_NUM_SERVERS > 0 */

#endif /* __BT_PBAP_NEW_PROFILE__ */
