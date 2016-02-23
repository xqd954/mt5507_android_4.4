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
 * Nov 26 2008 mbj06038
 * [MAUI_01342148] [New feature][SyncML] Device sync support
 * 
 *
 * Nov 7 2008 mtk01411
 * [MAUI_01269065] [MT6601 BQB IOPT] SDAS_BV_03
 * 
 *
 * Aug 12 2008 mtk01411
 * [MAUI_00820444] [Bluetooth] SyncML feature supported in BT
 * 
 *
 * Jun 27 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Apr 20 2008 mtk01239
 * [MAUI_00758933] PBAP feature check in
 * 
 *
 * Apr 20 2008 mtk01239
 * [MAUI_00758933] PBAP feature check in
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Jan 5 2007 mtk01239
 * [MAUI_00356742] [Bluetooth] add SDP record for BIP
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Nov 27 2006 mtk01239
 * [MAUI_00346296] [Bluetooth] Sync BT Host protocol code.
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334713] [Bluetooth]Remove Report to kal_trace 1
 * 
 *
 * Jul 31 2006 mtk01239
 * [MAUI_00214015] [BT] update BT code and add OPP, FTP SDP record
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
#include "bt_common.h"
#include "stdio.h"
#include "l2cap.h"
#include "sdp.h"
#include "hfg.h"


typedef struct _ObexRecords
{
    BOOL registered;
    SdpRecord record;   /* SDP record */
} ObexRecords;

ObexRecords BT_FtpRecord;
ObexRecords BT_OppRecord;
ObexRecords BT_BipResponderRecord;
ObexRecords BT_BipRefobjRecord;

#if defined(__BT_PBAP_PROFILE__)
ObexRecords BT_PbapRecord;
#endif 

#ifdef __BT_SUPPORT_SYNCML__
/* For SyncML BT Server Testing */
ObexRecords BT_SyncML_Server_Record;
ObexRecords BT_SyncML_Client_Record;
#endif /* __BT_SUPPORT_SYNCML__ */

#if defined(__BT_MAPS_PROFILE__)
ObexRecords BT_MapsRecord[NUM_MAP_SRVS];
extern S8 maps_get_curr_registration(void);
#endif 
#if defined(__BT_MAPC_PROFILE__)
ObexRecords BT_MapcMnsRecord;
#endif 
/****************************************************************************
 * SDP Entries
 ****************************************************************************/
static const U8 FtpServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence, 3 bytes */
    /* This is a list of one element of type UUID */
    SDP_UUID_16BIT(SC_OBEX_FILE_TRANSFER)   /* FTP UUID in Big Endian */
};

static const U8 OppServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),          /* Data element sequence, 3 bytes */
    /* This is a list of one element of type UUID */
    SDP_UUID_16BIT(SC_OBEX_OBJECT_PUSH) /* FTP UUID in Big Endian */
};

static const U8 BipServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data Element Sequence, UUIDs*3 */
    /* This is an element list of type UUID */
    SDP_UUID_16BIT(SC_IMAGING_RESPONDER),   /* BIP Imaging Responder UUID */
};

static const U8 BipRefObjServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),                      /* Data Element Sequence, UUIDs*3 */
    /* This is an element list of type UUID */
    SDP_UUID_16BIT(SC_IMAGING_REFERENCED_OBJECTS),  /* BIP Imaging Responder UUID */
};

#if defined(__BT_PBAP_PROFILE__)
static const U8 PbapServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, UUIDs*3 */
    /* This is an element list of type UUID */
    SDP_UUID_16BIT(SC_PBAP_SERVER), /* PBAP UUID */
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

#ifdef __BT_SUPPORT_SYNCML__
static const U8 SyncMLServerServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17),      /* Data Element Sequence, UUIDs:1+16 */
    /* This is an element list of type UUID */
    SDP_UUID_128BIT(SC_SYNCML_BT_SERVER_128), /* SyncML Server UUID */
};

static const U8 SyncMLClientServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17),      /* Data Element Sequence, UUIDs:1+16 */
    /* This is an element list of type UUID */
    SDP_UUID_128BIT(SC_SYNCML_BT_CLIENT_128), /* SyncML Client UUID */
};
#endif /* __BT_SUPPORT_SYNCML__ */

#if defined(__BT_MAPS_PROFILE__)
static const U8 MapServClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, UUIDs*3 */
    /* This is an element list of type UUID */
    SDP_UUID_16BIT(SC_MAP_SERVER), /* MAP server UUID */
};
#endif /* defined(__BT_MAPS_PROFILE__) */ 

#if defined(__BT_MAPC_PROFILE__)
static const U8 MapClientClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, UUIDs*3 */
    /* This is an element list of type UUID */
    SDP_UUID_16BIT(SC_MAP_MNS_SERVER), /* MAP MNS server (MAP client) UUID */
};
#endif /* defined(__BT_MAPC_PROFILE__) */ 

/*
 * * OPTIONAL * Language BaseId List (goes with the service name).
 */
static const U8 ObexLangBaseIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* English Language */
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */
};

/*
 * OPTIONAL File Transfer service name (digianswer requires it).
 */

static const U8 FtpServServiceName[] = 
{
    SDP_TEXT_8BIT(19),  /* Null terminated text string, 19 bytes */
    'O', 'B', 'E', 'X', ' ', 'F', 'i', 'l', 'e', ' ',
    'T', 'r', 'a', 'n', 's', 'f', 'e', 'r', '\0'
};

