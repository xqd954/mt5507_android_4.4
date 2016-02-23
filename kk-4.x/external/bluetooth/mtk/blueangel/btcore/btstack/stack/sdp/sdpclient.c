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
 *     $Workfile:sdpclient.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the code for the Bluetooth 
 *     Service Discovery Protocol Client.
 *
 * Created:
 *     October 18, 1999
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
#include "bt_common.h"
#include "bt_feature.h"
#include "btalloc.h"
#include "sdp.h"
#include "sys/sdpi.h"
#include "sys/mei.h"
#include "sys/debug.h"

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

static BtStatus StartNextQuery(BtRemoteDevice *remDev);
static BtStatus SendSdpClientQuery(BtRemoteDevice *remDev);
void SdpClientCallback(BtRemoteDevice *remDev, BtEventType type);
void SdpClientProcessResp(BtRemoteDevice *remDev, U16 len, U8 *data);
void SdpClientStartQuery(BtRemoteDevice *remDev);
void SdpClientL2CapCallback(U16 ChannelId, L2capCallbackParms *Info);
void SdpClientQueryTimeout(EvmTimer *Timer);


/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  SdpInitClient
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 *****************************************************************************/
BtStatus SdpInitClient(void)
{
    SDPC(SdpClientPsm).callback = SdpClientL2CapCallback;
    SDPC(SdpClientPsm).psm = BT_INVALID_PSM;
    SDPC(SdpClientPsm).localMtu = SDP_CLIENT_LOCAL_MTU;
    SDPC(SdpClientPsm).minRemoteMtu = SDP_CLIENT_MIN_REMOTE_MTU;
    SDPC(SdpClientPsm).authorizedSetting = FALSE;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    SDPC(SdpClientPsm).inLinkMode = SDPC(SdpClientPsm).outLinkMode = L2MODE_BASIC;
#if 0
    SDPC(SdpClientPsm).inTransmitMax = 10;
    SDPC(SdpClientPsm).inBufferSize = SDP_CLIENT_LOCAL_MTU;
    SDPC(SdpClientPsm).inRetransTimeout = 100;
    SDPC(SdpClientPsm).inMonitorTimeout = 200;
#endif    
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */
#ifdef __BT_2_1_ERTM__
    SDPC(SdpClientPsm).lockStepNeeded = 0;
    SDPC(SdpClientPsm).highSpeedRequired = 0;
#endif

    if (L2CAP_RegisterPsm((L2capPsm*) (&SDPC(SdpClientPsm))) != BT_STATUS_SUCCESS)
    {
        return BT_STATUS_FAILED;
    }

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  SdpDeinitClient
 * DESCRIPTION
 *  Deinitialize SDP client.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SdpDeinitClient(void)
{
#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    L2CAP_DeregisterPsm((L2capPsm*) (&SDPC(SdpClientPsm)));
#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  SDP_Query
 * DESCRIPTION
 *  Perform an SDP query to a remote device
 * PARAMETERS
 *  token       [IN]        
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SDP_Query(SdpQueryToken *token, SdpQueryMode mode)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /*
     * Assign the default contState and contStateLen fields for BSQM_FIRST
     * * queries only.
     */
    if ((token) && (mode == BSQM_FIRST))
    {
        token->contState = &SDPC(contState);
        token->contStateLen = 1;
    }

#if XA_ERROR_CHECK == XA_ENABLED
    /* Verify the fields of the Query token */
    if ((token == 0) || (token->rm == 0) || (token->callback == 0) || (token->parms == 0) || (token->plen == 0) ||
        ((mode != BSQM_FIRST) && (mode != BSQM_CONTINUE)))
    {
        status = BT_STATUS_INVALID_PARM;
        bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 1);
        goto done;
    }

    /*
     * Validate the contState and contStateLen fields for BSQM_CONTINUE
     * queries only.
     */
    if ((mode == BSQM_CONTINUE) &&
        ((token->contState == 0) || (token->contStateLen <= 1) || (token->contStateLen > 17)))
    {
        status = BT_STATUS_INVALID_PARM;
        bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 2);
        goto done;
    }

    if ((IsNodeOnList(&(token->rm->queryList), &(token->node))) || (token->rm->curToken == token))
    {
        status = BT_STATUS_FAILED;
        bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 3);
        goto done;
    }

    if ((token->type != BSQT_SERVICE_SEARCH_REQ) && (token->type != BSQT_ATTRIB_REQ) &&
        (token->type != BSQT_SERVICE_SEARCH_ATTRIB_REQ))
    {
        status = BT_STATUS_INVALID_TYPE;
        bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 4);
        goto done;
    }

