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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
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
 * Bt_a2dp_android.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Tina Shen
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/

#include "string.h"
#include "bt_a2dp_android.h"
#include "bt_ext_debug.h"
#include "bt_a2dp_hdl.h"
#include "bt_a2dp_api.h"
#include "bt_message.h"

#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "bt_ext_debug.h"
#include "cutils/sockets.h"
#include <sys/select.h>
#include "sbc.h"

typedef I8 BtStatus;

#define ENC_DATA_BUFF    1024
#define MAX_OUTGOING_PACKET   5
#define MAX_PAYLOAD_SIZE 700
#define SBC_NULL	0x00000001
#define RESPONSE_TIMEOUT 3000 /* ms */

struct sbc_struct {
	unsigned long flags;

	int rate;
	int channels;

	void *data;
	int size;
	int len;

	void *priv;
};
typedef struct sbc_struct sbc_t;

struct sbc_priv {
	int init;
	SBC_Frame_State frame;
	SBC_Decoder_State dec_state;
	SBC_Encoder_State enc_state;
	short sb_sample[2*16*8];
};

struct prog_input {
	U8 subbands;
	U8 blocks;
	U8 allocate_method;
	U8 bitpool;
	kal_bool joint_stereo;
	U32 bitrate;
	char input_filename[120];
	int stop_frame;
};

typedef struct _BT_A2DP_DATA_CTX
 {
    pthread_mutex_t data_thread_mutex;
    pthread_t data_thread;
	U16 payload_size;
    U8 enc_data[ENC_DATA_BUFF];
    U16 enc_data_len;
    U32 sample_count;   //reset when A2DP_START
    int srvcsock;    // a2dp data service sock (for receiving indication)
    int extsock;           // for a2dp data send request and receive response
    int streamsock;    // for sending a2dp streaming data
    int sbc_interval;
    kal_uint32 last_tx_time;
    bt_a2dp_audio_cap_struct audio_cap;
    //a2dp stream state
    U8 is_streaming;
    U8 is_cleanup;  //free a2dp_data_ctx when receiving stop confirm, if cleanup
    //when receiving disc indication, a2dp_write will return directly
    U8 recv_disc_ind;
    //when sending disc req, a2dp_write will return directly
    U8 sending_disc;
    //a2dp external adaptor socket
    struct sockaddr_un ext_addr;
    socklen_t   ext_addr_len;
    //pcm input format
    int pcm_rate;
    int pcm_channels;
    //sbc param
    U8 channel_mode;
    U8 subbands;
    U8 blocks;
    U8 allocate_method;
    U8 bitpool;
    //kal_int16 channels;
    kal_int16 pcm_frame_len;
    U32 bitrate;
    U32 fs;
    struct sbc_struct sbc;
	U8 wifi_connected;
	U8 start_ind;
	U8 setSbcDone;
}BT_A2DP_DATA_CTX;

static kal_bool a2dp_startEventLoop(BT_A2DP_DATA_CTX *bt_a2dp_data);
static void* a2dpdata_eventLoopMain(void *ptr);
static U16 a2dp_enc_data(BT_A2DP_DATA_CTX *bt_a2dp_data, U8 stream_handle, const U8 *source, U16 src_len);
static void btmtk_a2dp_send_stream_data_req(BT_A2DP_DATA_CTX *bt_a2dp_data, U8 stream_handle);

static BtStatus a2dpdata_handle_message(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message);
static void a2dp_handle_stream_data_out(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message);
static void a2dp_handle_stream_data_config_change(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message);
static BtStatus A2DPDATA_SendMessage(BT_A2DP_DATA_CTX *bt_a2dp_data, int server, msg_type msg_id, module_type mod_id, ilm_struct * ilm, U16 size);
static BtStatus a2dp_send_appi_bt_start_request (BT_A2DP_DATA_CTX *bt_a2dp_data);
static BtStatus a2dp_send_appi_bt_stop_request (BT_A2DP_DATA_CTX *bt_a2dp_data);
static BtStatus a2dp_wait_stream_data_config_get(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message, U16 *size);
static BtStatus a2dp_wait_msg_from_bt_mmi(BT_A2DP_DATA_CTX *bt_a2dp_data, unsigned long msg_id, ilm_struct *message, U16 *size);
static void a2dp_set_config_as_default(BT_A2DP_DATA_CTX *bt_a2dp_data);
static void a2dp_set_sbc_param(BT_A2DP_DATA_CTX *bt_a2dp_data);
static void a2dp_free_data_ctx(BT_A2DP_DATA_CTX *bt_a2dp_data);
static int sbc_init(sbc_t *sbc, U32 flags);
static void sbc_finish(sbc_t *sbc);
static void sbc_set(sbc_t *sbc, int allocation_method, int nrof_subbands, int nrof_blocks, int bitpool, U8 channel_mode);
static U8 get_header_data( int sampling_frequency, int blocks, int channel_mode, int allocation_method, int subbands );
static U32 calc_bitrate_from_bitpool( U32 bitpool, U8 nrof_subbands, U8 nrof_blocks, U8 channel_mode, U32 fs );
static int sbc_encode(sbc_t *sbc, const void *data, int count);
static void sbc_encoder_init(SBC_Encoder_State *state, const SBC_Frame_State *frame);
static int sbc_get_frame_interval(sbc_t *sbc, U8 subbands, U8 blocks, U32 frequency);
static kal_uint32 get_microseconds();
static BtStatus a2dp_get_msg_from_bt_mmi(BT_A2DP_DATA_CTX *bt_a2dp_data, unsigned long msg_id, ilm_struct *message, U16 *size);
extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);


#define BT_STATUS_SUCCESS 0
#define BT_STATUS_FAILED  1
static U32 btA2dpAdress = 0;

static U8 is_start_request = 0;

