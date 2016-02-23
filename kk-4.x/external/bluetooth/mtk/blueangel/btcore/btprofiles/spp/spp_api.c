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

/* 
 * spp_api.c
 * 
 * This file is part of the SPP profile that includes the interfaces for internal ADP.
 * Flow direction: internal ADP API --> SPP API --> SPP core
 */

 /***************************************************************************
 *
 * File:
 *     $Workfile:sppdrv.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *      This is a little glue layer that performs functions two functions:
 *
 *      1) Initialize the serial devices.
 *
 *      2) Provides a thin layer normally provided by an OS. It implements 
 *      a POSIX-like API for the application to call strictly for 
 *      demonstration purposes. When porting to an OS, these functions
 *      can probably be eliminated.
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
/* basic system service headers */

#include "spp.h"
#include "ring.h"
#include "sdp_adp.h"

#include "bluetooth_struct.h"
#include "sdp.h"
#include "sdpdb.h"
#include "config.h"

#include "btbm_adp.h"
#ifdef MULTI_RF_CHANNEL
#include "bt_common_config.h"
#endif

#include "bt_common.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_spp_struct.h"

#ifdef BTMTK_ON_LINUX
#include <sys/mman.h>   // for madvise(), mmap()
#include <cutils/ashmem.h>
#include "bt_adp_spp_ashm.h"
#endif

/* Note 2007-0714: From this, the following definition and function are only valid if SPP_CLIENT or SPP_SERVER is XA_ENABLED */
#if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)



 
/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/
#define SPP_BASENAME        "/dev/spp/"
#define SPP_BASENAME_LEN    (sizeof(SPP_BASENAME) - 1)  /* exclude trailing 0 */

/*
 *  Ring buffer sizes.
 *  127 is the default RFCOMM buffer size. For tx, we'll arbitrarily make
 *  it 3x.
 */
#define RING_RX_BUFSIZE     (RF_MAX_FRAME_SIZE * SPP_RX_CREDIT)
#define RING_TX_BUFSIZE     (RF_MAX_FRAME_SIZE * SPP_RX_CREDIT)
/* note. (RF_MAX_FRAME_SIZE * SPP_RX_CREDIT) should larger then 1.5K */


/****************************************************************************
 *
 * Local function prototypes
 *
 ****************************************************************************/


/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/

/* 
 *  Structures for our read/write buffers. These would probably be 
 *  allocated by the OS using whatever services and structures available
 *  for buffer management.
 */
static RingBuf ringRBuf[NUM_SPP_DEVS];
static RingBuf ringWBuf[NUM_SPP_DEVS];

/*
 *  Similar to the structures above, the actual data buffers would surely
 *  be dynamically allocated and reallocated. We'll allocate read and write
 *  buffers for each device. Note 'read' and 'write' are from the local
 *  perspective: receive data into 'read', send data via 'write'.
 *
 *  We'll make the rx buffer 6x for flow-control reasons. When we flow
 *  off rx, there still could be packets in the pipe. We'll flow off when
 *  there is room for an additional 3 packets. Note that it has not been
 *  imperically determined how many packets may be in the pipeline.
 */
static U8 rBuffer[NUM_SPP_DEVS][RING_RX_BUFSIZE];
static U8 wBuffer[NUM_SPP_DEVS][RING_TX_BUFSIZE];

#ifdef __SPP_AUTO_TX__
U8 sppAutoTxFlag;
U16 sppAutoTxCnt;

/* U8 sppAutoTxBuff[RING_TX_BUFSIZE]; */
#endif /* __SPP_AUTO_TX__ */ 

/* Device structures */
static SPP_DEV_OS sppOsDev[NUM_SPP_DEVS];

/*
 * Tx Packets for each device. The number is arbitrary. >1 is recommended
 * * for high rates of outbound data. Note: because this sample does not
 * * copy data from the ring buffer to txpacket, it can only use 1 for number
 * * of tx packets.
 */
static BtPacket txPacket[NUM_SPP_DEVS][1];

#if SPP_SERVER == XA_ENABLED
/* RFCOMM service structures server devices */

#if (NUM_SPP_SRVS > 0)
static SppService SerialPortServiceArray[NUM_SPP_SRVS];
#ifdef MULTI_RF_CHANNEL
static RfServicePack SerialPortRfServiceArray[NUM_SPP_SERVERS] = {0};  
static SdpRecord SerialPortSdpRecordArray[NUM_SPP_SERVERS] = {0};
#else
static SdpRecord SerialPortSdpRecordArray[NUM_SPP_SRVS];
#endif
   
#endif /* (NUM_SPP_SRVS > 0) */

/* Server SDP records */

#if DUN_SERVER == XA_ENABLED
static SppService dialupService;
static SdpRecord dialupSdpRecord;
#ifdef MULTI_RF_CHANNEL 
static RfServicePack dialupRfService;  
#endif
#endif /* DUN_SERVER == XA_ENABLED */

#if SPP_SCO_SERVER == XA_ENABLED

static SppService sppSCOServiceArray[NUM_SPP_SCO_SRVS];
static SdpRecord sppSCOSdpRecordArray[NUM_SPP_SCO_SRVS];
#ifdef MULTI_RF_CHANNEL
static RfServicePack sppSCORfService[NUM_SPP_SCO_SRVS] = {0};
#endif

#endif /* SPP_SCO_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED
static SppService faxService;
static SdpRecord faxSdpRecord;
#ifdef MULTI_RF_CHANNEL
static RfServicePack faxRfService;
#endif
#endif /* FAX_SERVER == XA_ENABLED */

#endif /* SPP_SERVER == XA_ENABLED */

#ifdef __GEMINI__
extern module_type Get_DualSim_Uart_Default_Owner(void);
#endif 

SPPCallback SPPCB;  /* CUSTOM_SPP Global callback context */

//#ifdef __BT_HFG_PROFILE__
#if 0
#include "hfg.h"
#include "bt_hfg_struct.h"
#include "bt_adp_hfg.h"
#define hf_port (BT_START_OF_VIRTUAL_PORT + NUM_SPP_DEVS)
#else /* __BT_HFG_PROFILE__ */ 
#define hf_port (BT_START_OF_VIRTUAL_PORT + NUM_SPP_DEVS)
#endif /* __BT_HFG_PROFILE__ */ 


/* Remove static keyword, because this function will be also used in other files */
extern BtStatus openClientPort(SppDev *dev, BtRemoteDevice *btDevice);

extern void SPPOS_SendUartReadyToReadInd(UART_PORT port, module_type owner);

/* using two functions defined in custom folder to specify and obtain the data pattern */
extern BtStatus sppConnect(SppDev *dev, BtRemoteDevice *remDev, U8 serverId);
extern void closeDevice(SppDev *dev);
extern void SetDevUartOwnerAsDefault(SppDev *dev);
extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);



#if 0
/*****************************************************************************
 * FUNCTION
 *  btmtk_spp_activation_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  param1      [IN]        
 *  param2      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_spp_activation_req(U8 param1, U8 param2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}
#endif


#if SPP_CLIENT == XA_ENABLED
/*****************************************************************************
 * FUNCTION
 *  SearchSppClientOsDevBySdpTkn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  sqt     [IN]     
 * RETURNS
 *  
 *****************************************************************************/
SPP_DEV_OS *SearchSppClientOsDevBySdpTkn(SdpQueryToken *sqt)
{
    SPP_DEV_OS *sppclient = NULL;
    int i = 0;

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        sppclient = &sppOsDev[i];
        if ((sppclient->sppDev.portType == SPP_CLIENT_PORT) && ((&(sppclient->sppDev.type.client.sdpToken)) == sqt))
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, FIND_THE_SQT_OWNER_OF_THIS_QUERYCALLBACK_ITS_INDEXxD, i);
        #endif 
            return sppclient;
        }
    }
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  SearchSppClientOsDevBySPPDev
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]    
 * RETURNS
 *  
 *****************************************************************************/
SPP_DEV_OS *SearchSppClientOsDevBySPPDev(SppDev *dev)
{
    int i = 0;

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        if (dev == (&sppOsDev[i].sppDev) && (dev->portType == SPP_CLIENT_PORT))
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, IN_SEARCHSPPCLIENTOSDEVBYSPPDEV_FIND_SPPCLIENT_AT_INXxD, i);
        #endif 
            return &sppOsDev[i];
        }
    }
    return NULL;
}
#endif /* SPP_CLIENT == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  IsThisPortAsSPPServer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_bool IsThisPortAsSPPServer(UART_PORT port)
{
    int i = 0;

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        if ((sppOsDev[i].port == port) && (sppOsDev[i].sppDev.portType == SPP_SERVER_PORT))
        {
            return KAL_TRUE;
        }
    }
    return KAL_FALSE;
}

#if (SPP_SCO_SUPPORT == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  SPP_RetrieveRSSI_Timeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void SPP_RetrieveRSSI_Timeout(EvmTimer *Timer)
{
    BtStatus rssi_status = BT_STATUS_FAILED;
    BtRemoteDevice *remDev = NULL;
    kal_uint16 report_rssi_timer_period = 0;
    SppDev *dev = (SppDev*) Timer->context;
bt_prompt_trace(MOD_BT, "[BT] SPP_RetrieveRSSI_Timeout");
    dev->rssi_timer.func = 0;
    /* Report(("SPP_RetrieveRSSI_Timeout, dev_state=%d",dev->state)); */
    if (dev->state == DEVICE_STATE_CONNECTED)
    {
        /* Add check the sco feature is client or server */
    #if (SPP_SCO_SERVER == XA_ENABLED) && (SPP_SCO_CLIENT == XA_ENABLED)
        if (dev->portType == SPP_SERVER_PORT)
        {
            remDev = dev->type.sppService->cmgr_hdlr.remDev;
        }
        else
        {
            remDev = dev->type.client.remDev;
        }
    #elif (SPP_SCO_CLIENT == XA_ENABLED)
        if (dev->portType == SPP_CLIENT_PORT)
        {
            remDev = dev->type.client.remDev;
        }
    #else 
        if (dev->portType == SPP_SERVER_PORT)
        {
            remDev = dev->type.sppService->cmgr_hdlr.remDev;
        }
    #endif 

        Assert(remDev);

        report_rssi_timer_period = BTGet_Report_RSSI_Timer_Period();
        /* Report(("Report_RSSI_Timer_Period=%d, current link mode=%d",report_rssi_timer_period,remDev->mode)); */
        kal_trace(TRACE_GROUP_1, BT_SPP_SCO_RSSI_TIMER_PERIOD_LINK_MODE, report_rssi_timer_period, remDev->mode);
        if (remDev->mode == BLM_ACTIVE_MODE)
        {
            /* Report(("Ask RSSI value in Active mode")); */
            kal_trace(TRACE_GROUP_1, BT_SPP_SCO_ASK_RSSI_IN_ACTIVE_MODE);
            rssi_status = CMGR_ReadRSSI(remDev);
        }
        else if (remDev->mode == BLM_SNIFF_MODE)
        {
            kal_uint8 read_read_rssi_mode = BTGet_Decide_Report_RSSI_Sniff_Mode();

            if (read_read_rssi_mode == 0)
            {
                /* Report(("Not ask RSSI value in Sniff mode,link state=%d",remDev->state)); */
                kal_trace(TRACE_GROUP_1, BT_SPP_NOT_ASK_RSSI_IN_SNIFF_MODE, remDev->state);
                /*  Set rssi_status as BT_STATUS_PENDING to restart timer to check later again */
                if (remDev->state == BDS_CONNECTED)
                {
                    rssi_status = BT_STATUS_PENDING;
                }
            }
            else
            {
                /* Report(("Still ask RSSI value in Sniff mode")); */
                kal_trace(TRACE_GROUP_1, BT_SPP_ASK_RSSI_IN_SNIFF_MODE);
                rssi_status = CMGR_ReadRSSI(remDev);
            }
        }

        if (rssi_status == BT_STATUS_PENDING)
        {
            /* Report(("Call CMGR_ReadRSSI() to obtain RSSI value")); */
            dev->rssi_timer.func = SPP_RetrieveRSSI_Timeout;
            EVM_StartTimer(&dev->rssi_timer, report_rssi_timer_period);
        }
        else
        {
            /* remDev->state is not in BDS_CONNECTED state */
            /* Report(("RemDev is not in BDS_CONNECTED state, not to start rssi_timer")); */
            kal_trace(TRACE_GROUP_1, BT_SPP_REMDEV_NOT_IN_CONNECTED_RSSI_TIMEOUT);
        }
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SPP_RSSI_TIMEOUT_DEV_STATE, dev->state);
    }

}
#endif /* (SPP_SCO_SUPPORT == XA_ENABLED) */


/*****************************************************************************
 * FUNCTION
 *  SPPDRV_Init
 * DESCRIPTION
 *  This function is OS-specific. It should be called during system init
 *  It would probably initialize the driver and return pointers to the driver entry points.
 *  It might also create the devices themselves.    
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL SPPDRV_Init(void)
{
    int i;
    SPP_DEV_OS *osDev;
#ifdef MULTI_RF_CHANNEL
    int j = 0;
    int srv_index = 0;
    int total_num_spp_servers = 0;
#endif

#if SPP_SERVER == XA_ENABLED

    /* Initialize the related records array: 2007-0328 */
    SppSdpDB_InitRecordsArray();

    //Note that: If NUM_SPP_SRVS =0, it will not enter the for-loop
    //The function SDPDB_GetLocalServiceName and SDPDB_GetLocalServiceAttribute shoudl not be called.
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, REGISTER_GENERAL_SPP_SERVER_NONSPPSCOSERVER_NUM_OF_ENTRIESxD, NUM_SPP_SRVS);
#endif 
#if (NUM_SPP_SRVS > 0)
#ifdef MULTI_RF_CHANNEL
    for (i = 0; i < NUM_SPP_SERVERS; i++)
    {
        total_num_spp_servers += spp_ch_number[i];
    }
    if (total_num_spp_servers != NUM_SPP_SRVS)
    {
        Assert(0);
    }

    for (i = 0; i < NUM_SPP_SERVERS; i++)
    {
        SerialPortSdpRecordArray[i].num = SDPDB_GetLocalServiceAttribute(
                                    SC_SERIAL_PORT0 + i,
                                    &SerialPortSdpRecordArray[i].attribs,
                                    &SerialPortRfServiceArray[i].service.serviceId);
        for (j = 0; j < spp_ch_number[i]; j++, srv_index++)
        {
            SerialPortServiceArray[srv_index].numPorts = 0;            
            osDev = &sppOsDev[srv_index];
            osDev->profile = SPP_PROFILE;
            osDev->name = "sSerial";
            osDev->sppDev.portType = SPP_SERVER_PORT;
            osDev->sppDev.type.sppService = &SerialPortServiceArray[srv_index];
            SerialPortServiceArray[srv_index].nameLen = SDPDB_GetLocalServiceName(SC_SERIAL_PORT0 + i, &SerialPortServiceArray[srv_index].name);
            //kal_trace(TRACE_GROUP_1, BTLOG_RFCH_ASSIGN_SRV_AND_SDP, i);
            osDev->sppDev.type.sppService->rfService = &SerialPortRfServiceArray[i].service;   
            osDev->sppDev.type.sppService->sdpRecord = &SerialPortSdpRecordArray[i];                            
        }
    }
#else
    for (i = 0; i < NUM_SPP_SRVS; i++)
    {
        SerialPortServiceArray[i].numPorts = 0;
        SerialPortServiceArray[i].nameLen = SDPDB_GetLocalServiceName((U16)(SC_SERIAL_PORT0 + i), &SerialPortServiceArray[i].name);
        SerialPortSdpRecordArray[i].num = SDPDB_GetLocalServiceAttribute(
                                            (U16)(SC_SERIAL_PORT0 + i),
                                            &SerialPortSdpRecordArray[i].attribs,
                                            &SerialPortServiceArray[i].service.serviceId);
        osDev = &sppOsDev[i];
        osDev->profile = SPP_PROFILE;
        osDev->name = "sSerial";
        osDev->sppDev.portType = SPP_SERVER_PORT;
        osDev->sppDev.type.sppService = &SerialPortServiceArray[i];
        osDev->sppDev.type.sppService->sdpRecord = &SerialPortSdpRecordArray[i];

    }
