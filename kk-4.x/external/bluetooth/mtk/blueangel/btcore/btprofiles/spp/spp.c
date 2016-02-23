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
 *     $Workfile:spp.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Sample Port API Profile for RFCOMM.
 *
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
#ifdef __BT_SPP_PROFILE__

#include "bt_common.h"
#include "me.h"
#include "sdp.h"
#include "rfcomm.h"
#include "spp.h"
#include "osapi.h"
#include "conmgr.h"

#include "sdap.h"
#include "btconfig.h"
#include "bluetooth_struct.h"
#include "btbm_adp.h"
//#include "bt_adp_spp.h"
#include "bt_common_config.h"
#include "bt_feature.h"
#include "bt_os_api.h"
#include "bluetooth_spp_struct.h"
#ifdef __SPP_THROUGHPUT_TEST__
#include "time.h"
#endif


#if XA_MULTITASKING == XA_DISABLED
#if BT_SPP_IGNORE_MULTITASKING != XA_ENABLED
#   error !!! SPP is designed for a multitasking OS.
#endif /* BT_SPP_IGNORE_MULTITASKING */
#endif /* XA_MULTITASKING == XA_DISABLED */

U32 spp_tx_timer1;
U32 spp_tx_timer2;

/***********************************************************************
 *
 * Defines, Typedefs and Structures
 *
 ***********************************************************************/

#define Display(x)          Report(x)

#define txSleepTime 500 /* ms */
#define txSleepCount 20 /* every txSleepCount, sleep txSleepTime */

#define DC1     0x11    /* Default XON char */
#define DC3     0x13    /* Default XOFF char */
typedef BOOL(*DevState) (SppDev *, RfCallbackParms *);

#define stateDisconnected     stateDefaultHandler
#define stateDeactivating     stateDefaultHandler

/* Note 2007-0714: From this, the following definition and function are only valid if SPP_CLIENT or SPP_SERVER is XA_ENABLED */
#if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)
//extern kal_uint8 *BtGet_SPPClientFirstConnDataPattern(kal_uint8 *len);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
extern void SPPOS_CheckReadyToWrite(void *dev);

#ifdef __GEMINI__
extern module_type Get_DualSim_Uart_Default_Owner(void);
#endif 

#if (SPP_CLIENT == XA_ENABLED)
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
#if (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED)
static U16 spp_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST
};
#endif /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */

#endif /* (SPP_CLIENT == XA_ENABLED) */

extern kal_bool CheckSPPFlowControlSetting(module_type owner);

/***********************************************************************
 *
 * Function Prototypes
 *
 ***********************************************************************/

static void rfCallbackFunc(RfChannel *chan, RfCallbackParms *parms);
static void defaultEventHandler(SppDev *dev, RfCallbackParms *parms);
static BOOL stateDefaultHandler(SppDev *dev, RfCallbackParms *parms);
static BOOL stateConnected(SppDev *dev, RfCallbackParms *parms);
static BOOL stateClosed(SppDev *dev, RfCallbackParms *parms);
static void sendToRfComm(SppDev *dev);
static void setHighWaterMark(SppDev *dev);
static void advanceCredit(SppDev *dev);

/* Remove static keyword, the sppConnect() function is also used in bt_adp_spp.c */
void closeDevice(SppDev *dev);
static void initDevice(SppDev *dev);
static void sppSleepHandler(EvmTimer *timer);

#if (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED)
static BtStatus SPPStartServiceQuery(SppDev *dev);

static void SPPQueryCallback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value);
#endif /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */

#if SPP_CLIENT == XA_ENABLED
/* Remove static keyword, because this function will be also used in other files */
BtStatus openClientPort(SppDev *dev, BtRemoteDevice *btDevice);

#if (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED)
static BtStatus queryForService(SppDev *dev);
static void queryCallback(const BtEvent *event);
#endif /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */

/* Remove static keyword, the sppConnect() function is also used in bt_adp_spp.c */
BtStatus sppConnect(SppDev *dev, BtRemoteDevice *remDev, U8 serverId);

#endif /* SPP_CLIENT == XA_ENABLED */
#if SPP_SERVER == XA_ENABLED
static BtStatus openServerPort(SppDev *dev);
static void deRegisterService(SppDev *dev);
static BtStatus addSdpService(SppDev *dev);
static void removeSdpService(SppDev *dev);
void spp_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status);

#endif /* SPP_SERVER == XA_ENABLED */

#ifdef __SPP_AUTO_TX__
extern U8 sppAutoTxFlag;
extern U16 sppAutoTxCnt;

#define spp_auto_tx_size (329)
static U8 sppAutoTxBuff[spp_auto_tx_size];
#endif /* __SPP_AUTO_TX__ */ 

extern SPPCallback SPPCB;

#ifdef __SPP_THROUGHPUT_TEST__
#define SPP_THROUGHPUT_TEST_DATA_LENGTH (50 * 1024 * 1024)	// 1 GBytes
#define SPP_THROUGHPUT_TEST_DATA_PERIODIC_LENGTH (1024*1024*10)		// 10 MBytes
extern U32 g_sppThroughputTotalSentLength;
extern U32 g_sppThroughputPeriodicSentLength;
extern U32 g_sppThroughputTotalGotLength;
extern U8 g_sppThroughputTestClientPort;
extern U8 g_sppThroughputTestServerPort;
extern time_t g_sppThroughputTestTimeSendStart;
extern time_t g_sppThroughputTestTimeSendStop;
extern time_t g_sppThroughputTestPeriodicTimeSendStop;
extern time_t g_sppThroughputTestTimeGetStart;
extern time_t g_sppThroughputTestTimeGetStop;

extern void spp_throughput_test_send(BOOL bServer);
#endif

/***********************************************************************
 *
 * ROM Variables
 *
 ***********************************************************************/

/*-------------------------------------------------------------------------
 * SPP state machine.
 */
static const DevState deviceState[] = 
{
    stateClosed,
    stateDisconnected,
    stateConnected, 
    stateDeactivating
};

/****************************************************************************
 *
 * RAM Data Section
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Function Section
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  SPP_InitDevice
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev             [IN]        
 *  txPacket        [IN]        
 *  numPackets      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPP_InitDevice(SppDev *dev, BtPacket *txPacket, U16 numPackets)
{
    U16 i;

    if ((!dev) || ((dev->portType != SPP_CLIENT_PORT) && (dev->portType != SPP_SERVER_PORT)) || (txPacket == NULL) ||
        (numPackets == 0))
    {
        return;
    }

    dev->state = DEVICE_STATE_CLOSED;
#ifdef __SPP_SLEEP__
    /* Initialize command sppSleep timer */
    dev->sppSleep.func = sppSleepHandler;
    dev->sppSleep.context = NULL;
    dev->txCount = 0;
#endif /* __SPP_SLEEP__ */        
    dev->con_id = 0;

    InitializeListHead(&dev->txPacketList);
    for (i = 0; i < numPackets; i++)
    {
        InsertHeadList(&dev->txPacketList, (ListEntry*) (txPacket + i));
    }
}


/*****************************************************************************
 * FUNCTION
 *  SPP_Open
 * DESCRIPTION
 *  This is the common entry poS16 for opening a server or client port.
 * PARAMETERS
 *  dev             [IN]        
 *  btDevice        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_Open(SppDev *dev, BtRemoteDevice *btDevice)
{
    BtStatus rc;

    if (!dev)
    {
        return BT_STATUS_FAILED;
    }

    if (dev->state != DEVICE_STATE_CLOSED)
    {
        return BT_STATUS_FAILED;
    }

    SPPOS_LockDev(dev->osDev);

    initDevice(dev);

    if (dev->portType & SPP_SERVER_PORT)
    {
    #if SPP_SERVER == XA_ENABLED
        rc = openServerPort(dev);
    #else 
        rc = BT_STATUS_FAILED;
    #endif 
    }
    else
    {
    #if SPP_CLIENT == XA_ENABLED
        rc = openClientPort(dev, btDevice);
    #else 
        rc = BT_STATUS_FAILED;
    #endif 
    }

    if ((rc == BT_STATUS_SUCCESS) || (rc == BT_STATUS_PENDING))
    {
        dev->state = DEVICE_STATE_DISCONNECTED;
        rc = BT_STATUS_SUCCESS;
    }
    else
    {
        rc = BT_STATUS_FAILED;
        dev->state = DEVICE_STATE_CLOSED;
    }

    SPPOS_UnlockDev(dev->osDev);

    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  SPP_Close
 * DESCRIPTION
 *  Close the device and release any resources as appropriate for your OS.
 *  The caller of this function assures that its caller owns the device.
 *  Therefore, if dev->state == NULL, return success anyway.
 *  This condition will exist if the remove device closes the connection first.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_Close(SppDev *dev)
{
    BtStatus status = BT_STATUS_FAILED;

    SPPOS_LockDev(dev->osDev);

    if (dev->state != DEVICE_STATE_CLOSED)
    {
        if (dev->state == DEVICE_STATE_CONNECTED)
        {
            SPPOS_UnlockDev(dev->osDev);
            status = RF_CloseChannel(&dev->channel);
            if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
            {
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = BT_STATUS_FAILED;
            }
            SPPOS_LockDev(dev->osDev);
        }
        else
        {
            status = BT_STATUS_SUCCESS;
        }
    }
    /* Bingyi: In Client port case, this port should be disconnected and the state has been changed to DEVICE_STATE_CLOSED */
    else if (dev->portType & SPP_CLIENT_PORT)
    {
    	status = BT_STATUS_SUCCESS;
    }

    SPPOS_UnlockDev(dev->osDev);

#if SPP_SERVER == XA_ENABLED
    if ((dev->portType & SPP_SERVER_PORT) && (dev->state != DEVICE_STATE_CLOSED))
    {
        deRegisterService(dev);
        closeDevice(dev);
    }
#endif /* SPP_SERVER == XA_ENABLED */

    dev->state = DEVICE_STATE_CLOSED;

#ifdef __SPP_SLEEP__ 
    if (dev->sppSleep.context != NULL)
    {
        EVM_CancelTimer(&(dev->sppSleep));
    }
