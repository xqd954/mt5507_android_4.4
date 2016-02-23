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
#if defined(__BT_HDP_PROFILE__)

#include "bt_common.h"
#include "stdio.h"
#include "sdp.h"
#include "hdp.h"

#define ELEMENT_SEQUENCE_SIZE 2
#define ELEMENT_FEATURE_BASIC_SIZE 7 // including mdepid (2), role(2) and datatype(3) 
#define ELEMENT_FEATURE_DESCRIPTION_LEN 256
#define ELEMENT_FEATURE_MAX_SIZE (ELEMENT_FEATURE_BASIC_SIZE + ELEMENT_SEQUENCE_SIZE + ELEMENT_FEATURE_DESCRIPTION_LEN) 

typedef struct _MDEP
{
	BOOL inUse;
	HdpFeature feature;
} MDEP;

typedef struct _HdpRecords
{
    BOOL registered;
	MDEP mdeplist[HDP_INSTANCE_MAX_NUM];
	U8	activeNum;
    SdpRecord record;   /* SDP record */
} HdpRecords;

HdpRecords BT_HdpRecord;
extern void *hdp_util_malloc(U32 size);
extern void hdp_util_free(void *value);
/****************************************************************************
 * SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP ServiceClassIDList
 */
static U8 HdpClassId[] = 
{
   SDP_ATTRIB_HEADER_8BIT(0x00),					/* Data Element Sequence  */
   SDP_UUID_16BIT(0x0000),							/* First service class entry, mandontary */
   SDP_UUID_16BIT(0x0000),							/* Second service class entry, optional */

};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by Hdp.
 * 
 * Value of the protocol descriptor list for the Hdp Profile.
 * This structure is a ROM'able representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service.
 */
static U8 HdpProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(16), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for L2CAP, 6 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),         /* L2CAP UUID */
    SDP_UINT_16BIT(HDP_SDP_DEFAULT_PSM),    /* Hdp PSM, Notes: the value has to be modified to the real value*/

	/*
	 * Next protocol descriptor in the list is Hdp.
	 */
	SDP_ATTRIB_HEADER_8BIT(6),				/* Data element sequence for L2CAP, 6 bytes */
	SDP_UUID_16BIT(PROT_MCAP_CONTROL_CHANNEL), 				/* MCAP control channel UUID */
	SDP_UINT_16BIT(HDP_SDP_MCAP_VERSION),	/* Hdp PSM, Notes: the value has to be modified to the real value*/
};

/*---------------------------------------------------------------------------
 * SDP Profile Descriptor List
 */
static const U8 HdpProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* Data element sequence, 8 bytes */
    SDP_ATTRIB_HEADER_8BIT(6),  /*
                                 			* Data element sequence for ProfileDescriptor,
                                 			* 6 bytes.
                                 			*/
    SDP_UUID_16BIT(SC_HDP),
    SDP_UINT_16BIT(HDP_SDP_HDP_VERSION)
};


/*---------------------------------------------------------------------------
 * SDP AdditionalProtocol Descriptor List object registered by Hdp.
 * 
 * Value of the additional protocol descriptor list for the Hdp.
 * This structure is a ROM'able representation of the RAM structure.
 * During HCRP_ServerRegisterSDP, this structure is copied into the RAM 
 * structure and used to register the client or server.
 */
static U8 HdpAddProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(15), /* Data element sequence, 15 bytes  */

    /*
     * Each element of this list is a protocol descriptor list.  
     */

    SDP_ATTRIB_HEADER_8BIT(13), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.  The second element is Hdp.
     */
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP),         /* Uuid16 L2CAP                    */
    SDP_UINT_16BIT(HDP_SDP_DEFAULT_PSM),    /* L2CAP PSM (varies)         */

    /*
     * The next protocol descriptor is for Hdp. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for HCRP Control */
    SDP_UUID_16BIT(PROT_MCAP_DATA_CHANNEL)   /* Uuid16 Hdp Channel            */
};

/*---------------------------------------------------------------------------
 * Profile Version
 */
