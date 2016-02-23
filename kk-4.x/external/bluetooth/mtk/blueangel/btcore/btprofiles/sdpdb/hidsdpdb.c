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

#include "bt_common.h"
#include "stdio.h"
#include "sdp.h"
#include "hid.h"

typedef struct _HidRecords
{
    BOOL registered;
    SdpRecord record;   /* SDP record */
} HidRecords;

HidRecords BT_HidRecord;

/****************************************************************************
 * SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP ServiceClassIDList
 */
static const U8 HidSrcClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),                  /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(SC_HUMAN_INTERFACE_DEVICE)   /* Human Interface Device */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by HID.
 * 
 * Value of the protocol descriptor list for the HID Profile.
 * This structure is a ROM'able representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service.
 */
static const U8 HidProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(13), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for L2CAP, 6 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),         /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_HID_CTRL),    /* HID PSM */

    /*
     * Next protocol descriptor in the list is HID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for AVDTP, 2 bytes */
    SDP_UUID_16BIT(PROT_HIDP)   /* HID UUID */
};

/*---------------------------------------------------------------------------
 * SDP AdditionalProtocol Descriptor List object registered by HID.
 * 
 * Value of the additional protocol descriptor list for the HID.
 * This structure is a ROM'able representation of the RAM structure.
 * During HCRP_ServerRegisterSDP, this structure is copied into the RAM 
 * structure and used to register the client or server.
 */
static const U8 HidAddProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(15), /* Data element sequence, 15 bytes  */

    /*
     * Each element of this list is a protocol descriptor list.  For
     * HID, there is only one list.
     */

    SDP_ATTRIB_HEADER_8BIT(13), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.  The second element is HID.
     */
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP),         /* Uuid16 L2CAP                    */
    SDP_UINT_16BIT(BT_PSM_HID_INTR),    /* L2CAP PSM (varies)         */

    /*
     * The next protocol descriptor is for HID. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for HCRP Control */
    SDP_UUID_16BIT(PROT_HIDP)   /* Uuid16 HID Channel (varies)           */
};

/*---------------------------------------------------------------------------
 * SDP Language Base Attribute ID List.  
 *
 * Only defines the English language.  This must be modified if other language
 * support is required.
 */
static const U8 HidLangBaseAttrIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),

    /*
     * Elements of this list occur in triplets that describe the Language
     * Base Attribute ID.
     */
    SDP_UINT_16BIT(0x656e), /* English "en" */
    SDP_UINT_16BIT(0x006a), /* UTF-8 endoding */
    SDP_UINT_16BIT(0x0100)  /* Primary Language Base ID */
        /*
         * Additional languages may be defined.  Each language is defined by a
         * "triplet" of 16-bit elements.
         * 
         * The first element of the triplet defines the language.  The language is
         * encoded according to ISO 639:1988 (E/F):  "Code for the representation
         * of names of languages."  
         * 
         * The second element describes the character encoding.  Values for
         * character encoding can be found in IANA's database2, and have the values
         * that are referred to as MIBEnum values. The recommended character
         * encoding is UTF-8.
         * 
         * The third element of each triplet contains an attribute ID that serves as
         * the base attribute ID for the natural language in the service record.
         * 
         * The length of this attribute must be modified to include the length
         * of any additional triplets.
         */
};

/*---------------------------------------------------------------------------
 * HID Language ID Base List.
 *
 * Defines how Bluetooth strings are mapped to HID LANGID and string 
 * indices.  For a complete description, see section 7.11.7 of the
 * Bluetooth HID Profile specification.  The default value is a sample
 * that supports only United States English.
 */
static const U8 HidLangIdBaseList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* Data element sequence */

    /*
     * Each element of the list is a data element sequence describing a HID
     * language ID base.  Only one element is included.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence. */
    SDP_UINT_16BIT(0x0409),     /* Language = English (United States) */
    SDP_UINT_16BIT(0x0100)      /* Bluetooth String Offset */
        /*
         * Additional languages may be defined.  Each language is defined by
         * a data element sequence with 2 elements.
         * 
         * The first element defines the language and is encoded according to the
         * the "Universal Serial Bus Language Identifiers (LANGIDs)" specification.
         * 
         * The second element defines the Bluetooth base attribute ID as is defined
         * in the Language Base Attribute ID List (see HidLangBaseAttrIdList above).
         * 
         * Headers must be added along with the actual element (HID Language ID
         * base) data.  Also,  the length in the first header must be adjusted.
         */
};

/*---------------------------------------------------------------------------
 * SDP Public Browse Group.
 */
static const U8 HidBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/*---------------------------------------------------------------------------
 * SDP Profile Descriptor List
 */