static const U8 OppServServiceName[] = 
{
    SDP_TEXT_8BIT(17),  /* Null terminated text string, 17 bytes */
    'O', 'B', 'E', 'X', ' ', 'O', 'b', 'j', 'e', 'c', 't',
    ' ', 'P', 'u', 's', 'h', '\0'
};

static const U8 BipServServiceName[] = 
{
    SDP_TEXT_8BIT(8),   /* Null terminated text string, 8 bytes */
    'I', 'm', 'a', 'g', 'i', 'n', 'g', '\0'
};

static const U8 BipRefObjServServiceName[] = 
{
    SDP_TEXT_8BIT(26),  /* Null terminated text string, 8 bytes */
    'I', 'm', 'a', 'g', 'i',
    'n', 'g', ' ', 'r', 'e',
    'f', 'e', 'r', 'e', 'n',
    'c', 'e', 'd', ' ', 'O',
    'b', 'j', 'e', 'c', 't',
    '\0'
};

#if defined(__BT_PBAP_PROFILE__)
static const U8 PbapServServiceName[] = 
{
    SDP_TEXT_8BIT(20),  /* Null terminated text string, 8 bytes */
    'P', 'h', 'o', 'n', 'e', 'b', 'o', 'o',
    'k', ' ', 'A', 'c', 'c', 'e', 's', 's',
    ' ', 'P', 'S', 'E'
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

#ifdef __BT_SUPPORT_SYNCML__
static const U8 SyncMLServServiceName[] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'S', 'y', 'n', 'c', 'M', 'L', 
    'S', 'e', 'r', 'v', 'e', 'r', 
    '\0'
};

static const U8 SyncMLClientServiceName[] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'S', 'y', 'n', 'c', 'M', 'L', 
    'C', 'l', 'i', 'e', 'n', 't', 
    '\0'
};
#endif /* __BT_SUPPORT_SYNCML__ */

#if defined(__BT_MAPS_PROFILE__)
static U8 MapServServiceName1[255 + 2] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'M', 'A', 'P', ' ', 'M', 'A', 'S', '-',
    'n', 'a', 'm', 'e', '\0'
};
static U8 MapServServiceName2[255 + 2] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'M', 'A', 'P', ' ', 'M', 'A', 'S', '-',
    'n', 'a', 'm', 'e', '\0'
};
static U8 MapServServiceName3[255 + 2] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'M', 'A', 'P', ' ', 'M', 'A', 'S', '-',
    'n', 'a', 'm', 'e', '\0'
};
static U8 MapServServiceName4[255 + 2] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'M', 'A', 'P', ' ', 'M', 'A', 'S', '-',
    'n', 'a', 'm', 'e', '\0'
};

static U8 *MapServServiceNameList[NUM_MAP_SRVS] = {MapServServiceName1, 
                                                   MapServServiceName2, 
                                                   MapServServiceName3, 
                                                   MapServServiceName4};

#endif /* defined(__BT_MAPS_PROFILE__) */ 

#if defined(__BT_MAPC_PROFILE__)
static const U8 MapClientServiceName[] = 
{
    SDP_TEXT_8BIT(13),  /* Null terminated text string, 8 bytes */
    'M', 'A', 'P', ' ', 'M', 'N', 'S', '-',
    'n', 'a', 'm', 'e', '\0'
};
#endif /* defined(__BT_MAPC_PROFILE__) */ 

/*---------------------------------------------------------------------------
 * Ftp Public Browse Group.
 */
static const U8 ObexBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by OBEX.
 * 
 * Value of the protocol descriptor list for the Object Push profile.
 * This structure is a ROM'able representation of the RAM structure.
 * During ServerInit, this structure is copied into the RAM structure
 * used to register the server. A RAM structure must be used to be able
 * to dynamically set the RFCOMM channel in the RFCOMM protocol descriptor.
 */

static const U8 FTPProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * * data element sequence. The first element is L2CAP which only
     * * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 for L2CAP in Big Endian */

    /*
     * Next protocol descriptor in the list is RFCOMM. It contains two
     * elements which are the UUID and the channel. Ultimately this
     * channel will need to filled in with value returned by RFCOMM.
     * For now it will be set to 0x99 for testing purposes
     */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 for RFCOMM in Big Endian */
    // SDP_UINT_8BIT(FTP_RFCOMM_SRV_CHN),   /* Uint8 RFCOMM channel number - (set at run time) */
    SDP_UINT_8BIT(FTP_CHNL_NUM_START),
    /*
     * The next protocol descriptor is for OBEX. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for OBEX, 3 bytes */
    SDP_UUID_16BIT(PROT_OBEX)               /* Uuid16 for OBEX in Big Endian */
};

#ifdef __BT_GOEP_V2__
static const U8 FtpBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES for BIP, 6 bytes              */
    SDP_UUID_16BIT(SC_OBEX_FILE_TRANSFER), /* FTP UUID in Big Endian  */
    SDP_UINT_16BIT(0x0100)      /* Uint16 version number             */
};

static const U8 FTPBothProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * * data element sequence. The first element is L2CAP which only
     * * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 for L2CAP in Big Endian */

    /*
     * Next protocol descriptor in the list is RFCOMM. It contains two
     * elements which are the UUID and the channel. Ultimately this
     * channel will need to filled in with value returned by RFCOMM.
     * For now it will be set to 0x99 for testing purposes
     */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 for RFCOMM in Big Endian */
    // SDP_UINT_8BIT(FTP_RFCOMM_SRV_CHN),   /* Uint8 RFCOMM channel number - (set at run time) */
    SDP_UINT_8BIT(FTP_CHNL_NUM_START),
    /*
     * The next protocol descriptor is for OBEX. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for OBEX, 3 bytes */
    SDP_UUID_16BIT(PROT_OBEX)               /* Uuid16 for OBEX in Big Endian */
};
#endif

