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

#ifndef __OBSTOREH
#define __OBSTOREH

/****************************************************************************
 *
 * File:          obstore.h
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * Created:       May 15, 1997
 *
 * Version:       MTObex 3.4
 *
 * Copyright 1997-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#include <stdio.h>
#include <obexerr.h>
#include <utils.h>
#include <obex.h>

/****************************************************************************
 *
 * These structure definitions, functions and constants are used by the 
 * OBEX Protocol and must be provided by all Object Store implementations.
 *
 ****************************************************************************/
 
/****************************************************************************
 *
 * Structure and Constants Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ObStoreHandle Structure
 */
typedef struct _ObStoreEntry *ObStoreHandle;

/*---------------------------------------------------------------------------
 *
 * Constants used with OBSTORE_UpdatePathUnicode() function
 */
#define OBS_UPU_NONE            0x00
#define OBS_UPU_BACKUP          0x01
#define OBS_UPU_DONT_CREATE     0x02

/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_AppendNameAscii(ObStoreHandle obs, 
 *                                             const char    *name, 
 *                                             U16           len)
 *
 * Description:   Append the ASCII string to the name. 
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                name - pointer to ASCII string to append
 *
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void OBSTORE_AppendNameAscii(ObStoreHandle obs, const char *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_AppendNameUnicode(ObStoreHandle obs, 
 *                                                       const U8     *name, 
 *                                                       U16           len)
 *
 * Description:   Append the unicode string to the name. A state machine is 
 *                used so the unicode string does not have to be an even 
 *                number of bytes.
 *
 * Parameters:    obs - handle of object store entry.
 *                name - pointer to Unicode string to append
 *                len - length in bytes of string to append
 *
 * Returns:       OBRC_SUCCESS      - Appended Name successfully.
 *                OBRC_UNAUTHORIZED - Unicode Name did not fit in buffer.
 */
ObexRespCode OBSTORE_AppendNameUnicode(ObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_AppendPathUnicode(const U8  *path,
 *                                                       U16        len,
 *                                                       BOOL       first )
 *
 * Description:   Append the Unicode path to the current OBEX Server path.
 *
 * Parameters:    path  - Unicode path string.
 *                len   - Length of the path.
 *                first - Start of a new path.
 *
 * Returns:       OBRC_SUCCESS      - The path was successfully appended.
 *                OBRC_UNAUTHORIZED - Unable to append the path.
 */
ObexRespCode OBSTORE_AppendPathUnicode(const U8 *path, U16 len, BOOL first);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_AppendPathAscii(const U8  *path,
 *                                                       U16        len,
 *                                                       BOOL       first )
 *
 * Description:   Append the Ascii path to the current OBEX Server path.
 *
 * Parameters:    path  - Unicode path string.
 *                len   - Length of the path.
 *                first - Start of a new path.
 *
 * Returns:       OBRC_SUCCESS      - The path was successfully appended.
 *                OBRC_UNAUTHORIZED - Unable to append the path.
 */
ObexRespCode OBSTORE_AppendPathAscii(const U8 *path, U16 len, BOOL first);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_AppendType(ObStoreHandle  obs, 
 *                                        const U8      *type,
 *                                        U16            len )
 *
 * Description:   Append the ASCII string to the object type. 
 *
 * Parameters:    obs - handle of object store entry.
 *                type- pointer to ASCII string to append
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void OBSTORE_AppendType(ObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_CancelPathUpdate(void)
 *
 * Description:   Cancel a path update.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void OBSTORE_CancelPathUpdate(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Close(ObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode OBSTORE_Close(ObStoreHandle *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Create(ObStoreHandle *obs, BOOL useInbox)
 *
 * Description:   Open the specified object store item for writing. 
 *
 * Parameters:    useInbox: TRUE  - Create the object from within the inbox.
 *                          FALSE - Create object in initial directory.
 *
 *                obs - Handle of object store item to create.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_NOT_FOUND - Could not create object store item.
 */

ObexRespCode OBSTORE_Create(ObStoreHandle obs, BOOL useInbox);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_Deinit()
 *
 * Description:   Deinitialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void OBSTORE_Deinit(void);
#define OBSTORE_Deinit()

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Delete(ObStoreHandle *obs)
 *
 * Description:   Delete the object and free the storage.
 *
 * Parameters:    obs - handle to object store entry to delete. 
 *
 * Returns:       OBRC_NOT_FOUND - File could not be deleted.
 *                OBRC_SUCCESS   - File was deleted.
 */
ObexRespCode OBSTORE_Delete(ObStoreHandle *obs, BOOL useInbox);

/*---------------------------------------------------------------------------
 *
 * Prototype:     char* OBSTORE_GetNameAscii(ObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define OBSTORE_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 *
 * Prototype:     char* OBSTORE_GetTypeAscii(ObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's type in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII type.
 */
#define OBSTORE_GetTypeAscii(obs) ((obs)->type)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 OBSTORE_GetNameLenUnicode(ObStoreHandle obs)
 *
 * Description:   Get the Unicode length of the object stores entries
 *                name including the terminator.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Number of bytes in the Unicode name.
 */
#define OBSTORE_GetNameLenUnicode(_OBS) (((_OBS)->nameLen + 1) * 2)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 OBSTORE_GetObjectLen(void *obs)
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
U32 OBSTORE_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL OBSTORE_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
BOOL OBSTORE_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObStoreHandle OBSTORE_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
ObStoreHandle OBSTORE_New(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Open(ObStoreHandle obs, BOOL useInbox)
 *
 * Description:   Open an object store item for reading. If no name has been
 *                set then open the default object.
 *
 * Parameters:    obs - pointer to object store entry
 *
 *                useInbox: TRUE open the object from within the inbox.
 *                          FALSE open object in current directory.
 *
 * Returns:       OBRC_SUCCESS indicates that the open is successful. 
 *                OBRC_NOT_FOUND indicates failure. 
 */
ObexRespCode OBSTORE_Open(ObStoreHandle obs, BOOL useInbox);

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_SetOffset(void *obs, 
 *                                               U32  offset)
 *
 * Description:   Sets the offset into the current object store entry.  This
 *                is used to move the value of the file pointer backwards in 
 *                the case that data needs to be retransmitted.
 *
 * Parameters:    obs - pointer to the object store entry
 *                offset - offset into the current object
 *
 * Returns:       OBRC_SUCCESS indicates that the save is successful. 
 *                OBRC_NOT_FOUND indicates failure to save. 
 */
ObexRespCode OBSTORE_SetOffset(void *obs, U32 offset);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Read(void *obs, U8* buff, U16 len)
 *
 * Description:   Read data from the object store entry.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to location where data is read into.
 *
 *                len - length in bytes of data to read
 *
 * Returns:       OBRC_SUCCESS      - Read was successful.
 *                OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode OBSTORE_Read(void *obs, U8 *buff, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 OBSTORE_ReadNameUnicode(ObStoreHandle obs
 *                                            U8           *buffer,
 *                                            U16           len,
 *                                            BOOL          first)
 *
 * Description:   Copy the Object Store item's name into the provided buffer
 *                in Unicode. Copied name is null terminated.
 *
 * Parameters:    obs - handle of object store entry.
 *                buffer - buffer to receive the name.
 *                len - length of the buffer in bytes.
 *                first - TRUE means start reading at the beginning of the name.
 *                        FALSE means continue from where the last read ended.
 *
 * Returns:       Number of bytes copied including the null terminator.
 */
U16 OBSTORE_ReadNameUnicode(ObStoreHandle obs, U8 *buffer, U16 len, BOOL first);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_ResetPath(void)
 *
 * Description:   Reset the path to the inbox root.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void OBSTORE_ResetPath(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_SetNameDefault(ObStoreHandle obs)
 *
 * Description:   Set the name of the Object store entry to the default name
 *                for a received object that has no name.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       void 
 */
void OBSTORE_SetNameDefault(ObStoreHandle obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_SetObjectLen(ObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define OBSTORE_SetObjectLen(obs, len) ((obs)->fileLen = (len))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_UpdatePath(U8     flags
 *                                                BOOL   update)
 *
 * Description:   Apply the path flags or the updated server path.
 *
 * Parameters:    flags  - SetPath Flags to apply.
 *                update - Apply current Path (built with AppendPathUnicode).
 *
 * Returns:       OBRC_SUCCESS  - The update was successfully applied.
 *                OBRC_NOT_FOUND - The dir could not be created/switched to.
 *                OBRC_UNAUTHORIZED - Refused to apply the update.
 */
ObexRespCode OBSTORE_UpdatePath(U8 flags, BOOL update);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Write(void *obs, U8* buff, U16 len)
 *
 * Description:   Write data to the object store entry.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to data to write
 *
 *                len - length in bytes of data to write
 *
 * Returns:       OBRC_SUCCESS      - Write was successful.
 *                OBRC_UNAUTHORIZED - Write failed. 
 */
ObexRespCode OBSTORE_Write(void *obs, U8 *buff, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_SetUiContext(ObStoreHandle obs, U32 context)
 *
 * Description:   Set the UI context handle for the object. This context
 *                handle is passed back to to the UI to so it can update the
 *                progress as an object is transferred.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                context - handle of UI object
 *
 * Returns:       void
 */
void OBSTORE_SetUIContext(ObStoreHandle obs, U32 uiContext);

#define OBSTORE_SetUIContext(_OBS, _UIC)  (_OBS)->uiContext = (_UIC)

/****************************************************************************
 *
 * Object Store Definitions
 *
 * Definitions used internally by the object store.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Structure and Constants Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Size Constants used by ObStoreEntry
 */
#define OBS_MAX_NAME_LEN    128
#define OBS_MAX_PATH_LEN    OBS_MAX_NAME_LEN
#define OBS_MAX_TYPE_LEN    32
#define OBS_MAX_NUM_ENTRIES 4

/*---------------------------------------------------------------------------
 *
 * Flag Constants used in ObStoreEntry "flags" field
 */
#define OBSEF_ANU_UNICODE_ODD  0x01   /* AppendNameUnicode() position flag */
#define OBSEF_RNU_UNICODE_ODD  0x02   /* ReadNameUnicode() position flag */
#define OBSEF_IN_INBOX         0x04   /* File is in the Inbox Directory */
#define OBSEF_DEFAULT_OBJECT   0x08   /* Object name set to default name */ 

/****************************************************************************
 *
 * Data structures 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ObStoreEntry Structure
 */
typedef struct _ObStoreEntry {
    ListEntry   node;
    FILE       *fp;
    char       *tmpFile;
    char       *curNamePos;
    char        name[OBS_MAX_NAME_LEN];
    char        type[OBS_MAX_TYPE_LEN];
    U32         amount;
    U32         uiContext;
    U32         fileLen;
    U16         nameLen;
    U8          typeLen;
    U8          flags;
} ObStoreEntry;


#endif /* __OBSTOREH */

