/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "bt_common.h"
#include "btalloc.h"
#include "btrom.h"
#include "sys/l2cap_i.h"
#include "sys/l2capxp.h"
#include "sys/mei.h"
#include "bt_devices_list.h"
#include "btaccctrl.h"

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
void L2Cap_HandleLocalBusyDetected(L2CAP_Channel *Channel);
void L2Cap_HandleLocalBusyClear(L2CAP_Channel *Channel);
void L2Cap_HandleEnhancedRetransmissionTimout(L2CAP_Channel *Channel);

void L2Cap_EnhancementRetransmissionSetPollBit(L2CAP_Channel *Channel, U8 index);
BOOL L2Cap_EnhancedAddToTxRejectTable(L2CAP_Channel *Channel, U16 ReqSeq);
BOOL L2Cap_EnhancedAddToRxMissingRejectTable(L2CAP_Channel *Channel, U16 ReqSeq);
extern U16 L2Cap_GetSequenceWrapValue(L2CAP_Channel *Channel);
extern BtStatus L2Cap_SendFeaturesReq(L2CAP_Channel *Channel, BtPacket *Request);
void L2Cap_SendRR(L2CAP_Channel *Channel, U8 index);
void L2Cap_SendRNR(L2CAP_Channel *Channel);
void L2Cap_SendSREJ(L2CAP_Channel *Channel, U16 ReqSeq);
U16 L2Cap_EnhancedUtilGetSrejListEntryReqSeq(U32 src);
U8 L2Cap_EnhancedUtilGetSrejListEntrySent(U32 src);
U32 L2Cap_EnhancedUtilSetSrejListEntrySent(U32 src, U8 set);
void L2Cap_HandleEnhancedMonitorTimout(L2CAP_Channel *Channel);

extern U16 L2Cap_GetRxUnAckNumber(L2CAP_Channel *Channel, U16 wrapValue);

void L2Cap_HandleLocalBusyDetected(L2CAP_Channel *Channel)
{
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        Channel->enhancedLocalBusy = 1;
        L2Cap_SendRNR(Channel);
    }
}

void L2Cap_HandleLocalBusyClear(L2CAP_Channel *Channel)
{
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        if(Channel->enhancedRetransRNRSent == 1)
        {
            Channel->enhancedLocalBusy = 0; /* It is different from spec write */
            Channel->enhancedRetransRNRSent = 0;
            L2Cap_EnhancementRetransmissionSetPollBit(Channel, 3);
            L2Cap_SendRR(Channel, 2);
            Channel->enhancedSFrameRetryCount = 1;
            L2Cap_StartMonitorTimer(Channel);
        }
        else
        {
            Channel->enhancedLocalBusy = 0;
            Channel->enhancedRetransRNRSent = 0;            
        }
    }
}


void L2Cap_HandleEnhancedRetransmissionTimout(L2CAP_Channel *Channel)
{
	Report(("L2CAP enhanced Retransmission Timeout Wait F bit:%d cid:%d",Channel->enhancedWait_F, Channel->localCid));
    if(Channel->enhancedWait_F == 0)
    {   
        /* In XMIT State */
        Report(("L2CAP enhanced Retransmission timout"));        
        L2Cap_EnhancementRetransmissionSetPollBit(Channel, 5);
        if(Channel->enhancedLocalBusy)
        {
            Report(("L2CAP enhanced Sent RNR"));
            L2Cap_SendRNR(Channel);
        }
        else
        {
            Report(("L2CAP enhanced Sent RR"));
            L2Cap_SendRR(Channel, 3);
        }
        Channel->enhancedSFrameRetryCount = 1;
        L2Cap_StartMonitorTimer(Channel);        
    }
    else
    {
        U8 i;
        for(i=0;i<10;i++)
        {
            Report(("L2CAP enhanced Retransmission timeout but in WAIT_F state"));
        }
        /* 
                It is not defined in BT spec. If Monitor timer starts, rx Retransmission ack, stop monitor again, retransmission restarts.
                We have to retry the previous Monitor timer if we are sill waiting for enhancedWait_F again.
            */
        L2Cap_HandleEnhancedMonitorTimout(Channel);
        
    }
}

