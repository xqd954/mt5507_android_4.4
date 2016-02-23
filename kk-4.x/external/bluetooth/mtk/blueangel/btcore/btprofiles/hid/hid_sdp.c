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
 *     $Workfile:hid_sdp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description:
 *     SDP Registration and queries.
 *
 * Created:
 *     Jul 14, 2004
 *
 * Copyright 2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)

//#include "kal_release.h"
//#include "bluetooth_trc.h"
#include <sys/hidalloc.h>
//#include <oslib.h>
#include "sdap.h"
#include "sdp.h"
#include "bt_os_api.h"

extern void Hid_Init_Parser(HidParseData*	tmpHidParser);
extern BtStatus HidAddDescriptor(HidChannel *channel);
extern void Hid_Free_Parser(HidParseData *hpd);
#if SDP_PARSING_FUNCS == XA_DISABLED
#error The HID SDK requires the SDP parsing functions.
#endif /* SDP_PARSING_FUNCS == XA_DISABLED */

#if HID_HOST == XA_ENABLED
static void HidSdpEventHandler(SdpQueryToken *sqt, U8 result, U8 attr_idx, U8 *attr_val);
#endif
void HidVerifySdpQueryRsp(HidChannel *Channel, U16 AttribId, U8 *attr_val, U16 attr_len, U8 result);

static kal_uint16 bt_hid_attr_id_list[] = 
{
    //AID_HID_VIRTUAL_CABLE,
    //AID_HID_RECONNECT_INIT,
    AID_HID_DESCRIPTOR_LIST,
    //AID_HID_SDP_DISABLE,
    //AID_HID_NORM_CONNECTABLE,
    //AID_HID_BOOT_DEVICE
};

HidQueryFlags bt_hid_attr_flags = //SDPQ_FLAG_VIRTUAL_CABLE |
							//SDPQ_FLAG_RECONNECT_INIT |
							SDPQ_FLAG_DESCRIPTOR_LIST;
							//SDPQ_FLAG_SDP_DISABLE |
							//SDPQ_FLAG_NORM_CONNECTABLE |
							//SDPQ_FLAG_BOOT_DEVICE;

typedef struct{
	BD_ADDR		BtAddr;
	U8*			reportDescriptor;
	U32			reportDescriptorLength;
	BT_BOOL		inUse;
}ReportDescriptorList;

#define HID_MAX_REPORT_DESCRIPTOR_NUM	10
//ReportDescriptorList CachedReportDescriptor[HID_MAX_REPORT_DESCRIPTOR_NUM] = {{{{0}},NULL,0,FALSE}};
ReportDescriptorList CachedReportDescriptor[HID_MAX_REPORT_DESCRIPTOR_NUM] = {0};

ReportDescriptorList* HidFindReportDescriptorByAddr(BD_ADDR addr)
{
    U8 i = 0;
	
    for (i = 0; i < HID_MAX_REPORT_DESCRIPTOR_NUM; i++)
    {
        if(CachedReportDescriptor[i].BtAddr.addr[0] == addr.addr[0] && \
			CachedReportDescriptor[i].BtAddr.addr[1] == addr.addr[1] && \
			CachedReportDescriptor[i].BtAddr.addr[2] == addr.addr[2] && \
			CachedReportDescriptor[i].BtAddr.addr[3] == addr.addr[3] && \
			CachedReportDescriptor[i].BtAddr.addr[4] == addr.addr[4] && \
			CachedReportDescriptor[i].BtAddr.addr[5] == addr.addr[5]&& \
			CachedReportDescriptor[i].inUse)
        {
            return &CachedReportDescriptor[i];
        }
    }
    return NULL;
}

