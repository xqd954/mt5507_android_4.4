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
#include <btalloc.h>
#include "sdp.h"
#include <pan.h>

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/****************************************************************************
 *
 * SDP objects registered by PAN Servers.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP PANU Service Class ID.
 */
static const U8 PanPANUServiceClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(3),  /* UUID Data Element Sequence */
    SDP_UUID_16BIT(SC_PANU)     /* PANU UUID */
};

/*---------------------------------------------------------------------------
 * SDP PANU Bluetooth Profile Descriptor List.
 */
static const U8 PanPANUBtProfDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),      /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES for BPP, 6 bytes              */
    SDP_UUID_16BIT(SC_PANU),        /* Uuid16 BNEP protocol type         */
    SDP_UINT_16BIT(0x0100)          /* Uint16 BNEP version               */
};

/*---------------------------------------------------------------------------
 * SDP PANU service name.
 */
static const U8 PanPANUServiceNameSize[] = {
    PAN_PANU_SERVICE_NAME
};

static const U8 PanPANUServiceName[] = {
    SDP_TEXT_8BIT(sizeof(PanPANUServiceNameSize)),
    PAN_PANU_SERVICE_NAME           /* Null terminated text string */
};

/*---------------------------------------------------------------------------
 * SDP PANU service description.
 */
static const U8 PanPANUServiceDescriptionSize[] = {
    PAN_PANU_SERVICE_DESCRIPTION
};

static const U8 PanPANUServiceDescription[] = {
    SDP_TEXT_8BIT(sizeof(PanPANUServiceDescriptionSize)),
    PAN_PANU_SERVICE_DESCRIPTION    /* Null terminated text string */
};

/*---------------------------------------------------------------------------
 * SDP GN Service Class ID.
 */
static const U8 PanGNServiceClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(3),  /* UUID Data Element Sequence */
    SDP_UUID_16BIT(SC_GN)       /* GN UUID */
};

/*---------------------------------------------------------------------------
 * SDP GN Bluetooth Profile Descriptor List.
 */
static const U8 PanGNBtProfDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),      /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES for BPP, 6 bytes              */
    SDP_UUID_16BIT(SC_GN),          /* Uuid16 BNEP protocol type         */
    SDP_UINT_16BIT(0x0100)          /* Uint16 BNEP version               */
};

/*---------------------------------------------------------------------------
 * SDP GN service name.
 */
static const U8 PanGNServiceNameSize[] = {
    PAN_GN_SERVICE_NAME
};

static const U8 PanGNServiceName[] = {
    SDP_TEXT_8BIT(sizeof(PanGNServiceNameSize)),
    PAN_GN_SERVICE_NAME             /* Null terminated text string */
};

/*---------------------------------------------------------------------------
 * SDP GN service description.
 */
static const U8 PanGNServiceDescriptionSize[] = {
    PAN_GN_SERVICE_DESCRIPTION
};

static const U8 PanGNServiceDescription[] = {
    SDP_TEXT_8BIT(sizeof(PanGNServiceDescriptionSize)),
    PAN_GN_SERVICE_DESCRIPTION      /* Null terminated text string */
};

/*---------------------------------------------------------------------------
 * SDP GN IPv4 Subnet.
 */
static const U8 PanGNIPv4SubnetSize[] = {
    PAN_GN_IPV4_SUBNET
};

static const U8 PanGNIPv4Subnet[] = {
    SDP_TEXT_8BIT(sizeof(PanGNIPv4SubnetSize)),    /* text string */
    PAN_GN_IPV4_SUBNET
};

/*---------------------------------------------------------------------------
 * SDP GN IPv6 Subnet.
 */
static const U8 PanGNIPv6SubnetSize[] = {
    PAN_GN_IPV6_SUBNET
};

static const U8 PanGNIPv6Subnet[] = {
    SDP_TEXT_8BIT(sizeof(PanGNIPv6SubnetSize)),    /* text string */
    PAN_GN_IPV6_SUBNET
};


/*---------------------------------------------------------------------------
 * SDP NAP Service Class ID.
 */
static const U8 PanNAPServiceClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(3),  /* UUID Data Element Sequence */
    SDP_UUID_16BIT(SC_NAP)      /* NAP UUID */
};

/*---------------------------------------------------------------------------
 * SDP NAP Bluetooth Profile Descriptor List.
 */
