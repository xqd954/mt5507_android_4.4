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
 *     $Workfile:hfg_sm.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the connection state machine for the 
 *     hands-free component of the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

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
*****************************************************************************/
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * Jun 16 2009 mtk80029
 * [MAUI_01869480] [BT HFP] revise code related to audio connection request
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/
#include "stdio.h"
#include "btalloc.h"
#include "rfcomm_adp.h"
#include "conmgr.h"
#include "chn.h"

/*---------------------------------------------------------------------------
 *            HfRfCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  RFCOMM callback for the HF state machine.
 *
 * Return:    void
 */
void ChnRfCallback(RfChannel *rfChannel, RfCallbackParms *Parms)
{
    ChnChannel  *Channel;
    /*AtCommands  atParms;*/
    U8          event = 0;
    U16         offset = 0;
    BtStatus status = BT_STATUS_SUCCESS;

    Report(("[CHN][ChnRfCallback] +rfChannel==x%X, RfEvent==0x%X", (U32)rfChannel, Parms->event) );

    Channel = ChnFindRegisteredChannel(rfChannel);
    Assert(Channel != 0);

    switch (Parms->event) 
    {
    case RFEVENT_OPEN_IND:
        DBG_PRINT_EVENT(("[CHN][EVENT] RFCOMM Event : RFEVENT_OPEN_IND."));
        if(Channel)
        {
            Channel->remDev = Parms->ptrs.remDev;
            if(ChnGetSubstate(Channel) == CHN_SUBSTATE_CLOSED1)
            {
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_IN1);
            }
            else
            {
                Report(("ChnRfCallback: Only in C1, the server channel is registered."));
                /* Only in C1, the server channel is registered, so there shall no open ind received in other 
                substates other than C1. */
                (void)RF_RejectChannel(rfChannel);
            }
        }
        else
        {
            Report(("ChnRfCallback: RFEVENT_OPEN_IND: HFP shall never get into this case."));
            /* HFP shall never get into this case */
            DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            (void)RF_RejectChannel(rfChannel);
        }
        break;

    case RFEVENT_OPEN:
        DBG_PRINT_EVENT(("[CHN][EVENT] RFCOMM Event : RFEVENT_OPEN."));
        /* Register a SCO handler */
        if (Channel) 
        {
            Channel->remDev = Parms->ptrs.remDev;
            //CMGR_SetDeviceAsMaster(&(Channel->cmgrHandler));
            //HfgEnableSniffTimerMode(&(Channel->cmgrHandler));
            Channel->linkFlags |= CHN_LINK_HANDSFREE;
            Report(("ChnRfCallback: RFCOMM HF connection established."));

            switch(ChnGetSubstate(Channel))
            {
            case CHN_SUBSTATE_CONN_OUT3:
            case CHN_SUBSTATE_CONN_IN2:
		  // Direct change state to SLC1
                ChnChangeSubstate(Channel, CHN_SUBSTATE_SLC1);
                break;
            case CHN_SUBSTATE_DISC1:
                ChnDisconnecting(Channel);
                break;
            default:
                Report(("ChnRfCallback: RFEVENT_OPEN:CHN shall not get into this case."));
                /* CHN shall not get into this case. */
                DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
                Assert(0);
                RF_CloseChannel(rfChannel);
                break;
            }
        }
        else
        {
            RF_CloseChannel(rfChannel);
        }
        break;
    case RFEVENT_DATA_IND:
        DBG_PRINT_EVENT(("[CHN][EVENT] RFCOMM Event : RFEVENT_DATA_IND."));
        if (Channel) 
        {
        	RfBuffer	rfBuf;
        	rfBuf.len = Parms->dataLen;
        	rfBuf.buf = Parms->ptrs.data;
            	Report(("ChnRfCallback: RFCOMM HF data received."));
            	RF_AdvanceCredit(rfChannel, 1);

		ChnAppCallback(Channel, CHN_EVENT_RX_DATA, Parms->status, (U32)&rfBuf);
        }
        break;

    case RFEVENT_PACKET_HANDLED:
        DBG_PRINT_EVENT(("[CHN][EVENT] RFCOMM Event : RFEVENT_PACKET_HANDLED, Status=%d", Parms->status));
        if (Channel) 
        {
        	Channel->bTxInProgress = FALSE;
            status = Parms->status;
            if(status != BT_STATUS_SUCCESS)
            {
                /* Only when RFCOMM channel is closed, it does not return success */
                DBG_PRINT_AT(("[CHN][AT] RFEVENT_PACKET_HANDLED : Fail"));
            }
            else
            {
	            	if(ChnRemainTxData(Channel) > 0)
			{
				DBG_PRINT_AT(("[CHN][AT] Bytestosend=%d", ChnRemainTxData(Channel) ));
				status = ChnSendTxBuffer(Channel);
				if (status == BT_STATUS_PENDING) 
				{
				    DBG_PRINT_AT(("[CHN] ChnSendTxBuffer return Pending"));
				}
            		}
            }
        } 
        else 
        {
            Report(("ChnRfCallback: RFEVENT_PACKET_HANDLED: No CHN channel."));
            return;
        }
        break;

    case RFEVENT_CLOSED:
        DBG_PRINT_EVENT(("[CHN][EVENT] RFCOMM Event : RFEVENT_CLOSED"));
        if (Channel) 
        {
            //HfgDisableSniffTimerMode(&(Channel->cmgrHandler));
            Channel->linkFlags &= ~CHN_LINK_HANDSFREE;

            switch(ChnGetSubstate(Channel))
            {
            case CHN_SUBSTATE_CONN_OUT3:
            case CHN_SUBSTATE_CONN_IN1:
            case CHN_SUBSTATE_CONN_IN2:
            case CHN_SUBSTATE_OPEN1:
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
                break;
            case CHN_SUBSTATE_OPEN2:
            case CHN_SUBSTATE_SLC1:
            case CHN_SUBSTATE_SLC2:
            case CHN_SUBSTATE_DISC1:
                ChnDisconnecting(Channel);
                break;
            default:
                DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
                Assert(0);
                break;
            }
        }
        break;

    default:
        DBG_PRINT_EVENT(("[CHN][EVENT] RFCOMM ignore other events : %d", Parms->event));
        /* Ignore other events */
        break;
    }
    Report(("- ChnRfCallback."));
}

