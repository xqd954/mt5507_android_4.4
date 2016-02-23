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
 * File:        bip_sm.c
 *     $Workfile:bip_sm.c$ for XTNDAccess Blue SDK, Version 2.x
 *     $Revision: #1 $
 *
 * Description: This file contains code for the BIP 
 *              application. 
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
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Sep 19 2009 mtk80029
 * [MAUI_01898776] Revise code according to Klocword tool review result
 * 
 *
 * Sep 9 2009 mtk80029
 * [MAUI_01896097] [BT] GOEP notify CONNECT TARGET to application and BIP record the TARGET header
 * 
 *
 * Sep 2 2009 mtk80029
 * [MAUI_01596143] Loading displays in file manager when Cancel sending.
 * Responder: check the abort reason
 *
 * Jul 31 2009 mtk80029
 * [MAUI_01884483] [BT BIP] Fix a slip in BIP profile code
 * 
 *
 * Jun 9 2009 mtk80029
 * [MAUI_01866671] [BT] OBEX, GOEP,PBAP,BIP,HFP: replace "Report" trace with kal_trace
 * 
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106048] [BT BIP] send pushed file detail info to MMI
 * check server->object is NULL or not before use it
 *
 * Jul 18 2008 mbj06032
 * [MAUI_01091714] [BT BIP] server show connect confirm when RFCOMM Open Indication
 * notify server when  RFCOMM Open Ind to let user confirm:
 *
 * May 16 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * revise code
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
// #include "kal_release.h"      	/* Basic data type */

#ifdef __BT_BIP_PROFILE__
 
#include "osapi.h"
#include "bipi.h" //"sys/bipi.h"
#include "bt_osal.h"

#include "bluetooth_trc.h"

/*---------------------------------------------------------------------------
 *            BipAppCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Call the application with the specified event.
 *
 * Return:    
 */
void BipAppCallBack(BipCallbackParms *parms, U16 status, BipEvent event)
{
    BipCallback     callback = 0;       /* Application callback function */

    parms->status = status;
    parms->event = event;
    parms->channel = parms->obex.client->channel;

    switch (parms->channel) {
#if BIP_NUM_INITIATORS > 0
    case BIPCH_INITIATOR_PRIMARY:
    case BIPCH_INITIATOR_SECONDARY:
    callback = BIP(initiatorCallback);
        break;
#endif

#if BIP_NUM_RESPONDERS > 0
    case BIPCH_RESPONDER_PRIMARY:
    case BIPCH_RESPONDER_SECONDARY:
    callback = BIP(responderCallback);
        break;
#endif

    default:
        break;
    }

    /* Copy the channel data pointer into parms */
    switch (parms->channel) {
    case BIPCH_INITIATOR_PRIMARY:
    case BIPCH_RESPONDER_SECONDARY:
        parms->data = parms->obex.client->data;
        break;

    case BIPCH_INITIATOR_SECONDARY:
    case BIPCH_RESPONDER_PRIMARY:
        parms->data = &parms->obex.server->request;
        break;

    default:
        break;
    }
    Assert(callback);
    callback(parms);
}

#if BIP_NUM_RESPONDERS > 0
static BOOL BipResponderOp(GoepServerEvent *event, BipCallbackParms *parms);

/*---------------------------------------------------------------------------
 *            FindBipResponder()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return the BIP Responder channel given the ObexServerApp pointer.
 *
 * Return:    BipObexClient* or BipObexServer*
 */
