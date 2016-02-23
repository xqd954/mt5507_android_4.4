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

#ifndef __SDPH
#define __SDPH

/****************************************************************************
 *
 * File:
 *     $Workfile:sdp.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     Service Discovery Protocol component. 
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
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
 ****************************************************************************/

#include "sdp_adp.h"

/*---------------------------------------------------------------------------
 * Service Discovery Protocol (SDP) layer
 *
 *     The SDP layer is a "yellow pages" of services available on a device.
 *     With the SDP API, you can create and store "service records" for
 *     remote devices to find. After this is done, SDP handles
 *     all queries from remote devices automatically.
 *
 *     You can also query for the service records on connected remote
 *     devices. To do this, you create a query token, fill it with
 *     the parameters for your query, and pass it to SDP. SDP then
 *     handles the interactions with the remote device, returns the 
 *     query token to you, and helps you parse the results.
 *
 *     Note: unlike many other layers of the Bluetooth protocol,
 *     the SDP uses big-endian format, so more significant bytes are stored
 *     and transferred before less significant bytes.
 */


/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SDP_AddRecord()
 *
 *     Adds a service record to the SDP database.
 * 
 * Parameters:
 *     record - The record to add
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The record was added successfully.
 *
 *     BT_STATUS_FAILED - The record was already added.
 *
 *     BT_STATUS_INVALID_PARM - The record not added because it was
 *         improperly formed. It may contain attributes with duplicate
 *         IDs, or the attributes are not in ascending order.
 *         (XA_ERROR_CHECK only.)
 */
BtStatus SDP_AddRecord(SdpRecord *record);

/*---------------------------------------------------------------------------
 * SDP_RemoveRecord()
 *
 *     Removes a service record from the SDP database.
 *
 * Parameters:
 *     record - The record to remove
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is removed successfully.
 *
 *     BT_STATUS_FAILED - record does not exist in database.
 */
BtStatus SDP_RemoveRecord(SdpRecord *record);

#ifdef BT_FOR_NONE_MP_ONLY
/*---------------------------------------------------------------------------
 * SDP_LockRecord()
 *
 *     If no current SDP Queries are in progress and if the SDP record to
 *     update is a valid entry, lock SDP for record updating.  SDP will
 *     refuse any SDP queries via an SDP error response until the record has
 *     been updated (signaled by SDP_UnlockRecord()).  In order to guarantee 
 *     safe SDP record updates, this function should be called prior to 
 *     altering any SDP record.  SDP can only be locked one time, thus
 *     preventing simultaneous record updated by multiple tasks.
 *
 * Parameters:
 *     record - The record to be updated.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is valid, SDP is locked successfully.
 *
 *     BT_STATUS_FAILED - record is invalid, or SDP cannot be locked at 
 *          this time.
 *
 *     BT_STATUS_INVALID_PARM - The record is improperly formed. 
 *          (XA_ERROR_CHECK only.)
 */
BtStatus SDP_LockRecord(SdpRecord *record);

/*---------------------------------------------------------------------------
 * SDP_UnlockRecord()
 *
 *     Unlocks SDP from record updating, and once again allows SDP queries 
 *     to be handled normally.  Once this function is called, it is no longer 
 *     valid to alter any SDP record. If the unlocked record has changed, 
 *     its internal fields are updated appropriately. SDP must be locked 
 *     prior to calling this function.
 *
 * Parameters:
 *     record - The record to be updated.
 *
 *     updated - Indicates whether the record has actually changed.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is unlocked and updated successfully.
 *
 *     BT_STATUS_FAILED - record is invalid, or SDP is not already locked.
 *
 *     BT_STATUS_INVALID_PARM - The record is improperly formed. 
 *          (XA_ERROR_CHECK only.)
 */
BtStatus SDP_UnlockRecord(SdpRecord *record, BOOL updated);
#endif /* BT_FOR_NONE_MP_ONLY */

