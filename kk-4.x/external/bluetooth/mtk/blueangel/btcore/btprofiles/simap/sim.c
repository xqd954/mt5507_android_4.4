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
 *     $Workfile:sim.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:
 *     API calls for the SIM Access profile.
 *
 * Created:
 *     May 1, 2002
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#ifdef __BT_SIM_PROFILE__

#include "btconfig.h"
#include "btalloc.h"
#include "sdp.h"
#include "bt_simi.h"
//#include "bt_adp_simap.h"
#ifdef __SIMAP_MAUI__
#include "kal_trace.h"  /* Trace Header Files */
//#include "bluetooth_bm_struct.h"
#endif
#include "bluetooth_gap_struct.h"
#include "bluetooth_trc.h"
#include "btaccctrl.h"


extern SIMAPCallback SIMAPCB;

extern U8 BTBMGapGetAuthenticationMode(void);
extern void SimApCloseRFCommChnl(RfChannel *rfChannel);
extern void BTBMGapSetAuthenticationMode(
                BOOL reported,
                bt_bm_write_authentication_mode_req_struct *btbm_adp_input_msg);
extern void BTBMReturnNormalLinkState(void);
#ifdef __SIMAP_MAUI__
extern void bt_simap_start_timer(eventid *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr);
#else
extern void bt_simap_start_timer(void *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr);
#endif

#if (BT_SECURITY != XA_ENABLED) || (RF_SECURITY != XA_ENABLED)
#error BT_SECURITY and RF_SECURITY must be enabled for SIM Access Profile
#endif 

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SDP Service Class ID.
 *
 * SDP objects registered by the SIM Access Service.  They are the Service 
 * Class ID, the Protocol Descriptor List, Bluetooth Profile Descriptor
 * List, and Service Name.
 */
static const U8 SimClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence, 6 bytes */

    /* This is a list of one element of type UUID */
    SDP_UUID_16BIT(SC_GENERIC_TELEPHONY),   /* Generic Telephony */
    SDP_UUID_16BIT(SC_SIM_ACCESS)           /* SIM Access */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by the SIM Access Service.
 * 
 * Value of the protocol descriptor list for the SIM Access Service.
 * This structure is a ROM'able representation of the RAM structure.
 * During SIM_RegisterServer, this structure is copied into the RAM structure 
 * and used to register the server. A RAM structure must be used to be able 
 * to dynamically set certain values.
 */
static const U8 SimProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12), /* Data element sequence, 11 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which has
     * a UUID element.  The second element is RFCOMM, which ahs a
     * UUID element and a server channel.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP                  */

    /*
     * The next protocol descriptor is for RFCOMM. It contains a UUID and
     * a server channel.
     */
    SDP_ATTRIB_HEADER_8BIT(5),              /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),            /* Uuid16 RFCOMM     */
    // SDP_UINT_8BIT(SIMAP_RFCOMM_SRV_CHN)  /* Server channel (varies) */
    SDP_UINT_8BIT(SIMAP_CHNL_NUM_START)
};

/*
 * * SIM Access Service Name.
 */
static const U8 SimServiceName[] = 
{
    SDP_TEXT_8BIT(11),  /* Null terminated text string, 11 bytes */
    'S', 'I', 'M', ' ', 'A', 'c', 'c', 'e', 's', 's', '\0'
};

/*
 * SIM Access Profile Descriptor List
 */
static const U8 SimProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),  /* Data element sequence, 8 bytes */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_SIM_ACCESS),
    SDP_UINT_16BIT(0x0100)      /* Version 1.0 */
};

/*---------------------------------------------------------------------------
 * SIM Access Public Browse Group.
 */
static const U8 SimBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP), /* Public Browse Group */
};

/*
 * SIMM Access Service attributes.
 * This is a ROM template for the RAM structure used to register the
 * SIM Access Profile's SDP record.  A RAM structure must be used to be able
 * to dynamically set certain values.
 */
static const SdpAttribute SimServerSdpAttributes[] = 
{
    /* Print SIM Access Service class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SimClassId),
    /* SIM Access protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SimProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, SimBrowseGroup),
    /* SIM Access profile descriptor */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, SimProfileDescList),
    /* SIM Access service name attribute */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SimServiceName),
};
#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
/*-------------------------------------------------------------------------
 *
 * SDP query info
 */
