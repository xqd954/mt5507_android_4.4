/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2005
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * timeattdb.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time profile
 *
 * Author:
 * -------
 * Jacob Lee
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 11 04 2011 dlight.ting
 * NULL
 * .
 *
 * 10 31 2011 dlight.ting
 * NULL
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_TIMES_PROFILE__
#include "attdb.h"
#include "attdef.h"
#include "attdbmain.h"
#include "ctsservice.h"
#include "ndcsservice.h"
#include "rtusservice.h"

extern U8 time_att_db_cttime_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 time_att_db_cttime_config_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 time_att_db_local_time_info_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 time_att_db_ref_time_info_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 time_att_db_time_with_dst_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 time_att_db_update_ctrl_point_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 time_att_db_update_state_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);


BtCtsServiceCbHandler CtsCttimeHandle;
BtCtsServiceCbHandler CtsCttimeConfigHandle;
BtCtsServiceCbHandler CtsLocaltimeInfoHandle;
BtCtsServiceCbHandler CtsReftimeInfoHandle;

BtNdcsServiceCbHandler NdcsWithDstHandle;

BtRtusServiceCbHandler RtusUpdateControlPointHandle;
BtRtusServiceCbHandler RtusUpdateStateHandle;


U8 TimeAttDB_AddRecord(void) {
	BtStatus status;

    CtsActivateService();

    InitializeListEntry(&(CtsCttimeHandle.node));
    InitializeListEntry(&(CtsCttimeConfigHandle.node));
    InitializeListEntry(&(CtsLocaltimeInfoHandle.node));
    InitializeListEntry(&(CtsReftimeInfoHandle.node)); 

    InitializeListEntry(&(NdcsWithDstHandle.node)); 

    InitializeListEntry(&(RtusUpdateControlPointHandle.node));
    InitializeListEntry(&(RtusUpdateStateHandle.node)); 

    CtsCttimeHandle.callback = time_att_db_cttime_callback;
    CtsRegisterAttdbCallback(ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE,
             &(CtsCttimeHandle));

    CtsCttimeConfigHandle.callback = time_att_db_cttime_config_callback;
    CtsRegisterAttdbCallback(ATT_HANDLE_CLIENT_CHARACTERISTIC_CONFIG_CT_TIME_VALUE,
             &(CtsCttimeConfigHandle));

    CtsLocaltimeInfoHandle.callback = time_att_db_local_time_info_callback;
    CtsRegisterAttdbCallback(ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO_VALUE,
             &(CtsLocaltimeInfoHandle));

    CtsReftimeInfoHandle.callback = time_att_db_ref_time_info_callback;
    CtsRegisterAttdbCallback(ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO_VALUE,
             &(CtsReftimeInfoHandle));    


    NdcsActivateService();
    NdcsWithDstHandle.callback = time_att_db_time_with_dst_callback;
    NdcsRegisterAttdbCallback(ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST_VALUE,
             &(NdcsWithDstHandle));    
    
    RtusActivateService();
    RtusUpdateControlPointHandle.callback = time_att_db_update_ctrl_point_callback;
    RtusRegisterAttdbCallback(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_VALUE,
             &(RtusUpdateControlPointHandle));    
    
    RtusUpdateStateHandle.callback = time_att_db_update_state_callback;
    RtusRegisterAttdbCallback(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE_VALUE,
             &(RtusUpdateStateHandle));    
	return (U8) status;
}

U8 TimeAttDB_RemoveRecord(void) {

    CtsDeregisterAttdbCallback(&(CtsCttimeHandle));
    CtsDeregisterAttdbCallback(&(CtsCttimeConfigHandle));
    CtsDeregisterAttdbCallback(&(CtsLocaltimeInfoHandle));
    CtsDeregisterAttdbCallback(&(CtsReftimeInfoHandle));    
    CtsDeactivateService();

    NdcsDeregisterAttdbCallback(&(NdcsWithDstHandle));    
    NdcsDeactivateService();
    
    RtusDeregisterAttdbCallback(&(RtusUpdateControlPointHandle));
    RtusDeregisterAttdbCallback(&(RtusUpdateStateHandle));    
    RtusDeactivateService();


	return (U8) BT_STATUS_SUCCESS;
}
#endif
