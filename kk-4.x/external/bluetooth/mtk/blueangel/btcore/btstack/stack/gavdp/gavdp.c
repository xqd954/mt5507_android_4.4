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

#include "gavdp.h"



typedef struct _BtGavdpCmgrNode
{
  ListEntry node;  
  CmgrHandler cmgr_hdlr;
  U8 isUsed;
} BtGavdpCmgrNode;

typedef struct _BtGavdpChnlNode 
{
  ListEntry node;      
  AvdtpChannel channel;
  U8 isUsed;
} BtGavdpChnlNode;

struct _BtGavdpSepSwitchPair
{
  StreamEndPoint* fake_sep;  	
  StreamEndPoint* guess_sep;
};

struct _BtGavdpContext
{
  U8 sep_num;
  U8 sig_conn_state;

  U32 assigned_seid;
  StreamEndPoint fake_sep;  	
  CmgrHandler cmgr_hdlr;

  GavdpSepSwitchPair switchPair;
//  AvdtpStreamInfo remote_sep_info[MAX_NUM_REMOTE_SEIDS];

  
  ListEntry sepList;  
  ListEntry chnlList;
  ListEntry cmgrList;  
};


#if XA_CONTEXT_PTR == XA_ENABLED
#define GAVDP(s) (gavdpContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define GAVDP(s) (gavdpContext.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

#define ASSERT Assert
//------------------------------------------------------------------------------
extern BOOL BtSysAdpVerifyInPanicState(void);
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);
extern BOOL AVDTP_SendStreamData(AvdtpStream *Stream, U16 length, U8 *buffer);

static BOOL GavdpAlloc(void);

static BtGavdpCmgrNode* GavdpFindCmgrNode(CmgrHandler *cmgr_hdlr);
static BtGavdpChnlNode* GavdpFindChnlNode(AvdtpChannel* channl);
static BtGavdpCmgrNode* GavdpFindFreeCmgrNode(void);
static BtGavdpChnlNode* GavdpFindFreeChnlNode(void);

static StreamEndPoint*  GavdpFindSepByChannel(AvdtpChannel *channel);
static StreamEndPoint*  GavdpFindSepByCmgrHdl(CmgrHandler *cmgr_hdlr);
static StreamEndPoint* GavdpFindSepById(U8 id);

static BOOL GavdpIsSepAvailable(void);
static BOOL GavdpIsDevConnected(BD_ADDR *device_addr);
static void GavdpCopyCodec(AvdtpCodec* des, AvdtpCodec* src);
static BOOL GavdpDecideConfig(AvdtpCodec* config_codec, AvdtpCodec* local_codec, AvdtpCodec* remote_codec);
static void GavdpDiscSigChnl(EvmTimer *timer);

static void GavdpConnectAvdtpChannel(StreamEndPoint *sep);	
static void GavdpDiscoverStreamsReq(StreamEndPoint *sep);
static void GavdpGetCapabilityReq(StreamEndPoint *sep);
static void GavdpSetConfigurationReq(StreamEndPoint *sep);
static void GavdpOpenStreamReq(StreamEndPoint *sep);


static void GavdpGavdpEventCallback(StreamEndPoint *sep, GavdpEvent event, U8 error);	
static void GavdpCmgrEventCallback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status);
static void GavdpAvdtpEventCallback(AvdtpChannel *channel, AvdtpCallbackParms *info);


//------------------------------------------------------------------------------
//part1

#if XA_CONTEXT_PTR == XA_ENABLED
BtGavdpContext temp;
BtGavdpContext *gavdpContext = &temp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */ 
BtGavdpContext gavdpContext;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */ 
U8 gavdpContextState = GAVDP_STATE_INACTIVATED;


static BOOL GavdpAlloc(void)
{
    U8 *ptr;

    /* Fill memory with 0. */
#if XA_CONTEXT_PTR == XA_ENABLED
    ptr = (U8*)gavdpContext;
#else 
    ptr = (U8*)&gavdpContext;
#endif /* XA_CONTEXT_PTR */

    OS_MemSet(ptr, 0, (U32)sizeof(BtGavdpContext));

    return TRUE;
}


static BtGavdpCmgrNode* GavdpFindCmgrNode(CmgrHandler *cmgr_hdlr)
{
    BtGavdpCmgrNode *cmgrNode;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_FIND_CMGR_NODE, cmgr_hdlr);	
    cmgrNode = (BtGavdpCmgrNode*)GetHeadList(&GAVDP(cmgrList));
    while (cmgrNode != (BtGavdpCmgrNode*)&GAVDP(cmgrList))
    {
        if (&cmgrNode->cmgr_hdlr == cmgr_hdlr)
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_FIND_CMGR_FOUND);	          
            break;
        }
        cmgrNode = (BtGavdpCmgrNode*)GetNextNode(&cmgrNode->node); 
    }

    if ( cmgrNode ==  (BtGavdpCmgrNode*)&GAVDP(cmgrList))
    {
        cmgrNode = 0;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_FIND_CMGR_NOT_FOUND);			
    }
	
    return cmgrNode;
}


static BtGavdpChnlNode* GavdpFindChnlNode(AvdtpChannel* channel)
{
    BtGavdpChnlNode *chnlNode;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_FIND_CHNL_NODE, channel);	
    chnlNode = (BtGavdpChnlNode*)GetHeadList(&GAVDP(chnlList));
    while (chnlNode != (BtGavdpChnlNode*)&GAVDP(chnlList))
    {
        if (&chnlNode->channel == channel)
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_FIND_CHNL_FOUND);	        
            break;
        }
        chnlNode = (BtGavdpChnlNode*)GetNextNode(&chnlNode->node);         	
    }

    if ( chnlNode ==  (BtGavdpChnlNode*)&GAVDP(chnlList))
    {
        chnlNode = 0;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_FIND_CHNL_NOT_FOUND);		
    }
    return chnlNode;
}

static BtGavdpCmgrNode* GavdpFindFreeCmgrNode(void)
{
    BtGavdpCmgrNode *cmgrNode;

    cmgrNode = (BtGavdpCmgrNode*)GetHeadList(&GAVDP(cmgrList));
    while (cmgrNode != (BtGavdpCmgrNode*)&GAVDP(cmgrList))
    {
        if (cmgrNode->isUsed == 0)
        {
            break;
        }
        cmgrNode = (BtGavdpCmgrNode*)GetNextNode(&cmgrNode->node); 
    }

    if ( cmgrNode ==  (BtGavdpCmgrNode*)&GAVDP(cmgrList))
    {
        cmgrNode = 0;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ALL_CMGR_USED);		
    }

    return cmgrNode;
}

static BtGavdpChnlNode* GavdpFindFreeChnlNode(void)
{
    BtGavdpChnlNode *chnlNode;

    chnlNode = (BtGavdpChnlNode*)GetHeadList(&GAVDP(chnlList));
    while (chnlNode != (BtGavdpChnlNode*)&GAVDP(chnlList))
    {
        if (chnlNode->isUsed == 0)
        {
            break;
        }
        chnlNode = (BtGavdpChnlNode*)GetNextNode(&chnlNode->node); 
    }

    if ( chnlNode ==  (BtGavdpChnlNode*)&GAVDP(chnlList))
    {
        chnlNode = 0;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ALL_CHNL_USED);			
    }

    return chnlNode;
}



static StreamEndPoint* GavdpFindSepByChannel(AvdtpChannel *channel)
{
    StreamEndPoint *sep;

#ifdef __BT_GAVDP_SEP_CHNL_DGB__	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHNL_FIND_SEP, channel);
#endif

    sep = (StreamEndPoint*)GetHeadList(&GAVDP(sepList));
    while (sep != (StreamEndPoint*)&GAVDP(sepList))
    {
        if ((sep->signal_channel != 0)&&(sep->signal_channel == channel))
        {        
#ifdef __BT_GAVDP_SEP_CHNL_DGB__        
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHNL_FIND_SEP_FOUND);
#endif
            break;
        }
        sep = (StreamEndPoint*)GetNextNode(&sep->node); 
    }

    if ( sep == (StreamEndPoint*)&GAVDP(sepList) )
    {
        sep = 0;	
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHNL_FIND_SEP_NOT_FOUND);
    }
    
    return sep;
}


static StreamEndPoint*  GavdpFindSepByCmgrHdl(CmgrHandler *cmgr_hdlr)
{
    StreamEndPoint *sep;
	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_FIND_SEP, cmgr_hdlr);	
    sep = (StreamEndPoint*)GetHeadList(&GAVDP(sepList));
    while (sep != (StreamEndPoint*)&GAVDP(sepList))
    {
        if ((sep->signal_channel != 0)&&(sep->cmgr_hdlr == cmgr_hdlr))
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_FIND_SEP_FOUND);        
            break;
        }
        sep = (StreamEndPoint*)GetNextNode(&sep->node); 
    }

    if ( sep == (StreamEndPoint*)&GAVDP(sepList) )
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_FIND_SEP_NOT_FOUND);	    
        sep = 0;
    }

    return sep;
}


static StreamEndPoint* GavdpFindSepById(U8 id)
{
    StreamEndPoint *sep;

    
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ID_FIND_SEP, id);     
    sep = (StreamEndPoint*)GetHeadList(&GAVDP(sepList));
    while (sep != (StreamEndPoint*)&GAVDP(sepList))
    {
        if ((sep->info.id == id))
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ID_FIND_SEP_FOUND);         
            break;
        }
        sep = (StreamEndPoint*)GetNextNode(&sep->node); 
    }

    if ( sep == (StreamEndPoint*)&GAVDP(sepList) )
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ID_FIND_SEP_NOT_FOUND);    
        sep = 0;
    }
    
    return sep;
}


static BOOL GavdpIsSepAvailable(void)
{
    StreamEndPoint *sep;

    sep = (StreamEndPoint*)GetHeadList(&GAVDP(sepList));
    while (sep != (StreamEndPoint*) &GAVDP(sepList))
    {
        if ( 1 != sep->info.inUse )
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEP_AVAILABLE);        
            break;
        }
	 sep = (StreamEndPoint*)GetNextNode(&sep->node);
    }

    if ( sep == (StreamEndPoint*)&GAVDP(sepList) )
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEP_NOT_AVAILABLE);     
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}



static BOOL GavdpIsDevConnected(BD_ADDR *device_addr)
{
    BtGavdpChnlNode* chnlNode; 
    AvdtpChannel *chnl;
    BD_ADDR *remote_addr;

    chnlNode = (BtGavdpChnlNode*)GetHeadList(&GAVDP(chnlList));
    while ( chnlNode != (BtGavdpChnlNode*)&GAVDP(chnlList) )
    {
        if (1 == chnlNode->isUsed)
        {
            if( NULL != chnlNode->channel.conn.remDev )
            {
                remote_addr = &chnlNode->channel.conn.remDev->bdAddr;	
                if( OS_MemCmp(remote_addr->addr, 6, device_addr->addr, 6) ) 	
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEV_CONNECTED);                
                    break;
                }
            }			
        } 			
	 chnlNode = (BtGavdpChnlNode*)GetNextNode(&chnlNode->node);
    }

	
    if ( chnlNode == (BtGavdpChnlNode*)&GAVDP(chnlList) )
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEV_NOT_CONNECTED);    
        return FALSE;
    }
    else
    {
        return TRUE;
    }	
}


static void GavdpCopyCodec(AvdtpCodec* des, AvdtpCodec* src)
{
    des->codecType = src->codecType;
    des->elemLen = src->elemLen;
    memcpy(des->elements, src->elements, src->elemLen);
}


