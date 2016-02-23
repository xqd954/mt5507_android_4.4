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

#ifdef __BT_HDP_PROFILE__

#include <bttypes.h>
#include <btalloc.h>
#include "sdap.h"
#include "sdp.h"
#include "hdp.h"

static U16 hdp_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST,				//L2CAP PSM in control channel, MCAP version
    AID_BT_PROFILE_DESC_LIST,			//HDP version			
    AID_ADDITIONAL_PROT_DESC_LISTS,		//L2CAP PSM in data channel
    AID_HDP_SUPPORTED_FEATURES,			//HDP supported features: MDEP ID, MDEP ROLE, MDEP data type and description 
    AID_HDP_MCAP_SUPPORTED_PROC,		//MCAP supported procedure
};

/*mapping with hdp_attribute_id_list*/
static U16 hdp_query_flag_list[] = 
{
	HDP_SDP_QUERY_FLAG_PROTOCOL,
	HDP_SDP_QUERY_FLAG_PROFILE,
	HDP_SDP_QUERY_FLAG_ADDITIONAL_PROTOCAL ,
	HDP_SDP_QUERY_FLAG_HDP_FEATURES ,
	HDP_SDP_QUERY_FLAG_MCAP_PROC ,
};

/*{entry, offset}*/
static U16 hdp_feature_order[][2] = 
{
	{0x08, 2}, 	/*MDEP ID*/
	{0x09, 2}, /*DATA TYPE*/
	{0x08, 3},	/*ROLE*/
	{0x25, 2}	/*DESCRIPTION*/
};

#define HDP_FEAUTURE_ELEMENT_NUM (sizeof(hdp_feature_order)/sizeof(hdp_feature_order[0]))

static HdpFeature feature;
void hdp_sdp_callback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value);
extern void hdp_adp_callback(void *buffer, HdpEvent event, BtStatus status);
extern BtStatus hdp_mdl_connect(HdpChannel *channel);
extern BtStatus hdp_mcl_connect(HdpChannel *channel);
extern BOOL hdp_channel_check_active(HdpChannel *channel);
/****************************************************************************
 *
 * SDP objects used to query HDP services.
 *
 ****************************************************************************/
BOOL hdp_sdp_feature_cmp(HdpFeature *localFeature, HdpFeature *targetFeature)
{
	return ((localFeature->dataType == targetFeature->dataType ) &&
		 	((localFeature->role == HDP_ROLE_SINK && targetFeature->role == HDP_ROLE_SOURCE) ||
		  	 (localFeature->role == HDP_ROLE_SOURCE && targetFeature->role == HDP_ROLE_SINK)));
}


U8 hdp_sdp_move_to_next_feature(U8 **buffer)
{
	U8 len = 2 + (*buffer)[1];
	*buffer += len;
	return len;
}

BOOL hdp_sdp_verify_element(U8 index, U8 value)
{
	return hdp_feature_order[index][0] == value;
}
U8 hdp_sdp_get_offset(U8 index)
{
	return hdp_feature_order[index][1];
}
/*Notes: rawdata contains the whole feature fields including len*/
BtStatus hdp_sdp_parse_new_feature(HdpFeature * feature, const U8 *rawData)
{
	U8 totalLen = *(rawData + 1);
	U8 index = 0;
	const U8 *data = rawData;
	
	OS_Report("[HDP][SDP]hdp_sdp_parse_new_feature:total len is 0x%x", totalLen);
	data += hdp_sdp_get_offset(index);
	if (hdp_sdp_verify_element(index, (*data)))
	{
		feature->mdepId = *(data+1);
		OS_Report("mdepId is : 0x%2x", feature->mdepId);
	}
	else
	{
		OS_Report("feature index %d is invalid: 0x%2x", index, *data);
		goto error;
	}

	index ++;
	data += hdp_sdp_get_offset(index);
	if (hdp_sdp_verify_element(index, (*data)))
	{
		feature->dataType = SDP_GetU16(data + 1);
		OS_Report("dataType is : 0x%4x", feature->dataType);
	}
	else
	{
		OS_Report("feature index %d is invalid: 0x%2x", index, *data);
		goto error;
	}
	
	index ++;
	data += hdp_sdp_get_offset(index);
	if (hdp_sdp_verify_element(index, (*data)))
	{
		feature->role = *(data+1);
		OS_Report("role is : 0x%2x, data is %d", feature->role, data);
	}
	else
	{
		OS_Report("feature index %d is invalid: 0x%2x", index, *data);
		goto error;
	}

	//TODO: enable desc check
	/*if (data - rawData < totalLen)
	{
		index ++;
		data += hdp_sdp_get_offset(index);
		OS_Report("desc:data is %d, index is %d, offset is %d", data, index, hdp_sdp_get_offset(index));
		if (hdp_sdp_verify_element(index, (*data)))
		{
			OS_StrnCpy(feature->descriotion, data + 2, *(data+1));
		}
		else
		{
			OS_Report("feature index %d is invalid: 0x%2x", index, *data);
			goto error;
		}
	}*/
	
	return BT_STATUS_SUCCESS;
error:
	return BT_STATUS_FAILED;
	
}

