/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * sm.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy Security Manager
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 11 24 2011 dlight.ting
 * NULL
 * .
 *
 * 11 24 2011 dlight.ting
 * NULL
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_4_0_BLE__

#include "bt_common.h"
#include "btconfig_le.h"
#include "btalloc.h"
#include "metypes.h"
#include "me_adp.h"
#include "l2cap_i.h"
#include "ddb.h"
#include "smi.h"
#include "bt_adp_bm.h"
#include "btbm.h"
#include "btbm_export.h"
#include "btbm_int.h"
#include "btutils.h"
extern BtStatus btbmBleNameReq(BtDeviceContext *bdc, BtCallBack callback);
extern BtStatus BTBMGATT_ServiceSearchAll(BtRemoteDevice *link, U8 *uuid, U8 size);

/*---------------------------------------------------------------------------
 * SM Context Structure
 *
 *   This structure provides the context and state information for the entire SM module
 */
typedef struct _SmContext
{
    ListEntry cmdPackets;                 /* Queue of packets used for signalling */
    SmCmdPacket _cmdPacket[SM_MAX_PKT_NUM];  /* Packet pool RAM */
    SmSession session[NUM_BT_DEVICES];
    
    cmacContext cmacCntx;
} SmContext;


static SmContext smCtx;
extern BMCONTENT_T BMController;
#ifdef __UPF__
extern upfBondParm upfParm;
#endif

#define __BT_BLE_PAIR_OOB_ABSENT_FALLBACK__

#ifdef LE_DEBUG
U8 sampleLTK[16] = {0xBF, 0x01, 0xFB, 0x9D, 0x4E, 0xF3, 0xBC, 0x36, 0xD8, 0x74, 0xF5, 0x39, 0x41, 0x38, 0x68, 0x4C};
#endif

static void SM_Phase1FSM(const BtEvent *event);
static void SM_Phase2FSM(const BtEvent *event);
static void SM_Phase3FSM(const BtEvent *event);

void SM_L2capCallback(L2capChannelId cid, L2capCallbackParms *Parms);

extern void btbmSendBondingDeviceCfm(U8 result, btbm_bd_addr_t bd_addr);
extern void btbmSendBondingResultInd(
                U8 result, 
                U8 type, 
                btbm_bd_addr_t bd_addr, 
                U32 cod, 
                U8 key_type, 
                U8 *key);
extern void BT_ATT_HandleSMCompleted(BtRemoteDevice *link);
extern BtStatus L2Cap_FixCidSend(U8 cid, BtRemoteDevice *link, BtPacket *Packet);

void SM_Init(void)
{
    int i;
    
    bt_trace(TRACE_FUNC, BT_SM_SM_INIT);
    OS_MemSet((U8 *)&smCtx, 0x00, sizeof(SmContext));
    
    btCtxLe.smCtx = &smCtx;
    /* Init packet pool */
    InitializeListHead(&smCtx.cmdPackets);
    InitializeListHead(&smCtx.cmacCntx.signQueue);
    InitializeListHead(&smCtx.cmacCntx.verifyQueue);
    
    for (i = 0; i < SM_MAX_PKT_NUM; i++)
    {
        InsertTailList(&smCtx.cmdPackets, &smCtx._cmdPacket[i].packet.node);
    }

    for (i = 0; i < SM_MAX_SIGN_QUEUE_SIZE; i++)
    {
        InitializeListEntry(&(smCtx.cmacCntx.signData[i].node));
    }
    for (i = 0; i < SM_MAX_SIGN_QUEUE_SIZE; i++)
    {
        InitializeListEntry(&(smCtx.cmacCntx.verifyData[i].node));
    }
    L2CAP_RegisterFixCid(SM_L2capCallback, SM_L2CAP_CID);
    SMP_Init();
}

/*****************************************************************************
 * FUNCTION
 *  SM_AllocCmdPkt
 * DESCRIPTION
 *  Get available packet from pool
 * PARAMETERS
 *  void
 * RETURNS
 *  packet
 *****************************************************************************/
BtPacket *SM_AllocCmdPkt(void)
{
    SmCmdPacket *packet;

    if (IsListEmpty(&smCtx.cmdPackets))
    {
        Assert(0);
    }

    packet = (SmCmdPacket*) RemoveHeadList(&smCtx.cmdPackets);
    packet->packet.data = packet->pdu;
    packet->packet.dataLen = packet->packet.tailLen = packet->packet.headerLen = 0;
    packet->packet.flags = BTP_FLAG_TAIL;
    packet->packet.priority_type = BTP_FLAG_HIGH_PRIPORTY;

    OS_Report("SM_AllocCmdPkt 0x%x", &packet->packet);
    return &packet->packet;
}

/*****************************************************************************
 * FUNCTION
 *  SM_FreeCmdPkt
 * DESCRIPTION
 *  Get available packet from pool
 * PARAMETERS
 *  void
 * RETURNS
 *  packet
 *****************************************************************************/
void SM_FreeCmdPkt(BtPacket *packet)
{
    OS_Report("SM_FreeCmdPkt 0x%x", packet);
    InsertTailList(&smCtx.cmdPackets, &packet->node);
}

void SM_ResetTimer(BtRemoteDevice *remDev)
{
    U8 i;
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (smCtx.session[i].bdc && remDev == smCtx.session[i].bdc->link)
        {
            EVM_CancelTimer(&smCtx.session[i].smTimer);
            EVM_StartTimer(&smCtx.session[i].smTimer, SM_REQUEST_TIMEOUT);
            return;
        }
    }
}

void SM_CancelTimer(BtRemoteDevice *remDev)
{
    U8 i;
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (smCtx.session[i].bdc && remDev == smCtx.session[i].bdc->link)
        {
            EVM_CancelTimer(&smCtx.session[i].smTimer);
            return;
        }
    }
}

void SM_HandleReqTimeout(EvmTimer *Timer)
{
    SmSession *session = (SmSession *)Timer->context;
    BtEvent event;

    bt_trace(TRACE_FUNC, BT_SM_SM_HANDLEREQTIMEOUT);
    event.eType = BTEVENT_LE_SM_RESULT;
    event.errCode = BEC_HOST_TIMEOUT;
    event.p.remDev = session->bdc->link;
    BTBMDumpBdAddr(event.p.remDev->bdAddr.addr);
    SM_LinkCallback(session->bdc->link, &event);
}

void SM_HandleDiscDelayTimeout(EvmTimer *Timer)
{
    SmSession *session = (SmSession *)Timer->context;
    BtEvent event;
    U16 idx = HciGetTableEntry(session->bdc->link->hciHandle);
	
    bt_trace(TRACE_FUNC, BT_SM_SM_HANDLEDISCDELAYTIMEOUT, HCC(con)[idx].packetsSent, session->smDiscCount);
    EVM_ResetTimer(Timer);
	
    /* 1. Always wait 2s to see if any further data incoming 
       2. Not yet send to controller: HCC(con)[idx].conFlags & HCI_DATA_REQ 
       3. Controller not yet finish sending: HCC(con)[idx].packetsSent > 0 */
    if ((session->smDiscCount == 0) || 
        ((HCC(con)[idx].conFlags & HCI_DATA_REQ || HCC(con)[idx].packetsSent > 0) && 
        (session->smDiscCount < 5)))
    {
        Timer->func = SM_HandleDiscDelayTimeout;
        Timer->context = (void *)session;
        EVM_StartTimer(Timer, (5 - session->smDiscCount) * 400/*session->bdc->link->leConnInterval*/);
        session->smDiscCount++;
    }
    else
    {
#if 0 
        /* Not remove LE link */
        if (IsNodeOnList(&session->bdc->link->handlers, &session->smHandler.node))
        {
            ME_LE_DiscLink(&session->smHandler, &session->bdc->addr, BEC_USER_TERMINATED);
        }
#endif
        event.eType = BTEVENT_LE_SM_RESULT;
        event.errCode = HC_STATUS_SUCCESS;
        event.p.remDev = session->bdc->link;
        session->smHandler.callback(&event);
    }
}

static void SM_GenLTK(U8 *ltk, BD_ADDR bdAddr, U8 keySize)
{
    S32 r32;
    S32 m = (S32)&r32;
    TimeT t;
    
    bt_trace(TRACE_FUNC, BT_SM_SM_GENLTK, keySize);
    /* 128bit LTK: 48bit raddr || 16bit mem address LSBs || 32bit rand|| 32bit time */
    OS_MemCopy(ltk, bdAddr.addr, 6);
    StoreLE16(ltk + 6, (U16)m);
    r32 = OS_Rand32();
    OS_MemCopy(ltk + 8, (U8 *)&r32, 4);
    t = OS_GetSystemTime();
    OS_MemCopy(ltk + 12, (U8 *)&t, 4);
    if (keySize < SM_LTK_VALUE_LENGTH)
    {
        SM_util_mask_MSBs(ltk, SM_LTK_VALUE_LENGTH, SM_LTK_VALUE_LENGTH - keySize, 0x0);
    }
    bt_parm_trace(keySize, ltk);
}

static U16 SM_GenEDIV(void)
{
    BtDeviceRecord rec;
    int i = 0;
    U16 max_ediv = 0;

    while (DDB_EnumDeviceRecords(i, &rec) == BT_STATUS_SUCCESS)
    {
        max_ediv = ((max_ediv > rec.distEDIV) ?  max_ediv : (rec.distEDIV + 1));
        i++;
    }
    bt_trace(TRACE_FUNC, BT_SM_SM_GENEDIV, max_ediv);
    return max_ediv;
}

void SM_GenCSRK(U8 *csrk)
{
    S32 r32;
    S32 m = (S32)&r32;
    TimeT t;

    bt_trace(TRACE_FUNC, BT_SM_SM_GENCSRK);
    Assert(csrk);
    /* 128bit LTK: 48bit raddr || 16bit mem address LSBs || 32bit rand|| 32bit time */
    OS_MemCopy(csrk, MEC(bdaddr).addr, 6);
    StoreLE16(csrk + 6, (U16)m);
    r32 = OS_Rand32();
    OS_MemCopy(csrk + 8, (U8 *)&r32, 4);
    t = OS_GetSystemTime();
    OS_MemCopy(csrk + 12, (U8 *)&t, 4);
    bt_parm_trace(16, csrk);
}

void SM_GenIRK(U8 *irk)
{
    bt_trace(TRACE_FUNC, BT_SM_SM_GENIRK);
    SM_GenCSRK(irk);
}

static BtRemoteDevice *SM_GetRemDev(BtEvent *event)
{
    switch (event->eType)
    {
    case BTEVENT_NAME_RESULT:
        return event->p.meToken->remDev;
    default:
        return event->p.remDev;
    }
    return NULL;
}

