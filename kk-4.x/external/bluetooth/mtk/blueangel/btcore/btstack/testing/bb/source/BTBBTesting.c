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

#include "bt_common.h"
#include "xatypes.h"
#include "btconfig.h"
#include "osapi.h"
#include "bluetooth_gap_struct.h"
#include "eventmgr.h"
#include "btalloc.h"
#include "BtBBTesting.h"
#include "conmgr.h"
#include "rfcomm.h"
#include "rfc.h"

#include "app_buff_alloc.h"
#include "tst_sap.h"

#ifdef MTK_BB_TESTING

typedef struct _BtBBTestSPPContext {
    CmgrHandler cmgrHandler;
    RfChannel       channel;        
    RfService service;
    U16 spp_tx_size;
    EvmTimer spp_timer;
    U16 timer_count;
} BtBBTestSPPContext;

typedef struct _BtBBTestContext {
    U8 BTBBTestItem;
    EvmTimer btbb_inquiry;
    EvmTimer btbb_inquiry_assert;
    U8 common_data[MTK_BB_SPP_TEST_MAXTXSIZE];
    ListEntry       pkt_List;    
    BtPacket btp[BTBB_ACL_NUMBER_NO];
    BtBBTestSPPContext spp;
    U32 btbb_LoopBack_tx_counter;
    U32 btbb_LoopBack_rx_counter;
} BtBBTestContext;


BtBBTestContext btChipTestContext;

#define BtBBTestSpp(s) (btChipTestContext.spp.s)
#define BtBBTest(s) (btChipTestContext.s)


extern void MeEnabledLocalLoopback(void);
extern void BTBMGapInquiry(bt_bm_discovery_req_struct *btbm_adp_input_msg);

void BTBB_InquiryTimer(EvmTimer *Timer);
void btBBTestSppInit(void);
void btBBTestSppCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, BtStatus Status);

void btBBTestSppCallbackFunc(RfChannel *chan, RfCallbackParms *parms);
void btBBTestSppOpenServer(void);
U8 btBBTestSppOpenClientChannel(U8 *bd_addr);
void btBBTestSppTxData(void);
void btBBTestSppTxTimeout(EvmTimer *Timer);
void btBBTestLocalLoopbackMode(U8 loopBackMode);
void btBBTestReportCommandSend(const HciCommand *cmd);
BtPacket *BtBBTestHciTxReady(HciCallbackParms *parms);
void btBBTestHciCallback(U8 event, HciCallbackParms *parms);
void btBBTestLocalLoopbackTx(void);

U16 btBBtestCovertU16(char *string);
U8 btBBtestCovertU8(char *string);



