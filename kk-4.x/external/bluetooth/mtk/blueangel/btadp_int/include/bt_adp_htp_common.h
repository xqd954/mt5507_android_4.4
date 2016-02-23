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
 *   This file is for MTK Bluetooth Health Thermometer Profile Common Function 
 *   Definition for Internal Adaptation Layer.
 *
 * Author:
 * -------
 * Ting Zheng
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
#ifndef __BT_ADP_HTP_COMMON_H__
#define __BT_ADP_HTP_COMMON_H__


void htp_send_msg(msg_type msg, module_type dstMod, local_para_struct *local_para);
void htp_convert_float_to_IEEE(float in_value, HtpTemperatureValue *out_value);
void htp_convert_IEEE_to_float(HtpTemperatureValue *in_value, float *out_value);


#endif
