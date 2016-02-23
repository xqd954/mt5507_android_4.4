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
 *     $Workfile:conmgr.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the connection manager.
 *             
 * Created:     June 12, 2003
 *
 * Copyright 2003-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "bt_common.h"
#include "sys/conmgri.h"
#include "medev.h"
#include "me.h"
#include "btconfig.h"
#include "btaccctrl.h"
#ifdef MTK_BB_TESTING
#include "BtBBtesting.h"
#endif 

#if XA_DEBUG == XA_ENABLED
#include "sys/debug.h"
#endif 

#if NUM_SCO_CONNS > 0
#include "mesco.h"
#endif 

#include "btbm.h" /* NOKIA_BH_100 */
#include "btalloc.h"
#include "stdlib.h"

EvmTimer audioSettingTimer;

#if CMGR_MEMORY_EXTERNAL == XA_DISABLED
#if XA_CONTEXT_PTR == XA_ENABLED
BtCmgrContext *cmgrContext;

#define CMGR(s) (cmgrContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
BtCmgrContext cmgrContext;

#define CMGR(s) (cmgrContext.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
#endif /* CMGR_MEMORY_EXTERNAL == XA_DISABLED */

U8 *bt_audio_setting = 0;


/* HFP 1.6 */
/* eSCO settings for command HCI_Enhanced_Setup_Synchronous_Connection */
const BtEnhancedScoAudioSettings bt_enhanced_sco_settings[NUM_OF_CMGR_CODEC] = 
{  
    /* CMGR_AUDIO_CODEC_MSBC */
    {
        0x1f40,                           /* transmitBandwidth */
        0x1f40,                           /* receiveBandwidth */
        {
            0x5, /* id */
            {
                0, /* comanyId */
                0 /* id */
            }
        },                                          /* txCoding */
        {
            0x5, /* id */
            {
                0, /* comanyId */
                0 /* id */
            }
        },                                           /* rxCoding */
        0x3C,                                       /* txCodecFrameSize */
        0x3C,                                       /* rxCodecFrameSize */
        0x0100,                                    /* inputBandwith */
        0x0100,                                    /* outputBandwith */
        {
            0x4, /* id */
            {
                0, /* comanyId */
                0 /* id */
            }
        },                                            /* inputCoding */
        {
            0x4, /* id */
            {
                0, /* comanyId */
                0 /* id */
            }
        },                                            /* outputCoding */
        0x01E0,                                    /* inputCodedDataSize */
        0x01E0,                                    /* outputCodedDataSize */
        0x02,                                       /* inputPCMDataFormat */
        0x02,                                       /* outputPCMDataFormat */
        0x00,                                       /* inputPCMSamplePayloadMSBPos */
        0x00,                                       /* outputPCMSamplePayloadMSBPos */
        0x01,                                       /* inputDataPath */
        0x01,                                       /* outputDataPath */
        0x10,                                       /* inputTransportUnitSize */
        0x10,                                       /* outputTransportUnitSize */
        0x0F,                                       /* maxLatency */
        0x0388,                                    /* packetType */
        0x02,                                       /* retransmissionEffort */
    },
};

/****************************************************************************
 *
 * external Functions
 *
 ****************************************************************************/
extern U8 BTBMCheckAllowScoConnectReq(void);
extern BOOL A2DP_StreammingDataSendingVerify(void);
extern BOOL BTBMCheckBMStatusNotExistSnifferMode(void);
extern BtStatus BTBMDoRoleSwitch(BtRemoteDevice *link);
extern BOOL BTCheckPktServicedTimeLong(U32 int_time, U32 time_value);
extern void BTBMHIDNoRoleSwitch(U8 ON_OFF);
extern kal_bool hfga_is_profile_support_esco(void);
extern BOOL BTBMChangeScattnetRoleForSCO(BtRemoteDevice *remDev);
extern void BTBMChangeAllLinkPolcy(BtRemoteDevice *remDev, BOOL disabled);
extern BOOL btbmCheckCreateLinkKeyState(void);
extern U8 BTQueryPowerOnState(void);

#if (SPP_SCO_SUPPORT == XA_ENABLED) || (BT_READ_RSSI == XA_ENABLED)
BtStatus ME_ReadRSSI(BtRemoteDevice *remDev);
#endif 

#if defined (__BT_2_1_BLOCK_DEVICE__)
extern BOOL BTBMCheckBlockList(U8 *addr);
#endif 
extern U8 Me_WriteAudioSetting(void);


/*****************************************************************************
 * FUNCTION
 *  ConnMgrIsLinkUp
 * DESCRIPTION
 *  Returns the state of the data link (connection manager internal)
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL ConnMgrIsLinkUp(CmgrHandler *Handler)
{
    BOOL status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);
//    status = ConnMgrCheckLinkStatus(Handler, BDS_DISCONNECTED);
    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        if ((Handler->bdc) && (Handler->bdc->link))
        {
            status = (BOOL)((ME_GetRemDevState(Handler->bdc->link) == BDS_DISCONNECTED) ? FALSE : TRUE);
        }
        else
        {
            status = FALSE;
        }
    }
    else
    {
        status = FALSE;
    }

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrIsLinkConnected
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL ConnMgrIsLinkConnected(CmgrHandler *Handler)
{
    BOOL status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);
//    status = ConnMgrCheckLinkStatus(Handler, BDS_CONNECTED);
#if 1
    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        if ((Handler->bdc) && (Handler->bdc->link))
        {
            status = (BOOL)((ME_GetRemDevState(Handler->bdc->link) == BDS_CONNECTED) ? TRUE : FALSE);
        }
        else
        {
            status = FALSE;
        }
    }
    else
    {
        status = FALSE;
    }

    OS_UnlockStack();
#endif
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrGetMaxSniffTime
 * DESCRIPTION
 *  Finds the maximum sniff time enabled by registered handlers
 *  and return a pointer to that handler.  Called only when setting
 *  a new timer.
 * PARAMETERS
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static CmgrHandler *ConnMgrGetMaxSniffTime(BtRemoteDevice *RemDev)
{
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
    CmgrHandler *maxHandler = 0;

    while (handler != (CmgrHandler*) & CMGR(handlerList))
    {
        if (handler->bdc && (handler->bdc->link == RemDev))
        {

            /* Cancel any running timer */
            if (handler->sniffTimer.context)
            {
                EVM_CancelTimer(&handler->sniffTimer);
                handler->sniffTimer.context = 0;
            }

            /* Find the max timer */
            if (maxHandler)
            {
                if (handler->sniffTimeout > maxHandler->sniffTimeout)
                {
                    maxHandler = handler;
                }
            }
            else
            {
                maxHandler = handler;
            }
        }

        handler = (CmgrHandler*) GetNextNode(&handler->node);
    }

    return maxHandler;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrCheckStartSniffTimer
 * DESCRIPTION
 *  Checks for the correct state and starts the timer.
 * PARAMETERS
 *  RemDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ConnMgrCheckStartSniffTimer(BtRemoteDevice *RemDev)
{
    CmgrHandler *handler = ConnMgrGetMaxSniffTime(RemDev);

    /* Start timer if possible */
    bt_trace(TRACE_GROUP_1, CONNMGRCHECKSTARTSNIFFTIMER);
    if (handler)
    {
        if ((handler->sniffTimeout > 0) && ConnMgrIsLinkUp(handler) &&
    #if NUM_SCO_CONNS > 0
            !CMGR_IsAudioUp(handler) &&
    #endif 
            (handler->sniffTimerActivated == 1) && (ME_GetCurrentMode(handler->bdc->link) == BLM_ACTIVE_MODE))
        {
            /* Start the timer */
            handler->sniffTimer.context = handler;
            EVM_StartTimer(&handler->sniffTimer, handler->sniffTimeout);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrSniffTimeoutHandler
 * DESCRIPTION
 *  Puts the device in sniff mode when the specified time has
 *  expired.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ConnMgrSniffTimeoutHandler(EvmTimer *timer)
{
    CmgrHandler *handler = timer->context;
    U32 next_start_time = 0;
    U32 temp_interval = 0;
    U32 current_time = 0;

    bt_trace(TRACE_GROUP_1, CONNMGRSNIFFTIMEOUTHANDLER);
    if (ConnMgrIsLinkUp(handler) &&
#if NUM_SCO_CONNS > 0
        !CMGR_IsAudioUp(handler) &&
#endif 
        (ME_GetCurrentMode(handler->bdc->link) == BLM_ACTIVE_MODE))
    {
        Assert(handler->bdc->link != 0);
        if (!BTCheckPktServicedTimeLong(handler->bdc->link->lastTxRxTime, (U32)(handler->sniffTimeout / 50)))
        {
            bt_trace(TRACE_GROUP_1, SNIFFER_TIMER_SHALL_REOPEN);
            current_time = OS_GetSystemTime();

            if (current_time >= handler->bdc->link->lastTxRxTime)
            {
                temp_interval = current_time - handler->bdc->link->lastTxRxTime;
            }
            else
            {
                temp_interval = current_time + ((0xFFFFFFFF) - handler->bdc->link->lastTxRxTime);
            }
            bt_trace(TRACE_GROUP_1, TIME_INTERVALx08X, temp_interval);
            if (temp_interval * 50 > handler->sniffTimeout)
            {
                next_start_time = 0;
            }
            else
            {
                next_start_time = handler->sniffTimeout - (U32)(temp_interval * 50);
            }
            if (next_start_time > handler->sniffTimeout)
            {
                handler->sniffTimeout = handler->sniffTimeout;
                bt_trace(TRACE_GROUP_1, THE_TIMER_LOOKS_LIKE_SOME_ERRORS);
            }
            else
            {
                if (next_start_time < 5)
                {
                    next_start_time = 0;
                }
            }
            bt_trace(TRACE_GROUP_1, NEXT_START_TIMEx08X, next_start_time);
            if (next_start_time != 0)
            {
                EVM_StartTimer(timer, next_start_time);
                return;
            }
        }
        /* OK to start sniff mode */
    #ifdef __BT_A2DP_PROFILE__
        if (!A2DP_StreammingDataSendingVerify())
        {
            bt_trace(TRACE_GROUP_1, NO_A2DP_DATA_STREAMMING);
            if (ME_StartSniff(handler->bdc->link, &handler->sniffInfo) != BT_STATUS_PENDING)
            {

                /* Could not start sniff mode, start the timer again */
                EVM_StartTimer(timer, handler->sniffTimeout);
            }
            else
            {
                /* Timer is off untill sniff mode is exited */
                timer->context = 0;
            }
        }
        else
        {
            EVM_StartTimer(timer, handler->sniffTimeout);
        }
    #else /* __BT_A2DP_PROFILE__ */ 
        if (ME_StartSniff(handler->bdc->link, &handler->sniffInfo) != BT_STATUS_PENDING)
        {

            /* Could not start sniff mode, start the timer again */
            EVM_StartTimer(timer, handler->sniffTimeout);
        }
        else
        {
            /* Timer is off untill sniff mode is exited */
            timer->context = 0;
        }
    #endif /* __BT_A2DP_PROFILE__ */ 
    }
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrSnifferTimeOutImmediately
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ConnMgrSnifferTimeOutImmediately(BtRemoteDevice *link)
{
    CmgrHandler *cmgrHandler;
    CmgrHandler *handler = ConnMgrGetMaxSniffTime(link);

    cmgrHandler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
    while (cmgrHandler != (CmgrHandler*) & CMGR(handlerList))
    {
        if ((cmgrHandler->bdc != 0) && (OS_MemCmp(link->bdAddr.addr, 6, cmgrHandler->bdc->addr.addr, 6)))
        {
            CMGR_ClearSniffTimer(cmgrHandler);
            if (ConnMgrIsLinkUp(cmgrHandler) && (ME_GetCurrentMode(cmgrHandler->bdc->link) == BLM_ACTIVE_MODE))
            {
                if (ME_StartSniff(cmgrHandler->bdc->link, &cmgrHandler->sniffInfo) != BT_STATUS_PENDING)
                {
                    cmgrHandler->sniffTimer.context = handler;
                    EVM_StartTimer(&cmgrHandler->sniffTimer, cmgrHandler->sniffTimeout);
                }
                else
                {
                    cmgrHandler->sniffTimer.context = 0;
                    break;
                }
            }
            else
            {
                cmgrHandler->sniffTimer.context = handler;
                EVM_StartTimer(&cmgrHandler->sniffTimer, cmgrHandler->sniffTimeout);
            }
        }
        cmgrHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);
    }

}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrGetSniffExitPolicy
 * DESCRIPTION
 *  Gets composite sniff exit policy of all handlers on a given
 *  link.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static CmgrSniffExitPolicy ConnMgrGetSniffExitPolicy(CmgrHandler *Handler)
{
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
    CmgrSniffExitPolicy sniffPolicy = CMGR_SNIFF_DONT_CARE;

    if (!BTBMCheckBMStatusNotExistSnifferMode())
    {
        while (handler != (CmgrHandler*) & CMGR(handlerList))
        {
            if (Handler->bdc && handler->bdc && (Handler->bdc->link == handler->bdc->link))
            {

                /*
                 * Set the sniff exit policy.  It will be based on the policy of
                 * * all devices on the link.
                 */
                sniffPolicy |= Handler->sniffPolicy;
            }

            handler = (CmgrHandler*) GetNextNode(&handler->node);
        }
    }

    return sniffPolicy;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrGetFirstHandlerByRemDev
 * DESCRIPTION
 *  Gets the first handler using the specified link.
 * PARAMETERS
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static CmgrHandler *ConnMgrGetFirstHandlerByRemDev(BtRemoteDevice *RemDev)
{
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));

    while (handler != (CmgrHandler*) & CMGR(handlerList))
    {
        if (handler->bdc && (handler->bdc->link == RemDev))
        {
            break;
        }

        handler = (CmgrHandler*) GetNextNode(&handler->node);
    }

    if (handler == (CmgrHandler*) & CMGR(handlerList))
    {
        /* No handler found */
        handler = 0;
    }

    return handler;
}


static BtStatus ConnMgrRemoveDataLink(CmgrHandler *Handler){
    BtStatus status = BT_STATUS_SUCCESS;
    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
#if NUM_SCO_CONNS > 0
        if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING))
        {
            (void)CMGR_DeregisterScoHandler(Handler);
        }
#endif /* NUM_SCO_CONNS > 0 */

        if (ConnMgrIsLinkConnected(Handler) &&
             IsNodeOnList(&(Handler->bdc->link->handlers), &(Handler->btHandler.node)))
        {
            status = ME_DisconnectLink(&Handler->btHandler, Handler->bdc->link);
            if (status != BT_STATUS_SUCCESS)
            {
                status = ME_ForceDisconnectLinkWithReason(0, Handler->bdc->link, BEC_USER_TERMINATED, TRUE);
            }
        }
        else
        {
            if ((Handler->bdc) && (Handler->bdc->link))
            {
                if (IsNodeOnList(&(Handler->bdc->link->handlers), &(Handler->btHandler.node)))
                {
                    RemoveEntryList(&(Handler->btHandler.node));
                }
            }
        }

    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }
    return status;
}

