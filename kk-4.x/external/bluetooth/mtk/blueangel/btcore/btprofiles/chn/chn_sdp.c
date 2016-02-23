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
 * File:
 *     $Workfile:hf_sdp.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the SDP code for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
 /*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * Jun 2 2009 mtk80029
 * [MAUI_01837169] [BT] remove kal_prompt_trace
 * add trace
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#include "stdio.h"
#include "btalloc.h"
#include "sdpdb.h"
#include "sdp.h"
#include "sdap.h"
#include "chn.h"

static U16 chn_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST, 
};

static void ChnQueryCallback(	SdpQueryToken *sqt, 
                                       	U8 result, 
                                       	U8 attribute_index,
                                       	U8 *attribute_value );
/* Internal function prototypes */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            ChnRegisterSdpServices()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers the SDP services.
 *
 * Return:    See SDP_AddRecord().
 */
BtStatus ChnRegisterSdpServices(ChnChannel *Channel, U16 svc)
{
    BtStatus status = BT_STATUS_SUCCESS;

    DBG_PRINT_FLOW(("[CHN][FLOW] HfRegisterSdpServices"));

    if(Channel->chnSdpRecord.num == 0)
    {
        Channel->chnSdpRecord.num = SDPDB_GetLocalServiceAttribute(
                                                                    svc,
                                                                    &(Channel->chnSdpRecord.attribs),
                                                                    &(Channel->chnService.serviceId));
	if(Channel->chnSdpRecord.num == 0)
	{
		switch(svc)
		{
		case SC_HANDSFREE:
			Channel->chnSdpRecord.num = HfphfSdpDB_GetAttribute(
                                                                    svc,
                                                                    &(Channel->chnSdpRecord.attribs),
                                                                    &(Channel->chnService.serviceId));
			break;
		default:
			break;
		}		
	}
	
	if(Channel->chnSdpRecord.num > 0)
	{
            status = SDP_AddRecord(&(Channel->chnSdpRecord));
            if(status != BT_STATUS_SUCCESS)
                DBG_PRINT_ERROR(("[CHN][ERR] SDP_AddRecord fail : %d - (%s,%d)", status, __FILE__, __LINE__));
        }
        else
        {	
            DBG_PRINT_ERROR(("[CHN][ERR] ChnRegisterSdpServices fail - (%s,%d)", __FILE__, __LINE__));
            Report(("ChnRegisterSdpServices: Channel->chnSdpRecord.num <= 0 (%d)", (Channel->chnSdpRecord.num)));
            Assert(Channel->chnSdpRecord.num > 0);
            status = BT_STATUS_FAILED;
        }            
    }
    else
    {
        /* Already registered */
        DBG_PRINT_WARN(("[CHN][WARN] ChnRegisterSdpServices : already registered. (%s,%d)", __FILE__, __LINE__));
    }
    
    Report(("- ChnRegisterSdpServices: status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            ChnDeregisterSdpServices()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters the SDP services.
 *
 * Return:    See SDP_RemoveRecord().
 */
BtStatus ChnDeregisterSdpServices( ChnChannel *Channel )
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    DBG_PRINT_FLOW(("[CHN][FLOW] ChnDeregisterSdpServices"));
    
    /* Remove the Hands-free entry */
    if(Channel->chnSdpRecord.num > 0)
    {
        status = SDP_RemoveRecord(&(Channel->chnSdpRecord));
        if(status != BT_STATUS_SUCCESS)
            DBG_PRINT_ERROR(("[CHN][ERR] SDP_RemoveRecord fail : %d - (%s,%d)", status, __FILE__, __LINE__));        
    }
    /* As a mark that the SDP record is not registered  */
    /* num and serviceId are all get through ChnRegisterSdpService, so we clear these to 0 */
    Channel->chnSdpRecord.num = 0;
    Channel->chnService.serviceId = 0;
    Report(("- ChnDeregisterSdpServices: status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            ChnStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate the SDP service query.
 *
 * Return:    See SDP_Query().
 */
BtStatus ChnStartServiceQuery(ChnChannel *Channel, SdpQueryMode mode)
{
    BtStatus status;
    sdap_service_search_multi_attribute_struct search_pattern;
    	
    Report(("+ ChnStartServiceQuery: mode=%d", mode));

    /* Reset the query flag */
    Channel->queryFlags = 0x00;
    search_pattern.rm = Channel->cmgrHandler.remDev;
    search_pattern.sqt = &Channel->sdpQueryToken;
    search_pattern.uuid = Channel->remote_svc_type;
    search_pattern.callback = ChnQueryCallback;

	search_pattern.attribute_num = sizeof(chn_attribute_id_list)/sizeof(chn_attribute_id_list[0]);
	search_pattern.attribute_id = chn_attribute_id_list;

	status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);    

    DBG_PRINT_FLOW(("[CHN][FLOW] Start SDP query : %d", status));

    if(status == BT_STATUS_PENDING)
    {
        Channel->queryFlags |= SDP_QUERY_FLAG_ONGOING;
    }
    else
    {
        DBG_PRINT_ERROR(("[CHN][ERR] SDAP_ServiceSearchMultipleAttribute : %d - (%s,%d)", status, __FILE__, __LINE__));
    }

    Report(("- ChnStartServiceQuery: status=%d", status));
    return status;
} 

/*************************************************************************
* FUNCTION                                                            
*	hfgQueryCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*   Modified by MTK Dlight mtk01239
*
* PARAMETERS
*	sqt  : SdpQueryToken contains information such as remote device information. uuid...
*	result  : The service search result.
*      attribute_index: the current request attribute list index
*      attribute_value: points to the search result
* RETURNS
*	None
* LOCAL AFFECTED
*   
*************************************************************************/

static void ChnQueryCallback(	SdpQueryToken *sqt, 
                                       	U8 result, 
                                        	U8 attribute_index,
                                       	U8 *attribute_value )
{
    	BtStatus status;
    	ChnChannel     *Channel = ContainingRecord( sqt, ChnChannel, sdpQueryToken );
   	U16 attrib = AID_PROTOCOL_DESC_LIST;

    	Report( ("[CHN][ChnQueryCallback] result==%d, attribute_index==x%X, attribute==0x%X", result, attribute_index, attrib) );
    
    	switch (result) 
    	{
	case BT_STATUS_SUCCESS:
            switch (attrib)
            {
                case AID_PROTOCOL_DESC_LIST:
                    Channel->rfServerChannel = attribute_value[0];
                    //Channel->queryFlags |= SDP_QUERY_FLAG_PROTOCOL;
                    DBG_PRINT_EVENT(("[CHN][EVENT] SDP callback : AID_PROTOCOL_DESC_LIST, server channel=%d", Channel->rfServerChannel));
                    break;
            }
            break;
        case BT_STATUS_CONNECTION_FAILED:
            DBG_PRINT_EVENT(("[CHN][EVENT] SDP callback : BT_STATUS_CONNECTION_FAILED"));
            break;
        case BT_STATUS_FAILED:
            DBG_PRINT_EVENT(("[CHN][EVENT] SDP callback : BT_STATUS_FAILED"));
            break;
        case BT_STATUS_NOSERVICES:
            DBG_PRINT_EVENT(("[CHN][EVENT] SDP callback : BT_STATUS_NOSERVICES"));
            break; 
        default:
            DBG_PRINT_EVENT(("[CHN][EVENT] SDP callback : unknown result : %d", result));
            break;
    	}

	Channel->queryFlags &= ~SDP_QUERY_FLAG_ONGOING;
	
    if( result != BT_STATUS_SUCCESS )
    {
        /* SDP query is over then clear ongoing bit  */
        //Channel->queryFlags &= ~SDP_QUERY_FLAG_ONGOING;
        Report( ("[CHN][ChnQueryCallback] SDP query complete") );
    }

    if(ChnGetSubstate(Channel) == CHN_SUBSTATE_CONN_OUT2)
    {
        if(result == BT_STATUS_SUCCESS)
        {
            //if( (Channel->queryFlags & completeFlags) == completeFlags )
            {
                /* All attributes are returned */
                Assert(Channel->rfChannel->userContext == 0);
                Assert(Channel->rfServerChannel != 0);
                status = RF_OpenClientChannel(Channel->cmgrHandler.bdc->link,
                                                  		    Channel->rfServerChannel,
                                                  		    Channel->rfChannel, 1);
                DBG_PRINT_FLOW(("[CHN][FLOW] Open RFCOMM channel : %d", status));
                if (status == BT_STATUS_PENDING) 
                {
                    ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_OUT3);
                } 
                else 
                {
                    Channel->cmgrHandler.errCode = BEC_LOCAL_TERMINATED;
                    ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
                }
            }
        }
        else
        {
            ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
        }
    }
    else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_OPEN2)
    {
        ChnChangeSubstate(Channel, CHN_SUBSTATE_OPEN1);
    }
    else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_DISC1)
    {
        ChnDisconnecting(Channel);
    }
    else
    {
        DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
        Assert(0);
    }	
    Report(("- ChnQueryCallback."));
}