#endif
#endif /* (NUM_SPP_SRVS > 0) */

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, REGISTER_SPP_SCO_SERVER_NUM_OF_ENTRIES_xD, NUM_SPP_SCO_SRVS);
#endif 
#if SPP_SCO_SERVER == XA_ENABLED
    for (i = 0; i < NUM_SPP_SCO_SRVS; i++)
    {
        sppSCOServiceArray[i].numPorts = 0;
        sppSCOServiceArray[i].nameLen = SDPDB_GetLocalServiceName(SC_SPP_SCO0 + i, &sppSCOServiceArray[i].name);
#ifdef MULTI_RF_CHANNEL        
        sppSCOServiceArray[i].rfService = &sppSCORfService[i].service;
        sppSCOSdpRecordArray[i].num = SDPDB_GetLocalServiceAttribute(
                                    SC_SPP_SCO0 + i,
                                    &sppSCOSdpRecordArray[i].attribs,
                                    &sppSCOServiceArray[i].rfService->serviceId);       
#else
        sppSCOSdpRecordArray[i].num = SDPDB_GetLocalServiceAttribute(
                                        SC_SPP_SCO0 + i,
                                        &sppSCOSdpRecordArray[i].attribs,
                                        &sppSCOServiceArray[i].service.serviceId);
#endif
        osDev = &sppOsDev[NUM_SPP_SRVS + i];
        /* Add 2007-0907 */
        osDev->profile = SPP_PROFILE;
        osDev->name = "sSerial";
        osDev->sppDev.portType = SPP_SERVER_PORT;
        osDev->sppDev.type.sppService = &sppSCOServiceArray[i];
        osDev->sppDev.type.sppService->sdpRecord = &sppSCOSdpRecordArray[i];
        osDev->sppDev.scoType = 1;
    }
#endif /* SPP_SCO_SERVER == XA_ENABLED */

#if DUN_SERVER == XA_ENABLED
    /* Initialize dialup service structure. */
    dialupService.numPorts = 0;
    dialupService.nameLen = SDPDB_GetLocalServiceName(SC_DIALUP_NETWORKING, &dialupService.name);    
#ifdef MULTI_RF_CHANNEL    
    dialupRfService.service.serviceId = 0;
    dialupSdpRecord.num = SDPDB_GetLocalServiceAttribute(   
                            SC_DIALUP_NETWORKING,
                            &dialupSdpRecord.attribs,
                            &dialupRfService.service.serviceId);
    dialupService.rfService = &dialupRfService.service;   
#else
    dialupService.service.serviceId = 0;
    dialupSdpRecord.num = SDPDB_GetLocalServiceAttribute(
                            SC_DIALUP_NETWORKING,
                            &dialupSdpRecord.attribs,
                            &dialupService.service.serviceId);
#endif
    osDev = &sppOsDev[NUM_SPP_SRVS + NUM_SPP_SCO_SRVS];
    osDev->profile = DUN_PROFILE;
    osDev->name = "sDialup";
    osDev->sppDev.portType = SPP_SERVER_PORT;
    osDev->sppDev.type.sppService = &dialupService;
    osDev->sppDev.type.sppService->sdpRecord = &dialupSdpRecord;
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED

    faxService.numPorts = 0;
    faxService.nameLen = SDPDB_GetLocalServiceName(SC_FAX, &faxService.name);
#ifdef MULTI_RF_CHANNEL    
    faxSdpRecord.num = SDPDB_GetLocalServiceAttribute(SC_FAX, &faxSdpRecord.attribs, &faxRfService.service.serviceId); 
    faxService.rfService = &faxRfService.service;   
#else
    faxSdpRecord.num = SDPDB_GetLocalServiceAttribute(SC_FAX, &faxSdpRecord.attribs, &faxService.service.serviceId);
#endif
    osDev = &sppOsDev[NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS];

    osDev->profile = FAX_PROFILE;
    osDev->name = "sFax";
    osDev->sppDev.portType = SPP_SERVER_PORT;
    osDev->sppDev.type.sppService = &faxService;
    osDev->sppDev.type.sppService->sdpRecord = &faxSdpRecord;

#endif /* FAX_SERVER == XA_ENABLED */

#endif /* SPP_SERVER == XA_ENABLED */

#if SPP_CLIENT == XA_ENABLED
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, REGISTER_GENERAL_SPP_CLIENT_NONSPPSCO_CLIENT_NUM_OF_ENTRIES_xD, NUM_SPP_CLIS);
#endif 
    /* Dev? is a client port for generic Serial Port. */
    for (i = 0; i < NUM_SPP_CLIS; i++)
    {
        osDev = &sppOsDev[NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS + NUM_FAX_SRVS + i];
        osDev->profile = SPP_PROFILE;   
        osDev->name = "cSerial";
        osDev->sppDev.portType = SPP_CLIENT_PORT;
    }

#if DUN_CLIENT == XA_ENABLED
    /* Dev? is a client port for DUN. */
    osDev = &sppOsDev[NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS + NUM_FAX_SRVS + NUM_SPP_CLIS];
    osDev->profile = DUN_PROFILE;   
    osDev->name = "cDialup";
    osDev->sppDev.portType = SPP_CLIENT_PORT;
#endif /* DUN_CLIENT == XA_ENABLED */

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, REGISTER_SPP_SCO_CLIENT_NUM_OF_ENTRIES_xD, NUM_SPP_SCO_CLIS);
#endif 
#if SPP_SCO_CLIENT == XA_ENABLED
    /* Dev? is a client port for LAN Access. */
    osDev = &sppOsDev[NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS + NUM_FAX_SRVS + NUM_SPP_CLIS + NUM_DUN_CLIS];
    osDev->profile = SPP_PROFILE;   
    osDev->name = "cSerial";
    osDev->sppDev.portType = SPP_CLIENT_PORT;
    osDev->sppDev.scoType = SPP_WITH_SCO;
#endif /* SPP_SCO_CLIENT == XA_ENABLED */

#if FAX_CLIENT == XA_ENABLED
    /* Dev? is a client port for FAX. */
    osDev =
        &sppOsDev[NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS + NUM_FAX_SRVS + NUM_SPP_CLIS + NUM_DUN_CLIS +
                  NUM_SPP_SCO_CLIS];
    osDev->profile = FAX_PROFILE;   
    osDev->name = "cFax";
    osDev->sppDev.portType = SPP_CLIENT_PORT;

#endif /* FAX_CLIENT == XA_ENABLED */

#endif /* SPP_CLIENT == XA_ENABLED */

    /* Do initialization common to all of the ports. */
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, NUM_SPP_DEVSxD, NUM_SPP_DEVS);
#endif 
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        osDev = &sppOsDev[i];
        SPP_InitDevice(&osDev->sppDev, txPacket[i], sizeof(txPacket[i]) / sizeof(BtPacket));

        Assert((BT_START_OF_VIRTUAL_PORT + i) < (BT_END_OF_VIRTUAL_PORT + 1));
        osDev->port = BT_START_OF_VIRTUAL_PORT + i;
    #ifndef __GEMINI__
        #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/   
            osDev->owner = MOD_ATCI;
        #else   /* BTMTK_ON_WISE */
        osDev->owner = MOD_EXTERNAL;
        #endif  /* BTMTK_ON_WISE */
    #else 
        osDev->owner = Get_DualSim_Uart_Default_Owner();
    #endif 
        osDev->is_tx_empty = TRUE;
        osDev->notifyReadyToWrite = FALSE;
    #if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
        osDev->dcd_valid = FALSE;
        osDev->dcd = 0;
    #endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */ 
        osDev->readyToRead = TRUE;

        osDev->sppDev.osDev = &sppOsDev[i];

        /* Setup read buffer */
        osDev->rRing = &ringRBuf[i];
        RING_BufInit(osDev->rRing, (char*)rBuffer[i], sizeof(rBuffer[i]));

        /* Setup write buffer */
        osDev->wRing = &ringWBuf[i];
        RING_BufInit(osDev->wRing, (char*)wBuffer[i], sizeof(wBuffer[i]));

        osDev->sppDev.uuid = 0;
        osDev->wait_uart_data_available_ind_rsp_length = 0;
    }

#ifdef __SPP_AUTO_TX__
    sppAutoTxFlag = FALSE;
    sppAutoTxCnt = 0;
    /* OS_MemSet(sppAutoTxBuff, 0x58, RING_TX_BUFSIZE); */
#endif /* __SPP_AUTO_TX__ */ 

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  initVp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void initVp(SPP_DEV_OS *dev)
{
    dev->is_tx_empty = TRUE;
    dev->notifyReadyToWrite = FALSE;
#if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
    dev->dcd_valid = FALSE;
    dev->dcd = 0;
#endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */ 
    dev->readyToRead = TRUE;

#if ((SPP_SCO_CLIENT == XA_ENABLED) || (SPP_SCO_SERVER == XA_ENABLED))
    /* Add by mtk01411: 2007-0912 */
    if (dev->sppDev.rssi_timer.func != 0)
    {
        EVM_CancelTimer(&dev->sppDev.rssi_timer);
    }
#endif /* ((SPP_SCO_CLIENT == XA_ENABLED) || (SPP_SCO_SERVER == XA_ENABLED)) */

}


/*****************************************************************************
 * FUNCTION
 *  setDevOwner
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void setDevOwner(UART_PORT port, module_type owner)
{
    if( (port >= BT_START_OF_VIRTUAL_PORT) && ((port-BT_START_OF_VIRTUAL_PORT) < NUM_SPP_DEVS) ){
        sppOsDev[port - BT_START_OF_VIRTUAL_PORT].owner = owner;
    }
}


/*****************************************************************************
 * FUNCTION
 *  getDev
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
SPP_DEV_OS *getDev(UART_PORT port)
{
    if( (port >= BT_START_OF_VIRTUAL_PORT) && ((port-BT_START_OF_VIRTUAL_PORT) < NUM_SPP_DEVS) ){
        return sppOsDev[port - BT_START_OF_VIRTUAL_PORT].sppDev.osDev;
    }else{
        return NULL;
    }
}

#if SPP_CLIENT == XA_ENABLED
//Add by s tanley: 2007-0314
//This function should be called when ACL link is not established successfully to remove cmgrhdlr.
//If ACL link exists originally, it should not to terminate this due spp client connection failed (May be other profile uses this link)


/*****************************************************************************
 * FUNCTION
 *  bt_sppclient_cmgr_link_close
 * DESCRIPTION
 *  
 * PARAMETERS
 *  sppclient_ptr       [IN] 
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sppclient_cmgr_link_close(SPP_DEV_OS *sppclient_ptr)
{
    BtStatus status;

    if (sppclient_ptr->sppDev.type.client.cmgr_hdlr.bdc != 0)
    {
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPPC_WITH_PORT_TRY_TO_DISC, sppclient_ptr->port);
    #endif 

        /* CMGR_RemoveDataLink(&sppclient_ptr->cmgr_hdlr); */
        status = CMGR_RemoveDataLink(&sppclient_ptr->sppDev.type.client.cmgr_hdlr);
        CMGR_RemoveCmgrWithDevice(&sppclient_ptr->sppDev.type.client.cmgr_hdlr);
    #ifdef MTK_BT_DEBUG_TRACE
        if (status == BT_STATUS_NOT_FOUND)
        {
            kal_trace(TRACE_GROUP_1, BT_SPPC_CMGRHDLR_NOT_FOUND);
        }
    #endif /* MTK_BT_DEBUG_TRACE */ 
    }

#ifdef MTK_BT_DEBUG_TRACE
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SPPC_CMGR_BDC_ALREADY_NULL);
    }
#endif /* MTK_BT_DEBUG_TRACE */ 

    /* Reset the following context to default value: 2007-0410 */
    closeDevice(&(sppclient_ptr->sppDev));
    sppclient_ptr->sppDev.type.client.in_use = KAL_FALSE;
    sppclient_ptr->sppDev.type.client.flag = 0;
    if(sppclient_ptr->sppDev.con_id_flag == KAL_TRUE)
    {
    BTBMProfileReleaseConnIndex(sppclient_ptr->sppDev.con_id);
    }
    sppclient_ptr->sppDev.con_id = 0;
    /* Check if a sniffe timer is running or not triggered by this cmgr_handler */
    CMGR_ClearSniffTimer(&(sppclient_ptr->sppDev.type.client.cmgr_hdlr));
}


