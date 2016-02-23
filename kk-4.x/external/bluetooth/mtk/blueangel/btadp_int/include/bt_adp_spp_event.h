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
 * bt_adp_spp_event.h
 * 
 * This file is the header file of Internal Adaptation of SPP profile that profile uses to send event to external ADP.
 * Flow direction: external ADP API <-- message queue <-- internal ADP <-- profile API
 */

#ifndef __BT_ADP_SPP_EVENT_H__
#define __BT_ADP_SPP_EVENT_H__


#include "spp.h"



void SPPOS_sendMsg(msg_type msg, module_type dstMod, sap_type sap, local_para_struct *local_para, peer_buff_struct *peer_buff);
void SPPAdpConnectInd(SPPCallbackParms *Parms);
void SPPAdpConnectIndReq(SPPCallbackParms *Parms);
void SPPAdpDisconnectInd(SPPCallbackParms *Parms);
void SPPAdpActivateCnf(SPPCallbackParms *Parms);
void SPPAdpDeactivateCnf(SPPCallbackParms *Parms);
void SPPAdpDisconnectCnf(SPPCallbackParms *Parms);
void SPPAdpAuthReq(SPPCallbackParms *Parms);
void SPPAdpAudioConnectCnf(SPPCallbackParms *Parms);
void SPPAdpAudioConnectInd(SPPCallbackParms *Parms);
void SPPAdpAudioDisconnectCnf(SPPCallbackParms *Parms);
void SPPAdpAudioDisconnectInd(SPPCallbackParms *Parms);
void SPPAdpScoRssiInd(SPPCallbackParms *Parms);
void SPPAdpConnectCnf(SPPCallbackParms *Parms);
void SPPAdpUartOwnerInd(SPPCallbackParms *Parms);
void DUNAdpConnectInd(SPPCallbackParms *Parms);
void DUNAdpConnectIndReq(SPPCallbackParms *Parms);
void DUNAdpDisconnectInd(SPPCallbackParms *Parms);
void DUNAdpActivateCnf(SPPCallbackParms *Parms);
void DUNAdpDeactivateCnf(SPPCallbackParms *Parms);
void DUNAdpDisconnectCnf(SPPCallbackParms *Parms);
void DUNAdpConnectCnf(SPPCallbackParms *Parms);
void FAXAdpConnectInd(SPPCallbackParms *Parms);
void FAXAdpConnectIndReq(SPPCallbackParms *Parms);
void FAXAdpDisconnectInd(SPPCallbackParms *Parms);
void FAXAdpActivateCnf(SPPCallbackParms *Parms);
void FAXAdpDeactivateCnf(SPPCallbackParms *Parms);
void FAXAdpDisconnectCnf(SPPCallbackParms *Parms);
void FAXAdpConnectCnf(SPPCallbackParms *Parms);
void UARTAdpPlugoutInd(SPPCallbackParms *Parms);
void UARTAdpReadyToReadInd(SPPCallbackParms *Parms);
void UARTAdpReadyToWriteInd(SPPCallbackParms *Parms);
void SPPAdpInitializeCnf(SPPCallbackParms *Parms);
void SPPUARTPutBytesCnf(SPPCallbackParms *Parms);
void SPPUARTGetBytesCnfSend(SPPCallbackParms *Parms, U16 lengthSent, U16 lengthToSend);
void SPPUARTGetBytesCnf(SPPCallbackParms *Parms);
void SPPUARTOpenCnf(SPPCallbackParms *Parms);
void SPPUARTCloseCnf(SPPCallbackParms *Parms);
void SPPUARTGetBytesAvailCnf(SPPCallbackParms *Parms);
void SPPUARTTxRoomLeftCnf(SPPCallbackParms *Parms);
void SPPUARTSetOwnerCnf(SPPCallbackParms *Parms);
void SPPUARTCtrlDcdCnf(SPPCallbackParms *Parms);
void SPPUARTCtrlBreakCnf(SPPCallbackParms *Parms);
void SPPUARTGetOwnerIDCnf(SPPCallbackParms *Parms);
void SPPUARTClrRxBufferCnf(SPPCallbackParms *Parms);
void SPPUARTClrTxBufferCnf(SPPCallbackParms *Parms);
void SPPUARTDataAvailableIndSend(SPPCallbackParms *Parms, U16 lengthSent, U16 lengthToSend);
void SPPUARTDataAvailableInd(SPPCallbackParms *Parms);
void btmtk_adp_spp_int_event_callback(SPPCallbackParms *Parms);


#endif /* __BT_ADP_SPP_EVENT_H__ */

