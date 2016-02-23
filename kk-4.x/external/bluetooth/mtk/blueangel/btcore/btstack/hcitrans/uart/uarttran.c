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
 *     $Workfile:uarttran.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Code for UART transport driver.
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
#include "bt_common.h"

#include "osapi.h"
#include "uart.h"
#include "sys/uarttran.h"
#include "sys/evmxp.h"
#include "btconfig.h"
#include "hcitrans.h"
#ifdef MTK_BB_TESTING
#include "BtBBtesting.h"
#endif
#include "x_file.h"
#include "host.h"
#include "bt_feature.h"
#include "pal_hci_struct.h"
#include "hci_amp.h"
#include "bt_adp_system.h"

#ifdef ANDROID
#include <hardware_legacy/power.h>
#endif
#if defined(BTMTK_ON_LINUX)
#include <errno.h>
#endif

#ifdef DEMO
    #undef Report
    #define Report(S)
#endif

#define BT_VERIFY_HOST_TIMEOUT_SETTING 4000 // (4000/4.615)
#define BT_FFC0_WAKEUP_TIMEOUT_SETTING 1000 // (1000/4.615)

/* Local functions */
static void uartEventHandler(U8 event);
static U8 mtkuartTransmitData(void);
static void uartReadData(void);
static void uartCommandWatchdog(EvmTimer *timer);
#ifdef __KEEP_HOST_AWAKE__
static void kickHostAwake(void);
static void keepHostAwake(U8 awake);
static void uartAwakeTimeout(EvmTimer *timer);
#endif
static void UARTTRAN_SendData(HciPacket *hciPacket);
static void UARTTRAN_BufferAvailable(HciBufFlags buffType);
static void UARTTRAN_SetSpeed(U16 speed);
static void UARTTRAN_ReSendData(void);
static void UARTTRAN_AMPSendData(HciPacket *hciPacket);

#ifdef __ENABLE_SLEEP_MODE__
static void uartWakeupTimeout(EvmTimer *timer);
#endif
static void verifyHostSleepTimeout(EvmTimer *timer);



#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
static void uartMT66xxRetransmissionStartTimer(void);
static void uartMT66xxRetransmissionPostiveAck(void);
static void uartMT66xxRetransmissionTimeout(EvmTimer *timer);
#endif

/*external functions*/
extern void BTSysAdpHostSleep(U8 sleep);
extern BOOL BTCheckPktServicedTimeLong(U32 int_time, U32 time_value);

/****************************************************************************
 *
 *  RAM Data
 *
 ****************************************************************************/

#if (XA_CONTEXT_PTR == XA_ENABLED)

/* If context pointers are used create a pointer called uaxprt 
 */
UATRAN_Context uaxprtTemp;
UATRAN_Context *uaxprt = &uaxprtTemp;

#elif (XA_CONTEXT_PTR == XA_DISABLED)

/* If context pointers are NOT used create a static object.
 */
UATRAN_Context uaxprt;

#endif


#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
U8 BT_UART_TX_BUFFER[1200];

#endif
/*---------------------------------------------------------------------------
 * UARTTRAN_Init() 
 *
 *     Initialize the UART transport and hardware drivers
 */
BtStatus UARTTRAN_Init(TranCallback tranCallback)
{
    /* 2009-0424: Add for POWER ON/HCI_RESET Retry Mechanism */ 
    /* If the HCI_RESET is failed and called back from the chip, verifyHostSleepTimer maybe still running */
    /* It should remove this timer from BTC(timerList) before resetting uaxprt context : 2009-0424 */
    #ifdef __ENABLE_SLEEP_MODE__
    if(UATRAN(verifyHostSleepTimer).func != 0)
    {
        EVM_ResetTimer(&UATRAN(verifyHostSleepTimer));
    }
    #endif /* __ENABLE_SLEEP_MODE__ */
	
    /* Clear the transport's context memory */
#if (XA_CONTEXT_PTR == XA_ENABLED)
    OS_MemSet((U8 *)uaxprt, 0, sizeof(UATRAN_Context));

#elif (XA_CONTEXT_PTR == XA_DISABLED)
    OS_MemSet((U8 *)&uaxprt, 0, sizeof(UATRAN_Context));
#endif

    UATRAN(callback) = tranCallback;
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    UATRAN(mt66xxTransmissionFlag) = BT_UART_MT66XX_RETRANSMISSION_OFF;
    UATRAN(uartFlushAllRxDataFlag) = BT_UART_MT66XX_FLUSH_DATA_OFF;
    EvmResetTimer(&UATRAN(hciAckTimer));
#endif
    /* Initialize RX/TX queues */
    InitializeListHead(&(UATRAN(txQueue)));
    InitializeListHead(&(UATRAN(txEnroute)));

    /* Initialize rx state */
    UARTTRAN_ResetRxState();

    /* Initialize tx state */
    UATRAN(txState) = TXS_IDLE;

    /* Initialize command watchdog timer */
    UATRAN(watchdog).func = uartCommandWatchdog;
    UATRAN(watchdog).context = 0;
    UATRAN(unackedCommand) = FALSE;
    #ifdef __ENABLE_SLEEP_MODE__
    UATRAN(verifyHostSleepTimer).func = verifyHostSleepTimeout;
    UATRAN(verifyHostSleepTimer).context = 0;
    EVM_StartTimer(&UATRAN(verifyHostSleepTimer), BT_VERIFY_HOST_TIMEOUT_SETTING);
    #endif /* __ENABLE_SLEEP_MODE__ */
    UATRAN(hostSleep) = 0;
    BTSysAdpHostSleep(0);
#ifdef __ENABLE_SLEEP_MODE__
    UATRAN(wakeupControllerTimer).func = uartWakeupTimeout;
    UATRAN(wakeupControllerTimer).context = 0;
    UATRAN(wakeupCounter) = 0;
#endif /* __ENABLE_SLEEP_MODE__ */    
    /* Initialize command watchdog timer */
    UATRAN(txrx) = FALSE;
    UATRAN(txPacketOffset) = 0;
    UATRAN(pendingTxPacket) = 0;
    UATRAN(tranEntry).sendData = UARTTRAN_SendData;
    UATRAN(tranEntry).buffAvail = UARTTRAN_BufferAvailable;
    UATRAN(tranEntry).setSpeed = UARTTRAN_SetSpeed;
    UATRAN(tranEntry).reSendData = UARTTRAN_ReSendData;
#ifdef __BT_3_0_HS__
    UATRAN(tranEntry).sendAMPData = UARTTRAN_AMPSendData;
#else
    UATRAN(tranEntry).sendAMPData = 0;
#endif
#ifdef __ENABLE_SLEEP_MODE__
    UATRAN(wakeupCmdMade) = 0;
#endif /* __ENABLE_SLEEP_MODE__ */
    UATRAN(wakeupFactorMode) = 0;
    UATRAN(uartLastTxRx) = OS_GetSystemTime();
    HCI_RegisterTransport(&UATRAN(tranEntry));
#ifdef BTMTK_ON_WISE 
    if(stack_query_boot_mode() == FACTORY_BOOT)
    {
        UATRAN(wakeupFactorMode) = 0x01;
    }
#endif  /* BTMTK_ON_WISE */

#ifdef __KEEP_HOST_AWAKE__
    UATRAN(awakeTimer).func = NULL;
    UATRAN(awakeTimer).context = NULL;
    UATRAN(awake_wakelock_acquired) = 0;
    UATRAN(awakeTimeout) = (U32)btGetCustvalue(CUSTID_KEEP_AWAKE_TIME);
    OS_Report("set uart awake timeout to %u ms", UATRAN(awakeTimeout));
#endif

    /* Init the serial interface */
    return UART_Init(uartEventHandler);
}