/*****************************************************************************
 * FUNCTION
 *  bt_sppclient_cmgr_event_callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  cmgr_hdlr       [IN]        
 *  event           [IN]        
 *  status          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sppclient_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status)
{
    SPP_DEV_OS *sppclient_ptr = NULL;
    BD_ADDR *bt_addr_for_connect = NULL;
    btbm_bd_addr_t spp_btaddr_t;

#if ((SPP_SCO_SERVER == XA_ENABLED) || (SPP_SCO_CLIENT == XA_ENABLED))
    msg_type msg;
#endif 
    int i = 0;

    spp_btaddr_t.lap = 0;
    spp_btaddr_t.uap = 0;
    spp_btaddr_t.nap = 0;
    bt_addr_for_connect = (&(cmgr_hdlr->remDev->bdAddr));
    CONVERT_ARRAY2BDADDR(&spp_btaddr_t, bt_addr_for_connect->addr);

    /* search for which client's cmgr_hdlr is the same with the input paramter */
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        sppclient_ptr = &sppOsDev[i];
        if ((sppclient_ptr->sppDev.portType == SPP_CLIENT_PORT) &&
            ((&(sppclient_ptr->sppDev.type.client.cmgr_hdlr)) == cmgr_hdlr))
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, BT_SPPC_CMGR_EVENT_CALLBACK_EVENT_WITH_DEV_INX, event, i);
        #endif 
            break;
        }

    }

    /* This LINK_CON_CNF is not for this cmgr hdlr */
    if (i == NUM_SPP_DEVS)
    {
        return;
    }

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, BT_SPPC_CMGR_EVENT_CALLBACK_EVENT_STATUS, event, status);
#endif 
    switch (event)
    {
    #if ((SPP_SCO_SERVER == XA_ENABLED) || (SPP_SCO_CLIENT == XA_ENABLED))
        case CMEVENT_AUDIO_LINK_CON:
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, BT_SPP_CMEVENT_AUDIO_LINK_CON_SCO_INIT_CMD, sppclient_ptr->sppDev.scoInitCmd);
    #endif 
            if (sppclient_ptr->sppDev.scoInitCmd == SCO_CMD_INIT)
            {
                msg = MSG_ID_BT_SPP_AUDIO_CONNECT_CNF;
            }
            else
            {
                msg = MSG_ID_BT_SPP_AUDIO_CONNECT_IND;
            }
            sppclient_ptr->sppDev.scoInitCmd = SCO_CMD_NONE;
            if (status != BT_STATUS_SUCCESS)
            {
                sppa_send_audio_command_result(sppclient_ptr->port, BTBM_ADP_FAILED, msg);
            }
            else
            {
                sppa_send_audio_command_result(sppclient_ptr->port, BTBM_ADP_SUCCESS, msg);
            }
            break;
        case CMEVENT_AUDIO_LINK_DIS:
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(
                TRACE_GROUP_1,
                BT_SPP_CMEVENT_AUDIO_LINK_DIS_SCO_INIT_CMD_STATUS,
                sppclient_ptr->sppDev.scoInitCmd,
                status);
        #endif /* MTK_BT_DEBUG_TRACE */ 
            if (sppclient_ptr->sppDev.scoInitCmd == SCO_CMD_INIT)
            {
                msg = MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF;
            }
            else
            {
                msg = MSG_ID_BT_SPP_AUDIO_DISCONNECT_IND;
            }
            sppclient_ptr->sppDev.scoInitCmd = SCO_CMD_NONE;
            if (status != BT_STATUS_SUCCESS)
            {
                sppa_send_audio_command_result(sppclient_ptr->port, BTBM_ADP_FAILED, msg);
            }
            else
            {
                sppa_send_audio_command_result(sppclient_ptr->port, BTBM_ADP_SUCCESS, msg);
            }
            break;

        case CMEVENT_READ_RSSI_COMPLETED:
            /* Notify MMI */
            kal_trace(TRACE_GROUP_1, BT_SPP_SCO_READ_RSSI_COMPLETED, cmgr_hdlr->errCode, cmgr_hdlr->remDev->rssi);
            if (cmgr_hdlr->errCode == 0)
            {
                sppa_send_rssi_result(sppclient_ptr->port, cmgr_hdlr->remDev->bdAddr.addr, cmgr_hdlr->remDev->rssi);
            }

            break;
    #endif /* ((SPP_SCO_SERVER == XA_ENABLED) || (SPP_SCO_CLIENT == XA_ENABLED)) */
        case CMEVENT_DATA_LINK_CON_CNF:

            /* Check if this LINK_CNF is reported due to SPPC's create ACL Link request or not */
            if (sppclient_ptr->sppDev.type.client.flag & SEND_SPPC_ACL_LINK_CONNECT_REQ)
            {
                sppclient_ptr->sppDev.type.client.flag |= RECV_SPPC_ACL_LINK_CONNECT_CNF;
            }
            else
            {
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_LINK_CNF_NOT_FOR_IT);
            #endif 
                return;
            }

            if (status == BT_STATUS_SUCCESS)
            {
                //Do SDP Query
                //status = openClientPort(sppclient_ptr->sppDev, sppclient_ptr->cmgr_hdlr->bdc->link);

                /* This case means that SPP client does not call CMGR_CreateDataLink() function, return here and ignore all events */
                if (sppclient_ptr->sppDev.type.client.cmgr_hdlr.bdc == NULL)
                {
                #ifdef MTK_BT_DEBUG_TRACE
                    kal_trace(TRACE_GROUP_1, BT_SPPC_NOT_CALL_CMGRCREATELINK);
                #endif 
                    return;
                }

                // Assign the bdc->link to client's remDev structure: This only means that the ACL link is established successfully.
                // if SPP is OPEN, dev->con_id will be a valid value in defaultEventHandler() with event RFEVENT_OPEN

            #if ((SPP_SCO_SERVER == XA_ENABLED) || (SPP_SCO_CLIENT == XA_ENABLED))
                sppclient_ptr->sppDev.scoInitCmd = SCO_CMD_NONE;
            #endif 
                /* Using openClientPort() to do SDP query for SPP server's srvchnl_num */
                status = openClientPort(&(sppclient_ptr->sppDev), sppclient_ptr->sppDev.type.client.cmgr_hdlr.bdc->link);

                bt_addr_for_connect = (&(sppclient_ptr->sppDev.type.client.remDev->bdAddr));
                CONVERT_ARRAY2BDADDR(&spp_btaddr_t, bt_addr_for_connect->addr);

                /* This case occurs when SDP query is skipped and srv_chnl_num of msg is used to connect */
                if (status == BT_STATUS_SUCCESS)
                {
                    /* In openClientPort():remDev will be set to sppclient_ptr->sppDev.type.client.cmgr_hdlr.bdc->link */
                    Assert(sppclient_ptr->sppDev.type.client.remDev != NULL);
                    /* Using the srvchnl_num to connect to the SPP Server through the function sppConnect() */
                    status = sppConnect(
                                &(sppclient_ptr->sppDev),
                                sppclient_ptr->sppDev.type.client.remDev,
                                sppclient_ptr->sppDev.type.client.serverId);

                    if (status == BT_STATUS_SUCCESS)
                    {
                        /* SPP Client sends RFCOMM channel connection request to peer side */
                        sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_SERVICE_REQ;
                    }
                    else
                    {
                        sppclient_ptr->sppDev.state = DEVICE_STATE_CLOSED;
                    #ifdef MTK_BT_DEBUG_TRACE
                        kal_trace(TRACE_GROUP_1, BT_SPPC_SPPCONNECT_FAILED_01, status);
                    #endif 
                        //Failure of open rfcomm client channel:
                        //If this spp client received SPP_CONNECT_REQ previously, it is time to send 
                        if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
                        {
                            sppa_send_connect_cnf_failed(
                                spp_btaddr_t.lap,
                                spp_btaddr_t.uap,
                                spp_btaddr_t.nap,
                                SPPC_SERVICE_LEVEL_CON_FAILED
                            #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                                , sppclient_ptr->profile
                            #endif /* DUN_CLIENT == XA_ENABLED */
                                );
                            sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;
                            /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                            bt_sppclient_cmgr_link_close(sppclient_ptr);
                        }
                    }
                }
                else if (status == BT_STATUS_PENDING)
                {
                    /* SDP query is ongoing, waiting for its response in token's callback handler (e.g.,queryCallback) */
                #ifdef MTK_BT_DEBUG_TRACE
                    kal_trace(TRACE_GROUP_1, BT_SPPC_WIAT_SDP_RSP_01);
                #endif 

                    sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_SDP_REQ;
                }
                else if (status == BT_STATUS_INVALID_PARM)
                {
                    sppclient_ptr->sppDev.state = DEVICE_STATE_CLOSED;

                    //remDev is wrong for SDP Query, can not to start a connect to this remote device
                    //if the SPP client recevied MSG_ID_SPP_CONNECT_REQ, it's time to send CNF to MMI
                    if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
                    {
                        sppa_send_connect_cnf_failed(
                            spp_btaddr_t.lap,
                            spp_btaddr_t.uap,
                            spp_btaddr_t.nap,
                            SPPC_SDP_QUERY_REQ_INVALID_PARM
                        #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                            , sppclient_ptr->profile
                        #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */                            
                            );
                        /* SPPOS_sendMsg(MSG_ID_BT_SPP_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)bt_spp_connect_cnf, NULL); */
                        sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;
                    }
                    /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                    bt_sppclient_cmgr_link_close(sppclient_ptr);

                }
                else if (status == BT_STATUS_FAILED)
                {
                    /* SDP query is failed */
                    sppclient_ptr->sppDev.state = DEVICE_STATE_CLOSED;
                #ifdef MTK_BT_DEBUG_TRACE
                    kal_trace(
                        TRACE_GROUP_1,
                        BT_SPP_SDP_QUERY_FAILED_ADDR,
                        bt_addr_for_connect->addr[0],
                        bt_addr_for_connect->addr[1],
                        bt_addr_for_connect->addr[2],
                        bt_addr_for_connect->addr[3],
                        bt_addr_for_connect->addr[4],
                        bt_addr_for_connect->addr[5]);
                #endif /* MTK_BT_DEBUG_TRACE */ 
                    /* bt_sppclient_cmgr_link_close(sppclient_ptr); */

                    /* if the SPP client recevied MSG_ID_SPP_CONNECT_REQ, it's time to send CNF to MMI */
                    if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
                    {
                        sppa_send_connect_cnf_failed(
                            spp_btaddr_t.lap,
                            spp_btaddr_t.uap,
                            spp_btaddr_t.nap,
                            SPPC_SDP_QUERY_REQ_FAILED
                        #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                            , sppclient_ptr->profile
                        #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */                            
                            );
                        sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;

                    }

                    /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                    bt_sppclient_cmgr_link_close(sppclient_ptr);

                }

            }
            else
            {
                //Receive LINK_CONNECT_CNF with error status
                //if the SPP client recevied MSG_ID_SPP_CONNECT_REQ, it's time to send CNF to MMI
                if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
                {
                #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                    sppa_send_connect_cnf_failed(spp_btaddr_t.lap, spp_btaddr_t.uap, spp_btaddr_t.nap, 1, sppclient_ptr->profile);
                #else  /*DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLE */
                    sppa_send_connect_cnf_failed(spp_btaddr_t.lap, spp_btaddr_t.uap, spp_btaddr_t.nap, 1);
                #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLE */                    
                    sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;
                }
                /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                bt_sppclient_cmgr_link_close(sppclient_ptr);
            }
            break;
        case CMEVENT_DATA_LINK_DIS:
            /* Bingyi: We should close the ACL link and clear the Client related context. */
	     bt_sppclient_cmgr_link_close(sppclient_ptr);
            break;
        default:
            break;
    }
}
#endif /* SPP_CLIENT == XA_ENABLED */

#if (SPP_SCO_SERVER == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  bt_sppserver_cmgr_event_callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  cmgr_hdlr       [IN]
 *  event           [IN]        
 *  status          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sppserver_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status)
{
    msg_type msg;
    int i = 0;
    SPP_DEV_OS *spp_ptr = NULL;

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        spp_ptr = &sppOsDev[i];
        if ((spp_ptr->sppDev.portType == SPP_SERVER_PORT) && (spp_ptr->sppDev.scoType == 1) &&
            ((&(spp_ptr->sppDev.type.sppService->cmgr_hdlr)) == cmgr_hdlr))
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, BT_SPPC_CMGR_EVENT_CALLBACK_EVENT_WITH_DEV_INX, event, i);
        #endif 
            break;
        }

    }
    if (i == NUM_SPP_DEVS)
    {
        return;
    }

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, BT_SPPC_CMGR_EVENT_CALLBACK_EVENT_STATUS, event, status);
#endif 

    switch (event)
    {
        case CMEVENT_AUDIO_LINK_CON:
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, BT_SPP_CMEVENT_AUDIO_LINK_CON_SCO_INIT_CMD, spp_ptr->sppDev.scoInitCmd);
    #endif 
            if (spp_ptr->sppDev.scoInitCmd == SCO_CMD_INIT)
            {
                msg = MSG_ID_BT_SPP_AUDIO_CONNECT_CNF;
            }
            else
            {
                msg = MSG_ID_BT_SPP_AUDIO_CONNECT_IND;
            }
            spp_ptr->sppDev.scoInitCmd = SCO_CMD_NONE;
            if (status != BT_STATUS_SUCCESS)
            {
                sppa_send_audio_command_result(spp_ptr->port, BTBM_ADP_FAILED, msg);
            }
            else
            {
                sppa_send_audio_command_result(spp_ptr->port, BTBM_ADP_SUCCESS, msg);
            }
            break;
        case CMEVENT_AUDIO_LINK_DIS:
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(
                TRACE_GROUP_1,
                BT_SPP_CMEVENT_AUDIO_LINK_DIS_SCO_INIT_CMD_STATUS,
                spp_ptr->sppDev.scoInitCmd,
                status);
        #endif /* MTK_BT_DEBUG_TRACE */ 
            if (spp_ptr->sppDev.scoInitCmd == SCO_CMD_INIT)
            {
                msg = MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF;
            }
            else
            {
                msg = MSG_ID_BT_SPP_AUDIO_DISCONNECT_IND;
            }
            spp_ptr->sppDev.scoInitCmd = SCO_CMD_NONE;
            if (status != BT_STATUS_SUCCESS)
            {
                sppa_send_audio_command_result(spp_ptr->port, BTBM_ADP_FAILED, msg);
            }
            else
            {
                sppa_send_audio_command_result(spp_ptr->port, BTBM_ADP_SUCCESS, msg);
            }
            break;
        case CMEVENT_READ_RSSI_COMPLETED:
            kal_trace(TRACE_GROUP_1, BT_SPP_SCO_READ_RSSI_COMPLETED, cmgr_hdlr->errCode, cmgr_hdlr->remDev->rssi);
            if (cmgr_hdlr->errCode == 0)
            {
                sppa_send_rssi_result(spp_ptr->port, cmgr_hdlr->remDev->bdAddr.addr, cmgr_hdlr->remDev->rssi);
            }
            break;
    }
}
#endif /* (SPP_SCO_SERVER == XA_ENABLED) */


/*****************************************************************************
 * FUNCTION
 *  sppa_send_connect_cnf_success
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port                    [IN]        
 *  con_id                  [IN]        
 *  lap                     [IN]        
 *  uap                     [IN]        
 *  nap                     [IN]        
 *  server_chnl_number      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sppa_send_connect_cnf_success(
        UART_PORT port,
        kal_uint8 con_id,
        U32 lap,
        U8 uap,
        U16 nap,
        kal_uint8 server_chnl_number
    #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
        , kal_uint8 profile
    #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
        )
{
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;   
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
    if (DUN_PROFILE == profile)
    {
        parms.event = DUN_EVENT_CONNECT_CNF;        
        parms.p.dun_connect_cnf.cid = con_id;
        parms.p.dun_connect_cnf.port = port;
        parms.p.dun_connect_cnf.lap = 0;
        parms.p.dun_connect_cnf.lap = lap;
        parms.p.dun_connect_cnf.uap = 0;
        parms.p.dun_connect_cnf.uap = uap;
        parms.p.dun_connect_cnf.nap = 0;
        parms.p.dun_connect_cnf.nap = nap;
        parms.p.dun_connect_cnf.server_chnl_num = server_chnl_number;
        parms.p.dun_connect_cnf.result = SPPC_CONNECT_SUCCESS;  /* SPP_CONNEC_CNF with successful result code */
    }
    else if (FAX_PROFILE == profile)
    {
        parms.event = FAX_EVENT_CONNECT_CNF;        
        parms.p.fax_connect_cnf.cid = con_id;
        parms.p.fax_connect_cnf.port = port;
        parms.p.fax_connect_cnf.lap = 0;
        parms.p.fax_connect_cnf.lap = lap;
        parms.p.fax_connect_cnf.uap = 0;
        parms.p.fax_connect_cnf.uap = uap;
        parms.p.fax_connect_cnf.nap = 0;
        parms.p.fax_connect_cnf.nap = nap;
        parms.p.fax_connect_cnf.server_chnl_num = server_chnl_number;
        parms.p.fax_connect_cnf.result = SPPC_CONNECT_SUCCESS;  /* SPP_CONNEC_CNF with successful result code */
    }
    else
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
    {
        parms.event = SPP_EVENT_CONNECT_CNF;        
        parms.p.spp_connect_cnf.cid = con_id;
        parms.p.spp_connect_cnf.port = port;
        parms.p.spp_connect_cnf.lap = 0;
        parms.p.spp_connect_cnf.lap = lap;
        parms.p.spp_connect_cnf.uap = 0;
        parms.p.spp_connect_cnf.uap = uap;
        parms.p.spp_connect_cnf.nap = 0;
        parms.p.spp_connect_cnf.nap = nap;
        parms.p.spp_connect_cnf.server_chnl_num = server_chnl_number;
        parms.p.spp_connect_cnf.result = SPPC_CONNECT_SUCCESS;  /* SPP_CONNEC_CNF with successful result code */
    }
    parms.dstMod = MOD_MMI;
    SPPCB(&parms);
/* #else  #ifdef CUSTOM_SPP */
}


/*****************************************************************************
 * FUNCTION
 *  sppa_send_connect_cnf_failed
 * DESCRIPTION
 *  
 * PARAMETERS
 *  lap         [IN]        
 *  uap         [IN]        
 *  nap         [IN]        
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
void sppa_send_connect_cnf_failed(U32 lap, U8 uap, U16 nap, kal_uint8 result, kal_uint8 profile)
#else  /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
void sppa_send_connect_cnf_failed(U32 lap, U8 uap, U16 nap, kal_uint8 result)
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
{
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;  
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
    if (DUN_PROFILE == profile)
    {
        parms.event = DUN_EVENT_CONNECT_CNF;
        parms.p.dun_connect_cnf.cid = 0;
        parms.p.dun_connect_cnf.port = 0;
        parms.p.dun_connect_cnf.lap = 0;
        parms.p.dun_connect_cnf.lap = lap;
        parms.p.dun_connect_cnf.uap = 0;
        parms.p.dun_connect_cnf.uap = uap;
        parms.p.dun_connect_cnf.nap = 0;
        parms.p.dun_connect_cnf.nap = nap;
        parms.p.dun_connect_cnf.server_chnl_num = 0;    /* for cnf with failed result, this field is set to 0 and will be skipped by MMI layer */    
        parms.p.dun_connect_cnf.result = result;  
    }
    else if (FAX_PROFILE == profile)
    {
        parms.event = FAX_EVENT_CONNECT_CNF;
        parms.p.fax_connect_cnf.cid = 0;
        parms.p.fax_connect_cnf.port = 0;
        parms.p.fax_connect_cnf.lap = 0;
        parms.p.fax_connect_cnf.lap = lap;
        parms.p.fax_connect_cnf.uap = 0;
        parms.p.fax_connect_cnf.uap = uap;
        parms.p.fax_connect_cnf.nap = 0;
        parms.p.fax_connect_cnf.nap = nap;
        parms.p.fax_connect_cnf.server_chnl_num = 0;    /* for cnf with failed result, this field is set to 0 and will be skipped by MMI layer */    
        parms.p.fax_connect_cnf.result = result;  
    }
    else
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
{
        parms.event = SPP_EVENT_CONNECT_CNF;
        parms.p.spp_connect_cnf.cid = 0;
        parms.p.spp_connect_cnf.port = 0;
        parms.p.spp_connect_cnf.lap = 0;
        parms.p.spp_connect_cnf.lap = lap;
        parms.p.spp_connect_cnf.uap = 0;
        parms.p.spp_connect_cnf.uap = uap;
        parms.p.spp_connect_cnf.nap = 0;
        parms.p.spp_connect_cnf.nap = nap;
        parms.p.spp_connect_cnf.server_chnl_num = 0;    /* for cnf with failed result, this field is set to 0 and will be skipped by MMI layer */    
        parms.p.spp_connect_cnf.result = result;  
    }
    parms.dstMod = MOD_MMI;
    SPPCB(&parms);    