//TODO: handle non-SBC codecs 
static BOOL GavdpDecideConfig(AvdtpCodec* config_codec, AvdtpCodec* local_codec, AvdtpCodec* remote_codec)
{
    config_codec->codecType = local_codec->codecType;
    config_codec->elemLen = local_codec->elemLen;	

    memset(config_codec->elements, 0, config_codec->elemLen);

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LOCAL_SBC_BYTE0, local_codec->elements[0]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LOCAL_SBC_BYTE1, local_codec->elements[1]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LOCAL_SBC_BYTE2, local_codec->elements[2]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LOCAL_SBC_BYTE3, local_codec->elements[3]);					

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REMOTE_SBC_BYTE0, remote_codec->elements[0]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REMOTE_SBC_BYTE1, remote_codec->elements[1]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REMOTE_SBC_BYTE2, remote_codec->elements[2]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REMOTE_SBC_BYTE3, remote_codec->elements[3]);
    
    if (GAVDP_CODEC_TYPE_SBC_FREQ_48K & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_FREQ_48K;
    }
    else if(GAVDP_CODEC_TYPE_SBC_FREQ_44K & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_FREQ_44K;
    }
    else if(GAVDP_CODEC_TYPE_SBC_FREQ_32K & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_FREQ_32K;
    }
    else if(GAVDP_CODEC_TYPE_SBC_FREQ_16K & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_FREQ_16K;
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_NO_MATCHED_SMAPLING_FREQ);       
        return FALSE;
    }		

    if(GAVDP_CODEC_TYPE_SBC_JOINT_STEREO & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_JOINT_STEREO;
    }
    else if(GAVDP_CODEC_TYPE_SBC_STEREO & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_STEREO;
    }
    else if(GAVDP_CODEC_TYPE_SBC_DUAL & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_DUAL;
    }	
    else if (GAVDP_CODEC_TYPE_SBC_MONO & local_codec->elements[0] & remote_codec->elements[0] )
    {
        config_codec->elements[0] |=  GAVDP_CODEC_TYPE_SBC_MONO;
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_NO_MATCHED_CH_MODE);        
        return FALSE;
    }		
	

    if (GAVDP_CODEC_TYPE_SBC_BLOCK_16 & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_BLOCK_16;
    }
    else if(GAVDP_CODEC_TYPE_SBC_BLOCK_12 & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_BLOCK_12;
    }
    else if(GAVDP_CODEC_TYPE_SBC_BLOCK_8 & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_BLOCK_8;
    }	
    else if(GAVDP_CODEC_TYPE_SBC_BLOCK_4 & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_BLOCK_4;
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_NO_MATCHED_BLK_LEN);        
        return FALSE;
    }	


    if (GAVDP_CODEC_TYPE_SBC_SUB_8  & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_SUB_8 ;
    }
    else if(GAVDP_CODEC_TYPE_SBC_SUB_4  & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_SUB_4 ;
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_NO_MATCHED_SBAND_NUM);        
        return FALSE;
    }	


    if(GAVDP_CODEC_TYPE_SBC_LOUDNESS & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |= GAVDP_CODEC_TYPE_SBC_LOUDNESS;
    }
    else if (GAVDP_CODEC_TYPE_SBC_SNR  & local_codec->elements[1] & remote_codec->elements[1] )
    {
        config_codec->elements[1] |=  GAVDP_CODEC_TYPE_SBC_SNR;
    }	
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_NO_MATCHED_ALLOC);        
        return FALSE;
    }

    if ( local_codec->elements[2] > remote_codec->elements[2] )	
    {
        config_codec->elements[2] = local_codec->elements[2];     
    }
    else
    {
        config_codec->elements[2] = remote_codec->elements[2];         
    }

    if ( local_codec->elements[3] > remote_codec->elements[3] )	
    {
        config_codec->elements[3] = remote_codec->elements[3];     
    }
    else
    {
        config_codec->elements[3] = local_codec->elements[3];         
    }

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CFG_SBC_BYTE0, config_codec->elements[0]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CFG_SBC_BYTE1, config_codec->elements[1]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CFG_SBC_BYTE2, config_codec->elements[2]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CFG_SBC_BYTE3, config_codec->elements[3]);	
    
    return TRUE;
}


static void GavdpDiscSigChnl(EvmTimer *timer)
{
    OS_Report("[GAVDP][GavdpDiscSigChnl]AVDTP_Disconnect");
	
    timer->func = 0;    
    AVDTP_Disconnect(((StreamEndPoint*)timer->context)->signal_channel);
    return;	
}


static void GavdpSwitchRole(EvmTimer *timer)
{
    OS_Report("[GAVDP][GavdpSwitchRole]+++");
    GavdpSepSwitchPair* sepPair;    
    StreamEndPoint* sep;
    StreamEndPoint* fakeSep;

    sepPair = (GavdpSepSwitchPair*)timer->context;
    fakeSep = sepPair->fake_sep;
    sep = sepPair->guess_sep;
    
    sep->cmgr_hdlr = fakeSep->cmgr_hdlr;
    sep->signal_channel = fakeSep->signal_channel; 
    //sep->command_stage = GAVDP(fake_sep).command_stage;
    //sep->stream.chnl = sep->signal_channel;                       
    memcpy(&sep->remote_bd_addr, &fakeSep->remote_bd_addr, sizeof(BD_ADDR));    
    //GavdpCopyCodec(&sep->current_cfg_codec, &sep->remote_cfg_codec); 
//content protection						
//#ifdef __CONTENT_PROTECTION_SUPPORT__ 					
//                        if (sep->stream.endPointType == AVDTP_STRM_ENDPOINT_SNK && (AvdtpContentProt*)&info->p.configReq->cp.cpType != 0 )
//                       {
//                           GavdpCopyContentProtection(&sep->current_cfg_cp, &sep->remote_cfg_cp);
//                       }
//#endif
    sep->stream_state = GAVDP_STATE_IDLE;
    sep->command_stage = GAVDP_WAIT_NONE;
    GAVDP(sig_conn_state) = GAVDP_STATE_SIG_STRM_OUT_CONNECTING;
    
    GavdpDiscoverStreamsReq(sep);        

    OS_Report("[GAVDP][GavdpSwitchRole]---");
}





//------------------------------------------------------------------------------
//part 2
static void GavdpConnectAvdtpChannel(StreamEndPoint *sep)
{
    BtStatus status;

    bt_trace(BT_TRACE_G7_GAVDP,  BT_GAVDP_LINK_AVDTP_CHNL, GAVDP(sig_conn_state), sep, sep->info.id, sep->command_stage, sep->stream_state);    
	
    //A2dpConnectAvdtpChannel(&device_addr, cmgr_hdlr->bdc->link);
    status = AVDTP_Connect(sep->signal_channel, sep->cmgr_hdlr->bdc->link); //select one channel from the list and 					

				
    if (status == BT_STATUS_BUSY)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LINK_AVDTP_CHNL_BUSY);     
        //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_NONE; 		   
        //BT_A2DP_ENTER_STATE(BT_A2DP_STATE_CONNECTED);
        sep->stream_state = GAVDP_STATE_IDLE;
        sep->command_stage = GAVDP_WAIT_NONE; 
					
        GavdpDiscoverStreamsReq(sep);
    }
    else if (status == BT_STATUS_PENDING)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LINK_AVDTP_CHNL_PENDING);    
        //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_CONNECT;
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_LINK_AVDTP_CHNL_FAILED);    
        GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
        CMGR_RemoveDataLink(&GAVDP(cmgr_hdlr));
					   
        BTBMProfileReleaseConnIndex(sep->conn_id);
        sep->conn_id = 0xFF; 		  
        CMGR_RemoveCmgrWithDevice(&GAVDP(cmgr_hdlr));

        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_FATAL);			   
    }
    
    return;
}


//void A2DP_StreamDiscoverReq(U16 connect_id)
static void GavdpDiscoverStreamsReq(StreamEndPoint *sep)
{
    BtStatus status;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DISCOVER_STRM_REQ, GAVDP(sig_conn_state), sep, sep->info.id, sep->command_stage, sep->stream_state);	
	
    if( BtSysAdpVerifyInPanicState() )
    { 
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_PANIC);
        return;
    }

    if ( sep->stream_state != GAVDP_STATE_IDLE && sep->command_stage != GAVDP_WAIT_NONE ) 
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);        
        return;
    }

    status = AVDTP_DiscoverStreams(sep->signal_channel);
    if (status == BT_STATUS_PENDING)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DISCOVER_STRM_REQ_PENDING);
        sep->command_stage = GAVDP_EST_WAIT_CNF_DISCOVER;
        sep->remote_sep_count = 0;
    }
    else
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);		
    }
    
}


//void A2DP_StreamGetCapReq(U16 connect_id, U8 acp_seid)
static void GavdpGetCapabilityReq(StreamEndPoint *sep)
{
    BtStatus status;
    U8 i;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_GET_CAP_REQ, GAVDP(sig_conn_state), sep, sep->info.id, sep->command_stage, sep->stream_state);	
	
    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_PANIC);		
        return;
    }

    if ( sep->stream_state != GAVDP_STATE_IDLE && sep->command_stage != GAVDP_WAIT_NONE ) 
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);		
        return;
    }

    //TODO: handle multiple type seid. It only handles mediaType == AVDTP_MEDIA_TYPE_AUDIO && streamType == AVDTP_STRM_ENDPOINT_SNK now.
    // if ( sep->info->mediaType == AVDTP_MEDIA_TYPE_AUDIO && sep->info->streamType == AVDTP_STRM_ENDPOINT_SRC) select Audio SINK to connect    
   
   for ( i = sep->current_cfg_sep_id; i < sep->remote_sep_count; i++ )
   //for ( i = 0; i < sep->remote_sep_count; i++ )    
   {
       if ( AVDTP_MEDIA_TYPE_AUDIO == sep->remote_sep_info[i].mediaType &&
               AVDTP_STRM_ENDPOINT_SNK == sep->remote_sep_info[i].streamType )
       {
           sep->current_cfg_sep_id =  i;
           break;
       }
	   	
   }
	
    status = AVDTP_GetCapabilities(sep->signal_channel, sep->remote_sep_info[i].id);	
    if (status == BT_STATUS_PENDING)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_GET_CAP_REQ_PENDING);
        sep->command_stage = GAVDP_EST_WAIT_CNF_GET_CAP;
        sep->remote_cap_count = 0;
    }
    else
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);		
    }
    
}


//void A2DP_StreamSetConfigReq(U16 connect_id, U8 acp_seid, U8 int_seid, bt_a2dp_audio_cap_struct *audio_cap)      
static void GavdpSetConfigurationReq(StreamEndPoint *sep)
{
    BtStatus status;
    U8 elements[10];

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SET_CFG_REQ, GAVDP(sig_conn_state), sep, sep->info.id, sep->command_stage, sep->stream_state);
	
    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_PANIC);		
        return;
    }

    if ( sep->stream_state != GAVDP_STATE_IDLE && sep->command_stage != GAVDP_WAIT_NONE ) 
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);		
        return;
    }


    //sep->local_cfg_codec.elements[0] = 0x21;
    //sep->local_cfg_codec.elements[1] = 0x15;		
    //TODO: how to choose and handle multiple capabilities
    if ( FALSE == GavdpDecideConfig(&sep->local_cfg_codec, &sep->codec, &sep->remote_cap_info[0]) )
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SET_CFG_REQ_NO_MATCHED_CODEC);
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_INVALID_CAPABILITIES);			
        return;
    }
     
     //TODO: handle multiple seid, try other SEPs    
    sep->stream.remStrmId = sep->remote_sep_info[sep->current_cfg_sep_id].id;
    status = AVDTP_SetConfig(sep->signal_channel, &sep->stream, sep->remote_sep_info[sep->current_cfg_sep_id].id, &sep->local_cfg_codec);
    if (status == BT_STATUS_PENDING)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SET_CFG_REQ_PENDING);
        sep->command_stage = GAVDP_EST_WAIT_CNF_CONFIG;
    }
    else
    {

        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);			
    }
    
}


static void GavdpOpenStreamReq(StreamEndPoint *sep)
{
    BtStatus status;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_OPEN_STRM_REQ, GAVDP(sig_conn_state), sep, sep->info.id, sep->command_stage, sep->stream_state);	

    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_PANIC);			
        return;
    }
    if ( sep->stream_state != GAVDP_STATE_IDLE && sep->command_stage != GAVDP_WAIT_NONE ) 
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);		
        return;
    }
	
    //ASSERT(stream_handle == bt_a2dp_ctx.stream.locStrmId);
    //status = AVDTP_OpenStream(&bt_a2dp_ctx.signal_channel, &bt_a2dp_ctx.stream);
    status = AVDTP_OpenStream(sep->signal_channel, &sep->stream);   
    if (status == BT_STATUS_PENDING)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_OPEN_STRM_REQ_PENDING);
        //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_OPEN; 
        sep->command_stage = GAVDP_EST_WAIT_CNF_OPEN;
    }
    else
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);		
    }
    
}










//------------------------------------------------------------------------------
//part 3
static void GavdpGavdpEventCallback(StreamEndPoint *sep, GavdpEvent event, U8 error)
{
    GavdpCallbackParms para;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_GAVDP_CBK_INFO1, sep, event, error);	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_GAVDP_CBK_SEP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state ); 		
	
    //TODO: error handle (disconnect), reset sep->command_stage by error
    switch (event)
    {
    case  GAVDP_EVENT_STREAM_ESTABLISH_CNF:
        if ( GAVDP_ERR_FATAL != error && AVDTP_ERR_NO_ERROR != error && 
	       GAVDP_ERR_CONN_IN != error && GAVDP_ERR_NO_RESOURCE != error &&
	       GAVDP_ERR_CONNECTING != error)
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_GAVDP_CBK_STRM_EST_FAIL);
            GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
            sep->command_stage = GAVDP_WAIT_NONE;
            sep->stream_state = GAVDP_STATE_IDLE;			
            AVDTP_Disconnect(sep->signal_channel);
        }
	break;
    case  GAVDP_EVENT_STREAM_ESTABLISH_IND:
        break;
    case  GAVDP_EVENT_STREAM_START_CNF:
        break;
    case  GAVDP_EVENT_STREAM_START_IND:		
        break;
    case GAVDP_EVENT_STREAM_SUSPEND_CNF:
        break;        
    case GAVDP_EVENT_STREAM_SUSPEND_IND:
        break;
    case GAVDP_EVENT_CHANGE_PARAMETERS_CNF:
        break;
    case GAVDP_EVENT_CHANGE_PARAMETERS_IND:
        break;
    case GAVDP_EVENT_ABORT_CNF:
        break;
    case GAVDP_EVENT_ABORT_IND:
        break;
    case GAVDP_EVENT_STREAM_RELEASE_CNF:
        break;
    case GAVDP_EVENT_STREAM_RELEASE_IND:
        break;
    }				    

    para.event = event;
    para.error = error;
    sep->callback (sep, &para);
    
}


