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
 * File:        bip.c
 *     $Workfile:bip.c$ for XTNDAccess Blue SDK, Version 2.x
 *     $Revision: #1 $
 *
 * Description: This file contains code for the BIP profile interface. 
 *
 * Created:     August 6, 2002
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2002-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Jun 9 2009 mtk80029
 * [MAUI_01866671] [BT] OBEX, GOEP,PBAP,BIP,HFP: replace "Report" trace with kal_trace
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_01091714] [BT BIP] server show connect confirm when RFCOMM Open Indication
 * notify server when  RFCOMM Open Ind to let user confirm:
 *
 * May 15 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * Server send Partial Content rsp code for partial content
 *
 * May 6 2008 mbj06032
 * [MAUI_01035146] [BT][1]Assert Fail: 0 bt_adp_bip.c 880 -BT.
 * Add BIP_TpDisconnect API to avoid calling GOEP_TpDisconnect two times continually
 *
 * Apr 29 2008 mbj06032
 * [MAUI_00678106] [BT BIP] fix code error
 * 
 *
 * Apr 25 2008 mbj06032
 * [MAUI_00669575] [BT BIP]Revise BIP code and modify trace info
 * 
 *
 * Apr 11 2008 mbj06032
 * [MAUI_00653579] [BT BIP ADP] add $Log: $ to bip adp files
 * 
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 *
 ****************************************************************************/
/* Keep this header file outside the compile option to avoid compile warning */
//#include "kal_release.h"      	/* Basic data type */

#ifdef __BT_BIP_PROFILE__

#include "osapi.h"

#include "sys/goepext.h"
#include "bipi.h" //"sys/bipi.h"
#include "bt_osal.h"

#if BT_STACK == XA_ENABLED
#include "me.h"
#include "bttypes.h"
#endif /* BT_STACK == XA_ENABLED */

#include "bluetooth_trc.h"

/*
 * The BIP layer relies on the UTF-8 to UTF-16 conversions that GOEP provides.
 * Therefore this feature must be enabled when using BIP.
 */
 /* att
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
#error "GOEP_DOES_UNICODE_CONVERSIONS Must be enabled."
#endif
 */
 /****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Object Store Function call tables
 *
 ***************************************************************************/
#if BIP_NUM_INITIATORS > 0
static const ObStoreFuncTable InitiatorStoreFuncTable = {
    BIPOBS_Read,
    BIPOBS_Write,
    BIPOBS_GetObjectLen,
};
#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
static const ObStoreFuncTable ResponderStoreFuncTable = {
    BIPOBS_Read,
    BIPOBS_Write,
    BIPOBS_GetObjectLen,
};
#endif /* BIP_NUM_RESPONDERS */

static const U8 BipImagePushUUID[] = {
    0xE3, 0x3D, 0x95, 0x45, 0x83, 0x74, 0x4A, 0xD7,
    0x9E, 0xC5, 0xC1, 0x6B, 0xE3, 0x1E, 0xDE, 0x8E
};

static const U8 BipImagePullUUID[] = {
    0x8E, 0xE9, 0xB3, 0xD0, 0x46, 0x08, 0x11, 0xD5,
    0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E
};

static const U8 BipRemoteCameraUUID[] = {
    0x94, 0x7E, 0x74, 0x20, 0x46, 0x08, 0x11, 0xD5,
    0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E
};

#if 0 /* Remove SDP related code */
#if BT_STACK == XA_ENABLED
/****************************************************************************
 *
 * SDP objects registered by Basic Imaging Responders.
 *
 ****************************************************************************/

static const U8 BipServClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(3),          /* Data Element Sequence, UUIDs*3 */
    /* This is an element list of type UUID */
    SDP_UUID_16BIT(SC_IMAGING_RESPONDER), /* BIP Imaging Responder UUID */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by the BIP Service.
 * 
 * Value of the protocol descriptor list for the BIP Service.
 * This structure is a ROM'able representation of the RAM structure.
 * During BIP_InitResponder this structure is copied 
 * into the RAM structure and used to register the printer. A RAM 
 * structure must be used to be able to dynamically set certain values.
 */
static const U8 BipProtoDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(17),     /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),     /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
        SDP_UINT_8BIT(0x00),        /* Uint8 Job Channel on RFCOMM (varies)     */

    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};

/*---------------------------------------------------------------------------
 * Bluetooth Profile Descriptor List.
 */
static const U8 BipBtProfDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),      /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES for BIP, 6 bytes              */
    SDP_UUID_16BIT(SC_IMAGING),     /* Basic Imaging UUID in Big Endian  */
    SDP_UINT_16BIT(0x0100)          /* Uint16 version number             */
};

/*---------------------------------------------------------------------------
 * Basic Imaging service name.
 */
static const U8 BipServServiceName[] = {
    SDP_TEXT_8BIT(8),          /* Null terminated text string, 8 bytes */
    'I','m','a','g','i','n','g','\0'
};

/*---------------------------------------------------------------------------
 * BIP supported capabilities bit mask.
 */
static const U8 BipSupCapabilities[] = {
    SDP_UINT_8BIT(BIP_SUPPORTED_CAPABILITIES)
};

/*---------------------------------------------------------------------------
 * BIP supported features bit mask.
 */
static const U8 BipSupFeatures[] = {
    SDP_UINT_16BIT(BIP_SUPPORTED_FEATURES)
};

/*---------------------------------------------------------------------------
 * BIP supported functions.
 */
static const U8 BipSupFunctions[] = {
    SDP_UINT_32BIT(BIP_SUPPORTED_FUNCTIONS)
};

/*---------------------------------------------------------------------------
 * BIP Total Imaging Data Capacity.
 */
static const U8 BipImagingDataCapacity[] = {
    SDP_UINT_64BIT(BIP_IMAGING_DATA_CAPACITY)
};

/*---------------------------------------------------------------------------
 * BIP Public Browse Group.
 */