#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SDP_Query()
 *
 *     Performs an SDP query of a remote device's SDP database.
 *
 *     All appropriate fields in the SdpQueryToken must be filled in
 *     before calling SDP_Query, and an ACL connection must already be
 *     present with the remote device.
 *
 *     If this function returns BT_STATUS_PENDING, the token will
 *     be returned later to the callback.
 *
 *     If the callback event is SDEVENT_QUERY_RSP, the query succeeded and
 *     result parameters can be found in the results field of the token.
 *     However, the results buffer must be processed during the callback:
 *     the buffer is invalid after the callback returns.  SDP_ParseAttributes
 *     can be used to process the results buffer for specific values and to
 *     retrieve any continuation state information.
 *
 *     If the callback event is SDEVENT_QUERY_ERR, the remote device
 *     responded with an error code. This can be found in the token.
 *
 *     If the callback event is SDEVENT_QUERY_FAILED, an error prevented
 *     successful the query/response process. This can be caused by
 *     link failure, a failure of the remote device to respond, or
 *     if the query exceeds the server's L2CAP MTU size.
 *
 *     A successful query result may be incomplete. This is indicated
 *     when the continuation state information in the results buffer starts
 *     with a non-zero value. To retrieve more results for the same query,
 *     merely resubmit the query, which will have had the SdpQueryToken
 *     "contState" and "contStateLen" fields updated upon the last
 *     SDP_ParseAttributes call (Note: If SDP_ParseAttributes was not used
 *     to parse the response, the "contState" and "contStateLen" fields must
 *     be modified to contain the continuation state information).  If the 
 *     continuation state information in the results buffer starts with 0, 
 *     all results have been sent.
 *
 * Parameters:
 *     token - Describes the query. If this call returns BT_STATUS_PENDING,
 *         do not modify the token until a callback event returns it.
 *
 *     mode - Indicates the query mode. Use BSQM_FIRST for the
 *         initial query. Use BSQM_CONTINUE only if the
 *         ContinuationState from the initial query's result is non-0.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started successfully. The
 *         callback in the token will be sent result.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because there is
 *         no ACL connection to the remote device specified in the token.
 *     
 *     BT_STATUS_NO_RESOURCES - The operation failed because the
 *         query was found to exceed the MTU size of the remote device.
 *     
 *     BT_STATUS_BUSY - The operation failed because connection is in park 
 *         mode.
 *
 *     BT_STATUS_INVALID_TYPE - The operation failed because the type
 *         field in the token is invalid. (XA_ERROR_CHECK only.)
 *     
 *     BT_STATUS_INVALID_PARM - The operation failed because the
 *         parms field in the token is not valid. (XA_ERROR_CHECK only.)
 *
 *     BT_STATUS_FAILED - The operation failed because the fields in
 *         the token are not properly set or the token is already
 *         involved in a query. (XA_ERROR_CHECK only.)
 */
BtStatus SDP_Query(SdpQueryToken *token, SdpQueryMode mode);

BtStatus SDP_QueryCancel(SdpQueryToken *token);
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * SDP_GetU16()
 *
 *     Reads a 16-bit integer from the SDP results buffer.
 *
 * Parameters:
 *     buff - The location in the results buffer of a 16-bit integer
 *
 * Returns:
 *     A 16-bit value.
 */
U16 SDP_GetU16(U8 *buff);

#define SDP_GetU16(buff) BEtoHost16((buff))

/*---------------------------------------------------------------------------
 * SDP_GetU32()
 *
 *     Reads a 32-bit integer from the SDP results buffer.
 *
 * Parameters:
 *     buff - The location in the results buffer of a 32-bit integer
 *
 * Returns:
 *     A 32-bit value.
 */
U32 SDP_GetU32(U8 *buff);

#define SDP_GetU32(buff) BEtoHost32((buff))

/*---------------------------------------------------------------------------
 * SDP_PutU16()
 *
 *     Writes a 16-bit integer into an SDP buffer.
 *
 * Parameters:
 *     buff - The write location
 *
 *     val  - The integer to write
 */
void SDP_PutU16(U8 *buff, U16 val);

#define SDP_PutU16(buff,val) StoreBE16((buff),(val))