void L2Cap_HandleEnhancedMonitorTimout(L2CAP_Channel *Channel)
{
    Report(("L2CAP enhanced L2CAP enhanced:L2Cap_HandleEnhancedMonitorTimout enhancedWait_F:%d",Channel->enhancedWait_F));

    if(Channel->enhancedWait_F ==0)
    {
        U8 i;
        for(i=0;i<10;i++)
        {
            Report(("L2CAP enhanced Monitor timeout but in XMIT state"));
        }
    }
    else
    {   
        Report(("L2CAP enhanced Retry count:%02x, max:%02x",Channel->enhancedSFrameRetryCount, Channel->transmitMax));
        if((Channel->transmitMax == 0) /* Infinite retransmission */
            || (Channel->enhancedSFrameRetryCount < Channel->transmitMax))
        {
            L2Cap_EnhancementRetransmissionSetPollBit(Channel, 7);
            if(Channel->enhancedLocalBusy)
                L2Cap_SendRNR(Channel);
            else
                L2Cap_SendRR(Channel, 4);
            if(Channel->transmitMax == 0)
                Channel->enhancedSFrameRetryCount = 1;
            else
                Channel->enhancedSFrameRetryCount++;
            L2Cap_StartMonitorTimer(Channel);
        }
        else
        {
            Report(("L2CAP enhanced transmitMax:0x%02x reach ! disconnect", Channel->transmitMax));
            Report(("L2CAP flush packets"));
            L2Cap_FlushChannelTransmits(Channel); // ALPS0025480 retry64 uses all sframe(syspkt)
            L2Cap_SendDiscReqInternal(Channel);
        }
    }
}



void L2Cap_ForceToGetAnAck(L2CAP_Channel *Channel)
{
    Report(("L2Cap_ForceToGetAnAck"));
	Report(("L2CAP enhanced Retransmission Timeout Wait F bit:%d",Channel->enhancedWait_F));
    if(Channel->enhancedWait_F == 0)
    {   
        /* In XMIT State */
        Report(("L2CAP enhanced Retransmission timout"));        
        L2Cap_EnhancementRetransmissionSetPollBit(Channel, 17);
        if(Channel->enhancedLocalBusy)
        {
            Report(("L2CAP enhanced Sent RNR"));
            L2Cap_SendRNR(Channel);
        }
        else
        {
            Report(("L2CAP enhanced Sent RR"));
            L2Cap_SendRR(Channel, 23);
        }
        Channel->enhancedSFrameRetryCount = 1;
        L2Cap_StartMonitorTimer(Channel);        
    }
}

struct timeval g_rr_p_last_tv1; //


void L2Cap_SendRR(L2CAP_Channel *Channel, U8 index)
{
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("l2data: L2CAP Send RR cid:%d from:%d recvSeq:%d", Channel->localCid, index, Channel->recvSeq));
#endif
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        Channel->flags2 &= ~(L2CHF2_SEND_REJECT | L2CHF2_SEND_RR | L2CHF2_SEND_RNR | L2CHF2_SEND_SREJ );
        Channel->flags2 |= L2CHF2_SEND_RR;
        L2Cap_SendSFrame(Channel);
    }
}

void L2Cap_SendRNR(L2CAP_Channel *Channel)
{
    Report(("l2data: L2CAP Send RNR:0x%x cid:%d", Channel, Channel->localCid));
	
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        Channel->flags2 &= ~(L2CHF2_SEND_REJECT | L2CHF2_SEND_RR | L2CHF2_SEND_RNR | L2CHF2_SEND_SREJ );
        Channel->flags2 |= L2CHF2_SEND_RNR;
        Channel->enhancedRetransRNRSent = 1;
        L2Cap_SendSFrame(Channel);
    }
}

void L2Cap_SendSREJ(L2CAP_Channel *Channel, U16 ReqSeq)
{
    Report(("l2data: L2CAP Send SREJ:0x%x reqseq:0x%x cid:%d", Channel, ReqSeq, Channel->localCid));
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
    
        L2Cap_EnhancedAddToRxMissingRejectTable(Channel, ReqSeq);
        Channel->enhancedSrejSent = 1;
		if((Channel->flags2 & L2CHF2_SEND_SREJ) == 0)
		{
			Channel->flags2 &= ~(L2CHF2_SEND_REJECT | L2CHF2_SEND_RR | L2CHF2_SEND_RNR | L2CHF2_SEND_SREJ );
			Channel->flags2 |= L2CHF2_SEND_SREJ;
			L2Cap_SendSFrame(Channel);
		}
    }
}

