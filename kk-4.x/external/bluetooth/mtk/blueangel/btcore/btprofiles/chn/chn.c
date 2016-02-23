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
 *     $Workfile:hfg.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains API functions for the Hands-free Audio
 *              Gateway SDK.
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
 ****************************************************************************/
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
 * Jun 2 2009 mtk80029
 * [MAUI_01837169] [BT] remove kal_prompt_trace
 * add trace
 * 
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#include "btalloc.h"
#include "chn.h"
/*---------------------------------------------------------------------------
 *            chn_init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize HF Gateway SDK.  Initialize state and other variables.
 *
 * Return:    TRUE - Memory successfully initialized.
 */
BOOL chn_init(void)
{
    U16 i;
    BOOL ret = TRUE;

    DBG_PRINT_API(("[CHN][API] + chn_init"));
	
    /* Initialize context memory */
    if (!(ChnAlloc())) 
    {
        DBG_PRINT_ERROR(("[CHN][ERR] ChnAlloc Fail - (%s,%d)", __FILE__, __LINE__));
        ret = FALSE;
        goto exit;
    }

    /* Initialize lists */
    InitializeListHead(&chnContext->channelList);
    InitializeListHead(&chnContext->freeRfChannelList);
    for (i = 0; i < NUM_BT_DEVICES * 2 + 2; i++) 
    {
        chnContext->rfChannels[i].userContext = 0;
        InsertTailList(&chnContext->freeRfChannelList, &chnContext->rfChannels[i].node);
#ifndef __BT_2_1_SIMPLE_PAIRING__        
        RF_ChannelInit(&(chnContext->rfChannels[i]), BT_SEC_HIGH);
#else
        RF_ChannelInit(&(chnContext->rfChannels[i]), BT_SEC_MED);
#endif
    }

exit:
    DBG_PRINT_API(("[CHN][API] - chn_init : return %s", ret ? "TRUE" : "FALSE"));
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            chn_register()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes all necessary data structures, registers with
 *            RFCOMM and SDP. 
 *
 * Return:    (See header file)
 *
 */
BtStatus chn_register(ChnChannel *Channel, ChnCallback Callback, U16 serv_type, U16 remote_svc)
{
	BtStatus   status = BT_STATUS_SUCCESS;

	DBG_PRINT_API(("[CHN][API] + chn_register(Channel=0x%X, Callback=0x%X)", (U32)Channel, (U32)Callback));

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

	OS_LockStack();

    /* Register this channel in the list */
    if (!IsNodeOnList(&chnContext->channelList, &Channel->node)) 
    {
        /* Clear the channel structure */
        OS_MemSet((U8*)Channel, 0, sizeof(ChnChannel));

		Channel->svc_type = serv_type;
		Channel->remote_svc_type = remote_svc;

        /* Register SDP record. */
        {
            /* This is the fist registration with RFCOMM */
            Channel->chnService.serviceId = 0;

            /* Register with SDP if this is the first channel */
            status = ChnRegisterSdpServices( Channel, serv_type );

            if (status != BT_STATUS_SUCCESS) 
            {
                Report(("chn_register: ChnRegisterSdpServices result failed. (%d).", status));
                goto error;
            }

            #if BT_SECURITY == XA_ENABLED
            /* Also, register security records */
            Channel->chnSecRec.id =  SEC_RFCOMM_ID;
            Channel->chnSecRec.channel = Channel->chnService.serviceId;
            Channel->chnSecRec.level = BSL_DEFAULT;    
            status = SEC_Register(&(Channel->chnSecRec));
            if (status != BT_STATUS_SUCCESS) 
            {
                DBG_PRINT_ERROR(("[CHN][ERR] SEC_Register fail : %d - (%s,%d)", status, __FILE__, __LINE__));
                Report(("chn_register: SEC_Register result failed. (%d).", status));
                /* Failed security registration, deregister SDP record */
                (void)ChnDeregisterSdpServices( Channel );
                goto error;
            }
            #endif			
        }

        /* Each ChnChannel allocates a RfChannel and register to RFCOMM  */
        if(status == BT_STATUS_SUCCESS)
        {
            /* Register a channel to receive Handsfree connections */
            Channel->rfChannel = ChnAllocRfChannel();
            if (Channel->rfChannel) 
            {
                status = ChnRegisterServerChannel(Channel);
            } 
            else 
            {
                Report(("chn_register: No channels left to register."));
                /* No channels left to register */
                status = BT_STATUS_NO_RESOURCES;
            }
            
            if( status != BT_STATUS_SUCCESS )
            {
                /* Register server channel fail */
                if(Channel->rfChannel)
                {
                    ChnFreeRfChannel(Channel->rfChannel);
                    Channel->rfChannel = NULL;
                }
		SEC_Unregister(&(Channel->chnSecRec));
		ChnDeregisterSdpServices( Channel );
                goto error;
            }
        }

        if (status == BT_STATUS_SUCCESS) 
        {
            /* Register channel */
            Channel->callback = Callback;

            /* Insert the channel on the list */
            InsertTailList(&chnContext->channelList, &Channel->node);
            (void)CMGR_RegisterHandler(&Channel->cmgrHandler, ChnServiceConnectionCallback);
		ChnResetTxBuffer(Channel);
		Channel->bTxInProgress = FALSE;
        }
    }
    else 
    {
        DBG_PRINT_ERROR(("[CHN][ERR] chn_register: The channel is already in use. (%s,%d)", __FILE__, __LINE__));
        /* Already registered */
        status = BT_STATUS_IN_USE;
        goto error;
    }
error:
    OS_UnlockStack();
    DBG_PRINT_API(("[CHN][API] - chn_register : return status=%d", status));
    return status;
}


/*---------------------------------------------------------------------------
 *            chn_deregister()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters with RFCOMM and SDP. 
 *
 * Return:    (See header file)
 *
 */
BtStatus chn_deregister(ChnChannel *Channel)
{
	BtStatus   status = BT_STATUS_SUCCESS;

	DBG_PRINT_API(("[CHN][API] + chn_deregister(Channel=0x%X)", (U32)Channel));

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
	OS_LockStack();
	if (IsNodeOnList(&chnContext->channelList, &Channel->node)) 
	{
		/* If the RfChannel is registered to RFCOMM, then deregister from RFCOMM */
		status = ChnDeregisterServerChannel(Channel);
		if (status != BT_STATUS_SUCCESS) 
		{
			/* Can not deregister server channel */
			Report(("chn_deregister: ChnFreeRfChannel failed. status=%d.", status));
		}

                if(Channel->rfChannel)
		{
			/* A HF channel was unregistered, free it */
			ChnFreeRfChannel(Channel->rfChannel);
			Channel->rfChannel = NULL;
		}

		(void)CMGR_DeregisterHandler(&Channel->cmgrHandler);
		RemoveEntryList(&Channel->node);

		/* Deregister with SDP */
		(void)ChnDeregisterSdpServices( Channel );

		#if BT_SECURITY == XA_ENABLED
		/* Remove Security Record */
		SEC_Unregister(&(Channel->chnSecRec));
		#endif
	}
	else 
	{
		Report(("chn_deregister: Channel is not registered."));
		/* Channel is not registered */
		status = BT_STATUS_NOT_FOUND;
	}
	OS_UnlockStack();
	DBG_PRINT_API(("[CHN][API] - chn_deregister : return status=%d", status));
	return status;
}

/*---------------------------------------------------------------------------
 *            chn_create_service_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens a service level connection. 
 *
 * Return:    (See header file)
 *
 */
BtStatus chn_create_service_link(ChnChannel *Channel, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[CHN][API] + chn_create_service_link(Channel=ox%X, Addr=0x%02X%02X%02X%02X%02X%02X)", 
    		                 (U32)Channel, 
    		                 Addr->addr[0], 
    		                 Addr->addr[1], 
    		                 Addr->addr[2], 
    		                 Addr->addr[3], 
    		                 Addr->addr[4], 
    		                 Addr->addr[5]));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    OS_LockStack();

    /* See if the channel is registered */
    if (IsNodeOnList(&chnContext->channelList, &Channel->node)) 
    {
        if(ChnGetSubstate(Channel) == CHN_SUBSTATE_CLOSED1)
        {
            status = ChnDeregisterServerChannel(Channel);
            if(status == BT_STATUS_SUCCESS)
		{
			/* FLOW : Create/Associate ACL link */
                    status = ChnOpenChannel(Channel, Addr);
            		if (status == BT_STATUS_SUCCESS)
			{
				/* FLOW : Start SDP query */
				status = ChnStartServiceQuery(Channel, BSQM_FIRST);
				if(status == BT_STATUS_PENDING)
				{
                                ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_OUT2);
				}
				else
				{
                                ChnCloseChannel(Channel);
                                status = ChnRegisterServerChannel(Channel);
                                if( status != BT_STATUS_SUCCESS )
                                    Assert(0);
				}
            		}
			else if(status == BT_STATUS_PENDING)
			{
                        ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_OUT1);
			}
			else
			{
                        /* Keep substate in C1 */
                        status = ChnRegisterServerChannel(Channel);
                        if( status != BT_STATUS_SUCCESS )
                        Assert(0);
                }
            }
            else
            {
                    /* we shall check if the ChnDeregisterServerChannel is failed 
                    due to the authentication is ongoing */
		}
        } 
        else 
        {
            /* Channel is already in use */
            DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            status = BT_STATUS_IN_USE;
        }
    }
    else
    {
        DBG_PRINT_ERROR(("[CHN][ERR] Channel is not registered. Channel=0x%X - (%s,%d)", (U32)Channel, __FILE__, __LINE__));
    }

    OS_UnlockStack();
    DBG_PRINT_API(("[CHN][API] - chn_create_service_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            chn_disconnect_service_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close a service level connection. 
 *
 * Return:    (See header file)
 *
 */
BtStatus chn_disconnect_service_link(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[CHN][API] + chn_disconnect_service_link(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    OS_LockStack();

    /* See if the channel is registered */
    if (IsNodeOnList(&chnContext->channelList, &Channel->node))
    {
        switch(ChnGetSubstate(Channel))
        {
        case CHN_SUBSTATE_OPEN1:
        case CHN_SUBSTATE_OPEN2:
        case CHN_SUBSTATE_SLC1:
        case CHN_SUBSTATE_SLC2:
            status = ChnDisconnecting(Channel);
            break;
         /* Below is unexpected substates 
          * Unexpected state handling */
        case CHN_SUBSTATE_CLOSED1:
            status = BT_STATUS_NO_CONNECTION;
            break;
        case CHN_SUBSTATE_DISC1:
            /* Tell caller the disconnection is already in progress */
            status = BT_STATUS_IN_PROGRESS;
            break;
        default:
            DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
            status = BT_STATUS_BUSY;
            break;
        }	
    }
    OS_UnlockStack();
    DBG_PRINT_API(("[CHN][API] - chn_disconnect_service_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            chn_create_audio_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Creates an audio (SCO) link to the audio gateway.
 *
 * Return:    (See header file)
 *
 */
BtStatus chn_create_audio_link(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[CHN][API] + chn_create_audio_link(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    OS_LockStack();

    if (IsNodeOnList(&chnContext->channelList, &Channel->node)) 
    {
        if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC1)
        {
            /* FLOW : Create Audio channel */
            if( CMGR_IsAudioUp(&Channel->cmgrHandler) )
            {
                Report(("chn_create_audio_link: Audio connection already exists."));
                /* Audio connection exist, return success directly */
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = CMGR_CreateAudioLink(&Channel->cmgrHandler);
                DBG_PRINT_FLOW(("[CHN][FLOW] Create Audio channel : %d", status));
                if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
                {
                    status = BT_STATUS_PENDING;
                    Channel->linkFlags |= CHN_LINK_SCO_CONNECTING;
                    ChnChangeSubstate(Channel, CHN_SUBSTATE_SLC2);
                }
            }			
        }
        else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC2)
        {
            DBG_PRINT_FLOW(("[CHN][FLOW] Audio creation ongoing : %s",(Channel->linkFlags | CHN_LINK_SCO_CONNECTING)?"Yes":"No"));
            /* If state is in S2 */
            if(Channel->linkFlags | CHN_LINK_SCO_CONNECTING)
            {
                Report(("chn_create_audio_link: Audio creation is ongoing."));
                /* Audio creation is ongoing */
                status = BT_STATUS_IN_PROGRESS;
                DBG_PRINT_FLOW(("[CHN][FLOW] Return in progress"));
            }
            else
            {
                if (Channel->linkFlags | CHN_LINK_SCO_DISCONNECTING)
                {
                    Report(("chn_create_audio_link: Audio disconnection ongoing."));
                    /* Audio disconnecting is ongoing */
                    status = BT_STATUS_FAILED;
                }
                DBG_PRINT_FLOW(("[CHN][FLOW] Return Fail"));
            }
        }
        else
        {
            Report(("chn_create_audio_link: No connection."));
            status = BT_STATUS_NO_CONNECTION;
        }
    }

    OS_UnlockStack();
    DBG_PRINT_API(("[CHN][API] - chn_create_audio_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            chn_disconnect_audio_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Releases the audio connection with the audio gateway.
 *
 * Return:    (See header file)
 *
 */
BtStatus chn_disconnect_audio_link(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[CHN][API] + chn_disconnect_audio_link(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    OS_LockStack();

    if (IsNodeOnList(&chnContext->channelList, &Channel->node)) 
    {
        if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC1)
        {
            if( !CMGR_IsAudioUp(&Channel->cmgrHandler) )
            {
                Report(("chn_disconnect_audio_link: No audio connection."));
                /* No audio connection, return success directly */
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                Report(("chn_disconnect_audio_link: Remove audio link."));
                status = CMGR_RemoveAudioLink(&Channel->cmgrHandler);
                DBG_PRINT_FLOW(("[CHN][FLOW] Close Audio channel : %d", status));
                if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
                {
                    status = BT_STATUS_PENDING;
                    Channel->linkFlags |= CHN_LINK_SCO_DISCONNECTING;
                    ChnChangeSubstate(Channel, CHN_SUBSTATE_SLC2);
                }
            }
        }
        else if(ChnGetSubstate(Channel) == CHN_SUBSTATE_SLC2)
        {
            DBG_PRINT_FLOW(("[CHN][FLOW] Audio disconnecting ongoing : %s",(Channel->linkFlags | CHN_LINK_SCO_DISCONNECTING)?"Yes":"No"));
            if(Channel->linkFlags | CHN_LINK_SCO_DISCONNECTING)
            {
                Report(("chn_disconnect_audio_link: Audio disconnection in progress."));
                /* Audio disconnection in progress */
                status = BT_STATUS_IN_PROGRESS;
                DBG_PRINT_FLOW(("[CHN][FLOW] Return in progress"));
            }
            else 
            {
                if(Channel->linkFlags | CHN_LINK_SCO_CONNECTING)
                {
                    Report(("chn_disconnect_audio_link: Audio connection in progress, so return fail."));
                    /* Audio connection in progress, so return fail */
                    status = BT_STATUS_FAILED;
                }
                DBG_PRINT_FLOW(("[CHN][FLOW] Return Fail"));
            }
        }
        else
        {
            Report(("chn_disconnect_audio_link: No connection or Busy."));
            if( ChnGetState(Channel) != CHN_STATE_SLC)
                status = BT_STATUS_NO_CONNECTION;
            else
                status = BT_STATUS_BUSY;
        }
    }

    OS_UnlockStack();
    DBG_PRINT_API(("[CHN][API] - chn_disconnect_audio_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            chn_accept_connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept incoming connection request.
 *
 * Return:    (See header file)
 */
BtStatus chn_accept_connect(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;

    DBG_PRINT_API(("[CHN][API] + chn_accept_connect(Channel=0x%X)", (U32)Channel));
	
    if(ChnGetSubstate(Channel) == CHN_SUBSTATE_CONN_IN1)
    {
        status = ChnOpenChannel(Channel, &Channel->remDev->bdAddr);
        if (status == BT_STATUS_SUCCESS)
        {
            status=RF_AcceptChannel(Channel->rfChannel);
            DBG_PRINT_FLOW(("[CHN][FLOW] Accept RFCOMM channel : %d", status));
            if (status == BT_STATUS_PENDING) 
            {
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_IN2);
            }
            else
            {
                /* free RF channel and close chn channel */
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
            }            
        }
        else
        {
            Report(("chn_accept_connect: Open channel failed. Try to reject the RF channel."));
            status = RF_RejectChannel(Channel->rfChannel);
            if(status == BT_STATUS_PENDING)
            {
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_IN2);
            }
            else
            {
                ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
            }
        }
    }
    else
    {
        DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
        status = BT_STATUS_FAILED;
    }

    DBG_PRINT_API(("[CHN][API] - chn_accept_connect : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            chn_reject_connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Reject incoming connection request.
 *
 * Return:    (See header file)
 */
BtStatus chn_reject_connect(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;

    DBG_PRINT_API(("[CHN][API] + chn_reject_connect(Channel=0x%X)", (U32)Channel));

    if(ChnGetSubstate(Channel) == CHN_SUBSTATE_CONN_IN1)
    {
        status = RF_RejectChannel(Channel->rfChannel);
        if(status == BT_STATUS_PENDING)
        {
            ChnChangeSubstate(Channel, CHN_SUBSTATE_CONN_IN2);
        }
        else
        {
            ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
        }
    }
    else
    {
        DBG_PRINT_ERROR( ("[CHN][ERR] Unexpected substate==0x%X - (%s,%d)", ChnGetSubstate(Channel), __FILE__, __LINE__) );
        status = BT_STATUS_FAILED;
    }
    	
    DBG_PRINT_API(("[CHN][API] - chn_reject_connect : return status=%d", status));
    return status;
}

BtStatus chn_send_data(ChnChannel *Channel, U8* data, U16 len)
{
	BtStatus status = BT_STATUS_FAILED;
	U16 writeLen;
	if (ChnGetState(Channel) >= CHN_STATE_OPEN)
	{
		if(data != NULL && len)
		{
			writeLen = ChnWriteTxBuffer(Channel, data, len);
			if(writeLen < len)
				status = BT_STATUS_NO_RESOURCES;
			else
				status = BT_STATUS_SUCCESS;
		}
	}
	return status;
}