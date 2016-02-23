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
 * avrcpsdpdb.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions and global variable to the AVRCP Service Records. 
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
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * 09 01 2010 sh.lai
 * NULL
 * Integration change. into 1036OF
 *
 * Jun 27 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 11 2007 mtk01411
 * [MAUI_00391541] [Bluetooth BQB PTS2.1.1.0 Testing] Patch for IOPT test case TC_SDAS_BV_03_I
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

/*****************************************************************************
 *
 * File:
 *     $Workfile:avrcpsdp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains code for the AVRCP profile. 
 *
 * Created:     May 19, 2004
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#ifdef __BT_AVRCP_PROFILE__

#include "sdp.h"
#include "avrcp.h"

/********
Constants 
*********/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/
const U8 BtBaseUUID[] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};

/****************************************************************************
 *
 * SDP objects registered by AVRCP Targets.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP Service Class ID.
 */
const U8 AvrcpCtServiceClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* DES  */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL)    /* UUID */
};

/*---------------------------------------------------------------------------
 * SDP Service Class ID.
 */
const U8 AvrcpTgServiceClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),                  /* DES  */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL_TARGET) /* UUID */
};

/*---------------------------------------------------------------------------
 * SDP Bluetooth Profile Descriptor List.
 */
#if 0
const U8 AvrcpBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES #0, 6 bytes */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL),   /* Uuid16          */
#ifdef __BT_AVRCP_V14__
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
    SDP_UINT_16BIT(0x0104)                  /* Uint16 version */
#else    
    SDP_UINT_16BIT(0x0103)                  /* Uint16 version */
#endif
#else    
#ifdef __BT_AVRCP_V13__
    SDP_UINT_16BIT(0x0103)                  /* Uint16 version  */	
#else
    SDP_UINT_16BIT(0x0100)                  /* Uint16 version  */
#endif    
#endif  
};
#endif

const U8 AvrcpCtBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES #0, 6 bytes */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL),   /* Uuid16          */
    SDP_UINT_16BIT(0x0100)                  /* Uint16 version  */
};


const U8 AvrcpTg10BtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES #0, 6 bytes */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL),   /* Uuid16          */
    SDP_UINT_16BIT(0x0100)                  /* Uint16 version  */
};

const U8 AvrcpTg13BtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES #0, 6 bytes */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL),   /* Uuid16          */
    SDP_UINT_16BIT(0x0103)                  /* Uint16 version */
};

const U8 AvrcpTg14BtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES #0, 6 bytes */
    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL),   /* Uuid16          */
    SDP_UINT_16BIT(0x0104)                  /* Uint16 version */
};


/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List.
 * 
 * Value of the protocol descriptor list for the AVRCP Service.
 * This structure is a ROM'able representation of the RAM structure.
 * During AVRCP_Init, this structure is copied into the RAM structure
 * and used to register. A RAM structure is used to enable dynamic
 * setting of certain values.
 */
const U8 AvrcpTgProtoDescList[] = 
{
    /* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(16),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.
     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_L2CAP),     /* Uuid16         */
    SDP_UINT_16BIT(BT_PSM_AVCTP),   /* PSM            */

    /*
     * The next protocol descriptor is for AVCTP. It contains 2 elements
     * UUID and Version.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_AVCTP), /* Uuid16         */
    /* always declear 1.3 version AVCTP. 1.0 & 1.3 is the same. No QDID for AVCTP 1.3 yet. */
    SDP_UINT_16BIT(0x0100),     /* Uint16 version */
};

const U8 AvrcpCtProtoDescList[] = 
{
    /* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(16),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.
     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_L2CAP),     /* Uuid16         */
    SDP_UINT_16BIT(BT_PSM_AVCTP),   /* PSM            */

    /*
     * The next protocol descriptor is for AVCTP. It contains 2 elements
     * UUID and Version.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_AVCTP), /* Uuid16         */
    /* always declear 1.3 version AVCTP. 1.0 & 1.3 is the same. No QDID for AVCTP 1.3 yet. */
    SDP_UINT_16BIT(0x0100),     /* Uint16 version */    
};