/* #else #ifdef CUSTOM_SPP */    
}


/*****************************************************************************
 * FUNCTION
 *  sppa_send_disconnect_cnf_result
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  con_id      [IN]        
 *  lap         [IN]        
 *  uap         [IN]        
 *  nap         [IN]        
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
void sppa_send_disconnect_cnf_result(UART_PORT port, U8 con_id, U32 lap, U8 uap, U16 nap, U8 result, U8 profile)
#else /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
void sppa_send_disconnect_cnf_result(UART_PORT port, kal_uint8 con_id, U32 lap, U8 uap, U16 nap, U8 result)
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
{
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;   

    parms.dstMod = MOD_MMI;
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
    if (DUN_PROFILE == profile)
    {
        parms.event = DUN_EVENT_DISCONNECT_CNF;
        parms.p.dun_disconnect_cnf.cid = con_id;
        parms.p.dun_disconnect_cnf.port = port;
        parms.p.dun_disconnect_cnf.lap = lap;
        parms.p.dun_disconnect_cnf.uap = uap;
        parms.p.dun_disconnect_cnf.nap = nap;
        parms.p.dun_disconnect_cnf.result = result;    
    }
    else if (FAX_PROFILE == profile)
    {
        parms.event = FAX_EVENT_DISCONNECT_CNF;
        parms.p.fax_disconnect_cnf.cid = con_id;
        parms.p.fax_disconnect_cnf.port = port;
        parms.p.fax_disconnect_cnf.lap = lap;
        parms.p.fax_disconnect_cnf.uap = uap;
        parms.p.fax_disconnect_cnf.nap = nap;
        parms.p.fax_disconnect_cnf.result = result;    
    }
    else
#endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
    {
        parms.event = SPP_EVENT_DISCONNECT_CNF;
        parms.p.spp_disconnect_cnf.cid = con_id;
        parms.p.spp_disconnect_cnf.port = port;
        parms.p.spp_disconnect_cnf.lap = lap;
        parms.p.spp_disconnect_cnf.uap = uap;
        parms.p.spp_disconnect_cnf.nap = nap;
        parms.p.spp_disconnect_cnf.result = result;    
    }
    SPPCB(&parms);    
/* #else  #ifdef CUSTOM_SPP */
}


/*****************************************************************************
 * FUNCTION
 *  sppa_send_audio_command_result
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  result      [IN]        
 *  msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sppa_send_audio_command_result(UART_PORT port, kal_uint8 result, msg_type msg)
{
    kal_uint8 audio_path = 0;

/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;   
    parms.dstMod = MOD_MMI;
    switch (msg)
    {
        case MSG_ID_BT_SPP_AUDIO_CONNECT_CNF:
            parms.event = SPP_EVENT_AUDIO_CONNECT_CNF;
            parms.p.spp_audio_connect_cnf.port = port;
            parms.p.spp_audio_connect_cnf.result = result;
            audio_path = 1;
            break;
        case MSG_ID_BT_SPP_AUDIO_CONNECT_IND:
            parms.event = SPP_EVENT_AUDIO_CONNECT_IND;            
            parms.p.spp_audio_connect_ind.port = port;
            audio_path = 1;
            break;
        case MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF:
            parms.event = SPP_EVENT_AUDIO_DISCONNECT_CNF;            
            parms.p.spp_audio_disconnect_cnf.port = port;
            parms.p.spp_audio_disconnect_cnf.result = result;
            audio_path = 0;
            break;
        case MSG_ID_BT_SPP_AUDIO_DISCONNECT_IND:
            parms.event = SPP_EVENT_AUDIO_DISCONNECT_IND;            
            parms.p.spp_audio_disconnect_ind.port = port;
            audio_path = 0;
            break;
        default:
            break;	 
    }
    SPPCB(&parms);    
/* #else  #ifdef CUSTOM_SPP */
}

#if SPP_SCO_SUPPORT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  sppa_send_rssi_result
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  addr            [IN]        
 *  rssi_value      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sppa_send_rssi_result(UART_PORT port, kal_uint8 *addr, kal_uint8 rssi_value)
{

/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;   
    parms.event = SPP_EVENT_SCO_RSSI_IND;
    parms.dstMod = MOD_MMI;
    btmtk_os_memcpy(parms.p.spp_sco_rssi_ind.bd_address, addr, 6);
    parms.p.spp_sco_rssi_ind.rssi_value = rssi_value;    
    SPPCB(&parms);
/* #else  #ifdef CUSTOM_SPP */
}
#endif /* SPP_SCO_SUPPORT == XA_ENABLED */ 


void SPP_Register(SPPCallback Callback)
{
    SPPCB = Callback;
}


#if BT_SPP_AUTHORIZE_BY_MMI
void SPPHandleInternalConnectIndRes(UART_PORT Port, kal_uint32 Lap, 
    kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Result)
{
    BtStatus con_res_status = BT_STATUS_FAILED;
    kal_uint8 result = 0;
    UART_PORT port = 0;
    int i = 0;

    port = Port;
    result = Result;

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        if (sppOsDev[i].port == port)
        {
            Assert(sppOsDev[i].set_auth_req_mode == KAL_TRUE);
            sppOsDev[i].set_auth_req_mode = KAL_FALSE;

            /* Accept or Reject this SPP req */
            if ((sppOsDev[i].sppDev.state == DEVICE_STATE_CLOSED) || (result == KAL_FALSE))
            {
                con_res_status = RF_RejectChannel(&sppOsDev[i].sppDev.channel);
            }
            else
            {
                con_res_status = RF_AcceptChannel(&sppOsDev[i].sppDev.channel);
            }
        #ifdef MTK_BT_DEBUG_TRACE
            if (con_res_status != BT_STATUS_PENDING)
            {
                kal_trace(TRACE_GROUP_1, SPP_RF_ACCEPTCHANNEL_FAILED_xD, con_res_status);
            }
        #endif /* MTK_BT_DEBUG_TRACE */ 
            break;
        }

    }
}
#else /* #if BT_SPP_AUTHORIZE_BY_MMI */
void SPPHandleAuthRsp(UART_PORT Port, kal_bool Result)
{
    BtStatus status;
    int i = 0;

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, BT_SPP_AUTH_RSP_WITH_RESULT, Port, Result);
#endif 

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        if (sppOsDev[i].port == Port)
        {
            Assert(sppOsDev[i].set_auth_req_mode == KAL_TRUE);
            sppOsDev[i].set_auth_req_mode = KAL_FALSE;

            /* Accept or Reject this SPP req */
            if ((sppOsDev[i].sppDev.state == DEVICE_STATE_CLOSED) || (Result == KAL_FALSE))
            {
                status = RF_RejectChannel(&sppOsDev[i].sppDev.channel);
            }
            else
            {
                status = RF_AcceptChannel(&sppOsDev[i].sppDev.channel);
            }
        #ifdef MTK_BT_DEBUG_TRACE
            if (status != BT_STATUS_PENDING)
            {
                kal_trace(TRACE_GROUP_1, SPP_RF_ACCEPTCHANNEL_FAILED_xD, status);
            }
        #endif /* MTK_BT_DEBUG_TRACE */ 
            break;
        }

    }
}    
#endif /* #if BT_SPP_AUTHORIZE_BY_MMI */

void SPPHandleUartOwnerCnf(UART_PORT Port)
{
    int i = 0;
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        if (sppOsDev[i].port == Port)
        {
            Assert(sppOsDev[i].set_auth_req_mode == KAL_TRUE);
            sppOsDev[i].set_auth_req_mode = KAL_FALSE;

            /* Check if BT needs to send UART_READY_TO_READ_IND to MMI or not */
            if (sppOsDev[i].readyToRead == KAL_TRUE)
            {

                if (sppOsDev[i].set_auth_req_mode == KAL_FALSE)
                {
                #ifdef MTK_BT_DEBUG_TRACE

                    if (sppOsDev[i].sppDev.portType == SPP_SERVER_PORT)
                    {
                        kal_trace(
                            TRACE_GROUP_1,
                            DATA_FROM_PEER_DEVICE_IS_ALREADY_RECEIVED_NOW_RECV_UART_OWNER_CNF_FOR_SERVER_PORT);
                    }
                    else
                    {
                        kal_trace(
                            TRACE_GROUP_1,
                            DATA_FROM_PEER_DEVICE_IS_ALREADY_RECEIVED_NOW_RECV_UART_OWNER_CNF_FOR_CLIENT_PORT);
                    }

                    kal_trace(TRACE_GROUP_1, BT_SPPC_SEND_UART_READY_TO_READ_TO_MMI);
                    /* BT_RFCOMM_FUNC_ENTRY(BT_RFCOMMM_SET_READYTOREAD_FALSE); */
                #endif /* MTK_BT_DEBUG_TRACE */ 

                    sppOsDev[i].readyToRead = KAL_FALSE;
                    SPPOS_SendUartReadyToReadInd(sppOsDev[i].port, sppOsDev[i].owner);
                }

            }
            break;
        }
    }
}    
void UartHandlePlugoutCnf(UART_PORT Port)
{
    int i = 0;

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        if (sppOsDev[i].port == Port)
        {
            kal_trace(TRACE_GROUP_1,BT_SPP_RECV_PLUGOUT_CNF_PORT, Port);
            Assert(sppOsDev[i].wait_plugout_cnf_msg== KAL_TRUE);
            sppOsDev[i].wait_plugout_cnf_msg = KAL_FALSE;

            /* Set the uart owner to the default value (i.e., MOD_ATCI) */
            SetDevUartOwnerAsDefault(&sppOsDev[i].sppDev);
            break;
        }
    }

}    

#if SPP_CLIENT == XA_ENABLED
/* Both dun client and spp client use this function to handle connect request */   
void SPPHandleInternalConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, 
    kal_uint8 Server_chnl_num, msg_type Msg_id, U16 remoteUUID)
{
    /* for SPP Client */
    /* Create Data Link by using CMGR, refer to other profile */
    BtStatus status = 0;
    SPP_DEV_OS *sppclient_ptr = NULL;
    BD_ADDR bt_addr_for_connect;
    int i = 0;

    //Check if this port type is SPP_CLIENT_PORT or not: only SPP client is allowed to connect with others
    //Find an available SPP client device for this SPP_CONNECT_REQ

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
/*    #ifdef CUSTOM_SPP    Rewrite the code to support DUN client */
        if ((sppOsDev[i].sppDev.portType == SPP_CLIENT_PORT) &&
            (sppOsDev[i].sppDev.type.client.in_use == KAL_FALSE))
        {
            /* 
             * If SPP_SCO_CLIENT == XA_ENABLED, we have to checke scoType 
             * If DUN_CLIENT == XA_ENABLED, we have to check MSG_ID_BT_DUN_CONNECT_REQ
             **/
        #if (SPP_SCO_CLIENT == XA_ENABLED) && (DUN_CLIENT == XA_ENABLED)
            if ( (Msg_id == MSG_ID_BT_SPP_CONNECT_REQ 
                  && sppOsDev[i].sppDev.scoType != SPP_WITH_SCO 
                  && sppOsDev[i].profile == SPP_PROFILE) 
                  || (Msg_id == MSG_ID_BT_DUN_CONNECT_REQ 
                      && sppOsDev[i].sppDev.scoType != SPP_WITH_SCO 
                      && sppOsDev[i].profile == DUN_PROFILE) 
                  || (Msg_id == MSG_ID_BT_FAX_CONNECT_REQ 
                      && sppOsDev[i].sppDev.scoType != SPP_WITH_SCO 
                      && sppOsDev[i].profile == FAX_PROFILE) 
                  || (Msg_id == MSG_ID_BT_SPP_SCO_CONNECT_REQ 
                      && sppOsDev[i].sppDev.scoType == SPP_WITH_SCO 
                      && sppOsDev[i].profile == SPP_PROFILE)
                )
        #elif (SPP_SCO_CLIENT == XA_ENABLED) && (DUN_CLIENT != XA_ENABLED)
            if ( (Msg_id == MSG_ID_BT_SPP_CONNECT_REQ 
                  && sppOsDev[i].sppDev.scoType != SPP_WITH_SCO 
                  && sppOsDev[i].profile == SPP_PROFILE) 
                  || (Msg_id == MSG_ID_BT_SPP_SCO_CONNECT_REQ 
                      && sppOsDev[i].sppDev.scoType == SPP_WITH_SCO 
                      && sppOsDev[i].profile == SPP_PROFILE)
                )
        #elif (SPP_SCO_CLIENT != XA_ENABLED) && (DUN_CLIENT == XA_ENABLED)
            if ( (Msg_id == MSG_ID_BT_SPP_CONNECT_REQ 
                  && sppOsDev[i].profile == SPP_PROFILE) 
                  ||  (Msg_id == MSG_ID_BT_DUN_CONNECT_REQ 
                      && sppOsDev[i].profile == DUN_PROFILE)
                  ||  (Msg_id == MSG_ID_BT_FAX_CONNECT_REQ 
                      && sppOsDev[i].profile == FAX_PROFILE)
                )
        #else
            if ( (Msg_id == MSG_ID_BT_SPP_CONNECT_REQ 
                  && sppOsDev[i].profile == SPP_PROFILE) 
                )
        #endif
            {
                sppOsDev[i].sppDev.type.client.in_use = KAL_TRUE;
                if (0 != remoteUUID)
                {
                	sppOsDev[i].sppDev.uuid = remoteUUID;
                }
                else if ((0 == remoteUUID) && (SPP_PROFILE == sppOsDev[i].profile))
                {
                	sppOsDev[i].sppDev.uuid = SC_SERIAL_PORT;
                }
                else if ((0 == remoteUUID) && (DUN_PROFILE == sppOsDev[i].profile))
                {
                	sppOsDev[i].sppDev.uuid = SC_DIALUP_NETWORKING;
                }
                else if ((0 == remoteUUID) && (FAX_PROFILE == sppOsDev[i].profile))
                {
                	sppOsDev[i].sppDev.uuid = SC_FAX;
                }
                sppclient_ptr = &sppOsDev[i];
                
                break;
            }
        }            
    /* #else  CUSTOM_SPP */        
    }

    if (i == NUM_SPP_DEVS)
    {

        //Can't find an available SPPoSDev with SPP_CLIENT_PORT type, return CONNECT_CNF with error
        //Not yet define return sap for MMI here !!! 2006-0316
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPPC_NO_AVAILABLE_DEVS);
    #endif 

        /* Send CONNECT_CNF with error back to MMI instead of Asertion it directly */
    #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
        if (Msg_id == MSG_ID_BT_DUN_CONNECT_REQ)
        {
            sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_NO_AVAILABLE_CLI_DEVS, DUN_PROFILE);
        }
        else if (Msg_id == MSG_ID_BT_FAX_CONNECT_REQ)
        {
            sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_NO_AVAILABLE_CLI_DEVS, FAX_PROFILE);
        }
        else
        {
            sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_NO_AVAILABLE_CLI_DEVS, SPP_PROFILE);
        }
    #else /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
        sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_NO_AVAILABLE_CLI_DEVS);    
    #endif
        return;
    }
    /* In any possible states, it should to check the CON_REQ is set or not */
    sppclient_ptr->sppDev.type.client.flag |= RECV_SPPC_CONNECT_REQ;

    /* Remember the server channel number for SPP client to use */
    sppclient_ptr->sppDev.type.client.serverId = Server_chnl_num;

    OS_MemSet(bt_addr_for_connect.addr, 0, BD_ADDR_SIZE);
    CONVERT_BDADDR2ARRAY(bt_addr_for_connect.addr, Lap, Uap, Nap);
    /* status = CMGR_CreateDataLink(sppclient_ptr->cmgr_hdlr, &bt_addr_for_connect); */
    status = CMGR_CreateDataLink(&(sppclient_ptr->sppDev.type.client.cmgr_hdlr), &bt_addr_for_connect);
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, BT_SPP_CMGR_CREATELINK_STATUS, status);
#endif 
    if (status == BT_STATUS_SUCCESS)
    {
        /* Do SDP Query because the ACL Link exists */
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPP_ACLLINK_EXISTED_SET_DISCFLAG_TRUE);
    #endif 
        sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_ACL_LINK_CONNECT_REQ;
        sppclient_ptr->sppDev.type.client.flag |= RECV_SPPC_ACL_LINK_CONNECT_CNF;

        status = openClientPort(&(sppclient_ptr->sppDev), sppclient_ptr->sppDev.type.client.cmgr_hdlr.bdc->link);

        /* This case occurs when SDP query is skipped and srv_chnl_num of msg is used to connect */
        if (status == BT_STATUS_SUCCESS)
        {
            Assert(sppclient_ptr->sppDev.type.client.remDev != NULL);
            /* Using the srvchnl_num to connect to the SPP Server through the function sppConnect() */
            status = sppConnect(
                        &(sppclient_ptr->sppDev),
                        sppclient_ptr->sppDev.type.client.remDev,
                        sppclient_ptr->sppDev.type.client.serverId);

            if (status == BT_STATUS_SUCCESS)
            {
                /* SPP Client sends RFCOMM channel connection request to peer side */
                sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_SERVICE_REQ;
            }
            else
            {
                sppclient_ptr->sppDev.state = DEVICE_STATE_CLOSED;

            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_SPPCONNECT_FAILED_02, status);
            #endif 
                //Failure of open rfcomm client channel:
                //If this spp client received SPP_CONNECT_REQ previously, it is time to send 
                if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
                {
                #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                    sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_SERVICE_LEVEL_CON_FAILED, sppclient_ptr->profile);    
                #else  /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */                    
                    sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_SERVICE_LEVEL_CON_FAILED);
                #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
                    sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;
                    /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                    bt_sppclient_cmgr_link_close(sppclient_ptr);
                }

            }

        }
        else if (status == BT_STATUS_PENDING)
        {
            /* SDP query is ongoing, waiting for its response in token's callback handler (e.g.,queryCallback) */
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, BT_SPPC_WIAT_SDP_RSP_02);
        #endif 

            /* sppclient_ptr->sppDev.type.client.state |= SPP_CLIENT_WAIT_SDP_QUERY_RSP; */
            sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_SDP_REQ;
        }
        else if (status == BT_STATUS_INVALID_PARM)
        {
            /* remDev is wrong for SDP query, can not to start a connect to this remote device */
            sppclient_ptr->sppDev.state = DEVICE_STATE_CLOSED;
            /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
            bt_sppclient_cmgr_link_close(sppclient_ptr);

            Assert(0);
        }
        else if (status == BT_STATUS_FAILED)
        {
            //SDP query is failed
            //sppclient_ptr->sppDev.type.client.flag |= 
            kal_trace(TRACE_GROUP_1, BT_SPP_SDP_QUERY_FAILED_ADDR);
            //If this ACL link is already existed, other profiles may be connected, so 
            //so failure of the SDP query, just report MSG_ID_SPP_CONNECT_CNF with error to MMI 
            //bt_sppclient_cmgr_link_close(sppclient_ptr);

            sppclient_ptr->sppDev.state = DEVICE_STATE_CLOSED;

            if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
            {
            #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
                sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_SDP_QUERY_REQ_FAILED, sppclient_ptr->profile);
            #else  /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
                sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_SDP_QUERY_REQ_FAILED);
            #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
                sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;
                /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                bt_sppclient_cmgr_link_close(sppclient_ptr);

            }
        }

    }
    else if (status == BT_STATUS_PENDING)
    {
        /* Must wait for LINK_CONNECT_CNF and Do SDP query after that */
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPPC_WAIT_LINK_CNF);
    #endif 

        sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_ACL_LINK_CONNECT_REQ;
    }
    else
    {
        /* Error happens */
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPP_CMGR_CREATE_LINK_FAILED, status);
    #endif 
        if (sppclient_ptr->sppDev.type.client.flag & RECV_SPPC_CONNECT_REQ)
        {
        #if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
            sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_CREATE_ACL_LINK_ERROR, sppclient_ptr->profile);
        #else  /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
            sppa_send_connect_cnf_failed(Lap, Uap, Nap, SPPC_CREATE_ACL_LINK_ERROR);
        #endif /* DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED */
            sppclient_ptr->sppDev.type.client.flag |= SEND_SPPC_CONNECT_CNF;
        }
        // Bingyi: Clear the context if ACL link creation failed.
    	 bt_sppclient_cmgr_link_close(sppclient_ptr);        
    }
}    
#endif /* #if SPP_CLIENT == XA_ENABLED */