#if 0
    if (!SdpValidateQueryParms(token->type, token->parms, token->plen, token->contState, token->contStateLen, mode))
    {
    #if UPF_TWEAKS == XA_DISABLED
        status = BT_STATUS_INVALID_PARM;
        bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 5);
        goto done;
    #endif /* UPF_TWEAKS == XA_DISABLED */
    }
#endif
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    /* Now determine if the connection is active */
    if (token->rm->state != BDS_CONNECTED)
    {
        status = BT_STATUS_NO_CONNECTION;
        bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 6);
        goto done;
    }

    /* Put the token on the queue */
    if (mode == BSQM_FIRST)
    {
        InsertTailList(&(token->rm->queryList), &(token->node));
    }
    else
    {
        InsertHeadList(&(token->rm->queryList), &(token->node));
    }

    /* Attempt to start a query if one is not already started */
    status = BT_STATUS_PENDING;
    if (token->rm->curToken == 0)
    {
        status = StartNextQuery(token->rm);
        if (status != BT_STATUS_PENDING)
        {
            bt_trace(TRACE_GROUP_1, BT_SDP_QUERY_FAILED_CASE, 7);
            token->rm->curToken = 0;
        }
    }

  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SDP_QueryCancel
 * DESCRIPTION
 *  Cancel an SDP query to a remote device
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SDP_QueryCancel(SdpQueryToken *token)
{
    BtRemoteDevice *remDev = token->rm;
    if (remDev == NULL || remDev->state == BDS_OUT_CON)
    {
        return BT_STATUS_PENDING;
    }
    else if(remDev->sdpClientState == SDP_CS_L2CAP_CONNECT)
    {
        Assert(remDev->sdpClientChannel != 0);
        (void)L2CAP_DisconnectReq(remDev->sdpClientChannel);
        remDev->sdpClientState = SDP_CS_L2CAP_DISCON;
    }
    return BT_STATUS_SUCCESS;
}


#if SDP_PARSING_FUNCS == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  SDP_ParseAttributes
 * DESCRIPTION
 *  Parses an SDP Query response buffer for an Attribute ID and
 *  optionally a UUID within that Attribute ID.  The length of the
 *  Attribute ID or UUID result value is returned along with a
 *  pointer to the beginning of the result value.
 *  
 *  This function should be called during the SDEVENT_QUERY_RSP
 *  event, since the results buffer is only valid during this callback.
 *  In the case of a continuation state, additional SDP Queries
 *  must be issued during the context of the callback as well, or
 *  the L2CAP connection may be disconnected, due to a lack of SDP
 *  Queries to process.  If the L2CAP connection goes down, any
 *  continuation state that was returned is no longer valid and
 *  the SDP Queries will have to be reissued.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - Parsing was successful and a result
 *  value and length was returned.
 *  
 *  If additional unparsed data exists (see the token->remainBuff
 *  and token->remainLen fields), additional parsing can be issued
 *  from the point where parsing ended last (see SdpParsingMode).
 *  However, it should be known that SDP attributes are returned
 *  in ascending order. For best results, attempt to parse
 *  attributes in ascending order.  This will prevent needing to
 *  reparse the entire response for each attribute.
 *  
 *  BT_STATUS_FAILED - Parsing encountered an error or the attribute
 *  requested was not found.  In this case, the internal parsing
 *  state will be restored to its status prior to the failed parse.
 *  
 *  BT_STATUS_SDP_CONT_STATE - A continuation state exists (see the
 *  token->contState and token->contStateLen fields).  In the
 *  rare case that a continuation state occurs in the middle
 *  of parsing the result value, the partial results will be
 *  returned (see the token->totalValueLen, token->availValueLen,
 *  and token->valueBuff fields).  In this case, the partial results
 *  must be saved until the rest of the results are retrieved from
 *  additional SDP queries.  This is the only situation where the
 *  result value fields will be valid during this return code.
 *  Additional SDP Queries may be issued to retrieve the remainder
 *  of the SDP response.
 *****************************************************************************/