int a2dp_init(int rate, int channels, void **ptr)
{
    BT_A2DP_DATA_CTX *bt_a2dp_data = NULL;
    
    struct sockaddr_un a2dpextname;
    socklen_t   a2dpextnamelen;
    struct sockaddr_un a2dpintname;
    socklen_t   a2dpintnamelen;    
    struct sockaddr_un a2dpstreamname;
    socklen_t   a2dpstreamnamelen;    
    int memsize = 262144;

    bt_ext_log("[A2DP] a2dp_init rate:%d, ch:%d", rate, channels);
    bt_ext_log("[A2DP] a2dp_init btA2dpAdress 1: %d[%x]", btA2dpAdress, btA2dpAdress);


    if( 0 != btA2dpAdress) 
    { 
        bt_ext_log("[A2DP][ERR] a2dp_init btA2dpAdress is not null, waiting for main thread to exist");
        
        /* ALPS00780105 multi-thread issue: without lock protection */
        if (0 != bt_a2dp_data)
        bt_a2dp_data->is_cleanup = TRUE;
        return BT_STATUS_FAILED;
 #if 0       
        ALOGD("[A2DP][DBG] Close sockets");
        
        // send stop message to terminate main loop thread
        a2dp_stop(btA2dpAdress);
        usleep(100);
        
        if (((BT_A2DP_DATA_CTX *)btA2dpAdress)->srvcsock>=0)
        {
            close(((BT_A2DP_DATA_CTX *)btA2dpAdress)->srvcsock);
            ALOGD("[A2DP][DBG] Close srvsock[%d] : %s, errno=%d",((BT_A2DP_DATA_CTX *)btA2dpAdress)->srvcsock, strerror(errno), errno);
        }
        if (((BT_A2DP_DATA_CTX *)btA2dpAdress)->extsock>=0)
        {
            close(((BT_A2DP_DATA_CTX *)btA2dpAdress)->extsock);
            ALOGD("[A2DP][DBG] Close extsock failed[%d] : %s, errno=%d",((BT_A2DP_DATA_CTX *)btA2dpAdress)->extsock, strerror(errno), errno);
        }
        if (((BT_A2DP_DATA_CTX *)btA2dpAdress)->streamsock>=0)
        {
            close(((BT_A2DP_DATA_CTX *)btA2dpAdress)->streamsock);
            ALOGD("[A2DP][DBG] Close streamsock failed[%d] : %s, errno=%d",((BT_A2DP_DATA_CTX *)btA2dpAdress)->streamsock, strerror(errno), errno);
        }
        
        free(btA2dpAdress);
        btA2dpAdress = NULL;
#endif
    }
	

    *ptr = NULL;
    bt_a2dp_data = malloc(sizeof(BT_A2DP_DATA_CTX));
    btA2dpAdress = (unsigned int)bt_a2dp_data;
    bt_ext_log("[A2DP] a2dp_init btA2dpAdress 2: %d[%x]", btA2dpAdress, btA2dpAdress);
    ALOGD("[A2DP] malloc bt_a2dp_data, address:%x size:%d",(unsigned int)bt_a2dp_data,sizeof(BT_A2DP_DATA_CTX));
    if (!bt_a2dp_data)
    {
        bt_ext_log("[A2DP] Allocate BT_A2DP_DATA_CTX fail");
        return BT_STATUS_FAILED;
    }

    bt_a2dp_data->wifi_connected = 0;
    memset(bt_a2dp_data, 0, sizeof(BT_A2DP_DATA_CTX));

    //init socket handler
    bt_a2dp_data->srvcsock = -1;
    bt_a2dp_data->extsock = -1;
    bt_a2dp_data->streamsock = -1;

    //-----------------------------------------------------------------
    //           keep the input PCM format
    //-----------------------------------------------------------------
    bt_a2dp_data->pcm_rate = rate;
    bt_a2dp_data->pcm_channels = channels;
    //-----------------------------------------------------------------
    //           start setup socket
    //-----------------------------------------------------------------
    // Setup bt server address
    a2dpintname.sun_family = AF_UNIX;
    strcpy (a2dpintname.sun_path, /*BT_SERV_SOCK_ADDR*/BT_SOCK_NAME_INT_ADP);
    a2dpintnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (a2dpintname.sun_path) + 1);    
    // Setup a2dp data streaming server address
    a2dpstreamname.sun_family = AF_UNIX;
    strcpy (a2dpstreamname.sun_path, BT_SOCK_NAME_A2DP_STREAM);
    a2dpstreamnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (a2dpstreamname.sun_path) + 1);    
    // Setup a2dp service socket
    bt_a2dp_data->srvcsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_A2DP_DATA, 
                                                                 ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    if(bt_a2dp_data->srvcsock < 0)
    {
        ALOGE("[A2DP][ERR] create a2dp data server socket failed : %s, errno=%d", strerror(errno), errno);
        return BT_STATUS_FAILED;
    }
    else
    {
        a2dpextnamelen = sizeof(a2dpextname.sun_path);
        a2dpextname.sun_path[0] = '\0';
	 ALOGI("[A2DP] create a2dp data server socket success");
        if (getsockname(bt_a2dp_data->srvcsock, (struct sockaddr*)&a2dpextname, &a2dpextnamelen) < 0)
        {
            ALOGI("[A2DP] get data service sockname failed : %s, errno=%d", strerror(errno), errno);
        }
        else
        {
            ALOGI("[A2DP] get data service sockname successfully : len=%d, addr=%s", a2dpextnamelen, &a2dpextname.sun_path[1]);
        }
    }
    // Setup a2dp api socket
    a2dpextnamelen = sizeof(short);
    bt_a2dp_data->extsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    ALOGI("[A2DP] extsock==%d", bt_a2dp_data->extsock);
    if (bt_a2dp_data->extsock < 0)
    {
        ALOGE("[A2DP][ERR] create extsock failed : %s, errno=%d", strerror(errno), errno);
        return BT_STATUS_FAILED;
    }
    if (bind (bt_a2dp_data->extsock, (struct sockaddr *) &a2dpextname, a2dpextnamelen) < 0)
    {
        ALOGE("[A2DP][ERR] bind extsock failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
#if 0
    else
    {
        a2dpextnamelen = sizeof(a2dpextname.sun_path);
        a2dpextname.sun_path[0] = '\0';
        if (getsockname(bt_a2dp_data->extsock, (struct sockaddr*)&a2dpextname, &a2dpextnamelen) < 0)
        {
            ALOGE("[A2DP][ERR] get extsock name failed : %s, errno=%d", strerror(errno), errno);
        }
        else
        {
            ALOGI("[A2DP] Auto bind extsock socket : len=%d, addr=%s", a2dpextnamelen, &a2dpextname.sun_path[1]);
        }
    }
#endif
    if ( connect(bt_a2dp_data->extsock, (const struct sockaddr*)&a2dpintname, a2dpintnamelen) < 0)
    {
        ALOGE("[A2DP][ERR] connect to /data/btserv failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }

    // Setup a2dp data streaming socket
    bt_a2dp_data->streamsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    ALOGI("[A2DP] streamsock==%d", bt_a2dp_data->streamsock);
    if (bt_a2dp_data->streamsock < 0)
    {
        ALOGE("[A2DP][ERR] create streamsock failed : %s, errno=%d", strerror(errno), errno);
        return BT_STATUS_FAILED;
    }
    if (bind (bt_a2dp_data->streamsock, (struct sockaddr *) &a2dpextname, a2dpextnamelen) < 0)
    {
        ALOGE("[A2DP][ERR] bind streamsock failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
    if ( connect(bt_a2dp_data->streamsock, (const struct sockaddr*)&a2dpstreamname, a2dpstreamnamelen) < 0)
    {
        ALOGE("[A2DP][ERR] connect to a2dp streaming server failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }

    if(setsockopt(bt_a2dp_data->streamsock, SOL_SOCKET , SO_SNDBUF , &memsize, sizeof(memsize)) < 0)
    {
        ALOGE("[ERR] set a2dp streaming socket send buffer size fail: %s errno=%d\n", strerror(errno), errno);
    }

    //-----------------------------------------------------------------
    //           set a2dp external adaptor socket address
    //-----------------------------------------------------------------
    socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_A2DP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&bt_a2dp_data->ext_addr, &bt_a2dp_data->ext_addr_len);

    *ptr = bt_a2dp_data;

    ALOGD("[DBG] bt_a2dp_data->srvcsock[%d]\n",bt_a2dp_data->srvcsock);
    ALOGD("[DBG] bt_a2dp_data->extsock[%d]\n",bt_a2dp_data->extsock);
    ALOGD("[DBG] bt_a2dp_data->streamsock[%d]\n",bt_a2dp_data->streamsock);

    //-----------------------------------------------------------------
    //       3. Start Thread (to listen reconfig ind)
    //-----------------------------------------------------------------
    bt_a2dp_data->is_cleanup = FALSE;
    a2dp_startEventLoop(bt_a2dp_data);
	
    return BT_STATUS_SUCCESS;

exit:
    if (bt_a2dp_data->srvcsock>=0)
    {
        close(bt_a2dp_data->srvcsock);
        bt_a2dp_data->srvcsock = -1;
    }
    if (bt_a2dp_data->extsock>=0)
    {
        close(bt_a2dp_data->extsock);
        bt_a2dp_data->extsock = -1;
    }
   
	ALOGD("[A2DP] Free bt_a2dp_data ,address:%x ",(unsigned int)bt_a2dp_data);
    free(bt_a2dp_data);
	bt_a2dp_data = NULL;
	btA2dpAdress = 0;
    return BT_STATUS_FAILED;
}
void a2dp_set_sink(void *ptr, const char* address)
{
    BT_A2DP_DATA_CTX *bt_a2dp_data = (BT_A2DP_DATA_CTX*)ptr;
    
    bt_ext_log("[A2DP] a2dp_set_sink addr:%02x:%02x:%02x:%02x:%02x:%02x", 
    	                     address[0], address[1], address[2], address[3], address[4], address[5]);
}
int a2dp_write(void *ptr, const void *buffer, int count)
{
    BT_A2DP_DATA_CTX *bt_a2dp_data = (BT_A2DP_DATA_CTX*)ptr;
    bt_a2dp_sbc_codec_cap_struct *sbc;
    U8 *bufptr;
    U16 sent_bytes;
    BtStatus ret;
#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] a2dp_write count:%d", count);
#endif
    bt_ext_log("[A2DP] a2dp_write count:%d", count);
    if( ptr == NULL){
        ALOGE("Ptr is not right!!, ptr_address:%x",(unsigned int)ptr);
		return 0;
	}else if((unsigned int)ptr != btA2dpAdress ){
		ALOGE("Adress is not right!!, ptr_address:%x",(unsigned int)ptr);
		
	}else{
		ALOGD("Adress is right, ptr_address:%x",(unsigned int)ptr);
	}
    //when receiving disc indication or sending disc req, a2dp_write will return directly
    if(bt_a2dp_data->recv_disc_ind || bt_a2dp_data->sending_disc)
        return count;

    if(!bt_a2dp_data->is_streaming)
    {
 		/*1. check if there's start cnf exit on message queue*/
		ilm_struct ilm;
        U16 size;
		ret = a2dp_get_msg_from_bt_mmi(bt_a2dp_data, MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, &ilm, &size); 
		if(ret == BT_STATUS_SUCCESS){
			bt_a2dp_stream_data_start_cnf_struct *msg;
                msg = (bt_a2dp_stream_data_start_cnf_struct *)ilm.ilm_data;
                if(msg->result == BT_STATUS_SUCCESS)
                {
                    bt_ext_log("[A2DP] start Success!");
                    bt_a2dp_data->last_tx_time = 0;
                    bt_a2dp_data->is_streaming = TRUE;
                    bt_a2dp_data->payload_size = msg->prefer_size;
                }
                else
                {
                    ALOGE("[A2DP][ERR] start failed");
                    return 0;
                }
		}else{
    //-----------------------------------------------------------------
    //       2. Send Start Command (throught A2DP external adaptor)
    //-----------------------------------------------------------------
        //send message to bt_a2dp_hdl

        is_start_request = true;
        ret = a2dp_send_appi_bt_start_request(bt_a2dp_data);
        if(ret == BT_STATUS_SUCCESS)
        {
            ilm_struct ilm;
            U16 size;

            ret = a2dp_wait_msg_from_bt_mmi(bt_a2dp_data, MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, &ilm, &size); 
            is_start_request = false;
            
            if(ret == BT_STATUS_SUCCESS)
            {
                bt_a2dp_stream_data_start_cnf_struct *msg;
                msg = (bt_a2dp_stream_data_start_cnf_struct *)ilm.ilm_data;
                if(msg->result == BT_STATUS_SUCCESS)
                {
                    bt_ext_log("[A2DP] start Success!");
                    bt_a2dp_data->last_tx_time = 0;
                    bt_a2dp_data->is_streaming = TRUE;
                    bt_a2dp_data->payload_size = msg->prefer_size;
                    memcpy(&bt_a2dp_data->audio_cap, &msg->current_config, sizeof(bt_a2dp_audio_cap_struct));
                }
                else
                {
                    ALOGE("[A2DP][ERR] start failed");
                    return 0;
                }
            }
            else
            {
                ALOGE("[A2DP][ERR] wait start confirm failed");
                return 0;
            }
        }
        else
        {//send config get fail
            is_start_request = false;
            return 0;
        }
		}
  
        a2dp_set_sbc_param(bt_a2dp_data);
        
    }
	else{//For the case when a2dp is streaming then open wifi
		if((bt_a2dp_data->wifi_connected == 1)&&(bt_a2dp_data->setSbcDone==0)){
			bt_a2dp_data->setSbcDone = 1;
			a2dp_set_sbc_param(bt_a2dp_data);
		}
		if((bt_a2dp_data->wifi_connected == 0)&&(bt_a2dp_data->setSbcDone==1)){
			bt_a2dp_data->setSbcDone = 0;
		}
	}
    sbc = &bt_a2dp_data->audio_cap.codec_cap.sbc;

#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] Enter streaming state, start to enc data!bit pool:%d block:%d  subband: %d, alloc: %d, sr:%d, cm:%d",
                       bt_a2dp_data->bitpool, bt_a2dp_data->blocks, bt_a2dp_data->subbands, bt_a2dp_data->allocate_method, bt_a2dp_data->sample_count, bt_a2dp_data->channel_mode); 
#endif

    sent_bytes = a2dp_enc_data(bt_a2dp_data, 0, buffer, count);
    return sent_bytes;
}
int a2dp_stop(void *ptr)
{
    BtStatus ret;
    BT_A2DP_DATA_CTX *bt_a2dp_data = (BT_A2DP_DATA_CTX*)ptr;

    if(!bt_a2dp_data)
    {
        ALOGW("[A2DP] a2dp_stop. but ptr is NULL");
        return 0;
    }

    if((!bt_a2dp_data->is_streaming)&&(bt_a2dp_data->start_ind != TRUE))
    {
        ALOGW("[A2DP] a2dp_stop. but not streaming");
        return 0;
    }
	if(bt_a2dp_data->start_ind == TRUE){
		bt_a2dp_data->is_streaming = TRUE;
		bt_a2dp_data->start_ind = FALSE;
	}
    bt_ext_log("[A2DP] a2dp_stop. is_streaming:%d", bt_a2dp_data->is_streaming);
    //-----------------------------------------------------------------
    //       1. terminate pthread
    //-----------------------------------------------------------------
    //no need to wait 
    //pthead will be terminated when receiving stop confirm.
    
    //-----------------------------------------------------------------
    //       2. Send STOP Command (throught A2DP external adaptor)
    //-----------------------------------------------------------------
    //send message to bt_a2dp_hdl
    ret = a2dp_send_appi_bt_stop_request(bt_a2dp_data);
    if(ret == BT_STATUS_SUCCESS)
    {
#if 0
        ilm_struct ilm;
        U16 size;
        ret = a2dp_wait_msg_from_bt_mmi(bt_a2dp_data, MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, &ilm, &size); 
        if(ret == BT_STATUS_SUCCESS)
        {
            bt_a2dp_stream_data_stop_cnf_struct *msg;
            msg = (bt_a2dp_stream_data_stop_cnf_struct *)ilm.ilm_data;
            if(msg->result == BT_STATUS_SUCCESS)
            {
                bt_ext_log("[A2DP] stop Success!");
                bt_a2dp_data->is_streaming = FALSE;
            }
            else
            {
                ALOGE("[A2DP][ERR] stop failed");
                return 0;
            }
        }
        else
        {
            ALOGE("[A2DP][ERR] wait top confirm failed");
            return 0;
        }
#else
        //no need to wait 
        //pthead will be terminated when receiving stop confirm.
        return BT_STATUS_SUCCESS;
#endif
    }
    else
    {//send STOP fail
        return BT_STATUS_FAILED;
    }
}
void a2dp_cleanup(void *ptr)
{
    BtStatus ret;
    BT_A2DP_DATA_CTX *bt_a2dp_data = (BT_A2DP_DATA_CTX*)ptr;
    bt_ext_log("[A2DP] a2dp_cleanup");

    //-----------------------------------------------------------------
    //       case 1. Streaming
    //           we will send stop request first
    //           when receiving stop confirm --> 
    //           close sbc, free socket, free bt_a2dp_data, terminate thread
    //-----------------------------------------------------------------
    //if(bt_a2dp_data->is_streaming)
    {
        bt_ext_log("[A2DP] is streaming");
	
        //to avoid fail case : mainthread not be sheduled
	//and bt_a2dp_data->is_cleanup equals false,socket will not be closed
	bt_a2dp_data->is_cleanup = TRUE;

        ret = a2dp_send_appi_bt_stop_request(bt_a2dp_data);
        if(ret == BT_STATUS_SUCCESS)
        {
            //no need to wait 
            //pthead will be terminated when receiving stop confirm.
            //bt_a2dp_data->is_cleanup = TRUE;
            return;
        }
        else
        {//send STOP fail
            bt_ext_log("[A2DP] a2dp_send_appi_bt_stop_request fail!");
            // bt_a2dp_data->is_cleanup = FALSE;
            return;
        }
    }
	//else
    //{
    //-----------------------------------------------------------------
    //       case 2. Not Streaming
    //           close sbc, free socket, free bt_a2dp_data, terminate thread
    //-----------------------------------------------------------------
	    //bt_ext_log("[A2DP] no streaming");
        //a2dp_free_data_ctx(bt_a2dp_data);
//}
}

 static kal_bool a2dp_startEventLoop(BT_A2DP_DATA_CTX *bt_a2dp_data)
{
    pthread_attr_t thread_attr;
    ALOGI("[A2DP] a2dp_startEventLoop...");

    pthread_mutex_init(&bt_a2dp_data->data_thread_mutex, NULL);
    pthread_mutex_lock(&bt_a2dp_data->data_thread_mutex);    
    
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&bt_a2dp_data->data_thread, &thread_attr, a2dpdata_eventLoopMain, bt_a2dp_data);
    
    pthread_mutex_unlock(&bt_a2dp_data->data_thread_mutex);
    pthread_attr_destroy(&thread_attr);

    return BT_STATUS_SUCCESS;
}

 static void* a2dpdata_eventLoopMain(void *ptr)
 {
    fd_set readfs;
    int res = 0;
    ilm_struct ilm;
    BT_A2DP_DATA_CTX *bt_a2dp_data = (BT_A2DP_DATA_CTX*)ptr;
    BtStatus ret;

    struct timeval Timeout;

     pthread_mutex_lock(&bt_a2dp_data->data_thread_mutex);

    ALOGI("[A2DP][DataEventLoop] a2dpdata_eventLoopMain");
    
    while (1)
    {
#if A2DP_STREAM_DEBUG
        ALOGI("[A2DP][DataEventLoop] Start retrieve data from extsock ");
#endif

        FD_ZERO(&readfs);
        if(bt_a2dp_data->srvcsock>=0)
        {
            FD_SET(bt_a2dp_data->srvcsock, &readfs);
        }
        else
        {
            ALOGE("[A2DP][DataEventLoop][ERR] srvcsock < 0. exit");
        }
//#if A2DP_STREAM_DEBUG
        ALOGI("[A2DP][DataEventLoop] Start select : sockfd=%d", bt_a2dp_data->srvcsock);
//#endif
        Timeout.tv_usec = (RESPONSE_TIMEOUT%1000)*1000;
        Timeout.tv_sec  = RESPONSE_TIMEOUT/1000;
        
        res = TEMP_FAILURE_RETRY(select(bt_a2dp_data->srvcsock+1, &readfs, NULL, NULL, &Timeout));
//#if A2DP_STREAM_DEBUG
        ALOGI("[A2DP][DataEventLoop] Return from select : soresckfd. res=%d", res);
//#endif
        if(res > 0)
        {
            if (true == is_start_request) {
                 ALOGI("[A2DP] waiting for a2dp_write to receive start cnf message");
                 usleep(100);
                 continue;
            }
        	  	
            res = recvfrom(bt_a2dp_data->srvcsock, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
//#if A2DP_STREAM_DEBUG
            ALOGI("[A2DP][Media DataEventLoop] Recv A2DP external msg : %lu", ilm.msg_id);
//#endif

            ret = a2dpdata_handle_message(bt_a2dp_data, &ilm);
            if(ret == BT_STATUS_FAILED){
                if(bt_a2dp_data->is_cleanup)
                {   //close sbc, free socket, free bt_a2dp_data, terminate thread
                    ALOGI("[A2DP]pthread_mutex_unlock");
                    pthread_mutex_unlock(&bt_a2dp_data->data_thread_mutex);
                    pthread_mutex_destroy(&bt_a2dp_data->data_thread_mutex);
                    bt_a2dp_data->is_cleanup = FALSE;
                    a2dp_free_data_ctx(bt_a2dp_data);
                    ALOGI("[A2DP][DataEventLoop] thread finished");
                    return NULL;
                }
            }
                //goto done;
        }
        else if(res == 0)
        {
            ALOGW("[A2DP][DataEventLoop][WRN] timeout waiting indication");
#if 1
            /* because all the control command maybe not always transmit between media process and bt, so we should not terminate the thread when select function timeout */
            if(bt_a2dp_data->is_cleanup)
            {   
                ALOGI("[A2DP] timeout waiting for stop response, exit thread.");
                pthread_mutex_unlock(&bt_a2dp_data->data_thread_mutex);
                pthread_mutex_destroy(&bt_a2dp_data->data_thread_mutex);
                bt_a2dp_data->is_cleanup = FALSE;
                a2dp_free_data_ctx(bt_a2dp_data);
                ALOGI("[A2DP][DataEventLoop] thread finished");
                return NULL;
            }
#endif
            //break;
        }
        else
        {
            ALOGE("[A2DP][DataEventLoop][ERR] recvfrom failed : %s, %d", strerror(errno), errno);
            break;
        }
    }


done:
    
    if(bt_a2dp_data->is_cleanup)
    {
        // close sbc, free socket, free bt_a2dp_data, terminate thread

        ALOGI("[A2DP]pthread_mutex_unlock");
        pthread_mutex_unlock(&bt_a2dp_data->data_thread_mutex);
        pthread_mutex_destroy(&bt_a2dp_data->data_thread_mutex);

        bt_a2dp_data->is_cleanup = FALSE;
        a2dp_free_data_ctx(bt_a2dp_data);

        ALOGI("[A2DP][DataEventLoop] thread finished");
    }
 
    return NULL;
 }
 
static U16 a2dp_enc_data(BT_A2DP_DATA_CTX *bt_a2dp_data, U8 stream_handle, const U8 *source, U16 src_len)
{
    const U8 *srcptr; 
    int encoded_sbc_count = 0; //data needs to tx
    int sbc_frame_count = 0;
    U16 parsed_src_len = 0; //source data len that has been parsed
    //U16 sbc_read_count;  //sbc frame size
    kal_uint32 now = 0;

#ifdef __BT_A2DP_LOW_POWER__

    int a2dp_packet_counter = 0;
    int a2dp_sbc_frame_counter = 0;

#endif

    srcptr = source;
    
#if A2DP_STREAM_DEBUG
    ALOGI("[A2DP] a2dp_enc_data. src_len:%d", src_len);
#endif

    //sbc_read_count = ( src_len + bt_a2dp_data->pcm_frame_len - 1 ) / bt_a2dp_data->pcm_frame_len;

    //-----------------------------------------------------------------
    //           start encoding
    //-----------------------------------------------------------------

    if(src_len < bt_a2dp_data->pcm_frame_len)
    {
        bt_ext_log("[A2DP] PCM data needs encoded is not enough src_len:%d, pcm_frame_len:%d", src_len, bt_a2dp_data->pcm_frame_len);
        return 0;
    }
    //encoded_sbc_count start from 1. (leave one byte for SBC header format)
    //b7: F bit->fragmented     b6: S bit -> starting packet   b5: L bit -> last packet   b4 RFA
    //b3~b0: if F =0 means Number of frames
    encoded_sbc_count = 1;

    do
    {
        int count;

        count = sbc_encode( &bt_a2dp_data->sbc, srcptr+parsed_src_len, bt_a2dp_data->pcm_frame_len);
        if (0 == count) return parsed_src_len;
        	
        //copy the encoded sbc frame
        memcpy((bt_a2dp_data->enc_data)+encoded_sbc_count, bt_a2dp_data->sbc.data, bt_a2dp_data->sbc.len);
        sbc_frame_count ++;
        
#ifdef __BT_A2DP_LOW_POWER__
        a2dp_sbc_frame_counter++;
#endif /* __BT_A2DP_LOW_POWER__ */

        encoded_sbc_count += bt_a2dp_data->sbc.len;
        parsed_src_len += count;
        bt_a2dp_data->sample_count += count;

        //ALOGI("[A2DP] sbc_encode. sbc.len:%d count:%d encoded_sbc_count:%d parsed_src_len:%d sample_count:%d", sbc.len, count, encoded_sbc_count, parsed_src_len, bt_a2dp_data->sample_count);

        if((encoded_sbc_count+bt_a2dp_data->sbc.len) > bt_a2dp_data->payload_size || (src_len-parsed_src_len)<bt_a2dp_data->pcm_frame_len)
        {
            // send encoded packet
            now = get_microseconds();
            bt_a2dp_data->enc_data_len = encoded_sbc_count;
            // first byte keep the SBC header
            *(bt_a2dp_data->enc_data) = sbc_frame_count;
            
#ifdef __BT_A2DP_LOW_POWER__
            a2dp_packet_counter++;
#endif

#if A2DP_STREAM_DEBUG
            ALOGI("[A2DP] Finish one packet. Send...  sbc_frame_count:%d, encoded_sbc_count:%d", *(bt_a2dp_data->enc_data), encoded_sbc_count);
#endif
            if(!bt_a2dp_data->last_tx_time)
                bt_a2dp_data->last_tx_time = now;
            else
            	
#ifdef __BT_A2DP_LOW_POWER__

            if (a2dp_packet_counter >= 10) // 10 -> max a2dp packet number 
            {
                U32 interval = bt_a2dp_data->sbc_interval * a2dp_sbc_frame_counter;
                a2dp_sbc_frame_counter = 0;
                a2dp_packet_counter = 0;
                
#else /* __BT_A2DP_LOW_POWER__ */

            {
                U32 interval = bt_a2dp_data->sbc_interval * sbc_frame_count;
                
#endif /* __BT_A2DP_LOW_POWER__ */

                long wait = bt_a2dp_data->last_tx_time + interval - now;
#if A2DP_STREAM_DEBUG
                ALOGI("[A2DP] last_tx_time:%ld  interval:%ldms now:%ld wait:%ldms", bt_a2dp_data->last_tx_time, interval/1000, now, wait/1000);
#endif
                if(wait > 0)
                {
                    usleep(wait);
#if A2DP_STREAM_DEBUG
                    ALOGI("[A2DP] wackup to send data");
#endif
                 }
                if(wait < -3000000)
                {
                    ALOGI("[A2DP] reset audio sync clock");
                    bt_a2dp_data->last_tx_time = 0;
                }
                else
                    bt_a2dp_data->last_tx_time += interval;
            }

            btmtk_a2dp_send_stream_data_req(bt_a2dp_data, stream_handle);
            //audio will send 10240bytes, we will split two frame to send. (740 & 332 bytes)
            //for sending performance. we just send 740 and return. (the sequence will be 740, 740, 740...)
            //break;
            
            //encoded_sbc_count start from 1. (leave one byte for SBC header format)
            //b7: F bit->fragmented     b6: S bit -> starting packet   b5: L bit -> last packet   b4 RFA
            //b3~b0: if F =0 means Number of frames
            encoded_sbc_count = 1;
            sbc_frame_count = 0;
        }
    }
    while((src_len-parsed_src_len)>=bt_a2dp_data->pcm_frame_len);

#ifdef __BT_A2DP_LOW_POWER__
    
    now = get_microseconds();
    U32 interval = bt_a2dp_data->sbc_interval * a2dp_sbc_frame_counter;
    long wait = bt_a2dp_data->last_tx_time + interval - now;
    
    if(wait > 0)
    {
         usleep(wait);
#if A2DP_STREAM_DEBUG
         ALOGI("[A2DP] low power wait: %dms", wait/1000);
#endif
    }
    if(wait < -3000000)
    {
         ALOGI("[A2DP] reset audio sync clock");
         bt_a2dp_data->last_tx_time = 0;
    }
    else
         bt_a2dp_data->last_tx_time += interval;
	     
#endif /* __BT_A2DP_LOW_POWER__ */

    return parsed_src_len;
}

static void btmtk_a2dp_send_stream_data_req(BT_A2DP_DATA_CTX *bt_a2dp_data, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    U16 payload_size;
    U16 tx_data_len;

#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] btmtk_a2dp_send_stream_data_req!");
#endif

    ((bt_a2dp_stream_data_send_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;
    memcpy(((bt_a2dp_stream_data_send_req_struct*)ilmptr->ilm_data)->data, bt_a2dp_data->enc_data, bt_a2dp_data->enc_data_len);
    ((bt_a2dp_stream_data_send_req_struct*)ilmptr->ilm_data)->len = bt_a2dp_data->enc_data_len;
    ((bt_a2dp_stream_data_send_req_struct*)ilmptr->ilm_data)->sample_count = bt_a2dp_data->sample_count;
    
    A2DPDATA_SendMessage(bt_a2dp_data, bt_a2dp_data->streamsock, MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_data_send_req_struct));
}

void a2dp_handle_stream_qos_ind(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message)
{
	static int counter = 0;
	struct sbc_priv *priv;
	bt_a2dp_stream_qos_ind_struct *msg;

	if (!bt_a2dp_data || !message) return;

	msg  = (bt_a2dp_stream_qos_ind_struct *)message->ilm_data;
	priv = bt_a2dp_data->sbc.priv;

	if (msg->qos == 0x0a && priv->frame.bitpool > 0x26)
	{
		if ( bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool <= 0x26 )
		{      
			if (bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool >= 0x26)
			priv->frame.bitpool = 0x26;
		else
                		priv->frame.bitpool = bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool;
        	}
		else
			priv->frame.bitpool = bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool;

		counter = 0;
	}
	else if (msg->qos == 0x06)
	{
		if (counter > 1 && priv->frame.bitpool > bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool) 
		{
			priv->frame.bitpool = priv->frame.bitpool - counter;

			if (priv->frame.bitpool < 0x26 &&
			    bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool >= 0x26)
				priv->frame.bitpool = 0x26;
		}
		counter++;
	}
	else
		counter = 0;

	bt_ext_log("[A2DP] QoS adjust, current bitpool: %d", priv->frame.bitpool);
}

static BtStatus a2dpdata_handle_message(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message)
{
    BtStatus ret = BT_STATUS_SUCCESS;
#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] a2dpdata_handle_message msg id:%lu", message->msg_id);
#endif
    switch(message->msg_id)
    {
        case MSG_ID_BT_A2DP_STREAM_QOS_IND:

            a2dp_handle_stream_qos_ind(bt_a2dp_data, message);
            
            ret = BT_STATUS_SUCCESS;
            break;
        case MSG_ID_BT_A2DP_STREAM_DATA_OUT:
            a2dp_handle_stream_data_out(bt_a2dp_data, message);
            ret = BT_STATUS_SUCCESS;
            break;
        case MSG_ID_BT_A2DP_STREAM_DATA_CONFIG_CHANGE:
            a2dp_handle_stream_data_config_change(bt_a2dp_data, message);
            ret = BT_STATUS_SUCCESS;
            break;
        case MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF:
            ret = BT_STATUS_FAILED;
            bt_a2dp_data->is_streaming = FALSE;
            //after media player receive a2dp state change (disconnected), it will call a2dp_stop
            //then we reset recv_disc_ind and sending_disc, when stop finish.
            bt_a2dp_data->recv_disc_ind = FALSE;
            bt_a2dp_data->sending_disc = FALSE;
            break;
        case MSG_ID_BT_A2DP_STREAM_DATA_DISC_IND:
            //when bt_a2dp_hdl reveiving disc_ind. it will send message to bt_a2dp_android.
            bt_a2dp_data->recv_disc_ind = TRUE;
            break;
        case MSG_ID_BT_A2DP_STREAM_DATA_DISCONNECTING_IND:
            //when bt_a2dp_api send disc_req. it will send message to inform bt_a2dp_android.
            ret = BT_STATUS_FAILED;
            bt_a2dp_data->is_streaming = FALSE;
            bt_a2dp_data->recv_disc_ind = FALSE;
            bt_a2dp_data->sending_disc = TRUE;
            break;
	
	case MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED:
            bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED");
            bt_a2dp_data->wifi_connected = 1;
            break;
	case MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED:
            bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED");
            bt_a2dp_data->wifi_connected = 0;
            break;
	case MSG_ID_BT_A2DP_STREAM_START_IND:
            bt_a2dp_data->start_ind = TRUE;
            bt_a2dp_data->is_streaming = TRUE;
            bt_a2dp_data->payload_size = ((bt_a2dp_stream_data_start_ind_struct *)(message->ilm_data))->prefer_size;
            memcpy(&bt_a2dp_data->audio_cap, &(((bt_a2dp_stream_data_start_ind_struct *)(message->ilm_data))->current_config), sizeof(bt_a2dp_audio_cap_struct));
            a2dp_set_sbc_param(bt_a2dp_data);
            bt_ext_log("[A2DP] anroid.c recevied start ind, set sbc parameter.");
            break;
	case MSG_ID_BT_A2DP_STREAM_DATA_START_CNF:
            a2dp_send_appi_bt_start_request(bt_a2dp_data);
            bt_ext_log("[A2DP] received a2dp start cnf in a2dpdata_handle_message fun.");
            break;
	
    }
    return ret;
}

static void a2dp_handle_stream_data_out(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message)
{
    bt_a2dp_stream_data_out_struct *msg;
    msg = (bt_a2dp_stream_data_out_struct *)message->ilm_data;

#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] stream_data_out. Send next enc_data.");
#endif
}

static void a2dp_handle_stream_data_config_change(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message)
{
    bt_a2dp_stream_data_config_change_struct *msg;
    msg = (bt_a2dp_stream_data_config_change_struct *)message->ilm_data;

    bt_ext_log("[A2DP] stream_data_config_change. min bit pool: %d, max bit pool: %d, block_len:%d",
                       msg->audio_cap.codec_cap.sbc.min_bit_pool, 
                       msg->audio_cap.codec_cap.sbc.max_bit_pool, 
                       msg->audio_cap.codec_cap.sbc.block_len); 
    bt_ext_log("[A2DP] Cont. subband: %d, alloc_method: %d, sample_rate:%d, channel_mode:%d",
                       msg->audio_cap.codec_cap.sbc.subband_num, 
                       msg->audio_cap.codec_cap.sbc.alloc_method, 
                       msg->audio_cap.codec_cap.sbc.sample_rate, 
                       msg->audio_cap.codec_cap.sbc.channel_mode); 
    if(msg->audio_cap.codec_cap.sbc.min_bit_pool == 0)
    {
        ALOGE("[A2DP][ERR] Receive wrong config change event. Use dafault setting."); 
        a2dp_set_config_as_default(bt_a2dp_data);
    }
    else
    {
        memcpy(&bt_a2dp_data->audio_cap, &msg->audio_cap, sizeof(bt_a2dp_audio_cap_struct));
    }
    a2dp_set_sbc_param(bt_a2dp_data);

}

static BtStatus A2DPDATA_SendMessage(BT_A2DP_DATA_CTX *bt_a2dp_data, int server, msg_type msg_id, module_type mod_id, ilm_struct * ilm, U16 size)
{
    int ret;
    int sockfd;
    U16 *field;
	
#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] msg send to server:%d msg=%u, ptr=0x%X, size=%d", server, msg_id, (unsigned int)ilm, size);
#endif

    ret = -1;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = mod_id;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    if (msg_id != MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ)
    {
        *(++field) = size;
    }
    else
    {// use fake length to improve log performance
        *(++field) = size-800;
    }

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
#if A2DP_STREAM_DEBUG
    bt_ext_log("[A2DP] A2DPDATA_SendMessage size: %d", size);
#endif
    if(mod_id == MOD_MMI)
    {//send to a2dp external adaptor
        if(bt_a2dp_data->srvcsock>=0)
        {
            ret = sendto(bt_a2dp_data->srvcsock, ilm, size, 0, (const struct sockaddr*)&bt_a2dp_data->ext_addr, bt_a2dp_data->ext_addr_len);
            if(ret < 0)
            {
                bt_ext_err("[A2DP] data send msg to a2dp external adaptor fail : %s, %d", strerror(errno), errno);
            }
            else
            {
                bt_ext_log("[A2DP] data send msg to to a2dp external adaptor success : %d", ret);
            }
        }
        else
        {
            bt_ext_err("[A2DP] g_srvcsockfd uninitialized");
        }
    }
    else
    {
        if(server)
        {
            ret = send(server, ilm, size, 0);
            if(ret < 0)
            {
                bt_ext_err("[A2DP] data send msg fail : %s, %d", strerror(errno), errno);
            }
            else
            {
    #if A2DP_STREAM_DEBUG
                bt_ext_log("[A2DP] data send msg success : %d", ret);
    #endif
            }
        }
        else
        {
            bt_ext_err("[A2DP] extsock uninitialized");
        }
    }
    return (ret >= 0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}

static BtStatus a2dp_send_appi_bt_start_request (BT_A2DP_DATA_CTX *bt_a2dp_data)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_ext_log("[A2DP] a2dp_send_appi_bt_start_request!");

    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_START;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->req_fm_a2dp_data = TRUE;
    return A2DPDATA_SendMessage(bt_a2dp_data, 0, MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}

static BtStatus a2dp_send_appi_bt_stop_request (BT_A2DP_DATA_CTX *bt_a2dp_data)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_ext_log("[A2DP] a2dp_send_appi_bt_stop_request!");
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_STOP;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->req_fm_a2dp_data = TRUE;
    return A2DPDATA_SendMessage(bt_a2dp_data, 0, MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}

static BtStatus a2dp_wait_stream_data_config_get(BT_A2DP_DATA_CTX *bt_a2dp_data, ilm_struct *message, U16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;
    int sockfd = bt_a2dp_data->srvcsock;
    int i = 3;

    do
    {
        Timeout.tv_usec = (RESPONSE_TIMEOUT%1000)*1000;
        Timeout.tv_sec  = RESPONSE_TIMEOUT/1000;
        FD_ZERO(&readfs);
        if(sockfd>=0)
        {
            FD_SET(sockfd, &readfs);
        }
        else
        {
            bt_ext_err("[A2DP][ERR] Wait config get. bt_a2dp_data->srvcsock < 0. exit");
            break;
        }
        res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, &Timeout));
        if(res > 0)
        {
            res = recvfrom(sockfd, message, sizeof(ilm_struct), 0, NULL, NULL);
            if(res < 0)
            {
                bt_ext_err("[A2DP] Wait config get. recvfrom failed : %s, %d", strerror(errno), errno);
                break;
            }
            else
            {
                if(message->msg_id != MSG_ID_BT_A2DP_STREAM_DATA_CONFIG_CNF)
                {
                    // add by yi for wifi on changes
                    if(message->msg_id == MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED){
                        bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED");
                        bt_a2dp_data->wifi_connected = 1;
                    }else if(message->msg_id == MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED){
                        bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED");
                        bt_a2dp_data->wifi_connected = 0;
                    }
					
                    res = -1;
                    bt_ext_err("[A2DP] Wait config get or config change. recv resp=%u is mismatch.", (unsigned int)message->msg_id);
                }
                else
                {
                    *size = res;            
                    bt_ext_log("[A2DP] Wait config get. recv resp=%u, bytes=%d", (unsigned int)message->msg_id, res);
                    break;
                }
            }
        }
        else if(res == 0)
        {
            bt_ext_err("[JNI] timeout waiting response, in %d milliseconds", RESPONSE_TIMEOUT);
            break;
        }
        else
        {
            bt_ext_err("[JNI] wait select failed : %s, %d", strerror(errno), errno);
            break;
        }
        i--;
    }
    while(i>0);
    return (res>0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}

static BtStatus a2dp_wait_msg_from_bt_mmi(BT_A2DP_DATA_CTX *bt_a2dp_data, unsigned long msg_id, ilm_struct *message, U16 *size)
{
    int res = 0;
    int ret = 0;
    fd_set readfs;
    struct timeval Timeout;
    int sockfd = bt_a2dp_data->srvcsock;

    Timeout.tv_usec = (RESPONSE_TIMEOUT%1000)*1000;
    Timeout.tv_sec  = RESPONSE_TIMEOUT/1000;
    FD_ZERO(&readfs);
    if(sockfd>=0)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        bt_ext_err("[A2DP][ERR] Wait msg from mmi:%u. bt_a2dp_data->srvcsock < 0. exit", (unsigned int)msg_id);
    }
    res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, &Timeout));
    if(res > 0)
    {
        res = recvfrom(sockfd, message, sizeof(ilm_struct), 0, NULL, NULL);
        if(res < 0)
        {
            bt_ext_err("[A2DP] Wait msg from mmi:%u. recvfrom failed : %s, %d", (unsigned int)msg_id, strerror(errno), errno);
        }
        else
        {
            if(message->msg_id != msg_id)
            {
                ret = a2dpdata_handle_message(bt_a2dp_data, message);
                if(ret == BT_STATUS_FAILED){
                    if(bt_a2dp_data->is_cleanup)
                    {   
                        // close sbc, free socket, free bt_a2dp_data, terminate thread
                        bt_a2dp_data->is_cleanup = FALSE;
                        a2dp_free_data_ctx(bt_a2dp_data);
                        ALOGI("[A2DP][DataEventLoop] thread finished");
                    }
                }
                
                res = -1;
                bt_ext_err("[A2DP] Wait msg from mmi: %u. But recv %u. Mismatch!!", (unsigned int)msg_id, (unsigned int)message->msg_id);
            }
            else
            {
                *size = res;            
                bt_ext_log("[A2DP] Wait msg from mmi. recv resp=%u, bytes=%d", (unsigned int)message->msg_id, res);
            }
        }
    }
    else if(res == 0)
    {
        bt_ext_err("[A2DP] timeout waiting response, in %d milliseconds", RESPONSE_TIMEOUT);
    }
    else
    {
        bt_ext_err("[A2DP] wait select failed : %s, %d", strerror(errno), errno);
    }
    return (res>0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}

static void a2dp_set_config_as_default(BT_A2DP_DATA_CTX *bt_a2dp_data)
{
    bt_a2dp_data->audio_cap.codec_type = BT_A2DP_SBC;
    bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool = 19;
    bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool = 50;
    bt_a2dp_data->audio_cap.codec_cap.sbc.block_len = 1; //16
    bt_a2dp_data->audio_cap.codec_cap.sbc.subband_num = 1; //subbands no: 8
    bt_a2dp_data->audio_cap.codec_cap.sbc.alloc_method = 1; //Loudness
    bt_a2dp_data->audio_cap.codec_cap.sbc.sample_rate = 2; //44100
    bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode = 1; //joint stereo
}

static void a2dp_set_sbc_param(BT_A2DP_DATA_CTX *bt_a2dp_data)
{

    bt_ext_log("[A2DP] a2dp_set_sbc_param, cm: %d, %d, %d, %d, %d",
        bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode,
        bt_a2dp_data->audio_cap.codec_cap.sbc.subband_num,
        bt_a2dp_data->audio_cap.codec_cap.sbc.block_len,
        bt_a2dp_data->audio_cap.codec_cap.sbc.alloc_method,
        bt_a2dp_data->audio_cap.codec_cap.sbc.sample_rate);
    //bt_a2dp_data->channels = bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode == 8 /*MONO*/ ? 1 : 2;

    if(bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode == 1)
        bt_a2dp_data->channel_mode = SBC_CM_JOINT_STEREO;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode == 2)
        bt_a2dp_data->channel_mode = SBC_CM_STEREO;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode == 4)
        bt_a2dp_data->channel_mode = SBC_CM_DUAL_CHANNEL;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.channel_mode == 8)
        bt_a2dp_data->channel_mode = SBC_CM_MONO;
    
    if(bt_a2dp_data->audio_cap.codec_cap.sbc.subband_num == 1)
        bt_a2dp_data->subbands = 8;
    else //subband_num ==  2
        bt_a2dp_data->subbands = 4;

    if(bt_a2dp_data->audio_cap.codec_cap.sbc.block_len == 1)
        bt_a2dp_data->blocks = 16;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.block_len == 2)
        bt_a2dp_data->blocks = 12;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.block_len == 4)
        bt_a2dp_data->blocks = 8;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.block_len == 8)
        bt_a2dp_data->blocks = 4;

    if(bt_a2dp_data->audio_cap.codec_cap.sbc.alloc_method == 1)
        bt_a2dp_data->allocate_method = LOUDNESS;
    else if(bt_a2dp_data->audio_cap.codec_cap.sbc.alloc_method == 2)
        bt_a2dp_data->allocate_method = SNR;

    if( bt_a2dp_data->audio_cap.codec_cap.sbc.sample_rate == 1)
        bt_a2dp_data->fs = 48000;
    else if( bt_a2dp_data->audio_cap.codec_cap.sbc.sample_rate == 2)
        bt_a2dp_data->fs = 44100;
    else if( bt_a2dp_data->audio_cap.codec_cap.sbc.sample_rate == 4)
        bt_a2dp_data->fs = 32000;
    else//if( bt_a2dp_data->audio_cap.codec_cap.sbc.sample_rate == 8)
        bt_a2dp_data->fs = 16000;
    if(bt_a2dp_data->wifi_connected == 0){
		bt_ext_log("[A2DP] wifi is not connected , increase a2dp throughput");
    //[a2dp performance][noise][XH] 	
    if( bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool >= 0x35 && bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool <= 0x35 )
    {
	bt_a2dp_data->bitpool = 0x35;//53
    }
	else if(bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool >= 0x23 && bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool <= 0x23)
    {
        bt_a2dp_data->bitpool = 0x23;//35
    }else
    {
        bt_a2dp_data->bitpool = bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool;
	//bt_a2dp_data->bitpool = bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool;
    }
    }else{
        bt_ext_log("[A2DP] wifi is connected , decrease a2dp throughput");
        if( bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool >= 0x26 && bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool <= 0x26 )
        {
            bt_a2dp_data->bitpool = 0x26;  
        }
        else if(bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool < 0x26)
        {
            bt_a2dp_data->bitpool = bt_a2dp_data->audio_cap.codec_cap.sbc.max_bit_pool;
        }
        else
        {
            bt_a2dp_data->bitpool = bt_a2dp_data->audio_cap.codec_cap.sbc.min_bit_pool;
        }
    }
    bt_a2dp_data->bitrate = calc_bitrate_from_bitpool( bt_a2dp_data->bitpool, bt_a2dp_data->subbands, bt_a2dp_data->blocks, bt_a2dp_data->channel_mode, bt_a2dp_data->fs);

    bt_a2dp_data->pcm_frame_len = bt_a2dp_data->pcm_channels * bt_a2dp_data->subbands * bt_a2dp_data->blocks * sizeof(kal_int16);

    //release memory if allocated
    sbc_finish(&bt_a2dp_data->sbc);
    sbc_init( &bt_a2dp_data->sbc, SBC_NULL);
    bt_a2dp_data->sbc.rate = bt_a2dp_data->fs;
    bt_a2dp_data->sbc.channels = bt_a2dp_data->pcm_channels;
    sbc_set( &bt_a2dp_data->sbc, bt_a2dp_data->allocate_method, bt_a2dp_data->subbands, bt_a2dp_data->blocks, bt_a2dp_data->bitpool, bt_a2dp_data->channel_mode );
    bt_a2dp_data->sbc_interval = sbc_get_frame_interval(&bt_a2dp_data->sbc, bt_a2dp_data->subbands,bt_a2dp_data->blocks, bt_a2dp_data->fs);
    bt_ext_log("[A2DP] sbc_interval:%d us.", bt_a2dp_data->sbc_interval);
}

