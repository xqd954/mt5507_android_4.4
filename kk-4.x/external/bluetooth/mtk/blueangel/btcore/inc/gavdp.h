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

#ifndef __GAVDP_H_
#define __GAVDP_H_

#include "avdtp.h"

#include "conmgr.h"
#include "me_adp.h"

#include "utils.h"
#include "bluetooth_struct.h"
#include "bttypes.h"
#include "bt_os_api.h"

#include "bt_common.h"
#include "btconfig.h"
#include "sec.h"
#include "sys/avalloc.h"
#include "l2cap.h"
#include "sys/avsigmgr.h"
#include "sys/avdtpcon.h"
#include "bt_feature.h"

#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"
#endif

//-----------------------------------------------------------------------------

#ifdef MAX_NUM_REMOTE_SEIDS
#undef MAX_NUM_REMOTE_SEIDS
#endif
#define MAX_NUM_REMOTE_SEIDS  8
#define MAX_NUM_REMOTE_CAPS  4


#define GAVDP_STATE_INACTIVATED 0x11
#define GAVDP_STATE_INITIALIZED 0x12
#define GAVDP_STATE_DEINITIALIZING 0x13



#define GAVDP_STATE_SIG_IDLE 0x01
#define GAVDP_STATE_SIG_CONNECT_INCOMING 0x02
#define GAVDP_STATE_SIG_CONNECT_OUTGOING 0x03
#define GAVDP_STATE_SIG_STRM_IN_CONNECTING 0x04
#define GAVDP_STATE_SIG_STRM_OUT_CONNECTING 0x05

//#define GAVDP_STATE_SIG_CONNECTED 0x05
#define GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTING 0x06
#define GAVDP_STATE_SIG_PASSIVE_STRM_DISCONNECTED 0x07

#define GAVDP_STATE_IDLE 0x08
#define GAVDP_STATE_OPEN 0x09
#define GAVDP_STATE_STREAMING 0x0a
#define GAVDP_STATE_ABORTED 0x0b
#define GAVDP_STATE_STRM_DEREGING 0x0c


#define GAVDP_WAIT_NONE 0x00
#define GAVDP_EST_WAIT_CNF_DISCOVER 0x01
#define GAVDP_EST_WAIT_CNF_GET_CAP 0x02
#define GAVDP_EST_WAIT_CNF_CONFIG 0x03
#define GAVDP_EST_WAIT_CNF_OPEN 0x04
#define GAVDP_WAIT_CNF_RECONFIG 0x15
#define GAVDP_WAIT_CNF_START 0x11
#define GAVDP_WAIT_CNF_SUSPEND 0x12
#define GAVDP_WAIT_CNF_CLOSE 0x13
#define GAVDP_WAIT_CNF_ABORT 0x14
#define GAVDP_WAIT_CNF_DISCONNECT 0x15

#define GAVDP_EST_WAIT_IND_DISCOVER 0x21
#define GAVDP_EST_WAIT_IND_GET_CAP 0x22
#define GAVDP_EST_WAIT_IND_CONFIG 0x23
#define GAVDP_EST_WAIT_IND_OPEN 0x24 
#define GAVDP_WAIT_RES_RECONFIG 0x31
#define GAVDP_WAIT_RES_START 0x32
#define GAVDP_WAIT_RES_SUSPEND 0x33
#define GAVDP_WAIT_RES_CLOSE 0x34
#define GAVDP_WAIT_RES_ABORT 0x35


typedef U8 GavdpEvent;
//#define GAVDP_EVENT_SIG_CONNECT_IND 0x01
//#define GAVDP_EVENT_SIG_CONNECT 0x02
//If the device is the initiator, this event is received after calling AVDTP_Connect().  
//If the device is the acceptor, this event is received after calling AVDTP_ConnectRsp().
#define GAVDP_EVENT_SIG_DISCONNECTED 0x03

#define GAVDP_EVENT_STREAM_ESTABLISH_CNF 0x04
#define GAVDP_EVENT_STREAM_ESTABLISH_IND 0x05

#define GAVDP_EVENT_STREAM_START_CNF 0x06
#define GAVDP_EVENT_STREAM_START_IND 0x07

#define GAVDP_EVENT_CHANGE_PARAMETERS_CNF 0x08
#define GAVDP_EVENT_CHANGE_PARAMETERS_IND 0x09

#define GAVDP_EVENT_STREAM_SUSPEND_CNF 0x0a
#define GAVDP_EVENT_STREAM_SUSPEND_IND 0x0b

#define GAVDP_EVENT_STREAM_RELEASE_CNF 0x0c
#define GAVDP_EVENT_STREAM_RELEASE_IND 0x0d

#define GAVDP_EVENT_ABORT_CNF 0x0e
#define GAVDP_EVENT_ABORT_IND 0x0f

#define GAVDP_EVENT_STREAM_PACKET_SENT 0x10
#define GAVDP_EVENT_STREAM_PACKET_RECV 0x11

#define GAVDP_EVENT_SIG_ESTABLISHED 0x12