BtStatus SDP_ParseAttributes(SdpQueryToken *token)
{
    U16 curLen, offset;
    U8 n, i, uuid[2], *curPos = 0;
    BtStatus status = BT_STATUS_FAILED;
    SdpClientParsingStateInfo tempData;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!token || !token->results || token->rLen == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    /* Verify the "remainBuff" and remainLen" fields for BSPM_RESUME */
    if ((token->mode == BSPM_RESUME) &&
        ((token->parseData.flags != READY_RESUME) || (!token->remainBuff) || (token->remainLen == 0) ||
         ((token->remainBuff + token->remainLen) != (token->results + token->rLen))))
    {
        return BT_STATUS_INVALID_PARM;
    }

    if ((token->mode == BSPM_CONT_STATE) && (token->parseData.flags != READY_CONT_STATE))
    {
        return BT_STATUS_INVALID_PARM;
    }

#endif /* XA_ERROR_CHECK == XA_ENABLED */ 

    if (token->mode == BSPM_RESUME)
    {
        /* We automatically use "remainBuff" and "remainLen" for BSPM_RESUME */
        curPos = token->remainBuff;
        curLen = token->remainLen;
    }
    else
    {
        /* We use "results" and "rLen" for BSPM_BEGINNING and BSPM_CONT_STATE */
        curPos = token->results;
        curLen = token->rLen;
        /* No need to keep around old "remainBuff" and "remainLen" values */
        token->remainBuff = 0;
        token->remainLen = 0;
    }

    /* Clear out all possible return values */
    /* Continuation State is only valid once */
    token->contState = 0;
    token->contStateLen = 0;
    /* Available result value is updated each time */
    token->availValueLen = 0;
    /* Result value buffer varies */
    token->valueBuff = 0;
    /* Total Result value length may be valid for a continuation state */
    if (token->mode != BSPM_CONT_STATE)
    {
        token->totalValueLen = 0;
    }

    /*
     * Save the parsing data before altering it.  If this call fails, we will
     * * restore the parsing data back to its state prior to this call.
     */
    OS_MemCopy((U8*) & tempData, (U8*) & token->parseData, (U32) sizeof(SdpClientParsingStateInfo));

    if (token->mode == BSPM_BEGINNING)
    {
        /* Clear out any data from a previous query */
        OS_MemSet((U8*) & token->parseData, 0, (U32) sizeof(SdpClientParsingStateInfo));

        token->parseData.state = SDP_PARSE_ATTRIB_LIST_HEADER;
        token->parseData.curStoredOff = 0;
        token->parseData.storedLen = 1;
    }

    /* Only get the Attribute List Byte Count when parsing from the 
       beginning of an SDP response. */
    if (token->mode != BSPM_RESUME)
    {
        /* 
         *  Verify we have at least the Attribute List Byte Count (2 bytes),
         *  (2 bytes) and a Continuation State byte (1 byte) 
         */
        if (curLen < 3)
        {
            goto done;
        }

        /* Get Attribute List Byte Count */
        token->parseData.attribByteCount = SDP_GetU16(curPos);
        curPos += 2;
        curLen -= 2;
    }

    /* Clear the parsing flags */
    token->parseData.flags = READY_PARSE;

    /*
     * Loop until we have processed all of the attribute list byte count
     * or until we have found the attribute Id and uuid we were looking for. 
     */
    while (token->parseData.attribByteCount > 0)
    {
        n = min((U16) (token->parseData.storedLen - token->parseData.curStoredOff), curLen);

        /* Stage the data: save 2B-parsed data to token->parseData.storedBuff */
        for (i = token->parseData.curStoredOff; n > 0; n--, i++)
        {
            /* Can't stage any more data. Must parse the continuation state now */
            if (token->parseData.attribByteCount == 0)
            {
                goto contState;
            }

            /* Update Attribute Byte Count */
            if (token->parseData.attribByteCount > 0)
            {
                token->parseData.attribByteCount--;
            }
            /* Update Attribute List Length */
            if (token->parseData.attribListLen > 0)
            {
                token->parseData.attribListLen--;
            }
            /* Update Attribute Id List Length */
            if (token->parseData.attribIdLen > 0)
            {
                token->parseData.attribIdLen--;
            }

            token->parseData.storedBuff[i] = *curPos;
            curPos++;
            curLen--;
            token->parseData.curStoredOff++;
        }

        /*
         * Only call the state machine if the data has been completely staged or
         * if we are processing a continuation state. 
         */
        if (token->parseData.curStoredOff == token->parseData.storedLen)
        {
            switch (token->parseData.state)
            {
                case SDP_PARSE_ATTRIB_LIST_HEADER:
                    if (token->parseData.storedLen != 1)
                    {
                        goto done;
                    }
                    /* Get the Attribute List header information */
                    token->parseData.storedLen = SdpParseElementHeader(token->parseData.storedBuff[0]);
                    if ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_SEQ)
                    {
                        token->parseData.state = SDP_PARSE_ATTRIB_LIST_LEN;
                    }
                    else
                    {
                        if (token->parseData.storedLen != 1)
                        {
                            goto done;
                        }
                        token->parseData.storedLen = (U8) SDP_ParseDataElement(token->parseData.storedBuff, &offset);
                        token->parseData.state = SDP_PARSE_ATTRIB_ID;
                    }
                    break;

                case SDP_PARSE_ATTRIB_LIST_LEN:
                    if ((token->parseData.storedLen != 2) && (token->parseData.storedLen != 3) &&
                        (token->parseData.storedLen != 5))
                    {
                        goto done;
                    }
                    /* Get the Attribute List length */
                    token->parseData.attribListLen = SDP_ParseDataElement(token->parseData.storedBuff, &offset);
                    token->parseData.storedLen = 1;
                    token->parseData.state = SDP_PARSE_ATTRIB_LIST_HEADER;
                    break;

                case SDP_PARSE_ATTRIB_ID:
                    if (token->parseData.storedLen != 2)
                    {
                        goto done;
                    }
                    /* Get the Attribute Id */
                    token->parseData.attribId = SDP_GetU16(token->parseData.storedBuff);
                    if ((token->parseData.attribId == token->attribId) && !(token->uuid))
                    {
                        status = BT_STATUS_SUCCESS;
                        token->parseData.state = SDP_PARSE_RESULT_HEADER;
                    }
                    else
                    {
                        token->parseData.state = SDP_PARSE_ATTRIB_ID_HEADER;
                    }
                    token->parseData.storedLen = 1;
                    break;

                case SDP_PARSE_ATTRIB_ID_HEADER:
                    if (token->parseData.storedLen != 1)
                    {
                        goto done;
                    }
                    /* Get the Attribute ID Header information */
                    token->parseData.storedLen = SdpParseElementHeader(token->parseData.storedBuff[0]);
                    if (((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_SEQ) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_TEXT) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_URL) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_ALT))
                    {
                        token->parseData.state = SDP_PARSE_ATTRIB_ID_LIST_LEN;
                    }
                    else
                    {
                        if (token->parseData.storedLen != 1)
                        {
                            goto done;
                        }
                        token->parseData.storedLen =
                            1 + (U8) SDP_ParseDataElement(token->parseData.storedBuff, &offset);
                        /* 16-byte UUID and 1-byte header is our maximum storage size */
                        if (token->parseData.storedLen > 17)
                        {
                            goto done;
                        }
                        token->parseData.state = SDP_PARSE_ATTRIB_VALUE;
                    }
                    break;

                case SDP_PARSE_ATTRIB_ID_LIST_LEN:
                    if ((token->parseData.storedLen != 2) && (token->parseData.storedLen != 3) &&
                        (token->parseData.storedLen != 5))
                    {
                        goto done;
                    }
                    /* Get the Attribute ID List Length */
                    if (token->parseData.attribIdLen == 0)
                    {
                        /* Don't keep track of the sublists within the Attribute ID List */
                        token->parseData.attribIdLen = SDP_ParseDataElement(token->parseData.storedBuff, &offset);
                    }

                    token->parseData.state = SDP_PARSE_ATTRIB_ID_HEADER;

                    if (((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_TEXT) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_URL) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_ALT))
                    {
                        /* Parse this data by skipping over it and moving to the next AttribId */
                        token->parseData.attribSkipLen = SDP_ParseDataElement(token->parseData.storedBuff, &offset);
                        if (token->parseData.attribSkipLen > 0)
                        {
                            /* Skip over the value and move to the next AttribId */
                            token->parseData.state = SDP_SKIP_ATTRIB_VALUE;
                        }
                        else
                        {
                            /* No value to skip since the value was a 0-byte value */
                            if ((token->parseData.attribListLen == 0) || (token->parseData.attribIdLen == 0))
                            {
                                token->parseData.state = SDP_PARSE_ATTRIB_LIST_HEADER;
                            }
                            else
                            {
                                token->parseData.state = SDP_PARSE_ATTRIB_ID_HEADER;
                            }
                        }
                    }
                    token->parseData.storedLen = 1;
                    break;

                case SDP_PARSE_ATTRIB_VALUE:
                    /* Get the Attribute ID Value */
                    if ((token->parseData.attribListLen == 0) || (token->parseData.attribIdLen == 0))
                    {
                        token->parseData.state = SDP_PARSE_ATTRIB_LIST_HEADER;
                    }
                    else
                    {
                        token->parseData.state = SDP_PARSE_ATTRIB_ID_HEADER;
                    }

                    if ((token->parseData.attribId == token->attribId) &&
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_UUID))
                    {
                        /* See if the UUID matches the requested UUID */
                        StoreBE16(uuid, token->uuid);
                        if (SDP_UuidCmp
                            (token->parseData.storedBuff + 1, (U16) (token->parseData.storedLen - 1), uuid, 2))
                        {
                            status = BT_STATUS_SUCCESS;
                            token->parseData.state = SDP_PARSE_RESULT_HEADER;
                        }
                    }
                    token->parseData.storedLen = 1;
                    break;

                case SDP_SKIP_ATTRIB_VALUE:
                    if (token->parseData.storedLen != 1)
                    {
                        goto done;
                    }
                    /* Skip over the Text, Alt, or Url Attrib ID value */
                    token->parseData.attribSkipLen--;
                    if (token->parseData.attribSkipLen == 0)
                    {
                        if ((token->parseData.attribListLen == 0) || (token->parseData.attribIdLen == 0))
                        {
                            token->parseData.state = SDP_PARSE_ATTRIB_LIST_HEADER;
                        }
                        else
                        {
                            token->parseData.state = SDP_PARSE_ATTRIB_ID_HEADER;
                        }
                    }
                    break;

                    /* We found a match!  Now parse it out according to the Result Value Type and Size */
                case SDP_PARSE_RESULT_HEADER:
                    Assert(token->parseData.storedLen == 1);
                    /* Get the Result Value header information */
                    token->parseData.storedLen = SdpParseElementHeader(token->parseData.storedBuff[0]);
                    if (((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_SEQ) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_URL) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_TEXT) ||
                        ((SDP_GetElemType(token->parseData.storedBuff[0]) & DETD_MASK) == DETD_ALT))
                    {
                        /* Our Result is a list of information */
                        token->parseData.state = SDP_PARSE_RESULT_LIST_LEN;
                    }
                    else
                    {
                        if (token->parseData.storedLen != 1)
                        {
                            status = BT_STATUS_FAILED;
                            goto done;
                        }
                        Assert((token->parseData.storedBuff[0] & DESD_MASK) < 5);
                        /* Our Result is a standard value type */
                        token->totalValueLen = SDP_ParseDataElement(token->parseData.storedBuff, &offset);
                        token->parseData.resultValueLen = token->totalValueLen;
                        token->parseData.storedLen = 1;
                        token->parseData.state = SDP_PARSE_RESULT_VALUE;
                    }
                    break;

                case SDP_PARSE_RESULT_LIST_LEN:
                    if ((token->parseData.storedLen != 2) && (token->parseData.storedLen != 3) &&
                        (token->parseData.storedLen != 5))
                    {
                        status = BT_STATUS_FAILED;
                        goto done;
                    }
                    /* Get the Result List Length */
                    token->totalValueLen = SDP_ParseDataElement(token->parseData.storedBuff, &offset);

                    token->parseData.resultValueLen = token->totalValueLen;
                    token->parseData.storedLen = 1;
                    token->parseData.state = SDP_PARSE_RESULT_VALUE;

                    if (token->parseData.resultValueLen == 0)
                    {
                        status = BT_STATUS_SUCCESS;
                        goto done;
                    }
                    break;

                case SDP_PARSE_RESULT_VALUE:
                    Assert(token->parseData.storedLen == 1);
                    /* Assign the Result Value buffer pointer. */
                    if (!token->valueBuff)
                    {
                        token->valueBuff = curPos - 1;
                    }
                    /* Keep processing the Result Value until we have it all */
                    token->availValueLen += token->parseData.storedLen;
                    token->parseData.resultValueLen -= token->parseData.storedLen;
                    if (token->parseData.resultValueLen == 0)
                    {
                        status = BT_STATUS_SUCCESS;
                        goto done;
                    }
                    break;
            }   /* End switch */

            if ((token->parseData.state != SDP_PARSE_ATTRIB_LIST_LEN) &&
                (token->parseData.state != SDP_PARSE_ATTRIB_ID_LIST_LEN) &&
                (token->parseData.state != SDP_PARSE_ATTRIB_VALUE) &&
                (token->parseData.state != SDP_PARSE_RESULT_LIST_LEN))
            {
                token->parseData.curStoredOff = 0;
            }
        }   /* End if (curStageOff == stageLen) */
    }       /* End while */

  contState:
    /* Process the Continuation State for the SDP response */
    if (curLen == 0)
    {
        /* We must have at least one continuation state byte */
        status = BT_STATUS_FAILED;
        goto done;
    }

    token->contState = curPos;
    token->contStateLen = (*token->contState) + 1;
    if (curLen != token->contStateLen)
    {
        /* We must have the correct continuation state length */
        status = BT_STATUS_FAILED;
        goto done;
    }
    curLen -= token->contStateLen;
    Assert(curLen == 0);
    if (token->contStateLen > 1)
    {
        token->parseData.flags = READY_CONT_STATE;
        status = BT_STATUS_SDP_CONT_STATE;
    }
    else
    {
        /* Clear out the missing or invalid contination values */
        token->contState = 0;
        token->contStateLen = 0;
    }

  done:
    if (status == BT_STATUS_FAILED)
    {
        /*
         * Restore the SDP parsing state machine to its state when this call 
         * was originally made.
         */
        OS_MemCopy((U8*) & token->parseData, (U8*) & tempData, (U32) sizeof(SdpClientParsingStateInfo));
    }
    else if (status == BT_STATUS_SUCCESS)
    {
        /* Restore the values after parsing the result value */
        token->parseData.curStoredOff = 0;
        token->parseData.storedLen = 1;
        /* 
         * We either need to parse a new attribute ID or List, or we need
         * to continue parsing an attribute ID List 
         */
        if ((token->parseData.attribListLen == 0) || (token->parseData.attribIdLen == 0))
        {
            token->parseData.state = SDP_PARSE_ATTRIB_LIST_HEADER;
        }
        else
        {
            token->parseData.state = SDP_PARSE_ATTRIB_ID_HEADER;
        }
        /* Assign the remainder of the response yet to parse */
        token->remainBuff = curPos;
        token->remainLen = curLen;

        /* when attributrByteCount=0 and remainLen=1 : The remaining unparsed data is INFO field (specify the len of continue state) */
        if ((token->remainLen > 0) && (token->parseData.attribByteCount > 0))
        {
            token->parseData.flags = READY_RESUME;
        }
        else
        {
            token->parseData.flags = READY_PARSE;
        }

    }
    return status;
}
#endif /* SDP_PARSING_FUNCS == XA_ENABLED */ 


