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
 * File:        obparse.c
 *
 * Description: This file contains code for the OBEX packet parser.
 *
 * Created:     April 29, 1997
 *
 * Version:     MTObex 3.4
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
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Jan 13 2009 mtk80035
 * [MAUI_01496536] MTK:Bluetooth_[1] ASSERT fail: obclient.c 1083 - BT
 * 
 *
 * Dec 26 2008 mbj06038
 * [MAUI_01173710] MTK:Bluetooth FTP_FTP profile auto disconnect
 * 
 *
 * Aug 7 2008 mbj06032
 * [MAUI_01099468] [BT OBEX] Client send PUT opcode with FinalBit in the las data packet
 * move OBEX_GET_USE_END_OF_BODY  defination to obconfig.h
 *
 * Jul 15 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * start Tx timer after call RF_SendData; when timeout, disconnect and return the tx buffer
 *
 * Jul 4 2008 mbj06032
 * [MAUI_01082197] [BT]Long time display Connecting and can't connect server.
 * return the TxBuffer if not use it; use a buffer pointer to record the TxBuffer, if the TxBuffer has not been return when RF channel closed, then return it
 *
 * Jun 25 2008 mbj06032
 * [MAUI_01072847] [BT OBEX] add trace info
 * 
 *
 * Jun 17 2008 mbj06032
 * [MAUI_01077981] [BT]ASSERT Fail: observer.c 1085 -BT
 * 
 *
 * Jun 17 2008 mbj06032
 * [MAUI_01077981] [BT]ASSERT Fail: observer.c 1085 -BT
 * 
 *
 * Jun 13 2008 mbj06032
 * [MAUI_01072101] [BT]ASSERT Fail: len>=3 observer.c 1864 -BT
 * if RF channel is closing, it's frame size is 0, then return
 *
 * May 28 2008 mbj06032
 * [MAUI_00678189] [PBAP]Can't get the missed call wth Nokia BH-902
 * Server support fitting whole data into one packet with header, if possible
 *
 * May 23 2008 mbj06032
 * [MAUI_01046086] [BT OBEX] modify OBEX timer: for RFCOMM packet
 * 
 *
 * May 13 2008 mbj06032
 * [MAUI_00093122] Screen always displays recieving X%, x/x.
 * start timer when sending out a OBEX command
 *
 * May 7 2008 mbj06032
 * [MAUI_00318047] MTK:Bluetooth_Assert Fail:OSC(resp)!=(OBRC_CONTINUEIOB_FINAL_observer.c 872-BT
 * 
 *
 * May 6 2008 mbj06032
 * [MAUI_00318047] MTK:Bluetooth_Assert Fail:OSC(resp)!=(OBRC_CONTINUEIOB_FINAL_observer.c 872-BT
 * remove assert
 *
 * Apr 10 2008 mbj06032
 * [MAUI_00653223] [BT OBEX]modify trace info
 * 
 *
 * Mar 21 2008 mbj06032
 * [MAUI_00639839] [BT OBEX] modify trace info
 * 
 *
 * Mar 21 2008 mbj06032
 * [MAUI_00096220] BIP gets disconnected in the mentioned scenario.
 * modify ObParserTxDone func: when check if it is sending an OBEX Packet, not only check its handle, but also check OBPF_PENDING flag
 *
 * Mar 20 2008 mbj06032
 * [MAUI_00096220] BIP gets disconnected in the mentioned scenario.
 * add trace
 *
 * Mar 14 2008 mbj06032
 * [MAUI_00636946] [BT OBEX ] Add trace
 * 
 *
 * Feb 29 2008 mbj06032
 * [MAUI_00725495] BT-Always in "Sending" screen
 * add trace
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <sys/obxalloc.h>
#include <sys/obex_utl.h>
#include "bluetooth_trc.h"
#include "goep.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

static ObStatus ObParserSendPacket(ObexAppHandle *AppHndl, U8 cmd);
static U8 ParseHeaderType(ObexHeaderType headerType);

/*---------------------------------------------------------------------------
 *            ObSetParserInitState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the OBEX parser to its initial state.
 *
 * Return:    void
 */
void ObParserSetInitState(ObexParser *obp)
{
    obp->curStageOff = 0;
    obp->stageLen = 3;
    obp->handle = 0;
    obp->rsp = 0;

    obp->rxState = OBSC_RX_WAIT;

}

/*---------------------------------------------------------------------------
 *            ObParseRx()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process received data for the given OBEX Parser.
 *
 * Return:    void
 */
void ObParseRx(ObexAppHandle *AppHndl, U8* RxBuff, U16 RxLen)
{
    U16         n;
    U8          i;
    ObexOpcode  tmpOp;
    ObexParser *obp = &(AppHndl->parser);
    kal_bool b_ObexPacketDone = FALSE;

    //kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX, AppHndl, obp, obp->rxState, RxBuff[0],RxBuff[1],RxBuff[2]);

    ASSERT(IsObexLocked());

#if JETTEST == XA_ENABLED
    /* If there's a sniffer, send the data there */
    if (OBD(sniffer)) {
        OBD(sniffer)(AppHndl, OBSNIFF_RX, RxBuff, RxLen);
    }
#endif
    
    if (obp->rxState == OBSC_RX_NOP) {
        /* Ignore the data */
        return;
    }

    /* Store the receive buffer and length */
    obp->rxBuff = RxBuff;
    obp->rxLen = RxLen;

    /* Process the data in obp->rxBuff of length obp->rxLen. */
    while (obp->rxLen > 0) {
        ASSERT(obp->stageLen <= 4); 
        ASSERT((obp->curStageOff == 0) || (obp->curStageOff < obp->stageLen));

        n = min((U16)(obp->stageLen - obp->curStageOff), obp->rxLen);
        ASSERT(n <= 4);

        /* Stage the data */
        for (i = obp->curStageOff; n > 0; n--, i++) {
            obp->stageBuff[i] = *(obp->rxBuff);
            (obp->rxBuff)++;
            obp->rxLen--;
            obp->curStageOff++;
        }

        /* Only call the state machine if the data has been completely
         * staged.
         */
        if (obp->curStageOff == obp->stageLen) {
            switch (obp->rxState) {

            case OBSC_RX_WAIT:
                /* We have the start of an OBEX packet. Get the packet length. */
                obp->packetLen = BEtoHost16(obp->stageBuff + 1);

                if (IsServerParser(obp)) {
                    /* This is the server so get the packet opcode */
                    obp->opcode = obp->stageBuff[0];
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_OPCODE,obp->opcode,obp->rxState,obp,obp->packetLen, obp);
                } else {
                    /* This the client so get the packet response */
                    obp->rsp = obp->stageBuff[0];
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_RSP,obp->rsp,obp->packetLen, obp);
                }

                /* Determine the opcode. Some opcodes like CONNECT and 
                 * SetPath have values beyond the length that are not headers.
                 */
                switch (obp->opcode & 0x7f) {
                case OB_OPCODE_CONNECT:
                    /* We need to get the OBEX version flags and
                     * Maximum OBEX packet length. */
                    obp->stageLen = 4;
                    obp->rxState = OBSC_RX_CONNECT;
                    obp->curPacketLen = OB_CONNECT_LEN;     /* 7 */
                    break;

                case OB_OPCODE_SET_PATH:
                    /* Set path command get the flags. The flags are only sent
                     * in the request packet (received by the server). The response
                     * does not include the extra 2 bytes. In this case drop 
                     * into the default case.
                     */
                    if (IsServerParser(obp)) {
                        obp->stageLen = 2;
                        obp->curPacketLen = 5;
                        obp->rxState = OBSC_RX_PATH;
                        break;
                    }

                default:
                    /* The other opcodes only have length followed
                     * by headers.
                     */
                    obp->curPacketLen = 3;
                    goto GiveStartInd;
                }
                break;

            case OBSC_RX_WAIT2:
            case OBSC_RX_WAIT3:
                /* Save the packet length */
                obp->packetLen = BEtoHost16((obp->stageBuff) + 1);
                obp->curPacketLen = 3;

                if (IsServerParser(obp)) {
                    /* If the opcode of this packet does not match the current
                     * opcode or the abort opcode then we have a problem so
                     * disconnect. Only PUT and GET can be aborted so check for
                     * that too. Note that all this only applies to the server.
                     */
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_OPCODE,obp->stageBuff[0],obp->rxState,obp,obp->packetLen, obp);
                     
                    tmpOp = obp->stageBuff[0] & 0x7f;
                    if (tmpOp != (obp->opcode & 0x7f)) {
                        if ((tmpOp != OB_OPCODE_ABORT) || 
                            (((obp->opcode & 0x7f) != OB_OPCODE_PUT) &&
                             ((obp->opcode & 0x7f) != OB_OPCODE_GET))) {
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_OPCODE_MISMATCH, obp->opcode, obp->stageBuff[0]);
                            goto disconnect;
                        } 
                        else {
                            /* We received an abort, indicate it as a new operation */
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_COMPARE,obp->stageBuff[0],obp->opcode);
                            obp->opcode = obp->stageBuff[0];

                            /* If we haven't send a response to the current
                             * operation yet (rxState == WAIT2) then flag the
                             * condition where have two responses to send.
                             */
#ifdef __BT_GOEP_V2__
							// GOEPv2 support send packet without response
#else
                            if (obp->rxState == OBSC_RX_WAIT2)
                            {
                                kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_DEFER_OPER);
                                obp->flags |= OBPF_DEFER_OPER_IND;
                            }
#endif

                            goto GiveStartInd;
                        }
                    }
                    /* This is a command packet so save the opcode */
                    obp->opcode = obp->stageBuff[0];
#ifndef __BT_GOEP_V2__  // Mark for GOEPv2 to avoid check twice push request and someting like that
                    if(obp->rxState != OBSC_RX_WAIT3)
                    {
                        /* Received another command before sent previous response, 
                            this means we are talking to a buggy OBEX client and 
                            We have 2 possible policies to handle such case: 
                           1. Be rudely, disconnect directly.
                           2. Be kindly, just ignore the duplicate packet */
                        /* We used to be rudely, now we decide to be more kindly */
                        kal_trace(BT_TRACE_G6_OBEX,OBEX_RECEIVED_NEW_OBEX_COMMAND_BEFORE_PREVIOUS_RESPONSE_SENT_SUCCESSFULLY);
                        obp->rxState = OBSC_RX_IGNORE;
                        obp->stageLen = 0;
                        obp->headerLen = obp->packetLen - obp->curPacketLen;
                        break;
                    }
#endif
                } 
				else {
                    /* This is a response packet so save the response */
                    obp->rsp = obp->stageBuff[0];
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_RSP,obp->rsp,obp->packetLen, obp);
                }

                /* Now deal with the headers */
                obp->headerLen = 0;
                goto HeaderCheckDone;

            case OBSC_RX_PATH:
                /* The set path flags & constants are in the stage buffer */
                obp->spFlags = obp->stageBuff[0];
                goto GiveStartInd;

            case OBSC_RX_CONNECT:
                /* We got the OBEX Connect parms so save them away. */
                obp->obexVersion = obp->stageBuff[0];
                obp->maxTxPacket = BEtoHost16(obp->stageBuff + 2);
                kal_trace(BT_TRACE_G6_OBEX, OBEX_REMOTE_MTU_VALUE, obp->maxTxPacket);
                /* We should disconnect in response to any OBEX Connect with a 
                 * packet size lower than 255 bytes, but we'll be lenient since
                 * the Nokia 6210 phone passes in an illegal 59-byte packet size.  
                 * It does this since 59 bytes is the maximum amount of OBEX data 
                 * in a 64-byte IrDA frame.
                 */
                if (obp->maxTxPacket < 59) 
					goto disconnect;
				
                obp->flags |= OBPF_CONNECTED;

                kal_trace(BT_TRACE_G6_OBEX,OBEX_RECEIVED_OBEX_CONNECT_OBEX_PACKET_SIZE_xD_BYTES , obp->maxTxPacket);
