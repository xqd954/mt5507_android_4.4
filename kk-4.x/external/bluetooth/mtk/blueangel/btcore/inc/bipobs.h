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

#ifndef __BIPOBS_H
#define __BIPOBS_H

/****************************************************************************
 *
 * File:          bipobs.h
 *     $Workfile:bipobs.h$ for XTNDAccess Blue SDK, Version 1.4
 *     $Revision: #1 $
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * Created:       August 6, 2002
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
 * Apr 11 2008 mbj06032
 * [MAUI_00653579] [BT BIP ADP] add $Log: $ to bip adp files
 * 
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 *
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
typedef struct _BipObStoreEntry {
    ListEntry       node;
    void           *app;
    int		        ocx;
    U32             objLen;
    U32             offset;
    U16             nameLen;
    U8              name[GOEP_MAX_UNICODE_LEN*2];
    U8              typeLen;
    U8              type[GOEP_MAX_TYPE_LEN];
    U32             bipOp;
    ObexRespCode    code;
} BipObStoreEntry;

/*---------------------------------------------------------------------------
 *
 * BipObStoreHandle Structure
 */
typedef struct _BipObStoreEntry *BipObStoreHandle;


/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_AppendName(BipObStoreHandle obs,
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
void BIPOBS_AppendName(BipObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_AppendType(BipObStoreHandle  obs, 
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
void BIPOBS_AppendType(BipObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_Deinit()
 *
 * Description:   Deinitialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void BIPOBS_Deinit(void);
#define BIPOBS_Deinit()

/*---------------------------------------------------------------------------
 *
 * Prototype:     char* BIPOBS_GetNameAscii(BipObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define BIPOBS_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 BIPOBS_GetObjectLen(void *obs)
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
U32 BIPOBS_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL BIPOBS_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
BOOL BIPOBS_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BipObStoreHandle BIPOBS_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
BipObStoreHandle BIPOBS_New(void *app);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BIPOBS_Close(BipObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obshPtr - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode BIPOBS_Close(BipObStoreHandle *obshPtr);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_SetObjectLen(BipObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define BIPOBS_SetObjectLen(obs, len) ((obs)->objLen = (len))

/*---------------------------------------------------------------------------
 *
 * Prototype:   ObexRespCode BIPOBS_Read(void *obs, U8* buff, U16 len)
 *
 * Description: Read data from the Initiator or Responder.
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
ObexRespCode BIPOBS_Read(void *obs, U8* buff, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BIPOBS_Write(void *obs, U8* buff, U16 len)
 *
 * Description:   Write data to the Initiator or Responder.
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
ObexRespCode BIPOBS_Write(void *obs, U8 *buff, U16 len);
#endif /* __BIPOBS_H */