static const U8 PanNAPBtProfDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),      /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES for BPP, 6 bytes              */
    SDP_UUID_16BIT(SC_NAP),         /* Uuid16 BNEP protocol type         */
    SDP_UINT_16BIT(0x0100)          /* Uint16 BNEP version               */
};

/*---------------------------------------------------------------------------
 * SDP NAP service name.
 */
static const U8 PanNAPServiceNameSize[] = {
    PAN_NAP_SERVICE_NAME
};

static const U8 PanNAPServiceName[] = {
    SDP_TEXT_8BIT(sizeof(PanNAPServiceNameSize)),
    PAN_NAP_SERVICE_NAME            /* Null terminated text string */
};

/*---------------------------------------------------------------------------
 * SDP NAP service description.
 */
static const U8 PanNAPServiceDescriptionSize[] = {
    PAN_NAP_SERVICE_DESCRIPTION
};

static const U8 PanNAPServiceDescription[] = {
    SDP_TEXT_8BIT(sizeof(PanNAPServiceDescriptionSize)),
    PAN_NAP_SERVICE_DESCRIPTION    /* Null terminated text string */
};

/*---------------------------------------------------------------------------
 * SDP NAP Network Access Type.
 */
static const U8 PanNAPNetAccessType[] = {
    SDP_UINT_16BIT(PAN_NAP_NET_ACCESS_TYPE) /* Uint16 Network Access Type */
};

/*---------------------------------------------------------------------------
 * SDP NAP Max Network Access Rate.
 */
static const U8 PanNAPMaxNetAccessRate[] = {
    SDP_UINT_32BIT(PAN_NAP_MAX_NET_ACCESS_RATE) /* Uint32 Max Net Access Rate */
};

/*---------------------------------------------------------------------------
 * SDP NAP IPv4 Subnet.
 */
static const U8 PanNAPIPv4SubnetSize[] = {
    PAN_NAP_IPV4_SUBNET
};

static const U8 PanNAPIPv4Subnet[] = {
    SDP_TEXT_8BIT(sizeof(PanNAPIPv4SubnetSize)),    /* text string */
    PAN_NAP_IPV4_SUBNET
};

/*---------------------------------------------------------------------------
 * SDP NAP IPv6 Subnet.
 */
static const U8 PanNAPIPv6SubnetSize[] = {
    PAN_NAP_IPV6_SUBNET
};

static const U8 PanNAPIPv6Subnet[] = {
    SDP_TEXT_8BIT(sizeof(PanNAPIPv6SubnetSize)),    /* text string */
    PAN_NAP_IPV6_SUBNET
};


/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by the PAN Service.
 * 
 * Value of the protocol descriptor list for the PAN Service.
 * This structure is a ROM'able representation of the RAM structure.
 * During BNEP_Init, this structure is copied into the RAM structure
 * and used to register the PAN. A RAM structure must be used to be
 * able to dynamically set certain values.
 */
static const U8 PanPktTypeListSize[] = {
    PAN_PKT_TYPE_LIST
};

static const U8 PanProtoDescList[] = {
    /* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(18 + sizeof(PanPktTypeListSize)),

    /* Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.
     */
    SDP_ATTRIB_HEADER_8BIT(6), /* Data element sequence for L2CAP, 6 bytes */     
    SDP_UUID_16BIT(PROT_L2CAP),     /* Uuid16 L2CAP                  */
    SDP_UINT_16BIT(BT_PSM_BNEP),    /* BNEP PSM */

    /* The next protocol descriptor is for BNEP. It contains 3 elements
     * UUID, Version and list of packet types supported.
     */
    SDP_ATTRIB_HEADER_8BIT(8+sizeof(PanPktTypeListSize)), /* Data element sequence for BNEP, 3 bytes */     
    SDP_UUID_16BIT(PROT_BNEP),      /* Uuid16 BNEP protocol type */
    SDP_UINT_16BIT(0x0100),         /* Uint16 BNEP version */
    SDP_ATTRIB_HEADER_8BIT(sizeof(PanPktTypeListSize)), /* pkt types */    
    PAN_PKT_TYPE_LIST
};


/*---------------------------------------------------------------------------
 * SDP Language Base ID List object registered by the PAN Service.
 */
static const U8 PanLangBaseIdList[] = {
    SDP_ATTRIB_HEADER_8BIT(9),      /* Data Element Sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),         /* English Language */
    SDP_UINT_16BIT(0x006A),         /* UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)          /* Primary language base Id */
};