GiveStartInd:
                /* Call the Command Indication start function and then
                 * check for headers.
                 */
                obp->headerLen = 0;
                obp->eventIndFunc(AppHndl, OBPE_OPER_IND_START);
                
                /* If the Command Interpreter aborted the operation, stop processing. */
                if (obp->rxState == OBSC_RX_NOP)
                    return;
                
                goto HeaderCheckDone;

            case OBSC_RX_PUT_HEAD1:
                /* StageBuff contains the Header type, save it off. */
                obp->header = (ObexHeaderType)obp->stageBuff[0];

                /* Determine how much more of the header to stage based
                 * on it's type
                 */
                obp->stageLen = ParseHeaderType(obp->header);
                obp->rxState = OBSC_RX_PUT_HEAD2;

                /* Update the amount of data received so far in the packet.
                 * Optimize by adding the data were about to stage since we
                 * wont check the length before we process it.
                 */
                obp->curPacketLen += obp->stageLen + 1;
                break;

            case OBSC_RX_PUT_HEAD2:
                /* If staged length is 2 bytes then it's the headers length field */
                if (obp->stageLen == 2) {
                    obp->totalHeaderLen = BEtoHost16(obp->stageBuff)-3;
                    obp->headerLen = obp->totalHeaderLen;
                }

                /* Although frivolous, this allows the application to call
                 * GetHeaderWriteLen() during a OBPE_PUT_HEADER event.
                 * This is important because the app can't distinguish
                 * between OBPE_PUT and OBPE_WRITE events.
                 */
                obp->dataLen = 0;

                /* Tell the next layer we are giving it a header. If the
                 * the header is a single byte or 4 byte quantity then we
                 * are done. If the header is an integer header then the
                 * 4 bytes of the result are in obp->stageBuff. For headers 
                 * with two bytes lengths we need to read more of the header.
                 */
                obp->eventIndFunc(AppHndl, OBPE_PUT_HEADER);
                
                /* If the Command Interpreter aborted the operation, stop processing. */
                if (obp->rxState == OBSC_RX_NOP)
                    return;

                /* If the whole header has been obtained then see if
                 * the packet is done (jump to HeaderCheckDone).
                 */
                if ((obp->stageLen != 2) || (obp->headerLen == 0)) {
                    goto HeaderCheckDone;
                } 
                
                obp->rxState = OBSC_RX_PUT_HEAD3;
                obp->stageLen = 0;
                
                /* Were passing up a pointer to rxBuff, if their is data
                 * present fall through to the next case.
                 */
                if (obp->rxLen == 0) {
                    break;
                }

            case OBSC_RX_PUT_HEAD3:
                /* Write the header. The data is pointed to by obp->rxBuff
                 * and the length is in obp->dataLen. */
                obp->dataLen = min(obp->rxLen, obp->headerLen);
                obp->headerLen -= obp->dataLen;
                obp->curPacketLen += obp->dataLen;

                obp->eventIndFunc(AppHndl, OBPE_WRITE_HEADER);

                /* If the Command Interpreter aborted the operation, stop processing. */
                if (obp->rxState == OBSC_RX_NOP)
                    return;
                
                /* We have processed obp->dataLen bytes in the buffer so update
                 * the buffer variables.
                 */
                obp->rxLen -= obp->dataLen;
                obp->rxBuff += obp->dataLen;

                /* See if we have any more data in the packet. Headers are
                 * suppose to fit completely in a single packet so we could
                 * check this and if an errors occurs abort the operation.
                 */