static void GavdpCmgrEventCallback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status)
{
    bt_addr_struct device_addr;
    GavdpCallbackParms para;

    StreamEndPoint* sep;	
    BtGavdpCmgrNode* cmgrNode;
    BtGavdpChnlNode* chnlNode;	

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_CBK_INFO1, cmgr_hdlr, event, status);	


    //if (bt_a2dp_ctx.state == BT_A2DP_STATE_ACTIVATED &&  bt_a2dp_ctx.wait_cnf == BT_A2DP_WAIT_CNF_DATA_LINK)
    if ( GAVDP(sig_conn_state) == GAVDP_STATE_SIG_CONNECT_OUTGOING )
   {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_CBK_CON_OUTGOING);
        
        sep = GavdpFindSepByCmgrHdl(cmgr_hdlr);
        if (sep == 0)
        { 
            return;
        }

	
        switch (event)
        {
        case CMEVENT_DATA_LINK_CON_CNF:
            //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_NONE;
            //bt_a2dp_get_bt_device_addr(&bt_a2dp_ctx.remote_bd_addr, &device_addr);
            if (status == BT_STATUS_SUCCESS)
            {
/*
#ifdef SKIP_SDP_QUERY
		A2dpConnectAvdtpChannel(&device_addr, cmgr_hdlr->bdc->link);
#else
							
		status = A2dpSdpQuery();
				
		BT_A2DP_VAL_TRACE1(status);
				
		if (status == BT_STATUS_PENDING)
		{
			bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_SDP_QUERY;
		}
		else
		{
			A2dpCmgrLinkClose();
			CMGR_RemoveCmgrWithDevice(&bt_a2dp_ctx.cmgr_hdlr);				
			A2dpHandleConnectCnf(
							0,
							BT_A2DP_RESULT_TIMEOUT,
							&device_addr);
		}
								 
#endif // SKIP_SDP_QUERY 
*/
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_CBK_LINK_CON_CNF_SUCCESS);
                GavdpConnectAvdtpChannel(sep);			
            }			
            else
            {
                //Status == BT_STATUS_SCO_REJECT, shall handle SCO not allow
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CMGR_CBK_LINK_CON_CNF_FAILED);
                GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
                CMGR_RemoveDataLink(sep->cmgr_hdlr);				
                BTBMProfileReleaseConnIndex(sep->conn_id);
                sep->conn_id = 0xFF;		   
                CMGR_RemoveCmgrWithDevice(sep->cmgr_hdlr);
                //GavdpHandleEstablishConnectionCnf( sep, GAVDP_ERR_FATAL);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_FATAL);

                cmgrNode = GavdpFindCmgrNode(sep->cmgr_hdlr);                
                if( 0 != cmgrNode )
                {
                    cmgrNode->isUsed = 0;
                }				
                chnlNode = GavdpFindChnlNode(sep->signal_channel);
                if( 0 != chnlNode )
                {
                    chnlNode->isUsed = 0; 
                }			

                sep->cmgr_hdlr = 0;
	         sep->signal_channel = 0;				
            }
        }
    }
    
}


