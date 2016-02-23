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

#ifndef __DEBUG_H
#define __DEBUG_H
/***************************************************************************
 *
 * File:
 *     $Workfile:debug.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains prototypes for debug message functions.
 *
 * Created:
 *     Aug 13, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * Debug functions for converting protocol and status types to strings.
 *
 * NOTE: Do not include this file in a header file. It should only be included
 *       in source code files and should be the last file in the include list.
 *
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * SniffProtocolId type
 *
 *     This type is used in ChannelInfo to identify the valid protocol id's.
 */
typedef U16 SniffProtocolId;

#define SPI_LM          0x0001  /* LMP Connection Handle */
#define SPI_L2CAP       0x0002  /* L2CAP Protocol */
#define SPI_SDP         0x0004  /* SDP Protocol */
#define SPI_RFCOMM      0x0008  /* RFCOMM Channel protocol */
#define SPI_OBEX        0x0010  /* OBEX Protocol */
#define SPI_TCS         0x0020  /* TCS Binary protocol */
#define SPI_DATA        0x0040  /* Application data */
#define SPI_L2CAP_FEC   0x0080  /* L2CAP Protocol in Flow Control mode */

/* End of SniffProtocolId */

#if XA_SNIFFER == XA_ENABLED && HCI_ALLOW_PRESCAN == XA_ENABLED

/*---------------------------------------------------------------------------
 * SnifferRegisterEndpoint()
 *
 *     This function is called by protocol implementations to register a
 *     new connection in the sniffer connection table. The registering
 *     protocol provides information about the local endpoint, remote
 *     device and the protocol that is running over that endpoint.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     EndpointType - Protocol type of 'Endpoint'. Must be one of SPI_L2CAP
 *         or SPI_RFCOMM.
 *
 *     Endpoint - Identifier for the endpoint. Must be a pointer to an
 *         L2CAP L2capChannelId, or RFCOMM RfChannel->dlci.
 *
 *     RemDev - BtRemoteDevice pointer.
 *
 *     Protocol - Protocol layered above 'Endpoint'.
 */
void SnifferRegisterEndpoint(
        SniffProtocolId EndpointType,
        void *Endpoint,
        BtRemoteDevice *RemDev,
        SniffProtocolId Protocol);
#else /* XA_SNIFFER == XA_ENABLED && HCI_ALLOW_PRESCAN == XA_ENABLED */ 
#define SnifferRegisterEndpoint(A,B,C,D)   (void)0
#endif /* XA_SNIFFER == XA_ENABLED && HCI_ALLOW_PRESCAN == XA_ENABLED */ 

/*
 *---------------------------------------------------------------------------
 *            General Purpose Debug functions
 *---------------------------------------------------------------------------
 */
const char *pBT_Status(I8 Status);
const char *pME_Event(I8 event);
const char *pHC_Status(U8 Status);
const char *pHciEvent(U8 Event);
const char *pHciCommand(U16 Cmd);

/*
 *---------------------------------------------------------------------------
 *            L2CAP Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__L2CAP_H)
const char *pL2CAP_Event(I8 Event);
const char *pL2CAP_DiscReason(U16 Reason);
const char *pL2CAP_ConnStatus(U16 Status);
const char *pLLC_Event(I8 Event);
const char *pL2CAP_SignalOpcode(U8 Opcode);
#endif /* defined(__L2CAP_H) */ 

/*
 *---------------------------------------------------------------------------
 *            HCI Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__HCITRANS_H)
#endif 

/*
 *---------------------------------------------------------------------------
 *            RFCOMM Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__RFCOMM_H)
#endif 

/*
 *---------------------------------------------------------------------------
 *            SDP Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__SDPH)
const char *pSdpService(U32 Service);
const char *pCodMajorClass(BtClassOfDevice Cod);
U16 BtCod2ServiceString(BtClassOfDevice Cod, char *Result, U16 Max);
#endif /* defined(__SDPH) */ 

/*
 *---------------------------------------------------------------------------
 *            ME Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__ME_H)
#endif 

/*
 *---------------------------------------------------------------------------
 *            TCS Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__TCS_H)
const char *pTCS_Event(TcsEvent tcs_event);
const char *pTCS_CallState(TcsCallState State);
const char *pTCS_Message(TcsMsgType msg);
#endif /* defined(__TCS_H) */ 

#endif /* __DEBUG_H */ 

