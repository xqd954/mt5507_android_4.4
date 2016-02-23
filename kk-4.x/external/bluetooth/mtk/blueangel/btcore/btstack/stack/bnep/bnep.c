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

/*****************************************************************************
 *
 * File:
 *     $Workfile:bnep.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains code for the BNEP protocol. 
 *
 * Created:     Feb 28, 2001
 *
 * Copyright 2000-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#ifdef __BT_BNEP__

#include <bttypes.h>
#include <btalloc.h>
#include <sys/bnepi.h>
#include <sys/debug.h>

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#if XA_CONTEXT_PTR == XA_ENABLED
static BnepContext  tempContext;
BnepContext        *bnepcx = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
BnepContext         bnepcx;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            BNEP_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize BNEP. This should be the first function called for
 *            BNEP. 
 *
 * Return:    TRUE - BNEP was successfully initialized.
 *            FALSE  - BNEP failed to initialize.
 */
BOOL BNEP_Init(void)
{
    OS_LockStack();

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)bnepcx, 0, sizeof(BnepContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&bnepcx, 0, sizeof(BnepContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

    /* ToDo: Should BNEP_Init fail if we can't get the local address? */
    /* status = */
    //ReadLocalBdAddr();

    OS_Report("[BNEP]Initialized");

    OS_UnlockStack();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            BNEP_DeInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize BNEP. This should be the last function called for
 *            BNEP. 
 *
 * Return:    BT_STATUS_SUCCESS - service was successfully removed.
 *            BT_STATUS_FAILED  - BNEP was not initialized.
 */
BtStatus BNEP_DeInit(void)
{
    U8 i;

    OS_LockStack();

    /* Close any open BNEP connections */
    for (i = 0; i < NUM_BNEP_PANUS; i++) {
        if (BNEP(panu)[i] != 0) {
            BNEP_Close(BNEP(panu)[i]);
        }
    }

    OS_Report("[BNEP]Deinitialized");

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            BNEP_Register()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Register the BNEP callback. This should be the first function
 *            called for BNEP. 
 *
 * Return:    BT_STATUS_SUCCESS - The BNEP callback was successfully registered.
 *            BT_STATUS_FAILED  - BNEP callback registration failed.
 *            BT_STATUS_INVALID_PARM - Invalid parameter.
 *            BT_STATUS_NO_RESOURCES - A BNEP callback was already registered.
 */
BtStatus BNEP_Register(BnepCallback Callback)
{
    BtStatus status;

#if XA_ERROR_CHECK == XA_ENABLED
    if (Callback == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if (BNEP(callback) != 0) {
        return BT_STATUS_NO_RESOURCES;
    }
#endif



    Assert(Callback != 0);
    Assert(BNEP(callback) == 0);

    OS_LockStack();

    ReadLocalBdAddr();

    /* Register BNEP with L2CAP */
    BNEP(psm).callback = L2BnepCallback;
    BNEP(psm).psm = BT_PSM_BNEP;
    BNEP(psm).localMtu = L2CAP_MAXIMUM_MTU;
    BNEP(psm).minRemoteMtu = BNEP_MINIMUM_MTU;
#if L2CAP_FLOW_CONTROL == XA_ENABLED
    BNEP(psm).inLinkMode = BNEP(psm).outLinkMode = L2MODE_BASIC;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */
#ifdef __BT_2_1_ERTM__
    BNEP(psm).lockStepNeeded = 0;
    BNEP(psm).highSpeedRequired = 0;
#endif

    if ((status = L2CAP_RegisterPsm(&(BNEP(psm)))) != BT_STATUS_SUCCESS) {
        /* Unable to register */
        OS_Report("[BNEP]Unable to register with L2CAP");
        status = BT_STATUS_FAILED;
        goto fail;
    }



#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    kal_trace(BT_TRACE_G2_PROFILES, TRY_TO_REGISTER_SECURITY_RECORD);
    InitializeListEntry(&(BNEP(bnepSecRecord).node));
    BNEP(bnepSecRecord).id = SEC_L2CAP_ID;
    BNEP(bnepSecRecord).channel = BT_PSM_BNEP;
    BNEP(bnepSecRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT | BSL_ENCRYPTION_IN | BSL_ENCRYPTION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
    BNEP(bnepSecRecord).level_type = BT_SEC_LOW;
#else
    BNEP(bnepSecRecord).level_type = BT_SEC_MED;
#endif
    SEC_RegisterRecord(&BNEP(bnepSecRecord));
#endif



    BNEP(callback) = Callback;

    OS_Report("[BNEP]Registered");

fail:
    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            BNEP_DeRegister()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregister a BNEP callback. This should be the last function
 *            called for BNEP. 
 *
 * Return:    BT_STATUS_SUCCESS - callback was successfully deregistered.
 *            BT_STATUS_FAILED  - a BNEP callback is not registered.
 *            BT_STATUS_IN_PROGRESS - BNEP PANU currently up. Call 
                    BNEP_Close() first.
 */
BtStatus BNEP_DeRegister(void)
{
    U8 i;
    BtStatus status;

#if XA_ERROR_CHECK == XA_ENABLED
    if (BNEP(callback) == 0)
        return BT_STATUS_FAILED;
#endif

    Assert(BNEP(callback) != 0);

    OS_LockStack();

    for (i = 0; i < NUM_BNEP_PANUS; i++) {
        if (BNEP(panu)[i] != 0) {
            status = BT_STATUS_IN_PROGRESS;
            goto fail;
        }
    }

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    L2CAP_DeregisterPsm(&(BNEP(psm)));
#endif

#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    SEC_Unregister(&BNEP(bnepSecRecord));
#endif

    BNEP(callback) = 0;
    status = BT_STATUS_SUCCESS;

    OS_Report("[BNEP]Deregistered");

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BNEP_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open a BNEP PANU 
 *
 * Return:    BT_STATUS_PENDING - BNEP panu open process was successfully 
 *            started.
 *            BT_STATUS_FAILED  - panu failed to open.
 *            BT_STATUS_INVALID_PARM - (Error Check enabled)  Invalid parameter.
 */
BtStatus BNEP_Open(BtRemoteDevice *remDev, BnepPanu *panu)
{
    BtStatus status;

#if XA_ERROR_CHECK == XA_ENABLED
    if (BNEP(callback) == 0) {
        return BT_STATUS_FAILED;
    }
    if (panu == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if (remDev == 0) {
        return BT_STATUS_INVALID_PARM;
    }
#endif

    Assert(BNEP(callback) != 0);
    Assert(panu != 0);
    Assert(remDev != 0);

    OS_LockStack();

    status = InitBnepPanu(panu, L2CID_INVALID, remDev);
    if (status != BT_STATUS_SUCCESS) {
        goto fail;
    }

    /* Establish a new connection to the device */
    OS_Report("[BNEP]Establishing L2CAP connection");
    status = L2CAP_ConnectReq(&(BNEP(psm)), BNEP(psm).psm, 
                    remDev, 0, &panu->l2ChannelId);
    if (status != BT_STATUS_PENDING) {
        OS_Report("[BNEP]L2CAP connect request failed %d", status);
        DeInitBnepPanu(panu);
        goto fail;
    }


    panu->bnepAddr = BdToBnepAddr(remDev->bdAddr);

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BNEP_Accept()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept a BNEP PANU 
 *
 * Return:    BT_STATUS_PENDING - BNEP panu accept process was successfully 
 *            started.
 *            BT_STATUS_FAILED  - panu failed to open.
 *            BT_STATUS_INVALID_PARM - (Error Check enabled)  Invalid parameter.
 */
BtStatus BNEP_Accept(BnepPanu *panu, U16 l2ChannelId, BtRemoteDevice *remDev)
{
    BtStatus status = BT_STATUS_PENDING;

#if XA_ERROR_CHECK == XA_ENABLED
    if (BNEP(callback) == 0) {
        return BT_STATUS_FAILED;
    }
    if (l2ChannelId == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if (remDev == 0) {
        return BT_STATUS_INVALID_PARM;
    }
#endif

    Assert(BNEP(callback) != 0);
    Assert(l2ChannelId != 0);
    Assert(remDev != 0);

    OS_LockStack();

    if (panu != 0) {
        status = InitBnepPanu(panu, l2ChannelId, remDev);
    }

    if (panu == 0 || status == BT_STATUS_NO_RESOURCES) {
        /* Reject the connection */
        L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
    }
    else {
        panu->l2ChannelId = l2ChannelId;
        panu->bnepAddr = BdToBnepAddr(remDev->bdAddr);
        OS_Report("[BNEP]panu open ind %d", l2ChannelId);
        /* Accept the connection */
        status = L2CAP_ConnectRsp(l2ChannelId, L2CONN_ACCEPTED, 0);
    }

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            BNEP_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the BNEP PANU.  
 *
 * Return:    BT_STATUS_SUCCESS - BNEP panu closed.
 *            BT_STATUS_PENDING - BNEP panu close started 
 */
BtStatus BNEP_Close(BnepPanu *panu)
{
    BtStatus status;


    OS_Report("[BNEP]BNEP_Close+++");

#if XA_ERROR_CHECK == XA_ENABLED
    if (panu == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if (panu->state != BNEP_STATE_CLOSED &&
        panu->l2ChannelId == L2CID_INVALID) {
        return BT_STATUS_INVALID_PARM;
    }
#endif

    Assert(panu != 0);

    if (panu->state == BNEP_STATE_CLOSED) {
        return BT_STATUS_SUCCESS;
    }

    OS_LockStack();

    /* Free the l2cap channel */
    status = L2CAP_DisconnectReq(panu->l2ChannelId);
    if (status ==  BT_STATUS_PENDING) {
        panu->state = BNEP_STATE_CLOSE_PENDING;
    }
    else {
        panu->state = BNEP_STATE_CLOSED;
        status = BT_STATUS_SUCCESS;
    }

    OS_UnlockStack();


    OS_Report("[BNEP]BNEP_Close---");
    return status;
}

/*---------------------------------------------------------------------------
 *            BNEP_SendEtherPkt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an Ethernet packet to the remote device.
 *
 * Return:    BT_STATUS_PENDING the send packet process has been 
 *            successfully started.
 */
BtStatus BNEP_SendEtherPkt(BnepPanu *panu,
                           BtPacket *btPacket,
                           U8 *etherPkt,
                           U16 etherPktLen,
                           U8 *extHdr)
{
    U8             *b;
    BtStatus        status;
    BOOL            lastOne, startTimer = FALSE;
    BnepPacketType  type = BNEP_GENERAL_ETHERNET;
    BnepControlType timerResponse = 0;

#if XA_ERROR_CHECK == XA_ENABLED
    if (BNEP(callback) == 0) {
        return BT_STATUS_FAILED;
    }
    if (panu == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if (etherPktLen > L2CAP_MTU - 1) {
        return BT_STATUS_INVALID_PARM;
    }
#endif

    Assert(BNEP(callback) != 0);
    Assert(panu != 0);
    Assert(etherPktLen <= L2CAP_MTU - 1);

    OS_LockStack();	
    //20111103 mtk03036
    btPacket->priority_type	= BTP_FLAG_HIGH_PRIPORTY;
    //20111103 mtk03036 end

    /* determine if it is possible to compress the addresses */
    if (AreBdAddrsEqual(&BNEP(bnepAddr), (BNEP_ADDR *)&etherPkt[EtherPacket_hdr_source_addr])) {
        /* dest compressed */
        type = BNEP_COMPRESSED_ETHERNET_DEST_ONLY;
    }

    if (AreBdAddrsEqual(&panu->bnepAddr, (BNEP_ADDR *)&etherPkt[EtherPacket_hdr_dest_addr])) {
        if (type == BNEP_GENERAL_ETHERNET) {
            /* source compressed */
            type = BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY;
        }
        else {
            /* fully compressed */
            type = BNEP_COMPRESSED_ETHERNET;
        }
    }

    /* set up the packet (BNEP) header */
    btPacket->headerLen = BNEP_GNRL_HDR_SIZE;
    //(U8 *)b = btPacket->header + BT_PACKET_HEADER_LEN - BNEP_GNRL_HDR_SIZE;
    b = btPacket->header + BT_PACKET_HEADER_LEN - BNEP_GNRL_HDR_SIZE;
    OS_MemCopy(b + BnepGeneralHeader_etherHdr, etherPkt, ETHER_HDR_SIZE);
    /* indicate packet came from BNEP_SendEtherPkt */
    *((BtStatus (**)(BnepPanu *, BtPacket *, U8 *, U16, U8 *))&btPacket->ulpContext) = BNEP_SendEtherPkt;

    /* compress the addresses */
    switch (type) {
    case BNEP_GENERAL_ETHERNET:
        /* no compression possible */
        break;
    case BNEP_COMPRESSED_ETHERNET:
        /* remove all addresses, fix up dataLen */
        //(U8 *)b += BNEP_ADDR_SIZE * 2;
        b += BNEP_ADDR_SIZE * 2;
        btPacket->headerLen -= BNEP_ADDR_SIZE * 2;
        break;
    case BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY:
        /* remove the destination address, fix up dataLen */
        //(U8 *)b += BNEP_ADDR_SIZE;
        b += BNEP_ADDR_SIZE;
        btPacket->headerLen -= BNEP_ADDR_SIZE;
        break;
    case BNEP_COMPRESSED_ETHERNET_DEST_ONLY:
        /* remove the source address (1st move dest addr) */
        OS_MemCopy(b + BnepGeneralHeader_etherHdr_source_addr,
                            b + BnepGeneralHeader_etherHdr_dest_addr, BNEP_ADDR_SIZE);
        //(U8 *)b += BNEP_ADDR_SIZE;
        b += BNEP_ADDR_SIZE;
        /* fix up dataLen */
        btPacket->headerLen -= BNEP_ADDR_SIZE;
        break;
    default:
        break;
    }
    b[0] = type;

    /* set up the packet data and extension header */
    btPacket->flags &= BTP_FLAG_INUSE;
    if (extHdr) {
        b[0] |= BNEP_E_BIT;
        btPacket->flags |= BTP_FLAG_TAIL;
        btPacket->tailLen = etherPktLen - ETHER_HDR_SIZE;
        btPacket->tail = etherPkt + ETHER_HDR_SIZE;
        btPacket->data = extHdr;
        btPacket->dataLen = 0;
        startTimer = FALSE;
        lastOne = (extHdr[0] & BNEP_E_BIT) ? FALSE : TRUE;
        while (extHdr[0] & BNEP_E_BIT || lastOne) {
            if ((extHdr[0] & ~BNEP_E_BIT) == BNEP_EXTENSION_CONTROL) {
                switch (extHdr[BnepFilterExtHdr_filterType]) {
                case BNEP_SETUP_CONNECTION_REQUEST_MSG:
                    timerResponse = BNEP_SETUP_CONNECTION_RESPONSE_MSG;
                    break;
                case BNEP_FILTER_NET_TYPE_SET_MSG:
                    timerResponse = BNEP_FILTER_NET_TYPE_RESPONSE_MSG;
                    break;
                case BNEP_FILTER_MULTI_ADDR_SET_MSG:
                    timerResponse = BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG;
                    break;
                default:
                    break;
                }
            }
            if (timerResponse != 0) {
                status = BnepSetTimer(panu, timerResponse, btPacket);
                if (status != BT_STATUS_SUCCESS) {
                    goto fail;
                }
                timerResponse = 0;
                startTimer = TRUE;
            }
            btPacket->dataLen += BNEP_EXT_HDR_SIZE + extHdr[BnepExtensionHeader_hdrDataLen];
            if (!lastOne) {
                extHdr += BNEP_EXT_HDR_SIZE + extHdr[BnepExtensionHeader_hdrDataLen];
                lastOne = (extHdr[0] & BNEP_E_BIT) ? FALSE : TRUE;
#if 0
#if XA_ERROR_CHECK == XA_ENABLED
                if (extHdr - etherPkt > L2CAP_MTU - 5) {
                    status = BT_STATUS_INVALID_PARM;
                    goto fail;
                }
#endif
#endif
            }
            else {
                lastOne = FALSE;
            }
        }
#if XA_ERROR_CHECK == XA_ENABLED
//Paul 20110516
        //if (btPacket->dataLen > 256) {
        if (btPacket->dataLen + btPacket->tailLen> L2CAP_MTU - BNEP_GNRL_HDR_SIZE) {			
            status = BT_STATUS_INVALID_PARM;
            goto fail;
        }
#endif
    }
    else {
        btPacket->dataLen = etherPktLen - ETHER_HDR_SIZE;
        btPacket->data = (U8 *)etherPkt + ETHER_HDR_SIZE;
    }

    if (startTimer) {
        BnepStartTimer(panu);
        OS_Report("[BNEP]Control command started");
    }

    /* Send BNEP packet to remote device */
    status = L2CAP_SendData(panu->l2ChannelId, btPacket);
    OS_Report("[BNEP]Send status %d", status);
    OS_Report("[BNEP]Send packet type %d", type);

fail:
    OS_UnlockStack();
    return(status);
}

#if BNEP_ETHERNET_EMUL_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 *            BNEP_SendPkt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a BNEP packet to the remote device.
 *
 * Return:    BT_STATUS_PENDING the send packet process has been 
 *            successfully started.
 */
BtStatus BNEP_SendPkt(BnepPanu *panu,
                      BtPacket *btPacket,
                      U8 *bnepPkt,
                      U16 pktLen)
{
    BtStatus        status;
    BOOL            lastOne, startTimer = FALSE;
    U8             *extHdr;
    BnepControlType timerResponse = 0;

#if XA_ERROR_CHECK == XA_ENABLED
    if (BNEP(callback) == 0) {
        return BT_STATUS_FAILED;
    }
    if (panu == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if ((bnepPkt[0] & ~BNEP_E_BIT) == BNEP_CONTROL) {
        return BT_STATUS_INVALID_PARM;
    }
    if (pktLen > L2CAP_MTU) {
        return BT_STATUS_INVALID_PARM;
    }
#endif

    Assert(BNEP(callback) != 0);
    Assert(panu != 0);
    Assert((bnepPkt[0] & ~BNEP_E_BIT) != BNEP_CONTROL);
    Assert(pktLen <= L2CAP_MTU);

    OS_LockStack();
    //20111103 mtk03036
    btPacket->priority_type	= BTP_FLAG_HIGH_PRIPORTY;
    //20111103 mtk03036 end	
	

    btPacket->dataLen = pktLen - BNEP_TYPE_SIZE;
    btPacket->data = bnepPkt + BNEP_TYPE_SIZE;

    if (bnepPkt[0] & BNEP_E_BIT) {
        switch (bnepPkt[0] & ~BNEP_E_BIT) {
        case BNEP_GENERAL_ETHERNET:
            extHdr = bnepPkt + BnepGeneralPacket_etherPkt_data;
            break;
        case BNEP_COMPRESSED_ETHERNET:
            extHdr = bnepPkt + BnepCompressedPacket_etherPkt_data;
            break;
        case BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY:
            extHdr = bnepPkt + BnepCompSrcPacket_etherPkt_data;
            break;
        case BNEP_COMPRESSED_ETHERNET_DEST_ONLY:
            extHdr = bnepPkt + BnepCompDestPacket_etherPkt_data;
            break;
        default:
            status = BT_STATUS_INVALID_PARM;
            goto fail;
            break;
        }

        /* check for a BNEP_CONTROL command start, start timer if found. */
        startTimer = FALSE;
        lastOne = (extHdr[0] & BNEP_E_BIT) ? FALSE : TRUE;
        while (extHdr[0] & BNEP_E_BIT || lastOne) {
            if ((extHdr[0] & ~BNEP_E_BIT) == BNEP_EXTENSION_CONTROL) {
                switch (extHdr[BnepFilterExtHdr_filterType]) {
                case BNEP_SETUP_CONNECTION_REQUEST_MSG:
                    timerResponse = BNEP_SETUP_CONNECTION_RESPONSE_MSG;
                    break;
                case BNEP_FILTER_NET_TYPE_SET_MSG:
                    timerResponse = BNEP_FILTER_NET_TYPE_RESPONSE_MSG;
                    break;
                case BNEP_FILTER_MULTI_ADDR_SET_MSG:
                    timerResponse = BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG;
                    break;
                default:
                    break;
                }
            }
            if (timerResponse != 0) {
                status = BnepSetTimer(panu, timerResponse, btPacket);
                if (status != BT_STATUS_SUCCESS) {
                    goto fail;
                }
                timerResponse = 0;
                startTimer = TRUE;
            }
            if (!lastOne) {
                extHdr += BNEP_EXT_HDR_SIZE + extHdr[BnepExtensionHeader_hdrDataLen];
                lastOne = (extHdr[0] & BNEP_E_BIT) ? FALSE : TRUE;
            }
            else {
                lastOne = FALSE;
            }
        }
    }

    if (startTimer) {
        BnepStartTimer(panu);
        OS_Report("[BNEP]Control command started");
    }

    OS_Report("[BNEP]Send packet type %d", bnepPkt[0]);

    btPacket->flags &= BTP_FLAG_INUSE;
    btPacket->headerLen = BNEP_TYPE_SIZE;
    btPacket->header[BT_PACKET_HEADER_LEN - BNEP_TYPE_SIZE] = bnepPkt[0];
    /* indicate packet came from BNEP_SendPkt */
    *((BtStatus (**)(BnepPanu *, BtPacket *, U8 *, U16))&btPacket->ulpContext) =  BNEP_SendPkt;

    /* Send BNEP packet to remote device */
    status = L2CAP_SendData(panu->l2ChannelId, btPacket);
    OS_Report("[BNEP]Send status %d", status);

fail:
    OS_UnlockStack();
    return(status);
}
#endif /* BNEP_ETHERNET_EMUL_ONLY == XA_DISABLED */

/*---------------------------------------------------------------------------
 *            BNEP_Control()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a BNEP control packet.
 *
 * Return:    BT_STATUS_PENDING the BNEP control process has been 
 *            successfully started.
 */
BtStatus BNEP_Control(BnepPanu *panu,
                      BtPacket *btPacket,
                      U8 *controlPkt,
                      U16 controlPktLen)
{
    BtStatus status;
    U8             *extHdr;
    BOOL            lastOne, startTimer = FALSE;
    BnepControlType timerResponse = 0;

#if XA_ERROR_CHECK == XA_ENABLED
    if (BNEP(callback) == 0) {
        return BT_STATUS_FAILED;
    }
    if (panu == 0) {
        return BT_STATUS_INVALID_PARM;
    }
    if ((controlPkt[0] & ~BNEP_E_BIT) != BNEP_CONTROL) {
        return BT_STATUS_INVALID_PARM;
    }
    if (controlPktLen > L2CAP_MTU) {
        return BT_STATUS_INVALID_PARM;
    }
#endif

    Assert(BNEP(callback) != 0);
    Assert(panu != 0);
    Assert((controlPkt[0] & ~BNEP_E_BIT) == BNEP_CONTROL);
    Assert(controlPktLen <= L2CAP_MTU);

    OS_LockStack();
    //20111103 mtk03036
    btPacket->priority_type	= BTP_FLAG_HIGH_PRIPORTY;
    //20111103 mtk03036 end	
	
    /* set the BtPacket data pointer to the control packet */
    btPacket->dataLen = controlPktLen - BNEP_TYPE_SIZE;
    btPacket->data = controlPkt + BNEP_TYPE_SIZE;
    
    /* Check flags and set header */
    btPacket->flags &= BTP_FLAG_INUSE;
    btPacket->headerLen = BNEP_TYPE_SIZE;
    btPacket->header[BT_PACKET_HEADER_LEN - BNEP_TYPE_SIZE] = controlPkt[0];
    /* indicate packet came from BNEP_Control */
    *((BtStatus (**)(BnepPanu *, BtPacket *, U8 *, U16))&btPacket->ulpContext) = BNEP_Control;

    switch (controlPkt[BnepControlPkt_hdr_controlType] & ~BNEP_E_BIT) {
    case BNEP_SETUP_CONNECTION_REQUEST_MSG:
        extHdr = controlPkt + BNEP_CONTROL_HDR_SIZE + BNEP_UUID_LENGTH_SIZE +
            controlPkt[BnepControlPkt_data_length]*2; 
        timerResponse = BNEP_SETUP_CONNECTION_RESPONSE_MSG;
        break;
    case BNEP_FILTER_NET_TYPE_SET_MSG:
        extHdr = controlPkt + BNEP_CONTROL_HDR_SIZE + BNEP_LIST_LENGTH_SIZE +
            BEtoHost16(&controlPkt[BnepControlPkt_data_length]); 
        timerResponse = BNEP_FILTER_NET_TYPE_RESPONSE_MSG;
        break;
    case BNEP_FILTER_MULTI_ADDR_SET_MSG:
        extHdr = controlPkt + BNEP_CONTROL_HDR_SIZE + BNEP_LIST_LENGTH_SIZE +
            BEtoHost16(&controlPkt[BnepControlPkt_data_length]); 
        timerResponse = BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG;
        break;
    case BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD:
    case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
    case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
    case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
        extHdr = controlPkt +
            BNEP_CONTROL_HDR_SIZE + sizeof(BnepSetupResponseMsg);
        OS_Report("[BNEP]Control Response");
        break;
    case 0x7F:
        /* this unknown control type is for testing only */
        extHdr = controlPkt + BNEP_CONTROL_HDR_SIZE;
        break;
    default:
        OS_Report("[BNEP]Unknown control type %u",
            controlPkt[BnepControlPkt_hdr_controlType] & ~BNEP_E_BIT);
        return BT_STATUS_INVALID_PARM;
        break;
    }

    if (timerResponse != 0) {
        //status = BnepSetTimer(panu, timerResponse);
        status = BnepSetTimer(panu, timerResponse | BNEP_RETRANSMISSION_ENABLE, btPacket);		
        if (status != BT_STATUS_SUCCESS) {
            goto fail;
        }
        timerResponse = 0;
        startTimer = TRUE;
    }

    /* check if we need to start timers in the extension header */
    if ((controlPkt[0] & BNEP_E_BIT) &&
        (controlPkt - extHdr > controlPktLen)) {
        lastOne = (extHdr[0] & BNEP_E_BIT) ? FALSE : TRUE;
        while (extHdr[0] & BNEP_E_BIT || lastOne) {
            if ((extHdr[0] & ~BNEP_E_BIT) == BNEP_EXTENSION_CONTROL) {
                switch (extHdr[BnepFilterExtHdr_filterType]) {
                case BNEP_SETUP_CONNECTION_REQUEST_MSG:
                    timerResponse = BNEP_SETUP_CONNECTION_RESPONSE_MSG;
                    break;
                case BNEP_FILTER_NET_TYPE_SET_MSG:
                    timerResponse = BNEP_FILTER_NET_TYPE_RESPONSE_MSG;
                    break;
                case BNEP_FILTER_MULTI_ADDR_SET_MSG:
                    timerResponse = BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG;
                    break;
                default:
                    break;
                }
            }
            if (!lastOne) {
                extHdr += BNEP_EXT_HDR_SIZE + extHdr[BnepExtensionHeader_hdrDataLen];
                lastOne = (extHdr[0] & BNEP_E_BIT) ? FALSE : TRUE;
            }
            else {
                lastOne = FALSE;
            }
            if (timerResponse != 0) {
                status = BnepSetTimer(panu, timerResponse, btPacket);
                if (status != BT_STATUS_SUCCESS) {
                    goto fail;
                }
                timerResponse = 0;
                startTimer = TRUE;
            }
        }
    }

    /* start timers */
    if (startTimer) {
        BnepStartTimer(panu);
        OS_Report("[BNEP]Control command started");
    }

    /* Send BNEP packet to remote device */
    status = L2CAP_SendData(panu->l2ChannelId, btPacket);
    OS_Report("[BNEP]Send status %d", status);

fail:
    OS_UnlockStack();
    return(status);
}

/*---------------------------------------------------------------------------
 *            BNEP_SetControlTimeout()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the BNEP control command timeout.
 *
 * Return:    BtStatus
 */
BtStatus BNEP_SetControlTimeout(BnepPanu *panu, TimeT controlTimeout)
{
    BtStatus status;

    OS_LockStack();

    if (controlTimeout > 0 && controlTimeout <= 30) {
        panu->controlTimeout = controlTimeout * 1000;
        status = BT_STATUS_SUCCESS;
    }
    else {
        status = BT_STATUS_INVALID_PARM;
    }

    OS_UnlockStack();
    return status;
}


/*---------------------------------------------------------------------------
 *            BnepGetLocalBnepAddr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the BNEP_ADDR of the local device.
 *
 * Return:    BNEP_ADDR
 */
void BnepGetLocalBnepAddr(BNEP_ADDR *bnepAddr)
{
    OS_LockStack();
    *bnepAddr = BNEP(bnepAddr);
    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            BnepGetRemoteBnepAddr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the BNEP_ADDR of the remote device.
 *
 * Return:    BNEP_ADDR
 */
void BnepGetRemoteBnepAddr(BnepPanu *panu, BNEP_ADDR *bnepAddr)
{
    OS_LockStack();
    *bnepAddr = panu->bnepAddr;
    OS_UnlockStack();
}

/*---------------------------------------------------------------------------
 *            bnep_addr_aton()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Convert a MSB ASCII aa:bb:cc:dd:ee:ff to a MSB BNEP_ADDR value. The
 *            BNEP_ADDR can be either a Bluetooth device address or an ethernet
 *            address.
 *
 * Return:    BNEP_ADDR
 */
void bnep_addr_aton(const char *addr, BNEP_ADDR *bnepAddr)
{
    U8       val, i;
    val = 0;
    
    for (i=0; *addr && i < BNEP_ADDR_SIZE; addr++) {
        if (*addr >= '0' && *addr <= '9')
            val = (val << 4) + *addr - '0';
        else if (*addr >= 'A' && *addr <= 'F')
            val = (val << 4) + *addr - 'A'+10;
        else if (*addr >= 'a' && *addr <= 'f')
            val = (val << 4) + *addr - 'a'+10;
        else bnepAddr->addr[i++] = val;
    }

    if ( i < BNEP_ADDR_SIZE) {
        bnepAddr->addr[i] = val;
    }
    Assert(i == 5);
}

/*---------------------------------------------------------------------------
 *            bnep_addr_ntoa()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Convert a MSB BNEP_ADDR to MSB ASCII notation aa:bb:cc:dd:ee:ff.
 *
 *            AddrString should point to no less than 20 bytes. It will be
 *            null-terminated upon return.
 *
 * Return:    pointer to the converted/provided AddrString
 */
char *bnep_addr_ntoa(const BNEP_ADDR *bnepAddr, char *addrString)
{
    char        *bp = addrString;
    U8           u, l;
    I8           i = 6;
    
    for (i=0; i < 6; i++) {
        u = (bnepAddr->addr[i]/16);
        l = (bnepAddr->addr[i]%16);
        
        if (u < 10)
            *bp++ = '0' + u;
        else *bp++ = 'A' + u-10;
        
        if (l < 10)
            *bp++ = '0' + l;
        else *bp++ = 'A' + l-10;
        
        *bp++ = ':';
    }
    
    *--bp = 0;
    
    return addrString;
}

#endif /* __BT_BNEP__ */

