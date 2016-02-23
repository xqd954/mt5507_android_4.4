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

#include "bt_feature.h"
#include "btconfig.h"
#include "sec_adp.h"
#include "l2cap.h"

BOOL l2cap_query_testing_state(void);

#ifdef __L2CAP_TESTER_
#include "conmgr.h"

#define L2CAP_TESTER_CONNECT_REQ 0x01
#define L2CAP_TESTER_PING_REQ 0x02
#define L2CAP_TESTER_INFO_REQ 0x03

typedef struct _BtL2capTesterContext
{
    L2capPsm Protocol;
    U8 cid;
    CmgrHandler cmgrHandler;
    U8 common_data[48];
    U8 pending_command;
    BtPacket btp;
    U16 info_type;
    BOOL testing_enabled;
    BtSecurityRecord secRecord;

#if L2CAP_NUM_GROUPS > 0
    CmgrHandler cmgrHandler1;
    CmgrHandler cmgrHandler2;
    L2capGroup tsgroup;
    U16 groupId;
#endif /* L2CAP_NUM_GROUPS > 0 */ 
} BtL2capTesterContext;

typedef struct _L2capConnectRequest
{
    U8 bd_addr[6];
} L2capConnectRequest;

typedef struct _L2capDisconnectRequest
{
    U8 cid;
} L2capDisconnectRequest;

typedef struct _L2capInfoRequest
{
    U8 bd_addr[6];
    U16 info_type;
} L2capInfoRequest;

typedef struct L2capEchoRequest
{
    U8 bd_addr[6];
} L2capEchoRequest;

typedef struct L2capConfigRequest
{
    U8 cid;
    U16 mtu;

} L2capConfigRequest;

typedef struct _L2capConnectResponse
{
    U8 bd_addr[6];
    U8 connect_result;
} L2capConnectResponse;

void l2capTesterInit(void);

U8 L2CA_ConnectReq(L2capConnectRequest *parms);
U8 L2CA_DisconnectReq(L2capDisconnectRequest *parms);
void L2CA_TesterDisconnect(U8 cid);
U8 L2CA_EchoReq(L2capEchoRequest *parms);
U8 L2CA_InfoReq(L2capInfoRequest *parms);
U8 L2capSendData(void);
BOOL l2capTester_get_bdaddr_from_string(U8 *bd_addr, char *string);

#if L2CAP_NUM_GROUPS > 0
typedef struct _L2capGroupAddr
{
    U8 bd_addr[6];
} L2capGroupAddr;

U8 L2CA_AddGroup(L2capGroupAddr *parms);
U8 L2CA_RemoveGroup(L2capGroupAddr *parms);
void L2CA_GroupDataReq(void);
#ifdef __BT_3_0_UNICAST__
void L2CA_UniCastDataReq(void);
#endif
#endif /* L2CAP_NUM_GROUPS > 0 */ 

#endif /* __L2CAP_TESTER_ */ 