static void a2dp_free_data_ctx(BT_A2DP_DATA_CTX *bt_a2dp_data)
{
    bt_ext_log("[A2DP] a2dp_free_data_ctx");
    //close sbc, free socket, free bt_a2dp_data
    sbc_finish(&bt_a2dp_data->sbc);
    if (bt_a2dp_data->srvcsock>=0)
    {
        close(bt_a2dp_data->srvcsock);
        bt_a2dp_data->srvcsock = -1;
    }
    if (bt_a2dp_data->extsock>=0)
    {
        close(bt_a2dp_data->extsock);
        bt_a2dp_data->extsock = -1;
    }
     if (bt_a2dp_data->streamsock>=0)
    {
        close(bt_a2dp_data->streamsock);
        bt_a2dp_data->streamsock = -1;
    }
	 
	ALOGD("[A2DP] Free bt_a2dp_data success, address:%x",(unsigned int)bt_a2dp_data);
    free(bt_a2dp_data);
	bt_a2dp_data = NULL;
    btA2dpAdress = 0;
}

static int sbc_init(sbc_t *sbc, U32 flags)
{
	if (!sbc)
		return -EIO;

	memset(sbc, 0, sizeof(sbc_t));

	sbc->priv = malloc(sizeof(struct sbc_priv));
        ALOGD("[A2DP] malloc sbc_priv, address:%x size:%d",(unsigned int)sbc->priv,sizeof(struct sbc_priv));
	if (!sbc->priv)
		return -ENOMEM;

	memset(sbc->priv, 0, sizeof(struct sbc_priv));
	
	((struct sbc_priv *)sbc->priv)->frame.sb_sample = ((struct sbc_priv *)sbc->priv)->sb_sample;

	return 0;
}