void L2Cap_SendREJ(L2CAP_Channel *Channel)
{
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("l2data: L2Cap_SendREJ Channel->enhancedRejSent:%d cid:%d",Channel->enhancedRejSent, Channel->localCid));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
    Report(("L2Cap_SendREJ Channel->enhancedRejSent:%d",Channel->enhancedRejSent));
#endif
    if((Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION) &&
        (Channel->enhancedRejSent ==0))
    {
        /* Rej is not allowed to duplicate sent. */
        Channel->enhancedRejSent = 1;
		L2Cap_EnhancementRetransmissionSetPollBit(Channel, 9);
        Channel->flags2 &= ~(L2CHF2_SEND_REJECT | L2CHF2_SEND_RR | L2CHF2_SEND_RNR | L2CHF2_SEND_SREJ );
        Channel->flags2 |= L2CHF2_SEND_REJECT;
        L2Cap_SendSFrame(Channel);
    }
}


void L2Cap_EnhancementRetransmissionSetPollBit(L2CAP_Channel *Channel, U8 index)
{
	{
		gettimeofday(&g_rr_p_last_tv1, NULL);	
		Report(("ping1 time:%04d%04d", g_rr_p_last_tv1.tv_sec, g_rr_p_last_tv1.tv_usec));		
	}

    Report(("L2Cap_EnhancementRetransmissionSetPollBit set poll bit (index:%d) cid:%d", index, Channel->localCid));
    Channel->enhancedWait_F = 1;
    Channel->enhancedPollFinal |= L2CONTROLFRAMEWITHPOLL;
}

void L2Cap_ProcessRecvPorFBit(L2CAP_Channel *Channel, U8 f_control, U8 p_control)
{
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("L2CAP enhanced:L2Cap_ProcessRecvPorFBit p_control:%d cid:%d",p_control, Channel->localCid));
    Report(("L2CAP enhanced:L2Cap_ProcessRecvPorFBit - f_control:%d", f_control));
#endif
    if(p_control == 1)
    {
        Channel->enhancedNeedToSendFinal = 1;
        Report(("L2CAP enhanced:L2Cap_ProcessRecvPorFBit - f_control:%d", f_control));
    }
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("B channel->enhancedNeedToSendFinal:%d",Channel->enhancedNeedToSendFinal));    
    Report(("L2CAP enhanced:L2Cap_ProcessRecvPorFBit enhancedWait_F:%d cid:%d",Channel->enhancedWait_F, Channel->localCid));
#endif
    if(Channel->enhancedWait_F == 1)
    {
        if(f_control == 1)
        {
            /// show the diff period (PING)
            struct timeval g_rr_p_last_tv2;
            U32 diff = 0;
            gettimeofday(&g_rr_p_last_tv2, NULL);
            if( g_rr_p_last_tv2.tv_usec > g_rr_p_last_tv1.tv_usec ){
                diff = g_rr_p_last_tv2.tv_usec - g_rr_p_last_tv1.tv_usec;
            }else{
                diff = g_rr_p_last_tv1.tv_usec - g_rr_p_last_tv2.tv_usec;           
            }
            Report(("ping2 time:%04d%04d", g_rr_p_last_tv2.tv_sec, g_rr_p_last_tv2.tv_usec));
            Report(("ping back diff:%d", diff));
	
				
            /* Final bit received */
            Channel->enhancedWait_F = 0;
            Channel->enhancedSFrameRetryCount = 0;
            if((Channel->flags2 & L2CHF2_SEND_LOCAL_BUSY_CLEAR) !=0)
            {
                L2Cap_HandleLocalBusyClear(Channel);
            }

            Report(("L2CAP enhanced:L2Cap_ProcessRecvPorFBit enhancedAckWaiting:%d cid:%d",Channel->enhancedAckWaiting, Channel->localCid));
            
            if (Channel->enhancedAckWaiting == 1)
            {
            	/* Clear the AckWaiting */
                Channel->enhancedAckWaiting = 0;
                if(Channel->enhancedLocalBusy)
                    L2Cap_SendRNR(Channel);
                else
                    L2Cap_SendRR(Channel, 5);
            }
        }
    }

}



