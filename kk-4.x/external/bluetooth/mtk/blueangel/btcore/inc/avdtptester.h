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

#include "btconfig.h"
#ifdef __AVDTP_TESTER_
#include "conmgr.h"
#include "avdtp.h"

#define AVDTP_TESTER_SBC_CODEC_INF_ELEMENT(f, c, b, s, m, a, i)  \
                                  (U8)((f) | (c)),               \
                                  (U8)((b) | (s) | (m)),         \
                                  (U8)(a),                       \
                                  (U8)(i)

#define AVDTP_TESTER_SBC_CODEC_FREQ_16000       0x80
#define AVDTP_TESTER_SBC_CODEC_FREQ_32000       0x40
#define AVDTP_TESTER_SBC_CODEC_FREQ_44100       0x20
#define AVDTP_TESTER_SBC_CODEC_FREQ_48000       0x10
#define AVDTP_TESTER_SBC_CODEC_CHNL_MODE_MONO   0x08
#define AVDTP_TESTER_SBC_CODEC_CHNL_MODE_DUAL   0x04
#define AVDTP_TESTER_SBC_CODEC_CHNL_MODE_STEREO 0x02
#define AVDTP_TESTER_SBC_CODEC_CHNL_MODE_JOINT  0x01
#define AVDTP_TESTER_SBC_CODEC_BLOCKS_4            0x80
#define AVDTP_TESTER_SBC_CODEC_BLOCKS_8            0x40
#define AVDTP_TESTER_SBC_CODEC_BLOCKS_12           0x20
#define AVDTP_TESTER_SBC_CODEC_BLOCKS_16           0x10
#define AVDTP_TESTER_SBC_CODEC_SUBBANDS_4          0x08
#define AVDTP_TESTER_SBC_CODEC_SUBBANDS_8          0x04
#define AVDTP_TESTER_SBC_CODEC_ALLOCATION_SNR      0x02
#define AVDTP_TESTER_SBC_CODEC_ALLOCATION_LOUDNESS 0x01

#define AVDTP_TESTER_DEV_FLAG_OUTGOING      0x01

#define AVDTP_TESTER_STRM_FLAG_DISCOVERY    0x02
#define AVDTP_TESTER_STRM_FLAG_CLOSE_ACP    0x04
#define AVDTP_TESTER_STRM_FLAG_QUERY_CODEC  0x08
#define AVDTP_TESTER_STRM_FLAG_QUERY_CP     0x10
#define AVDTP_TESTER_STRM_FLAG_GET_CAP      0x20
#define AVDTP_TESTER_STRM_FLAG_IDLE         0x40

#define AVDTP_TESTER_STREAM_TYPE_SOURCE  0

/* The stream is a sink */
#define AVDTP_TESTER_STREAM_TYPE_SINK    1

typedef struct _AvdtpTesterStream AvdtpTesterStream;

/* Contains information about the remote device */
typedef struct _AvdtpTesterDevice
{
    ListEntry node;                 /* Used internally by A2DP. */
    AvdtpChannel channel;           /* AVDTP channel */
    AvdtpTesterStream *currentStream;
    /* SDP variables for client */
    CmgrHandler cmgrHandler;        /* Used for SDP connection */
    SdpQueryToken sdpQueryToken;    /* SDP query token */
    U8 flags;
    U8 bd_addr[6];
} AvdtpTesterDevice;

struct _AvdtpTesterStream
{
    ListEntry node;                 /* Used internally by A2DP. */
    AvdtpStream stream;             /* Stream structure used by this source */
    ListEntry sbcQueue;             /* Transmit queue for SBC data */
    AvdtpMediaHeader mediaHeader;   /* Media header for SBC packets */
    U32 timeInc;                    /* Time stamp increment */
    U8 flags;                       /* State flags */
    EvmTimer closeTimer;            /* Closes stream when open never completes */
    U8 type;                        /* Stream endpoint type */
    AvdtpTesterDevice *device;      /* The device associated with the stream */
    AvdtpStreamInfo currentInfo;
};

typedef struct _BtAvdtpTesterContext
{
    /* Device Pool */
    ListEntry freeDeviceList;
    ListEntry inuseDeviceList;
    ListEntry streamList;
    U8 activeStreamCount;
    BOOL avdtpIssueCmd;
    AvdtpTesterDevice device[1];
    AvdtpTesterDevice *currentDevice;
    AvdtpConfigRequest currentCfgReq;
    U8 avdtpTxRxDataBuffer[150];
    U8 state;
    U16 buffer_offset;
} BtAvdtpTesterContext;