/*---------------------------------------------------------------------------
 * SDP Service Availability object registered by the PAN Service.
 */
static const U8 PanServiceAvailability[] = {
    SDP_UINT_8BIT(0xFF)
};

/*---------------------------------------------------------------------------
 * SDP PAN security description.
 */
static const U8 PanSecurityDescription[] = {
    SDP_UINT_16BIT(PAN_SECURITY_LEVEL)       /* Uint16 Security description */
};

/*---------------------------------------------------------------------------
 * PAN Public Browse Group.
 */
static const U8 PanBrowseGroup[] = {
    SDP_ATTRIB_HEADER_8BIT(3),          /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP),  /* Public Browse Group */
};

#ifdef PAN_ROLE_NAP
/*---------------------------------------------------------------------------
 * PAN attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * PAN SDP record. The number of elements of this array is defined
 * by PAN_NAP_NUM_ATTRIBUTES. It is modified for the other profiles (PANU
 * and GN.)
 */
SdpAttribute PanNapSdpAttributes[] = {
    /* PAN service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PanNAPServiceClassId), 
    /* PAN Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PanProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, PanBrowseGroup),
    /* PAN Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, PanLangBaseIdList),
    /* PAN Service Availability attribute */
    SDP_ATTRIBUTE(AID_SERVICE_AVAILABILITY, PanServiceAvailability),
    /* PAN Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, PanNAPBtProfDescList),
    /* PAN service name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), PanNAPServiceName),
    /* PAN service description */
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), PanNAPServiceDescription),
    /* PAN security description */
    SDP_ATTRIBUTE(AID_SECURITY_DESC, PanSecurityDescription),
    /* PAN Network Access Type */
    SDP_ATTRIBUTE(AID_NET_ACCESS_TYPE, PanNAPNetAccessType),
    /* PAN Max Network Access Rate */
    SDP_ATTRIBUTE(AID_MAX_NET_ACCESS_RATE, PanNAPMaxNetAccessRate),
    /* PAN IPv4 Subnet */
    SDP_ATTRIBUTE(AID_IPV4_SUBNET, PanNAPIPv4Subnet),
    /* PAN IPv6 Subnet */
    SDP_ATTRIBUTE(AID_IPV6_SUBNET, PanNAPIPv6Subnet)
};
#endif
#ifdef PAN_ROLE_GN
SdpAttribute PanGnSdpAttributes[] = {
    /* PAN service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PanGNServiceClassId), 
    /* PAN Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PanProtoDescList),
    /* PAN Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, PanLangBaseIdList),
    /* PAN Service Availability attribute */
    SDP_ATTRIBUTE(AID_SERVICE_AVAILABILITY, PanServiceAvailability),
    /* PAN Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, PanGNBtProfDescList),
    /* PAN service name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), PanGNServiceName),
    /* PAN service description */
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), PanGNServiceDescription),
    /* PAN security description */
    SDP_ATTRIBUTE(AID_SECURITY_DESC, PanSecurityDescription),
    /* PAN IPv4 Subnet */
    SDP_ATTRIBUTE(AID_IPV4_SUBNET, PanGNIPv4Subnet),
    /* PAN IPv6 Subnet */
    SDP_ATTRIBUTE(AID_IPV6_SUBNET, PanGNIPv6Subnet)
};
#endif
#ifdef PAN_ROLE_PANU
SdpAttribute PanPanuSdpAttributes[] = {
    /* PAN service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PanPANUServiceClassId), 
    /* PAN Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PanProtoDescList),
    /* PAN Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, PanLangBaseIdList),
    /* PAN Service Availability attribute */
    SDP_ATTRIBUTE(AID_SERVICE_AVAILABILITY, PanServiceAvailability),
    /* PAN Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, PanPANUBtProfDescList),
    /* PAN service name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), PanPANUServiceName),
    /* PAN service description */
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), PanPANUServiceDescription),
    /* PAN security description */
    SDP_ATTRIBUTE(AID_SECURITY_DESC, PanSecurityDescription),
};
#endif

#ifdef PAN_ROLE_NAP
typedef struct _NapSdpRecord
{
	BOOL registered;
	SdpRecord record;	/* SDP record */
} NapSdpRecord;