void L2Cap_EnhancedRetransmissionSendAck(L2CAP_Channel *Channel, U8 sendFlag)
{
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("l2data L2cap send ack Wait_F:%d busy:%d sendFlag:%d", Channel->enhancedWait_F, Channel->enhancedLocalBusy, sendFlag));
#endif

    if(Channel->enhancedWait_F == 0)
    {
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("L2CAP enhanced Try to send ack."));
#endif
        Channel->enhancedAckWaiting = 0;
    }
    else
    {
        Report(("L2CAP enhanced Try to send ack but a outgoind poll is sent."));
        Channel->enhancedAckWaiting = 1;
        Report(("L2CAP enhanced Try to send ack in wait final."));
    }
    if(Channel->enhancedLocalBusy)
    {
        Report(("L2CAP Send RNR"));
        L2Cap_SendRNR(Channel);
    }
    else
    {
        if(sendFlag == 1)
        {
            L2Cap_SendRR(Channel, 6);
        }
        else
        {
            U16 nonAckNo;
            U16 wrapValue;
            //U16 txWindow;
            
            wrapValue = L2Cap_GetSequenceWrapValue(Channel);
            nonAckNo = L2Cap_GetRxUnAckNumber(Channel, wrapValue);
#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("Dlight remote wait ack is:%d",nonAckNo));
#endif
#if 0            
            if(nonAckNo == (Channel->txWindow))
            {
                Report(("Send RR a11_4"));
                L2Cap_SendRR(Channel, 17);
            }
#endif


            if((nonAckNo > (Channel->txWindow-2)) &&
                (nonAckNo !=0))
            {
                Report(("Send RR a11_4"));
                L2Cap_SendRR(Channel, 17);
            }
#if 0			
            else if((nonAckNo == (Channel->txWindow-2)/2) &&
                (nonAckNo !=0))
            {
                if((Channel->flags2 & L2CHF2_SEND_RR) ==0)
                {
                    L2Cap_SendRR(Channel, 177);
                }
            }
#endif			

           if( nonAckNo > 0 && (nonAckNo % 4) == 0 ){
                if((Channel->flags2 & L2CHF2_SEND_RR) ==0){
                    L2Cap_SendRR(Channel, 178);
                }
           }

           if( nonAckNo > 0 && (nonAckNo+1) > Channel->txWindow ){
                Report(("L2cap out of nonAckNo (%d)/window(%d). wait sync RR ", nonAckNo,Channel->txWindow ));
           }

        }
    }

}

void L2Cap_EnhancedRetransmissionPassToTxFBit(L2CAP_Channel *Channel, U8 f_control, U16 ReqSeq)
{
    if(f_control == 0x01)
    {
         Channel->enhancedWait_F = 0;
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("L2CAP enhanced:L2Cap_EnhancedRetransmissionPassToTxFBit enhancedAckWaiting:%d",Channel->enhancedAckWaiting));
#endif
        if (Channel->enhancedAckWaiting == 1)
        {
            Channel->enhancedAckWaiting = 0;
            if(Channel->enhancedLocalBusy)
                L2Cap_SendRNR(Channel);
            else
                L2Cap_SendRR(Channel, 7);
        }         
    }
}

void L2Cap_EnhancedRetransmissionSrejResentIFrame(L2CAP_Channel *Channel, U16 ReqSeq)
{
    BOOL result = FALSE;
    if(Channel->txComposeSeq >= Channel->ackSeq)
    {
        /* In sequence, ackSeq, ReqSeq, TxComposeSeq */
		/* If ReqSeq == Channel->ackSeq, it means acked befored. */
        if ((ReqSeq >= Channel->ackSeq) && (ReqSeq <= Channel->txComposeSeq))
            result = TRUE;
    }
    else
    {
        /* In sequence, 
                    ReqSeq, TxComposeSeq  ackSeq or 
                    TxComposeSeq  ackSeq, ReqSeq
                */
		/* If ReqSeq == Channel->ackSeq, it means acked befored. */

        if ((ReqSeq <= Channel->txComposeSeq) || (ReqSeq >= Channel->ackSeq))
            result = TRUE;
    }
    if(result == TRUE)
    {
		L2Cap_EnhancedAddToTxRejectTable(Channel,ReqSeq);
		HCI_RequestToSend(Channel->link->hciHandle);
    }
}