/* Bingyi: modify to support activation on each port. */
void SPPHandleActivateReq(SppPortType portType, char *svcName, U16 svcUUID)
{   /* open server port */
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;
    int i = 0;       
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;   
/* #endif  #ifdef CUSTOM_SPP */               

    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        vp_ptr = &sppOsDev[i];
        if ((SPP_SERVER_PORT == portType) && (vp_ptr->sppDev.portType & SPP_SERVER_PORT) && (vp_ptr->sppDev.state == DEVICE_STATE_CLOSED) &&
            (vp_ptr->profile == SPP_PROFILE))
        {
        	/* Update customization SDP record array */
        	if ((NULL != svcName) || (0 != svcUUID))
        	{
        		if (NULL != svcName)
        		{
        			SerialPortServiceArray[i].nameLen = SppSdpDB_SetNGetCustomServiceName((U16) i, (U8 **) &SerialPortServiceArray[i].name, svcName);
        		}
        		if (0 != svcUUID)
        		{
        			vp_ptr->sppDev.uuid = svcUUID;
			       SerialPortSdpRecordArray[i].num = SppSdpDB_SetNGetCustomAttribute(
						                                            (U16) i,
						                                            &SerialPortSdpRecordArray[i].attribs,
						                                            &SerialPortServiceArray[i].service.serviceId, 
						                                            svcUUID);
        		}
			vp_ptr->sppDev.type.sppService = &SerialPortServiceArray[i];
			vp_ptr->sppDev.type.sppService->sdpRecord = &SerialPortSdpRecordArray[i];
        	}
            rc = SPP_Open(&(vp_ptr->sppDev), NULL);
            Assert(rc == BT_STATUS_SUCCESS);
        #if (SPP_SCO_SERVER == XA_ENABLED)
            if (vp_ptr->sppDev.scoType == 1)
            {
                vp_ptr->sppDev.type.sppService->cmgrcallback = bt_sppserver_cmgr_event_callback;
                CMGR_RegisterHandler(
                    &(vp_ptr->sppDev.type.sppService->cmgr_hdlr),
                    vp_ptr->sppDev.type.sppService->cmgrcallback);
            }
        #endif /* (SPP_SCO_SERVER == XA_ENABLED) */
        	parms.p.spp_activate_cnf.result = TRUE;
        	parms.p.spp_activate_cnf.port = vp_ptr->port;
        	break;
        }

        /* for SPP client to allow to connect with other device, cmgr handler and its callback is needed : 2007-0314 */
        else if ((SPP_CLIENT_PORT == portType) && (vp_ptr->sppDev.portType == SPP_CLIENT_PORT) && (KAL_FALSE == vp_ptr->sppDev.type.client.in_use))
        {
        #if SPP_CLIENT == XA_ENABLED
            vp_ptr->sppDev.type.client.cmgrcallback = bt_sppclient_cmgr_event_callback;
            /* For SPP client, register its cmgr handler's callback */
            CMGR_RegisterHandler(
                &(vp_ptr->sppDev.type.client.cmgr_hdlr),
                vp_ptr->sppDev.type.client.cmgrcallback);
            vp_ptr->sppDev.type.client.in_use = KAL_FALSE;

	     vp_ptr->sppDev.type.client.flag = 0;
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_ACTIVATE_REQ;
            vp_ptr->sppDev.type.client.flag |= SEND_SPPC_ACTIVATE_CNF;
            parms.p.spp_activate_cnf.result = TRUE;
            parms.p.spp_activate_cnf.port = vp_ptr->port;
            break;
        #endif /* SPP_CLIENT == XA_ENABLED */
        }

}
    if (NUM_SPP_DEVS == i)
    {
    	parms.p.spp_activate_cnf.result = FALSE;
    	parms.p.spp_activate_cnf.port = 0;
    }
/* #ifdef CUSTOM_SPP */
    parms.event = SPP_EVENT_ACTIVATE_CNF;
    parms.dstMod = MOD_MMI;
    SPPCB(&parms);
/* #else  #ifdef CUSTOM_SPP */
}

#if (DUN_SERVER == XA_ENABLED) || (DUN_CLIENT == XA_ENABLED)
void DUNHandleActivateReq(SppPortType portType)
{
    SPP_DEV_OS *vp_ptr = 0;    
    BtStatus rc = 0;
    int i = 0;    
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;  
/* #endif  #ifdef CUSTOM_SPP */    

    /* open server port */
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        vp_ptr = &sppOsDev[i];
        if ((SPP_SERVER_PORT == portType) && (vp_ptr->sppDev.portType & SPP_SERVER_PORT) && 
        	(vp_ptr->sppDev.state == DEVICE_STATE_CLOSED) && (vp_ptr->profile == DUN_PROFILE))
        {
            rc = SPP_Open(&(vp_ptr->sppDev), NULL);
            if(rc == BT_STATUS_SUCCESS)
            {
	            parms.p.dun_activate_cnf.result = TRUE;
	            parms.p.dun_activate_cnf.port = vp_ptr->port;
            }
			else
			{
	            parms.p.dun_activate_cnf.result = FALSE;
	            parms.p.dun_activate_cnf.port = 0;
            }	
            break;
        }
        else if ((SPP_CLIENT_PORT == portType) && (vp_ptr->sppDev.portType == SPP_CLIENT_PORT) && (vp_ptr->profile == DUN_PROFILE))
        {
        #if (SPP_CLIENT == XA_ENABLED) && (DUN_CLIENT == XA_ENABLED)
            vp_ptr->sppDev.type.client.cmgrcallback = bt_sppclient_cmgr_event_callback;
            /* For SPP client, register its cmgr handler's callback */
            CMGR_RegisterHandler(
                &(vp_ptr->sppDev.type.client.cmgr_hdlr),
                vp_ptr->sppDev.type.client.cmgrcallback);
            vp_ptr->sppDev.type.client.in_use = KAL_FALSE;

	     vp_ptr->sppDev.type.client.flag = 0;
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_ACTIVATE_REQ;
            vp_ptr->sppDev.type.client.flag |= SEND_SPPC_ACTIVATE_CNF;
            parms.p.dun_activate_cnf.result = TRUE;
            parms.p.dun_activate_cnf.port = vp_ptr->port;
            break;
        #endif /* (SPP_CLIENT == XA_ENABLED) && (DUN_CLIENT == XA_ENABLED) */
        }
        
    }
    if (NUM_SPP_DEVS == i)
    {
    	parms.p.dun_activate_cnf.result = FALSE;
    	parms.p.dun_activate_cnf.port = 0;
    }
/* #ifdef CUSTOM_SPP */
    parms.event = DUN_EVENT_ACTIVATE_CNF;
    parms.dstMod = MOD_MMI;
    SPPCB(&parms);
/* #else  #ifdef CUSTOM_SPP */               
}

void DUNHandleDeactivateReq(UART_PORT port)
{
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;    
    int i = 0;   
 /* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;  
 /* #endif  #ifdef CUSTOM_SPP */    
  
    /* close the designated opened port */
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        vp_ptr = &sppOsDev[i];
        if ((port == vp_ptr->port) && (vp_ptr->sppDev.portType & SPP_SERVER_PORT) && 
        	(vp_ptr->profile == DUN_PROFILE) && (DEVICE_STATE_CLOSED != vp_ptr->sppDev.state))
        {
            if (DEVICE_STATE_CONNECTED == vp_ptr->sppDev.state)
            {
	        /* Bingyi 20091223: Set a flag to record that this port has to be deactivated after the RFCOMM channel has been closed. */
	        DUNHandleDisconnectReq(port);
	        vp_ptr->sppDev.state = DEVICE_STATE_DEACTIVATING;
	        return ;
            }
            rc = SPP_Close(&(vp_ptr->sppDev));
            Assert(rc == BT_STATUS_SUCCESS);
#ifdef __SPP_SHARED_MEMORY__
	     vp_ptr->getMemPtr = NULL;
	     vp_ptr->getMemSize = 0;
#endif	/* __SPP_SHARED_MEMORY__ */
            parms.p.dun_deactivate_cnf.result = TRUE;
            parms.p.dun_deactivate_cnf.port = vp_ptr->port;
            parms.event = DUN_EVENT_DEACTIVATE_CNF;
            parms.dstMod = MOD_MMI;
            SPPCB(&parms);
            break;
        }
        #if (SPP_CLIENT == XA_ENABLED) && (DUN_CLIENT == XA_ENABLED)
        else if ((port == vp_ptr->port) && (vp_ptr->sppDev.portType & SPP_CLIENT_PORT) && (vp_ptr->profile == DUN_PROFILE) && (NULL != vp_ptr->sppDev.type.client.cmgrcallback))
        {
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_DEACTIVATE_REQ;
            rc = SPP_Close(&(vp_ptr->sppDev));
#ifdef __SPP_SHARED_MEMORY__
	     vp_ptr->getMemPtr = NULL;
	     vp_ptr->getMemSize = 0;
#endif	/* __SPP_SHARED_MEMORY__ */

            if (rc == BT_STATUS_SUCCESS)
            {
                //This means that:
                //case#1: dev->state = DEVICE_STATE_CONNECTED and call RF_CloseChannel() successfully, so when receiving RF_CLOSED event, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag
                //case#2: dev->state = DEVICE_STATE_DISCONNECTED: 
                //2-1: sppclient waits for SDP query rsp, so when receiving SDP query rsp, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
                //2-2: sppclinet called sppConnect() and waits for RF_OPEN event, so when receiving RF_OPEN, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DEACTIVATE_REQ_WAIT_CLOSED);
            #endif 
                vp_ptr->sppDev.type.client.flag |= SEND_SPPC_SERVICE_DISC_REQ;

            }
            else
            {
            /* #ifdef CUSTOM_SPP */
                SPPCallbackParms parms2;
                parms2.p.uart_plugout_ind.port = vp_ptr->port;
            /* #else  #ifdef CUSTOM_SPP */

                /* This means that: Calling RF_CloseChannel() is failed. (returned status is neither BT_STATUS_PENDING nor BT_STATUS_IN_PROGRESS) */
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DEACTIVATE_REQ_BUT_SPPCLOSE_FAILED);
            #endif 

                /* Send DEACTIVATE_CNF with error code back to MMI */
            /* #ifdef CUSTOM_SPP */
                parms.event = DUN_EVENT_DEACTIVATE_CNF;
                parms.dstMod = MOD_MMI;
                parms.p.spp_deactivate_cnf.port = vp_ptr->port;
                parms.p.spp_deactivate_cnf.result = FALSE;
                SPPCB(&parms);
            /* #else  #ifdef CUSTOM_SPP */
                vp_ptr->sppDev.type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;

                /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                bt_sppclient_cmgr_link_close(vp_ptr);

            }
            break;
        }
        #endif /* SPP_CLIENT == XA_ENABLED */
    }
    if (NUM_SPP_DEVS == i)
    {
    	 parms.p.dun_deactivate_cnf.result = FALSE;
    	 parms.p.dun_deactivate_cnf.port = port;
	 parms.event = DUN_EVENT_DEACTIVATE_CNF;
	 parms.dstMod = MOD_MMI;
	 SPPCB(&parms);
    }
}    

#endif /* #if (SPP_CLIENT == XA_ENABLED) || (DUN_CLIENT == XA_ENABLED) */

