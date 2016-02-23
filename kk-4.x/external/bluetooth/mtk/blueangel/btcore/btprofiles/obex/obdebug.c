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
 * File:        obdebug.c
 *
 * Description: This file contains OBEX debugging code.
 *
 * Created:     Jan 6, 2000
 *
 * Version:     MTObex 3.4
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 * Mar 9 2008 mbj06032
 * [MAUI_00093193] [1] ASSERT Fail: (status == BT_STATUS_PENDING)|| (obxbtstk.c 236 - BT
 * modify trace info
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <sys/obxalloc.h>
//#include "bluetooth_trc.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

//#if XA_DEBUG == XA_ENABLED
char *pServerEvent(ObServerEvent event);

/*
 * Verify that the current event is proper given the last event.
 */
ObServerEvent ServerEventVerifier(ObServerEvent lastEvent, ObServerEvent event)
{
    BOOL   ok = FALSE;


    /* Check the validity of the even for the appropriate server */
    switch (lastEvent) {
        case OBSE_NO_EVENT:
            ok = (event == OBSE_CONNECTED);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED 
            ok = (ok || event == OBSE_DISCOVERY_FAILED || event == OBSE_NO_SERVICE_FOUND);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
            break;

#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED 
        case OBSE_DISCOVERY_FAILED:
        case OBSE_NO_SERVICE_FOUND:
            ok = (event == OBSE_DISCOVERY_FAILED || event == OBSE_NO_SERVICE_FOUND || event == OBSE_CONNECTED);
            break;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
            
#if OBEX_SESSION_SUPPORT == XA_ENABLED
        case OBSE_SESSION_SUSPENDED:
            ok = (event == OBSE_DISCONNECT);
            /* Drop into next case. */
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

        case OBSE_COMPLETE:
#if OBEX_SESSION_SUPPORT == XA_ENABLED
            ok = (event == OBSE_RESUME_OPER);
#endif
        case OBSE_ABORTED:
        case OBSE_CONNECTED:
            ok = (ok || event == OBSE_PUT_START || event == OBSE_GET_START || event == OBSE_SET_PATH_START ||
                  event == OBSE_CONNECT_START || event == OBSE_DISCONNECT_START || 
                  event == OBSE_ABORT_START || event == OBSE_DISCONNECT ||
				  event == OBSE_ACTION_START
				  );
#if OBEX_SESSION_SUPPORT == XA_ENABLED 
            ok = (ok || event == OBSE_SESSION_START || event == OBSE_SESSION_SUSPENDED);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
            break;
            
        case OBSE_DISCONNECT:
            ok = (event == OBSE_CONNECTED || event == OBSE_DISCONNECT);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED 
            ok = (ok || event == OBSE_DISCOVERY_FAILED || event == OBSE_NO_SERVICE_FOUND);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
            break;
            
        case OBSE_PUT_START:
            ok = (event == OBSE_PROVIDE_OBJECT || event == OBSE_DELETE_OBJECT || event == OBSE_HEADER_RX ||
                  event == OBSE_ABORTED);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;

        case OBSE_GET_START:
            ok = (event == OBSE_PROVIDE_OBJECT || event == OBSE_HEADER_RX || event == OBSE_ABORTED);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;
            
        case OBSE_SET_PATH_START:
            ok = (event == OBSE_ABORTED || event == OBSE_HEADER_RX);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#else /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            ok = (ok || event == OBSE_PRECOMPLETE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;
         case OBSE_ACTION_START:
            ok = (event == OBSE_ABORTED || event == OBSE_HEADER_RX);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#else /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            ok = (ok || event == OBSE_PRECOMPLETE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;           
        case OBSE_CONNECT_START:
        case OBSE_DISCONNECT_START:
            ok = (event == OBSE_COMPLETE || event == OBSE_ABORTED || event == OBSE_HEADER_RX);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
#if OBEX_SESSION_SUPPORT == XA_ENABLED 
            ok = (ok || event == OBSE_SUSPENDED);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
            break;
            
        case OBSE_ABORT_START:
            /* OBSE_ABORTED is invalid!!! */
            ok = (event == OBSE_COMPLETE || event == OBSE_HEADER_RX || event == OBSE_ABORTED);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;
            
#if OBEX_SESSION_SUPPORT == XA_ENABLED
        case OBSE_SESSION_START:
            ok = (event == OBSE_ABORTED || event == OBSE_COMPLETE || event == OBSE_HEADER_RX ||
                  event == OBSE_SESSION_PARMS_RX || event == OBSE_PROVIDE_SESSION);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;

        case OBSE_PROVIDE_SESSION:
            ok = (event == OBSE_ABORTED || event == OBSE_HEADER_RX || event == OBSE_COMPLETE);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;

        case OBSE_SESSION_PARMS_RX:
            ok = 0;
            break;

        case OBSE_RESUME_OPER:
            ok = (event == OBSE_DELETE_OBJECT || event == OBSE_HEADER_RX || event == OBSE_ABORTED ||
                  event == OBSE_PRECOMPLETE || event == OBSE_COMPLETE || event == OBSE_SUSPENDED);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

        case OBSE_HEADER_RX:
            ok = (0);
            break;
            
        case OBSE_PROVIDE_OBJECT:
            ok = (event == OBSE_ABORTED || event == OBSE_HEADER_RX);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#else /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            ok = (ok || event == OBSE_PRECOMPLETE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
#if OBEX_SESSION_SUPPORT == XA_ENABLED 
            ok = (ok || event == OBSE_SUSPENDED);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
            break;

#if OBEX_SESSION_SUPPORT == XA_ENABLED 
        case OBSE_SUSPENDED:
            ok = (event == OBSE_DISCONNECT);
            break;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
            
        case OBSE_DELETE_OBJECT:
            ok = (event == OBSE_PRECOMPLETE || event == OBSE_ABORTED);
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
            ok = (ok || event == OBSE_SEND_RESPONSE);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            break;
            
        case OBSE_PRECOMPLETE:
            ok = (event == OBSE_COMPLETE || event == OBSE_ABORTED || event == OBSE_ABORT_START);
            break;

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
        case OBSE_SEND_RESPONSE:
            ok = (event == OBSE_SEND_RESPONSE || event == OBSE_COMPLETE || event == OBSE_PROVIDE_OBJECT ||
                  event == OBSE_HEADER_RX || event == OBSE_ABORTED || event == OBSE_ABORT_START ||
                  event == OBSE_PRECOMPLETE);
#if OBEX_SESSION_SUPPORT == XA_ENABLED 
            ok = (ok || event == OBSE_SUSPENDED);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
            break;
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
        default:
            ok = 0;
            break;
    }
    
    if (!ok) {
        kal_trace(BT_TRACE_G6_OBEX, OBEX_VERIFY_SERVER_EVENT_FAILED,
                    event, lastEvent);
        
    }
        
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if (event != OBSE_HEADER_RX && event != OBSE_SESSION_PARMS_RX) {
#else
    if (event != OBSE_HEADER_RX) {
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        return event;
    }
    return lastEvent;
}

char *pServerEvent(ObServerEvent event)
{
    switch (event) {
        case OBSE_NO_EVENT:         return "OBSE_NO_EVENT";
        case OBSE_CONNECTED:        return "OBSE_CONNECTED";
        case OBSE_DISCONNECT:       return "OBSE_DISCONNECT";
        case OBSE_ABORTED:          return "OBSE_ABORTED";
        case OBSE_COMPLETE:         return "OBSE_COMPLETE";
        case OBSE_PUT_START:        return "OBSE_PUT_START";
        case OBSE_GET_START:        return "OBSE_GET_START";
        case OBSE_SET_PATH_START:   return "OBSE_SET_PATH_START";
        case OBSE_CONNECT_START:    return "OBSE_CONNECT_START";
        case OBSE_DISCONNECT_START: return "OBSE_DISCONNECT_START";
        case OBSE_ABORT_START:      return "OBSE_ABORT_START";
        case OBSE_HEADER_RX:        return "OBSE_HEADER_RX";
        case OBSE_PROVIDE_OBJECT:   return "OBSE_PROVIDE_OBJECT";
        case OBSE_DELETE_OBJECT:    return "OBSE_DELETE_OBJECT";
        case OBSE_PRECOMPLETE:      return "OBSE_PRECOMPLETE";
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
        case OBSE_SEND_RESPONSE:    return "OBSE_SEND_RESPONSE";
#endif
#if OBEX_SESSION_SUPPORT == XA_ENABLED
        case OBSE_SESSION_START:    return "OBSE_SESSION_START";
        case OBSE_SESSION_SUSPENDED:return "OBSE_SESSION_SUSPENDED";
        case OBSE_SESSION_PARMS_RX: return "OBSE_SESSION_PARMS_RX";
        case OBSE_PROVIDE_SESSION:  return "OBSE_PROVIDE_SESSION";
        case OBSE_SUSPENDED:        return "OBSE_SUSPENDED";
        case OBSE_RESUME_OPER:      return "OBSE_RESUME_OPER";
#endif
    }
    return "UNKNOWN";
}

char *pClientEvent(ObClientEvent event)
{
    switch (event) {
    case OBCE_NO_EVENT:         return "OBCE_NO_EVENT";
    case OBCE_CONNECTED:        return "OBCE_CONNECTED";
    case OBCE_DISCONNECT:       return "OBCE_DISCONNECT";
    case OBCE_DISCOVERY_FAILED: return "OBCE_DISCOVERY_FAILED";
    case OBCE_COMPLETE:         return "OBCE_COMPLETE";
    case OBCE_ABORTED:          return "OBCE_ABORTED";
    case OBCE_HEADER_RX:        return "OBCE_HEADER_RX";
    case OBCE_NO_SERVICE_FOUND: return "OBCE_NO_SERVICE_FOUND";
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
    case OBCE_SEND_COMMAND:     return "OBCE_SEND_COMMAND";
#endif
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    case OBCE_SESSION_SUSPENDED:return "OBCE_SESSION_SUSPENDED";
    case OBCE_SESSION_PARMS_RX: return "OBCE_SESSION_PARMS_RX";
    case OBCE_SUSPENDED:        return "OBCE_SUSPENDED";
    case OBCE_RESUME_OPER:      return "OBCE_RESUME_OPER";
#endif
    }
    return "UNKNOWN";
}

const char *pObexOpcode(U8 Opcode)
{
    switch (Opcode & 0x7F) {
    case 0: return "Connect";
    case 1: return "Disconnect";
    case 2: return "Put";
    case 3: return "Get";
    case 5: return "Set Path";
    case 7: return "Session";
    case 10: return "Create Session";     /* Obex-Opcode 7: Session-Opcode 0 */
    case 11: return "Close Session";      /* Obex-Opcode 7: Session-Opcode 1 */
    case 12: return "Suspend Session";    /* Obex-Opcode 7: Session-Opcode 2 */
    case 13: return "Resume Session";     /* Obex-Opcode 7: Session-Opcode 3 */
    case 14: return "Set Session Timeout";/* Obex-Opcode 7: Session-Opcode 4 */
    case 0x7F: return "Abort";
    }
    
    return "Unknown";
}

#if OBEX_SESSION_SUPPORT == XA_ENABLED
extern int sprintf(char *buffer, const char *format, ... );
/*---------------------------------------------------------------------------
 *            ReportSessionParms()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Dumps the session parameters header to the report console.
 *
 * Return:    void
 *
 */
void ReportSessionParms(ObexSessionParms *SessionParms)
{
    I8      i, x;
    char    buff[100];

    for (i = 0; i < 8; i++) {
        if ((SessionParms->fields & (1 << i)) == 0) 
            continue;

        switch (1 << i) {
        case OBSP_FIELD_DEVADDR:
            /* 4 byte (TCP and IRDA) and 6 byte (Bluetooth) device addresses are valid */
#if (BT_STACK == XA_ENABLED)
            if (SessionParms->deviceLen == 6)
                bdaddr_ntoa((BD_ADDR *)SessionParms->device, buff);
            else 
#endif /* (BT_STACK == XA_ENABLED) */
                sprintf(buff, "%d.%d.%d.%d", SessionParms->device[0],
                         SessionParms->device[1], SessionParms->device[2],
                         SessionParms->device[3]);
            //Report(("*   Dev Addr: %s\n", buff));
            break;

        case OBSP_FIELD_NONCE:
            /* 4-16 byte nonces are valid */
            kal_trace(BT_TRACE_G6_OBEX, OBEX_NONCE_TRC);
            for (x = 0; x < SessionParms->nonceLen; x++)
                kal_trace(BT_TRACE_G6_OBEX, OBEX_x02X , SessionParms->nonce[x]);
            break;
            
        case OBSP_FIELD_SESSIONID:
            /* 16 byte session ids are valid */
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SESSION_ID_TRC);
            for (x = 0; x < 16; x++)
                kal_trace(BT_TRACE_G6_OBEX, OBEX_x02X , SessionParms->sessionId[x]);
            break;
            
        case OBSP_FIELD_SEQNUM:
            /* 1 byte sequence numbers are valid */
            kal_trace(BT_TRACE_G6_OBEX,SEQNUM_xD , SessionParms->sequenceNum);
            break;
            
        case OBSP_FIELD_TIMEOUT:
            /* 4 byte timeouts are valid, convert from BE to Host on the fly. */
            kal_trace(BT_TRACE_G6_OBEX, OBEX_TIMEOUT_x0X , SessionParms->timeout);
            break;
        }
    }
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */


#if IRDA_STACK == XA_ENABLED
#include <irias.h>
#endif /* IRDA_STACK == XA_ENABLED */
#include <sys/obexcmd.h>

void ObProtRxChopper(ObexAppHandle *App, U8 *buff, U16 len)
{
    U16 len2;
    
    while (len) {
        len2 = OS_Rand() & 0xF;
        len2 = min(len2, len);
        OBPROT_ParseRx(App, buff, len2);
        len -= len2;
        buff += len2;
    }
}

//#endif /* XA_DEBUG == XA_ENABLED */