HeaderCheckDone:
                if (obp->curPacketLen == obp->packetLen) {
                    /* The packet is done. */
					
                    /* Tell the Command Interpreter that the OBEX packet is done. */
                    kal_trace(BT_TRACE_G6_OBEX, OBEX_OBEXPACKET_RECEIVED, obp->packetLen);
                    b_ObexPacketDone = TRUE;

#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED
                    if (IsClientParser(obp))
                    {
                        if ((obp->opcode == (OB_OPCODE_ABORT|OB_FINAL_BIT))
							&& (obp->rsp == (OBRC_CONTINUE|OB_FINAL_BIT)))
                        {
                            /* For ABORT request, client expect SUCCESS (0xA0) response.
                             * If CONTINUE (0x90) is received, then don't stop the timer 
                             */
                        }
						else
					    {
                             OBEX_ClientStopReqTimer(AppHndl);
                        }
                    }
#endif
					
                    /* The packet is done. If the current header is not done then
                     * we are talking to a buggy implementation so disconnect
                     */
                    if (obp->headerLen > 0) {
                        kal_trace(BT_TRACE_G6_OBEX, OBEX_PARSERX_HEADERLEN_ERROR);
                        OS_Report("OBEX: obp->rxLen:%d obp->headerLen:%d obp->curPacketLen:%d", obp->rxLen, obp->headerLen, obp->curPacketLen);
                        goto disconnect;
                    }

                    obp->rxState = OBSC_RX_WAIT2;
                    obp->stageLen = 3;

                    /* Don't indicate OPER_IND if we already have an
                     * outstanding OPER_IND event. However, we need to float
                     * a no-op event through the command interpreter when
                     * OBEX_SERVER_CONS_SIZE is in use because it caches the
                     * start ind and may not have delivered the ABORT_START yet.
                     */
                    if (obp->flags & OBPF_DEFER_OPER_IND) {
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
                        kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSE_RX_DEFER_OPER_FLOAT);

                        obp->eventIndFunc(AppHndl, OBPE_NO_EVENT);
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
                        break;
                    }
                    obp->eventIndFunc(AppHndl, OBPE_OPER_IND);
                } 
				else if (obp->headerLen == 0) {
                    /* Process the next header. We know that there is
                     * more data, so we can safely stage one byte.
                     */
                    obp->rxState = OBSC_RX_PUT_HEAD1;
                    obp->stageLen = 1;
                }
                break;

            case OBSC_RX_IGNORE:
            case OBSC_RX_IGNORE3:

                /* Read data until the whole packet end */
                obp->dataLen = min(obp->rxLen, obp->headerLen);
                obp->curPacketLen += obp->dataLen;
                obp->headerLen -= obp->dataLen;
                obp->rxLen -= obp->dataLen;
                obp->rxBuff += obp->dataLen;

                if (obp->headerLen == 0)
                {
                    if (obp->rxState == OBSC_RX_IGNORE3)
                        obp->rxState = OBSC_RX_WAIT3;
                    else                        
                        obp->rxState = OBSC_RX_WAIT2;
                    obp->stageLen = 3;
                }

                break;

            default:
                ASSERT(0);
                break;
            
            } /* End switch(rxState) */
            
            /* In PUT_HEAD1 the stagelen was added to curPacketLen. This
             * allows us to check here if were trying to stage more data
             * than their is in the packet. This can detect packets that are
             * too long with garbage on the end. It can also detect undersized
             * CONNECT or SETPATH operations.
             */
            if (obp->curPacketLen > obp->packetLen) {
                /* We processed more data than the packet length. This
                 * indicates a buggy implementation so disconnect.
                 */
                kal_trace(BT_TRACE_G6_OBEX, OBEX_PARSERX_PACKETLEN_ERROR, obp->packetLen, obp->curPacketLen);
                goto disconnect;
            }
            obp->curStageOff = 0;
        
        } /* End if (curStageOff == stageLen) */
    
    } /* End While() */


