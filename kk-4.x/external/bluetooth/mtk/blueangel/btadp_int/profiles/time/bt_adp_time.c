
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
 * bt_adp_time.c
 *
 * Project:
 * --------
 * BT Project
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time Profile
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "bttypes.h"
#include "bt_common.h"

#include "bluetooth_struct.h"
#include "bluetooth_time_struct.h"

#ifdef __BT_TIMES_PROFILE__
extern void times_adp_init();
extern void btmtk_adp_times_handle_message(ilm_struct *ilm_ptr);
#endif

#ifdef __BT_TIMEC_PROFILE__
extern void timec_adp_init();
extern void btmtk_adp_timec_handle_message(ilm_struct *ilm_ptr);
#endif

#if defined(__BT_TIMES_PROFILE__) || defined(__BT_TIMEC_PROFILE__)
/* Send ILM message */
void time_send_msg(msg_type msg_id, local_para_struct *local_para,
		peer_buff_struct *peer_buff) {
	module_type dst_id = MOD_MMI;

	if (local_para != NULL) {
		BT_SendMessage(msg_id, dst_id, local_para, local_para->msg_len);
	} else {
		BT_SendMessage(msg_id, dst_id, NULL, 0);
	}
}

/********************************************************************************************
 * Entry function for message handlers
 ********************************************************************************************/
void btmtk_adp_time_handle_message(ilm_struct *ilm_ptr) {
       #ifdef __BT_TIMES_PROFILE__
	if ((ilm_ptr->msg_id >= MSG_ID_BT_TIMES_GROUP_START) &&
		(ilm_ptr->msg_id <= MSG_ID_BT_TIMES_GROUP_END)) {
		btmtk_adp_times_handle_message(ilm_ptr);
	} else 
	#endif
       #ifdef __BT_TIMEC_PROFILE__ 
	if ((ilm_ptr->msg_id >= MSG_ID_BT_TIMEC_GROUP_START) &&
			   (ilm_ptr->msg_id <= MSG_ID_BT_TIMEC_GROUP_END)) {
		btmtk_adp_timec_handle_message(ilm_ptr);
	} else 
	#endif
	{
		OS_Report("[Time] Invalid msg_id: %d", ilm_ptr->msg_id);
	}
}
#endif

void time_adp_init() {
	U8 i = 0;

#if defined(__BT_TIMES_PROFILE__) || defined(__BT_TIMEC_PROFILE__)
	// time_gatt_init();
#endif

#ifdef __BT_TIMES_PROFILE__
	times_adp_init();
#endif

#ifdef __BT_TIMEC_PROFILE__
	timec_adp_init();
#endif

}