static void sbc_finish(sbc_t *sbc)
{
	struct sbc_priv *priv;
	if (!sbc)
		return;
	priv = sbc->priv;

	if (sbc->data)
	{
        ALOGD("[A2DP] Free sbc->data,adress:%x",(unsigned int)sbc->data);
		free(sbc->data);
		sbc->data = NULL;
	}

	if (priv)
	{
		ALOGD("[A2DP] Free priv,adress:%x",(unsigned int)priv);
		free(priv);
		priv = NULL;
        
	}

	memset(sbc, 0, sizeof(sbc_t));
}

static void sbc_set(sbc_t *sbc, int allocation_method, int nrof_subbands, int nrof_blocks, int bitpool, U8 channel_mode)
{
	struct sbc_priv *priv;
	
	priv = sbc->priv;
	
	priv->frame.sampling_frequency = sbc->rate;
	priv->frame.allocation_method = allocation_method;
	priv->frame.subbands = nrof_subbands;
	priv->frame.blocks = nrof_blocks;
	priv->frame.bitpool = bitpool;
	priv->frame.channel_mode = channel_mode;
	priv->frame.channels = (channel_mode==SBC_CM_MONO)? 1: 2;

	priv->frame.sbc_frame_header_data = get_header_data( sbc->rate, nrof_blocks, channel_mode, allocation_method, nrof_subbands );
	
	bt_ext_log("[A2DP] sbc para, freq:%d, blocks:%d, channel:%d, subband:%d, allocation:%d, bitpool:%d", \
		priv->frame.sampling_frequency,  \
		priv->frame.blocks,  \
		priv->frame.channels,  \
		priv->frame.subbands,  \
		priv->frame.allocation_method, \
		priv->frame.bitpool );
}

