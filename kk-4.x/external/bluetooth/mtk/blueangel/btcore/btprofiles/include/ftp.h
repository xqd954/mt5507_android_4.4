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
 * opp.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   
 *
 * Author:
 * -------
 * Daylong Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifndef __FTP_H__
#define __FTP_H__
#ifndef __BT_GOEP_V2__
#include "bluetooth_struct.h"

typedef struct _BTFTPCallbackParms BTFTPCallbackParms;
typedef void (*BTFTPCallback)(BTFTPCallbackParms *Parms);
void btmtk_ftp_register(BTFTPCallback Callback);

void bt_ftp_event_hdlr(U32 msg_id, void* data, U32 u4Size);
#else

#if BT_STACK == XA_ENABLED
#include <sdp.h>
#endif /* BT_STACK == XA_ENABLED */
#include <obex.h>
#include <goep.h>
#include "ftpobs.h"
#include "bluetooth_struct.h"


typedef struct _BTFTPCallbackParms BTFTPCallbackParms;
typedef void (*BTFTPCallback)(BTFTPCallbackParms *Parms);
void btmtk_ftp_register(BTFTPCallback Callback);
void bt_ftp_event_hdlr(U32 msg_id, void* data, U32 u4Size);

#define FTPS_TEMP_PUSH_FILEPATH  L"/data/@btmtk/ftp.temp.file"
#define FTP_UPDATE_PROGRESS_GAP  10  /* 0~100 */

/*---------------------------------------------------------------------------
 * File Transfer Profile (Ftp) Layer 
 *
 *     The FTP layer provides two levels of service for applications
 *     implementing the File Transfer profile. The basic service provides
 *     functions for registering the FTP SDP entries and for building
 *     FTP SDP queries. These functions can be used directly with the
 *     OBEX API. The expanded services include functions which expand the
 *     basic GOEP API to include Ftp specific functionality.
 */

/*---------------------------------------------------------------------------
 * FTP_EXPANDED_API configuration option
 *
 *     This option toggles support for the full File Transfer API.
 */
#ifndef FTP_EXPANDED_API
#define FTP_EXPANDED_API  XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * FOLDER_LISTING_OBJECT_TYPE constant
 *
 *     The FOLDER_LISTING_OBJECT_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting a folder listing
 *     object.
 */
#define FOLDER_LISTING_OBJECT_TYPE      "x-obex/folder-listing"

#if FTP_EXPANDED_API == XA_ENABLED

#if OBEX_SERVER_CONS_SIZE == 0
#error "OBEX_SERVER_CONS_SIZE Must be greater than zero."
#endif
#endif /* FTP_EXPANDED_API == XA_ENABLED */
 
/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * FTP_Init()
 *
 *     Initialize the FTP component.  This must be the first FTP 
 *     function called by the application layer, or if multiple 
 *     FTP applications exist, this function should be called
 *     at system startup (see XA_LOAD_LIST in config.h).  GOEP 
 *     and OBEX must also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL FTP_Init(void);

#if BT_STACK == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_AddFtpServiceRecord()
 *
 *     Registers the File Transfer SDP record in the local SDP database.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     BtStatus of the SDP_AddRecord() function call.
 *     
 */
BtStatus FTP_AddServiceRecord(GoepServerApp *Server);

/*---------------------------------------------------------------------------
 * FTP_RemoveFtpServiceRecord()
 *
 *     Removes the previously registered File Transfer SDP record from the
 *     local SDP database
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     BtStatus of the SDP_RemoveRecord() function call.
 *     
 */
BtStatus FTP_RemoveServiceRecord(GoepServerApp *Server);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_BuildServiceQuery()
 *
 *     Builds a File Transfer SDP query compatible with the OBEX ObexTpAddr
 *     format for Bluetooth targets.
 *
 * Parameters:
 *     QueryPtr - This pointer receives a pointer to the SDP Query data.
 *     QueryLen - This pointer is set to the length of the Query data.    
 *     QueryType - This pointer is set to the type of SDP query provided.
 *
 * Returns:
 *     void
 *
 */
void FTP_BuildServiceQuery(const U8 **QueryPtr, U16 *QueryLen, 
                           SdpQueryType *QueryType);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* BT_STACK == XA_ENABLED */

