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
 * prxsdpdb.c
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
 * Daylong Chen
 *      Create 2011/03/14
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
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
#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)

#include "sdp.h"
#include "attdb.h"
#include "l2cap_adp.h"

/********
Constants 
*********/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/
typedef struct _PrxRecords
{
	BOOL registered;
	SdpRecord record;	/* SDP record */
} PrxRecords;

PrxRecords prxImalertRecord;
PrxRecords prxTxpowerRecord;
PrxRecords prxLinklossRecord;

/****************************************************************************
 * Proximity Record
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by A2DP.
 * 
 * Value of the protocol descriptor list for the A2DP Profile.
 * This structure is a ROM'able representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service. A RAM structure must be used to be able to dynamically set the 
 * RFCOMM channel in the RFCOMM protocol descriptor.
 */
const U8 PrxLLSProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(19), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),     /* TODO: ATT PSM */

    /*
     * Next protocol descriptor in the list is AVDTP. It contains one
     * element which is the version.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for AVDTP, 3 bytes */
    SDP_UUID_16BIT(PROT_ATT),     /* TODO: ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS),     /* Uint16 start handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END),     /* Uint16 end handle */
};

const U8 PrxIASProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(19), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),     /* TODO: ATT PSM */

    /*
     * Next protocol descriptor in the list is AVDTP. It contains one
     * element which is the version.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for AVDTP, 3 bytes */
    SDP_UUID_16BIT(PROT_ATT),     /* TODO: ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT),     /* Uint16 start handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT_END),     /* Uint16 end handle */
};

const U8 PrxTPSProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(19), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),     /* TODO: ATT PSM */

    /*
     * Next protocol descriptor in the list is AVDTP. It contains one
     * element which is the version.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for AVDTP, 3 bytes */
    SDP_UUID_16BIT(PROT_ATT),     /* TODO: ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER),     /* Uint16 start handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER_END),     /* Uint16 end handle */
};

/*---------------------------------------------------------------------------
 * A2DP Public Browse Group.
 */
const U8 PrxPublicBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/****************************************************************************
 * A2DP SDP Entries - Audio Sink
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList
 */
const U8 PrxImalertClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_ALERT)   
};
const U8 PrxLinklossClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_LINKLOSS)
};
const U8 PrxTxpowerClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),      /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_TXPOWER)   
};

/******************************
Definitions Of Exported Globals 
*******************************/
/*
 * Proximity attributes.
 * 
 */
SdpAttribute PrxLinkLossSdpAttributes[] = 
{
    /* Audio Sink class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PrxLinklossClassId),
    /* protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PrxLLSProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, PrxPublicBrowseGroup),
};

SdpAttribute PrxTxpowerSdpAttributes[] = 
{
    /* Audio Sink class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PrxTxpowerClassId),
    /* protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PrxTPSProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, PrxPublicBrowseGroup),
};

SdpAttribute PrxImalertSdpAttributes[] = 
{
    /* Audio Sink class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, PrxImalertClassId),
    /* protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, PrxIASProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, PrxPublicBrowseGroup),
};

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  
 * DESCRIPTION
 *  This function is used to add service record of proximity (BR/EDR)
 * PARAMETERS
 *  index        [IN]        index infomation
 *  capability           [IN]    services list     
 * RETURNS
 *  U8 : success or fail
 *****************************************************************************/
U8 PrxSdpDB_AddRecord(U8 index, U8 capability)
{
	BtStatus status;
	OS_Report("[PRX] PrxSdpDB_AddRecord index:%d capability:%d", index, capability);
	
	if( 0 != index ){
		OS_Report("[PRX][WRN] only support index 0 now!");
		return BT_STATUS_FAILED;
	}
	
	// TODO: Update the start & end handle in sdp record !
	
    /* Return the total number of attribute number */
	if( capability & 0x01){
		prxLinklossRecord.registered = TRUE;
		prxLinklossRecord.record.num = sizeof(PrxLinkLossSdpAttributes) / sizeof( *(PrxLinkLossSdpAttributes) );
		prxLinklossRecord.record.attribs = (SdpAttribute*) &PrxLinkLossSdpAttributes;
		status = SDP_AddRecord( &prxLinklossRecord.record );
		OS_Report("[PRX] status:%d", status);
	}
	if( capability & 0x02){
		prxTxpowerRecord.registered = TRUE;
		prxTxpowerRecord.record.num = sizeof(PrxTxpowerSdpAttributes) / sizeof( *(PrxTxpowerSdpAttributes) );
		prxTxpowerRecord.record.attribs = (SdpAttribute*) &PrxTxpowerSdpAttributes;
		status = SDP_AddRecord( &prxTxpowerRecord.record );		
		OS_Report("[PRX] status:%d", status);
	}
	if( capability & 0x04){
		prxImalertRecord.registered = TRUE;
		prxImalertRecord.record.num = sizeof(PrxImalertSdpAttributes) / sizeof( *(PrxImalertSdpAttributes) );
		prxImalertRecord.record.attribs = (SdpAttribute*) &PrxImalertSdpAttributes;
		status = SDP_AddRecord( &prxImalertRecord.record );
		OS_Report("[PRX] status:%d", status);

	}
	return BT_STATUS_SUCCESS;
}

U8 PrxSdpDB_RemoveRecord(U8 index, U8 capability)
{
	BtStatus status;
	OS_Report("[PRX] PrxSdpDB_RemoveRecord index:%d capability:%d", index, capability);	
	if( 0 != index ){
		OS_Report("[PRX][WRN] only support index 0 now!");
		return BT_STATUS_FAILED;
	}

	if( TRUE == prxLinklossRecord.registered && (capability & 0x01) ){
		prxLinklossRecord.registered = FALSE;
		status = SDP_RemoveRecord( &prxLinklossRecord.record );
		OS_Report("[PRX] remove linkloss status:%d", status);
	}
	if( TRUE == prxTxpowerRecord.registered && (capability & 0x02) ){
		prxTxpowerRecord.registered = FALSE;
		status = SDP_RemoveRecord( &prxTxpowerRecord.record );		
		OS_Report("[PRX] remove txpower status:%d", status);		
	}
	if( TRUE == prxImalertRecord.registered && (capability & 0x04) ){
		prxImalertRecord.registered = FALSE;
		status = SDP_RemoveRecord( &prxImalertRecord.record );
		OS_Report("[PRX] remove imalert status:%d", status);		
	}
	return BT_STATUS_SUCCESS;
}
#endif