static void GavdpAvdtpEventCallback(AvdtpChannel *channel, AvdtpCallbackParms *info)
{
    StreamEndPoint *sep = 0;

    AvdtpEvent event = info->event;
    AvdtpStreamInfo *stream_info;
    AvdtpCodec *codec_info;

    GavdpCallbackParms para;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_INFO1, channel, event);   


    if (0 == GAVDP(sep_num))
    {
        OS_Report("[GAVDP][GavdpAvdtpEventCallback] no registered SEP.");
        return;
    }       

  
    sep = GavdpFindSepByChannel(channel);
    if (sep == 0)
    { 
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SEP_NOT_REG);      
        //TODO: only for event with sep, excluding conn_ind, disconvery ind, get cap ind, set config ind		
        if ( AVDTP_EVENT_CONNECT_IND == info->event ||
		AVDTP_EVENT_DISCOVER_IND == info->event ||
		AVDTP_EVENT_GET_CAP_IND == info->event ||
		AVDTP_EVENT_SET_CFG_IND == info->event ||
              (AVDTP_EVENT_CONNECT ==  info->event && GAVDP_STATE_SIG_CONNECT_INCOMING == GAVDP(sig_conn_state)) ||
              (AVDTP_EVENT_DISCONNECT == info->event && 
                (GAVDP_STATE_SIG_STRM_IN_CONNECTING == GAVDP(sig_conn_state) || GAVDP_STATE_SIG_CONNECT_INCOMING == GAVDP(sig_conn_state) ) ) )
        {
            //todo: callback to all SEPs which are not in used state
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_USE_FAKE_SEP);              
            sep = &GAVDP(fake_sep);
            if ( AVDTP_EVENT_CONNECT_IND == info->event )
            {
                memset(sep, 0, sizeof(StreamEndPoint));	            
            }			
        }
        else
        {
            return;
        }
    }
	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SEP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state); 		
    switch (event)
    {
        case AVDTP_EVENT_CONNECT:
        {

            bt_addr_struct device_addr;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECTED, info->error);
			
            //bt_a2dp_ctx.device_type = BTBMGetDeviceModel(&bt_a2dp_ctx.remote_bd_addr);
            if (info->error == AVDTP_ERR_NO_ERROR)
            {
                //#ifdef __A2DP_WAIT_SCO_DISCONNECT_TO_CONTINUE__
                //if(a2dp_wait_sco_disconnecting)
                //{
                //      a2dp_wait_sco_disconnecting = FALSE;
                //      BT_A2DP_FUNC_ENTRY1(BT_A2DP_SIGNAL_CHECK_CONNECT_CNF,a2dp_wait_sco_disconnecting);
                //}
                //#endif
                
                //bt_a2dp_get_bt_device_addr(&bt_a2dp_ctx.remote_bd_addr, &device_addr);
                if ( GAVDP(sig_conn_state) == GAVDP_STATE_SIG_CONNECT_OUTGOING)
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECTED_OUT);
                    GAVDP(sig_conn_state) = GAVDP_STATE_SIG_STRM_OUT_CONNECTING;
                    sep->stream_state = GAVDP_STATE_IDLE;
                    GavdpDiscoverStreamsReq(sep);						
					
                    //parms.event = BTA2DP_EVENT_CONNECT_CNF;
                    //parms.result = result;
                    //parms.p.A2DPEventSignalConnectCnf.connect_id = connect_id;
                    //parms.p.A2DPEventSignalConnectCnf.device_addr = device_addr;
                    //bt_a2dp_ctx.a2dpCBfunc(&parms);                    
                }
	         else if ( GAVDP(sig_conn_state) == GAVDP_STATE_SIG_CONNECT_INCOMING)
	         {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECTED_IN);
                    GAVDP(sig_conn_state) = GAVDP_STATE_SIG_STRM_IN_CONNECTING;
                    sep->stream_state = GAVDP_STATE_IDLE;
                    sep->command_stage = GAVDP_EST_WAIT_IND_DISCOVER;					 

	             //sep->sig_disc_timer.func = GavdpDiscSigChnl;
                    //sep->sig_disc_timer.context = (void*)sep;
	             sep->sig_disc_timer.func = GavdpSwitchRole;
                    GAVDP(switchPair).fake_sep = sep; 	
                    GAVDP(switchPair).guess_sep = (StreamEndPoint*)GetHeadList(&GAVDP(sepList));
                    sep->sig_disc_timer.context = (void*)&GAVDP(switchPair);                    
	            EVM_StartTimer(&sep->sig_disc_timer, 2000);                    				 
                    //bt_a2dp_ctx.connect_id = BTBMProfileAllocateConnIndex(SC_AUDIO_SOURCE, bt_a2dp_ctx.remote_bd_addr);
                    //A2dpHandleConnectCnf( (U16) bt_a2dp_ctx.connect_id, result, &device_addr );				
                }
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECTED_FAILED);
                    if ( sep != &GAVDP(fake_sep) ) 
                    {
                        //GAVDP_STATE_SIG_CONNECT_OUTGOING
                        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);              
                    }
                    else
                    {
                        //GAVDP_STATE_SIG_CONNECT_INCOMING
                        GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
                        sep->command_stage = GAVDP_WAIT_NONE;				
                        AVDTP_Disconnect(sep->signal_channel);  
                    }					
                }
	         //bt_a2dp_ctx.fm_over_sbc_enabled = 0;
            }
            else
            {
                Assert(0);
            }
            
        }	     
        break;

        case AVDTP_EVENT_CONNECT_IND:
            {
                BtStatus status ;
                BtGavdpCmgrNode*	cmgrNode;
                BtGavdpChnlNode* chnlNode;					   
                

                if (GAVDP_STATE_SIG_IDLE != GAVDP(sig_conn_state))
				{
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECT_IND_BAD_STAT);		  
                    AVDTP_ConnectRsp(channel, FALSE); 
                    return;
                }
		  
                if ( GavdpIsSepAvailable() )
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECT_IND);       
					
                    cmgrNode = GavdpFindFreeCmgrNode();
                    if(0 == cmgrNode)
                    {
                        para.event = GAVDP_EVENT_STREAM_ESTABLISH_CNF;
                        para.error = GAVDP_ERR_FATAL;
                        sep->callback (sep, &para);    
                        return;    
                    }
                    else
                    {
                        cmgrNode->isUsed = 1;
                    }
	
                    chnlNode =  GavdpFindChnlNode(channel);
                    if(0 == chnlNode)
                    {
                        para.event = GAVDP_EVENT_STREAM_ESTABLISH_CNF;
                        para.error = GAVDP_ERR_FATAL;
                        sep->callback (sep, &para);    
                        return;    
                    }
                    else
                    {
                        chnlNode->isUsed = 1;
                    }	

                    sep->cmgr_hdlr = &cmgrNode->cmgr_hdlr;
                    sep->signal_channel = &chnlNode->channel;
                    memcpy(&sep->remote_bd_addr, &info->p.remDev->bdAddr, sizeof(BD_ADDR));

                   
                    status = CMGR_CreateDataLink(sep->cmgr_hdlr, &info->p.remDev->bdAddr);					
                    if(status == BT_STATUS_SUCCESS || status == BT_STATUS_PENDING)
                    {
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECT_IND_CMGR_SUCCESS);
                         //bt_a2dp_ctx.fm_over_sbc_enabled = 0;
                        GAVDP(sig_conn_state) = GAVDP_STATE_SIG_CONNECT_INCOMING;
                        AVDTP_ConnectRsp(channel, TRUE);	   //AVDTP_EVENT_CONNECT
                        CMGR_ActivateSniffTimer(sep->cmgr_hdlr);
                        return;
                    }
                    else
                    {
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECT_IND_CMGR_FAILED);
                        AVDTP_ConnectRsp(channel, FALSE); // AVDTP_EVENT_CONNECT
                        CMGR_RemoveCmgrWithDevice(sep->cmgr_hdlr);
					
                        chnlNode->isUsed = 0;
                        cmgrNode->isUsed = 0;
          
                        sep->cmgr_hdlr = 0;
                        sep->signal_channel = 0;
                        //TODO: error handling, disconnect? 						
                    }    
                }
                else
                {
                     bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CONNECT_IND_NO_SEP);
					 AVDTP_ConnectRsp(channel, FALSE); 
                }				
            }				  
        break;

        case AVDTP_EVENT_DISCOVER_IND:

            EVM_CancelTimer(&sep->sig_disc_timer);			
            
            if ( GAVDP_EST_WAIT_IND_DISCOVER != sep->command_stage )
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISCOVER_IND_BAD_STAT, sep->command_stage);
                AVDTP_DiscoverStreamsRsp(AVDTP_RSP_REJECT, channel, 0, NULL);
                //TODO: error handling, disconnect? 
            }
            else
            {
                U8 i = 0;
                U8 sep_num = GAVDP(sep_num);
                AvdtpStreamInfo stream_info_list[sep_num];
                StreamEndPoint *temp_sep;

                temp_sep = (StreamEndPoint*)GetHeadList(&GAVDP(sepList));
                for ( i = 0; i< sep_num; i++)
                {
                    stream_info_list[i].inUse = temp_sep->info.inUse;
                    stream_info_list[i].mediaType = temp_sep->info.mediaType;
                    stream_info_list[i].streamType = temp_sep->info.streamType;
                    stream_info_list[i].id = temp_sep->info.id;
                    //bt_a2dp_ctx.stream.locStrmId = stream_info_list[0].id = sep_list[0].seid;

                    temp_sep = (StreamEndPoint*)GetNextNode(&GAVDP(sepList));
                }
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISCOVER_IND_ACPT);
                AVDTP_DiscoverStreamsRsp(AVDTP_RSP_ACCEPT, channel, sep_num, &stream_info_list[0]);
                sep->command_stage = GAVDP_EST_WAIT_IND_GET_CAP;

                sep->sig_disc_timer.func = GavdpDiscSigChnl;
                sep->sig_disc_timer.context = (void*)sep;
                EVM_StartTimer(&sep->sig_disc_timer, 500);  
            }
            
            break;

        case AVDTP_EVENT_STREAM_INFO: 

            if (GAVDP_EST_WAIT_CNF_DISCOVER == sep->command_stage)
            { 
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_STRM_INFO);         
                if (sep->remote_sep_count < MAX_NUM_REMOTE_SEIDS)
                // !info->p.streamInfo->inUse && 
                //info->p.streamInfo->mediaType == AVDTP_MEDIA_TYPE_AUDIO &&
                //info->p.streamInfo->streamType == AVDTP_STRM_ENDPOINT_SNK
                {
                    stream_info = &sep->remote_sep_info[sep->remote_sep_count];
                    stream_info->inUse = info->p.streamInfo->inUse;
                    stream_info->id = info->p.streamInfo->id;
                    stream_info->mediaType = info->p.streamInfo->mediaType;
                    stream_info->streamType = info->p.streamInfo->streamType;
                    sep->remote_sep_count++;
                }
            }	
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_STRM_INFO_BAD_STAT, sep->command_stage);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);				
            }			
            
            break;

        case AVDTP_EVENT_DISCOVER_CNF:
            
            if (GAVDP_EST_WAIT_CNF_DISCOVER == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_NONE;            
                if (info->error == AVDTP_ERR_NO_ERROR && sep->remote_sep_count!= 0)
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISCOVER_CNF);                
                    sep->current_cfg_sep_id = 0;		
                    GavdpGetCapabilityReq(sep);
                }
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISCOVER_CNF_FAILED);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, info->error);					
                }
            }	
	     else
	     {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISCOVER_CNF_BAD_STAT, sep->command_stage);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);			
	     }
            
            break;

			
        case AVDTP_EVENT_GET_CAP_IND:
        {
            U8 acp_id; 
	    StreamEndPoint* acp_sep;
            U8 elements[10];		 
            
            
            EVM_CancelTimer(&sep->sig_disc_timer);			

            if ( GAVDP_EST_WAIT_IND_GET_CAP != sep->command_stage )
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_GET_CAP_IND_BAD_STAT, sep->command_stage);            
                AVDTP_GetCapabilitiesRsp( AVDTP_RSP_REJECT, channel, &sep->stream, AVDTP_ERR_BAD_STATE);
		  //the fake SEP doesn't register stream.		
                //TODO: error handling, disconnect? 		
            }
            else
            { 
                //void A2DP_StreamGetCapRes(U16 connect_id, U16 result, bt_a2dp_audio_cap_struct *audio_cap_list)
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_GET_CAP_IND_RAW_DATALEN, info->rawDataLen);
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_GET_CAP_IND_RAW_DATA0, info->p.data[0]);
				
                acp_id = (U8) (info->p.data[0] >> 2);
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_GET_CAP_IND_ACP_SEP_ID, acp_id);
                acp_sep = GavdpFindSepById(acp_id);
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_GET_CAP_IND_ACP_SEP_ADDR, acp_sep);
				
                sep->stream.codec = &sep->codec;
                sep->stream.codec->elements = elements;
				
                GavdpCopyCodec(sep->stream.codec, &acp_sep->codec);
                sep->stream.mediaType= acp_sep->stream.mediaType;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_GET_CAP_IND_ACPT);
                AVDTP_GetCapabilitiesRsp(AVDTP_RSP_ACCEPT, channel, &sep->stream, AVDTP_ERR_NO_ERROR);
				
                sep->command_stage = GAVDP_EST_WAIT_IND_CONFIG;
            }
            
       }			
       break;

			
        case AVDTP_EVENT_CAPABILITY:
            /*
            if (info->p.capability->type == AVDTP_SRV_CAT_MEDIA_CODEC)
            {
                switch(bt_a2dp_ctx.device_type)
                {
                    case DEVICE_MOTO_HT820:
                        if (info->p.capability->p.codec.codecType == AVDTP_CODEC_TYPE_SBC)
                        {
                            if (info->p.capability->p.codec.elements[3] > 35)
                            {
                              info->p.capability->p.codec.elements[3] = 35; // sbc->max_bit_pool 
                            }
                        }
                        break;
					
                    default:
                        break;
                }                        
            }
            */
            
            //TODO: intepret audio, video, and multi-media capabilities			
            if (info->p.capability->p.codec.codecType == AVDTP_CODEC_TYPE_SBC)
            {	
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CAP_INFO);            
                if (sep->remote_cap_count < MAX_NUM_REMOTE_CAPS)
                {                
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CAP_INFO_CNT, sep->remote_cap_count);
                    codec_info = &sep->remote_cap_info[sep->remote_cap_count];			
                    codec_info->codecType = info->p.capability->p.codec.codecType;
                    codec_info->elemLen = info->p.capability->p.codec.elemLen;
                    memcpy(codec_info->elements, info->p.capability->p.codec.elements, codec_info->elemLen);					
                    sep->remote_cap_count++;
                }					
            }
            else
            {
                    OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_CAPABILITY]not AVDTP_CODEC_TYPE_SBC");
            }				
            
            break;
			
        case AVDTP_EVENT_GET_CAP_CNF:
           
            if (GAVDP_EST_WAIT_CNF_GET_CAP == sep->command_stage)
            {  
                sep->command_stage = GAVDP_WAIT_NONE;            
                if (info->error == AVDTP_ERR_NO_ERROR)
                {
                    if (sep->remote_cap_count != 0)
                    {
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CAP_CNF);
                        GavdpSetConfigurationReq(sep);
                    }
                    else
                    {
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CAP_CNF_FAILED);					
                        if( sep->remote_sep_count > 1 && sep->current_cfg_sep_id  < MAX_NUM_REMOTE_SEIDS)
                        {
                            OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_GET_CAP_CNF]try another SEP");  
                            sep->current_cfg_sep_id ++;							
                            GavdpGetCapabilityReq(sep);
			   }
			   else
			   {
                            GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_INVALID_CAPABILITIES);			   
			   }
                    }					
                }
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CAP_CNF_FAILED);					
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, info->error);					
                }
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CAP_CNF_BAD_STAT, sep->command_stage);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);				
            }
            
            break;

			
        case AVDTP_EVENT_SET_CFG_IND:
            {
            U8 result;				
            
            
            if ( GAVDP_EST_WAIT_IND_CONFIG!= sep->command_stage && GAVDP_EST_WAIT_IND_DISCOVER != sep->command_stage)
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_BAD_STAT, sep->command_stage);
                AVDTP_SetConfigRsp( AVDTP_RSP_REJECT, sep->signal_channel, &sep->stream, AVDTP_ERR_BAD_STATE, channel->capability.type);	
		  //the fake SEP doesn't register stream.	
                //TODO: error handling, disconnect? 					
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_RAW_DATALEN, info->rawDataLen);
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_RAW_DATA012, info->rawData[0], info->rawData[1], info->rawData[2]);
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_PKT_DATA012, info->p.data[0], info->p.data[1], info->p.data[2]);
                
				
                sep = GavdpFindSepById(info->rawData[0]>>2);
                //sep = GavdpFindSepById((U8) (info->p.data[0] >> 2));			
                if (0 == sep)
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_SEP_NOT_FOUND);
                    AVDTP_SetConfigRsp( AVDTP_RSP_REJECT, sep->signal_channel, &sep->stream, AVDTP_ERR_BAD_ACP_SEID, AVDTP_SRV_CAT_MEDIA_CODEC);					
                    //TODO: error handling, disconnect?
                }
                else
                {	

                    GavdpCopyCodec(&sep->remote_cfg_codec, (AvdtpCodec *)&info->p.configReq->codec);

                    if ( FALSE == GavdpDecideConfig(&sep->remote_cfg_codec, &sep->codec, (AvdtpCodec *)&info->p.configReq->codec ) )
                    {
                        sep = &GAVDP(fake_sep);
						
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_CODEC_MISMATCHED);
                        AVDTP_SetConfigRsp( AVDTP_RSP_REJECT, sep->signal_channel, &sep->stream, AVDTP_ERR_INVALID_CAPABILITIES, AVDTP_SRV_CAT_MEDIA_CODEC);
                        //AVDTP_SetConfigRsp( AVDTP_RSP_REJECT, sep->signal_channel, &sep->stream, AVDTP_ERR_INVALID_CAPABILITIES, channel->capability.type);						
                        //TODO: error handling, disconnect?                        
                    }
                    else
                    {
                        sep->cmgr_hdlr = GAVDP(fake_sep).cmgr_hdlr;
                        sep->signal_channel = GAVDP(fake_sep).signal_channel;
                        sep->command_stage = GAVDP(fake_sep).command_stage;
                        sep->stream.chnl = sep->signal_channel;
                        memcpy(&sep->remote_bd_addr, &GAVDP(fake_sep).remote_bd_addr, sizeof(BD_ADDR));	

					
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_IND_ACPT);
                        GavdpCopyCodec(&sep->current_cfg_codec, &sep->remote_cfg_codec);                        
                        AVDTP_SetConfigRsp( AVDTP_RSP_ACCEPT, sep->signal_channel, &sep->stream, AVDTP_ERR_NO_ERROR, AVDTP_SRV_CAT_MEDIA_CODEC);//only handle srv_cat_media					
                        sep->command_stage = GAVDP_EST_WAIT_IND_OPEN;  
                    }
                }
            }
            
            }			
	     break;		


        case AVDTP_EVENT_SET_CFG_CNF:
            
            if (GAVDP_EST_WAIT_CNF_CONFIG == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_NONE;
                if (info->error == AVDTP_ERR_NO_ERROR)
                {
                    // we may have a race condition: config req vs config ind 
                    //BT_A2DP_ENTER_STATE(BT_A2DP_STATE_CONFIGURED);
                    //inform bt_a2dp_android.c configuration change
                    //A2dpHandleConfigChange(&bt_a2dp_ctx.cur_config);
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_CNF);                    
                    GavdpCopyCodec(&sep->current_cfg_codec, &sep->local_cfg_codec);
                    GavdpOpenStreamReq(sep); 					
                }
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_CNF_FAILED);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, info->error);
                    /*
                    //TODO: remote sep id is not in sequence, 1, 2, 3...
                    if ( sep->current_cfg_sep_id < sep->remote_sep_count )
                    {
                        OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_SET_CFG_CNF]current_cfg_sep_id:%d, get another sep capability",sep->current_cfg_sep_id);
                        sep->current_cfg_sep_id++; 						
                        GavdpGetCapabilityReq(sep);  			
                    }
                    else
                    {
                        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, info->error);
                    }
                    */					
                }				
            }	
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SET_CFG_CNF_BAD_STAT, sep->command_stage);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);				
            }
            //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_NONE;
            //A2dpHandleStreamConfigCnf( bt_a2dp_ctx.connect_id, result, bt_a2dp_ctx.stream.locStrmId);
            
            break;

			
        case AVDTP_EVENT_STREAM_OPEN_IND:
            

            // if ( sep !=  GavdpFindSepById(info->rawData[0]>>2))			
            if ( sep != GavdpFindSepById(info->p.data[0] >> 2) )			
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_IND_BAD_SEP, info->p.data[0] >> 2, sep->info.id);        
                AVDTP_OpenStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
               //TODO: error handling, disconnect?        
            }
		
            if ( GAVDP_EST_WAIT_IND_OPEN!= sep->command_stage )
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_IND_BAD_STAT , sep->command_stage);
                AVDTP_OpenStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
               //TODO: error handling, disconnect? 
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_IND);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_IND, AVDTP_ERR_NO_ERROR);				
            }	
            //bt_a2dp_ctx.device_type = BTBMGetDeviceModel(&bt_a2dp_ctx.remote_bd_addr);
            //switch (bt_a2dp_ctx.device_type)
            //{
            //    case DEVICE_WIREVO_HSH200:
            //        Report(("WIREVO_HSH200 no role switch"));
            //        break;
            //    default:
            //        if(info->error !=AVDTP_ERR_NO_ERROR)
            //            CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.cmgr_hdlr);
            //        break;
            //}
            
            break;

        case AVDTP_EVENT_STREAM_OPEN:
            //if(info->error !=AVDTP_ERR_NO_ERROR)
            //    CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.cmgr_hdlr);
            
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_CNF);			
            GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;	
            if (GAVDP_EST_WAIT_CNF_OPEN == sep->command_stage)
            {  
                sep->command_stage = GAVDP_WAIT_NONE;            
                if (info->error == AVDTP_ERR_NO_ERROR)
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_CNF_SUCCESS);
                    sep->stream_state = GAVDP_STATE_OPEN;
                }
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_CNF_FAILED);
                    sep->stream_state = GAVDP_STATE_IDLE;				
                }	
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, info->error);				
            }
            else
            {
                if ( sep->stream_state == GAVDP_STATE_OPEN )
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_CNF_HAS_OPEND);
                    //headset initiates connection, it doesn't need to callback 
                }
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_OPEN_CNF_BAD_STAT, sep->command_stage);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);					
                }
                //TODO: error handle (disconnect), sep->command_stage = GAVDP_WAIT_NONE;				
            }
            
            break;
    
        case AVDTP_EVENT_STREAM_START_IND:
            
            if (GAVDP_STATE_OPEN == sep->stream_state && GAVDP_WAIT_NONE == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_RES_START;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_IND_CMD_STAT);				
                sep->prefer_size = (U16)AVDTP_MediaPacketSize(&sep->stream) - 13;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_IND);				
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_START_IND, AVDTP_ERR_NO_ERROR);				
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_IND_BAD_STAT, sep->command_stage);
                AVDTP_StartStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);           
            }
            
            break;

        case AVDTP_EVENT_STREAM_STARTED:
            //bt_a2dp_ctx.device_type = BTBMGetDeviceModel(&bt_a2dp_ctx.remote_bd_addr);
            //CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.cmgr_hdlr);
            
			
            if (GAVDP_STATE_OPEN == sep->stream_state && GAVDP_WAIT_CNF_START == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_NONE;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_CNF_CMD_STAT);					
                if(AVDTP_ERR_NO_ERROR == info->error)
                {
                    sep->stream_state = GAVDP_STATE_STREAMING;	
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_CNF_STAT_TRANS);
		    sep->prefer_size = (U16)AVDTP_MediaPacketSize(&sep->stream) - 13;

                    //A2dpHandleStreamStartCnf(result, bt_a2dp_ctx.stream.locStrmId, A2DP_GetPreferredPayloadSize(bt_a2dp_ctx.stream.locStrmId));
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_CNF_SUCCESS);
                }					
                else
                {
                    //if(bt_a2dp_ctx.fm_over_bt_mode == 1){bt_a2dp_ctx.fm_over_bt_mode = 0;}
                    //A2dpHandleStreamStartCnf(result, bt_a2dp_ctx.stream.locStrmId, 0);
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_CNF_FAILED);
                }
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_START_CNF, info->error);				
            }			
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_START_CNF_BAD_STAT, sep->command_stage);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_START_CNF, AVDTP_ERR_BAD_STATE);				
			