NapSdpRecord       BT_NapSdpRecord;
//SdpAttribute    BT_NapSdpAttrib[PAN_NAP_NUM_ATTRIBUTES];
#endif

#ifdef PAN_ROLE_GN
typedef struct _GnSdpRecord
{
	BOOL registered;
	SdpRecord record;	/* SDP record */
} GnSdpRecord;

GnSdpRecord       BT_GnSdpRecord;
//SdpAttribute    BT_GnSdpAttrib[PAN_GN_NUM_ATTRIBUTES];
#endif

#ifdef PAN_ROLE_PANU
typedef struct _PanuSdpRecord
{
	BOOL registered;
	SdpRecord record;	/* SDP record */
} PanuSdpRecord;

PanuSdpRecord       BT_PanuSdpRecord;
//SdpAttribute    BT_PanuSdpAttrib[PAN_PANU_NUM_ATTRIBUTES];
#endif

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

 /*---------------------------------------------------------------------------
  * 		   PanRegisterSdp()
  *---------------------------------------------------------------------------
  *
  * Synopsis:  Registers an SDP entry for a PAN.
  *
  * Return:    BT_STATUS_FAILED
  * 		   (Also see SDP_AddRecord)
  */
 U8 PanSdpDB_AddRecord(U16 service_type)
 {
	 BtStatus status = BT_STATUS_SUCCESS;
 
	 /* Copy the SDP attributes to RAM */
 
	 /* Modify the attributes */
#ifdef PAN_ROLE_PANU
	if (service_type == PANSVC_PANU)
	{
		BT_PanuSdpRecord.registered = TRUE;
		BT_PanuSdpRecord.record.num = sizeof(PanPanuSdpAttributes) / sizeof(*(PanPanuSdpAttributes));
		BT_PanuSdpRecord.record.attribs = (SdpAttribute*) & PanPanuSdpAttributes;
		BT_PanuSdpRecord.record.classOfDevice = COD_NETWORKING;
		status += SDP_AddRecord(&BT_PanuSdpRecord.record);
	} 
#endif 
#ifdef PAN_ROLE_GN
	if (service_type == PANSVC_GN)
	{
		BT_GnSdpRecord.registered = TRUE;
		BT_GnSdpRecord.record.num = sizeof(PanGnSdpAttributes) / sizeof(*(PanGnSdpAttributes));
		BT_GnSdpRecord.record.attribs = (SdpAttribute*) & PanGnSdpAttributes;
		BT_GnSdpRecord.record.classOfDevice = COD_NETWORKING;
		status += SDP_AddRecord(&BT_GnSdpRecord.record);
	} 
#endif 
#ifdef PAN_ROLE_NAP
	if (service_type == PANSVC_NAP)
	{
		BT_NapSdpRecord.registered = TRUE;
		BT_NapSdpRecord.record.num = sizeof(PanNapSdpAttributes) / sizeof(*(PanNapSdpAttributes));
		BT_NapSdpRecord.record.attribs = (SdpAttribute*) & PanNapSdpAttributes;
		BT_NapSdpRecord.record.classOfDevice = COD_NETWORKING;
		status += SDP_AddRecord(&BT_NapSdpRecord.record);
	}
#endif 
	 return (U8)status;
 }
 
 
 /*---------------------------------------------------------------------------
  * 		   PanDeregisterSdp()
  *---------------------------------------------------------------------------
  *
  * Synopsis:  Deregisters an SDP entry.
  *
  * Return:    BT_STATUS_FAILED
  * 		   (Also see SDP_RemoveRecord)
  */
 U8 PanSdpDB_RemoveRecord(U16 service_type)
 {
	 BtStatus status = BT_STATUS_SUCCESS;
 
#ifdef PAN_ROLE_PANU
	if (BT_PanuSdpRecord.registered && (service_type == PANSVC_PANU))
		 status += SDP_RemoveRecord(&BT_PanuSdpRecord.record);
#endif	 
#ifdef PAN_ROLE_GN
	if (BT_GnSdpRecord.registered && (service_type == PANSVC_GN))
		 status += SDP_RemoveRecord(&BT_GnSdpRecord.record);
#endif	 
#ifdef PAN_ROLE_NAP
	if (BT_NapSdpRecord.registered && (service_type == PANSVC_NAP))
		 status += SDP_RemoveRecord(&BT_NapSdpRecord.record);
#endif	 

	 return (U8)status;
 }

