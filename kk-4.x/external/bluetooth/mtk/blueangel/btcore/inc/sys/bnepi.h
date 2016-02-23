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

#ifdef __BT_BNEP__

#ifndef __BNEPI_H
#define __BNEPI_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:bnepi.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the internal header file for the BNEP 
 *              protocol.
 *
 * Created:     Feb 28, 2001
 *
 * Copyright 2000-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/

#include <l2cap.h>
#include <bnep.h>

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

#define BNEP_MINIMUM_MTU    0x069B  /* 1691 bytes decimal */

/* BNEP Context */
typedef struct _BnepContext
{
    BnepPanu       *panu[NUM_BNEP_PANUS];
    L2capPsm        psm;                        /* L2CAP PSM */
    BnepCallback    callback;                   /* Application callback function */
    MeCommandToken  token;                      /* used to obtain local bdAddr */
    BNEP_ADDR       bnepAddr;                   /* Local device BD_ADDR */
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    BtSecurityRecord bnepSecRecord;
#endif

}   BnepContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern BnepContext *bnepcx;
#define BNEP(s)  (bnepcx->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BnepContext bnepcx;
#define BNEP(s)  (bnepcx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/*---------------------------------------------------------------------------
 * Packet Offsets
 *
 *      Element offsets of various Ethernet and BNEP packets
 */

/* EtherPacket */                                           
#define EtherPacket_hdr_dest_addr                           0x00
#define EtherPacket_hdr_source_addr                         0x06
#define EtherPacket_hdr_protocol                            0x0c
#define EtherPacket_data                                    0x0e

/* BnepGeneralHeader */
#define BnepGeneralHeader_type                              0x00
#define BnepGeneralHeader_etherHdr                          0x01
#define BnepGeneralHeader_etherHdr_dest_addr                0x01
#define BnepGeneralHeader_etherHdr_source_addr              0x07
#define BnepGeneralHeader_etherHdr_protocol                 0x0d

/* BnepGeneralPacket */
#define BnepGeneralPacket_type                              0x00
#define BnepGeneralPacket_etherPkt_hdr_dest_addr            0x01
#define BnepGeneralPacket_etherPkt_hdr_source_addr          0x07
#define BnepGeneralPacket_etherPkt_hdr_protocol             0x0d
#define BnepGeneralPacket_etherPkt_data                     0x0f

/* BnepCompressedPacket */
#define BnepCompressedPacket_type                           0x00
#define BnepCompressedPacket_etherPkt_compHdr_protocol      0x01
#define BnepCompressedPacket_etherPkt_data                  0x03

/* BnepCompSrcPacket */
#define BnepCompSrcPacket_type                              0x00
#define BnepCompSrcPacket_etherPkt_compSrcHdr_source_addr   0x01
#define BnepCompSrcPacket_etherPkt_compSrcHdr_protocol      0x07
#define BnepCompSrcPacket_etherPkt_data                     0x09

/* BnepCompDestPacket*/
#define BnepCompDestPacket_type                             0x00
#define BnepCompDestPacket_etherPkt_compDestHdr_dest_addr   0x01
#define BnepCompDestPacket_etherPkt_compDestHdr_protocol    0x07
#define BnepCompDestPacket_etherPkt_data                    0x09

/* BnepControlPkt */
#define BnepControlPkt_hdr_type                             0x00
#define BnepControlPkt_hdr_controlType                      0x01
#define BnepControlPkt_data_length                          0x02
#define BnepControlPkt_data_response                        0x02

/* BnepExtensionHeader */
#define BnepExtensionHeader_type                            0x00
#define BnepExtensionHeader_hdrDataLen                      0x01
#define BnepExtensionHeader_data                            0x02

/* BnepFilterExtHdr */
#define BnepFilterExtHdr_type                               0x00
#define BnepFilterExtHdr_hdrDataLen                         0x01
#define BnepFilterExtHdr_filterType                         0x02
#define BnepFilterExtHdr_range                              0x03

/* Function prototypes */
void L2BnepCallback(U16 ChannelId, L2capCallbackParms *Info);
void BnepAppCallBack(BnepCallbackParms *parms, U16 status, BnepEvent event);
BnepPanu *FindBnepPanu(U16 ChannelId);
BtStatus InitBnepPanu(BnepPanu *panu, U16 l2ChannelId, BtRemoteDevice *remDev);
void DeInitBnepPanu(BnepPanu *panu);
const char *pBNEP_Event(I8 Event);
BtStatus BnepSetTimer(BnepPanu *panu, BnepControlType type, BtPacket* btPacket);
void BnepStartTimer(BnepPanu *panu);
EvmTimer *BnepFindPanuTimer(BnepPanu *panu, BnepControlType type);
void BnepTimeoutHandler(EvmTimer *timer);
BtStatus ReadLocalBdAddr(void);
BNEP_ADDR BdToBnepAddr(BD_ADDR bdAddr);
BD_ADDR BnepToBdAddr(BNEP_ADDR bnepAddr);
void BnepMemMove(U8 *dest, U8 *src, U16 len);

//20110503 Paul
U8 BnepFindRestransmissionCtrlPktIdx(BnepPanu** panu, EvmTimer *timer);
#endif /* __BNEPI_H */

#endif /* __BT_BNEP__ */