static U8 get_header_data( int sampling_frequency, int blocks, int channel_mode, int allocation_method, int subbands )
{
	U8 data=0;

	if( sampling_frequency==16000 )
		data = 0x0<<6;
	else if( sampling_frequency==32000 )
		data = 0x1<<6;
	else if( sampling_frequency==44100 )
		data = 0x2<<6;
	else if( sampling_frequency==48000 )
		data = 0x3<<6;
	else
		assert(false);

	if( blocks==4 )
		data |= 0x0<<4;
	else if( blocks==8 )
		data |= 0x1<<4;
	else if( blocks==12 )
		data |= 0x2<<4;
	else if( blocks==16 )
		data |= 0x3<<4;
	else
		assert(false);

	if( channel_mode==SBC_CM_MONO )
		data |= 0x0<<2;
	else if( channel_mode==SBC_CM_DUAL_CHANNEL )
		data |= 0x1<<2;
	else if( channel_mode==SBC_CM_STEREO)
		data |= 0x2<<2;
	else if( channel_mode==SBC_CM_JOINT_STEREO)
		data |= 0x3<<2;
	else
		assert(false);

	if( allocation_method==SBC_AM_LOUDNESS )
		data |= 0x0<<1;
	else if( allocation_method==SBC_AM_SNR )
		data |= 0x1<<1;
	else
		assert(false);

	if( subbands==4 )
		data |= 0x0;
	else if( subbands==8 )
		data |= 0x1;
	else
		assert(false);

	return( data );
}