#ifdef MTK_BB_LOOPBACK_MODE
extern void GORMcmd_HCC_UART_TEST(void);
#endif
void btBBTestDataTest (void)
{
            btBBTestSppInit();
            BtBBTest(btbb_LoopBack_tx_counter) = 0;
            BtBBTest(btbb_LoopBack_rx_counter) = 0;                
            btBBTestLocalLoopbackTx();
}
EvmTimer btLoopBackTimer;    
void BTBBDisableLoopbackTimer(EvmTimer *Timer)
{
    Timer->func = 0;
    bt_prompt_trace(MOD_BT, "[BT] BTBBDisableLoopbackTimer");
    btBBTestLocalLoopbackMode(0x00);
}
void BTBBHandleTestCases(U8 *string)
{
    U8 i =0;
    U8 bd_addr[6];

    BtBBTest(BTBBTestItem) = btBBtestCovertU8((char *)string);
    bt_trace(TRACE_GROUP_1,BTBBTESTITEMBT_DEBUGx02X ,BtBBTest(BTBBTestItem));
    switch (BtBBTest(BTBBTestItem))
    {
        case BTBBTESTING_AUTO_SLEEP_INQUIRY:
            EVM_ResetTimer(&BtBBTest(btbb_inquiry));
            BtBBTest(btbb_inquiry).func = BTBB_InquiryTimer;
            BtBBTest(btbb_inquiry).context = 0;
            EVM_StartTimer(&BtBBTest(btbb_inquiry), 10000);            
            EVM_ResetTimer(&BtBBTest(btbb_inquiry_assert));
            BtBBTest(btbb_inquiry_assert).func = BTBB_InquiryTimer;
            BtBBTest(btbb_inquiry_assert).context = 0;
            EVM_StartTimer(&BtBBTest(btbb_inquiry_assert), 120000);            
            break;
        case BTBBTESTING_AUTO_SLEEP_DCM:
						BtRadio_DCM();
            break;
        case BTBBTESTING_AUTO_SLEEP_FIX_32:
						BtRadio_DCM_Fix32();
            break;
        case BTBBTESTING_AUTO_SLEEP_FIX_64:
						BtRadio_DCM_Fix64();
            break;
        case BTBBTESTING_SPP_TXRX_TEST_SERVER:
            btBBTestSppInit();
            btBBTestSppOpenServer();
            break;
        case BTBBTESTING_SPP_TXRX_TEST_CLIENT:
            for(i=0;i<12;i++)
            {
                if ((string[i+2] >='0') && (string[i+2] <='9'))
                {
                    string[i+2] -= '0';
                }
                else if ((string[i+2] <='f') && (string[i+2] >='a'))
                {
                    string[i+2] = string[i+2] - 'a'+10;
                }
                else if ((string[i+2] <='F') && (string[i+2] >='A'))
                {
                    string[i+2] = string[i+2] - 'A'+10;
                }
                else
                    return;
            }
            bd_addr[5] = (string[2]) << 4 | (string[3]);
            bd_addr[4] = (string[4]) << 4 | (string[5]);
            bd_addr[3] = (string[6]) << 4 | (string[7]);
            bd_addr[2] = (string[8]) << 4 | (string[9]);
            bd_addr[1] = (string[10]) << 4 | (string[11]);
            bd_addr[0] = (string[12]) << 4 | (string[13]);
            btBBTestSppInit();
            btBBTestSppOpenClientChannel((U8 *)&bd_addr);
            break;
        case BTBBTESTING_SPP_TXRX_CONFIG:
            {    
                U16 size;
                size = btBBtestCovertU16((char *)string+2);
                if(size > MTK_BB_SPP_TEST_MAXTXSIZE)
                    BtBBTestSpp(spp_tx_size) = MTK_BB_SPP_TEST_MAXTXSIZE;
                else
                    BtBBTestSpp(spp_tx_size) = size;
                bt_trace(TRACE_GROUP_1,SET_SPP_TX_SIZE_xD ,BtBBTestSpp(spp_tx_size));                
            }
            {    
                U16 count;
                count = btBBtestCovertU16((char *)string+6);
                BtBBTestSpp(timer_count) = count;
                bt_trace(TRACE_GROUP_1,SET_SPP_TIMER_xD ,BtBBTestSpp(timer_count));        
            }    
            break;
        case BTBBTESTING_SPP_TXRX_START:
            btBBTestSppTxData();
            break;
        case BTBBTESTING_SET_BB_BUFFER_SIZE:
            {
                U8 num;
                num = btBBtestCovertU8((char *)string+2);
                bt_trace(TRACE_GROUP_1,BUFFER_SIZExD ,num);
                Assert(num <= 0x08);
                HCC(aclTxPacketsLeft) = num;
            }
            break;
        case BTBBTESTING_SET_BB_LOCAL_LOOPBACK_ON:
            btBBTestLocalLoopbackMode(0x01);
            break;
        case BTBBTESTING_SET_BB_LOCAL_LOOPBACK_OFF:
            btBBTestLocalLoopbackMode(0x00);
            break;
        case BTBBTESTING_SET_BB_TX_START:
            btBBTestDataTest();
            break;
#ifdef MTK_BB_LOOPBACK_MODE
        case 0x0D:
            GORMcmd_HCC_UART_TEST();
            break;
#endif            
    }
}
        
void BTBB_InquiryTimer(EvmTimer *Timer)
{
    bt_bm_discovery_req_struct inquiry_msg;
    bt_prompt_trace(MOD_BT, "[BT] BTBB_InquiryTimer");
    inquiry_msg.cod = 0xffffffff;
    inquiry_msg.inquiry_length = 60;
    inquiry_msg.inquiry_number = 15;
    inquiry_msg.access_mode = BTBM_GENERAL_INQUIRY;
    inquiry_msg.discovery_mode = BTBM_DISCOVERY_WITH_NAME;
    BtRadio_DCM_Test();
//    BTBMGapInquiry(&inquiry_msg);
    EVM_ResetTimer(&BtBBTest(btbb_inquiry));
    BtBBTest(btbb_inquiry).func = BTBB_InquiryTimer;
    BtBBTest(btbb_inquiry).context = 0;
    EVM_StartTimer(&BtBBTest(btbb_inquiry), 3000);
}

