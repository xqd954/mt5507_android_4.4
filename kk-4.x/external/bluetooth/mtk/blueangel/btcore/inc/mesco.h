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

#ifndef __MESCO_H
#define __MESCO_H

/****************************************************************************
 *
 * File:
 *     $Workfile:mesco.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Management Entity.
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

#include "mesco_adp.h"

#if NUM_SCO_CONNS > 0

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SCO_CreateLink()
 *     
 *     Creates an SCO (audio) connection to a remote device. An ACL
 *     link with the device must already be established.
 *
 * Parameters:
 *     scocon - Receives a pointer to an SCO connection structure
 *
 *     remDev - pointer to remote device.
*
 *     types - indicates packet types allowed for connection.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has been started successfully.
 *         A BTEVENT_SCO_CONNECT_CNF event will be sent to the SCO handler to
 *         indicate the status of the SCO connection. If the operation is
 *         successful, then the error code will be BEC_NO_ERROR. Other
 *         codes indicate errors. If the remote device attempts a SCO
 *         connection generating an incoming connection at the same time,
 *         the outgoing connection will fail with the error
 *         BEC_LMP_TRANS_COLLISION or BEC_REQUEST_CANCELLED. In this case,
 *         the application should ignore the outgoing connection error and
 *         accept the incoming connection to resolve the conflict.
 *
 *     BT_STATUS_FAILED - A SCO handler must be registered first. See the 
 *         SCO_RegisterHandler function.
 *
 *     BT_STATUS_NO_RESOURCES - Too many SCO connections already in place
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus SCO_CreateLink(BtScoConnect **scocon, BtRemoteDevice *remDev, BtScoPacketType types);

/*---------------------------------------------------------------------------
 * SCO_SetupLink()
 *     
 *     Creates or modifies a SCO or eSCO connection to a remote device. An
 *     ACL link with the device must already be established.
 *
 * Parameters:
 *     scocon - In the case of a new connection, receives a pointer to a
 *         SCO connection structure. In the case of an existing eSCO
 *         connection to be modified, is the address of a pointer to
 *         an initialized SCO connection structure.
 *
 *     remDev - pointer to remote device.
 *
 *     scoTxParms - pointer to initialized BtScoTxParms structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has been started successfully.
 *         A BTEVENT_SCO_CONNECT_CNF event will be sent to the SCO handler to
 *         indicate the status of a new eSCO connection.
 *         A BTEVENT_SCO_CONN_CHNG event will be sent to the SCO handler to
 *         indicate the status of a modified eSCO connection.
 *         If the operation is successful then the error code will be
 *         BEC_NO_ERROR. Other codes indicate errors.  If the remote device
 *         attempts a SCO connection generating an incoming connection at the
 *         same time, the outgoing connection will fail with the error
 *         BEC_LMP_TRANS_COLLISION or BEC_REQUEST_CANCELLED. In this case,
 *         the application should ignore the outgoing connection error and
 *         accept the incoming connection to resolve the conflict.

 *
 *     BT_STATUS_FAILED - A SCO handler must be registered first. See the 
 *         SCO_RegisterHandler function.
 *
 *     BT_STATUS_NO_RESOURCES - Too many SCO connections already in place
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus SCO_SetupLink(BtScoConnect **scocon, BtRemoteDevice *remDevice, BtScoTxParms *scoTxParms);

/*---------------------------------------------------------------------------
 * SCO_DisconnectLink()
 *
 *     Disconnects the SCO link.
 *
 * Parameters:
 *     scocon - SCO connection to disconnect.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation has started successfully.
 *         When the link is disconnected, BTEVENT_SCO_DISCONNECT will be
 *         sent to the handler(s).
 *
 *     BT_STATUS_IN_PROGRESS - A disconnect operation is already in progress.
 *
 *     BT_STATUS_BUSY - A connect operation is in progress; wait for
 *         completion of the connection process before calling this function.
 *
 *     BT_STATUS_SUCCESS - The given BtScoConnect structure is not
 *         currently connected.
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus SCO_DisconnectLink(BtScoConnect *scocon);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * SCO_SendData()
 *
 *     Sends the specified audio data on the SCO link.
 *
 * Requires:
 *     BT_SCO_HCI_DATA enabled.
 *
 * Parameters:
 *     scocon - The connection over which to send data.
 *
 *     packet - The packet of data to send. After this call, the SCO
 *         manager owns the packet. When the packet has been transmitted
 *         to the host controller, BTEVENT_SCO_DATA_CNF is sent to the
 *         handler(s).
 *
 * Returns:
 *     BT_STATUS_PENDING - The packet was queued successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No SCO connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus SCO_SendData(BtScoConnect *scocon, BtPacket *packet);
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * SCO_RegisterHandler()
 *
 *     Register the given SCO handler for the given remote device. Only one
 *     handler can be registered for any one ACL connection. Because multiple
 *     SCO connections may exist over a single ACL link, this handler
 *     may receive messages for up to three distinct SCO connections
 *     (BtScoConnect structures).
 *
 * Parameters:
 *     remDev - Register a handler for this ACL device connection.
 *
 *     handler - Handler to notify of all SCO events pertaining to this
 *         device. Note that this same handler may be registered as
 *         a global ME event handler. If the handler's mask allows SCO
 *         events, two copies of each SCO event will be received.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was registered.
 *
 *     BT_STATUS_BUSY - A handler is already in place (it must be
 *         unregistered with SCO_UnregisterHandler).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus SCO_RegisterHandler(BtRemoteDevice *remDev, BtHandler *handler);

/*---------------------------------------------------------------------------
 * SCO_UnregisterHandler()
 *
 *     Unregister the given SCO handler for the given remote device. The SCO 
 *     handler is automatically unregistered after a BTEVENT_LINK_DISCONNECT 
 *     event is received by the ACL link associated with the given 
 *     BtRemoteDevice structure.
 *
 * Parameters:
 *     remDev - Remote device.
 *
 *     handler - Handler to unregister for "remDev".
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was successfully unregistered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter or the specified handler
 *         was not previously registered for "remDev" (XA_ERROR_CHECK only).
 */