static const U8 OPPProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 for L2CAP in Big Endian */

    /*
     * Next protocol descriptor in the list is RFCOMM. It contains two
     * elements which are the UUID and the channel. Ultimately this
     * channel will need to filled in with value returned by RFCOMM.
     * For now it will be set to 0x99 for testing purposes
     */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 for RFCOMM in Big Endian */
    // SDP_UINT_8BIT(OPP_RFCOMM_SRV_CHN),   /* Uint8 RFCOMM channel number - (set at run time) */
    SDP_UINT_8BIT(OPP_CHNL_NUM_START),
    /*
     * The next protocol descriptor is for OBEX. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for OBEX, 3 bytes */
    SDP_UUID_16BIT(PROT_OBEX)               /* Uuid16 for OBEX in Big Endian */
};

#ifdef __BT_GOEP_V2__
static const U8 OPPOnlyL2capProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(15), /* Data element sequence, 17 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 for L2CAP in Big Endian */

    /*
     * Next protocol descriptor in the list is RFCOMM. It contains two
     * elements which are the UUID and the channel. Ultimately this
     * channel will need to filled in with value returned by RFCOMM.
     * For now it will be set to 0x99 for testing purposes
     */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 for RFCOMM in Big Endian */
    // SDP_UINT_8BIT(OPP_RFCOMM_SRV_CHN),   /* Uint8 RFCOMM channel number - (set at run time) */
    //SDP_UINT_8BIT(OPP_CHNL_NUM_START),
    /*
     * The next protocol descriptor is for OBEX. It contains one element
     * which is the UUID.
     */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data element sequence for OBEX, 3 bytes */
    SDP_UUID_16BIT(PROT_OBEX)               /* Uuid16 for OBEX in Big Endian */
};
#endif

static const U8 BIPProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),                      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),                    /* Uuid16 RFCOMM                            */
    // SDP_UINT_8BIT(BIP_RESPONDER_RFCOMM_SRV_CHN), /* Uint8 Job Channel on RFCOMM (varies)     */
    SDP_UINT_8BIT(BIP_RESPONDER_CHNL_NUM_START),
    SDP_ATTRIB_HEADER_8BIT(3),                      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)                       /* Uuid16 OBEX                              */
};

static const U8 BIPRefObjProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),                      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),                    /* Uuid16 RFCOMM                            */
    // SDP_UINT_8BIT(BIP_REFOBJ_RFCOMM_SRV_CHN),    /* Uint8 Job Channel on RFCOMM (varies)     */
    SDP_UINT_8BIT(BIP_REFOBJ_CHNL_NUM_START),
    SDP_ATTRIB_HEADER_8BIT(3),                      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)                       /* Uuid16 OBEX                              */
};

#if defined(__BT_PBAP_PROFILE__)
static const U8 PBAPProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(PBAP_CHNL_NUM_START),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

#ifdef __BT_SUPPORT_SYNCML__
static const U8 SyncMLServerProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(SyncML_SRV_CHNL_START),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX */                  

};

static const U8 SyncMLClientProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(SyncML_CLI_CHNL_START),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX */                  

};
#endif /* __BT_SUPPORT_SYNCML__ */

#if defined(__BT_MAPS_PROFILE__)
const U8 MAPServerProtoDescList1[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(MAP_SRV_CHNL_NUM_START),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};

const U8 MAPServerProtoDescList2[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(MAP_SRV_CHNL_NUM_START + 1),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};

const U8 MAPServerProtoDescList3[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(MAP_SRV_CHNL_NUM_START + 2),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};

const U8 MAPServerProtoDescList4[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(MAP_SRV_CHNL_NUM_START + 3),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

#if defined(__BT_MAPC_PROFILE__)
static const U8 MAPClientProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17), /* Data element sequence, 17 bytes          */

    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                             */

    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM                            */
    SDP_UINT_8BIT(MAP_CLI_CHNL_NUM_START),
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data element sequence for OBEX, 3 bytes  */
    SDP_UUID_16BIT(PROT_OBEX)       /* Uuid16 OBEX                              */
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 


static const U8 BipBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES for BIP, 6 bytes              */
    SDP_UUID_16BIT(SC_IMAGING), /* Basic Imaging UUID in Big Endian  */
    SDP_UINT_16BIT(0x0100)      /* Uint16 version number             */
};

static const U8 BipRefObjBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* DES for BIP, 6 bytes              */
    SDP_UUID_16BIT(SC_IMAGING), /* Basic Imaging UUID in Big Endian  */
    SDP_UINT_16BIT(0x0100)      /* Uint16 version number             */
};

#if defined(__BT_PBAP_PROFILE__)
static const U8 PbapBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),      /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES for PBAP, 6 bytes              */
    SDP_UUID_16BIT(SC_PBAP), /* Phonebook Access Profile UUID in Big Endian  */
    SDP_UINT_16BIT(0x0100)          /* Uint16 version number             */
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
static const U8 MapBtProfDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),      /* DES for ProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(6),      /* DES for PBAP, 6 bytes              */
    SDP_UUID_16BIT(SC_MAP), /* Phonebook Access Profile UUID in Big Endian  */
    SDP_UINT_16BIT(0x0100)          /* Uint16 version number             */
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