static const U8 HidProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* Data element sequence, 8 bytes */
    SDP_ATTRIB_HEADER_8BIT(6),  /*
                                 * Data element sequence for ProfileDescriptor,
                                 * 6 bytes.
                                 */
    SDP_UUID_16BIT(SC_HUMAN_INTERFACE_DEVICE),
    SDP_UINT_16BIT(0x0100)
};

/*---------------------------------------------------------------------------
 * Device Release Number.
 */
static const U8 HidDeviceRelease[] = 
{
    SDP_UINT_16BIT(HID_DEVICE_RELEASE)
};

/*---------------------------------------------------------------------------
 * Parser Version.
 */
static const U8 HidParserVersion[] = 
{
    SDP_UINT_16BIT(HID_PARSER_VERSION)
};

/*---------------------------------------------------------------------------
 * Device Subclass.
 */
static const U8 HidDeviceSubclass[] = 
{
    SDP_UINT_8BIT(HID_DEVICE_SUBCLASS)
};

/*---------------------------------------------------------------------------
 * Country Code.
 */
static const U8 HidCountrycode[] = 
{
    SDP_UINT_8BIT(HID_COUNTRY_CODE)
};

/*---------------------------------------------------------------------------
 * Virtual Cable.
 */
static const U8 HidVirtualCable[] = 
{
    SDP_BOOL(HID_VIRTUAL_CABLE)
};

/*---------------------------------------------------------------------------
 * Initiate Reconnect.
 */
static const U8 HidReconnectInitiate[] = 
{
    SDP_BOOL(HID_RECONNECT_INITIATE)
};

/*---------------------------------------------------------------------------
 * Descriptor List.
 */
static const U8 HidDescriptorList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(HID_DESCRIPTOR_LEN + 6), /* Data element sequence */

    /*
     * Each element of the list is a HID descriptor which is a
     * data element sequence.
     */
    SDP_ATTRIB_HEADER_8BIT(HID_DESCRIPTOR_LEN + 4),
    SDP_UINT_8BIT(HID_DESCRIPTOR_TYPE), /* Report Descriptor Type */

    /* Length of the HID descriptor */
    SDP_TEXT_8BIT(HID_DESCRIPTOR_LEN),

    /* The actual descriptor is defined in hid.h or in overide.h */
    HID_DESCRIPTOR
        /*
         * Addition descriptors may be added, but the header must be added
         * along with the actual descriptor data.  Also, all lengths in the 
         * headers above must be adjusted.
         */
};

/*---------------------------------------------------------------------------
 * Profile Version
 */
static const U8 HidProfileVersion[] = 
{
    SDP_UINT_16BIT(0x0100)
};

/*---------------------------------------------------------------------------
 * Boot Device
 */
