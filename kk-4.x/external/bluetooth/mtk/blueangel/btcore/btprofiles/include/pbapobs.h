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
 * File:          pbapobs.h
 *     $Workfile:pbapobs.h$ for iAnywhere Blue SDK, PBAP Version 1.1.2
 *     $Revision: #1 $
 *
 * Description:   This file specifies the OBEX object store interface.
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

#ifndef __PBAPOBS_H
#define __PBAPOBS_H

#ifdef __BT_PBAP_PROFILE__
#include "obexerr.h"
#include "utils.h"
#include "obex.h"

/*---------------------------------------------------------------------------
 *
 * Phonebook Access Profile Object Store (PBAPOBS) Layer
 *
 *     These functions are used internally by PBAP to handle the OBEX object
 *     store requirements.  However, PBAP does receive information in
 *     response to the object store function calls.  Specifically, 
 *     PBAPOBS_Read, PBAPOBS_ReadFlex, and PBAPOPBS_Write will cause the 
 *     PBAP_EVENT_DATA_REQ and PBAP_EVENT_DATA_IND events to occur.  These
 *     events allow the PBAP application to deal with data requests and 
 *     data indications as they occur. More information on these events is
 *     available in pbap.h.
 *
 */

/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * OBS_MAX_NUM_ENTRIES Constant
 *
 *  Defines the number of object store entries supported.
 *
 *  The default value is 4.
 */
#ifndef PBAP_OBS_MAX_NUM_ENTRIES
#define PBAP_OBS_MAX_NUM_ENTRIES 4
#endif

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PbapObStoreEntry structure
 *
 *  Phonebook Access Profile object store entry.
 */
typedef struct _PbapObStoreEntry {
    ListEntry      node;
    void           *app;
    U32             objLen;
    U32             offset;
    U16             nameLen;
    U8              name[GOEP_MAX_UNICODE_LEN];
    U8              typeLen;
    U8              type[GOEP_MAX_TYPE_LEN];
    U32             pbapOp;
    ObexRespCode    code;
} PbapObStoreEntry;


/*---------------------------------------------------------------------------
 * PbapObStoreHandle struct
 *
 *  Phonebook Access Profile object store handle.
 */
typedef struct _PbapObStoreEntry *PbapObStoreHandle;

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PBAPOBS_AppendName()
 *
 *     Append the ASCII string to the name.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 *     name - pointer to ASCII string to append
 *
 *     len - length in bytes of string to append
 *
 * Returns:
 *     None
 */
extern void PBAPOBS_AppendName(PbapObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 * PBAPOBS_AppendType()
 *
 *     Append the ASCII string to the object type.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 *     name - pointer to ASCII string to append
 *
 *     len - length in bytes of string to append
 *
 * Returns:
 *     None
 */
extern void PBAPOBS_AppendType(PbapObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 * PBAPOBS_GetNameAscii()
 *
 *     Return a pointer to the Object Store item's name in ASCII.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 * Returns:
 *     Pointer to NULL terminated ASCII name.
 */
char* PBAPOBS_GetNameAscii(PbapObStoreHandle obs);
#define PBAPOBS_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 * PBAPOBS_GetTypeAscii()
 *
 *     Return a pointer to the Object Store item's type in ASCII.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 * Returns:
 *     Pointer to NULL terminated ASCII type.
 */
char* PBAPOBS_GetTypeAscii(PbapObStoreHandle obs);
#define PBAPOBS_GetTypeAscii(obs) ((obs)->type)

/*---------------------------------------------------------------------------
 * PBAPOBS_GetObjectLen()
 *
 *     Get the length of an object store item. A function that performs 
 *     this behavior is REQUIRED by the OBEX Protocol.  This function fills 
 *     in the OBS_GetObjectLen field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 * Returns:
 *     The object length.
 */
extern U32 PBAPOBS_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 * PBAPOBS_Init()
 *
 *     Initialize the object store.
 *
 * Parameters:    
 *     None
 *
 * Returns:
 *     TRUE - object store was initialized successfully. 
 *     FALSE - Unable to create inbox.
 */
extern kal_bool PBAPOBS_Init(void);

/*---------------------------------------------------------------------------
 * PBAPOBS_New()
 *
 *     Get a handle to a blank object store item.
 *
 * Parameters:    
 *     app - pointer to the GOEP client or server using the object store handle.
 *
 * Returns:
 *     Handle of an object store entity. 0 means out of objects. 
 */
extern PbapObStoreHandle PBAPOBS_New(void *app);

/*---------------------------------------------------------------------------
 * PBAPOBS_Close()
 *
 *     Close an object store item.
 *
 * Parameters:    
 *     obshPtr - handle of object store entry.
 *
 * Returns:
 *     OBRC_SUCCESS - Operation was successful.
 *     OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *     OBRC_NOT_FOUND - Could not access object store item. 
 */
extern ObexRespCode PBAPOBS_Close(PbapObStoreHandle *obshPtr);

/*---------------------------------------------------------------------------
 * PBAPOBS_SetObjectLen()
 *
 *     Set the length of a created object store item.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 *     len - length of the object.
 *
 * Returns:
 *     None
 */
void PBAPOBS_SetObjectLen(PbapObStoreHandle obs, U32 len);
#define PBAPOBS_SetObjectLen(obs, len) ((obs)->objLen = (len))

/*---------------------------------------------------------------------------
 * PBAPOBS_Read()
 *
 *     Read a phonebook object from the Server. A function that performs 
 *     this behavior is REQUIRED by the OBEX Protocol. This function fills 
 *     in the OBS_Read field of the ObStoreFuncTable (obex.h). This 
 *     function will trigger the PBAP_EVENT_DATA_REQ event to indicate that 
 *     the PBAP server must retrieve the requested data from its phonebook 
 *     databases.
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 *     buff - pointer to location where data is read into.
 *
 *     len - length in bytes of data to read
 *
 * Returns:
 *     OBRC_SUCCESS      - Read was successful.
 *     OBRC_UNAUTHORIZED - Read failed.
 */
extern ObexRespCode PBAPOBS_Read(void *obs, U8* buff, U16 len);

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAPOBS_ReadFlex()
 *
 *     Read a phonebook object from the Server. The Pull Vcard Listing 
 *     operation will typically use this method, as it does not know the 
 *     size of the dynamic folder listing ahead of time.  A function that 
 *     performs this behavior is REQUIRED by the OBEX Protocol if 
 *     OBEX_DYNAMIC_OBJECT_SUPPORT is enabled.  This function fills in the 
 *     OBS_ReadFlex field of the ObStoreFuncTable (obex.h). This function 
 *     will trigger the PBAP_EVENT_DATA_REQ event to indicate that the PBAP 
 *     server must retrieve the requested data from its phonebook databases.
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 *     buff - pointer to location where data is read into.
 *
 *     len -  On input, contains the maximum length in bytes of 
 *            data to read. Before returning, the object store 
 *            must write the actual number of bytes read into "buff".
 *
 *     more - Before returning, the object store must write 
 *            "TRUE" or "FALSE" into the boolean referenced by 
 *            this parameter. TRUE indicates that there is more 
 *            data to read, FALSE indicates that all object data 
 *            has been read.
 *
 * Returns:
 *     OBRC_SUCCESS      - Read was successful.
 *     OBRC_UNAUTHORIZED - Read failed.
 */
extern ObexRespCode PBAPOBS_ReadFlex(void *obs, U8 *buff, U16 *len, kal_bool *more);
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */


#endif /* __BT_PBAP_NEW_PROFILE__ */
#endif /* __PBAPOBS_H */