/*---------------------------------------------------------------------------
 * UARTTRAN_Shutdown() 
 *
 *     Shut down the UART transport and hardware drivers
 */
BtStatus UARTTRAN_Shutdown(void)
{
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    if (UATRAN(hciAckTimer).func != 0)
    {
        EVM_CancelTimer(&UATRAN(hciAckTimer));
    }
#endif    	
    if (UATRAN(watchdog).context == (void *)1) {
        EVM_CancelTimer(&UATRAN(watchdog));
    }


#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    UATRAN(mt66xxTransmissionFlag) = BT_UART_MT66XX_RETRANSMISSION_OFF;
#endif

#ifdef __KEEP_HOST_AWAKE__
    if(UATRAN(awakeTimer).func != NULL){
        EVM_CancelTimer(&UATRAN(awakeTimer));
        UATRAN(awakeTimer).func = NULL;
    }
    keepHostAwake(0);
#endif
    return UART_Shutdown();
}


void UARTTRAN_ResetRxState(void)
{
    Report(("UART Rx State Reset"));
    /* Initialize rx state */
    UATRAN(maxReadLen) = 1;
    UATRAN(buffType) = 0;
    UATRAN(ptr) = &(UATRAN(buffType));
    UATRAN(rxState) = RXS_GET_TYPE;
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    UATRAN(uartFlushAllRxDataFlag) = BT_UART_MT66XX_FLUSH_DATA_OFF;
#endif
}


#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
void UARTTRAN_EnableMT66xxFlowControl(void)
{
    UATRAN(mt66xxTransmissionFlag) = BT_UART_MT66XX_RETRANSMISSION_ON;
}

#endif

#ifdef __ENABLE_SLEEP_MODE__
void StartVerifyHostSleepTimer()
{
    /* Start x seconds verify system shall sleep timer */
    EVM_ResetTimer(&UATRAN(verifyHostSleepTimer));
    UATRAN(verifyHostSleepTimer).func = verifyHostSleepTimeout;                
    EVM_StartTimer(&UATRAN(verifyHostSleepTimer), BT_VERIFY_HOST_TIMEOUT_SETTING);
}
#endif /* __ENABLE_SLEEP_MODE__ */

#ifdef __BT_3_0_HS__
U32 tx_timer_tick = 0;
extern U8 HciSentAMPCommand(
                        msg_type msg,
                        local_para_struct *local_para,
                        U32 msg_len,
                        U8 controllerId);

pal_ilm_struct dlight_ilm;
U16 dlight_sequence_no = 0;
U8 UARTRAN_AMP_ForceTxDataTest()
{
    U16 bytesWritten;
    U16 length=0;
    U16 writeLength = 0;
	U8 *buffer;
    U8 txFrag = 0;
    ilm_struct *ilm_ptr = NULL;
    BtPacket *pkt;
    U16 hciHandler;
    U16 pdu_len;
    U8 result;
    U8 total_no;
    pal_bt_data_command_struct *bt_data_command	= NULL;
    
    hciHandler = 0x100;
    OS_MemSet(&dlight_ilm, 0, sizeof(pal_ilm_struct));
    bt_data_command = (pal_bt_data_command_struct *) &dlight_ilm.ilm_data;
    bt_data_command->ref_hdr.ref_count = 1;
    bt_data_command->ref_hdr.msg_len= sizeof(pal_bt_data_command_struct);
    UATRAN(ampTxData).ptr = (void *)bt_data_command;
    bt_data_command->u2Handle = 0x100;
    bt_data_command->u2Data_total_len = 1478;
    bt_data_command->aucData_p[0] =     (U8) ((dlight_sequence_no & 0xff00) >> 8);
    bt_data_command->aucData_p[1] =     (U8) (dlight_sequence_no & 0x00ff);
    dlight_sequence_no++;
    bt_data_command->aucData_p1[0] =    (U8) ((dlight_sequence_no & 0xff00) >> 8);
    bt_data_command->aucData_p1[1] =     (U8) (dlight_sequence_no & 0x00ff);
    dlight_sequence_no++;
    bt_data_command->aucData_p2[0] =     (U8) ((dlight_sequence_no & 0xff00) >> 8);
    bt_data_command->aucData_p2[1] =     (U8) (dlight_sequence_no & 0x00ff);
    dlight_sequence_no++;
    bt_data_command->aucData_p3[0] =     (U8) ((dlight_sequence_no & 0xff00) >> 8);
    bt_data_command->aucData_p3[1] =     (U8) (dlight_sequence_no & 0x00ff);
    dlight_sequence_no++;

    bt_data_command->u2Data_total_len1 = 1478;
    bt_data_command->u2Data_total_len2 = 1478;
    bt_data_command->u2Data_total_len3 = 1478;
    bt_data_command->ucPb_flag          = 0x03;
    bt_data_command->ucBc_flag          = 0x00;
    bt_data_command->data_count = 4;
    result = BT_SendPalMessage(MSG_ID_PAL_BT_DATA_COMMAND, &dlight_ilm, sizeof(pal_bt_data_command_struct));    
    return result;


}