static SmSession *SM_GetSession(BtRemoteDevice *remDev)
{
    U8 i;
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (smCtx.session[i].bdc && smCtx.session[i].bdc->link == remDev)
        {
            return &smCtx.session[i];
        }
    }

    bt_trace(TRACE_WARNING, BT_SM_SM_GETSESSION_NOT_FOUND);
    BTBMDumpBdAddr(remDev->bdAddr.addr);
    return NULL;
}

static SmSession *SM_StartSession(BtDeviceContext *bdc, BtCallBack handlerCb, BtSMPairType type, SmPairAttribute *attr)
{
    int i;

    bt_trace(TRACE_FUNC, BT_SM_SM_STARTSESSION, 
             type, bdc->addr.addr[0], bdc->addr.addr[1], bdc->addr.addr[2], bdc->addr.addr[3], bdc->addr.addr[4], bdc->addr.addr[5]);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (smCtx.session[i].bdc == NULL)
        {
            OS_MemSet((U8 *)&smCtx.session[i], 0x0, sizeof(SmSession));
            smCtx.session[i].type = type;
            smCtx.session[i].pairAttr.mitm = attr->mitm;
            smCtx.session[i].pairAttr.oob = attr->oob;
            smCtx.session[i].pairAttr.bond = attr->bond;
            smCtx.session[i].bdc = bdc;
            smCtx.session[i].smState = SM_SESSION_NULL;
            smCtx.session[i].smGCState = SM_GC_STATE_NULL;
            smCtx.session[i].smHandler.callback = handlerCb;
            smCtx.session[i].smTimer.func = SM_HandleReqTimeout;
            smCtx.session[i].smTimer.context = (void *)&smCtx.session[i];
            smCtx.session[i].service_search = attr->service_list;
            smCtx.session[i].service_no = attr->service_no;
            return &smCtx.session[i];
        }
    }
    Assert(0);
    return NULL;
}

static void SM_UpdateSession(SmSession *session, BtCallBack handlerCb)
{
    bt_trace(TRACE_FUNC, BT_SM_SM_UPDATESESSION);
    Assert(session);
    session->smHandler.callback = handlerCb;
}

void gatt_test_callback(const BtEvent *event)
{
    if (event->errCode == BEC_NO_ERROR)
    {
        OS_Report("GATT device name %s", event->p.meToken->p.name.io.out.name);
    }
    else
    {
        OS_Report("GATT device name failed");
    }
}

static void SM_EndSessionFSM(const BtEvent *event)
{
    SmSession *session = SM_GetSession(event->p.remDev);  /* Trick: put remDev as 1st field in all LE event structures */
    if (session == NULL)
    {
        bt_trace(TRACE_ERROR, BT_SM_SM_PHASEFSM_SESSION_NOT_FOUND, 101);
        return;
    }

    bt_trace(TRACE_FUNC, BT_SM_SM_UPDATESESSIONFSM, session->smGCState);

    switch (session->smGCState)
    {
    case SM_GC_STATE_NULL:
        session->smGCState = SM_GC_STATE_SEND_PAIR_FAIL;
    case SM_GC_STATE_SEND_PAIR_FAIL:
        session->smGCState = SM_GC_STATE_DISC;
        if (session->bdc->link->state == BDS_CONNECTED &&
            session->smState > SM_PHASE1_PAIR_REQ_RECEIVE && session->smState < SM_PHASE2_PAIR_DONE &&
            session->smpFailSent == FALSE)
        {
            SM_SendPairFailed(session->bdc->link, SM_REASON_UNSPECIFIED);
            return;
        }
        /* fall-through */
    case SM_GC_STATE_DISC:
        session->smGCState = SM_GC_STATE_RESET_CNTX;
        EVM_CancelTimer(&session->smTimer);
        if ((session->bdc->link->state == BDS_CONNECTED) && 
			(session->type == SM_PAIR_TYPE_BOND))
        {
            session->smDiscTimer.context = (void *)session;
            SM_HandleDiscDelayTimeout(&session->smDiscTimer);
			return;
        }
        /* fall - through */
    case SM_GC_STATE_RESET_CNTX:
    {
        btbm_bd_addr_t bd_addr;

        CONVERT_ARRAY2BDADDR(&bd_addr, session->bdc->addr.addr);

        /* pair or not connected */
        EVM_ResetTimer(&session->smTimer);        
        EVM_ResetTimer(&session->smDiscTimer);

        if (IsNodeOnList(&session->bdc->link->handlers, &session->smHandler.node))
        {
            RemoveEntryList(&session->smHandler.node);
        }

        if (session->bdc == BMController.pin_bdc)
        {
            BMController.pin_bdc = 0;
            BMController.bm_opactivity = BM_OP_NONE;
        }

        switch (session->smState)
        {
        case SM_PHASE3_DONE:
        case SM_PHASE3_KEY_DIST:
        case SM_PHASE3_KEY_RECEIVE:
        case SM_PHASE3_ENCRYPTED:
        case SM_PHASE2_ENCRYPT:
        case SM_PHASE2_PAIR_DONE:
        case SM_PHASE2_CALC_STK:
        case SM_PHASE2_CONFIRM_VERIFY:
        case SM_PHASE2_RAND_RECEIVE:
        case SM_PHASE2_RAND_SEND:
        case SM_PHASE2_CONFIRM_RECEIVE:
        case SM_PHASE2_CONFIRM_SEND:
        case SM_PHASE2_CONFIRM_WAIT:
        case SM_PHASE2_CONFIRM_CAL:
        case SM_PHASE2_RAND_GEN:
        case SM_PHASE1_USER_PAIR:
        case SM_PHASE1_PAIR_RSP_SEND:
        case SM_PHASE1_PAIR_RSP_RECEIVE:
        case SM_PHASE1_PAIR_REQ_RECEIVE:
        case SM_PHASE1_PAIR_REQ_SEND:
        case SM_SESSION_INIT:
            if (session->bdc->under_used & BDS_BONDING)
            {
                btbmSendBondingResultInd((U8)bt_util_convert_err2(session->pairStatus), TRUE, bd_addr, 0, 0, NULL);
            }
            else
            {
                /* pairing /auth */
                btbmSendBondingResultInd((U8)bt_util_convert_err2(session->pairStatus), FALSE, bd_addr, 0, 0, NULL);
            }
            Report(("session->pairStatus:%d",session->pairStatus));
            if (session->service_no !=0)
            {
                Report(("Try to serach service"));
                BTBMGATT_ServiceSearchAll(session->bdc->link, session->service_search, session->service_no);
            }	
		
            /* fall - through */
        case SM_SESSION_NULL:
            if (session->bdc->under_used & BDS_BONDING)
            {
                btbmSendBondingDeviceCfm((U8)bt_util_convert_err2(session->pairStatus), bd_addr);
                BT_ATT_HandleSMCompleted(session->bdc->link);
            }

            /* WCNCR00005488  : call SecProcessAllSecurityTokens to remove tokens */
            if(session->pairStatus != BT_STATUS_SUCCESS && session->bdc->link){
                SecProcessAllSecurityTokens(session->bdc->link);
            }
            
            Report(("bdc->under_used=0x%x", session->bdc->under_used));
            session->bdc->under_used &= ~BDS_BONDING;
            session->bdc->under_used &= ~BDS_CONNECTING;

            OS_MemSet((U8 *)session, 0x0, sizeof(SmSession));
#ifdef __BT_LE_STANDALONE__
            ME_LE_SetAccessible(btCtxMe.accModeCur, NULL);
#endif
            break;
        }
    }
    }
}

static void SM_EndSession(SmSession *session, BtStatus status)
{
    BtEvent event;
    if (session == NULL)
    {
        bt_trace(TRACE_ERROR, BT_SM_SM_PHASEFSM_SESSION_NOT_FOUND, 100);
        return;
    }
//    BT_ATT_HandleSMCompleted(session->bdc->link);
    bt_trace(TRACE_FUNC, BT_SM_SM_ENDSESSION, status, session->smState);
    BTBMDumpBdAddr(session->bdc->addr.addr);
    SM_UpdateSession(session, SM_EndSessionFSM);
    event.eType = BTEVENT_LE_SM_RESULT;
    event.p.remDev = session->bdc->link;
    //session->smGCState = ((status == BT_STATUS_SUCCESS) ? SM_GC_STATE_DISC : SM_GC_STATE_NULL);
    if (status == BT_STATUS_SUCCESS)
    {
        session->smGCState = SM_GC_STATE_DISC;
    }
    else if (status == BT_STATUS_CANCELLED)
    {
        session->smGCState = SM_GC_STATE_RESET_CNTX;
    }
    else
    {
        session->smGCState = SM_GC_STATE_NULL;
    }
    session->pairStatus = ((session->smState >= SM_PHASE2_PAIR_DONE) ? BT_STATUS_SUCCESS : status);
    SM_EndSessionFSM(&event);
}