/*---------------------------------------------------------------------------
 * SDP_PutU32()
 *
 *     Writes a 32-bit integer into an SDP buffer.
 *
 * Parameters:
 *     buff - The write location
 *
 *     val  - The integer to write
 */
void SDP_PutU32(U8 *buff, U32 val);

#define SDP_PutU32(buff,val) StoreBE32((buff),(val))

/*---------------------------------------------------------------------------
 * SDP_ParseDataElement()
 *
 *     Determines the offset and number of bytes in a Data Element. Note that
 *     offset plus the return of this function is the total size of the
 *     Data Element.
 *
 * Parameters:
 *     buff - Pointer to a Data Element
 *
 *     offset - On function exit: contains the number of bytes between buff 
 *         and the start of the Data Element's data field.
 *
 * Returns:
 *     The length of the Data Element's data field.
 */
U32 SDP_ParseDataElement(U8 *buff, U16 *offset);

#define SDP_ParseDataElement(buff,offset) SdpParseElement((buff),(offset))

/*---------------------------------------------------------------------------
 * SDP_UuidCmp()
 *
 *     Compares two Bluetooth UUIDs. UUIDs must be 2, 4, or 16 bytes in
 *     length. UUIDs of different lengths can be compared with this function.
 *
 * Parameters:
 *     uuid1 - Pointer to a buffer holding the first UUI
 *
 *     len1 - Number of bytes of the first UUI
 *
 *     uuid2 - Pointer to a buffer holding the second UUI
 *     
 *     len2 - Number of bytes of the second UUI
 *
 * Returns:
 *     TRUE - UUIDs are equal
 *
 *     FALSE - UUIDS are not equal.
 */
BOOL SDP_UuidCmp(U8 *uuid1, U16 len1, U8 *uuid2, U16 len2);

#define SDP_UuidCmp(uuid1,len1,uuid2,len2) SdpUuidCmp((uuid1), (len1), \
                                                      (uuid2), (len2))

/*---------------------------------------------------------------------------
 * SDP_GetElemType()
 *
 *     Returns a Data Element's type.
 *
 * Parameters:
 *     header - The first byte of a Data Element
 *
 * Returns:
 *     The type of the Data Element.
 */
SdpDataElemType SDP_GetElemType(U8 header);

#define SDP_GetElemType(header) ((header) & DETD_MASK)

/*---------------------------------------------------------------------------
 * SDP_GetElemSize()
 *
 *     Returns a Data Element's size descriptor.
 *
 * Parameters:
 *     header - The first byte of a Data Element
 *
 * Returns:
 *     The size characteristic of the Data Element.
 */
SdpDataElemSize SDP_GetElemSize(U8 header);

#define SDP_GetElemSize(header) ((header) & DESD_MASK)

/*---------------------------------------------------------------------------
 * SDP_GetElemTypeP()
 *
 *     Returns a Data Element's type.
 *
 * Parameters:
 *     buffer - Pointer to the beginning of a Data Element
 *
 * Returns:
 *     The type of the Data Element.
 */
SdpDataElemType SDP_GetElemTypeP(U8 *buffer);

#define SDP_GetElemTypeP(buffer) (((buffer)[0]) & DETD_MASK)

/*---------------------------------------------------------------------------
 * SDP_GetElemSizeP()
 *
 *     Returns a Data Element's size descriptor.
 *
 * Parameters:
 *     buffer - Pointer to the beginning of a Data Element
 *
 * Returns:
 *     The size characteristic of the Data Element.
 */
SdpDataElemSize SDP_GetElemSizeP(U8 *buffer);

#define SDP_GetElemSizeP(buffer) (((buffer)[0]) & DESD_MASK)

