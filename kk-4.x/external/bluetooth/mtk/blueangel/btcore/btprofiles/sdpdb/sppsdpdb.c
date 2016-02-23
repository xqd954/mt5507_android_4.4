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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * sppsdpdb.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions and global variable to the SPP, DUN, LAP, FAX Service Records. 
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 04 19 2011 sh.lai
 * [ALPS00143428] [Android Build Warning Issue] mediatek/source/external/bluetooth/blueangel/btcore/btprofiles/sdpdb/sppsdpdb.c
 * .
 *
 * Jun 27 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Oct 22 2007 mtk01411
 * [MAUI_00563448] [Bluetooth] patch BT code.
 * 
 *
 * Jul 15 2007 mtk01411
 * [MAUI_00416972] [Bluetooth] SPP with SCO functionality
 * 
 *
 * May 11 2007 mtk01411
 * [MAUI_00391541] [Bluetooth BQB PTS2.1.1.0 Testing] Patch for IOPT test case TC_SDAS_BV_03_I
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Jul 16 2006 mtk01239
 * [MAUI_00210782] [Bluetooth][ESI]update Bluetooth codes
 * 
 *
 * Apr 24 2006 mtk01239
 * [MAUI_00189553] ESI Bluetooth project update
 * update
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

/***************************************************************************
 *
 * File:
 *     $Workfile:sppdrv.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *      This is a little glue layer that performs functions two functions:
 *
 *      1) Initialize the serial devices.
 *
 *      2) Provides a thin layer normally provided by an OS. It implements 
 *      a POSIX-like API for the application to call strictly for 
 *      demonstration purposes. When porting to an OS, these functions
 *      can probably be eliminated.
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
#ifdef __BT_SPP_PROFILE__

#include "sdp.h"
#include "btconfig.h"
#include "spp.h"
#include "bluetooth_spp_struct.h"

/********
Constants 
*********/

/****************************************************************************
 *
 * ROM Data
 *
 ****************************************************************************/

/*
 * Public Browse Group Attribute
 *
 *     Any SdpAttribute structure can include this attribute to add itself
 *     to the root level of the Public Browse Group.
 */
static const U8 publicBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/* Attribute value of Language Base. This can be common to all/any endpoint. */
static const U8 languageBase[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* Uint16 "en" (English) */
    SDP_UINT_16BIT(0x006A),     /* Uint16 UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Uint16 primary language base ID */
};

/*-------------------------------------------------------------------------
 * Serial Port Entity SDP attributes
 *
 * Value of service class ID is a data element sequence of UUIDs
 */
#if SPP_SERVER == XA_ENABLED
static const U8 serialPortServiceClassIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UUID_16BIT(SC_SERIAL_PORT)
};

U8 customSPPServiceClassIdList[NUM_SPP_SRVS][5];
#endif /* SPP_SERVER == XA_ENABLED */

/*-------------------------------------------------------------------------
 *
 * Dialup Networking SDP record attributes
 */
#if DUN_SERVER == XA_ENABLED

/* Attribute value of Dial-up Networking Service class ID list */
static const U8 dialupServiceClass[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),              /* Data element sequence, 6 bytes */
    /* ServiceClassIdList - most specific to most general class */
    SDP_UUID_16BIT(SC_DIALUP_NETWORKING),   /* Uuid16 Dialup Networking */
    SDP_UUID_16BIT(SC_GENERIC_NETWORKING)   /* Uuid16 Generic Networking */
};

/*
 * Attribute value of Dial-up Netwokring Protocol descriptor list.
 * Note: this is not ROM since it will be modified but put here to keep
 * SDP attributes together. 
 */
static const U8 dialupProtocolDescriptor[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12),             /* Data element sequence, 12 bytes */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),             /* Uuid16 L2CAP */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 RFCOMM */
    // SDP_UINT_8BIT(DUN_RFCOMM_SRV_CHN)    /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(DUN_CHNL_NUM_START)
};

