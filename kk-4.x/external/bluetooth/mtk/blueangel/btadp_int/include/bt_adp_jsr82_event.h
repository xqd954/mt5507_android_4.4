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

/* 
 * bt_adp_jsr82_event.h
 * 
 * This file is the header file of Internal Adaptation of JSR82 that it uses to send event to external ADP.
 * Flow direction: external ADP API <-- message queue <-- internal ADP <-- JSR82 API
 */

#ifndef __BT_ADP_JSR82_EVENT_H__
#define __BT_ADP_JSR82_EVENT_H__


void JSR82_sendMsg(
        msg_type msg,
        module_type dstMod,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff);

void bt_jsr82_SessionApEnableServiceCfn(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApTurnOnServiceCfn(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApTurnOffServiceCfn(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApDisableServiceCfn(JSR82CallbackParms *Parms);
/* Add index field: 2007-0916 */
void bt_jsr82_SessionApConnectInd(JSR82CallbackParms *Parms);
/* Add index field: 2007-0916 */
void bt_jsr82_SessionApConnectCfn(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApCloseInd(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApTxCfn(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApRxInd(JSR82CallbackParms *Parms);
void bt_jsr82_SessionPutBytesCNF(JSR82CallbackParms *Parms);
void bt_jsr82_SessionGetBytesCNF(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApRxReadyInd(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApTxReadyInd(JSR82CallbackParms *Parms);
void bt_jsr82_SetACLSecurityCnf(JSR82CallbackParms *Parms);
void bt_jsr82_SessionApDisconnectAllAclCfn(void);
void jsr82_session_sendData(JSR82CallbackParms *Parms);
void jsr82_spp_getData(JSR82CallbackParms *Parms);
void bt_jsr82_DataAvailableInd(JSR82CallbackParms *Parms);
void bt_jsr82_AssignBufferCnf(JSR82CallbackParms *Parms);
void btmtk_adp_jsr82_int_event_callback(JSR82CallbackParms *Parms);


#endif /* __BT_ADP_JSR82_EVENT_H__ */


