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

/***************************************************************************
 *
 * File:
 *     $Workfile:radiomgr.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains initialization code for hardware drivers, 
 *     transports and the radio module.
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

#include "btalloc.h"
#include "radiomgr.h"
#include "hcitrans.h"
#include "ddb.h"

/* external function */
extern void BTSysAdpPowerOnCnf(BOOL reslut);
extern void BTSysAdpPowerOffCnf(BOOL reslut);

/* Internal functions */
static void RmgrHciCallback(U8 event, HciCallbackParms *parms);
static void RmgrTranCallback(TranEvent Event, BtStatus Status);
static void RmgrRadioCallback(RadioEvent Event, BtStatus Status);
static void RmgrRetryInit(void);


/*****************************************************************************
 * FUNCTION
 *  RMGR_RadioInit
 * DESCRIPTION
 *  Called to initialize the radio module, including the HCI
 *  transport drivers, the HCI, and the radio itself if special
 *  initialization is required.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RMGR_RadioInit(void)
{
    BtStatus status = BT_STATUS_FAILED;

    Report(("RADIOMGR:  RMGR_RadioInit\n"));
    OS_LockStack();

    Report(("RADIOMGR:  Beginning Initialization\n"));

    if (RMG(rmState) == RMGR_STATE_SHUTDOWN)
    {
        RMG(rmState) = RMGR_STATE_INITIALIZING;

        /* Register the Radio Manager with the HCI */
        HCI_RegisterRadioHandler(RmgrHciCallback);

        /* Initialize the transport */
        RMG(retries) = 0;
        Report(("RADIOMGR:  Initializing HCI\n"));
        if ((status = HCI_Init()) == BT_STATUS_SUCCESS)
        {
            Report(("RADIOMGR:  Initializing Transport\n"));
            if ((status = TRAN_Init(RmgrTranCallback)) == BT_STATUS_SUCCESS)
            {
                Report(("RADIOMGR:  Transport initialized\n"));
                /* Initialize Radio RADIO_Init(). normally return BT_STATUS_PENDING */
                if ((status = RADIO_Init(RmgrRadioCallback)) == BT_STATUS_SUCCESS)
                {
                    Report(("RADIOMGR:  Radio Initialized\n"));
                    RMG(rmState) = RMGR_STATE_READY;

                    /* Deregister the Radio Manager */
                    HCI_DeregisterRadioHandler();

                    /* Do HCI flow control negotiation */
                    HCI_Config();

                }
                else if (status == BT_STATUS_FAILED)
                {
                    Report(("RADIOMGR:  Initialization failed\n"));
                    RMG(rmState) = RMGR_STATE_SHUTDOWN;
                    HCI_RadioInitialized(BT_STATUS_FAILED);
                }
            }
        }
    }
    else if ((RMG(rmState) == RMGR_STATE_INITIALIZING))
    {
        Report(("RADIOMGR:  Init called during init\n"));
        status = BT_STATUS_PENDING;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RMGR_RadioShutdown
 * DESCRIPTION
 *  Called to shut down the radio module, including the HCI
 *  transport drivers, the HCI, and the radio itself if special
 *  clean up is required.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RMGR_RadioShutdown(void)
{
    BtStatus status = BT_STATUS_PENDING;

    Report(("RADIOMGR:  RMGR_RadioShutdown\n"));
    OS_LockStack();

    Report(("RADIOMGR:  Shutdown initiated\n"));

    if (RMG(rmState) == RMGR_STATE_READY)
    {
        RMG(rmState) = RMGR_STATE_SHUTTING_DOWN;

        if (RADIO_Shutdown() == BT_STATUS_PENDING)
        {
            Report(("RADIOMGR:  Radio shutting down\n"));

            /* Register the Radio Manager with the HCI */
            HCI_RegisterRadioHandler(RmgrHciCallback);

            goto exit;
        }
        Report(("RADIOMGR:  Radio Module is down\n"));

        /* Deinit HCI */
        Report(("RADIOMGR:  HCI deinit started\n"));
        HCI_Deinit();
        if ((status = TRAN_Shutdown()) != BT_STATUS_PENDING)
        {
            Report(("RADIOMGR:  Transport is down\n"));
            RMG(rmState) = RMGR_STATE_SHUTDOWN;
            Report(("RADIOMGR:  HCI is down\n"));
            HCI_RadioDeinitialized();

            /* Radio shutdown ok, send power off cnf to MMI */
            BTSysAdpPowerOffCnf(TRUE);

        }
    }
    else
    {
        Report(("RADIOMGR:  Shutdown failed (already down or intializing)\n"));
        status = BT_STATUS_FAILED;
    }

  exit:

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RMGR_HciReset
 * DESCRIPTION
 *  Called by the HCI when an an HCI transport error occurs.  The
 *  HCI transport drivers and the HCI are deinitialized and then
 *  HCI reset is called to restablish synchronization with the
 *  the radio module.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RMGR_HciReset(void)
{
    BtStatus status = BT_STATUS_PENDING;

    Report(("RADIOMGR:  RMGR_HciReset\n"));
    OS_LockStack();

    Report(("RADIOMGR:  Reset initiated\n"));

    switch (RMG(rmState))
    {
        case RMGR_STATE_RESETTING:
        case RMGR_STATE_INITIALIZING:
        case RMGR_STATE_READY:
            /* Shutdown */
            Report(("RADIOMGR:  HCI deinit started\n"));
            HCI_Deinit();

            RMG(rmState) = RMGR_STATE_RESETTING;
            Report(("RADIOMGR:  Transport shutting down\n"));
            status = TRAN_Shutdown();
            if (status == BT_STATUS_PENDING)
            {
                break;
            }
            else if (status == BT_STATUS_FAILED)
            {
                Report(("RADIOMGR:  Transport failed shutdown\n"));
                Report(("RADIOMGR:  HCI is down\n"));
                RMG(rmState) = RMGR_STATE_SHUTDOWN;
                HCI_RadioDeinitialized();
                break;
            }
            else
            {
                Report(("RADIOMGR:  Transport is down\n"));
                Report(("RADIOMGR:  HCI is down\n"));
                HCI_RadioDeinitialized();
            }
            /* Drop Through */
        case RMGR_STATE_SHUTDOWN:
            /* Register the Radio Manager with the HCI */
            HCI_RegisterRadioHandler(RmgrHciCallback);

            /* Init the transport */
            Report(("RADIOMGR:  Transport initializing\n"));
            if ((status = HCI_Init()) == BT_STATUS_SUCCESS)
            {
                if ((status = TRAN_Init(RmgrTranCallback)) == BT_STATUS_SUCCESS)
                {
                    Report(("RADIOMGR:  Transport initiated\n"));

                    if ((status = RADIO_Init(RmgrRadioCallback)) == BT_STATUS_SUCCESS)
                    {
                        Report(("RADIOMGR:  Radio Initialized\n"));
                        RMG(rmState) = RMGR_STATE_READY;

                        /* Deregister the Radio Manager */
                        HCI_DeregisterRadioHandler();

                        /* Do HCI flow control negotiation */
                        HCI_Config();

                    }
                    else if (status == BT_STATUS_FAILED)
                    {
                        Report(("RADIOMGR:  Initialization failed\n"));
                        RMG(rmState) = RMGR_STATE_SHUTDOWN;
                        HCI_RadioInitialized(BT_STATUS_FAILED);
                    }
                }
            }

            if (status == BT_STATUS_FAILED)
            {
                /* Unrecoverable error */
                Report(("RADIOMGR:  Could not init transport during reset\n"));
                RMG(rmState) = RMGR_STATE_SHUTDOWN;
                HCI_RadioInitialized(BT_STATUS_FAILED);
            }
            break;
        case RMGR_STATE_SHUTTING_DOWN:
            /* Unrecoverable error */
            Report(("RADIOMGR:  Reset during shutdown\n"));
            Report(("RADIOMGR:  HCI deinit started\n"));
            HCI_DeregisterRadioHandler();
            HCI_Deinit();
            RMG(rmState) = RMGR_STATE_SHUTDOWN;
            Report(("RADIOMGR:  HCI is down\n"));
            HCI_RadioDeinitialized();
            break;
        default:
            Report(("RADIOMGR:  Reset failed\n"));
            status = BT_STATUS_FAILED;
            break;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RMGR_RadioDeinit
 * DESCRIPTION
 *  Called to deinitialize the HCI transport drivers and the HCI.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void RMGR_RadioDeinit(void)
{

    Report(("RADIOMGR:  RadioDeinit\n"));
    OS_LockStack();

    if (RMG(rmState) != RMGR_STATE_SHUTDOWN)
    {
        Report(("RADIOMGR:  HCI deinit started\n"));
        HCI_DeregisterRadioHandler();
        HCI_Deinit();
        RMG(rmState) = RMGR_STATE_SHUTDOWN;
        Report(("RADIOMGR:  HCI is down\n"));
        HCI_RadioDeinitialized();
    }

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  RmgrRetryInit
 * DESCRIPTION
 *  Attempts to re-initialize.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void RmgrRetryInit(void)
{
#if BT_HCI_NUM_INIT_RETRIES > 0
    /* Keep retrying */
    if (RMG(retries)++ < BT_HCI_NUM_INIT_RETRIES)
    {
        Report(("RADIOMGR:  HCI init error\n"));
        HCI_RadioInitialized(BT_STATUS_HCI_INIT_ERR);
        if (RMGR_HciReset() == BT_STATUS_FAILED)
        {
            Report(("RADIOMGR:  Initialization failed\n"));
            RMG(rmState) = RMGR_STATE_SHUTDOWN;
            HCI_RadioInitialized(BT_STATUS_FAILED);
        }
    }
    else
    {
#endif /* BT_HCI_NUM_INIT_RETRIES > 0 */
        /* Give up */
        Report(("RADIOMGR:  Initialization failed\n"));
        RMG(rmState) = RMGR_STATE_SHUTDOWN;
        HCI_RadioInitialized(BT_STATUS_FAILED);

        /* Radio Init failed, send power on cnf to MMI */
        BTSysAdpPowerOnCnf(FALSE);

    #if BT_HCI_NUM_INIT_RETRIES > 0
    }
    #endif /* BT_HCI_NUM_INIT_RETRIES > 0 */
}


/*****************************************************************************
 * FUNCTION
 *  RmgrHciCallback
 * DESCRIPTION
 *  Callback function to receive HCI events.
 * PARAMETERS
 *  event       [IN]        
 *  Parms       [IN] 
 * RETURNS
 *  void
 *****************************************************************************/
void RmgrHciCallback(U8 event, HciCallbackParms *Parms)
{

    Report(("RADIOMGR:  RmgrHciCallback: %#x\n", event));
    switch (event)
    {
        case HCI_CONTROLLER_EVENT:
        case HCI_COMMAND_HANDLED:
        case HCI_BLUETOOTH_LOGO:
        case HCI_VENDOR_SPECIFIC:
            Report(("RADIOMGR:  RmgrHciCallback: valid event.\n"));

            if ((event == HCI_CONTROLLER_EVENT) && 
            #if defined(BTMTK_ON_LINUX)
#ifdef __MTK_BT_EXTERNAL_PLATFORM__
                (LEtoHost16(&(Parms->ptr.hciEvent->parms[1])) == HCC_RESET)
            #else
                (LEtoHost16(&(Parms->ptr.hciEvent->parms[1])) == HCC_READ_LOCAL_VERSION)
#endif                
            #else
                (LEtoHost16(&(Parms->ptr.hciEvent->parms[1])) == HCC_RESET)
            #endif
                )
            {
                /* Zero the Radio Manager retries */
                RMG(retries) = 0;
            }

            RADIO_EventHandler(event, Parms);
            break;
        case HCI_INIT_STATUS:
            if (Parms->status == BT_STATUS_SUCCESS)
            {
                /* Only handle failures through HCI_INIT_STATUS */
                Assert(0);
            }
            else
            {
                /* Keep retrying */
                Report(("RADIOMGR:  HCI init failed (retrying)\n"));
                RmgrRetryInit();
            }
            break;
        default:
            Assert(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RmgrTranCallback
 * DESCRIPTION
 *  Callback function to receive Transport events.
 * PARAMETERS
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RmgrTranCallback(TranEvent Event, BtStatus Status)
{
    BtStatus status;

    Report(("RADIOMGR:  RmgrTranCallback: %#x\n", Event));
    switch (Event)
    {
        case TRAN_INIT_STATUS:
            if (Status == BT_STATUS_SUCCESS)
            {
                Report(("RADIOMGR:  Transport initialized\n"));
                if ((status = RADIO_Init(RmgrRadioCallback)) == BT_STATUS_SUCCESS)
                {
                    Report(("RADIOMGR:  Radio Initialized\n"));
                    RMG(rmState) = RMGR_STATE_READY;

                    /* Deregister the Radio Manager */
                    HCI_DeregisterRadioHandler();

                    /* Tell the HCI that initialization is complete */
                    HCI_RadioInitialized(BT_STATUS_SUCCESS);
                }
                else if (status == BT_STATUS_FAILED)
                {
                    Report(("RADIOMGR:  Initialization failed\n"));
                    RMG(rmState) = RMGR_STATE_SHUTDOWN;
                    HCI_RadioInitialized(BT_STATUS_FAILED);
                }
            }
            else
            {
                /* Keep retrying */
                Report(("RADIOMGR:  Transport init failed (retrying)\n"));
                RmgrRetryInit();
            }
            break;
        case TRAN_SHUTDOWN_STATUS:
            Report(("RADIOMGR:  Transport is down\n"));
            if (Status == BT_STATUS_SUCCESS)
            {
                if (RMG(rmState) == RMGR_STATE_RESETTING)
                {
                    Report(("RADIOMGR:  HCI is down\n"));
                    HCI_RadioDeinitialized();

                    /* Register the Radio Manager with the HCI */
                    HCI_RegisterRadioHandler(RmgrHciCallback);

                    /* Init the transport */
                    Report(("RADIOMGR:  Initializing transport during reset\n"));
                    status = TRAN_Init(RmgrTranCallback);
                    switch (status)
                    {
                        case BT_STATUS_SUCCESS:
                            Report(("RADIOMGR:  Transport initialized\n"));
                            Report(("RADIOMGR:  Intializing Radio\n"));
                            if ((status = RADIO_Init(RmgrRadioCallback)) == BT_STATUS_SUCCESS)
                            {
                                Report(("RADIOMGR:  Radio Initialized\n"));
                                RMG(rmState) = RMGR_STATE_READY;

                                /* Deregister the Radio Manager */
                                HCI_DeregisterRadioHandler();

                                /* Tell the HCI that initialization is complete */
                                HCI_RadioInitialized(BT_STATUS_SUCCESS);
                            }
                            else if (status == BT_STATUS_FAILED)
                            {
                                Report(("RADIOMGR:  Initialization failed\n"));
                                RMG(rmState) = RMGR_STATE_SHUTDOWN;
                                HCI_RadioInitialized(BT_STATUS_FAILED);
                            }
                            break;
                        case BT_STATUS_FAILED:
                            Report(("RADIOMGR:  Transport init failed (giving up)\n"));
                            RMG(rmState) = RMGR_STATE_SHUTDOWN;
                            HCI_RadioInitialized(BT_STATUS_FAILED);
                            break;
                    }
                }
                else
                {
                    Report(("RADIOMGR:  HCI is down\n"));
                    RMG(rmState) = RMGR_STATE_SHUTDOWN;
                    HCI_RadioDeinitialized();
                }
            }
            else
            {
                /* Unrecoverable error */
                Report(("RADIOMGR:  Transport deinitialization failed\n"));
                RMG(rmState) = RMGR_STATE_SHUTDOWN;
                HCI_RadioDeinitialized();
            }
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RmgrRadioCallback
 * DESCRIPTION
 *  Callback function to receive Radio specific events.
 * PARAMETERS
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RmgrRadioCallback(RadioEvent Event, BtStatus Status)
{
    Report(("RADIOMGR:  RmgrRadioCallback: %#x\n", Event));
    switch (Event)
    {
        case RADIO_INIT_STATUS:
            Report(("RMGR:  RADIO_INIT_STATUS\n"));
            if (Status == BT_STATUS_SUCCESS)
            {
                /* Radio Module is up */
                Report(("RADIOMGR:  Radio initialized\n"));
                RMG(rmState) = RMGR_STATE_READY;

                /* Deregister the Radio Manager */
                HCI_DeregisterRadioHandler();

                /* Do HCI flow control negotiation */
                HCI_Config();
            }
            else
            {
                /* Keep retrying */
                Report(("RADIOMGR:  Radio init failed (retrying)\n"));
                RmgrRetryInit();
            }
            break;
        case RADIO_SHUTDOWN_STATUS:
            /* Radio Module shut down */
            Report(("RADIOMGR:  Radio is down\n"));

            /* Deregister the Radio Manager */
            HCI_DeregisterRadioHandler();

            /* Deinit HCI */
            Report(("RADIOMGR:  HCI deinit started\n"));
            HCI_Deinit();
            if (TRAN_Shutdown() != BT_STATUS_PENDING)
            {
                Report(("RADIOMGR:  Transport is down\n"));
                RMG(rmState) = RMGR_STATE_SHUTDOWN;
                Report(("RADIOMGR:  HCI is down\n"));
                HCI_RadioDeinitialized();

                /* Radio shutdown ok, send power off cnf to MMI */
                BTSysAdpPowerOffCnf(TRUE);
            }
            break;
    }
}