/* Attribute value of Dial-up networking profile descriptor list */
static const U8 dialupProfileDescriptor[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),              /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_DIALUP_NETWORKING),   /* Uuid16 Dialup networking */
    SDP_UINT_16BIT(0x0102)                  /* Uint16 version number */
};

/* Attribute value of Dialup networking service name */
const U8 dialupServiceName[] = 
{
    SDP_TEXT_8BIT(18),  /* Null terminated text string, 19 bytes */
    'D', 'i', 'a', 'l', '-', 'u', 'p', ' ',
    'N', 'e', 't', 'w', 'o', 'r', 'k', 'i', 'n', 'g'
};

/* Attribute value of Dialup networking audio feedback support */
static const U8 dialupAudioFeedback[] = 
{
    SDP_BOOL(0) /* No Audio Feedback Support */
};

#endif /* DUN_SERVER == XA_ENABLED */

/*-------------------------------------------------------------------------
 *
 * Fax SDP record attributes
 */

#if FAX_SERVER == XA_ENABLED

/* Attribute value of Fax Service class ID list */
static const U8 faxServiceClass[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),              /* Data element sequence, 6 bytes */
    /* ServiceClassIdList - most specific to most general class */
    SDP_UUID_16BIT(SC_FAX),                 /* Uuid16 Fax */
    SDP_UUID_16BIT(SC_GENERIC_TELEPHONY)    /* Uuid16 Generic telephony */
};

/* Attribute value of Fax protocol descriptor list */
static const U8 faxProtocolDescriptor[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12),             /* Data element sequence, 12 bytes */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),             /* Uuid16 L2CAP */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 RFCOMM */
    // SDP_UINT_8BIT(FAX_RFCOMM_SRV_CHN)    /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(FAX_CHNL_NUM_START)
};

/* Attribute value of Fax Bluetooth profile descriptor list */
static const U8 faxProfileDescriptor[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_FAX),     /* Uuid16 fax */
    SDP_UINT_16BIT(0x0100)      /* Uint16 version number */
};

/* Attribute value of Fax Service name */
const U8 faxServiceName[] = 
{
    SDP_TEXT_8BIT(3),   /* Null terminated text string, 4 bytes */
    'F', 'a', 'x'       /* Text string */
};

/* Attribute value of Fax Audio Feeback */
static const U8 faxAudioFeedbackSupport[] = 
{
    SDP_BOOL(0) /* No Audio Feedback Support */
};

/* Attribute value of Fax Class 1 Support */
static const U8 faxClass1Support[] = 
{
    SDP_BOOL(0) /* No Fax Class 1 Support */
};

/* Attribute value of Fax Class 2.0 Support */
static const U8 faxClass20Support[] = 
{
    SDP_BOOL(0) /* No Fax Class 2.0 Support */
};

/* Attribute value of Fax Class 2 Support */
static const U8 faxClass2Support[] = 
{
    SDP_BOOL(0) /* No Fax Class 2 Support */
};
#endif /* FAX_SERVER == XA_ENABLED */

/*-------------------------------------------------------------------------
 *
 * LAN Access SDP record attributes
 */

/*
 * begin LAN service common attributes (common to all LAN servers)
 */
#if SPP_SCO_SERVER == XA_ENABLED
/* Attribute value of Spp SCO Service Name */
const U8 sppSCOServiceName[] = 
{
    SDP_TEXT_8BIT(22),  /* Null terminated text string, 21 bytes */
    'S', 'P', 'P', ' ', /* Text String */
    'w', 'i', 't', 'h', ' ', 'S', 'C',
    'o', 'C', 'a', 'p', 'a', 'b',
    'i', 'l', 'i', 't', 'y'
};

