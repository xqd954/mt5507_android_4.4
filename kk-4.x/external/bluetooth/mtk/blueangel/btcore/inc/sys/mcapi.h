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

#ifdef __BT_MCAP__

#ifndef __MCAPI_H
#define __MCAPI_H

#include <l2cap.h>
#include <mcap.h>
#include <stdio.h>

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/
	

#define MCAP_MINIMUM_MTU    48  
#define MCAP_MAX_CHANNEL_NUM 20

#define mcap_malloc(size)			get_ctrl_buffer(size)
#define mcap_free(p)				do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		


/* Prototype for MCL state functions */                    
typedef void (*McapMclStateFunc)(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms);

/* Prototype for MDL state functions */                    
typedef void (*McapMdlStateFunc)(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms);

/* MCAP Context */
typedef struct _McapContext
{
    ListEntry       	mcapMclList;				/*MCL list in the context*/
	U8					mclNum;						/*Number of MCL*/
    L2capPsm        	controlPsm;                 /* L2CAP psm for control channel of the MCL */
	L2capPsm			dataPsm;					/*L2CAP psm for data channel of the MDL*/
    McapCallback    	callback;                   /* Application callback function */
    BtPacket  			packet;                      /*Packet used to buffer data sent to L2CAP */
	BOOL				btPacketInUse;				/*Whether the packet is in use*/
	McapMclStateFunc	mclState[5];				/*State machine for MCL*/
	McapMdlStateFunc	mdlState[4];				/*State machine for MDL*/
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
	BtSecurityRecord	mclSecRecord;	/*MCL security record*/
	BtSecurityRecord	mdlSecRecord;	/*MDL security record*/
#endif
}   McapContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern McapContext *mcapcx;
#define MCAP(s)  (mcapcx->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern McapContext mcapcx;
#define MCAP(s)  (mcapcx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/* Function prototypes */

/* MCL states */
void McapMclStateIdle(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMclStateConnected(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMclStateActive(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMclStatePending(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMclStateDisconnecting(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms);

/* MDL states */
void McapMdlStateClosed(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMdlStateOpen(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMdlStateConnPending(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms);
void McapMdlStateDiscPending(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms);

/* MCAP callback functions */
void L2McapMclCallback(L2capChannelId ChannelId, L2capCallbackParms *Info);
void L2McapMdlCallback(L2capChannelId ChannelId, L2capCallbackParms *Info);
void McapAppCallBack(McapCallbackParms *parms, U16 status, McapEvent event);

McapMcl* McapFindMclByCid(L2capChannelId ChannelId);
McapMdl* McapFindMdlByCid(McapMcl *mcl, L2capChannelId ChannelId);
McapMcl* McapFindMclByBTAddr(BD_ADDR addr);
McapMcl* McapFindMclByTimer(EvmTimer *timer);
McapMdlId McapGenerateMdlId(McapMcl *mcl);
McapMdl* McapGetMdlByMdlId(McapMcl *mcl, McapMdlId mdlId);
void McapHandleMclDataInd(McapMcl *mcl, McapDataPacket* dataPkt, McapCallbackParms *parms);
BtStatus McapMclCheckParms(McapMcl *mcl, McapDataPacket* dataPkt);
BtStatus McapMclStartTimer(McapMcl *mcl, McapControlPacket *controlPkt);
void McapMclStopTimer(McapMcl *mcl, McapMdlId mdiId);
void McapMclStopAllTimer(McapMcl *mcl);
void McapMclStopDisTimer(McapMcl* mcl);
BOOL McapDeviceAddrEqual(BD_ADDR src, BD_ADDR des);
BOOL McapIsMclEmpty(McapMcl *mcl);

McapMcl* McapNewMcl(BtRemoteDevice *remDev, McapMclRole role, L2capChannelId ChannelId);
McapMdl* McapNewMdl(McapMcl *mcl, McapMdlId mdlId, U8 mdepId);
void McapFreeMcl(McapMcl *mcl);
void McapFreeMdl(McapMcl *mcl, McapMdl *mdl);

BOOL McapCheckMclState(McapMcl *mcl, McapMclState mclState);
void McapMclStateTrans(McapMcl *mcl, McapMclState mclState);

BOOL McapCheckMdlState(McapMdl *mdl, McapMdlState mdlState);
void McapMdlStateTrans(McapMdl *mcl, McapMdlState mdlState);

McapMdl* McapGetMdlByMdlId(McapMcl *mcl, McapMdlId mdlId);

BtStatus McapConfigMdl(McapMcl *mcl, McapMdl *mdl, U8 configuration);
BtStatus McapSendControl(McapMcl *mcl, McapControlPacket *controlPkt);
BtStatus McapSendData(McapMdl *mdl, BtPacket *btPacket, McapDataPacket *txPkt);

#endif /* __MCAPI_H */

#endif /* __BT_MCAP__ */