void BTBB_InquiryNoEventTimer(EvmTimer *Timer)
{
    bt_trace(TRACE_GROUP_1,INQUIRY_AFTER_120_SECONDS_NOT_COMPLETED);
    Assert(0);
}


void BTBB_InquiryCompleted(void)
{
    EVM_ResetTimer(&BtBBTest(btbb_inquiry));
    BtBBTest(btbb_inquiry).func = BTBB_InquiryTimer;
    BtBBTest(btbb_inquiry).context = 0;
    EVM_StartTimer(&BtBBTest(btbb_inquiry), 10000);            
}

BOOL BTBBTestingCases(U8 item)
{
    BOOL result = FALSE;
    if(BtBBTest(BTBBTestItem) == item)
    {
        switch (BtBBTest(BTBBTestItem))
        {
            case 0x00:
                result = FALSE;
                break;
            case BTBBTESTING_NAME_REQUEST_LOOP:
                result = TRUE;
                break;
            case BTBBTESTING_HEADSET_ALWAYS_UNSNIFF:
                result = TRUE;
                break;
            case BTBBTESTING_AUTO_SLEEP_INQUIRY:
                result = TRUE;
                break;
        }
    }
    bt_trace(TRACE_GROUP_1,BTBBTESTITEM_RESULTBT_DEBUGx02X ,result);
    return result;
}

void BtBBTestInit(void)
{
    int i; 
    
    memset((U8 *)&btChipTestContext, 0, sizeof(btChipTestContext));
    InitializeListHead(&BtBBTest(pkt_List));  
    for(i=0; i< BTBB_ACL_NUMBER_NO;i++)
        InsertTailList(&BtBBTest(pkt_List), &(BtBBTest(btp)[i].node));
    for(i=0; i< MTK_BB_SPP_TEST_MAXTXSIZE;i++)
        BtBBTest(common_data)[i] = (U8)i ;
}

void btBBTestSppInit(void)
{
    int i;
    
    memset((U8 *)&btChipTestContext.spp, 0, sizeof(btChipTestContext.spp));
    BtBBTestSpp(channel).callback     = btBBTestSppCallbackFunc;
    BtBBTestSpp(channel).maxFrameSize = RF_MAX_FRAME_SIZE;
    BtBBTestSpp(channel).priority     = 63;
    BtBBTestSpp(spp_tx_size) = MTK_BB_SPP_TEST_MAXTXSIZE;
    BtBBTestSpp(timer_count) = 0;
    EVM_ResetTimer(&BtBBTestSpp(spp_timer));
    InitializeListHead(&BtBBTest(pkt_List));  
    for(i=0; i< BTBB_ACL_NUMBER_NO; i++)
        InsertTailList(&BtBBTest(pkt_List), &(BtBBTest(btp)[i].node));
    for(i=0; i< MTK_BB_SPP_TEST_MAXTXSIZE;i++)
        BtBBTest(common_data)[i] = (U8)i ;
}


U8 btBBTestSppOpenClientChannel(U8 *addr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;

    memcpy((U8 *)bd_addr.addr, (U8 *)addr,6);
    bt_trace(TRACE_GROUP_1,BD_ADDRBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X ,addr[0],addr[1],addr[2],
                                        addr[3],addr[4],addr[5]);
    (void)CMGR_RegisterHandler(&BtBBTestSpp(cmgrHandler), btBBTestSppCmgrCallback);
    status = CMGR_CreateDataLink(&BtBBTestSpp(cmgrHandler), &bd_addr);
    if (status == BT_STATUS_SUCCESS) 
    {
    }
    return status;
}

void btBBTestSppOpenServer(void)
{
    BtBBTestSpp(service).serviceId = RFCOMMTESTER_RFCOMM_SRV_CHN;
    RF_RegisterServerChannel(&BtBBTestSpp(channel), 
                                   &BtBBTestSpp(service),
                                   0);
}



void btBBTestSppCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    switch (Event) {

    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            RF_OpenClientChannel(BtBBTestSpp(cmgrHandler).remDev, 
                                          RFCOMMTESTER_RFCOMM_SRV_CHN,
                                          &BtBBTestSpp(channel),
                                          0x00);   
            return;
        }
        else
            bt_trace(TRACE_GROUP_1,OPEN_CHANNEL_FAILED);
        (void)CMGR_RemoveDataLink(&BtBBTestSpp(cmgrHandler));
        (void)CMGR_DeregisterHandler(&BtBBTestSpp(cmgrHandler));
        break;
    }
}