/*---------------------------------------------------------------------------
 * SDP_VerifyDataElement()
 *
 *     Verifies that a Data Element is properly formed. All size descriptors
 *     in the element are checked to make sure that the complete Data Element
 *     fits into the indicated length.
 *
 *     In the case of Data Element Sequences or Alternates, this
 *     function is called recursively. The parameter "maxLevel" is used
 *     to stop the recursion in the case of bad data. "maxLevel" must
 *     be large enough to handle the complete Data Element. For example,
 *     a simple data element such as DETD_UINT would need a "maxLevel" of 1,
 *     but a Data Element Sequence of UUIDs would need a "maxLevel" of 2.
 * 
 * Parameters:
 *     data - Pointer to a buffer containing a Data Element.
 *
 *     len - Length (in bytes) of the entire data element. The size
 *         of the Data Element must be less than or equal to this parameter
 *         or this function will fail.
 *
 *     maxLevel - The maximum level of recursion. Must be at least 1.
 *
 * Returns:
 *     TRUE - if the data element is properly formed
 *
 *     FALSE - data element not properly formed, or "maxLevel" reached
 *         0 during recursion.
 */
BOOL SDP_VerifyDataElement(U8 *data, U16 len, U8 maxLevel);

#define SDP_VerifyDataElement(data,len,maxLevel) \
             SdpVerifyDataElement(data,len,maxLevel)

#if (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED)
/*---------------------------------------------------------------------------
 * SDP_ParseAttributes()
 *
 *     Parses an SDP Query response buffer for an Attribute ID and optionally 
 *     a UUID within that Attribute ID.  For example, to get the RFCOMM 
 *     channel from an SDP response you would provide AID_PROTOCOL_DESC_LIST
 *     as the Attribute ID and PROT_RFCOMM as the UUID.  
 *
 *     The length of the Attribute ID or UUID result value is returned along 
 *     with a pointer to the beginning of the result value. For example, 
 *     imagine the following hex data was returned in an SDP response:
 *     09 00 04 35 05 19 00 03 08 01.  The first 3 bytes indicate the
 *     AID_PROTOCOL_DESC_LIST (0x0004).  0x35 0x05 is a list of 5 elements. The
 *     next 3 bytes indicate the PROT_RFCOMM (0x0003) UUID.  0x08 is a header
 *     describing the result (in this case a 1-byte UINT).  The RFCOMM channel 
 *     is 0x01.  If you requested the RFCOMM channel, you would be returned a 
 *     length of 1 along with a pointer to the RFCOMM channel (0x01).
 *  
 *     This function should be called during the SDEVENT_QUERY_RSP event, 
 *     since the results buffer is only valid during this callback.
 *
 *     In the case of a continuation state, additional SDP Queries must be 
 *     issued during the context of the callback, or the L2CAP connection
 *     may be disconnected, due to a lack of SDP Queries to process.  If the 
 *     L2CAP connection goes down, any continuation state that was returned is
 *     no longer valid and the SDP Queries will have to be reissued.
 *
 *     This function will properly parse ServiceAttribReq and 
 *     ServiceSearchAttribReq queries.
 *
 *     RECOMMENDATIONS
 *     -  Parse attributes in ascending order since this is how they are
 *        returned.  Failure to do so may result in a failure to find an 
 *        attribute, and may force reparsing of the entire set of SDP 
 *        result buffers.  Note that in the case of a continuation state 
 *        where all of the SDP results buffers might not be available, this 
 *        would dictate reissuing the original SDP query/queries in order 
 *        to retrieve the result buffers.
 *
 *     -  The BSQM_RESUME mode facilitates quicker SDP parsing by starting where
 *        the last SDP parsing call finished and also enables the retrieval of
 *        the same attribute from multiple SDP records.  However, if you query for
 *        attributes out of order, you may query for an attribute that you have 
 *        already passed in a previous SDP results buffer.  A failure to find 
 *        an attribute may require the reparsing of the entire set of SDP result
 *        buffers. 
 *
 * Requires:
 *     SDP_PARSING_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     token - SDP query token data structure that also contains the SDP
 *         parsing data fields.  Some of this structure must be filled in
 *         prior to use (see SdpQueryToken).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Parsing was successful and a result value and 
 *         length was returned. 
 * 
 *         If additional unparsed data exists (see the token->remainBuff 
 *         and token->remainLen fields), additional parsing can be issued 
 *         from the point where parsing ended last (see SdpParsingMode).  
 *         However, it should be known that SDP attributes are returned 
 *         in ascending order.  For best results, attempt to parse 
 *         attributes in ascending order.  This will prevent needing to 
 *         reparse the entire response for each attribute.  
 *
 *     BT_STATUS_FAILED - Parsing encountered an error or the attribute
 *         requested was not found.  In this case, the internal parsing 
 *         state will be restored to its status prior to the failed parse.
 *
 *     BT_STATUS_SDP_CONT_STATE - A continuation state exists (see the
 *         token->contState and token->contStateLen fields).  In the 
 *         case that a continuation state occurs in the middle
 *         of parsing the result value, the partial results will be 
 *         returned (see the token->totalValueLen, token->availValueLen,
 *         and token->valueBuff fields).  In this case, the partial results 
 *         must be saved until the rest of the results are retrieved from 
 *         additional SDP queries.  This is the only situation where the
 *         result value fields will be valid during this return code.
 *         Additional SDP Queries may be issued to retrieve the remainder 
 *         of the SDP response.
 *
 *     token - some fields are filled in by SDP (see SdpQueryToken).
 */
