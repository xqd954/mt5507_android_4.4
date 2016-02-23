/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_htp_common.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time Profile Common Function 
 *   Definition for Internal Adaptation Layer.
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __BT_ADP_TIME_H__
#define __BT_ADP_TIME_H__

void time_send_msg(msg_type msg_id, local_para_struct *local_para, peer_buff_struct *peer_buff);

#endif
