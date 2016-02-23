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
 * timesdpdb.c
 *
 * Project:
 * --------
 *   Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions and global variable to the A2DP Service Records. 
 *
 * Author:
 * -------
 * Jacob Lee
 *      Create 2011/07/22
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#if defined(__BT_TIMES_PROFILE__) || defined(__BT_TIMEC_PROFILE__)

#include "attdb.h"
#include "l2cap_adp.h"
#include "sdp.h"

typedef struct _TimeRecord
{
	BOOL registered;
	SdpRecord record;	/* SDP record */
} TimeRecord;

TimeRecord CurTimeRecord;
TimeRecord NextDSTRecord;
TimeRecord RefTimeUpdateRecord;

/* Protocol Descriptor List for Current Time Service */
const U8 CurTimePrtclDescList[] = 
{
	/* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(19),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),     /* TODO: ATT PSM */

    /*
     * Next protocol descriptor in the list is ATT.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for ATT */
    SDP_UUID_16BIT(0x1111),     /* TODO: ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME),     /* Uint16 start handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME_END),     /* Uint16 end handle */
};

/* Protocol Descriptor List for Next DST Change Service */
const U8 NextDSTPrtclDescList[] = 
{
	/* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(19),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),

    /*
     * Next protocol descriptor in the list is ATT.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for ATT */
    SDP_UUID_16BIT(0x1111),     /* TODO: ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE),     /* Uint16 start handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE_END),     /* Uint16 end handle */
};

/* Protocol Descriptor List for Reference Time Update Service */
const U8 RefTimeUpdatePrtclDescList[] = 
{
	/* Data element sequence */
    SDP_ATTRIB_HEADER_8BIT(19),

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP.
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP), /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),

    /*
     * Next protocol descriptor in the list is ATT.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for ATT */
    SDP_UUID_16BIT(0x1111),     /* TODO: ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE),     /* Uint16 start handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE_END),     /* Uint16 end handle */
};

/*---------------------------------------------------------------------------
 * Time Public Browse Group.
 */
const U8 TimePublicBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/****************************************************************************
 * Time SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList
 */
const U8 CurTimeClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_TIME) 
};
const U8 NextDSTClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_DST)
};
const U8 RefTimeUpdateClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_TIMEUPDATE)
};

/***********************************
 * Definitions Of Exported Globals 
 ***********************************/
/*
 * Time attributes.
 */
SdpAttribute CurTimeSdpAttributes[] = 
{
    /* Class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, CurTimeClassId),
    /* protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, CurTimePrtclDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, TimePublicBrowseGroup),
};

SdpAttribute NextDSTSdpAttributes[] = 
{
    /* Class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, NextDSTClassId),
    /* protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, NextDSTPrtclDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, TimePublicBrowseGroup),
};

SdpAttribute RefTimeUpdateSdpAttributes[] = 
{
    /* Class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, RefTimeUpdateClassId),
    /* protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, RefTimeUpdatePrtclDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, TimePublicBrowseGroup),
};


/***********************************
 * Function Definitions 
 ***********************************/

/*****************************************************************************
 * FUNCTION
 *  
 * DESCRIPTION
 *  This function is used to add service record of time profile (BR/EDR)
 * PARAMETERS
 *  idx          [IN] index infomation
 *  capability   [IN] services list     
 * RETURNS
 *  U8 : success or fail
 *****************************************************************************/
U8 TimeSdpDB_AddRecord(U8 idx, U8 cap)
{
	BtStatus status;
	OS_Report("[Time] TimeSdpDB_AddRecord idx:%d cap:%d", idx, cap);
	
	if (0 != idx) {
		OS_Report("[Time] Error: invalid index");
		return BT_STATUS_FAILED;
	}
	
	// TODO: Update the start & end handle in sdp record !
	
	if (cap & 0x01) {
		CurTimeRecord.registered = TRUE;
		CurTimeRecord.record.num = sizeof(CurTimeSdpAttributes) / sizeof(*(CurTimeSdpAttributes));
		CurTimeRecord.record.attribs = (SdpAttribute *) & CurTimeSdpAttributes;
		status = SDP_AddRecord(&CurTimeRecord.record);
		OS_Report("[Time] Add CurTimeRecord to SDP, status: %d", status);
	}
	if (cap & 0x02) {
		NextDSTRecord.registered = TRUE;
		NextDSTRecord.record.num = sizeof(NextDSTSdpAttributes) / sizeof(*(NextDSTSdpAttributes));
		NextDSTRecord.record.attribs = (SdpAttribute *) & NextDSTSdpAttributes;
		status = SDP_AddRecord(&NextDSTRecord.record);
		OS_Report("[Time] Add NextDSTRecord to SDP, status: %d", status);
	}
	if (cap & 0x04) {
		RefTimeUpdateRecord.registered = TRUE;
		RefTimeUpdateRecord.record.num = sizeof(RefTimeUpdateSdpAttributes) / sizeof(*(RefTimeUpdateSdpAttributes));
		RefTimeUpdateRecord.record.attribs = (SdpAttribute *) & RefTimeUpdateSdpAttributes;
		status = SDP_AddRecord(&RefTimeUpdateRecord.record);
		OS_Report("[Time] Add RefTimeUpdateRecord to SDP, status: %d", status);
	}

	return BT_STATUS_SUCCESS;
}

U8 TimeSdpDB_RemoveRecord(U8 idx, U8 cap)
{
	BtStatus status;
	OS_Report("[Time] TimeSdpDB_RemoveRecord idx:%d cap:%d", idx, cap);	
	if (0 != idx) {
		OS_Report("[Time] Error: invalid index");
		return BT_STATUS_FAILED;
	}

	if ((TRUE == CurTimeRecord.registered) && (cap & 0x01)) {
		CurTimeRecord.registered = FALSE;
		status = SDP_RemoveRecord(&CurTimeRecord.record);
		OS_Report("[Time] Remove CurTimeRecord to SDP, status: %d", status);
	}
	if ((TRUE == NextDSTRecord.registered) && (cap & 0x01)) {
		NextDSTRecord.registered = FALSE;
		status = SDP_RemoveRecord(&NextDSTRecord.record);
		OS_Report("[Time] Remove NextDSTRecord to SDP, status: %d", status);
	}
	if ((TRUE == RefTimeUpdateRecord.registered) && (cap & 0x01)) {
		RefTimeUpdateRecord.registered = FALSE;
		status = SDP_RemoveRecord(&RefTimeUpdateRecord.record);
		OS_Report("[Time] Remove RefTimeUpdateRecord to SDP, status: %d", status);
	}

	return BT_STATUS_SUCCESS;
}
#endif