/*Notes: when HDP supported features item is abtain, the total len is stored in totalValueLen @sdpQueryToken, but not in attribute_value*/
BtStatus hdp_sdp_parse_features(HdpChannel *channel, U8 *attribute_value)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	U32 totalLen = 0;
	U8 currLen = 0;
	U8 offset = 0;
	U8 *buffer = attribute_value;
	BtStatus ret = BT_STATUS_FAILED;
	/*----------------------------------------------------------------*/
    /* code body                                               */
    /*----------------------------------------------------------------*/

	if (HDP_MDEP_ID_ECHO_TEST == channel->instance->feature.mdepId)
	{
		channel->rmSdpRecord.mdepId = HDP_MDEP_ID_ECHO_TEST;
		return BT_STATUS_SUCCESS;
	}

	totalLen = channel->sdpQueryToken.totalValueLen;

	if (totalLen == 0)
	{
		OS_Report("[HDP][SDP] ERR: supported features len is zero");
		return ret;
	}
	OS_Report("[HDP]hdp_sdp_parse_features, len is %d", totalLen);	

	while (currLen < totalLen)
	{
		OS_MemSet(&feature, 0x0, sizeof(HdpFeature));	
		ret = hdp_sdp_parse_new_feature(&feature, buffer);
		if (BT_STATUS_SUCCESS != ret)
		{	
			OS_Report("[err] parse feature");		
			break;
		}
		else if (hdp_sdp_feature_cmp(&(channel->instance->feature), &feature))
		{
			channel->rmSdpRecord.mdepId = feature.mdepId;
			break;
		}
		
		currLen += hdp_sdp_move_to_next_feature(&buffer); 
	}
	return ret;
}

BtStatus hdp_sdp_parse_attribute(HdpChannel *channel, U8 attribute_index, U8 *attribute_value)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	U16 attribute = hdp_attribute_id_list[attribute_index];
	HDPSdpQueryFlag flag = hdp_query_flag_list[attribute_index];
	HdpSdpRecord *sdpRecord = &channel->rmSdpRecord;
	BtStatus ret = BT_STATUS_SUCCESS;
	/*----------------------------------------------------------------*/
    /* code body                                               */
    /*----------------------------------------------------------------*/

	OS_Report("[HDP][SDP]hdp_sdp_parse_attribute, attribute is 0x%x", attribute);

	switch(attribute)
	{
		case AID_PROTOCOL_DESC_LIST:
			sdpRecord->ctrChnPsm = SDP_GetU16(attribute_value);
			break;
			
		case AID_BT_PROFILE_DESC_LIST:
			sdpRecord->hdpVersion = SDP_GetU16(attribute_value);
			break;
			
		case AID_ADDITIONAL_PROT_DESC_LISTS:
			sdpRecord->dataChnPsm = SDP_GetU16(attribute_value);
			break;
			
		case AID_HDP_SUPPORTED_FEATURES:
			ret = hdp_sdp_parse_features(channel, attribute_value);
			break;
			
		case AID_HDP_MCAP_SUPPORTED_PROC:
			sdpRecord->mcapProc = attribute_value[0];
			break;
			
		default:
			OS_Report("[HDP][SDP] invalid attribute %d", attribute);
			ret = BT_STATUS_INVALID_PARM;
			
	}
	if (BT_STATUS_SUCCESS == ret)
	{
		channel->queryFlags |= flag;
	}
	return ret;
}

