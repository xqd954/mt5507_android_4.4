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
 * attsdpdb.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Attribute protocol definition
 *
 * Author:
 * -------
 * Autumn Li (mtk01174)
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
#include "bt_common.h"
#include "stdio.h"
#include "sdp.h"
#include "l2cap_adp.h"
#include "attdb.h"
#include "attdef.h"


typedef struct _AttRecords
{
    BOOL registered;
    SdpRecord record;   /* SDP record */
} AttRecords;

AttRecords BT_AttRecord;


/****************************************************************************
 * SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP ServiceClassIDList
 */
static const U8 AttSrcClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),                  /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(BT_UUID_GATT_SERVICE_GAP)   /* Human Interface Device */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by ATT.
 * 
 * Value of the protocol descriptor list for the ATT Profile.
 * This structure is a ROM'able representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service.
 */
static const U8 AttProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(19), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for L2CAP, 6 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),         /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),         /* ATT PSM */

    /*
     * Next protocol descriptor in the list is ATT.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for ATT, 2 bytes */
    SDP_UUID_16BIT(PROT_ATT),    /* ATT UUID */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_GAP),        /* GAP start attribute handle */
    SDP_UINT_16BIT(ATT_HANDLE_PRIMARY_SERVICE_GAP_END)    /* GAP end attribute handle */
};

/*---------------------------------------------------------------------------
 * SDP Public Browse Group.
 */
static const U8 AttBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/*---------------------------------------------------------------------------
 * ATT attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * ATT SDP record.
 */
SdpAttribute AttSdpAttributes[] = 
{
    /* Mandatory SDP Attributes */
    /* HID class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AttSrcClassId),
    /* HID protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AttProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, AttBrowseGroup),
};


/*****************************************************************************
 * FUNCTION
 *  AttDeviceSdpDB_Registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 AttDeviceSdpDB_Registration(void)
{
    BtStatus status;

    Report(("[ATT] AttDeviceSdpDB_Registration()"));
    BT_AttRecord.registered = TRUE;
    BT_AttRecord.record.num = sizeof(AttSdpAttributes) / sizeof(*(AttSdpAttributes));
    /* Retrieve the server channel from the service channel */
    BT_AttRecord.record.attribs = (SdpAttribute*) & AttSdpAttributes;
    status = SDP_AddRecord(&BT_AttRecord.record);
    Assert((status != BT_STATUS_INVALID_PARM));
    kal_trace(TRACE_GROUP_1, SDP_REG_RESULTx02X, status);
    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  AttDeviceSdpDB_DeRegistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 AttDeviceSdpDB_DeRegistration(void)
{
    BtStatus status;

    status = SDP_RemoveRecord(&BT_AttRecord.record);
    return (U8)status;
}