/*---------------------------------------------------------------------------
 *            ChnServiceConnectionCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by device manager with link state events.
 *
 * Return:    (See header file)
 *
 */
void ChnServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status) 
{
    ChnChannel *Channel = ContainingRecord(Handler, ChnChannel, cmgrHandler);
    BtStatus return_code;

    Report(("+ ChnServiceConnectionCallback: Event=%d, Status=%d.", Event, Status));

    switch (Event) 
    {
    case CMEVENT_DATA_LINK_CON_CNF:
        DBG_PRINT_EVENT(("[CHN][EVENT] CMGR Event : CMEVENT_DATA_LINK_CON_CNF, Status=%d.", Status));
        if(ChnGetSubstate(Channel) == CHN_SUBSTATE_CONN_OUT1)
        {
            if (Status == BT_STATUS_SUCCESS) 
            {
                Channel->linkFlags |= CHN_LINK_ACL;
                /* FLOW : Start SDP query */
                if ((return_code = ChnStartServiceQuery(Channel, BSQM_FIRST)) == BT_STATUS_PENDING)
                {
                    ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_OUT2);
                }
                else
                {
                    ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
                }
            }
            else
            {
                /* This event shall not be received in other substates other than C1 */
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
            }
        }
        else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_DISC1)
        {
            ChnDisconnecting(Channel);
        }
        else
        {
            DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
        }
        break;

        case CMEVENT_DATA_LINK_DIS:
            /* We shall never run into this case */
            DBG_PRINT_EVENT(("[CHN][EVENT] CMGR Event : CMEVENT_DATA_LINK_DIS, Status=%d.", Status));
            DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            ChnCloseChannel(Channel);
            break;

        case CMEVENT_AUDIO_LINK_CON:
            DBG_PRINT_EVENT(("[CHN][EVENT] CMGR Event : CMEVENT_AUDIO_LINK_CON, Status=%d.", Status));
            /* Clear SCO connecting flag */
            Channel->linkFlags &= ~CHN_LINK_SCO_CONNECTING;
            ChnAppCallback(Channel, CHN_EVENT_AUDIO_CONNECTED, Status, 
            (U32)&Channel->cmgrHandler.bdc->link->bdAddr);
            if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC1)
            {
                /* Remote connect SCO link */
                /* It could happen in SLC1 but no action needed */
            }
            else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC2)
            {
                ChnChangeSubstate(Channel, CHN_SUBSTATE_SLC1);
            }
            else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_DISC1)
            {
                ChnDisconnecting(Channel);
            }
            else
            {
                DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            }
            break;

        case CMEVENT_AUDIO_LINK_DIS:
            DBG_PRINT_EVENT(("[CHN][EVENT] CMGR Event : CMEVENT_AUDIO_LINK_DIS, Status=%d.", Status));
            /* Clear SCO disconnecting flag */
            Channel->linkFlags &= ~CHN_LINK_SCO_DISCONNECTING;
            ChnAppCallback(Channel, CHN_EVENT_AUDIO_DISCONNECTED, Status, 
            (U32)&Channel->cmgrHandler.bdc->link->bdAddr);

            if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC1)
            {
                /* Remote disconnect SCO link */
                /* It could happen in SLC1 but no action needed */
            }
            else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC2)
            {
                ChnChangeSubstate(Channel, CHN_SUBSTATE_SLC1);
            }
            else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_DISC1)
            {
                ChnDisconnecting(Channel);
            }
            else
            {
                DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            }
            break;

        #if BT_SCO_HCI_DATA == XA_ENABLED
        case CMEVENT_AUDIO_DATA:
            DBG_PRINT_EVENT(("[CHN][EVENT] CMGR Event : CMEVENT_AUDIO_DATA, Status=%d.", Status));
            ChnAppCallback(Channel, CHN_EVENT_AUDIO_DATA, Status, (U32)&Handler->audioData);
            break;

        case CMEVENT_AUDIO_DATA_SENT:
            DBG_PRINT_EVENT(("[CHN][EVENT] CMGR Event : CMEVENT_AUDIO_DATA_SENT, Status=%d.", Status));
            ChnAppCallback(Channel, CHN_EVENT_AUDIO_DATA_SENT, Status, (U32)&Handler->audioPacket);
            break;
        #endif
    }
    Report(("- ChnServiceConnectionCallback."));
}

