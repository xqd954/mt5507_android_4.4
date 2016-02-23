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

#ifndef __L2CAPXP_H
#define __L2CAPXP_H
/***************************************************************************
 *
 * File:
 *     $Workfile:l2capxp.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Funtion exports for lower layer (ME).
 *
 * Created:
 *     July 15, 1999
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

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus L2CAP_Init()
 *
 * Descriptions:  Initialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - L2CAP is initialized.
 *
 *                BT_STATUS_FAILED - L2CAP initialization failed.
 */
BtStatus L2CAP_Init(void);

/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_Deinit()
 *
 * Description:   Denitialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void L2CAP_Deinit(void);

/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_HciReceive()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_HciReceive(const HciBuffer *BuffDesc, HciHandle HciHndl);

/*---------------------------------------------------------------------------
 * Prototype:     BtPacket *L2CAP_HciTxReady()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
/* BtPacket *L2CAP_HciTxReady(HciHandle HciHndl); */
BtPacket *L2CAP_HciTxReady(HciCallbackParms *parms, U8 controllerType);

/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_HciTxDone()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_HciTxDone(BtPacket *Packet, BtStatus Status, HciHandle HciHndl);

/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_LinkDisconnect()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_LinkDisconnect(BtRemoteDevice *remDev);

#endif /* __L2CAPXP_H */ 