#if NUM_SCO_CONNS > 0


/*****************************************************************************
 * FUNCTION
 *  ConnMgrScoClearState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void ConnMgrScoClearState(void)
{
    if (CMGR(scoRetryTimer).context != 0)
    {
        EVM_CancelTimer(&CMGR(scoRetryTimer));
    }
    CMGR(scoRetryTimer).context = 0;
    CMGR(scoTimerDevice) = 0;
    CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ESCO;
    CMGR(scoTryryTimerCount) = 0;
    CMGR(scoMaxRetryTimerCount) = 0;    
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrScoNotify
 * DESCRIPTION
 *  Notify all relevant handlers of the SCO connect.
 * PARAMETERS
 *  btEvent     [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL ConnMgrScoNotify(const BtEvent *btEvent, CmgrEvent Event, BtStatus Status)
{
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
    CmgrHandler *nextHandler;
    BOOL notified = FALSE;

    while (handler != (CmgrHandler*) & CMGR(handlerList))
    {
        if (handler->bdc && (handler->bdc->link == btEvent->p.scoConnect.scoCon->scord) &&
            (handler->flags & CMGR_FLAG_SCO_REGISTERED))
        {

            /* Set up and call the SCO handler */
            handler->errCode = btEvent->errCode;
            handler->scoConnect = btEvent->p.scoConnect.scoCon;

            if (Event == CMEVENT_AUDIO_LINK_CON)
            {
                if (Status == BT_STATUS_SUCCESS)
                {
                    handler->flags |= CMGR_FLAG_AUDIO_UP;
                }
                else
                {
                    handler->flags &= ~CMGR_FLAG_AUDIO_UP;
                }
            }
            else if (Event == CMEVENT_AUDIO_LINK_DIS)
            {
                handler->flags &= ~CMGR_FLAG_AUDIO_UP;
            #if NUM_SCO_CONNS > 0
            #if BT_SCO_HCI_DATA == XA_ENABLED
            }
            else if (Event == CMEVENT_AUDIO_DATA_SENT)
            {
                handler->audioPacket = btEvent->p.scoPacketHandled.scoPacket;
            }
            else if (Event == CMEVENT_AUDIO_DATA)
            {
                handler->audioData.len = btEvent->p.scoDataInd.len;
                handler->audioData.data = btEvent->p.scoDataInd.ptr;
            #endif /* BT_SCO_HCI_DATA == XA_ENABLED */
            }
            else if (Event == CMEVENT_AUDIO_LINK_REQ)
            {
                CMGR(scoRequest) = TRUE;
            #endif /* NUM_SCO_CONNS > 0 */
            }

            /*
             * Get the next node before the callback in case it is
             * deregistered.
             */
            nextHandler = (CmgrHandler*) GetNextNode(&handler->node);

            if ((Event != CMEVENT_AUDIO_LINK_REQ) || (handler->flags & CMGR_FLAG_HANDLE_SCO))
            {
                handler->callback(handler, Event, Status);
                notified = TRUE;
            }
        }
        else
        {
            nextHandler = (CmgrHandler*) GetNextNode(&handler->node);
        }

        handler = nextHandler;
    }

    return notified;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrScoFakeNotify
 * DESCRIPTION
 *  Notify all relevant handlers of the SCO connect failed.
 * PARAMETERS
 *  remDev          [IN]        
 *  scoConnect      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL ConnMgrScoFakeNotify(BtRemoteDevice *remDev, BtScoConnect *scoConnect)
{
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
    CmgrHandler *nextHandler;
    BOOL notified = FALSE;
    
    OS_Report("[CMGR] SCO create failed without a confirm. Make fake report to upper layers");
    while (handler != (CmgrHandler*) & CMGR(handlerList))
    {
        if ((handler->bdc != 0) && (handler->bdc->link == remDev))
        {
            handler->errCode = BT_STATUS_CHIP_REASON;
            handler->scoConnect = scoConnect;
            nextHandler = (CmgrHandler*) GetNextNode(&handler->node);
            handler->callback(handler, CMEVENT_AUDIO_LINK_CON, BT_STATUS_CHIP_REASON);
            notified = TRUE;
        }
        else
        {
            nextHandler = (CmgrHandler*) GetNextNode(&handler->node);
        }

        handler = nextHandler;
    }
    return notified;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrIsAudioUp
 * DESCRIPTION
 *  See if an audio link is up on the remote device.
 * PARAMETERS
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL ConnMgrIsAudioUp(BtRemoteDevice *RemDev)
{
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));

    while (handler != (CmgrHandler*) & CMGR(handlerList))
    {
        if (handler->bdc && (RemDev == handler->bdc->link))
        {
            if (handler->flags & CMGR_FLAG_AUDIO_UP)
            {
                return TRUE;
            }
        }

        handler = (CmgrHandler*) GetNextNode(&handler->node);
    }

    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrCountScoHandlers
 * DESCRIPTION
 *  Count the number of Sco handlers registered on the
 *  specified device.
 * PARAMETERS
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static U16 ConnMgrCountScoHandlers(BtRemoteDevice *RemDev)
{
    U16 count = 0;
    CmgrHandler *handler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));

    while (handler != (CmgrHandler*) & CMGR(handlerList))
    {
        if (handler->bdc && (handler->bdc->link == RemDev))
        {
            if (handler->flags & CMGR_FLAG_SCO_REGISTERED)
            {
                count++;
            }
        }

        handler = (CmgrHandler*) GetNextNode(&handler->node);
    }

    return count;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrCreateAudioLink
 * DESCRIPTION
 *  Create an eSCO or SCO connection.
 * PARAMETERS
 *  scoConnect      [IN]        
 *  RemDev          [IN]        
 *  flag            [IN]        
 * RETURNS
 *  (see SCO_CreateLink and SCO_SetupLink)
 *****************************************************************************/
