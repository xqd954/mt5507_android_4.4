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

#ifndef BTMTK_ON_WISE
#include <windows.h>
#endif  /* BTMTK_ON_WISE */
#include "bt_common.h"
#include "btconfig.h"
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
#include "bluetooth_gap_struct.h"
#include "rfcomm.h"
#include "rfc.h"
#include "osapi.h"
#include "rfcommtester.h"
#include "sdap.h"
#include "me.h"


extern void SdpRFCOMMTesterInit(void);

BtRfcommTesterContext rfcommTesterContext;

void RfcommTesterInit(void)
{
    int i;
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_INIT);
    memset((U8 *)&rfcommTesterContext, 0, sizeof(rfcommTesterContext));
    RFCOMMTS(channel).callback     = rfcommTesterCallbackFunc;
    RFCOMMTS(channel).maxFrameSize = RF_MAX_FRAME_SIZE;
    RFCOMMTS(channel).priority     = 63;
    RFCOMMTS(testing_init) = TRUE;
    InitializeListHead(&RFCOMMTS(rfcomm_pkt_List));  

    for(i=0; i< 10;i++)
    {
        InsertTailList(&RFCOMMTS(rfcomm_pkt_List), &(RFCOMMTS(btp)[i].node));
    }    
}


U8 RfcommTesterOpenClientChannel(RfcommConnectRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
	U8 bd_addr1[] = { 0x12, 0x66, 0x00, 0x13, 0x00, 0x00};
//	U8 bd_addr1[] = { 0x00, 0x00, 0x46, 0x66, 0x11, 0x02};

    BD_ADDR bd_addr;
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_OPENCLIENT);
    /* Establish the ACL link */
    memcpy((U8 *)bd_addr.addr, (U8 *)bd_addr1,6);
    (void)CMGR_RegisterHandler(&RFCOMMTS(cmgrHandler), rfcommTesterCmgrCallback);
    status = CMGR_CreateDataLink(&RFCOMMTS(cmgrHandler), &bd_addr);
	return status;
}

void RfcommTesterOpenServer(void)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_REGSERVER);
    RFCOMMTS(service).serviceId = 1;
    RF_RegisterServerChannel(&RFCOMMTS(channel), 
                                   &RFCOMMTS(service),
                                   0);
}

void RfcommTesterSendPN(void)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_SENDPN);
    RF_SendPN(&RFCOMMTS(channel));
}

void RfcommTesterSendMSC(U8 on_off)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_MSC, on_off);
    if(on_off == 0x01)
    {
        RFCOMMTS(channel).rfSignals &= RF_FLOW;
    }
    else
        RFCOMMTS(channel).rfSignals &= ~RF_FLOW;
    RfSendModemStatus(&RFCOMMTS(channel),RFCOMMTS(channel).rfSignals, 0, CR_BIT);
}

U8 write_data_context=0;

void RfcommTesterDataReq(U8 periodic_data_req)
{
    U16 rfcomm_size = 0;
    BtPacket *btp;
    printf("RfcommTesterDataReq\n");
    if (IsListEmpty(&RFCOMMTS(rfcomm_pkt_List)))    
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_NO_BUFFER);
        return;
    }
    btp = (BtPacket *)RemoveHeadList(&RFCOMMTS(rfcomm_pkt_List));
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_SENDDATA);
    if(periodic_data_req == 0x00)
        RFCOMMTS(periodic_data_req_flag) = 0x00;
    else
        RFCOMMTS(periodic_data_req_flag) = 0x01;
    btp->flags = BTP_FLAG_INUSE;
    btp->headerLen = 0;
    btp->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    if(RF_QuerySpecVersion(&RFCOMMTS(channel)))
    {
        rfcomm_size = RF_FrameSize(&RFCOMMTS(channel))-1;
    }
    else
        rfcomm_size = RF_FrameSize(&RFCOMMTS(channel));
    printf("data send all:%02x\n",write_data_context);
    memset( (U8 *)RFCOMMTS(common_data), write_data_context, rfcomm_size);
    write_data_context++;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
    {
        SYSTEMTIME st, lt;

        GetSystemTime(&st);
        GetLocalTime(&lt);

        printf(" The local time is: %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);        
    }
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
    btp->data = (U8 *)RFCOMMTS(common_data);
    btp->dataLen = rfcomm_size;
    RF_SendData(&RFCOMMTS(channel),  btp);
}