BtStatus SDP_ParseAttributes(SdpQueryToken *token);
#endif /* (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED) */

/****************************************************************************
 *
 * Section: SDP Macros 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP_ATTRIBUTE()
 *
 *     Macro that formats an SdpAttribute structure using the supplied Attribute ID 
 *     and Attribute value. This macro is very useful when formatting the
 *     SdpAttribute structures used to form the attributes in an SDP Record.
 *
 * Parameters:
 *     attribId - SdpAttributeId value (see the AID_ values).
 *     attrib - Array containing the attribute value.
 */
#define SDP_ATTRIBUTE(attribId, attrib)                     \
          { attribId,           /* Attribute ID */          \
            sizeof(attrib),     /* Attribute Size */        \
            attrib,             /* Attribute Value */       \
            0x0000 }    /* Flag - For Internal Use */

/*---------------------------------------------------------------------------
 * SDP_ATTRIBUTE_ARM()
 *
 *     Macro that formats an SdpAttribute structure using the supplied Attribute ID 
 *     and Attribute size. This macro is very useful when formatting the
 *     SdpAttribute structures used to form the attributes in an SDP Record.
 *
 *     This macro is used for ARM compilers that cannot set the attribute value
 *     using a pointer to ROM. The pointer must be set later at run-time after
 *     the structure is copied to RAM.
 *
 * Parameters:
 *     attribId - SdpAttributeId value (see the AID_ values).
 *     attrib - Array containing the attribute value (used to set size only.)
 */
#define SDP_ATTRIBUTE_ARM(attribId, attrib)                             \
          { attribId,           /* Attribute ID */                      \
            sizeof(attrib),     /* Attribute Size */                    \
            0,                  /* Attribute Value - to be set later */ \
            0x0000 }    /* Flag - For Internal Use */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_8BIT()
 *
 *     Macro that forms a Data Element Sequence header using the supplied 8-bit
 *     size value.  Data Element Sequence headers are used in SDP Record Attributes
 *     and SDP Queries. Notice that this macro only forms the header portion
 *     of the Data Element Sequence. The actual Data Elements within this
 *     sequence will need to be formed using other SDP macros.
 *
 * Parameters:
 *     size - 8-bit size of the Data Element Sequence.
 */
#define SDP_ATTRIB_HEADER_8BIT(size)                                     \
            DETD_SEQ + DESD_ADD_8BITS,      /* Type & size index 0x35 */ \
            size        /* 8-bit size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_16BIT()
 *
 *     Macro that forms a Data Element Sequence header using the supplied 16-bit
 *     size value.  Data Element Sequence headers are used in SDP Record Attributes
 *     and SDP Queries. Notice that this macro only forms the header portion
 *     of the Data Element Sequence. The actual Data Elements within this
 *     sequence will need to be formed using other SDP macros.
 *
 * Parameters:
 *     size - 16-bit size of the Data Element Sequence.
 */
