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

#ifndef __RADIOMGR_H
#define __RADIOMGR_H
/***************************************************************************
 *
 * File:
 *     $Workfile:radiomgr.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains definitions for the radio manager.
 *
 * Created:
 *     March 26, 2003
 *
 * Copyright 2003-2005 Extended Systems, Inc.
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
 * Radio Manager API layer
 *
 *     The Radio Manager controls the initialization, deinitialization, and 
 *     shutdown of the HCI, HCI transport, and radio module.  
 *
 *     Initialization and shutdown of the transport is handled by the transport
 *     driver (see transprt.h) and is implementation specific.  It may include
 *     initialization of memory and queues, installing and uninstalling 
 *     interrupt routines, port probing, downloading of firmware, or any other
 *     necessary procedures.  Initialization and shutdown of transports are 
 *     asynchronous procedures.
 *
 *     Initializing and shutting down the radio module is handled by special 
 *     radio handling code and is implementation specific.  This initialization 
 *     is typically handled by vendor specific HCI commands, but any HCI command 
 *     may be used.  Initializing and shutting down the radio module are 
 *     asynchronous procedures.
 *
 *     Initializing and deinitializing the HCI is handled internally by the
 *     HCI layer.
 *
 *     Radio Manager controls how these three components are called.  During
 *     initialization, the transports are initialized, then the HCI is 
 *     initialized, and finally the radio module is initialized.  If there are
 *     any failures during this initialization, Radio Manager will retry 
 *     initialization BT_HCI_NUM_INIT_RETRIES times.  If initialization does
 *     not complete after all retries have been made, then Radio Manager will
 *     give up.  During shutdown, the radio module is shut down, then the
 *     transport is shut down, and finally the HCI is deinitialized.  If an
 *     error occurs during shutdown, shutdown of the radio module and transport
 *     is aborted and HCI is deinitialized.
 *
 *     Most Radio Manager functions operate asynchronously, but there are no
 *     callbacks made to the services calling these functions.  Instead, all
 *     notification of the completion of these procedures is made through
 *     the Management Entity (ME).  The ME will generate global events to 
 *     indicate the status of HCI.  See me.h for a description of the following
 *     events:
 *
 *         BTEVENT_HCI_INIT_ERROR
 *         BTEVENT_HCI_FATAL_ERROR
 *         BTEVENT_HCI_INITIALIZED
 *         BTEVENT_HCI_DEINITIALIZED
 *         BTEVENT_HCI_FAILED
 */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RMGR_RadioInit()
 *
 *     Called to initialize the radio module, including the HCI 
 *     transport drivers, the HCI, and the radio itself if special 
 *     initialization is required.  This function is called by the 
 *     event manager in EVM_Init() when the stack initialization sequence 
 *     begins, but can be called by services and applications afterwards if
 *     necessary (RMGR_RadioShutdown() has been called).
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.  ME will
 *         be notified during the call that initialization is complete.
 *     BT_STATUS_PENDING - Initialization was started successfully.  ME will
 *         be notified when initialization is complete.
 *     BT_STATUS_FAILED - Initialization failed to start.
 */
BtStatus RMGR_RadioInit(void);

/*---------------------------------------------------------------------------
 * RMGR_RadioDeinit()
 *
 *     Called to deinitialize the radio module.  No attempt is make to 
 *     gracefully close the transport or send HCI commands to the radio module.
 *     EVM_Deinit() calls this function during stack deinitialization.
 *     This function should only be called when unloading the stack from
 *     memory.
 *
 * Parameters:  void
 *
 * Returns:   void
 */
void RMGR_RadioDeinit(void);

/*---------------------------------------------------------------------------
 * RMGR_RadioShutdown()
 *
 *     Called to shut down the radio module, including the HCI 
 *     transport drivers, the HCI, and the radio itself if special 
 *     clean up is required through sending HCI commands.
 *     Applications can also call ME_RadioShutdown instead of 
 *     RMGR_RadioShutdown() to gracefully shut the radio down. The
 *     radio may be reinitialized by calling RMGR_RadioInit().
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The radio is down.  ME will be notified during
 *         the call that the shutdown was complete.
 *     BT_STATUS_PENDING - The radio will be shut down.  ME will be notified
 *        when shutdown is complete.
 *     BT_STATUS_FAILED - The radio is already down.
 */
BtStatus RMGR_RadioShutdown(void);

/*---------------------------------------------------------------------------
 * RMGR_HciReset()
 *
 *     Called by the HCI when an HCI transport error occurs.  The
 *     HCI transport drivers and the HCI are deinitialized and then the
 *     HCI is reset to reestablish synchronization with the radio module.
 *     Transports should call HCI_TransportError() instead of calling this
 *     function directly from an interrupt routine or transport driver
 *     thread.
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Reset was successful.  ME will be notified during 
 *         the call that initialization is complete.
 *     BT_STATUS_PENDING - Reset had begun.  ME will be notified when reset
 *         is complete.
 *     BT_STATUS_FAILED - Reset could not be initiated, because the radio 
 *         manager is shutting down.
 */
BtStatus RMGR_HciReset(void);

#endif /* __RADIOMGR_H */ 