/* The TxReject table is to add a queue to retransmite the I frame queue */
BOOL L2Cap_EnhancedAddToTxRejectTable(L2CAP_Channel *Channel, U16 ReqSeq)
{
    U8 i=0;
	U8 index = 0xFF;
	U16 ackSeq;
	U16 txComposeSeq;
	U16 ReqSeq1;
	U8 wrapStatus = 0;
	U8 compareResult = 0;
	
	ackSeq = Channel->ackSeq;
	txComposeSeq = Channel->txComposeSeq;
    if(txComposeSeq >= ackSeq)
    {
		wrapStatus = 0;
    }
    else
    {
    	wrapStatus = 1;
    }


    for(i=0; i< Channel->enhancedSrejListTxCount; i++)
    {
    	ReqSeq1 = Channel->enhancedTxSrejFrame[i].ReqSeq;
        if(ReqSeq1 == ReqSeq)
        {
			/* Already in the SRej list. */
            return TRUE;
        }
		if(wrapStatus == 0)
		{
			if(ReqSeq1 >= ReqSeq)
			{
				index = i;
				compareResult = 1;
				/* The ReqSeq1 > ReqSeq, so insert ReqSeq in front of ReqSeq1*/
				break;
			}
		}
		else
		{
			if(((ReqSeq1 >= ackSeq) && (ReqSeq >= ackSeq)) ||
				((ReqSeq1 <= txComposeSeq) && (ReqSeq >= txComposeSeq)))
			{
				if(ReqSeq1 >= ReqSeq)
				{
					index = i;
					compareResult = 1;
					/* The ReqSeq1 > ReqSeq, so insert ReqSeq in front of ReqSeq1*/
					break;
				}
			}
			else
			{
				if(ReqSeq1 < ReqSeq)
				{
					index = i;
					compareResult = 1;
					/* The ReqSeq1 > ReqSeq, so insert ReqSeq in front of ReqSeq1*/
					break;
				}
			}
		}
		
    }
    if(Channel->enhancedSrejListTxCount == 0)
    {
        Channel->enhancedTxSrejFrame[0].ReqSeq = ReqSeq;
        Channel->enhancedSrejListTxCount++;
        return TRUE;
    }
    else if(Channel->enhancedSrejListTxCount != L2C_MAX_SUPPORTED_TX_SELECTED_REJECT_NO)
    {
    	for(i= Channel->enhancedSrejListTxCount-1; i>index; i--)
		{
		    if( i < L2C_MAX_SUPPORTED_TX_SELECTED_REJECT_NO && i > 0 ){
			Channel->enhancedTxSrejFrame[i].ReqSeq = Channel->enhancedTxSrejFrame[i-1].ReqSeq;
		    }
		}
		Channel->enhancedTxSrejFrame[index].ReqSeq = ReqSeq;
        Channel->enhancedSrejListTxCount++;
        return TRUE;
    }
    else
	{
        return FALSE;
	}
}

/* The TxReject table is to remove a queue to retransmite the I frame queue */
BOOL L2Cap_EnhancedRemoveFromTxRejectTable(L2CAP_Channel *Channel, U16 ReqSeq)
{
	U8 i = 0;
	
    if(Channel->enhancedSrejListTxCount == 0)
    {
        return FALSE;
    }
    else if(Channel->enhancedSrejListTxCount != L2C_MAX_SUPPORTED_TX_SELECTED_REJECT_NO)
    {
        if( Channel->enhancedSrejListTxCount <= L2C_MAX_SUPPORTED_TX_SELECTED_REJECT_NO ){ 
    	for(i= 0; i<Channel->enhancedSrejListTxCount-1; i++)
		{
                        if( i < (L2C_MAX_SUPPORTED_TX_SELECTED_REJECT_NO-1) ){
			Channel->enhancedTxSrejFrame[i].ReqSeq = Channel->enhancedTxSrejFrame[i+1].ReqSeq;
		}
                }
		Channel->enhancedTxSrejFrame[Channel->enhancedSrejListTxCount-1].ReqSeq = 0x00;
        Channel->enhancedSrejListTxCount--;
        return TRUE;
        }else{
        	OS_Report("wrn SrejListTxCount:%d", Channel->enhancedSrejListTxCount);
        	return FALSE;
        }
    }
    else
	{
        return FALSE;
	}
}

