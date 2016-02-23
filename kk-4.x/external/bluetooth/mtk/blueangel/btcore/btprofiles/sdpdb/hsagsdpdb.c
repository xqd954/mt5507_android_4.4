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
 * hsagsdpdb.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions and global variable to the HeadSet AG Service Records. 
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
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
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
 *     $Workfile:headset.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the headset profile core.
 *             
 * Created:     September 12, 2000
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

#include "sdp.h"
#include "hs_gw.h"

/********
Constants 
*********/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object.
 * 
 * Value of the protocol descriptor list for the Headset Audio Gateway profile.
 * This structure is a ROM'able representation of the RAM structure.
 * During AG_Register, this structure is copied into the RAM structure
 * used to register the server. A RAM structure must be used to be able
 * to dynamically set the RFCOMM channel in the RFCOMM protocol descriptor.
 */
const U8 AgProtoDescList[] = 
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
    // SDP_UINT_8BIT(HS_AG_SRV_CHN) /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(HS_AG_CHNL_NUM_START)
};

/*
 * BluetoothProfileDescriptorList
 */
const U8 AgProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_HEADSET), /* Uuid16 Hands-Free */
    SDP_UINT_16BIT(0x0102)      /* Uint16 version number */
};

/*---------------------------------------------------------------------------
 *
 * SDP objects registered by Headset Audio Gateway.
 * They are the Service Class ID and the Protocol Descriptor List
 */
const U8 AgClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),                  /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(SC_HEADSET_AUDIO_GATEWAY),   /* Headset Audio Gateway UUID in Big Endian */
    SDP_UUID_16BIT(SC_GENERIC_AUDIO)            /* Generic Audio UUID in Big Endian */
};

/*
 * * OPTIONAL * Language BaseId List (goes with the service name).
 */
const U8 AgLangBaseIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* English Language */
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */
};

/*
 * * OPTIONAL * Audio Gateway service name (digianswer requires it).
 */
const U8 AgServiceName[] = 
{
    SDP_TEXT_8BIT(22),  /* Null terminated text string, 13 bytes */
    'H', 'e', 'a', 'd', 's', 'e', 't', ' ',
    'A', 'u', 'd', 'i', 'o', ' ',
    'G', 'a', 't', 'e', 'w', 'a', 'y', '\0'
};

/*---------------------------------------------------------------------------
 * Audio Gateway Public Browse Group.
 */
const U8 AgBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/******************************
Definitions Of Exported Globals 
*******************************/

/*
 * Headset Audio Gateway's attributes.
 * 
 * This is a ROM template for the RAM structure used to register the
 * Headset Audio Gateway's SDP record.
 */
SdpAttribute HspAgSdpAttributes[] = 
{
    /* Headset audio gateway class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AgClassId),
    /* Audio Gateway protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AgProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AgBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, AgLangBaseIdList),
    /* Audio Gateway profile descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, AgProfileDescList),
    /* Audio Gateway service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), AgServiceName),
};

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  HspSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the Headset Audio Gateway service record attributes pointer,
 *  attribute number and the service channel in the service record
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_HEADSET_AUDIO_GATEWAY.
 *  attribute           [IN]        
 *  srv_chn             [OUT]       The RFCOMM service channel of the service
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 HspSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn)
{
    U8 attribute_num = 0;

    switch (service_type)
    {
        /* Get the service records of Headset gateway */
    case SC_HEADSET_AUDIO_GATEWAY:
        *attribute = (SdpAttribute*) & HspAgSdpAttributes;
        attribute_num = sizeof(HspAgSdpAttributes) / sizeof(*(HspAgSdpAttributes));
        /* Retrieve the server channel from the service channel */
        *srv_chn = (U8) AgProtoDescList[sizeof(AgProtoDescList) - 1];
        break;
    }
    /* Return the total number of attribute number */
    return attribute_num;
}