#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED
    if (IsClientParser(obp) && !b_ObexPacketDone)
    {
        OBEX_ClientRestartReqTimer(AppHndl);
    }
#endif
    
    return;

disconnect:
    GOEP_Report("[OBEX][ERR] ObparseRx error and cause disconnect");
    ObParserDisconnect(obp);

}


/*---------------------------------------------------------------------------
 *            ObParserSendPacket()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an OBEX packet. Calling this function starts the xmit
 *            of an OBEX packet. The parser will call the command interpreter
 *            iteratively looking for enough data to fill an OBEX packet or 
 *            until there is no more data. This is done via ObParserTxBuffAvail().
 *
 * cmd: Command interpreter, header TX state ex.HB_CLIENT_HEADERS, HB_CLIENT_OBJECT.. 
 */
static ObStatus ObParserSendPacket(ObexAppHandle *AppHndl, U8 cmd)
{
    U8          *buff;
    U8          overhead;
    U16         len, obPacketLen, obexPacketSize;
    ObStatus    status;
#if (OBEX_ROLE_CLIENT == XA_ENABLED) && (OBEX_SESSION_SUPPORT == XA_ENABLED)
    ObexClientSession *session;
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) && (OBEX_SESSION_SUPPORT == XA_ENABLED) */
    void        *ObexApp = AppHndl;
    ObexParser  *obp = &(AppHndl->parser);
    ObexRespCode ReadResult = OBRC_SUCCESS;
	U8 new_cmd = cmd;
    U32 i;
    U8 bConnType = 0;

    ASSERT(IsObexLocked());
    ASSERT(!(obp->flags & OBPF_PENDING));

    //SRM_Report("[SRM] ObParserSendPacket rxState:%d cmd:0x%x", obp->rxState, cmd);

    if(obp->rxState == OBSC_RX_NOP)
    {
        /* The case that OBEX connection is already disconnected */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SEND_PACKET_WHEN_TRANSPORT_LAYER_IS_DISCONNECTED);
        return XA_STATUS_DISCONNECT;
    }

    if ((obp->opcode & 0x7F) != OB_OPCODE_ABORT) {
        /* An Abort packet might be sent out during the receipt of a packet. */
        ASSERT(obp->rxState == OBSC_RX_WAIT2 || obp->rxState == OBSC_RX_WAIT);
    }

    len = OBSTACK_MaxTxSize(obp->stack);   /* We assume Rx PDU size equals Tx */
    if (len == 0)
    {
        /* If RF channel is closing, it's frame size is 0, then just return */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_BUFAVAIL_MAX_TX_SIZE_ZERO, obp);
        return XA_STATUS_DISCONNECT;
    }

    /* Send the packet and header block data. */
    buff = OBSTACK_GetTxBuffer();
    if (buff == 0) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_NO_FREETXBUFFER, obp, cmd);
        obp->flags |= OBPF_PENDING;
        return OB_STATUS_PENDING;
    }
    
    len = OBSTACK_MaxTxSize(obp->stack);   /* We assume Rx PDU size equals Tx */

    if ((obp->opcode & 0x7F) == OB_OPCODE_CONNECT) {
        /* 
         * This is an OBEX Connect request or response packet so write our
         * Connect parameters. We calculate the optimal OBEX Packet size,
         * based on our PERFORMANCE_MULTIPLIER and the TPDU size.
         */
         GOEP_Report("OBEX: AppHndl:0x%x mru:%d len:%d", AppHndl, AppHndl->mru, len);
        if(AppHndl->mru)
        {
            obexPacketSize = AppHndl->mru; /* use app defined MRU */
        }
        else
        {
            obexPacketSize = len * OBEX_PERFORMANCE_MULTIPLIER;
            obexPacketSize = max(obexPacketSize, OBEX_DEFAULT_PACKET_SIZE);
            AppHndl->mru = obexPacketSize;
	    GOEP_Report("OBEX: AppHndl:0x%x mru to mru:%d", AppHndl, AppHndl->mru);			
        }

#if BT_GOEP_V2 == XA_ENABLED
        if( IsServerParser(obp)){
            bConnType = ((ObexServerApp *)ObexApp)->trans.ObexServerBtTrans.server.bConnType;
            kal_trace(BT_TRACE_G6_OBEX, OBEX_BCONNTYPE, ObexApp, bConnType);
            if( ((ObexServerApp *)ObexApp)->trans.ObexServerBtTrans.server.bConnType == OBEX_TP_L2CAP_ONLY ){
                GOEP_Report("[OBEX] ObParserSendPacket OPCODE_CONNECT server  mru as l2cap_mtu len:%d", len);
                obexPacketSize = len;
            }
        }
        if( IsClientParser(obp)){
        	bConnType = ((ObexServerApp *)ObexApp)->trans.ObexClientBtTrans.client.bConnType;
        	kal_trace(BT_TRACE_G6_OBEX, OBEX_BCONNTYPE, ObexApp, bConnType);
                if( ((ObexClientApp *)ObexApp)->trans.ObexClientBtTrans.client.bConnType == OBEX_TP_L2CAP_ONLY ){
                    GOEP_Report("[OBEX] ObParserSendPacket OPCODE_CONNECT client mru as l2cap_mtu len:%d", len);
                    obexPacketSize = len;

                    if( obexPacketSize > (L2CAP_MAXIMUM_MTU-BT_PACKET_HEADER_LEN) ){
                        GOEP_Report("[OBEX] adjust to max l2cap pdu size len:%d", len);
                        obexPacketSize = L2CAP_MAXIMUM_MTU-BT_PACKET_HEADER_LEN;
                    }
                }
            }            
#endif
        
        buff[3] = 0x10;     /* OBEX protocol version 1.0 */
#if BT_GOEP_V2 == XA_ENABLED
        if( 0 != AppHndl->obexver && 0x12 == AppHndl->obexver && bConnType == OBEX_TP_L2CAP_ONLY ){ 
            /* only support to change version as 0x12 */
            GOEP_Report("[OBEX] Use OBEX version 0x%x", AppHndl->obexver);
            buff[3] = AppHndl->obexver;
        }
#else

#endif
        buff[4] = 0x00;     /* No Connect flags */
        StoreBE16(buff+5, obexPacketSize); /* Our OBEX Packet size */
        kal_trace(BT_TRACE_G6_OBEX, OBEX_VERSION_MTU, obexPacketSize, buff[3]);

        overhead = OB_CONNECT_LEN;

    } else if (IsClientParser(obp) && 
               ((obp->opcode & 0x7F) == OB_OPCODE_SET_PATH)) {
        /* This is a set path packet for the client send the client
         * set path parameters. The server does not send parameters in
         * the response (a slight inconsistency in the OBEX protocol).
         */
        overhead = 5;
        buff[3] = obp->spFlags;
        buff[4] = 0;
    } else {
        overhead = 3;   /* For OBEX opcode and packet length */
    }

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if ((obp->opcode & 0x7F) != OB_OPCODE_SESSION) {
        if (IsClientParser(obp)) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
            session = ((ObexClientApp *)ObexApp)->client.activeSession;
            if (session) {
                /* Add OBEX Session Sequence Number header to non-Session commands */
                buff[overhead++] = OBEXH_SESSION_SEQ_NUM;
                buff[overhead++] = session->seqNum;
                if (((ObexClientApp *)ObexApp)->client.seqNumError == ERR_NONE)
                    session->nextSeqNum++;
            }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
        }
        else if (IsServerParser(obp)) {
#if OBEX_ROLE_SERVER == XA_ENABLED
            if (((ObexServerApp *)ObexApp)->server.activeSession) {
                /* Add OBEX Session Sequence Number header to non-Session commands */
                buff[overhead++] = OBEXH_SESSION_SEQ_NUM;
                buff[overhead++] = ((ObexServerApp *)ObexApp)->server.activeSession->seqNum;
            }
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
        }
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    /* Update len & obPacketLen value. 
     * overhead is opcode&its fields. obp->header is extra header block 
     * len is temp variable to generate obPacketLen value
     * obPacketLen is the final Obex Packet Length
     */
    if (obp->handle != 0) {
        /* We have a header block so read as much as we can
         * into the tx buffer.
         */
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
        if (ObGetHeaderBlockLen(ObexApp, obp->handle) == 0) {
            /* The object size is unknown, offer to read as much as will
             * fit. If we are not "connected", we probably need to reduce
             * the packet size to the default size (from the frame size). 
             */
            if (obp->flags & OBPF_CONNECTED)
                len = min(obp->maxTxPacket, len);
            else len = min(ObParserDefaultObexPacketSize(), len);

            len = ObReadHeaderBlockFlex(ObexApp, obp->handle, buff + overhead, (U16)(len - overhead));
            obPacketLen = overhead + len;
            len = obPacketLen;
#if OBEX_GET_USE_END_OF_BODY == XA_ENABLED
			if (IsServerParser(obp)) {
				/* ObReadHeaderBlockFlex could have changed the response code to SUCCESS 
				 * if it determined that the last portion of data was being sent out. Make
				 * sure we account for a potentially new response code
				 */
				new_cmd = ((ObexServerApp *)ObexApp)->server.resp;
				obp->rsp = ((ObexServerApp *)ObexApp)->server.resp;
			}
#endif /* OBEX_GET_USE_END_OF_BODY == XA_ENABLED */
        } else 
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
        {
            /* determine max length here */
            Assert(len  > overhead);

            /* revised for PBAP server */
            if (IsServerParser(obp) && (((ObexServerApp *)ObexApp)->server.canFitOnePacket))
            {
                obPacketLen = ObServerGetHeaderBlockLen_FitOnePkt(ObexApp, obp->handle) + overhead;
                len = min(len, obPacketLen); /* determine the min(OBEX, RFOMM) to tx length */
                ObServerReadHeaderBlock_FitOnePkt(ObexApp, obp->handle, buff + overhead, (U16)(len - overhead));            
            }
            else
            {
                obPacketLen = ObGetHeaderBlockLen(ObexApp, obp->handle) + overhead;
                //SRM_Report("[CONN] ReadBody obPacketLen:%d len:%d overhead:%d", obPacketLen, len, overhead);
                len = min(len, obPacketLen); /* determine the min(OBEX, RFOMM) to tx length */
                ReadResult = ObReadHeaderBlock(ObexApp, obp->handle, buff + overhead, (U16)(len - overhead));
            }
            
            /* ObReadHeaderBlock could have changed the response code to failed */
            if (IsServerParser(obp) && (cmd != ((ObexServerApp *)ObexApp)->server.resp)) 
            {
                new_cmd = ((ObexServerApp *)ObexApp)->server.resp;
                obp->rsp = ((ObexServerApp *)ObexApp)->server.resp;
                
                kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSER_SENDPACKET, AppHndl, obp, cmd);

                /* clear the data length */
				OSC(txLength) = 0;
            }

			/* ObReadHeaderBlock may have some read error */
			if (IsClientParser(obp) && (ReadResult != OBRC_SUCCESS))
			{
			    /* for client, read error occured, send ABORT request */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSER_SEND_ABORT_FOR_READ_ERROR,ReadResult);
			    new_cmd = (OB_OPCODE_ABORT|OB_FINAL_BIT);
				obPacketLen = 3; /* ABORT request packet len is 3 */
				len = 3;

                /* clear the data length */
				OCC(txLength) = 0;
            }
        }
    } else {
        /* Only Send the Opcode+Field+ObexMandatoryHeader */
        len = overhead;
        obPacketLen = overhead;
    }