#if FTP_EXPANDED_API == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_RegisterServer()
 *
 *     Registers the file transfer server with the GOEP multiplexor. This 
 *     includes registering the SDP records for the file transfer Service.
 *     All of the events specified in the GOEP layer are delivered to the
 *     FTP server.
 *
 * Parameters:
 *     Server - The server application's registration structure. The 
 *          'callback' and 'obstore' fields must be filled in.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Server is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Server.
 *     
 *     OB_STATUS_BUSY - An OPush server is already registered.
 *     
 */
ObStatus FTP_RegisterServer(GoepServerApp *Server, 
                                const ObStoreFuncTable *obStoreFuncs);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_DeregisterServer()
 *
 *     Deregisters the file transfer server from the GOEP multiplexor.
 *     This includes removing the SDP records for the file transfer Service.
 *     
 * Parameters:
 *     Server - The server used in the registration call.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The server was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The server is not registered.
 *
 *     OB_STATUS_BUSY - The server could not be deregistered because
 *          it is currently processing an operation.
 *
 *     OB_STATUS_FAILED - The OBEX server failed to deinitialize.
 */
ObStatus FTP_DeregisterServer(GoepServerApp *Server);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * FTP_ServerContinue()
 *
 *     This function is called by the server in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The server is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus FTP_ServerContinue(GoepServerApp *Server);
#define FTP_ServerContinue(_SERVER) GOEP_ServerContinue(_SERVER)

/*---------------------------------------------------------------------------
 * FTP_ServerAccept()
 *
 *     This function is called to accept a Push or Pull request.
 *     It MUST be called during the GOEP_EVENT_PROVIDE_OBJECT indication for
 *     these operations. Failure to do so will abort the operation.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *   
 *     Obsh - The handle of an Object store object handle. This handle
 *          is not interpreted by the GOEP server (but it must not be zero).
 *          It is passed to the registered object store functions.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server does not have an operation in progress.
 *
 *     OB_STATUS_SUCCESS - The object was accepted.
 *     
 */
ObStatus FTP_ServerAccept(GoepServerApp *Server, void *Obsh);
#define FTP_ServerAccept(_SERVER, _OBSH) GOEP_ServerAccept(_SERVER, _OBSH)

/*---------------------------------------------------------------------------
 * FTP_ServerAbort()
 *
 *     Aborts the current server operation.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Resp - The response code to send to the client expressing the reason
 *         for the abort.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The abort was successfully requested.
 *
 *     OB_STATUS_FAILED - The abort operation failed.
 *            
 *     OB_STATUS_NO_CONNECT - No transport connection exists.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus FTP_ServerAbort(GoepServerApp *Server, ObexRespCode Resp);
#define FTP_ServerAbort(_SERVER, _RESP) GOEP_ServerAbort(_SERVER, _RESP)

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_ServerAuthenticate()
 *
 *     This function is used to either respond to an authentication challenge
 *     received from a client, or to send a challenge to the client. The
 *     server never sends both headers at the same time. If the client's
 *     request is challenged, it is automatically rejected with the reason
 *     OBRC_UNAUTHORIZED and the oper will complete with GOEP_EVENT_ABORTED.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Response - The authentication response to a received challenge.
 *
 *     Challenge - The authentication challenge.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server is not the active server or FTP
 *          encountered an error building the authentication header.
 *
 *     OB_STATUS_INVALID_PARM - The supplied authentication information
 *          conflicts with what is expected or acceptable.
 *
 *     OB_STATUS_SUCCESS - The authentication information was accepted.
 */
ObStatus FTP_ServerAuthenticate(GoepServerApp *Server, 
                                ObexAuthResponse *Response, 
                                ObexAuthChallenge *Challenge);
#define FTP_ServerAuthenticate(_SERVER, _RESP, _CHAL) \
	GOEP_ServerAuthenticate(_SERVER, _RESP, _CHAL)

/*---------------------------------------------------------------------------
 * FTP_ServerVerifyAuthResponse()
 *
 *     This function is used by the Server to verify an authentication
 *     response received from the client. The server should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Password - The password that the client was expected to provide. This
 *         value is used in the MD5 verification digest calculation.
 *
 *     PasswordLen - The length, in bytes of the password.
 *
 * Returns:
 *     TRUE - Verification succeeded, the digest provided by the client
 *           matched the one generated locally by the server.
 *
 *     FALSE - Verification failed, the client is not authenticated.
 */
BOOL FTP_ServerVerifyAuthResponse(GoepServerApp *Server, 
								  U8 *Password, U8 PasswordLen);