/*            
                BT_A2DP_VAL_TRACE1(bt_a2dp_ctx.wait_started);
                if(bt_a2dp_ctx.wait_started)
                {
                    bt_a2dp_ctx.wait_started = FALSE;
#ifdef MTKA2DP_WISE
#ifndef BTMTK_ON_WISE
                    A2dpHandleStreamStartIndCfm();
#endif
#endif
                    // this is the case that suspend_req comes between start_res and started callback 
                    if( bt_a2dp_ctx.postpond_suspend )
                    {
                        bt_a2dp_ctx.postpond_suspend = FALSE;
                        status = AVDTP_SuspendStream(&bt_a2dp_ctx.stream);
                        BT_A2DP_VAL_TRACE1(status);
                        if (status == BT_STATUS_PENDING)
                        {
                            bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_SUSPEND;
                        }
                        else
                        {
                            A2dpHandleStreamSuspendCnf( BT_A2DP_RESULT_BAD_STATE, (U8)bt_a2dp_ctx.stream.locStrmId);
                        }
                    }
                }
            }
            //bt_a2dp_ctx.device_type = BTBMGetDeviceModel(&bt_a2dp_ctx.remote_bd_addr);
            //CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.cmgr_hdlr);
*/ 	
        }
            
        break;

			
        case AVDTP_EVENT_STREAM_SUSPEND_IND:
            //A2DP_StopFMOverSBC(info->p.remDev);            
            
            if (GAVDP_STATE_STREAMING == sep->stream_state && GAVDP_WAIT_NONE == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_RES_SUSPEND;	
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_IND_CMD_STAT);
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_IND);				
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_SUSPEND_IND, AVDTP_ERR_NO_ERROR);				
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_IND_BAD_STAT, sep->command_stage);            
                AVDTP_SuspendStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
            }
            
            break;

        case AVDTP_EVENT_STREAM_SUSPENDED:
            //A2DP_StopFMOverSBC(info->p.remDev);
            
            if (GAVDP_STATE_STREAMING == sep->stream_state && GAVDP_WAIT_CNF_SUSPEND == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_NONE;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_CNF_CMD_STAT);
                if(AVDTP_ERR_NO_ERROR == info->error)
                {
                    sep->stream_state = GAVDP_STATE_OPEN;
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_CNF_STAT_TRANS);
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_CNF_SUCCESS);
                }					
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_CNF_FAILED);
                }
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_SUSPEND_CNF, info->error); 				
            }			
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_SUSPEND_CNF_BAD_STAT, sep->command_stage);             
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_SUSPEND_CNF, AVDTP_ERR_BAD_STATE);					 
            }	
            
            break;				
 				

        case AVDTP_EVENT_STREAM_RECONFIG_IND:
            
            if (GAVDP_STATE_OPEN== sep->stream_state && GAVDP_WAIT_NONE == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_RES_RECONFIG;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_IND_CMD_STAT);
                memcpy(&sep->remote_cfg_codec, (AvdtpCodec *)&info->p.configReq->codec, sizeof(AvdtpCodec));
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_IND);			
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_IND, AVDTP_ERR_NO_ERROR);				
//#ifdef BTMTK_ON_LINUX
                //keep the remote request configuration setting
                //bt_adp_a2dp_unpack_audio_cap((A2dpCodec *)&info->p.configReq->codec, &bt_a2dp_ctx.remote_req_config);
//#endif
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_IND_BAD_STAT, sep->command_stage);
                AVDTP_ReconfigRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE, sep->codec.codecType);
            }
            
            break;

        case AVDTP_EVENT_STREAM_RECONFIG_CNF:
            
            if (GAVDP_STATE_OPEN == sep->stream_state && GAVDP_WAIT_CNF_RECONFIG == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_NONE;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_CNF_CMD_STAT);				
                if(AVDTP_ERR_NO_ERROR == info->error)
                {
                    GavdpCopyCodec(&sep->current_cfg_codec, &sep->local_cfg_codec);
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_CNF_SUCCESS);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_CNF, AVDTP_ERR_NO_ERROR);					
                }					
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_CNF_FAILED);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_CNF, info->error);					
                }
            }			
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_RECONFIG_CNF_BAD_STAT, sep->command_stage);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_CNF, AVDTP_ERR_BAD_STATE);				
            }
            
            break;


        case AVDTP_EVENT_STREAM_ABORT_IND:
            //A2DP_StopFMOverSBC(info->p.remDev);
            
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_ABORT_IND_CMD_STAT);			
            sep->command_stage = GAVDP_WAIT_RES_ABORT;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_ABORT_IND);			
            GavdpGavdpEventCallback(sep, GAVDP_EVENT_ABORT_IND, AVDTP_ERR_NO_ERROR);			
            
            break;			

        case AVDTP_EVENT_STREAM_ABORTED:
            //A2DP_StopFMOverSBC(info->p.remDev);    
            
	     if (GAVDP_STATE_IDLE > sep->stream_state)
            {                        
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_ABORT_CNF_BAD_STAT, sep->command_stage);
            }
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_ABORT_CNF_STAT_TRANS);
            //sep->stream_state = GAVDP_STATE_IDLE;
            if ( sep->stream_state != GAVDP_STATE_IDLE )
            {
                sep->stream_state = GAVDP_STATE_ABORTED;
            }          
			
            if (  GAVDP_WAIT_CNF_ABORT == sep->command_stage )
            {            
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_ABORT_CNF);
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_ABORT_CNF, AVDTP_ERR_NO_ERROR);				
            }            
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_ABORT_CNF_CMD_STAT);
            sep->command_stage = GAVDP_WAIT_NONE;			
            
            break;



        case AVDTP_EVENT_STREAM_CLOSE_IND:
            //A2DP_StopFMOverSBC(info->p.remDev);
            
            if (GAVDP_STATE_IDLE < sep->stream_state && GAVDP_WAIT_NONE == sep->command_stage)
            {
                sep->command_stage = GAVDP_WAIT_RES_CLOSE;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_IND_CMD_STAT);	
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_IND);				
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_IND, AVDTP_ERR_NO_ERROR);				
            }
            else
            {
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_IND_BAD_STAT, sep->command_stage);

                sep->stream_state = GAVDP_STATE_IDLE;
                GAVDP(sig_conn_state) = GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTING;

                AVDTP_CloseStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
            }	
            
            break;

//TODO: how to handle this event and "void A2DP_SignalDisconnectReq(U16 connect_id)"
        case AVDTP_EVENT_STREAM_CLOSED:			
            //A2DP_StopFMOverSBC(info->p.remDev);
            
            if ( GAVDP_STATE_ABORTED == sep->stream_state ||
		    (GAVDP_STATE_IDLE < sep->stream_state && GAVDP_WAIT_CNF_CLOSE == sep->command_stage))
            {
                sep->command_stage = GAVDP_WAIT_NONE;
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_CNF_CMD_STAT);
                if(AVDTP_ERR_NO_ERROR == info->error)
                {
                    sep->stream_state = GAVDP_STATE_IDLE;
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_CNF_STAT_TRANS);
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_CNF_SUCCESS);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, AVDTP_ERR_NO_ERROR);			
                }					
                else
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_CNF_FAILED);
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, info->error); 
                }		
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_CNF_DISC_SIG_CHNL);		 
                AVDTP_Disconnect(sep->signal_channel);					
            }			
            //for disonnect req while streaming, some earphone will send closed first and then 
            //disconnected so we shall ignore this close ind in this case
            //TODO: how to handle the following case	 		
            //else if(bt_a2dp_ctx.wait_cnf != BT_A2DP_WAIT_CNF_DISCONNECT)
            //{
            //or some earphone will send stream closed directly to handset (not close ind)
            //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_NONE;
            //A2dpHandleStreamCloseInd(bt_a2dp_ctx.stream.locStrmId);
            //}			
            else
            {


			
            if( GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTING == GAVDP(sig_conn_state) && GAVDP_WAIT_NONE == sep->command_stage )
            {
                //after sending GVADP_ReleaseConnectRsp
                OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_STREAM_CLOSED][AVDP_STATE_SIG_PASSIVE_DISCONNECTING]start sig_disc_timer");
                GAVDP(sig_conn_state) = GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTED;
	         sep->sig_disc_timer.func = GavdpDiscSigChnl;
                sep->sig_disc_timer.context = (void*)sep;			 
	         EVM_StartTimer(&sep->sig_disc_timer, 1000);	
            }	
            else if( GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTED == GAVDP(sig_conn_state) && GAVDP_WAIT_NONE == sep->command_stage )
            {
                //after L2EVENT_DISCONNETED, AVDTP Stream Disconnect
               OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_STREAM_CLOSED][AVDP_STATE_SIG_PASSIVE_DISCONNECTED]");                
               GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, AVDTP_ERR_NO_ERROR);				
            }
            else
            {
             //TODO: error handling.	
                bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_CLOSE_CNF_BAD_STAT, sep->command_stage);
                if ( GAVDP_STATE_SIG_STRM_OUT_CONNECTING == GAVDP(sig_conn_state) &&
                     GAVDP_STATE_IDLE == sep->stream_state &&
                     GAVDP_EST_WAIT_CNF_OPEN == sep->command_stage )
                {
                    OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_STREAM_CLOSED]GAVDP_STATE_SIG_STRM_OUT_CONNECTING");
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_NO_RESOURCE);
                }
                else if  ( GAVDP_STATE_SIG_STRM_IN_CONNECTING == GAVDP(sig_conn_state) &&
                     GAVDP_STATE_IDLE == sep->stream_state)
                {
                    OS_Report("[GAVDP][GavdpAvdtpEventCallback][AVDTP_EVENT_STREAM_CLOSED]GAVDP_STATE_SIG_STRM_IN_CONNECTING");                
                }
                else
                {
                    GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, AVDTP_ERR_BAD_STATE);
                }
                sep->command_stage = GAVDP_WAIT_NONE;
            }


			
            }	
            //TODO: Design Rlease Connection flow (stream channel and signal channel)
            //sep->info.inUse = 0;	
            
            break;


        case AVDTP_EVENT_STREAM_PACKET_SENT:
            
            para.event = GAVDP_EVENT_STREAM_PACKET_SENT;
            para.error = AVDTP_ERR_NO_ERROR;
            para.p.data = info->p.data;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_PACKET_SENT);   			
            sep->callback (sep, &para);			
            
            break;

        case AVDTP_EVENT_DISCONNECT:
            {			
                
                BtGavdpCmgrNode* cmgrNode;
                BtGavdpChnlNode* chnlNode;	

                if ( 0 == sep->signal_channel->conn.l2ChannelId) {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISC_SIG_CHNL);
                EVM_CancelTimer(&sep->sig_disc_timer);	                   

                    if ( GAVDP_STATE_SIG_CONNECT_OUTGOING == GAVDP(sig_conn_state) ||
                          (GAVDP_STATE_SIG_STRM_OUT_CONNECTING == GAVDP(sig_conn_state) && 
                               (GAVDP_EST_WAIT_CNF_DISCOVER == sep->command_stage ||
                                GAVDP_EST_WAIT_CNF_GET_CAP == sep->command_stage ||
                                GAVDP_EST_WAIT_CNF_CONFIG == sep->command_stage ||
                                GAVDP_EST_WAIT_CNF_OPEN == sep->command_stage)))					
                    {
                        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_BAD_SEP);	              
                    }			

					
                    CMGR_RemoveDataLink(sep->cmgr_hdlr);
                    BTBMProfileReleaseConnIndex(sep->conn_id);
                    sep->conn_id = 0xFF;            
                    CMGR_RemoveCmgrWithDevice(sep->cmgr_hdlr);
				
                    cmgrNode = GavdpFindCmgrNode(sep->cmgr_hdlr);                
                    if( 0 == cmgrNode )
                    {
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISC_CMGR);
                    }
                    else
                    {
                         cmgrNode->isUsed = 0;
                    }
			
                    chnlNode = GavdpFindChnlNode(sep->signal_channel);
                    if( 0 == chnlNode )
                    {
                        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISC_CHNL);                    
                    }
                    else
                    {
                        chnlNode->isUsed = 0;
                    }
					
                    sep->info.inUse = 0;
                    sep->cmgr_hdlr = 0;
	             sep->signal_channel = 0;
                    memset(&sep->remote_bd_addr, 0, sizeof(BD_ADDR));
					
                    GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
                    sep->stream_state = GAVDP_STATE_IDLE;
                    sep->command_stage = GAVDP_WAIT_NONE;                					
                }
                else 
                {
                    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_AVDTP_CBK_DISC_STRM_CHNL);
                }			
