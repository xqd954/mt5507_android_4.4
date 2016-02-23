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
 * smp.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy Security Manager Protocol
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_4_0_BLE__

#include "bt_common.h"
#include "btconfig_le.h"
#include "btalloc.h"
#include "btutils.h"
#include "me_adp.h"
#include "l2cap_i.h"
#include "smi.h"

typedef struct 
{
    BtRemoteDevice *remDev;
    U8 randLen;
    U8 randCount;
    U8 tmpRandValue[8];
} sm_rand_op;

typedef struct 
{
    BtRemoteDevice *remDev;
    U8 encryptCount;
    U8 k[16];
    U8 p2[16];
} sm_confirm_op;

typedef struct
{
    sm_rand_op smRandOps;
    sm_confirm_op smConfirmOps;
    BtRemoteDevice *smCalcSTKOps;
} smp_cntx;

smp_cntx smpCntx;

extern BtStatus L2Cap_FixCidSend(U8 cid, BtRemoteDevice *link, BtPacket *Packet);

void SMP_Init(void)
{
    bt_trace(TRACE_FUNC, BT_SMP_SMP_INIT);
    OS_MemSet((U8 *)&smpCntx, 0x00, sizeof(smp_cntx));
}

static BtStatus SM_SendL2capData(BtRemoteDevice *remDev, BtPacket *packet)
{
    Assert(remDev != NULL);
    Assert(packet != NULL);
    packet->headerLen = 0;
    packet->flags = BTP_FLAG_INUSE;
    packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    return L2Cap_FixCidSend(SM_L2CAP_CID, remDev, packet);
}


SmPairModel SM_GetPairModelByIo(SmIOCapability initiator, SmIOCapability responder)
{
    bt_trace(TRACE_FUNC, BT_SMP_SM_GETPAIRMODELBYIO, initiator, responder);
    if (initiator == SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT || responder == SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT)
    {
        return SM_MODEL_JUST_WORK;
    }
    else if ((initiator == SM_IO_CAPABILITY_DISPLAY_ONLY || initiator == SM_IO_CAPABILITY_DISPLAY_YESNO) &&
             (responder == SM_IO_CAPABILITY_DISPLAY_ONLY || responder == SM_IO_CAPABILITY_DISPLAY_YESNO))
    {
        return SM_MODEL_JUST_WORK;
    }
    else
    {
        return SM_MODEL_PASSKEY_ENTRY;
    }
}