/* Attribute value of Spp SCO Service Description */
static const U8 sppSCOServiceDescription[] = 
{
    SDP_TEXT_8BIT(22),  /* Null terminated text string, 21 bytes */
    'S', 'P', 'P', ' ', /* Text String */
    'w', 'i', 't', 'h', ' ', 'S', 'C',
    'o', 'C', 'a', 'p', 'a', 'b',
    'i', 'l', 'i', 't', 'y'
};

/* Attribute value of Spp SCO Service Class Id List */
static const U8 sppSCOServiceClass[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence, 3 bytes */
    SDP_UUID_16BIT(SC_SERIAL_PORT)  /* Uuid16 LAN Access using PPP */
};

/* end Spp SCO service common attributes (common to all Spp SCO servers) */

/* Attribute value of Spp SCO Service Availability */
static const U8 sppSCOServiceAvailability[] = 
{
    SDP_UINT_8BIT(0xFF) /*
                         * Service not in use, accepting new clients 
                         * This value can vary 
                         */
};

static const U8 sppSCOSupCapabilities[] = 
{
    SDP_UINT_8BIT(0x01)
};

#endif /* SPP_SCO_SERVER == XA_ENABLED */

/* end of SDP data base records */

/******************************
Definitions Of Exported Globals 
*******************************/

#if DUN_SERVER == XA_ENABLED

SdpAttribute dialupSdpAttribs[] = 
{
    /* Dialup networking service class ID list */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, dialupServiceClass),
    /* Dialup networking protocol descriptor list */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, dialupProtocolDescriptor),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, publicBrowseGroup),
    /* Dialup networking language base attribute ID list */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, languageBase),
    /* Dialup networking Bluetooth profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, dialupProfileDescriptor),
    /* Dialup networking service name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), dialupServiceName),
    /* Audio Feedback Support - where is this attribute defined? */
    SDP_ATTRIBUTE(AID_AUDIO_FEEDBACK_SUPPORT, dialupAudioFeedback)
};
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED

SdpAttribute faxSdpAttribs[] = 
{
    /* Fax service class ID list */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, faxServiceClass),
    /* Fax protocol descriptor list */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, faxProtocolDescriptor),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, publicBrowseGroup),
    /* Fax language base attribute ID list */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, languageBase),
    /* Fax Bluetooth profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, faxProfileDescriptor),
    /* Fax service name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), faxServiceName),
    /* Fax class 1 support */
    SDP_ATTRIBUTE(AID_FAX_CLASS_1_SUPPORT, faxClass1Support),
    /* Fax class 2.0 support */
    SDP_ATTRIBUTE(AID_FAX_CLASS_20_SUPPORT, faxClass20Support),
    /* Fax class 2 support */
    SDP_ATTRIBUTE(AID_FAX_CLASS_2_SUPPORT, faxClass2Support),
    /* Audio feedback support */
    SDP_ATTRIBUTE(AID_AUDIO_FEEDBACK_SUPPORT, faxAudioFeedbackSupport)
};
#endif /* FAX_SERVER == XA_ENABLED */

#if SPP_SERVER == XA_ENABLED

/* if NUM_SPP_SRVS >=1, the following Patterns are allocated */
#if (NUM_SPP_SRVS >=1)
U8 SerialPortProtocolDescriptorListPattern[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12),         /* Data element sequence, 12 bytes */
    SDP_ATTRIB_HEADER_8BIT(3),          /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),         /* Uuid16 L2CAP */
    SDP_ATTRIB_HEADER_8BIT(5),          /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),        /* Uuid16 RFCOMM */
    SDP_UINT_8BIT(SPP_CHNL_NUM_START)   /* Uint8 RFCOMM channel number - value can vary */
};

U8 SerialPortServiceNamePattern[] = 
{
    SDP_TEXT_8BIT(12),
    'S', 'e', 'r', 'i', 'a', 'l', ' ', 'P', 'o', 'r', 't', '0'
};