static BtStatus ConnMgrCreateAudioLink(BtScoConnect **scoConnect, BtRemoteDevice *RemDev, U8 flag)
{
    BtStatus status;
    BtScoTxParms scoTxParms;
    DEVICE_MODEL deviceModel = 0;    /* NOKIA_BH_100 */

    OS_Report("ConnMgrCreateAudioLink(scoConnect=%p, RemDev=%p, flag=%u)", scoConnect, RemDev, flag);
    /* HFP 1.6 */
    if(flag == CMGR_AUDIO_PARMS_ESCO_CODEC){
        scoTxParms.isExtendedSco = 1;
        OS_Report("ConnMgrCreateAudioLink Create ext SCO with codec = %d", RemDev->scoCodec);
        Assert(RemDev->scoCodec < NUM_OF_CMGR_CODEC);
        scoTxParms.extScoSettings = bt_enhanced_sco_settings[RemDev->scoCodec];
    }else{
        scoTxParms.isExtendedSco = 0;
        /* Create a custom eSCO link */
        scoTxParms.transmitBandwidth = CMGR(scoCustomParms).transmitBandwidth;
        scoTxParms.receiveBandwidth = CMGR(scoCustomParms).receiveBandwidth;
        scoTxParms.voiceSetting = CMGR(scoCustomParms).voiceSetting;
        scoTxParms.maxLatency = CMGR(scoCustomParms).maxLatency;
        bt_trace(TRACE_GROUP_1, SUPPORT_ESCO_STATUSx02X, RemDev->supportESCO);

        /* NOKIA_BH_100 */
        deviceModel = BTBMGetDeviceModel(&RemDev->bdAddr);    

        /* 
         * MAUI_01330592: Terax chip (remoteVersion 0x38 and 0x00) has eSCO IOT problem.            
         * MAUI_01729133: NOKIA_BH_100 has eSCO IOT problem
         */
        if (((RemDev->remoteVersion[1] == 0x38) && (RemDev->remoteVersion[2] == 0x00)) || 
            (flag == CMGR_AUDIO_PARMS_SCO) || (RemDev->supportESCO == 0) ||
            (deviceModel == DEVICE_NOKIA_BH100)
            #ifdef __BT_USE_CUST_DATA__
            || (U32)btGetDevCustvalueById(RemDev->devId, CUSTID_FORCE_SCO) != 0
            #endif
            )
        {
            bt_trace(TRACE_GROUP_1, BT_TRY_TO_OPEN_SCO);
            #ifdef __BT_USE_CUST_DATA__
            scoTxParms.eScoPktType = (U32)btGetDevCustvalueById(RemDev->devId, CUSTID_SCO_PKT_TYPE);
            #else /* __BT_USE_CUST_DATA__ */
            scoTxParms.eScoPktType = 0x03C7;
            #endif /* __BT_USE_CUST_DATA__ */
            scoTxParms.retransmissionEffort = 0x00;
            RemDev->scoParms = CMGR_AUDIO_PARMS_SCO;
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BT_TRY_TO_OPEN_ESCO);
            #ifdef __BT_USE_CUST_DATA__
            scoTxParms.eScoPktType = (U32)btGetDevCustvalueById(RemDev->devId, CUSTID_ESCO_PKY_TYPE);
            #else /* __BT_USE_CUST_DATA__ */
            scoTxParms.eScoPktType = 0x0388;
            #endif /* __BT_USE_CUST_DATA__ */
            scoTxParms.retransmissionEffort = CMGR(scoCustomParms).retransmissionEffort;
            RemDev->scoParms = CMGR_AUDIO_PARMS_ESCO;
        }
    }
    bt_trace(TRACE_GROUP_1, BTLOG_CREATE_AUDIOLINK_SCO_HANDLER, RemDev->scoHandler);
    bt_trace(TRACE_GROUP_1, BTLOG_CREATE_AUDIOLINK_SCO_HANDLER1, scoConnect);
    /* Create an eSCO link */
    status = SCO_SetupLink(scoConnect, RemDev, &scoTxParms);
    bt_trace(TRACE_GROUP_1, BTLOG_CREATE_AUDIOLINK_SCO_STATUS, status);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ConnMgrAcceptAudioLink
 * DESCRIPTION
 *  Accept eSCO or SCO connection.
 * PARAMETERS
 *  scoConnect      [IN]        
 * RETURNS
 *  (see SCO_AcceptIncomingConnection)
 *****************************************************************************/
static BtStatus ConnMgrAcceptAudioLink(BtScoConnect *scoConnect)
{
    BtStatus status;

    bt_trace(TRACE_GROUP_1, SCOCONNECTSCOLINKTYPEx02X, scoConnect->scoLinkType);
    if (scoConnect->scoLinkType == BLT_ESCO)
    {
        scoConnect->scoTxParms.transmitBandwidth = 0x1f40;  /* 8000       */
        scoConnect->scoTxParms.receiveBandwidth = 0x1f40;   /* 8000       */
        scoConnect->scoTxParms.maxLatency = CMGR(scoCustomParms).maxLatency;           /* Don't Care */
        scoConnect->scoTxParms.retransmissionEffort = 0x01; /* Don't Care */
        scoConnect->scoTxParms.voiceSetting = CMGR(scoCustomParms).voiceSetting;
        scoConnect->scoTxParms.eScoPktType = 0x0388;
    }
    else
    {
        scoConnect->scoTxParms.transmitBandwidth = CMGR(scoCustomParms).transmitBandwidth;
        scoConnect->scoTxParms.receiveBandwidth = CMGR(scoCustomParms).receiveBandwidth;
        scoConnect->scoTxParms.maxLatency = CMGR(scoCustomParms).maxLatency;
        scoConnect->scoTxParms.voiceSetting = CMGR(scoCustomParms).voiceSetting;
        scoConnect->scoTxParms.eScoPktType = 0x03C7;
        scoConnect->scoTxParms.retransmissionEffort = 0x00;
    }
    bt_trace(
        TRACE_GROUP_1,
        CONNMGRACCEPTAUDIOLINK_SCOCONNECTSCOTXPARMSESCOPKTTYPEx02X,
        scoConnect->scoTxParms.eScoPktType);
    status = SCO_AcceptIncomingConnection(scoConnect, &CMGR(scoHandler));

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  ConnMgrGetScoMaxRetryTimerCount
 * DESCRIPTION
 *  Get the max count of SCO retry timer.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static U8 ConnMgrGetScoMaxRetryTimerCount()
{
    BtRemoteDevice *remDev = 0;
    U8 count = 0;
    U8 i = 0;

    for (i = 0; (remDev = MeEnumerateRemoteDevices(i)) != 0; i++)
    {
        if ((remDev->state == BDS_CONNECTED) && 
            (ME_GetCurrentRole(remDev) == BCR_SLAVE))
        {
#ifdef __BT_USE_CUST_DATA__
            count += (U32)btGetDevCustvalueById(remDev->devId, CUSTID_ROLE_SWITCH_RETRY_COUNT);
#else
            count += 1;
#endif            
            if (ME_GetCurrentMode(remDev) == BLM_SNIFF_MODE)
            {        
                count += 3;
            }
        }
    } 
    return count;
}

/*****************************************************************************
 * FUNCTION
 *  ConnMgrScoRetryTimeout
 * DESCRIPTION
 *  Delayed creation of the audio link.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ConnMgrScoRetryTimeout(EvmTimer *timer)
{
    BtScoConnect *scoCon;
    BOOL scatternetStatus = FALSE;
    BtRemoteDevice *remDev = 0;
    U8 piconet_no, scatternet_no, sco_no, esco_no, msco_no, ssco_no;

    remDev = (BtRemoteDevice*) timer->context;
    OS_Report("CMGR CMGR(scoRetryTimerType):%02x", CMGR(scoRetryTimerType));
    if (CMGR(scoRetryTimerType) == BT_CMGR_SCO_RETRY_WAIT_ESCO)
    {
        Report(("3 scoCon->scord->scoParms"));
        CMGR(scoRetryTimer).context = 0;
        if (remDev->scoParms == CMGR_AUDIO_PARMS_ESCO)
        {
            /* BT_CMGR_SCO_RETRY_WAIT_ESCO is set while crete eSCO failed, we'll try SCO*/
            remDev->scoParms = CMGR_AUDIO_PARMS_SCO;
        }
        scatternetStatus = BTBMChangeScattnetRoleForSCO(remDev);
        if (scatternetStatus == TRUE)
        {
            Report(("SCO wait for Role Switch retry for SCO"));
            CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ROLE_SWITCH;
            CMGR(scoRetryTimer).context = remDev;
            CMGR(scoTryryTimerCount)++;
            CMGR(scoMaxRetryTimerCount) = ConnMgrGetScoMaxRetryTimerCount();
            EVM_StartTimer(&CMGR(scoRetryTimer), 300);
            return;
        }
        else if (remDev->switchState != 0x00)
        {
            CMGR(scoRetryTimer).context = remDev;
            CMGR(scoTryryTimerCount)++;
            EVM_StartTimer(&CMGR(scoRetryTimer), 100);
            return;
        }
    }
    else
    {
        Report(("SCO wait for Role switch count:%d,remDev=%04X", CMGR(scoTryryTimerCount),remDev));
        CMGR(scoRetryTimer).context = 0;

        /* ALPS00119675 : BMW X5 Carkit connect HFP to ALPS, make a outgoing call. The ringtone only alarm once.
           Cause : We trigger one more role switch.
           Solution : If retry count is reach the maximun, do not call BTBMChangeScattnetRoleForSCO because it will trigger role switch.
         */
        if ((CMGR(scoTryryTimerCount) < CMGR(scoMaxRetryTimerCount))
            && ((scatternetStatus = BTBMChangeScattnetRoleForSCO(remDev)) == TRUE)
            )
        {
            Report(("SCO wait for Role Switch retry for SCO"));
            CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ROLE_SWITCH;
            CMGR(scoRetryTimer).context = remDev;
            CMGR(scoTryryTimerCount)++;
            EVM_StartTimer(&CMGR(scoRetryTimer), 300);
            return;
        }

        BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &msco_no, &ssco_no);
        if (scatternet_no > 0)
        {
            if ((scatternet_no > 1) || ((piconet_no != 0) && (scatternet_no == 1)))
            {
                /* ALPS00119675 : BMW X5 Carkit connect HFP to ALPS, make a outgoing call. The ringtone only alarm once.
                   Cause : We trigger one more role switch.
                   Solution : If retry count is reach the maximun, do not call BTBMChangeScattnetRoleForSCO because it will trigger role switch.
                */
                if (CMGR(scoTryryTimerCount) < CMGR(scoMaxRetryTimerCount))
                {
                    Report(("SCO wait for Role Switch retry 1"));
                    BTBMChangeScattnetRoleForSCO(remDev);                    
                    CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ROLE_SWITCH;
                    CMGR(scoRetryTimer).context = remDev;
                    CMGR(scoTryryTimerCount)++;
                    EVM_StartTimer(&CMGR(scoRetryTimer), 300);
                }
                else
                {
                    Report(("SCO wait for Role Switch retry 2"));
                    if (ConnMgrScoFakeNotify(remDev, 0) != TRUE)
                    {
                        Assert(0);
                    }
                    ConnMgrScoClearState();
                }
                return;
            }
            else
            {
                Report(("Only 1 scatternet, can create SCO"));
            }
        }
    }
    ConnMgrScoClearState();
    if (remDev->state == BDS_CONNECTED)
    {
        bt_trace(TRACE_GROUP_1, BTCURRENT_SCOPARMSx02X, remDev->scoParms);
        Report(("remDev->scoParms 2:%d", remDev->scoParms));
        ConnMgrCreateAudioLink(&scoCon, remDev, remDev->scoParms);
    }

}

#endif /* NUM_SCO_CONNS > 0 */