typedef struct _AvdtpTesterPacket
{
    ListEntry node; /* Used internally by A2DP  */

    U8 *data;       /* Pointer to transmit data */
    U16 dataLen;    /* Length of transmit data  */
    U16 frameSize;  /* Size of an SBC frame     */

    /* === Internal use only === */
    BtPacket packet;    /* For sending over L2CAP   */
    U16 dataSent;
    U16 frmDataSent;
} AvdtpTesterPacket;

typedef struct _AvdtpConnectRequest
{
    U8 bd_addr[6];
} AvdtpConnectRequest;

typedef struct _AvdtpStreamDiscoverRequest
{
    U8 transaction;
    U8 bd_addr[6];
} AvdtpStreamDiscoverRequest;

typedef struct _AvdtpStreamGetCapabilitiesRequest
{
    U8 transaction;
    U8 bd_addr[6];
    U8 acp_seid;
} AvdtpStreamGetCapabilitiesRequest;

typedef struct _AvdtpStreamSetConfigurationRequest
{
    U8 transaction;
    U8 bd_addr[6];
    U8 acp_seid;
    U8 int_seid;
    U8 codecType;
    U8 element_len;
    U8 element[30];
} AvdtpStreamSetConfigurationRequest;

typedef struct _AvdtpStreamReConfigurationRequest
{
    U8 transaction;
    U16 stream_handle;
    U8 codecType;
    U8 element_len;
    U8 element[30];
} AvdtpStreamReConfigurationRequest;

typedef struct _AvdtpStreamOpenRequest
{
    U8 transaction;
    U16 stream_handle;
} AvdtpStreamOpenRequest;

typedef struct _AvdtpStreamStartRequest
{
    U8 transaction;
    U16 stream_handle[50];
} AvdtpStreamStartRequest;

typedef struct _AvdtpStreamCloseRequest
{
    U8 transaction;
    U16 stream_handle;
} AvdtpStreamCloseRequest;

typedef struct _AvdtpStreamSuspendRequest
{
    U8 transaction;
    U16 stream_handle[50];
} AvdtpStreamSuspendRequest;

typedef struct _AvdtpStreamAbortRequest
{
    U8 transaction;
    U16 stream_handle;
} AvdtpStreamAbortRequest;

typedef struct _AvdtpStreamDiscoverResponse
{
    U8 transaction;
    U8 bd_addr[6];
    U8 stream_info_num;
    AvdtpStreamInfo stream_info[20];
    U8 error_code;
} AvdtpstreamdiscoverResponse;

typedef struct _AvdtpStreamGetCapabilitiesResponse
{
    U8 transaction;
    U8 bd_addr[6];
    U8 capabilities_len; 
    U8 capabilities[50];
    U8 error_code;
    U8 seid;
} AvdtpStreamGetCapabilitiesResponse;

typedef struct _AvdtpStreamSetConfigurationResponse
{
    U8 transaction;
    U16 stream_handle;
    U8 category[5];
    U8 error_code;
} AvdtpStreamSetConfigurationResponse;

typedef struct _AvdtpStreamReConfigurationResponse
{
    U8 transaction;
    U16 stream_handle;
    U8 category[5];
    U8 error_code;
} AvdtpStreamReConfigurationResponse;

typedef struct _AvdtpStreamOpenResponse
{
    U8 transaction;
    U16 stream_handle;
    U8 error_code;
} AvdtpStreamOpenResponse;

typedef struct _AvdtpStreamStartResponse
{
    U8 transaction;
    U16 stream_handle;
    U8 error_code;
} AvdtpStreamStartResponse;

typedef struct _AvdtpStreamCloseResponse
{
    U8 transaction;
    U16 stream_handle;
    U8 error_code;
} AvdtpStreamCloseResponse;

typedef struct _AvdtpStreamSuspendResponse
{
    U8 transaction;
    U16 stream_handle;
    U16 first_failed_stream_handle;
    U8 error_code;
} AvdtpStreamSuspendResponse;

typedef struct _AvdtpStreamAbortResponse
{
    U8 transaction;
    U16 stream_handle;
    U8 error_code;
} AvdtpStreamAbortResponse;