static const U8 FtpServGoepL2capPsm[] = 
{
    SDP_UINT_16BIT(BT_FTP_GOEPV2_PSM)         /* 'Any type of object' value BT_OPP_L2CAP_PSM */
};


static const U8 OppServSupportedFormats[] = 
{
    SDP_ATTRIB_HEADER_8BIT(2),  /* Data Element Sequence, 2 bytes */
    SDP_UINT_8BIT(0xFF)         /* 'Any type of object' value */
};

#ifdef __BT_GOEP_V2__
static const U8 OppServGoepL2capPsm[] = 
{
    //SDP_ATTRIB_HEADER_8BIT(3),              /* Data Element Sequence, 2 bytes */
    SDP_UINT_16BIT(BT_OPP_GOEPV2_PSM)         /* 'Any type of object' value BT_OPP_L2CAP_PSM */
};

static const U8 OppServBothProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(11),      /* DES for BTProfDescList, 8 bytes     */
    SDP_ATTRIB_HEADER_8BIT(9),      /* DES for OPP goepl2cappsm, 6 bytes   */
    SDP_UUID_16BIT(SC_OBEX_OBJECT_PUSH), /* OBEX PUSH Profile UUID in Big Endian  */
    SDP_UINT_16BIT(0x0102),         /* Version Number, 3 bytes*/
    SDP_UINT_16BIT(BT_OPP_GOEPV2_PSM)          /* PSM value, 3bytes BT_OPP_L2CAP_PSM */
};
#endif

/*---------------------------------------------------------------------------
 * BIP_SUPPORTED_CAPABILITIES constant
 *     Defines the supported imaging capabilities of the responder.  This
 *     value is stored in the SDP entry as a UINT8 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_CAPABILITIES
#define BIP_SUPPORTED_CAPABILITIES  0x09
#endif 

/*---------------------------------------------------------------------------
 * BIP_SUPPORTED_FEATURES constant
 *     Defines the supported imaging features of the responder. This value is
 *     stored in the SDP entry as a UINT16 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_FEATURES
#define BIP_SUPPORTED_FEATURES      0x002B
#endif 

/*---------------------------------------------------------------------------
 * BIP_SUPPORTED_FUNCTIONS constant
 *     Defines the supported imaging functions of the responder. This value
 *     is stored in the SDP entry as a UINT32 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_FUNCTIONS
#define BIP_SUPPORTED_FUNCTIONS     0x000B
#endif 

#ifndef BIP_REF_SUPPORTED_FUNCTIONS
#define BIP_REF_SUPPORTED_FUNCTIONS     0x1001
#endif 

/*---------------------------------------------------------------------------
 * BIP_SUPPORTED_CAPABILITIES constant
 *     Defines the supported imaging capabilities of the responder.  This
 *     value is stored in the SDP entry as a UINT8 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_CAPABILITIES
#define BIP_SUPPORTED_CAPABILITIES  0x09
#endif 

/*---------------------------------------------------------------------------
 * BIP_IMAGING_DATA_CAPACITY constant
 *     Defines the total imaging data capacity in bytes supported by the
 *     responder. This value is stored in the SDP entry as a UINT64. For
 *     portability, it is represented here as an 8 byte array. The default
 *     is 8 Megabytes.
 */
#ifndef BIP_IMAGING_DATA_CAPACITY
#define BIP_IMAGING_DATA_CAPACITY \
        0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00
#endif /* BIP_IMAGING_DATA_CAPACITY */ 

static const U8 BipSupCapabilities[] = 
{
    SDP_UINT_8BIT(BIP_SUPPORTED_CAPABILITIES)
};

static const U8 BipSupFeatures[] = 
{
    SDP_UINT_16BIT(BIP_SUPPORTED_FEATURES)
};

static const U8 BipSupFunctions[] = 
{
    SDP_UINT_32BIT(BIP_SUPPORTED_FUNCTIONS)
};

static const U8 BipRefSupFunctions[] = 
{
    SDP_UINT_32BIT(BIP_REF_SUPPORTED_FUNCTIONS)
};

static const U8 BipImagingDataCapacity[] = 
{
    SDP_UINT_64BIT(BIP_IMAGING_DATA_CAPACITY)
};

const U8 BipRefServiceId[] = 
{
    SDP_UUID_16BIT(0x111D)
};

#if defined(__BT_PBAP_PROFILE__)
static U8 PbapSupportedRepositories[] = 
{
    //    SDP_UINT_8BIT(PBAP_LOCAL_PHONEBOOK_SUPPORTED | 
    //                  (PBAP_SIM_PHONEBOOK_SUPPORTED << 1))
    SDP_UINT_8BIT(0x03)
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

SdpAttribute FtpSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, FtpServClassId),
    /* OBEX protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, FTPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Object push service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), FtpServServiceName)
};

#ifdef __BT_GOEP_V2__
SdpAttribute Ftpl2capSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, FtpServClassId),
    /* OBEX protocol descriptor list attribute */