BtStatus HidCacheReportDescriptor(BD_ADDR addr, U8* HidDescriptorList, U32 length)
{
    U8 i = 0;
	
    for (i = 0; i < HID_MAX_REPORT_DESCRIPTOR_NUM; i++)
    {
        if (!CachedReportDescriptor[i].inUse)
        {
            btmtk_os_memcpy(CachedReportDescriptor[i].BtAddr.addr, addr.addr, 6);
            CachedReportDescriptor[i].inUse = TRUE;
            CachedReportDescriptor[i].reportDescriptor = (U8*)hid_malloc(length);
            if(!CachedReportDescriptor[i].reportDescriptor)
            {
                bt_prompt_trace(MOD_BT,"[HID]CachedReportDescriptor[i].reportDescriptor hid_malloc error");
                return BT_STATUS_FAILED;
            }
            btmtk_os_memset(CachedReportDescriptor[i].reportDescriptor, 0 ,length);
            btmtk_os_memcpy(CachedReportDescriptor[i].reportDescriptor, HidDescriptorList, length);
            CachedReportDescriptor[i].reportDescriptorLength = length;
            break;
        }
    }
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            HidStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate the SDP service query for a HID device.
 *
 * Return:    See SDP_Query().
 */
BtStatus HidStartServiceQuery(HidChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    sdap_service_search_multi_attribute_struct search_pattern;
    ReportDescriptorList* reportDescriptorList = NULL;
    HidParseData*	hpd ;
	
    reportDescriptorList = HidFindReportDescriptorByAddr(Channel->cmgrHandler.remDev->bdAddr);
    if(reportDescriptorList != NULL && Channel->state == HID_STATE_OPEN)
    {
		Channel->queryRsp.descriptorLen = reportDescriptorList->reportDescriptorLength;
		Channel->queryRsp.descriptorList = (U8*)hid_malloc(reportDescriptorList->reportDescriptorLength);
		btmtk_os_memset((U8*) Channel->queryRsp.descriptorList, 0, reportDescriptorList->reportDescriptorLength); 			
		btmtk_os_memcpy((U8*) Channel->queryRsp.descriptorList, reportDescriptorList->reportDescriptor, reportDescriptorList->reportDescriptorLength);
		hpd = (HidParseData*)hid_malloc(sizeof(HidParseData));
		Hid_Init_Parser(hpd);
				Channel->reportCons[0] = (HidReportConstructor*)hid_malloc(HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
		btmtk_os_memset((U8*) Channel->reportCons[0], 0, HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
		
		if(HidAddDescriptor(Channel) == BT_STATUS_SUCCESS)
			bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor Successed!");
		else
			bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor failed!");
        
		Hid_Free_Parser(hpd);
		Channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
		HidSdpCallback(Channel, BT_STATUS_SUCCESS);
		return BT_STATUS_SUCCESS;
    }
    else
    {
	    search_pattern.rm = Channel->cmgrHandler.remDev;
	    search_pattern.sqt = &Channel->sdpQueryToken;
	    search_pattern.uuid = SC_HUMAN_INTERFACE_DEVICE;
	    search_pattern.callback = HidSdpEventHandler;
	    search_pattern.attribute_num = sizeof(bt_hid_attr_id_list)/sizeof(bt_hid_attr_id_list[0]);
	    search_pattern.attribute_id = bt_hid_attr_id_list;
	    status = SDAP_ServiceSearchMultipleAttribute2(search_pattern);
	    return status;
    }
} 

void HidSdpEventHandler(SdpQueryToken *sqt, U8 result, U8 attr_idx, U8 *attr_val)
{
    HidChannel     *channel = ContainingRecord(sqt, 
                                               HidChannel, sdpQueryToken);
    ReportDescriptorList* reportDescriptorList = NULL;
    HidParseData*	hpd ;
    kal_trace(BT_TRACE_G2_PROFILES,BT_HID_SDP_QUERY_CALLBACK_x02X ,result);

    switch (result) 
    {
        case BT_STATUS_SDP_CONT_STATE:
            kal_trace(BT_TRACE_G2_PROFILES,HID_SDP_SUCCESSFULLY);
            HidVerifySdpQueryRsp(channel, bt_hid_attr_id_list[attr_idx], attr_val, sqt->availValueLen, result);  
            break;

        case BT_STATUS_SUCCESS:
            HidVerifySdpQueryRsp(channel, bt_hid_attr_id_list[attr_idx], attr_val, sqt->availValueLen, result);
            if (channel->bSDPDiscPending)
            {
                HidClearConnection(channel, result, 0);
            }
            else 
            {
				hpd = (HidParseData*)hid_malloc(sizeof(HidParseData));
				Hid_Init_Parser(hpd);
                		channel->reportCons[0] = (HidReportConstructor*)hid_malloc(HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
				btmtk_os_memset((U8*) channel->reportCons[0], 0, HID_REPORT_ID_MAX * sizeof(HidReportConstructor));

				reportDescriptorList = HidFindReportDescriptorByAddr(channel->cmgrHandler.remDev->bdAddr);
				
				if(reportDescriptorList != NULL )
				{		
					btmtk_os_memset(reportDescriptorList->BtAddr.addr, 0, 6);
					reportDescriptorList->inUse = FALSE;
					hid_free(reportDescriptorList->reportDescriptor);
					reportDescriptorList->reportDescriptorLength = 0;
				}
				
				HidCacheReportDescriptor(channel->cmgrHandler.remDev->bdAddr, channel->queryRsp.descriptorList, channel->queryRsp.descriptorLen);
				
        		if(HidAddDescriptor(channel) == BT_STATUS_SUCCESS)
        			bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor Successed!");
        		else
        			bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor failed!");   
                
				Hid_Free_Parser(hpd);
                channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
                HidSdpCallback(channel, result);
            }
            break;
		case BT_STATUS_NOSERVICES:
			{
				static const U8 HidDescriptorList[] = 
				{
				    HID_DESCRIPTOR
				};
				channel->queryRsp.descriptorLen = HID_DESCRIPTOR_LEN;
				channel->queryRsp.descriptorList = (U8*)hid_malloc(HID_DESCRIPTOR_LEN);
				btmtk_os_memset((U8*) channel->queryRsp.descriptorList, 0, HID_DESCRIPTOR_LEN);				
				btmtk_os_memcpy((U8*) channel->queryRsp.descriptorList, HidDescriptorList, HID_DESCRIPTOR_LEN);
				hpd = (HidParseData*)hid_malloc(sizeof(HidParseData));
				Hid_Init_Parser(hpd);
                		channel->reportCons[0] = (HidReportConstructor*)hid_malloc(HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
				btmtk_os_memset((U8*) channel->reportCons[0], 0, HID_REPORT_ID_MAX * sizeof(HidReportConstructor));

                if(HidAddDescriptor(channel) == BT_STATUS_SUCCESS)
                    bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor Successed!");
                else
                    bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor failed!");
                
				Hid_Free_Parser(hpd);
                channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
                HidSdpCallback(channel, BT_STATUS_SUCCESS);
				break;
			}
        case BT_STATUS_CONNECTION_FAILED:
        case BT_STATUS_FAILED:
        default:
            channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
            HidClearConnection(channel, result, 0);
            break;
    }

}

U8 HidSdpGetValidSize(U8 size_descriptor)
{
    U8 element_size = 0;

    switch (size_descriptor)
    {
        case DESD_1BYTE:
            element_size = 1;
            break;
        case DESD_2BYTES:
            element_size = 2;
            break;
        case DESD_4BYTES:
            element_size = 4;
            break;
        case DESD_8BYTES:
            element_size = 8;
            break;
        case DESD_16BYTES:
            element_size = 16;
            break;
    }
    return element_size;

}


/*---------------------------------------------------------------------------
 *            HidSdpparse8Bit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses the SDP query response for an 8 bit value.
 *
 * Return:    (see SDP_ParseAttributes)
 */
static void HidSdpParse8Bit(HidChannel *Channel, U8 *in_value, 
                                U16 AttribId, U16 QueryFlag, U8 *out_value)
{
    if (in_value == NULL)
    {
        return;
    }

    if (!(Channel->queryRsp.queryFlags & QueryFlag)) {

        *out_value = in_value[0];
        Channel->queryRsp.queryFlags |= QueryFlag;

        kal_trace(BT_TRACE_G2_PROFILES,HID_SDP_PARSEATTRIBUTES_SUCCEEDED_VALUE__0Xx02XUPDATED_BUFF_LEN__xD , *out_value, 
                1);
    }

}

/*---------------------------------------------------------------------------
 *            HidSdpparse16Bit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses the SDP query response for an 16 bit value.
 *
 * Return:    (see SDP_ParseAttributes)
 */
static void HidSdpParse16Bit(HidChannel *Channel, U8 *in_value, 
                                 U16 AttribId, U16 QueryFlag, U16 *out_value)
{
    if (in_value == NULL)
    {
        return;
    }

    if (!(Channel->queryRsp.queryFlags & QueryFlag)) {

        /* We had better have a U16 version */
        *out_value = SDP_GetU16(in_value);

        kal_trace(BT_TRACE_G2_PROFILES,HID_SDP_PARSEATTRIBUTES_SUCCEEDED_VALUE__0Xx04XUPDATED_BUFF_LEN__xD , *out_value, 
                2);

        Channel->queryRsp.queryFlags |= QueryFlag;

    }

}

/*---------------------------------------------------------------------------
 *            HidSdpParseDescriptorList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses the SDP query response for an 16 bit value.
 *
 * Return:    (see SDP_ParseAttributes)
 */
static void HidSdpParseDescriptorList(HidChannel *Channel, U8 *in_value, U16 in_len, U8 result)
{
   // HidParseData	*hpd;
    //hpd = Hid_Init_Parser();

    if (in_value == NULL)
    {
        return;
    }
	
    if (!(Channel->queryRsp.queryFlags & SDPQ_FLAG_DESCRIPTOR_LIST)) {

        if (in_len == 0) {
            return;
        }
        
        //if ((result == BT_STATUS_SDP_CONT_STATE) &&
	     if(Channel->queryRsp.descriptorLenOffset == 0)
        {
            U8 elemType = 0;
            U8 elemSizeDescpt = 0;			
            U8 elemSize = 0;			
            U8 elemAddSize = 0;			
            U8 offset = 0;
            U8 i = 0;
			
            /* Only the first packet is preceded with data element header. The packet
              * in continuation sequence is raw data.
              */

            while (offset + Channel->queryRsp.descriptorLen < in_len) 
            {
                elemType = SDP_GetElemType(in_value[offset]);
                elemSizeDescpt = SDP_GetElemSize(in_value[offset]);
                switch (elemType)
                {
                    case DETD_SEQ:
                        if (elemSizeDescpt == DESD_ADD_8BITS) 
                        {
                            offset = offset + 1;
                        }
                        else if (elemSizeDescpt == DESD_ADD_16BITS) 
                        {
                            offset = offset + 2;
                        }
                        else if (elemSizeDescpt == DESD_ADD_32BITS) 
                        {
                            offset = offset + 4;
                        }
                        break;
						
                    case DETD_UINT:
                        if (elemSizeDescpt > DESD_16BYTES) 
                        {
                            return;
                        }
                        else
                        {
                            elemSize = HidSdpGetValidSize(elemSizeDescpt);
                            offset += elemSize;
                        }
                        break;
						
                    case DETD_TEXT:
                        if (elemSizeDescpt == DESD_ADD_8BITS) 
                        {
                            elemAddSize = 1;
                        }
                        else if (elemSizeDescpt == DESD_ADD_16BITS) 
                        {
                            elemAddSize = 2;
                        }
                        else if (elemSizeDescpt == DESD_ADD_32BITS) 
                        {
                            elemAddSize = 4;
                        }

                        for (i = 0; i < elemAddSize; i++)
                        {
                            Channel->queryRsp.descriptorLen = (Channel->queryRsp.descriptorLen << 8) | (in_value[offset + i + 1]);
                        }
                        offset = offset + elemAddSize;
                        break;
						
                    default:
                        // other data element shall not be come out in HID attribute list						
                        break;
                }

                offset++;
            }

            if((in_len - offset) <= 0)
            {
                return;
            }

            /* Allocate a new buffer */
            Channel->queryRsp.descriptorList = (U8 *)hid_malloc((U32)Channel->queryRsp.descriptorLen);
            if (Channel->queryRsp.descriptorList == NULL)
            {
                return;
            }
            btmtk_os_memset(Channel->queryRsp.descriptorList, 0, Channel->queryRsp.descriptorLen);

            /* Copy whatever data is available */
            btmtk_os_memcpy(Channel->queryRsp.descriptorList, in_value + offset, in_len - offset);
			
            Channel->queryRsp.descriptorLenOffset = in_len - offset;
        }
        else
        {
            btmtk_os_memcpy(Channel->queryRsp.descriptorList + Channel->queryRsp.descriptorLenOffset, 
				        in_value, in_len);

            /* in case that there is more continuation packet */
            Channel->queryRsp.descriptorLenOffset += in_len;		
	/*
	   if(Hid_SdpParse(Channel, hpd) == BT_STATUS_SUCCESS)
		bt_prompt_trace(MOD_BT,"[HID]Successed  SDP parser ");
	   else if(Hid_SdpParse(Channel, hpd) == BT_STATUS_FAILED)
	   	bt_prompt_trace(MOD_BT,"[HID]Failed  SDP parser ");
	   Hid_Free_Parser(hpd);*/
        }

        kal_trace(BT_TRACE_G2_PROFILES,HID_SDP_PARSEATTRIBUTES_SUCCEEDED_BYTES_READ__xD_UPDATED_BUFF_LEN__xD , in_len, Channel->queryRsp.descriptorLenOffset);

        if (result == BT_STATUS_SUCCESS)
        {
            Channel->queryRsp.queryFlags |= SDPQ_FLAG_DESCRIPTOR_LIST;
        }
    }

}

/*---------------------------------------------------------------------------
 *            HidVerifySdpQueryRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses the SDP response for all relevent SDP attributes and
 *            various HID parameters.  In the case of a continuation state, 
 *            additional SDP queries will be issued until all of the attributes 
 *            have been found or until the entire SDP response has been parsed.
 *            
 *
 * Return:    BT_STATUS_SUCCESS if SDP parsing is successful.
 *            BT_STATUS_FAILED if SDP parsing encountered an error.
 *            BT_STATUS_SDP_CONT_STATE if SDP parsing encountered a 
 *            continuation state that requires an additional SDP query.
 */
void HidVerifySdpQueryRsp(HidChannel *Channel, U16 AttribId, U8 *attr_val, U16 attr_len, U8 result)
{
    switch (AttribId)
    {
        case AID_HID_DEVICE_RELEASE:
            HidSdpParse16Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_DEVICE_RELEASE, &Channel->queryRsp.deviceRelease);			
            break;
			
        case AID_HID_PARSER_VERSION:
            HidSdpParse16Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_PARSER_VERSION, &Channel->queryRsp.parserVersion);			
            break;			

        case AID_HID_DEVICE_SUBCLASS:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_DEVICE_SUBCLASS, &Channel->queryRsp.deviceSubclass);			
            break;			

        case AID_HID_COUNTRY_CODE:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_COUNTRY_CODE, &Channel->queryRsp.countryCode);			
            break;			

        case AID_HID_VIRTUAL_CABLE:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_VIRTUAL_CABLE, &Channel->queryRsp.virtualCable);			
            break;			

        case AID_HID_RECONNECT_INIT:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_RECONNECT_INIT, &Channel->queryRsp.reconnect);			
            break;			

        case AID_HID_DESCRIPTOR_LIST:
            HidSdpParseDescriptorList(Channel, attr_val, attr_len, result);			
            break;			

        case AID_HID_SDP_DISABLE:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_SDP_DISABLE, &Channel->queryRsp.sdpDisable);			
            break;			

        case AID_HID_BATTERY_POWER:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_BATTERY_POWER, &Channel->queryRsp.batteryPower);			
            break;			

        case AID_HID_REMOTE_WAKE:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_REMOTE_WAKE, &Channel->queryRsp.remoteWakeup);			
            break;			

        case AID_HID_PROFILE_VERSION:
            HidSdpParse16Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_PROFILE_VERSION, &Channel->queryRsp.profileVersion);			
            break;			

        case AID_HID_SUPERV_TIMEOUT:
            HidSdpParse16Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_SUPERV_TIMEOUT, &Channel->queryRsp.supervTimeout);			
            break;			

        case AID_HID_NORM_CONNECTABLE:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_NORM_CONNECTABLE, &Channel->queryRsp.normConnectable);			
            break;			

        case AID_HID_BOOT_DEVICE:
            HidSdpParse8Bit(Channel, attr_val, AttribId, 
                                SDPQ_FLAG_BOOT_DEVICE, &Channel->queryRsp.bootDevice);			
            break;			

        default:
            break;			
    }

}

/*---------------------------------------------------------------------------
 *            HidSdpCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Makes a callback to the application with query response.
 */
void HidSdpCallback(HidChannel *Channel, BtStatus Status)
{
    /* Indicate the query response to the application */
    HidAppCallback(Channel, HIDEVENT_QUERY_CNF, Status,
                   &Channel->queryRsp, (U16)(sizeof(HidQueryRsp)));

    hid_free(Channel->queryRsp.descriptorList);
    btmtk_os_memset((U8 *)&Channel->queryRsp, 0, sizeof(Channel->queryRsp));
}

BtStatus HidAddDescriptor(HidChannel *channel)
{
	if(!channel)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty Channel");
		return BT_STATUS_FAILED;
	}			
	
    
    bt_prompt_trace(MOD_BT,"[HID]channel->descLen=%d",channel->descLen);
    
    btmtk_os_memcpy(channel->descStr + channel->descLen, 
                    channel->queryRsp.descriptorList, 
                    channel->queryRsp.descriptorLen);
    
    channel->descLen += channel->queryRsp.descriptorLen;

    bt_prompt_trace(MOD_BT,"[HID]channel->descLen=%d",channel->descLen);
    HidShowBytes(channel->descStr, channel->descLen);

    return 0;
}


#endif