static const U8 BipBrowseGroup[] = {
    SDP_ATTRIB_HEADER_8BIT(3),          /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP),  /* Public Browse Group */
};

/*---------------------------------------------------------------------------
 * BIP Basic Imaging attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * BIP Responder SDP record. The number of elements of this array is defined
 * by BIP_NUM_ATTRIBUTES.
 */
const SdpAttribute BipSdpAttributes[] = {
    /* BIP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, BipServClassId), 
    /* BIP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, BipProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, BipBrowseGroup), 
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, BipBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), BipServServiceName),
    /* Supported Capabilities */
    SDP_ATTRIBUTE(AID_SUPPORTED_CAPABILITIES, BipSupCapabilities),
    /* Supported features */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, BipSupFeatures),
    /* Supported functions */
    SDP_ATTRIBUTE(AID_SUPPORTED_FUNCTIONS, BipSupFunctions),
    /* Total image data capacity */
    SDP_ATTRIBUTE(AID_TOTAL_IMAGE_DATA_CAPACITY, BipImagingDataCapacity)
};

/****************************************************************************
 *
 * SDP objects used to query Basic Imaging Responders.
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 *
 * SDP query info
 *
 * Service search attribute request for Basic Imaging.
 * The service search pattern is very specific. It contains the UUIDs
 * for BIP Imaging Responder, OBEX, L2CAP, and RFCOMM.
 */
const U8 BipServiceSearchAttribReq[] = {
    /* First parameter is the search pattern in data element format. It
     * is a list of 4 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(12),         /* Data Element Sequence, 12 bytes */
    /* The first UUID is BIP Responder */
    SDP_UUID_16BIT(SC_IMAGING_RESPONDER), /* Basic Imaging Responder UUID */
    /* The second UUID is L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP),         /* L2CAP UUID in Big Endian */
    /* The third UUID is RFCOMM */
    SDP_UUID_16BIT(PROT_RFCOMM),        /* UUID for RFCOMM in Big Endian */
    /* The fourth UUID is OBEX */
    SDP_UUID_16BIT(PROT_OBEX),          /* UUID for OBEX in Big Endian */

    /* The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x02, 0xA0,                 /* Max number of bytes for attribute is 672 */

    /* The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST)  /* Prot descriptor list ID */
};

#endif /* BT_STACK == XA_ENABLED */
#endif /* Remove SDP related code */

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#if XA_CONTEXT_PTR == XA_ENABLED
static BipContext      tempBip;
BipContext          *bip = &tempBip;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
BipContext           bip;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/****************************************************************************
 *
 * Shared Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            BIP_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the Object Store.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
BOOL BIP_Init(void)
{
    BOOL status = TRUE;

    OS_LockStack();
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)bip, 0, sizeof(BipContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&bip, 0, sizeof(BipContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
    
    /* Initialize the Object Store Component */
    if (BIPOBS_Init() == FALSE) {
        //Report(("BIP: Object Store Initialization Failed!\n"));
        status = FALSE;
        goto done;
    }

    BIP(initialized) = BIP_CONTEXT_INIT;

done:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the Basic Imaging Profile.
 *
 * Return:    void
 *
 */