U8 UARTRAN_AMP_ForceTxData()
{
    U8 result;
    pal_bt_data_command_struct *bt_data_command	= NULL;

    OS_StopHardware();
    if(UATRAN(ampTxData).total_number != 0x00)
    {
        bt_data_command = (pal_bt_data_command_struct *) UATRAN(ampTxData).ptr;
        bt_data_command->ucPb_flag          = 0x03;
        bt_data_command->ucBc_flag          = 0x00;
        bt_data_command->data_count = UATRAN(ampTxData).total_number;

//        result = HciSentAMPCommand(MSG_ID_PAL_BT_DATA_COMMAND, (local_para_struct *)bt_data_command, BT_PAL_CONTROLLERID);

        result = BT_SendPalMessage(MSG_ID_PAL_BT_DATA_COMMAND, &dlight_ilm, sizeof(pal_bt_data_command_struct));

        BTCoreGetSystemTick1();

        if(result != 0) // enqueue 0 is success
        {
            return 0x01;
        }
        else
        {
            UATRAN(ampTxData).total_number = 0;
            UATRAN(ampTxData).ptr = NULL;
            return 0x00;
        }
    }
    return 0x00;
}




static void UARTTRAN_AMPSendData(HciPacket *hciPacket)
{
    U16 bytesWritten;
    U16 length=0;
    U16 writeLength = 0;
	U8 *buffer;
    U8 txFrag = 0;
    BtPacket *pkt;
    U16 hciHandler;
    U16 pdu_len;
    U8 result;

    pal_bt_data_command_struct *bt_data_command	= NULL;
    
    hciHandler = (LEtoHost16(hciPacket->header) & 0x0fff);
    OS_MemSet(&dlight_ilm, 0, sizeof(pal_ilm_struct));
    bt_data_command = (pal_bt_data_command_struct *) &dlight_ilm.ilm_data;
    bt_data_command->ref_hdr.ref_count = 1;
    bt_data_command->ref_hdr.msg_len= sizeof(pal_bt_data_command_struct);
    bt_data_command->ucPb_flag          = 0x03;
    bt_data_command->ucBc_flag          = 0x00;    
    bt_data_command->data_count = 1;
    bt_data_command->u2Handle = hciHandler;
    buffer = bt_data_command->aucData_p;
    UATRAN(ampTxData).ptr = (void *)bt_data_command;
    length = 0;
    txFrag=0;
    while(txFrag < hciPacket->fCount)
    {
        OS_MemCopy(buffer+length, hciPacket->f[txFrag].buffer, hciPacket->f[txFrag].len);
        length += hciPacket->f[txFrag].len;
        txFrag++;
    }    


    if(hciPacket->txCounter != 0x00)
    {
        if(hciPacket->txCounter[0] == 0x00)
        {
            Report(("Return and ignore write to UART"));
            /* Packet shall be ignored when the remote ack the local device*/
            HCI_PacketSent(hciPacket);
            HCI_ReturnAMPWifiOneNoneSentACL(hciPacket->hciConEntryindex);
            return;
        }
    }
    HciAMPControllerTxReduce(1, length, hciHandler);
    bt_data_command->u2Data_total_len = LEtoHost16(hciPacket->header+2);
}
#endif

/*---------------------------------------------------------------------------
 * UARTTRAN_SendData() 
 *      Send a packet to the BT host via a UART interface.
 *      This function is called from HCI_Process, part of the EVM_Process or
 *      stack thread.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 *      BT_STATUS_PENDING
 */
static void UARTTRAN_SendData(HciPacket *hciPacket)
{
    /* Check the type of the packet */
    U16 opcode_host_number_pkt = 0;
    //bt_prompt_trace(MOD_BT, "[BT] +UARTTRAN_SendData");
    if((hciPacket->flags & HCI_BUFTYPE_COMMAND) == HCI_BUFTYPE_COMMAND)
    {
        opcode_host_number_pkt = ((HCC_HOST_NUM_COMPLETED_PACKETS & 0x00FF) ^ BT_XFileGetKey0()) | ((((HCC_HOST_NUM_COMPLETED_PACKETS & 0xFF00)>>8)^ BT_XFileGetKey1()) << 8);
        if(LEtoHost16(hciPacket->header) != opcode_host_number_pkt) 
        {
            /* If it's a command, start the watchdog timer. */
            OS_StopHardware();
            UATRAN(unackedCommand) = TRUE;
            UATRAN(watchdog).context = (void *)1;
            OS_ResumeHardware();
            EVM_StartTimer(&UATRAN(watchdog), UTP_UART_EVENT_WAIT_TIME);
        }
    }
    /* Because we're manipulating sensitive data structures, we must
     * request that the hardware thread/interrupt does not execute.
     */
    OS_StopHardware();
    InsertTailList(&UATRAN(txQueue), (ListEntry *)&hciPacket->node);
    /* Keep host to awake */
#ifdef __KEEP_HOST_AWAKE__
    kickHostAwake();
#endif
    if(UATRAN(pendingTxPacket) ==0)
    {
        if (IsListEmpty(&UATRAN(txQueue))) 
        {
            UATRAN(txPacket) = 0;
        } 
        else 
        {
            if(UATRAN(txrx) == FALSE)
            {
                UATRAN(txrx) = TRUE;
            }
            do
            {
                if( mtkuartTransmitData() == 1)
                {
                    /* send wakeup command or under wakeup procedure */
#ifndef __ENABLE_SLEEP_MODE__
                    /* No wakeup cmd is sent in MT6616 */ 
                    Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */
                    break;
                }
                if (IsListEmpty(&UATRAN(txQueue)))
                {
                    break;
                }

            }while(UATRAN(pendingTxPacket) ==0);
        }
    }
    OS_ResumeHardware();
    //bt_prompt_trace(MOD_BT, "[BT] -UARTTRAN_SendData");
}