/*			
            //A2DP_StopFMOverSBC(info->p.remDev);
            //A2dpSignalDisconnectCnf(channel, info);
            //bt_a2dp_ctx.fm_over_sbc_enabled = 0;
            {            
                bt_addr_struct device_addr;
                U16 result;
                U16 connect_id = (U16) bt_a2dp_ctx.connect_id;
                U8 wait_cnf = bt_a2dp_ctx.wait_cnf;
                U8 wait_res = bt_a2dp_ctx.wait_res;

                //BT_A2DP_FUNC_ENTRY4(BT_A2DP_SIGNAL_DISCONNECT_CNF, info->error, bt_a2dp_ctx.state, bt_a2dp_ctx.wait_cnf, bt_a2dp_ctx.wait_res);
                if (info->error == AVDTP_ERR_NO_ERROR)
                {
                    result = BT_A2DP_RESULT_OK;
                }
                else
                {
                    result = BT_A2DP_RESULT_TIMEOUT;
                }

                bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_NONE;
                bt_a2dp_ctx.wait_res = BT_A2DP_WAIT_RES_NONE;

                // connect ind and then fail 
                //if (wait_res == BT_A2DP_WAIT_RES_CONNECT)
                if (GAVDP(sig_conn_state) == GAVDP_STATE_SIG_CONNECT_OUTGOING)		
                    return;
    
                bt_a2dp_get_bt_device_addr(&bt_a2dp_ctx.remote_bd_addr, &device_addr);

                switch (wait_cnf)
                {
                    case BT_A2DP_WAIT_CNF_NONE:
                        if (bt_a2dp_ctx.state == BT_A2DP_STATE_STREAMING &&
                           !(wait_res == BT_A2DP_WAIT_RES_SUSPEND ||
                           wait_res == BT_A2DP_WAIT_RES_CLOSE || wait_res == BT_A2DP_WAIT_RES_ABORT))
                        {
                            A2dpHandleStreamSuspendInd( bt_a2dp_ctx.stream.locStrmId);
                        }
                    break;
        case BT_A2DP_WAIT_CNF_DATA_LINK:
        case BT_A2DP_WAIT_CNF_SDP_QUERY:
        case BT_A2DP_WAIT_CNF_CONNECT:
            A2dpHandleConnectCnf(0, BT_A2DP_RESULT_TIMEOUT, &device_addr);
            break;
        case BT_A2DP_WAIT_CNF_DISCONNECT:
            A2dpHandleDisconnectCnf(connect_id, result);
            break;
        case BT_A2DP_WAIT_CNF_DISCOVER:
            A2dpHandleSepDiscoverCnf( connect_id, BT_A2DP_RESULT_NO_CONNECTION, 0, 0);
            break;
        case BT_A2DP_WAIT_CNF_GET_CAP:
            A2dpHandleCapabilitiesGetCnf( connect_id, BT_A2DP_RESULT_NO_CONNECTION);
            break;
        case BT_A2DP_WAIT_CNF_CONFIG:
            A2dpHandleStreamConfigCnf( 0, BT_A2DP_RESULT_NO_CONNECTION, 0);
            break;
        case BT_A2DP_WAIT_CNF_OPEN:
            A2dpHandleStreamOpenCnf( BT_A2DP_RESULT_NO_CONNECTION, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_RECONFIG:
            A2dpHandleStreamReconfigCnf( BT_A2DP_RESULT_NO_CONNECTION, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_START:
            A2dpHandleStreamStartCnf( BT_A2DP_RESULT_NO_CONNECTION, bt_a2dp_ctx.stream.locStrmId, 0);
            break;
        case BT_A2DP_WAIT_CNF_SUSPEND:
            A2dpHandleStreamSuspendCnf( BT_A2DP_RESULT_NO_CONNECTION, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_CLOSE:
            A2dpHandleStreamCloseCnf( BT_A2DP_RESULT_NO_CONNECTION, bt_a2dp_ctx.stream.locStrmId);
            break;
                    case BT_A2DP_WAIT_CNF_ABORT:
                        A2dpHandleStreamAbortCnf( bt_a2dp_ctx.stream.locStrmId);
                    break;
                }

                if (wait_cnf != BT_A2DP_WAIT_CNF_CONNECT &&
                    wait_cnf != BT_A2DP_WAIT_CNF_DISCONNECT)
                {
                    //A2dpHandleDisconnectInd(connect_id);
                    para.event = GAVDP_EVENT_SIG_DISCONNECTED;
                    para.error = info->error;
                    sep->callback (sep, &para);		
                }

                //BT_A2DP_ENTER_STATE(BT_A2DP_STATE_ACTIVATED);
                //bt_a2dp_ctx.stream_info_count = 0;
                sep->remote_sep_count = 0;
                sep->remote_cap_count = 0;
	
                CMGR_RemoveDataLink(&GAVDP(cmgr_hdlr));//need to find correct cmgr handler    
                BTBMProfileReleaseConnIndex(sep->conn_id);
                sep->conn_id = 0xFF;            
                CMGR_RemoveCmgrWithDevice(&GAVDP(cmgr_hdlr));//need to find correct cmgr handler 
*/
        	}
            
            break;
    }	
    
}











//------------------------------------------------------------------------------
//part5

GavdpError GAVDP_Init(void)
{
    GavdpError status = GAVDP_ERR_NO_ERR;

    if ( GAVDP_STATE_INITIALIZED != gavdpContextState || 0 == GAVDP(sep_num))
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_INIT);

        if ( gavdpContextState == GAVDP_STATE_DEINITIALIZING )
        {
            OS_Report("[GAVDP][GAVDP_Init]GAVDP_STATE_DEINITIALIZING");        
            BtGavdpChnlNode* chnlNode;
            BtGavdpCmgrNode* cmgrNode;			
            while ( !IsListEmpty(&GAVDP(chnlList)) )
            {
                OS_Report("[GAVDP][GAVDP_Init]free channel node");             
                chnlNode = (BtGavdpChnlNode*)RemoveHeadList(&GAVDP(chnlList));
		  free(chnlNode);
            }
            while ( !IsListEmpty(&GAVDP(cmgrList)) )
            {
                OS_Report("[GAVDP][GAVDP_Init]free cmgr node");             
                cmgrNode = (BtGavdpCmgrNode*)RemoveHeadList(&GAVDP(cmgrList));
		  free(cmgrNode);
            }			
        }		
        gavdpContextState = GAVDP_STATE_INITIALIZED;		
        GavdpAlloc();
	
        //CMGR_RegisterHandler(&GAVDP(cmgr_hdlr), GavdpCmgrEventCallback); //every sep/channel needs one cmgr handler
        GAVDP(sep_num) = 0;
        GAVDP(assigned_seid) = 1;
        GAVDP(sig_conn_state) =  GAVDP_STATE_SIG_IDLE; 	


        InitializeListHead(&GAVDP(sepList));
        InitializeListHead(&GAVDP(chnlList));
        InitializeListHead(&GAVDP(cmgrList));
	
        AVDTP_Init();
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_INIT_ALREADY);	    
        status = GAVDP_ERR_ALREADY_INIT;
    }

    return status;
}


void GAVDP_DeInit(void)
{
    BtGavdpCmgrNode* cmgrNode;

    OS_Report("[GAVDP][GAVDP_DeInit]");
    gavdpContextState = GAVDP_STATE_DEINITIALIZING;

    EVM_CancelTimer(&GAVDP(fake_sep).sig_disc_timer);
    
	
    cmgrNode = (BtGavdpCmgrNode*)GetHeadList(&GAVDP(cmgrList));
    while (cmgrNode != (BtGavdpCmgrNode*)&GAVDP(cmgrList))
    {
        OS_Report("[GAVDP][GAVDP_DeInit]CMGR_DeregisterHandler");
        CMGR_DeregisterHandler(&cmgrNode->cmgr_hdlr);		
        cmgrNode = (BtGavdpCmgrNode*)GetNextNode(&cmgrNode->node); 
    }
		
	
}


void GAVDP_Register(StreamEndPoint* sep)
{
    BtStatus status;

    BtGavdpChnlNode* chnlNode;
    BtGavdpCmgrNode* cmgrNode;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG, sep);	
	
    if(TRUE == IsNodeOnList(&GAVDP(sepList), (ListEntry*)sep))
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_NO_SEP);    
        return; 		 
    }	

  
    chnlNode = (BtGavdpChnlNode*)malloc(sizeof(BtGavdpChnlNode));
    memset(chnlNode, 0, sizeof(BtGavdpChnlNode));
	
    cmgrNode = (BtGavdpCmgrNode*)malloc(sizeof(BtGavdpCmgrNode));
    memset(cmgrNode, 0, sizeof(BtGavdpCmgrNode));		

	
    GAVDP(sep_num) ++;
    sep->info.id = GAVDP(assigned_seid);	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_SEP_ID, sep->info.id);	
	

    // register signal channel and callback function 
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_CHNL);    
    status = AVDTP_Register(&chnlNode->channel, GavdpAvdtpEventCallback);	
    Assert (status == BT_STATUS_SUCCESS);
	
    // register stream & codec 
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_STRM);      
    status = AVDTP_RegisterStream(&sep->stream, &sep->codec);
    sep->stream.locStrmId = GAVDP(assigned_seid)++; 	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_STRM_ID, sep->stream.locStrmId);  	
    Assert(status == BT_STATUS_SUCCESS);
	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_CMGR);  	
    status = CMGR_RegisterHandler(&cmgrNode->cmgr_hdlr, GavdpCmgrEventCallback);

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_INS_SEP); 
    InsertTailList(&GAVDP(sepList), &sep->node);	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_INS_CHNL); 	
    InsertTailList(&GAVDP(chnlList), &chnlNode->node);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REG_INS_CMGR); 	
    InsertTailList(&GAVDP(cmgrList), &cmgrNode->node);
	
    
    return;	
}


BtStatus GAVDP_DeRegister(StreamEndPoint* sep)
{
    AvdtpChannel* channel;
    CmgrHandler* cmgr_hdlr;
	
    BtGavdpCmgrNode* cmgrNode;
    BtGavdpChnlNode* chnlNode;
    BtStatus status = BT_STATUS_SUCCESS;	

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG, sep); 
    if(FALSE == IsNodeOnList(&GAVDP(sepList), (ListEntry*)sep))
    {
         bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG_NO_SEP);     
         return BT_STATUS_NOT_FOUND; 		 
    }

    EVM_CancelTimer(&sep->sig_disc_timer);

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG_STRM); 	

    if (GAVDP_STATE_SIG_STRM_OUT_CONNECTING == GAVDP(sig_conn_state))
    {
        GAVDP(sig_conn_state) = GAVDP_STATE_STRM_DEREGING;
        AVDTP_Disconnect(sep->signal_channel);
        status = BT_STATUS_BUSY;
        return status; 
    }
    else
    {
        status = AVDTP_DeregisterStream(&sep->stream);
        if(status != BT_STATUS_SUCCESS)
        {
            return status;
            //temp solution of ALPS00118063
            //GAVDP_ReleaseConnectionReq(sep);
	     //return BT_STATUS_PENDING;
        }    
    }


    if ( 0 == sep->signal_channel )
    {          
        chnlNode = GavdpFindFreeChnlNode();
        if ( 0 == chnlNode )
        {
            OS_Report("[GAVDP][GAVDP_DeRegister]Free channel node is not found. To release the node while initializing.");	 
        }
        else
        {
            channel = &chnlNode->channel; 

            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG_CHNL);  		
            status = AVDTP_Deregister(channel);
            OS_Report("[GAVDP][GAVDP_DeRegister]AVDTP_Deregister: %d", status);		
            if(status == BT_STATUS_SUCCESS)
            { 
                RemoveEntryList(&chnlNode->node);	
                free(chnlNode);	
            }	
            else
            {
                chnlNode->isUsed = 1;
            }
        }		
    }
    else
    { 	
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG_CHNL_NOT_RECALL); 	
    }

	
    if ( 0 == sep->cmgr_hdlr )
    {   
        cmgrNode = GavdpFindFreeCmgrNode();
        if ( 0 == cmgrNode )
        {
            OS_Report("[GAVDP][GAVDP_DeRegister]Free CMGR node is not found. To release the node while initializing.");        
        }
        else
        {
            cmgr_hdlr = &cmgrNode->cmgr_hdlr;
		
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG_CMGR);	
            CMGR_DeregisterHandler(cmgr_hdlr);
            RemoveEntryList(&cmgrNode->node);
            free(cmgrNode);
        }		
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_DEREG_CMGR_NOT_RECALL);  		
    }

 
    RemoveEntryList(&sep->node);
    GAVDP(sep_num) --;	
    
    return status;	
}


