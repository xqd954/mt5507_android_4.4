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

#ifndef __BPPOBS_H
#define __BPPOBS_H

/****************************************************************************
 *
 * File:          bppobs.h
 *     $Workfile:bppobs.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * Created:       May 15, 1997
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 1997-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#include "stdio.h"
#include "obexerr.h"
#include "utils.h"
#include "obex.h"

/****************************************************************************
 *
 * Object Store Definitions
 *
 * Definitions used internally by the object store.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Constants Reference
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *
 * Size Constants used by ObStoreEntry
 */
#define OBS_MAX_NUM_ENTRIES 4


/****************************************************************************
 *
 * Data structures 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ObStoreEntry Structure
 */
typedef struct _BppObStoreEntry {
    ListEntry       node;
    U16             bppOp;
    void           *app;
    U8             *buff;
    U32             objLen;
    U32             offset;
    U16             nameLen;
    U8              name[GOEP_MAX_UNICODE_LEN];
    U8              typeLen;
    U8              type[GOEP_MAX_TYPE_LEN];
    U8              appParms[255];
    void           *job;
    ObexRespCode    code;
    void           *ocx;
} BppObStoreEntry;

/*---------------------------------------------------------------------------
 *
 * BppObStoreHandle Structure
 */
typedef struct _BppObStoreEntry *BppObStoreHandle;


/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL BPPOBS_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
BOOL BPPOBS_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BPPOBS_Close(BppObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obshPtr - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode BPPOBS_Close(BppObStoreHandle *obshPtr);


/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 BPPOBS_GetObjectLen(void *obs)
 *
 * Description:   Get the length of an object store item.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 * Returns:       The object length
 */
U32 BPPOBS_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_SetObjectLen(BppObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define BPPOBS_SetObjectLen(obs, len) ((obs)->objLen = (len))









/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_AppendName(BppObStoreHandle obs,
 *                                       const U16    *name, 
 *                                       U16           len)
 *
 * Description:   Append the UTF16 string to the name. 
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                name - pointer to UTF16 string to append
 *
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void BPPOBS_AppendName(BppObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_AppendType(BppObStoreHandle  obs, 
 *                                       const U8      *type,
 *                                       U16            len )
 *
 * Description:   Append the ASCII string to the object type. 
 *
 * Parameters:    obs - handle of object store entry.
 *                type- pointer to ASCII string to append
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void BPPOBS_AppendType(BppObStoreHandle obs, const U8 *type, U16 len);




/*---------------------------------------------------------------------------
 *
 * Prototype:     char* BPPOBS_GetNameAscii(BppObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define BPPOBS_GetNameAscii(obs) ((obs)->name)





/*---------------------------------------------------------------------------
 *
 * Prototype:     BppObStoreHandle BPPOBS_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
BppObStoreHandle BPPOBS_New(void *app);



#if BPP_NUM_SENDERS > 0
/*---------------------------------------------------------------------------
 *
 * Prototype:   ObexRespCode BPPOBS_ReadDocument(void *obs,
 *                                               U8* buff, U16 len)
 *
 * Description: Read file data from the object store entry.
 *              A function that performs this behavior is REQUIRED 
 *              by the OBEX Protocol.  This function fills in the 
 *              OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:  obs - pointer to the handle of object store entry.
 *                    Casting from void* to the object store entry
 *                    must be done inside this function.
 *
 *              buff - pointer to location where data is read into.
 *
 *              len - length in bytes of data to read
 *
 * Returns:     OBRC_SUCCESS      - Read was successful.
 *              OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode BPPOBS_ReadDocument(void *obs, U8* buff, U16 len);
ObexRespCode BPPOBS_ReadFlex(void *obs, U8* buff, U16 *len, BOOL *more);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BPPOBS_RecvSoapResponse(void *obs,
 *                                                     U8* buff, U16 len)
 *
 * Description:   Send SOAP data to the Sender.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Write field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to data to send.
 *
 *                len - length in bytes of data to send.
 *
 * Returns:       OBRC_SUCCESS      - Print was successful.
 *                OBRC_UNAUTHORIZED - Print failed. 
 */
ObexRespCode BPPOBS_RecvSoapResponse(void *obs, U8 *buff, U16 len);
#endif /* BPP_NUM_SENDERS */

#endif /* __BPPOBS_H */