void BIP_Deinit(void)
{
    U8          i;
    ObStatus    status;

    OS_LockStack();
#if BIP_NUM_INITIATORS > 0
    /* Go through the BIP sender array looking for clients. */
    for (i = 0; i < BIP_NUM_INITIATORS; i++) {
        if (BIP(initiator)[i]) {

#if OBEX_DEINIT_FUNCS == XA_ENABLED
            /* Deinitialize Initiator primary */
            status = GOEP_DeregisterClient(&BIP(initiator)[i]->primary.cApp);
            if (status != OB_STATUS_SUCCESS) {
                kal_trace(BT_TRACE_G6_OBEX,BT_BIP_DEINIT_CLIENT_FAILED,status);
            }
#endif /* OBEX_DEINITE_FUNCS == XA_ENABLED */
        }
    }
#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
    /* Go through the BIP printer array looking for servers. */
    for (i = 0; i < BIP_NUM_RESPONDERS; i++) {
        if (BIP(responder)[i]) {

#if OBEX_DEINIT_FUNCS == XA_ENABLED
            /* Deinitialize Responder Job */
            status = GOEP_DeregisterServer(&BIP(responder)[i]->primary.sApp);
            if (status != OB_STATUS_SUCCESS) {
                kal_trace(BT_TRACE_G6_OBEX,BT_BIP_DEINIT_SERVER_FAILED,status);
				
            }
#endif /* OBEX_DEINITE_FUNCS == XA_ENABLED */

#if 0 /* Remove SDP related code */
#if BT_STACK == XA_ENABLED
            status = SDP_RemoveRecord(&BIP(responder)[i]->responderRecord);
            Assert(status == BT_STATUS_SUCCESS);
            BIP(responder)[i]->primary.state = BIPCHS_UNKNOWN;
#endif /* BT_STACK == XA_ENABLED */
#endif /* Remove SDP related code */
        }
    }
#endif /* BIP_NUM_RESPONDERS */

    BIPOBS_Deinit();
    BIP(initialized) = 0;

    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            BIP_ClientContinue
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle a server Flow Control Event.
 *
 * Return:    void
 *
 */
void BIP_ClientContinue(BipObexClient *client)
{
    OS_LockStack();
    /* Indicate ready to receive more data */
    GOEP_ClientContinue(&client->cApp);
    OS_UnlockStack();
}


/*---------------------------------------------------------------------------
 *            BIP_ClientAccept
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept a client Operation.
 *
 * Return:    void
 *
 */
void BIP_ClientAccept(BipObexClient *client, BipData *data)
{
    BipObStoreHandle    obsh = (BipObStoreHandle)client->pushPull.object;

    OS_LockStack();

    switch (client->data->bipOp) {
    case BIPOP_GET_CAPABILITIES:
    case BIPOP_GET_IMAGES_LIST:
    case BIPOP_GET_IMAGE_PROPERTIES:
        if (data == NULL) {
            /* Client didn't accept, abort */
            GOEP_ClientAbort(&client->cApp);
        }
        else {
            /* Response was accepted, set up object for receive */
            client->data = data;
        }
        break;

    case BIPOP_GET_IMAGE:
    case BIPOP_GET_LINKED_THUMBNAIL:
    case BIPOP_GET_MONITORING_IMAGE:
        obsh->ocx = data->ocx;
        break;

    default:
        break;
    }

    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            BIP_ServerContinue
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle a server Flow Control Event.
 *
 * Return:    void
 *
 */
void BIP_ServerContinue(BipObexServer *server)
{
    OS_LockStack();
    /* Indicate ready to receive more data */
    GOEP_ServerContinue(&server->sApp);
    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            BIP_ServerAccept
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept a server Operation.
 *
 * Return:    void
 *
 */
void BIP_ServerAccept(BipObexServer *server, ObexRespCode code, BipData *data)
{
    OS_LockStack();
    if (code != OBRC_ACCEPTED && code != OBRC_PARTIAL_CONTENT) {
        GOEP_ServerAbort(&server->sApp, code);
        goto done;
    }

    switch (server->request.bipOp) {
    case BIPOP_PUT_IMAGE:
        /* Responder created an Img-Handle on PutImage. Queue it for response to
         * the Initiator.
         */
        if (data->r.rImg.imgHandle[0] == 0) {
            GOEP_ServerAbort(&server->sApp, OBRC_INTERNAL_SERVER_ERR);
        }

        /* Make sure the handle is null terminated */
        data->r.rImg.imgHandle[7] = 0;
        /* Copy the handle for transmission in the final Put Response packet */
        OS_MemCopy((U8 *)server->request.r.rImg.imgHandle, (U8 *)data->r.rImg.imgHandle, 
                    sizeof(BipImgHandle));
        /* drop through */

    case BIPOP_PUT_LINKED_THUMBNAIL:
    case BIPOP_GET_IMAGES_LIST:
        /* Set the response code */
        server->object->code = code;
        server->object->ocx = data->ocx;
        break;

    default:
        break;
    }

    /* If we didn't abort, GOEP_ServerAccept() will be called when the
     * GOEP_PROVIDE_OBJECT event occurs in the BipResponderCallback handler.
     */ 
done:
    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            BIP_ServerAuthorizeResp
 *---------------------------------------------------------------------------
 *
 * Synopsis:  server authorize result.
 *
 * Return:    void
 *
 */
void BIP_ServerAuthorizeResp(BipObexServer *server, kal_uint8 rsp_code)
{
     GOEP_ServerAuthorizeResponse(&server->sApp, rsp_code);
}

ObStatus BIP_ServerAuthenticate(BipObexServer *server, 
                                 ObexAuthResponse *response, 
                                 ObexAuthChallenge *challenge)
{
    return GOEP_ServerAuthenticate(&server->sApp, response, challenge);
}

U8 BIP_ServerVerifyAuthResponse(BipObexServer *server, U8 *Password, U8 PasswordLen)
{
    if (GOEP_ServerVerifyAuthResponse(&server->sApp, Password, PasswordLen))
        return 1;
    return 0;
}

/*---------------------------------------------------------------------------
 *            BIP_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Requests the current operation be aborted.
 *
 * Return:    void
 *
 */
void BIP_Abort(void *session, BipChannel channel, kal_uint8 abort_reason)
{
#if BIP_NUM_INITIATORS > 0
    BipObexClient  *client = 0;
#endif
#if BIP_NUM_RESPONDERS > 0
    BipObexServer  *server = 0;
#endif

    OS_LockStack();
    kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ABORT,abort_reason);

    switch (channel) {
#if BIP_NUM_INITIATORS > 0
    case BIPCH_INITIATOR_PRIMARY:
        client = &((BipInitiatorSession *)session)->primary;
        break;
#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
    case BIPCH_RESPONDER_PRIMARY:
        server = &((BipResponderSession *)session)->primary;
        break;
#endif /* BIP_NUM_RESPONDERS */

    default:
        break;
    }

    switch (channel) {
#if BIP_NUM_INITIATORS > 0
    case BIPCH_INITIATOR_PRIMARY:
        GOEP_ClientAbort(&client->cApp);
        break;
#endif

#if BIP_NUM_RESPONDERS > 0
    case BIPCH_RESPONDER_PRIMARY:
        GOEP_ServerAbort(&server->sApp, abort_reason/*OBRC_RESET_CONTENT*/);
        break;
#endif

    default:
        break;
    }

    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            BipObexClientInit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes the Obex Client.
 *
 * Return:    ObStatus
 *
 */
ObStatus BipObexClientInit(BipObexClient *client, void *callback)
{
    ObStatus status = OB_STATUS_FAILED;

    OS_MemSet((U8 *)client, 0, sizeof(BipObexClient));

    client->cApp.type = GOEP_PROFILE_BIP;
    client->cApp.callback = (GoepClientCallback)callback;
    client->cApp.connFlags = GOEP_NEW_CONN;
    client->cApp.connId = 0;
    client->state = BIPCHS_UNKNOWN;

#if BIP_NUM_INITIATORS > 0
    status = GOEP_RegisterClient(&client->cApp, &InitiatorStoreFuncTable);
#endif

    if (status != OB_STATUS_SUCCESS) {
		kal_trace(BT_TRACE_G6_OBEX,BT_BIP_REGISTERCLIENT_FAILED,status);
    }

    return status;
}

/*---------------------------------------------------------------------------
 *            BipObexServerInit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes the Obex Server.
 *
 * Return:    ObStatus
 *
 */
ObStatus BipObexServerInit(BipObexServer *server, U16 uuid)
{
    ObStatus status = OB_STATUS_FAILED;
    U8       /*freeId,*/ i;

    server->state = BIPCHS_UNKNOWN;

    if (uuid != SC_IMAGING_RESPONDER) {
        /* We only support Responder primary channel currently. */
        return status;
    }

    i = 0;
    if (BIPF_IMAGE_PUSH & BIP_SUPPORTED_FEATURES) {
        /* set up the Image Push target */
        server->targetHdr[i].targetLen = 16;
        OS_MemCopy(server->bipImgPushUuid, BipImagePushUUID, 16);
        server->targetHdr[i].target = server->bipImgPushUuid;
        server->sApp.target[i] = &server->targetHdr[i];
        i++;
    }
    
    if (BIPF_IMAGE_PULL & BIP_SUPPORTED_FEATURES) {
        /* set up the Image Pull target */
        server->targetHdr[i].targetLen = 16;
        OS_MemCopy(server->bipImgPullUuid, BipImagePullUUID, 16);
        server->targetHdr[i].target = server->bipImgPullUuid;
        server->sApp.target[i] = &server->targetHdr[i];
        i++;
    }
/*  We do not support camera feature
    if (BIPF_REMOTE_CAMERA & BIP_SUPPORTED_FEATURES) {
        server->targetHdr[i].targetLen = 16;
        OS_MemCopy(server->bipRemoteCameraUuid, BipRemoteCameraUUID, 16);
        server->targetHdr[i].target = server->bipRemoteCameraUuid;
        server->sApp.target[i] = &server->targetHdr[i];
        i++;
    }
*/
#if BIP_NUM_RESPONDERS > 0
    server->sApp.callback = BipResponderCallback;
#endif
    server->sApp.numTargets = i;
    server->sApp.type = GOEP_PROFILE_BIP;
    server->sApp.bt_service = GOEP_PROFILE_BIP;

    /*freeId = */ GOEP_GetConnectionId(GOEP_SERVER, server->sApp.type);
/*  Always use one new connection
    if (freeId != 0xFF) {
        // Multiplex this BIP OBEX server over an existing connection
        server->sApp.connFlags = GOEP_MUX_CONN;
        server->sApp.connId = freeId;
    }
    else {
*/
        /* Make a new connection for the BIP OBEX server */
        server->sApp.connFlags = GOEP_NEW_CONN;
        /* Initialize the connId - filled in later */
        server->sApp.connId = 0;
/*
    }
*/
#if BIP_NUM_RESPONDERS > 0
    status = GOEP_RegisterServer(&server->sApp, &ResponderStoreFuncTable);
#endif

    if (status != OB_STATUS_SUCCESS) {
		kal_trace(BT_TRACE_G6_OBEX,BT_BIP_REGISTERSERVER_FAILED,status);	
        return status;
    }

    return status;
}

/*---------------------------------------------------------------------------
 *            FindBipSession()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return the BIP Initiator Session given the ObexClientApp pointer.
 *
 * Return:    BipInitiatorSession* or BipResponderSession*
 */
void *FindBipSession(void *app)
{
    I8 i;

#if BIP_NUM_INITIATORS > 0
    /* Go through the BIP client array. */
    for (i = 0; i < BIP_NUM_INITIATORS; i++) {
        if (BIP(initiator)[i] && &BIP(initiator)[i]->primary.cApp == app) {
            return BIP(initiator)[i];
        }
    }
#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
    /* Go through the BIP server array. */
    for (i = 0; i < BIP_NUM_RESPONDERS; i++) {
        if (BIP(responder)[i] && &BIP(responder)[i]->primary.sApp == app) {
            return BIP(responder)[i];
        }
    }
#endif /* BIP_NUM_RESPONDERS */

    return 0;
}

#if BIP_NUM_RESPONDERS > 0
/*---------------------------------------------------------------------------
 *            BIP_RegisterResponder
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Register BIP Responder application callback.
 *
 * Return:    void
 *
 */
BtStatus BIP_RegisterResponder(BipResponderSession *responder, BipCallback callback)
{
    BtStatus    status = BT_STATUS_SUCCESS;
    U8          i, j = BIP_NUM_RESPONDERS;

#if XA_ERROR_CHECK == XA_ENABLED
    if (callback == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if (BIP(responderCallback) != 0 && BIP(responderCallback) != callback) {
        return BT_STATUS_NO_RESOURCES;
    }
#endif

    Assert(callback != 0);
    Assert(BIP(responderCallback) == 0 || BIP(responderCallback) == callback);

    OS_LockStack();

    if (BIP(initialized) != BIP_CONTEXT_INIT) {
        if (BIP_Init() == FALSE) {
            status = BT_STATUS_FAILED;
            goto error;
        }
    }

    if (BIP(responderCallback) == 0 || BIP(responderCallback) == callback) {
        BIP(responderCallback) = callback;
        /* insert the responder in an unused slot */
        for (i = BIP_NUM_RESPONDERS; i > 0; i--) {
            if (BIP(responder)[i-1] == responder) {
                status = BT_STATUS_IN_USE;
                goto error;
            }
            if (BIP(responder)[i-1] == 0) {
                j = i - 1;
            }
        }
    }
    else {
        /* invalid callback */
        status = BT_STATUS_INVALID_PARM;
        goto error;
    }

    if (j < BIP_NUM_RESPONDERS) {
        /* Init the primary channel */
        BIP(responder)[j] = responder;
        status = BIP_InitResponderChannel(responder, BIPCH_RESPONDER_PRIMARY);
        if (status != OB_STATUS_SUCCESS) {
            status = BT_STATUS_FAILED;
            goto error;
        }
        //Report(("BIP: responder Registered.\n"));
    }
    else {
        status = BT_STATUS_NO_RESOURCES;
        goto error;
    }

error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_DeregisterResponder
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregister BIP application.
 *
 * Return:    BtStatus
 *
 */
BtStatus BIP_DeregisterResponder(BipResponderSession *responder)
{
    U8 i, numActiveResponders = 0, numActiveInitiators = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

#if OBEX_DEINIT_FUNCS == XA_ENABLED
    /* Deinitialize Responder Job */
    status = GOEP_DeregisterServer(&responder->primary.sApp);
    if (status != OB_STATUS_SUCCESS) {
        kal_trace(BT_TRACE_G6_OBEX,BT_BIP_DEINIT_SERVER_FAILED,status);
        goto done;
    }
#endif /* OBEX_DEINITE_FUNCS == XA_ENABLED */
#if 0 /* Remove SDP related code */
    if (responder->primary.state > BIPCHS_UNKNOWN &&
        responder->primary.state <= BIPCHS_CANCELED) {
        status = SDP_RemoveRecord(&responder->responderRecord);
        Assert(status == BT_STATUS_SUCCESS);
        responder->primary.state = BIPCHS_UNKNOWN;
    }
#endif /* Remove SDP related code */
    for (i = 0; i < BIP_NUM_RESPONDERS; i++) {
        if (BIP(responder)[i] == responder) {
            BIP(responder)[i] = 0;
            status = BT_STATUS_SUCCESS;
        }
        if (BIP(responder)[i] != 0) {
            numActiveResponders++;
        }
    }

    if (numActiveResponders == 0) {
        BIP(responderCallback) = 0;
    }

#if BIP_NUM_INITIATORS > 0
    /* check for active initiators */
    for (i = 0; i < BIP_NUM_INITIATORS; i++) {
        if (BIP(initiator)[i] != 0) {
            numActiveInitiators++;
        }
    }
#endif /* BIP_NUM_INITIATORS */

    if (numActiveInitiators == 0 && numActiveResponders == 0) {
        BIP_Deinit();
    }
    //Report(("BIP: responder Deregistered.\n"));
    
#if OBEX_DEINIT_FUNCS == XA_ENABLED
 done:
#endif
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_InitResponderChannel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize channel structure.
 *
 * Return:    BtStatus
 *
 */
BtStatus BIP_InitResponderChannel(BipResponderSession *responder, BipChannel channel)
{
    ObStatus        status;

    OS_LockStack();

    if (BIP(initialized) != BIP_CONTEXT_INIT) {
        if (BIP_Init() == FALSE) {
            status = BT_STATUS_FAILED;
            goto done;
        }
    }

    switch (channel) {
    case BIPCH_RESPONDER_PRIMARY:
        OS_MemSet((U8 *)&responder->primary, 0, sizeof(BipObexServer));
        status = BipObexServerInit(&responder->primary, SC_IMAGING_RESPONDER);
        if (status != OB_STATUS_SUCCESS) {
            goto done;
        }
        responder->primary.channel = channel;

#if 0 /* Remove SDP related code */
        /* Add the responder records */
        Assert(sizeof(BipSdpAttributes) == sizeof(responder->responderAttrib));
        OS_MemCopy( (U8 *)&(responder->responderAttrib), 
                    (U8 *)&BipSdpAttributes, 
                    sizeof(BipSdpAttributes));

        OBEX_ServerGetSdpProtocolDescList(
            &GOES(servers)[responder->primary.sApp.connId].obs.trans.ObexServerBtTrans,
            &(responder->responderAttrib[1]), 1);
        responder->responderRecord.attribs = responder->responderAttrib;
        responder->responderRecord.num = BIP_NUM_ATTRIBUTES;
        responder->responderRecord.classOfDevice = COD_OBJECT_TRANSFER;
#endif /* Remove SDP related code */
        break;

    case BIPCH_RESPONDER_SECONDARY:
    default:
        //Report(("BIP: Responder Channel not available.\n"));
        status = BT_STATUS_FAILED;
        goto done;
        /* break; */
    }

#if 0 /* Remove SDP related code */
    /* Register with SDP */
    status = SDP_AddRecord(&(responder->responderRecord));
    if (status == BT_STATUS_SUCCESS) {
        responder->primary.state = BIPCHS_INITIALIZED;
    }
#endif /* Remove SDP related code */
done:
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_SendResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Begin a Responder Response.
 *
 * Return:    void
 *
 */
ObStatus BIP_SendResponse(BipObexServer *server, BipData *data)
{
    ObStatus    status = OB_STATUS_SUCCESS;
    /* Advance the ap pointer beyond the length byte */
    U8         *ap = server->appParms + 1;

    OS_LockStack();

    Assert(data->bipOp == server->request.bipOp);

    switch (data->bipOp) {
    case BIPOP_GET_CAPABILITIES:
        break;

    case BIPOP_GET_IMAGES_LIST:
        /* Queue the GetImagesList Application Paramaters */
        ap[0] = BIPAPTAG_NB_RETURNED_HANDLES;
        ap[1] = 2;
        StoreBE16(ap + 2, data->r.rList.nbReturnedHandles);
        if (GOEP_ServerQueueHeader(&server->sApp,
            OBEXH_APP_PARAMS, ap, 4) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }

        /* Responder created an XML-HandlesDescriptor on GetImagesList. Queue
         * it for response to the Initiator.
         */
		/* needs to recheck
        if (data->r.rList.handlesDescriptorLen == 0) {
            status = OB_STATUS_INVALID_PARM;
            goto done;
        }
		*/
        if (GOEP_ServerQueueHeader(&server->sApp, BIPH_IMG_DESCRIPTION,
            data->r.rList.handlesDescriptor,
            (U16)data->r.rList.handlesDescriptorLen) == FALSE) {
            status = OB_STATUS_FAILED;
            GOEP_ServerAbort(&server->sApp, OBRC_INTERNAL_SERVER_ERR);
            goto done;
        }
        break;

    case BIPOP_GET_IMAGE_PROPERTIES:
        break;

    case BIPOP_GET_IMAGE:
        break;

    case BIPOP_GET_LINKED_THUMBNAIL:
        break;

    case BIPOP_GET_MONITORING_IMAGE:
        /* Responder created an Img-Handle. Queue it for response to
         * the Initiator.
         */
        if (data->r.rImg.imgHandle[0] == 0) {
            GOEP_ServerAbort(&server->sApp, OBRC_INTERNAL_SERVER_ERR);
        }

        /* Queue the Image Handle header */
        if (data->r.rImg.imgHandle != 0) {
            /* Make sure the handle is null terminated */
            kal_uint8 uni_img_hdlr[16];
            int i = 0;
            /* Make sure the handle is null terminated */
            data->r.rImg.imgHandle[7] = 0;

            while (data->r.rImg.imgHandle[i] != 0)
            {
                uni_img_hdlr[2*i] = 0;
                uni_img_hdlr[2*i + 1] = data->r.rImg.imgHandle[i];
                i++;
            }
            uni_img_hdlr[2*i] = 0;
            uni_img_hdlr[2*i + 1] = 0;
        
            if (GOEP_ServerQueueHeader(&server->sApp, BIPH_IMG_HANDLE,
                (U8 *)uni_img_hdlr,(U16) (2*i+2)) == FALSE) {
                GOEP_ServerAbort(&server->sApp, OBRC_INTERNAL_SERVER_ERR);
                goto done;
                }
            }
        break;

    default:
        status = OB_STATUS_INVALID_PARM;
        goto done;
    }

    server->object->bipOp = data->bipOp;
    server->object->ocx = data->ocx;
    /* Set the length of the image we are going to send */
    BIPOBS_SetObjectLen(server->object, data->totLen);

    //Report(("BIP: Response Sent.\n"));

    goto done;
done:
    OS_UnlockStack();
    return status;
}
#endif /* BIP_NUM_RESPONDERS */

#if BIP_NUM_INITIATORS > 0
/*---------------------------------------------------------------------------
 *            BIP_RegisterInitiator
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Register BIP Initiator application callback.
 *
 * Return:    void
 *
 */
BtStatus BIP_RegisterInitiator(BipInitiatorSession *initiator, BipCallback callback)
{
    ObStatus    status = OB_STATUS_SUCCESS;
    U8          i, j = BIP_NUM_INITIATORS;

    OS_LockStack();
#if XA_ERROR_CHECK == XA_ENABLED
    if (callback == 0) {
        status = BT_STATUS_INVALID_PARM;
        goto error;
    }
    if (BIP(initiatorCallback) != 0 && BIP(initiatorCallback) != callback) {
        status = BT_STATUS_NO_RESOURCES;
        goto error;
    }
#endif

    Assert(callback != 0);
    Assert(BIP(initiatorCallback) == 0 || BIP(initiatorCallback) == callback);

    if (BIP(initialized) != BIP_CONTEXT_INIT) {
        if (BIP_Init() == FALSE) {
            status = BT_STATUS_FAILED;
            goto error;
        }
    }

    if (BIP(initiatorCallback) == 0 || BIP(initiatorCallback) == callback) {
        BIP(initiatorCallback) = callback;
        /* insert the initiator in an unused slot */
        for (i = BIP_NUM_INITIATORS; i > 0; i--) {
            if (BIP(initiator)[i-1] == initiator) {
                status = BT_STATUS_IN_USE;
                goto error;
            }
            if (BIP(initiator)[i-1] == 0) {
                j = i - 1;
            }
        }
    }
    else {
        /* invalid callback */
        status = BT_STATUS_INVALID_PARM;
        goto error;
    }

    if (j < BIP_NUM_INITIATORS) {
        /* Init the primary channel */
        BIP(initiator)[j] = initiator;
        status = BIP_InitInitiatorChannel(initiator, BIPCH_INITIATOR_PRIMARY);
        if (status != OB_STATUS_SUCCESS) {
            status = BT_STATUS_FAILED;
            goto error;
        }
        //Report(("BIP: Registered.\n"));
    }
    else {
        status = BT_STATUS_NO_RESOURCES;
        goto error;
    }

error:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_DeregisterInitiator
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregister BIP Initiator.
 *
 * Return:    BtStatus
 *
 */
BtStatus BIP_DeregisterInitiator(BipInitiatorSession *initiator)
{
    U8 i, numActiveInitiators = 0, numActiveResponders = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

#if OBEX_DEINIT_FUNCS == XA_ENABLED
    /* Deinitialize Initiator primary */
    status = GOEP_DeregisterClient(&initiator->primary.cApp);
    if (status != OB_STATUS_SUCCESS) {
            kal_trace(BT_TRACE_G6_OBEX,BT_BIP_DEINIT_CLIENT_FAILED,status);
    }
#endif /* OBEX_DEINITE_FUNCS == XA_ENABLED */

    for (i = 0; i < BIP_NUM_INITIATORS; i++) {
        if (BIP(initiator)[i] == initiator) {
            BIP(initiator)[i] = 0;
            status = BT_STATUS_SUCCESS;
        }
        if (BIP(initiator)[i] != 0) {
            numActiveInitiators++;
        }
    }

    if (numActiveInitiators == 0) {
        BIP(initiatorCallback) = 0;
    }

#if BIP_NUM_RESPONDERS > 0
    /* check for active responders */
    for (i = 0; i < BIP_NUM_RESPONDERS; i++) {
        if (BIP(responder)[i] != 0) {
            numActiveResponders++;
        }
    }
#endif /* BIP_NUM_RESPONDERS */

    if (numActiveInitiators == 0 && numActiveResponders == 0) {
        BIP_Deinit();
    }
    //Report(("BIP: Deregistered.\n"));

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_InitInitiatorChannel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize channel structure.
 *
 * Return:    BtStatus
 *
 */
BtStatus BIP_InitInitiatorChannel(BipInitiatorSession *initiator, BipChannel channel)
{
    ObStatus    status;

    OS_LockStack();

    if (BIP(initialized) != BIP_CONTEXT_INIT) {
        if (BIP_Init() == FALSE) {
            status = BT_STATUS_FAILED;
            goto done;
        }
    }

    switch (channel) {
    case BIPCH_INITIATOR_PRIMARY:
        status = BipObexClientInit(&initiator->primary, (void *) &BipInitiatorCallback);
        initiator->primary.channel = channel;
        break;

    default:
        //Report(("BIP: Initiator Channel not available.\n"));
        status = BT_STATUS_FAILED;
        goto done;
        /*break;*/
    }

done:
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_Connect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Connect a Initiator channel.
 *
 * Return:    void
 *
 */
ObStatus BIP_Connect(BipInitiatorSession *initiator, ObexTpAddr *addr, BipService bipService)
{
    ObStatus        status;
    BipObexClient  *client;
    GoepClientEvent event;
//    U16             bipTarget = 0;

    OS_LockStack();

    switch (bipService) {
/*
    case BIPSVC_RESPONDER:
        client = &initiator->primary;
        OS_MemCopy(client->bipUuid, BipImagePushUUID, 16);
        bipTarget = SC_IMAGING_RESPONDER;
        break;
*/
    case BIPSVC_IMAGE_PUSH:
        client = &initiator->primary;
        OS_MemCopy(client->bipUuid, BipImagePushUUID, 16);
        addr->proto.bt.uuid = SC_IMAGING_RESPONDER;
        break;
    case BIPSVC_IMAGE_PULL:
        client = &initiator->primary;
        OS_MemCopy(client->bipUuid, BipImagePullUUID, 16);
        addr->proto.bt.uuid = SC_IMAGING_RESPONDER;
        break;

    case BIPSVC_REMOTE_CAMERA:
        client = &initiator->primary;
        OS_MemCopy(client->bipUuid, BipRemoteCameraUUID, 16);
        addr->proto.bt.uuid = SC_IMAGING_RESPONDER;
        break;

    case BIPSVC_AUTO_ARCHIVE:
    case BIPSVC_REFERENCED_OBJECTS:
    default:
        addr->proto.bt.uuid = 0;
        status = OB_STATUS_INVALID_PARM;
        goto done;
        /* break; */
    }

    /* Set up connection target */
    client->connect.target = client->bipUuid;
    client->connect.targetLen = 16;

#if 0 /* Remove SDP related code */
    if (addr->type == OBEX_TP_BLUETOOTH) {

        OS_MemCopy(initiator->srchAttr,
                   BipServiceSearchAttribReq, 
                   BIP_NUM_SEARCH_ATTRIBUTES);

        StoreBE16(&(initiator->srchAttr[3]), bipTarget);

        addr->proto.bt.sdpQuery = initiator->srchAttr;
        addr->proto.bt.sdpQueryLen = BIP_NUM_SEARCH_ATTRIBUTES;
        addr->proto.bt.sdpQueryType = BSQT_SERVICE_SEARCH_ATTRIB_REQ;
    }
#endif /* Remove SDP related code */

    /* Connect to Responder */
    client->data = &client->tData;
    
    if (client->state == BIPCHS_AUTHENTICATING)
    {
        client->state = BIPCHS_CONNECTING;
        status = GOEP_Connect(&client->cApp, &client->connect);        
        return status;
    }
    else
    {
        client->state = BIPCHS_CONNECTING;
        client->b_TpDisconnect_called = KAL_FALSE;    
        
        status = GOEP_TpConnect(&client->cApp, addr);
        if (status == OB_STATUS_SUCCESS)
        {
            event.event = GOEP_EVENT_TP_CONNECTED;
            client->cApp.callback(&event);
        }
    }
    //Report(("[BIP] Transport layer connect returned %d.\n", status));

done:
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect an Initiator.
 *
 * Return:    void
 *
 */
ObStatus BIP_Disconnect(BipInitiatorSession *initiator)
{
    ObStatus            status;

    OS_LockStack();
    status = GOEP_Disconnect(&initiator->primary.cApp);

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_SendRequest
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Begin a Initiator XML Request.
 *
 * Return:    void
 *
 */
ObStatus BIP_SendRequest(BipInitiatorSession *initiator, BipData *data)
{
    ObStatus        status;
    U8             *descriptor, typeHdr[30];
    U16             descriptorLen = 0;
    BipImgHandlePtr h;
    BipObexClient  *client = &initiator->primary;
    /* Advance the ap pointer beyond the length byte */
    U8             *ap = client->appParms + 1;

    OS_LockStack();

    if (client->state < BIPCHS_CONNECTED) {
        status = OB_STATUS_NO_CONNECT;
        goto done;
    }

    /* Initialize the name to null */
    client->pushPull.name = 0;

    /* Set the channel and check availability */
    switch (data->bipOp) {
    case BIPOP_GET_CAPABILITIES:
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_CAPABILITIES, 22);
        break;

    case BIPOP_GET_IMAGES_LIST:
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_LISTING, 17);
        /* Queue the GetImagesList Application Paramaters */
        ap[0] = BIPAPTAG_NB_RETURNED_HANDLES;
        ap[1] = 2;
        StoreBE16(ap + 2, data->r.rList.nbReturnedHandles);
        ap[4] = BIPAPTAG_LIST_START_OFFSET;
        ap[5] = 2;
        StoreBE16(ap + 6, data->r.rList.listStartOffset);
        ap[8] = BIPAPTAG_LATEST_CAPTURED_IMAGES;
        ap[9] = 1;
        ap[10] = data->r.rList.latestCapturedImages;
        if (GOEP_ClientQueueHeader(&client->cApp,
            OBEXH_APP_PARAMS, ap, 11) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }
        break;

    case BIPOP_GET_IMAGE_PROPERTIES:
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_PROPERTIES, 20);
        break;

    case BIPOP_PUT_IMAGE:
/* create empty
        if (data->totLen == 0) {
            status = OB_STATUS_INVALID_PARM;
            goto done;
        }
*/
        /* Manually build the Type and Name headers, in that order. 
         * This is not the default order that GOEP will build them.
         */
        if (GOEP_ClientQueueHeader(&client->cApp,
            OBEXH_TYPE, (const U8*)BIPTYPESTR_IMG, 13) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }
        if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_NAME, 
            (U8 *)data->r.rImg.name, (U16)((btmtk_wstrlen((kal_wchar *)data->r.rImg.name)+1)*2)) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }
        
        break;

    case BIPOP_PUT_LINKED_THUMBNAIL:
/* create empty
        if (data->totLen == 0) {
            status = OB_STATUS_INVALID_PARM;
            goto done;
        }
*/
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_THM, 13);
        break;

    case BIPOP_GET_IMAGE:
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_IMG, 13);
        break;

    case BIPOP_GET_LINKED_THUMBNAIL:
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_THM, 13);
        break;

    case BIPOP_GET_MONITORING_IMAGE:
        OS_MemCopy(typeHdr, (const U8*)BIPTYPESTR_MONITORING, sizeof(BIPTYPESTR_MONITORING));
        /* Queue the GetMonitoringImage Application Paramaters */
        ap[0] = BIPAPTAG_STORE_FLAG;
        ap[1] = 1;
        ap[2] = data->r.rMon.storeFlag;
        if (GOEP_ClientQueueHeader(&client->cApp,
            OBEXH_APP_PARAMS, ap, 3) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }
        break;

    default:
        status = OB_STATUS_INVALID_PARM;
        goto done;
    }

    /* Queue the Img-Handle header */
    switch (data->bipOp) {
    case BIPOP_GET_IMAGE_PROPERTIES:
        h = data->r.rProp.imgHandle;
        break;

    case BIPOP_PUT_LINKED_THUMBNAIL:
    case BIPOP_GET_LINKED_THUMBNAIL:
        h = data->r.rThm.imgHandle;
        break;

    case BIPOP_GET_IMAGE:
        h = data->r.rImg.imgHandle;
        break;

    default:
        h = 0;
        break;
    }

    if (h != 0) {
        /* Make sure the handle is null terminated */
        kal_uint8 uni_img_hdlr[16];
        int i = 0;
        h[7] = 0;

        while (h[i] != 0)
        {
            uni_img_hdlr[2*i] = 0;
            uni_img_hdlr[2*i + 1] = h[i];
            i++;
        }
        uni_img_hdlr[2*i] = 0;
        uni_img_hdlr[2*i + 1] = 0;
        
        if (GOEP_ClientQueueHeader(&client->cApp, BIPH_IMG_HANDLE,
            (U8 *)uni_img_hdlr, (U16)(2*i+2)) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }
    }

    /* Queue the Img-Description header */
    switch (data->bipOp) {
    case BIPOP_GET_IMAGES_LIST:
        /* Initiator created a HandlesDescriptor header. Queue
         * it for the request from the Responder.
         */
        descriptor = data->r.rList.handlesDescriptor;
        descriptorLen = data->r.rList.handlesDescriptorLen;
        break;

    case BIPOP_GET_IMAGE:
    case BIPOP_PUT_IMAGE:
        /* Initiator created an ImageDescriptor header. Queue
         * it for the request from the Responder.
         */
        descriptor = data->r.rImg.imageDescriptor;
        descriptorLen = data->r.rImg.imageDescriptorLen;
        break;

    default:
        descriptor = 0;
        break;
    }

    if (descriptor != 0) {
        /* Zero-length headers are OK. */
        if (GOEP_ClientQueueHeader(&client->cApp, BIPH_IMG_DESCRIPTION,
            descriptor, descriptorLen) == FALSE) {
            status = OB_STATUS_FAILED;
            goto done;
        }
    }

    /* Set the type header */
    if (data->bipOp == BIPOP_PUT_IMAGE) {
        /* PutImage manually creates the Type header */
        client->pushPull.type = 0;
    } else {
        /* GOEP builds the Type header */
        client->pushPull.type = typeHdr;
    }

    client->data = data;
    if (BipBuildReq(client) == OB_STATUS_FAILED) {
        //Report(("[BIP]Initiator: Could not start request.\n"));
        status = OB_STATUS_INVALID_PARM;
        goto done;
    }

    switch (data->bipOp) {
    case BIPOP_GET_CAPABILITIES:
    case BIPOP_GET_IMAGES_LIST:
    case BIPOP_GET_IMAGE_PROPERTIES:
    case BIPOP_GET_IMAGE:
    case BIPOP_GET_LINKED_THUMBNAIL:
    case BIPOP_GET_MONITORING_IMAGE:
        status = GOEP_Pull(&client->cApp, &client->pushPull, FALSE);
        break;

    case BIPOP_PUT_IMAGE:
    case BIPOP_PUT_LINKED_THUMBNAIL:
        status = GOEP_Push(&client->cApp, &client->pushPull);
        break;

    default:
        status = OB_STATUS_INVALID_PARM;
        goto done;
    }


    //Report(("BIP: Request Sent.\n"));

done:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BIP_TpDisconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect an Initiator.
 *
 * Return:    void
 *
 */
ObStatus BIP_TpDisconnect(BipInitiatorSession *initiator)
{
    ObStatus            status;
    BipObexClient  *client = &initiator->primary;

    OS_LockStack();

	if (client->b_TpDisconnect_called == KAL_TRUE)
	{
	    status = OB_STATUS_PENDING;
	}
	else
	{
        status = GOEP_TpDisconnect(&initiator->primary.cApp);
        client->b_TpDisconnect_called = KAL_TRUE;
	}

    OS_UnlockStack();
    return status;
}

/*
 * 
 */
ObStatus BIP_ClientAuthenticate(BipObexClient *client, 
                                 ObexAuthResponse *response, 
                                 ObexAuthChallenge *challenge)
{
    if (!response && !challenge)
        return OB_STATUS_FAILED;

    /* now initiator only support OBEX authenctication in CONNECT!! */
    client->connect.challenge = challenge;
    client->connect.response = response;

    return OB_STATUS_SUCCESS;
}

U8 BIP_ClientVerifyAuthResponse(BipObexClient *client, U8 *Password, U8 PasswordLen)
{
    if (GOEP_ClientVerifyAuthResponse(&client->cApp, Password, PasswordLen))
        return 1;
    return 0;
}
#endif /* BIP_NUM_INITIATORS */

#endif /*__BT_BIP_PROFILE__*/

