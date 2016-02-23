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

#include "btconfig.h"
#ifdef __AVCTP_TESTER_
#include "conmgr.h"
#include "avctp.h"

typedef struct _AvctpTesterChannel
{
    /* AVCTP Channel */
    AvctpChannel chnl;
    /* Connection Manager Handler */
    CmgrHandler cmgrHandler;
    U8 data[150];
    U8 data_offset;
    U8 current_operation;
    AvctpRawFrame raw_data;
    U8 special_state;
} AvctpTesterChannel;

typedef struct _AvctpConnectRequest
{
    U8 bd_addr[6];
} AvctpConnectRequest;

typedef struct _AvctpConnectResponse
{
    U8 bd_addr[6];
    U8 connect_result;
} AvctpConnectResponse;

typedef struct _AvctpDisconnectRequest
{
    U8 bd_addr[6];
    U16 pid;
} AvctpDisconnectRequest;

typedef struct _AvctpSendMessaegReq
{
    U8 bd_addr[6];
    U8 transaction_id;
    U8 type;
    U16 pid;
    U8 data[50];
    U16 length;
} AvctpSendMessaegReq;

extern void Send_AVCTP_Event_Init(U8 register_result);
extern void Send_AVCTP_Event_Registration_Req_Result(U8 register_result);
void AvctpTesterInit(void);
U8 AVCT_ConnectReq(AvctpConnectRequest *parms);
U8 AVCT_ConnectRsp(AvctpConnectResponse *parms);
U8 AVCT_DisconnectReq(AvctpDisconnectRequest *parms);

void AvctpTesterCallback(AvctpChannel *Chnl, AvctpCallbackParms *Parms);
void AvctpTesterCmgrCallback(CmgrHandler *cHandler, CmgrEvent Event, BtStatus Status);

#endif /* __AVCTP_TESTER_ */ 