#if XA_ERROR_CHECK == XA_ENABLED
    /* Insure that we're not overflowing the OBEX packet size */
    if (((obp->flags & OBPF_CONNECTED) && (obPacketLen > obp->maxTxPacket)) ||
         (!(obp->flags & OBPF_CONNECTED) && (obPacketLen > ObParserDefaultObexPacketSize()))) {
        kal_trace(BT_TRACE_G6_OBEX, OBEX_PACKET_SIZE_OVERFLOW, obPacketLen);
        ASSERT(0);
    }
#endif /* XA_DEBUG == XA_ENABLED */
    

    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSER_SENDPACKET, AppHndl, obp, new_cmd, obPacketLen);
    AppHndl->SendingRFPktNum = 0;

    /* Put in the cmd and the length */

    buff[0] = new_cmd;
    buff[1] = (U8)(obPacketLen >> 8);
    buff[2] = (U8) obPacketLen;

    /* Now send this buffer */
    status = OBSTACK_SendTxBuffer(obp->stack, buff, len);

    if (status == OB_STATUS_PENDING)
    {
        AppHndl->SendingRFPktNum++;
    }


#if JETTEST == XA_ENABLED
    /* If there's a sniffer, send it there as well */
    if ((status == OB_STATUS_PENDING) || (status == OB_STATUS_SUCCESS)) {
        if (OBD(sniffer))
            OBD(sniffer)(AppHndl, OBSNIFF_TX, buff, len);
    }
