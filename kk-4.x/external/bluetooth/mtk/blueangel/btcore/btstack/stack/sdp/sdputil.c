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

/***************************************************************************
 *
 * File:
 *     $Workfile:sdputil.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the utilities used by the Bluetooth 
 *     Service Discover Protocol Client and Server.
 *
 * Created:
 *     November 29, 1999
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

#include "btalloc.h"
#include "btrom.h"
#include "sys/sdpi.h"
#include "sys/mei.h"

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

static BOOL VerifyServiceSearchPattern(const U8 *searchPat, U16 maxLen);
static BOOL VerifyAttributeIdList(const U8 *list, U16 maxLen);
void SdpPromoteUUIDto32(const U8 *uuid, U16 len, U8 *buff);

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  SdpValidateQueryParms
 * DESCRIPTION
 *  Validate the query parameters.
 * PARAMETERS
 *  type                [IN]        
 *  parms               [IN]        
 *  len                 [IN]        
 *  contState           [IN]        
 *  contStateLen        [IN]        
 *  mode                [IN]        
 * RETURNS
 *  TRUE if valid. FALSE otherwise
 *****************************************************************************/
BOOL SdpValidateQueryParms(
        SdpQueryType type,
        const U8 *parms,
        U16 len,
        U8 *contState,
        U16 contStateLen,
        SdpQueryMode mode)
{
    U32 plen;
    U16 offset;
    U8 contLen;

    switch (type)
    {
        case BSQT_SERVICE_SEARCH_REQ:
            /*
             * Service Search request consists of a Service Search pattern, Maximum
             * service record count and a continuation state. First make sure that the
             * query is of minimum size.
             */
            if ((len + contStateLen) < SDP_SERVICE_SEARCH_MIN_SIZE)
            {
                goto err;
            }

            /* Verify the service search pattern. */
            if (mode == BSQM_DONT_CARE)
            {
                /* The Server passes the continuation state directly in the "parms" */
                if (!VerifyServiceSearchPattern(parms, (U16) (len - 3)))
                {
                    goto err;
                }
            }
            else
            {
                /* The Client passes the continuation state using "contState" */
                if (!VerifyServiceSearchPattern(parms, (U16) (len - 2)))
                {
                    goto err;
                }
            }

            /* Now get the size of the sequence */
            plen = SdpParseElement(parms, &offset);
            plen += offset;

            /* Verify that the maximum service record count is not 0 */
            if (BEtoHost16(parms + plen) == 0)
            {
                goto err;
            }

            /* Advance plen to the continuation state */
            plen += 2;
            break;

        case BSQT_ATTRIB_REQ:
            /*
             * A attribute request consists of a ServiceRecordHandle, a 
             * MaximumAttributeByteCount, an Attribute ID list, and a
             * continuation state. First verify that the length can support
             * the mimimal request. 
             */
            if ((len + contStateLen) < SDP_ATTRIB_REQ_MIN_SIZE)
            {
                goto err;
            }

            /* Now verify that the MaximumAttributeByteCount is in the proper range */
            offset = BEtoHost16(parms + 4);
            if (offset < 7)
            {
                goto err;
            }

            /* Verify that the attribute ID list is properly formed */
            if (mode == BSQM_DONT_CARE)
            {
                /* The Server passes the continuation state directly in the "parms" */
                if (!VerifyAttributeIdList(parms + 6, (U16) (len - 7)))
                {
                    goto err;
                }
            }
            else
            {
                /* The Client passes the continuation state using "contState" */
                if (!VerifyAttributeIdList(parms + 6, (U16) (len - 6)))
                {
                    goto err;
                }
            }

            /* Point to the continuation state. */
            plen = SdpParseElement(parms + 6, &offset);
            plen += offset + 6;
            break;

        case BSQT_SERVICE_SEARCH_ATTRIB_REQ:
            /*
             * A Service Search Attribute Request consists of a Service Search
             * Pattern, a Maximum Attribute Byte Count, an AttributeID List and
             * a continuation state. First check for a minimum size.
             */
            if ((len + contStateLen) < SDP_SERVICE_SEARCH_ATTRIB_MIN_SIZE)
            {
                goto err;
            }

            /* Verify the service search pattern. */
            if (mode == BSQM_DONT_CARE)
            {
                /* The Server passes the continuation state directly in the "parms" */
                if (!VerifyServiceSearchPattern(parms, (U16) (len - 3)))
                {
                    goto err;
                }
            }
            else
            {
                /* The Client passes the continuation state using "contState" */
                if (!VerifyServiceSearchPattern(parms, (U16) (len - 2)))
                {
                    goto err;
                }
            }

            /* Now get the size of the sequence */
            plen = SdpParseElement(parms, &offset);
            plen += offset;

            /* Now verify that the MaximumAttributeByteCount is in the proper range */
            offset = BEtoHost16(parms + plen);
            if (offset < 7)
            {
                goto err;
            }

            /* Verify that the attribute ID list is properly formed */
            if (mode == BSQM_DONT_CARE)
            {
                /* The Server passes the continuation state directly in the "parms" */
                if (!VerifyAttributeIdList(parms + plen + 2, (U16) ((len - plen) - 3)))
                {
                    goto err;
                }
            }
            else
            {
                /* The Client passes the continuation state using "contState" */
                if (!VerifyAttributeIdList(parms + plen + 2, (U16) ((len - plen) - 2)))
                {
                    goto err;
                }
            }

            /* Point to the continuation state */
            plen += SdpParseElement(parms + plen + 2, &offset);
            plen += offset + 2;
            break;

        default:
            /* Not a valid query so return FALSE */
            goto err;
    }

    /* Now check the continuation state. First get the continuation state */
    if (mode == BSQM_DONT_CARE)
    {
        /* The Server passes the continuation state directly in the "parms" */
        contLen = parms[plen];

        /* Verify the length */
        if ((U32) len < (U32) (plen + 1 + contLen))
        {
            goto err;
        }
    }
    else
    {
        /* The Client passes the continuation state using "contState" */
        contLen = *contState;

        /* Verify the continuation state length */
        if (contLen != (contStateLen - 1))
        {
            goto err;
        }

        /* Verify the length */
        if ((U32) len < (U32) (plen))
        {
            goto err;
        }
    }

    /* Verify that the continuation state is properly formed */
    switch (mode)
    {
        case BSQM_FIRST:
            if (contLen != 0)
            {
                goto err;
            }
            break;

        case BSQM_CONTINUE:
            if (contLen == 0)
            {
                goto err;
            }
            break;

        default:
            break;
    }

    /* Looks like every thing is ok */
    return TRUE;

  err:
    return FALSE;
}

