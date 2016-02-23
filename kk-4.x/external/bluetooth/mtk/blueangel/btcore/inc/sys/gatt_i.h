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

#ifndef __GATT_I_H
#define __GATT_I_H

#include "gatt.h"

#define MAX_GATT_CON_NO  NUM_BT_DEVICES


#define MAX_GATT_DISCONNECT_TIMEOUT_VALUE  20000 /* 20 seconds */


#define GATT_CONN_STATE_IDLE            0x00
#define GATT_CONN_STATE_CONNECTED       0x01

typedef struct _BtGattConn
{
    U8 state;
    BtRemoteDevice *link;
    BtAttEvent *attEvent;
    BtGattEvent gattEvent;
    ListEntry gattRequestOpList;       /* GATT Operation queue */
    ListEntry gattNotificationOpList;       /* GATT Operation queue */
    ListEntry gattIndicationOpList;       /* GATT Operation queue */
    BtOperation *gattRequestCurOp;     /* GATT Current operation */
    BtOperation *gattNotificationCurOp;     /* GATT Current operation */
    BtOperation *gattIndicationCurOp;     /* GATT Current operation */
    GattOpHandler gattRequestHandler;
    GattOpHandler gattNotificationHandler;
    GattOpHandler gattIndicationHandler;
    U16 att_mtu;
    BtGattOp  mtuOp;
    U32 disconnectTimerValue;
    EvmTimer disconnectTimer;    
} BtGattConn;


typedef struct _BtGattContext
{
    BtGattConn conn[MAX_GATT_CON_NO];
	ListEntry regList;
} BtGattContext;





#endif /* __ATT_I_H */

