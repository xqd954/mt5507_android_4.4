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

#ifndef __OBPROTIF_H
#define __OBPROTIF_H
/****************************************************************************
 *
 * File:          obprotif.h
 *
 * Description:   Defines functions called by the Transport Adapter to
 *                indicate events to OBEX.
 * 
 * Created:       February 10, 1997
 *
 * Version:       MTObex 3.4
 *
 * Copyright 1997-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#include <obex.h>
#include <utils.h>

/*---------------------------------------------------------------------------
 * OBEX Transport Interface layer
 *
 *     This layer defines the APIs that are called by OBEX Transport
 *     modules up to the OBEX layer. These functions indicate events or perform
 *     housekeeping. See the "OBEX Transport Driver" layer for information
 *     about calls made by OBEX down to the transport module.
 *
 *     Several different OBEX Transports may be in use simultaneously.
 *     Therefore, most of the APIs here take an ObexAppHandle parameter
 *     which associates the event with a specific application/transport pair.
 *     The ObexAppHandle parameter is provided to the transport
 *     via a transport-specific initialization function, such as
 *     BTSTACK_ClientInit.
 */

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObexDiscReason type
 *
 *     OBEX Transport Disconnection reason. Not all reasons apply to all
 *     transports.
 */
typedef U8 ObexDiscReason;

/* The disconnect happened for an unknown reason. Either asynchronously,
 * or as a result of a failed connect request where no other disconnect
 * reason code applies. 
 */
#define ODR_UNKNOWN             0

/* The disconnect was requested by the client. (via ClientDisconnect()) */
#define ODR_USER_REQUEST        1

/* The connect request failed because the service query failed. */
#define ODR_NO_SERVICE_FOUND    2

/* The connect request failed because a suitable remote device could not
 * be located.
 */
#define ODR_DISCOVERY_FAILED    3

/* The connect request failed because of hardware limitation (SCO is running) */
#define ODR_SCO_REJECT    4

/* The connect request failed because of hardware limitation (other than SCO) */
#define ODR_BTCHIP_REJECT    5

/* End of ObexDiscReason */

/*--------------------------------------------------------------------------
 * ObexPacket structure
 *
 *     Represents all possible types of packet structures (Bluetooth,
 *     IrDA, etc.) These structures are associated with a shared data
 *     buffer which contains the actual transmit data.
 */
typedef struct _ObexPacket
{
    /* Union of packet structures for all supported transports. */
    union {

        /* The node field must be the first field in the structure. */
        ListEntry  node;

#if IRDA_STACK == XA_ENABLED
        /* This field is only accessible when compiled with IRDA_STACK
         * set to XA_ENABLED.
         */
        IrPacket    irp;
#endif /* IRDA_STACK == XA_ENABLED */
        
#if BT_STACK == XA_ENABLED
        /* This field is only accessible when compiled with BT_STACK
         * set to XA_ENABLED.
         */
        BtPacket    btp;
#endif /* BT_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
        /* This field is only accessible when compiled with OEM_STACK
         * set to XA_ENABLED.
         */
        struct _OemPacket {
            /* A node for keeping this packet on a list if necessary */
            ListEntry   node;

            /* A field to store the pointer to the data of the queued packet */
            U8          *buff;

            /* A field for storing the buffer length if the packet is queued. */
            U16         length;
        } oem;
#endif /* OEM_STACK == XA_ENABLED */

    } pkt;

    /* Actual data to be transmitted */
    U8          buffer[OBEX_TX_BUFF_SIZE];
} ObexPacket;

#define btpkt pkt.btp
#define irpkt pkt.irp


/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * OBPROT_Connected()
 *
 *     Notifies the OBEX layer that the transport's connection has been
 *     established. No OBTRANS_ functions are called by OBEX until this
 *     function indicates that the connection is active.
 *
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 */
void OBPROT_Connected(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBPROT_Disconnected()
 *
 *     Notifies the OBEX layer that the transport's connection has been
 *     lost. This function is used for both asynchronous disconnections as
 *     well as the completion of a disconnect request (OBTRANS_Disconnect).
 *
 *     This function automatically unlinks the OBEX Transport interface from
 *     OBEX so that no further OBTRANS_ calls are made.
 * 
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 *     Reason - The reason for the disconnect, taken from the
 *               list of ObexDiscReason defines.
 */
void OBPROT_Disconnected(ObexAppHandle *AppHndl, ObexDiscReason Reason);

/*---------------------------------------------------------------------------
 * OBPROT_LinkTransport()
 *
 *     Associates a specific application handle with an OBEX transport.
 *     This is normally done by a server transport when a connection
 *     indication has been received. When the connection is ready for
 *     data exchange, the transport must still call OBPROT_Connected to
 *     indicate that the transport is ready for data.
 * 
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 *     Trans - transport structure.
 *
 * Returns:
 *     TRUE - If transport was linked to OBEX protocol.
 *
 *     FALSE - If transport was not linked because the application 
 *             already has a connection through a different transport.
 */
BOOL OBPROT_LinkTransport(ObexAppHandle *AppHndl, ObexTransport *Trans);

/*---------------------------------------------------------------------------
 * OBPROT_UnlinkTransport()
 *
 *     Removes the association between an application handle and an OBEX 
 *     transport. This function is not needed when OBPROT_Disconnect is used, 
 *     because OBPROT_Disconnect automatically unlinks the application and 
 *     transport. In cases where a transport must be unlinked when no 
 *     connection is in progress, this function must be called.
 *
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 * Returns:
 *     TRUE - If transport was linked to OBEX protocol.
 *
 *     FALSE - If transport was not linked because the
 *             protocol already has a connection.
 */
void OBPROT_UnlinkTransport(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBPROT_SetInitState()
 *
 *     Resets the OBEX protocol parser. It is not normally necessary for
 *     a transport to call this function because parser initialization
 *     is performed internally by OBEX.
 *
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 */
void OBPROT_SetInitState(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBPROT_ParseRx()
 *
 *     Indicates received data to the OBEX protocol parser.
 *
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 *     RxBuff - Pointer to buffer to parse.
 *
 *     RxLen - Length of the buffer.
 *
 */
void OBPROT_ParseRx(ObexAppHandle *AppHndl, U8 *RxBuff, U16 RxLen);

/*---------------------------------------------------------------------------
 * OBPROT_TxDone()
 *
 *     Returns an ObexPacket to the packet pool. When a transport calls
 *     this function, it also signals to the OBEX layer that more data can
 *     be transmitted if necessary.
 *
 * Parameters:
 *     AppHndl - Application Handle provided during Init.
 *
 *     Packet - transmit buffer we're completing.
 */
void OBPROT_TxDone(ObexAppHandle *AppHndl, ObexPacket *Packet);

/*---------------------------------------------------------------------------
 * OBPROT_ReturnTxBuffer()
 * 
 *    Returns packet to the packet pool. This function is usually only
 *    called (by a transport) when a transmit request fails.
 *
 * Parameters:
 *     Buffer - pointer to buffer received via OBTRANS_SendTxBuffer().
 */
void OBPROT_ReturnTxBuffer(U8 *Buffer);

#endif /* __OBPROTIF_H */