/* SPP service record Pattern */
/* Attribute IDs in ascending order. */
/* SerialPortSdpAttribsPattern has 5 elements with type SdpAttribute */
SdpAttribute SerialPortSdpAttribsPattern[] = 
{
    /* Service Class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, serialPortServiceClassIdList),
    /* Protocol Descriptor List attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SerialPortProtocolDescriptorListPattern),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, publicBrowseGroup),
    /* Language Base ID List attribute */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, languageBase),
    /* Serial Port Profile Service Name */
    SDP_ATTRIBUTE(AID_SERVICE_NAME + 0x0100, SerialPortServiceNamePattern)
};

#define SerialPortProtocolDescriptorListPattern_Size 14
#define SerialPortServiceNamePattern_Size 14
#define SerialPortSdpAttribsPattern_Size 5
#endif /* (NUM_SPP_SRVS >=1) */

/* if NUM_SPP_SRVS >1, this means more space must be allocated for each SPP Server */
#if (NUM_SPP_SRVS >1)
U8 SerialPortProtocolDescriptorListArray[NUM_SPP_SRVS - 1][SerialPortProtocolDescriptorListPattern_Size];
U8 SerialPortServiceNameArray[NUM_SPP_SRVS - 1][SerialPortServiceNamePattern_Size];
U8 SerialPortServiceCustomNameArray[NUM_SPP_SRVS][SPP_MAX_SERVICE_NAME_LENGTH];
SdpAttribute SerialPortSdpAttribsArray[NUM_SPP_SRVS - 1][SerialPortSdpAttribsPattern_Size];
#endif /* (NUM_SPP_SRVS >1) */

// Modified parts are:
// For i=0 to NUM_SPP_SRVS-1

// memcpy(&SerialPortProtocolDescriptorListArray[i],SerialPortProtocolDescriptorListPattern,SerialPortProtocolDescriptorListPattern_Size);
// (&SerialPortProtocolDescriptorListArray[i])[SerialPortProtocolDescriptorListPattern_Size -1] = server_channel_num_i ;

// memcpy(&SerialPortServiceNameArray[i],SerialPortServiceNamePattern,SerialPortServiceNamePattern_Size);
// (&SerialPortServiceNameArray[i])[2] - [SerialPortServiceNamePattern_Size-1]: Can specify name
// SdpAttribute (*attrib)[SerialPortSdpAttribsPattern_Size] = (&SerialPortSdpAttribsArray[i]);
// attrib[1].value = (SerialPortProtocolDescriptorListArray[i]);
// attrib[4].value = (SerialPortServiceNameArray[i]);
// (SerialPortSdpAttribsArray[i][1]).value = SerialPortProtocolDescriptorListArray[i]
// (SerialPortSdpAttribsArray[i][4]).value = SerialPortServiceNameArray[i]

#if SPP_SCO_SERVER == XA_ENABLED

/* if NUM_SPP_SCO_SRVS >=1, the following Patterns are allocated */
#if (NUM_SPP_SCO_SRVS >= 1)
/* Attribute value of LAN Protocol Descriptor List. */
U8 SppSCOProtocolDescriptorListPattern[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12),             /* Data element sequence, 12 bytes */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),             /* Uuid16 L2CAP */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 RFCOMM */
    SDP_UINT_8BIT(SPP_SCO_CHNL_NUM_START)   /* Uint8 RFCOMM channel number - value can vary */
};

U8 SppSCOServiceNamePattern[] = 
{
    SDP_TEXT_8BIT(22),
    'S', 'P', 'P', ' ', /* Text String */
    'w', 'i', 't', 'h', ' ', 'S', 'C',
    'o', 'C', 'a', 'p', 'a', 'b',
    'i', 'l', 'i', 't', 'y'
};