#if FAX_SERVER == XA_ENABLED
void FAXHandleActivateReq(SppPortType portType)
{
    SPP_DEV_OS *vp_ptr = 0;    
    BtStatus rc = 0;
    int i = 0;
    SPPCallbackParms parms;
    
    /* open server port */
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        vp_ptr = &sppOsDev[i];
        if ((SPP_SERVER_PORT == portType) && (vp_ptr->sppDev.portType & SPP_SERVER_PORT) && 
        	(vp_ptr->sppDev.state == DEVICE_STATE_CLOSED) && (vp_ptr->profile == FAX_PROFILE))
        {
            rc = SPP_Open(&(vp_ptr->sppDev), NULL);
            Assert(rc == BT_STATUS_SUCCESS);
            parms.p.fax_activate_cnf.result = TRUE;
            parms.p.fax_activate_cnf.port = vp_ptr->port;
            break;
        }
        else if ((SPP_CLIENT_PORT == portType) && (vp_ptr->sppDev.portType == SPP_CLIENT_PORT) && (vp_ptr->profile == FAX_PROFILE))
        {
        #if (SPP_CLIENT == XA_ENABLED) && (DUN_CLIENT == XA_ENABLED)
            vp_ptr->sppDev.type.client.cmgrcallback = bt_sppclient_cmgr_event_callback;
            /* For SPP client, register its cmgr handler's callback */
            CMGR_RegisterHandler(
                &(vp_ptr->sppDev.type.client.cmgr_hdlr),
                vp_ptr->sppDev.type.client.cmgrcallback);
            vp_ptr->sppDev.type.client.in_use = KAL_FALSE;

	     vp_ptr->sppDev.type.client.flag = 0;
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_ACTIVATE_REQ;
            vp_ptr->sppDev.type.client.flag |= SEND_SPPC_ACTIVATE_CNF;
            parms.p.fax_activate_cnf.result = TRUE;
            parms.p.fax_activate_cnf.port = vp_ptr->port;
            break;
        #endif /* (SPP_CLIENT == XA_ENABLED) && (DUN_CLIENT == XA_ENABLED) */
        }
    }
    if (NUM_SPP_DEVS == i)
    {
    	parms.p.fax_activate_cnf.result = FALSE;
    	parms.p.fax_activate_cnf.port = 0;
    }
    parms.event = FAX_EVENT_ACTIVATE_CNF;
    parms.dstMod = MOD_MMI;
    SPPCB(&parms);
}   

void FAXHandleDeactivateReq(UART_PORT port)
{
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;    
    int i = 0;
    SPPCallbackParms parms;
    
    /* close the designated opened port */
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        vp_ptr = &sppOsDev[i];
        if ((port == vp_ptr->port) && (vp_ptr->sppDev.portType & SPP_SERVER_PORT) && 
        	(vp_ptr->profile == FAX_PROFILE) && (DEVICE_STATE_CLOSED != vp_ptr->sppDev.state))
        {
            if (DEVICE_STATE_CONNECTED == vp_ptr->sppDev.state)
            {
	        /* Bingyi 20091223: Set a flag to record that this port has to be deactivated after the RFCOMM channel has been closed. */
	        FAXHandleDisconnectReq(port);
	        vp_ptr->sppDev.state = DEVICE_STATE_DEACTIVATING;
	        return ;
            }
            rc = SPP_Close(&(vp_ptr->sppDev));
            Assert(rc == BT_STATUS_SUCCESS);
#ifdef __SPP_SHARED_MEMORY__
	     vp_ptr->getMemPtr = NULL;
	     vp_ptr->getMemSize = 0;
#endif	/* __SPP_SHARED_MEMORY__ */
            parms.p.fax_deactivate_cnf.result = TRUE;
            parms.p.fax_deactivate_cnf.port = vp_ptr->port;
	     parms.event = FAX_EVENT_DEACTIVATE_CNF;
	     parms.dstMod = MOD_MMI;
	     SPPCB(&parms);
            break;
        }
        #if (SPP_CLIENT == XA_ENABLED) && (FAX_CLIENT == XA_ENABLED)
        else if ((port == vp_ptr->port) && (vp_ptr->sppDev.portType & SPP_CLIENT_PORT) && (vp_ptr->profile == FAX_PROFILE) && (NULL != vp_ptr->sppDev.type.client.cmgrcallback))
        {
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_DEACTIVATE_REQ;
            rc = SPP_Close(&(vp_ptr->sppDev));
#ifdef __SPP_SHARED_MEMORY__
	     vp_ptr->getMemPtr = NULL;
	     vp_ptr->getMemSize = 0;
#endif	/* __SPP_SHARED_MEMORY__ */

            if (rc == BT_STATUS_SUCCESS)
            {
                //This means that:
                //case#1: dev->state = DEVICE_STATE_CONNECTED and call RF_CloseChannel() successfully, so when receiving RF_CLOSED event, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag
                //case#2: dev->state = DEVICE_STATE_DISCONNECTED: 
                //2-1: sppclient waits for SDP query rsp, so when receiving SDP query rsp, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
                //2-2: sppclinet called sppConnect() and waits for RF_OPEN event, so when receiving RF_OPEN, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DEACTIVATE_REQ_WAIT_CLOSED);
            #endif 
                vp_ptr->sppDev.type.client.flag |= SEND_SPPC_SERVICE_DISC_REQ;

            }
            else
            {
            /* #ifdef CUSTOM_SPP */
                SPPCallbackParms parms2;
                parms2.p.uart_plugout_ind.port = vp_ptr->port;
            /* #else  #ifdef CUSTOM_SPP */

                /* This means that: Calling RF_CloseChannel() is failed. (returned status is neither BT_STATUS_PENDING nor BT_STATUS_IN_PROGRESS) */
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DEACTIVATE_REQ_BUT_SPPCLOSE_FAILED);
            #endif 

                /* Send DEACTIVATE_CNF with error code back to MMI */
            /* #ifdef CUSTOM_SPP */
                parms.event = FAX_EVENT_DEACTIVATE_CNF;
                parms.dstMod = MOD_MMI;
                parms.p.spp_deactivate_cnf.port = vp_ptr->port;
                parms.p.spp_deactivate_cnf.result = FALSE;
                SPPCB(&parms);
            /* #else  #ifdef CUSTOM_SPP */
                vp_ptr->sppDev.type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;

                /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                bt_sppclient_cmgr_link_close(vp_ptr);

            }
            break;
        }
        #endif /* SPP_CLIENT == XA_ENABLED */
    }
    if (NUM_SPP_DEVS == i)
    {
    	 parms.p.fax_deactivate_cnf.result = FALSE;
    	 parms.p.fax_deactivate_cnf.port = port;
	 parms.event = FAX_EVENT_DEACTIVATE_CNF;
	 parms.dstMod = MOD_MMI;
	 SPPCB(&parms);
    }
}    

void FAXHandleDisconnectReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;    
    
    /* disconnect specific port */
    vp_ptr = getDev(Port);
    if (DEVICE_STATE_CONNECTED != vp_ptr->sppDev.state)
    {
    	sppa_send_disconnect_cnf_result(Port, 0, 0, 0, 0, NOT_SUCCESS_RETURN_FROM_SPP_DISC_FUNC
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
    		, FAX_PROFILE
#endif
    		);
    	return ;
    }
#if FAX_CLIENT == XA_ENABLED
    /* fax client reuse SPPHandleDisconnectReq */
    if(vp_ptr->sppDev.portType == SPP_CLIENT_PORT)
    {
        SPPHandleDisconnectReq(Port);
        return;
    }
#endif /* FAX_CLIENT == XA_ENABLED */
    rc = SPP_Disconnect(&(vp_ptr->sppDev));
    if (rc == BT_STATUS_SUCCESS)
    {
        SPPCallbackParms parms;  
                     
        parms.event = FAX_EVENT_DISCONNECT_CNF;        
        parms.dstMod = MOD_MMI;
        parms.p.fax_disconnect_cnf.port = Port;     
        parms.p.fax_disconnect_cnf.cid = vp_ptr->sppDev.con_id;
        parms.p.fax_disconnect_cnf.lap = 0;
        parms.p.fax_disconnect_cnf.uap = 0;
        parms.p.fax_disconnect_cnf.nap = 0;
        parms.p.fax_disconnect_cnf.result = SPP_DISCONNECT_SUCCESS;
        SPPCB(&parms);
    }
}    
#endif /* #if FAX_SERVER == XA_ENABLED */

void SPPHandleSendDataReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
#if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)    
    RfSignals signals = 0;    
#endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */    
    /* App send this message to indicate SPP to send data */
    vp_ptr = getDev(Port);

    if (vp_ptr->sppDev.state == DEVICE_STATE_CONNECTED)
    {
        SPP_Write(&(vp_ptr->sppDev));
    #if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
        if (vp_ptr->dcd_valid)
        {
            vp_ptr->dcd_valid = KAL_FALSE;
            if (vp_ptr->dcd)
            {
                signals = (U8) (RF_DTR | RF_CTS | RF_CD);
            }
            else
            {
                signals = (U8) (RF_DTR | RF_CTS | ~(RF_CD));
            }

            SPP_SetModemControl(&(vp_ptr->sppDev), signals);

        }
    #endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */ 

    }
}

void SPPHandleGetDataReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
    
    //App send this message to indicate SPP that app has got data from rx buff
    //SPP may do something, like send credit to rfcomm
    vp_ptr = getDev(Port);
    if (vp_ptr->sppDev.state == DEVICE_STATE_CONNECTED)
    {
        SPP_Read(&(vp_ptr->sppDev));
    }
}

void SPPHandleDeactivateReq(UART_PORT port)
{
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;
    int i = 0;
    SPPCallbackParms parms;
    
    /* Close corresponding port */
    for (i = 0; i < NUM_SPP_DEVS; i++)
    {
        vp_ptr = &sppOsDev[i];
        /* Bingyi: we also have to handle the case that SPP_Activation had not been issued before SPP_Deactivation */
        if ((port == vp_ptr->port) && (vp_ptr->sppDev.portType & SPP_SERVER_PORT) && (vp_ptr->profile == SPP_PROFILE) && (DEVICE_STATE_CLOSED != vp_ptr->sppDev.state))
        {
            if (DEVICE_STATE_CONNECTED == vp_ptr->sppDev.state)
            {
	        /* Bingyi 20091223: Set a flag to record that this port has to be deactivated after the RFCOMM channel has been closed. */
	        SPPHandleDisconnectReq(port);
	        vp_ptr->sppDev.state = DEVICE_STATE_DEACTIVATING;
	        return ;
            }
            /* CLOSED_Event will be received later and send uart_plugout_ind msg later */
            rc = SPP_Close(&(vp_ptr->sppDev));
            Assert(rc == BT_STATUS_SUCCESS);
#ifdef __SPP_SHARED_MEMORY__
	     vp_ptr->getMemPtr = NULL;
	     vp_ptr->getMemSize = 0;
#endif	/* __SPP_SHARED_MEMORY__ */
            parms.p.spp_deactivate_cnf.result = TRUE;
	     parms.event = SPP_EVENT_DEACTIVATE_CNF;
	     parms.dstMod = MOD_MMI;
	     parms.p.spp_deactivate_cnf.port = vp_ptr->port;
	     SPPCB(&parms);
	     break;
        }
        #if SPP_CLIENT == XA_ENABLED
        else if ((port == vp_ptr->port) && (vp_ptr->sppDev.portType & SPP_CLIENT_PORT) && (vp_ptr->profile == SPP_PROFILE) && (NULL != vp_ptr->sppDev.type.client.cmgrcallback))
        {
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_DEACTIVATE_REQ;
            rc = SPP_Close(&(vp_ptr->sppDev));
#ifdef __SPP_SHARED_MEMORY__
	     vp_ptr->getMemPtr = NULL;
	     vp_ptr->getMemSize = 0;
#endif	/* __SPP_SHARED_MEMORY__ */

            if (rc == BT_STATUS_SUCCESS)
            {
                //This means that:
                //case#1: dev->state = DEVICE_STATE_CONNECTED and call RF_CloseChannel() successfully, so when receiving RF_CLOSED event, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag
                //case#2: dev->state = DEVICE_STATE_DISCONNECTED: 
                //2-1: sppclient waits for SDP query rsp, so when receiving SDP query rsp, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
                //2-2: sppclinet called sppConnect() and waits for RF_OPEN event, so when receiving RF_OPEN, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DEACTIVATE_REQ_WAIT_CLOSED);
            #endif 
                vp_ptr->sppDev.type.client.flag |= SEND_SPPC_SERVICE_DISC_REQ;

            }
            else
            {
            /* #ifdef CUSTOM_SPP */
                SPPCallbackParms parms2;
                parms2.p.uart_plugout_ind.port = vp_ptr->port;
            /* #else  #ifdef CUSTOM_SPP */

                /* This means that: Calling RF_CloseChannel() is failed. (returned status is neither BT_STATUS_PENDING nor BT_STATUS_IN_PROGRESS) */
            #ifdef MTK_BT_DEBUG_TRACE
                kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DEACTIVATE_REQ_BUT_SPPCLOSE_FAILED);
            #endif 

                /* Send DEACTIVATE_CNF with error code back to MMI */
            /* #ifdef CUSTOM_SPP */
                parms.event = SPP_EVENT_DEACTIVATE_CNF;
                parms.dstMod = MOD_MMI;
                parms.p.spp_deactivate_cnf.port = vp_ptr->port;
                parms.p.spp_deactivate_cnf.result = FALSE;
                SPPCB(&parms);
            /* #else  #ifdef CUSTOM_SPP */
                vp_ptr->sppDev.type.client.flag |= SEND_SPPC_DEACTIVATE_CNF;

#if 0	// Bingyi: Do we still have to send PLUGOUT_IND to owner (when DISCONNECTED_IND has already sent this IND) ?
                /* 2008-0930: Set this wait_plugout_cnf_msg = KAL_TRUE and waits for MSG_ID_BT_SPP_UART_PLUGOUT_CNF from MMI later */
                /* For SPP Client, its port owener will be not MOD_ATCI/MOD_ACTI2, it must set wait_plugout_cnf_msg flag */
            #ifndef __GEMINI__
                if (vp_ptr->owner != MOD_ATCI)
            #else 
                if ((vp_ptr->owner != MOD_ATCI) && (vp_ptr->owner != MOD_ATCI_2))
            #endif 
                    vp_ptr->wait_plugout_cnf_msg = KAL_TRUE;
            /* #ifdef CUSTOM_SPP */
                parms2.event = UART_EVENT_PLUGOUT_IND;
                parms2.dstMod = vp_ptr->owner;
                parms2.p.uart_plugout_ind.profile = vp_ptr->profile;
                SPPCB(&parms2);
            /* #else  #ifdef CUSTOM_SPP */
#endif

                /* Remove cmgr hdlr and check it is necessary to disc ACL Link */
                bt_sppclient_cmgr_link_close(vp_ptr);

            }
            break;
        }
        #endif /* SPP_CLIENT == XA_ENABLED */
    }
    
    if (NUM_SPP_DEVS == i)
    {
    	 parms.p.spp_deactivate_cnf.result = FALSE;
        parms.event = SPP_EVENT_DEACTIVATE_CNF;
        parms.dstMod = MOD_MMI;
        parms.p.spp_deactivate_cnf.port = port;
        SPPCB(&parms);
    }
} 

void SPPHandleDisconnectReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;    
    
    /* disoonnect specific port */
    if (Port < BT_START_OF_VIRTUAL_PORT)
    {
        Assert(0);
    }
    /* PORT_NUM of hf_port is defined as (BT_START_OF_VIRTUAL_PORT + NUM_SPP_DEVS) */
    else if (Port >= (BT_START_OF_VIRTUAL_PORT + NUM_SPP_DEVS))
    {
        Assert(0);
    }
    vp_ptr = getDev(Port);
    if (DEVICE_STATE_CONNECTED != vp_ptr->sppDev.state)
    {
    	sppa_send_disconnect_cnf_result(Port, 0, 0, 0, 0, NOT_SUCCESS_RETURN_FROM_SPP_DISC_FUNC
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
		, SPP_PROFILE
#endif
    		);
    	return ;
    }
    rc = SPP_Disconnect(&(vp_ptr->sppDev));
    if (rc == BT_STATUS_SUCCESS)
    {

    #if SPP_CLIENT == XA_ENABLED
        if (vp_ptr->sppDev.portType == SPP_CLIENT_PORT)
        {
            //This means that:
            //case#1: dev->state = DEVICE_STATE_CONNECTED and call RF_CloseChannel() successfully, so when receiving RF_CLOSED event, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag
            //case#2: dev->state = DEVICE_STATE_DISCONNECTED: 
            //2-1: sppclient waits for SDP query rsp, so when receiving SDP query rsp, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
            //2-2: sppclinet called sppConnect() and waits for RF_OPEN event, so when receiving RF_OPEN, sppclient should check if set RECV_SPPC_DISCONNECT_REQ flag 
            vp_ptr->sppDev.type.client.flag |= RECV_SPPC_DISCONNECT_REQ;
            vp_ptr->sppDev.type.client.flag |= SEND_SPPC_SERVICE_DISC_REQ;
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, BT_SPPC_RECV_DISC_REQ_WAIT_CLOSED);
        #endif 
        }
    #endif /* SPP_CLIENT == XA_ENABLED */

    }
    else
    {
        //Send DISCONNECT_CNF with error [question]???
        //Because: if return value from SPP_Disconnect() is not success, this means:
        //case#1. dev->state is DEVICE_STATE_CLOSED (it never sends SABM to establish RFCOMM chnl, it is impossilbe to receive RF_CLOSED event)
        //case#2. Calling RF_CloseChannel() failed, it nevers send DISC RF chnl to peer. But it may receive RF_CLOSED event due to peer's DISC later

        btbm_bd_addr_t spp_btaddr_t;

    #if SPP_CLIENT == XA_ENABLED
        BD_ADDR *bt_addr_for_connect = NULL;
    #endif 
        if (vp_ptr->sppDev.portType & SPP_SERVER_PORT)
        {
            /* Add by mtk01411: 2007-0713: For SPP Server, no information is obtained about bd_address, but port is unique for MMI to determine which spp entry's disconnection */
            spp_btaddr_t.lap = 0;
            spp_btaddr_t.uap = 0;
            spp_btaddr_t.nap = 0;
        }
        #if SPP_CLIENT == XA_ENABLED
        else if (vp_ptr->sppDev.portType & SPP_CLIENT_PORT)
        {
            bt_addr_for_connect = (&(vp_ptr->sppDev.type.client.remDev->bdAddr));
            CONVERT_ARRAY2BDADDR(&spp_btaddr_t, bt_addr_for_connect->addr);
        }
        #endif /* SPP_CLIENT == XA_ENABLED */
        sppa_send_disconnect_cnf_result(
            Port,
            0,
            spp_btaddr_t.lap,
            spp_btaddr_t.uap,
            spp_btaddr_t.nap,
            NOT_SUCCESS_RETURN_FROM_SPP_DISC_FUNC
    #if DUN_CLIENT == XA_ENABLED
            , vp_ptr->profile
    #endif 
            );
    #if SPP_CLIENT == XA_ENABLED
        bt_sppclient_cmgr_link_close(vp_ptr);
    #endif 
    }
} 