static SmPairFailReason SM_SetPairInfo(SmSession *session, const BtEvent *event)
{
    SmPairAttribute *attr = &session->pairAttr;
    SmPairInfo *info = &session->pairInfo;

    bt_trace(TRACE_FUNC, BT_SM_SM_SETPAIRINFO, 
             session->type, 
             event->p.smPairResponse.ioCapability, 
             event->p.smPairResponse.OOBDataPresent,
             event->p.smPairResponse.authRequirements,
             event->p.smPairResponse.maxEncKeySize, 
             event->p.smPairResponse.initKeyDist,
             event->p.smPairResponse.respKeyDist);
    info->ioCapability = event->p.smPairResponse.ioCapability;

#ifdef __UPF__
    info->encKeySize = min(upfParm.maxKeySize, event->p.smPairResponse.maxEncKeySize);
    info->initKeyDist = (upfParm.iniKey & event->p.smPairResponse.initKeyDist);
    info->respKeyDist = (upfParm.resKey & event->p.smPairResponse.respKeyDist);
#else
    info->encKeySize = min(SM_DEFAULT_MAX_ENC_KEY_SIZE, event->p.smPairResponse.maxEncKeySize);
    info->initKeyDist = (SM_DEFAULT_INITIATOR_KEY_DIST & event->p.smPairResponse.initKeyDist);
    info->respKeyDist = (SM_DEFAULT_RESPONDER_KEY_DIST & event->p.smPairResponse.respKeyDist);
#endif

    if (info->encKeySize < SM_DEFAULT_MIN_ENC_KEY_SIZE || info->encKeySize > 16)
    {
        return SM_REASON_ENCRYPT_KEY_SIZE;
    }

    if (event->p.smPairResponse.OOBDataPresent == SM_OOB_DATA_PRESENT_YES)
    {
        if (attr->oob == SM_OOB_DATA_PRESENT_YES)
        {
            /* Use OOB pairing */
            info->model = SM_MODEL_OUT_OF_BAND;
            return SM_REASON_RESERVED;
        }
#ifndef __BT_BLE_PAIR_OOB_ABSENT_FALLBACK__
        else
        {
            return SM_REASON_OOB_NOT_AVAILABLE;
        }
#endif
    }

    if ((event->p.smPairResponse.authRequirements & 0x1) == 0)
    {
        OS_Report("Remote do not want to bond");
        session->pairAttr.bond = SM_BONDING_NO;
    }

    if (attr->mitm == FALSE && (event->p.smPairResponse.authRequirements & 0x4) == 0)
    {
        /* Use Just Works */
        info->model = SM_MODEL_JUST_WORK;
    }
    else
    {
        /* Decide by IO capability */
        if (session->type == SM_PAIR_TYPE_BOND)
        {
#ifdef __UPF__
            info->model = SM_GetPairModelByIo(upfParm.io, event->p.smPairResponse.ioCapability);
#else
            info->model = SM_GetPairModelByIo(SM_DEFAULT_IO_CAPABILITY, event->p.smPairResponse.ioCapability);
#endif
            if (info->model == SM_MODEL_PASSKEY_ENTRY)
            {
#ifdef __UPF__
                info->role = SM_GetPasskeyRole(session->type, upfParm.io, event->p.smPairResponse.ioCapability);
#else
                info->role = SM_GetPasskeyRole(session->type, SM_DEFAULT_IO_CAPABILITY, event->p.smPairResponse.ioCapability);
#endif
            }
        }
        else
        {
#ifdef __UPF__
            info->model = SM_GetPairModelByIo(event->p.smPairResponse.ioCapability, upfParm.io);
#else
            info->model = SM_GetPairModelByIo(event->p.smPairResponse.ioCapability, SM_DEFAULT_IO_CAPABILITY);
#endif
            if (info->model == SM_MODEL_PASSKEY_ENTRY)
            {
#ifdef __UPF__
                info->role = SM_GetPasskeyRole(session->type, event->p.smPairResponse.ioCapability, upfParm.io);
#else
                info->role = SM_GetPasskeyRole(session->type, event->p.smPairResponse.ioCapability, SM_DEFAULT_IO_CAPABILITY);
#endif
            }
        }
    }

    if (info->model == SM_MODEL_JUST_WORK)
    {
//        if (attr->mitm == TRUE || (event->p.smPairResponse.authRequirements & 0x4))
//        {
//            return SM_REASON_AUTH_REQUIREMENT;
//        }
//        else
//        {
            OS_MemSet(info->TK, 0x0, 16);
//        }
    }

    return SM_REASON_RESERVED;
}

BtStatus SM_Bond(BtDeviceContext *bdc, SmPairAttribute *attr)
{
    BtStatus status;
    SmSession *session;
    BtRemoteDevice *remDev;
    BtLELinkParm parms;

    if(bdc == 0)
    {
        Report(("SM bond bdc 0"));
        return BT_STATUS_FAILED;
    }

    bt_trace(TRACE_FUNC, BT_SM_SM_BOND, 
             bdc->under_used, bdc->addr.addr[0], bdc->addr.addr[1], bdc->addr.addr[2], bdc->addr.addr[3], bdc->addr.addr[4], bdc->addr.addr[5]);

    Report(("bdc->under_used:%d",bdc->under_used));
    Report(("BMController.bm_opactivity:%d",BMController.bm_opactivity));    
    if((bdc->under_used & BDS_BONDING) != 0){
        /* WCNCR00005488  : Check if the SM procedure is failed */
        if(bdc->link){
            session = SM_GetSession(bdc->link);
            if(session && session->pairStatus != BT_STATUS_SUCCESS) return BT_STATUS_FAILED;
        }
        return BT_STATUS_PENDING;
    }else if(BMController.bm_opactivity != BM_OP_NONE){
        return BT_STATUS_FAILED;
    }

    BMController.pin_bdc = bdc;
    BMController.bm_opactivity = BM_OP_BONDING;
    bdc->under_used |= BDS_BONDING;
    parms.connIntervalMax = 15;
    parms.connIntervalMin = 10;
    parms.privacy = FALSE;
    parms.scanInterval = 0x12;
    parms.scanWindow = 0x12;

    session = SM_StartSession(bdc, SM_Phase1FSM, SM_PAIR_TYPE_BOND, attr);

    status = ME_LE_CreateLink(&session->smHandler, &bdc->addr, &parms, &remDev);
    if (status == BT_STATUS_SUCCESS)
    {
        BtEvent event;

        session->smState = SM_SESSION_INIT;
        event.eType = BTEVENT_LE_SM_RESULT;
        event.errCode = BEC_NO_ERROR;
        event.p.remDev = bdc->link;
        SM_LinkCallback(bdc->link, &event);
        status = BT_STATUS_PENDING;
    }
    else if (status == BT_STATUS_PENDING)
    {
        session->smState = SM_SESSION_INIT;
    }
    else
    {
        //WCNCR00006302 : SM failed due to no resource to allocate BtRemoteDevice.
        // Because SM FSM will use pointer to BtRemoteDevice (It will be NULL) and cause
        // exception. So just clear the session and return fail
        //SM_EndSession(session, status);
        OS_MemSet((U8 *)session, 0x0, sizeof(SmSession));
    }

    return status;
}

BtStatus SM_CancelBonding(BtDeviceContext *bdc)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    SmSession *session = NULL;

    if (bdc && bdc->link)
    {   
        session = SM_GetSession(bdc->link);
        if (session)
        {
            SM_EndSession(session, BT_STATUS_CANCELLED);
            status = BT_STATUS_SUCCESS; 
        }
    }
    OS_Report("SM_CancelBonding status %d", status);
    return status;
}