/* Attribute list for LAN */
SdpAttribute SppSCOSdpAttribsPattern[] = 
{
    /* Service Class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, sppSCOServiceClass),
    /* Protocol Descriptor List attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SppSCOProtocolDescriptorListPattern),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, publicBrowseGroup),
    /* Language Base ID List attribute */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, languageBase),
    /* Service Availability attribute */
    SDP_ATTRIBUTE(AID_SERVICE_AVAILABILITY, sppSCOServiceAvailability),
    /* Serial Port Profile Service Name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SppSCOServiceNamePattern),
    /* Service Description attribute */
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), sppSCOServiceDescription),
    /* Additional Pro attribute */
    SDP_ATTRIBUTE(AID_SUPPORTED_CAPABILITIES, SppSCOProtocolDescriptorListPattern)
};

#define SppSCOProtocolDescriptorListPattern_Size 14
#define SppSCOSdpAttribsPattern_Size 8
#define SppSCOServiceNamePattern_Size 23
#endif /* (NUM_SPP_SCO_SRVS >= 1) */

#if (NUM_SPP_SCO_SRVS > 1)
/* LAN service records glboal context */
U8 SppSCOProtocolDescriptorListArray[NUM_SPP_SCO_SRVS - 1][SerialPortProtocolDescriptorListPattern_Size];
U8 SppSCOServiceNameArray[NUM_SPP_SCO_SRVS - 1][SppSCOServiceNamePattern_Size];
SdpAttribute SppSCOSdpAttribsArray[NUM_SPP_SCO_SRVS - 1][SppSCOSdpAttribsPattern_Size];
#endif /* (NUM_SPP_SCO_SRVS > 1) */

#endif /* SPP_SCO_SERVER == XA_ENABLED */

#endif /* SPP_SERVER == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  SppSdpDB_InitRecordsArray
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SppSdpDB_InitRecordsArray()
{

#if SPP_SERVER == XA_ENABLED

#if ((NUM_SPP_SRVS >1) || ((SPP_SCO_SERVER == XA_ENABLED) && (NUM_SPP_SCO_SRVS >1)))
    int i = 0;
#endif 

    //For SPP Server recoreds, update all array's data
    //if NUM_SPP_SRVS =1, the pattern is used for SPP server    
#if (NUM_SPP_SRVS >1)

    for (i = 0; i < (NUM_SPP_SRVS - 1); i++)
    {
        OS_MemCopy(
            SerialPortProtocolDescriptorListArray[i],
            SerialPortProtocolDescriptorListPattern,
            SerialPortProtocolDescriptorListPattern_Size);
        //Specify the server channel number for this SPP server record (server_chnl_num is from 1 to NUM_SPP_SRVS-1)
        //Because the patter uses SPP_CHNL_NUM_START, so next record uses from SPP_CHNL_NUM_START+1
        SerialPortProtocolDescriptorListArray[i][SerialPortProtocolDescriptorListPattern_Size - 1] =
            SPP_CHNL_NUM_START + 1 + i;
        /* Specify the name for this SPP server record */
        OS_MemCopy(
            (U8*) SerialPortServiceNameArray[i],
            (U8*) SerialPortServiceNamePattern,
            SerialPortServiceNamePattern_Size);
        SerialPortServiceNameArray[i][SerialPortServiceNamePattern_Size - 1] = '1' + i;
        /* Sepcify the attributes for this SPP server record */
        OS_MemCopy(
            (U8*) (&SerialPortSdpAttribsArray[i][0]),
            (U8*) (&SerialPortSdpAttribsPattern[0]),
            sizeof(SerialPortSdpAttribsPattern));
        (SerialPortSdpAttribsArray[i][1]).value = SerialPortProtocolDescriptorListArray[i];
        (SerialPortSdpAttribsArray[i][4]).value = SerialPortServiceNameArray[i];
    }
#endif /* (NUM_SPP_SRVS >1) */

#if SPP_SCO_SERVER == XA_ENABLED
    //For LAN Server records, update all array's data
    //if NUM_SPP_SCO_SRVS =1, the pattern is used for SPP server
#if (NUM_SPP_SCO_SRVS >1)

    for (i = 0; i < (NUM_SPP_SCO_SRVS - 1); i++)
    {
        OS_MemCopy(
            LanProtocolDescriptorArray[i],
            SppSCOProtocolDescriptorListPattern,
            SppSCOProtocolDescriptorListPattern_Size);
        /* Specify the server channel number for this LAN server record (server_chnl_num is from 1 to NUM_SPP_SCO_SRVS-1) */
        LanProtocolDescriptorArray[i][SppSCOProtocolDescriptorListPattern_Size - 1] = SPP_SCO_CHNL_NUM_START + 1 + i;
        /* Specify the name for this LAN server record */
        OS_MemCopy((U8*) SppSCOServiceNameArray[i], (U8*) SppSCOServiceNamePattern, SppSCOServiceNamePattern_Size);
        SppSCOServiceNameArray[i][SppSCOServiceNamePattern_Size - 1] = '1' + i;
        /* Specify the attributes for this LAN server record */
        OS_MemCopy(&SppSCOSdpAttribsArray[i][0]), (U8*) (&SppSCOSdpAttribsPattern[0]),
            sizeof(SppSCOSdpAttribsPattern));
        (SppSCOSdpAttribsArray[i][1]).value = SppSCOProtocolDescriptorListArray[i];
        (SppSCOSdpAttribsArray[i][6]).value = SppSCOServiceNameArray[i];
    }