typedef struct _AvdtpWriteDataReq
{
    U16 stream_handle;
    U16 length;
    U8 outbuffer[20];
    U32 time_info;
    U8 payload_type;
    U16 marker;
} AvdtpWriteDataReq;

typedef struct _AvdtpReadDataRequestResult
{
    U8 result;
    U16 stream_handle;
    U16 length;
    U8 inbuffer[20];
    U32 time_info;
    U8 payload_type;
    U16 marker;
    U8 reliability;

} AvdtpReadDataRequestResult;

typedef struct _AvdtpReadDataReq
{
    U16 stream_handle;
    U16 expected_length;
    U32 buffer;
} AvdtpReadDataReq;

typedef struct _AvdtpStreamDiscoverRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamDiscoverRequestResult;

typedef struct _AvdtpStreamGetCapabilitiesRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamGetCapabilitiesRequestResult;

typedef struct _AvdtpStreamSetConfigurationRequestResult
{
    U8 result;
    U8 transaction;
    U16 stream_handle;
} AvdtpStreamSetConfigurationRequestResult;

typedef struct _AvdtpStreamReConfigurationRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamReConfigurationRequestResult;

typedef struct _AvdtpStreamOpenRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamOpenRequestResult;

typedef struct _AvdtpStreamStartRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamStartRequestResult;

typedef struct _AvdtpStreamCloseRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamCloseRequestResult;

typedef struct _AvdtpStreamSuspendRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamSuspendRequestResult;

typedef struct _AvdtpStreamAbortRequestResult
{
    U8 result;
    U8 transaction;
} AvdtpStreamAbortRequestResult;

typedef struct _AvdtpStreamDataRequestResult
{
    U8 result;
} AvdtpStreamDataRequestResult;

extern void Send_AVDTP_Event_Init(U8 register_result);
extern void Send_AVDTP_Event_Registration_Req_Result(U8 register_result);
void AvdtpTesterInit();
U8 AVDT_Discover_Req(AvdtpStreamDiscoverRequest *parms);
U8 AVDT_Get_Capabilities_Req(AvdtpStreamGetCapabilitiesRequest *parms);
U8 AVDT_Set_Configuration_Req(AvdtpStreamSetConfigurationRequest *parms);
U8 AVDT_ReConfigure_Req(AvdtpStreamReConfigurationRequest *parms);
U8 AVDT_Open_Req(AvdtpStreamOpenRequest *parms);
U8 AVDT_Close_Req(AvdtpStreamCloseRequest *parms);
U8 AVDT_Start_Req(AvdtpStreamStartRequest *parms);
U8 AVDT_Suspend_Req(AvdtpStreamSuspendRequest *parms);
U8 AVDT_Abort_Req(AvdtpStreamAbortRequest *parms);

U8 AVDT_Discover_Rsp(AvdtpstreamdiscoverResponse *parms);
U8 AVDT_Get_Capabilities_Rsp(AvdtpStreamGetCapabilitiesResponse *parms);
U8 AVDT_Set_Configuration_Rsp(AvdtpStreamSetConfigurationResponse *parms);
U8 AVDT_Re_Configuration_Rsp(AvdtpStreamReConfigurationResponse *parms);
U8 AVDT_Open_Rsp(AvdtpStreamOpenResponse *parms);
U8 AVDT_Close_Rsp(AvdtpStreamCloseResponse *parms);
U8 AVDT_Start_Rsp(AvdtpStreamStartResponse *parms);
U8 AVDT_Suspend_Rsp(AvdtpStreamSuspendResponse *parms);
U8 AVDT_Abort_Rsp(AvdtpStreamAbortResponse *parms);
U8 AVDT_DisconnectSignal_Req(void);
void AVDT_HandleRxData(U8 *data, U16 length);
void AVDT_Read_Data(AvdtpReadDataReq *parms);
U8 AVDT_Write_Data(AvdtpWriteDataReq *parms);
void AvdtpTesterCallback(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms);
BtStatus AvdtpTesterCreateAclLink(BD_ADDR *Addr, CmgrHandler *Handler, CmgrCallback Callback);
void AvdtpTesterCmgrCallback(CmgrHandler *cHandler, CmgrEvent Event, BtStatus Status);
#endif /* __AVDTP_TESTER_ */ 