#define FTP_ServerVerifyAuthResponse(_SERVER, _PASS, _LEN) \
	GOEP_ServerVerifyAuthResponse(_SERVER, _PASS, _LEN)
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_RegisterClient()
 *
 *     Registers the file transfer client with the GOEP multiplexor.
 *     All of the events specified in the GOEP layer are delivered to the
 *     FTP client, except for GOEP_EVENT_TP_CONNECTED.
 *     
 * Parameters:
 *     Client - The client application's registration structure. The 
 *          'callback' and 'obstore' fields must be filled in.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Client is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Client.
 *
 *     OB_STATUS_BUSY - An Ftp client is already registered.
 *     
 */
ObStatus FTP_RegisterClient(GoepClientApp *Client, 
                                const ObStoreFuncTable *obStoreFuncs);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_DeregisterClient()
 *
 *     Deregisters the file transfer client from the GOEP multiplexor.
 *     
 * Parameters:
 *     Client - The structure used to register the client.     
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The client was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The client is not registered.
 *
 *     OB_STATUS_BUSY - The client could not be deregistered because
 *          it is currently sending an operation.
 *
 *     OB_STATUS_FAILED - The OBEX client failed to deinitialize.
 *
 */
ObStatus FTP_DeregisterClient(GoepClientApp *Client);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * FTP_Connect()
 *
 *     This function is used by the client to create a transport connection
 *     to the specified device and issue an OBEX Connect Request. The FTP
 *     Target header is automatically sent in the OBEX Connect request.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Target - This structure describes the server to which the client
 *         wishes to connect.
 *
 *     ConnReq - Optional connect request parameters. This parameter may
 *         be zero if no authentication headers are necessary.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled by an event to the application callback. 
 *
 *         If the connection is successful, a GOEP_EVENT_COMPLETE event for
 *         the GOEP_OPER_CONNECT operation will be signaled.
 *
 *         If the transport connection is successful, but the OBEX Connect
 *         failed, the completion event will be GOEP_EVENT_ABORTED for the
 *         operation GOEP_OPER_CONNECT. At this point the transport is
 *         CONNECTED but a successful OBEX Connect has not been sent.
 *         Additional calls to this function will generate new OBEX Connect
 *         requests. 
 * 
 *         If the transport connection is unsuccessful, the completion event
 *         will be GOEP_EVENT_TP_DISCONNECTED, GOEP_EVENT_DISCOVERY_FAILED, or
 *         GOEP_EVENT_NO_SERVICE_FOUND.
 *
 *     OB_STATUS_SUCCESS - The client is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the client
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The client is currently executing an operation.
 *
 */
ObStatus FTP_Connect(GoepClientApp *Client, ObexTpAddr *Target, 
                         GoepConnectReq *ConnReq);

/*---------------------------------------------------------------------------
 * FTP_Disconnect()
 *
 *     This function is used by the client to issue an OBEX Disconnect Request.
 *     When the OBEX Disconnect is completed, the transport connection will
 *     be disconnected automatically.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the GOEP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 */
ObStatus FTP_Disconnect(GoepClientApp *Client);
#define FTP_Disconnect(_CLIENT) GOEP_Disconnect(_CLIENT)

/*---------------------------------------------------------------------------
 * FTP_ClientAbort()
 *
 *     Aborts the current client operation. The completion event will signal
 *     the status of the operation in progress, either COMPLETE or ABORTED.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Operation was successful.
 *
 *     OB_STATUS_FAILED - Operation failed to start.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameter.
 */
ObStatus FTP_ClientAbort(GoepClientApp *Client);
#define FTP_ClientAbort(_CLIENT) GOEP_ClientAbort(_CLIENT)

/*---------------------------------------------------------------------------
 * FTP_Push()
 *
 *     Initiates the OBEX "Put" operation to send an object to the remote
 *     server.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Object - Parameters necessary to build a push request. This
 *         parameter must be provided.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_FAILED - Operation was not started because
 *         of an underlying transport problem.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 *     
 */
ObStatus FTP_Push(GoepClientApp *Client, GoepObjectReq *Object);
#define FTP_Push(_CLIENT, _OBJECT) GOEP_Push(_CLIENT, _OBJECT)