#endif /* (NUM_SPP_SCO_SRVS >1) */
#endif /* SPP_SCO_SERVER == XA_ENABLED */

#endif /* SPP_SERVER == XA_ENABLED */

}


/*****************************************************************************
 * FUNCTION
 *  SppSdpDB_GetServiceName
 * DESCRIPTION
 *  
 * PARAMETERS
 *  i       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 *SppSdpDB_GetServiceName(int i)
{
#if (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0)
    if ((i >= SC_SERIAL_PORT0) && (i <= (SC_SERIAL_PORT0 + NUM_SPP_SRVS - 1)))
    {
        /* use the NamePattern directly */
        if (i == SC_SERIAL_PORT0)
        {
            return (U8*) SerialPortServiceNamePattern;
        }

    #if (NUM_SPP_SRVS > 1)
        else
            /* NameArray[] is used to store "1 ~ (NUM_SPP_SRVS-1)" from inx=0 to (NUM_SPP_SRVS-2) */
        {
            return SerialPortServiceNameArray[i - SC_SERIAL_PORT0 - 1];
        }
    #endif /* (NUM_SPP_SRVS > 1) */

    }
    else
#endif /* (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0) */

#if SPP_SCO_SERVER == XA_ENABLED
    if ((i >= SC_SPP_SCO0) && (i <= (SC_SPP_SCO0 + NUM_SPP_SCO_SRVS - 1)))
    {
        /* use the NamePattern directly */
        if (i == SC_SPP_SCO0)
        {
            return (U8*) SppSCOServiceNamePattern;
        }
    #if (NUM_SPP_SCO_SRVS > 1)
        else
            /* NameArray[] is used to store "1 ~ (NUM_SPP_SCO_SRVS-1)" from inx=0 to (NUM_SPP_SCO_SRVS-2) */
        {
            return SppSCOServiceNameArray[i - SC_SPP_SCO0 - 1];
        }
    #endif /* (NUM_SPP_SCO_SRVS > 1) */

    }
    else