void GAVDP_EstablishConnectionReq(StreamEndPoint* sep, BD_ADDR* device_addr)
{
    GavdpCallbackParms para;
    BtStatus status;
    BtGavdpCmgrNode*	cmgrNode;
    BtGavdpChnlNode* chnlNode;	

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ, 
        sep, device_addr->addr[0],device_addr->addr[1],device_addr->addr[2],device_addr->addr[3],device_addr->addr[4],device_addr->addr[5]);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);	

    OS_MemCopy(sep->remote_bd_addr.addr, device_addr->addr, 6);

    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_PANIC);		
        return;
    }
  
    if ( GAVDP_STATE_SIG_IDLE != GAVDP(sig_conn_state) )
    {
        if ( GAVDP_STATE_SIG_CONNECT_INCOMING == GAVDP(sig_conn_state) || 
	 		GAVDP_STATE_SIG_STRM_IN_CONNECTING == GAVDP(sig_conn_state) )
        {   
            OS_Report("[GAVDP][GAVDP_EstablishConnectionReq][WRN]Connection or Stream is incoming");    
            GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_CONN_IN);           
        }
        else
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ_BAD_STAT);    
            if( GAVDP_STATE_SIG_STRM_OUT_CONNECTING == GAVDP(sig_conn_state))
            {
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_CONNECTING);            
            }
            else
            {
                GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, AVDTP_ERR_BAD_STATE);
            }      
        }
        return;
    }

    if (GavdpIsDevConnected(device_addr))
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_SAME_DEV);    
        return;   
    }
	
	
    cmgrNode = GavdpFindFreeCmgrNode();
    if(0 == cmgrNode)
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_NO_RESOURCE);
        return;    
    }
    else
    {
        cmgrNode->isUsed = 1;
    }
	
    chnlNode = GavdpFindFreeChnlNode();
    if(0 == chnlNode)
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_NO_RESOURCE);		
        return;    
    }
    else
    {
        chnlNode->isUsed = 1;
    }	

    sep->cmgr_hdlr = &cmgrNode->cmgr_hdlr;
    sep->signal_channel = &chnlNode->channel;
//#ifdef __A2DP_WAIT_SCO_DISCONNECT_TO_CONTINUE__
    //a2dp_tmp_device_addr = *(device_addr);
    //
    //if( (hfga_get_current_state()==hfg_sco_disconnecting)||(hsga_get_current_state()==hsg_sco_disconnecting))
    //{
    //    a2dp_wait_sco_disconnecting = TRUE;
    //    BT_A2DP_FUNC_ENTRY1(BT_A2DP_SIGNAL_WAIT_SCO_DISCONNECT,a2dp_wait_sco_disconnecting);
    //}
    //else
//#endif
    {	
        //OS_MemCopy(sep->remote_bd_addr.addr, device_addr->addr, 6);
	 sep->conn_id = BTBMProfileAllocateConnIndex(sep->uuid, sep->remote_bd_addr);
        status = CMGR_CreateDataLink(sep->cmgr_hdlr, &sep->remote_bd_addr);
	 GAVDP(sig_conn_state) = GAVDP_STATE_SIG_CONNECT_OUTGOING;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ_STAT_TRANS);  	 

        if (status == BT_STATUS_PENDING)
        {
            //bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_DATA_LINK;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ_CMGR_PENDING);              
        }
        else if (status == BT_STATUS_SUCCESS)
        {        
        //#ifdef SKIP_SDP_QUERY
            //A2dpConnectAvdtpChannel(device_addr, cmgr_hdlr->bdc->link);
            //// we already send cnf back, so let's set status to be pending 
            //// to skip the following status checking and avoid to send cnf twice 
            //status = BT_STATUS_PENDING;
        //#else
            //status = A2dpSdpQuery();
            //BT_A2DP_VAL_TRACE1(status);
            //if (status == BT_STATUS_PENDING)
            //{
            //    bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_SDP_QUERY;
            //}
            //else
            //{
            //    A2dpCmgrLinkClose();
            //    CMGR_RemoveCmgrWithDevice(&bt_a2dp_ctx.cmgr_hdlr);              
            //}
        //#endif // SKIP_SDP_QUERY 
             bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ_CMGR_SUCCESS);          
             GavdpConnectAvdtpChannel(sep);
        }
        else
        {          
            GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_REQ_CMGR_FAILED);  			
			
            CMGR_RemoveDataLink(sep->cmgr_hdlr);		 
	    BTBMProfileReleaseConnIndex(sep->conn_id);
            sep->conn_id = 0xFF;            
            CMGR_RemoveCmgrWithDevice(sep->cmgr_hdlr);

            GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_ESTABLISH_CNF, GAVDP_ERR_FATAL);				
        } 
    }
    
    return;
}


//void A2DP_StreamOpenRes(U16 result, U8 stream_handle)
void GAVDP_EstablishConnectionRsp(StreamEndPoint* sep, U16 result)//AVDTP_ERR_NO_ERROR is acception
{
    U8 accept;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_RSP, sep, result);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_RSP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state); 	
    
    GAVDP(sig_conn_state) = GAVDP_STATE_SIG_IDLE;
    sep->stream_state = GAVDP_STATE_IDLE;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_RSP_STAT_TRANS); 	
	
    //TODO: does it need timeout to response the est. conn. ind.? 
    //TODO: does it need to check the command stage again (it is also checked when receiving ind.)?	
    if ( GAVDP_EST_WAIT_IND_OPEN == sep->command_stage)
    {  
        sep->command_stage = GAVDP_WAIT_NONE; 	    
        if (result == AVDTP_ERR_NO_ERROR)
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_RSP_ACPT);         
            sep->stream_state = GAVDP_STATE_OPEN;  
            accept = AVDTP_RSP_ACCEPT;		
        }
        else
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_RSP_REJ);         
            accept = AVDTP_RSP_REJECT;			
        }	
        AVDTP_OpenStreamRsp(accept, &sep->stream, result);
    }
    else
    {  
        sep->command_stage = GAVDP_WAIT_NONE;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_EST_CON_RSP_BAD_STAT); 		
        AVDTP_OpenStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
        //TODO: error handling		
    }
    
}


void GAVDP_StartStreamReq(StreamEndPoint* sep)
{
    BtStatus status;
	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_REQ, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state); 	
    
    if( BtSysAdpVerifyInPanicState() )
    {
        //if(bt_a2dp_ctx.fm_over_bt_mode == 1){	bt_a2dp_ctx.fm_over_bt_mode = 0;}
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_START_CNF, GAVDP_ERR_PANIC);		
        return;
    }

    
    //if (bt_a2dp_ctx.wait_res == BT_A2DP_WAIT_RES_START)
    //{
    //    // this is the case that start_req comes between start_ind and start_res 
    //    bt_a2dp_ctx.wait_res = BT_A2DP_WAIT_RES_NONE;
    //    AVDTP_StartStreamRsp(AVDTP_RSP_ACCEPT, &bt_a2dp_ctx.stream, AVDTP_ERR_NO_ERROR);
    //    status = BT_STATUS_PENDING;
    //}
    //else
    //{
        if(  GAVDP_STATE_OPEN != sep->stream_state || GAVDP_WAIT_NONE!= sep->command_stage)
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_REQ_BAD_STAT);         
            //if(bt_a2dp_ctx.fm_over_bt_mode == 1){bt_a2dp_ctx.fm_over_bt_mode = 0;}
            GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_START_CNF, AVDTP_ERR_BAD_STATE);			
            return;
        }
/*
        // Pierre: move FM over BT start to here
        U16 l2cap_mtu = 0;
        U16 l2cap_cid = 0;
        U16 AVDTP_sig_sid = 0;
        U16 AVDTP_sig_cid = 0;
        U16 features = 0x0;

        //A2dpHandleStreamStartCnf(result, bt_a2dp_ctx.stream.locStrmId, A2DP_GetPreferredPayloadSize(bt_a2dp_ctx.stream.locStrmId));
        if((bt_a2dp_ctx.fm_over_sbc_enabled ==0)&&(bt_a2dp_ctx.fm_over_bt_mode==1))
        {
            Report(("A2DP_GetPreferredPayloadSize start"));
            l2cap_mtu = A2DP_GetPreferredPayloadSize(bt_a2dp_ctx.stream.locStrmId);
            Report(("A2DP_GetPreferredPayloadSize end"));
            AVDTP_sig_sid = bt_a2dp_ctx.signal_channel.conn.l2ChannelId;
            Report(("L2CAP_GetL2CAPRemoteCid start"));
            AVDTP_sig_cid = L2CAP_GetL2CAPRemoteCid(AVDTP_sig_sid);
            Report(("L2CAP_GetL2CAPRemoteCid end"));
            Report(("AVDTP_MediaGetChannelId start"));
            l2cap_cid = AVDTP_MediaGetChannelId(bt_a2dp_ctx.stream);
            Report(("AVDTP_MediaGetChannelId end"));
            // change to remote
            Report(("L2CAP_GetL2CAPRemoteCid start"));
            l2cap_cid = L2CAP_GetL2CAPRemoteCid(l2cap_cid);
            Report(("L2CAP_GetL2CAPRemoteCid end"));
            //AVDTP_sig_cid = bt_a2dp_ctx.signal_channel.sigChnl.rxId;
            bt_a2dp_ctx.fm_over_sbc_enabled = 1;
            Report(("MeWriteEnableFMoverSBC start"));

            setCondecElements();
            MeWriteEnableFMoverSBC(bt_a2dp_ctx.cmgr_hdlr.remDev, 
                                    l2cap_mtu,
                                    AVDTP_sig_sid,
                                    AVDTP_sig_cid,
                                    l2cap_cid,
                                    bt_a2dp_ctx.codec.elements,
                                    features);
            Report(("MeWriteEnableFMoverSBC end"));
       }
*/      
       status = AVDTP_StartStream(&sep->stream);       
    //}
	
    if (status == BT_STATUS_PENDING)
    {        		    
        sep->command_stage = GAVDP_WAIT_CNF_START;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_REQ_PENDING); 		
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_REQ_FAILED);     
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_START_CNF, AVDTP_ERR_BAD_STATE);		
    }
    
}


void GAVDP_StartStreamRsp(StreamEndPoint* sep, U16 result)
{
    U8 accept;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_RSP, sep, result);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_RSP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state); 	
    
    if ( GAVDP_STATE_OPEN == sep->stream_state && GAVDP_WAIT_RES_START == sep->command_stage )
    {
        sep->command_stage = GAVDP_WAIT_NONE;
        if ( result == AVDTP_ERR_NO_ERROR )
        {            
            accept = AVDTP_RSP_ACCEPT;
            sep->stream_state = GAVDP_STATE_STREAMING;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_RSP_ACPT); 			
        }
        else
        {                 
            accept = AVDTP_RSP_REJECT;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_RSP_REJ); 			
        }
/*
        //Pierre
        //AVDTP_StartStreamRsp(accept, &bt_a2dp_ctx.stream, (AvdtpError) result);
        if(result == BT_A2DP_RESULT_OK)
        {
            U16 l2cap_mtu = 0;
            U16 l2cap_cid = 0;
            U16 AVDTP_sig_sid = 0;
            U16 AVDTP_sig_cid = 0;
            U16 features = 0x0;

            if(bt_a2dp_ctx.fm_over_sbc_enabled ==0)
            {
                bt_a2dp_ctx.fm_over_sbc_enabled = 1;
                l2cap_mtu = A2DP_GetPreferredPayloadSize(bt_a2dp_ctx.stream.locStrmId);
                //AVDTP_sig_cid = bt_a2dp_ctx.signal_channel.sigChnl.rxId; // 0x7
                AVDTP_sig_sid = bt_a2dp_ctx.signal_channel.conn.l2ChannelId;
                AVDTP_sig_cid = L2CAP_GetL2CAPRemoteCid(AVDTP_sig_sid);
                l2cap_cid = AVDTP_MediaGetChannelId(bt_a2dp_ctx.stream);
                // change to remote
                l2cap_cid = L2CAP_GetL2CAPRemoteCid(l2cap_cid);
                setCondecElements();
                MeWriteEnableFMoverSBC(bt_a2dp_ctx.cmgr_hdlr.remDev, 
                                        l2cap_mtu,
                                        AVDTP_sig_sid,
                                        AVDTP_sig_cid,
                                        l2cap_cid,
                                        bt_a2dp_ctx.codec.elements,
                                        features);
            }
        }
        // Pierre move to here
*/        
        AVDTP_StartStreamRsp(accept, &sep->stream, (AvdtpError) result);		
    }
    else
    {
        sep->command_stage = GAVDP_WAIT_NONE;    
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_START_STRM_RSP_BAD_STAT); 		
        AVDTP_StartStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
        //TODO: error handling		
    }
    
}