#endif /* __SPP_SLEEP__    */
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SPP_Disconnect
 * DESCRIPTION
 *  Close the device and release any resources as appropriate for your OS.
 *  The caller of this function assures that its caller owns the device.
 *  Therefore, if dev->state == NULL, return success anyway.
 *  This condition will exist if the remove device closes the connection first.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_Disconnect(SppDev *dev)
{
    BtStatus status = BT_STATUS_FAILED;

    SPPOS_LockDev(dev->osDev);

    if (dev->state != DEVICE_STATE_CLOSED)
    {
        if (dev->state == DEVICE_STATE_CONNECTED)
        {
            SPPOS_UnlockDev(dev->osDev);
            status = RF_CloseChannel(&dev->channel);
            if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
            {
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = BT_STATUS_FAILED;
            }
            SPPOS_LockDev(dev->osDev);
        }
        else
        {
            //In this case, dev->state is in DEVICE_STATE_DISCONNECTED:
            //If tihs is a SPPClient, it may not return from openClientPort() but already call openClientPort() 
            //OR, it may already call sppConnect( ) successfully and wait for peer's rsp 
            status = BT_STATUS_SUCCESS;
        }
    }

    SPPOS_UnlockDev(dev->osDev);
    dev->state = DEVICE_STATE_DISCONNECTED;
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  SPP_Read
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_Read(SppDev *dev)
{

    advanceCredit(dev);
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  SPP_Write
 * DESCRIPTION
 *  Driver write routine. Data is copied to the driver's buffer and
 *  sent to RFCOMM. The caller's buffer is free to reuse when this when call returns.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_Write(SppDev *dev)
{
    sendToRfComm(dev);
#ifndef __SPP_ORG_TRANSMIT_METHOD__
    SPPOS_CheckReadyToWrite(dev);
#endif 
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  SPP_Ioctl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev             [IN]        
 *  function        [IN]        
 *  arg             [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_Ioctl(SppDev *dev, U16 function, void *arg)
{
    BtStatus status;

    SPPOS_LockDev(dev->osDev);

    dev->portSettings.parmMask = 0;
    status = SPPOS_Ioctl(dev->osDev, function, arg);

#if RF_SEND_CONTROL == XA_ENABLED
    if ((status == BT_STATUS_SUCCESS) && (dev->portSettings.parmMask))
    {
        status = SPP_SendPortSettings(dev);
    }
#endif /* RF_SEND_CONTROL == XA_ENABLED */

    SPPOS_UnlockDev(dev->osDev);

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SPP_SetBaud
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev         [IN]        
 *  baud        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPP_SetBaud(SppDev *dev, U8 baud)
{
    dev->portSettings.baudRate = baud;
    dev->portSettings.parmMask = RF_PARM_BAUDRATE;
}


/*****************************************************************************
 * FUNCTION
 *  SPP_SetFlowControl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 *  val     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPP_SetFlowControl(SppDev *dev, U8 val)
{
    dev->portSettings.flowControl = val;
    dev->portSettings.parmMask |= RF_PARM_FLOW_DTR_DSR | RF_PARM_FLOW_RTS_CTS | RF_PARM_FLOW_XON_XOFF;
}


/*****************************************************************************
 * FUNCTION
 *  SPP_SetDataFormat
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 *  val     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPP_SetDataFormat(SppDev *dev, U8 val)
{
    dev->portSettings.dataFormat =
        val & (U8) (RF_PARITY_TYPE_MASK | RF_PARITY_MASK | RF_STOP_BITS_MASK | RF_DATA_BITS_MASK);
    dev->portSettings.parmMask |= RF_PARM_DATA_FORMAT;
}

#if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)


/*****************************************************************************
 * FUNCTION
 *  SPP_SetModemControl
 * DESCRIPTION
 *  This function SPP_SetModemControl(): is used to modify DCD value when dial-up (PPP Procedure) is involved
 * PARAMETERS
 *  dev         [IN]        
 *  signals     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_SetModemControl(SppDev *dev, RfSignals signals)
{
    BtStatus status = BT_STATUS_SUCCESS;

    dev->lModemStatus.signals = signals;

#if RF_SEND_CONTROL == XA_ENABLED
    status = RF_SetModemStatus(&dev->channel, &dev->lModemStatus);
    if (status == BT_STATUS_PENDING)
    {
        status = BT_STATUS_SUCCESS;
        SPPOS_Wait(dev->osDev, RF_T2_TIMEOUT);
    }
    else
    {
        status = BT_STATUS_FAILED;
    }
#endif /* RF_SEND_CONTROL == XA_ENABLED */
    return status;

}
#endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */ 


/*****************************************************************************
 * FUNCTION
 *  SPP_SendPortSettings
 * DESCRIPTION
 *  Tell RFCOMM to send the port settings to the remote device.
 *  Note that we can have only one outstanding port request at a time.
 *  To ensure that we will block until the response is returned or a timer expires.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPP_SendPortSettings(SppDev *dev)
{
#if RF_SEND_CONTROL == XA_ENABLED
    BtStatus rc;

    SPPOS_UnlockDev(dev->osDev);

    kal_trace(TRACE_GROUP_1, SENDING_PORT_SETTINGS);
    if (RF_RequestPortSettings(&dev->channel, &dev->portSettings) == BT_STATUS_PENDING)
    {
        rc = BT_STATUS_SUCCESS;
        SPPOS_Wait(dev->osDev, RF_T2_TIMEOUT);
    }
    else
    {
        rc = BT_STATUS_FAILED;
    }
    SPPOS_LockDev(dev->osDev);

    return rc;
#else /* RF_SEND_CONTROL == XA_ENABLED */
    return BT_STATUS_SUCCESS;
#endif /* RF_SEND_CONTROL == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  sppSleepHandler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void sppSleepHandler(EvmTimer *timer)
{
bt_prompt_trace(MOD_BT, "[BT] sppSleepHandler");
    if (timer->context != NULL)
    {
        sendToRfComm((SppDev*) (timer->context));
        timer->context = NULL;
        kal_trace(TRACE_GROUP_1, SPP_SPPSLEEPHANDLER);
    }
}

#if SPP_CLIENT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  sppConnect
 * DESCRIPTION
 *  If remDev is not NULL, save the args in the device struct.
 *  If the device is a client and already open, tart an RFCOMM connection to a server.
 * PARAMETERS
 *  dev             [IN]        
 *  remDev          [IN]        
 *  serverId        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus sppConnect(SppDev *dev, BtRemoteDevice *remDev, U8 serverId)
{
    BtStatus rc = BT_STATUS_FAILED;

    if (remDev)
    {
        dev->type.client.remDev = remDev;
        dev->type.client.serverId = serverId;
    }
    /* In operClientPort(): dev->state is changed to DEVICE_STATE_DISCONNECTED */
    if (dev->state == DEVICE_STATE_DISCONNECTED)
    {
        /* RF_OpenClientChannel checks for null remDev so we need not. */
        rc = RF_OpenClientChannel(dev->type.client.remDev, dev->type.client.serverId, &dev->channel, dev->credit);

        if (rc == BT_STATUS_PENDING)
        {
            rc = BT_STATUS_SUCCESS;
        }
    }
    else if (dev->state == DEVICE_STATE_CONNECTED)
    {
        rc = BT_STATUS_SUCCESS;
    }

    return rc;
}

#if (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  queryForService
 * DESCRIPTION
 *  Start an SDP query of the specified remoted device for a serial port.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus queryForService(SppDev *dev)
{

    if (!dev)
    {
        return 0;
    }

    dev->type.client.sdpToken.attribId = AID_PROTOCOL_DESC_LIST;
    dev->type.client.sdpToken.uuid = PROT_RFCOMM;
    dev->type.client.sdpToken.mode = BSPM_BEGINNING;
    dev->type.client.sdpToken.callback = queryCallback;
    if (SDP_Query(&dev->type.client.sdpToken, BSQM_FIRST) == BT_STATUS_PENDING)
    {
        return BT_STATUS_SUCCESS;
    }
    else
    {
        return BT_STATUS_FAILED;
    }
}
#endif /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */

#endif /* SPP_CLIENT == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  initDevice
 * DESCRIPTION
 *  Do some common initialization to device.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void initDevice(SppDev *dev)
{

    /* Initialize RS232 signals and other port settings */
    dev->portSettings.baudRate = RF_BAUD_9600;
    dev->portSettings.dataFormat = RF_DATA_BITS_8 | RF_STOP_BITS_1 | RF_PARITY_NONE;
    dev->portSettings.flowControl = RF_FLOW_CTRL_NONE;
#ifdef __SPP_XONOFF_CHAR__
    dev->portSettings.xonChar = DC1;
    dev->portSettings.xoffChar = DC3;
#endif /* __SPP_XONOFF_CHAR__ */ 
#ifdef __SPP_LINE_STATUS__
    dev->lineStatus = 0;
#endif 

#ifdef __SPP_GET_REMOTE_MODEM_STATUS__
    dev->rModemStatus.signals = 0;
#endif 

    dev->credit = 1;

    /* Flush the buffers */
    SPPOS_FlushRx(dev->osDev);
    SPPOS_FlushTx(dev->osDev);
}

#if SPP_SERVER == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  addSdpService
 * DESCRIPTION
 *  Establish some links in the device structure and register the service with SDP.
 *  When we initialized the device structure (SPP_InitDevice),
 *  we copied some SDP-related templates into the device structure.
 *  Some fields in the device structure have been updated after SPP_InitDevice,
 *  so now we can go through and fix some links.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS
 *  Error code from SDP_AddRecord
 *****************************************************************************/
static BtStatus addSdpService(SppDev *dev)
{
    BtStatus status;

    /* Put the channel (serverId) in the Protocol Descriptor list */
    /* Remove by MTK Dlight, we assign the RFCOMM channel centrialize */
    /*
     * insertRfCommChannel(dev->type.sppService->sdpRecord, 
     * dev->type.sppService->service.serviceId);
     */
    status = SDP_AddRecord(dev->type.sppService->sdpRecord);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  removeSdpService
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void removeSdpService(SppDev *dev)
{
    //AssertEval(SDP_RemoveRecord(dev->type.sppService->sdpRecord) != BT_STATUS_FAILED);
    /* Unnecessary to assert here, since SDP may return failure if SDP context is cleared when BT is off. */
    if (SDP_RemoveRecord(dev->type.sppService->sdpRecord) == BT_STATUS_FAILED)
    {
        OS_Report("[SPP] remove sdp service failed!");
    }
}


/*****************************************************************************
 * FUNCTION
 *  openServerPort
 * DESCRIPTION
 *  Register the server with RFCOMM and initialize values.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS
 *  BT_STATUS_FAILED
 *****************************************************************************/
static BtStatus openServerPort(SppDev *dev)
{
    BtStatus bts;

    dev->channel.callback = rfCallbackFunc;
    dev->channel.maxFrameSize = RF_MAX_FRAME_SIZE;
    dev->channel.priority = 63;
    dev->state = DEVICE_STATE_DISCONNECTED;
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
    RF_ChannelInit(&(dev->channel), BT_SEC_LOW);
#else 
    RF_ChannelInit(&(dev->channel), BT_SEC_MED);
#endif 
    SPPOS_UnlockDev(dev->osDev);
    /* Does not return pending */
#ifdef MULTI_RF_CHANNEL
    bts = RF_RegisterServerChannel(&dev->channel, dev->type.sppService->rfService, dev->credit);   
#else
    bts = RF_RegisterServerChannel(&dev->channel, &dev->type.sppService->service, dev->credit);
#endif
    SPPOS_LockDev(dev->osDev);
    if (bts == BT_STATUS_SUCCESS)
    {   
    #ifdef MULTI_RF_CHANNEL
        RfServicePack* rfs;
        rfs = ContainingRecord(dev->type.sppService->rfService, RfServicePack, service);
        rfs->refCount++;
        /*  kal_trace(
         *   TRACE_GROUP_1,
         *   SPP_OPEN_SERVICE_AVAILABLE_ON_RFCOMM_CHANNEL_xD,
         *   dev->type.sppService->rfService->serviceId); 
         */
    #else
        kal_trace(
            TRACE_GROUP_1,
            SPP_OPEN_SERVICE_AVAILABLE_ON_RFCOMM_CHANNEL_xD,
            dev->type.sppService->service.serviceId);    
    #endif
        if (dev->type.sppService->numPorts == 0)
        {
            bts = addSdpService(dev);
            if (bts != BT_STATUS_SUCCESS)
            {
                kal_trace(TRACE_GROUP_1, ADD_SERVICE_RECORD_FAILED);
                bts = BT_STATUS_FAILED;
                deRegisterService(dev);
                closeDevice(dev);
                dev->type.sppService->numPorts--;   /* Negate following increment */
            }
        }
        dev->type.sppService->numPorts++;
    }
    else
    {
        Assert(0);
    }

    return bts;
}


/*****************************************************************************
 * FUNCTION
 *  spp_cmgr_event_callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  cmgr_hdlr       [IN]        
 *  event           [IN]        
 *  status          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void spp_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status)
{
}

#endif /* SPP_SERVER == XA_ENABLED */

#if SPP_CLIENT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  openClientPort
 * DESCRIPTION
 *  Initialize values.
 * PARAMETERS
 *  dev             [IN]        
 *  btDevice        [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS
 *  BT_STATUS_INVALID_PARM
 *****************************************************************************/
BtStatus openClientPort(SppDev *dev, BtRemoteDevice *btDevice)
{
    BtStatus status;

#if XA_ERROR_CHECK == XA_ENABLED
    if ((btDevice == 0) || (ME_FindRemoteDevice(&btDevice->bdAddr) != btDevice))
    {
        return BT_STATUS_INVALID_PARM;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

#if !defined (__BT_2_1_SIMPLE_PAIRING__)
    RF_ChannelInit(&(dev->channel), BT_SEC_LOW);
#else 
    RF_ChannelInit(&(dev->channel), BT_SEC_MED);
#endif 

    dev->channel.callback = rfCallbackFunc;
    dev->channel.maxFrameSize = RF_MAX_FRAME_SIZE;
    dev->channel.priority = 63;
    dev->state = DEVICE_STATE_DISCONNECTED;
    dev->type.client.remDev = btDevice;

    if (btDevice->discTimeoutLevel == BT_L2CAP_DISCONNECT_TIMEOUT_500MS)
    {
        kal_trace(TRACE_GROUP_1, BT_SPPC_DISC_TIMER_500MS);
    }
    else if (btDevice->discTimeoutLevel == BT_L2CAP_DISCONNECT_TIMEOUT_50S)
    {
        kal_trace(TRACE_GROUP_1, BT_SPPC_DISC_TIMER_50S);
    }

#if (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED)
    dev->type.client.sdpToken.rm = btDevice;
    /* status = queryForService(dev); */
    status = SPPStartServiceQuery(dev);
#else /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */
    /* Skip SDP query procedure, do sppConnect() directly */
    status = BT_STATUS_SUCCESS;
    dev->type.client.flag |= SEND_SPPC_SDP_REQ;
    dev->type.client.flag |= RECV_SPPC_SDP_RSP;
#endif /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */

    return status;
}

#if (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  SPPStartServiceQuery
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus SPPStartServiceQuery(SppDev *dev)
{
    BtStatus status;
    sdap_service_search_multi_attribute_struct search_pattern;

    search_pattern.rm = dev->type.client.sdpToken.rm;
    search_pattern.sqt = &dev->type.client.sdpToken;
    /* Search the SPP server record of remote devices */
    search_pattern.uuid = dev->uuid;
    search_pattern.callback = SPPQueryCallback;
    search_pattern.attribute_num = 0x01;
    search_pattern.attribute_id = spp_attribute_id_list;
    status = SDAP_ServiceSearchMultipleAttribute(search_pattern);
    /* Report(("Return status=%d of SPPStartServiceQuery",status)); */
    kal_trace(TRACE_GROUP_1, BT_SPPC_SPPSTARTSERVICEQUERY_STATUS, status);
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  SPPQueryCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  sqt                 [IN]        
 *  result              [IN]        
 *  attribute_index     [IN]        
 *  attribute_value     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SPPQueryCallback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value)
{

    BD_ADDR *bt_addr_for_connect = NULL;
    btbm_bd_addr_t spp_btaddr_t;
    bt_spp_connect_cnf_struct *bt_spp_connect_cnf = NULL;
    SPP_DEV_OS *sppclient = NULL;
    SppDev *dev = NULL;
    bt_spp_disconnect_cnf_struct *bt_spp_disconnect_cnf = NULL;

    /* Check if this sdp query rsp is for one specific SPP client or not */
    sppclient = SearchSppClientOsDevBySdpTkn(sqt);
    Assert(sppclient != NULL);

    if (sppclient->sppDev.type.client.flag & SEND_SPPC_SDP_REQ)
    {
        sppclient->sppDev.type.client.flag |= RECV_SPPC_SDP_RSP;
    }
    else
    {
        /* Report(("In SPPQueryCallback(): This SPP query rsp is not for SPP client")); */
        kal_trace(TRACE_GROUP_1, BT_SPPC_SDP_QUERY_NOT_FOR_IT);
        return;
    }

    bt_addr_for_connect = (&(sppclient->sppDev.type.client.remDev->bdAddr));
    CONVERT_ARRAY2BDADDR(&spp_btaddr_t, bt_addr_for_connect->addr);

    dev = (&(sppclient->sppDev));

    /* Check if this SPPClient already set the RECV_SPPC_DISCONNECT_REQ or RECV_SPPC_DEACTIVATE_REQ flags or not */
    if ((dev->type.client.flag & RECV_SPPC_DEACTIVATE_REQ) && (dev->type.client.flag & RECV_SPPC_DISCONNECT_REQ))
    {
    /* #ifdef CUSTOM_SPP */
        SPPCallbackParms parms;               
    /* #endif #ifdef CUSTOM_SPP */                                                         
        /* Send DISC_CNF back to MMI here */
        /* Because the SPP connection is not yet issued, return CNF here */

        sppa_send_disconnect_cnf_result(
            ((SPP_DEV_OS*) dev)->port,
            dev->con_id,
            spp_btaddr_t.lap,
            spp_btaddr_t.uap,
            spp_btaddr_t.nap,
            SPP_DISCONNECT_SUCCESS
    #if DUN_CLIENT == XA_ENABLED
            , ((SPP_DEV_OS*) dev)->profile
    #endif             
            );
    /* #ifdef CUSTOM_SPP */
        parms.event = SPP_EVENT_DEACTIVATE_CNF;
        parms.dstMod = MOD_MMI;
        SPPCB(&parms);
    /* #else #ifdef CUSTOM_SPP */               

        /* When SPP device is deactivated, call the closeDevice() function */
        bt_sppclient_cmgr_link_close(sppclient);
        return;
    }
    else if (dev->type.client.flag & RECV_SPPC_DEACTIVATE_REQ)
    {
    /* #ifdef CUSTOM_SPP */
        SPPCallbackParms parms;               
    /* #endif #ifdef CUSTOM_SPP */                                                                 
        
        dev->type.client.flag |= RECV_SPPC_SERVICE_DISC_CNF;

        //[question] If this state is ACTIVATED or DEACTIVATED ??
        //dev->type.client.state = 0;
        //Report(("Recv CLOSED event due to DEACTIVATE_REQ"));
        kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_CLOSED_DUE_TO_DEACTIVATE_REQ);
        /* kal_trace(TRACE_GROUP_1,BT_SPP_RECV_CLOSED_EVENT_FOR_SPPCLIENT); */

        /* Send SPP_Deactivate_CNF back to MMI */
        dev->type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;
    /* #ifdef CUSTOM_SPP */
        parms.event = SPP_EVENT_DEACTIVATE_CNF;
        parms.dstMod = MOD_MMI;
        SPPCB(&parms);
    /* #else #ifdef CUSTOM_SPP */               

        /* When SPP device is deactivated, call the closeDevice() function */
        bt_sppclient_cmgr_link_close(sppclient);

        return;
    }
    else if (dev->type.client.flag & RECV_SPPC_DISCONNECT_REQ)
    {

        dev->type.client.flag |= RECV_SPPC_SERVICE_DISC_CNF;

        /* Report(("Recv CLOSED event due to DISCONNECT_REQ")); */
        kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_CLOSED_DUE_TO_DISC_REQ);
        /* Send SPP_DISCONNECT_CNF back to MMI */

        dev->type.client.flag |= SEND_SPPC_DISCONNECT_CNF;
        sppa_send_disconnect_cnf_result(
            ((SPP_DEV_OS*) dev)->port,
            dev->con_id,
            spp_btaddr_t.lap,
            spp_btaddr_t.uap,
            spp_btaddr_t.nap,
            SPP_DISCONNECT_SUCCESS
    #if DUN_CLIENT == XA_ENABLED
            , ((SPP_DEV_OS*) dev)->profile
    #endif             
            );
        bt_sppclient_cmgr_link_close(sppclient);
        return;
    }

    if (result == BT_STATUS_SUCCESS)
    {
        U8 spp_serverchannel_num = 0;
        BtStatus status = 0;

        switch (spp_attribute_id_list[attribute_index])
        {
            case AID_PROTOCOL_DESC_LIST:
                // found SPP Server record
                // the attribute value only cared is SPP server channel number (1 byte)
                // But it is possible that the peers supports more than one SPP servers, so rsp contains more than one server channel number

                spp_serverchannel_num = attribute_value[0];
                /* Report(("In SPPQueryCallback(), get server_channel_num=%d",spp_serverchannel_num)); */
                kal_trace(TRACE_GROUP_1, BT_SPPC_GET_SRV_CHNL, spp_serverchannel_num);
                /* Using the srvchnl_num to connect to the SPP Server through the function sppConnect() */
                status = sppConnect(&(sppclient->sppDev), sqt->rm, spp_serverchannel_num);

                if (status == BT_STATUS_SUCCESS)
                {
                    /* SPP Client sends RFCOMM channel connection request to peer side */
                    sppclient->sppDev.type.client.flag |= SEND_SPPC_SERVICE_REQ;
                }
                else
                {
                    sppclient->sppDev.state = DEVICE_STATE_CLOSED;

                    /* Report(("In SPPQueryCallback(), call sppConnect() failed. status=%d",status)); */
                    kal_trace(TRACE_GROUP_1, BT_SPPC_SPPCONNECT_FAILED_03, status);
                    //Failure of open rfcomm client channel:
                    //If this spp client received SPP_CONNECT_REQ previously, it is time to send 
                    if (sppclient->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
                    {
                        sppa_send_connect_cnf_failed(
                            spp_btaddr_t.lap,
                            spp_btaddr_t.uap,
                            spp_btaddr_t.nap,
                            SPPC_SERVICE_LEVEL_CON_FAILED
                        #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                            , sppclient->profile
                        #endif 
                            );
                        sppclient->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;

                        /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                        bt_sppclient_cmgr_link_close(sppclient);

                    }
                }
                break;
            default:
                Report(("The attribute ID of this rsp value=%04x", spp_attribute_id_list[attribute_index]));
                Assert(0);
                break;
        }

    }
    else
    {
        /* If result is not SUCCESS, and this SPP client received SPP_CONNECT_REQ previously, it is time to send CONNEC_CNF */
        if (sppclient->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
        {
            sppa_send_connect_cnf_failed(
                spp_btaddr_t.lap,
                spp_btaddr_t.uap,
                spp_btaddr_t.nap,
                SPPC_SDP_QUERY_RSP_FAILED
            #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                , sppclient->profile
            #endif 
                );
            sppclient->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;

        }

        //If result is not SUCCESS, may be due to the remote device does not have this service!
        //Error: Should close the ACL link
        //Assert(0);

        /* Bingyi: We should close the ACL link and clear the Client related context. */
        bt_sppclient_cmgr_link_close(sppclient);
    }

}


/*****************************************************************************
 * FUNCTION
 *  queryCallback
 * DESCRIPTION
 *  Callback function for queryForService.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void queryCallback(const BtEvent *event)
{
    BtStatus status;
    SdpQueryToken *token;
    SppDev *dev;
    SppClient *client;

    /*
     * The token is a member of a structure within the SppDev structure.
     * SppDev->type.client.sdpToken (type is a union)
     * thus we can find the address of SppDev.
     */
    token = event->p.token;
    client = ContainingRecord(event->p.token, SppClient, sdpToken);
    dev = ContainingRecord(client, SppDev, type);
    /* Assert((client->state & SPP_CLIENT_WAIT_SDP_QUERY_RSP)); */

    switch (event->eType)
    {

        case SDEVENT_QUERY_RSP:
            status = SDP_ParseAttributes(token);
            if (status == BT_STATUS_SUCCESS)
            {
                /* Value returned should be U8 for channel. */
                Assert(token->totalValueLen == 1);
                Report(("In queryCallback(): get spp serverid=%d", token->valueBuff[0]));
                status = sppConnect(dev, token->rm, token->valueBuff[0]);
                kal_trace(TRACE_GROUP_1, SPP_SDP_CONNECT_RETURNED_xD, status);
            }
            else if (status == BT_STATUS_SDP_CONT_STATE)
            {
                /* We need to continue the query. */
                token->mode = BSPM_CONT_STATE;
                status = SDP_Query(token, BSQM_CONTINUE);
                kal_trace(TRACE_GROUP_1, SPP_SDP_QUERY_RETURNED_xD, status);
            }
            break;

        case SDEVENT_QUERY_ERR:
        case SDEVENT_QUERY_FAILED:
            kal_trace(TRACE_GROUP_1, SPP_SDP_QUERY_FAILED);
            /* Do we not indicate this back somehow? */
            break;
    }
}
#endif /* (SPP_CLIENT_CONNECT_DO_SDP == XA_ENABLED) */

#endif /* SPP_CLIENT == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  rfCallbackFunc
 * DESCRIPTION
 *  RFCOMM callback function. This routine is used for all serial
 *  ports' callback so we need to ensure that it is re-entrant.
 * PARAMETERS
 *  chan        [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void rfCallbackFunc(RfChannel *chan, RfCallbackParms *parms)
{
    SppDev *dev;

    /*
     * The RfChannel * that we passed to RFCOMM was a pointer to the 
     * channel in our device structure. Now we need to get the address
     * of that structure so we know to whom this even belongs.
     */
    dev = ContainingRecord(chan, SppDev, channel);

    SPPOS_LockDev(dev->osDev);

    if ((*deviceState[dev->state]) (dev, parms) == FALSE)
    {
        /* Event was not handled by state machine */
        /* for DEVICE_STATE_DISCONNECTED, DEVICE_STATE_CONNECTED */
        defaultEventHandler(dev, parms);
    }

    SPPOS_UnlockDev(dev->osDev);
}


/*****************************************************************************
 * FUNCTION
 *  defaultEventHandler
 * DESCRIPTION
 *  This is a default handler for RFCOMM events. Each state gets a
 *  chance to intercept events. If it doesn't care about them or likes
 *  the default, it can be handled here.
 * PARAMETERS
 *  dev         [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void defaultEventHandler(SppDev *dev, RfCallbackParms *parms)
{
    BtStatus status;
    U8 connection_id;
    U32 lap = 0;
    U8 uap = 0;
    U16 nap = 0;

    switch (parms->event)
    {

        case RFEVENT_PACKET_HANDLED:
            /* 
             *  An acknowledgment for RF_SendData() is received. Remove those
             *  bytes from our buffer and send any more data we have pending.
             */
    #ifdef __SPP_AUTO_TX__
            if (sppAutoTxFlag == FALSE)
    #endif 

#ifdef __SPP_THROUGHPUT_TEST__
#else
                SPPOS_ReturnBuf(dev->osDev, (char*)parms->ptrs.packet->data, parms->ptrs.packet->dataLen);
#endif

            InsertHeadList(&dev->txPacketList, &parms->ptrs.packet->node);
            /* Report(("In defaultEventHandler():RFEVENT_PACKET_HANDLED, sentdata=%d on DLCI=%d ",parms->ptrs.packet->dataLen)); */
            kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPP_PKT_RETURNED, parms->ptrs.packet->dataLen, dev->channel.dlci);
#ifdef __SPP_THROUGHPUT_TEST__
	     if (g_sppThroughputTotalSentLength < SPP_THROUGHPUT_TEST_DATA_LENGTH)
	     {
	        //printf("g_sppThroughputTotalSentLength < SPP_THROUGHPUT_TEST_DATA_LENGTH\n");
	     	if (g_sppThroughputPeriodicSentLength >  SPP_THROUGHPUT_TEST_DATA_PERIODIC_LENGTH)
	     	{
	     		time_t timePeriodicSentStop;
			U32 u4PeriodicWriteTime = 0;
			float fThroughputPeriodic = 0;

                        //printf("g_sppThroughputPeriodicSentLength >  SPP_THROUGHPUT_TEST_DATA_PERIODIC_LENGTH\n");
				
	     		// Calculate the throughput of past (SPP_THROUGHPUT_TEST_DATA_PERIODIC_LENGTH) MBytes
	     		time(&timePeriodicSentStop);
			u4PeriodicWriteTime = (timePeriodicSentStop - g_sppThroughputTestPeriodicTimeSendStop);
			fThroughputPeriodic = (g_sppThroughputPeriodicSentLength / u4PeriodicWriteTime / 1024);
			printf("[SPP][Throughput_Send] Periodic sent data=%d bytes; Time=%d seconds; Throughput=%f KBytes/Sec\n", g_sppThroughputPeriodicSentLength, u4PeriodicWriteTime, fThroughputPeriodic);
			
	     		g_sppThroughputPeriodicSentLength = 0;
			g_sppThroughputTestPeriodicTimeSendStop = timePeriodicSentStop;
	     	}
            sendToRfComm(dev);
	     }
	     else
	     {
		U32 u4TotalWriteTime = 0;
		float fThroughput = 0;

		// Deactivate this port
		SPPHandleDeactivateReq(g_sppThroughputTestClientPort);
		
		time(&g_sppThroughputTestTimeSendStop);
		u4TotalWriteTime = (g_sppThroughputTestTimeSendStop - g_sppThroughputTestTimeSendStart);
		fThroughput = (g_sppThroughputTotalSentLength / u4TotalWriteTime / 1024);
		printf("[SPP][Throughput_Send] Testing done. Total sent data=%d bytes; Total time=%d seconds; Throughput=%f KBytes/Sec\n", g_sppThroughputTotalSentLength, u4TotalWriteTime, fThroughput);
	     }
#else
            sendToRfComm(dev);
#endif
            break;

        case RFEVENT_CLOSE_IND:
            kal_trace(TRACE_GROUP_1, SPP_RFEVENT_CLOSE_IND);
            break;

        case RFEVENT_CLOSED:
        {
            /*
             * Channel was closed. The following code usually pertains only to
             * clients closing: either remote or local. If we're a server
             * closing, the event is handled by the state machine; however, 
             * servers can get this event here instead of in the state machine
             * when the link is forced down.
             */
#ifdef __SPP_THROUGHPUT_TEST__
		if (dev->portType == SPP_SERVER_PORT)
		{
			U32 u4TotalGetTime = 0;
			float fThroughput = 0;
			
			time(&g_sppThroughputTestTimeGetStop);
			u4TotalGetTime = (g_sppThroughputTestTimeGetStop - g_sppThroughputTestTimeGetStart);
			fThroughput = (g_sppThroughputTotalGotLength / u4TotalGetTime / 1024);
			printf("[SPP][Throughput_GET] Testing done. Total got data=%d bytes; Total time=%d seconds; Throughput=%f KBytes/Sec\n", g_sppThroughputTotalGotLength, u4TotalGetTime, fThroughput);
		}
#endif
        #if SPP_CLIENT == XA_ENABLED
            kal_uint32 oldclientflag = 0;
        #endif 
            kal_trace(TRACE_GROUP_1, SPP_RFEVENT_CLOSED);

        #ifdef __SPP_GET_REMOTE_MODEM_STATUS__
            dev->rModemStatus.signals &= ~(RF_CD | RF_DSR | RF_CTS);
        #endif 

            /* reset dev context */
            initDevice(dev);
            /* reset vp context */
            initVp(dev->osDev);
            if(dev->con_id_flag == KAL_TRUE)
            {
            BTBMProfileReleaseConnIndex(dev->con_id);
            }

        #if (SPP_SCO_SUPPORT == XA_ENABLED)
            if (dev->rssi_timer.func != 0)
            {
                /* Report(("RSSI timer still running, cancel it due to RFEVENT_CLOSED")); */
                kal_trace(TRACE_GROUP_1, BT_SPP_STOP_RSSI_TIMER);
                dev->rssi_timer.func = 0;
                EVM_CancelTimer(&dev->rssi_timer);
            }
        #endif /* (SPP_SCO_SUPPORT == XA_ENABLED) */

            lap = 0;
            btmtk_os_memcpy((kal_uint8*) &lap, &parms->ptrs.remDev->bdAddr.addr[0], 3);
            uap = 0;
            uap = parms->ptrs.remDev->bdAddr.addr[3];
            nap = 0;
            btmtk_os_memcpy((kal_uint8*) &nap, & parms->ptrs.remDev->bdAddr.addr[4], 2);

            if (dev->portType == SPP_SERVER_PORT)
            {
                /* Bingyi 20091223: Call deRegisterService() to free the RFCOMM channel after RFCOMM has been closed. */
                if (DEVICE_STATE_DEACTIVATING == dev->state)
                {
                	SPP_DEV_OS *vp_ptr = 0;
                	U8 uCount = 0;

                	for (uCount = 0; uCount < NUM_SPP_DEVS; uCount++)
                	{
                		vp_ptr = getDev((UART_PORT) (BT_START_OF_VIRTUAL_PORT + uCount));
                		if ((vp_ptr->sppDev.portType & SPP_SERVER_PORT) && (dev->type.sppService->service.serviceId == vp_ptr->sppDev.type.sppService->service.serviceId))
                		{
                			Assert(vp_ptr->port == (BT_START_OF_VIRTUAL_PORT + uCount));

                			if (SPP_PROFILE == vp_ptr->profile)
                			{
                				SPPHandleDeactivateReq(vp_ptr->port);
                			}
#if (DUN_SERVER == XA_ENABLED)
                			else if (DUN_PROFILE == vp_ptr->profile)
                			{
                				DUNHandleDeactivateReq(vp_ptr->port);
                			}
#endif /* #if DUN_SERVER == XA_ENABLED */
#if FAX_SERVER == XA_ENABLED
                			else if (FAX_PROFILE == vp_ptr->profile)
                			{
                				FAXHandleDeactivateReq(vp_ptr->port);
                			}
#endif /* #if FAX_SERVER == XA_ENABLED */
                			else
                			{
                				Assert(0);
                			}

                			break;
                		}
                	}
                }

                if (dev->state != DEVICE_STATE_CLOSED)
                {
                    dev->state = DEVICE_STATE_DISCONNECTED;
                }
            }
            else
            {
                //This is SPPClient's case: if it receives RF_CLOSED event, it should not receive any events from peer's side later
                //Once this SPPClient wants to connect peer's side, it invokes openClientPort() then sppConnect(),
                //at that time, SPPClient's dev->state is DEVICE_STATE_DISCONNECTED:
                //so this SPPClient can recevie any events from peer's side whe callback from rfCallbacFunc()
                if (dev->state != DEVICE_STATE_CLOSED)
                {
                    dev->state = DEVICE_STATE_CLOSED;
                }
            }

            /* Check if this is SPP client's CLOSED Event */
            if (dev->portType == SPP_CLIENT_PORT)
            {

            #if SPP_CLIENT == XA_ENABLED

            #if (SPP_SCO_CLIENT == XA_ENABLED)
                if (dev->scoType == SPP_WITH_SCO)
                {
                    CMGR_DeregisterScoHandler(&dev->type.client.cmgr_hdlr);
                }
            #endif /* (SPP_SCO_CLIENT == XA_ENABLED) */
                if ((dev->type.client.flag & SEND_SPPC_SERVICE_DISC_REQ) &&
                    (dev->type.client.flag & RECV_SPPC_DEACTIVATE_REQ) &&
                    (dev->type.client.flag & RECV_SPPC_DISCONNECT_REQ))
                {
                 /* #ifdef CUSTOM_SPP */
                    SPPCallbackParms parms;               
                /* #endif #ifdef CUSTOM_SPP */

                    dev->type.client.flag |= RECV_SPPC_SERVICE_DISC_CNF;

                    /* Check if BT needs send UART_PLUGOUT_IND to MMI or not */
                    if (dev->type.client.flag & SEND_SPPC_CONNECT_CNF)
                    {
                    #ifdef __GEMINI__
                        if (((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI &&
                            ((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI_2)
                    #else /* __GEMINI__ */ 
                        if (((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI)
                    #endif /* __GEMINI__ */ 
                        {
                            kal_bool Enable_SPP_FlowControl = KAL_FALSE;
                            /* Only task priority is lower than MOD_BT, it is necessary to apply this flow control mechanism */
                            Enable_SPP_FlowControl = CheckSPPFlowControlSetting(((SPP_DEV_OS*) dev)->owner);
                            if (Enable_SPP_FlowControl)
                            {
                                kal_trace(TRACE_GROUP_1,BT_SPP_SET_WAIT_PLUGOUT_CNF_FLAG_INX,1);
                                ((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg = KAL_TRUE;
                            }
                        }
                    /* #ifdef CUSTOM_SPP */
                        parms.event = UART_EVENT_PLUGOUT_IND;
                        parms.dstMod = ((SPP_DEV_OS*) dev->osDev)->owner;
                        parms.p.uart_plugout_ind.port = ((SPP_DEV_OS*) dev->osDev)->port;
                        parms.p.uart_plugout_ind.profile = ((SPP_DEV_OS*) dev->osDev)->profile;
                        SPPCB(&parms);
                    /* #else #ifdef CUSTOM_SPP */                                   

                    }

                    dev->type.client.flag |= SEND_SPPC_DISCONNECT_CNF;

                    /* Send DISC_CNF back to MMI here !!!!! */

                    sppa_send_disconnect_cnf_result(
                        ((SPP_DEV_OS*) dev)->port,
                        dev->con_id,
                        lap,
                        uap,
                        nap,
                        SPP_DISCONNECT_SUCCESS
                #if DUN_CLIENT == XA_ENABLED
                        , ((SPP_DEV_OS*) dev)->profile
                #endif             
                        );

                    dev->type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;

                /* #ifdef CUSTOM_SPP */
                    parms.event = SPP_EVENT_DEACTIVATE_CNF;
                    parms.dstMod = MOD_MMI;
                    SPPCB(&parms);
                /* #else #ifdef CUSTOM_SPP */               

                    /* When SPP device is deactivated, call the closeDevice() function */

                }
                else if ((dev->type.client.flag & SEND_SPPC_SERVICE_DISC_REQ) &&
                         (dev->type.client.flag & RECV_SPPC_DEACTIVATE_REQ))
                {
                /* #ifdef CUSTOM_SPP */
                    SPPCallbackParms parms;                    
                /* #endif #ifdef CUSTOM_SPP */

                    dev->type.client.flag |= RECV_SPPC_SERVICE_DISC_CNF;

                    //[question] If this state is ACTIVATED or DEACTIVATED ??
                    //dev->type.client.state = 0;
                    //Report(("In defaultEventHandler():Recv CLOSED event due to DEACTIVATE_REQ"));
                    kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_CLOSED_DUE_TO_DEACTIVATE_REQ_02);

                    //Send SPP_DISCONNECT_CNF back to MMI [question]??? 
                    //SPPOS_sendMsg(msg_type msg, module_type dstMod, sap_type sap, local_para_struct * local_para, peer_buff_struct * peer_buff)
                    dev->type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;

                    /* Check if BT needs send UART_PLUGOUT_IND to MMI or not */
                    if (dev->type.client.flag & SEND_SPPC_CONNECT_CNF)
                    {
                    #ifdef __GEMINI__
                        if (((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI &&
                            ((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI_2)
                    #else /* __GEMINI__ */ 
                        if (((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI)
                    #endif /* __GEMINI__ */ 
                        {
                            kal_bool Enable_SPP_FlowControl = KAL_FALSE;
                            /* Only task priority is lower than MOD_BT, it is necessary to apply this flow control mechanism */
                            Enable_SPP_FlowControl = CheckSPPFlowControlSetting(((SPP_DEV_OS*) dev)->owner);
                            if (Enable_SPP_FlowControl)
                            {
                                kal_trace(TRACE_GROUP_1,BT_SPP_SET_WAIT_PLUGOUT_CNF_FLAG_INX,2);
                                ((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg = KAL_TRUE;
                            }

                        }
                    /* #ifdef CUSTOM_SPP */
                        parms.event = UART_EVENT_PLUGOUT_IND;
                        parms.dstMod = ((SPP_DEV_OS*) dev->osDev)->owner;
                        parms.p.uart_plugout_ind.port = ((SPP_DEV_OS*) dev->osDev)->port;
                        parms.p.uart_plugout_ind.profile = ((SPP_DEV_OS*) dev->osDev)->profile;
                        SPPCB(&parms);
                    /* #else #ifdef CUSTOM_SPP */                                   
                    }
                /* #ifdef CUSTOM_SPP */
                    parms.event = SPP_EVENT_DEACTIVATE_CNF;
                    parms.dstMod = MOD_MMI;
                    SPPCB(&parms);
                /* #else #ifdef CUSTOM_SPP */

                    /* When SPP device is deactivated, call the closeDevice() function */
                }
                else if ((dev->type.client.flag & SEND_SPPC_SERVICE_DISC_REQ) &&
                         (dev->type.client.flag & RECV_SPPC_DISCONNECT_REQ))
                {
                /* #ifdef CUSTOM_SPP */
                    SPPCallbackParms parms;                    
                /* #endif #ifdef CUSTOM_SPP */

                    dev->type.client.flag |= RECV_SPPC_SERVICE_DISC_CNF;

                    /* Report(("In defaultEventHandler():Recv CLOSED event due to DISCONNECT_REQ")); */
                    kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_CLOSED_DUE_TO_DISC_REQ_02);

                    dev->type.client.flag |= SEND_SPPC_DISCONNECT_CNF;

                    /* Check if BT needs send UART_PLUGOUT_IND to MMI or not */
                    if (dev->type.client.flag & SEND_SPPC_CONNECT_CNF)
                    {
                    #ifdef __GEMINI__
                        if (((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI &&
                            ((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI_2)
                    #else /* __GEMINI__ */ 
                        if (((SPP_DEV_OS*) dev->osDev)->owner != MOD_ATCI)
                    #endif /* __GEMINI__ */ 
                        {
                            kal_bool Enable_SPP_FlowControl = KAL_FALSE;
                            /* Only task priority is lower than MOD_BT, it is necessary to apply this flow control mechanism */
                            Enable_SPP_FlowControl = CheckSPPFlowControlSetting(((SPP_DEV_OS*) dev)->owner);
                            if (Enable_SPP_FlowControl)
                            {
                                kal_trace(TRACE_GROUP_1,BT_SPP_SET_WAIT_PLUGOUT_CNF_FLAG_INX,3);
                                ((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg = KAL_TRUE;
                            }
                        }
                    /* #ifdef CUSTOM_SPP */
                        parms.event = UART_EVENT_PLUGOUT_IND;
                        parms.dstMod = ((SPP_DEV_OS*) dev->osDev)->owner;
                        parms.p.uart_plugout_ind.port = ((SPP_DEV_OS*) dev->osDev)->port;
                        parms.p.uart_plugout_ind.profile = ((SPP_DEV_OS*) dev->osDev)->profile;
                        SPPCB(&parms);                        
                    /* #else #ifdef CUSTOM_SPP */                                                       
                    }

                    /* Send SPP_DISCONNECT_CNF back to MMI */
                    sppa_send_disconnect_cnf_result(
                        ((SPP_DEV_OS*) dev)->port,
                        dev->con_id,
                        lap,
                        uap,
                        nap,
                        SPP_DISCONNECT_SUCCESS
                #if DUN_CLIENT == XA_ENABLED
                        , ((SPP_DEV_OS*) dev)->profile
                #endif             
                        );

                }
                else if ((dev->type.client.flag & RECV_SPPC_SERVICE_RSP) != RECV_SPPC_SERVICE_RSP)
                {
                    //This case means that: During SPP Client tries to connect with a SPP Server, something error causes L2CAP chnl to close
                    //So RFCOMM layer receives CLOSED event due to L2CAP chnl is closed.
                    //E.g., SPP Client is trying to connect with Server, but negotiation of DLCI failed:
                    //So our L2CAP sends disc req to peer device, and receives L2CAP disc cnf with ok,
                    //Then, L2CAP DISCONNECTED Event is reported to RFCOMM layer, so CLOSED event is received.

                    /* Send CONNECT_CNF with failed to MMI */
                #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                    sppa_send_connect_cnf_failed(lap, uap, nap, SPPC_NOT_CONNECTED_BUT_RECV_CLOSED, ((SPP_DEV_OS*)dev)->profile);
                #else  
                    sppa_send_connect_cnf_failed(lap, uap, nap, SPPC_NOT_CONNECTED_BUT_RECV_CLOSED);
                #endif 
                }
                else
                {
                    //This case: SPP Client is connected with a SPP Server, but receives CLOSED event not issed by client now!
                    //Report(("SPPClient is already connected; Now: Recv CLOSED event not issued by itself"));
                    kal_trace(TRACE_GROUP_1, BT_SPPC_CONNECTED_RECV_CLOSE_NOT_IT_ISSUED);

                }

                /* Before cleaning the client.flag, assign its value to the variable "oldclientflag" */
                oldclientflag = dev->type.client.flag;
                /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                bt_sppclient_cmgr_link_close((SPP_DEV_OS*) dev->osDev);
            #endif /* SPP_CLIENT == XA_ENABLED */

            }
            else if (dev->portType == SPP_SERVER_PORT)
            {
            #if (SPP_SCO_SERVER == XA_ENABLED)
                if (dev->scoType == 1)
                {
                    CMGR_RemoveDataLink(&(dev->type.sppService->cmgr_hdlr));
                    CMGR_RemoveCmgrWithDevice(&(dev->type.sppService->cmgr_hdlr));
                }
            #endif /* (SPP_SCO_SERVER == XA_ENABLED) */
            }

        #if (SPP_CLIENT == XA_ENABLED) && (SPP_SERVER == XA_ENABLED)
            if ((dev->portType == SPP_SERVER_PORT) ||
                ((dev->portType == SPP_CLIENT_PORT) && (oldclientflag & RECV_SPPC_SERVICE_RSP) &&
                 ((oldclientflag & SEND_SPPC_SERVICE_DISC_REQ) != SEND_SPPC_SERVICE_DISC_REQ)))

        #elif (SPP_CLIENT == XA_ENABLED)
            if ((dev->portType == SPP_CLIENT_PORT) && (oldclientflag & RECV_SPPC_SERVICE_RSP) &&
                ((oldclientflag & SEND_SPPC_SERVICE_DISC_REQ) != SEND_SPPC_SERVICE_DISC_REQ))

        #elif (SPP_SERVER == XA_ENABLED)
            if (dev->portType == SPP_SERVER_PORT)
        #else 
            /* Nothing */
        #endif 
            {
                SPPOS_DisConnectInd(dev->osDev);
            }

        }   /* end case RFEVENT_CLOSED */
            break;

        case RFEVENT_DATA_IND:
        {
            U16 bytes = parms->dataLen;

            /* Data was received on the channel. */
            dev->credit--;
#if 0
            /* Parsing data: Check if this dev is SPP Server or not */
            if ((dev->portType == SPP_SERVER_PORT) &&
                (((SPP_DEV_OS*) (dev->osDev))->wait_first_received_data_in_openstate == KAL_TRUE))
            {
                kal_uint8 pattern_len = 0;
                kal_uint8 *data_pattern = BtGet_SPPClientFirstConnDataPattern(&pattern_len);

                //Report(("Recv SPP Client first data=%s",parms->ptrs.data));
                //kal_trace(TRACE_GROUP_1,BT_SPPS_RECV_FIRST_DATA,parms->ptrs.data);
                //No matter that this first data is specific pattern or not, set wait_first_received_data_in_openstate to FALSE
                ((SPP_DEV_OS*) (dev->osDev))->wait_first_received_data_in_openstate = KAL_FALSE;

                if (btmtk_os_memcmp(parms->ptrs.data, pattern_len, data_pattern, pattern_len) == 0)
                    /* if(strncmp((char*)(parms->ptrs.data),str,pattern_len) == 0) */
                {
                /* #ifdef CUSTOM_SPP */
                    SPPCallbackParms parms;
                    parms.p.uart_plugout_ind.port = ((SPP_DEV_OS*) dev)->port;                
                /* #else #ifdef CUSTOM_SPP */                    
                    /* At this time, the port owner should be MOD_ATCI or MOD_ATCI2, it is not necessary to set wait_plugout_cnf_msg flag */
                #ifndef __GEMINI__
                    if (((SPP_DEV_OS*) dev)->owner != MOD_ATCI)
                #else 
                    if ((((SPP_DEV_OS*) dev)->owner != MOD_ATCI) && (((SPP_DEV_OS*) dev)->owner != MOD_ATCI_2))
                #endif
                    {
                        kal_bool Enable_SPP_FlowControl = KAL_FALSE;
                        /* Only task priority is lower than MOD_BT, it is necessary to apply this flow control mechanism */
                        Enable_SPP_FlowControl = CheckSPPFlowControlSetting(((SPP_DEV_OS*) dev)->owner);
                        if (Enable_SPP_FlowControl)
                        {
                            kal_trace(TRACE_GROUP_1,BT_SPP_SET_WAIT_PLUGOUT_CNF_FLAG_INX,4);
                            ((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg = KAL_TRUE;
                        }
                    }
                /* #ifdef CUSTOM_SPP */
                    parms.event = UART_EVENT_PLUGOUT_IND;
                    parms.dstMod = ((SPP_DEV_OS*) dev)->owner;
                    parms.p.uart_plugout_ind.profile = ((SPP_DEV_OS*) dev)->profile;
                    SPPCB(&parms);         
                /* #else #ifdef CUSTOM_SPP */

                    //Before receiving MSG_ID_BT_SPP_UART_OWNER_CNF, BT can not send MSG_ID_UART_READY_TO_READ to MMI
                    //Otherwise, MMI can not receive data successfully due to it is not yet the owner of this port.
                    //This is because MMI has lower priority than BT, if it not yet receives (port owner not changed):
                    //BT will send indication to old owner of this port to notify it to receive data instead of MMI!
                 #ifdef __BT_SPP_GENERAL__ 
                    ((SPP_DEV_OS*) (dev->osDev))->set_auth_req_mode = FALSE;
                 #else /* #ifdef __BT_SPP_GENERAL__ */
                    ((SPP_DEV_OS*) (dev->osDev))->set_auth_req_mode = TRUE;
                    parms.event = SPP_EVENT_UART_OWNER_IND;
                    parms.dstMod = MOD_MMI;
                    parms.p.spp_uart_owner_ind.port = ((SPP_DEV_OS*) dev)->port;
                    /* Notify the MMI is the new owner of this port: Must be added later! */
                    SPPCB(&parms);                    
                 #endif /* #ifdef __BT_SPP_GENERAL__ */
                    /* UART_Open(((SPP_DEV_OS *)dev)->port, MOD_MMI); */
                }

            }
#endif            
        #if (DUMP_DATA_FROM_SPP_CLI == 1)
            //else if (dev->portType == SPP_SERVER_PORT)
            if (dev->portType == SPP_SERVER_PORT)
            {
                /* Recving the data (not first data in openstate) from SPP Client */
                Report(("Recv SPP Client data len=%d,data=%s", bytes, parms->ptrs.data));
            }
        #endif /* (DUMP_DATA_FROM_SPP_CLI == 1) */
        #if (DUMP_DATA_FROM_SPP_SRV == 1)
            else if (dev->portType == SPP_CLIENT_PORT)
            {
                /* Recving the data from SPP Server */
                Report(("Recv SPP Server data len=%d,data=%s", bytes, parms->ptrs.data));
            }
        #endif /* (DUMP_DATA_FROM_SPP_SRV == 1) */

            if (((SPP_DEV_OS*) (dev->osDev))->set_auth_req_mode == KAL_TRUE)
            {
                kal_trace(TRACE_GROUP_1, BT_SPPC_NO_UART_OWNER_CNF_NO_READY_TO_READ);
            }

            SPPOS_DevRx(dev->osDev, (char*)parms->ptrs.data, &bytes);
            kal_trace(TRACE_GROUP_1, RX_xD_SAVED_xD, parms->dataLen, bytes);

            advanceCredit(dev);

            if (bytes < parms->dataLen)
            {
                /* Our buffer was full so log an overrun */
            #ifndef __SPP_LINE_STATUS__
                Assert(0);  /* overrun */
            #else /* __SPP_LINE_STATUS__ */ /* If overrun happens, it is not possible to execute the following codes, so disable it */
                dev->lineStatus |= RF_OVERRUN;
            #if RF_SEND_CONTROL == XA_ENABLED
                status = RF_SetLineStatus(&dev->channel, dev->lineStatus);
                Assert((status == BT_STATUS_PENDING) || (status == XA_STATUS_IN_PROGRESS));
            #endif /* RF_SEND_CONTROL == XA_ENABLED */
            #endif /* __SPP_LINE_STATUS__ */ 
            }
        }
            break;

        case RFEVENT_OPEN_IND:
        {
#ifdef __SPP_THROUGHPUT_TEST__
	     SPP_DEV_OS* spp_dev_os_ptr = ((SPP_DEV_OS*) (dev->osDev));
	     RF_AcceptChannel(&(spp_dev_os_ptr->sppDev.channel));
	     return ;
#else
        #if BT_SPP_AUTHORIZE_BY_MMI
            /* After BT v2.1: new sap for MMI AUTHORIZE is applied */
            btbm_bd_addr_t con_ind_addr;
            SPP_DEV_OS* spp_dev_os_ptr = ((SPP_DEV_OS*) (dev->osDev));
            btmtk_os_memset((kal_uint8*) & con_ind_addr, 0, sizeof(btbm_bd_addr_t));
            CONVERT_ARRAY2BDADDR((btbm_bd_addr_t*) & con_ind_addr, parms->ptrs.remDev->bdAddr.addr);
            
            kal_trace(TRACE_GROUP_1,BT_SPP_RECV_OPEN_IND_SET_AUTH_MODE_WAIT_PLUGOUT_CNF_FLAG,spp_dev_os_ptr->set_auth_req_mode,spp_dev_os_ptr->wait_plugout_cnf_msg);
            if (spp_dev_os_ptr->wait_plugout_cnf_msg == KAL_TRUE)
            {
                /* It means that the previous's UART_PLUGOU_CNF is not responded from MOD_XXX (ie., non-MOD_ATCI,e.g., MOD_MMI or MOD_DT) */
                kal_trace(TRACE_GROUP_1,BT_SPP_RECV_OPEN_IND_BUT_REJECT_DUE_TO_WAIT_PLUGOUT_CNF);
                RF_RejectChannel(&(spp_dev_os_ptr->sppDev.channel));
                return;
            }
            
            ((SPP_DEV_OS*) (dev->osDev))->set_auth_req_mode = KAL_TRUE;

            /* Add profile_uuid field of struct bt_spp_auth_req_struct for BT2.1 Profile authroization request : 2008-0330 */
            if (((SPP_DEV_OS*) (dev->osDev))->profile == SPP_PROFILE)
            {
/* #ifdef CUSTOM_SPP */
                SPPCallbackParms parms;
                parms.event = SPP_EVENT_CONNECT_IND_REQ;
                parms.dstMod = MOD_MMI;
                parms.p.spp_connect_ind_req.lap = 0;
                parms.p.spp_connect_ind_req.lap = con_ind_addr.lap;
                parms.p.spp_connect_ind_req.uap = 0;
                parms.p.spp_connect_ind_req.uap = con_ind_addr.uap;
                parms.p.spp_connect_ind_req.nap = 0;
                parms.p.spp_connect_ind_req.nap = con_ind_addr.nap;
                parms.p.spp_connect_ind_req.port = ((SPP_DEV_OS*) (dev->osDev))->port;
                SPPCB(&parms);
/* #else   #ifdef CUSTOM_SPP */
            }
            else if (((SPP_DEV_OS*) (dev->osDev))->profile == DUN_PROFILE)
            {

            /* #ifdef CUSTOM_SPP */
                SPPCallbackParms parms;   
                parms.event = DUN_EVENT_CONNECT_IND_REQ;
                parms.dstMod = MOD_MMI;
                parms.p.dun_connect_ind_req.lap = 0;
                parms.p.dun_connect_ind_req.lap = con_ind_addr.lap;
                parms.p.dun_connect_ind_req.uap = 0;
                parms.p.dun_connect_ind_req.uap = con_ind_addr.uap;
                parms.p.dun_connect_ind_req.nap = 0;
                parms.p.dun_connect_ind_req.nap = con_ind_addr.nap;
                parms.p.dun_connect_ind_req.port = ((SPP_DEV_OS*) (dev->osDev))->port;
                SPPCB(&parms);
            /* #else #ifdef CUSTOM_SPP */
            }
            else if (((SPP_DEV_OS*) (dev->osDev))->profile == FAX_PROFILE)
            {
                SPPCallbackParms parms;   
                parms.event = FAX_EVENT_CONNECT_IND_REQ;
                parms.dstMod = MOD_MMI;
                parms.p.fax_connect_ind_req.lap = 0;
                parms.p.fax_connect_ind_req.lap = con_ind_addr.lap;
                parms.p.fax_connect_ind_req.uap = 0;
                parms.p.fax_connect_ind_req.uap = con_ind_addr.uap;
                parms.p.fax_connect_ind_req.nap = 0;
                parms.p.fax_connect_ind_req.nap = con_ind_addr.nap;
                parms.p.fax_connect_ind_req.port = ((SPP_DEV_OS*) (dev->osDev))->port;
                SPPCB(&parms);
            }

        #else /* BT_SPP_AUTHORIZE_BY_MMI */ 
            {
                /* 2nd stage impelmentation before BT2.0 */
            /* #ifdef CUSTOM_SPP */
                SPPCallbackParms parms;               
            /* #else #ifdef CUSTOM_SPP */

                ((SPP_DEV_OS*) (dev->osDev))->set_auth_req_mode = KAL_TRUE;

                //Report(("defaultEventHandler(): recv RFEVENT_OPEN_IND from port=%d",bt_spp_auth_req->port));
                //Report(("Send Auth_Req to MMI to determine accept this req or not"));
            
            /* #ifdef CUSTOM_SPP */
                parms.event = SPP_EVENT_AUTH_REQ;
                parms.dstMod = MOD_MMI;
                parms.p.spp_auth_req.port = ((SPP_DEV_OS*) (dev->osDev))->port;
                SPPCB(&parms);                
            /* #else #ifdef CUSTOM_SPP */
            }
        #endif /* BT_SPP_AUTHORIZE_BY_MMI */ 

#endif
            break;
        }
        case RFEVENT_OPEN:
            kal_trace(TRACE_GROUP_1, SPP_CHANNEL_OPEN);
            dev->state = DEVICE_STATE_CONNECTED;
    #if defined (__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
            dev->lModemStatus.signals = RF_DTR | RF_CTS | RF_CD;
    #endif 
            setHighWaterMark(dev);
            advanceCredit(dev);
            /* Send connection indication to APP */
            connection_id = BTBMProfileAllocateConnIndex(SC_SERIAL_PORT, parms->ptrs.remDev->bdAddr);
            CMGR_SetRemoteAsMaster(parms->ptrs.remDev);
        #ifdef MULTI_RF_CHANNEL
            // BT_BTBM_FUNC_ENTRY3(BT_BTBM_SHOW_BDADDR, parms->ptrs.remDev->bdAddr.addr[0], parms->ptrs.remDev->bdAddr.addr[1], parms->ptrs.remDev->bdAddr.addr[2]);
            // BT_BTBM_FUNC_ENTRY3(BT_BTBM_SHOW_BDADDR, parms->ptrs.remDev->bdAddr.addr[3], parms->ptrs.remDev->bdAddr.addr[4], parms->ptrs.remDev->bdAddr.addr[5]);
        #endif    
            
            lap = 0;
            btmtk_os_memcpy((kal_uint8*) &lap, &parms->ptrs.remDev->bdAddr.addr[0], 3);
            uap = 0;
            uap = parms->ptrs.remDev->bdAddr.addr[3];
            nap = 0;
            btmtk_os_memcpy((kal_uint8*) &nap, & parms->ptrs.remDev->bdAddr.addr[4], 2);
            dev->con_id = connection_id;
            dev->con_id_flag = KAL_TRUE;
            ((SPP_DEV_OS*) (dev->osDev))->wait_first_received_data_in_openstate = KAL_TRUE;

            /* For SPP client, check some additional conditions */
            if (dev->portType == SPP_CLIENT_PORT)
            {

            #if SPP_CLIENT == XA_ENABLED

            #if (SPP_SCO_CLIENT == XA_ENABLED)
                if (dev->scoType == SPP_WITH_SCO)
                {
                    CMGR_RegisterScoHandler(&dev->type.client.cmgr_hdlr);
                    if (BTGet_Decide_SPP_SCO_Enter_Sniff_Mode() != 0)
                    {
                        /* Report(("RFEVENT_OPEN client side: CMGR_ActivateSniffTimer()")); */
                        kal_trace(TRACE_GROUP_1, BT_SPP_SCO_ACTIVATE_SNIFF_TIMER_IN_RFEVENT_OPEN_CLIENT);
                        CMGR_ActivateSniffTimer(&dev->type.client.cmgr_hdlr);
                    }
                }
            #endif /* (SPP_SCO_CLIENT == XA_ENABLED) */
                dev->type.client.flag |= RECV_SPPC_SERVICE_RSP;

                //In this case: means that DISC or DEACTIVATE Req is issused during trying to connect
                //So, just disconnect when connection is established
                //No uart_ind and connect_ind/cnf are needed report to MMI ! 
                if ((dev->type.client.flag & RECV_SPPC_CONNECT_REQ) && (dev->type.client.flag & SEND_SPPC_SERVICE_REQ)
                    && ((dev->type.client.flag & RECV_SPPC_DISCONNECT_REQ) ||
                        (dev->type.client.flag & RECV_SPPC_DEACTIVATE_REQ)))
                {
                    BtStatus rc = 0;

                    //Try to close the SPP RFCOMM channel due to receive SPP_DISCONNECT_REQ or DEACTIVATE_CNF previously
                    //Actually, the SPPC_CONNECT_CNF is not sent, DISC_CNF or DEACTIVATE_CNF is sent latter.

                    //DO NOT set "dev->type.client.flag |= SEND_SPPC_CONNECT_CNF" 
                    //Because if the flag SEND_SPPC_CONNECT_CNF is not set:
                    //meaning that no uart_ind and connect_cnf are sent to MMI, and MMI is only waiting DISC_CNF or DEACTIVATE_CNF
                    //When receiving RF_CLOSED event: 
                    //if((dev->type.client.flag & SEND_SPPC_CONNECT_CNF) != SEND_SPPC_CONNECT_CNF) : 
                    //Uart_plugout_ind is not necessary sent to MMI !

                    rc = SPP_Disconnect(dev);
                    if (rc == BT_STATUS_SUCCESS)
                    {
                        dev->type.client.flag |= SEND_SPPC_SERVICE_DISC_REQ;
                        /* Report(("Call SPP_Disconnect():Waiting to receive RF_CLOSED event")); */
                        kal_trace(TRACE_GROUP_1, BT_SPPC_CALL_SPPDISC);
                    }
                    else
                    {
                        //Because dev->state is DEVICE_STATE_CONNECTED, if failed to disconnect, this means:
                        //Calling RF_CloseChannel() is failed.

                        /* Send DISCONNECT_CNF or DEACTIVATE_CNF with failure result code back to MMI */
                        if (dev->type.client.flag & RECV_SPPC_DISCONNECT_REQ)
                        {
                            dev->type.client.flag |= SEND_SPPC_DISCONNECT_CNF;
                            sppa_send_disconnect_cnf_result(
                                ((SPP_DEV_OS*) dev)->port,
                                dev->con_id,
                                lap,
                                uap,
                                nap,
                                NOT_SUCCESS_RETURN_FROM_SPP_DISC_FUNC
                        #if DUN_CLIENT == XA_ENABLED
                                , ((SPP_DEV_OS*) dev)->profile
                        #endif             
                                );
                        }
                        if (dev->type.client.flag & RECV_SPPC_DEACTIVATE_REQ)
                        {
                        /* #ifdef CUSTOM_SPP */
                            SPPCallbackParms parms;
                        /* #endif #ifdef CUSTOM_SPP */                                                        
                            dev->type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;
                        /* #ifdef CUSTOM_SPP */
                            parms.event = SPP_EVENT_DEACTIVATE_CNF;
                            parms.dstMod = MOD_MMI;
                            SPPCB(&parms);
                        /* #else #ifdef CUSTOM_SPP */
                        }

                        /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                        bt_sppclient_cmgr_link_close((SPP_DEV_OS*) dev->osDev);

                    }

                }
                else if ((dev->type.client.flag & RECV_SPPC_CONNECT_REQ) &&
                         (dev->type.client.flag & SEND_SPPC_SERVICE_REQ))
                {
                /* #ifdef CUSTOM_SPP */
                #ifdef __BT_SPP_GENERAL__
                #else
                    SPPCallbackParms parms;   
                #endif //__BT_SPP_GENERAL__
                /* #else #ifdef CUSTOM_SPP */                    

                    /* Send CONNECT_CNF back to MMI */
                    dev->type.client.flag |= SEND_SPPC_CONNECT_CNF;
                    sppa_send_connect_cnf_success(
                        ((SPP_DEV_OS*) dev)->port,
                        dev->con_id,
                        lap,
                        uap,
                        nap,
                        (kal_uint8) (dev->channel.dlci >> 1)
                #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                        , ((SPP_DEV_OS*) dev)->profile
                #endif 
                        );
                    /*
                     * UART_OWNER_IND must be sent later than the CONNEC_CNF msg 
                     * Because, the MMI receives CONNECT_CNF to fill the corresponding entry with port information:
                     * Then MMI uses the port carried in the UART_OWNER_IND to find the entry's app_inx
                     * If UART_OWNER_IND is sent first, spp client's entry still has no port info due to not receive cnf,
                     * It is not possible to find the correct app_inx of the corresponding SPP Client Application
                     */
                    /* Notify MMI: MSG_ID_BT_SPP_UART_OWNER_IND */
                    /* Using this flag set_auth_req_mode to guarantee the new uart owner will recv UART_Ready_To_Read_Ind after the btstack receives its uart owner cnf */
                #ifdef __BT_SPP_GENERAL__
                    ((SPP_DEV_OS*) dev)->set_auth_req_mode = FALSE;                    
                #else /* #ifdef __BT_SPP_GENERAL__ */
                    ((SPP_DEV_OS*) dev)->set_auth_req_mode = TRUE;                
                    parms.event = SPP_EVENT_UART_OWNER_IND;
                    parms.dstMod = MOD_MMI;
                    parms.p.spp_uart_owner_ind.port = ((SPP_DEV_OS*) dev)->port;                    
                    /* Notify the MMI is the new owner of this port: Must be added later! */
                    SPPCB(&parms);                                                            
                #endif /* #ifdef __BT_SPP_GENERAL__ */                                          
                }
                else
                {
                    Assert(0);
                }
            #endif /* SPP_CLIENT == XA_ENABLED */
            }

            else if (dev->portType == SPP_SERVER_PORT)
            {
            #if (SPP_SCO_SERVER == XA_ENABLED)
                if (dev->scoType == 1)
                {
                    CMGR_CreateDataLink(&(dev->type.sppService->cmgr_hdlr), &(parms->ptrs.remDev->bdAddr));
                    CMGR_RegisterAudioHandler(&(dev->type.sppService->cmgr_hdlr));
                    if (BTGet_Decide_SPP_SCO_Enter_Sniff_Mode() != 0)
                    {
                        /* Report(("RFEVENT_OPEN server side: CMGR_ActivateSniffTimer()")); */
                        kal_trace(TRACE_GROUP_1, BT_SPP_SCO_ACTIVATE_SNIFF_TIMER_IN_RFEVENT_OPEN_SERVER);
                        CMGR_ActivateSniffTimer(&(dev->type.sppService->cmgr_hdlr));
                    }
                }
            #endif /* (SPP_SCO_SERVER == XA_ENABLED) */
            
                /* SPP Server does not use cmgrhdlr, it can set discFlag as TRUE via CMGR_CreateDataLink() */
                /* But if link discFlag does not set as TRUE, if MMI requests to disc SPP then this ACL link will not be disonnected by our side */
                /* If the peer device does not disc ACL link, this device (e.g., IVT version:6.4.249.0) will not bond via our side anymore */
                parms->ptrs.remDev->discFlag = TRUE;
                SPPOS_ConnectInd(dev->osDev, connection_id, lap, uap, nap);

            }

            /* Ask RSSI value */
        #if (SPP_SCO_SUPPORT == XA_ENABLED)
            {
                if (dev->scoType == SPP_WITH_SCO)
                {
                    /* Report(("Recv RFEVENT_OPEN: read RSSI value")); */
                    if (dev->rssi_timer.func != 0)
                    {
                        EVM_CancelTimer(&dev->rssi_timer);
                    }
                    dev->rssi_timer.func = SPP_RetrieveRSSI_Timeout;
                    dev->rssi_timer.context = (void*)dev;
                    EVM_StartTimer(&dev->rssi_timer, 0);
                }
            }
        #endif /* (SPP_SCO_SUPPORT == XA_ENABLED) */

            /* App may have already written to the device. */
            sendToRfComm(dev);

#ifdef __SPP_THROUGHPUT_TEST__
	     if (dev->portType == SPP_CLIENT_PORT)
	     {
	            //printf("SPP client send data\n");
		     g_sppThroughputTestClientPort = ((SPP_DEV_OS*) dev)->port;
		     SPPUartOpen(((SPP_DEV_OS*) dev)->port, MOD_EXTERNAL);
		     spp_throughput_test_send(KAL_FALSE);		 		     
	     }
	     else if (dev->portType == SPP_SERVER_PORT)
	     {
     		     g_sppThroughputTestServerPort = ((SPP_DEV_OS*) dev)->port;
		     time(&g_sppThroughputTestTimeGetStart);
		     	SPPUartOpen(((SPP_DEV_OS*) dev)->port, MOD_EXTERNAL);
	     		spp_throughput_test_send(KAL_TRUE);		 
	     }
	     /*SPPUartOpen(((SPP_DEV_OS*) dev)->port, MOD_EXTERNAL);
	     spp_throughput_test_send();		 */
#endif
            break;

        case RFEVENT_PORT_NEG_IND:
            /* Port negotiation request was received */
            /* Automatically accept since we're basically a legacy serial driver */
            dev->portSettings = *parms->ptrs.portSettings;
            break;

        #if RF_SEND_CONTROL == XA_ENABLED

        case RFEVENT_PORT_NEG_CNF:  /* Port negotiation confirmed */
            SPPOS_Resume(dev->osDev);
            dev->portSettings = *parms->ptrs.portSettings;
            break;

        case RFEVENT_MODEM_STATUS_CNF:  /* Modem status confirmed */
            SPPOS_Resume(dev->osDev);
            break;

        #ifdef __SPP_GET_REMOTE_MODEM_STATUS__
        case RFEVENT_MODEM_STATUS_IND:  /* Modem status received */
            dev->rModemStatus.signals = parms->ptrs.modemStatus->signals;
            dev->rModemStatus.breakLen = parms->ptrs.modemStatus->breakLen;
            break;
        #endif /* __SPP_GET_REMOTE_MODEM_STATUS__ */ 

        #ifdef __SPP_LINE_STATUS__
        case RFEVENT_LINE_STATUS_IND:   /* Line status received */
            /* Line status should not be cleared until the app reads it. */
            dev->lineStatus |= *(parms->ptrs.lineStatus);
            break;
        #endif /* __SPP_LINE_STATUS__ */ 

        case RFEVENT_LINE_STATUS_CNF:   /* Line status confirmed */
            break;

        case RFEVENT_PORT_STATUS_IND:
            status = RF_SendPortStatus(&dev->channel, &dev->portSettings);
            Assert(status == BT_STATUS_SUCCESS);
            break;

        case RFEVENT_PORT_STATUS_CNF:
            break;
        #endif /* RF_SEND_CONTROL == XA_ENABLED */

        case RFEVENT_FLOW_OFF_IND:
        case RFEVENT_FLOW_ON_IND:
            break;

        #if RF_SEND_TEST == XA_ENABLED
        case RFEVENT_TEST_CNF:
            kal_trace(TRACE_GROUP_1, SPP_PASSTHRU_EVENT__xD, parms->event);
            break;
        #endif /* RF_SEND_TEST == XA_ENABLED */

        default:
            kal_trace(TRACE_GROUP_1, SPP_UNKNOWN_EVENT__xD, parms->event);
    }
}


/*****************************************************************************
 * FUNCTION
 *  stateDefaultHandler
 * DESCRIPTION
 *  State machine: catch-all allows defaultHandler to handle events
 * PARAMETERS
 *  dev         [IN]        
 *  parms       [IN]        
 * RETURNS
 *  FALSE   pass event to default handler
 *****************************************************************************/
static BOOL stateDefaultHandler(SppDev *dev, RfCallbackParms *parms)
{
    //UNUSED_PARAMETER(dev);
    //UNUSED_PARAMETER(parms);
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  stateClosed
 * DESCRIPTION
 *  State machine: ignore all RFCOMM events
 * PARAMETERS
 *  dev         [IN]        
 *  parms       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL stateClosed(SppDev *dev, RfCallbackParms *parms)
{
    //UNUSED_PARAMETER(dev);
    //UNUSED_PARAMETER(parms);
    kal_trace(TRACE_GROUP_1, SPP_DEVICE_STATE_CLOSED);
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  stateConnected
 * DESCRIPTION
 *  State machine: the channel/port is open and ready for activity.
 * PARAMETERS
 *  dev         [IN]        
 *  parms       [IN]        
 * RETURNS
 *  TRUE    event was handled here
 *  FALSE   pass event to default handler
 *****************************************************************************/
static BOOL stateConnected(SppDev *dev, RfCallbackParms *parms)
{
    //UNUSED_PARAMETER(dev);
    //UNUSED_PARAMETER(parms);
    return FALSE;
}

#if SPP_SERVER == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  deRegisterService
 * DESCRIPTION
 *  Deregisters the channel. If it is the last channel open for the
 *  service, it also removes the SDP service record.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void deRegisterService(SppDev *dev)
{
    BtStatus status;
#ifdef MULTI_RF_CHANNEL        
    RfServicePack *rfs;

    status = RF_DeregisterServerChannel(&dev->channel, dev->type.sppService->rfService);
#else

    status = RF_DeregisterServerChannel(&dev->channel, &dev->type.sppService->service);
#endif    
    Assert(status != BT_STATUS_FAILED);

    if (status == BT_STATUS_SUCCESS)
    {
        dev->type.sppService->numPorts--;
    #ifdef MULTI_RF_CHANNEL        
        rfs = ContainingRecord(dev->type.sppService->rfService, RfServicePack, service);
        rfs->refCount--;
        // kal_trace(TRACE_GROUP_1, BTLOG_RFCHC_RFCOUNT, rfs, rfs->refCount);
        if (dev->type.sppService->numPorts == 0 && rfs->refCount == 0)
    #else
        if (dev->type.sppService->numPorts == 0)
    #endif
        {
            /* This is the last channel registered for the service. */
    #ifdef MULTI_RF_CHANNEL
            RF_DeregisterService(dev->type.sppService->rfService);
    #else
            RF_DeregisterService(&dev->type.sppService->service);
    #endif
            removeSdpService(dev);
    #ifdef MULTI_RF_CHANNEL
            dev->type.sppService->rfService->serviceId = 0; 
    #else
	    /* 
	     * Bingyi: Do not clear the channal ID that is generated in SPPDRVInit(). 
	     * Register the same RFCOMM channal in activation
	     */
            //dev->type.sppService->service.serviceId = 0;
    #endif
        }
    }
}
#endif /* SPP_SERVER == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  sendToRfComm
 * DESCRIPTION
 *  If we have not been xoff'ed and there is data in the write queue
 *  for the port, send it to RFCOMM. Since RFCOMM does not have it's
 *  own buffers, we must leave our data untouched until a packet
 *  handled event is returned.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void sendToRfComm(SppDev *dev)
{
    U16 bytes;
    BtStatus status;
    BtPacket *pkt;

    SPP_DEV_OS *vp_ptr = (SPP_DEV_OS*) (dev->osDev);

    if (IsListEmpty(&dev->txPacketList))
    {
        /* No packets for transmitting */
        return;
    }

#ifdef __SPP_SLEEP__ 
    //We do not start timer mechanism for SPP to avoid PC's lost data problem in FAX and DUN case
    if ((vp_ptr->profile != FAX_PROFILE) && (vp_ptr->profile != DUN_PROFILE) && (dev->txCount > txSleepCount))
    {
        dev->sppSleep.context = (void*)dev;
        EVM_StartTimer(&(dev->sppSleep), txSleepTime);
        dev->txCount = 0;

        kal_trace(TRACE_GROUP_1, TXSLEEPCOUNT);
        return;
    }
#endif /* __SPP_SLEEP__ */

    pkt = (BtPacket*) RemoveHeadList(&dev->txPacketList);

    /* Safely grab frame size. */
    SPPOS_UnlockDev(dev->osDev);
    bytes = RF_FrameSize(&dev->channel);

    /* bytes = spp_auto_tx_size; */
    SPPOS_LockDev(dev->osDev);

    /* kal_trace(TRACE_GROUP_1,RF_xD , bytes); */
#ifdef __SPP_AUTO_TX__
    if (sppAutoTxFlag == TRUE)
    {
        OS_MemSet(sppAutoTxBuff, 0x58, spp_auto_tx_size);

        if (sppAutoTxCnt == 0x00)
        {
            spp_tx_timer1 = 0;
            /* kal_get_time(&spp_tx_timer1); */
        }
        if (sppAutoTxCnt < 1000)
        {
            sppAutoTxCnt++;
            pkt->data = sppAutoTxBuff;
        }
        else
        {
            bytes = 0;
            /* kal_get_time(&spp_tx_timer2); */
            kal_trace(TRACE_GROUP_1, TX_FINISHED_STARTxD_ENDxD, spp_tx_timer1, spp_tx_timer2);
            {
                U32 rate;

                rate = (1000 * spp_auto_tx_size * 8) / ((spp_tx_timer2 - spp_tx_timer1 + 1) * 4.615);
                kal_trace(TRACE_GROUP_1, TX_RATExD, rate);

            }
            sppAutoTxCnt = 0;
        }
    }
    else
#endif /* __SPP_AUTO_TX__ */ 

        SPPOS_DevTx(dev->osDev, (char **)&pkt->data, &bytes);
#ifdef __SPP_THROUGHPUT_TEST__
	 g_sppThroughputTotalSentLength += bytes;
         g_sppThroughputPeriodicSentLength += bytes;
         //printf("SPP throughput test : %d bytes sent\n",g_sppThroughputTotalSentLength);
         {
             static int spp_count = 0;
             spp_count++;
             if( (spp_count&0xFF) == 0)
             {
	         printf("SPP throughput test : %d bytes sent\n",g_sppThroughputTotalSentLength);
             }
         }
#endif

    /* kal_trace(TRACE_GROUP_1,xD , bytes); */
    if (bytes > 0)
    {

        pkt->dataLen = bytes;
        /*
         * Note that it is possible (probable) that if this is the
         * app thread, we will get a callback BEFORE LOCKDEV() is 
         * reached below.
         */
        SPPOS_UnlockDev(dev->osDev);
        /* Report(("In sendToRfComm, data ready to send=%d",bytes)); */
        kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPP_SENDTORFCOMM_DATA, bytes);
        status = RF_SendData(&dev->channel, pkt);
        SPPOS_LockDev(dev->osDev);
        if (status != BT_STATUS_PENDING)
        {
            InsertHeadList(&dev->txPacketList, &pkt->node);
            kal_trace(TRACE_GROUP_1, SPP_RF_SENDDATA_FAILED_xD, status);
        }
#ifdef __SPP_SLEEP__
        else
        {
            dev->txCount++;
        }
#endif /* __SPP_SLEEP__    */                

    }
    else
    {
        /* Report(("In sendToRfComm(): call SPPOS_DevTxEmpty")); */
        kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPPOS_TX_EMPTY);
        SPPOS_DevTxEmpty(dev->osDev);
        InsertHeadList(&dev->txPacketList, &pkt->node);
    }
}


/*****************************************************************************
 * FUNCTION
 *  advanceCredit
 * DESCRIPTION
 *  Advance credit to the other device.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void advanceCredit(SppDev *dev)
{
    BtStatus status;
    S16 intCredit;
    U8 credit;
    U16 a, b, c;

    /* Calculate total amount of credit. */
    SPPOS_UnlockDev(dev->osDev);
    Assert(RF_FrameSize(&dev->channel) != 0);
    intCredit = (S8) ((S16) (dev->highWater - SPPOS_RxBytes(dev->osDev)) / RF_FrameSize(&dev->channel));
    /* SPPOS_LockDev(dev->osDev); modify for fax profile and add the followings */
    a = dev->highWater;
    b = SPPOS_RxBytes(dev->osDev);
    c = RF_FrameSize(&dev->channel);
    /* Report(("highWater %d, SPPOS_RxBytes %d\n, RF_FrameSize %d\n", a, b, c)); */
    kal_trace(TRACE_GROUP_1, BT_SPP_HIGHWATER_RXBUTES_RFFRAMESIZE, a, b, c);

    /* Check for U8 overflow. */
    if (intCredit > 0xff)
    {
        credit = 0xff;
    }
    else
    {
        credit = (U8) intCredit;
    }
    kal_trace(TRACE_GROUP_1, CREDIT_xD_DEVCREDIT_xD, credit, dev->credit);

    if (credit > dev->credit)
    {
        /* Subtract credit already outstanding. */
        credit -= dev->credit;
        SPPOS_UnlockDev(dev->osDev);
        status = RF_AdvanceCredit(&dev->channel, credit);

        SPPOS_LockDev(dev->osDev);
        if (status == BT_STATUS_SUCCESS)
        {
            dev->credit += credit;
        }

    #if defined (__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
        dev->lModemStatus.signals &= ~RF_FLOW;
    #endif 

    }
}


/*****************************************************************************
 * FUNCTION
 *  setHighWaterMark
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void setHighWaterMark(SppDev *dev)
{
    if (RF_CreditFlowEnabled(&dev->channel) == TRUE)
    {
        /* Use entire buffer. */
        dev->highWater = SPPOS_RxFree(dev->osDev) - RF_FrameSize(&dev->channel);
    }
    else
    {
        /* Inefficient flow control. Set mark at 50% */
        dev->highWater = SPPOS_RxFree(dev->osDev) / 2;
        /* Must have at least one packet's space. */
        Assert(dev->highWater >= RF_FrameSize(&dev->channel));
    }
}


/*****************************************************************************
 * FUNCTION
 *  SetDevUartOwnerAsDefault
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SetDevUartOwnerAsDefault(SppDev *dev)
{
#ifdef __GEMINI__
    ((SPP_DEV_OS*) (dev->osDev))->owner = Get_DualSim_Uart_Default_Owner();
#else /* __GEMINI__ */ 
    /* Set device's owner is the same as the initialition conditions */
    ((SPP_DEV_OS*) (dev->osDev))->owner = MOD_EXTERNAL;
#endif /* __GEMINI__ */ 

}


/*****************************************************************************
 * FUNCTION
 *  closeDevice
 * DESCRIPTION
 *  Cleanup when closing a device.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void closeDevice(SppDev *dev)
{
    SPPOS_FlushRx(dev->osDev);
    SPPOS_FlushTx(dev->osDev);
    dev->state = DEVICE_STATE_CLOSED;

}

#endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */
#endif