SmPasskeyEntryRole SM_GetPasskeyRole(BtSMPairType type, SmIOCapability initiator, SmIOCapability responder)
{
    Assert(initiator != SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT && responder != SM_IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    Assert(!((initiator == SM_IO_CAPABILITY_DISPLAY_ONLY || initiator == SM_IO_CAPABILITY_DISPLAY_YESNO) &&
            (responder == SM_IO_CAPABILITY_DISPLAY_ONLY || responder == SM_IO_CAPABILITY_DISPLAY_YESNO)));
    
    bt_trace(TRACE_FUNC, BT_SMP_SM_GETPASSKEYROLE, type, initiator, responder);
    switch(initiator)
    {
    case SM_IO_CAPABILITY_DISPLAY_ONLY:
    case SM_IO_CAPABILITY_DISPLAY_YESNO:
        if (type == SM_PAIR_TYPE_BOND)
        {
            return SM_PASSKEY_ROLE_DISPLAY;
        }
        else
        {
            return SM_PASSKEY_ROLE_INPUT;
        }
        break;
    case SM_IO_CAPABILITY_KEYBOARD_ONLY:
        if (type == SM_PAIR_TYPE_BOND)
        {
            return SM_PASSKEY_ROLE_INPUT;
        }
        else
        {
            if (responder == SM_IO_CAPABILITY_KEYBOARD_ONLY)
            {
                return SM_PASSKEY_ROLE_INPUT;
            }
            else
            {
                return SM_PASSKEY_ROLE_DISPLAY;
            }
        }
        break;
    case SM_IO_CAPABILITY_KEYBOARD_DISPLAY:
        if (type == SM_PAIR_TYPE_BOND)
        {
            if (responder == SM_IO_CAPABILITY_DISPLAY_ONLY || responder == SM_IO_CAPABILITY_DISPLAY_YESNO)
            {
                return SM_PASSKEY_ROLE_INPUT;
            }
            else
            {
                return SM_PASSKEY_ROLE_DISPLAY;
            }
        }
        else
        {
            if (responder == SM_IO_CAPABILITY_DISPLAY_ONLY || responder == SM_IO_CAPABILITY_DISPLAY_YESNO)
            {
                return SM_PASSKEY_ROLE_DISPLAY;
            }
            else
            {
                return SM_PASSKEY_ROLE_INPUT;
            }
        }
        break;
    default:
        Assert(0);
    }
    return SM_PASSKEY_ROLE_INPUT;
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendPairReq
 * DESCRIPTION
 *  Send SMP pairing request
 * PARAMETERS
 *  remDev         [IN]
 *  preqData       [OUT]
 *  ioCapability   [IN]
 *  oobFlag        [IN]
 *  bondFlag       [IN]
 *  MITMProtect    [IN]
 *  maxKeySize     [IN]
 *  iKeyDist       [IN]
 *  rKeyDist       [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendPairReq(BtRemoteDevice *remDev,
                        U8 *preqData,
                        SmIOCapability ioCapability, 
                        SmOOBFlag oobFlag, 
                        SmBondingFlag bondFlag, 
                        BOOL MITMProtect, 
                        U8 maxKeySize,
                        SmKeyDistribute iKeyDist,
                        SmKeyDistribute rKeyDist)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    Assert(preqData != NULL);

    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDPAIRREQ, ioCapability, oobFlag, bondFlag, MITMProtect, maxKeySize, iKeyDist, rKeyDist);
    packet->data[i++] = SMC_PAIR_REQ;
    packet->data[i++] = ioCapability;
    packet->data[i++] = oobFlag;
    packet->data[i++] = (MITMProtect == TRUE) ? ((1 << 2) | bondFlag) : bondFlag;
    packet->data[i++] = maxKeySize;
    packet->data[i++] = iKeyDist;
    packet->data[i++] = rKeyDist;
    packet->dataLen = i;
    if (preqData)
    {
        OS_MemCopy(preqData, packet->data, 7);
        bt_parm_trace(7, preqData);
    }

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendPairRsp
 * DESCRIPTION
 *  Send SMP pairing response
 * PARAMETERS
 *  remDev         [IN]
 *  presData       [OUT]
 *  ioCapability   [IN]
 *  oobFlag        [IN]
 *  bondFlag       [IN]
 *  MITMProtect    [IN]
 *  maxKeySize     [IN]
 *  iKeyDist       [IN]
 *  rKeyDist       [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendPairRsp(BtRemoteDevice *remDev,
                        U8 *presData,
                        SmIOCapability ioCapability, 
                        SmOOBFlag oobFlag, 
                        SmBondingFlag bondFlag, 
                        BOOL MITMProtect, 
                        U8 maxKeySize,
                        SmKeyDistribute iKeyDist,
                        SmKeyDistribute rKeyDist)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();
    
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDPAIRRSP, ioCapability, oobFlag, bondFlag, MITMProtect, maxKeySize, iKeyDist, rKeyDist);
    Assert(remDev != NULL);
    Assert(presData != NULL);
    packet->data[i++] = SMC_PAIR_RSP;
    packet->data[i++] = ioCapability;
    packet->data[i++] = oobFlag;
    packet->data[i++] = (MITMProtect == TRUE) ? ((1 << 2) | bondFlag) : bondFlag;
    packet->data[i++] = maxKeySize;
    packet->data[i++] = iKeyDist;
    packet->data[i++] = rKeyDist;
    packet->dataLen = i;
    if (presData)
    {
        OS_MemCopy(presData, packet->data, 7);
        bt_parm_trace(7, presData);
    }

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendPairConfirm
 * DESCRIPTION
 *  Send SMP pairing confirm value
 * PARAMETERS
 *  remDev         [IN]
 *  confirm        [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendPairConfirm(BtRemoteDevice *remDev, U8 *confirm)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    Assert(confirm != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDPAIRCONFIRM);
    bt_parm_trace(SM_CONFIRM_VALUE_LENGTH, confirm);

    packet->data[i++] = SMC_PAIR_CONFIRM;
    OS_MemCopy(packet->data + 1, confirm, SM_CONFIRM_VALUE_LENGTH);
    packet->dataLen = 1 + SM_CONFIRM_VALUE_LENGTH;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendPairRandom
 * DESCRIPTION
 *  Send SMP pairing random value
 * PARAMETERS
 *  remDev         [IN]
 *  random         [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendPairRandom(BtRemoteDevice *remDev, U8 *random)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    Assert(random != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDPAIRRANDOM);
    bt_parm_trace(SM_RANDOM_VALUE_LENGTH, random);

    packet->data[i++] = SMC_PAIR_RANDOM;
    OS_MemCopy(packet->data + 1, random, SM_RANDOM_VALUE_LENGTH);
    packet->dataLen = 1 + SM_RANDOM_VALUE_LENGTH;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendPairFailed
 * DESCRIPTION
 *  Send SMP pairing failed response
 * PARAMETERS
 *  remDev         [IN]
 *  reason         [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendPairFailed(BtRemoteDevice *remDev, SmPairFailReason reason)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDPAIRFAILED, reason);

    packet->data[i++] = SMC_PAIR_FAILED;
    packet->data[i++] = reason;
    packet->dataLen = i;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendEncryptInfo
 * DESCRIPTION
 *  Send SMP pairing encryption long term key information
 * PARAMETERS
 *  remDev         [IN]
 *  ltk            [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendEncryptInfo(BtRemoteDevice *remDev, U8 *ltk)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    Assert(ltk != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDENCRYPTINFO);
    bt_parm_trace(SM_LTK_VALUE_LENGTH, ltk);
    Report(("UPF Debug LTK Send to Remote Side"));
    Report(("UPF Debug LTK LSB->MSB:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
        ltk[0],ltk[1],ltk[2],ltk[3],ltk[4],ltk[5],ltk[6],ltk[7]));
    Report(("UPF Debug LTK LSB->MSB:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
        ltk[8],ltk[9],ltk[10],ltk[11],ltk[12],ltk[13],ltk[14],ltk[15]));

    packet->data[i++] = SMC_ENCRYPT_INFO;
    OS_MemCopy(packet->data + 1, ltk, SM_LTK_VALUE_LENGTH);
    packet->dataLen = 1 + SM_LTK_VALUE_LENGTH;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

/*****************************************************************************
 * FUNCTION
 *  SM_SendMasterIdentity
 * DESCRIPTION
 *  Send SMP pairing identity information
 * PARAMETERS
 *  remDev         [IN]
 *  ediv           [IN]
 *  rand           [IN]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus SM_SendMasterIdentity(BtRemoteDevice *remDev, U16 ediv, U64 rand)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENMASTERIDENTITY, ediv, rand);

    packet->data[i++] = SMC_MASTER_IDEN;
    StoreLE16(packet->data + i, ediv);
    i += SM_EDIV_VALUE_LENGTH;

    Report(("UPF Debug ediv Send to Remote Side:%04x", ediv));

    StoreLE64(packet->data + i, rand);
    Report(("UPF Debug rand Send to Remote Side LSB->MSB:%02x,%02x,%02x,02x,%02x,%02x,%02x,%02x",
        packet->data[i],packet->data[i+1], packet->data[i+2],packet->data[i+3],packet->data[i+4],packet->data[i+5],packet->data[i+6],packet->data[i+7]));

    i += SM_RAND_VALUE_LENGTH;
    packet->dataLen = i;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

BtStatus SM_SendIdentityInfo(BtRemoteDevice *remDev, U8 *irk)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    Assert(irk != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDIDENTITYINFO);
    bt_parm_trace(SM_IRK_VALUE_LENGTH, irk);

    packet->data[i++] = SMC_IDEN_INFO;
    OS_MemCopy(packet->data + 1, irk, SM_IRK_VALUE_LENGTH);
    packet->dataLen = i + SM_IRK_VALUE_LENGTH;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

BtStatus SM_SendIdentityAddrInfo(BtRemoteDevice *remDev, LeAddrType addrType, BD_ADDR bdAddr)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDIDENTITYADDRINFO, 
             addrType, bdAddr.addr[0], bdAddr.addr[1], bdAddr.addr[2], bdAddr.addr[3], bdAddr.addr[4], bdAddr.addr[5]);

    packet->data[i++] = SMC_IDEN_ADDR_INFO;
    packet->data[i++] = addrType;
    OS_MemCopy(packet->data + i, bdAddr.addr, BD_ADDR_SIZE);
    packet->dataLen = i + BD_ADDR_SIZE;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

BtStatus SM_SendSigningInfo(BtRemoteDevice *remDev, U8 *csrk)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    Assert(csrk != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDSIGNINGINFO);
    bt_parm_trace(SM_CSRK_VALUE_LENGTH, csrk);

    packet->data[i++] = SMC_SIGN_INFO;
    OS_MemCopy(packet->data + 1, csrk, SM_CSRK_VALUE_LENGTH);
    packet->dataLen = i + SM_CSRK_VALUE_LENGTH;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

BtStatus SM_SendSecurityReq(BtRemoteDevice *remDev, SmBondingFlag bondFlag, BOOL MITMProtect)
{
    int i = 0;
    BtPacket *packet = SM_AllocCmdPkt();

    Assert(remDev != NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_SENDSECURITYREQ, bondFlag, MITMProtect);
    packet->data[i++] = SMC_SECURITY_REQ;
    packet->data[i++] = (MITMProtect == TRUE) ? ((1 << 2) | bondFlag) : bondFlag;
    packet->dataLen = i;

    SM_ResetTimer(remDev);
    return SM_SendL2capData(remDev, packet);
}

void SM_util_mask_MSBs(U8 *array, int len, int masklen, U8 value)
{
    int i;
    
    Assert(masklen <= len);
    for (i = 0; i < masklen; i++)
    {
        array[len - i - 1] = value;
    }
}

void SM_util_128bit_XOR(U8 *out, U8 *in1, U8 *in2)
{
    int i;
    for ( i = 0; i < 16; i++)
    {
        out[i] = in1[i] ^ in2[i];
    }
}

void SM_util_32bit_2_128bit(U8 *out, U32 value)
{
    OS_MemSet(out + 4, 0x0, 12);
    out[3] = (0xFF000000 & value) >> 24;
    out[2] = (0xFF0000 & value) >> 16;
    out[1] = (0xFF00 & value) >> 8;
    out[0] = 0xFF & value;
}

void SM_util_leftshift_1bit(U8 *in, U8 *out)
{
    int i;
    unsigned char onebit = 0;

    for (i = 15; i >= 0; i--)
    {
      out[i] = in[i] << 1;
      out[i] |= onebit;
      onebit = (in[i] & 0x80) ? 1 : 0;
    }
}

void SM_util_cmac_padding (U8 *lastb, U8 *pad, int length)
{
    S32 j;
  
    /* original last block */
    for ( j = 0; j < 16; j++ ) 
    {
        if ( j < length ) 
        {
            pad[j] = lastb[j];
        } 
        else if ( j == length ) 
        {
            pad[j] = 0x80;
        } 
        else 
        {
            pad[j] = 0x00;
        }
    }
}

static void SM_CalcPairConfirm_FSM(const BtEvent *event)
{
    BtEvent newEvent;
    bt_trace(TRACE_FUNC, BT_SMP_SM_CALCPAIRCONFIRMFSM, smpCntx.smConfirmOps.encryptCount, event->errCode);
    switch (smpCntx.smConfirmOps.encryptCount)
    {
    case 0:
        if (event->errCode == HC_STATUS_SUCCESS)
        {
            smpCntx.smConfirmOps.encryptCount++;
            SM_util_128bit_XOR(smpCntx.smConfirmOps.p2, smpCntx.smConfirmOps.p2, (U8 *)event->p.smEncryptResponse.EncryptData);
            LeHciAES128Encrypt(SM_CalcPairConfirm_FSM, smpCntx.smConfirmOps.k, smpCntx.smConfirmOps.p2);
            break;
        }
        /* fall-through */
    case 1:
        newEvent.errCode = event->errCode;
        newEvent.eType = BTEVENT_LE_SM_RESULT;
        newEvent.p.smConfirmResponse.remDev = smpCntx.smConfirmOps.remDev;
        OS_MemCopy(newEvent.p.smConfirmResponse.ConfirmValue, event->p.smEncryptResponse.EncryptData, SM_CONFIRM_VALUE_LENGTH);
        SM_LinkCallback(smpCntx.smConfirmOps.remDev, &newEvent);
        OS_MemSet((U8 *)&smpCntx.smConfirmOps, 0x00, sizeof(sm_confirm_op));
        break;
    }
}

BtStatus SM_CalcPairConfirm(BtRemoteDevice *remDev, U8 *tk, U8 *rand, U8 *preq, U8 *pres, LeAddrType iat, LeAddrType rat, BD_ADDR ia, BD_ADDR ra)
{
    U8 p1[16] = {0};
    U8 iat1 = iat & 0x01;
    U8 rat1 = rat & 0x01;

    Assert (smpCntx.smConfirmOps.remDev == NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_CALCPAIRCONFIRM, iat, rat);
    bt_parm_trace(16, tk);
    bt_parm_trace(16, rand);
    bt_parm_trace(7, preq);
    bt_parm_trace(7, pres);
    BTBMDumpBdAddr(ia.addr);
    BTBMDumpBdAddr(ra.addr);

    smpCntx.smConfirmOps.remDev = remDev;
    smpCntx.smConfirmOps.encryptCount = 0;


    OS_MemCopy(smpCntx.smConfirmOps.k, tk, 16);
    /* p2 = (MSB)padding || ia || ra */
    OS_MemCopy((U8 *)&smpCntx.smConfirmOps.p2, ra.addr, 6);
    OS_MemCopy(&smpCntx.smConfirmOps.p2[6], ia.addr, 6);

    /* p1 = (MSB)pres || preq || rat1 || iat1 */
    p1[0] = iat1;
    p1[1] = rat1;
    OS_MemCopy(&p1[2], preq, 7);
    OS_MemCopy(&p1[9], pres, 7);

    /* r OXR p1 */
    SM_util_128bit_XOR(p1, p1, rand);
    return LeHciAES128Encrypt(SM_CalcPairConfirm_FSM, tk, p1);
}

static void SM_CalcSTK_FSM(const BtEvent *event)
{
    BtEvent newEvent;
    newEvent.errCode = event->errCode;
    newEvent.eType = BTEVENT_LE_SM_RESULT;
    newEvent.p.smEncryptResponse.remDev = smpCntx.smCalcSTKOps;
    OS_MemCopy(newEvent.p.smEncryptResponse.EncryptData, event->p.smEncryptResponse.EncryptData, 16);
    SM_LinkCallback(smpCntx.smCalcSTKOps, &newEvent);
    smpCntx.smCalcSTKOps = NULL;
}

BtStatus SM_CalcSTK(BtRemoteDevice *remDev, U8 *tk, U8 *srand, U8 *mrand)
{
    U8 r[16] = {0};

    Assert (smpCntx.smCalcSTKOps == NULL);
    bt_trace(TRACE_FUNC, BT_SMP_SM_CALCSTK);
    bt_parm_trace(16, tk);
    bt_parm_trace(8, srand);  /* r1 */
    bt_parm_trace(8, mrand);  /* r2 */

    smpCntx.smCalcSTKOps = remDev;

    /* r = (MSB)r1 || r2 */
    OS_MemCopy(r, mrand, 8);
    OS_MemCopy(&r[8], srand, 8);
    
    return LeHciAES128Encrypt(SM_CalcSTK_FSM, tk, r);
}

static void SM_Rand_FSM(const BtEvent *event)
{
    BtEvent newEvent;
    bt_trace(TRACE_FUNC, BT_SMP_SM_RANDFSM, smpCntx.smRandOps.randLen, smpCntx.smRandOps.randCount);
    bt_parm_trace(8, (U8 *)event->p.smRand64Response.randValue64);
    if(smpCntx.smRandOps.randLen == 16)
    {
        switch (smpCntx.smRandOps.randCount)
        {
        case 0:
            if (event->errCode == HC_STATUS_SUCCESS)
            {
                smpCntx.smRandOps.randCount++;
                OS_MemCopy((U8 *)smpCntx.smRandOps.tmpRandValue, (U8 *)event->p.smRand64Response.randValue64, SM_RAND_VALUE_LENGTH);
                LeHciRand(SM_Rand_FSM);
                return;
            }
        case 1:
            if (event->errCode == HC_STATUS_SUCCESS)
            {
                U8 tmp[SM_RAND_VALUE_LENGTH];
                OS_MemCopy(tmp, event->p.smRand64Response.randValue64, SM_RAND_VALUE_LENGTH);
                OS_MemCopy(newEvent.p.smRand128Response.randValue128, smpCntx.smRandOps.tmpRandValue, SM_RAND_VALUE_LENGTH);
                OS_MemCopy(newEvent.p.smRand128Response.randValue128 + SM_RAND_VALUE_LENGTH, tmp, SM_RAND_VALUE_LENGTH);
                break;
            }
        }
        newEvent.p.smRand128Response.remDev = smpCntx.smRandOps.remDev;
    }
    else
    {
        newEvent.p.smRand64Response.remDev = smpCntx.smRandOps.remDev;
    }
    newEvent.eType = BTEVENT_LE_SM_RESULT;
    newEvent.errCode = event->errCode;
    SM_LinkCallback(smpCntx.smRandOps.remDev, &newEvent);
    OS_MemSet((U8 *)&smpCntx.smRandOps, 0x00, sizeof(sm_rand_op));
 }

BtStatus SM_Rand(BtRemoteDevice *remDev, U8 len)
{
    bt_trace(TRACE_FUNC, BT_SMP_SM_RAND, len);
    Assert(smpCntx.smRandOps.remDev == NULL);
    smpCntx.smRandOps.remDev = remDev;
    smpCntx.smRandOps.randLen = len;
    smpCntx.smRandOps.randCount = 0;
    return LeHciRand(SM_Rand_FSM);
}

#endif /* #ifdef __BT_4_0_BLE__ */