void RfcommTesterRls(void)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_RLS);
    RfSendLineStatus(&RFCOMMTS(channel), 0x09, CR_BIT);
}

void RfcommTesterCloseChannel(void)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_CLOSE);
    RF_CloseChannel(&RFCOMMTS(channel));
}

BtStatus rfcommTesterServiceQuery(void)
{
    BtStatus status;
    sdap_service_search_struct search_pattern;

    search_pattern.rm = RFCOMMTS(cmgrHandler).remDev;
    search_pattern.sqt = &RFCOMMTS(sdpQueryToken);
    search_pattern.uuid = SC_RFCOMM_TESTER;    
    search_pattern.callback = rfcommTesterQueryCallback;    

    status = SDAP_ServiceSearch(search_pattern);
    return status;
} 


void rfcommTesterQueryCallback(
                        SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
            RF_OpenClientChannel(RFCOMMTS(cmgrHandler).remDev, 
                                          serv_chn,
                                          &RFCOMMTS(channel),
                                          0x00);            
            break;
        case BT_STATUS_FAILED:
            (void)CMGR_RemoveDataLink(&RFCOMMTS(cmgrHandler));
            (void)CMGR_DeregisterHandler(&RFCOMMTS(cmgrHandler));
            break;
        case BT_STATUS_NOSERVICES:
            (void)CMGR_RemoveDataLink(&RFCOMMTS(cmgrHandler));
            (void)CMGR_DeregisterHandler(&RFCOMMTS(cmgrHandler));
            break;            
    }
}


void rfcommTesterCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    switch (Event) {

    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            RF_OpenClientChannel(RFCOMMTS(cmgrHandler).remDev, 
                              1,
                              &RFCOMMTS(channel),
                              0x06);   
            return;
        }
        /* Disconnect and call the application */
        (void)CMGR_RemoveDataLink(&RFCOMMTS(cmgrHandler));
        (void)CMGR_DeregisterHandler(&RFCOMMTS(cmgrHandler));
        break;
    }
}


void rfcommTesterCallbackFunc(RfChannel *chan, RfCallbackParms *parms)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_EVENT, parms->event);
    switch (parms->event) 
    {
        
        case RFEVENT_PACKET_HANDLED:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMTS_PKT_SENT);
            InsertTailList(&RFCOMMTS(rfcomm_pkt_List),&( parms->ptrs.packet->node));
            if (RFCOMMTS(periodic_data_req_flag) == 0x01)
                RfcommTesterDataReq(0x01);
            break;
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
//            L2CAP_SendLockStepConfigReq(0x40);
            RF_AdvanceCredit(chan, 6);
            RfcommTesterDataReq(0x01);

            break;
        case RFEVENT_PORT_NEG_IND:        
        
            break;

        case RFEVENT_PORT_NEG_CNF:        /* Port negotiation confirmed */
            break;
        case RFEVENT_MODEM_STATUS_CNF:    /* Modem status confirmed */
            break;
        case RFEVENT_MODEM_STATUS_IND:    /* Modem status received */
            break;
        case RFEVENT_LINE_STATUS_IND:    /* Line status received */
            break;
        case RFEVENT_LINE_STATUS_CNF:    /* Line status confirmed */
            break;
        case RFEVENT_PORT_STATUS_IND:
            break;
        case RFEVENT_PORT_STATUS_CNF:
            break;
        case RFEVENT_FLOW_OFF_IND:
        case RFEVENT_FLOW_ON_IND:
            break;
        default:
           break;
 
   }


}





BOOL RfcommTesterStateQueuy(void)
{
    if(RFCOMMTS(testing_init))
        return TRUE;
    else
        return FALSE;
}