/*****************************************************************************
 * FUNCTION
 *  StartNextQuery
 * DESCRIPTION
 *  Start the next query if we can.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus StartNextQuery(BtRemoteDevice *remDev)
{
    BtStatus status;

    Assert(remDev->curToken == 0);
    if (!IsListEmpty(&(remDev->queryList)))
    {
        remDev->curToken = (SdpQueryToken*) RemoveHeadList(&(remDev->queryList));

        /* See if the link is up */
        if (remDev->state != BDS_CONNECTED)
        {
            return BT_STATUS_NO_CONNECTION;
        }

        switch (remDev->sdpClientState)
        {
            case SDP_CS_DISCONNECTED:
                /* A channel does not exist so start one */
                status = L2CAP_ConnectReq(
                            (L2capPsm*) (&SDPC(SdpClientPsm)),
                            BT_PSM_SDP,
                            remDev,
                            0,
                            &(remDev->sdpClientChannel));

                if (status != BT_STATUS_PENDING)
                {
                    bt_trace(TRACE_GROUP_1, SDP_CLIENT_FAILED_TO_START_L2CAP_STATUS__xD, status);
                    return status;
                }

                /* The L2CAP channel has started */
                bt_trace(TRACE_GROUP_1, SDP_CLIENT_SUCCESSFULLY_STARTED_L2CAP);
                remDev->sdpClientState = SDP_CS_L2CAP_STARTED;
                return BT_STATUS_PENDING;

            case SDP_CS_L2CAP_CONNECT:
                /* Connection is up so start the query */
                return SendSdpClientQuery(remDev);

            case SDP_CS_L2CAP_STARTED:
                return BT_STATUS_PENDING;

            default:
                /*
                 * The L2CAP channel is going down so put the
                 * the item back on the queue and wait
                 */
                InsertHeadList(&(remDev->queryList), &(remDev->curToken->node));
                remDev->curToken = 0;
                return BT_STATUS_PENDING;
        }
    }
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  SendSdpClientQuery
 * DESCRIPTION
 *  Send the query.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  BT_STATUS_PENDING if packet is taken otherwise BT_STATUS_FAILED.
 *****************************************************************************/
