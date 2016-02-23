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
 *     $Workfile:bneputil.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains utilities for the BNEP protocol.
 *
 * Created:     Feb 28, 2001
 * 
 * Copyright 1999-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifdef __BT_BNEP__

#include <sys/bnepi.h>
#include <sys/hci.h>
#include <btalloc.h>

//static void ReadLocalBdAddrCallback(const BtEvent *Event);

/*---------------------------------------------------------------------------
 *            FindBnepPANU(U16 ChannelId)
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return the BNEP PANU given the L2cap Channel Id.
 *
 * Return:    BnepPanu *
 */
BnepPanu *FindBnepPanu(U16 ChannelId)
{
    I8 i;

    /* Go through the BNEP PANU array looking for the given channelId. */
    for (i = 0; i < NUM_BNEP_PANUS; i++) {
        if (BNEP(panu)[i] && BNEP(panu)[i]->l2ChannelId == ChannelId) {
            return BNEP(panu)[i];
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------
 *            InitBnepPANU()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize a BNEP PANU and set the L2cap Channel Id.
 *
 * Return:    BtStatus
 */
BtStatus InitBnepPanu(BnepPanu *panu, U16 l2ChannelId, BtRemoteDevice *remDev)
{
    U8 i, j = NUM_BNEP_PANUS;

    /* Find an unused BNEP PANU slot ... */
    for (i = NUM_BNEP_PANUS; i > 0; i--) {
        if (BNEP(panu)[i-1] == panu) {
            return BT_STATUS_IN_USE;
        }
        if (BNEP(panu)[i-1] == 0) {
            j = i-1;
        }
    }

    /* ... and initialize the structure */
    if (j < NUM_BNEP_PANUS) {
        BNEP(panu)[j] = panu;
        OS_MemSet((U8 *)panu, 0, sizeof(BnepPanu));
        panu->l2ChannelId = l2ChannelId;
        panu->remDev = remDev;
        panu->controlTimeout = BNEP_CONTROL_TIMEOUT * 1000;
        return BT_STATUS_SUCCESS;
    } else
        return BT_STATUS_NO_RESOURCES;
}

/*---------------------------------------------------------------------------
 *            DeInitBnepPANU()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize a BNEP PANU and set the L2cap Channel Id.
 *
 * Return:    void
 */
void DeInitBnepPanu(BnepPanu *panu)
{
    U8 i;

    /* Cancel any outstanding timer */
    for (i = 0; i < BNEP_NUM_TIMERS; i++) {
        if (panu->timer[i].func != 0) {
            EVM_CancelTimer(&panu->timer[i]);
        }
    }

    /* Remove the panu from BNEP context */
    for (i = 0; i < NUM_BNEP_PANUS; i++) {
        if (BNEP(panu)[i] == panu) {
            BNEP(panu)[i] = 0;
        }
    }

    if (panu != 0 ) {
        panu->l2ChannelId = L2CID_INVALID;
        panu->state = BNEP_STATE_CLOSED;
    }
}

/*---------------------------------------------------------------------------
 *            BnepSetTimer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets up Control response timers.
 *
 * Return:    void
 */
BtStatus BnepSetTimer(BnepPanu *panu, BnepControlType type, BtPacket* btPacket)
{
    U8 i;

    OS_Report("[BNEP]BnepSetTimer+++");


    /* first check if already have active timer */
    for (i = 0; i < BNEP_NUM_TIMERS; i++) {
        //if (panu->timer[i].context == (void *)(type) {
        if (panu->timer[i].context == (void *)(type & ~BNEP_RETRANSMISSION_ENABLE)) {			
            /* check if 2 like timers were set from this command */
            if (panu->timer[i].func == 0) {
                /* clear all timers not started and fail the command */
                for (i = 0; i < BNEP_NUM_TIMERS; i++) {
                    if (panu->timer[i].func == 0) {
                        panu->timer[i].context = 0;
                        //20110504  Paul start
                        OS_Report("[BNEP]BnepSetTimer memset panu[%p]:controlPkt[%d])", panu, i);		                        
                        memset( (void*)&(panu->controlPkt[i]), 0, sizeof(BtPacket));
                        panu->retransmission[i] = 0;			 
                        //20110504  Paul end
                    }
                }
                OS_Report("[BNEP]BnepSetTimer BT_STATUS_FAILED---");				
                return BT_STATUS_FAILED;
            } else {
                OS_Report("[BNEP]BnepSetTimer BT_STATUS_IN_PROGRESS---");            
                return BT_STATUS_IN_PROGRESS;
            }
        }
    }
 
    /* now, add timer if slot is available */
    for (i = 0; i < BNEP_NUM_TIMERS; i++) {
        if (panu->timer[i].context == 0) {
	     //20110504 Paul start
            //panu->timer[i].context = (void *)type;	     
	     if (type & BNEP_RETRANSMISSION_ENABLE)
	     {	 
                OS_Report("[BNEP]BnepSetTimer memcpy panu[%p]:controlPkt[%d])", panu, i);			     
	         memcpy((void*)&(panu->controlPkt[i]), btPacket, sizeof(BtPacket));
                panu->retransmission[i] = BNEP_RETRANSMISSION_TIMES;			
	     }
	     else
	     {
	         panu->retransmission[i] = BNEP_RETRANSMISSION_TIMES + 1;
	     }
            panu->timer[i].context = (void *)(type & ~BNEP_RETRANSMISSION_ENABLE);		 
	     //20110504 Paul end
            OS_Report("[BNEP]BnepSetTimer BT_STATUS_SUCCESS---");	     
            return BT_STATUS_SUCCESS;
        }
    }
    OS_Report("[BNEP]BnepSetTimer BT_STATUS_IN_PROGRESS---");
    return BT_STATUS_IN_PROGRESS;
}

/*---------------------------------------------------------------------------
 *            BnepStartTimer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Starts Control response timers.
 *
 * Return:    void
 */
void BnepStartTimer(BnepPanu *panu)
{
    U8 i;

    /* start the timers */
    for (i = 0; i < BNEP_NUM_TIMERS; i++) {
        if (panu->timer[i].context != 0) {
            panu->timer[i].func = BnepTimeoutHandler;
            EVM_StartTimer(&panu->timer[i], panu->controlTimeout);
        }
    }
}

/*---------------------------------------------------------------------------
 *            BnepFindPanuTimer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Finds panu for timer.
 *
 * Return:    void
 */
EvmTimer *BnepFindPanuTimer(BnepPanu *panu, BnepControlType type)
{
    U8 i;

    for (i = 0; i < BNEP_NUM_TIMERS; i++) {
        if (panu->timer[i].context == (void *)(U32)type) {
                return &panu->timer[i];
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------
 *            BnepFindTimerPanu()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Finds panu for timer.
 *
 * Return:    void
 */
BnepPanu *BnepFindTimerPanu(EvmTimer *timer)
{
    U8 i, j;

    for (i = 0; i < NUM_BNEP_PANUS; i++) {
        if (BNEP(panu)[i]) {
            for (j = 0; j < BNEP_NUM_TIMERS; j++) {
                if (&BNEP(panu)[i]->timer[j] == timer) {
                    return BNEP(panu)[i];
                }
            }
        }
    }

    return 0;
}

/* 20110503 Paul find the retransmission control packet*/
U8 BnepFindRestransmissionCtrlPktIdx(BnepPanu** panu, EvmTimer *timer)
{
    U8 i;

    for (i = 0; i < BNEP_NUM_TIMERS; i++) {
         if (&(*panu)->timer[i] == timer )
         {
            // if( 0 < *panu->retransmission[i] && *panu->retransmission[i] <= BNEP_RETRANSMISSION_TIMES) 
	     // {
             //   *panu->retransmission[i] --;
                return i;
             //}
         }
    }
   
    return BNEP_NUM_TIMERS;
}


/*---------------------------------------------------------------------------
 *            BnepTimeoutHandler()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Timer handler for Control response timer.
 *
 * Return:    void
 */
void BnepTimeoutHandler(EvmTimer *timer)
{
    BnepCallbackParms   parms;
    U8 ctrlPktIdx;

    OS_Report("[BNEP]BnepTimeoutHandler+++");

    /* A request was sent, but no response was received */
    parms.panu = BnepFindTimerPanu(timer);


    /* Check for active link */
    if (ME_GetCurrentMode(parms.panu->remDev) != BLM_ACTIVE_MODE) {
        /* The link is in hold, park or sniff, restart the timer. */
        EVM_StartTimer(timer, parms.panu->controlTimeout);
        return;
    }

    //20110503 Paul
    ctrlPktIdx= BnepFindRestransmissionCtrlPktIdx( &(parms.panu), timer);
    if ( BNEP_NUM_TIMERS == ctrlPktIdx )
    { //todo
        OS_Report("[BNEP]BnepTimeoutHandler Timer is not found");
    }
    else
    {
 	
        if ( parms.panu->retransmission[ctrlPktIdx] == BNEP_RETRANSMISSION_TIMES + 1 )		
        {
            //do nothing
           timer->func = 0;
           timer->context = 0; 
           parms.panu->retransmission[ctrlPktIdx] = 0;
           BnepAppCallBack(&parms, BT_STATUS_TIMEOUT, BNEPEVENT_CONTROL_TIMEOUT);
       }
       else if (  parms.panu->retransmission[ctrlPktIdx] > 0 &&  BNEP_RETRANSMISSION_TIMES <= parms.panu->retransmission[ctrlPktIdx] )
       {
            //do retransmission and restart timer
           parms.panu->retransmission[ctrlPktIdx] --;   
           L2CAP_SendData(parms.panu->l2ChannelId, parms.panu->controlPkt + ctrlPktIdx);
           EVM_StartTimer(timer, parms.panu->controlTimeout);		
       }	
       else // parms.panu->retransmission[ctrlPktIdx] == 0
       {
            // retransmission times is 0, close the connection
            // clear the timer so it can be used again 
            timer->func = 0;
            timer->context = 0;
            parms.panu->retransmission[ctrlPktIdx] = 0;
            memset( (void*)(parms.panu->controlPkt + ctrlPktIdx), 0, sizeof(BtPacket));		
        		
            // report to app 
           BnepAppCallBack(&parms, BT_STATUS_TIMEOUT, BNEPEVENT_CONTROL_TIMEOUT);
           BNEP_Close(parms.panu);		
        }
    }
	
/*
    if ( BNEP_SETUP_CONNECTION_RESPONSE_MSG == (BnepControlType*)timer->context ) {
        OS_Report("[BNEP]BNEP_SETUP_CONNECTION_TIMEOUT");
        BNEP_Close(parms.panu);
    }
    
    // clear the timer so it can be used again 
    timer->func = 0;
    timer->context = 0;

    // report to app 
    BnepAppCallBack(&parms, BT_STATUS_TIMEOUT, BNEPEVENT_CONTROL_TIMEOUT);
*/
    OS_Report("[BNEP]BnepTimeoutHandler---");
}

/*---------------------------------------------------------------------------
 *            ReadLocalBdAddr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read the local device BD_ADDR.
 *
 * Return:    BtStatus
 */
BtStatus ReadLocalBdAddr(void)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 i = 0;
 
    //BNEP(token).callback = ReadLocalBdAddrCallback;
    //BNEP(token).p.general.in.parmLen = 0;
    //BNEP(token).p.general.in.hciCommand = HCC_READ_BD_ADDR;
    //BNEP(token).p.general.in.event = HCE_COMMAND_COMPLETE;

    //status = ME_SendHciCommandSync(&BNEP(token));
    //Assert(status == BT_STATUS_PENDING);
    
    
    for (i = 0; i < BD_ADDR_SIZE; i++) {
        BNEP(bnepAddr).addr[i] = MEC(bdaddr).addr[BD_ADDR_SIZE-1-i];
    }
    return  status;
}
/*
static void ReadLocalBdAddrCallback(const BtEvent *Event)
{
    U8 i;

    if (Event->p.meToken->p.general.out.status == BT_STATUS_SUCCESS) {
        Assert(Event->p.meToken->p.general.out.event == HCE_COMMAND_COMPLETE);

        if (Event->p.meToken->p.general.out.parms[3] == 0) {
            Assert(Event->p.meToken->p.general.out.parmLen == 10);

            // store the address in the BNEP context 
            BNEP(bnepAddr) = BdToBnepAddr(*(BD_ADDR *)(Event->p.meToken->p.general.out.parms+4));
            return;
        }
    }

     //didn't get the local address, fill it with zeros 
    for (i=0; i < BD_ADDR_SIZE; i++) {
        BNEP(bnepAddr).addr[i] = 0;
    }
}
*/
BNEP_ADDR BdToBnepAddr(BD_ADDR bdAddr)
{
    U8 i;
    BNEP_ADDR bnepAddr;

    for (i=0; i<BNEP_ADDR_SIZE; i++) {
        bnepAddr.addr[i] = bdAddr.addr[BNEP_ADDR_SIZE - i - 1];
    }

    return bnepAddr;
}

BD_ADDR BnepToBdAddr(BNEP_ADDR bnepAddr)
{
    U8 i;
    BD_ADDR bdAddr;

    for (i=0; i<BD_ADDR_SIZE; i++) {
        bdAddr.addr[i] = bnepAddr.addr[BD_ADDR_SIZE - i - 1];
    }

    return bdAddr;
}

/*
 * void BnepMemMove(U8 *dest, U8 *src, U16 len)
 * {
 *     U16 i;
 * 
 *     if (dest < src) {
 *         for (i=0; i<len; i++) {
 *             dest[i] = src[i];
 *         }
 *     }
 *     else if (src < dest) {
 *         for (i=len; i > 0; i--) {
 *             dest[i-1] = src[i-1];
 *         }
 *     }
 * }
 */

#endif /* __BT_BNEP__ */