BtStatus SCO_UnregisterHandler(BtRemoteDevice *remDev, BtHandler *handler);


/*---------------------------------------------------------------------------
 * SCO_AcceptIncomingConnection()
 *
 *     Accepts the given SCO connection request from the remote device. For
 *     Bluetooth v1.2 compliant radios, the "scoTxParms" structure fields
 *     in "scoCon" must be set correctly per Bluetooth v1.2 eSCO connection
 *     procedures.
 *
 * Parameters:
 *     scoCon - Connection structure indicated during BTEVENT_SCO_CONNECT_REQ
 *         event. The BTEVENT_SCO_CONNECT_REQ event will also indicate the
 *         "scoTxParms" and "scoRxParms" structures when the link type is
 *         BLT_ESCO. The "scoTxParms" structure fields must be set to
 *         correct values when the link type is BLT_ESCO.
 *
 *     handler - The handler for this connection. This handler must have
 *         been previously registered with SCO_RegisterHandler.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection will be accepted. When the
 *         connection is up, the handler will receive the
 *         BTEVENT_SCO_CONNECT_IND event with "errCode" set to zero. If
 *         an error occurred during the connection process, "errCode"
 *         will indicate the type of error.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter or handlers did not
 *         match (XA_ERROR_CHECK only).
 *
 */
BtStatus SCO_AcceptIncomingConnection(BtScoConnect *scoCon, BtHandler *handler);

/*---------------------------------------------------------------------------
 * SCO_RejectIncomingConnection()
 *
 *     Rejects the incoming SCO connection.
 *
 * Parameters: 
 *     scoCon - Connection structure indicated during
 *         BTEVENT_SCO_CONNECT_REQ event.
 *
 *     handler - The handler for this connection. This handler must have
 *         been previously registered with SCO_RegisterHandler.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection will be rejected. The handler
 *         will receive a BTEVENT_SCO_CONNECT_IND with a non-zero "errCode"
 *         field.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter or handler did not
 *         match (XA_ERROR_CHECK only).
 */
BtStatus SCO_RejectIncomingConnection(BtScoConnect *scoCon, BtHandler *handler, BtErrorCode reason);

#endif /* NUM_SCO_CONNS > 0 */ 

#endif /* __MESCO_H */

