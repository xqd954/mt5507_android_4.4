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
/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_gap_api.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Dlight Ting
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
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include <windows.h>
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
#include "string.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_chn_message.h"
#include "bluetooth_chn_struct.h"
#include "bt_chn_struct.h"

#define InitLock()
#define DeinitLock()	
#define Lock()
#define Unlock()


/*******************************************************************
*   Exported functions
*******************************************************************/
BtStatus btmtk_chn_register(ChnChannelContext *Channel, BTMTK_EventCallBack Callback, U16 svc, U16 remote_svc)
{
    BtStatus status;
    bt_chn_activate_req_struct msg;    
    Channel->callback = Callback;
    InitLock();
    msg.req_context = (void*)Channel;
    msg.svc = svc;
    msg.remote_svc = remote_svc;
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_ACTIVATE_REQ, MOD_BT, &msg, sizeof(msg));
    
    if(status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
    {
        DeinitLock();
    }
    return status;
}

BtStatus btmtk_chn_deregister(ChnChannelContext *Channel)
{
    BtStatus status;
    bt_chn_deactivate_req_struct msg;

    Lock();
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_DEACTIVATE_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();

    if(status != BT_STATUS_PENDING)
        Unlock();
    return status;
}

BtStatus btmtk_chn_create_service_link(ChnChannelContext *Channel, U8 *Addr)
{
    BtStatus status;
    bt_chn_connect_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    memcpy(&msg.bt_addr, Addr, sizeof(msg.bt_addr));
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_CONNECT_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}

BtStatus btmtk_chn_disconnect_service_link(ChnChannelContext *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    bt_chn_disconnect_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_DISCONNECT_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}

BtStatus btmtk_chn_create_audio_link(ChnChannelContext *Channel)
{
    BtStatus status;
    bt_chn_sco_connect_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    /* Wait confirm */

    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_SCO_CONNECT_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}

BtStatus btmtk_chn_disconnect_audio_link(ChnChannelContext *Channel)
{
    BtStatus status;
    bt_chn_sco_disconnect_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;

    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_SCO_DISCONNECT_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}

BtStatus btmtk_chn_accept_connect(ChnChannelContext *Channel)
{
    BtStatus status;
    bt_chn_accept_channel_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_ACCEPT_CHANNEL_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}

BtStatus btmtk_chn_reject_connect(ChnChannelContext *Channel)
{
    BtStatus status;
    bt_chn_reject_channel_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    /* Wait confirm */
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_REJECT_CHANNEL_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}

BtStatus btmtk_chn_send_data(ChnChannelContext *Channel, U8* buf, U16 len)
{
    BtStatus status;
    bt_chn_send_data_req_struct msg;

    Lock();
    /* Fill msg parameters */
    msg.pContext = Channel->chnContext;
    msg.req_context = Channel;
    msg.size = len;
    memcpy(msg.data, buf, len);

    /* Wait confirm */
    status = BT_STATUS_PENDING;
    BTCMD_SendMessage(MSG_ID_BT_CHN_SEND_DATA_REQ, MOD_BT, &msg, sizeof(msg));
    Unlock();
    return status;
}