static U32 calc_bitrate_from_bitpool( U32 bitpool, U8 nrof_subbands, U8 nrof_blocks, U8 channel_mode, U32 fs )
{
	U8 nrof_channels, join;
	U32 frame_length=0, bitrate=0;

	nrof_channels = ( channel_mode==SBC_CM_MONO )? 1: 2;
	join = ( channel_mode==SBC_CM_JOINT_STEREO)? 1: 0;

	frame_length = 4 + ( 4 * nrof_subbands * nrof_channels ) / 8;
	if( channel_mode==SBC_CM_MONO || channel_mode==SBC_CM_DUAL_CHANNEL )
		frame_length += (nrof_blocks * nrof_channels * bitpool) / 8;
	else if( channel_mode==SBC_CM_STEREO || channel_mode==SBC_CM_JOINT_STEREO )
		frame_length += ( join * nrof_subbands + nrof_blocks * bitpool ) / 8;
	else
		assert(false);

	bitrate = ( 8 * frame_length * fs ) / ( nrof_subbands * nrof_blocks );
	bitrate = (bitrate + 500) / 1000; // in kb/s with rounding

	return( bitrate );
}

static int sbc_encode(sbc_t *sbc, const void *data, int count)
{
	struct sbc_priv *priv;
	const char *ptr;
	int i, ch, framelen, samples;
	short pcm_sample[2][16*8];

	if (!sbc)
		return -EIO;

	priv = sbc->priv;

	if (!priv->init) {
		sbc_encoder_init(&priv->enc_state, &priv->frame);
		priv->init = 1;
	}

	ptr = data;

//	for (i = 0; i < priv->frame.subbands * priv->frame.blocks; i++) {
//		for (ch = 0; ch < sbc->channels; ch++) {
//			pcm_sample[ch][i] = (ptr[1] & 0xff) << 8 | (ptr[0] & 0xff);
//			ptr += 2;
//		}
//	}

	samples = sbc_analyze_audio(sbc->channels, &priv->enc_state, &priv->frame, (short*)data);
//            sbc_analyze_audio( 1, priv->enc_state, &sbc.frame_state, sbc.subband_buffer );
//    samples = sbc.frame_state.subbands * sbc.frame_state.blocks;
	if (!sbc->data) {
		sbc->size = 1024;
		sbc->data = malloc(sbc->size);
        ALOGD("[A2DP] malloc sbc->data, address:%x size:1024",(unsigned int)sbc->data);
	}

	if (!sbc->data) {
		sbc->size = 0;
		return -ENOMEM;
	}

	sbc_pack_frame(&priv->frame, sbc->data, &framelen);

	sbc->len = framelen;

	return samples * sbc->channels * 2;
}