#define SDP_ATTRIB_HEADER_16BIT(size)                                     \
            DETD_SEQ + DESD_ADD_16BITS,      /* Type & size index 0x36 */ \
            (U8)(((size) & 0xff00) >> 8),    /* Bits[15:8] of size */     \
            (U8)((size) & 0x00ff)       /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_32BIT()
 *
 *     Macro that forms a Data Element Sequence header using the supplied 32-bit
 *     size value.  Data Element Sequence headers are used in SDP Record Attributes
 *     and SDP Queries. Notice that this macro only forms the header portion
 *     of the Data Element Sequence. The actual Data Elements within this
 *     sequence will need to be formed using other SDP macros.
 *
 * Parameters:
 *     size - 32-bit size of the Data Element Sequence.
 */
#define SDP_ATTRIB_HEADER_32BIT(size)                                        \
            DETD_SEQ + DESD_ADD_32BITS,         /* Type & size index 0x37 */ \
            (U8)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size */    \
            (U8)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size */    \
            (U8)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size */     \
            (U8)((size) & 0x000000ff)   /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_UUID_16BIT()
 *
 *     Macro that forms a UUID Data Element from the supplied 16-bit UUID value. 
 *     UUID data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uuid - 16-bit UUID value (see the SC_ and PROT_ values).
 */