static BtStatus SendSdpClientQuery(BtRemoteDevice *remDev)
{
    I8 offset;

    offset = BT_PACKET_HEADER_LEN - 5;

    /*
     * If the amount of data to send exceeds the remote device's MTU size
     * then return an error.
     */
    if (remDev->curToken->plen + 5 > L2CAP_GetTxMtu(remDev->sdpClientChannel))
    {
        return BT_STATUS_NO_RESOURCES;
    }

    /* Put in the header for the query */
    remDev->sdpClientPacket.header[offset] = remDev->curToken->type;
    StoreBE16(remDev->sdpClientPacket.header + offset + 1, SDPC(transID));
    StoreBE16(
        remDev->sdpClientPacket.header + offset + 3,
        (U16) (remDev->curToken->plen + remDev->curToken->contStateLen));
    remDev->sdpClientPacket.headerLen = 5;
    remDev->transId = SDPC(transID);

    /* Select a new transaction ID */
    SDPC(transID)++;

    /* Now point to the data portion */
    remDev->sdpClientPacket.data = (U8*) remDev->curToken->parms;
    remDev->sdpClientPacket.dataLen = remDev->curToken->plen;
    remDev->sdpClientPacket.tail = remDev->curToken->contState;
    remDev->sdpClientPacket.tailLen = remDev->curToken->contStateLen;
    remDev->sdpClientPacket.flags = BTP_FLAG_TAIL;
    remDev->sdpClientPacket.priority_type = BTP_FLAG_HIGH_PRIPORTY;

    /* Call L2CAP to send the data */
    bt_trace(
        TRACE_GROUP_1,
        BT_LOG_SDP_CLIENT_QUERY_DATALEN_PTR,
        remDev->sdpClientPacket.dataLen,
        remDev->sdpClientPacket.data);
    EVM_ResetTimer(&remDev->sdapTimer);
    remDev->sdapTimer.func = SdpClientQueryTimeout;
    remDev->sdapTimer.context = remDev;
    EVM_StartTimer(&remDev->sdapTimer, 5000);
    return L2CAP_SendData(remDev->sdpClientChannel, &(remDev->sdpClientPacket));
}