/*****************************************************************************
 * FUNCTION
 *  ConnManager
 * DESCRIPTION
 *  Tracks and manages the ACL link.
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ConnManager(const BtEvent *Event)
{
    BtStatus status = BT_STATUS_FAILED;
    CmgrHandler *cmgrHandler = 0;
    CmgrHandler *temp_cmgrHandler;

    Report(("ConnManager: event=%d", Event->eType));
    switch (Event->eType)
    {

        case BTEVENT_LINK_CONNECT_IND:

            if (Event->handler == &CMGR(globHandler))
            {
                if (Event->errCode == BEC_NO_ERROR)
                {
                    CMGR(linkCount)++;
                    status = BT_STATUS_SUCCESS;
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }

                cmgrHandler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
                while (cmgrHandler != (CmgrHandler*) & CMGR(handlerList))
                {
                    Assert(cmgrHandler != 0);
                    temp_cmgrHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);
                    if (!ConnMgrIsLinkUp(cmgrHandler))
                    {
                        cmgrHandler->remDev = Event->p.remDev;
                        cmgrHandler->errCode = Event->errCode;
                        Assert(cmgrHandler->callback != 0);
                        cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_CON_IND, BT_STATUS_SUCCESS);
                    }

                    cmgrHandler = temp_cmgrHandler;
                }

                if (Event->errCode == BEC_NO_ERROR)
                {
                    /* Start sniff timer if it has been set */
                    ConnMgrCheckStartSniffTimer(Event->p.remDev);

                #if NUM_SCO_CONNS > 0
                    /* Start with the highest possible audio settings */
                    CMGR(scoParms) = CMGR(scoDefaultParms);
                #endif /* NUM_SCO_CONNS > 0 */
                }
            }
            break;
        case BTEVENT_LINK_DISCONNECT:

            cmgrHandler = ContainingRecord(Event->handler, CmgrHandler, btHandler);

            if (Event->handler == &CMGR(globHandler))
            {
                Assert(CMGR(linkCount) > 0);
                CMGR(linkCount)--;
            }
            //cmgrHandler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
            if ((CMGR(scoTimerDevice) != 0) && (Event->p.remDev == CMGR(scoTimerDevice)))
            {
                ConnMgrScoClearState();
                if(ConnMgrScoFakeNotify(Event->p.remDev,  NULL) != TRUE){
                    Assert(0);
                }
            }
            
            if (IsNodeOnList(&CMGR(handlerList), &cmgrHandler->node) && cmgrHandler->bdc && cmgrHandler->bdc->link){
                CMGR_ClearSniffTimer(cmgrHandler);
                cmgrHandler->remDev = Event->p.remDev;
                cmgrHandler->errCode = Event->errCode;
                Assert(cmgrHandler->callback != 0);
                cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_DIS, BT_STATUS_SUCCESS);
                cmgrHandler->bdc = 0;
            }
#if 0            
            while (cmgrHandler != (CmgrHandler*) & CMGR(handlerList))
            {
                Assert(cmgrHandler != 0);
                temp_cmgrHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);
                if ((cmgrHandler->bdc != 0) &&
                    (OS_MemCmp(Event->p.remDev->bdAddr.addr, 6, cmgrHandler->bdc->addr.addr, 6)))
                {
                    CMGR_ClearSniffTimer(cmgrHandler);
                    cmgrHandler->remDev = Event->p.remDev;
                    cmgrHandler->errCode = Event->errCode;
                    Assert(cmgrHandler->callback != 0);
                    cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_DIS, BT_STATUS_SUCCESS);
                    cmgrHandler->bdc = 0;
                }
                cmgrHandler = temp_cmgrHandler;
            }
#endif            
            break;
        case BTEVENT_ROLE_CHANGE:
            if (Event->handler == &CMGR(globHandler))
            {
                if (Event->p.roleChange.newRole == BCR_MASTER)
                {
                    bt_trace(TRACE_GROUP_1, CMGR_ROLE_IS_MASTER);
                    }
                    else
                    {
                        bt_trace(TRACE_GROUP_1, CMGR_ROLE_IS_SLAVE);
                }
            }
            break;

        case BTEVENT_LINK_CONNECT_CNF:

            cmgrHandler = ContainingRecord(Event->handler, CmgrHandler, btHandler);

            if (Event->handler == &CMGR(globHandler))
            {
                if (Event->errCode == BEC_NO_ERROR)
                {
                    CMGR(linkCount)++;
                }
            }
            else
            {
                if (Event->errCode == BEC_NO_ERROR)
                {

                    /* Connection connection was successful */
                    status = BT_STATUS_SUCCESS;
                }
                else
                {
                    /* Connection connection failed, no retry */
                    if ((Event->errCode == 0x56) || (Event->errCode == 0x57) || (Event->errCode == 0x09))
                    {
                        status = BT_STATUS_SCO_REJECT;
                    }
                    else if (Event->errCode == 0xF1)
                    {
                        status = BT_STATUS_CHIP_REASON;
                    }
                    else
                    {
                        status = BT_STATUS_FAILED;
                    }
                }

                cmgrHandler->remDev = Event->p.remDev;
                cmgrHandler->errCode = Event->errCode;
                cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_CON_CNF, status);

                if (status == BT_STATUS_SUCCESS)
                {
                    /* Start sniff timer if it has been set */
                    if (IsNodeOnList(&CMGR(handlerList), &cmgrHandler->node) && cmgrHandler->bdc && cmgrHandler->bdc->link)
                    {
                        /* The handler is still registered */
                        ConnMgrCheckStartSniffTimer(cmgrHandler->bdc->link);
                    }
                }else{
                    cmgrHandler->bdc = 0;
                }
            }
            break;
#if 0
        case BTEVENT_DEVICE_SELECTED:

            cmgrHandler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
            while (cmgrHandler != (CmgrHandler*) & CMGR(handlerList))
            {

                if (cmgrHandler->flags & CMGR_FLAG_SELECTING)
                {
                    /* Device selected, complete the connection */

                    cmgrHandler->flags &= ~CMGR_FLAG_SELECTING;
                    cmgrHandler->errCode = Event->errCode;

                    nextHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);

                    if (Event->errCode == BEC_NO_ERROR)
                    {
                        /* Continue on with trying to create ACL */
                        Assert(Event->p.select != 0);
                        Assert(Event->p.select->result != 0);

                        cmgrHandler->bdc = Event->p.select->result;

                        status = ME_CreateLink(
                                    &cmgrHandler->btHandler,
                                    &cmgrHandler->bdc->addr,
                                    &cmgrHandler->bdc->psi,
                                    &cmgrHandler->bdc->link);

                        if (status == BT_STATUS_SUCCESS)
                        {
                            cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_CON_CNF, BT_STATUS_SUCCESS);
                        }
                        else if (status != BT_STATUS_PENDING)
                        {
                            cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_CON_CNF, BT_STATUS_FAILED);
                        }
                    }
                    else
                    {
                        /* Connection attempt failed */
                        cmgrHandler->callback(cmgrHandler, CMEVENT_DATA_LINK_CON_CNF, BT_STATUS_FAILED);
                    }
                }
                else
                {
                    nextHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);
                }

                cmgrHandler = nextHandler;
            }
            break;
#endif
        #if NUM_SCO_CONNS > 0
        case BTEVENT_SCO_CONNECT_IND:
            /* SCO is connected */
            ConnMgrScoClearState();

            if (Event->errCode == BEC_NO_ERROR)
            {
                status = BT_STATUS_SUCCESS;
                if (Event->p.scoConnect.scoLinkType == BLT_ESCO)
                {
                    Event->p.scoConnect.remDev->supportESCO = 1;
                }
            }
            else
            {
                status = BT_STATUS_FAILED;
            }

            /* Report the SCO connection */
            if(ConnMgrScoNotify(Event, CMEVENT_AUDIO_LINK_CON, status) != TRUE)
            {
                Assert(0);
            }
            break;

        case BTEVENT_SCO_CONNECT_REQ:
            /* Incoming SCO connection */
            ConnMgrScoClearState();
            if (BTBMCheckAllowScoConnectReq() == 0)
            {
                SCO_RejectIncomingConnection(Event->p.scoConnect.scoCon, &CMGR(scoHandler), BEC_POWER_OFF);
            }
#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
            else if(CMGR(DualPCMSCOAeeptValue) == 0)
            {
                SCO_RejectIncomingConnection(Event->p.scoConnect.scoCon, &CMGR(scoHandler), BEC_POWER_OFF);
            }
