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
 * File:        bipobs.c
 *     $Workfile:bipobs.c$ for XTNDAccess Blue SDK, Version 2.x
 *     $Revision: #1 $
 *
 * Description: This file contains the code for shared Object store
 *              components used in the BIP profile.
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
 * Sep 19 2009 mtk80029
 * [MAUI_01898776] Revise code according to Klocword tool review result
 * 
 *
 * Jun 9 2009 mtk80029
 * [MAUI_01866671] [BT] OBEX, GOEP,PBAP,BIP,HFP: replace "Report" trace with kal_trace
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
 *            BIPOBS_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the object store.
 *
 * Return:    TRUE - object store initialized.
 *            FALSE - could not create inbox.
 *
 */
BOOL BIPOBS_Init(void)
{
    U8 i;

    /* Initialize the object store entries */
    InitializeListHead(&BIP(obsList));

    for (i = 0; i < OBS_MAX_NUM_ENTRIES; i++) {
        InsertTailList(&BIP(obsList), &BIP(obsEntries[i].node));
    }

#if BIP_NUM_RESPONDERS > 0
    /* Initialize the print job queue */
    InitializeListHead(&BIP(jobQueue));
#endif

    return TRUE;
}


/*---------------------------------------------------------------------------
 *            BIPOBS_New()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Obtain an object store item.
 *
 * Return:    Handle of object store or 0 if no more objects store entries.
 *
 */
BipObStoreHandle BIPOBS_New(void *app)
{
    BipObStoreHandle obs = 0;

    if (IsListEmpty(&BIP(obsList)) == FALSE) {
        obs = (BipObStoreHandle)RemoveHeadList(&BIP(obsList));
        obs->app = app;
        obs->ocx = 0;
        obs->objLen = 0;
        obs->offset = 0;
        obs->nameLen = 0;
        obs->name[0] = 0;
        obs->typeLen = 0;
        obs->type[0] = 0;
        obs->code = 0;
    }
    else {
        //Report(("[BIP OBS]BIPOBS_New: Out of entries\n"));
    }

    return obs;

}

/*---------------------------------------------------------------------------
 *            BIPOBS_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the object store item and return the handle to the pool.
 *
 * Return:    void 
 *
 */
ObexRespCode BIPOBS_Close(BipObStoreHandle *obshPtr)
{
    ObexRespCode  rcode = OBRC_SUCCESS;
    BipObStoreHandle obsh = *obshPtr;

    Assert(obsh);

    if (obsh->ocx != 0) {
        obsh->ocx = 0;
    }
    
    /* Return the BipObStoreHandle to the pool */
    InsertTailList(&BIP(obsList), &(obsh->node));

    *obshPtr = 0;
    return rcode;
}

/*---------------------------------------------------------------------------
 *            BIPOBS_AppendName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the name. 
 *
 * Return:    void
 *
 */
void BIPOBS_AppendName(BipObStoreHandle obs, const U8 *name, U16 len)
{
    U16 finalLen;
    U16 i;

    if ((obs == 0) || (len == 0)) {
        return;
    }
  
    /* Append the string to the existing name. */
	ASSERT(obs->nameLen < GOEP_MAX_UNICODE_LEN*2); 
    finalLen = min((GOEP_MAX_UNICODE_LEN*2 - 1) - obs->nameLen, len);
    OS_MemCopy((U8 *)(obs->name + obs->nameLen), (U8 *)name, finalLen);
    
    i = obs->nameLen + finalLen;

    /* Put a 0 at the end of the name if one does not exists */
    if (obs->name[i-1] != 0) {
        obs->name[i] = 0;
        i++;
    }
    /* Update nameLen. It does not include the 0 */
    obs->nameLen = (i - 1);
}

/*---------------------------------------------------------------------------
 *            BIPOBS_AppendType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the object type field.
 *
 * Return:    void
 *
 */
void BIPOBS_AppendType(BipObStoreHandle obs, const U8 *type, U16 len)
{
    U16 copyLen;
    U16 i;

    if ((obs == 0) || (len == 0)) {
        return;
    }
  
    /* Append the ASCII string to the existing ASCII name. */
	ASSERT(obs->typeLen < GOEP_MAX_TYPE_LEN); 
    copyLen = min((GOEP_MAX_TYPE_LEN - 1) - obs->typeLen, len);
    OS_MemCopy((U8 *)(obs->type + obs->typeLen), type, copyLen);
    
    i = obs->typeLen + copyLen;

    /* Put a 0 at the end of the type if one does not exist */
    if (obs->type[i-1] != 0) {
        obs->type[i] = 0;
        i++;
    }
    /* Update typeLen. It does not include the 0 */
    obs->typeLen = (i - 1);
}

/*---------------------------------------------------------------------------
 *            BIPOBS_GetObjectLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the length of an object store item.
 *
 * Return:    The length of the object in bytes.
 *
 */
U32 BIPOBS_GetObjectLen(void *obs)
{
    BipObStoreHandle obsh = (BipObStoreHandle)obs;
    
    return obsh ? obsh->objLen : 0;
}