void *FindBipResponder(void *app)
{
    I8 i;

    /* Go through the BIP server array. */
    for (i = 0; i < BIP_NUM_RESPONDERS; i++) {
        if (BIP(responder)[i] && &BIP(responder)[i]->primary.sApp == app) {
            return &(BIP(responder)[i]->primary);
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------
 *            BipResponderCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes Responder events.
 *
 * Return:    void
 *
 */
void BipResponderCallback(GoepServerEvent *event)
{
    ObexRespCode        rcode;
    ObStatus            status;
    BipObexServer      *server;
    BipCallbackParms    parms;
    U8                 *p;
    BipImgHandlePtr     ih = NULL;

    parms.obex.server = FindBipResponder(event->handler);
    if (parms.obex.server == 0) {
		kal_trace(BT_TRACE_G6_OBEX,BT_BIP_RESPONDER_NOT_FOUND);
        return;
    }
    server = parms.obex.server;

    switch (event->event) {
    case GOEP_EVENT_TP_CONNECTED:
        //Report(("[BIP ADP] Server: Transport Connection has come up.\n"));
        break;

    case GOEP_EVENT_TP_DISCONNECTED:
        //Report(("[BIP ADP] Server: Transport Connection has been disconnected.\n"));

        if (server->object) {
            /* close any objects associated with the aborted operation */
            BIPOBS_Close(&server->object);
        }
        server->request.bipOp = BIPOP_CONNECT;
        BipAppCallBack(&parms, OB_STATUS_DISCONNECT, BIPRESPONDER_COMPLETE);

        server->request.offset = server->request.totLen = 0;
        break;

    case GOEP_EVENT_ABORTED:
		/*
        since OBEX do not updated now...
        if (event->abortReason != OBRC_LINK_DISCONNECT)
        */
		{
            /* Check if thumbnail is requested by Partial Content rspcode */		
        if ((event->oper == GOEP_OPER_PUSH) &&
			(server->object) && 
            (server->object->bipOp == BIPOP_PUT_IMAGE) &&
            (server->object->code == OBRC_PARTIAL_CONTENT)) {
            /* Handle the Partial Content special case on PutImage */
            BIPOBS_Close(&server->object);
            BipAppCallBack(&parms, OBRC_PARTIAL_CONTENT, BIPRESPONDER_COMPLETE);
            break;
        }
        if ((event->oper == GOEP_OPER_PULL) && server->object &&
            (server->object->bipOp == BIPOP_GET_IMAGES_LIST) &&
            (server->object->code == OBRC_PARTIAL_CONTENT)) {
            /* Handle the Partial Content special case on PutImage */
            BIPOBS_Close(&server->object);
            BipAppCallBack(&parms, OBRC_PARTIAL_CONTENT, BIPRESPONDER_COMPLETE);
            break;
        }
		}

        kal_trace(BT_TRACE_G6_OBEX,BT_BIP_SERVER_OPER_ABORTED, event->oper);
        if (server->object) {
            /* close any objects associated with the aborted operation */
            BIPOBS_Close(&server->object);
        }
            
        /* Tell the responder to abort */
        BipAppCallBack(&parms, 0, BIPRESPONDER_ABORT);
        break;

    case GOEP_EVENT_PRECOMPLETE:
        if ((event->oper == GOEP_OPER_PUSH) && 
            (server->object->bipOp == BIPOP_PUT_IMAGE) &&
            (server->object->code == OBRC_PARTIAL_CONTENT)) {
            /* Handle the Partial Content special case on PutImage */
            kal_trace(BT_TRACE_G6_OBEX,BT_BIP_SERVER_PARTIAL_CONTENT_PUTIMG);
            GOEP_ServerAbort(&server->sApp, OBRC_PARTIAL_CONTENT);
        }
        else if ((event->oper == GOEP_OPER_PULL) && 
            (server->object->bipOp == BIPOP_GET_IMAGES_LIST) &&
            (server->object->code == OBRC_PARTIAL_CONTENT)) {
            /* Handle the Partial Content special case on PutImage */
            kal_trace(BT_TRACE_G6_OBEX,BT_BIP_SERVER_PARTIAL_CONTENT_GETIMGLIST);
	        
            GOEP_ServerAbort(&server->sApp, OBRC_PARTIAL_CONTENT);
        }
        break;

    case GOEP_EVENT_CONTINUE:
        status = OB_STATUS_SUCCESS;

        switch(event->oper) {
        case GOEP_OPER_DISCONNECT:
            status = OB_STATUS_DISCONNECT;
            /* No break; fall through */
        case GOEP_OPER_ABORT:
            /* <!-- added by yfchu on 2007.8.16 */
            GOEP_ServerContinue(&server->sApp);
            /* -->    */
            return;

        case GOEP_OPER_CONNECT:
            server->request.bipOp = BIPOP_CONNECT;
            break;
            
        case GOEP_OPER_PUSH:
            if ((server->object->bipOp == BIPOP_PUT_IMAGE) &&
                (event->info.pushpull.finalBit)) {
                /* Queue the Image Handle header in the final Put Response packet */
/*            Avoid to abort operation for lack of image handle info
                if (server->request.r.rImg.imgHandle[0] == 0) {
                GOEP_ServerAbort(&server->sApp, OBRC_INTERNAL_SERVER_ERR);
                }
*/
                if (server->request.r.rImg.imgHandle != 0) {
                    int i = 0;
                    
                    server->request.r.rImg.imgHandle[7] = 0;

                    while (server->request.r.rImg.imgHandle[i] != 0)
                    {
                        server->request.r.rImg.imgHandleUni[2*i] = 0;
                        server->request.r.rImg.imgHandleUni[2*i + 1] = server->request.r.rImg.imgHandle[i];
                        i++;
                    }
                    server->request.r.rImg.imgHandleUni[2*i] = 0;
                    server->request.r.rImg.imgHandleUni[2*i + 1] = 0;
        
                    if (GOEP_ServerQueueHeader(&server->sApp, BIPH_IMG_HANDLE,
                        (U8 *)server->request.r.rImg.imgHandleUni, (U16)(2*i+2)) == FALSE) {
                        GOEP_ServerAbort(&server->sApp, OBRC_INTERNAL_SERVER_ERR);
                    }
                }
            }
            else if (server->request.bipOp == BIPOP_PUT_IMAGE && 
                (server->request.r.rImg.name[0] == 0 &&
                server->request.r.rImg.name[1] == 0))
            {
                OS_MemCopy(server->request.r.rImg.name, server->object->name,
                    GOEP_MAX_UNICODE_LEN*2);
                server->request.r.rImg.img_len = server->object->objLen;
            }
            break;
        }

		parms.FinalPacket = event->info.pushpull.finalBit;
        BipAppCallBack(&parms, status, BIPRESPONDER_CONTINUE);
        break;

    case GOEP_EVENT_COMPLETE:
        switch (event->oper) {
        case GOEP_OPER_CONNECT:
            server->request.bipOp = BIPOP_CONNECT;
            BipAppCallBack(&parms, OB_STATUS_SUCCESS, BIPRESPONDER_COMPLETE);
            break;

        case GOEP_OPER_PUSH:
        case GOEP_OPER_PULL:
            rcode = BIPOBS_Close(&server->object);
            if (rcode != OBRC_SUCCESS) {
                GOEP_ServerAbort(&server->sApp, rcode);
            }
            BipAppCallBack(&parms, OB_STATUS_SUCCESS, BIPRESPONDER_COMPLETE);
            break;

        default:
            break;
        }

        //Report(("BIP: Server: %d operation complete.\n", event->oper));
        server->request.offset = server->request.totLen = 0;
        server->request.bipOp = BIPOP_NOP;
        break;

    case GOEP_EVENT_START:
        if (event->oper == GOEP_OPER_PULL || event->oper == GOEP_OPER_PUSH) {
            BipObexServer  *rServer = parms.obex.server;

            rServer->object = BIPOBS_New(&rServer->sApp);
            Assert(rServer->object);

            /* New operation, initialize BipData */
            OS_MemSet((U8 *)&rServer->request, 0, sizeof(BipData));
        } 
		if (event->oper == GOEP_OPER_CONNECT)
		{
            BipObexServer  *rServer = parms.obex.server;
            /* New operation, initialize connect target */
            OS_MemSet(rServer->request.r.rConnect.targetUuid, 0, sizeof(rServer->request.r.rConnect.targetUuid));
			rServer->request.r.rConnect.targetLen = 0;
            rServer->request.r.rConnect.targetService = 0;
		}
        break;

    case GOEP_EVENT_HEADER_RX:
        switch (event->header.type) {
        case OBEXH_LENGTH:
            if (event->oper == GOEP_OPER_PUSH) {
                /* Set the object length for Put operations only */
                BIPOBS_SetObjectLen(server->object, event->info.pushpull.objectLen);
            }
            break;

        case BIPH_IMG_DESCRIPTION:
            if (event->header.len == 0 ||
                event->header.len == event->header.totalLen) {
                /* reset the offset */
                server->request.offset = 0;
            }
            if (event->header.len != 0) {
                /* tell the responder we have data */
                server->request.totLen = event->header.totalLen;
                server->request.len = event->header.len;

                switch (server->request.bipOp) {
                case BIPOP_GET_IMAGES_LIST:
                    server->request.i.iHDsc.buff = event->header.buffer;
                    server->request.dataType = BIPIND_HANDLES_DESCRIPTOR;
                    break;

                case BIPOP_PUT_IMAGE:
                case BIPOP_GET_IMAGE:
                    server->request.i.iIDsc.buff = event->header.buffer;
                    server->request.dataType = BIPIND_IMAGE_DESCRIPTOR;
                    break;

                default:
                    if (event->oper == GOEP_OPER_PULL || event->oper == GOEP_OPER_PUSH) {
                        BipResponderOp(event, &parms);
                    }
                    server->request.i.iIDsc.buff = event->header.buffer;
                    server->request.dataType = BIPIND_IMAGE_DESCRIPTOR;
                    break;
                }

                BipAppCallBack(&parms, 0, BIPRESPONDER_DATA_IND);

                /* Check if we have the entire description */
                if (server->request.totLen >
                    event->header.len + server->request.offset) {
                    server->request.offset += event->header.len;
                }
                else {
                    /* Set the offset back to null */
                    server->request.offset = server->request.totLen = 0;
                }
            }
            break;

        case BIPH_IMG_HANDLE:
            /* GOEP indicates the entire Unicode header */
            if (event->header.len != 0) {
                int i;
                switch (server->request.bipOp) {
                case BIPOP_GET_IMAGE_PROPERTIES:
                    ih = server->request.r.rProp.imgHandle;
                    break;

                case BIPOP_PUT_LINKED_THUMBNAIL:
                case BIPOP_GET_LINKED_THUMBNAIL:
                    ih = server->request.r.rThm.imgHandle;
                    break;

                case BIPOP_GET_IMAGE:
                    ih = server->request.r.rImg.imgHandle;
                    break;

                default:
                    /* We must not have the type header yet. Assume this is
                     * a PUT_LINKED_THUMBNAIL.
                     */
                    if (event->oper == GOEP_OPER_PULL || event->oper == GOEP_OPER_PUSH) {
                        BipResponderOp(event, &parms);
                    ih = server->request.r.rThm.imgHandle;
                    }
                    break;
                }

                /* copy the data from the buffer */
				if (ih != NULL)
				{
	                for(i = 0; i < (event->header.len/2); i++)
	                {
	                    ih[i] = event->header.buffer[2*i+1];
	                }
					/*OS_MemCopy((U8 *)ih, event->header.buffer, event->header.len); */
				}
            }
            break;

        case OBEXH_APP_PARAMS:
            /* Check for parameters */
            if (event->header.len == 0) {
                break;
            }

            /* Reassemble the parameters */
			ASSERT(server->appParms[0] + event->header.len <= sizeof(server->appParms));
            p = server->appParms + server->appParms[0] + 1;
            OS_MemCopy(p, event->header.buffer, event->header.len);
            server->appParms[0] += event->header.len;

            if (server->appParms[0] == event->header.totalLen) {
                /* Parse the App Parameters */
                p = server->appParms + 1;
                while (p - (server->appParms + 1) < event->header.totalLen) {
                    switch (p[0]) {
                    case BIPAPTAG_NB_RETURNED_HANDLES:
                        server->request.r.rList.nbReturnedHandles =
                            BEtoHost16(p + 2);
                        break;

                    case BIPAPTAG_LIST_START_OFFSET:
                        server->request.r.rList.listStartOffset =
                            BEtoHost16(p + 2);
                        break;

                    case BIPAPTAG_LATEST_CAPTURED_IMAGES:
                        server->request.r.rList.latestCapturedImages = p[2];
                        break;

                    case BIPAPTAG_STORE_FLAG:
                        server->request.r.rMon.storeFlag = p[2];
                        break;

                    case BIPAPTAG_PARTIAL_FILE_LENGTH:
                    case BIPAPTAG_PARTIAL_FILE_START_OFFSET:
                    case BIPAPTAG_TOTAL_FILE_SIZE:
                    case BIPAPTAG_END_FLAG:
                    case BIPAPTAG_REMOTE_DISPLAY:
                    case BIPAPTAG_SERVICE_ID:
                        break;

                    default:
                        break;
                    }
                    p += p[1] + 2;
                }
                /* Reset the App Param length */
                server->appParms[0] = 0;
            }
            break;

        case OBEXH_BODY:
            break;

        case OBEXH_END_BODY:
            break;

        case OBEXH_TARGET: /* CONNECT TARGET header */
			{    
				U8 len;

	            /* Reassemble the header */
				ASSERT (server->request.r.rConnect.targetLen + event->header.len <= sizeof(server->request.r.rConnect.targetUuid));
	            p = (U8*)(server->request.r.rConnect.targetUuid) + server->request.r.rConnect.targetLen;
	            OS_MemCopy(p, event->header.buffer, event->header.len);
	            server->request.r.rConnect.targetLen += event->header.len;

				len = server->request.r.rConnect.targetLen;
				p = (U8 *)(server->request.r.rConnect.targetUuid);
	            if (len == event->header.totalLen) 
				{
				    BipService targetService = 0;
				   
				    /* target header received */
	                if (OS_MemCmp(p, len, server->bipImgPullUuid, sizeof(server->bipImgPullUuid)))
	                {
	                    targetService = BIPSVC_IMAGE_PULL;
	                }
					else if (OS_MemCmp(p, len, server->bipImgPushUuid, sizeof(server->bipImgPushUuid)))
	                {
	                    targetService = BIPSVC_IMAGE_PUSH;
	                }

					server->request.r.rConnect.targetService = targetService;
	            }
        	}
			
            break;

        default:
            break;
        }
        break;

    case GOEP_EVENT_PROVIDE_OBJECT:
        /* If an operation has not yet been assigned, figure it out */
        if ((server->request.bipOp == BIPOP_NOP) &&
            (event->oper == GOEP_OPER_PULL || event->oper == GOEP_OPER_PUSH)) {
            BipResponderOp(event, &parms);
        }

        if (event->oper == GOEP_OPER_PULL &&
            server->request.bipOp & BIPOP_PULL_MASK) {
            BipAppCallBack(&parms, 0, BIPRESPONDER_REQUEST);
            if ((server->object->objLen != 0) &&
                (server->request.bipOp == BIPOP_GET_IMAGE ||
                server->request.bipOp == BIPOP_GET_MONITORING_IMAGE)) {
                /* Setup the LENGTH header for a Get Image response only */
                event->info.pushpull.objectLen = server->object->objLen;
            } else {
                /* Make sure we do not issue a LENGTH header for any other responses.  It
                 * is possible that we could have a value, if we received a LENGTH header 
                 * in a GET Request, so we always want to reset this value to zero.
                 */
                event->info.pushpull.objectLen = 0;
            }
        }
        else if (event->oper == GOEP_OPER_PUSH &&
            (server->request.bipOp == BIPOP_PUT_IMAGE ||
            server->request.bipOp == BIPOP_PUT_LINKED_THUMBNAIL)) {
            if (server->request.bipOp == BIPOP_PUT_IMAGE) {
                OS_MemCopy(server->request.r.rImg.name, server->object->name,
                    GOEP_MAX_UNICODE_LEN*2);
                server->request.r.rImg.img_len = server->object->objLen;
            }
            BipAppCallBack(&parms, 0, BIPRESPONDER_REQUEST);
        }
        GOEP_ServerAccept(&server->sApp, server->object);
        break;

    case GOEP_EVENT_AUTHORIZE_IND:
        BipAppCallBack(&parms, 0, BIPRESPONDER_AUTHORIZE_IND);
        break;

#if OBEX_AUTHENTICATION == XA_ENABLED

    case GOEP_EVENT_AUTH_CHALLENGE:
        parms.challenge = &event->challenge;
        BipAppCallBack(&parms, 0, BIP_OBAUTH_CHAL);
        break;

    case GOEP_EVENT_AUTH_RESPONSE:
        parms.response = &event->response;
        BipAppCallBack(&parms, 0, BIP_OBAUTH_RSP);
        break;
        
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

    default:
        //Report(("BIP: Server Event %d ignored.\n", event));
        break;
    }
}

/*---------------------------------------------------------------------------
 *            BipResponderOp
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process the Responder Operation.
 *
 * Return:    Success in processing the operation. (FALSE means operation
 *            was aborted.)
 *
 */
static BOOL BipResponderOp(GoepServerEvent *event, BipCallbackParms *parms)
{
    BipObexServer  *rServer = parms->obex.server;
    BipOp bipOp;

    Assert(rServer->object);

    switch (event->oper) {

    case GOEP_OPER_PULL:
        /* Parse the operation from the Type header */
        switch (BipGetType(event->info.pushpull.type, BIPTYPE_PULL)) {
        case BIPTYPE_CAPABILITIES:
            bipOp = BIPOP_GET_CAPABILITIES;
            break;

        case BIPTYPE_LISTING:
            bipOp = BIPOP_GET_IMAGES_LIST;
            break;

        case BIPTYPE_PROPERTIES:
            bipOp = BIPOP_GET_IMAGE_PROPERTIES;
            break;

        case BIPTYPE_IMG:
            bipOp = BIPOP_GET_IMAGE;
            break;

        case BIPTYPE_THM:
            bipOp = BIPOP_GET_LINKED_THUMBNAIL;
            break;

        case BIPTYPE_MONITORING:
            bipOp = BIPOP_GET_MONITORING_IMAGE;
            break;

        /* These are not yet supported */
        case BIPTYPE_ATTACHMENT:
            /* bipOp = BIPOP_GET_LINKED_ATTACHMENT */
        case BIPTYPE_PARTIAL:
            /* bipOp = BIPTYPE_PARTIAL */
        case BIPTYPE_STATUS:
            /* bipOp = BIPOP_GETSTATUS */
            goto abort;
            /* break; */

        default:
            /* This isn't a valid Pull operation, abort */
            goto abort;
            /* break; */
        }

        rServer->request.bipOp = rServer->object->bipOp = bipOp;
        break;

    case GOEP_OPER_PUSH:
        switch (BipGetType(event->info.pushpull.type, BIPTYPE_PUSH)) {
        case BIPTYPE_IMG:
            bipOp = BIPOP_PUT_IMAGE;
            /* DeleteImage isn't currently supported. It will need further
             * parsing when added as it has the same type as PutImage.
             * bipOp = BIPOP_DELETE_IMAGE;
             */
            break;

        case BIPTYPE_THM:
            bipOp = BIPOP_PUT_LINKED_THUMBNAIL;
            break;

        /* These are not yet supported */
        case BIPTYPE_ATTACHMENT:
            /* bipOp = BIPOP_PUT_LINKED_ATTACHMENT */
        case BIPTYPE_DISPLAY:
            /* bipOp = BIPOP_REMOTE_DISPLAY */
        case BIPTYPE_PRINT:
            /* bipOp = BIPOP_START_PRINT */
        case BIPTYPE_ARCHIVE:
            /* bipOp = BIPOP_START_ARCHIVE */
            goto abort;
            break;

        default:
            /* This isn't a valid Push operation, abort */
            goto abort;
            /* break; */
        }
        rServer->request.bipOp = rServer->object->bipOp = bipOp;

        BIPOBS_AppendName(rServer->object,
                (U8*)event->info.pushpull.name, event->info.pushpull.nameLen);

        BIPOBS_AppendType(rServer->object,
                event->info.pushpull.type, event->info.pushpull.typeLen);

	    GOEP_ServerResetPushNameInfo(&rServer->sApp);

        break;

    default:
        goto abort;
        /* break; */
    }

    return TRUE;

abort:
    GOEP_ServerAbort(&rServer->sApp, OBRC_BAD_REQUEST);
    //Report(("BIP: %d operation aborted.\n", rServer->request.bipOp));
    return FALSE;

}
#endif /* BIP_NUM_RESPONDERS */

#if BIP_NUM_INITIATORS > 0
/*---------------------------------------------------------------------------
 *            FindBipInitiator()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return the BIP Initiator channel given the ObexClientApp pointer.
 *
 * Return:    BipObexClient* or BipObexServer*
 */
void *FindBipInitiator(void *app)
{
    I8 i;

    /* Go through the BIP client array. */
    for (i = 0; i < BIP_NUM_INITIATORS; i++) {
        if (BIP(initiator)[i] && &BIP(initiator)[i]->primary.cApp == app) {
            return &(BIP(initiator)[i]->primary);
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------
 *            BipInitiatorCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes Initiator events.
 *
 * Return:    void
 *
 */
void BipInitiatorCallback(GoepClientEvent *event)
{
    BipObexClient      *client;
    BipCallbackParms    parms;
    BipImgHandlePtr     ih;
    U8                 *p;

    parms.obex.client = FindBipInitiator(event->handler);
    if (parms.obex.client == 0) {
		kal_trace(BT_TRACE_G6_OBEX,BT_BIP_INITIATOR_NOT_FOUND);	
        return;
    }
    client = parms.obex.client;

    switch (event->event) {
    case GOEP_EVENT_TP_CONNECTED:
        /* The transport layer connection is up. Send an OBEX Connect. */
        //Report(("BIP: Client: Transport Layer connection established.\n"));
        GOEP_Connect(&client->cApp, &client->connect);
        break;

    case GOEP_EVENT_TP_DISCONNECTED:
        /* The transport layer connection has been disconnected. */
        //Report(("BIP: Client: The transport connection has been disconnected.\n"));
       
        if (client->state == BIPCHS_CONNECTING) {
            BipAppCallBack(&parms, event->reason, BIPINITIATOR_FAILED);
        }

        client->data = &client->tData;
        client->data->bipOp = BIPOP_CONNECT;
        BipAppCallBack(&parms, OB_STATUS_DISCONNECT, BIPINITIATOR_COMPLETE);

        client->tData.offset = client->tData.totLen = 0;
        if (client->pushPull.object) {
            BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);
        }
        break;
        
    case GOEP_EVENT_COMPLETE:
        /* The requested operation has completed. */
        switch (event->oper) {
        case GOEP_OPER_CONNECT:
            /* OBEX connection is established. */
            client->state = BIPCHS_CONNECTED;
            //Report(("BIP: Client: OBEX Connect Operation is complete.\n"));
            client->data->bipOp = BIPOP_CONNECT;
            BipAppCallBack(&parms, OB_STATUS_SUCCESS, BIPINITIATOR_COMPLETE);
            break;

        case GOEP_OPER_PUSH:
            Assert(client->pushPull.object);
            BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);
            /* OBEX put is complete. */
            client->state = BIPCHS_COMPLETED;
            //Report(("BIP: Client: OBEX Put Operation is complete.\n"));
            BipAppCallBack(&parms, OB_STATUS_SUCCESS, BIPINITIATOR_COMPLETE);
            client->state = BIPCHS_CONNECTED;
            break;

        case GOEP_OPER_PULL:
            BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);

            client->state = BIPCHS_COMPLETED;
            //Report(("BIP: Client: OBEX Pull Operation is complete.\n"));
            BipAppCallBack(&parms, OB_STATUS_SUCCESS, BIPINITIATOR_COMPLETE);
            client->state = BIPCHS_CONNECTED;
            break;

        case GOEP_OPER_DISCONNECT:
            /* OBEX Disconnect is complete, disconnect link. */
            //Report(("BIP: Client: OBEX Disconnect Operation is complete.\n"));
            client->state = BIPCHS_INITIALIZED;
            GOEP_TpDisconnect(&client->cApp);
            client->b_TpDisconnect_called = KAL_TRUE;
            break;

        case GOEP_OPER_ABORT:
            //Report(("BIP: Client: OBEX Abort Operation is complete.\n"));
            client->state = BIPCHS_ABORTED;
            BipAppCallBack(&parms, event->reason, BIPINITIATOR_ABORT);

            if (client->pushPull.object) {
                /* close any objects associated with the aborted operation */
                BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);
            }
            client->state = BIPCHS_CONNECTED;
            break;

        default:
            //Report(("BIP: Client: OBEX Operation %d is complete.\n", event->oper));
            break;
        }

        client->tData.offset = client->tData.totLen = 0;
        client->data = &client->tData;
        client->tData.bipOp = BIPOP_NOP;
        break;
        
    case GOEP_EVENT_ABORTED:
        /* Handle the Partial Content special case on PutImage */
        if (event->oper == GOEP_OPER_PUSH &&
            ((BipObStoreHandle)client->pushPull.object)->bipOp == BIPOP_PUT_IMAGE &&
            event->reason == OBRC_PARTIAL_CONTENT) {
            BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);
            client->state = BIPCHS_COMPLETED;
			kal_trace(BT_TRACE_G6_OBEX,BT_BIP_I_PARTIALCONTENT_PUTIMG);	
            BipAppCallBack(&parms, OBRC_PARTIAL_CONTENT, BIPINITIATOR_COMPLETE);
            client->state = BIPCHS_CONNECTED;
            break;
        }
        if (event->oper == GOEP_OPER_PULL && client->pushPull.object &&
            ((BipObStoreHandle)client->pushPull.object)->bipOp == BIPOP_GET_IMAGES_LIST &&
            event->reason == OBRC_PARTIAL_CONTENT) {
            BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);
            client->state = BIPCHS_COMPLETED;
			kal_trace(BT_TRACE_G6_OBEX,BT_BIP_I_PARTIALCONTENT_GETIMGLIST);	
            BipAppCallBack(&parms, OBRC_PARTIAL_CONTENT, BIPINITIATOR_COMPLETE);
            client->state = BIPCHS_CONNECTED;
            break;
        }
        if(event->oper == GOEP_OPER_CONNECT)
        {
            //Report(("BIP: Client: Received failed connect response, disconnect the transport layer.\n"));
            if (event->reason == OBRC_LINK_DISCONNECT)
            {
                /* in case of TP_DISCONNECT, OBEX will invoke ABORT firtly */
                client->state = BIPCHS_INITIALIZED;
                client->b_TpDisconnect_called = KAL_TRUE;
                break;
            }
            if (client->state == BIPCHS_AUTHENTICATING)
            {
                client->data = &client->tData;
                client->data->bipOp = BIPOP_CONNECT;
                BipAppCallBack(&parms, (U16)(event->failed_rsp_code & 0x7F), BIPINITIATOR_ABORT);
            }
            else
            {                
                client->state = BIPCHS_INITIALIZED;
                GOEP_TpDisconnect(&client->cApp);
            }            
            break;
        }
        /* The requested operation has failed. Alert the user and cleanup. */
        //Report(("BIP: Client: The Operation was aborted, %d.\n", event->reason));
        client->state = BIPCHS_ABORTED;
        //BipAppCallBack(&parms, event->reason, BIPINITIATOR_ABORT);
        BipAppCallBack(&parms, (U16)(event->failed_rsp_code & 0x7F), BIPINITIATOR_ABORT);

        if (client->pushPull.object) {
            /* close any objects associated with the aborted operation */
            BIPOBS_Close((BipObStoreHandle *)&client->pushPull.object);
        }

        client->state = BIPCHS_CONNECTED;
        break;
        
    case GOEP_EVENT_CONTINUE:
        BipAppCallBack(&parms, event->reason, BIPINITIATOR_CONTINUE);
        /* Allways continue on sender side */
/*      GOEP_ClientContinue(&client->cApp);   */
        break;

    case GOEP_EVENT_HEADER_RX:
        switch (event->header.type) {
        case OBEXH_LENGTH:
            if (event->oper == GOEP_OPER_PULL) {
                /* Set the object length for Pull operations only */
                BIPOBS_SetObjectLen((BipObStoreHandle)client->pushPull.object,
                                    event->info.pull.objectLen);
            }
            break;

        case BIPH_IMG_DESCRIPTION:
            if (event->header.len == 0 ||
                event->header.len == event->header.totalLen) {
                /* reset the offset */
                client->data->offset = 0;
            }
            if (event->header.len != 0) {
                /* tell the initiator we have data */
                client->data->totLen = event->header.totalLen;
                client->data->len = event->header.len;

                switch (client->data->bipOp) {
                case BIPOP_GET_IMAGES_LIST:
                    client->data->i.iHDsc.buff = event->header.buffer;
                    client->data->dataType = BIPIND_HANDLES_DESCRIPTOR;
                    break;

                case BIPOP_PUT_IMAGE:
                case BIPOP_GET_IMAGE:
                    client->data->i.iIDsc.buff = event->header.buffer;
                    client->data->dataType = BIPIND_IMAGE_DESCRIPTOR;
                    break;

                default:
                    break;
                }

                BipAppCallBack(&parms, 0, BIPINITIATOR_DATA_IND);

                /* Check if we have the entire description */
                if (client->data->totLen >
                    event->header.len + client->data->offset) {
                    client->data->offset += event->header.len;
                }
                else {
                    /* Set the offset back to null */
                    client->data->offset = client->data->totLen = 0;
                }
            }
            break;

        case BIPH_IMG_HANDLE:
            /* GOEP indicates the entire Unicode header */
            if (event->header.len != 0) {
                int i;
                switch (client->data->bipOp) {
                case BIPOP_PUT_IMAGE:
                case BIPOP_REMOTE_DISPLAY:
                case BIPOP_GET_MONITORING_IMAGE:
                    ih = client->data->i.iHndl.imgHandle;
                    client->data->dataType = BIPIND_IMAGE_HANDLE;
                    break;

                default:
                    /* We don't expect an Image Handle for other ops,
                     * but this is the end of the op, so ignore it.
                     */
                    //Report(("BIP: Image Handle %s ignored.\n", event->header.buffer));
                    return;
                }

                /* copy the data from the buffer */
                for(i=0; i < (event->header.len/2); i++)
                {
                    ih[i] = event->header.buffer[2*i + 1];
                }
/*              OS_MemCopy((U8 *)ih, event->header.buffer, event->header.len); */

                /* Indicate the complete Image Handle header */
                BipAppCallBack(&parms, 0, BIPINITIATOR_DATA_IND);
            }
            break;

        case OBEXH_APP_PARAMS:
            /* Check for parameters */
            if (event->header.len == 0) {
                break;
            }

            /* Reassemble the parameters */
			ASSERT(client->appParms[0] + event->header.len <= sizeof(client->appParms));
            p = client->appParms + client->appParms[0] + 1;
            OS_MemCopy(p, event->header.buffer, event->header.len);
            client->appParms[0] += event->header.len;

            if (client->appParms[0] == event->header.totalLen) {
                /* Parse the App Parameters */
                p = client->appParms + 1;
                while (p - (client->appParms + 1) < event->header.totalLen) {
                    switch (p[0]) {
                    case BIPAPTAG_NB_RETURNED_HANDLES:
                        client->data->i.iList.nbReturnedHandles =
                            BEtoHost16(p + 2);
                        break;

                    case BIPAPTAG_LIST_START_OFFSET:
                        break;

                    case BIPAPTAG_LATEST_CAPTURED_IMAGES:
                        break;

                    case BIPAPTAG_PARTIAL_FILE_LENGTH:
                    case BIPAPTAG_PARTIAL_FILE_START_OFFSET:
                    case BIPAPTAG_TOTAL_FILE_SIZE:
                    case BIPAPTAG_END_FLAG:
                    case BIPAPTAG_REMOTE_DISPLAY:
                    case BIPAPTAG_SERVICE_ID:
                    case BIPAPTAG_STORE_FLAG:
                        break;

                    default:
                        break;
                    }
                    p += p[1] + 2;
                }
                /* Reset the App Param length */
                client->appParms[0] = 0;
            }
            break;

        default:
            break;
        }
        break;

    case GOEP_EVENT_DISCOVERY_FAILED:
    case GOEP_EVENT_NO_SERVICE_FOUND:
        client->data = &client->tData;
        client->data->bipOp = BIPOP_CONNECT;
        BipAppCallBack(&parms, BIP_NO_SERVICE_FOUND, BIPINITIATOR_FAILED);
        break;

    case GOEP_EVENT_SCO_REJECT:
        client->data = &client->tData;
        client->data->bipOp = BIPOP_CONNECT;
        BipAppCallBack(&parms, BIP_SCO_REJECT, BIPINITIATOR_FAILED);
        break;

    case GOEP_EVENT_BTCHIP_REJECT:
        client->data = &client->tData;
        client->data->bipOp = BIPOP_CONNECT;
        BipAppCallBack(&parms, BIP_BTCHIP_REJECT, BIPINITIATOR_FAILED);
        break;
        
#if OBEX_AUTHENTICATION == XA_ENABLED

    case GOEP_EVENT_AUTH_CHALLENGE:
        client->state = BIPCHS_AUTHENTICATING;
        parms.challenge = &event->challenge;
        BipAppCallBack(&parms, 0, BIP_OBAUTH_CHAL);
        break;

    case GOEP_EVENT_AUTH_RESPONSE:
        parms.response = &event->response;
        BipAppCallBack(&parms, 0, BIP_OBAUTH_RSP);
        break;
        
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

    default:
        //Report(("BIP: Client Event %d ignored.\n", event));
        break;
    }
}