#endif /* __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__ */
            else
            {
                if (ConnMgrIsAudioUp(Event->p.scoConnect.scoCon->scord))
                {
                    /* Audio is up already, reject this request */
                    SCO_RejectIncomingConnection(Event->p.scoConnect.scoCon, &CMGR(scoHandler), BEC_LIMITED_RESOURCE);
                    return;
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, AUDIO_LINK_TYPEBT_DEBUGx02X, Event->p.scoConnect.scoCon->scoLinkType);
                    if (!ConnMgrScoNotify(Event, CMEVENT_AUDIO_LINK_REQ, BT_STATUS_SUCCESS))
                    {
                        /* Accept the SCO connection */
                        status = ConnMgrAcceptAudioLink(Event->p.scoConnect.scoCon);
                        CMGR(scoRequest) = FALSE;
                    }
                }
            }
            break;

        case BTEVENT_SCO_CONNECT_CNF:
            /* Report the SCO connection */
            ConnMgrScoClearState();
            if (Event->errCode == BEC_NO_ERROR)
            {
                status = BT_STATUS_SUCCESS;
                BTBMChangeAllLinkPolcy(Event->p.scoConnect.scoCon->scord, TRUE);
            }
            else
            {
                if ((Event->p.scoConnect.scoCon->scord->scoParms == CMGR_AUDIO_PARMS_SCO) ||
                    (Event->p.scoConnect.scoCon->scoTxParms.isExtendedSco != 0))
                {
                    bt_trace(TRACE_GROUP_1, BTEVENT_SCO_CONNECT_CNF_SCO_FAILED);
                    status = BT_STATUS_FAILED;
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, BTEVENT_SCO_CONNECT_CNF_ESCO_FAILED);
                    Event->p.scoConnect.remDev->supportESCO = 0;
                    if (Event->p.scoConnect.remDev->state == BDS_CONNECTED)
                    {
                        ConnMgrScoClearState();
                        CMGR(scoRetryTimer).context = Event->p.scoConnect.remDev;
                        CMGR(scoTimerDevice) = Event->p.scoConnect.remDev;
                        CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ESCO;
                        EVM_StartTimer(&CMGR(scoRetryTimer), 3);
                        return;
                    }
                }
            }

            if(ConnMgrScoNotify(Event, CMEVENT_AUDIO_LINK_CON, status) != TRUE)
            {
                Assert(0);
            }

            if (Event->errCode != BEC_NO_ERROR)
            {
                /* Start sniff timer if it has been set */
                if((Event->p.scoConnect.scoCon !=0) &&
                    (Event->p.scoConnect.scoCon->scord!=0))
                    ConnMgrCheckStartSniffTimer(Event->p.scoConnect.scoCon->scord);
                else if(Event->p.scoConnect.remDev !=0)
                    ConnMgrCheckStartSniffTimer(Event->p.scoConnect.remDev);
            }
            break;

        case BTEVENT_SCO_DISCONNECT:
            /* SCO connection is down */

            if (Event->errCode == BEC_NO_ERROR)
            {
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = BT_STATUS_FAILED;
            }
            BTBMChangeAllLinkPolcy(Event->p.scoConnect.scoCon->scord, FALSE);
            if (ConnMgrScoNotify(Event, CMEVENT_AUDIO_LINK_DIS, status) != TRUE)
            {
                Assert(0);
            }
        
            /* Start sniff timer if it has been set */
            ConnMgrCheckStartSniffTimer(Event->p.scoConnect.scoCon->scord);

            break;

        #if BT_SCO_HCI_DATA == XA_ENABLED
        case BTEVENT_SCO_DATA_CNF:

            if (Event->errCode == BEC_NO_ERROR)
            {
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = BT_STATUS_FAILED;
            }

            if (ConnMgrScoNotify(Event, CMEVENT_AUDIO_DATA_SENT, status) != TRUE)
            {
                Assert(0);
            }
            break;
        case BTEVENT_SCO_DATA_IND:
            if (ConnMgrScoNotify(Event, CMEVENT_AUDIO_DATA, BT_STATUS_SUCCESS) != TRUE)
            {
                Assert(0);
            }
            break;
        #endif /* BT_SCO_HCI_DATA == XA_ENABLED */
        #endif /* NUM_SCO_CONNS > 0 */

        case BTEVENT_MODE_CHANGE:
            /* Sniff mode change */
            bt_trace(TRACE_GROUP_1, CONNMGR_BTEVENT_MODE_CHANGE);
            status = BT_STATUS_SUCCESS;
            if (Event->p.modeChange.curMode == BLM_ACTIVE_MODE)
            {

                /* Indicate that sniff mode was exited */
                if (Event->handler != &CMGR(globHandler))
                {
                    cmgrHandler = ContainingRecord(Event->handler, CmgrHandler, btHandler);
                    cmgrHandler->errCode = Event->errCode;
                    bt_trace(
                        TRACE_GROUP_1,
                        CURRENT_MODE_BT_DEBUGx02XERROR_CODEBT_DEBUGx02X,
                        Event->p.modeChange.curMode,
                        Event->errCode);
                    if (!IsNodeOnList(&CMGR(handlerList), &cmgrHandler->node))
                    {
                        /* The handler is no longer registered */
                        return;
                    }
                }
                /* Start sniff timer if it has been set */
                if (Event->handler == &CMGR(globHandler))
                {
                    bt_trace(TRACE_GROUP_1, SNIFFER_TIMER_RESTART_AGAIN);
                    ConnMgrCheckStartSniffTimer(Event->p.modeChange.remDev);
                    return;
                }

                else
                {
                    bt_trace(TRACE_GROUP_1, NOT_GOOBALHANDLER);
                }

            #if NUM_SCO_CONNS > 0
                Assert(cmgrHandler != 0);
                if (cmgrHandler->flags & CMGR_FLAG_STARTING_SCO)
                {
                    cmgrHandler->flags &= ~CMGR_FLAG_STARTING_SCO;
                    bt_trace(TRACE_GROUP_1, TRY_TO_OPEN_SCO_LINK);
                    /* Create the audio link */
                    if (BTBMChangeScattnetRoleForSCO(cmgrHandler->bdc->link) == TRUE)
                    {
                        ConnMgrScoClearState();
                        CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ROLE_SWITCH;
                        CMGR(scoRetryTimer).context = cmgrHandler->bdc->link;
                        CMGR(scoTimerDevice) = cmgrHandler->bdc->link;
                        CMGR(scoTryryTimerCount)++;
                        CMGR(scoMaxRetryTimerCount) = ConnMgrGetScoMaxRetryTimerCount();                        
                        EVM_StartTimer(&CMGR(scoRetryTimer), 300);
                        status = BT_STATUS_PENDING;
                    }
                    else
                    {
                        Report(("ConnMgrCreateAudioLink a"));
                        status = ConnMgrCreateAudioLink(
                                    &cmgrHandler->scoConnect,
                                    cmgrHandler->bdc->link,
                                    cmgrHandler->bdc->link->scoParms);

                    }

                    if (status != BT_STATUS_PENDING)
                    {
                        /* Can't start SCO link, report failure */
                        Assert((cmgrHandler->callback != 0));
                        cmgrHandler->callback(cmgrHandler, CMEVENT_AUDIO_LINK_CON, status);
                    }
                    else
                    {
                        return;
                    }
                }
            #endif /* NUM_SCO_CONNS > 0 */
            }
            else
            {
                /* Indicate that sniff mode was entered */
                bt_trace(TRACE_GROUP_1, ENTER_SNIFF_MODE);
                /* Disable the sniffer timer */
            #if defined (__BT_2_1_SNIFF_SUBRATING__)
                if ((Event->p.modeChange.remDev->remoteFeature[5] & 0x02) != 0)
                {
                    ME_StartSniffSubrating(Event->p.modeChange.remDev);
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, BTLOG_NOT_SUPPORT_SNIFF_SUBRATING_MODE);
                }
            #endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 

                ConnMgrGetMaxSniffTime(Event->p.modeChange.remDev);
            #ifdef MTK_BB_TESTING
                if (BTBBTestingCases(BTBBTESTING_HEADSET_ALWAYS_UNSNIFF))
                {
                    ME_StopSniff(Event->p.modeChange.remDev);
                }
            #endif /* MTK_BB_TESTING */ 
            }
            break;
        case BTEVENT_ACL_DATA_NOT_ACTIVE:

            /* Trying to send data while in Park Mode */
            if (ME_GetCurrentMode(Event->p.remDev) == BLM_PARK_MODE)
            {
                /* Stop Park Mode */
                (void)ME_StopPark(Event->p.remDev);
            }
            else if (ME_GetCurrentMode(Event->p.remDev) == BLM_SNIFF_MODE)
            {

                cmgrHandler = ConnMgrGetFirstHandlerByRemDev(Event->p.remDev);
                /* Search for a connection that requires exiting sniff mode */
                Report(("The cmgrHandler is :%d",cmgrHandler));
                if (cmgrHandler && (ConnMgrGetSniffExitPolicy(cmgrHandler) & CMGR_SNIFF_EXIT_ON_SEND))
                {

                    /* Stop Sniff Mode */
                    (void)ME_StopSniff(Event->p.remDev);
                }

			#if defined (__SDP_EXIT_SNIFF_MODE__)
                else if(cmgrHandler == 0)
                {
                    U8 stopSniffStatus;
                       
                    stopSniffStatus = ME_StopSniff(Event->p.remDev);
                    Report(("The stopSniffStatus result:%d",stopSniffStatus));
                }
			#endif /* __SDP_EXIT_SNIFF_MODE__ */

            }
            break;

        case BTEVENT_READ_RSSI_COMPLETED:
            bt_trace(TRACE_GROUP_1, CONNMGR_BTEVENT_READ_RSSI_COMPLETED);
            cmgrHandler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
            while (cmgrHandler != (CmgrHandler*) & CMGR(handlerList))
            {
                Assert(cmgrHandler != 0);
                temp_cmgrHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);
                if ((cmgrHandler->bdc != 0) &&
                    (OS_MemCmp(Event->p.rssi.remDev->bdAddr.addr, 6, cmgrHandler->bdc->addr.addr, 6)))
                {
                    cmgrHandler->remDev = Event->p.rssi.remDev;
                    cmgrHandler->remDev->rssi = Event->p.rssi.RSSI;
                    cmgrHandler->errCode = Event->errCode;
                    cmgrHandler->callback(cmgrHandler, CMEVENT_READ_RSSI_COMPLETED, BT_STATUS_SUCCESS);
                }
                cmgrHandler = temp_cmgrHandler;
            }
            {
                /* Send message back to MOD_MMI */
                bt_trace(TRACE_GROUP_1,BTLOG_CONNMANAGER_RSSI_VAL,Event->p.rssi.RSSI);
            }   
            break;
    
        case BTEVENT_READ_TX_POWER_COMPLETED:
            Report(("Read Tx Power done"));
            cmgrHandler = (CmgrHandler*) GetHeadList(&CMGR(handlerList));
            while (cmgrHandler != (CmgrHandler*) & CMGR(handlerList))
            {
                Assert(cmgrHandler != 0);
                temp_cmgrHandler = (CmgrHandler*) GetNextNode(&cmgrHandler->node);
                if ((cmgrHandler->bdc != 0) &&
                    (OS_MemCmp(Event->p.txPower.remDev->bdAddr.addr, 6, cmgrHandler->bdc->addr.addr, 6)))
                {
                    cmgrHandler->remDev = Event->p.rssi.remDev;
                    cmgrHandler->remDev->txPower = Event->p.txPower.txPower;
                    cmgrHandler->errCode = Event->errCode;
                    cmgrHandler->callback(cmgrHandler, CMEVENT_READ_TX_POWER_COMPLETED, BT_STATUS_SUCCESS);
                }
                cmgrHandler = temp_cmgrHandler;
            }
            break;
          
    }
}

/*---------------------------------------------------------------------------
 *            CMGR_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the connection manager
 *
 * Return:    (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  CMGR_Init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL CMGR_Init(void)
{
    /* Register the Connection Manager as global handler */
    ME_InitHandler(&CMGR(globHandler));
    CMGR(globHandler).callback = ConnManager;
    CMGR(linkCount) = 0;
    ME_RegisterGlobalHandler(&CMGR(globHandler));
    ME_SetEventMask(
        &CMGR(globHandler),
        BEM_ROLE_CHANGE | BEM_MODE_CHANGE | BEM_LINK_CONNECT_IND | BEM_LINK_CONNECT_CNF | BEM_LINK_DISCONNECT);

    InitializeListHead(&CMGR(handlerList));
#if NUM_SCO_CONNS > 0
    CMGR(scoRequest) = FALSE;
    CMGR(scoDefaultParms) = CMGR_AUDIO_DEFAULT_PARMS;
    CMGR(scoParms) = CMGR(scoDefaultParms);
    CMGR(scoCustomParms).voiceSetting = BSAS_DEFAULT;
    CMGR(scoCustomParms).maxLatency = 0x000F;
    CMGR(scoCustomParms).retransmissionEffort = 0x01;
    CMGR(scoCustomParms).eScoPktType = BESPT_EV3;
    CMGR(scoCustomParms).transmitBandwidth = 0x1f40;
    CMGR(scoCustomParms).receiveBandwidth = 0x1f40;
    CMGR(scoRetryTimer).func = ConnMgrScoRetryTimeout;
    CMGR(createLinkCount) = 0;
#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
    CMGR(DualPCMSCOAeeptValue) = 1;
#endif /* __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__ */
    ConnMgrScoClearState();

#endif /* NUM_SCO_CONNS > 0 */
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  CMGR_Init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
void CMGR_Deinit(void){
    /* Clear handlerList to prevent profiles to clear context without deregister first */
    bt_prompt_trace(MOD_BT, "CMGR_Deinit : clear CMGR(handlerList)");
    InitializeListHead(&CMGR(handlerList));
}