BOOL L2Cap_EnhancedAddToRxMissingRejectTable(L2CAP_Channel *Channel, U16 ReqSeq)
{
    U8 i=0;
	U8 index = 0xFF;
	U16 ackSeq;
	U16 txComposeSeq;
	U16 ReqSeq1;
	U8 wrapStatus = 0;
	U8 compareResult = 0;
	
	ackSeq = Channel->ackSeq;
	txComposeSeq = Channel->txComposeSeq;
    if(txComposeSeq >= ackSeq)
    {
		wrapStatus = 0;
    }
    else
    {
    	wrapStatus = 1;
    }

    Report(("Channel->enhancedSrejList:%d",Channel->enhancedSrejList));

    for(i=0; i< Channel->enhancedSrejList; i++)
    {
    	ReqSeq1 = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[i]);
        if(ReqSeq1 == ReqSeq)
        {
			/* Already in the SRej list. */
			Channel->txSrejList[i] = L2Cap_EnhancedUtilSetSrejListEntrySent(Channel->txSrejList[i], 1);
			/* Set the sent flag to 1 again. */
            return TRUE;
        }
		if(wrapStatus == 0)
		{
			if(ReqSeq1 >= ReqSeq)
			{
				index = i;
				compareResult = 1;
				/* The ReqSeq1 > ReqSeq, so insert ReqSeq in front of ReqSeq1*/
				break;
			}
		}
		else
		{
			if(((ReqSeq1 >= ackSeq) && (ReqSeq >= ackSeq)) ||
				((ReqSeq1 <= txComposeSeq) && (ReqSeq >= txComposeSeq)))
			{
				if(ReqSeq1 >= ReqSeq)
				{
					index = i;
					compareResult = 1;
					/* The ReqSeq1 > ReqSeq, so insert ReqSeq in front of ReqSeq1*/
					break;
				}
			}
			else
			{
				if(ReqSeq1 < ReqSeq)
				{
					index = i;
					compareResult = 1;
					/* The ReqSeq1 > ReqSeq, so insert ReqSeq in front of ReqSeq1*/
					break;
				}
			}
		}
		
    }
    if(Channel->enhancedSrejList == 0)
    {
        Report(("Channel->enhancedSrejList:%d",Channel->enhancedSrejList));    
        Report(("ReqSeq:%d",ReqSeq));            
        Channel->txSrejList[0] = (U32)ReqSeq;
		Channel->txSrejList[0] = L2Cap_EnhancedUtilSetSrejListEntrySent(Channel->txSrejList[0], 1);
        Report(("Channel->txSrejList[0]:%d",Channel->txSrejList[0]));            
        Channel->enhancedSrejList++;
        return TRUE;
    }
    else if(Channel->enhancedSrejList < Channel->enhancedMaxMissingSrejNo)
    {
    	for(i= Channel->enhancedSrejList-1; i>index; i--)
		{
			Channel->txSrejList[i] = Channel->txSrejList[i-1];
		}
		Channel->txSrejList[index] = (U32)ReqSeq;
		Channel->txSrejList[0] = L2Cap_EnhancedUtilSetSrejListEntrySent(Channel->txSrejList[0], 1);
        Channel->enhancedSrejList++;
        return TRUE;
    }
    else
	{
        return FALSE;
	}

}


void L2Cap_EnhancedRemoveFromRxMissingRejectTable(L2CAP_Channel *Channel, U16 ReqSeq)
{
    U8 i;
	U8 index;

	if(Channel->enhancedSrejList == 0)
		return;
    for(i = 0; i < Channel->enhancedSrejList; i++)
    {
	    if(L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[i]) == ReqSeq)
    	{
    		break;
    	}
    }
    for(index = i; index < Channel->enhancedSrejList-1; index++)
    {
		Channel->txSrejList[index] = Channel->txSrejList[index+1];
    }
	Channel->txSrejList[Channel->enhancedSrejList-1] = 0x00;
	Channel->enhancedSrejList--;
}

void L2Cap_EnhancedRemoveAllFromRxMissingRejectTable(L2CAP_Channel *Channel)
{
    U8 i;

	if(Channel->enhancedSrejList == 0)
		return;
    for(i = 0; i < Channel->enhancedSrejList; i++)
    {
	    Channel->txSrejList[i] = 0;
    }
	Channel->enhancedSrejList = 0;
}