/*---------------------------------------------------------------------------
 *            hdp_sdp_query()
 *---------------------------------------------------------------------------
 *  
 * query HDP service record in remote device.
 *
 *---------------------------------------------------------------------------
 * 
 * BtRemoteDevice : remote device
 * SdpQueryToken : 
 * UUID:
 *---------------------------------------------------------------------------
 */

BtStatus hdp_sdp_query(HdpChannel *channel)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	BtStatus status;
    sdap_service_search_multi_attribute_struct search_pattern;
	
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
    	
    OS_Report("[HDP]hdp_sdp_query, rm device is %d, rm uuid is %d", channel->cmgrHdlr.remDev, channel->rmUuid);
	
    /* Reset the query flag */
    channel->queryFlags = 0x00;
	
    search_pattern.rm = channel->cmgrHdlr.remDev;
    search_pattern.sqt = &channel->sdpQueryToken;
    search_pattern.uuid = channel->rmUuid;
    search_pattern.callback = hdp_sdp_callback;

	search_pattern.attribute_num = sizeof(hdp_attribute_id_list)/sizeof(hdp_attribute_id_list[0]);
    search_pattern.attribute_id = hdp_attribute_id_list;
   
    status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);  

    if(status == BT_STATUS_PENDING)
    {
        channel->queryFlags |= HDP_SDP_QUERY_OUTGOING;
    }
    else
    {
        OS_Report("[HDP][ERR] SDAP_ServiceSearchMultipleAttribute : %d", status);
    }
    return status;
}


/*---------------------------------------------------------------------------
 *            hdp_sdp_callback()
 *---------------------------------------------------------------------------
 *
 *
 * Return:    void
 */

void show_sdp(HdpChannel *channel)
{
	OS_Report("[HDP]show_sdp");
	OS_Report("control channel is 0x%x", channel->rmSdpRecord.ctrChnPsm);
	OS_Report("data channel is 0x%x", channel->rmSdpRecord.dataChnPsm);
	OS_Report("rm mdepid is 0x%x", channel->rmSdpRecord.mdepId);
}
void hdp_sdp_callback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value)
{
    BtStatus    status = BT_STATUS_FAILED;
	HdpChannel *channel = ContainingRecord(sqt, HdpChannel, sdpQueryToken);
	BOOL notify = FALSE;

	OS_Report("[HDP][SDP]hdp_sdp_callback, result is %d, attribute_index is %d, channel is %d", result, attribute_index, channel);

	if (NULL == channel || FALSE == hdp_channel_check_active(channel))
	{
		OS_Report("[HDP][SDP]no channel is found for the SDP query token");
		return;
	}
    switch (result) 
    {
        case BT_STATUS_SUCCESS:  

			status = hdp_sdp_parse_attribute(channel, attribute_index, attribute_value);

			if (BT_STATUS_SUCCESS != status) 
			{
				notify	= TRUE;
			}
			else if(channel->queryFlags & HDP_SDP_QUERY_FLAG_MCAP_PROC)
			{
				channel->queryFlags &= ~(HDP_SDP_QUERY_OUTGOING);
				show_sdp(channel);

				status = hdp_mcl_connect(channel);
				if (BT_STATUS_SUCCESS == status)
				{
					status = hdp_mdl_connect(channel);

				}

				if (BT_STATUS_PENDING != status)
				{
					notify = TRUE;
				}				
			}				
            break;
			
        case BT_STATUS_SDP_CONT_STATE:
			break;
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:			
        default:  
			notify = TRUE;
            break;            
    }

	if (notify)
	{
		hdp_adp_callback(channel, HDP_EVENT_CONNECT_CNF, status);
	}
}


#endif

