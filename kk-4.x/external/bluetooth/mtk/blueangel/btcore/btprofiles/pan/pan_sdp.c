/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
 *
 * File:
 *     $Workfile:pan_sdp.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains code for the PAN profile. 
 *
 * Created:     Dec 19, 2002
 *
 * Copyright 2000-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/
#ifdef __BT_PAN_PROFILE__

#include <bttypes.h>
#include <btalloc.h>
#include "sdap.h"
#include "sdp.h"
#include <pan.h>


/****************************************************************************
 *
 * SDP objects used to query PAN services.
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 *
 * SDP query info
 *
 * Service search attribute request for a PAN.
 * The service search pattern is very specific. It contains the UUIDs
 * for PAN, BNEP and L2CAP.
 */
/*
static U16 bt_pan_attr_id_list[] = 
{
    AID_SERVICE_CLASS_ID_LIST,
    AID_PROTOCOL_DESC_LIST
};
*/

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            PanStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Starts the SDP query.
 *            
 *
 * Return:    
 *
 */
BtStatus PanStartServiceQuery(PanUser *pan, U16 uuid)
{

    BtStatus status = BT_STATUS_SUCCESS;
/*
    sdap_service_search_multi_attribute_struct search_pattern;

    search_pattern.rm = pan->cmgrHandler.remDev;
    search_pattern.sqt = &pan->sdpQueryToken;
    search_pattern.uuid = uuid;
    search_pattern.callback = PanSdpCallback;
    search_pattern.attribute_num = sizeof(bt_pan_attr_id_list)/sizeof(bt_pan_attr_id_list[0]);
    search_pattern.attribute_id = bt_pan_attr_id_list;
    status = SDAP_ServiceSearchMultipleAttribute(search_pattern);
*/
    sdap_service_search_struct search_pattern;

    search_pattern.rm = pan->cmgrHandler.remDev;
    search_pattern.sqt = &pan->sdpQueryToken;
    search_pattern.uuid = uuid;    
    search_pattern.callback = PanSdpCallback;    

    status = SDAP_ServiceSearch(search_pattern);

    return status;
}

/*---------------------------------------------------------------------------
 *            PanVerifySdpQueryRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses the PAN Server's SDP response for the PAN Service Class 
 *            and Protocol Descriptor List.  In the case of a continuation 
 *            state, additional SDP queries will be issued until all of the
 *            attributes have been found or until the entire SDP response
 *            has been parsed.
 *            
 *
 * Return:    BT_STATUS_SUCCESS if SDP parsing is successful.
 *            BT_STATUS_FAILED if SDP parsing encountered an error.
 *            BT_STATUS_SDP_CONT_STATE if SDP parsing encountered a 
 *            continuation state that requires an additional SDP query.
 */
BtStatus PanVerifySdpQueryRsp(PanUser *pan, U16 attr_id, U8 *attr_val, U16 attr_len)
{
    BOOL          foundBnep = FALSE;
    BtStatus      status = BT_STATUS_FAILED;

    /* Verify PAN Service */
    if (attr_id == AID_SERVICE_CLASS_ID_LIST)
    {
        U16 uuid = SDP_GetU16(attr_val);    
        if (uuid == pan->dstService)
        {
            status = BT_STATUS_SUCCESS;
        }
    }
    /* Verify BNEP protocol */
    else if (attr_id == AID_PROTOCOL_DESC_LIST) 
    {
        U8 offset = 8; //L2CAP uuid + L2CAP PSM
        U16 proto_uuid = SDP_GetU16(attr_val + offset);    
        if (proto_uuid == PROT_BNEP) 
        {
            status = BT_STATUS_SUCCESS;
        }
    } 

    return status;
}

/*---------------------------------------------------------------------------
 *            PanSdpCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  PAN callback routine for SDP events.
 *
 * Return:    void
 */
void PanSdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
    PanUser *pan = ContainingRecord(sqt, 
                                         PanUser, 
                                         sdpQueryToken);

    BtStatus    status = BT_STATUS_FAILED;

    kal_trace(BT_TRACE_G2_PROFILES, PAN_SDP_QUERY_CALLBACK_RESULT, result);
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
            status  = BNEP_Open(pan->cmgrHandler.remDev, &pan->bnepPanu);
            if ((status == BT_STATUS_SUCCESS) ||
                (status == BT_STATUS_PENDING))
            {
                kal_trace(BT_TRACE_G2_PROFILES, PAN_SDP_QUERY_CALLBACK_DONE);
                return;
            }
            break;
            
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:
        default:    
            break;            
    }

    PanClearConnection(pan, status);

/*
    switch (result) 
    {
        case BT_STATUS_SDP_CONT_STATE:
            status = PanVerifySdpQueryRsp(pan, bt_pan_attr_id_list[attr_idx], attr_val, sqt->availValueLen);
            if (status == BT_STATUS_SUCCESS)	
            {
                return;
            }
            break;			

        case BT_STATUS_SUCCESS:
            status = PanVerifySdpQueryRsp(pan, bt_pan_attr_id_list[attr_idx], attr_val, sqt->availValueLen);
            if (status == BT_STATUS_SUCCESS)
            {
                // Connect to channel 
                status  = BNEP_Open(pan->bnepPanu.remDev, &pan->bnepPanu);
                if ((status == BT_STATUS_SUCCESS) ||
		   (status == BT_STATUS_PENDING))
                {
                    kal_trace(BT_TRACE_G2_PROFILES, PAN_SDP_QUERY_CALLBACK_DONE);
                    return;
                }
            }
            break;			

        default:
           break;			
    }
		
    PanClearConnection(pan, status);
*/
}

#endif
