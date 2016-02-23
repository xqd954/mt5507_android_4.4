/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_adp_pan_ip.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is platform based implementation. It is responsible for network packet transmission. 
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_PAN_PROFILE__
#include <errno.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/in.h>

#include "bttypes.h"
#include "btnet.h"
#include "bt_adp_pan_ip.h"


/*
 * pan_ip_open_network_dev 
 * 
 * [in] none
 * [out] hw_addr
 */
S32 pan_ip_open_network_dev(U8 *hw_addr, U8 *if_name)
{
    S32 result;

    result =  openBtNet(hw_addr, if_name);

    return result;
}

void pan_ip_close_network_dev()
{
    closeBtNet();
}

void pan_ip_send_packet(U8 *data, U16 len)
{
    S32 btnfd = getBtNetHandle();
    S16 numWrite;
    
    if (btnfd >= 0)
    {
        numWrite = write(btnfd, data, len);
        bt_prompt_trace(MOD_BT, "pan ip write packet len: %d", numWrite);
        if (numWrite < 0) 
        {
            // error message
            bt_prompt_trace(MOD_BT, "write error: %s (%d)", strerror(errno), errno);
        }   
    }
}

S16 pan_ip_receive_packet(U8 *buffer)
{
    S32 btnfd = getBtNetHandle();
    S16 numRead;

    numRead = read(btnfd, buffer, PAN_MRU);
    bt_prompt_trace(MOD_BT, "pan ip read packet len: %d", numRead);
    bt_prompt_trace(MOD_BT, "pan ip read packet data: %x, %x, %x", buffer[0], buffer[1], buffer[2]);


    if (numRead < 0)
    {
        // error message
        bt_prompt_trace(MOD_BT, "read error: %s (%d)", strerror(errno), errno);
    }
    return numRead;
}
#endif
