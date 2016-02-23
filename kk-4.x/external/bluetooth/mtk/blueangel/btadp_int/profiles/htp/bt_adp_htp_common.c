/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_htp_common.c
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
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)

#include "bt_os_api.h"
#include "htp.h"

#include "bluetooth_htp_message.h"

#define HTP_POSITIVE_INFINITY	0x007ffffe
#define HTP_NEGATIVE_INFINITY	0xff800002


void btmtk_adp_htpc_handle_message(void *msg);
void btmtk_adp_htpt_handle_message(void *msg);

void btmtk_adp_htp_handle_message(void *msg)
{
    ilm_struct *msg_p = (ilm_struct *)msg;

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_INTADP_HANDLE_MESSAGE, msg_p->msg_id);

    if ((msg_p->msg_id >= MSG_ID_BT_HTPC_GROUP_START) && (msg_p->msg_id <= MSG_ID_BT_HTPC_GROUP_END))
    {
        btmtk_adp_htpc_handle_message(msg_p);
    }
    else if ((msg_p->msg_id >= MSG_ID_BT_HTPT_GROUP_START) && (msg_p->msg_id <= MSG_ID_BT_HTPT_GROUP_END))
    {
        btmtk_adp_htpt_handle_message(msg_p);
    }
}

void htp_send_msg(
        msg_type msg,
        module_type dstMod,
        local_para_struct *local_para)
{
    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

void htp_convert_float_to_IEEE(float in_value, HtpTemperatureValue *out_value)
{
    S8 exponent = 0;
    const float EPSILON = 0.000001;
    float temp;

    btmtk_os_memset((U8 *)out_value, 0, sizeof(HtpTemperatureValue));

    if (in_value > EPSILON)	// positive
    {
        while (((int)in_value < in_value) && ((temp = in_value * 10) <= HTP_POSITIVE_INFINITY))
        {
            in_value = temp;
            exponent++;
        }
        in_value += 0.5;
    }
    else if (in_value < - EPSILON)  // negative
    {
        while (((int)in_value > in_value) && ((int)(temp = in_value * 10) >= HTP_NEGATIVE_INFINITY))
        {
            in_value = temp;
            exponent++;
        }
        in_value -= 0.5;
    }
    else
    {
        // zero, do nothing here
    }

    exponent = ~exponent + 1;
	
    out_value->exponent = exponent;
    out_value->mantissa[2] = ((int)in_value & 0xff0000) >> 16;
    out_value->mantissa[1] = ((int)in_value & 0xff00) >> 8;
    out_value->mantissa[0] = (int)in_value & 0xff;
}

void htp_convert_IEEE_to_float(HtpTemperatureValue *in_value, float *out_value)
{
    U8 exponent;
    U32 temp;

    exponent = ~(in_value->exponent - 1);
	
    temp = ((in_value->mantissa[2] << 16) & 0xff0000) |
		((in_value->mantissa[1] << 8) & 0xff00) |
		(in_value->mantissa[0] & 0xff);


    if ((temp & 0x800000) == 0)
    {
        *out_value = temp;         // positive or zero
    }
    else
    {
        *out_value = ~((temp | 0xff000000) - 1);    // negative
    }

    while (exponent > 0)
    {
        *out_value = *out_value/10;
        exponent--;		
    }
    
    if ((temp & 0x800000) != 0)
    {
        // negative
        *out_value = ~((int)*out_value) + 1 - (*out_value - (int)*out_value);
    }
}

#endif
