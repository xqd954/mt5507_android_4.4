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
* File:        pbapobs.c
*     $Workfile:pbapobs.c$ for iAnywhere Blue SDK, PBAP Version 1.1.2
*     $Revision: #1 $
*
* Description: This file contains the code for shared Object store
*              components used in the PBAP application.
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
*             HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Log: $
*
* 
*
* 
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
****************************************************************************/
/* Keep this header file outside the compile option to avoid compile warning */

#ifdef __BT_PBAP_PROFILE__

#include "osapi.h"
#include "pbap.h"
#include "pbapobs.h"
#include "bluetooth_trc.h"

/*---------------------------------------------------------------------------
*            PBAPOBS_Init()
*---------------------------------------------------------------------------
*
* Synopsis:  Initialize the object store.
*
* Return:    TRUE - object store initialized.
*            FALSE - could not create inbox.
*
*/
kal_bool PBAPOBS_Init(void)
{
    U8 i;
	
    /* Initialize the object store entries */
    InitializeListHead(&PBAP(obsList));
	
    //   for (i = 0; i < PBAP_OBS_NUM_ENTRIES; i++)
    for (i = 0; i < 4; i++)
    {
        InsertTailList(&PBAP(obsList), &PBAP(obsEntries[i].node));
    }
	
    return KAL_TRUE;
}

/*---------------------------------------------------------------------------
*            PBAPOBS_New()
*---------------------------------------------------------------------------
*
* Synopsis:  Obtain an object store item.
*
* Return:    Handle of object store or 0 if no more objects store entries.
*
*/
PbapObStoreHandle PBAPOBS_New(void *app)
{
    PbapObStoreHandle obs = 0;
	
    if (IsListEmpty(&PBAP(obsList)) == FALSE)
    {
        obs = (PbapObStoreHandle)RemoveHeadList(&PBAP(obsList));
        obs->app = app;
        obs->objLen = 0;
        obs->offset = 0;
        obs->nameLen = 0;
        obs->name[0] = 0;
        obs->typeLen = 0;
        obs->type[0] = 0;
        obs->code = 0;
    }
    else 
    {
		kal_trace(TRACE_GROUP_5, BT_PBAP_OBS_NEW_FAIL);	
    }
	
    return obs;
	
}

/*---------------------------------------------------------------------------
*            PBAPOBS_Close()
*---------------------------------------------------------------------------
*
* Synopsis:  Close the object store item and return the handle to the pool.
*
* Return:    void 
*
*/
ObexRespCode PBAPOBS_Close(PbapObStoreHandle *obshPtr)
{
    ObexRespCode  rcode = OBRC_SUCCESS;
    PbapObStoreHandle obsh = *obshPtr;
	ASSERT(obsh);
    /* Return the PbapObStoreHandle to the pool */
    InsertTailList(&PBAP(obsList), &(obsh->node));
	
    *obshPtr = 0;
    return rcode;
}

/*---------------------------------------------------------------------------
*            PBAPOBS_AppendName
*---------------------------------------------------------------------------
*
* Synopsis:  Append the ASCII string to the name. 
*
* Return:    void
*
*/
void PBAPOBS_AppendName(PbapObStoreHandle obs, const U8 *name, U16 len)
{
    U16 finalLen;
    U16 i;
	
    if ((obs == 0) || (len == 0))
    {
        return;
    }
	
    /* Append the string to the existing name. */
    finalLen = min((GOEP_MAX_UNICODE_LEN - 1) - obs->nameLen, len);
    OS_MemCopy((U8 *)(obs->name + obs->nameLen), (U8 *)name, finalLen);
    
    i = obs->nameLen + finalLen;
	
    /* Put a 0 at the end of the name if one does not exists */
    if (obs->name[i-1] != 0)
    {
        obs->name[i] = 0;
        i++;
    }
    /* Update nameLen. It does not include the 0 */
    obs->nameLen = (i - 1);
}


/*---------------------------------------------------------------------------
*            PBAPOBS_AppendType()
*---------------------------------------------------------------------------
*
* Synopsis:  Append the ASCII string to the object type field.
*
* Return:    void
*
*/
void PBAPOBS_AppendType(PbapObStoreHandle obs, const U8 *type, U16 len)
{
    U16 copyLen;
    U16 i;
	
    if ((obs == 0) || (len == 0))
    {
        return;
    }
	
    /* Append the ASCII string to the existing ASCII name. */
    copyLen = min((GOEP_MAX_TYPE_LEN - 1) - obs->typeLen, len);
    OS_MemCopy((U8 *)(obs->type + obs->typeLen), type, copyLen);
    
    i = obs->typeLen + copyLen;
	
    /* Put a 0 at the end of the type if one does not exist */
    if (obs->type[i-1] != 0)
    {
        obs->type[i] = 0;
        i++;
    }
    /* Update typeLen. It does not include the 0 */
    obs->typeLen = (i - 1);
}

/*---------------------------------------------------------------------------
*            PBAPOBS_GetObjectLen()
*---------------------------------------------------------------------------
*
* Synopsis:  Get the length of an object store item.
*
* Return:    The length of the object in bytes.
*
*/
U32 PBAPOBS_GetObjectLen(void *obs)
{
    PbapObStoreHandle obsh = (PbapObStoreHandle)obs;    
    return obsh->objLen;
}