static void UARTTRAN_ReSendData(void)
{

#ifdef __ENABLE_SLEEP_MODE__
    if(UATRAN(wakeupCmdMade) == 1)
    {
        return;
    }
#endif /* __ENABLE_SLEEP_MODE__ */

    if(UATRAN(pendingTxPacket) !=0)
    {
        /* The AMP controller is not possible to retransmitted */
        do
        {
            if((UATRAN(pendingTxPacket) == 0) &&
                (!IsListEmpty(&UATRAN(txQueue))))
            {
                if( mtkuartTransmitData() == 1)
                {
                    /* send wakeup command or under wakeup procedure */
#ifndef __ENABLE_SLEEP_MODE__
                    /* No wakeup cmd is sent in MT6616 */ 
                    Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */                    
                    break;
                }
            }
            else
            {
                break;
            }
        }while(UATRAN(pendingTxPacket) ==0);                  
    }
    else
    {

        if (!IsListEmpty(&UATRAN(txQueue))) 
        {
    		if(UATRAN(txrx) == FALSE)
    		{
    			UATRAN(txrx) = TRUE;
    		}
            do
            {
                if( mtkuartTransmitData() == 1)
                {
                    /* send wakeup command or under wakeup procedure */
#ifndef __ENABLE_SLEEP_MODE__
                    /* No wakeup cmd is sent in MT6616 */ 
                    Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */                    
                    break;
                }
                if (IsListEmpty(&UATRAN(txQueue)))
                {
                    break;
                }
            }while(UATRAN(pendingTxPacket) ==0);                
        }
    }
}


/*---------------------------------------------------------------------------
 * UARTTRAN_BufferAvailable() 
 *
 *     A receive buffer is avialable
 */
static void UARTTRAN_BufferAvailable(HciBufFlags buffType)
{
    //UNUSED_PARAMETER(buffType);
    if (UATRAN(rxState) == RXS_GET_BUFFER) {
        /* Restart the read */
        uartReadData();
    }
}

/*---------------------------------------------------------------------------
 * UARTTRAN_SetSpeed() 
 *
 *     Set the UART baud rate
 */
static void UARTTRAN_SetSpeed(U16 speed)
{
    UART_SetSpeed(speed);
}

void uartExpiredImmediately(void)
{
#ifdef __ENABLE_SLEEP_MODE__ 
    /* Force to wakeup*/
    U8 uart_buffer[2];
    U16 bytesWritten;
#endif /* __ENABLE_SLEEP_MODE__ */ 
    if(UATRAN(pendingTxPacket) !=0)
    {
        return;
    }
    BTSysAdpHostSleep(0);
    if(UATRAN(wakeupFactorMode) == 0)
    {
        EVM_ResetTimer(&UATRAN(verifyHostSleepTimer));
        UATRAN(verifyHostSleepTimer).func = verifyHostSleepTimeout;
        EVM_StartTimer(&UATRAN(verifyHostSleepTimer), BT_VERIFY_HOST_TIMEOUT_SETTING);
#ifdef __ENABLE_SLEEP_MODE__        
        Report(("Try to call chip wakeup 1"));
        EVM_ResetTimer(&UATRAN(wakeupControllerTimer));
        UATRAN(wakeupControllerTimer).func = uartWakeupTimeout;
        EVM_StartTimer(&UATRAN(wakeupControllerTimer), BT_FFC0_WAKEUP_TIMEOUT_SETTING);

#if defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__)
                {
                    void GORM_ARM9_wakeup_ARM7(void);
                    GORM_ARM9_wakeup_ARM7();
                }
#endif // #if defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__)
        
        uart_buffer[0] = 0xFF;
        bytesWritten = (U16) UART_Write(uart_buffer, 1);
        UATRAN(wakeupCounter)++;
        if(bytesWritten == 1)
        {
            UATRAN(wakeupCmdMade) = 1;
        }
#endif /* __ENABLE_SLEEP_MODE__ */        
    }
}

/*---------------------------------------------------------------------------
 * uartEventHandler()
 *
 *     Called from UART hardware driver to indicate rx/tx events.
 */
static void uartEventHandler(U8 event)
{
    switch (event) {
    case UE_WRITE_COMPLETE:
        if(UATRAN(pendingTxPacket) !=0)
        {
            /* AMP controller is not required to handle the fragment sent. */
            if(mtkuartTransmitData() == 1)
            {
                /* send wakeup command or under wakeup procedure */
#ifndef __ENABLE_SLEEP_MODE__
                /* No wakeup cmd is sent in MT6616 */ 
                Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */                    
                return;
            }
            do
            {
                if((UATRAN(pendingTxPacket) == 0) &&
                    (!IsListEmpty(&UATRAN(txQueue))))
                {
                    if( mtkuartTransmitData() == 1)
                    {
                        /* send wakeup command or under wakeup procedure */
#ifndef __ENABLE_SLEEP_MODE__
                        /* No wakeup cmd is sent in MT6616 */ 
                        Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */   
                        break;
                    }
                }
                else
                {
                    break;
                }
            }while(UATRAN(pendingTxPacket) ==0);                  
        }
        else
        {
            if (IsListEmpty(&UATRAN(txQueue))) 
            {
                UATRAN(txPacket) = 0;
            } 
            else 
            {
        		if(UATRAN(txrx) == FALSE)
        		{
        			UATRAN(txrx) = TRUE;
        		}
                do
                {
                    if( mtkuartTransmitData() == 1)
                    {
                        /* send wakeup command or under wakeup procedure */
#ifndef __ENABLE_SLEEP_MODE__
                        /* No wakeup cmd is sent in MT6616 */ 
                        Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */                          
                        break;
                    }
                    if (IsListEmpty(&UATRAN(txQueue)))
                    {
                        break;
                    }
                }while(UATRAN(pendingTxPacket) ==0);                
            }
        }
        break;
    case UE_DATA_TO_READ:
        uartReadData();
        break;
    case UE_INIT_COMPLETE:
        UATRAN(callback)(TRAN_INIT_STATUS, BT_STATUS_SUCCESS);
        break;
    case UE_SHUTDOWN_COMPLETE:
        UATRAN(callback)(TRAN_SHUTDOWN_STATUS, BT_STATUS_SUCCESS);
        break;
    default:
        /* Invalid event received */
        Assert(FALSE);
        break;
    }        
}

