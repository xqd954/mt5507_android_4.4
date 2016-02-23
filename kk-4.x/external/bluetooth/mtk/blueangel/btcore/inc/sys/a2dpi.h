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

/****************************************************************************
 *
 * File:
 *     $Workfile:a2dpi.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains internal definitions for the Advanced Audio
 *     Distribution Profile (A2DP).
 *             
 * Created:     June 3, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#ifndef __A2DPI_H_
#define __A2DPI_H_

#include "sys/a2alloc.h"

#ifndef A2DP_APP
#define A2DP_APP XA_ENABLED
#endif

/* SDP Query Flags */
#define A2DP_SDP_QUERY_FLAG_SERVICE   0x01
#define A2DP_SDP_QUERY_FLAG_PROTOCOL  0x02
#define A2DP_SDP_QUERY_FLAG_PROFILE   0x03
#define A2DP_SDP_QUERY_FLAG_FEATURES  0x08

/* Device Flags */                                        
#define A2DP_DEV_FLAG_OUTGOING      0x01
                                      
/* Stream Flags */
#define A2DP_STRM_FLAG_DISCOVERY    0x02
#define A2DP_STRM_FLAG_CLOSE_ACP    0x04
#define A2DP_STRM_FLAG_QUERY_CODEC  0x08
#define A2DP_STRM_FLAG_QUERY_CP     0x10
#define A2DP_STRM_FLAG_GET_CAP      0x20
#define A2DP_STRM_FLAG_IDLE         0x40

/* Function Prototypes */
BtStatus A2dpRegisterSdpService(A2dpEndpointType streamType);
BtStatus A2dpDeregisterSdpService(A2dpEndpointType streamType);
BtStatus A2dpStartServiceQuery(A2dpDevice *Device, A2dpEndpointType streamType, 
                               SdpQueryMode mode);
void A2dpAvdtpCallback(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms);
void A2dpFreeStreamInfo(A2dpStream *Stream);
void A2dpStreamClose(A2dpStream *Stream, AvdtpCallbackParms *Parms);
void A2dpClose(A2dpDevice *Device);
void A2dpCmgrCallback(CmgrHandler *cHandler, CmgrEvent Event, BtStatus Status);
BtStatus A2dpStreamSendSbcPacket(A2dpStream *Stream, A2dpSbcPacket *Packet);

#endif /* __A2DPI_H_ */