const U8 AvrcpProtoDescList[] = 
{
    /* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(16),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.
     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_L2CAP),     /* Uuid16         */
    SDP_UINT_16BIT(BT_PSM_AVCTP),   /* PSM            */

    /*
     * The next protocol descriptor is for AVCTP. It contains 2 elements
     * UUID and Version.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_AVCTP), /* Uuid16         */
    SDP_UINT_16BIT(0x0100),     /* Uint16 version */
};

/*---------------------------------------------------------------------------
 * Public Browse Group.
 */
const U8 AvrcpBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP), /* Public Browse Group */
};

const U8 AvrcpSupportedFeatures[] = 
{
    SDP_UINT_16BIT(0x0F),   /* Features are set when registering service */
};

const U8 AvrcpCtSupportedFeatures[] = 
{
    SDP_UINT_16BIT(0x0F),   /* Features are set when registering service */
};

const U8 AvrcpTg10SupportedFeatures[] = 
{
    SDP_UINT_16BIT(0x01),   /* Features are set when registering service */
};

const U8 AvrcpTg13SupportedFeatures[] = 
{
	SDP_UINT_16BIT(0x21),   /* Features are set when registering service */
};

const U8 AvrcpTg14SupportedFeatures[] = 
{
    SDP_UINT_16BIT(0x21),   /* Features are set when registering service */
};

/*---------------------------------------------------------------------------
 * SDP Additional Protocol Descriptor List.
 * 
 * Value of the Addition protocol descriptor list for the AVRCP Service.
 * This structure is a ROM'able representation of the RAM structure.
 * During AVRCP_Init, this structure is copied into the RAM structure
 * and used to register. A RAM structure is used to enable dynamic
 * setting of certain values.
 */
const U8 AvrcpTgAddProtoDescList[] = 
{
    /* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(18),

    /* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(16),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element and a PSM.
     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_L2CAP),     /* Uuid16         */
    SDP_UINT_16BIT(BT_PSM_AVCTP_BROWSING),   /* PSM            */

    /*
     * The next protocol descriptor is for AVCTP. It contains 2 elements
     * UUID and Version.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES, 6 bytes   */
    SDP_UUID_16BIT(PROT_AVCTP), /* Uuid16         */
    SDP_UINT_16BIT(0x0100),     /* Uint16 version. No QDID for AVCTP 1.3 yet. */
};


/******************************
Definitions Of Exported Globals 
*******************************/

/*---------------------------------------------------------------------------
 * AVRCP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SDP record. The number of elements of this array is defined
 * by AVRCP_NUM_ATTRIBUTES.
 */
SdpAttribute AvrcpCt10SdpAttributes[] = 
{
    /* Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AvrcpCtServiceClassId),    /* AvrcpCtServiceClassId */
    /* Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AvrcpCtProtoDescList),  /* AvrcpProtoDescList */
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AvrcpBrowseGroup), /* AvrcpBrowseGroup */
    /* Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AvrcpCtBtProfDescList),       /* AvrcpBtProfDescList */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, AvrcpCtSupportedFeatures)
};

SdpAttribute AvrcpTg10SdpAttributes[] = 
{
    /* Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AvrcpTgServiceClassId),    /* AvrcpCtServiceClassId */
    /* Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AvrcpTgProtoDescList),  /* AvrcpProtoDescList */
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AvrcpBrowseGroup), /* AvrcpBrowseGroup */
    /* Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AvrcpTg10BtProfDescList),       /* AvrcpBtProfDescList */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, AvrcpTg10SupportedFeatures)
};

/*---------------------------------------------------------------------------
 * AVRCP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SDP record. The number of elements of this array is defined
 * by AVRCP_NUM_ATTRIBUTES.
 */