static const U8 HidBootDevice[] = 
{
    SDP_BOOL(HID_BOOT_DEVICE)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ServiceName
 *
 * This is the English string.  Other languguages can be defined.
 */
static const U8 HidServiceName[] = 
{
    SDP_TEXT_8BIT(23),  /* Null terminated text string */
    'H', 'u', 'm', 'a', 'n', ' ', 'i', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e', ' ', 'd', 'e', 'v',
    'i', 'c', 'e', '\0'
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ServiceDescription
 *
 * This is the English string.  Other languguages can be defined.
 */
static const U8 HidServiceDescription[] = 
{
    SDP_TEXT_8BIT(23),  /* Null terminated text string */
    'H', 'u', 'm', 'a', 'n', ' ', 'i', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e', ' ', 'd', 'e', 'v',
    'i', 'c', 'e', '\0'
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ProviderName
 *
 * This is the English string.  Other languguages can be defined.
 */
static const U8 HidProviderName[] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string */
    'M', 'e', 'd', 'i', 'a', 't', 'e', 'k', ' ', 'I', 'n', 'c', '\0'
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  HidSdpDisable
 */
static const U8 HidSdpDisable[] = 
{
    SDP_BOOL(FALSE)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  HidBatteryPower
 */
static const U8 HidBatteryPower[] = 
{
    SDP_BOOL(HID_BATTERY_POWER)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  HidRemoteWake
 */
static const U8 HidRemoteWake[] = 
{
    SDP_BOOL(HID_REMOTE_WAKE)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  HidSupervisionTimeout
 */
static const U8 HidSupervisionTimeout[] = 
{
    SDP_UINT_16BIT(HID_SUPERVISION_TIMEOUT)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  HidNormallyConnectable
 */
static const U8 HidNormallyConnectable[] = 
{
    SDP_BOOL(HID_NORMALLY_CONNECTABLE)
};

/*---------------------------------------------------------------------------
 * HID attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * HID SDP record.
 */
SdpAttribute HidSdpAttributes[] = 
{
    /* Mandatory SDP Attributes */
    /* HID class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, HidSrcClassId),
    /* HID protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, HidProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, HidBrowseGroup),
    /* Language Base ID List */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, HidLangBaseAttrIdList),
    /* HID profile descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, HidProfileDescList),
    /* HID additional protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_ADDITIONAL_PROT_DESC_LISTS, HidAddProtoDescList),
    /*
     * Optional Human readable attributes.  The strings provided are English.
     * Other languages can be added.  Each language should have a different
     * Language Base Attribute ID (defined in HidLangBaseAttrIdList).  This ID
     * is added to the universal attribute ID for Service Name, Service
     * Description, and Provider Name.
     */
    /* HID Service Name in English */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), HidServiceName),
    /* HID Service Description in English */
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), HidServiceDescription),
    /* HID Provider Name in English */
    SDP_ATTRIBUTE((AID_PROVIDER_NAME + 0x0100), HidProviderName),
    /* Mandatory HID attributes */
    /* Device release number */
    SDP_ATTRIBUTE(AID_HID_DEVICE_RELEASE, HidDeviceRelease),
    /* HID parser version */
    SDP_ATTRIBUTE(AID_HID_PARSER_VERSION, HidParserVersion),
    /* Device subclass */
    SDP_ATTRIBUTE(AID_HID_DEVICE_SUBCLASS, HidDeviceSubclass),
    /* Country Code */
    SDP_ATTRIBUTE(AID_HID_COUNTRY_CODE, HidCountrycode),
    /* Virtual Cable */
    SDP_ATTRIBUTE(AID_HID_VIRTUAL_CABLE, HidVirtualCable),
    /* Device initiates reconnect */
    SDP_ATTRIBUTE(AID_HID_RECONNECT_INIT, HidReconnectInitiate),
    /* HID descriptor list */
    SDP_ATTRIBUTE(AID_HID_DESCRIPTOR_LIST, HidDescriptorList),
    /* Language ID Base List */
    SDP_ATTRIBUTE(AID_HID_LANG_ID_BASE_LIST, HidLangIdBaseList),
    /* Optional HID attibutes */
    /* SDP Disable/Enable */
    SDP_ATTRIBUTE(AID_HID_SDP_DISABLE, HidSdpDisable),
    /* Battery powered */
    SDP_ATTRIBUTE(AID_HID_BATTERY_POWER, HidBatteryPower),
    /* Device support of remote wakeup */
    SDP_ATTRIBUTE(AID_HID_REMOTE_WAKE, HidRemoteWake),
    /* Mandatory HID attribute */
    /* HID profile version */
    SDP_ATTRIBUTE(AID_HID_PROFILE_VERSION, HidProfileVersion),
    /* Optional HID attributes */
    /* Recommended supervision timeout */
    /* SDP_ATTRIBUTE(AID_HID_SUPERV_TIMEOUT, HidSupervisionTimeout), */
    /* Device connectability */
    SDP_ATTRIBUTE(AID_HID_NORM_CONNECTABLE, HidNormallyConnectable),
    /* Mandatory HID attribute */
    /* Support for boot protocol */
    SDP_ATTRIBUTE(AID_HID_BOOT_DEVICE, HidBootDevice),
};

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  HidDeviceSdpDB_Registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 HidDeviceSdpDB_Registration(void)
{
    BtStatus status;

    BT_HidRecord.registered = TRUE;
    BT_HidRecord.record.num = sizeof(HidSdpAttributes) / sizeof(*(HidSdpAttributes));
    /* Retrieve the server channel from the service channel */
    BT_HidRecord.record.attribs = (SdpAttribute*) & HidSdpAttributes;
    status = SDP_AddRecord(&BT_HidRecord.record);
    Assert((status != BT_STATUS_INVALID_PARM));
    kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
    return (U8)status;
}


/*****************************************************************************
 * FUNCTION
 *  HidDeviceSdpDB_DeRegistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 HidDeviceSdpDB_DeRegistration(void)
{
    BtStatus status;

    status = SDP_RemoveRecord(&BT_HidRecord.record);
    return (U8)status;
}

void HidDeviceSdpDB_ReplaceRecord(U16 uuid, U8 *attribute, U8 size)
{
    U8 i = 0;
    U8 record_num;

    record_num = sizeof(HidSdpAttributes) / sizeof(*(HidSdpAttributes));
    for(i=0;i < record_num; i++)
    {
        if(HidSdpAttributes[i].id == uuid)
        {
            HidSdpAttributes[i].value = attribute;
            HidSdpAttributes[i].len = size;
        }
    }

}
#endif /* defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__) */