/*---------------------------------------------------------------------------
 * FTP_Pull()
 *
 *     Initiates the OBEX "Get" operation to retrieve an object from
 *     the remote server.  
 *
 *     If this operation fails due to a failure in adding queued headers
 *     (GOEP_ClientQueueHeader), it may be useful to set the "More" parameter 
 *     when reissuing this command.  Doing so will allow the GET operation to
 *     send all the headers that will fit in the existing GET request, but will 
 *     also force the GET request to not send the final bit. This allows
 *     subsequent GET request packets to send additional headers prior to a 
 *     GET response sending back the object being described in the GET request 
 *     headers.  Any additional headers that cannot fit in the initial GET 
 *     request will be sent when FTP_ClientContinue is called during the 
 *     GOEP_EVENT_CONTINUE.  GOEP_ClientQueueHeader should be called before or 
 *     during this event to ensure they are sent in the next GET request packet.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Object - Parameters necessary to build a pull request. This
 *         parameter must be provided.
 *
 *     More - More headers exist in addition to the queued headers already
 *         added via GOEP_ClientQueueHeader.  
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *     
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *     
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 */
ObStatus FTP_Pull(GoepClientApp *Client, GoepObjectReq *object, BOOL more);
#define FTP_Pull(_CLIENT, _OBJECT, _MORE) GOEP_Pull(_CLIENT, _OBJECT, _MORE)

/*---------------------------------------------------------------------------
 * FTP_Delete()
 *
 *     Initiates the OBEX "Put" operation to delete an object on the remote
 *     server. The 'ObjectName' field is required.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     ObjectName - The name of the object to delete on the server.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_FAILED - Operation was not started because
 *         of an underlying transport problem.
 */
ObStatus FTP_Delete(GoepClientApp *Client, const GoepUniType *ObjectName);

/*---------------------------------------------------------------------------
 * FTP_SetFolderFwd()
 *
 *     Performs the OBEX "SetPath" operation to a new folder.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     FolderName - The name of the child folder to change to (required).
 *
 *     AllowCreate - Set to TRUE to allow the server to create the folder
 *          if it does not exist (like a CreateFolder function).
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 */
ObStatus FTP_SetFolderFwd(GoepClientApp *Client, 
                           const GoepUniType *FolderName, 
                           BOOL AllowCreate);

/*---------------------------------------------------------------------------
 * FTP_SetFolderBkup()
 *
 *     Performs the OBEX "SetPath" operation to change to the parent folder.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 */