/*---------------------------------------------------------------------------
 *            BIPOBS_Read()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read an image file.
 *
 * Return:    OBRC_SUCCESS -      Buffer was read successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not read.
 *
 */
ObexRespCode BIPOBS_Read(void *obs, U8* buff, U16 len)
{
    BipObStoreHandle    obsh = (BipObStoreHandle)obs;
    ObexRespCode        ret = OBRC_NOT_FOUND;
    BipCallbackParms    parms;
    BipEvent            bipEvent;
    U32                *offset;

    if (obsh == 0) {
        return ret;
    }
    Assert(obsh->app != 0);

    switch (obsh->bipOp) {
#if BIP_NUM_RESPONDERS > 0
    case BIPOP_GET_CAPABILITIES:
    case BIPOP_GET_IMAGES_LIST:
    case BIPOP_GET_IMAGE_PROPERTIES:
    case BIPOP_GET_IMAGE:
    case BIPOP_GET_LINKED_THUMBNAIL:
    case BIPOP_GET_MONITORING_IMAGE:
        /* Setup for image, thumbnail, or xml read from responder */
        parms.obex.server = FindBipResponder(obsh->app);
		if (parms.obex.server == NULL)
		{
			return OBRC_BAD_REQUEST;
		}
		
        parms.obex.server->request.bipOp = obsh->bipOp;

        offset = &parms.obex.server->request.offset;
        parms.obex.server->request.len = len;
        parms.obex.server->request.ocx = obsh->ocx;
        parms.obex.server->request.totLen = obsh->objLen;
        parms.obex.server->request.offset = obsh->offset;
        bipEvent = BIPRESPONDER_DATA_REQ;
        break;
#endif

#if BIP_NUM_INITIATORS > 0
    case BIPOP_PUT_IMAGE:
    case BIPOP_PUT_LINKED_THUMBNAIL:
        /* set up for image or thumbnail read from initiator */
        parms.obex.client = FindBipInitiator(obsh->app);
		if (parms.obex.client == NULL)
		{
			return OBRC_BAD_REQUEST;
		}
		
        parms.obex.client->data->bipOp = obsh->bipOp;

        offset = &parms.obex.client->data->offset;
        parms.obex.client->data->len = len;
        parms.obex.client->data->ocx = obsh->ocx;
        parms.obex.client->data->totLen = obsh->objLen;
        parms.obex.client->data->offset = obsh->offset;
        bipEvent = BIPINITIATOR_DATA_REQ;
        break;
#endif

    default:
        return ret;
    }

    /* Set up read buffer */
    switch (obsh->bipOp) {
    case BIPOP_GET_CAPABILITIES:
        parms.obex.server->request.dataType = BIPREQ_IMG_CAPABILITIES_XML;
        parms.obex.server->request.r.rCapa.buff = buff;
        break;

    case BIPOP_GET_IMAGES_LIST:
        parms.obex.server->request.dataType = BIPREQ_IMG_LISTING_XML;
        parms.obex.server->request.r.rList.buff = buff;
        break;

    case BIPOP_GET_IMAGE_PROPERTIES:
        parms.obex.server->request.dataType = BIPREQ_IMG_PROPERTIES_XML;
        parms.obex.server->request.r.rProp.buff = buff;
        break;

    case BIPOP_GET_IMAGE:
    case BIPOP_GET_MONITORING_IMAGE:
        parms.obex.server->request.dataType = BIPREQ_IMAGE;
        parms.obex.server->request.r.rImg.buff = buff;
        break;

    case BIPOP_GET_LINKED_THUMBNAIL:
        parms.obex.server->request.dataType = BIPREQ_THUMBNAIL;
        parms.obex.server->request.r.rThm.buff = buff;
        break;

    case BIPOP_PUT_IMAGE:
        parms.obex.client->data->dataType = BIPREQ_IMAGE;
        parms.obex.client->data->r.rImg.buff = buff;
        break;

    case BIPOP_PUT_LINKED_THUMBNAIL:
        parms.obex.client->data->dataType = BIPREQ_THUMBNAIL;
        parms.obex.client->data->r.rThm.buff = buff;
        break;

    default:
        break;
    }

    /* If we're here, then we have an object to read */
    if (obsh->offset == 0) {
        *offset = 0;
        //Report(("Reading %10lu\n",obsh->offset));
    }
    BipAppCallBack(&parms, 0, bipEvent);
    if (parms.data->len == len) {
        obsh->offset += len;
        //Report(("%10lu\n", obsh->offset));
        ret = OBRC_SUCCESS;
    } else {
        kal_trace(BT_TRACE_G6_OBEX,BT_BIP_OBS_READ_ERROR,parms.data->len,len);
        ret = OBRC_FORBIDDEN;
    }

    *offset = obsh->offset;

    return ret;
}

/*---------------------------------------------------------------------------
 *            BIPOBS_Write()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Write an image file.
 *
 * Return:    OBRC_SUCCESS -      Buffer was written successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not written.
 *
 */
