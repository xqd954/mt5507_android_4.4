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
 * hfagsdpdb.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions and global variable to the Handsfree AG Service Records. 
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
 * Apr 3 2009 mtk00560
 * [MAUI_01661027] [HFP] remove audio_via_sco compile option from btprofile lib
 * 
 *
 * Jun 27 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfg_sdp.c$ for iAnywhere Blue SDK, Version 2.1.1
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

#include "sdp.h"
#include "bt_common_config.h"

/********
Constants 
*********/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/****************************************************************************
 * Hands-Free Gateway SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList 
 */
const U8 HfClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),  /*
                                 * Data Element Sequence, 
                                 * 6 bytes 
                                 */
    SDP_UUID_16BIT(SC_HANDSFREE), /*
                                                 * Hands-Free UUID in 
                                                 * Big Endian 
                                                 */
    SDP_UUID_16BIT(SC_GENERIC_AUDIO)    /*
                                         * Generic Audio UUID in Big
                                         * Endian 
                                         */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by Hands-Free Audio Gateway.
 * 
 * Value of the protocol descriptor list for the Hands-Free Audio Gateway 
 * profile.  This structure is a ROM'able representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service. A RAM structure must be used to be able to dynamically set the 
 * RFCOMM channel in the RFCOMM protocol descriptor.
 */
const U8 HfProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12), /* Data element sequence, 12 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP */

    /*
     * Next protocol descriptor in the list is RFCOMM. It contains two
     * elements which are the UUID and the channel. Ultimately this
     * channel will need to filled in with value returned by RFCOMM.
     */
    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM */
    // SDP_UINT_8BIT(HF_GW_SRV_CHN) /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(HF_GW_CHNL_NUM_START)
};

/*
 * BluetoothProfileDescriptorList
 */
const U8 HfProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_HANDSFREE),       /* Uuid16 Hands-Free */
    SDP_UINT_16BIT(0x0105)  /* Uint16 version number */
};

/*
 * * OPTIONAL * Language BaseId List (goes with the service name).
 */
const U8 HfLangBaseIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* English Language */
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */
};

/*
 * * OPTIONAL *  ServiceName
 */
const U8 HfServiceName[] = 
{
    SDP_TEXT_8BIT(16),  /* Null terminated text string */
    'H', 'a', 'n', 'd', 's', '-', 'F', 'r', 'e', 'e', ' ', 'u',
    'n', 'i', 't','\0'
};

/*
 * * OPTIONAL *  Supported Features
 */
U8 HfSupportedFeatures[3];

/*---------------------------------------------------------------------------
 * Hands-Free Public Browse Group.
 */
const U8 HfBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/******************************
Definitions Of Exported Globals 
*******************************/

/*
 * Hands-Free attributes.
 * 
 * This is a ROM template for the RAM structure used to register the
 * Hands-Free SDP record.
 */
SdpAttribute HfSdpAttributes[7];

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  HfpagSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the Handsfree Audio Gateway service record attributes pointer,
 *  attribute number and the service channel in the service record
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_HANDSFREE_AUDIO_GATEWAY.
 *  attribute           [IN]        
 *  srv_chn             [IN]        the RFCOMM service channel of the service
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 HfphfSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn)
{

    	U8 attribute_num = 0;

   	HfSupportedFeatures[0] = (U8)DETD_UINT + DESD_2BYTES;
	HfSupportedFeatures[1] = 0;
	HfSupportedFeatures[2] = 0x1F;

	HfSdpAttributes[0].id = AID_SERVICE_CLASS_ID_LIST;
	HfSdpAttributes[0].len = sizeof(HfClassId);
	HfSdpAttributes[0].value = (const U8*) (HfClassId);	
	HfSdpAttributes[0].flags = 0x0000;		

	HfSdpAttributes[1].id = AID_PROTOCOL_DESC_LIST;
	HfSdpAttributes[1].len = sizeof(HfProtoDescList);
	HfSdpAttributes[1].value = (const U8*) (HfProtoDescList);	
	HfSdpAttributes[1].flags = 0x0000;		

	HfSdpAttributes[2].id = AID_BROWSE_GROUP_LIST;
	HfSdpAttributes[2].len = sizeof(HfBrowseGroup);
	HfSdpAttributes[2].value = (const U8*) (HfBrowseGroup);	
	HfSdpAttributes[2].flags = 0x0000;		

	HfSdpAttributes[3].id = AID_LANG_BASE_ID_LIST;
	HfSdpAttributes[3].len = sizeof(HfLangBaseIdList);
	HfSdpAttributes[3].value = (const U8*) (HfLangBaseIdList);	
	HfSdpAttributes[3].flags = 0x0000;		

	HfSdpAttributes[4].id = AID_BT_PROFILE_DESC_LIST;
	HfSdpAttributes[4].len = sizeof(HfProfileDescList);
	HfSdpAttributes[4].value = (const U8*) (HfProfileDescList);	
	HfSdpAttributes[4].flags = 0x0000;		

	HfSdpAttributes[5].id = (AID_SERVICE_NAME + 0x0100);
	HfSdpAttributes[5].len = sizeof(HfServiceName);
	HfSdpAttributes[5].value = (const U8*) (HfServiceName);	
	HfSdpAttributes[5].flags = 0x0000;		

	HfSdpAttributes[6].id = AID_SUPPORTED_FEATURES;
	HfSdpAttributes[6].len = sizeof(HfSupportedFeatures);
	HfSdpAttributes[6].value = (const U8*) (HfSupportedFeatures);	
	HfSdpAttributes[6].flags = 0x0000;

    switch (service_type)
    {
        case SC_HANDSFREE:
            *attribute = (SdpAttribute*) & HfSdpAttributes;
            attribute_num = sizeof(HfSdpAttributes) / sizeof(*(HfSdpAttributes));
            *srv_chn = (U8) HfProtoDescList[sizeof(HfProtoDescList) - 1];
            break;
    }
    return attribute_num;
}