BtStatus SM_JustWorkRsp(BtDeviceContext *bdc, BOOL accept)
{
    SmSession *session = SM_GetSession(bdc->link);
    BtStatus status = BT_STATUS_FAILED;

    bt_trace(TRACE_FUNC, BT_SM_SM_SETOOBDATA, 
             bdc->addr.addr[0], bdc->addr.addr[1], bdc->addr.addr[2], bdc->addr.addr[3], bdc->addr.addr[4], bdc->addr.addr[5], 
             accept);
    if (session)
    {
        bt_trace(TRACE_GROUP_1, BT_SM_SM_PASSKEYENTRY1, session->type, session->smState);

        if ((session->type == SM_PAIR_TYPE_BOND || session->type == SM_PAIR_TYPE_PAIR) && 
            session->smState == SM_PHASE2_RAND_GEN)
        {
            BtEvent event;
            event.eType = BTEVENT_LE_SM_RESULT;
            event.p.remDev = bdc->link;
            if (accept)
            {
                event.errCode = BEC_NO_ERROR;
                SM_LinkCallback(bdc->link, &event);
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                event.errCode = BEC_AUTHENTICATE_FAILURE;
                SM_SendPairFailed(bdc->link, SM_REASON_UNSPECIFIED);
                session->smpFailSent = TRUE;
                SM_LinkCallback(bdc->link, &event);
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    return status;
}

BtStatus SM_PasskeyEntry(BtDeviceContext *bdc, BOOL accept, U32 passkey)
{
    SmSession *session = SM_GetSession(bdc->link);
    BtStatus status = BT_STATUS_FAILED;

    bt_trace(TRACE_FUNC, BT_SM_SM_PASSKEYENTRY, 
             bdc->addr.addr[0], bdc->addr.addr[1], bdc->addr.addr[2], bdc->addr.addr[3], bdc->addr.addr[4], bdc->addr.addr[5], 
             accept, passkey);
    if (session)
    {
        bt_trace(TRACE_GROUP_1, BT_SM_SM_PASSKEYENTRY1, session->type, session->smState);

        if ((session->type == SM_PAIR_TYPE_BOND || session->type == SM_PAIR_TYPE_PAIR) && 
            session->smState == SM_PHASE2_RAND_GEN)
        {
            BtEvent event;
            event.eType = BTEVENT_LE_SM_RESULT;
            event.p.remDev = bdc->link;
            if (accept  && passkey <= 999999)
            {
                event.errCode = BEC_NO_ERROR;
                SM_util_32bit_2_128bit(session->pairInfo.TK, passkey);

                if (session->smState == SM_PHASE2_RAND_GEN)
                {
                    /* master continue FSM; salve already received Mconfirm, continue FSM */
                    SM_LinkCallback(bdc->link, &event);
                }
                /* Else Mconfirm not received, wait */
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                event.errCode = BEC_AUTHENTICATE_FAILURE;
                SM_SendPairFailed(bdc->link, SM_REASON_PASSKEY_ENTRY_FAILED);
                session->smpFailSent = TRUE;
                SM_LinkCallback(bdc->link, &event);
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    return status;
}

BtStatus SM_SetOobData(BtDeviceContext *bdc, BOOL accept, U8 *tk)
{
    SmSession *session = SM_GetSession(bdc->link);
    BtStatus status = BT_STATUS_FAILED;

    bt_trace(TRACE_FUNC, BT_SM_SM_SETOOBDATA, 
             bdc->addr.addr[0], bdc->addr.addr[1], bdc->addr.addr[2], bdc->addr.addr[3], bdc->addr.addr[4], bdc->addr.addr[5], 
             accept);
    if (session)
    {
        bt_trace(TRACE_GROUP_1, BT_SM_SM_PASSKEYENTRY1, session->type, session->smState);

        if ((session->type == SM_PAIR_TYPE_BOND || session->type == SM_PAIR_TYPE_PAIR) && 
            session->smState == SM_PHASE2_RAND_GEN)
        {
            BtEvent event;
            event.eType = BTEVENT_LE_SM_RESULT;
            event.p.remDev = bdc->link;
            if (accept)
            {
                event.errCode = BEC_NO_ERROR;
#ifdef __UPF__
                OS_MemCopy(session->pairInfo.TK, upfParm.oobData, 16);
#else
                OS_MemCopy(session->pairInfo.TK, tk, 16);
#endif
                SM_LinkCallback(bdc->link, &event);
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                event.errCode = BEC_AUTHENTICATE_FAILURE;
                SM_SendPairFailed(bdc->link, SM_REASON_OOB_NOT_AVAILABLE);
                session->smpFailSent = TRUE;
                SM_LinkCallback(bdc->link, &event);
            }
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    return status;
}

static BtStatus SM_HandleEvent(const BtEvent *event)
{
    bt_trace(TRACE_FUNC, BT_SM_SM_HANDLEEVENT, event->eType, event->errCode);
    switch (event->eType)
    {
    case BTEVENT_LE_SM_RESULT:
    case BTEVENT_LE_LTK_RECEIVED:
    case BTEVENT_LE_EDIV_RAND_RECEIVED:
    case BTEVENT_LE_IRK_RECEIVED:
    case BTEVENT_LE_ADDR_RECEIVED:
    case BTEVENT_LE_CSRK_RECEIVED:
    case BTEVENT_ENCRYPTION_CHANGE:
    case BTEVENT_LINK_CONNECT_CNF:
    case BTEVENT_ENCRYPT_COMPLETE:
        if (event->errCode != HC_STATUS_SUCCESS)
        {
            BtDeviceContext *bdc ;

            bdc = DS_FindDevice(&event->p.remDev->bdAddr);
            if((bdc !=0) && (bdc->devType == BT_DEV_TYPE_LE))
            {
                BT_ATT_HandleSMCompleted(event->p.remDev);
            }
            return bt_util_convert_err3(event->errCode);
        }
        break;
    case BTEVENT_LINK_DISCONNECT:
        return BT_STATUS_CONNECTION_FAILED;
    case BTEVENT_NAME_RESULT:
        /* No matter name req succeeded or not, continue */
        break;
    }
    return BT_STATUS_SUCCESS;
}

static void SM_Phase1FSM(const BtEvent *event)
{
    BtStatus status = BT_STATUS_SUCCESS;
    SmPairFailReason reason = SM_REASON_RESERVED;
    BtRemoteDevice *remDev = SM_GetRemDev((BtEvent *)event);
    SmSession *session = SM_GetSession(remDev);

    if (session == NULL)
    {
        bt_trace(TRACE_ERROR, BT_SM_SM_PHASEFSM_SESSION_NOT_FOUND, 1);
        return;
    }

    if ((status = SM_HandleEvent(event)) != BT_STATUS_SUCCESS)
    {
        goto failed;
    }

    while (status == BT_STATUS_SUCCESS)
    {
        bt_trace(TRACE_GROUP_1, BT_SM_SM_PHASEFSM, session->type, session->smState);
        if(event->eType == BTEVENT_LINK_CONNECT_CNF)
        {
            Report(("SM shall wait for BTEVENT_GATT_ATT_READY"));
            return;
        }else if(event->eType == BTEVENT_GATT_ATT_READY){
            if(session->smState != SM_SESSION_INIT){
                OS_Report("Recv BTEVENT_GATT_ATT_READY, but state is not init : %d", session->smState);
                return;
            }
        }
        switch (session->smState)
        {
        case SM_SESSION_INIT:
//            btbmUtilConnectedDeviceNameEntryAllocate(remDev);
            session->bdc->link->authState = BAS_START_AUTHENTICATE;
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                session->smState = SM_PHASE1_PAIR_REQ_SEND;
                /* As name before sending pair REQ */
                if (btbmBleNameReq(session->bdc, btbmHandlerPinCodeAskNameQuery) == BT_STATUS_PENDING)
                {
                    status = BT_STATUS_PENDING;
                    /* if not pending, continue */
                }
                break;
            }
            else
            {
                session->smState = SM_PHASE1_PAIR_REQ_RECEIVE;
                /* fall-through */
            }
        case SM_PHASE1_PAIR_REQ_RECEIVE:
            reason = SM_SetPairInfo(session, event);
            if (reason != SM_REASON_RESERVED)
            {
                SM_SendPairFailed(session->bdc->link, reason);
                session->smpFailSent = TRUE;
                status = BT_STATUS_KEY_ERR;
                goto failed;
            }
            session->smState = SM_PHASE1_PAIR_RSP_SEND;
            /* fall-through */
        case SM_PHASE1_PAIR_RSP_SEND:
            /* Ask name after processing pair REQ */
            if (btbmBleNameReq(session->bdc, btbmHandlerPinCodeAskNameQuery) == BT_STATUS_PENDING)
            {
                status = BT_STATUS_PENDING;
                /* if not pending, continue */
            }
            SM_SendPairRsp(session->bdc->link,
                            session->pairInfo.pres,
#ifdef __UPF__
                            upfParm.io,
#else
                            SM_DEFAULT_IO_CAPABILITY,
#endif
                            session->pairAttr.oob,
                            session->pairAttr.bond,
                            session->pairAttr.mitm,
                            session->pairInfo.encKeySize,
                            session->pairInfo.initKeyDist,
                            session->pairInfo.respKeyDist);
            session->smState = SM_PHASE1_USER_PAIR;
            break;
        case SM_PHASE1_PAIR_REQ_SEND:
            session->smState = SM_PHASE1_PAIR_RSP_RECEIVE;
            status = SM_SendPairReq(session->bdc->link,
                                    session->pairInfo.preq,
#ifdef __UPF__
                                    upfParm.io,
#else
                                    SM_DEFAULT_IO_CAPABILITY,
#endif
                                    session->pairAttr.oob,
                                    session->pairAttr.bond,
                                    session->pairAttr.mitm,
#ifdef __UPF__
                                    upfParm.maxKeySize,
                                    upfParm.iniKey,
                                    upfParm.resKey);
#else
                                    SM_DEFAULT_MAX_ENC_KEY_SIZE,
                                    SM_DEFAULT_INITIATOR_KEY_DIST,
                                    SM_DEFAULT_RESPONDER_KEY_DIST);
#endif
            break;
        case SM_PHASE1_PAIR_RSP_RECEIVE:
            reason = SM_SetPairInfo(session, event);
            if (reason != SM_REASON_RESERVED)
            {
                SM_SendPairFailed(session->bdc->link, reason);
                session->smpFailSent = TRUE;
                status = BT_STATUS_KEY_ERR;
                goto failed;
            }
            session->smState = SM_PHASE1_USER_PAIR;
            /* fall-through */
        case SM_PHASE1_USER_PAIR:
            SM_UpdateSession(session, SM_Phase2FSM);
            session->smState = SM_PHASE2_RAND_GEN;

            if (session->pairInfo.model == SM_MODEL_OUT_OF_BAND)
            {
                BMCallbackParms parms;
                parms.event = BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION;
                CONVERT_ARRAY2BDADDR(&parms.p.SSPSecurityPairingIndication.bd_addr, session->bdc->addr.addr);
                parms.p.SSPSecurityPairingIndication.ssp_type = BT_BM_SSP_PIN_OOB_DATA;
                parms.p.SSPSecurityPairingIndication.name = BTBMGetLocalStoredName(session->bdc->addr.addr);
                parms.p.SSPSecurityPairingIndication.name_len = ((parms.p.SSPSecurityPairingIndication.name) ? OS_StrLen((char *)parms.p.SSPSecurityPairingIndication.name) : 0);
                parms.p.SSPSecurityPairingIndication.cod = 0;                
                parms.p.SSPSecurityPairingIndication.displayNumeric = FALSE;
                BMController.bmCB(&parms);
            }
            else if (session->pairInfo.model == SM_MODEL_JUST_WORK)
            {
                /* Use Just Works */
                if (session->type == SM_PAIR_TYPE_BOND){
                    SM_JustWorkRsp(session->bdc, TRUE);
                    /* Return here to prevent state machine already end session in SM_JustWorkRsp */
                    return;
                }else{
                    BMCallbackParms parms;
                    parms.event = BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION;
                    CONVERT_ARRAY2BDADDR(&parms.p.SSPSecurityPairingIndication.bd_addr, session->bdc->addr.addr);
                    parms.p.SSPSecurityPairingIndication.ssp_type = BT_BM_SSP_PIN_USER_CONFIRM;
                    parms.p.SSPSecurityPairingIndication.name = BTBMGetLocalStoredName(session->bdc->addr.addr);
                    parms.p.SSPSecurityPairingIndication.name_len = ((parms.p.SSPSecurityPairingIndication.name) ? OS_StrLen((char *)parms.p.SSPSecurityPairingIndication.name) : 0);
                    parms.p.SSPSecurityPairingIndication.cod = 0;                
                    parms.p.SSPSecurityPairingIndication.displayNumeric = FALSE;
                    BMController.bmCB(&parms);
                }
            }
            else if (session->pairInfo.model == SM_MODEL_PASSKEY_ENTRY)
            {
                BMCallbackParms parms;

                parms.event = BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION;
                CONVERT_ARRAY2BDADDR(&parms.p.SSPSecurityPairingIndication.bd_addr, session->bdc->addr.addr);
                parms.p.SSPSecurityPairingIndication.ssp_type = BT_BM_SSP_PIN_PASSKEY_ENTRY;
                parms.p.SSPSecurityPairingIndication.name = BTBMGetLocalStoredName(session->bdc->addr.addr);
                parms.p.SSPSecurityPairingIndication.name_len = ((parms.p.SSPSecurityPairingIndication.name) ? OS_StrLen((char *)parms.p.SSPSecurityPairingIndication.name) : 0);
                parms.p.SSPSecurityPairingIndication.cod = 0;                
                if (session->pairInfo.role == SM_PASSKEY_ROLE_INPUT)
                {
                    parms.p.SSPSecurityPairingIndication.displayNumeric = FALSE;
                    BMController.bmCB(&parms);
                }
                else
                {
                    BtEvent event;

                    /* Generate random passkey */
                    parms.p.SSPSecurityPairingIndication.numericValue = (U32)OS_Rand32();
                    parms.p.SSPSecurityPairingIndication.numericValue %= 1000000;
                    parms.p.SSPSecurityPairingIndication.displayNumeric = TRUE;
                    SM_util_32bit_2_128bit(session->pairInfo.TK, parms.p.SSPSecurityPairingIndication.numericValue);
                    BMController.bmCB(&parms);

                    /* with TK, we can go to SM_PHASE2_RAND_GEN directly */
                    event.eType = BTEVENT_LE_SM_RESULT;
                    event.errCode = HC_STATUS_SUCCESS;
                    event.p.remDev = session->bdc->link;
                    SM_LinkCallback(session->bdc->link, &event);
                }
            }
            status = BT_STATUS_PENDING;
            break;
        default:
            Assert(0);
        }
    }

    if (status == BT_STATUS_PENDING)
    {
        return;
    }
failed:
    SM_EndSession(session, status);
}

static void SM_Phase2FSM(const BtEvent *event)
{
    SmSession *session = SM_GetSession(event->p.remDev);  /* Trick: put remDev as 1st field in all LE event structures */
    BtStatus status;

    if (session == NULL)
    {
        bt_trace(TRACE_ERROR, BT_SM_SM_PHASEFSM_SESSION_NOT_FOUND, 2);
        return;
    }
    
    if ((status = SM_HandleEvent(event)) != BT_STATUS_SUCCESS)
    {
        goto failed;
    }

    while (status == BT_STATUS_SUCCESS)
    {
        bt_trace(TRACE_GROUP_1, BT_SM_SM_PHASEFSM, session->type, session->smState);
        switch (session->smState)
        {
        case SM_PHASE2_RAND_GEN:
            status = SM_Rand(session->bdc->link, SM_RANDOM_VALUE_LENGTH);
            session->smState = SM_PHASE2_CONFIRM_CAL;
            break;
        case SM_PHASE2_CONFIRM_CAL:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                OS_MemCopy(session->pairInfo.mRand, event->p.smRand128Response.randValue128, SM_RANDOM_VALUE_LENGTH);
                status = SM_CalcPairConfirm(event->p.remDev,
                                            session->pairInfo.TK, 
                                            session->pairInfo.mRand, 
                                            session->pairInfo.preq,
                                            session->pairInfo.pres,
                                            LE_ADDR_PUBLIC,
                                            session->bdc->addrType,
                                            MEC(bdaddr),
                                            session->bdc->addr);
                session->smState = SM_PHASE2_CONFIRM_SEND;
            }
            else
            {
                OS_MemCopy(session->pairInfo.sRand, event->p.smRand128Response.randValue128, SM_RANDOM_VALUE_LENGTH);
                status = SM_CalcPairConfirm(event->p.remDev,
                                            session->pairInfo.TK, 
                                            session->pairInfo.sRand, 
                                            session->pairInfo.preq,
                                            session->pairInfo.pres,
                                            session->bdc->addrType,
                                            LE_ADDR_PUBLIC,
                                            session->bdc->addr,
                                            MEC(bdaddr));
                session->smState = SM_PHASE2_CONFIRM_WAIT;
            }
            break;
        case SM_PHASE2_CONFIRM_WAIT:
            if (session->type == SM_PAIR_TYPE_PAIR)
            {
                OS_MemCopy(session->pairInfo.sConfirm, event->p.smConfirmResponse.ConfirmValue, SM_CONFIRM_VALUE_LENGTH);
                if (session->pairInfo.valueDist & SM_VALUE_DIST_MCONFIRM)
                {
                    /* Already received MConfirm, proceed */
                    session->smState = SM_PHASE2_CONFIRM_SEND;
                    status = BT_STATUS_SUCCESS;
                }
                else
                {
                    /* Not yet receive MConfirm */
                    session->smState = SM_PHASE2_CONFIRM_RECEIVE;
                    status = BT_STATUS_PENDING;
                }
            }
            else
            {
                Assert(0);
            }
            break;
        case SM_PHASE2_CONFIRM_SEND:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                OS_MemCopy(session->pairInfo.mConfirm, event->p.smConfirmResponse.ConfirmValue, SM_CONFIRM_VALUE_LENGTH);
                status = SM_SendPairConfirm(session->bdc->link, session->pairInfo.mConfirm);
                session->pairInfo.valueDist |= SM_VALUE_DIST_MCONFIRM;
                session->smState = SM_PHASE2_CONFIRM_RECEIVE;
            }
            else
            {
                status = SM_SendPairConfirm(session->bdc->link, session->pairInfo.sConfirm);
                session->pairInfo.valueDist |= SM_VALUE_DIST_SCONFIRM;
                session->smState = SM_PHASE2_RAND_RECEIVE;
            }
            break;
        case SM_PHASE2_CONFIRM_RECEIVE:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                OS_MemCopy(session->pairInfo.sConfirm, event->p.smConfirmResponse.ConfirmValue, SM_CONFIRM_VALUE_LENGTH);
                session->pairInfo.valueDist |= SM_VALUE_DIST_SCONFIRM;
                session->smState = SM_PHASE2_RAND_SEND;
                /* fall-through */
            }
            else
            {
                OS_MemCopy(session->pairInfo.mConfirm, event->p.smConfirmResponse.ConfirmValue, SM_CONFIRM_VALUE_LENGTH);
                session->pairInfo.valueDist |= SM_VALUE_DIST_MCONFIRM;
                session->smState = SM_PHASE2_CONFIRM_SEND;
                status = BT_STATUS_SUCCESS;
                break;
            }
        case SM_PHASE2_RAND_SEND:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                status = SM_SendPairRandom(session->bdc->link, session->pairInfo.mRand);
                session->pairInfo.valueDist |= SM_VALUE_DIST_MRAND;
                session->smState = SM_PHASE2_RAND_RECEIVE;
            }
            else
            {
                SM_SendPairRandom(session->bdc->link, session->pairInfo.sRand);
                session->pairInfo.valueDist |= SM_VALUE_DIST_SRAND;
                session->smState = SM_PHASE2_CALC_STK;
            }
            break;
        case SM_PHASE2_RAND_RECEIVE:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                OS_MemCopy(session->pairInfo.sRand, event->p.smRand128Response.randValue128, SM_RANDOM_VALUE_LENGTH);
                session->pairInfo.valueDist |= SM_VALUE_DIST_SRAND;
                status = SM_CalcPairConfirm(event->p.remDev,
                                            session->pairInfo.TK, 
                                            session->pairInfo.sRand, 
                                            session->pairInfo.preq,
                                            session->pairInfo.pres,
                                            LE_ADDR_PUBLIC,
                                            session->bdc->addrType,
                                            MEC(bdaddr),
                                            session->bdc->addr);
            }
            else
            {
                OS_MemCopy(session->pairInfo.mRand, event->p.smRand128Response.randValue128, SM_RANDOM_VALUE_LENGTH);
                session->pairInfo.valueDist |= SM_VALUE_DIST_MRAND;
                status = SM_CalcPairConfirm(event->p.remDev,
                                            session->pairInfo.TK, 
                                            session->pairInfo.mRand, 
                                            session->pairInfo.preq,
                                            session->pairInfo.pres,
                                            session->bdc->addrType,
                                            LE_ADDR_PUBLIC,
                                            session->bdc->addr,
                                            MEC(bdaddr));
            }
            session->smState = SM_PHASE2_CONFIRM_VERIFY;
            break;
        case SM_PHASE2_CONFIRM_VERIFY:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                if (OS_MemCmp(session->pairInfo.sConfirm, SM_CONFIRM_VALUE_LENGTH, event->p.smConfirmResponse.ConfirmValue, SM_CONFIRM_VALUE_LENGTH))
                {
                    /* Pairing successful, fall-through to calculate STK */
                    session->smState = SM_PHASE2_CALC_STK;
                }
                else
                {
                    SM_SendPairFailed(session->bdc->link, SM_REASON_CONFIRM_VALUE_FAILED);
                    session->smpFailSent = TRUE;
                    status = BT_STATUS_KEY_ERR;
                    goto failed;
                }
            }
            else
            {
                if (OS_MemCmp(session->pairInfo.mConfirm, SM_CONFIRM_VALUE_LENGTH, event->p.smConfirmResponse.ConfirmValue, SM_CONFIRM_VALUE_LENGTH))
                {
                    /* Pairing successful, send sRand */
                    session->smState = SM_PHASE2_RAND_SEND;
                }
                else
                {
                    SM_SendPairFailed(session->bdc->link, SM_REASON_CONFIRM_VALUE_FAILED);
                    session->smpFailSent = TRUE;
                    status = BT_STATUS_KEY_ERR;
                    goto failed;
                }
                break;
            }
            /* fall-through */
        case SM_PHASE2_CALC_STK:
            status = SM_CalcSTK(event->p.remDev, session->pairInfo.TK, session->pairInfo.sRand, session->pairInfo.mRand);
            session->smState = SM_PHASE2_PAIR_DONE;
            break;
        case SM_PHASE2_PAIR_DONE:
            OS_MemCopy(session->pairInfo.STK, event->p.smEncryptResponse.EncryptData, SM_STK_VALUE_LENGTH);
            /* Mask STK to max encryption key size */
#ifdef LE_DEBUG
            OS_MemCopy(session->pairInfo.STK, sampleLTK, SM_STK_VALUE_LENGTH);
#endif
            if (session->pairInfo.encKeySize < 16)
            {
                SM_util_mask_MSBs(session->pairInfo.STK, SM_STK_VALUE_LENGTH, 16 - session->pairInfo.encKeySize, 0x0);
            }
            session->pairInfo.record.devType = session->bdc->devType;
            session->pairInfo.record.addrType = session->bdc->addrType;
            session->pairInfo.record.bdAddr = session->bdc->addr;
            session->pairInfo.record.gapKeyType = ((session->pairInfo.model == SM_MODEL_JUST_WORK) ? 
                                                    BT_LINK_KEY_GAP_TYPE_UNAUTHENTICATED : BT_LINK_KEY_GAP_TYPE_AUTHENTICATED);
            OS_MemCopy(session->pairInfo.record.linkKey, session->pairInfo.STK, SM_STK_VALUE_LENGTH);
            SEC_AddDeviceRecord(&session->pairInfo.record);
            if (session->pairAttr.bond == SM_BONDING_YES)
            {
                session->bdc->link->pairingType = BPT_SAVE_NOT_TRUSTED;
            }
            else
            {
                session->bdc->link->pairingType = BPT_NOT_SAVED;
            }
            session->bdc->link->authState = BAS_AUTHENTICATED;
            session->bdc->link->secAccessState |= BAS_AUTH_COMPLETE;

            session->smState = SM_PHASE2_ENCRYPT;
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                /* fall-through */
            }
            else
            {
                /* wait for encrypt request */
                session->bdc->link->authState = BAS_WAITING_KEY_REQ;
                status = BT_STATUS_PENDING;
                break;
            }
        case SM_PHASE2_ENCRYPT:
            if (session->type == SM_PAIR_TYPE_BOND)
            {
                if (session->bdc->link->remoteLEFeature[0] & 0x1)
                {
                    BtLEEncParm parm;
                    parm.random = 0;
                    parm.ediv = 0;
                    OS_MemCopy(parm.ltk, session->pairInfo.STK, SM_LTK_VALUE_LENGTH);
                    status = ME_LE_EncryptLink(event->p.remDev, &parm);
                    session->bdc->link->authState = BAS_SENDING_KEY;
                }
                else
                {
                    /* WCNCR00005488 : Move SecProcessAllSecurityTokens to SM_EndSessionFSM */
                    //SecProcessAllSecurityTokens(session->bdc->link);
                    session->smState = SM_PHASE3_DONE;
                    SM_EndSession(session, BT_STATUS_SUCCESS);
                    return;
                }
            }
            else
            {
                LeHciLTKRequestReply(event->p.remDev->hciHandle, session->pairInfo.STK);
                status = BT_STATUS_PENDING;
            }
            SM_UpdateSession(session, SM_Phase3FSM);
            session->smState = SM_PHASE3_ENCRYPTED;
            break;
        default:
            Assert(0);
        }
    }

    if (status == BT_STATUS_PENDING)
    {
        return;
    }