/*---------------------------------------------------------------------------
*            PBAPOBS_Read()
*---------------------------------------------------------------------------
*
* Synopsis:  Read a phonebook object.
*
* Return:    OBRC_SUCCESS -      Buffer was read successfully.
*            OBRC_UNAUTHORIZED - Buffer was not read.
*
*/
ObexRespCode PBAPOBS_Read(void *obs, U8* buff, U16 len)
{
    ObexRespCode        ret = OBRC_NOT_FOUND;
	
#if PBAP_NUM_SERVERS > 0
    PbapObStoreHandle   obsh = (PbapObStoreHandle)obs;
	
    if (obsh == 0)
    {
        return ret;
    }
    ASSERT(obsh->app != 0);
    /* If we're here, then we have an object to read */
    if (obsh->offset == 0)
    {
        //Report(("Reading %10lu\n",obsh->offset));
    }
	
    /* Ask the upper layer for the data for the provided buffer */
    PBAP(serverParms).event = PBAP_EVENT_DATA_REQ;
    PBAP(serverParms).server = FindPbapServer(obsh->app);
    
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
    PBAP(serverParms).u.dataReq.type = PBAP_DATAREQ_READ;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
	
    PBAP(serverParms).u.dataReq.name = obsh->name;  /* Null-terminated, ASCII */
    PBAP(serverParms).u.dataReq.buffer = buff;
    PBAP(serverParms).u.dataReq.req.len = len;
    PBAP(serverCallback)(&PBAP(serverParms));
    
	
    if (PBAP(serverParms).u.dataReq.req.len == len)
    {
        obsh->offset += len;
        //Report(("%10lu\n", obsh->offset));
        ret = OBRC_SUCCESS;
    }
    else
    {
		kal_trace(TRACE_GROUP_5, BT_PBAP_OBS_READ_ERROR, PBAP(serverParms).u.dataReq.req.len, len);	
		
        ret = OBRC_FORBIDDEN;
    }
#endif /* PBAP_NUM_SERVERS > 0 */
	
    return ret;
}


#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
*            PBAPOBS_ReadFlex
*---------------------------------------------------------------------------
*
* Synopsis:  Read a phonebook object.  Pull Vcard Listing operation usually
*            uses this method, as it does not know the size of the 
*            dynamic folder listing ahead of time.
*
* Return:    OBRC_SUCCESS on success.
*
*/
ObexRespCode PBAPOBS_ReadFlex(void *obs, U8 *buff, U16 *len, kal_bool *more)
{
    ObexRespCode        ret = OBRC_NOT_FOUND;
	
#if PBAP_NUM_SERVERS > 0
    PbapObStoreHandle   obsh = (PbapObStoreHandle)obs;
	
    if (obsh == 0)
    {
        return ret;
    }
	
    ASSERT(obsh->app != 0);
	
    /* If we're here, then we have an object to read */
    if (obsh->offset == 0)
    {
        //Report(("Reading %10lu\n",obsh->offset));
    }
	  
    /* Ask the upper layer for the data for the provided buffer */
    PBAP(serverParms).event = PBAP_EVENT_DATA_REQ;
    PBAP(serverParms).server = FindPbapServer(obsh->app);
    PBAP(serverParms).u.dataReq.type = PBAP_DATAREQ_READ_FLEX;
    PBAP(serverParms).u.dataReq.name = obsh->name;  /* Null-terminated, ASCII */
    PBAP(serverParms).u.dataReq.buffer = buff;
    PBAP(serverParms).u.dataReq.flex.len = len;
    PBAP(serverParms).u.dataReq.flex.more = more;
    PBAP(serverCallback)(&PBAP(serverParms));
    
	
    if (*(PBAP(serverParms).u.dataReq.flex.len) <= *len)
    {
        obsh->offset += *len;
        //Report(("%10lu\n", obsh->offset));
        ret = OBRC_SUCCESS;
    }
    else 
    {
        //Report(("Error reading object\n"));
		
        ret = OBRC_FORBIDDEN;
    }
#endif /* PBAP_NUM_SERVERS > 0 */
	
    return ret;
}
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/* csj */
#if 0
/*---------------------------------------------------------------------------
*            PBAPOBS_Write()
*---------------------------------------------------------------------------
*
* Synopsis:  Write an image file.
*
* Return:    OBRC_SUCCESS -      Buffer was written successfully.
*            OBRC_UNAUTHORIZED - Buffer was not written.
*
*/
ObexRespCode PBAPOBS_Write(void *obs, U8 *buff, U16 len)
{
    ObexRespCode         ret = OBRC_FORBIDDEN;
	
#if PBAP_NUM_CLIENTS > 0
    PbapObStoreHandle    obsh = (PbapObStoreHandle)obs;
	
    if (obsh == 0)
    {
        return ret;
    }
	
    Assert(obsh->app != 0);
	
    /* If we're here, then we have an object to write. */
    if (obsh->offset == 0)
    {
        Report(("Writing %10lu\n",obsh->offset));
    }
	
    PBAP(clientParms).event = PBAP_EVENT_DATA_IND;
    PBAP(clientParms).client = FindPbapClient(obsh->app);
    PBAP(clientParms).u.dataInd.buffer = buff;
    PBAP(clientParms).u.dataInd.len = len;
    PBAP(clientParms).u.dataInd.name = obsh->name;  /* Null-terminated, ASCII */
    PBAP(clientCallback)(&PBAP(clientParms));
	
    if (PBAP(clientParms).u.dataInd.len == len)
    {
        obsh->offset += len;
        Report(("%10lu\n", obsh->offset));
        ret = OBRC_SUCCESS;
    }
    else
    {
        Report(("Error writing image\n"));
        ret = OBRC_FORBIDDEN;
    }
#endif /* PBAP_NUM_CLIENTS > 0 */
	
    return ret;
}
#endif /* 0 */


#endif /* __BT_PBAP_NEW_PROFILE__ */