ObexRespCode BIPOBS_Write(void *obs, U8 *buff, U16 len)
{
    BipObStoreHandle    obsh = (BipObStoreHandle)obs;
    ObexRespCode        ret = OBRC_FORBIDDEN;
    BipCallbackParms    parms;
    BipEvent            bipEvent;
    U32                *offset;

    if (obsh == 0) {
        return ret;
    }
    Assert(obsh->app != 0);

    switch (obsh->bipOp) {
#if BIP_NUM_INITIATORS > 0
    case BIPOP_GET_CAPABILITIES:
    case BIPOP_GET_IMAGES_LIST:
    case BIPOP_GET_IMAGE_PROPERTIES:
    case BIPOP_GET_LINKED_THUMBNAIL:
    case BIPOP_GET_IMAGE:
    case BIPOP_GET_MONITORING_IMAGE:
        /* Setup for image, thumbnail, or xml write to initiator */
        parms.obex.client = FindBipInitiator(obsh->app);
        if (parms.obex.client == NULL)
        {
            return OBRC_BAD_REQUEST;
        }
		
        parms.obex.client->data->bipOp = obsh->bipOp;
        parms.obex.client->data->dataType = BIPIND_NO_DATA;

        offset = &parms.obex.client->data->offset;
        parms.obex.client->data->len = len;
        parms.obex.client->data->totLen = obsh->objLen;
        parms.obex.client->data->offset = obsh->offset;
/*
        if ((obsh->bipOp == BIPOP_GET_IMAGE) ||
            (obsh->bipOp == BIPOP_GET_LINKED_THUMBNAIL) ||
            (obsh->bipOp == BIPOP_GET_MONITORING_IMAGE)) {
            parms.obex.client->data->ocx = obsh->ocx;
        }
*/
        bipEvent = BIPINITIATOR_DATA_IND;
        if (obsh->offset == 0) {
            /* Indicate response to initiator first */
            BipAppCallBack(&parms, 0, BIPINITIATOR_RESPONSE);
        }
        break;
#endif

#if BIP_NUM_RESPONDERS > 0
    case BIPOP_PUT_IMAGE:
    case BIPOP_PUT_LINKED_THUMBNAIL:
        /* set up for image or thumbnail write to responder */
        parms.obex.server = FindBipResponder(obsh->app);
		if (parms.obex.server == NULL)
		{
			return OBRC_BAD_REQUEST;
		}
        parms.obex.server->request.bipOp = obsh->bipOp;

        offset = &parms.obex.server->request.offset;
        parms.obex.server->request.len = len;
        parms.obex.server->request.totLen = obsh->objLen;
        parms.obex.server->request.offset = obsh->offset;
/*      parms.obex.server->request.ocx = obsh->ocx; */
        bipEvent = BIPRESPONDER_DATA_IND;
        break;
#endif

    default:
        return ret;
    }

    /* Set up write buffer */
    switch (obsh->bipOp) {
    case BIPOP_GET_CAPABILITIES:
        parms.obex.client->data->dataType = BIPIND_IMG_CAPABILITIES_XML;
        parms.obex.client->data->i.iCapa.buff = buff;
        break;

    case BIPOP_GET_IMAGES_LIST:
        parms.obex.client->data->dataType = BIPIND_IMG_LISTING_XML;
        parms.obex.client->data->i.iList.buff = buff;
        break;

    case BIPOP_GET_IMAGE_PROPERTIES:
        parms.obex.client->data->dataType = BIPIND_IMG_PROPERTIES_XML;
        parms.obex.client->data->i.iProp.buff = buff;
        break;

    case BIPOP_GET_IMAGE:
    case BIPOP_GET_MONITORING_IMAGE:
        parms.obex.client->data->dataType = BIPIND_IMAGE;
        parms.obex.client->data->i.iImg.buff = buff;
        break;

    case BIPOP_GET_LINKED_THUMBNAIL:
        parms.obex.client->data->dataType = BIPIND_THUMBNAIL;
        parms.obex.client->data->i.iThm.buff = buff;
        break;

    case BIPOP_PUT_IMAGE:
        parms.obex.server->request.dataType = BIPIND_IMAGE;
        parms.obex.server->request.i.iImg.buff = buff;
        break;

    case BIPOP_PUT_LINKED_THUMBNAIL:
        parms.obex.server->request.dataType = BIPIND_THUMBNAIL;
        parms.obex.server->request.i.iThm.buff = buff;
        break;

    default:
        break;
    }

    /* If we're here, then we have an object to write. */
    if (obsh->offset == 0) {
        *offset = 0;
        //Report(("Writing %10lu\n",obsh->offset));
    }
    BipAppCallBack(&parms, 0, bipEvent);
    if (parms.data->len == len) {
        obsh->offset += len;
        //Report(("%10lu\n", obsh->offset));
        ret = OBRC_SUCCESS;
    } else {
        kal_trace(BT_TRACE_G6_OBEX,BT_BIP_OBS_WRITE_ERROR,parms.data->len,len);
		
        ret = OBRC_FORBIDDEN;
    }

    *offset = obsh->offset;

    return ret;

}

#endif /*__BT_BIP_PROFILE__*/