void btBBTestSppCallbackFunc(RfChannel *chan, RfCallbackParms *parms)
{
    bt_trace(TRACE_GROUP_1,BTBBTESTSPPCALLBACKFUNC_BT_DEBUGx02X , parms->event);

    switch (parms->event) 
    {
        case RFEVENT_PACKET_HANDLED:
            InsertTailList(&BtBBTest(pkt_List),&( parms->ptrs.packet->node));
            if(BtBBTestSpp(timer_count) ==0)
                btBBTestSppTxData();
            else
            {
                BtBBTestSpp(spp_timer).func = btBBTestSppTxTimeout;
                EVM_StartTimer(&BtBBTestSpp(spp_timer), BtBBTestSpp(timer_count));
            }            
        case RFEVENT_CLOSE_IND:
            break;
        case RFEVENT_CLOSED:
              break;
        case RFEVENT_DATA_IND:
            RF_AdvanceCredit(chan, 1);
            break;
        case RFEVENT_OPEN_IND:
            RF_AcceptChannel(chan);
            break;
        case RFEVENT_OPEN:
            bt_trace(TRACE_GROUP_1,CHANNEL_OPENED);
            RF_AdvanceCredit(chan, 1);
            EVM_ResetTimer(&BtBBTestSpp(spp_timer));
            break;
        default:
           break;
   }
}

void btBBTestSppTxData(void)
{
    BtPacket *pkt;
    if (!IsListEmpty(&BtBBTest(pkt_List)))
    {
        pkt = (BtPacket *)RemoveHeadList(&BtBBTest(pkt_List));
        pkt->data = BtBBTest(common_data);
        pkt->dataLen = 329;
        pkt->headerLen = 0;
        pkt->priority_type = BTP_FLAG_LOW_PRIPORTY;
        RF_SendData(&BtBBTestSpp(channel), pkt);
    }
}

void btBBTestSppTxTimeout(EvmTimer *Timer)
{
    Timer->func = 0;
    bt_prompt_trace(MOD_BT, "[BT] btBBTestSppTxTimeout");
    btBBTestSppTxData();
}


void btBBTestLocalLoopbackMode(U8 loopBackMode)
{
    BtRemoteDevice *remDev;
    BD_ADDR *addr;
    U32 cod;
    BtCodRecord record;
    U8 i = 0;    
    BtScoConnect *scocon=0;
    
    addr = (BD_ADDR *)&MEC(bdaddr);
    cod = MEC(classOfDevice);
    MEC(loopbackMode) = loopBackMode;
    MeEnabledLocalLoopback();
    if(loopBackMode !=0)
    {
        if ((remDev = MeGetFreeRemoteDevice()) != 0) {
            /* Store the proper values */
            OS_MemCopy(remDev->bdAddr.addr, addr->addr, 6);
            remDev->cod = cod;
            remDev->state = BDS_IN_CON;
            MEC(pendCons) += 1;
            record.bdAddr = remDev->bdAddr;
            record.cod = remDev->cod;            
            DDB_COD_AddRecord(&record);
            remDev->restricted = FALSE;
            if (i < NUM_BT_DEVICES) 
            {
                scocon = ScoGetFreeConnect();
                if(scocon !=0)
                {
                    scocon->scord = remDev;
                    scocon->scostate = BDS_IN_CON;
                    scocon->scoLinkType = BLT_SCO;
                }
                else
                    bt_trace(TRACE_GROUP_1,SCOCON_IS_ZERO);
            }
        }
        else
            bt_trace(TRACE_GROUP_1,NO_AVAILABLE_REMDEV);
    }
}