/*---------------------------------------------------------------------------
 * uartCommandWatchdog()
 *     This function is called UART_EVENT_WAIT_TIME seconds after the last 
 *     command issued.
 */
static void uartCommandWatchdog(EvmTimer *timer)
{
    //UNUSED_PARAMETER(timer);
bt_prompt_trace(MOD_BT, "[BT] uartCommandWatchdog");
    if (UATRAN(unackedCommand)) {
        bt_trace(TRACE_GROUP_1,UATRAN_HCI_COMMAND_WAS_NOT_ACKNOWLEDGED_WITH_AN_EVENT);
        bt_android_log("[BT] UATRAN: HCI Command was not acknowledged with an event");
        HciTransportError1(BT_PANIC_HCI_CMD_NO_ACK);
        UATRAN(unackedCommand) = FALSE;
        UATRAN(watchdog).context = 0;
    }
}

#ifdef __KEEP_HOST_AWAKE__
static void kickHostAwake(void){
    if(UATRAN(awakeTimer).func == NULL && UATRAN(awakeTimeout)){
        keepHostAwake(1);
        UATRAN(awakeTimer).func = uartAwakeTimeout;        
        EVM_StartTimer(&UATRAN(awakeTimer), UATRAN(awakeTimeout));
    }
}

static void keepHostAwake(U8 awake){
    int ret;
    if(awake){
        if(!UATRAN(awake_wakelock_acquired)){
            UATRAN(awake_wakelock_acquired)= 1;
            OS_Report("Acquire uart awake wakelock");
            #ifdef ANDROID
            do{
                ret = acquire_wake_lock(PARTIAL_WAKE_LOCK, "btawake");
                if(ret <= 0){
                    OS_Report("acquire_wake_lock failed. ret=%d, %s, errno=%d", ret, strerror(errno), errno);
                }
            }while(ret < 0 && errno == EINTR);
            #endif
        }
    }else{
        if(UATRAN(awake_wakelock_acquired)){
            UATRAN(awake_wakelock_acquired) = 0;
            OS_Report("Release uart awake wakelock");
            #ifdef ANDROID
            ret = release_wake_lock("btawake");
            if(!ret){
                OS_Report("release_wake_lock failed. %s, errno=%d", strerror(errno), errno);
            }
            #endif
        }
    }
}

