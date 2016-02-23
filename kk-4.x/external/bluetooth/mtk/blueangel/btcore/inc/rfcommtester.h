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

BOOL RfcommTesterStateQueuy(void);

#include "conmgr.h"

typedef struct _BtRfcommTesterContext
{
    CmgrHandler cmgrHandler;
    SdpQueryToken sdpQueryToken;
    RfChannel channel;
    RfService service;
    U8 common_data[330];
    U8 pending_command;
    ListEntry rfcomm_pkt_List;
    BtPacket btp[10];
    U8 periodic_data_req_flag;
    BOOL testing_init;
    BOOL enable_change_pkt_type;
} BtRfcommTesterContext;

typedef struct _RfcommConnectRequest
{
    U8 bd_addr[6];
} RfcommConnectRequest;

typedef struct _RfcommDisconnectRequest
{
    U8 cid;
} RfcommDisconnectRequest;

extern BtRfcommTesterContext rfcommTesterContext;

#define RFCOMMTS(s) (rfcommTesterContext.s)

void rfcommTesterCallbackFunc(RfChannel *chan, RfCallbackParms *parms);
void RfcommTesterInit(void);
U8 RfcommTesterOpenClientChannel(RfcommConnectRequest *parms);
void RfcommTesterOpenServer(void);
void RfcommTesterDataReq(U8 periodic_data_req);
void RfcommTesterSendMSC(U8 on_off);
void RfcommTesterSendPN(void);
void RfcommTesterRls(void);
void RfcommTesterCloseChannel(void);
BtStatus rfcommTesterServiceQuery(void);
void rfcommTesterCmgrCallback(CmgrHandler *cHandler, CmgrEvent Event, BtStatus Status);
void rfcommTesterQueryCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn);
void rfcommtester_handler(ilm_struct *ilm_ptr);
void rfcomm_tester_handler_inject_msg(char *string);