#endif /* SPP_SCO_SERVER == XA_ENABLED */
    {
    }
    return NULL;
}

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  SppSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the service record attributes pointer and attribute number of
 *  SPP, LAP, DUN, FAX Service
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_DIALUP_NETWORKING, SC_FAX, SC_LAN_ACCESS_PPP, SC_SERIAL_PORT
 *  attribute           [OUT]       
 *  srv_chn             [OUT]       The RFCOMM service channel of the service
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 SppSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn)
{
    U8 attribute_num = 0;

#if DUN_SERVER == XA_ENABLED
    if (service_type == SC_DIALUP_NETWORKING)
    {
        *attribute = (SdpAttribute*) & dialupSdpAttribs;
        attribute_num = sizeof(dialupSdpAttribs) / sizeof(*(dialupSdpAttribs));
        /* Retrieve the server channel from the service channel */
        *srv_chn = (U8) dialupProtocolDescriptor[sizeof(dialupProtocolDescriptor) - 1];

    }
    else
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED
    if (service_type == SC_FAX)
    {
        *attribute = (SdpAttribute*) & faxSdpAttribs;
        attribute_num = sizeof(faxSdpAttribs) / sizeof(*(faxSdpAttribs));
        /* Retrieve the server channel from the service channel */
        *srv_chn = (U8) faxProtocolDescriptor[sizeof(faxProtocolDescriptor) - 1];

    }
    else
#endif /* FAX_SERVER == XA_ENABLED */

#if (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0)
    if ((service_type >= SC_SERIAL_PORT0) && (service_type <= (SC_SERIAL_PORT0 + NUM_SPP_SRVS - 1)))
    {
        if (service_type == SC_SERIAL_PORT0)
        {
            *attribute = (SdpAttribute*) SerialPortSdpAttribsPattern;
            attribute_num = sizeof(SerialPortSdpAttribsPattern) / sizeof(SerialPortSdpAttribsPattern[0]);
            *srv_chn = SerialPortProtocolDescriptorListPattern[SerialPortProtocolDescriptorListPattern_Size - 1];
        }
    #if (NUM_SPP_SRVS > 1)
        else
        {
            kal_uint8 inx = service_type - SC_SERIAL_PORT0 - 1;

            *attribute = (SdpAttribute*) SerialPortSdpAttribsArray[inx];
            attribute_num = sizeof(SerialPortSdpAttribsPattern) / sizeof(SerialPortSdpAttribsPattern[0]);
            *srv_chn =
                (U8) SerialPortProtocolDescriptorListArray[inx][SerialPortProtocolDescriptorListPattern_Size - 1];
        }
    #endif /* (NUM_SPP_SRVS > 1) */

    }
    else
#endif /* (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0) */

#if SPP_SCO_SERVER == XA_ENABLED
    if ((service_type >= SC_SPP_SCO0) && (service_type <= (SC_SPP_SCO0 + NUM_SPP_SCO_SRVS - 1)))
    {
        if (service_type == SC_SPP_SCO0)
        {
            *attribute = (SdpAttribute*) SppSCOSdpAttribsPattern;
            attribute_num = sizeof(SppSCOSdpAttribsPattern) / sizeof(SppSCOSdpAttribsPattern[0]);
            *srv_chn = SppSCOProtocolDescriptorListPattern[SppSCOProtocolDescriptorListPattern_Size - 1];
            Report(("SCO server channel:%02x", *srv_chn));
        }

    #if (NUM_SPP_SCO_SRVS > 1)
        else
        {
            kal_uint8 inx = service_type - SC_SPP_SCO0 - 1;

            *attribute = (SdpAttribute*) SerialPortSdpAttribsArray[inx];
            attribute_num = sizeof(SppSCOSdpAttribsPattern) / sizeof(SppSCOSdpAttribsPattern[0]);
            *srv_chn = (U8) SppSCOProtocolDescriptorListArray[inx][SppSCOProtocolDescriptorListPattern_Size - 1];
        }
    #endif /* (NUM_SPP_SCO_SRVS > 1) */

    }
    else
#endif /* SPP_SCO_SERVER == XA_ENABLED */
    {
        Assert(0);
    }

    /* Return the total number of attribute number */
    return attribute_num;
}