static void uartAwakeTimeout(EvmTimer *timer){
    UATRAN(awakeTimer).func = NULL;
    if(BTCheckPktServicedTimeLong(UATRAN(uartLastTxRx), MS_TO_TICKS(UATRAN(awakeTimeout)))){
        keepHostAwake(0);
    }else{
        kickHostAwake();
    }
}
#endif
/*---------------------------------------------------------------------------
 * uartReadData() 
 *      Called from lower layer when data is available for reading.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static void uartReadData(void)
{
    U16 readLen = 0; /* Length of data we most recently read from UART */
    BtStatus status;
    HciHandle hciConnHandle;
    U16 len;
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    U8 tempReadBuffer[300];
#endif

    if(UATRAN(txrx) == FALSE)
    {
        UATRAN(txrx) = TRUE;
    }
	
    if(UATRAN(hostSleep) == 1)
    {
        BTSysAdpHostSleep(0);
        UATRAN(hostSleep) = 0;
    }
	
    do {
        if (UATRAN(rxState) != RXS_GET_BUFFER) {
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
            /* Read available data */
            if(UATRAN(uartFlushAllRxDataFlag) == BT_UART_MT66XX_FLUSH_DATA_OFF)
            {
                readLen = UART_Read(UATRAN(ptr), UATRAN(maxReadLen));
            }
            else
            {
                /* Read and flush all data buffer in the UART driver. */
                do
                {
                    readLen = UART_Read(tempReadBuffer, 300);
                }
                while (readLen !=0);
                UATRAN(uartLastTxRx) = OS_GetSystemTime();
                return;
            }
#else
            readLen = UART_Read(UATRAN(ptr), UATRAN(maxReadLen));
#endif
        #ifdef UART_DUMP
//            bt_trace(TRACE_GROUP_1, BTLOG_UART_READ, UATRAN(maxReadLen), readLen);
        #endif

            //Assert(readLen >= 0);
            UATRAN(uartLastTxRx) = OS_GetSystemTime();
            UATRAN(ptr) += readLen;
#ifdef __KEEP_HOST_AWAKE__            
            kickHostAwake();
#endif
            /* See if enough data is available */
            if (readLen != UATRAN(maxReadLen)) {
                UATRAN(maxReadLen) -= readLen;
                return;
            }
        }

        switch (UATRAN(rxState)) {
        
        case RXS_GET_TYPE:
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
            /* Start the hciAck Timer */
            uartMT66xxRetransmissionStartTimer();
            UATRAN(rxBuffHandle) = 0;
#endif
            /* Get the type of buffer */
            if (UATRAN(buffType) == IND_EVENT) { /* Event */
                UATRAN(headerLen) = 2;
#if NUM_SCO_CONNS != 0
            } else if (UATRAN(buffType) == IND_SCO_DATA) {
                UATRAN(headerLen) = 3;
#endif /* NUM_SCO_CONNS != 0 */
            } else if (UATRAN(buffType) == IND_ACL_DATA) {
                UATRAN(headerLen) = 4;
            } else {
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
                /* The hciAck Timer timeout right now*/
                Report(("Receive unknown Type:%02x",UATRAN(buffType)));
                UATRAN(uartFlushAllRxDataFlag) = BT_UART_MT66XX_FLUSH_DATA_ON;
                UATRAN(maxReadLen) = 1;
                UATRAN(rxState) = RXS_GET_TYPE;
                do
                {
                    readLen = UART_Read(tempReadBuffer, 300);
                }
                while (readLen !=0);
                return;                
#else
//              Assert(0);
                HciTransportError1(BT_PANIC_INVALID_HCI_BUF_TYPE);
#endif
            } 

            /* Set up for the next state */
            UATRAN(ptr) = UATRAN(header);
            UATRAN(maxReadLen) = UATRAN(headerLen);
            UATRAN(rxState) = RXS_GET_HEADER;
            break;

        case RXS_GET_HEADER:

            /* Header info has been read */         
            UATRAN(rxState) = RXS_GET_BUFFER;

            /* Drop through to next state */

        case RXS_GET_BUFFER:

            /* Get an available buffer */
            switch (UATRAN(buffType)) {
            
            case IND_ACL_DATA:  /* ACL */
                len = (U16)(LEtoHost16(&(UATRAN(header)[2])) + UATRAN(headerLen));
                hciConnHandle = LEtoHost16(&(UATRAN(header)[0]));
                status = HCI_GetAclBuffer(&UATRAN(rxBuffHandle), 
                                          hciConnHandle, len);
                if (status == BT_STATUS_SUCCESS) {
                    /* Got an ACL buffer */
                    UATRAN(ptr) = HCI_GetRxPtr(UATRAN(rxBuffHandle));
                    UATRAN(maxReadLen) = LEtoHost16(&(UATRAN(header)[2]));


                } else if (status == BT_STATUS_NO_RESOURCES) {
                    /* No receive buffer available */
                    return;
                } else {
                    HciTransportError1(BT_PANIC_GET_ACL_BUF_FAILED);
                    return;
                }
                break;
#if NUM_SCO_CONNS != 0
            case IND_SCO_DATA:  /* SCO */
                len = (U16)(UATRAN(header)[2] + UATRAN(headerLen));
                hciConnHandle = LEtoHost16(&(UATRAN(header)[0]));
                status = HCI_GetScoBuffer(&UATRAN(rxBuffHandle), 
                                          hciConnHandle, len);
                if (status == BT_STATUS_SUCCESS) {
                    /* Got an SCO buffer */
                    UATRAN(ptr) = HCI_GetRxPtr(UATRAN(rxBuffHandle));
                    Assert(UATRAN(ptr));
                    UATRAN(maxReadLen) = UATRAN(header)[2];

                } else if (status == BT_STATUS_NO_RESOURCES) {
                    /* No receive buffer available */
                    return;
                } else {
                    HciTransportError1(BT_PANIC_GET_SCO_BUF_FAILED);
                    return;
                }
                break;
#endif /* NUM_SCO_CONNS != 0 */
            case IND_EVENT:  /* Event */
                len = (U16)(UATRAN(header)[1] + UATRAN(headerLen));
                status = HCI_GetEventBuffer(&UATRAN(rxBuffHandle), len);
                if (status == BT_STATUS_SUCCESS) {
                    /* Got an Event buffer */
                    UATRAN(ptr) = HCI_GetRxPtr(UATRAN(rxBuffHandle));
                    UATRAN(maxReadLen) = UATRAN(header)[1];


                } else if (status == BT_STATUS_NO_RESOURCES) {
                    /* No receive buffer available */

                    return;
                } else {
                    HciTransportError1(BT_PANIC_GET_EVENT_BUF_FAILED);
                    return;
                }
                break;
            default:
                /* Invalid framing */
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
                UATRAN(uartFlushAllRxDataFlag) = BT_UART_MT66XX_FLUSH_DATA_ON;
                Report(("Invalid framing"));
#else                
                HciTransportError1(BT_PANIC_INVALID_HCI_FRAME);
#endif
                return;
            }

            OS_MemCopy(UATRAN(ptr), UATRAN(header), UATRAN(headerLen));

            /* Set up for next state */
            UATRAN(ptr) += UATRAN(headerLen);
            UATRAN(rxState) = RXS_GET_DATA;
            readLen = 1; /* Force loop to re-execute */
            break;

        case RXS_GET_DATA:
            /* Pass it up */

            /* If this is an event then consider all commands ack'ed */
            if (UATRAN(rxBuffHandle)->flags & HCI_BUFTYPE_EVENT) {
                UATRAN(unackedCommand) = FALSE;  
            }
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
            if(UATRAN(mt66xxTransmissionFlag) == BT_UART_MT66XX_RETRANSMISSION_ON)
            {
                /* The flow control sets to ON, shall response the ack to Controller & Stop timer. */
                uartMT66xxRetransmissionPostiveAck();
            }
#endif
            /* +VSNIFF */
            if(p_sniffread){
                p_sniffread(UATRAN(buffType), 
                                 HCI_GetRxPtr(UATRAN(rxBuffHandle)), 
                                 UATRAN(rxBuffHandle)->len);
            }
            /* -VSNIFF */

            HCI_RxBufferInd(UATRAN(rxBuffHandle), 
                            BT_STATUS_SUCCESS);

            /* Reset to first state */
            UATRAN(buffType) = 0;
            UATRAN(ptr) = &(UATRAN(buffType));
            UATRAN(maxReadLen) = 1;
            UATRAN(rxState) = RXS_GET_TYPE;
            break;
        }   
    } while (readLen);
}

#ifdef __ENABLE_SLEEP_MODE__
void BTUartRadioWakeupCompleted(void)
{
    //bt_prompt_trace(MOD_BT, "[Radio] BTUartRadioWakeupCompleted");
    UATRAN(wakeupCmdMade) = 0;
    UATRAN(wakeupCounter) = 0;    
    UATRAN(uartLastTxRx) = OS_GetSystemTime();
    EVM_ResetTimer(&UATRAN(wakeupControllerTimer));
}
#endif /* __ENABLE_SLEEP_MODE__ */

void BTUartSetCurrentFacotrMode(U8 mode)
{
    if(mode ==1)
    {
        UATRAN(wakeupFactorMode) = 0x01;
    }
    else
    {
        UATRAN(wakeupFactorMode) = 0x00;
    }

}