#if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED
void DUNHandleDisconnectReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
    BtStatus rc = 0;    
    
    /* disoonnect specific port */
    vp_ptr = getDev(Port);
    if (DEVICE_STATE_CONNECTED != vp_ptr->sppDev.state)
    {
    	sppa_send_disconnect_cnf_result(Port, 0, 0, 0, 0, NOT_SUCCESS_RETURN_FROM_SPP_DISC_FUNC
#if (DUN_CLIENT == XA_ENABLED || FAX_CLIENT == XA_ENABLED)
    		, DUN_PROFILE
#endif
    		);
    	return ;
    }
#if DUN_CLIENT == XA_ENABLED
    /* dun client reuse SPPHandleDisconnectReq */
    if(vp_ptr->sppDev.portType == SPP_CLIENT_PORT)
    {
        SPPHandleDisconnectReq(Port);
        return;
    }
#endif /* DUN_CLIENT == XA_ENABLED */
    rc = SPP_Disconnect(&(vp_ptr->sppDev));
    if (rc == BT_STATUS_SUCCESS)
    {
        SPPCallbackParms parms;  
                     
        parms.event = DUN_EVENT_DISCONNECT_CNF;        
        parms.dstMod = MOD_MMI;
        parms.p.dun_disconnect_cnf.port = Port;     
        parms.p.dun_disconnect_cnf.cid = vp_ptr->sppDev.con_id;
        /* for DUN Server, no information is obtained about bd_address, but port is unique for MMI to determine which spp entry's disconnection */
        parms.p.dun_disconnect_cnf.lap = 0;
        parms.p.dun_disconnect_cnf.uap = 0;
        parms.p.dun_disconnect_cnf.nap = 0;
        parms.p.dun_disconnect_cnf.result = SPP_DISCONNECT_SUCCESS;
        SPPCB(&parms);
    }
}    
#endif /* #if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED */

void SPPHandleAudioConnectReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
#if (SPP_SCO_CLIENT == XA_ENABLED) || (SPP_SCO_SERVER == XA_ENABLED)
    BtStatus rc = 0;    
#endif    
    vp_ptr = getDev(Port);
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, TRY_TO_OPEN_SCO_HANDLER);
#endif 
    if (vp_ptr->sppDev.portType == SPP_CLIENT_PORT)
    {
    #if (SPP_SCO_CLIENT == XA_ENABLED)
        rc = CMGR_CreateAudioLink(&(vp_ptr->sppDev.type.client.cmgr_hdlr));
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPP_OPEN_SCO_HDLR, rc);
    #endif 
        if (rc == BT_STATUS_SUCCESS)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_SUCCESS,
                MSG_ID_BT_SPP_AUDIO_CONNECT_CNF);
        }
        else if (rc != BT_STATUS_PENDING)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_FAILED,
                MSG_ID_BT_SPP_AUDIO_CONNECT_CNF);
        }
        else
        {
            vp_ptr->sppDev.scoInitCmd = SCO_CMD_INIT;
        }
    #endif /* (SPP_SCO_CLIENT == XA_ENABLED) */
    }
    else if (vp_ptr->sppDev.portType == SPP_SERVER_PORT)
    {
    #if (SPP_SCO_SERVER == XA_ENABLED)
        rc = CMGR_CreateAudioLink(&(vp_ptr->sppDev.type.sppService->cmgr_hdlr));
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPP_OPEN_SCO_HDLR, rc);
    #endif 
        if (rc == BT_STATUS_SUCCESS)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_SUCCESS,
                MSG_ID_BT_SPP_AUDIO_CONNECT_CNF);
        }
        else if (rc != BT_STATUS_PENDING)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_FAILED,
                MSG_ID_BT_SPP_AUDIO_CONNECT_CNF);
        }
        else
        {
            vp_ptr->sppDev.scoInitCmd = SCO_CMD_INIT;
        }
    #endif /* (SPP_SCO_SERVER == XA_ENABLED) */
    }
}

void SPPHandleAudioDisconnectReq(UART_PORT Port)
{
    SPP_DEV_OS *vp_ptr = 0;
#if (SPP_SCO_CLIENT == XA_ENABLED) || (SPP_SCO_SERVER == XA_ENABLED)
    BtStatus rc = 0;    
#endif      
    
    vp_ptr = getDev(Port);
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, TRY_TO_DISCONNECT_SCO_HANDLER);
#endif 
    if (vp_ptr->sppDev.portType == SPP_CLIENT_PORT)
    {
    #if (SPP_SCO_CLIENT == XA_ENABLED)
        rc = CMGR_RemoveAudioLink(&(vp_ptr->sppDev.type.client.cmgr_hdlr));
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPP_DISC_SCO_HDLR, rc);
    #endif 
        if (rc == BT_STATUS_SUCCESS)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_SUCCESS,
                MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF);
        }
        else if (rc != BT_STATUS_PENDING)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_FAILED,
                MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF);
        }
        else
        {
            vp_ptr->sppDev.scoInitCmd = SCO_CMD_INIT;
        }
    #endif /* (SPP_SCO_CLIENT == XA_ENABLED) */
    }
    else if (vp_ptr->sppDev.portType == SPP_SERVER_PORT)
    {
    #if (SPP_SCO_SERVER == XA_ENABLED)
        rc = CMGR_RemoveAudioLink(&(vp_ptr->sppDev.type.sppService->cmgr_hdlr));
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, BT_SPP_DISC_SCO_HDLR, rc);
    #endif 
        if (rc == BT_STATUS_SUCCESS)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_SUCCESS,
                MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF);
        }
        else if (rc != BT_STATUS_PENDING)
        {
            sppa_send_audio_command_result(
                Port,
                BTBM_ADP_FAILED,
                MSG_ID_BT_SPP_AUDIO_DISCONNECT_CNF);
        }
        else
        {
            vp_ptr->sppDev.scoInitCmd = SCO_CMD_INIT;
        }
    #endif /* (SPP_SCO_SERVER == XA_ENABLED) */
    }
}    
/* #endif   end of CUSTOM_SPP */


/*****************************************************************************
 * FUNCTION
 *  SPPUartPutBytes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  Buffaddr        [IN]         
 *  Length          [IN]        
 *  ownerid         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
kal_uint16 SPPUartPutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid)
#else   /* BTMTK_ON_WISE */
#ifdef __SPP_THROUGHPUT_TEST__
U16 SPPUartPutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid)
#else	/* __SPP_THROUGHPUT_TEST__ */
void SPPUartPutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid)
#endif	/* __SPP_THROUGHPUT_TEST__ */
#endif  /* BTMTK_ON_WISE */
{
    kal_uint16 writeLength = 0;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/ || defined(__LINUX_SUPPRESS_ERROR__)
    SPPCallbackParms parms;
#endif    
    SPP_DEV_OS *vp_ptr = NULL;

    U8 *pPrevWrite = NULL;
    kal_bool app_regards_buf_empty = KAL_FALSE;

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        RingBuf *ring = NULL;

        vp_ptr = getDev(port);
        ASSERT(vp_ptr->owner == ownerid);
#ifdef __SPP_SHARED_MEMORY__
        //ASSERT(Buffaddr == vp_ptr->putMemPtr);
#endif	/* __SPP_SHARED_MEMORY__ */
        ring = ((SPP_DEV_OS*) vp_ptr)->wRing;

        if (vp_ptr->sppDev.state == DEVICE_STATE_CONNECTED)
        {

            /* Buffer is empty, sets app_regards_buf_empty to trigger to send data request to BT task:2007-1015 */
            if (ring->pRead == ring->pWrite)
            {
                app_regards_buf_empty = KAL_TRUE;
            }

#ifdef BTMTK_ON_LINUX
            /* Obtain shared memory for put buffer */
            Buffaddr = (U8 *)btmtk_adp_spp_obtain_ashm_put_buffer();
            bt_prompt_trace(MOD_BT, "at response in %d len shared memory: %d, %d, %d, %d", Length, Buffaddr[0], Buffaddr[1], Buffaddr[2], Buffaddr[3]);
#endif

            /*
             * Store the write pointer, if the context switch happens this line before RING_WriteData(): 
             * * Then the sender task will be remove all buf's data, then we can use:
             * * pPrevWrite == pRead to determine the buffer is empty !
             * * If the buffer is empty, the message 
             */
            pPrevWrite = ((SPP_DEV_OS*) vp_ptr)->wRing->pWrite;
            writeLength = RING_WriteData(((SPP_DEV_OS*) vp_ptr)->wRing, (char*)Buffaddr, Length);
            if (writeLength < Length)
            {
                /* Sender Task regards the buffer is full, sets the notifyReadyToWrite flag */
                ((SPP_DEV_OS*) vp_ptr)->notifyReadyToWrite = KAL_TRUE;
            }
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	     parms.p.spp_uart_put_bytes_cnf.length = writeLength;
#endif
	     
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G4_SPP, BT_SPPA_PUTBYTES, Length, writeLength, port);
        #endif 

            if (app_regards_buf_empty || (pPrevWrite == ring->pRead) || vp_ptr->is_tx_empty)
            {
                vp_ptr->is_tx_empty = FALSE;
            #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
                sppa_sendData(vp_ptr->owner, port);
            #else   /* BTMTK_ON_WISE */
		  parms.event = SPP_EVENT_PROFILE_SEND_DATA_REQ;
                parms.p.spp_profile_send_data_req.port = port;
		  parms.dstMod = MOD_BT;
		  SPPCB(&parms);
            #endif  /* BTMTK_ON_WISE */
            }

        }
        else
        {
        #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/            
            parms.p.spp_uart_put_bytes_cnf.length = 0;
        #endif        
        }
    }
#if 0 /* HFP doesn't go through SPP in wisdom project*/
#ifdef __BT_HFG_PROFILE__
    else
    {
        writeLength = RING_WriteData(hfga_cntx_p->wRing, (char*)Buffaddr, Length);
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(BT_TRACE_G4_SPP, BT_SPPA_PUTBYTES, Length, writeLength, port);
    #endif 

        if (hf_string(hfga_cntx_p->wRing))
        {
            hfa_sendData(hfga_cntx_p->owner, port);
        }

    }
#endif /* __BT_HFG_PROFILE__ */ 
#endif
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
    return writeLength;
#else
#ifdef __SPP_THROUGHPUT_TEST__
    return writeLength;
#else
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_PUT_BYTES_CNF;
	parms.p.spp_uart_put_bytes_cnf.port = port;
	parms.p.spp_uart_put_bytes_cnf.profile = vp_ptr->profile;
	SPPCB(&parms);    
#endif
#endif  /* BTMTK_ON_WISE */
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartGetBytes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  Buffaddr        [IN]         
 *  Length          [IN]        
 *  status          [IN]         
 *  ownerid         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /* SPP_PORTING */
kal_uint16 SPPUartGetBytes(
            UART_PORT port,
            kal_uint8 *Buffaddr,
            kal_uint16 Length,
            kal_uint8 *status,
            module_type ownerid)
#else   /* BTMTK_ON_WISE */
void SPPUartGetBytes(
            UART_PORT port,
//            kal_uint8 *Buffaddr,
            kal_uint16 Length,
            kal_uint8 *status,
            module_type ownerid)
#endif  /* BTMTK_ON_WISE */             
{

    kal_uint16 DataRead = 0;
    SPP_DEV_OS *vp_ptr = NULL;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__)/* SPP_PORTING */
    SPPCallbackParms parms;
    U16 maxBufferLength = 0;
    kal_uint8 *Buffaddr;
    parms.p.spp_uart_get_bytes_cnf.buffer = NULL;
    Buffaddr = parms.p.spp_uart_get_bytes_cnf.buffer;
#endif    /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
        ASSERT(vp_ptr->owner == ownerid);

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /* SPP_PORTING */
#ifdef __SPP_SHARED_MEMORY__
	 maxBufferLength = vp_ptr->getMemSize;
	 parms.p.spp_uart_get_bytes_cnf.buffer = vp_ptr->getMemPtr;
#else
	 maxBufferLength = SPP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

	 Length = (Length <= maxBufferLength)?Length:maxBufferLength;
#endif    /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

        if (vp_ptr->sppDev.state == DEVICE_STATE_CONNECTED)
        {
            #ifndef __LINUX_SUPPRESS_ERROR__
            DataRead = RING_ReadData(((SPP_DEV_OS*) vp_ptr)->rRing, (char*)Buffaddr, Length);
	    #endif
        }
        else
        {
        #ifdef BTMTK_ON_WISE  /* SPP_PORTING */
            return DataRead;
        #else   /* BTMTK_ON_WISE */
  	     /* Send CNF back to external ADP */
  	     parms.event = SPP_UART_EVENT_GET_BYTES_CNF;
  	     parms.p.spp_uart_get_bytes_cnf.port = port;
  	     parms.p.spp_uart_get_bytes_cnf.length = 0;
  	     parms.p.spp_uart_get_bytes_cnf.profile = vp_ptr->profile;
  	     SPPCB(&parms);
  	     return;
        #endif  /* BTMTK_ON_WISE */            
        }
    }
#if 0 /* HFP doesn't go through SPP in wisdom project*/
#ifdef __BT_HFG_PROFILE__
    else
    {
        DataRead = RING_ReadData(hfga_cntx_p->rRing, (char*)Buffaddr, Length); /*SPP_PORTING*/
        return DataRead;
    }
#endif /* __BT_HFG_PROFILE__ */ 
#endif

    kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPPA_GETBYTES_DATAREAD_LEN, DataRead, Length);
    if (DataRead < Length)
    {
        kal_uint16 ReadMore = 0;

        vp_ptr->readyToRead = KAL_TRUE;

        ReadMore = RING_DataLen(((SPP_DEV_OS*) vp_ptr)->rRing);

        kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPPA_GETBYTES_CASE1, ReadMore);

        if (ReadMore > 0)
        {
            if ((DataRead + ReadMore) < Length)
            {
                RING_ReadData(((SPP_DEV_OS*) vp_ptr)->rRing, (char*)(Buffaddr + DataRead), ReadMore);
                DataRead += ReadMore;

            }
            else
            {
                S16 available_data_len = 0;

                RING_ReadData(((SPP_DEV_OS*) vp_ptr)->rRing, (char*)(Buffaddr + DataRead), (S16)(Length - DataRead));   /*SPP_PORTING*/
                DataRead = Length;

                vp_ptr->readyToRead = KAL_FALSE;
                available_data_len = RING_DataLen(((SPP_DEV_OS*) vp_ptr)->rRing);

            #ifdef __SPP_ERR_HANDLE_FOR_APP__
                if (available_data_len > 0)
                {
                    vp_ptr->readyToRead = KAL_TRUE;
                }
            #endif /* __SPP_ERR_HANDLE_FOR_APP__ */ 
                kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPPA_GETBYTES_CASE2);
            }
        }
    }

    if( vp_ptr == NULL )
    {
        #ifdef BTMTK_ON_WISE
        return 0;
        #else
        return;
        #endif
    }

    if (vp_ptr->sppDev.credit < SPP_RX_CREDIT / 2)
    {
    #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
        sppa_getData(ownerid, port);
    #else   /* BTMTK_ON_WISE */
	 parms.event = SPP_EVENT_PROFILE_GET_DATA_REQ;
	 parms.p.spp_profile_get_data_req.port = port;
	 parms.dstMod = MOD_BT;
	 SPPCB(&parms);
    #endif  /* BTMTK_ON_WISE */        
    }
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
    return DataRead;