U16 L2Cap_EnhancedGetNextSrejRxMissingRejectTable(L2CAP_Channel *Channel, U8 *index)
{
    U8 i=0;
    U16 ReqSeq = 0xFFFF;
    
    Report(("L2Cap_EnhancedGetNextSrejRxMissingRejectTable"));
    Report(("Channel->enhancedSrejList %08x",Channel->enhancedSrejList));

    for(i=0; i< Channel->enhancedSrejList; i++)
    {
        Report(("Channel->txSrejList[%d]:%08x",i,Channel->txSrejList[i]));
        if(L2Cap_EnhancedUtilGetSrejListEntrySent(Channel->txSrejList[i]) != 0)
        {
            ReqSeq = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[i]);
			/* Set sent flag to 0.*/
            Channel->txSrejList[i] = L2Cap_EnhancedUtilSetSrejListEntrySent(Channel->txSrejList[i], 0);
			*index = i;
            Report(("L2Cap_EnhancedGetNextSrejRxMissingRejectTable:%08x", ReqSeq));
            Report(("*index:%08x", *index));

            return ReqSeq;
        }
    }
	*index = 0;
    Report(("L2Cap_EnhancedGetNextSrejRxMissingRejectTable A:%08x", ReqSeq) );
    Report(("*index B:%08x", *index));
    
    return ReqSeq;
}

U16 L2Cap_EnhancedGetRxMissingRejectTableTail(L2CAP_Channel *Channel)
{
    U8 i=0;
    U16 ReqSeq = 0xFFFF;

    if((Channel->enhancedSrejList !=0) &&
        (Channel->enhancedSrejList <= Channel->enhancedMaxMissingSrejNo))
    {
        ReqSeq = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[Channel->enhancedSrejList-1]);
    }
    return ReqSeq;
}

U16 L2Cap_EnhancedGetRxMissingRejectTableHead(L2CAP_Channel *Channel)
{
    U8 i=0;
    U16 ReqSeq = 0xFFFF;

    if(Channel->enhancedSrejList !=0)
    {
        ReqSeq = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[0]);
    }
    return ReqSeq;
}


BOOL L2Cap_EnhancedVerifyReqSeqInRxMissingRejectTable(L2CAP_Channel *Channel, U16 ReqSeq)
{
    U8 i = 0;
	U16 tempReqSeq;
	
	tempReqSeq = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[0]);
	
    for(i=0; i< Channel->enhancedSrejList; i++)
    {
		tempReqSeq = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[i]);
        if(tempReqSeq == ReqSeq)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL L2Cap_EnhancedVerifyReqSeqInRxMissingRejectTableHead(L2CAP_Channel *Channel, U16 ReqSeq)
{
	U16 tempReqSeq;

	tempReqSeq = L2Cap_EnhancedUtilGetSrejListEntryReqSeq(Channel->txSrejList[0]);
    if((Channel->enhancedSrejList !=0) &&
        (tempReqSeq == ReqSeq))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL L2Cap_EnhancedVerifySrejAllSentInRxMissingRejectTable(L2CAP_Channel *Channel)
{
    U8 i = 0;
	
    for(i=0; i< Channel->enhancedSrejList; i++)
    {
		if(L2Cap_EnhancedUtilGetSrejListEntrySent(Channel->txSrejList[i]) != 0)
		{	
			Report(("Still have SREJ S frame need to sent out"));
			return FALSE;
		}
    }
	Report(("No SREJ S frame need to sent out"));
    return TRUE;
}


U16 L2Cap_EnhancedUtilGetSrejListEntryReqSeq(U32 src)
{
	U16 reqSeq;
	
	reqSeq = (U16)(src & 0x0000FFFF);
	return reqSeq;
}

U8 L2Cap_EnhancedUtilGetSrejListEntrySent(U32 src)
{
	U8 sent;
	
	sent = (U8)((src & 0x80000000) >> 24);
	return sent;
}

U32 L2Cap_EnhancedUtilSetSrejListEntrySent(U32 src, U8 set)
{
	U32 dest;
	
	if(set)
	{
		dest = (src | 0x80000000);
	}
	else
	{
		dest = (src & 0x7FFFFFFF);
	}
    Report(("dest:%08x",dest));
	return dest;
}

#endif