SdpAttribute AvrcpCt13SdpAttributes[] = 
{
    /* Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AvrcpCtServiceClassId),    /* AvrcpCtServiceClassId */
    /* Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AvrcpCtProtoDescList),  /* AvrcpProtoDescList */
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AvrcpBrowseGroup), /* AvrcpBrowseGroup */
    /* Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AvrcpCtBtProfDescList),       /* AvrcpBtProfDescList */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, AvrcpCtSupportedFeatures)
};

SdpAttribute AvrcpTg13SdpAttributes[] = 
{
    /* Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AvrcpTgServiceClassId),    /* AvrcpCtServiceClassId */
    /* Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AvrcpTgProtoDescList),  /* AvrcpProtoDescList */
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AvrcpBrowseGroup), /* AvrcpBrowseGroup */
    /* AV13 Addition Protocol Descriptor Service */
    SDP_ATTRIBUTE(AID_ADDITIONAL_PROT_DESC_LISTS, AvrcpTgAddProtoDescList), /* Avrcp Browse channel */
    /* Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AvrcpTg13BtProfDescList),       /* AvrcpBtProfDescList */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, AvrcpTg13SupportedFeatures)
};

/*---------------------------------------------------------------------------
 * AVRCP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SDP record. The number of elements of this array is defined
 * by AVRCP_NUM_ATTRIBUTES.
 */
SdpAttribute AvrcpCt14SdpAttributes[] = 
{
    /* Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AvrcpCtServiceClassId),    /* AvrcpCtServiceClassId */
    /* Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AvrcpCtProtoDescList),  /* AvrcpProtoDescList */
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AvrcpBrowseGroup), /* AvrcpBrowseGroup */
    /* Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AvrcpCtBtProfDescList),       /* AvrcpBtProfDescList */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, AvrcpCtSupportedFeatures)
};

SdpAttribute AvrcpTg14SdpAttributes[] = 
{
    /* Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AvrcpTgServiceClassId),    /* AvrcpCtServiceClassId */
    /* Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AvrcpTgProtoDescList),  /* AvrcpProtoDescList */
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AvrcpBrowseGroup), /* AvrcpBrowseGroup */
    /* AV14 Addition Protocol Descriptor Service */
    SDP_ATTRIBUTE(AID_ADDITIONAL_PROT_DESC_LISTS, AvrcpTgAddProtoDescList), /* Avrcp Browse channel */
    /* Profile Descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AvrcpTg14BtProfDescList),       /* AvrcpBtProfDescList */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, AvrcpTg14SupportedFeatures)
};