/*****************************************************************************
 * FUNCTION
 *  SdpClientL2CapCallback
 * DESCRIPTION
 *  Handle the callbacks from L2CAP
 * PARAMETERS
 *  channelId       [IN]        
 *  info            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpClientL2CapCallback(U16 channelId, L2capCallbackParms *info)
{
    BtRemoteDevice *remDev;

    remDev = info->aclLink;

    switch (info->event)
    {
        case L2EVENT_CONNECTED:
            /* L2CAP connection has come up so send the query */
            bt_trace(TRACE_GROUP_1, SDP_CLIENT_L2CAP_CONNECTION_HAS_COME_UP);
            remDev->sdpClientChannel = channelId;
            remDev->sdpClientState = SDP_CS_L2CAP_CONNECT;
            Assert(remDev->curToken != 0);
            EVM_ResetTimer(&remDev->sdapTimer);
            /* SnifferRegisterEndpoint(SPI_L2CAP, &channelId, remDev, SPI_SDP); */

            /* Put the token back on the list then call the function to execute it */
            InsertHeadList(&(remDev->queryList), &(remDev->curToken->node));
            remDev->curToken = 0;
            SdpClientStartQuery(remDev);
            break;

        case L2EVENT_DISCONNECTED:
            bt_trace(TRACE_GROUP_1, SDP_CLIENT_CONNECTION_DISCONNECTED);
            remDev = SdpFindRemDevWithSdpChannel(channelId, BST_SDP_CLIENT);
            if (remDev != 0)
            {
                EVM_ResetTimer(&remDev->sdapTimer);
                remDev->sdpClientChannel = 0;
                remDev->sdpClientState = SDP_CS_DISCONNECTED;

                /* If there is an active token then call the callback */
                if (remDev->curToken != 0)
                {
                    SDPC(btEvent).errCode = BEC_SDP_DISCONNECT;
                    SdpClientCallback(remDev, SDEVENT_QUERY_FAILED);
                }
                else
                {
                    SdpClientStartQuery(remDev);
                }
            }
            break;

        case L2EVENT_DATA_IND:
            bt_trace(TRACE_GROUP_1, SDP_CLIENT_RECEIVED_RESPONSE);
            Assert(remDev != 0);
            if (remDev->sdpClientState == SDP_CS_L2CAP_CONNECT)
            {
            EVM_ResetTimer(&remDev->sdapTimer);
            SdpClientProcessResp(remDev, info->dataLen, info->ptrs.data);
            }
            else
            {
                OS_Report("[WARNING] %s(), Invalid sdpClientState: %d",
                    __FUNCTION__, remDev->sdpClientState);
            }
            break;

        case L2EVENT_PACKET_HANDLED:
            bt_trace(TRACE_GROUP_1, SDP_CLIENT_PACKET_RETURNED);
            break;
    }

}