void btBBTestHciCallback(U8 event, HciCallbackParms *parms)
{
bt_prompt_trace(MOD_BT, "[BT] btBBTestHciCallback");
    switch (event) 
    {
        case HCI_DATA_IND:
            bt_trace(TRACE_GROUP_1,CON_HDLBT_DEBUGx02X_RX_DATA_LEN_xD ,parms->hciHandle, parms->ptr.rxBuff->len);
            if(!OS_MemCmp((U8 *)BtBBTest(common_data), 
                parms->ptr.rxBuff->len,
                parms->ptr.rxBuff->buffer, 
                parms->ptr.rxBuff->len))
            {
                kal_prompt_trace(MOD_NIL, "len1 = %d", parms->ptr.rxBuff->len);            
                kal_prompt_trace(MOD_NIL, "len2 = %d", parms->ptr.rxBuff->len);            
                bt_trace(TRACE_GROUP_1,RX_DATA_NOT_EQUAL);
                Assert(0);
            }
            // James modify for next loopback test
            BtBBTest(btbb_LoopBack_rx_counter)++;
                
            if(BtBBTest(btbb_LoopBack_rx_counter) > BTBBTXCOUNTER)
            {
                kal_prompt_trace(MOD_NIL, "RX counter:%d", BtBBTest(btbb_LoopBack_rx_counter));            
                EVM_ResetTimer(&btLoopBackTimer);
                btLoopBackTimer.func = BTBBDisableLoopbackTimer;
                btLoopBackTimer.context = 0;
                EVM_StartTimer(&btLoopBackTimer, 1000);     
            }
            break;
        case HCI_PACKET_HANDLED:
            bt_trace(TRACE_GROUP_1,HCI_PACKET_HANDLED_TRC);
            InsertTailList(&BtBBTest(pkt_List),&(parms->ptr.packet->node));
            btBBTestLocalLoopbackTx();
            break;
        case HCI_COMMAND_HANDLED:
            MEC(commandPending) = FALSE;
            /* The command buffer is free */
            bt_trace(TRACE_GROUP_1,BTLOG_ME_HCIGENCMD_HANDLED);
            InitializeListEntry(&(parms->ptr.cmd->node));
            if (parms->status == BT_STATUS_SUCCESS)
                btBBTestReportCommandSend(parms->ptr.cmd);
    
            /* start operation queue, if it is waiting for the command buffer */
            if (MEC(opMask) & BOM_HCICOMMAND_BUSY) {
                MEC(opMask) &= ~BOM_HCICOMMAND_BUSY;
                MeStartOperation();
            }
            /* finish init radio task, if it is waiting */
            if (MEC(opMask) & BOM_INITRADIO_BUSY) {
                MEC(opMask) &= ~BOM_INITRADIO_BUSY;
                MeInitRadio(BOE_TASK_END);
            }
            break;
    
        case HCI_SEND_IND:
            parms->ptr.packet = BtBBTestHciTxReady(parms); 
            break;
        case HCI_CONTROLLER_EVENT:
            (void)MeHandleEvents(parms->ptr.hciEvent->event, 
                                 parms->ptr.hciEvent->len, 
                                 parms->ptr.hciEvent->parms);
            break;
        case HCI_INIT_STATUS:
            /* HCI initializaiton is complete */
            bt_trace(TRACE_GROUP_1,ME_HCI_INIT_COMPLETE_STATUS_xD , parms->status);
            MeProcessInitStatus(parms);
            break;
        case HCI_DEINIT_STATUS:
            /* Set state to ensure no commands are queued. */
            MEC(stackState) = BTSS_INITIALIZE_ERR;
            bt_trace(TRACE_GROUP_1,ME_HCI_DEINIT_COMPLETE_STATUS_xD , parms->status);
            MEC(curTask) = 0;
            MeFailPendingCommands();
            MEC(stackState) = BTSS_NOT_INITIALIZED;
            MEC(btEvent).eType = BTEVENT_HCI_DEINITIALIZED;
            MeReportNMResults();
            break;
        case HCI_TRANSPORT_ERROR:
            bt_trace(TRACE_GROUP_1,ME_HCI_TRANSPORT_ERROR);
            MEC(btEvent).eType = BTEVENT_HCI_FATAL_ERROR;
            MeReportNMResults();
            HCI_Deinit();
            break;
    }
}

void btBBTestReportCommandSend(const HciCommand *cmd)
{
    ListEntry*  entry;
    BtHandler*  handler;
    BtEvent     event;

    event.eType = BTEVENT_HCI_COMMAND_SENT;
    event.p.hciCmd.type = LEtoHost16(cmd->header);
    event.p.hciCmd.length = cmd->header[2];
    event.p.hciCmd.parms = cmd->parms;

    Assert(IsListCircular(&MEC(regList)));
    entry = MEC(regList).Flink;
    while(entry != &MEC(regList)) {
        /* Point to the next entry before calling the callback in case the
         * callback removes the current handler
         */
        handler = (BtHandler*)entry;
        entry = entry->Flink;
        
        /* Call the callback */
        event.handler = handler;
        handler->callback(&event);

        Assert(IsListCircular(entry));
    }
    Assert(IsListCircular(&MEC(regList)));
}