#ifdef __ENABLE_SLEEP_MODE__
static void uartWakeupTimeout(EvmTimer *timer)
{
bt_prompt_trace(MOD_BT, "[BT] uartWakeupTimeout");
    if(UATRAN(wakeupCounter) ==5)
    {
        HciTransportError1(BT_PANIC_UART_WAKEUP_TIMEOUT);
    }
    else
    {
        Report(("uartWakeupTimeout1"));
        EVM_ResetTimer(&UATRAN(wakeupControllerTimer));
        uartExpiredImmediately();
    }
}
#endif /* __ENABLE_SLEEP_MODE__ */

static void verifyHostSleepTimeout(EvmTimer *timer)
{
    OS_Report(("[SM] verifyHostSleepTimeout"));
    EVM_ResetTimer(&UATRAN(verifyHostSleepTimer));
    if(UATRAN(wakeupFactorMode) == 0)
    {
    	#ifndef __ENABLE_SLEEP_MODE__
	if(0)
	#else
        if (BTCheckPktServicedTimeLong(UATRAN(uartLastTxRx), (U32)(MS_TO_TICKS(BT_VERIFY_HOST_TIMEOUT_SETTING))))
	#endif
        {
            Report(("[SM] It can enter sleep"));
            UATRAN(hostSleep) = 1;
            BTSysAdpHostSleep(1);
        }
        else
        {
	     #ifdef __ENABLE_SLEEP_MODE__
            Report(("[SM] It will not sleep"));
            UATRAN(hostSleep) = 0;
            UATRAN(verifyHostSleepTimer).func = verifyHostSleepTimeout;
            EVM_StartTimer(&UATRAN(verifyHostSleepTimer), BT_VERIFY_HOST_TIMEOUT_SETTING);
	     #endif
        }
    }
}