/*****************************************************************************
 * FUNCTION
 *  SdpClientProcessResp
 * DESCRIPTION
 *  Process the response from the SDP server.
 * PARAMETERS
 *  remDev      [IN]        
 *  len         [IN]        
 *  data        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpClientProcessResp(BtRemoteDevice *remDev, U16 len, U8 *data)
{
    /* Make sure there is at least a header */
    if (len < 5)
    {
    	OS_Report("[WARNING] %s(), len < 5, len = %d", __FUNCTION__, len);
        goto err;
    }

    /* Check if response has the correct transID */
    if (remDev->transId != BEtoHost16(data + 1))
    {
	    OS_Report("[WARNING] %s(), transId not match %d, %d ", __FUNCTION__, remDev->transId, BEtoHost16(data + 1));
        goto err;
    }

    /*
     * If the parameter size does not match the number of bytes in the packet
     * return an error.
     */
    if (BEtoHost16(data + 3) != (len - 5))
    {
    	OS_Report("[WARNING] %s(), len not match: %d, %d", __FUNCTION__, BEtoHost16(data + 3), (len - 5));
        goto err;
    }

    /* See if the response is an error. */
    if (data[0] == BSQR_ERROR_RESP)
    {
        remDev->curToken->errorCode = BEtoHost16(data + 5);
        SdpClientCallback(remDev, SDEVENT_QUERY_ERR);
        return;
    }
    else
    {
        /* Make sure a successful response clears the error code */
        remDev->curToken->errorCode = 0;
    }

    /* Verify the response is the correct one */
    if ((data[0] - 1) != remDev->curToken->type)
    {
    	OS_Report("[WARNING] %s(), token type not match: %d, %d", __FUNCTION__, (data[0] - 1), remDev->curToken->type);
        goto err;
    }

    /* Return the complete result including the contination state. */
    remDev->curToken->results = data + 5;
    remDev->curToken->rLen = len - 5;

    SDPC(btEvent).errCode = BEC_NO_ERROR;
    SdpClientCallback(remDev, SDEVENT_QUERY_RSP);
    return;
  err:
    /* This is an invalid response call the callback */
    SDPC(btEvent).errCode = BEC_INVALID_SDP_PDU;
    SdpClientCallback(remDev, SDEVENT_QUERY_FAILED);
}