static const U8 HdpProfileVersion[] = 
{
    SDP_UINT_16BIT(0x0100)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ServiceName
 *
 * This is the English string.  Other languguages can be defined.
 */
static const U8 HdpServiceName[] = 
{
    SDP_TEXT_8BIT(14),  /* Null terminated text string */
    'H', 'e', 'a', 'l', 't', 'h', ' ', 'd', 'e', 'v', 'i', 'c', 'e','\0'
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ServiceDescription
 *
 * This is the English string.  Other languguages can be defined.
 */
static const U8 HdpServiceDescription[] = 
{
    SDP_TEXT_8BIT(14),  /* Null terminated text string */
    'H', 'e', 'a', 'l', 't', 'h', ' ', 'd', 'e', 'v', 'i', 'c', 'e','\0'
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ProviderName
 *
 * This is the English string.  Other languguages can be defined.
 */
static const U8 HdpProviderName[] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string */
    'M', 'e', 'd', 'i', 'a', 't', 'e', 'k', ' ', 'I', 'n', 'c', '\0'
};

static U8 *HdpSupportedFeatures = NULL;

/*---------------------------------------------------------------------------
 * * MONDATARY *  HDP supported features
 * Notes: the features may be changed if HDP instance changed
 */
//static U8 HdpMdepList[0]; 

static U8 HdpMdepList_Header[] =                 
{
	SDP_ATTRIB_HEADER_8BIT(0x00),               /* Data element sequence*/
												/* Notes: HdpSupFeatures_Len[1] : total size of HDP supported features*/
};

/*Notes: max size of a feature is (2 + 2 + 3 + 2 + 2 + 255) = 266*/
static U8 HdpMdepList_Feature[] = 
{
	SDP_ATTRIB_HEADER_8BIT(0x00),				/* First supported features, mandontary */
	SDP_UINT_8BIT(0x00),						/* MDEP ID, mandontary*/
	SDP_UINT_16BIT(0x0000),						/* MDEP data type, mandontary*/
	SDP_UINT_8BIT(0x00),						/* MDEP role, mandontary */
	SDP_TEXT_8BIT(0x00),						/* MDEP deccription, optional*/
												/* Notes: if MDEP description exsit, the string has to be followed*/
};

/*---------------------------------------------------------------------------
 * * MONDATARY *  MCAP supported procedure
 */
static const U8 HdpDataExchSpec[] = 
{
    SDP_UINT_8BIT(HDP_SDP_IEEE_11073)
};

/*---------------------------------------------------------------------------

 * * MONDATARY *  MCAP supported procedure
 */
static const U8 HdpMcapSupProc[] = 
{
    SDP_UINT_8BIT(HDP_SDP_MCAP_DEFAULT_FEATURES)
};

/*---------------------------------------------------------------------------
 * Hdp attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * Hdp SDP record.
 */
SdpAttribute HdpSdpAttributes[] = 
{
    /* Mandatory SDP Attributes */
    /* Hdp class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, HdpClassId),
    /* Hdp protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, HdpProtoDescList),
    /* Hdp profile descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, HdpProfileDescList),
	/* Hdp additional protocal descriptor list attribute */
	SDP_ATTRIBUTE(AID_ADDITIONAL_PROT_DESC_LISTS, HdpAddProtoDescList),
	/* Hdp service name attribute */
	SDP_ATTRIBUTE(AID_HDP_SERVICE_NAME, HdpServiceName),
	/* Hdp service description attribute */
	SDP_ATTRIBUTE(AID_HDP_SERVICE_DESCRIPTION, HdpServiceDescription),
	/* Hdp provider name attribute */
	SDP_ATTRIBUTE(AID_HDP_PROVIDER_NAME, HdpProviderName),
	/* Hdp supported features attribute */
	SDP_ATTRIBUTE(AID_HDP_SUPPORTED_FEATURES, NULL), //HdpMdepList_Header just occupy the position.
	/* Hdp data exchange specification attribute */
	SDP_ATTRIBUTE(AID_HDP_DATA_EXCHANGE_SPEC, HdpDataExchSpec),
	/* MCAP supported procedures attribute */
	SDP_ATTRIBUTE(AID_HDP_MCAP_SUPPORTED_PROC, HdpMcapSupProc),

};

/*********************
Function Definitions 
**********************/
/*
* Notes: normal flow to register HDP record:
	a. HdpSdpDB_ReplacePsm()
	b. HdpSdpDB_AddFeature();
	c. HdpSdpDB_Registration();
	d. HdpSdpDB_RemoveFeature();
	d. HdpSdpDB_DeRegistration();
*/
/*****************************************************************************
 * FUNCTION
 *  HdpDeviceSdpDB_Registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HdpSdpDB_Update();
MDEP * HdpSdpDB_FindAvailableMdep ();
MDEP * HdpSdpDB_FindMdep (MdepId id);
SdpAttribute * HdpSdpDB_FindAttribute (SdpAttributeId id);



BtStatus HdpSdpDB_Registration(void)
{
    BtStatus status;
	SdpAttribute *attribute = NULL;
	OS_Report("[HDP]HdpSdpDB_Registration: active instance num is %d",BT_HdpRecord.activeNum);
	if (TRUE == BT_HdpRecord.registered)
	{
		return BT_STATUS_SUCCESS;
	}
	if((BT_HdpRecord.activeNum <= 0))
	{
		return BT_STATUS_FAILED;
	}
	attribute = HdpSdpDB_FindAttribute(AID_HDP_SUPPORTED_FEATURES);
	
	if (NULL == attribute || NULL == attribute->value)
	{
		return BT_STATUS_FAILED;
	}

    BT_HdpRecord.registered = TRUE;
    BT_HdpRecord.record.num = sizeof(HdpSdpAttributes) / sizeof(*(HdpSdpAttributes));
    /* Retrieve the server channel from the service channel */
    BT_HdpRecord.record.attribs = (SdpAttribute*) & HdpSdpAttributes;
    status = SDP_AddRecord(&BT_HdpRecord.record);
    Assert((status != BT_STATUS_INVALID_PARM));
    kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HdpDeviceSdpDB_DeRegistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HdpSdpDB_DeRegistration(void)
{
    BtStatus status;
	SdpAttribute *attribute = NULL;
	OS_Report("[HDP][SDPDB]HdpSdpDB_DeRegistration");
	if (FALSE == BT_HdpRecord.registered)
	{
		OS_Report("[HDP][SDPDB]hdp sdp records have been deregistered");
		return BT_STATUS_SUCCESS;
	}
	BT_HdpRecord.registered = FALSE; 
    status = SDP_RemoveRecord(&BT_HdpRecord.record);
	
    if(HdpSupportedFeatures != NULL){
        OS_Report("[HDP]HdpSupportedFeatures reset: %p", HdpSupportedFeatures);
        hdp_util_free(HdpSupportedFeatures);
        HdpSupportedFeatures = NULL;
    }
	attribute = HdpSdpDB_FindAttribute(AID_HDP_SUPPORTED_FEATURES);
    if (NULL != attribute)
	{
		attribute->value = NULL;
	}

	//fix the issue that HDP instance record still is in cache when BT power off
	OS_MemSet(&BT_HdpRecord, 0x0, sizeof(HdpRecords));
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HdpSdpDB_AddFeature
 * DESCRIPTION
 *  add a new feature to sdp record
 * PARAMETERS
 *  HdpFeature
 * RETURNS
 * the assigned ID
 *  
 *****************************************************************************/

BtStatus HdpSdpDB_AddFeature(U16 mdepId, U8 role, U16 dataType, char *descriotion)
{
	MDEP *mdep = HdpSdpDB_FindAvailableMdep();
	OS_Report("HdpSdpDB_AddFeature: mdep id %d, role %d, type %d", 
				mdepId, role, dataType);
	if (NULL == mdep)
	{
		return BT_STATUS_FAILED;
	}
	mdep->inUse = TRUE;
	BT_HdpRecord.activeNum ++;
	mdep->feature.mdepId = mdepId;
	mdep->feature.role = role;
	mdep->feature.dataType = dataType;
	if (descriotion != NULL && 0 < OS_StrLen(descriotion))
	{
		OS_StrCpy(mdep->feature.descriotion, descriotion);
	}
	return HdpSdpDB_Update();
}
/*****************************************************************************
 * FUNCTION
 *  HdpSdpDB_AddFeature
 * DESCRIPTION
 *  remove a feature from sdp record
 * PARAMETERS
 *  HdpFeature
 * RETURNS
 * the assigned ID
 *  
 *****************************************************************************/

BtStatus HdpSdpDB_RemoveFeature(MdepId id)
{
	MDEP *mdep = HdpSdpDB_FindMdep(id);
	if (NULL == mdep)
	{
		return BT_STATUS_NOT_FOUND;
	}
	BT_HdpRecord.activeNum --;
	OS_MemSet(mdep, 0x0, sizeof(MDEP));

	return HdpSdpDB_Update();
}


/*****************************************************************************
 * FUNCTION
 *  HdpSdpDB_getBuffer
 * DESCRIPTION
 *  get psm buffer
 * PARAMETERS
 *  uuid
 * RETURNS
 *  pointer to buffer
 *  
 *****************************************************************************/

U8 *HdpSdpDB_getBuffer(U8 uuid) 
{
	switch(uuid)
	{
		case PROT_MCAP_CONTROL_CHANNEL:
			return HdpProtoDescList + 8;
		case PROT_MCAP_DATA_CHANNEL:
			return HdpAddProtoDescList + 10;
		default:
			return NULL;
	}
}

/*****************************************************************************
 * FUNCTION
 *  HdpSdpDB_ReplacePsm
 * DESCRIPTION
 *  remove a feature from sdp record
 * PARAMETERS
 *  controlPsm
 *  dataPsm
 * RETURNS
 *  fail or success
 *  
 *****************************************************************************/

BtStatus HdpSdpDB_ReplacePsm(U16 controlPsm, U16 dataPsm)
{	
	U8 *bufferC = HdpSdpDB_getBuffer(PROT_MCAP_CONTROL_CHANNEL);
	U8 *bufferD = HdpSdpDB_getBuffer(PROT_MCAP_DATA_CHANNEL);
	if (NULL == bufferC || NULL == bufferD)
	{
		return BT_STATUS_NOT_FOUND;
	}
	SDP_PutU16(bufferC, controlPsm);
	SDP_PutU16(bufferD, dataPsm);
	return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  HdpSdpDB_ReplacePsm
 * DESCRIPTION
 *  remove a feature from sdp record
 * PARAMETERS
 *  controlPsm
 *  dataPsm
 * RETURNS
 *  fail or success
 *  
 *****************************************************************************/


void HdpSdpDB_UpdateServiceClass()
{
	U8 index = 0;
	MDEP *mdep = NULL;
	BOOL sinkSupported = FALSE;
	BOOL sourceSupported = FALSE;
	U8 *value = HdpClassId;
	SdpAttribute *attribute = NULL;
	
	attribute = HdpSdpDB_FindAttribute(AID_SERVICE_CLASS_ID_LIST);
	if (NULL == attribute)
	{
		OS_Report("[HDP][SDP] fatal error: the sdp record does not contains AID_SERVICE_CLASS_ID_LIST");
		return;
	}

	for ( ; index < HDP_INSTANCE_MAX_NUM; index++)
	{
		mdep = &(BT_HdpRecord.mdeplist[index]);
		if (FALSE == mdep->inUse)
		{
			continue;
		}
		if (HDP_ROLE_SINK == mdep->feature.role)
		{
			sinkSupported = TRUE;
		}
		if (HDP_ROLE_SOURCE == mdep->feature.role)
		{
			sourceSupported = TRUE;
		}
	}
	HdpClassId[1] = 0;
	if (sinkSupported)
	{
		HdpClassId[1] += 3;
		value += 3;
		SDP_PutU16(value, SC_HDP_SINK);
	}
	if (sourceSupported)
	{
		HdpClassId[1] += 3;
		value += 3;
		SDP_PutU16(value, SC_HDP_SOURCE);
	}
	attribute->len = HdpClassId[1] + 2;
}

//TODO: add description in MEDP 
U8 HdpSdpDB_FillFeature(U8 * buffer, const HdpFeature *feature)
{
	U8 * data = HdpMdepList_Feature;
	U8 totalLen;
	U8 descLen = OS_StrLen(feature->descriotion);
	U8 dataLen = ELEMENT_FEATURE_BASIC_SIZE;
	if (descLen > 0)
	{
	//	dataLen = ELEMENT_FEATURE_BASIC_SIZE + ELEMENT_SEQUENCE_SIZE + descLen;
	}
	totalLen = dataLen + ELEMENT_SEQUENCE_SIZE;

	data++;
	*data = dataLen;
	
	data += 2;
	*data = feature->mdepId;
	data += 2;
	SDP_PutU16(data, feature->dataType);		
	data += 3;
	*data = feature->role;
		
/*	if (descLen > 0)
	{
		data += 2;
		*data = descLen;
		data ++;
		OS_StrnCpy(data, feature->descriotion, descLen);

		OS_Report("[HDP]HdpSdpDB_FillFeature: len is %d, %s", descLen, feature->descriotion);
	}*/

	OS_MemCopy(buffer, HdpMdepList_Feature, totalLen);

	return totalLen;
}



void HdpSdpDB_UpdateFeatures()
{
	U8 totallen= 0;
	U8 totalSize = ELEMENT_SEQUENCE_SIZE + ELEMENT_FEATURE_MAX_SIZE * BT_HdpRecord.activeNum;
	U8 index = 0;
	MDEP *mdep = NULL;
	U8 *value = NULL;
	SdpAttribute *attribute = NULL;
	
	attribute = HdpSdpDB_FindAttribute(AID_HDP_SUPPORTED_FEATURES);
	if (NULL == attribute)
	{
		OS_Report("[HDP][SDP] fatal error: the sdp record does not contains SUPPORTED_FEATURES");
		return;
	}
       attribute->len = 0;
       attribute->value = NULL;
       if (NULL != HdpSupportedFeatures)
	{
		OS_Report("[HDP]HdpSupportedFeatures reset: %p", HdpSupportedFeatures);
		hdp_util_free(HdpSupportedFeatures);
		HdpSupportedFeatures = NULL;
	}
	HdpSupportedFeatures = (U8 *)hdp_util_malloc( totalSize);
	OS_Report("[HDP]HdpSupportedFeatures malloc: %p", HdpSupportedFeatures);
	if (NULL == HdpSupportedFeatures)
	{
		OS_Report("[HDP][SDP] fatal error: fail to allocate mempory");
		return;
	}
	
	value = HdpSupportedFeatures;
	value += ELEMENT_SEQUENCE_SIZE;         //point to the first element entry
	for ( ; index < HDP_INSTANCE_MAX_NUM; index++)
	{
		U8 subLen = 0;
		mdep = &(BT_HdpRecord.mdeplist[index]);
		if (FALSE == mdep->inUse)
		{
			continue;
		}
		subLen = HdpSdpDB_FillFeature(value, &mdep->feature);
		value += subLen;		
		totallen += subLen;	
		OS_Report("[HDP]sub len for mdep id %d is %d", mdep->feature.mdepId, subLen);
	}
	if (totallen <= 0) 
	{
		OS_Report("[HDP]attribute->value reset: %d", attribute->value);
		hdp_util_free(HdpSupportedFeatures);
		HdpSupportedFeatures = NULL;
	} 
	else
	{
		attribute->len = totallen + ELEMENT_SEQUENCE_SIZE;
		HdpMdepList_Header[1] = totallen;
		OS_MemCopy(HdpSupportedFeatures, HdpMdepList_Header, sizeof(HdpMdepList_Header));
              attribute->value = HdpSupportedFeatures;
	}
}

BtStatus HdpSdpDB_Update()
{
	/*first update service class base on features*/
	BtStatus status;
	HdpSdpDB_UpdateServiceClass();
	HdpSdpDB_UpdateFeatures();

	if((BT_HdpRecord.activeNum > 0))
	{
		return HdpSdpDB_Registration();
	} 
	else
	{
		return HdpSdpDB_DeRegistration();

	} 
}

/*****************************************************************************
 * FUNCTION
 *  HdpSdpDB_FindAvailableMdep
 * DESCRIPTION
 *  find available mdep
 * PARAMETERS
 *  
 * RETURNS
 * pointer a mdep
 *  
 *****************************************************************************/

MDEP * HdpSdpDB_FindAvailableMdep ()
{
	U8 index = 0;
	for ( ; index < HDP_INSTANCE_MAX_NUM; index++)
	{
		if (BT_HdpRecord.mdeplist[index].inUse == FALSE)
		{
			return &(BT_HdpRecord.mdeplist[index]);
		}
	}
	return NULL;
}
MDEP * HdpSdpDB_FindMdep (MdepId id)
{
	U8 index = 0;
	for ( ; index < HDP_INSTANCE_MAX_NUM; index++)
	{
		if (BT_HdpRecord.mdeplist[index].inUse == TRUE &&
			BT_HdpRecord.mdeplist[index].feature.mdepId == id)
		{
			return &(BT_HdpRecord.mdeplist[index]);
		}
	}
	return NULL;
}
SdpAttribute * HdpSdpDB_FindAttribute (SdpAttributeId id)
{
	U8 record_num = sizeof(HdpSdpAttributes) / sizeof(*(HdpSdpAttributes));
	SdpAttribute *attribute = NULL;
	U8 index = 0;
	for (index = 0; index < record_num; index ++)
	{		
		attribute = &HdpSdpAttributes[index];
		if (attribute->id == id)
		{
			return attribute;
		}
	}
	return NULL;
}

#endif /* __BT_HDP_PROFILE__ */