//    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, FTPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, FtpBtProfDescList),    
    /* File transfer service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), FtpServServiceName),
    SDP_ATTRIBUTE(AID_GOEPL2CAP_PSM, FtpServGoepL2capPsm)
};


SdpAttribute FtpbothSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, FtpServClassId),
    /* OBEX protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, FTPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, FtpBtProfDescList),	
	/* File transfer service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), FtpServServiceName),
    SDP_ATTRIBUTE(AID_GOEPL2CAP_PSM, FtpServGoepL2capPsm)
};
#endif

#if defined(__BT_OPP_PROFILE__)
// GOEP v1 only rfcomm information
SdpAttribute OppSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, OppServClassId),
    /* OBEX protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, OPPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Object push service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), OppServServiceName),
    SDP_ATTRIBUTE(AID_SUPPORTED_FORMATS_LIST, OppServSupportedFormats)
};

#ifdef __BT_GOEP_V2__
// GOEPv2 only l2cap, no rfcomm information
SdpAttribute Oppl2capSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, OppServClassId),
    /* OBEX protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, OPPOnlyL2capProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Object push service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), OppServServiceName),
    SDP_ATTRIBUTE(AID_SUPPORTED_FORMATS_LIST, OppServSupportedFormats),
    SDP_ATTRIBUTE(AID_GOEPL2CAP_PSM, OppServGoepL2capPsm)
};

// GOEPv1+v2 goepl2cap psm and rfcomm information
SdpAttribute OppbothSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, OppServClassId),
    /* OBEX protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, OPPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Object push service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), OppServServiceName),
    SDP_ATTRIBUTE(AID_SUPPORTED_FORMATS_LIST, OppServSupportedFormats),
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, OppServBothProfileDescList),
    SDP_ATTRIBUTE(AID_GOEPL2CAP_PSM, OppServGoepL2capPsm)
};
#endif
#endif /* __BT_OPP_PROFILE__ */

SdpAttribute BipResponderSdpAttributes[] = 
{
    /* BIP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, BipServClassId),
    /* BIP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, BIPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, BipBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), BipServServiceName),
    /* Supported Capabilities */
    SDP_ATTRIBUTE(AID_SUPPORTED_CAPABILITIES, BipSupCapabilities),
    /* Supported features */
    SDP_ATTRIBUTE(AID_SUPPORTED_FEATURES, BipSupFeatures),
    /* Supported functions */
    SDP_ATTRIBUTE(AID_SUPPORTED_FUNCTIONS, BipSupFunctions),
    /* Total image data capacity */
    SDP_ATTRIBUTE(AID_TOTAL_IMAGE_DATA_CAPACITY, BipImagingDataCapacity)
};

SdpAttribute BipRefobjSdpAttributes[] = 
{
    /* Object push service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, BipRefObjServClassId),
    SDP_ATTRIBUTE(AID_SERVICE_ID, BipRefServiceId),

    /* OBEX protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, BIPRefObjProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, BipBtProfDescList),
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), BipRefObjServServiceName),
    SDP_ATTRIBUTE(AID_SUPPORTED_FUNCTIONS, BipRefSupFunctions),

    /* Object push service name (Optional) */

};

#if defined(__BT_PBAP_PROFILE__)
SdpAttribute PbapSdpAttributes[] = 
{
    /* BIP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PbapServClassId),
    /* BIP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PBAPProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, PbapBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), PbapServServiceName),
    /* Supported Capabilities */
    SDP_ATTRIBUTE(AID_SUPPORTED_REPOSITORIES, PbapSupportedRepositories),
};
#endif /* defined(__BT_PBAP_PROFILE__) */ 