#endif

    /* See if there is more header block data to send. */
    if (ObGetHeaderBlockLen(ObexApp, obp->handle) > 0) {
		//kal_trace(BT_TRACE_G6_OBEX,OBEX_CALL_OBPARSE_BUFFAVAIL, AppHndl, 4);
        ObParserTxBuffAvail(AppHndl);
    } else {
        /* We have sent the complete packet so set the handle to 0 */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_TX_DONE, obp);
        obp->handle = 0;
    }
    
    return status;
}


/*---------------------------------------------------------------------------
 *            ObServerSendResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a response. This function should only be called after
 *            A call to obp->eventIndFunc() with the OBPE_OPER_IND event.
 *
 * Return:    void
 */
ObStatus ObServerSendResponse(ObexAppHandle *AppHndl, ObexRespCode rsp, ObHeaderBlock hb)
{
   ObexParser *obp = &(AppHndl->parser);

   kal_trace(BT_TRACE_G6_OBEX,OBEX_SENDRESPONDSE, obp, rsp, hb);

   ASSERT(IsServerParser(obp) && !IsClientParser(obp));
   ASSERT(rsp & OB_FINAL_BIT);
   ASSERT(obp->stack);
   
   if (obp->handle != 0 || obp->flags & OBPF_PENDING) {
       GOEP_Report("[OBEX][WRN] Return Busy obp->handle:%d obp->flags:%d", obp->handle, obp->flags); // 101110: ftp server get-abort test
       return OB_STATUS_BUSY;
   }
   
   /* Save the handle and response */
   obp->handle = hb;
   obp->rsp = rsp;

   return ObParserSendPacket(AppHndl, rsp);

}

/*---------------------------------------------------------------------------
 *            ObClientSendCommand()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an OBEX command. 
 *
 * Return:    void
 */
ObStatus ObClientSendCommand(ObexAppHandle *AppHndl, ObexOpcode cmd, ObHeaderBlock hb)
{
   ObexParser *obp = &(AppHndl->parser);

   ASSERT(IsClientParser(obp) && !IsServerParser(obp));
   ASSERT(obp->stack);

   kal_trace(BT_TRACE_G6_OBEX,OBEX_SENDCOMMAND, obp, cmd, hb);

   /* Save the handle and opcode */
   obp->handle = hb;
   obp->opcode = cmd;

   return ObParserSendPacket(AppHndl, cmd);
}

/*---------------------------------------------------------------------------
 *            ObParserTxBuffAvail()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Continues the transmit of an OBEX Packet. This function is
 *            utilized when the OBEX packet size is greater than the transport
 *            PDU size. It is called by the parser and by the transport
 *            when a transmit packet is available.
 *
 * Return:    void
 */