void GAVDP_SuspendStreamReq(StreamEndPoint* sep)	
{
    BtStatus status;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_REQ, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state); 	
    
    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_SUSPEND_CNF, GAVDP_ERR_PANIC);		
        return;
    }

    //if (bt_a2dp_ctx.wait_res == BT_A2DP_WAIT_RES_SUSPEND)
    //{
    //    // this is the case suspend_req comes between suspend_ind and suspend_res 
    //    bt_a2dp_ctx.wait_res = BT_A2DP_WAIT_RES_NONE;
    //    AVDTP_SuspendStreamRsp(AVDTP_RSP_ACCEPT, &bt_a2dp_ctx.stream, AVDTP_ERR_NO_ERROR);
    //    status = BT_STATUS_PENDING;
    //}
    //else
    //{
        if(  GAVDP_STATE_STREAMING != sep->stream_state || GAVDP_WAIT_NONE != sep->command_stage)
        {
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_REQ_BAD_STAT);         
            GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_SUSPEND_CNF, AVDTP_ERR_BAD_STATE);				
            return;
        }
	
        //if (!(bt_a2dp_ctx.state == BT_A2DP_STATE_STREAMING &&
        //      bt_a2dp_ctx.wait_cnf == BT_A2DP_WAIT_CNF_NONE && bt_a2dp_ctx.wait_res <= BT_A2DP_WAIT_RES_GET_CAP))
        //{
            // check if we need to postponse suspend until stream is started
        //    if (bt_a2dp_ctx.wait_started && bt_a2dp_ctx.state == BT_A2DP_STATE_OPEN &&
        //      bt_a2dp_ctx.wait_cnf == BT_A2DP_WAIT_CNF_NONE && bt_a2dp_ctx.wait_res <= BT_A2DP_WAIT_RES_GET_CAP)
        //    {
                // this is the case that suspend_req comes between start_res and started callback 
        //        bt_a2dp_ctx.postpond_suspend = TRUE;
        //    }
        //    else
        //    {
        //       A2dpHandleStreamSuspendCnf( BT_A2DP_RESULT_BAD_STATE, stream_handle);
        //    }
        //    return;
        //}
        

        status = AVDTP_SuspendStream(&sep->stream);        
    //}

    if (status == BT_STATUS_PENDING)
    {
        sep->command_stage = GAVDP_WAIT_CNF_SUSPEND;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_REQ_PENDING); 		
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_REQ_FAILED);     
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_SUSPEND_CNF, AVDTP_ERR_BAD_STATE);		        		
    }
}


void GAVDP_SuspendStreamRsp(StreamEndPoint* sep, U16 result)
{
    U8 accept;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_RSP, sep, result);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_RSP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);	

    if ( GAVDP_STATE_STREAMING == sep->stream_state && GAVDP_WAIT_RES_SUSPEND== sep->command_stage )
    {
        sep->command_stage = GAVDP_WAIT_NONE; 

        if (result == AVDTP_ERR_NO_ERROR)
        {
            accept = AVDTP_RSP_ACCEPT;
            sep->stream_state = GAVDP_STATE_OPEN;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_RSP_ACPT);			
        }
        else
        {
            accept = AVDTP_RSP_REJECT;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_RSP_REJ);			
        }
        AVDTP_SuspendStreamRsp(accept, &sep->stream, (AvdtpError) result);        	
    }
    else
    {
        sep->command_stage = GAVDP_WAIT_NONE;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SUSPEND_STRM_RSP_BAD_STAT);		
        AVDTP_SuspendStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);		
	 //TODO: error handling
    }	
}


void GAVDP_ChangeParametersReq(StreamEndPoint* sep)
{
    BtStatus status;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_REQ, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);	
    
    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_CNF, GAVDP_ERR_PANIC);			
        return;
    }


    if(  GAVDP_STATE_OPEN != sep->stream_state || GAVDP_WAIT_NONE != sep->command_stage)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_REQ_BAD_STAT);    
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_CNF, AVDTP_ERR_BAD_STATE);		
        return;
    }

    status = AVDTP_Reconfig(&sep->stream, &sep->local_cfg_codec);	

    if (status == BT_STATUS_PENDING)
    {        
        sep->command_stage = GAVDP_WAIT_CNF_RECONFIG;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_REQ_PENDING);		
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_REQ_FAILED);    
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_CHANGE_PARAMETERS_CNF, AVDTP_ERR_BAD_STATE);		
    }	
}


void GAVDP_ChangeParametersRsp(StreamEndPoint* sep, U16 result)
{
    U8 accept;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_RSP, sep, result);	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_RSP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state );		

    if ( GAVDP_STATE_OPEN == sep->stream_state && GAVDP_WAIT_RES_RECONFIG== sep->command_stage )
    {
        sep->command_stage = GAVDP_WAIT_NONE; 
		
        if (result == AVDTP_ERR_NO_ERROR)
        {
            accept = AVDTP_RSP_ACCEPT;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_RSP_ACPT);				
	    //TODO:		
            //memcpy(&bt_a2dp_ctx.cur_config, &bt_a2dp_ctx.remote_req_config, sizeof(bt_a2dp_audio_cap_struct));
            //inform bt_a2dp_android.c configuration change
            //A2dpHandleConfigChange(&bt_a2dp_ctx.cur_config);
        }
        else
        {
            accept = AVDTP_RSP_REJECT;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_RSP_REJ);				
        }
        AVDTP_ReconfigRsp(accept, &sep->stream, (U8) result, sep->codec.codecType);
    }
    else
    {
        sep->command_stage = GAVDP_WAIT_NONE;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_CHG_PARA_RSP_BAD_STAT);			
        AVDTP_ReconfigRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE, sep->codec.codecType);	
	 //error handling
    }		
}


void GAVDP_AbortReq(StreamEndPoint* sep)
{
    BtStatus status;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_REQ, sep);	
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);			

    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_ABORT_CNF, GAVDP_ERR_PANIC);		
        return;
    }

    if (GAVDP_WAIT_NONE != sep->command_stage && GAVDP_WAIT_CNF_DISCONNECT >= sep->command_stage) 	    
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_REQ_BAD_STAT);    
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_ABORT_CNF, AVDTP_ERR_BAD_STATE);		
        return;
    }

    // cancel outstanding ind TODO: 
    if (  GAVDP_EST_WAIT_IND_OPEN <= sep->command_stage)
    {
        //ASSERT(wait_res >= BT_A2DP_WAIT_RES_OPEN);
        switch (sep->command_stage)
        {
             //TODO: TBD to this case	        
            case GAVDP_EST_WAIT_IND_OPEN:
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_OpenStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
                break;
				
            case GAVDP_WAIT_RES_RECONFIG:				
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_ReconfigRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE, sep->codec.codecType);
                break;

            case GAVDP_WAIT_RES_START:				
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_StartStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
                break;
				
            case GAVDP_WAIT_RES_SUSPEND:
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_SuspendStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
                break;

             //TODO: TBD to this case					
            case  GAVDP_WAIT_RES_CLOSE:
                sep->command_stage = GAVDP_WAIT_NONE;				
                AVDTP_CloseStreamRsp(AVDTP_RSP_ACCEPT, &sep->stream, AVDTP_ERR_NO_ERROR);
                return;
				
        }
    }


    status = AVDTP_AbortStream(&sep->stream);
    if (status == BT_STATUS_PENDING)
    {
        sep->command_stage = GAVDP_WAIT_CNF_ABORT;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_REQ_PENDING);
    }
    else
    {  
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_REQ_FAILED);    
	GavdpGavdpEventCallback(sep, GAVDP_EVENT_ABORT_CNF, AVDTP_ERR_BAD_STATE);	
    }	
}


void GAVDP_AbortRsp(StreamEndPoint* sep)
{
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_RSP, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_RSP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state); 	

    if (GAVDP_WAIT_RES_ABORT == sep->command_stage )
    {      
        //sep->stream_state = GAVDP_STATE_IDLE;
        if ( sep->stream_state != GAVDP_STATE_IDLE )
        {
            sep->stream_state = GAVDP_STATE_ABORTED;
        }
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_ABORT_RSP_ACPT);		
        AVDTP_AbortStreamRsp(&sep->stream);
    }
	
    sep->command_stage= GAVDP_WAIT_NONE;

    AVDTP_Disconnect(sep->signal_channel);	
}


void GAVDP_ReleaseConnectionReq(StreamEndPoint* sep)
{
    BtStatus status;

    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_REQ, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, GAVDP_ERR_PANIC);		
        return;
    }
  
    if ( GAVDP_STATE_OPEN > sep->stream_state || 
		(GAVDP_WAIT_NONE != sep->command_stage && GAVDP_WAIT_CNF_DISCONNECT >= sep->command_stage) )
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_REQ_BAD_STAT);    
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, AVDTP_ERR_BAD_STATE);		
        return;
    }

    //TODO: cancel outstanding ind
    if (  GAVDP_EST_WAIT_IND_OPEN < sep->command_stage)
    {
        switch (sep->command_stage)
        {			
            case GAVDP_WAIT_RES_RECONFIG:				
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_ReconfigRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE, sep->codec.codecType);
                break;

            case GAVDP_WAIT_RES_START:				
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_StartStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
                break;
				
            case GAVDP_WAIT_RES_SUSPEND:
                sep->command_stage = GAVDP_WAIT_NONE;
                AVDTP_SuspendStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);
                break;
            //TODO: TBD to this case					
            case  GAVDP_WAIT_RES_CLOSE:
                sep->command_stage = GAVDP_WAIT_NONE;				
                AVDTP_CloseStreamRsp(AVDTP_RSP_ACCEPT, &sep->stream, AVDTP_ERR_NO_ERROR);
                return;     			
        }
    }

    status = AVDTP_CloseStream(&sep->stream);

    if (status == BT_STATUS_PENDING)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_REQ_PENDING);    
        sep->command_stage = GAVDP_WAIT_CNF_CLOSE;
    }
    else
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_REQ_FAILED);    
        GavdpGavdpEventCallback(sep, GAVDP_EVENT_STREAM_RELEASE_CNF, AVDTP_ERR_BAD_STATE);		
    }
}

void GAVDP_ReleaseConnectionRsp(StreamEndPoint* sep, U16 result)
{
    U8 accept;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_RSP, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_RSP_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);	
    
    if ( GAVDP_STATE_IDLE < sep->stream_state && GAVDP_WAIT_RES_CLOSE == sep->command_stage )
    {
        sep->command_stage = GAVDP_WAIT_NONE; 
		
        if (result == AVDTP_ERR_NO_ERROR)
        {
            accept = AVDTP_RSP_ACCEPT;
            sep->stream_state = GAVDP_STATE_IDLE;
            GAVDP(sig_conn_state) = GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTING;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_RSP_ACPT);			
        }
        else
        {        
            accept = AVDTP_RSP_REJECT;
            bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_RSP_REJ);			
        }
        AVDTP_CloseStreamRsp(accept, &sep->stream, (U8) result);
    }
    else
    {
        sep->command_stage = GAVDP_WAIT_NONE;
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_REL_CON_RSP_BAD_STAT);		
        AVDTP_CloseStreamRsp(AVDTP_RSP_REJECT, &sep->stream, AVDTP_ERR_BAD_STATE);	
	 //error handling
    }				
}	


GavdpError GAVDP_StreamSendDataReq(StreamEndPoint* sep, GavdpRtpTag* rtp ,U8 *buffer, U32 buffer_size)
{
    // Subtract offset of media header to get the starting address of the packet buffer.
    U8 *packet_buffer = buffer - 12; 
    AvdtpMediaHeader media_header;
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEND_DATA_REQ, sep);
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEND_DATA_REQ_STAT, GAVDP(sig_conn_state), sep->command_stage,  sep->stream_state);	
    
    // This could happen if there is no any available payload to be sent.
    // We should free allocated memory automatically and return an error
    // to upper application.
    if (buffer_size == 0)
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEND_DATA_REQ_BAD_LEN);    
        //bt_a2dp_mem_free( packet_buffer );
        return GAVDP_ERR_BAD_LENGTH;
    }
    
    // Send stream data to BT stack
    media_header.version = 2;
    media_header.marker = 0;
    media_header.padding = 0;
    media_header.payloadType = 96;
    media_header.csrcCount = 0;
    media_header.ssrc = rtp->stream_handle;        
    media_header.sequenceNumber = rtp->data_seq_no;
    media_header.timestamp = rtp->data_timestamp;

    AVDTP_CreateMediaHeader(&media_header, packet_buffer);
    if (!AVDTP_SendStreamData(&sep->stream, buffer_size + 12, packet_buffer))
    {
        bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEND_DATA_REQ_FATAL);    
        //bt_a2dp_mem_free( packet_buffer );
        return GAVDP_ERR_FATAL;
    }
    bt_trace(BT_TRACE_G7_GAVDP, BT_GAVDP_SEND_DATA_REQ_SUCCESS);
    HCI_Process();
	
    
    return  GAVDP_ERR_NO_ERR;
}