U8 SppSdpDB_SetNGetCustomServiceName(int i, U8 **service_name, char *svcCustomName)
{
#if (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0)

    U8 customSerialPortServiceName[SPP_MAX_SERVICE_NAME_LENGTH];

    customSerialPortServiceName[0] = DETD_TEXT + DESD_ADD_8BITS;
    customSerialPortServiceName[1] = (U8)(OS_StrLen(svcCustomName));

    OS_MemCopy(&customSerialPortServiceName[2], (const U8*)svcCustomName, OS_StrLen(svcCustomName));

    /* Specify the name for this SPP server record */
    OS_MemCopy(
		(U8*) SerialPortServiceCustomNameArray[i],
		(U8*) customSerialPortServiceName,
		SPP_MAX_SERVICE_NAME_LENGTH);

    *service_name = (U8 *)&SerialPortServiceCustomNameArray[i];

    return (U8) OS_StrLen((const char*)&SerialPortServiceCustomNameArray[i]);
#endif /* (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0) */
}


U8 SppSdpDB_SetNGetCustomAttribute(I16 i, SdpAttribute **attribute, U8 *srv_chn, U16 customUUID)
{
#if (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0)
    U8 attribute_num = 0;
    SdpAttribute customAttrIdList; 
    SdpAttribute customAttrName; 

    // 1. Update customSPPServiceClassIdList first
    customSPPServiceClassIdList[i][0] = DETD_SEQ + DESD_ADD_8BITS;
    customSPPServiceClassIdList[i][1] = 3;
    customSPPServiceClassIdList[i][2] = DETD_UUID + DESD_2BYTES;
    customSPPServiceClassIdList[i][3] = (U8)(((customUUID) & 0xff00) >> 8);
    customSPPServiceClassIdList[i][4] = (U8)((customUUID) & 0x00ff);
    // 2. Create a new attribute list
    customAttrIdList.id = AID_SERVICE_CLASS_ID_LIST;
    customAttrIdList.len = sizeof(customSPPServiceClassIdList);
    customAttrIdList.value = (U8 *)customSPPServiceClassIdList;
    customAttrIdList.flags = 0x0000;

    // 3. Create a new name attribute
    customAttrName.id = (AID_SERVICE_NAME + 0x0100);
    customAttrName.len = (U16)OS_StrLen((char *)&SerialPortServiceCustomNameArray[i]);
    customAttrName.value = (const U8 *)&SerialPortServiceCustomNameArray[i];
    customAttrName.flags = 0x0000;

    if (0 == i)
    {
	 // Update SerialPortSdpAttribsPattern the the new one
	 OS_MemCopy((U8 *)&SerialPortSdpAttribsPattern[0], (U8 *)&customAttrIdList, sizeof(SdpAttribute));
	 OS_MemCopy((U8 *)&SerialPortSdpAttribsPattern[4], (U8 *)&customAttrName, sizeof(SdpAttribute));
    
        *attribute = (SdpAttribute*) SerialPortSdpAttribsPattern;
        attribute_num = sizeof(SerialPortSdpAttribsPattern) / sizeof(SerialPortSdpAttribsPattern[0]);
        *srv_chn = SerialPortProtocolDescriptorListPattern[SerialPortProtocolDescriptorListPattern_Size - 1];
    }
    else if (0 < i)
    {
	 // Update SerialPortSdpAttribsPattern
	 OS_MemCopy((U8 *)&SerialPortSdpAttribsArray[(i-1)][0], (U8 *)&customAttrIdList, sizeof(SdpAttribute));
	 OS_MemCopy((U8 *)&SerialPortSdpAttribsArray[(i-1)][4], (U8 *)&customAttrName, sizeof(SdpAttribute));

        *attribute = (SdpAttribute*) SerialPortSdpAttribsArray[(i-1)];
        attribute_num = sizeof(SerialPortSdpAttribsPattern) / sizeof(SerialPortSdpAttribsPattern[0]);
        *srv_chn =
            (U8) SerialPortProtocolDescriptorListArray[(i-1)][SerialPortProtocolDescriptorListPattern_Size - 1];
    }

    /* Return the total number of attribute number */
    return attribute_num;
    
#endif /* (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0) */    
}

#endif /* __BT_SPP_PROFILE__ */
