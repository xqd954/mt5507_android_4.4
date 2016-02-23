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
 * a2dpsdpdb.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions and global variable to the A2DP Service Records. 
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

/****************************************************************************
 *
 * File:
 *     $Workfile:a2dp_sdp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains SDP code for the A2DP profile
 *             
 * Created:     June 3, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#ifdef __BT_A2DP_PROFILE__

#include "sdp.h"

/********
Constants 
*********/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/****************************************************************************
 * A2DP SDP Entries - Audio Source
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList
 */
const U8 A2dpSrcClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(SC_AUDIO_SOURCE) /* Audio Sink UUID in Big Endian */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by A2DP.
 * 
 * Value of the protocol descriptor list for the A2DP Profile.
 * This structure is a ROM'able representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service. A RAM structure must be used to be able to dynamically set the 
 * RFCOMM channel in the RFCOMM protocol descriptor.
 */
const U8 A2dpProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(16), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(PROT_AVDTP), /* AVDTP PSM */

    /*
     * Next protocol descriptor in the list is AVDTP. It contains one
     * element which is the version.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for AVDTP, 3 bytes */
    SDP_UUID_16BIT(PROT_AVDTP), /* AVDTP UUID */
    SDP_UINT_16BIT(0x0100)      /* Uint8 AVDTP Version (1.0) */
};

/*---------------------------------------------------------------------------
 * A2DP Public Browse Group.
 */
const U8 A2dpBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/*---------------------------------------------------------------------------
 * A2DP Profile Descriptor List
 */
const U8 A2dpProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_AUDIO_DISTRIBUTION),
    SDP_UINT_16BIT(0x0100)
};

/*---------------------------------------------------------------------------
 * * OPTIONAL *  ServiceName
 */
const U8 A2dpServiceName[] = 
{
    SDP_TEXT_8BIT(15),  /* Null terminated text string */
    'A', 'd', 'v', 'a', 'n', 'c', 'e', 'd', ' ', 'A', 'u',
    'd', 'i', 'o', '\0'
};

/****************************************************************************
 * A2DP SDP Entries - Audio Sink
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList
 */
const U8 A2dpSnkClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(SC_AUDIO_SINK)   /* Audio Sink UUID in Big Endian */
};

/******************************
Definitions Of Exported Globals 
*******************************/

/*
 * A2DP Source attributes.
 * 
 * This is a ROM template for the RAM structure used to register the
 * A2DP Source SDP record.
 */
SdpAttribute A2dpSrcSdpAttributes[] = 
{
    /* Audio Source class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, A2dpSrcClassId),
    /* A2DP protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, A2dpProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, A2dpBrowseGroup),
    /* A2DP profile descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, A2dpProfileDescList),
    /* A2DP Service Name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), A2dpServiceName)
};

/*
 * A2DP Sink attributes.
 * 
 * This is a ROM template for the RAM structure used to register the
 * A2DP Sink SDP record.
 */
SdpAttribute A2dpSnkSdpAttributes[] = 
{
    /* Audio Sink class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, A2dpSnkClassId),
    /* A2DP protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, A2dpProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, A2dpBrowseGroup),
    /* A2DP profile descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, A2dpProfileDescList),
    /* A2DP Service Name */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), A2dpServiceName),
};

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  A2dpSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the service record attributes pointer and attribute number of A2DP
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as A2DP SOURCE and SINK.
 *  attribute           [IN]        
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 A2dpSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute)
{
    U8 attribute_num = 0;

    switch (service_type)
    {
            /* Get the service record of the A2DP source service */
        case SC_AUDIO_SOURCE:
            *attribute = (SdpAttribute*) & A2dpSrcSdpAttributes;
            attribute_num = sizeof(A2dpSrcSdpAttributes) / sizeof(*(A2dpSrcSdpAttributes));
            break;
            /* Get the service record of the A2DP sink service */
        case SC_AUDIO_SINK:
            *attribute = (SdpAttribute*) & A2dpSnkSdpAttributes;
            attribute_num = sizeof(A2dpSnkSdpAttributes) / sizeof(*(A2dpSnkSdpAttributes));
            break;
    }
    /* Return the total number of attribute number */
    return attribute_num;
}
#endif /* __BT_A2DP_PROFILE__ */
