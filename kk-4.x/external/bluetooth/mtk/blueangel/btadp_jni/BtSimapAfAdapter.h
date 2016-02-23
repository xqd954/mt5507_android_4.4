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

#ifndef __BTSIMAPAFADAPTER_H__
#define __BTSIMAPAFADAPTER_H__

#pragma once


#define UNKNOWN_PROTOCOL_TYPE     -1    // sync this value with that in BtSimapOperResponse.java

// Copied from BtSimapRILAdapter.h
// 256 bytes for data + 2 bytes status word SW1 and SW2
#define SIM_RSP_BYTEARRAY_BUF_LEN  258


// ID from Lower Layer (native)
typedef enum {
    SIM1 = 1,
    SIM2 = 2,
    SIM3 = 3,
    SIM4 = 4
}BT_SIMAP_SIM_ID;

// ID for Upper Layer (AF Telephony)
typedef enum {
    AF_SIM1 = 0,
    AF_SIM2 = 1,
    AF_SIM3 = 2,
    AF_SIM4 = 3
}AF_SIM_ID;

typedef enum {
    SUCCESS                        = 0x00,
    ERR_NO_REASON_DEFINED,        // 0x01    // CME ERROR: 611 (AF Telephony)
    ERR_CARD_NOT_ACCESSIBLE,      // 0x02    // CME ERROR: 612 (AF Telephony)
    ERR_CARD_POWERED_OFF,         // 0x03
    ERR_CARD_REMOVED,             // 0x04    // CME ERROR: 613 (AF Telephony)
    ERR_CARD_POWERED_ON,          // 0x05    // already power on
    ERR_DATA_NOT_AVAILABLE,       // 0x06
    ERR_NOT_SUPPORTED,            // 0x07

    //-----------------------------------------
    // Possible +CME Errors that we have known
    //-----------------------------------------
    // RMMI_ERR_BT_SAP_UNDEFINED      = 611,
    // RMMI_ERR_BT_SAP_NOT_ACCESSIBLE = 612,
    // RMMI_ERR_BT_SAP_CARD_REMOVED   = 613
    
    ERR_NOT_INITED,
    ERR_OUT_OF_MEMORY,
    ERR_NOT_CONNECTED,
    ERR_ALREADY_CONNECTED,
    ERR_UNKNOWN
}BT_SIMAP_RESULT_CODE;


BT_SIMAP_RESULT_CODE BT_SIMAP_Connect_SIM(BT_SIMAP_SIM_ID simId,  int& curType, int& supportType, char ATR[], int& atrLen);
BT_SIMAP_RESULT_CODE BT_SIMAP_Disconnect_SIM(void);

BT_SIMAP_RESULT_CODE BT_SIMAP_APDU_req(int type, char cmdAPDU[], int cmdLen,  char rspAPDU[], int& rspLen);

BT_SIMAP_RESULT_CODE BT_SIMAP_Reset_SIM(int type,  int& curType, char ATR[], int& atrLen);

BT_SIMAP_RESULT_CODE BT_SIMAP_PowerOn_SIM(int type,  int& curType, char ATR[], int& atrLen);
BT_SIMAP_RESULT_CODE BT_SIMAP_PowerOff_SIM(void);

void dumpData(void *p, int size);

#endif