ObStatus FTP_SetFolderBkup(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * FTP_SetFolderRoot()
 *
 *     Performs the OBEX "SetPath" operation to change to the root folder.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 */
ObStatus FTP_SetFolderRoot(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * FTP_ClientContinue()
 *
 *     This function is called by the client in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The client is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus FTP_ClientContinue(GoepClientApp *Client);
#define FTP_ClientContinue(_CLIENT) GOEP_ClientContinue(_CLIENT)

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_ClientVerifyAuthResponse()
 *
 *     This function is used by the Client to verify an authentication
 *     response received from the server. The client should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Password - The password that the server was expected to provide. This
 *         value is used in the MD5 verification digest calculation.
 *
 *     PasswordLen - The length, in bytes of the password.
 *
 * Returns:
 *     TRUE - Verification succeeded, the digest provided by the server
 *           matched the one generated locally by the client.
 *
 *     FALSE - Verification failed, the server is not authenticated.
 */
BOOL FTP_ClientVerifyAuthResponse(GoepClientApp *Client, 
								  U8 *Password, U8 PasswordLen);
#define FTP_ClientVerifyAuthResponse(_CLIENT, _PASS, _LEN) \
	GOEP_ClientVerifyAuthResponse(_CLIENT, _PASS, _LEN)
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* FTP_EXPANDED_API == XA_ENABLED */

/****************************************************************************
 *
 * Internal Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * FtpExtensionData structure
 *
 *     This structure is used internally by the GOEP Client & Server layers.
 */
typedef struct _FtpContext {
    BOOL                initialized;                    

#if FTP_EXPANDED_API == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED
    /* OBEX Server connection registration structure */
    ObexConnection      obexConn;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
    /* Holds clients OBEX Connect request while transport is connecting. */
    GoepConnectReq      connect;
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* FTP_EXPANDED_API == XA_ENABLED */

    /* ObStore Entries and List for ftp client */
    FtpcObStoreEntry     obcEntries[BT_FTP_MAX_OBS_NUMBER];
    ListEntry       obcList;
    /* ObStore Entries and List for ftp server */
//    FtpsObStoreEntry     obsEntries[OBS_MAX_NUM_ENTRIES];
//    ListEntry       obsList;

    GoepClientCallback  adpclientCb;
    GoepServerCallback  adpserverCb;
#ifdef GOEP_TEST_SRM
    U8              clientWaitCount; // client to send SRM wait test
    U8              serverWaitCount; // server to send SRM wait test
#endif

    /* ftps temporary data */
    U8 ftps_filepath[FTP_MAX_FILEPATH]; /* OS dependented filepath */
    U16 ftps_filepath_len;
} FtpContext;

/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
extern FtpContext gFtp;
#define FTP(f) (gFtp.f)

extern const unsigned char OBEX_FTP_UUID[];

/* start ftp handler prototype */
void ftps_register_server_req_hdlr(ftps_register_server_req_struct* register_server_req);
void ftps_deregister_server_req_hdlr(ftps_deregister_server_req_struct* deregister_server_req);
void ftps_send_authorize_ind_msg(U8 goep_conn_id, goep_bd_addr_struct* bd_addr_p, U8* dev_name);
void ftps_connect_rsp_hdlr(ftps_connect_rsp_struct* ftp_connect_rsp);
void ftps_authorize_rsp_hdlr(ftps_authorize_rsp_struct* ftp_authorize_rsp );
void ftps_auth_rsp_hdlr(ftps_auth_rsp_struct* ftp_auth_rsp);
void ftps_push_rsp_hdlr(ftps_push_rsp_struct* ftp_push_rsp );
void ftps_pull_rsp_hdlr(ftps_pull_rsp_struct* ftp_pull_rsp);
void ftps_set_folder_rsp_hdlr(ftps_set_folder_rsp_struct* ftp_set_folder_rsp);
void ftps_abort_rsp_hdlr(ftps_abort_rsp_struct* ftp_abort_rsp);

void ftps_send_register_cnf_msg(U32 cnf_msg_id, U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len, U8 req_id);
void ftps_send_action_ind_msg(U8 goep_conn_id, U8 action, U8 permission, 
                            U8* folder_name, U16 folder_name_len, U8* dst_name, U16 dst_name_len, U8* uuid_p, U8 uuid_len);
void ftps_send_pull_ind_msg(U8 goep_conn_id, goep_pkt_type_enum pkt_type,
                            U8* obj_mime_type, U8* obj_name, U16 obj_name_len,
                            U8* uuid_p, U8 uuid_len);
void ftps_send_push_ind_msg(U8 goep_conn_id, goep_pkt_type_enum pkt_type, 
                            goep_put_type_enum put_type, U32 total_obj_len,
                            U8* obj_name, U16 obj_name_len, 
                            U8* obj_mime_type, U16 obj_mime_type_len, 
                            U8* frag_ptr, U16 frag_len,
                            U8* uuid_p, U8 uuid_len);
void ftps_send_pushfile_ind_msg(U8 goep_conn_id, U32 total_obj_len,
                            U8* obj_name, U16 obj_name_len, 
                            U8* obj_mime_type, U16 obj_mime_type_len, 
                            U8 srm_enable);
void ftps_send_pullfile_ind_msg(U8 goep_conn_id,
                            U8* obj_mime_type, U8* obj_name, U16 obj_name_len,
                            U8 srm_enable);

void ftpc_connect_req_hdlr(ftpc_connect_req_struct* ftpc_connect_req);
void ftpc_auth_req_hdlr(ftps_auth_req_struct* ftp_auth_req );
void ftpc_push_req_hdlr(ftpc_push_req_struct* ftpc_push_req );
void ftpc_pull_req_hdlr(ftpc_pull_req_struct* ftp_pull_req) ;
void ftpc_set_folder_req_hdlr(ftpc_set_folder_req_struct* ftp_set_folder_req);
void ftpc_action_req_hdlr(ftpc_action_req_struct* ftpc_action_req);
void ftpc_abort_req_hdlr(ftpc_abort_req_struct* ftp_abort_req );
void ftpc_disconnect_req_hdlr(ftpc_disconnect_req_struct* ftp_disconnect_req);
void ftpc_TpDisconnect_req_hdlr(ftpc_disconnect_req_struct* ftp_disconnect_req);

void ftpc_send_pushfile_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);
void ftpc_send_pullfile_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason,
                            U8* obj_mime_type, U32 total_obj_len, U8* uuid_p, U8 uuid_len);
void ftps_send_file_progress_ind_msg(U8 goep_conn_id, U8 operation, U32 curr_length, U32 total_length);
void ftpc_send_file_progress_ind_msg(U8 goep_conn_id, U8 operation, U32 curr_length, U32 total_length);
/* end of ftp handler prototype */



#endif
#endif