/*****************************************************************************
 * FUNCTION
 *  CMGR_RegisterHandler
 * DESCRIPTION
 *  Registers and initalizes the connection manager handler
 * PARAMETERS
 *  Handler         [IN]        
 *  Callback        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_RegisterHandler(CmgrHandler *Handler, CmgrCallback Callback)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtSniffInfo sniffInfo;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Callback != 0);

    OS_LockStack();

    if (Handler && Callback)
    {
        if (!IsNodeOnList(&CMGR(handlerList), &Handler->node))
        {
            /* Initialize the handler */
            Report(("CMGR add handler:0x%x", Handler));
            Handler->bdc = 0;
            Handler->callback = Callback;

            /* Initialize sniff timer */
            Handler->sniffPolicy = CMGR_DEFAULT_SNIFF_EXIT_POLICY | CMGR_SNIFF_EXIT_ON_SEND;
            Handler->sniffTimeout = 0;
            OS_MemSet((U8*) & Handler->sniffInfo, 0, sizeof(BtSniffInfo));
            /* Start the sniff timer */
            sniffInfo.minInterval = MTK_SNIFF_MIN_INTERVAL;
            sniffInfo.maxInterval = MTK_SNIFF_MAX_INTERVAL;
            sniffInfo.attempt = MTK_SNIFF_ATTEMPT;
            sniffInfo.timeout = MTK_SNIFF_TIMEOUT;
            Handler->sniffTimer.func = ConnMgrSniffTimeoutHandler;
            Handler->sniffTimer.context = 0;
            Handler->sniffTimerActivated = 0;
            /* Put it on the list of registered handlers */
            InsertTailList(&CMGR(handlerList), &Handler->node);
            CMGR_SetSniffTimer(Handler, &sniffInfo, MTK_SNIFF_TIMER);

        }
        else
        {
            status = BT_STATUS_IN_USE;
        }
    }
    else
    {
        status = BT_STATUS_INVALID_PARM;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_DeregisterHandler
 * DESCRIPTION
 *  Deregisters a connection manager handler
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_DeregisterHandler(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (Handler)
    {
        if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
        {
            ConnMgrRemoveDataLink(Handler);
            /* Remove the handler */
            (void)CMGR_ClearSniffTimer(Handler);
            RemoveEntryList(&Handler->node);
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_INVALID_PARM;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_CreateDataLink
 * DESCRIPTION
 *  Create the ACL link
 * PARAMETERS
 *  Handler     [IN]        
 *  Addr        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_CreateDataLink(CmgrHandler *Handler, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_FAILED;
    BtDeviceContext tmplt;
    U8 piconet_no, scatternet_no, sco_no, esco_no, master_sco_no, slave_sco_no;
    BOOL addDevice = FALSE;
    BtRemoteDevice *link;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_Report("[CMGR][CON] CMGR_CreateDataLink=%p", Handler);
    
    if(BTQueryPowerOnState() == 0)
    {
        Report(("BT not in initialize state"));
        return BT_STATUS_FAILED;
    }
    OS_LockStack();
    if (BTAccCtrlCheckConnectPolicy(Addr) == FALSE)
    {
        status = BT_STATUS_RESTRICTED;
        goto exit;
    }

    BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &master_sco_no, &slave_sco_no);

    /* Reject for SCO quality. If destined to connected device, ACL'd be reused which won't affect SCO much */
//    if ((esco_no != 0 || (sco_no != 0 && master_sco == FALSE)) &&
    if (slave_sco_no > 0 && ME_FindRemoteDevice(Addr) == NULL)
    {
        status = BT_STATUS_CHIP_REASON;
        goto exit;
    }
#if defined (__BT_2_1_BLOCK_DEVICE__)
    if (BTBMCheckBlockList(Addr->addr) == TRUE)
    {
        status = BT_STATUS_RESTRICTED;
        goto exit;
    }
#endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 
    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        if((CMGR(createLinkCount) != 0) && (random() % 3) == 0 )
        {
#ifdef __BT_DLIGHT1_DEBUG__
            Report(("autumn connection already connected"));
#endif        
            status = BT_STATUS_SUCCESS;
            return status;
        }    
        if (!ConnMgrIsLinkUp(Handler))
        {
            link = ME_FindRemoteDevice(Addr);
            if((link !=0) && (link->state == BDS_CONNECTED))
            {
                Report(("CMGR Connected"));
            }
            //else
            //{
                //if (btbmCheckCreateLinkKeyState() == TRUE)
                //{
                    //status = BT_STATUS_FAILED;
                    //goto exit;
                //}
            //}
            Handler->bdc = 0;
            Handler->flags = 0;
            Handler->errCode = 0;

            /* Connect to the specified device */
            ME_InitHandler(&Handler->btHandler);
            Handler->btHandler.callback = ConnManager;

            /* Find a device (either discovered or in DDB */
            Handler->bdc = DS_FindDevice(Addr);
            if (!Handler->bdc)
            {

                if (Addr != 0)
                {
                    OS_MemSet((U8*) & tmplt, 0, (U32) sizeof(BtDeviceContext));
                    OS_MemCopy((U8*) & tmplt.addr, (U8*) Addr, sizeof(BD_ADDR));
                    status = DS_AddDevice(&tmplt, &Handler->bdc);
                    if (status  == BT_STATUS_SUCCESS)
                    {
                        addDevice = TRUE;
                    }
                }
                else
                {
                    Assert(0);
                }

                if (status != BT_STATUS_SUCCESS)
                {
                    goto exit;
                }
            }

            /* Create the ACL */
#ifdef __BT_4_0_BLE__
            if(Handler->bdc->devType == BT_DEV_TYPE_LE)
            {
                BtLELinkParm bleParms;

                /*
                bleParms.connIntervalMax = 0x38;
                bleParms.connIntervalMin = 0x28;
                */
                bleParms.connIntervalMax = (U16)(U32)btGetCustvalue(CUSTID_LE_CONN_INTERVAL_MAX);
                bleParms.connIntervalMin = (U16)(U32)btGetCustvalue(CUSTID_LE_CONN_INTERVAL_MIN);                 
                bleParms.privacy = FALSE;
                /*
                bleParms.scanInterval = 0x48;
                bleParms.scanWindow = 0x30;
                */ 
                bleParms.scanInterval = (U16)(U32)btGetCustvalue(CUSTID_LE_INITIATE_INTERVAL);
                bleParms.scanWindow = (U16)(U32)btGetCustvalue(CUSTID_LE_INITIATE_WINDOW);                
                status = ME_LE_CreateLink(&Handler->btHandler, &Handler->bdc->addr, &bleParms, &Handler->bdc->link);

            }
            else
                status = ME_CreateLink(&Handler->btHandler, &Handler->bdc->addr, &(Handler->bdc->psi), &Handler->bdc->link);
#else
            status = ME_CreateLink(&Handler->btHandler, &Handler->bdc->addr, &(Handler->bdc->psi), &Handler->bdc->link);

#endif
            if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING))
            {
                /* 
                  * MAUI_01229156:
                  * In this case: The action "InsertTailList(&(remDev->handlers), &(handler->node));" is not executed 
                  * But for profiles, if one race condition happens: Incoming con req waits for MMI accepting, a2dp tries to con req to same headset background 
                  * e.g., A2dp will invoke CMGR_RemoveDataLink() -> ME_DisconnectLink() if status code with BT_STATUS_NO_RESOURCES is returned and Handler->bdc !=0
                  * MAUI_01531783:
                  * ME_CreareLink() is failure to this acl already requests to disconnect:
                  * cmgrhdl->btHandler will be inserted then removed from remDev->handlers
                  * In this case, it should also reset Handler->bdc to zero!
                  * IN GENERAL SPEAKING:
                  * -> If resturned status is not BT_STATUS_SUCCESS nor BT_STATUS_PENDING, bdc must reset to zero:
                  *      because cmgrhdl->btHandler is not in remDev->handlers !
                  *      Even if app invokes CMGR_RemoveDataLink() due to failure of CMGR_CreateDataLink():
                  *      It will not cause assertion because app invokes CMGR_RemoveDataLink() -> ME_DisconnectLink(): No handler in remDev->handler     
                  */
                bt_trace(TRACE_GROUP_1,BTLOG_CMGR_CREATE_LINK_NO_OK,status);  
                if (addDevice)
                {
                    BtStatus deleted_status = BT_STATUS_FAILED;
                    deleted_status = DS_DeleteDevice(Addr);
                    bt_trace(TRACE_GROUP_1,BTLOG_CMGR_CREATE_LINK_NO_OK,0);
                    bt_trace(TRACE_GROUP_1,BTLOG_CMGR_CREATE_LINK_NO_OK,deleted_status);
                }
                Handler->btHandler.callback = 0;
                /* Reset Handler->bdc = 0 to avoid a2dp invoking CMGR_RemoveDataLink() because link is not established */
                Handler->bdc = 0;
                  
            }
            
        }
        else
        {
            /* 
              * For MAUI_01477282: SyncML OBEX Srv recvs RFEVENT_CLOSED for A, but ACL link for A is still connected
              * Then, RFEVENT_OPEN_IND for B is recved by SyncML OBEX Srv, at this moment:
              * OBEX Srv invokes CMGR_CreateDataLink(): finds Handler->bdc->link->state is BDS_CONNECTED:
              * but it should not accept this RFCOMM CON Req from B: because the device is different!
              * Btstack should not return SUCCESS for OBEX Srv to let it misunderstand that 
              * => Handler->bdc and Handler->remDev are changed to values of this new incoming device
              */
            BOOL is_same_device = OS_MemCmp(Handler->bdc->addr.addr, 6, Addr->addr,6);
            if (is_same_device == TRUE)
            {
                if (!IsNodeOnList(&(Handler->bdc->link->handlers), &(Handler->btHandler.node)))
                {
                    InsertTailList(&(Handler->bdc->link->handlers), &(Handler->btHandler.node));
                }
                Handler->bdc->link->discFlag = TRUE;
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                bt_trace(TRACE_GROUP_1,BTLOG_CMGR_CREATE_LINK_WITH_DIFF_ADDR);
                status = BT_STATUS_FAILED;
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

  exit:

    OS_UnlockStack();
    if (status == BT_STATUS_SUCCESS)
    {
        Handler->remDev = Handler->bdc->link;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_RemoveDataLink
 * DESCRIPTION
 *  Brings the ACL down
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_RemoveDataLink(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    status = ConnMgrRemoveDataLink(Handler);

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_RemoveCmgrWithDevice
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CMGR_RemoveCmgrWithDevice(CmgrHandler *Handler)
{
    CMGR_ClearSniffTimer(Handler);
    Handler->remDev = 0;
    Handler->bdc = 0;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_IsLinkUp
 * DESCRIPTION
 *  Returns the state of the data link
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL CMGR_IsLinkUp(CmgrHandler *Handler)
{
    BOOL status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();
    if((!HCI_IsValidAclPacketLen()) && ((random() % 3) == 0))
    {
        CMGR(createLinkCount) = 1;
    }
    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        if ((Handler->bdc) && (Handler->bdc->link))
        {
            status = (BOOL)((ME_GetRemDevState(Handler->bdc->link) == BDS_DISCONNECTED) ? FALSE : TRUE);
        }
        else if (Handler->remDev)
        {
            status = (BOOL)((ME_GetRemDevState(Handler->remDev) == BDS_DISCONNECTED) ? FALSE : TRUE);
        }
        else
        {
            status = FALSE;
        }
    }
    else
    {
        status = FALSE;
    }

    OS_UnlockStack();

    return status;
}


#if NUM_SCO_CONNS > 0


/*****************************************************************************
 * FUNCTION
 *  CmgrRegisterAudioHandler
 * DESCRIPTION
 *  Register a SCO handler.
 * PARAMETERS
 *  Handler     [IN]        
 *  flag        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CmgrRegisterAudioHandler(CmgrHandler *Handler, BOOL flag)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    /* Register a SCO handler */
    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {

        if (ConnMgrCountScoHandlers(Handler->bdc->link) == 0)
        {
            /* First registration */
            CMGR(scoParms) = CMGR(scoDefaultParms);
            ME_InitHandler(&CMGR(scoHandler));
            CMGR(scoHandler).callback = ConnManager;
            status = SCO_RegisterHandler(Handler->bdc->link, &CMGR(scoHandler));
        }

        if (status == BT_STATUS_SUCCESS)
        {
            Handler->flags |= CMGR_FLAG_SCO_REGISTERED;

            if (ConnMgrIsLinkUp(Handler) && (ConnMgrIsAudioUp(Handler->bdc->link)))
            {
                Handler->flags |= CMGR_FLAG_AUDIO_UP;
            }

            if (flag)
            {
                Handler->flags |= CMGR_FLAG_HANDLE_SCO;
            }
            else
            {
                Handler->flags &= ~CMGR_FLAG_HANDLE_SCO;
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CmgrDeregisterAudioHandler
 * DESCRIPTION
 *  Deregister a SCO handler.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CmgrDeregisterAudioHandler(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Unregister the SCO handler */
        if (Handler->flags & CMGR_FLAG_SCO_REGISTERED)
        {
            if (Handler->bdc && (ConnMgrCountScoHandlers(Handler->bdc->link) == 1))
            {
                status = SCO_UnregisterHandler(Handler->bdc->link, &CMGR(scoHandler));
            }

            if (status == BT_STATUS_SUCCESS)
            {
                Handler->flags &= ~(CMGR_FLAG_SCO_REGISTERED | CMGR_FLAG_HANDLE_SCO);
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_CreateAudioLink_internal
 * DESCRIPTION
 *  Create the SCO connection
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus CMGR_CreateAudioLink_internal(CmgrHandler *Handler, U8 scoParms, CmgrAudioCodec codec)
{
    BtStatus status = BT_STATUS_NO_CONNECTION;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        Handler->errCode = 0;
        if (ConnMgrIsLinkUp(Handler))
        {
            if (CMGR(scoTimerDevice) != 0)
            {
                Report(("Pervious SCO connect Request not completed"));
                status = BT_STATUS_BUSY;
                goto done;
            }
            Me_WriteAudioSetting();    
            if (!(Handler->flags & CMGR_FLAG_AUDIO_UP))
            {
                Handler->bdc->link->scoParms = scoParms;
                Handler->bdc->link->scoCodec = codec;
                Report(("Handler->bdc->link->scoParms 1:%d, codec=%d", scoParms, codec));
                if (Handler->bdc->link->switchState != 0x00)
                {
                    bt_trace(TRACE_GROUP_1, BT_IN_ROLE_SWITCH_NO_SCO);
                    status = BT_STATUS_BUSY;
                    goto done;
                }
                if (Handler->bdc->link->state != BDS_CONNECTED)
                {
                    Report(("For SCO link state:%02x", Handler->bdc->link->state));
                    status = BT_STATUS_FAILED;
                    goto done;
                }
                if (ME_GetCurrentMode(Handler->bdc->link) == BLM_ACTIVE_MODE)
                {
                    ConnMgrScoClearState();
                    if (BTBMChangeScattnetRoleForSCO(Handler->bdc->link) == TRUE)
                    {
                        ConnMgrScoClearState();
                        CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ROLE_SWITCH;
                        CMGR(scoTryryTimerCount) = 0;
                        CMGR(scoMaxRetryTimerCount) = ConnMgrGetScoMaxRetryTimerCount();                        
                        CMGR(scoRetryTimer).context = Handler->bdc->link;
                        CMGR(scoTimerDevice) = Handler->bdc->link;
                        EVM_StartTimer(&CMGR(scoRetryTimer), 300);
                        status = BT_STATUS_PENDING;
                    }
                    else
                    {
                        /* Create the audio link */
                        Report(("ConnMgrCreateAudioLink b"));
                        status = ConnMgrCreateAudioLink(
                                    &Handler->scoConnect,
                                    Handler->bdc->link,
                                    Handler->bdc->link->scoParms);

                    }

                }
                else
                {
                    if (ME_GetCurrentMode(Handler->bdc->link) == BLM_SNIFF_MODE)
                    {
                        if (ME_CheckCreatingLink())
                        {
                            status = BT_STATUS_CHIP_REASON;
                        }
                        else if (ConnMgrGetSniffExitPolicy(Handler) & CMGR_SNIFF_EXIT_ON_AUDIO)
                        {

                            /* Stop sniff mode fist */
                            Handler->flags |= CMGR_FLAG_STARTING_SCO;
                            status = ME_StopSniff(Handler->bdc->link);
                        }
                        else if (BTBMChangeScattnetRoleForSCO(Handler->bdc->link) == TRUE)
                        {
                            ConnMgrScoClearState();
                            CMGR(scoRetryTimerType) = BT_CMGR_SCO_RETRY_WAIT_ROLE_SWITCH;
                            CMGR(scoTryryTimerCount) = 0;
                            CMGR(scoMaxRetryTimerCount) = ConnMgrGetScoMaxRetryTimerCount();                            
                            CMGR(scoRetryTimer).context = Handler->bdc->link;
                            CMGR(scoTimerDevice) = Handler->bdc->link;
                            EVM_StartTimer(&CMGR(scoRetryTimer), 300);
                            status = BT_STATUS_PENDING;
                        }
                        else
                        {
                            /* Create the audio link */
                            Report(("ConnMgrCreateAudioLink c"));
                            status = ConnMgrCreateAudioLink(
                                        &Handler->scoConnect,
                                        Handler->bdc->link,
                                        Handler->bdc->link->scoParms);
                        }
                    }
                    else if (ME_GetCurrentMode(Handler->bdc->link) == BLM_PARK_MODE)
                    {

                        /* Stop park mode fist */
                        Handler->flags |= CMGR_FLAG_STARTING_SCO;
                        status = ME_StopPark(Handler->bdc->link);
                    }
                }
            }
            else
            {
                status = BT_STATUS_SUCCESS;
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }
  done:
    OS_UnlockStack();

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  CMGR_CreateAudioLink
 * DESCRIPTION
 *  Create the SCO connection
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_CreateAudioLink(CmgrHandler *Handler){
    OS_Report("CMGR_CreateAudioLink(%p)", Handler);
    return CMGR_CreateAudioLink_internal(Handler, CMGR_GetProfileSupportAudioType(), 0);
}

/* HFP 1.6 */
BtStatus CMGR_CreateAudioLink_Ex(CmgrHandler *Handler, CmgrAudioCodec codec)
{
    OS_Report("CMGR_CreateAudioLink_Ex(%p, %d)", Handler, codec);
    Assert(codec < NUM_OF_CMGR_CODEC);
    return CMGR_CreateAudioLink_internal(Handler, CMGR_AUDIO_PARMS_ESCO_CODEC, codec);
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_RemoveAudioLink
 * DESCRIPTION
 *  Bring down the SCO connection
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_RemoveAudioLink(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_NO_CONNECTION;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();
    Me_WriteAudioSetting();    
    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        if (Handler->flags & CMGR_FLAG_AUDIO_UP)
        {
            status = SCO_DisconnectLink(Handler->scoConnect);
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_IsAudioUp
 * DESCRIPTION
 *  Returns the state of the audio link
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL CMGR_IsAudioUp(CmgrHandler *Handler)
{
    BOOL status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        status = (BOOL)((Handler->flags & CMGR_FLAG_AUDIO_UP) ? TRUE : FALSE);
    }
    else
    {
        status = FALSE;
    }

    OS_UnlockStack();

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  CMGR_IsEnhancedAudioSupported
 * DESCRIPTION
 *  Check if enhanced audio is supported by chip
 * PARAMETERS
 *          
 * RETURNS
 *     TRUE - Support.
 *     FALSE - Not support.
 *****************************************************************************/
BOOL CMGR_IsEnhancedAudioSupported(void)
{
    return ((ME_IsEnhancedAudioSupported() == BT_STATUS_SUCCESS) ? TRUE : FALSE);
}

/*****************************************************************************
 * FUNCTION
 *  CMGR_GetAudioDefaultParms
 * DESCRIPTION
 *  Gets the default parameters for SCO or eSCO connections.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
CmgrAudioParms CMGR_GetAudioDefaultParms(void)
{
    return CMGR(scoDefaultParms);
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_SetAudioVoiceSettings
 * DESCRIPTION
 *  Sets the default parameters for SCO or eSCO connections.
 * PARAMETERS
 *  settings        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CMGR_SetAudioVoiceSettings(BtScoAudioSettings settings)
{
    OS_LockStack();
    Me_WriteAudioSetting();    

    CMGR(scoCustomParms).voiceSetting = settings;

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_GetAudioVoiceSettings
 * DESCRIPTION
 *  Returns current Audio Voice Settings.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtScoAudioSettings CMGR_GetAudioVoiceSettings(void)
{
    return (CMGR(scoCustomParms).voiceSetting);
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_SetAudioCustomParms
 * DESCRIPTION
 *  Sets the custom audio paraemters.
 * PARAMETERS
 *  customParms     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_SetAudioCustomParms(BtScoTxParms *customParms)
{
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, customParms != 0);

    OS_LockStack();

    OS_MemCopy((U8*) & CMGR(scoCustomParms), (U8*) customParms, sizeof(BtScoTxParms));

    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_GetProfileSupportAudioType
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 CMGR_GetProfileSupportAudioType(void)
{
#ifdef __BT_HFG_PROFILE__
    kal_bool type = KAL_FALSE;
    U8 eSCO_type = 0;

    type = hfga_is_profile_support_esco();
    bt_trace(TRACE_GROUP_1, BT_PROFILES_TYPE_FROM_MMIx02X, type);
    if (type == KAL_TRUE)
    {
        eSCO_type = CMGR_AUDIO_PARMS_ESCO;
    }
    else
    {
        eSCO_type = CMGR_AUDIO_PARMS_SCO;
    }
    bt_trace(TRACE_GROUP_1, BT_PROFILES_ESCO_TYPEx02X, eSCO_type);
    Me_WriteAudioSetting();    

    return eSCO_type;
#else /* __BT_HFG_PROFILE__ */ 
    return CMGR_AUDIO_PARMS_SCO;
#endif /* __BT_HFG_PROFILE__ */ 
}
#endif /* NUM_SCO_CONNS > 0 */


/*****************************************************************************
 * FUNCTION
 *  CMGR_SetSniffTimer
 * DESCRIPTION
 *  Sets the timeout value used to put the link into sniff mode.
 *  This timer starts after a link connection is complete.  It is
 *  disabled when a SCO (or eSCO) connection is established and the
 *  sniff mode is exited.  When the SCO (or eSCO) connection is
 *  terminated the timer is started again.
 *  The sniff timeout can be set to any value, however, it is
 *  important to note that a value of 0 means that the handler
 *  doesn't care if sniff mode is used and a value of -1 means
 *  that sniff mode is disabled for the specified handler (which
 *  also disables sniff mode for all handlers).
 * PARAMETERS
 *  Handler         [IN]        
 *  SniffInfo       [IN]        
 *  Time            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_SetSniffTimer(CmgrHandler *Handler, BtSniffInfo *SniffInfo, TimeT Time)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, SniffInfo != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Save sniff timer info */
        Handler->sniffTimeout = Time;
        OS_MemCopy((U8*) & Handler->sniffInfo, (U8*) SniffInfo, sizeof(BtSniffInfo));
        if (Handler->bdc)
        {
            ConnMgrCheckStartSniffTimer(Handler->bdc->link);
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_ActivateSniffTimer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_ActivateSniffTimer(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_SUCCESS;

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Save sniff timer info */
        if (Handler->bdc)
        {
            if(CMGR(createLinkCount) != 0)
            {
#ifdef __BT_DLIGHT1_DEBUG__
                Report(("autumn CMGR_ActivateSniffTimer success"));
#endif        
                status = BT_STATUS_SUCCESS;
                return status;
            }  
            Handler->sniffTimerActivated = 1;
            ConnMgrCheckStartSniffTimer(Handler->bdc->link);
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_ClearSniffTimer
 * DESCRIPTION
 *  Clears the sniff mode timeout value.  This is equivalent to
 *  saying that the handler no longer cares if the link is put
 *  into sniff mode or not.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_ClearSniffTimer(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Handler->sniffTimeout = 0; */
        if (Handler->sniffTimer.context)
        {
            EVM_CancelTimer(&Handler->sniffTimer);
        }
        Handler->sniffTimerActivated = 0;
        Handler->sniffTimer.context = 0;
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_IndicateLinkActivity
 * DESCRIPTION
 *  Indicates that activity has occured on the link, which restarts
 *  the sniff timeout.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CMGR_IndicateLinkActivity(CmgrHandler *Handler)
{
    OS_LockStack();

    if (Handler)
    {
        if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
        {
            if (Handler->sniffTimer.context)
            {
                EVM_CancelTimer(&Handler->sniffTimer);
                ConnMgrCheckStartSniffTimer(Handler->bdc->link);
            }
        }
    }

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_DisableSniffTimer
 * DESCRIPTION
 *  Disables sniff mode for the specified handler.  This also
 *  disables sniff mode for all handlers.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_DisableSniffTimer(CmgrHandler *Handler)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Disable sniff timer */
        Handler->sniffTimeout = CMGR_SNIFF_DISABLED;
        ConnMgrCheckStartSniffTimer(Handler->bdc->link);

        /* Stop sniff mode on this link */
        (void)ME_StopSniff(Handler->bdc->link);
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}

#if defined (__BT_2_1_SNIFF_SUBRATING__)


/*****************************************************************************
 * FUNCTION
 *  CMGR_EnableSniffSubratingMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_EnableSniffSubratingMode(CmgrHandler *Handler)
{
    BtRemoteDevice *remDev;
    BtStatus status = BT_STATUS_NOT_FOUND;

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Save sniff timer info */
        if (Handler->bdc)
        {
            remDev = Handler->bdc->link;
            if (remDev->state != BDS_CONNECTED)
            {
                return BT_STATUS_FAILED;
            }
            Handler->sniffSubratingEnabled = CMGR_SNIFF_SUBRATING_ENABLED;
            if (remDev->sniffSubrateMaxLatency != MTK_SNIFF_SUBRATING_MAX_LATENCY)
            {
                remDev->sniffSubrateState |= MTK_SNIFF_SUBRATING_PARAM_CHANGED;
                remDev->sniffSubrateMaxLatency = MTK_SNIFF_SUBRATING_MAX_LATENCY;
                remDev->sniffSubrateMinLocalTimeout = MTK_SNIFF_SUBRATING_MIM_REMOTE_TIMEOUT;
                remDev->sniffSubrateMinRemoteTimeout = MTK_SNIFF_SUBRATING_MIM_LOCAL_TIMEOUT;

            }
            status = BT_STATUS_SUCCESS;
        }
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_DisableSniffSubratingMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CMGR_DisableSniffSubratingMode(CmgrHandler *Handler)
{
    BtRemoteDevice *remDev;
    BOOL enterSniffSubratingFlag = FALSE;

    CmgrHandler *list = (CmgrHandler*) GetHeadList(&CMGR(handlerList));

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        /* Save sniff timer info */
        if (Handler->bdc)
        {
            remDev = Handler->bdc->link;
            if (remDev->state != BDS_CONNECTED)
            {
                return;
            }
            Handler->sniffSubratingEnabled = CMGR_SNIFF_SUBRATING_DISABLED;
            while (list != (CmgrHandler*) & CMGR(handlerList))
            {
                if (list->bdc && (list->bdc->link == remDev))
                {
                    if (list->sniffSubratingEnabled == CMGR_SNIFF_SUBRATING_ENABLED)
                    {
                        enterSniffSubratingFlag = TRUE;
                        break;
                    }
                }

                list = (CmgrHandler*) GetNextNode(&list->node);
            }
            if ((enterSniffSubratingFlag == FALSE) && (remDev->sniffSubrateMaxLatency != 0) &&
                ((remDev->remoteFeature[5] & 0x02) != 0))
            {
                remDev->sniffSubrateMaxLatency = 0;
                remDev->sniffSubrateMinLocalTimeout = 0;
                remDev->sniffSubrateMinRemoteTimeout = 0;
                remDev->sniffSubrateState |= MTK_SNIFF_SUBRATING_PARAM_CHANGED;
                if (ME_GetCurrentMode(remDev) == BLM_SNIFF_MODE)
                {
                    ME_StartSniffSubrating(remDev);
                }
            }
        }
    }
}

#endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 


/*****************************************************************************
 * FUNCTION
 *  CMGR_SetSniffExitPolicy
 * DESCRIPTION
 *  Sets the policy for exiting sniff mode.
 * PARAMETERS
 *  Handler     [IN]        
 *  Policy      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_SetSniffExitPolicy(CmgrHandler *Handler, CmgrSniffExitPolicy Policy)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        Handler->sniffPolicy = Policy;
        status = BT_STATUS_SUCCESS;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_GetSniffExitPolicy
 * DESCRIPTION
 *  Gets the policy for exiting sniff mode.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
CmgrSniffExitPolicy CMGR_GetSniffExitPolicy(CmgrHandler *Handler)
{
    CmgrSniffExitPolicy policy = 0;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        policy = Handler->sniffPolicy;
    }

    OS_UnlockStack();

    return policy;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_SetDeviceAsMaster
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_SetDeviceAsMaster(CmgrHandler *Handler)
{
    BtStatus status;

    Assert(Handler->remDev != 0);
    status = CMGR_SetRemoteAsMaster(Handler->remDev);

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_SetRemoteAsMaster
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_SetRemoteAsMaster(BtRemoteDevice *link)
{
    Assert(link != 0);
    if (link->role == BCR_SLAVE)
    {
        bt_trace(TRACE_GROUP_1, IN_CMGR_SETDEVICEASMASTER_CALL_ME_SWITCHROLE);
        BTBMDoRoleSwitch(link);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, IN_CMGR_SETDEVICEASMASTER_ALREADY_MASTER);
    }
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  CMGR_HidNoRoleSwitch
 * DESCRIPTION
 *  
 * PARAMETERS
 *  on_off      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CMGR_HidNoRoleSwitch(U8 on_off)
{
    BTBMHIDNoRoleSwitch(on_off);
}

#if (SPP_SCO_SUPPORT == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  CMGR_ReadRSSI
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_ReadRSSI(BtRemoteDevice *remDev)
{
    BtStatus status;

    status = ME_ReadRSSI(remDev);
    return status;
}
#endif /* (SPP_SCO_SUPPORT == XA_ENABLED) */

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
void CMGR_SetDualPCMSCOAcceptValue(U8 value)
{
    CMGR(DualPCMSCOAeeptValue) = value;
}
#endif /* __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__ */

#if 0
/*****************************************************************************
 * FUNCTION
 *  CMGR_CheckLinkStatus
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Handler      [IN]   
 *  link_state      [IN]   
 * RETURNS
 *  BOOL
 *****************************************************************************/
BOOL ConnMgrCheckLinkStatus(CmgrHandler *Handler, U8 link_state)
{
    BOOL status;

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        if ((Handler->bdc) && (Handler->bdc->link))
        {
            status = (ME_GetRemDevState(Handler->bdc->link) == link_state) ? FALSE : TRUE;
        }
        else
        {
            status = FALSE;
        }
    }
    else
    {
        status = FALSE;
    }

    OS_UnlockStack();
    return status;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  CMGR_SetQoS
 * DESCRIPTION
 *  Disables sniff mode for the specified handler.  This also
 *  disables sniff mode for all handlers.
 * PARAMETERS
 *  Handler     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus CMGR_SetQoS(CmgrHandler *Handler, int level)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtQoSInfo info;

    bt_trace(TRACE_GROUP_1, CMGR_SETQOS, level);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Handler != 0);
    CheckUnlockedParm(BT_STATUS_NO_CONNECTION, Handler->bdc != 0);    

    OS_LockStack();

    if (IsNodeOnList(&CMGR(handlerList), &Handler->node))
    {
        OS_MemSet((U8 *)&info, 0x00, sizeof(BtQoSInfo));
        switch (level)
        {
        case 0:
            info.role = BCR_UNKNOWN;
            info.flags = 0;
            info.serviceType = BT_QOS_BEST_EFFORT;
            break;
        case 1:  /* LEVEL_A2DP */
            info.role = BCR_SLAVE;
            info.flags = 0;
            info.serviceType = BT_QOS_GUARANTEED;
            info.tokenRate = 0x00009756;
            info.peakBandwidth = 0x0000EA60;
            info.latency = 0x00002BF2;
            info.delayVar = 0xFFFFFFFF;
            break;
        default:
            Assert(0);
        }
        (void)ME_SetUpQoS(Handler->bdc->link, &info);
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