#else   /* BTMTK_ON_WISE */
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_GET_BYTES_CNF;
	parms.p.spp_uart_get_bytes_cnf.port = port;
	parms.p.spp_uart_get_bytes_cnf.length = DataRead;
	parms.p.spp_uart_get_bytes_cnf.profile = vp_ptr->profile;
	SPPCB(&parms);
#endif  /* BTMTK_ON_WISE */
	
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartOpen
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
kal_bool SPPUartOpen(UART_PORT port, module_type owner)
#else   /* BTMTK_ON_WISE */
void SPPUartOpen(UART_PORT port, module_type owner)
#endif  /* BTMTK_ON_WISE */
{
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__)/*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    /* virtual port */
    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        setDevOwner(port, owner);
    }

#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
    return KAL_TRUE;
#else   /* BTMTK_ON_WISE */
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_OPEN_CNF;
	parms.p.spp_uart_open_cnf.port = port;
	parms.p.spp_uart_open_cnf.result = TRUE;
	SPPCB(&parms);
#endif  /* BTMTK_ON_WISE */
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartClose
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPUartClose(UART_PORT port, module_type ownerid)
{
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    // virtual port
    //sppa_vp_ptr->vp[port - BT_START_OF_VIRTUAL_PORT].owner = MOD_NIL;

	/* Send CNF back to external ADP */
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	parms.event = SPP_UART_EVENT_CLOSE_CNF;
	parms.p.spp_uart_close_cnf.port = port;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartGetBytesAvail
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
kal_uint16 SPPUartGetBytesAvail(UART_PORT port)
#else   /* BTMTK_ON_WISE */
void SPPUartGetBytesAvail(UART_PORT port)
#endif  /* BTMTK_ON_WISE */
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__) /*SPP_PORTING*/   
    SPPCallbackParms parms;
    U16 length = 0;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
        /* ok get byte here */
    #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/     
        return RING_DataLen(((SPP_DEV_OS*) vp_ptr)->rRing);
    #else   /* BTMTK_ON_WISE */
        length = RING_DataLen(((SPP_DEV_OS*) vp_ptr)->rRing);
    #endif  /* BTMTK_ON_WISE */
    }
#if 0 /* HFP doesn't go through SPP in wisdom project*/
#ifdef __BT_HFG_PROFILE__
    else
    {
    #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/     
        return RING_DataLen(hfga_cntx_p->rRing);
    #else   /* BTMTK_ON_WISE */
        length = RING_DataLen(hfga_cntx_p->rRing);
    #endif  /* BTMTK_ON_WISE */
    }
#endif /* __BT_HFG_PROFILE__ */ 
#endif

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_GET_BYTES_AVAIL_CNF;
	parms.p.spp_uart_get_bytes_avail_cnf.port = port;
	parms.p.spp_uart_get_bytes_avail_cnf.length = length;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartGetTxRoomLeft
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
kal_uint16 SPPUartGetTxRoomLeft(UART_PORT port)
#else   /* BTMTK_ON_WISE */
void SPPUartGetTxRoomLeft(UART_PORT port)
#endif  /* BTMTK_ON_WISE */
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__) /*SPP_PORTING*/
    SPPCallbackParms parms;
    U16 length = 0;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
        /* ok get byte here */
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
        return RING_DataLen(((SPP_DEV_OS*) vp_ptr)->wRing);
#else   /* BTMTK_ON_WISE */
        length = RING_DataLen(((SPP_DEV_OS*) vp_ptr)->wRing);
#endif  /* BTMTK_ON_WISE */
    }
#if 0 /* HFP doesn't go through SPP in wisdom project*/
#ifdef __BT_HFG_PROFILE__
    else
    {
    #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
        return RING_DataLen(hfga_cntx_p->wRing);
    #else   /* BTMTK_ON_WISE */
        length = RING_DataLen(hfga_cntx_p->wRing);
    #endif  /* BTMTK_ON_WISE */            
    }
#endif /* __BT_HFG_PROFILE__ */ 
#endif

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_GET_TX_ROOM_LEFT_CNF;
	parms.p.spp_uart_get_tx_room_left_cnf.port = port;
	parms.p.spp_uart_get_tx_room_left_cnf.length = length;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    

}


/*****************************************************************************
 * FUNCTION
 *  SPPUartSetOwner
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPUartSetOwner(UART_PORT port, module_type owner)
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
        vp_ptr->owner = owner;
    }
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_SET_OWNER_CNF;
	parms.p.spp_uart_set_owner_cnf.port = port;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    
	
}


/*****************************************************************************
 * FUNCTION
 *  SPPCtrlDCD
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  SDCD        [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPCtrlDCD(UART_PORT port, IO_level SDCD, module_type ownerid)
{
#if defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__)
    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__)/*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);

        vp_ptr->dcd_valid = KAL_TRUE;
        if (SDCD == io_low)
        {
            vp_ptr->dcd = KAL_FALSE;
        }
        else
        {
            vp_ptr->dcd = KAL_TRUE;
        }

        if (RING_DataLen(((SPP_DEV_OS*) vp_ptr)->wRing) == 0)
        {
        #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/           
            sppa_sendData(vp_ptr->owner, port);
        #else   /* BTMTK_ON_WISE */
	     parms.event = SPP_EVENT_PROFILE_SEND_DATA_REQ;
            parms.p.spp_profile_send_data_req.port = port;
	     parms.dstMod = MOD_BT;
	     SPPCB(&parms);
        #endif  /* BTMTK_ON_WISE */         
        }
    }

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_CTRL_DCD_CNF;
	parms.p.spp_uart_ctrl_dcd_cnf.port = port;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    	
#endif /* defined(__BT_DUN_PROFILE__) || defined(__BT_FAX_PROFILE__) */ 
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartCtrlBreak
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  SBREAK      [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPUartCtrlBreak(UART_PORT port, IO_level SBREAK, module_type ownerid)
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);

        if (SBREAK == io_low)
        {
            /* dlc->fSignalBreak=KAL_FALSE; */
        }
        else
        {
            /* dlc->fSignalBreak = KAL_TRUE; */
        }
        /* If there is no data to send, notify DLC scheduler */
        if (RING_DataLen(((SPP_DEV_OS*) vp_ptr)->wRing) == 0)
        {
            // Notify CMUX Task
            //cmux_notifyDataAvailable(vp_ptr->owner,port);
        }
    }
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_CTRL_BREAK_CNF;
	parms.p.spp_uart_ctrl_break_cnf.port = port;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    	
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartGetOwnerID
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
module_type SPPUartGetOwnerID(UART_PORT port)
#else   /* BTMTK_ON_WISE */
void SPPUartGetOwnerID(UART_PORT port)
#endif  /* BTMTK_ON_WISE */
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__) /*SPP_PORTING*/
    SPPCallbackParms parms;
    module_type owner;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */    

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
    #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/
        return vp_ptr->owner;
    #else   /* BTMTK_ON_WISE */
        owner = vp_ptr->owner;
    #endif  /* BTMTK_ON_WISE */
    }
#if 0 /* HFP doesn't go through SPP in wisdom project*/
#ifdef __BT_HFG_PROFILE__
    else
    {
    #ifdef BTMTK_ON_WISE  /*SPP_PORTING*/       
        return hfga_cntx_p->owner;
    #else   /* BTMTK_ON_WISE */
        owner = hfga_cntx_p->owner;
    #endif  /* BTMTK_ON_WISE */
    }
#endif /* __BT_HFG_PROFILE__ */ 
#endif

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_GET_OWNER_ID_CNF;
	parms.p.spp_uart_get_owner_id_cnf.port = port;
	parms.p.spp_uart_get_owner_id_cnf.owner = owner;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartClrRxBuffer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPUartClrRxBuffer(UART_PORT port, module_type ownerid)
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    kal_trace(TRACE_GROUP_1, SPPA_CLRRX, hf_port, port, ownerid);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
        RING_BufFlush(((SPP_DEV_OS*) vp_ptr)->rRing);
        vp_ptr->readyToRead = KAL_TRUE;
    }
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_CLR_RX_BUFFER_CNF;
	parms.p.spp_uart_clr_rx_buffer_cnf.port = port;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
}


/*****************************************************************************
 * FUNCTION
 *  SPPUartClrTxBuffer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPUartClrTxBuffer(UART_PORT port, module_type ownerid)
{

    SPP_DEV_OS *vp_ptr;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
    SPPCallbackParms parms;
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */

    ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
    if (port != hf_port)
    {
        vp_ptr = getDev(port);
        RING_BufFlush(((SPP_DEV_OS*) vp_ptr)->wRing);
    }

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/
	/* Send CNF back to external ADP */
	parms.event = SPP_UART_EVENT_CLR_TX_BUFFER_CNF;
	parms.p.spp_uart_clr_tx_buffer_cnf.port = port;
	SPPCB(&parms);
#endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
}


void SPPEnableReq(void)
{
	SPPCallbackParms parms;

	/* Send CNF back to external ADP */
	parms.event = SPP_EVENT_ENABLE_CNF;
	parms.dstMod = MOD_MMI;
	SPPCB(&parms);
}


void SPPDisableReq(void)
{
	SPPCallbackParms parms;

	/* Send CNF back to external ADP */
	parms.event = SPP_EVENT_DISABLE_CNF;
	parms.dstMod = MOD_MMI;
	SPPCB(&parms);
}


void SPPUartDataAvailableIndRsp(UART_PORT port, kal_uint16 Length)
{
	SPP_DEV_OS *vp_ptr;
	SPPCallbackParms parms;

	ASSERT(port <= BT_END_OF_VIRTUAL_PORT);

	vp_ptr = getDev(port);
	if (0 != Length)
	{
		RING_BufDelete(((SPP_DEV_OS*) vp_ptr)->rRing, Length);
	}
	((SPP_DEV_OS*) vp_ptr)->wait_uart_data_available_ind_rsp_length -= Length;


	SPPOS_SendUARTDataAvailableInd(port);
		
	if (vp_ptr->sppDev.credit < SPP_RX_CREDIT / 2)
	{
		//sppa_getData(ownerid, port);
		parms.event = SPP_EVENT_PROFILE_GET_DATA_REQ;
		parms.p.spp_profile_get_data_req.port = port;
		parms.dstMod = MOD_BT;
		SPPCB(&parms);
	}
}


#ifdef __SPP_SHARED_MEMORY__
void SPPAssignBufferReq(UART_PORT port, U8 *getBufPtr, U16 getBufSize)
{
	SPP_DEV_OS *vp_ptr;
	SPPCallbackParms parms;

#ifdef BTMTK_ON_LINUX

	ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
	
	vp_ptr = getDev(port);
	vp_ptr->getMemPtr = (U8 *)btmtk_adp_spp_obtain_ashm_get_buffer();
	vp_ptr->getMemSize = getBufSize;

#else
	ASSERT(port <= BT_END_OF_VIRTUAL_PORT);
	ASSERT(NULL != getBufPtr);
	ASSERT(0 >= getBufSize);

	vp_ptr = getDev(port);
	vp_ptr->getMemPtr = getBufPtr;
	vp_ptr->getMemSize = getBufSize;
#endif	/* BTMTK_ON_LINUX */

	parms.event = SPP_UART_EVENT_ASSIGN_BUFFER_CNF;
	parms.p.spp_uart_assign_buffer_cnf.port = port;
	parms.dstMod = MOD_MMI;
	SPPCB(&parms);
	
}
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef BTMTK_ON_WISE  /*SPP_PORTING*/

/*****************************************************************************
 * FUNCTION
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  Buffaddr        [IN]        
 *  Length          [IN]        
 *  ownerid         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint16 SppUartPutISRBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid)  
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Running to here means tst using IRCOMM port, not supporting TST use IRCOMM so should not run to here */
    ASSERT(KAL_FALSE);
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  SppaUartGetTxISRRoomLeft
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint16 SppUartGetTxISRRoomLeft(UART_PORT port)    
{
    /* Running to here means tst using IRCOMM port, not supporting TST use IRCOMM so should not run to here */
    ASSERT(KAL_FALSE);
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartPurge
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  dir         [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartPurge(UART_PORT port, UART_buffer dir, module_type ownerid)   
{
    ASSERT(port == uart_port_irda);

    /* Purge UART HW FIFO, IrDA has not FIFO to purge */
    return;
}


/*****************************************************************************
 * FUNCTION
 *  SppUartSetFlowCtrl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  XON         [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartSetFlowCtrl(UART_PORT port, kal_bool XON, module_type ownerid)   
{
    return;
}


/*****************************************************************************
 * FUNCTION
 *  SppUartConfigEscape
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port                [IN]        
 *  EscChar             [IN]        
 *  ESCGuardtime        [IN]        
 *  ownerid             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartConfigEscape(UART_PORT port, kal_uint8 EscChar, kal_uint16 ESCGuardtime, module_type ownerid)
{
    return;

}

/*****************************************************************************
 * FUNCTION
 *  SppUartSetDCBConfig
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  UART_Config     [IN]        
 *  ownerid         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartSetDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config, module_type ownerid)
{
    return;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartSetBaudRate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  baudrate        [IN]        
 *  ownerid         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
/* the following function is not supported */
void SppUartSetBaudRate(UART_PORT port, UART_baudrate baudrate, module_type ownerid)
{
    return;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartSendISRData
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  Buffaddr        [IN]        
 *  Length          [IN]        
 *  mode            [IN]        
 *  escape_char     [IN]        
 *  ownerid         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint16 SppUartSendISRData(
            UART_PORT port,
            kal_uint8 *Buffaddr,
            kal_uint16 Length,
            kal_uint8 mode,
            kal_uint8 escape_char,
            module_type ownerid)
{
    /* For TST L1 trace send data, replace flow-ctrl character in data */
    ASSERT(KAL_FALSE);
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartSendData
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port            [IN]        
 *  Buffaddr        [IN]        
 *  Length          [IN]        
 *  mode            [IN]        
 *  escape_char     [IN]        
 *  ownerid         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint16 SppUartSendData(
            UART_PORT port,
            kal_uint8 *Buffaddr,
            kal_uint16 Length,
            kal_uint8 mode,
            kal_uint8 escape_char,
            module_type ownerid)
{
    /* For TST send data, replace flow-ctrl character in data */
    ASSERT(KAL_FALSE);
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  SppUartPutUARTByte
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  data        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartPutUARTByte(UART_PORT port, kal_uint8 data)
{
    /* For TST dadicate use, when system crash, put uart data by polling */
    ASSERT(KAL_FALSE);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartGetUARTByte
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint8 SppUartGetUARTByte(UART_PORT port)
{
    /* For TST dadicate use, when system crash, get uart data by polling */
    ASSERT(KAL_FALSE);
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartRegister_RX_cb
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 *  func        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartRegister_RX_cb(UART_PORT port, module_type owner, UART_RX_FUNC func)
{
    ASSERT(port == uart_port_irda);

    /* Received data, call AP's callback */
    return;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartRegister_TX_cb
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 *  func        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartRegister_TX_cb(UART_PORT port, module_type owner, UART_TX_FUNC func)
{
    ASSERT(port == uart_port_irda);

    /* Buffer full --> Buffer empty, call AP's callback */
    return;
}

/*****************************************************************************
 * FUNCTION
 *  SppUartSetAutoBaud_Div
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  ownerid     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartSetAutoBaud_Div(UART_PORT port, module_type ownerid)
{
    return;
}


/*****************************************************************************
 * FUNCTION
 *  SppUartPutUARTBytes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  data        [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SppUartPutUARTBytes(UART_PORT port, kal_uint8 *data, kal_uint16 len)
{
    /* For TST dadicate use, when system crash, put uart data by polling */
    ASSERT(KAL_FALSE);
    return;
}


#endif  /* BTMTK_ON_WISE */
#endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */ 

#endif /* __BT_SPP_PROFILE__ */
