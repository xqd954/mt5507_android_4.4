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
 * lei.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy internal header
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef __LEI_H
#define __LEI_H

#include "utils.h"
#include "hci.h"
#include "metypes.h"

#define LE_HCI_CMD_LIST_SIZE 32
#define LE_STACK_POOL_SIZE 10

#define LE_ADV_DATA_SIZE 31
//#define LE_DUAL_INQUIRY_INTERVAL (10.24 / 2 * 1000)   /* ms */

typedef enum
{
    LE_ADV_CHANNEL_37 = 0x01,
    LE_ADV_CHANNEL_38 = 0x02,
    LE_ADV_CHANNEL_39 = 0x04,
    LE_ADV_CHANNEL_ALL = 0x07
} LeAdvChannelType;

typedef enum
{
    HCI_ADV_IND = 0x00,
    HCI_ADV_DIRECT_IND = 0x01,
    HCI_ADV_SCAN_IND = 0x02,
    HCI_ADV_NONCONN_IND = 0x03,

    HCI_SCAN_RSP = 0x04
    /* MTK: 0xFF is used to denote NOT ACCESSIBLE case */
} HciLeAdvertiseType;

typedef enum
{
    HCI_SCAN_PASSIVE = 0x00,
    HCI_SCAN_ACTIVE = 0x01
} HciLeScanType;

typedef enum
{
    ADV_ALLOW_ALL = 0x00,
    ADV_ALLOW_SCAN_WL = 0x01,          /* Allow scan from WL & connect from all */
    ADV_ALLOW_CONNECT_WL = 0x02,       /* Allow scan from all & connect from WL */
    ADV_ALLOW_SCAN_CONNECT_WL = 0x03,  /* Allow scan from WL & connect from WL */
} HciLeAdvertiseFilter;

typedef enum
{
    SCAN_ALLOW_ALL = 0x00,
    SCAN_ALLOW_WL_ONLY = 0x01,          /* Allow scan from WL */
} HciLeScanFilter;

typedef enum
{
    INITIAOR_NOT_USE_WL = 0x00,          /* Allow scan from WL */
    INITIAOR_USE_WL = 0x01,
} HciLeInitiatorFilter;

typedef enum
{
    LE_ADDR_PUBLIC,                        /* 0x00 */
    LE_ADDR_RANDOM                         /* 0x01 */
} LeAddrType;

typedef enum
{
    LE_CONN_DEV_REMOVE_REASON_TIMEOUT      = 0,
    LE_CONN_DEV_REMOVE_REASON_CONNECTED    = 1,     
} LeConnDevRemoveReason;

typedef enum
{
    LE_CONN_DEV_EVT_CHANGED                = 0,
    LE_CONN_DEV_EVT_WL_CLEARED             = 1,  
    LE_CONN_DEV_EVT_WL_ADDED               = 2,    
    LE_CONN_DEV_EVT_CONNECT_DONE           = 3,       
} LeConnDevEvt;

typedef enum
{
    LE_CONN_DEV_STATE_IDLE                 = 0,
    LE_CONN_DEV_STATE_CLEAR_WL             = 1,     
    LE_CONN_DEV_STATE_ADD_WL               = 2,    
    LE_CONN_DEV_STATE_CONNECT              = 3,
    LE_CONN_DEV_STATE_CONNECT_CANCEL       = 4,
} LeConnDevState;

typedef struct
{
    LeAddrType addrType;
    BD_ADDR    bdAddr; 
    BOOL       inWhiteList;
    BOOL       inUse;
} LeConnDev;

typedef struct 
{
    BtOperation op;  /* Chain to MEC(opList) */

    /* HCI command */
    HciCommandType cmd;
    U8 parmLen;                  /* command parm length and return event parm length */
    U8 parms[HCI_CMD_PARM_LEN];  /* command parm and return event parm */
    U8 cmdToHci;

    union
    {
        BOOL enable;  /* HCC_LE_SET_SCAN_ENABLE: need to remember parm due to same command but different event handling */
    } cmdParm;

    union
    {
        struct _BtRemoteDevice *remDev;  /* use link handler */
        BtCallBack callback;
    } handler;
} LeHciCmd;
/*---------------------------------------------------------------------------
 * Le Context Structure
 *
 *   This structure provides the context and state information for the entire SM module
 */
typedef struct _BtLeContext
{
    HciLeAdvertiseType advCur;
    U16 interval;
    U16 window;
    U8 type;
    U8 ownAddrType;
    U8 filter;
    BOOL enabled;
    U8 setScanParameterFlag;
    U8 setEnabledFlag;
    U8 currentEnabledState;
    LeHciCmd hciScanParmCmd;
    LeHciCmd hciScanEnabledCmd;
    U8 advData[LE_ADV_DATA_SIZE];
    U8 advDataLen;

    LeHciCmd hciCmd[LE_HCI_CMD_LIST_SIZE];
    U8 curCmdIdx;

    LeConnDev connDevList[NUM_BT_DEVICES];
    LeConnDevState connDevState;    
    U8 connDevSize;  
    BOOL clear;
    struct _SmContext *smCtx;

#ifdef __BT_4_0_PRIVACY__
    BD_ADDR random_bdaddr;

    union
    {
        struct
        {
            U8 prand[3];   /* intermediate data */
            BtCallBack cb; /* intermediate data */
        } genRandomAddr;
        struct
        {
            U8 recIndex;   /* intermediate data */
            BD_ADDR resAddr[BD_ADDR_SIZE];   /* intermediate data */
            BtCallBack cb; /* intermediate data */
        } resolveRandomAddr;
    }op;
#endif
} BtLeContext;

void SM_LinkCallback(struct _BtRemoteDevice *remDev, struct _BtEvent *event);

#endif /* __LEI_H */