BtPacket *BtBBTestHciTxReady(HciCallbackParms *parms)
{
    BtRemoteDevice *link;
    BtPacket       *packet;
    BtPacket       *txPacket = 0;
    ListEntry      *queue;
    I16             packetLen;
    HciHandle HciHndl;

    HciHndl = parms->hciHandle;
    if (0 != (link = MeMapHciToRemoteDevice((HciHandle)(HciHndl & 0x0FFF)))) {
        if (!IsListEmpty(&(link->txHighPriorityQueue)))
        {
            queue = &(link->txHighPriorityQueue);
            for (packet = (BtPacket *)GetHeadList(queue);
                 packet != (BtPacket *)queue;
                 packet = (BtPacket *)GetNextNode(&packet->node)) 
            {
                txPacket = (BtPacket *)packet;
                RemoveEntryList(&packet->node);
                break;
            } 
        } 
    }
    /* Pass the packet to the HCI */
    return txPacket;
}

void btBBTestLocalLoopbackTx(void)
{
    BtRemoteDevice *remDev;
    BD_ADDR *addr;
    BtPacket *pkt;
    BtStatus status;
    
    addr = (BD_ADDR *)&MEC(bdaddr);

    /* Locate the remote device */
    HCI_RegisterRadioHandler(btBBTestHciCallback);
    remDev = ME_FindRemoteDeviceP((U8 *)&addr->addr);
    if (remDev != 0) 
    {
    
        if(BtBBTest(btbb_LoopBack_tx_counter) > BTBBTXCOUNTER)
        {
            kal_prompt_trace(MOD_NIL, "TX counter:%d", BtBBTest(btbb_LoopBack_tx_counter));            
            bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);
            bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
            bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
            bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
            bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
            return;
        }
        Assert(remDev->state != BDS_DISCONNECTED);
        while (!IsListEmpty(&BtBBTest(pkt_List)))
        {
            BtBBTest(btbb_LoopBack_tx_counter)++;
            pkt = (BtPacket *)RemoveHeadList(&BtBBTest(pkt_List));
            pkt->data = BtBBTest(common_data);
            pkt->dataLen = 329;
            pkt->headerLen = 0;
            pkt->priority_type = BTP_FLAG_HIGH_PRIPORTY;
            InsertTailList(&(remDev->txHighPriorityQueue), &(pkt->node));
            remDev->okToSend = TRUE;
            status = HCI_RequestToSend(remDev->hciHandle);
            if(BtBBTest(btbb_LoopBack_tx_counter) > BTBBTXCOUNTER)
            {
                kal_prompt_trace(MOD_NIL, "TX counter2:%d", BtBBTest(btbb_LoopBack_tx_counter));            
                bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);
                bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
                bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
                bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
                bt_trace(TRACE_GROUP_1,STOPPED_TO_TEST_THE_BTBB_TEST);                
                break;
            }

        }
    }
    else
        bt_trace(TRACE_GROUP_1,EMPTY_REMDEV);

}

U16 btBBtestCovertU16(char *string)
{
    U8 i=0;
    U16 value;
    
    for(i=0;i<4;i++)
    {
        if ((string[i] >='0') && (string[i] <='9'))
        {
        string[i] -= '0';
        }
        else if ((string[i] <='f') && (string[i] >='a'))
        {
        string[i] = string[i] - 'a'+10;
        }
        else if ((string[i] <='F') && (string[i] >='A'))
        {
        string[i] = string[i] - 'A'+10;
        }
        else
            return FALSE;
    }
    bt_trace(TRACE_GROUP_1,STRINGBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X ,string[0],string[1],string[2],string[3]);
    value = (string[0]) << 12 | (string[1]) << 8 | (string[2]) << 4 | (string[3]);
    return value;
}

U8 btBBtestCovertU8(char *string)
{
    U8 i=0;
    U16 value;
    
    for(i=0;i<2;i++)
    {
        if ((string[i] >='0') && (string[i] <='9'))
        {
        string[i] -= '0';
        }
        else if ((string[i] <='f') && (string[i] >='a'))
        {
        string[i] = string[i] - 'a'+10;
        }
        else if ((string[i] <='F') && (string[i] >='A'))
        {
        string[i] = string[i] - 'A'+10;
        }
        else
            return FALSE;
    }
    bt_trace(TRACE_GROUP_1,STRINGBT_DEBUGx02XBT_DEBUGx02X ,string[0],string[1]);
    value = (string[0]) << 4 | (string[1]);
    return value;
}

#endif