/*---------------------------------------------------------------------------
 *            BipBuildReq
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Builds a request.
 *
 * Return:    OB_STATUS_FAILED - Could not build pull object.    
 *            Otherwise the operation Started Successfully.
 */
ObStatus BipBuildReq(BipObexClient *client)
{
    /* Get a new object store entry. */
    client->pushPull.object = BIPOBS_New(&client->cApp);
    if (client->pushPull.object == 0) {
        return OB_STATUS_FAILED;
    }
    if (client->pushPull.name) {
        BIPOBS_AppendName((BipObStoreHandle)client->pushPull.object,
            (U8*)client->pushPull.name, OS_StrLen((const char *)client->pushPull.name));
    }

    if (client->pushPull.type) {
        BIPOBS_AppendType((BipObStoreHandle)client->pushPull.object,
            (U8*)client->pushPull.type, OS_StrLen((const char *)client->pushPull.type));
    }
      
    /* Save to ObStore structure for use on the response. */
    ((BipObStoreHandle)client->pushPull.object)->bipOp = client->data->bipOp;
    ((BipObStoreHandle)client->pushPull.object)->ocx = client->data->ocx;

    if (client->data->bipOp == BIPOP_PUT_IMAGE || client->data->bipOp == BIPOP_PUT_LINKED_THUMBNAIL) {
        /* Set the object length of the image we are going to Put */
        BIPOBS_SetObjectLen((BipObStoreHandle)client->pushPull.object, client->data->totLen);
        client->pushPull.objectLen = client->data->totLen;
    }

    return OB_STATUS_SUCCESS;
}
#endif /* BIP_NUM_INITIATORS */

#endif /*__BT_BIP_PROFILE__*/