/*
 * Service search attribute request for the SIM Access protocol descriptor 
 * list. The service search pattern is very specific. It contains the UUIDs 
 * for the SIM Access Service, L2CAP, and the RFCOMM.  This structure is a 
 * ROM'able representation of the RAM structure.  A RAM structure must be 
 * used to be able to dynamically set certain values.
 */
static const U8 SimServiceSearchAttribReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(9),      /* Data Element Sequence, 9 bytes */
    /* The first UUID indicates the service type */
    SDP_UUID_16BIT(SC_SIM_ACCESS),  /* SIM Access */
    /* The second UUID is L2CAP */
    SDP_UUID_16BIT(PROT_L2CAP),     /* L2CAP UUID in Big Endian */
    /* The third UUID is RFCOMM */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* RFCOMM UUID in Big Endian */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),              /* Data Element Sequence, 3 bytes */
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST)  /* Value of prot descriptor list ID */
};
#endif /* SIM_CLIENT == XA_ENABLED */

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/

#if SDP_PARSING_FUNCS == XA_DISABLED
#error The SIM Access profile requires the SDP parsing functions.
#endif /* SDP_PARSING_FUNCS == XA_DISABLED */

#if XA_CONTEXT_PTR == XA_ENABLED
static BtSimContext tempContext;
BtSimContext *simContext = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
BtSimContext simContext;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/****************************************************************************
 *
 * Public API Functions
 *
 ****************************************************************************/

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            SIM_RegisterServer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers a SIM Access server.
 *
 * Return:    (See SIM.H)
 */


