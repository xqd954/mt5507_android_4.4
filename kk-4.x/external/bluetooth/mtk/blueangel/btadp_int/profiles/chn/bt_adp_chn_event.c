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
 * bt_adp_bm.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions which provide the service to be the adaption layer 
 *   between JAVA and MMI
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "stdio.h"      /* Basic C I/O functions */
#include "bt_mmi.h"
#include "bt_types.h"
#include "bluetooth_struct.h"
#include "sdap.h"
#include "bluetooth_chn_message.h"
#include "bluetooth_chn_struct.h"
#include "chn.h"
#include "bt_adp_chn.h"
/***************
Internal Globals 
****************/

/***************
External Globals 
****************/

/***************************
Internal function prototypes
****************************/
/*********************
Function Definitions 
**********************/

/*****************************************************************************
 * FUNCTION
 *  bma_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void chn_app_callback(ChnChannel *Channel, ChnCallbackParms *Parms)
{
    ChnAdpChannel *adp_channel = ContainingRecord(Channel, ChnAdpChannel, channel);
    U16 len = sizeof(ChnCallbackParms);

    if (!IsNodeOnList(&g_chn_channel_list, &adp_channel->node))
    {
        return;
    }
    
    switch (Parms->event)
    {
    case CHN_EVENT_AUTH_REQ:
    {
        bt_chn_connect_req_ind_struct *ind_msg_p;
        ind_msg_p = (bt_chn_connect_req_ind_struct*) construct_local_para(sizeof(bt_chn_connect_req_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
		OS_MemCopy( (U8*)&ind_msg_p->bt_addr, (const U8*)&Parms->p.addr, sizeof(BD_ADDR));
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_CONNECT_REQ_IND, (local_para_struct*)ind_msg_p);
        break;
    }
	
    case CHN_EVENT_RFCOMM_CONNECTED:
        break;
	
    case CHN_EVENT_SERVICE_CONNECTED:
    {
        bt_chn_connected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_chn_connected_ind_struct*) construct_local_para(sizeof(bt_chn_connected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
		OS_MemCopy( (U8*)ind_msg_p->bt_addr, (const U8*)Parms->p.addr->addr, sizeof(BD_ADDR));
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_CONNECTED_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case CHN_EVENT_SERVICE_DISCONNECTED:
    {
        bt_chn_disconnected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_chn_disconnected_ind_struct*) construct_local_para(sizeof(bt_chn_disconnected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
	 OS_MemCopy( (U8*)&ind_msg_p->bt_addr, (const U8*)&Parms->p.addr, sizeof(BD_ADDR));
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_DISCONNECTED_IND, (local_para_struct*)ind_msg_p);
        break;
    }
    case CHN_EVENT_AUDIO_CONNECTED:
    {
        bt_chn_sco_connected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_chn_sco_connected_ind_struct*) construct_local_para(sizeof(bt_chn_sco_connected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->status = Parms->status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_SCO_CONNECTED_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case CHN_EVENT_AUDIO_DISCONNECTED:
    {
        bt_chn_sco_disconnected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_chn_sco_disconnected_ind_struct*) construct_local_para(sizeof(bt_chn_sco_disconnected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_SCO_DISCONNECTED_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case CHN_EVENT_RX_DATA:
    {
        bt_chn_rx_data_ind_struct *ind_msg_p;
        len = Parms->p.rxBuf->len;
        ind_msg_p = (bt_chn_rx_data_ind_struct*) construct_local_para(sizeof(bt_chn_rx_data_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
	 ind_msg_p->size = len;
        OS_MemCopy((U8*)ind_msg_p->data, (const U8*)Parms->p.rxBuf->buf, len);
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_RX_DATA_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    default:
        Report(("chn_app_callback: Unsupported event."));
        break;
    }
}