#ifdef __BT_SUPPORT_SYNCML__
/* For SyncML BT Server Testing */
SdpAttribute SyncMLServerSdpAttributes[] =
{
    /* SyncML service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SyncMLServerServClassId),
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SyncMLServerProtoDescList),
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SyncMLServServiceName),
};

SdpAttribute SyncMLClientSdpAttributes[] =
{
    /* SyncML service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SyncMLClientServClassId),
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SyncMLClientProtoDescList),
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SyncMLClientServiceName),
};
#endif /* __BT_SUPPORT_SYNCML__ */

#if defined(__BT_MAPS_PROFILE__)
static U8 MapMasInstanceId1[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 MapMasInstanceId2[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 MapMasInstanceId3[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 MapMasInstanceId4[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 *MapMasInstanceIdList[NUM_MAP_SRVS] = {MapMasInstanceId1, 
                                          MapMasInstanceId2, 
                                          MapMasInstanceId3, 
                                          MapMasInstanceId4};

static U8 MapSupportedMessageTypes1[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 MapSupportedMessageTypes2[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 MapSupportedMessageTypes3[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 MapSupportedMessageTypes4[] = 
{
    SDP_UINT_8BIT(0x00)
};

static U8 *MapSupportedMessageTypesList[NUM_MAP_SRVS] = {MapSupportedMessageTypes1, 
                                                  MapSupportedMessageTypes2, 
                                                  MapSupportedMessageTypes3, 
                                                  MapSupportedMessageTypes4};

SdpAttribute MapServerSdpAttributes1[] = 
{
    /* MAP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, MapServClassId),
    /* MAP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, MAPServerProtoDescList1),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, MapBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), MapServServiceName1),
    /* MAS instance ID */
    SDP_ATTRIBUTE(AID_MAP_MAS_INSTANCE_ID, MapMasInstanceId1),
    /* Supported message types */
    SDP_ATTRIBUTE(AID_MAP_SUPPORTED_MSG_TYPES, MapSupportedMessageTypes1),
};

SdpAttribute MapServerSdpAttributes2[] = 
{
    /* MAP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, MapServClassId),
    /* MAP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, MAPServerProtoDescList2),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, MapBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), MapServServiceName2),
    /* MAS instance ID */
    SDP_ATTRIBUTE(AID_MAP_MAS_INSTANCE_ID, MapMasInstanceId2),
    /* Supported message types */
    SDP_ATTRIBUTE(AID_MAP_SUPPORTED_MSG_TYPES, MapSupportedMessageTypes2),
};

SdpAttribute MapServerSdpAttributes3[] = 
{
    /* MAP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, MapServClassId),
    /* MAP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, MAPServerProtoDescList3),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, MapBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), MapServServiceName3),
    /* MAS instance ID */
    SDP_ATTRIBUTE(AID_MAP_MAS_INSTANCE_ID, MapMasInstanceId3),
    /* Supported message types */
    SDP_ATTRIBUTE(AID_MAP_SUPPORTED_MSG_TYPES, MapSupportedMessageTypes3),
};

SdpAttribute MapServerSdpAttributes4[] = 
{
    /* MAP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, MapServClassId),
    /* MAP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, MAPServerProtoDescList4),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, MapBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), MapServServiceName4),
    /* MAS instance ID */
    SDP_ATTRIBUTE(AID_MAP_MAS_INSTANCE_ID, MapMasInstanceId4),
    /* Supported message types */
    SDP_ATTRIBUTE(AID_MAP_SUPPORTED_MSG_TYPES, MapSupportedMessageTypes4),
};

SdpAttribute *MapServerSdpAttributesList[NUM_MAP_SRVS] = {MapServerSdpAttributes1, 
                                                          MapServerSdpAttributes2, 
                                                          MapServerSdpAttributes3, 
                                                          MapServerSdpAttributes4};

#endif /* defined(__BT_MAPS_PROFILE__) */ 

#if defined(__BT_MAPC_PROFILE__)
SdpAttribute MapClientSdpAttributes[] = 
{
    /* BIP service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, MapClientClassId),
    /* BIP Protocol Descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, MAPClientProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, ObexBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, ObexLangBaseIdList),
    /* Profile descriptor list */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, MapBtProfDescList),
    /* Basic Imaging service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), MapClientServiceName),
};
#endif /* defined(__BT_MAPC_PROFILE__) */ 

/*********************
Function Definitions 
**********************/
#if defined(__BT_PBAP_PROFILE__)

void PBAPDeviceSdpDB_SupportedRepositories(U8 support_value)
{
     PbapSupportedRepositories[0x01] = support_value;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  ObexSdpDB_GetServChn
 * DESCRIPTION
 *  This function is used to get the Handsfree Audio Gateway service record attributes pointer,
 *  attribute number and the service channel in the service record
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_HANDSFREE_AUDIO_GATEWAY.
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 ObexSdpDB_GetServChn(U32 service_type, U8 instance)
{
    U16 uuid;
    U8 chnnel;

    uuid = (U16) service_type;
    switch (uuid)
    {
        case SC_OBEX_OBJECT_PUSH:
            chnnel = OPP_CHNL_NUM_START;
            break;
        case SC_OBEX_FILE_TRANSFER:
            chnnel = FTP_CHNL_NUM_START;
            break;
        case SC_IMAGING_RESPONDER:
            chnnel = BIP_RESPONDER_CHNL_NUM_START;
            break;
        case SC_IMAGING_REFERENCED_OBJECTS:
            chnnel = BIP_REFOBJ_CHNL_NUM_START;
            break;
    #if defined(__BT_PBAP_PROFILE__)
        case SC_PBAP_SERVER:
            chnnel = PBAP_CHNL_NUM_START;
            break;
    #endif /* defined(__BT_PBAP_PROFILE__) */
    #if defined(__BT_SUPPORT_SYNCML__)
        case SC_SYNCML_BT_CLIENT:
            chnnel = SyncML_CLI_CHNL_START;
            break;
        case SC_SYNCML_BT_SERVER:
            chnnel = SyncML_SRV_CHNL_START;
            break;
    #endif
    #if defined(__BT_MAPS_PROFILE__)
        case SC_MAP_SERVER:
            chnnel = MAP_SRV_CHNL_NUM_START + instance;
            break;
    #endif /* defined(__BT_PBAP_PROFILE__) */
    #if defined(__BT_MAPC_PROFILE__)
        case SC_MAP_MNS_SERVER:
            chnnel = MAP_CLI_CHNL_NUM_START;
            break;
    #endif /* defined(__BT_PBAP_PROFILE__) */
        default:
            chnnel = 0xFF;
            break;
    }
    return chnnel;
}


/*****************************************************************************
 * FUNCTION
 *  ObexSdpDB_Registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  service_type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 ObexSdpDB_RegistrationEx(U32 service_type, U8 tp_type)
{
    U16 uuid;
    BtStatus status;

    uuid = (U16) service_type;
    switch (uuid)
    {
#if defined(__BT_OPP_PROFILE__)
        case SC_OBEX_OBJECT_PUSH:
            BT_OppRecord.registered = TRUE;
            BT_OppRecord.record.num = sizeof(OppSdpAttributes) / sizeof(*(OppSdpAttributes));
            /* Retrieve the server channel from the service channel */
#ifdef  __BT_GOEP_V2__
            switch(tp_type){
                
                case 2: // GOEP_TP_BOTH:
                    OS_Report("[OBEX] register both opp rfcomm&l2cap sdp");
                    BT_OppRecord.record.num = sizeof(OppbothSdpAttributes) / sizeof(*(OppbothSdpAttributes));
                    BT_OppRecord.record.attribs = (SdpAttribute*) &OppbothSdpAttributes;
                    break;
                case 1: //GOEP_TP_L2CAP_ONLY:
                    OS_Report("[OBEX] register opp l2cap sdp");			
                    BT_OppRecord.record.num = sizeof(Oppl2capSdpAttributes) / sizeof(*(Oppl2capSdpAttributes));
                    BT_OppRecord.record.attribs = (SdpAttribute*) &Oppl2capSdpAttributes;
                break;
                  
                case 0: //GOEP_TP_RFCOMM_ONLY:
                default:
                    OS_Report("[OBEX] register  opp rfcomm sdp");
                    BT_OppRecord.record.attribs = (SdpAttribute*) & OppSdpAttributes;
                    break;
            }
            status = SDP_AddRecord(&BT_OppRecord.record);
#else
            BT_OppRecord.registered = TRUE;
            BT_OppRecord.record.num = sizeof(OppSdpAttributes) / sizeof(*(OppSdpAttributes));
            /* Retrieve the server channel from the service channel */
            BT_OppRecord.record.attribs = (SdpAttribute*) & OppSdpAttributes;
            status = SDP_AddRecord(&BT_OppRecord.record);
#endif
#endif /* __BT_OPP_PROFILE__ */
            break;
        case SC_OBEX_FILE_TRANSFER:
            BT_FtpRecord.registered = TRUE;
            BT_FtpRecord.record.num = sizeof(FtpSdpAttributes) / sizeof(*(FtpSdpAttributes));
            /* Retrieve the server channel from the service channel */
            BT_FtpRecord.record.attribs = (SdpAttribute*) & FtpSdpAttributes;
#ifdef __BT_GOEP_V2__
            switch(tp_type){
                case 2: // GOEP_TP_BOTH:
                    OS_Report("[OBEX] register both ftp rfcomm&l2cap sdp");                
		    BT_FtpRecord.record.num = sizeof(FtpbothSdpAttributes) / sizeof(*(FtpbothSdpAttributes));
		    BT_FtpRecord.record.attribs = (SdpAttribute*) & FtpbothSdpAttributes;
                break;
		case 1: //GOEP_TP_L2CAP_ONLY: 
		    OS_Report("[OBEX] register only ftp l2cap sdp");                
		    BT_FtpRecord.record.num = sizeof(Ftpl2capSdpAttributes) / sizeof(*(Ftpl2capSdpAttributes));
		    BT_FtpRecord.record.attribs = (SdpAttribute*) & Ftpl2capSdpAttributes;
                break;
		case 0: //GOEP_TP_RFCOMM_ONLY:
                default:
                    OS_Report("[OBEX] register only ftp rfcomm sdp");                
                    BT_FtpRecord.record.num = sizeof(FtpSdpAttributes) / sizeof(*(FtpSdpAttributes));
                    BT_FtpRecord.record.attribs = (SdpAttribute*) & FtpSdpAttributes;
                break;
            }
            status = SDP_AddRecord(&BT_FtpRecord.record);
#else
            BT_FtpRecord.registered = TRUE;
            BT_FtpRecord.record.num = sizeof(FtpSdpAttributes) / sizeof(*(FtpSdpAttributes));
            /* Retrieve the server channel from the service channel */
            BT_FtpRecord.record.attribs = (SdpAttribute*) & FtpSdpAttributes;
            status = SDP_AddRecord(&BT_FtpRecord.record);
            status = SDP_AddRecord(&BT_FtpRecord.record);
#endif
            break;
        case SC_IMAGING_RESPONDER:
            BT_BipResponderRecord.registered = TRUE;
            BT_BipResponderRecord.record.num = sizeof(BipResponderSdpAttributes) / sizeof(*(BipResponderSdpAttributes));
            /* Retrieve the server channel from the service channel */
            BT_BipResponderRecord.record.attribs = (SdpAttribute*) & BipResponderSdpAttributes;
            status = SDP_AddRecord(&BT_BipResponderRecord.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            //2011.10.13 mtk03036 
            //BT_BipRefobjRecord.registered = TRUE;
            //BT_BipRefobjRecord.record.num = sizeof(BipRefobjSdpAttributes) / sizeof(*(BipRefobjSdpAttributes));
            /* Retrieve the server channel from the service channel */
            //BT_BipRefobjRecord.record.attribs = (SdpAttribute*) & BipRefobjSdpAttributes;
            //status = SDP_AddRecord(&BT_BipRefobjRecord.record);
            //kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            //2011.10.13 mtk03036 end
            break;
    #if defined(__BT_PBAP_PROFILE__)
        case SC_PBAP_SERVER:
            BT_PbapRecord.registered = TRUE;
            BT_PbapRecord.record.num = sizeof(PbapSdpAttributes) / sizeof(*(PbapSdpAttributes));
            BT_PbapRecord.record.attribs = (SdpAttribute*) & PbapSdpAttributes;
            status = SDP_AddRecord(&BT_PbapRecord.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
    #endif /* defined(__BT_PBAP_PROFILE__) */ 
    #ifdef __BT_SUPPORT_SYNCML__
	case SC_SYNCML_BT_SERVER:
	    BT_SyncML_Server_Record.registered = TRUE;
            BT_SyncML_Server_Record.record.num = sizeof(SyncMLServerSdpAttributes) / sizeof(*(SyncMLServerSdpAttributes));
            BT_SyncML_Server_Record.record.attribs = (SdpAttribute*)& SyncMLServerSdpAttributes;
            status = SDP_AddRecord(&BT_SyncML_Server_Record.record);
	    kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
	case SC_SYNCML_BT_CLIENT:
	    BT_SyncML_Client_Record.registered = TRUE;
            BT_SyncML_Client_Record.record.num = sizeof(SyncMLClientSdpAttributes) / sizeof(*(SyncMLClientSdpAttributes));
            BT_SyncML_Client_Record.record.attribs = (SdpAttribute*)& SyncMLClientSdpAttributes;
            status = SDP_AddRecord(&BT_SyncML_Client_Record.record);
	    kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
     #endif /* __BT_SUPPORT_SYNCML__ */	 
    #if defined(__BT_MAPS_PROFILE__)
        case SC_MAP_SERVER:
        {
            S16 regIndx= -1;
            regIndx = maps_get_curr_registration();
            if( regIndx >= 0 ){
	            Assert(BT_MapsRecord[(U8)regIndx].registered == (char) FALSE);
	            BT_MapsRecord[(U8)regIndx].registered = (char) TRUE;
	            BT_MapsRecord[(U8)regIndx].record.num = sizeof(MapServerSdpAttributes1) / sizeof(*(MapServerSdpAttributes1));
	            BT_MapsRecord[(U8)regIndx].record.attribs = (SdpAttribute*) MapServerSdpAttributesList[(U8)regIndx];
	            status = SDP_AddRecord(&BT_MapsRecord[(U8)regIndx].record);
            }else{
                status = BT_STATUS_INVALID_PARM;
            }
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
        }
        break;
    #endif /* defined(__BT_MAPS_PROFILE__) */ 
    #if defined(__BT_MAPC_PROFILE__)
        case SC_MAP_MNS_SERVER:
            BT_MapcMnsRecord.registered = TRUE;
            BT_MapcMnsRecord.record.num = sizeof(MapClientSdpAttributes) / sizeof(*(MapClientSdpAttributes));
            BT_MapcMnsRecord.record.attribs = (SdpAttribute*) & MapClientSdpAttributes;
            status = SDP_AddRecord(&BT_MapcMnsRecord.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
    #endif /* defined(__BT_MAPC_PROFILE__) */ 
        default:
            status = BT_STATUS_NOT_SUPPORTED;
            break;
    }
    Assert((status != BT_STATUS_INVALID_PARM));
    kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
    return (U8) status;
}

U8 ObexSdpDB_Registration(U32 service_type){
    return ObexSdpDB_RegistrationEx(service_type, 0);
}


/*****************************************************************************
 * FUNCTION
 *  ObexSdpDB_DeRegistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  service_type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 ObexSdpDB_DeRegistration(U32 service_type)
{
    U16 uuid;
    BtStatus status;

    uuid = (U16) service_type;
    switch (uuid)
    {
        case SC_OBEX_OBJECT_PUSH:
            status = SDP_RemoveRecord(&BT_OppRecord.record);
            break;
        case SC_OBEX_FILE_TRANSFER:
            status = SDP_RemoveRecord(&BT_FtpRecord.record);
            break;
        case SC_IMAGING_RESPONDER:
            status = SDP_RemoveRecord(&BT_BipResponderRecord.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            //2011.10.13 mtk03036 
            //status = SDP_RemoveRecord(&BT_BipRefobjRecord.record);
            //kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            //2011.10.13 mek03036 end
            break;
    #if defined(__BT_PBAP_PROFILE__)
        case SC_PBAP_SERVER:
            status = SDP_RemoveRecord(&BT_PbapRecord.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
    #endif /* defined(__BT_PBAP_PROFILE__) */
#if defined(__BT_SUPPORT_SYNCML__)
        case SC_SYNCML_BT_CLIENT:
            status = SDP_RemoveRecord(&BT_SyncML_Client_Record.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
        case SC_SYNCML_BT_SERVER:
            status = SDP_RemoveRecord(&BT_SyncML_Server_Record.record);
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
            break;
#endif
#if defined(__BT_MAPS_PROFILE__)
    case SC_MAP_SERVER:
        {
            extern S8 maps_get_curr_deregistration(void);
            S8 regIndx = maps_get_curr_deregistration();
            status = SDP_RemoveRecord(&BT_MapsRecord[(U8)regIndx].record);
            BT_MapsRecord[(U8)regIndx].registered = FALSE;
            kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
        }
        break;
#endif /* defined(__BT_MAPS_PROFILE__) */
        default:
            status = BT_STATUS_NOT_SUPPORTED;
            break;
    }
    return (U8) status;
}

#if defined(__BT_MAPS_PROFILE__)
void MAPSDeviceSdpDB_ReplaceRecord(U8 idx, U16 uuid, U8 *attribute, U8 size)
{
    U8 i = 0;
    U8 record_num;
    SdpAttribute *sdpAttributes = MapServerSdpAttributesList[idx];

    record_num = sizeof(MapServerSdpAttributes1) / sizeof(*(MapServerSdpAttributes1));
    for(i=0;i < record_num; i++)
    {
        if(sdpAttributes[i].id == uuid)
        {
            if (uuid == AID_MAP_MAS_INSTANCE_ID)
            {
                U8 *value = MapMasInstanceIdList[idx];
                value[1] = *attribute;
            }
            else if (uuid == AID_MAP_SUPPORTED_MSG_TYPES)
            {
                U8 *value = MapSupportedMessageTypesList[idx];
                value[1] = *attribute;
            }
            else if (uuid == (AID_SERVICE_NAME + 0x0100))
            {
                U8 *value = MapServServiceNameList[idx];
                // size = (size > 255) ? 255 : size; // size is U8. the max value is 255
                value[1] = size;
                OS_StrnCpy((S8 *)&value[2], (const S8 *)attribute, size);
                sdpAttributes[i].len = 2 + size;
            }
            break;
        }
    }

}
#endif