#define SDP_UUID_16BIT(uuid)                                              \
            DETD_UUID + DESD_2BYTES,         /* Type & size index 0x19 */ \
            (U8)(((uuid) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((uuid) & 0x00ff)       /* Bits[7:0] of UUID */

/*---------------------------------------------------------------------------
 * SDP_UUID_32BIT()
 *
 *     Macro that forms a UUID Data Element from the supplied 32-bit UUID value. 
 *     UUID data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uuid - 32-bit UUID value (see the SC_ and PROT_ values for 16-bit 
 *            values). 16-bit UUID values can be converted to 32-bit by 
 *            zero extending the 16-bit value.
 */
#define SDP_UUID_32BIT(uuid)                                                 \
            DETD_UUID + DESD_4BYTES,            /* Type & size index 0x1A */ \
            (U8)(((uuid) & 0xff000000) >> 24),  /* Bits[32:24] of UUID */    \
            (U8)(((uuid) & 0x00ff0000) >> 16),  /* Bits[23:16] of UUID */    \
            (U8)(((uuid) & 0x0000ff00) >> 8),   /* Bits[15:8] of UUID */     \
            (U8)((uuid) & 0x000000ff)   /* Bits[7:0] of UUID */

/*---------------------------------------------------------------------------
 * SDP_UUID_128BIT()
 *
 *     Macro that forms a UUID Data Element from the supplied 128-bit UUID value. 
 *     UUID data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uuid - 128-bit UUID value (see the SC_ and PROT_ values for 16-bit 
 *            values). 16-bit UUID values can be converted to 128-bit using 
 *            the following conversion: 128_bit_value = 16_bit_value * 2^96 +
 *            Bluetooth Base UUID.
 */
#define SDP_UUID_128BIT(uuid)                /* UUID must be a 16-byte array */ \
            DETD_UUID + DESD_16BYTES,        /* Type & size index 0x1C */       \
            uuid        /* 128-bit UUID */

/*---------------------------------------------------------------------------
 * SDP_UINT_8BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 8-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 8-bit UINT value.
 */
#define SDP_UINT_8BIT(uint)                                               \
            DETD_UINT + DESD_1BYTE,          /* Type & size index 0x08 */ \
            (U8)(uint)  /* 8-bit UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_16BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 16-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 16-bit UINT value.
 */
#define SDP_UINT_16BIT(uint)                                              \
            DETD_UINT + DESD_2BYTES,         /* Type & size index 0x09 */ \
            (U8)(((uint) & 0xff00) >> 8),    /* Bits[15:8] of UINT */     \
            (U8)((uint) & 0x00ff)       /* Bits[7:0] of UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_32BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 32-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 32-bit UINT value.
 */
#define SDP_UINT_32BIT(uint)                                                 \
            DETD_UINT + DESD_4BYTES,            /* Type & size index 0x0A */ \
            (U8)(((uint) & 0xff000000) >> 24),  /* Bits[31:24] of UINT */    \
            (U8)(((uint) & 0x00ff0000) >> 16),  /* Bits[23:16] of UINT */    \
            (U8)(((uint) & 0x0000ff00) >> 8),   /* Bits[15:8] of UINT */     \
            (U8)((uint) & 0x000000ff)   /* Bits[7:0] of UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_64BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 64-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 64-bit UINT value.
 */
#define SDP_UINT_64BIT(uint)                    /* UINT must be an 8-byte array */ \
            DETD_UINT + DESD_8BYTES,            /* Type & size index 0x0B */       \
            uint        /* 64-bit UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_128BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 128-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 128-bit UINT value.
 */
#define SDP_UINT_128BIT(uint)                   /* UINT must be a 16-byte array */ \
            DETD_UINT + DESD_16BYTES,           /* Type & size index 0x0C */       \
            uint        /* 128-bit UINT */

/*---------------------------------------------------------------------------
 * SDP_TEXT_8BIT()
 *
 *     Macro that forms a TEXT Data Element Header from the supplied 8-bit size 
 *     value. TEXT data elements are used in SDP Record Attributes and SDP Queries.
 *     Notice that this macro only forms the header portion of the TEXT Data 
 *     Element. The actual TEXT data within this data element will need to 
 *     be provided separately.
 *
 * Parameters:
 *     size - 8-bit size value.
 */
#define SDP_TEXT_8BIT(size)                                               \
            DETD_TEXT + DESD_ADD_8BITS,      /* Type & size index 0x25 */ \
            (U8)(size)  /* 8-bit size */

/*---------------------------------------------------------------------------
 * SDP_TEXT_16BIT()
 *
 *     Macro that forms a TEXT Data Element Header from the supplied 16-bit size 
 *     value. TEXT data elements are used in SDP Record Attributes and SDP Queries.
 *     Notice that this macro only forms the header portion of the TEXT Data 
 *     Element. The actual TEXT data within this data element will need to 
 *     be provided separately.
 *
 * Parameters:
 *     size - 16-bit size value.
 */
#define SDP_TEXT_16BIT(size)                                               \
            DETD_TEXT + DESD_ADD_16BITS,      /* Type & size index 0x26 */ \
            (U8)(((size) & 0xff00) >> 8),     /* Bits[15:8] of size */     \
            (U8)((size) & 0x00ff)       /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_TEXT_32BIT()
 *
 *     Macro that forms a TEXT Data Element Header from the supplied 32-bit size 
 *     value. TEXT data elements are used in SDP Record Attributes and SDP Queries.
 *     Notice that this macro only forms the header portion of the TEXT Data 
 *     Element. The actual TEXT data within this data element will need to 
 *     be provided separately.
 *
 * Parameters:
 *     size - 32-bit size value.
 */
#define SDP_TEXT_32BIT(size)                                                 \
            DETD_TEXT + DESD_ADD_32BITS,        /* Type & size index 0x27 */ \
            (U8)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size */    \
            (U8)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size */    \
            (U8)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size */     \
            (U8)((size) & 0x000000ff)   /* Bits[7:0] of size */

#define SDP_URL_8BIT(size)     \
    DETD_URL + DESD_ADD_8BITS, \
    (U8)(size)

/*---------------------------------------------------------------------------
 * SDP_BOOL()
 *
 *     Macro that forms a BOOL Data Element from the supplied 8-bit boolean value. 
 *     BOOL data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     value - 8-bit boolean value.
 */
#define SDP_BOOL(value)                                                   \
            DETD_BOOL + DESD_1BYTE,          /* Type & size index 0x28 */ \
            (U8)(value) /* Boolean value */

/****************************************************************************
 *
 * Internal function prototypes
 *
 ****************************************************************************/

U32 SdpParseElement(const U8 *p, U16 *offset);
BOOL SdpVerifyDataElement(const U8 *data, U16 len, U8 maxLevel);
BOOL SdpUuidCmp(const U8 *uuid1, U16 len1, const U8 *uuid2, U16 len2);
#ifdef __BT_4_0_BLE__
BOOL SdpIsBLEService(U16 uuid);
#endif
#endif /* __SDPH */ 