static U8 mtkuartTransmitData(void)
{
    U16 bytesWritten;
    U8 uart_buffer[1050];
    U16 length=0;
    U16 opcode = 0;
    U16 writeLength = 0;
    U16 writeOffset = 0;
    BOOL wakeup_cmd = FALSE;

    if(UATRAN(pendingTxPacket) ==0)
    {
#ifdef __ENABLE_SLEEP_MODE__ 
        if(UATRAN(wakeupCmdMade) == 1)
        {
            bt_prompt_trace(MOD_BT,"[SM] wakeupCmdMade is already sent");
            return 1;
        }
        else if(
              #ifndef __ENABLE_SLEEP_MODE__
	       /* Prevent send wakeup cmd */
              (UATRAN(hostSleep) == 1)  
              #else
              ((UATRAN(hostSleep) == 1) || (BTCheckPktServicedTimeLong(UATRAN(uartLastTxRx), /*650*/MS_TO_TICKS(3000)))) 
            #endif
            && (UATRAN(wakeupFactorMode) == 0))
#else
        if(
              #ifndef __ENABLE_SLEEP_MODE__
	       /* Prevent send wakeup cmd */
              (UATRAN(hostSleep) == 1)  
              #else
              ((UATRAN(hostSleep) == 1) || (BTCheckPktServicedTimeLong(UATRAN(uartLastTxRx), /*650*/MS_TO_TICKS(3000)))) 
            #endif
            && (UATRAN(wakeupFactorMode) == 0))
#endif /* __ENABLE_SLEEP_MODE__ */
        {
            bt_prompt_trace(MOD_BT, "[SM] Stack try to call wakeup chip : hostSleep=%d", UATRAN(hostSleep));
#ifdef __ENABLE_SLEEP_MODE__        
            EVM_ResetTimer(&UATRAN(wakeupControllerTimer));
            UATRAN(wakeupControllerTimer).func = uartWakeupTimeout;
            EVM_StartTimer(&UATRAN(wakeupControllerTimer), BT_FFC0_WAKEUP_TIMEOUT_SETTING);
            /* Start 1 seconds 0xFF retry timer */
#endif /* __ENABLE_SLEEP_MODE__ */      
 
            if(UATRAN(hostSleep) == 1)
            {
                /* Start 4 seconds verify system shall sleep timer */
                bt_prompt_trace(MOD_BT, "[SM] start verifyHostSleepTimer");
                EVM_ResetTimer(&UATRAN(verifyHostSleepTimer));
                UATRAN(verifyHostSleepTimer).func = verifyHostSleepTimeout;
                EVM_StartTimer(&UATRAN(verifyHostSleepTimer), BT_VERIFY_HOST_TIMEOUT_SETTING);
            }
            UATRAN(hostSleep) = 0;
            BTSysAdpHostSleep(0);
            
#ifdef __ENABLE_SLEEP_MODE__ 
            Report(("[SM] Try to call chip wakeup 2"));

#if defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__)
                    {
                        void GORM_ARM9_wakeup_ARM7(void);
                        GORM_ARM9_wakeup_ARM7();
                    }
#endif // #if defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__)

            uart_buffer[0] = 0xFF;
            bytesWritten = (U16) UART_Write(uart_buffer, 1);
            UATRAN(wakeupCounter)++;
            if(bytesWritten == 1)
            {
                UATRAN(wakeupCmdMade) = 1;
                return 1;
            }
#else
            /* For MT6616: BT Host can send ptk data directly without sending 0xFF to the Controller first */
            UATRAN(txPacket) = (HciPacket *)RemoveHeadList(&UATRAN(txQueue));
			
#endif /* __ENABLE_SLEEP_MODE__ */
                    
        }
        else
        {
            UATRAN(txPacket) = (HciPacket *)RemoveHeadList(&UATRAN(txQueue));
        }
    }
    else
    {
        /* If there are pending tx packet, the controller shall not in sleep mode. */
        UATRAN(txPacket) = UATRAN(pendingTxPacket);
    }
    UATRAN(uartLastTxRx) = OS_GetSystemTime();    

    uart_buffer[1] = UATRAN(txPacket)->header[0];
    uart_buffer[2] = UATRAN(txPacket)->header[1];
    uart_buffer[3] = UATRAN(txPacket)->header[2];
    if (UATRAN(txPacket)->flags & HCI_BUFTYPE_ACL_DATA) 
    {
        uart_buffer[0] = 2;
        uart_buffer[4] = UATRAN(txPacket)->header[3];
        length+=5;
#ifdef UART_DUMP
        bt_trace(TRACE_GROUP_1, BTLOG_SEND_UART_ACL_DATA, uart_buffer[1] & 0xFF, uart_buffer[2] & 0x0F, uart_buffer[2] & 0x30, LEtoHost16(uart_buffer + 3));
#endif
    } 
    else if (UATRAN(txPacket)->flags & HCI_BUFTYPE_COMMAND) 
    {
        uart_buffer[0] = 1;
        length+= 4;
        opcode = ((U16)uart_buffer[2]) << 8 | (uart_buffer[1]);
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        UATRAN(txPacket)->txCounter = 0x00;
#endif
        if (BT_XFile_DeOpcodeOpcode(uart_buffer[1],uart_buffer[2]) == 0xFCC0)
        {
            wakeup_cmd = TRUE;
#ifndef __ENABLE_SLEEP_MODE__
            Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */
        }        
    }
#if NUM_SCO_CONNS != 0
    else if (UATRAN(txPacket)->flags & HCI_BUFTYPE_SCO_DATA) 
    {
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        UATRAN(txPacket)->txCounter = 0x00;
#endif
        uart_buffer[0] = 3;
        length+= 4;            
    } 
#endif /* NUM_SCO_CONNS != 0 */
    else 
    {
        Assert(0);
    }
    UATRAN(txFrag)=0;
    while(UATRAN(txFrag) < UATRAN(txPacket)->fCount)
    {
        OS_MemCopy(uart_buffer+length, UATRAN(txPacket)->f[UATRAN(txFrag)].buffer, UATRAN(txPacket)->f[UATRAN(txFrag)].len);
        length += UATRAN(txPacket)->f[UATRAN(txFrag)].len;
        UATRAN(txFrag)++;
    }    
    if (wakeup_cmd)
    {
#ifdef __ENABLE_SLEEP_MODE__    
        uart_buffer[0] = 0xFF;
        length = 1;				
#else
        Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */
    }      
#ifndef __BT_STACK_UT__
    if(UATRAN(pendingTxPacket) !=0)
    {
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        if((!wakeup_cmd) && ((UATRAN(txPacket)->txCounter) != 0x00))
        {
            /* Some data is sent into UART. We can not stop it. */
            memcpy(uart_buffer, BT_UART_TX_BUFFER, length);
        }
#endif
        writeLength = length - UATRAN(txPacketOffset);
        writeOffset = UATRAN(txPacketOffset);
    }
    else
    {
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        if((!wakeup_cmd) && ((UATRAN(txPacket)->txCounter) != 0x00))
        {
            memcpy(BT_UART_TX_BUFFER, uart_buffer, length);
            /* Retransmitted packet */
             if(UATRAN(txPacket)->txCounter[0] == 0x00)
            {
                Report(("Return and ignore write to UART"));
                /* Packet shall be ignored when the remote ack the local device*/
                UATRAN(txPacketOffset) = 0;
                UATRAN(pendingTxPacket)= 0;        
                HCI_PacketSent(UATRAN(txPacket));
                HCI_ReturnOneNoneSentACL(UATRAN(txPacket)->hciConEntryindex);
                UATRAN(txPacket) = 0;
                return 0;
            }
        }

#endif
        writeOffset = 0;
        writeLength = length;
    }
    bytesWritten = (U16) UART_Write(uart_buffer+writeOffset, writeLength);
    if(bytesWritten == writeLength)
    {
        UATRAN(txPacketOffset) = 0;
        UATRAN(pendingTxPacket)= 0;        
        HCI_PacketSent(UATRAN(txPacket));
    }
    else
    {
        bt_trace(TRACE_GROUP_1, BTLOG_UART_TRANSMIT_DATA, writeLength, bytesWritten);   
        UATRAN(txPacketOffset) +=  bytesWritten;   
        UATRAN(pendingTxPacket)= UATRAN(txPacket);
    }
    if(bytesWritten > 0)
    {
        UATRAN(uartLastTxRx) = OS_GetSystemTime();
    }
#else
    BTSingleChipVrtUart_Write(length, uart_buffer);
#endif
    UATRAN(txPacket) = 0;   
    return 0;

}


#ifdef __BT_MT66XX_HCI_RETRANSMISSION__

void uartMT66xxRetransmissionAck(U8 type)
{
    U8 ack_buffer[4] = {0x01, 0x00, 0xFC, 0x00};

    ack_buffer[2] = 0xFC ^ BT_XFileGetKey1();
    if(type == 0x01)
    {
        /* Postive ACK */
        Report(("P ack"));
        ack_buffer[1] = 0xBD ^ BT_XFileGetKey0();
    }
    else
    {
        /* Negative ACK */
        Report(("N ack"));
        ack_buffer[1] = 0xBE ^ BT_XFileGetKey0();
    }
    UART_Write(ack_buffer, 4);
}

static void uartMT66xxRetransmissionStartTimer(void)
{
    EVM_ResetTimer(&UATRAN(hciAckTimer));
    UATRAN(hciAckTimer).func = uartMT66xxRetransmissionTimeout;
    EVM_StartTimer(&UATRAN(hciAckTimer), 
                    BT_MT66XX_SW_RETRANSMISSION_TIMEOUT);    
}

static void uartMT66xxRetransmissionPostiveAck(void)
{
    EVM_ResetTimer(&UATRAN(hciAckTimer));
    uartMT66xxRetransmissionAck(1);
}


static void uartMT66xxRetransmissionTimeout(EvmTimer *timer)
{
    OS_Report(("MT66xx flow control timeout"));
    if(UATRAN(rxBuffHandle) != 0)
    {
        RXBUFF_Free(UATRAN(rxBuffHandle));
        UATRAN(rxBuffHandle) = 0;
    }

    UARTTRAN_ResetRxState();
    uartMT66xxRetransmissionAck(0);
}

#endif