/*****************************************************************************
 * FUNCTION
 *  SIM_RegisterServer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Server          [IN]        
 *  Callback        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_RegisterServer(SimServer *Server, SimServerCallback Callback)
{
    BtStatus status;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_IN_USE, !IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Register with RFCOMM */
    Server->rfChannel.callback = SimServerRfCallback;
    Server->rfChannel.maxFrameSize = RF_MAX_FRAME_SIZE;
    RF_ChannelInit(&(Server->rfChannel), BT_SEC_HIGH);

    /*
     * Before register rfchannel, must fill serviceId! 
     * otherwise, the actual registered rfchannel is not the same with that registered in SDPDB
     */

    /* SIM(rfService).serviceId = SIMAP_RFCOMM_SRV_CHN; */
    SIM(rfService).serviceId = SIMAP_CHNL_NUM_START;

    if ((status = RF_RegisterServerChannel(&(Server->rfChannel), &SIM(rfService), 7)) == BT_STATUS_SUCCESS)
    {
        SIM(serviceCount)++;

        /* Register SDP Entry if not already registered */
        if (SIM(serviceCount) == 1)
        {
            /*
             * Register a security handler.  All connections to this service must
             * be authenticated and encrypted.
             */

        #if (BT_SEC_LEVEL2_SETTING_ENABLED != XA_ENABLED)
            SIM(secRec).id = SEC_RFCOMM_ID;
            SIM(secRec).channel = SIM(rfService).serviceId;
            SIM(secRec).level = BSL_AUTHENTICATION_IN | BSL_ENCRYPTION_IN;
            status = SEC_Register(&SIM(secRec));
        #endif /* (BT_SEC_LEVEL2_SETTING_ENABLED != XA_ENABLED) */
            status = BT_STATUS_SUCCESS;
            if (status == BT_STATUS_SUCCESS)
            {
                /* Build the SDP record */

                /*
                 * Protocol Descriptor has a dynamic value and must be copied
                 * to RAM.  Then the value of the RFCOMM server channel must
                 * be set.
                 */

                OS_MemCopy((U8*) (Server->sdpProtoDescList), (U8*) SimProtoDescList, sizeof(SimProtoDescList));

                /* Set the server channel ID */
                Server->sdpProtoDescList[13] = SIM(rfService).serviceId;

                OS_MemCopy(
                    (U8*) (Server->sdpAttribute),
                    (U8*) SimServerSdpAttributes,
                    sizeof(SimServerSdpAttributes));

                /*
                 * Fix up SDP Profile Descriptor attribute, since we have a
                 * modified version with the RFCOMM service ID.
                 */
                Server->sdpAttribute[1].value = Server->sdpProtoDescList;

                /* Add the record */
                Server->sdpRecord.attribs = Server->sdpAttribute;
                Server->sdpRecord.num = 5;
                Server->sdpRecord.classOfDevice = COD_TELEPHONY;

                if (BT_STATUS_SUCCESS != SDP_AddRecord(&(Server->sdpRecord)))
                {
                    /* SDP Add Record Failed exception*/
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_ADD_SDPRECORD_OK);
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_REMOVE_SECREC_REG);
                }
            }
            else
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SEC_REG_FAILED);
            }
        }

        if (status != BT_STATUS_SUCCESS)
        {
            /* Clean up */
            kal_trace(TRACE_GROUP_1, DEREGISTER_SRV_CHANNEL);
            RF_DeregisterServerChannel(&Server->rfChannel, &SIM(rfService));

            if (SIM(serviceCount) >= 1)
            {
                SIM(serviceCount)--;
            }
            else
            {
                Assert(0);
            }
            status = BT_STATUS_FAILED;
        }
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_FAILED_CALL_RFREGSRVCHNL);
        status = BT_STATUS_FAILED;
    }

    if (status == BT_STATUS_SUCCESS)
    {

        bt_bm_write_authentication_mode_req_struct ptr;
        U8 i = 0;

        /* Initialize the server structure */
        Server->callback = Callback;
        Server->state = SIM_SERVER_STATE_CLOSED;
        OS_MemSet((U8*) & Server->simPacket, 0, sizeof(SimPacket));
        OS_MemSet((U8*) & Server->discPacket, 0, sizeof(SimPacket));
        OS_MemSet((U8*) & Server->discGracePacket, 0, sizeof(SimPacket));

        /* Add for this on 2007-0117 */
        for (i = 0; i < NUM_OF_SIMPACKETS; i++)
        {
            OS_MemSet((U8*) & Server->simPacketList[i], 0, sizeof(SimPacket));
        }

        InsertTailList(&SIM(serverList), &(Server->node));

        /* to register CMGR handler: 2006-10-15 */
        /* Register the connection manager handler */
        OS_MemSet((U8*) (&Server->cmgrHandler), 0, sizeof(CmgrHandler));
        /* AssertEval(BT_STATUS_SUCCESS == CMGR_RegisterHandler(&Server->cmgrHandler, SimServerCmgrCallback)); */

        /* Set Access Policy for SIM Access Profile */
        //BTAccCtrlSet((BT_PSM_POLICY_RFCOMM | BT_PSM_POLICY_SDP), BT_SERVICE_POLICY_SAP);

        /* Set Security to Security Level 2 (i.e., Service Level) */
        ptr.mode = BTBM_AUTHENTICATION_DISABLE;

        //for PTS testing: 2007-0304
        //ptr.mode = BTBM_AUTHENTICATION_ENABLE;

        /* Before setting the authentication mode, record the current BM's authentication setting. */

        Server->previous_security_mode = BTBMGapGetAuthenticationMode();
        BTBMGapSetAuthenticationMode(FALSE, &ptr);

    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_DeregisterServer
 * DESCRIPTION
 *  Deregisters a SIM Access server.
 *  This function is invoked when returning deactivate cnf msg back to MMI
 *  The following actions must be done:
 *  1. Deregister RFCOMMChannel of SIM Server
 *  2. Deregister SDP Record of SIM Access Profile
 *  3. Deregister SEC Record of SIM Access Profile
 * PARAMETERS
 *  Server      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_DeregisterServer(SimServer *Server)
{
    BtStatus status = BT_STATUS_BUSY;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    if (Server->state == SIM_SERVER_STATE_CLOSED)
    {
        /* 1. Deregister RFCommChannel */
        if ((status = RF_DeregisterServerChannel(&(Server->rfChannel), &SIM(rfService))) == BT_STATUS_SUCCESS)
        {
            if (SIM(serviceCount) >= 1)
            {
                SIM(serviceCount)--;
            }
            else
            {
                Assert(0);
            }

            /* 2. Deregister SDP entry if this is the last server */
            if (SIM(serviceCount) == 0)
            {
                bt_bm_write_authentication_mode_req_struct ptr;

                SDP_RemoveRecord(&(Server->sdpRecord));
                //SIM(rfService).serviceId = 0;
                /* 3. Deregister SEC Record */
                SEC_Unregister(&SIM(secRec));
                /* 4. Recovery to normal policy */
                //BTAccCtrlUnset();

                //Removed by Yufeng for Android  
                //BTBMReturnNormalLinkState();

                /* 5. Set Security to Security Level 3 (i.e.,Link Level) */
                /* Using the previos BM setting to restore the authentication mode */
                if (Server->previous_security_mode == BSM_SEC_LEVEL_1)
                {
                    ptr.mode = BTBM_AUTHENTICATION_DISABLE;
                }
                else
                {
                    ptr.mode = BTBM_AUTHENTICATION_ENABLE;
                }
                BTBMGapSetAuthenticationMode(FALSE, &ptr);
            }
            RemoveEntryList(&(Server->node));
        }
    }

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerRespondOpenInd
 * DESCRIPTION
 *  Accepts or rejects a Server connection with the client.
 * PARAMETERS
 *  Server          [IN]        
 *  ConnStatus      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerRespondOpenInd(SimServer *Server, SimConnStatus ConnStatus)
{
    BtStatus status = BT_STATUS_NO_CONNECTION;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Only in Connect_Pending state, it is possible to send CONNECT_RSP msg back to client */

    if (Server->state != SIM_SERVER_STATE_CONN_PENDING)
    {
        Assert(0);
    }

    /* Accept the connection if one is incoming */
    if (ConnStatus == SIM_CONN_STATUS_OK)
    {
        status = SimServerSendConnRspNoErr(Server, &ConnStatus);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerSendStatus
 * DESCRIPTION
 *  Sends card status information to the client.
 * PARAMETERS
 *  Server      [IN]        
 *  Status      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerSendStatus(SimServer *Server, SimCardStatus Status)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the status only in the correct state */
    if ((Server->state == SIM_SERVER_STATE_OPEN) || (Server->state == SIM_SERVER_STATE_CONN_PENDING))
    {
    	 SIMAPCallbackParms parms;
        status = SimServerSendStatus(Server, &Status);
        /* Report(("status of SimServerSendStatsu()=%02x",status)); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVSENDSTATUS, status);

        if (status != BT_STATUS_PENDING)
        {
            Assert(0);
        }
        if (SIM(simServer)[0].connect_mode == SLOW_CONNECT_MODE)
        {
            if ((Status == SIM_CARD_STATUS_RESET) && (Server->state == SIM_SERVER_STATE_CONN_PENDING))
            {
                /* An ATR request is expected for STATUS_IND with RESET in SLOW_CONNECT_MODE */
                Server->flags |= SIM_SERVER_FLAG_ATR;
                /* Start timer to wait client's ATR request */
                bt_simap_start_timer(
                    &SIM(simap_event_id),
                    MAX_WAIT_ATR_REQ_MSEC,
                    (kal_timer_func_ptr) bt_simap_wait_atr_in_connect_procedure_timeout_handler);
            }
            else if ((Status == SIM_CARD_STATUS_NOT_ACCESSIBLE) && (Server->state == SIM_SERVER_STATE_CONN_PENDING))
            {
                /*
                 * An ATR request is not necessary for STATUS_IND with CARD_NOT_ACCESSIBLE in SLOW_CONNECT_MODE 
                 * if client does not ask ATR, it will not use sim card!
                 */
                /* Report(("STATUS_IND with CARD_NOT_ACCESSIBLE will be sent in SLOW_CONNECT_MODE")); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_STATUS_IND_NOT_ACCESSIBLE_IN_SLOW_CON_MODE);
            }
        }
        else
            /* Will cause a status sent event in FAST_CONNECT_MODE */
        {
            Server->flags |= SIM_SERVER_FLAG_STATUS_PENDING;
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_CARD_STATUS_IND_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 parms.p.simap_card_status_ind_sent_ind.status = Status;
	 SIMAPCB(&parms);
    }
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerAtrRsp
 * DESCRIPTION
 *  Sends an ATR response.
 * PARAMETERS
 *  Server      [IN]        
 *  Len         [IN]        
 *  Atr         [IN]        
 *  Result      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerAtrRsp(SimServer *Server, U16 Len, U8 *Atr, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

	Report(("[BT SIMAP]SIM_ServerAtrRsp...Server->flags=0x%x, Result=%d, Len=%d", Server->flags, Result, Len));

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);
    if (SIM_RESULT_OK == Result)
    {
        CheckUnlockedParm(BT_STATUS_INVALID_PARM, Len != 0);
		Report(("[BT SIMAP]SIM_ServerAtrRsp...2"));
        CheckUnlockedParm(BT_STATUS_INVALID_PARM, Atr);
    }

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the ATR only if a response was expected */
    if (Server->flags & SIM_SERVER_FLAG_ATR)
    {
    	 SIMAPCallbackParms parms;

		Report(("[BT SIMAP]SIM_ServerAtrRsp...SIM_SERVER_FLAG_ATR is set!"));
		 
        if (Result == SIM_RESULT_OK)
        {
        	//kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_ATRRSP, Result, Atr);
        	Report(("[BT SIMAP]SimServerSendAtrRspNoErr: Result=%d, Atr=%s", Result, Atr));        
            status = SimServerSendAtrRspNoErr(Server, Len, Atr, &Result);
        }
        else
        {
        	//kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_ATRRSP_ERR, Result);
        	Report(("[BT SIMAP]SimServerSendAtrRspErr: Result=%d", Result));
            status = SimServerSendAtrRspErr(Server, &Result);
        }

		Report(("[BT SIMAP]SimServerSendAtrRspNoErr(or Err) return : status=%d", status));
		
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= (~(SIM_SERVER_FLAG_ATR | SIM_SERVER_FLAG_PROCESSING));
            /* Report(("Server->flags:%02x",Server->flags)); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE, Server->flags);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            Server->flags &= (~(SIM_SERVER_FLAG_ATR | SIM_SERVER_FLAG_PROCESSING));
            /* Report(("Server->flags:%02x",Server->flags)); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE, Server->flags);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerAtrRsp(): no packets, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 1);
        }
        
        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_ATR_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerApduRsp
 * DESCRIPTION
 *  Respond to an APDU request from the client.
 * PARAMETERS
 *  Server      [IN]        
 *  Len         [IN]        
 *  Apdu        [IN]        
 *  Result      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerApduRsp(SimServer *Server, U16 Len, U8 *Apdu, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);
    if (SIM_RESULT_OK == Result)
    {
        CheckUnlockedParm(BT_STATUS_INVALID_PARM, Len != 0);
        CheckUnlockedParm(BT_STATUS_INVALID_PARM, Apdu);
    }

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the APDU only if a response was expected */
    if (Server->flags & SIM_SERVER_FLAG_APDU)
    {
    	 SIMAPCallbackParms parms;
        if (Result == SIM_RESULT_OK)
        {
            status = SimServerSendApduRspNoErr(Server, Len, Apdu, &Result);
        }
        else
        {
            status = SimServerSendApduRspErr(Server, &Result);
        }
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_APDU | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_APDU | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerApduRsp(): no packtes, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 2);
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_APDU_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerSimOnRsp
 * DESCRIPTION
 *  Sends an SIM On response.
 * PARAMETERS
 *  Server      [IN]        
 *  Result      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerSimOnRsp(SimServer *Server, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the SIM On response only if it was expected */
    if (Server->flags & SIM_SERVER_FLAG_SIM_ON)
    {
    	 SIMAPCallbackParms parms;
        status = SimServerSendSimOnRsp(Server, &Result);
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_SIM_ON | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_SIM_ON | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerSimOnRsp(): no packets, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 3);
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_SIM_ON_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerSimOffRsp
 * DESCRIPTION
 *  Sends a SIM Off response.
 * PARAMETERS
 *  Server      [IN]        
 *  Result      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerSimOffRsp(SimServer *Server, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the SIM Off response only if it was expected */
    if (Server->flags & SIM_SERVER_FLAG_SIM_OFF)
    {
    	 SIMAPCallbackParms parms;
        status = SimServerSendSimOffRsp(Server, &Result);
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_SIM_OFF | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_SIM_OFF | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerSimOffRsp(): no packets, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 4);
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_SIM_OFF_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerResetSimRsp
 * DESCRIPTION
 *  Sends an Reset SIM response.
 * PARAMETERS
 *  Server      [IN]        
 *  Result      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerResetSimRsp(SimServer *Server, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the Reset SIM response only if it was expected */
    if (Server->flags & SIM_SERVER_FLAG_RESET_SIM)
    {
    	 SIMAPCallbackParms parms;
        status = SimServerSendResetSimRsp(Server, &Result);
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_RESET_SIM | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_RESET_SIM | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerResetSimRsp(): no packets, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 5);
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_SIM_RESET_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerCardReaderStatusRsp
 * DESCRIPTION
 *  Sends a Card Reader Status response.
 * PARAMETERS
 *  Server              [IN]        
 *  cardRdrStatus       [IN]        
 *  Result              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerCardReaderStatusRsp(SimServer *Server, SimCardReaderStatus cardRdrStatus, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the Card ReaderStatus response only if it was expected */
    if (Server->flags & SIM_SERVER_FLAG_CARD_RDR_STATUS)
    {
    	 SIMAPCallbackParms parms;
        if (Result == SIM_RESULT_OK)
        {
            status = SimServerSendCardRdrStatusRspNoErr(Server, &Result, &cardRdrStatus);
        }
        else
        {
            status = SimServerSendCardRdrStatusRspErr(Server, &Result);
        }
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_CARD_RDR_STATUS | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_CARD_RDR_STATUS | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerCardReaderStatusRsp(): no packets, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 6);
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_CARD_READER_STATUS_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerSetTransProtRsp
 * DESCRIPTION
 *  Sends a Set Transport Protocol response.
 * PARAMETERS
 *  Server      [IN]        
 *  Result      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerSetTransProtRsp(SimServer *Server, SimResultCode Result)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    /* Send the response only if expected */
    if (Server->flags & SIM_SERVER_FLAG_SET_TRANS_PROT)
    {
    	 SIMAPCallbackParms parms;
        status = SimServerSendSetTransProtRsp(Server, &Result);
        if (status == BT_STATUS_PENDING)
        {
            Server->flags &= ~(SIM_SERVER_FLAG_SET_TRANS_PROT | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
        }
        else if (status == BT_STATUS_BUSY)
        {
            /* Now no available simpackts to send, just ignore this request */
            Server->flags &= ~(SIM_SERVER_FLAG_SET_TRANS_PROT | SIM_SERVER_FLAG_PROCESSING);
            status = BT_STATUS_SUCCESS;
            /* Report(("In SIM_ServerSetTransProtRsp(): no packets, clear server flags")); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSRVRSP_NO_PKT, 7);
        }

        /* Send notification to MMI */
	 parms.event = SIMAP_EVENT_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND;
	 parms.dstMod = MOD_MMI;
	 SIMAPCB(&parms);
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ServerClose
 * DESCRIPTION
 *  Close the Server connection with the client.
 * PARAMETERS
 *  Server      [IN]        
 *  Type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ServerClose(SimServer *Server, SimDisconnectType Type)
{
    BtStatus status = BT_STATUS_NO_CONNECTION;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Server->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(serverList), &(Server->node)));

    if (Type == SIM_DISCONNECT_IMMEDIATE)
    {
        if (Server->state == SIM_SERVER_STATE_OPEN)
        {
            /* Report(("DISC_IND with immediate type case1")); */
            status = SimServerSendDiscInd(Server, &Type);
            Server->state = SIM_SERVER_STATE_DISC_PENDING;
            if (status != BT_STATUS_PENDING)
                /* Report(("Send DISC_IND case1 failed, status=%d",status)); */
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_IMMED_DISC_IND_FAILED_STATUS, status);
            }
        }
        else
        {
            /* Report(("DISC_IND with immediate type case2")); */
            SimApCloseRFCommChnl(&(Server->rfChannel));
        }

    }
    else if (Type == SIM_DISCONNECT_GRACEFUL)
    {
        if (Server->state == SIM_SERVER_STATE_OPEN)
        {
            status = SimServerSendDiscInd(Server, &Type);
            if (status != BT_STATUS_PENDING)
                /* Report(("Graceful DISC ind failed, status=%d",status)); */
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_GRACEFUL_DISC_IND_FAILED_STATUS, status);
            }
            /*
             * With Graceful type, still in Open State 
             * It is not necessary to change to another state 
             */
        }
        else
        {
            Assert(0);
        }
    }

    OS_UnlockStack();

    return status;
}

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED

/*---------------------------------------------------------------------------
 *            SIM_RegisterClient()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers a SIM Access client.
 *
 * Return:    (See SIM.H)
 */


/*****************************************************************************
 * FUNCTION
 *  SIM_RegisterClient
 * DESCRIPTION
 *  Registers a SIM Access client.
 * PARAMETERS
 *  Client          [IN]        
 *  Callback        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_RegisterClient(SimClient *Client, SimClientCallback Callback)
{
    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_IN_USE, !IsNodeOnList(&SIM(clientList), &(Client->node)));

    /* Clear the Client structure */
    OS_MemSet((U8*) Client, 0, sizeof(SimClient));

    /* SDP Registration */
    Client->sdpQueryToken.attribId = AID_PROTOCOL_DESC_LIST;
    Client->sdpQueryToken.mode = BSPM_BEGINNING;

    /* Initialize the Query token */
    Assert(sizeof(Client->sdpSearchAttribReq) == sizeof(SimServiceSearchAttribReq));

    OS_MemCopy(
        (U8*) (Client->sdpSearchAttribReq),
        (U8*) SimServiceSearchAttribReq,
        sizeof(SimServiceSearchAttribReq));

    Client->sdpQueryToken.parms = Client->sdpSearchAttribReq;
    Client->sdpQueryToken.plen = sizeof(SimServiceSearchAttribReq);
    Client->sdpQueryToken.type = BSQT_SERVICE_SEARCH_ATTRIB_REQ;
    Client->sdpQueryToken.callback = SimClientSdpCallback;

    /* Initialize the client structure */
    Client->callback = Callback;
    Client->state = SIM_CLIENT_STATE_CLOSED;
    OS_MemSet((U8*) & Client->simPacket, 0, sizeof(SimPacket));
    InsertTailList(&SIM(clientList), &(Client->node));

    /* Initialize the RFCOMM channel */
    Client->rfChannel.callback = SimClientRfCallback;
    Client->rfChannel.maxFrameSize = RF_MAX_FRAME_SIZE;
    Client->rfChannel.priority = 0;

    /* Register the connection manager handler */
    AssertEval(BT_STATUS_SUCCESS == CMGR_RegisterHandler(&Client->cmgrHandler, SimClientCmgrCallback));

    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_DeregisterClient
 * DESCRIPTION
 *  Deregisters a SIM Access client.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_DeregisterClient(SimClient *Client)
{
    BtStatus status = BT_STATUS_SUCCESS;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    if (Client->state == SIM_CLIENT_STATE_CLOSED)
    {
        RemoveEntryList(&(Client->node));

        /* Deregister the connection manager handler */
        AssertEval(BT_STATUS_SUCCESS == CMGR_DeregisterHandler(&Client->cmgrHandler));
    }
    else
    {
        status = BT_STATUS_BUSY;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientOpen
 * DESCRIPTION
 *  Opens a Client connection to the Server.
 * PARAMETERS
 *  Client      [IN]        
 *  Addr        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientOpen(SimClient *Client, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_FAILED;
    U8 maxMsgSize[2];

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    if (SIM(transportState) == SIM_TRANSPORT_DOWN)
    {

        /* Request a connection */
        status = CMGR_CreateDataLink(&Client->cmgrHandler, Addr);
        if (status == BT_STATUS_SUCCESS)
        {

            /* If connection is open, begin service query */
            status = SimStartServiceQuery(Client, BSQM_FIRST);
            if (status == BT_STATUS_PENDING)
            {
                Client->flags |= SIM_CLIENT_FLAG_QUERY_SDP;
            }
            else
            {
                status = BT_STATUS_FAILED;
            }

        }
        else if (status != BT_STATUS_PENDING)
        {
            status = BT_STATUS_FAILED;
        }

    }
    else if (Client->state == SIM_CLIENT_STATE_CLOSED)
    {
        /* Send the connect request */
        StoreBE16(maxMsgSize, SIM_MAX_MSG_SIZE);
        Client->simPacket.maxMsgSize = SIM_MAX_MSG_SIZE;
        status = SimClientSendConnReq(Client, maxMsgSize);
        if (status == BT_STATUS_PENDING)
        {
            Client->state = SIM_CLIENT_STATE_CONN_PENDING;
        }
        else
        {
            status = BT_STATUS_FAILED;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientAtrReq
 * DESCRIPTION
 *  Send an ATR request to the server.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientAtrReq(SimClient *Client)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the ATR request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_PROCESSING))
    {
        status = SimClientSendAtrReq(Client);
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_ATR;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SimClientApduReq
 * DESCRIPTION
 *  Send an APDU request to the server.
 * PARAMETERS
 *  Client      [IN]        
 *  Len         [IN]        
 *  Apdu        [IN]        
 *  type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SimClientApduReq(SimClient *Client, U16 Len, U8 *Apdu, SimApduType type)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (Apdu != 0) || (Len != 0));
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (type == SIM_APDU_GSM) || (type == SIM_APDU_IEC_7816));

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the APDU request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_PROCESSING))
    {
        switch (type)
        {
            case SIM_APDU_GSM:
                status = SimClientSendApduReq(Client, Len, Apdu);
                break;
            case SIM_APDU_IEC_7816:
                status = SimClientSendApdu7816Req(Client, Len, Apdu);
                break;
            default:
                Assert(0);
                break;
        }
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_APDU;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientSimOnReq
 * DESCRIPTION
 *  Send a SIM Power on request to the server.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientSimOnReq(SimClient *Client)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the SIM Power On request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_PROCESSING))
    {
        status = SimClientSendSimOnReq(Client);
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_SIM_ON;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientSimOffReq
 * DESCRIPTION
 *  Send a SIM Power on request to the server.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientSimOffReq(SimClient *Client)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the SIM Power Off request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_SIM_OFF))
    {
        status = SimClientSendSimOffReq(Client);
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_SIM_OFF;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientResetSimReq
 * DESCRIPTION
 *  Send a Reset SIM request to the server.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientResetSimReq(SimClient *Client)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the Reset SIM request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_RESET_SIM))
    {
        status = SimClientSendResetSimReq(Client);
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_RESET_SIM;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientSetTransProtReq
 * DESCRIPTION
 *  Send a Set Transport Protocol request to the server.
 * PARAMETERS
 *  Client      [IN]        
 *  id          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientSetTransProtReq(SimClient *Client, U8 id)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the Card Reader Status request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_PROCESSING))
    {
        status = SimClientSendSetTransProtReq(Client, id);
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_SET_TRANS_PROT;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientCardRdrStatusReq
 * DESCRIPTION
 *  Send a Card Reader Status request to the server.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientCardRdrStatusReq(SimClient *Client)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    /*
     * Send the Card Reader Status request if a connection exists and no
     * request is outstanding.
     */
    if ((Client->state == SIM_CLIENT_STATE_OPEN) && !(Client->flags & SIM_CLIENT_FLAG_PROCESSING))
    {
        status = SimClientSendCardRdrStatusReq(Client);
        if (status == BT_STATUS_PENDING)
        {
            Client->flags |= SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_CARD_RDR_STATUS;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SIM_ClientClose
 * DESCRIPTION
 *  Close the Client connection with the client.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SIM_ClientClose(SimClient *Client)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Client->callback != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&SIM(clientList), &(Client->node)));

    if (Client->state == SIM_CLIENT_STATE_OPEN)
    {
        if ((status = SimClientSendDiscReq(Client)) == BT_STATUS_PENDING)
        {
            Client->state = SIM_CLIENT_STATE_DISC_PENDING;
        }
    }
    else
    {
        /* SIM Access is not up, try to bring down the RFCOMM channel */
        status = RF_CloseChannel(&Client->rfChannel);
    }

    OS_UnlockStack();
    return status;
}

#endif /* SIM_CLIENT == XA_ENABLED */
#endif /* __BT_SIM_PROFILE__ */