/*---------------------------------------------------------------------------
 *            VerifyServiceSearchPattern()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Verify that the service search pattern is correctly formed. 
 *
 * Return:    TRUE - pattern is valid otherwise FALSE
 */


/*****************************************************************************
 * FUNCTION
 *  VerifyServiceSearchPattern
 * DESCRIPTION
 *  Verify that the service search pattern is correctly formed.
 * PARAMETERS
 *  searchPat       [IN]        
 *  maxLen          [IN]        
 * RETURNS
 *  TRUE - pattern is valid otherwise FALSE
 *****************************************************************************/
static BOOL VerifyServiceSearchPattern(const U8 *searchPat, U16 maxLen)
{
    U32 plen;
    U16 len;
    U16 pOffset;

    /* Verify that the pattern is of type Data Element Sequence */
    if ((searchPat[0] & DETD_MASK) != DETD_SEQ)
    {
        goto err;
    }

    /* Get the size of the sequence */
    plen = SdpParseElement(searchPat, &pOffset);

    /*
     * Verify that each element in the sequence is a UUID. Keep
     * a running total of the length according to the UUIDs
     */
    len = 0;
    while (len < plen)
    {
        /* First check the type */
        if ((searchPat[pOffset + len] & DETD_MASK) != DETD_UUID)
        {
            goto err;
        }

        /*
         * Determine if the size if valid. Only 2 byte, 4 byte or 16 byte
         * sizes are allowed for UUIDs
         */
        switch (searchPat[pOffset + len] & DESD_MASK)
        {
            case DESD_2BYTES:
                len += 3;
                break;

            case DESD_4BYTES:
                len += 5;
                break;

            case DESD_16BYTES:
                len += 17;
                break;

            default:
                goto err;
        }

        /* Make sure we don't read past the end of the buffer */
        if (len > maxLen)
        {
            goto err;
        }
    }
    /*
     * At this point the length calulated by the attributes should match
     * the length specified by the Data Element Sequence
     */
    if (len != plen)
    {
        goto err;
    }

    return TRUE;
  err:
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  VerifyAttributeIdList
 * DESCRIPTION
 *  Verify that the attribute ID list is correctly formed.
 * PARAMETERS
 *  list        [IN]        
 *  maxLen      [IN]        
 * RETURNS
 *  TRUE - list is valid otherwise FALSE
 *****************************************************************************/
static BOOL VerifyAttributeIdList(const U8 *list, U16 maxLen)
{
    U32 plen;
    U16 len;
    U16 pOffset;
    U16 lastID;
    U16 tmpID;
    U16 tmpID2;
    BOOL first;

    /* Verify that the list is of type Data Element Sequence */
    if ((list[0] & DETD_MASK) != DETD_SEQ)
    {
        goto err;
    }

    /* Get the size of the sequence */
    plen = SdpParseElement(list, &pOffset);

    /* Make sure it fits within maxLen */
    if ((plen + pOffset) > maxLen)
    {
        goto err;
    }

    /*
     * Verify that each element in the sequence is a 16 bit integer or a
     * 32-bit integer. Keep a running total of the size to compare to that
     * specified by the data element sequence.
     */
    len = 0;
    lastID = 0;
    first = TRUE;
    while (len < plen)
    {
        /* First check the type */
        if ((list[pOffset + len] & DETD_MASK) != DETD_UINT)
        {
            goto err;
        }
        /*
         * Determine if the size if valid. Only 2 byte, 4 byte 
         * sizes are allowed. Also verify that the attributes
         * IDs are in ascending order.
         */
        switch (list[pOffset + len] & DESD_MASK)
        {
            case DESD_2BYTES:
                tmpID = BEtoHost16((const U8*)(list + pOffset + len + 1));
                if (first)
                {
                    first = FALSE;
                }
                else if (tmpID <= lastID)
                {
                    /* Verify increasing order of attrib ID's */
                    goto err;
                }
                lastID = tmpID;
                len += 3;
                break;

            case DESD_4BYTES:
                tmpID = BEtoHost16((const U8*)(list + pOffset + len + 1));
                tmpID2 = BEtoHost16((const U8*)(list + pOffset + len + 3));
                if (first)
                {
                    first = FALSE;
                }
                else if ((tmpID <= lastID) || (tmpID2 <= lastID))
                {
                    /* Verify increasing order of attrib ID's */
                    goto err;
                }
                if (tmpID >= tmpID2)
                {
                    /* Verify increasing order of attrib ID range */
                    goto err;
                }
                lastID = tmpID2;
                len += 5;
                break;

            default:
                goto err;
        }
    }

    /*
     * At this point the length calulated by the attributes should match
     * the length specified by the Data Element Sequence
     */
    if (len != plen)
    {
        goto err;
    }

    return TRUE;
  err:
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  SdpVerifyDataElement
 * DESCRIPTION
 *  Verify that the data element is properly formed. It must fit
 *  within the length specified by len. This function is recursive
 *  so maxLevel keeps it from recursing forever on bad data. If
 *  maxLevel is reached then FALSE is returned.
 * PARAMETERS
 *  data            [IN]        
 *  len             [IN]        
 *  maxLevel        [IN]        
 * RETURNS
 *  TRUE - list is valid otherwise FALSE
 *****************************************************************************/
BOOL SdpVerifyDataElement(const U8 *data, U16 len, U8 maxLevel)
{
    U32 dLen, i;
    U16 offset;

    /* Check the level. If it is 0 then return FALSE */
    if (maxLevel == 0)
    {
        return FALSE;
    }

    /* Parse the data element sequence */
    dLen = SdpParseElement(data, &offset);

    /*
     * If the length of the data element exceeds the length allowed then return
     * FALSE.
     */
    if ((dLen + offset) > len)
    {
        return FALSE;
    }

    /* Don't allow 0-length attributes to be added */
    if (dLen == 0)
    {
        return FALSE;
    }

    /*
     * Validate the header by making sure the size descriptor is correct for
     * the given type descriptor.
     */
    switch (SDP_GetElemType(data[0]))
    {
        case DETD_NIL:  /* Nil, null type. Valid sizes are 0 */
        case DETD_BOOL: /* Boolean. Valid size 0 */
            if (SDP_GetElemSize(data[0]) != DESD_1BYTE)
            {
                return FALSE;
            }
            break;

        case DETD_UINT: /* Unsigned integer. Valid sizes 0,1,2,3,4 */
        case DETD_SINT: /* Signed integer. Valid sizes 0,1,2,3,4 */
            if (SDP_GetElemSize(data[0]) > DESD_16BYTES)
            {
                return FALSE;
            }
            break;

        case DETD_UUID: /* Universally Unique identifier. Valid size 1,2,4 */
            if ((SDP_GetElemSize(data[0]) != DESD_2BYTES) && (SDP_GetElemSize(data[0]) != DESD_4BYTES) &&
                (SDP_GetElemSize(data[0]) != DESD_16BYTES))
            {
                return FALSE;
            }
            break;

        case DETD_SEQ:  /* Data element sequence. Valid size 5,6,7 */
        case DETD_ALT:  /* Data element alternative. Valid size 5,6,7 */
            if (SDP_GetElemSize(data[0]) < DESD_ADD_8BITS)
            {
                return FALSE;
            }
            if (dLen != 0)
            {
                /* Now verify every element of the data element sequence */
                data += offset;
                i = 0;
                do
                {
                    /* See if the element is valid */
                    if (!SdpVerifyDataElement(data + i, (U16) (dLen - i), (U8) (maxLevel - 1)))
                    {
                        return FALSE;
                    }

                    /* Go to the next element */
                    i += SdpParseElement(data + i, &offset);
                    i += (U32) offset;

                } while (i < dLen);
                Assert(i == dLen);
            }
            break;

        case DETD_TEXT: /* Text string. Valid Size 5,6,7 */
        case DETD_URL:  /* Uniform Resource Locator. Valid size 5,6,7 */
            if (SDP_GetElemSize(data[0]) < DESD_ADD_8BITS)
            {
                return FALSE;
            }
            break;
    }
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  SdpFindRemDevWithSdpChannel
 * DESCRIPTION
 *  Return a pointer to a the device structure that contains the
 *  given Sdp Server channel or 0 if one is not found.
 * PARAMETERS
 *  channel     [IN]        
 *  type        [IN]        
 * RETURNS
 *  BtRemoteDevice*
 *****************************************************************************/
BtRemoteDevice *SdpFindRemDevWithSdpChannel(U16 channel, BtSdpType type)
{
    I8 i;
    U16 sdpChan = 0;
    BtRemoteDevice *remDev;

    /* Go through the RemoteDevice table looking for the first free entry. */
    for (i = 0; (remDev = MeEnumerateRemoteDevices(i)) != 0; i++)
    {
        Assert(remDev != 0);
        if (type == BST_SDP_SERVER)
        {
        #if SDP_SERVER_SUPPORT == XA_ENABLED
            if (remDev->sdpServInfo != 0)
            {
                sdpChan = remDev->sdpServInfo->channel;
            }
            else
            {
                sdpChan = 0;
            }
        #endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
        }
        else
        {
        #if SDP_CLIENT_SUPPORT == XA_ENABLED
            sdpChan = remDev->sdpClientChannel;
        #endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
        }
        if (sdpChan == channel)
        {
            return remDev;
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  SdpParseElement
 * DESCRIPTION
 *  Return the number of bytes in the value portion of the element
 *  and the offset to the value portion.
 * PARAMETERS
 *  p           [IN]        
 *  offset      [IN]        
 * RETURNS
 *  size of data element
 *****************************************************************************/
U32 SdpParseElement(const U8 *p, U16 *offset)
{
    U32 len = 0;

    if ((p[0] & DETD_MASK) == DETD_NIL)
    {
        *offset = 0;
        return 1;
    }

    switch (p[0] & DESD_MASK)
    {
        case DESD_1BYTE:
            *offset = 1;
            len = 1;
            break;

        case DESD_2BYTES:
            *offset = 1;
            len = 2;
            break;

        case DESD_4BYTES:
            *offset = 1;
            len = 4;
            break;

        case DESD_8BYTES:
            *offset = 1;
            len = 8;
            break;

        case DESD_16BYTES:
            *offset = 1;
            len = 16;
            break;

        case DESD_ADD_8BITS:
            *offset = 2;
            len = (U32) p[1];
            break;

        case DESD_ADD_16BITS:
            *offset = 3;
            len = (U32) BEtoHost16((const U8*)(p + 1));
            break;

        case DESD_ADD_32BITS:
            *offset = 5;
            len = BEtoHost32((const U8*)(p + 1));
            break;

        default:
            /* This case shouldn't be possible.  Complain if it somehow occurs */
            Assert(0);
            break;
    }
    return len;
}

#if (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  SdpParseElementHeader
 * DESCRIPTION
 *  Return the number of bytes in the header portion of the element.
 * PARAMETERS
 *  dataElem        [IN]        
 * RETURNS
 *  size of data element header
 *****************************************************************************/
U8 SdpParseElementHeader(U8 dataElem)
{
    if ((dataElem & DETD_MASK) == DETD_NIL)
    {
        return 0;
    }

    switch (dataElem & DESD_MASK)
    {
        case DESD_1BYTE:
        case DESD_2BYTES:
        case DESD_4BYTES:
        case DESD_8BYTES:
        case DESD_16BYTES:
            return 1;

        case DESD_ADD_8BITS:
            return 2;

        case DESD_ADD_16BITS:
            return 3;

        case DESD_ADD_32BITS:
            return 5;

        default:
            /* This case shouldn't be possible.  Complain if it somehow occurs */
            Assert(0);
            break;
    }
    return 0;
}
#endif /* (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED) */

/*****************************************************************************
 * FUNCTION
 *  SdpUuidCmp
 * DESCRIPTION
 *  Compare two UUIDs. The UUIDs are promoted if necessary. Valid
 *  lengths are 2, 4 or 16 bytes anything else will cause this
 *  function to return FALSE.
 * PARAMETERS
 *  uuid1       [IN]        
 *  len1        [IN]        
 *  uuid2       [IN]        
 *  len2        [IN]        
 * RETURNS
 *  TRUE if equal otherwise FALSE
 *****************************************************************************/
BOOL SdpUuidCmp(const U8 *uuid1, U16 len1, const U8 *uuid2, U16 len2)
{
    U8 scratch[16];

    OS_Report("SdpUuidCmp:uuid1, len=%d, (%2x %2x, %2x %2x, %2x %2x, %2x %2x)", len1, uuid1[0], uuid1[1], uuid1[2], uuid1[3], uuid1[4], uuid1[5], uuid1[6], uuid1[7]);
    OS_Report("SdpUuidCmp:uuid2, len=%d, (%2x %2x, %2x %2x, %2x %2x, %2x %2x)", len1, uuid2[0], uuid2[1], uuid2[2], uuid2[3], uuid2[4], uuid2[5], uuid2[6], uuid2[7]);
    /* If same size then just compare */
    if (len1 == len2)
    {
        return StrnCmp(uuid1, len1, uuid2, len2);
    }

    /*
     * They are not the same size so verify that both of them
     * * are legal size for promotion
     */
    if ((len1 != 2) && (len1 != 4) && (len1 != 16))
    {
        return FALSE;
    }

    if ((len2 != 2) && (len2 != 4) && (len2 != 16))
    {
        return FALSE;
    }

    /* If neither one is 16 bytes then promote to 32 bits and compare */
    if ((len1 < 16) && (len2 < 16))
    {
        SdpPromoteUUIDto32(uuid1, len1, scratch);
        SdpPromoteUUIDto32(uuid2, len2, scratch + 4);
        return StrnCmp(scratch, 4, scratch + 4, 4);
    }

    /* At this point one of the entities is 16 bytes and one is not */
    OS_MemCopy(scratch, (const U8*)BT_BASE_UUID, 16);

    /* If the first UUID is 16 bytes then promote the second to 16 and compare */
    if (len1 == 16)
    {
        Assert((len2 == 2) || (len2 == 4));
        SdpPromoteUUIDto32(uuid2, len2, scratch);
        return StrnCmp(uuid1, len1, scratch, 16);
    }

    /*
     * At this point the first is not 16 bytes and the second one is so promote the
     * the first one to 16 bytes and compare.
     */
    Assert(len2 == 16);
    Assert((len1 == 2) || (len1 == 4));
    SdpPromoteUUIDto32(uuid1, len1, scratch);
    return StrnCmp(scratch, 16, uuid2, len2);
}


/*****************************************************************************
 * FUNCTION
 *  SdpPromoteUUIDto32
 * DESCRIPTION
 *  Promote a 2 or 4 bytes UUID to 4 bytes by placing it into the
 *  given buffer.
 * PARAMETERS
 *  uuid        [IN]        
 *  len         [IN]        
 *  buff        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpPromoteUUIDto32(const U8 *uuid, U16 len, U8 *buff)
{
    Assert((len == 2) || (len == 4));

    if (len == 2)
    {
        buff[0] = 0;
        buff[1] = 0;
        buff[2] = uuid[0];
        buff[3] = uuid[1];
    }
    else
    {
        Assert(len == 4);
        OS_MemCopy(buff, uuid, 4);
    }
}

#ifdef __BT_4_0_BLE__
BOOL SdpIsBLEService(U16 uuid)
{
    switch (uuid)
    {
//    case BT_UUID_GATT_SERVICE_GAP:
//    case BT_UUID_GATT_SERVICE_GATT:
    case BT_UUID_GATT_SERVICE_ALERT:
    case BT_UUID_GATT_SERVICE_LINKLOSS:
    case BT_UUID_GATT_SERVICE_TXPOWER:
    case BT_UUID_GATT_SERVICE_TIME:
    case BT_UUID_GATT_SERVICE_TIMEUPDATE:
    case BT_UUID_GATT_SERVICE_DST:
    case BT_UUID_GATT_SERVICE_HEALTH:
    case BT_UUID_GATT_SERVICE_DEVICE:
    case BT_UUID_GATT_SERVICE_NETWORK:
    case BT_UUID_GATT_SERVICE_WATCHDOG:
    case BT_UUID_GATT_SERVICE_HEART:
    case BT_UUID_GATT_SERVICE_PHONEALERT:
    case BT_UUID_GATT_SERVICE_BATTERY:
    case BT_UUID_GATT_SERVICE_BLOODPRESSURE:
        return TRUE;
    default:
        return FALSE;
    }
}
#endif