static void sbc_encoder_init(SBC_Encoder_State *state, const SBC_Frame_State *frame)
{
	//memset(&state->S, 0, sizeof(state->S));
	memset(&state->X, 0, sizeof(state->X));
	//memset(&state->Y, 0, sizeof(state->Y));
	//memset(&state->Z, 0, sizeof(state->Z));
	//state->subbands = frame->subbands;
}

static int sbc_get_frame_interval(sbc_t *sbc, U8 subbands, U8 blocks, U32 frequency)
{
#if 0
	struct sbc_priv *priv;

	priv = sbc->priv;
	if (priv->init)
      {
		subbands = priv->frame.subbands;
		blocks = priv->frame.blocks;
	}
#endif
	return (1000000 * blocks * subbands) / frequency;
}

static kal_uint32 get_microseconds()
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (now.tv_sec * 1000000 + now.tv_nsec / 1000);
}

static BtStatus a2dp_get_msg_from_bt_mmi(BT_A2DP_DATA_CTX *bt_a2dp_data, unsigned long msg_id, ilm_struct *message, U16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;
    int sockfd = bt_a2dp_data->srvcsock;

    Timeout.tv_usec = (3001%1000)*1000;
    Timeout.tv_sec  = 0;
    FD_ZERO(&readfs);
    if(sockfd>=0)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        bt_ext_err("[A2DP][ERR] Wait msg from mmi:%u. bt_a2dp_data->srvcsock < 0. exit", (unsigned int)msg_id);
    }
    res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, &Timeout));

	bt_ext_log("[A2DP] a2dp_get_msg_from_bt_mmi");
  
	if(res > 0)
	{
	    res = recvfrom(sockfd, message, sizeof(ilm_struct), 0, NULL, NULL);
	    if(res < 0)
	    {
	        bt_ext_err("[A2DP] get msg from mmi:%u. recvfrom failed : %s, %d", (unsigned int)msg_id, strerror(errno), errno);
	    }
	    else
	    {
			if(message->msg_id == MSG_ID_BT_A2DP_STREAM_START_IND)
			{			
				bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_START_IND");
				bt_a2dp_data->is_streaming = TRUE;
				bt_a2dp_data->payload_size = ((bt_a2dp_stream_data_start_ind_struct *)(message->ilm_data))->prefer_size;
				memcpy(&bt_a2dp_data->audio_cap, &((bt_a2dp_stream_data_start_ind_struct *)(message->ilm_data))->current_config, sizeof(bt_a2dp_audio_cap_struct));
			}
	        if(message->msg_id != msg_id)
	        {
				if(message->msg_id == MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED){
					bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED~");
					bt_a2dp_data->wifi_connected = 1;
				}else if(message->msg_id == MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED){
					bt_ext_log("[A2DP] MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED~");
					bt_a2dp_data->wifi_connected = 0;
				}
				res = -1;
	            bt_ext_err("[A2DP] Wait msg from mmi: %u. But recv %u. Mismatch!!", (unsigned int)msg_id, (unsigned int)message->msg_id);
	        }
	        else
	        {
	            *size = res;            
	            bt_ext_log("[A2DP] Wait msg from mmi. recv resp=%u, bytes=%d", (unsigned int)message->msg_id, res);
	        }
	    }
	}
	else if(res == 0)
    {
        bt_ext_log("[A2DP] timeout waiting response, in %ld milliseconds", Timeout.tv_usec);
    }
    else
    {
        bt_ext_err("[A2DP] wait select failed : %s, %d", strerror(errno), errno);
    }

    return (res>0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}