failed:
    SM_EndSession(session, status);
}

static BOOL SM_Phase3_KeyReceive(const BtEvent *event, SmKeyDistribute keys)
{
    SmSession *session = SM_GetSession(event->p.remDev);  /* Trick: put remDev as 1st field in all LE event structures */

    bt_trace(TRACE_FUNC, BT_SM_SM_PHASE3KEYRECEIVE, event->eType, keys);
    switch (event->eType)
    {
    case BTEVENT_LE_LTK_RECEIVED:
        if (!(session->pairInfo.record.smKeyType & SM_KEY_LTK))
        {
            session->pairInfo.record.smKeyType |= SM_KEY_LTK;
            OS_MemCopy(session->pairInfo.record.linkKey, event->p.smLTKInfo.ltk, SM_LTK_VALUE_LENGTH);
            if (session->pairInfo.encKeySize < SM_LTK_VALUE_LENGTH)
            {
                SM_util_mask_MSBs(session->pairInfo.record.linkKey, SM_LTK_VALUE_LENGTH, SM_LTK_VALUE_LENGTH - session->pairInfo.encKeySize, 0x0);
            }
        }
        break;
    case BTEVENT_LE_EDIV_RAND_RECEIVED:
        if (!(session->pairInfo.record.smKeyType & SM_KEY_EDIV))
        {
            session->pairInfo.record.ediv = event->p.smMasterIDInfo.ediv;
            session->pairInfo.record.smKeyType |= SM_KEY_EDIV;
            session->pairInfo.record.rand = event->p.smMasterIDInfo.rand;
            session->pairInfo.record.smKeyType |= SM_KEY_RAND;
        }
        break;
#ifdef __BT_4_0_PRIVACY__
    case BTEVENT_LE_IRK_RECEIVED:
        if (!(session->pairInfo.record.smKeyType & SM_KEY_IRK))
        {
            session->pairInfo.record.smKeyType |= SM_KEY_IRK;
            OS_MemCopy(session->pairInfo.record.irk, event->p.smIDInfo.irk, SM_IRK_VALUE_LENGTH);
        }
        break;
    case BTEVENT_LE_ADDR_RECEIVED:
        if (!(session->pairInfo.record.smKeyType & SM_KEY_ADDR))
        {
            session->pairInfo.record.smKeyType |= SM_KEY_ADDR;
            session->pairInfo.record.addrType = event->p.smIDAddrInfo.addrType;
            session->pairInfo.record.bdAddr = event->p.smIDAddrInfo.addr;
        }
        break;
#endif
    case BTEVENT_LE_CSRK_RECEIVED:
        if (!(session->pairInfo.record.smKeyType & SM_KEY_CSRK))
        {
            session->pairInfo.record.smKeyType |= SM_KEY_CSRK;
            OS_MemCopy(session->pairInfo.record.csrk, event->p.smSignInfo.csrk, SM_CSRK_VALUE_LENGTH);
        }
        break;
    default:
//        Assert(0);
        OS_Report("SM_Phase3_KeyReceive(): Receive unexpected key");
        break;
    }

    /* Check if all keys are received */
    if (keys & SM_KEY_DIST_LTK)
    {
        if ((session->pairInfo.record.smKeyType & SM_KEY_LTK) && 
            (session->pairInfo.record.smKeyType & SM_KEY_EDIV) &&
            (session->pairInfo.record.smKeyType & SM_KEY_RAND))
        {
        }
        else
        {
            return FALSE;
        }
    }

#ifdef __BT_4_0_PRIVACY__
    if (keys & SM_KEY_DIST_IRK)
    {
        if ((session->pairInfo.record.smKeyType & SM_KEY_IRK) && 
            (session->pairInfo.record.smKeyType & SM_KEY_ADDR))
        {
        }
        else
        {
            return FALSE;
        }
    }
#endif

    if (keys & SM_KEY_DIST_CSRK)
    {
        if ((session->pairInfo.record.smKeyType & SM_KEY_CSRK))
        {
//            session->pairInfo.record.signCounter = 0;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL SM_Phase3_KeySend(SmSession *session, SmKeyDistribute keys)
{
    U64 rand64;
    U8 array[SM_CSRK_VALUE_LENGTH];

    bt_trace(TRACE_FUNC, BT_SM_SM_PHASE3KEYSEND, session->pairInfo.record.distSmKeyType, keys);
    if ((keys & SM_KEY_DIST_LTK) && (session->pairInfo.record.distSmKeyType & SM_KEY_LTK) == 0)
    {
        /* 128bit LTK */
        SM_GenLTK(session->pairInfo.record.distLTK, session->bdc->addr, session->pairInfo.encKeySize);
        SM_SendEncryptInfo(session->bdc->link, session->pairInfo.record.distLTK);
        session->pairInfo.record.distSmKeyType |= SM_KEY_LTK;
        return FALSE;
    }
    else if ((keys & SM_KEY_DIST_LTK) && (session->pairInfo.record.distSmKeyType & SM_KEY_EDIV) == 0)
    {
        /* 16bit EDIV */
        session->pairInfo.record.distEDIV = SM_GenEDIV();
        /* 64bit RAND */
        rand64 = LEtoHost64(session->pairInfo.sRand);
        rand64 = (U64)rand64 >> 32;
        rand64 = (((U64)rand64 << 32) | 0x00000000);
//        session->pairInfo.record.distRAND = (U64)((rand64 & (U64)0xFFFFFFFF00000000) | (U64)OS_GetSystemTime());
        session->pairInfo.record.distRAND = (U64)((rand64) | (U64)OS_GetSystemTime());

        SM_SendMasterIdentity(session->bdc->link, session->pairInfo.record.distEDIV, session->pairInfo.record.distRAND);
        session->pairInfo.record.distSmKeyType |= SM_KEY_EDIV;
        session->pairInfo.record.distSmKeyType |= SM_KEY_RAND;
        return FALSE;
    }
#ifdef __BT_4_0_PRIVACY__
    else if ((keys & SM_KEY_DIST_IRK) && (session->pairInfo.record.distSmKeyType & SM_KEY_IRK) == 0)
    {
        DDB_ReadLeKey(NULL, NULL, array);
        SM_SendIdentityInfo(session->bdc->link, array);
        SM_SendIdentityAddrInfo(session->bdc->link, LE_ADDR_PUBLIC, MEC(bdaddr));
        session->pairInfo.record.distSmKeyType |= SM_KEY_IRK;
        session->pairInfo.record.distSmKeyType |= SM_KEY_ADDR;
        return FALSE;
    }
#endif
    else if ((keys & SM_KEY_DIST_CSRK) && (session->pairInfo.record.distSmKeyType & SM_KEY_CSRK) == 0)
    {
        DDB_ReadLeKey(array, NULL, NULL);
        SM_SendSigningInfo(session->bdc->link, array);
        session->pairInfo.record.distSmKeyType |= SM_KEY_CSRK;
        return FALSE;
    }

    return TRUE;
}

U8 cmac_test_buf[27] = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'b', 'o', 'o', 'k', '\0'};
BtPacket CMACPacket;
void sm_cmac_callback(const BtEvent *event)
{
    CMACPacket.data = event->p.smSignData.data;
    CMACPacket.dataLen = event->p.smSignData.dataLen;
    CMACPacket.headerLen = 0;
    CMACPacket.flags = BTP_FLAG_INUSE;
    CMACPacket.priority_type = BTP_FLAG_HIGH_PRIPORTY;
    L2Cap_FixCidSend(SM_L2CAP_CID, smCtx.session[0].bdc->link, &CMACPacket);
    SM_EndSession(&smCtx.session[0], BT_STATUS_SUCCESS);
}

void sm_cmac_verify_callback(const BtEvent *event)
{
    if (event->errCode == BEC_NO_ERROR)
    {
        OS_Report("!!!!CMAC VERIFY SUCCESS!!!!");
    }
    else
    {
        OS_Report("!!!!CMAC VERIFY FAIL!!!!");
    }
    SM_EndSession(&smCtx.session[0], BT_STATUS_SUCCESS);
}

static void SM_Phase3FSM(const BtEvent *event)
{
    SmSession *session = SM_GetSession(event->p.remDev);  /* Trick: put remDev as 1st field in all LE event structures */
    BtStatus status = BT_STATUS_SUCCESS;

    if (session == NULL)
    {
        bt_trace(TRACE_ERROR, BT_SM_SM_PHASEFSM_SESSION_NOT_FOUND, 3);
        return;
    }
    
    if ((status = SM_HandleEvent(event)) != BT_STATUS_SUCCESS)
    {
        goto failed;
    }

    while (status == BT_STATUS_SUCCESS)
    {
        bt_trace(TRACE_GROUP_1, BT_SM_SM_PHASEFSM, session->type, session->smState);
        switch (session->smState)
        {
        case SM_PHASE3_ENCRYPTED:
            OS_Report("link role=%d, session->pairInfo.respKeyDist=0x%x", session->bdc->link->role, session->pairInfo.respKeyDist);
            if (session->bdc->link->role == BCR_MASTER)
            {
                session->smState = ((session->pairInfo.respKeyDist) ? SM_PHASE3_KEY_RECEIVE : SM_PHASE3_KEY_DIST);
            }
            else if (session->bdc->link->role == BCR_SLAVE)
            {
                session->smState = ((session->pairInfo.respKeyDist) ? SM_PHASE3_KEY_DIST : SM_PHASE3_KEY_RECEIVE);
            }
            
            if (session->smState == SM_PHASE3_KEY_RECEIVE)
            {
                status = BT_STATUS_PENDING;
            }
            break;
        case SM_PHASE3_KEY_RECEIVE:
            if (session->type == SM_PAIR_TYPE_PAIR && SM_Phase3_KeyReceive(event, session->pairInfo.initKeyDist))
            {
                session->smState = SM_PHASE3_DONE;
                status = BT_STATUS_SUCCESS;
                break;
            }
            else if (session->type == SM_PAIR_TYPE_BOND && SM_Phase3_KeyReceive(event, session->pairInfo.respKeyDist))
            {
                session->smState = SM_PHASE3_KEY_DIST;
                /* fall-through */
            }
            else
            {
                status = BT_STATUS_PENDING;
                break;
            }
        case SM_PHASE3_KEY_DIST:
            if (session->type == SM_PAIR_TYPE_PAIR && SM_Phase3_KeySend(session, session->pairInfo.respKeyDist))
            {
                if (session->pairInfo.initKeyDist)
                {
                    session->smState = SM_PHASE3_KEY_RECEIVE;
                    status = BT_STATUS_PENDING;
                    break;
                }
                else
                {
                    session->smState = SM_PHASE3_DONE;
                    /* fall-through */
                }
            }
            else if (session->type == SM_PAIR_TYPE_BOND && SM_Phase3_KeySend(session, session->pairInfo.initKeyDist))
            {
                session->smState = SM_PHASE3_DONE;
                /* fall-through */
            }
            else
            {
                status = BT_STATUS_PENDING;
                break;
            }
        case SM_PHASE3_DONE:
            SEC_DeleteDeviceRecord(&session->bdc->addr);
            SEC_AddDeviceRecord(&session->pairInfo.record);
            SM_EndSession(session, BT_STATUS_SUCCESS);
            return;
        default:
            Assert(0);
        }
    }
    
    if (status == BT_STATUS_PENDING)
    {
        return;
    }
failed:
    SM_EndSession(session, status);
}

unsigned char const_Rb[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
};
unsigned char const_Zero[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void SM_CMAC_FSM(const BtEvent *event)
{
    U8 *X, *L, K1[16], K2[16], tmp[16];
    SmSignData *curSignData = (SmSignData *)GetHeadList(&smCtx.cmacCntx.signQueue);
    BtEvent newEvent;

    bt_trace(TRACE_FUNC, BT_SM_SM_CMACFSM, smCtx.cmacCntx.state);
    switch (smCtx.cmacCntx.state)
    {
    case SM_CMAC_STATE_INIT:
    case SM_CMAC_STATE_GEN_SUBKEY_1:
        /* L = AES_128(k,Z) */
        LeHciAES128Encrypt(SM_CMAC_FSM, curSignData->k, const_Zero);
        smCtx.cmacCntx.state = SM_CMAC_STATE_GEN_SUBKEY_2;
        break;
    case SM_CMAC_STATE_GEN_SUBKEY_2:
        L = (U8 *)event->p.smEncryptResponse.EncryptData;
        if ( (L[0] & 0x80) == 0 ) 
        {
            /* If MSB(L) = 0, then K1 = L << 1 */
            SM_util_leftshift_1bit(L, K1);
        } 
        else 
        {    /* Else K1 = ( L << 1 ) (+) Rb */
            SM_util_leftshift_1bit(L, tmp);
            SM_util_128bit_XOR(K1, tmp, const_Rb);
        }
    
        if ( (K1[0] & 0x80) == 0 ) 
        {
            SM_util_leftshift_1bit(K1, K2);
        } 
        else 
        {
            SM_util_leftshift_1bit(K1, tmp);
            SM_util_128bit_XOR(K2, tmp, const_Rb);
        }
        smCtx.cmacCntx.state = SM_CMAC_STATE_BLOCK_CIPHER;
        /* fall-through */
    case SM_CMAC_STATE_BLOCK_CIPHER:
        smCtx.cmacCntx.blockNo = (curSignData->mLen + 15) / 16;       /* n is number of rounds */
        if (smCtx.cmacCntx.blockNo > 0 && (curSignData->mLen % 16) == 0)
        { /* last block is a complete block */
            SM_util_128bit_XOR(smCtx.cmacCntx.mLast, &curSignData->m[16 * (smCtx.cmacCntx.blockNo - 1)], K1);
        }
        else
        { /* last block is not complete block */
            smCtx.cmacCntx.blockNo = ((smCtx.cmacCntx.blockNo == 0) ? 1 : smCtx.cmacCntx.blockNo);
            SM_util_cmac_padding(&curSignData->m[16 * (smCtx.cmacCntx.blockNo - 1)], tmp, curSignData->mLen % 16);
            SM_util_128bit_XOR(smCtx.cmacCntx.mLast, tmp, K2);
        }
        smCtx.cmacCntx.blockCount = 0;
        smCtx.cmacCntx.state = SM_CMAC_STATE_BLOCK_CIPHER_LOOP;
        /* fall-through */
    case SM_CMAC_STATE_BLOCK_CIPHER_LOOP:
        if (smCtx.cmacCntx.blockCount == 0)
        {
            X = const_Zero;
        }
        else
        {
            X = (U8 *)event->p.smEncryptResponse.EncryptData;
        }
        if (smCtx.cmacCntx.blockCount < smCtx.cmacCntx.blockNo - 1)
        {
            SM_util_128bit_XOR(tmp, X, &curSignData->m[16 * smCtx.cmacCntx.blockCount]); /* Y = Mi (+) X  */
            /* X = AES_128(key,Y) */
            LeHciAES128Encrypt(SM_CMAC_FSM, curSignData->k, tmp);
            smCtx.cmacCntx.blockCount++;
        }
        else
        {
            SM_util_128bit_XOR(tmp, X, smCtx.cmacCntx.mLast);
            /* X = AES_128(key,Y) */
            LeHciAES128Encrypt(SM_CMAC_FSM, curSignData->k, tmp);
            smCtx.cmacCntx.state = SM_CMAC_STATE_DONE;
        }
        break;
    case SM_CMAC_STATE_DONE:
        OS_MemCopy(curSignData->m + curSignData->mLen, event->p.smEncryptResponse.EncryptData, curSignData->tLen);
        newEvent.eType = BTEVENT_LE_SIGN_COMPLETE;
        newEvent.errCode = BEC_NO_ERROR;
        newEvent.p.smSignData.bdAddr = curSignData->remoteAddr;
        newEvent.p.smSignData.data = curSignData->m;
        newEvent.p.smSignData.dataLen = curSignData->mLen + curSignData->tLen;  /* with signature */
        curSignData->callback((const BtEvent *)&newEvent);
        RemoveEntryList(&curSignData->node);
        if (IsListEmpty(&smCtx.cmacCntx.signQueue))
        {
            smCtx.cmacCntx.state = SM_CMAC_STATE_NULL;
        }
        else
        {
            smCtx.cmacCntx.state = SM_CMAC_STATE_INIT;
            SM_CMAC_FSM(NULL);
        }
        break;
    default:
        Assert(0);
    }
}

void SM_CMAC(BtCallBack callback, BD_ADDR *bdAddr, U8 *k, U8 *m, U32 mLen, U32 tLen)
{
    /* chain to list */
    int i;
    SmSignData *data;

    bt_trace(TRACE_FUNC, BT_SM_SM_CMAC, mLen, tLen);
    bt_parm_trace(16, k);
    bt_parm_trace(mLen, m);
    for (i = 0; i < SM_MAX_SIGN_QUEUE_SIZE && !IsEntryAvailable(&smCtx.cmacCntx.signData[i].node); i++);
    Assert(i < SM_MAX_SIGN_QUEUE_SIZE);
    data = &smCtx.cmacCntx.signData[i];
    OS_MemCopy(data->k, k, 16);
    data->m = m;
    data->mLen = mLen;
    data->tLen = tLen;
    data->remoteAddr = *bdAddr;
    data->callback = callback;
    InsertTailList(&smCtx.cmacCntx.signQueue, &data->node);
    if (smCtx.cmacCntx.state == SM_CMAC_STATE_NULL)
    {
        smCtx.cmacCntx.state = SM_CMAC_STATE_INIT;
        SM_CMAC_FSM(NULL);
    }
}

static void SM_CMAC_VerifyCB(const BtEvent *event)
{
    Assert(event->eType == BTEVENT_LE_SIGN_COMPLETE);
    bt_trace(TRACE_FUNC, BT_SM_SM_CMACVERIFYCB, event->errCode);
    if(event->errCode == BEC_NO_ERROR)
    {
        BOOL is_mac_equal;
        BtEvent newEvent;
        
        SmSignVerifyData *curVerifyData = (SmSignVerifyData *)GetHeadList(&smCtx.cmacCntx.verifyQueue);
        is_mac_equal = OS_MemCmp(curVerifyData->mac, SM_MAC_LENGTH, &event->p.smSignData.data[event->p.smSignData.dataLen - SM_MAC_LENGTH], SM_MAC_LENGTH);
        bt_parm_trace(SM_MAC_LENGTH, curVerifyData->mac);
        bt_parm_trace(SM_MAC_LENGTH, &event->p.smSignData.data[event->p.smSignData.dataLen - SM_MAC_LENGTH]);

        newEvent.eType = BTEVENT_LE_SIGN_VERIFY_COMPLETE;
        newEvent.errCode = (is_mac_equal) ? BEC_NO_ERROR : BEC_AUTHENTICATE_FAILURE;
        newEvent.p.smSignData.bdAddr = curVerifyData->remoteAddr;
        newEvent.p.smSignData.data = curVerifyData->buf;
        newEvent.p.smSignData.dataLen = curVerifyData->bufLen - 4;  /* signature excluded */
        curVerifyData->callback(&newEvent);
        RemoveEntryList(&curVerifyData->node);
    }
    else
    {
        Assert(0);
    }
}

void SM_CMAC_Verify(BtCallBack callback, BD_ADDR *bdAddr, U8 *k, U8 *m, U32 mLen, U32 tLen, U8 *mac)
{
    /* chain to list */
    int i;

    bt_trace(TRACE_FUNC, BT_SM_SM_CMACVERIFY);
    bt_parm_trace(SM_MAC_LENGTH, mac);
    for (i = 0; i < SM_MAX_SIGN_QUEUE_SIZE && !IsEntryAvailable(&smCtx.cmacCntx.verifyData[i].node); i++);
    Assert(i < SM_MAX_SIGN_QUEUE_SIZE);
    OS_MemCopy(smCtx.cmacCntx.verifyData[i].mac, mac, SM_MAC_LENGTH);
    smCtx.cmacCntx.verifyData[i].bufLen = mLen;
    smCtx.cmacCntx.verifyData[i].buf = m;
    smCtx.cmacCntx.verifyData[i].callback = callback;
    smCtx.cmacCntx.verifyData[i].remoteAddr = *bdAddr;
    InsertTailList(&smCtx.cmacCntx.verifyQueue, &smCtx.cmacCntx.verifyData[i].node);
    SM_CMAC(SM_CMAC_VerifyCB, bdAddr, k, m, mLen, tLen);
}

void SM_RecvData(BtRemoteDevice *remDev, U8 *data, U16 dataLen)
{
    BtEvent event;
    SmSession *session = SM_GetSession(remDev);

    bt_trace(TRACE_FUNC, BT_SM_SM_RECVDATA, data[0], ((session == NULL) ? 0 : session->smState), remDev->encryptState);
    switch (data[0])
    {
    case SMC_PAIR_REQ:
        if (session == NULL)
        {
            BtDeviceContext *bdc = DS_FindDevice(&remDev->bdAddr);
            SmPairAttribute attr = {SM_DEFAULT_MITM_PROTECT, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG};
#ifdef __UPF__
if (DDB_ReadUPF(&upfParm))
{
attr.bond = upfParm.bond;
attr.mitm = upfParm.mitm;
attr.oob = upfParm.oob;
}
#endif
            BMController.bm_opactivity = BM_OP_PAIRING;
            BMController.pin_bdc = bdc;
            session = SM_StartSession(bdc, SM_Phase1FSM, SM_PAIR_TYPE_PAIR, &attr);
            session->smState = SM_SESSION_INIT;

            InsertTailList(&(remDev->handlers), &session->smHandler.node);
            event.eType = BTEVENT_LE_SM_RESULT;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smPairRequest.remDev = remDev;
            event.p.smPairRequest.ioCapability = data[1];
            event.p.smPairRequest.OOBDataPresent = data[2];
            event.p.smPairRequest.authRequirements = data[3];
            event.p.smPairRequest.maxEncKeySize = data[4];
            event.p.smPairRequest.initKeyDist = (data[5] & 0x7);
            event.p.smPairRequest.respKeyDist = (data[6] & 0x7);
            OS_MemCopy(session->pairInfo.preq, data, 7);
            bt_parm_trace(7, &data[1]);
            SM_LinkCallback(bdc->link, &event);
        }
        break;
    case SMC_PAIR_RSP:
        if (session && session->smState == SM_PHASE1_PAIR_RSP_RECEIVE)
        {
            event.eType = BTEVENT_LE_SM_RESULT;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smPairResponse.remDev = remDev;
            event.p.smPairResponse.ioCapability = data[1];
            event.p.smPairResponse.OOBDataPresent = data[2];
            event.p.smPairResponse.authRequirements = data[3];
            event.p.smPairResponse.maxEncKeySize = data[4];
            event.p.smPairResponse.initKeyDist = (data[5] & 0x7);
            event.p.smPairResponse.respKeyDist = (data[6] & 0x7);
            OS_MemCopy(session->pairInfo.pres, data, 7);
            bt_parm_trace(7, &data[1]);
            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_PAIR_CONFIRM:
        if (session)
        {
            if (session->smState == SM_PHASE2_CONFIRM_RECEIVE)
            {
                event.eType = BTEVENT_LE_SM_RESULT;
                event.errCode = HC_STATUS_SUCCESS;
                event.p.smConfirmResponse.remDev = remDev;
                OS_MemCopy(event.p.smConfirmResponse.ConfirmValue, &data[1], SM_CONFIRM_VALUE_LENGTH);
                bt_parm_trace(SM_CONFIRM_VALUE_LENGTH, &data[1]);
                SM_LinkCallback(remDev, &event);
            }
            else if (session->type == SM_PAIR_TYPE_PAIR &&
                (session->smState > SM_PHASE1_PAIR_RSP_SEND && session->smState < SM_PHASE2_CONFIRM_RECEIVE))
            {
                /* Cache the received value */
                OS_Report("cache mConfirm");
                session->pairInfo.valueDist |= SM_VALUE_DIST_MCONFIRM;
                OS_MemCopy(session->pairInfo.mConfirm, &data[1], SM_CONFIRM_VALUE_LENGTH);
            }
        }
        break;
    case SMC_PAIR_RANDOM:
        if (session && session->smState == SM_PHASE2_RAND_RECEIVE)
        {
            event.eType = BTEVENT_LE_SM_RESULT;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smRand128Response.remDev = remDev;
            OS_MemCopy(event.p.smRand128Response.randValue128, &data[1], SM_RANDOM_VALUE_LENGTH);
            bt_parm_trace(SM_RANDOM_VALUE_LENGTH, &data[1]);
            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_PAIR_FAILED:
        if (session && session->smState > SM_PHASE1_PAIR_REQ_SEND)
        {
            session->smpFailSent = TRUE;
            event.eType = BTEVENT_LE_SM_RESULT;
            event.errCode = bt_util_convert_err5(data[1]);
            event.p.remDev = remDev;
            bt_parm_trace(1, &data[1]);
            SM_EndSession(session, bt_util_convert_err3(event.errCode));
        }
        break;
    case SMC_ENCRYPT_INFO:
        if (session && session->smState == SM_PHASE3_KEY_RECEIVE)
        {
            event.eType = BTEVENT_LE_LTK_RECEIVED;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smLTKInfo.remDev = remDev;
            OS_MemCopy(event.p.smLTKInfo.ltk, &data[1], SM_LTK_VALUE_LENGTH);
            bt_parm_trace(SM_LTK_VALUE_LENGTH, &data[1]);
            Report(("UPF Debug LTK Receive from Remote Side"));
            Report(("UPF Debug LTK LSB->MSB:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
                data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8]));
            Report(("UPF Debug LTK LSB->MSB:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
                data[9],data[10],data[11],data[12],data[13],data[14],data[15],data[16]));

            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_MASTER_IDEN:
        if (session && session->smState == SM_PHASE3_KEY_RECEIVE)
        {
            event.eType = BTEVENT_LE_EDIV_RAND_RECEIVED;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smMasterIDInfo.remDev = remDev;
            event.p.smMasterIDInfo.ediv = LEtoHost16(&data[1]);
            event.p.smMasterIDInfo.rand = LEtoHost64(&data[3]);
            Report(("UPF Debug ediv Receive from Remote Side:%04x", event.p.smMasterIDInfo.ediv));
            
            Report(("UPF Debug edrandiv Send from Remote Side LSB->MSB:%02x,%02x,%02x,02x,%02x,%02x,%02x,%02x",
                data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]));

            
            bt_parm_trace(10, &data[1]);
            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_IDEN_INFO:
        if (session && session->smState == SM_PHASE3_KEY_RECEIVE)
        {
            event.eType = BTEVENT_LE_IRK_RECEIVED;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smIDInfo.remDev = remDev;
            OS_MemCopy(event.p.smIDInfo.irk, &data[1], SM_IRK_VALUE_LENGTH);
            bt_parm_trace(SM_IRK_VALUE_LENGTH, &data[1]);
            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_IDEN_ADDR_INFO:
        if (session && session->smState == SM_PHASE3_KEY_RECEIVE)
        {
            event.eType = BTEVENT_LE_ADDR_RECEIVED;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smIDAddrInfo.remDev = remDev;
            event.p.smIDAddrInfo.addrType = data[1];
            OS_MemCopy(event.p.smIDAddrInfo.addr.addr, &data[2], BD_ADDR_SIZE);
            bt_parm_trace(7, &data[1]);
            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_SIGN_INFO:
        if (session && session->smState == SM_PHASE3_KEY_RECEIVE)
        {
            event.eType = BTEVENT_LE_CSRK_RECEIVED;
            event.errCode = HC_STATUS_SUCCESS;
            event.p.smSignInfo.remDev = remDev;
            OS_MemCopy(event.p.smSignInfo.csrk, &data[1], SM_CSRK_VALUE_LENGTH);
            bt_parm_trace(SM_CSRK_VALUE_LENGTH, &data[1]);
            SM_LinkCallback(remDev, &event);
        }
        break;
    case SMC_SECURITY_REQ:
        if (session == NULL && remDev->encryptState != BES_START_ENCRYPT)
        {
            BtDeviceRecord rec;
            BOOL mitm = ((data[1] & 0x4) ? TRUE : FALSE);
            /* Ref. Figure 2.2 in SM spec. */
            if ((SEC_FindDeviceRecord(&remDev->bdAddr, &rec) == BT_STATUS_SUCCESS) &&
                (rec.smKeyType & SM_KEY_LTK) &&
                (!mitm || 
                 (mitm && (rec.gapKeyType == BT_LINK_KEY_GAP_TYPE_AUTHENTICATED))))
            {
                ME_LE_EncryptLink(remDev, NULL);
            }
            else
            {
//                SmBondingFlag flag = ((data[1] == 0x1) ? SM_BONDING_YES : SM_BONDING_NO);
                SmPairAttribute attr = {mitm, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG};
#ifdef __UPF__
if (DDB_ReadUPF(&upfParm))
{
attr.oob = upfParm.oob;
}
#endif
                SM_Bond(DS_FindDevice(&remDev->bdAddr), &attr);
            }
        }
        break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  SM_LinkCallback
 * DESCRIPTION
 *  Call the callback of all the handlers bound to the remote device.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SM_LinkCallback(BtRemoteDevice *remDev, BtEvent *event)
{
    MeCallLinkHandlers(remDev, event);
}

void SM_L2capCallback(L2capChannelId cid, L2capCallbackParms *Parms)
{
    BtEvent event;
    SmSession *session = SM_GetSession(Parms->aclLink);
    int i;
    
    bt_trace(TRACE_FUNC, BT_SM_SM_L2CAPCALLBACK, Parms->event, ((session == NULL) ? 0 : session->smState));
    switch (Parms->event)
    {
    case L2EVENT_CONNECTED:
        break;
    case L2EVENT_PACKET_HANDLED:
        for (i = 0; i < SM_MAX_PKT_NUM; i++)
        {
            if (Parms->ptrs.packet == &smCtx._cmdPacket[i].packet)
            {
                SM_FreeCmdPkt(Parms->ptrs.packet);
                if (session)
                {
                    if (session->smGCState == SM_GC_STATE_DISC)  /* SMC_PAIR_FAILED is sent, proceed */
                    {
                        event.eType = BTEVENT_LE_SM_RESULT;
                        event.p.remDev = Parms->aclLink;
                        SM_LinkCallback(Parms->aclLink, &event);
                    }
                    else if (session->smState == SM_PHASE3_KEY_DIST)
                    {
                        event.eType = BTEVENT_LE_SM_RESULT;
                        event.errCode = HC_STATUS_SUCCESS;
                        event.p.remDev = Parms->aclLink;
                        SM_LinkCallback(Parms->aclLink, &event);
                    }
                }
                break;
            }
        }
        break;
    case L2EVENT_DATA_IND:
        SM_RecvData(Parms->aclLink, Parms->ptrs.data, Parms->dataLen);
        break;
    case L2EVENT_DISCONNECTED:
        if (session)
        {
            event.eType = BTEVENT_LINK_DISCONNECT;
            event.errCode = HC_STATUS_CONN_TERM_USER_REQ;
            event.p.remDev = Parms->aclLink;
            SM_LinkCallback(Parms->aclLink, &event);
        }
        break;
    }
}

#endif /* #ifdef __BT_4_0_BLE__ */