void ObParserTxBuffAvail(ObexAppHandle *AppHndl)
{
    U8         *buff;
    U16         len, blockLen;
    ObStatus    status;
    void       *ObexApp = AppHndl;
    ObexParser *obp = &(AppHndl->parser);
	ObexRespCode    rcode;


    ASSERT(IsObexLocked());
    
    /* If the PENDING flag is set, we were about to send a command
     * but there were no packets available. Re-run the send logic.
     */
    if (obp->flags & OBPF_PENDING) {
        obp->flags &= ~OBPF_PENDING;

        kal_trace(BT_TRACE_G6_OBEX,OBEX_RESEND_PENDING_PACKET, obp);
        
        if (IsClientParser(obp))
            status = ObParserSendPacket(AppHndl, obp->opcode);
        else 
			status = ObParserSendPacket(AppHndl, obp->rsp);

        if (status != OB_STATUS_PENDING && status != OB_STATUS_SUCCESS) {
            goto HandleTxError;
        }
        return;
    }

    /* If the handle is non-zero we have more data to send. Loop
     * until we're out of data or transmit buffers.
     */
    if (obp->handle != 0) {

        kal_trace(BT_TRACE_G6_OBEX,OBEX_OBPARSE_TXBUFAVAIL, obp);

        while ((blockLen = ObGetHeaderBlockLen(ObexApp, obp->handle)) > 0) {
            /* get Tx size */ 
            len = min(OBSTACK_MaxTxSize(obp->stack), blockLen);
            if (len == 0)
            {
                /* If RF channel is closing, it's frame size is 0, then just return */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_BUFAVAIL_MAX_TX_SIZE_ZERO, obp);
                break;
            }

            /* Get a tx buffer */
            buff = OBSTACK_GetTxBuffer();
            if (buff == NULL) 
			{
			    /* 0x1F in trace info is only a temp for this trace info */
		        kal_trace(BT_TRACE_G6_OBEX,OBEX_NO_FREETXBUFFER, obp, 0x1F);
                break;
            }
            
            rcode = ObReadHeaderBlock(ObexApp, obp->handle, buff, len);
			if (rcode != OBRC_SUCCESS)
            {                
				kal_trace(BT_TRACE_G6_OBEX,OBEX_DISCONNECT_FOR_READHEADBLK_ERR, rcode);
                OBPROT_ReturnTxBuffer(buff);
				goto HandleTxError;
			}

		    //kal_trace(BT_TRACE_G6_OBEX,OBEX_SENDTXBUFFER, AppHndl, obp);

            status = OBSTACK_SendTxBuffer(obp->stack, buff, len);

            if (status == OB_STATUS_PENDING)
            {
                AppHndl->SendingRFPktNum ++;
            }            
            
#if JETTEST == XA_ENABLED
            /* If there's a sniffer, send it there as well */
            if ((status == OB_STATUS_PENDING) || (status == OB_STATUS_SUCCESS)) {
                if (OBD(sniffer))
                    OBD(sniffer)(AppHndl, OBSNIFF_TX, buff, len);
            }
#endif
            if ((status != OB_STATUS_PENDING) && (status != OB_STATUS_SUCCESS)) {
HandleTxError:
                GOEP_Report("[OBEX][ERR] HandleTxError!");
                ObParserDisconnect(obp);
                return;
            }
        } /* End While() */

        if (blockLen == 0) {
            /* We are done with the Header block */
            //kal_trace(BT_TRACE_G6_OBEX,OBEX_TX_DONE, obp);
            GOEP_Report("[OBEX] ObParserTxBuffAvail ====obp 0x%x ObParserTxDone =====", obp);
            obp->handle = 0;
        }
    } 
}

/*---------------------------------------------------------------------------
 *            ObParserSetAbort()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is used to indicate to the parser whether an Abort
 *            operation must be started as soon as the last command has been
 *            sent.  Don't bother waiting for the OBEX Server to respond to
 *            current packet before sending an Abort.  The OBEX spec.
 *            indicates an Abort command can violate the request/response
 *            standard.
 *
 * Return:    void
 */
void ObParserSetAbort(ObexParser *obp)
{
    kal_trace(BT_TRACE_G6_OBEX,OBEX_SETABORT, obp);

    ASSERT(IsObexLocked());
    ASSERT(IsClientParser(obp));

    /* Inform the parser that an immediate abort is requested.
     * However, we must process this flag later, just in case
     * we are transmitting a packet when we request the 
     * immediate abort.
     */
    obp->abort = TRUE;
}

/*---------------------------------------------------------------------------
 *            ObParserTxDone()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is used to indicate the completion of a transmit
 *            request.
 *
 * Return:    void
 */
void ObParserTxDone(ObexAppHandle *AppHndl)
{
    ObStatus        status;
    ObexParser      *obp = &(AppHndl->parser);
    ObexClientApp * ObexApp = (ObexClientApp *) AppHndl;

    ASSERT(IsObexLocked());

    /* If were still sending an OBEX Packet we have nothing to do here. */
    if ((obp->handle != 0) || (obp->flags & OBPF_PENDING))
	{
        return;
    }
    
    /* A send just completed and we have no more data to send. */
    kal_trace(BT_TRACE_G6_OBEX,OBEX_TX_DONE, obp);

    if (IsClientParser(obp)) {
        if (obp->abort)
        {
            /* Send an Abort now that we are done sending the previous packet */
            obp->opcode = (OB_OPCODE_ABORT|OB_FINAL_BIT);
            OCC(txLength) = OCH(txLength);
            OCH(txLength) = 0;
            OCC(txOffset) = 0;
            //Report(("[OBEX]Client send ABORT packet...\n"));
            status = ObParserSendPacket(AppHndl, obp->opcode);
            obp->abort = FALSE;
            GOEP_Report("[OBEX] OCC(txLength) reset - ObParserTxDone");
        }
        else if (AppHndl->SendingRFPktNum == 0)
        {
            /* a command has sent out, start timer */
#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED
#ifdef __BT_GOEP_V2__
            if( OBEX_IsSRMMode(AppHndl) == TRUE ){
                //GOEP_Report("[OBEX][Timer] ignore the StartReqTimer");    
            }else{
                OBEX_ClientStartReqTimer(AppHndl, obp->opcode);
            }
#else
            OBEX_ClientStartReqTimer(AppHndl, obp->opcode);
#endif
#endif
        }
		
        /* has tx_complete to upper layer for handling SRM mode*/
        obp->eventIndFunc(AppHndl, OBPE_TX_COMPLETE);
        
    }

    if (IsServerParser(obp)) {
        /* If we are expected to send two responses, don't
         * complete the operation now. 
         */
        if (obp->flags & OBPF_DEFER_OPER_IND) {
            kal_trace(BT_TRACE_G6_OBEX,OBEX_REMOVE_DEFER, obp);
            
            obp->flags &= ~OBPF_DEFER_OPER_IND;
            obp->eventIndFunc(AppHndl, OBPE_OPER_IND);
            return;
        }

        kal_trace(BT_TRACE_G6_OBEX, OBEX_PARSE_TXDONE_SENDINGRFPNUM, AppHndl, AppHndl->SendingRFPktNum);
        if (AppHndl->SendingRFPktNum == 0) 
        {

            /* Authough response has been sent, we still need
                to ignore the previous whole packet */
            if (obp->rxState == OBSC_RX_IGNORE)
                obp->rxState = OBSC_RX_IGNORE3;
            else
                obp->rxState = OBSC_RX_WAIT3;

            /* only notify OBPE_OPER_COMPLETE once for one command */
            /* If the response code indicates that the operation is complete,
             * tell the command interpreter to complete the operation.
             */
            if (obp->rsp != (OBRC_CONTINUE | OB_FINAL_BIT)) {
                ObParserSetInitState(obp);
                obp->eventIndFunc(AppHndl, OBPE_OPER_COMPLETE);
            }        
            else{
                /* has tx_complete to upper layer for handling SRM mode*/
                obp->eventIndFunc(AppHndl, OBPE_TX_COMPLETE);
            }
        }
    }
}
            