/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  AvrcpSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the service record attributes pointer and attribute number of AVRCP
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as AVRCP Controller or AVRCP Target.
 *  attribute           [IN]        
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 AvrcpSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *version)
{
    U8 attribute_num = 0;
    U8 l_version = 0;
    if( 0 == version ){
        l_version = 0;
    }else{
        l_version = *version;
    }

    switch(l_version){
        case 0: 
        case 10: /* are avrcp 1.0 */
            Report(("AVRCP: waring should use 0x10 BCD as version. change 10 to 0x10"));
        case 0x10:
        default:
            switch (service_type)
            {
                /* Get the AVRCP TARGET records */
            case SC_AV_REMOTE_CONTROL_TARGET:
                *attribute = (SdpAttribute*) & AvrcpTg10SdpAttributes;
                attribute_num = sizeof(AvrcpTg10SdpAttributes) / sizeof(*(AvrcpTg10SdpAttributes));
                OS_Report("AVRCP sdp 1.0 target role");
                break;
                /* Get the AVRCP CONTROLLER records */
            case SC_AV_REMOTE_CONTROL:
                *attribute = (SdpAttribute*) & AvrcpCt10SdpAttributes;
                attribute_num = sizeof(AvrcpCt10SdpAttributes) / sizeof(*(AvrcpCt10SdpAttributes));
                OS_Report("AVRCP sdp 1.0 control role");                
                break;
            }
            break;
        
        case 13: /* avrcp 1.3 */
            Report(("AVRCP: waring should use 0x13 BCD as version. change 13 to 0x13"));
        case 0x13:            
            switch (service_type)
            {
                    /* Get the AVRCP TARGET records */
                case SC_AV_REMOTE_CONTROL_TARGET:
                    *attribute = (SdpAttribute*) & AvrcpTg13SdpAttributes;
                    attribute_num = sizeof(AvrcpTg13SdpAttributes) / sizeof(*(AvrcpTg13SdpAttributes));
                    OS_Report("AVRCP sdp 1.3 target role");                    
                    break;
                    /* Get the AVRCP CONTROLLER records */
                case SC_AV_REMOTE_CONTROL:
                    *attribute = (SdpAttribute*) & AvrcpCt10SdpAttributes;
                    attribute_num = sizeof(AvrcpCt13SdpAttributes) / sizeof(*(AvrcpCt13SdpAttributes));
                    OS_Report("AVRCP sdp 1.3 control role");
                    break;
            }

            break;
        case 0x14:
        case 14: /* avrcp 1.4 */
#ifdef __BT_AVRCP_V14__
            switch (service_type)
            {
                    /* Get the AVRCP TARGET records */
                case SC_AV_REMOTE_CONTROL_TARGET:
                    *attribute = (SdpAttribute*) & AvrcpTg14SdpAttributes;
                    attribute_num = sizeof(AvrcpTg14SdpAttributes) / sizeof(*(AvrcpTg14SdpAttributes));
                    OS_Report("AVRCP sdp 1.4 target role");                
                    break;
                    /* Get the AVRCP CONTROLLER records */
                case SC_AV_REMOTE_CONTROL:
                    *attribute = (SdpAttribute*) & AvrcpCt14SdpAttributes;
                    attribute_num = sizeof(AvrcpCt14SdpAttributes) / sizeof(*(AvrcpCt14SdpAttributes));
                    OS_Report("AVRCP sdp 1.4 control role");
                    break;
            }          
#else
            switch (service_type)
            {
                    /* Get the AVRCP TARGET records */
                case SC_AV_REMOTE_CONTROL_TARGET:
                    *attribute = (SdpAttribute*) & AvrcpTg13SdpAttributes;
                    attribute_num = sizeof(AvrcpTg13SdpAttributes) / sizeof(*(AvrcpTg13SdpAttributes));
                    OS_Report("AVRCP sdp 1.3 target role");                    
                    break;
                    /* Get the AVRCP CONTROLLER records */
                case SC_AV_REMOTE_CONTROL:
                    *attribute = (SdpAttribute*) & AvrcpCt10SdpAttributes;
                    attribute_num = sizeof(AvrcpCt13SdpAttributes) / sizeof(*(AvrcpCt13SdpAttributes));
                    OS_Report("AVRCP sdp 1.3 control role");
                    break;
            }            
#endif            
            break;
    }


    // default is AVRCP 1.0 
    if( 0 == attribute_num ){
        switch (service_type)
            {
            /* Get the AVRCP TARGET records */
        case SC_AV_REMOTE_CONTROL_TARGET:
            *attribute = (SdpAttribute*) & AvrcpTg10SdpAttributes;
            attribute_num = sizeof(AvrcpTg10SdpAttributes) / sizeof(*(AvrcpTg10SdpAttributes));
            OS_Report("AVRCP sdp 1.0 target role");
            break;
            /* Get the AVRCP CONTROLLER records */
        case SC_AV_REMOTE_CONTROL:
            *attribute = (SdpAttribute*) & AvrcpCt10SdpAttributes;
            attribute_num = sizeof(AvrcpCt10SdpAttributes) / sizeof(*(AvrcpCt10SdpAttributes));
            OS_Report("AVRCP sdp 1.0 control role");                
            break;
        }

    }
    /* Return the total number of attribute number */
    return attribute_num;
}
#endif /* __BT_AVRCP_PROFILE__ */