/*****************************************************************************
 * FUNCTION
 *  SdpClientCallback
 * DESCRIPTION
 *  Call the client back then handle the next token.
 * PARAMETERS
 *  remDev      [IN]        
 *  type        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpClientCallback(BtRemoteDevice *remDev, BtEventType type)
{
    /* Setup the event */
    SDPC(btEvent).eType = type;
    SDPC(btEvent).p.token = remDev->curToken;
    remDev->curToken = 0;

    /* Call the callback */
    if (SDPC(btEvent).p.token->callback != 0) {
	    SDPC(btEvent).p.token->callback(&(SDPC(btEvent)));
    } else {
    	OS_Report("[WARNING] %s(), SDPC(btEvet).p.token->callback is NULL", __FUNCTION__);
    }

    /* Start another query if one is not already started */
    if (remDev->curToken == 0)
    {
        SdpClientStartQuery(remDev);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SdpClientStartQuery
 * DESCRIPTION
 *  Start a query
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpClientStartQuery(BtRemoteDevice *remDev)
{
    BtStatus status;

    /*
     * Attempt to start a query until one is started or until there are no
     * more pending queries
     */
    do
    {
        status = StartNextQuery(remDev);
        switch (status)
        {
            case BT_STATUS_SUCCESS:
                /* There are no more tokens for this connection so disconnect L2CAP */
                if (remDev->sdpClientState == SDP_CS_L2CAP_CONNECT)
                {
                    bt_trace(TRACE_GROUP_1, SDP_CLIENT_NO_MORE_TOKENS_SO_DISCONNECTING_L2CAP);
                    Assert(remDev->sdpClientChannel != 0);
                    (void)L2CAP_DisconnectReq(remDev->sdpClientChannel);
                    remDev->sdpClientState = SDP_CS_L2CAP_DISCON;
                }
                break;

            case BT_STATUS_PENDING:
                break;

            case BT_STATUS_NO_RESOURCES:
                SDPC(btEvent).errCode = BEC_SDP_NO_RESOURCES;
                goto callback;

            case BT_STATUS_NO_CONNECTION:
                bt_trace(TRACE_GROUP_1, SDP_CLIENT_QUERY_FAILED_TO_START_NO_CONNECTION);
                SDPC(btEvent).errCode = BEC_SDP_DISCONNECT;
                goto callback;

            default:
                SDPC(btEvent).errCode = BEC_SDP_INTERNAL_ERR;
              callback:
                SdpClientCallback(remDev, SDEVENT_QUERY_FAILED);
                break;
        }

    } while ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING));
}


/*****************************************************************************
 * FUNCTION
 *  SdpClientQueryTimeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdpClientQueryTimeout(EvmTimer *Timer)
{
    BtRemoteDevice *remDev;
    bt_prompt_trace(MOD_BT, "[BT] SdpClientQueryTimeout"); 
    remDev = (BtRemoteDevice*) Timer->context;
    Assert(remDev != 0);
    remDev->sdapTimer.func = 0;
    remDev->discFlag = TRUE;
    bt_trace(TRACE_GROUP_1, SDPCLIENTQUERYTIMEOUT_TIMEOUTBT_DEBUGx02X, remDev->state);
    if (remDev->state == BDS_CONNECTED)
    {
    	// ALPS00460826: Do not disconnect ACL directly, try to close L2CAP channel for SDP first.
        // MeDisconnectLink(remDev, 11);
		if (remDev->sdpClientState == SDP_CS_L2CAP_CONNECT)	{
			bt_prompt_trace(MOD_BT, "[BT] SdpClientQueryTimeout: Close L2CAP Channel."); 
			Assert(remDev->sdpClientChannel != 0);
			(void)L2CAP_DisconnectReq(remDev->sdpClientChannel);
			remDev->sdpClientState = SDP_CS_L2CAP_DISCON;
		}
    }
}

#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */ 