/*---------------------------------------------------------------------------
 *            ObParserDisconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the OBEX parser connection.
 *
 * Return:    void
 */
void ObParserDisconnect(ObexParser *obp)
{
    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSER_DISONNECT, obp);
    
    ASSERT(IsObexLocked());

    obp->rxLen = 0;
    obp->handle = 0;
    obp->rxState = OBSC_RX_NOP;
    
    OBSTACK_Disconnect(obp->stack);
}

/*---------------------------------------------------------------------------
 *            ObParserDisconnectInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called from the stack indicating that the connection has
 *            disconnected.
 *
 * Return:    void
 */
void ObParserDisconnectInd(ObexAppHandle *AppHndl)
{
    ObexParser *obp = &(AppHndl->parser);

    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSER_DISONNECTIND, obp);

    ASSERT(IsObexLocked());
    
#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED
    if (IsClientParser(obp))
    {
        OBEX_ClientStopReqTimer(AppHndl);
    }
#endif
    
    ObParserSetInitState(obp);

    ObParserUnlinkTransport(obp); /* Unlink Transport */
    obp->flags &= ~(OBPF_CONNECTED|OBPF_PENDING|OBPF_DEFER_OPER_IND);
    obp->eventIndFunc(AppHndl, OBPE_DISCONNECT);
}

/*---------------------------------------------------------------------------
 *            ObParserLinkTransport()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called from the stack to link the parser to the transport
 *            connection. If the parser is already linked it fails the 
 *            request.
 *
 * Return:    void
 */
BOOL ObParserLinkTransport(ObexParser *obp, ObexTransport *trans)
{
    ASSERT(IsObexLocked());
    if (obp->stack)
        return FALSE;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_PARSER_LINKTRANS, obp);
    obp->stack = trans;
    ObParserSetInitState(obp);

    return TRUE;
}

/*---------------------------------------------------------------------------
 *            ObParserMaxHbSize()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determine the maximum amount of data the client or server 
 *            can send in the current packet, based on the opcode.
 *
 * Return:    Number of bytes
 */
U16 ObParserMaxHbSize(ObexParser *obp, ObexOpcode opcode)
{
    U16             maxHb;
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    ObexAppHandle  *AppHndl = (ObexAppHandle*)ContainingRecord(obp, ObexAppHandle, parser);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
    
    if (obp->flags & OBPF_CONNECTED){
        maxHb = obp->maxTxPacket - 3; 
    }else{
        maxHb = ObParserDefaultObexPacketSize() - 3;
    }


    if (opcode == OB_OPCODE_CONNECT)
        maxHb -= 4;
    else if ((opcode == OB_OPCODE_SET_PATH) && IsClientParser(obp))
        maxHb -= 2;
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    else if (opcode == OB_OPCODE_SESSION) {
        /* Subtract the maximum amount of internal header space
         * required for a session request or response.
         */
        if (IsClientParser(obp))
            maxHb -= 54;
        else if (IsServerParser(obp))
            maxHb -= 56;
    }
    
    if (opcode != OB_OPCODE_SESSION) {
        /* Session operations do not have a sequence number header */
        if (IsClientParser(obp)) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
            if (((ObexClientApp*)AppHndl)->client.activeSession)
                maxHb -= 2; /* Sequence Number header */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
        } else {
            ASSERT(IsServerParser(obp));
#if OBEX_ROLE_SERVER == XA_ENABLED
            if (((ObexServerApp*)AppHndl)->server.activeSession)
                maxHb -= 2; /* Sequence Number header */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
        }
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    ASSERT (maxHb > 0);
    return maxHb;
}


/*---------------------------------------------------------------------------
 *            ParseHeaderType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determine the number of bytes to stage based on the header type.
 *
 * Return:    Number of bytes to stage
 */
static U8 ParseHeaderType(ObexHeaderType headerType)
{
    U8 num = 1;

    switch (headerType & 0xc0) {
    case 0x00:
    case 0x40:
        num = 2;
        break;
    case 0xc0:
        num = 4;
        break;
    }
    return num;
}


/*---------------------------------------------------------------------------
 *            ObParserIsSendingData()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Check if the paser is busy : 
 *              if ((handle != 0) || (OBPF_PENDING)) then busy
 *
 * Return:    TRUE: busy, FALSE : free
 */
BOOL ObParserIsSendingData(ObexParser *obp)
{
    if ((obp->handle != 0) || (obp->flags & OBPF_PENDING))
        return TRUE;
    else
        return FALSE;
}