typedef U8 GavdpError;
#define GAVDP_ERR_NO_ERR 0x00
#define GAVDP_ERR_ALREADY_INIT 0xf7
#define GAVDP_ERR_CONNECTING 0xf8
#define GAVDP_ERR_CONN_IN 0xf9
#define GAVDP_ERR_SAME_DEV 0xfa
#define GAVDP_ERR_NO_RESOURCE 0xfb
#define GAVDP_ERR_BAD_LENGTH 0xfc
#define GAVDP_ERR_BAD_SEP 0xfd
#define GAVDP_ERR_PANIC 0xfe
#define GAVDP_ERR_FATAL 0xff


#define GAVDP_CODEC_TYPE_SBC_FREQ_16K 0x80
#define GAVDP_CODEC_TYPE_SBC_FREQ_32K 0x40
#define GAVDP_CODEC_TYPE_SBC_FREQ_44K 0x20
#define GAVDP_CODEC_TYPE_SBC_FREQ_48K 0x10
#define GAVDP_CODEC_TYPE_SBC_MONO  0x08
#define GAVDP_CODEC_TYPE_SBC_DUAL  0x04
#define GAVDP_CODEC_TYPE_SBC_STEREO  0x02
#define GAVDP_CODEC_TYPE_SBC_JOINT_STEREO  0x01
#define GAVDP_CODEC_TYPE_SBC_BLOCK_4  0x80
#define GAVDP_CODEC_TYPE_SBC_BLOCK_8  0x40
#define GAVDP_CODEC_TYPE_SBC_BLOCK_12  0x20
#define GAVDP_CODEC_TYPE_SBC_BLOCK_16  0x10
#define GAVDP_CODEC_TYPE_SBC_SUB_4  0x08
#define GAVDP_CODEC_TYPE_SBC_SUB_8  0x04
#define GAVDP_CODEC_TYPE_SBC_SNR  0x02
#define GAVDP_CODEC_TYPE_SBC_LOUDNESS  0x01




typedef struct _BtSepContext StreamEndPoint;
typedef struct _BtGavdpContext BtGavdpContext;
typedef struct _GavdpCallbackParms  GavdpCallbackParms;
typedef struct _GavdpRtpTag GavdpRtpTag; 	
typedef struct _BtGavdpSepSwitchPair GavdpSepSwitchPair;

typedef void(*GavdpAppCallback)(StreamEndPoint *Sep, GavdpCallbackParms *Info);

struct _BtSepContext
{
  ListEntry node; 

  U8 stream_state;
  U8 command_stage;
  U8 conn_id;

  U32 uuid; //SC_AUDIO_SOURC, 	 //TODO: add VIDEO SOURCE UUID and VIDEO SINK UUID
  GavdpAppCallback callback;

  AvdtpStreamInfo info;  
  AvdtpStream stream;    
  AvdtpCodec codec;  

  //only this two pointers are managed by GAVDP
  AvdtpChannel *signal_channel;
  CmgrHandler *cmgr_hdlr;
  EvmTimer sig_disc_timer;
  

  BD_ADDR remote_bd_addr;
  U32 remote_sep_count;
  AvdtpStreamInfo remote_sep_info[MAX_NUM_REMOTE_SEIDS];
  U32 remote_cap_count;
  AvdtpCodec remote_cap_info[MAX_NUM_REMOTE_CAPS];
  U32 current_cfg_sep_id;

  U16 prefer_size;
  AvdtpCodec current_cfg_codec;
  AvdtpCodec local_cfg_codec;
  AvdtpCodec remote_cfg_codec;

};

struct _GavdpCallbackParms
{
    GavdpEvent event;
    GavdpError error;    

    StreamEndPoint *sep; 
       
    BtStatus status;
    U16 len;
    U16 rawDataLen;
    U8 *rawData;

    union
    {
        BtRemoteDevice *remDev;
        U8 *data;  
        BtPacket *packet;
    } p;
};

struct _GavdpRtpTag
{
    U32 stream_handle;
    U32 data_timestamp;
    U16 data_seq_no;
};

//------------------------------------------------------------------------------

GavdpError GAVDP_Init(void);
void GAVDP_DeInit(void);

void GAVDP_Register(StreamEndPoint* sep);
BtStatus GAVDP_DeRegister(StreamEndPoint* sep);


void GAVDP_EstablishConnectionReq(StreamEndPoint* sep, BD_ADDR *device_addr);
void GAVDP_StartStreamReq(StreamEndPoint* sep);
void GAVDP_SuspendStreamReq(StreamEndPoint* sep);
void GAVDP_ChangeParametersReq(StreamEndPoint* sep);
void GAVDP_AbortReq(StreamEndPoint* sep);
void GAVDP_ReleaseConnectionReq(StreamEndPoint* sep);
GavdpError GAVDP_StreamSendDataReq(StreamEndPoint* sep, GavdpRtpTag* rtp ,U8 *buffer, U32 buffer_size);


void GAVDP_EstablishConnectionRsp(StreamEndPoint* sep, U16 result);
void GAVDP_StartStreamRsp(StreamEndPoint* sep, U16 result);
void GAVDP_SuspendStreamRsp(StreamEndPoint* sep, U16 result);
void GAVDP_ChangeParametersRsp(StreamEndPoint* sep, U16 result);
void GAVDP_AbortRsp(StreamEndPoint* sep);
void GAVDP_ReleaseConnectionRsp(StreamEndPoint* sep, U16 result);


#endif
