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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * A2dp.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file implements bt a2dp services
 *
 * Author:
 * -------
 * mtk80833
 *
 ****************************************************************************/
 
#ifdef __BT_A2DP_PROFILE__

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bt_common.h"
#include "a2dp.h"
#include "a2dp_inc.h"
#include "bt_devices_list.h"
#include "linux/sched.h"

#ifdef __BT_HFG_PROFILE__ 
#include "bluetooth_hfg_struct.h"
#endif

#include "sdap.h"
#include "bt_adp_system.h"
#include "btbm.h"
#include "btbm_int.h"

#ifdef __BT_SEQ_SUPPORT__
#include "che_api.h"
#include "l1audio.h"
#endif 

//Add for PTS v2.2.0.0 A2DP query by stanley
//extern BtStatus A2dpSdpQuery(void);
//#define SKIP_SDP_QUERY // use for passing PTS test only

/*define this compile option to temporarily solve MT6001 
  can not connect A2DP while SCO is disconnecting issue*/
#if 0 
    #define __A2DP_WAIT_SCO_DISCONNECT_TO_CONTINUE__
#endif

/* add for FM over SBC */
#include "l2cap_i.h"

bt_a2dp_ctx_struct bt_a2dp_ctx;

#ifdef __A2DP_WAIT_SCO_DISCONNECT_TO_CONTINUE__
bt_addr_struct a2dp_tmp_device_addr;
BOOL a2dp_wait_sco_disconnecting = FALSE;
extern bt_hfg_state hfga_get_current_state(void);
extern bt_hsg_state hsga_get_current_state(void);
#endif

#if (MAX_A2DP_DEV_NUM == 1)
#define __FM_OVER_BT_CHIP__
#endif

static U16 bt_a2dp_attr_id_list[] = 
{
    AID_BT_PROFILE_DESC_LIST
};


extern void bt_a2dp_get_bd_addr(bt_addr_struct *src, BD_ADDR *dst);
extern void bt_a2dp_get_bt_device_addr(BD_ADDR *src, bt_addr_struct *dst);
extern U8 *bt_a2dp_mem_alloc(U32 size);
extern void bt_a2dp_mem_free(U8 *ptr);
extern void bt_a2dp_mem_init(U8 *memory, U32 size);
extern U32 bt_a2dp_mem_get_alloc_size(void);
extern BOOL BtSysAdpVerifyInPanicState(void);
extern DEVICE_MODEL BTBMGetDeviceModel(BD_ADDR *addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern U8 SDPDB_GetLocalServiceAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn);
extern BtStatus SDP_AddRecord(SdpRecord *record);
extern BtStatus SDP_RemoveRecord(SdpRecord *record);

extern void A2dpHandleActivateCnf(U16 result);
extern void A2dpHandleDeactivateCnf(U16 result);
extern void A2dpHandleStreamReconfigCnf(U16 result,  U8 stream_handle);
extern void A2dpHandleStreamReconfigInd(U8 stream_handle, A2dpCodec *audio_cap);
extern void A2dpHandleStreamOpenCnf(U16 result, 
                                                U8 stream_handle, 
                                                bt_addr_struct *device_addr, 
                                                U8 *dev_name, U8 name_len);
extern void A2dpHandleStreamOpenInd(U8 stream_handle,
                                                bt_addr_struct *device_addr, 
                                                U8 *dev_name, U8 name_len);
extern void A2dpHandleStreamStartCnf(U16 result, 
                                                U8 stream_handle, 
                                                U16 prefer_size, 
                                                bt_a2dp_audio_cap_struct *config);
extern void A2dpHandleStreamStartInd(U8 stream_handle, U16 perfer_size, bt_a2dp_audio_cap_struct *config);
extern void A2dpHandleStreamSuspendCnf(U16 result, U8 stream_handle);
extern void A2dpHandleStreamSuspendInd(U8 stream_handle);
extern void A2dpHandleStreamQosInd(U8 stream_handle, U8 qos);
extern void A2dpHandleStreamCloseCnf(U16 result, U8 stream_handle);
extern void A2dpHandleStreamCloseInd(U8 stream_handle);
extern void A2dpHandleStreamAbortCnf(U8 stream_handle);
extern void A2dpHandleStreamAbortInd(U8 stream_handle);
extern void A2dpHandleStreamStartIndCfm(void);
extern void A2dpHandleStreamDataSentOut(U8 stream_handle);
extern void A2dpHandleConfigChange(bt_a2dp_audio_cap_struct *audio_cap);
extern void A2dpHandleConfigGet(bt_a2dp_audio_cap_struct *audio_cap);

#ifdef BTMTK_ON_LINUX
extern void MeWriteEnableFMoverSBC(BtRemoteDevice *link,U16 max_l2cap_size, U16 AVDTP_Sig_SID, U16 AVDTP_Sig_CID, U16 l2cap_cid, U8 *sbc_capability, U16 features);
extern void MeWriteDisableFMoverSBC(BtRemoteDevice *link);
extern U16 L2CAP_GetL2CAPRemoteCid(L2capChannelId ChannelId);
extern U16 AVDTP_MediaGetChannelId(AvdtpStream *Stream);
static void setSbcCodecElements();
extern BOOL bt_adp_a2dp_unpack_audio_cap(A2dpCodec *src, bt_a2dp_audio_cap_struct *dst);
#define AVDTP_MediaGetChannelId(s) ((s).conn.l2ChannelId)
#endif

extern void A2DP_StopFMOverSBC(BtRemoteDevice *link);
extern void A2DP_FMContrllerStartReq(U8 stream_handle);
extern void A2DP_FMContrllerStopReq(U8 stream_handle);
extern void A2DP_FMContrllerSuspendReq(U8 stream_handle);
extern void A2DP_FMContrllerResumeReq(U8 stream_handle);

extern U16 L2CAP_GetTxMtu(U16 ChannelId);
extern void HCI_Process(void);

#ifdef __BT_SEQ_SUPPORT__
extern char* release_verno(void);
#endif
//#endif
static BtStatus A2dpSdpQuery(U8);
static void a2dp_convert_codec(A2dpCodec *codec, bt_a2dp_audio_cap_struct *cap);
static U8 A2dpFindStreamIDbyAddr(BD_ADDR *addr);
kal_bool A2dpReleaseStreamHandler(U8 stream_handler);
BT_A2DP_RESULT_ENUM A2DP_StreamSendDataReq(U8 stream_handle, U8 *buffer, U32 buffer_size, U32 sample_count);
U8 A2dpGetState(void);
U16 A2DP_GetPreferredPayloadSize(U8 stream_handle);

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(a) Assert(a)


int SetPriorityByName(char *name, int method, int proirity)
{
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[256];
    char filetext[256];
    struct sched_param param;
    int pid;
    int file_len = 0;
	
    if (!name) return 0;

    kal_prompt_trace(MOD_BT, "[A2DP] setpriorityByName: %s", name);

    dir = opendir("/proc"); 
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) continue;
            if (DT_DIR != ptr->d_type) continue;
			if(strlen(ptr->d_name) > 256) continue;//Just to resolve Klockwork CR ALPS01395988 , directory name length cannot over 256 bytes in linux
            sprintf(filepath, "/proc/%s/comm", ptr->d_name);
            fp = fopen(filepath, "r");
			
            if (NULL != fp)
            {
                file_len = fread(filetext, 1, 256, fp);
				if(file_len > 0)//To resolve Klockwork CR ALPS01395964
                	filetext[file_len] = '\0';
				
                // kal_prompt_trace(MOD_BT, "[A2DP] len: %d, find cmdline: %s", file_len, filetext);
                if (strstr(filetext, name)) 
                {
                    kal_prompt_trace(MOD_BT, "[A2DP] set PID: %d schedule method to: %d proirity: %d", atoi(ptr->d_name), method, proirity);
					
                    param.sched_priority = proirity;
                    sched_setscheduler(atoi(ptr->d_name), method, &param);

                    fclose(fp);
                    closedir(dir);

                    return 1;
                }
                fclose(fp);
            }
        }
        closedir(dir);
    }
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *      A2dpCopyCodec
 *
 * DESCRIPTION
 *      copy the codec infor from src to dst
 *
 * PARAMETERS
 *      AvdtpCodec *src [IN]
 *      AvdtpCodec *dst [OUT]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static void A2dpCopyCodec(AvdtpCodec *src, AvdtpCodec *dst)
{
    dst->codecType = src->codecType;
    dst->elemLen   = src->elemLen;
    dst->elements = src->elements;
}



 /*****************************************************************************
 * FUNCTION
 *      A2dpCmgrLinkClose
 *
 * DESCRIPTION
 *      close cmgr link
 *
 * PARAMETERS
 *      U8 stream_id [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static void A2dpCmgrLinkClose(U8 stream_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    BT_A2DP_FUNC_ENTRY1(BT_A2DP_CMGR_CLOSE, stream_id);
        
    CMGR_RemoveDataLink(&bt_a2dp_ctx.a2dp_device_list[stream_id].cmgr_hdlr);
    CMGR_RemoveCmgrWithDevice(&bt_a2dp_ctx.a2dp_device_list[stream_id].cmgr_hdlr); 

    A2dpReleaseStreamHandler(stream_id);

}


/****************************************************************************
 * FUNCTION
 *        A2dpSetSdpQueryCount
 *
 * DESCRIPTION
 *        This function is used to set the a2dp sdp query count
 *        For IOT with other a2dp headset products, the a2dp query should be with UUID AUDIO_DISTRIBUTION
 *        But PTS only accepts a2dp sdp query with uuid AUDIO_SINK,
 *        so the a2dp_sdp_query_count is used to determine if re-do a2dp sdp query is needed or not. 
 *
 * PARAMETERS
 *        value [IN]
 *
 * RETURNS
 *        void
 *
 ***************************************************************************/
void A2dpSetSdpQueryCount(U8 value)
{
    // TODO: snyc with sdp owner
    bt_a2dp_ctx.a2dp_sdp_query_count = value;
}

/****************************************************************************
 * FUNCTION
 *        A2DP_GetSdpQueryCount
 *
 * DESCRIPTION
 *        This function is used to get the current a2dp sdp query count
 *        used by sdap.c
 *
 * PARAMETERS
 *        value [IN]
 *
 * RETURNS
 *         void
 *
 ***************************************************************************/
U8 A2DP_GetSdpQueryCount(void)
{
    // TODO: snyc with sdp owner
    return bt_a2dp_ctx.a2dp_sdp_query_count; 
}


/*****************************************************************************
 * FUNCTION
 *      A2dpSdpQueryCallback
 *
 * DESCRIPTION
 *      sdp query callback function
 *
 * PARAMETERS
 *      sqt              [?]         
 *      result           [IN]        
 *      attr_idx        [IN]        
 *      attr_val        [?]      
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static void A2dpSdpQueryCallback(SdpQueryToken *sqt, U8 result, U8 attr_idx, U8 *attr_val)
{
    bt_addr_struct device_addr;
    U8 stream_handler = 0;

    stream_handler = A2dpFindStreamIDbyAddr(&sqt->rm->bdAddr);
    
    // ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);
    if (stream_handler == UNKNOWN_STREAM_HANDLE) return;

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_SDP_EVENT_CALLBACK, 
        result, 
        stream_handler,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf,
        attr_idx);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_SDP_QUERY)
    {
        bt_a2dp_get_bt_device_addr(&sqt->rm->bdAddr, &device_addr);

        switch (result)
        {
        case BT_STATUS_SUCCESS:
            switch (bt_a2dp_attr_id_list[attr_idx])
            {
            case AID_BT_PROFILE_DESC_LIST:
                // found AVDTP protocol 
                // SDP_GetU16( sqt->valueBuff ); // avdtpVersion
                if(sqt->uuid != SC_AUDIO_DISTRIBUTION)
                {
                    A2dpSetSdpQueryCount(0);
                    BT_A2DP_FUNC_ENTRY(BT_A2DP_SDP_REQUERY_SUCCESS);
                }
                
                BT_A2DP_FUNC_ENTRY1(BT_A2DP_SDP_SUCCESS_AND_CONN_GAVDP, sqt->uuid);

                bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_OPEN;
                GAVDP_EstablishConnectionReq(bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep, &sqt->rm->bdAddr);
                
                break;
            }
            break;
        case BT_STATUS_CONNECTION_FAILED:
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:

            if(sqt->uuid == SC_AUDIO_DISTRIBUTION)
            {
                //Retry do sdp again, but try to use SC_AUDIO_SINK as new uuid
                BtStatus status = BT_STATUS_FAILED;
                
                A2dpSetSdpQueryCount(1);
                BT_A2DP_FUNC_ENTRY(BT_A2DP_DO_SDP_AGAIN);
                status = A2dpSdpQuery(stream_handler); 
                if(status != BT_STATUS_PENDING)
                {
                   goto exit;
                }
            }
            else if(sqt->uuid == SC_AUDIO_SINK)
            {
				goto exit;
			}
			break;
		default:/*[JM] mainly for SDP query timeout situation*/
 exit:                        
			A2dpSetSdpQueryCount(0);
			bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;
			A2dpCmgrLinkClose(stream_handler);
			A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_TIMEOUT, stream_handler, &device_addr, NULL, 0);
            break;
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *     A2dpSdpQuery
 * DESCRIPTION
 *     do sdp query for specific stream id
 * PARAMETERS
 *     U8 stream_id
 * RETURNS
 *     BtStatus
 *****************************************************************************/
static BtStatus A2dpSdpQuery(U8 stream_id)
{
    BtStatus status;

    sdap_service_search_multi_attribute_struct search_pattern;

    BT_A2DP_FUNC_ENTRY1(BT_A2DP_DO_SDP_QUERY, stream_id);
    
    // assign ACL link device
    bt_a2dp_ctx.a2dp_device_list[stream_id].sqt.rm = bt_a2dp_ctx.a2dp_device_list[stream_id].cmgr_hdlr.bdc->link;
    
    // search for SINK service
    search_pattern.rm = bt_a2dp_ctx.a2dp_device_list[stream_id].sqt.rm;
    search_pattern.sqt = &bt_a2dp_ctx.a2dp_device_list[stream_id].sqt;
    search_pattern.uuid = SC_AUDIO_SINK;
    search_pattern.callback = A2dpSdpQueryCallback;
    search_pattern.attribute_num = 0x01;
    search_pattern.attribute_id = bt_a2dp_attr_id_list;

    status = SDAP_ServiceSearchMultipleAttribute(search_pattern);

    return status;
}


/*****************************************************************************
 * FUNCTION
 *      A2dpFindStreamIDbyAddr
 *
 * DESCRIPTION
 *      find out the related stream id of given BT address
 *
 * PARAMETERS
 *      BD_ADDR *addr    
 *
 * RETURNS
 *      U8
 *
 *****************************************************************************/
static U8 A2dpFindStreamIDbyAddr(BD_ADDR *addr)
{
    int i = 0, j = 0;

    ASSERT(addr != NULL);
    
    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        if (bt_a2dp_ctx.a2dp_device_list[i].in_use == KAL_TRUE)
        {
            for (j = 0; j < BD_ADDR_SIZE; j++)
                if (addr->addr[j] != bt_a2dp_ctx.a2dp_device_list[i].remote_bd_addr.addr[j])
                    break;
        }
        
        if (j == BD_ADDR_SIZE) return i;
    }
    
    return UNKNOWN_STREAM_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *      A2dpFindStreamIDbySep
 *
 * DESCRIPTION
 *      find out the related stream id of given sep
 *
 * PARAMETERS
 *      StreamEndPoint *sep  
 *
 * RETURNS
 *      U8
 *
 *****************************************************************************/
static U8 A2dpFindStreamIDbySep(StreamEndPoint *sep)
{
    U8 i = 0;

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        if (bt_a2dp_ctx.a2dp_device_list[i].local_sep == sep)
            return i;
    }

    return UNKNOWN_STREAM_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *       A2dpCmgrEventCallback
 *
 * DESCRIPTION
 *  
 * PARAMETERS
 *       cmgr_hdlr      [?]         
 *       event           [IN]        
 *       status          [IN]     
 *
 * RETURNS
 *       void
 *
 *****************************************************************************/
static void A2dpCmgrEventCallback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_addr_struct device_addr;
    U8 stream_handler;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_A2DP_FUNC_ENTRY2(BT_A2DP_CMGR_EVENT_CALLBACK , event, status);
    bt_a2dp_get_bt_device_addr(&cmgr_hdlr->remDev->bdAddr, &device_addr);
    
    // TODO: maybe we should set cmgr_hdlr->remDev->bdAddr in A2dpContinueOpenReq function
    stream_handler = A2dpFindStreamIDbyAddr(&cmgr_hdlr->remDev->bdAddr);
    
    if (stream_handler == UNKNOWN_STREAM_HANDLE) 
    {
        BT_A2DP_FUNC_ENTRY(BT_A2DP_BAD_ADDRESS);
        return;
    }
    
    if (event == CMEVENT_DATA_LINK_DIS) 
    {
        A2dpCmgrLinkClose(stream_handler);
        return;
    }
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_DATA_LINK)
    {
        switch (event)
        {
        case CMEVENT_DATA_LINK_CON_CNF:
            
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;
            
            if (status == BT_STATUS_SUCCESS)
            {
            #ifdef SKIP_SDP_QUERY
                bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_OPEN;
                GAVDP_EstablishConnectionReq(bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep, &bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr);
            #else

                status = A2dpSdpQuery(stream_handler);

                BT_A2DP_VAL_TRACE1(status);

                if (status == BT_STATUS_PENDING)
                {
                    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_SDP_QUERY;
                }
                else
                {
                    A2dpCmgrLinkClose(stream_handler);
                    A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_TIMEOUT, stream_handler, &device_addr, NULL, 0);
                }
            #endif /* SKIP_SDP_QUERY */
            }
            else
            {
                /* Status == BT_STATUS_SCO_REJECT, shall handle SCO not allow */
                A2dpCmgrLinkClose(stream_handler);
                A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_TIMEOUT, stream_handler, &device_addr, NULL, 0);
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_Init
 * DESCRIPTION
 *  
 * PARAMETERS
 *      void
 * RETURNS
 *      void
 *****************************************************************************/
void A2DP_Init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_A2DP_FUNC_ENTRY(BT_A2DP_INIT);
    
    if (bt_a2dp_ctx.is_a2dp_active == TRUE)
    {
        kal_prompt_trace(MOD_BT, "[A2DP] a2dp init, deactive first");
        A2DP_Deactivate();
    }
    OS_MemSet((U8 *)&bt_a2dp_ctx, 0, sizeof(bt_a2dp_ctx_struct));
    bt_a2dp_mem_init(&bt_a2dp_ctx.packet_buffer[0], PACKET_BUFFER_SIZE);
    
    GAVDP_Init();
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_Deinit
 * DESCRIPTION
 *  
 * PARAMETERS
 *      void
 * RETURNS
 *      void
 *****************************************************************************/
void A2DP_Deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    BT_A2DP_FUNC_ENTRY(BT_A2DP_DEINIT);
    
}


/*****************************************************************************
 * FUNCTION
 *      A2dpGetStreamEndpoint
 *
 * DESCRIPTION
 *      get a stream endpoint from local_sep_List
 *
 * PARAMETERS
 *      void  
 *
 * RETURNS
 *      StreamEndPoint *
 *
 *****************************************************************************/
StreamEndPoint * A2dpGetStreamEndpoint(void)
{
    U8 i = 0;

    for (i = 0; i < MAX_A2DP_SEP_NUM; i++)
    {
        if (0 == bt_a2dp_ctx.local_sep_List[i].info.inUse)
        {
            bt_a2dp_ctx.local_sep_List[i].info.inUse = 1;
            return &bt_a2dp_ctx.local_sep_List[i];
        }
    }

    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *      A2dpReleaseStreamEndpoint
 *
 * DESCRIPTION
 *      release the specific sep
 *
 * PARAMETERS
 *      StreamEndPoint *sep
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
void A2dpReleaseStreamEndpoint(StreamEndPoint *sep)
{
    if (sep)
    {
        sep->info.inUse = 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2dpGetStreamHandler
 *
 * DESCRIPTION
 *      get a stream handler from idle device list
 *
 * PARAMETERS
 *      void
 *
 * RETURNS
 *      U8
 *
 *****************************************************************************/
U8 A2dpGetStreamHandler(void)
{
    U8 i = 0;

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        if (KAL_FALSE == bt_a2dp_ctx.a2dp_device_list[i].in_use)
        {
            bt_a2dp_ctx.a2dp_device_list[i].in_use = KAL_TRUE;
            return i;
        }
    }

    return UNKNOWN_STREAM_HANDLE;
}


/*****************************************************************************
 * FUNCTION
 *      A2dpReleaseStreamHandler
 *
 * DESCRIPTION
 *      reset the specific device list. (note: should not reset cmgr handler)
 *
 * PARAMETERS
 *      U8 stream_handler
 *
 * RETURNS
 *      kal_bool
 *
 *****************************************************************************/

kal_bool A2dpReleaseStreamHandler(U8 stream_handler)
{
    BT_A2DP_FUNC_ENTRY1(BT_A2DP_RELEASE_STREAM_HDL, stream_handler);
    
    if (stream_handler >= MAX_A2DP_DEV_NUM)
        return KAL_FALSE;

    /* NOTE: should not reset the memory of bt_a2dp_ctx.a2dp_device_list[stream_handler].cmgr_hdlr, or it will assert in cmgr module */
    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep)
        bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep->info.inUse = 0;
    
    bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep = NULL;  
    bt_a2dp_ctx.a2dp_device_list[stream_handler].in_use = KAL_FALSE;
    memset(&bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr, 0, sizeof(BD_ADDR));
    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_NONE;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_started = 0;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].data_seq_no = 0;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].data_timestamp = 0;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].device_type = 0;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].packet_buffer_count = 0;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].postpond_suspend = 0;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].a2dp_sdp_query_count = 0;
    
    if(bt_a2dp_ctx.fm_over_bt_mode == 1)
    {
        bt_a2dp_ctx.fm_over_bt_mode = 0;
    }
    
    return KAL_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *       A2dpContinueConnectReq
 *
 * DESCRIPTION
 *       continue to open a2dp streaming channel
 *
 * PARAMETERS
 *       bt_addr_struct *device_addr [IN]    
 *
 * RETURNS
 *       void
 *
 *****************************************************************************/
static void A2dpContinueOpenReq(bt_addr_struct *device_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    U8 i = 0, stream_handler = 0;
    CmgrHandler *cmgr_hdlr = 0;
    BD_ADDR tmp_addr;
    StreamEndPoint *tmp_sep;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    /* check if this device has been connected */
    bt_a2dp_get_bd_addr(device_addr, &tmp_addr);    
    stream_handler = A2dpFindStreamIDbyAddr(&tmp_addr);
    
    BT_A2DP_FUNC_ENTRY4(BT_A2DP_CONTINUE_OPEN_REQ, device_addr->lap, device_addr->nap, device_addr->uap, stream_handler);
    
    if (/*stream_handler >= 0 &&*/ stream_handler < MAX_A2DP_DEV_NUM)
    {
        if (bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep->stream_state >= GAVDP_STATE_OPEN)
        {
            BT_A2DP_FUNC_ENTRY(BT_A2DP_DEVICE_OPENED);
            A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_OK, stream_handler, device_addr, NULL, 0);
            return;
        }
        // A2dpReleaseStreamHandler(stream_handler);
        A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_FATAL_ERROR, stream_handler, device_addr, NULL, 0);
        return;
    }
    
    /* allocate stream handler here and release it in the callback function (open failed) or close confirm function */
    stream_handler = A2dpGetStreamHandler();
    if (stream_handler == UNKNOWN_STREAM_HANDLE)
    {
        BT_A2DP_FUNC_ENTRY(BT_A2DP_GET_STREAM_HDL_ERR);
        A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_FATAL_ERROR, stream_handler, device_addr, NULL, 0);
        return;
    }
   
    tmp_sep = A2dpGetStreamEndpoint();
    if (NULL == tmp_sep)
    {
        BT_A2DP_FUNC_ENTRY(BT_A2DP_GET_STREAM_SEP_ERR);
        A2dpReleaseStreamHandler(stream_handler);
        A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_FATAL_ERROR, stream_handler, device_addr, NULL, 0);
        return;
    }

    BT_A2DP_FUNC_ENTRY2(BT_A2DP_CONTINUE_OPEN_INFO, tmp_sep, stream_handler);
    
    /* link local sep to tmp sep */
    bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep = tmp_sep;
    cmgr_hdlr = &bt_a2dp_ctx.a2dp_device_list[stream_handler].cmgr_hdlr;
    
    /* store device address in device list */
    bt_a2dp_get_bd_addr(device_addr, &bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr);
    
#ifdef SKIP_SDP_QUERY

    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_OPEN;
    GAVDP_EstablishConnectionReq(tmp_sep, bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr);
    
    /* we already send cnf back, so let's set status to be pending */
    /* to skip the following status checking and avoid to send cnf twice */
    status = BT_STATUS_PENDING;
    
#else /* SKIP_SDP_QUERY */

    status = CMGR_CreateDataLink(cmgr_hdlr, &bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr);

    BT_A2DP_VAL_TRACE1(status);

    if (status == BT_STATUS_PENDING)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_DATA_LINK;
    }
    else if (status == BT_STATUS_SUCCESS)
    {
        //A2dpSetSdpQueryCount(1);
        status = A2dpSdpQuery(stream_handler);

        BT_A2DP_VAL_TRACE1(status);

        if (status == BT_STATUS_PENDING)
        {
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_SDP_QUERY;
        }
        else
        {
            A2dpCmgrLinkClose(stream_handler);
        }
    }
    else
    {
        A2dpCmgrLinkClose(stream_handler);
    }

#endif /* SKIP_SDP_QUERY */

    if (status != BT_STATUS_PENDING)
    {
         A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_FATAL_ERROR, stream_handler, device_addr, NULL, 0);
    }
}



/*****************************************************************************
 * FUNCTION
 *       A2DP_StreamOpenReq
 *
 * DESCRIPTION
 *       try to open a a2dp streaming channel, used by a2dp adp handler function
 *
 * PARAMETERS
 *       bt_addr_struct *device_addr  
 *
 * RETURNS
 *       void
 *
 *****************************************************************************/
void A2DP_StreamOpenReq(bt_addr_struct *device_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_STREAM_OPEN, device_addr->lap, device_addr->nap, device_addr->uap);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleStreamOpenCnf( BT_A2DP_RESULT_PANIC, UNKNOWN_STREAM_HANDLE, device_addr, NULL, 0);
        return;
    }
    

#ifdef __A2DP_WAIT_SCO_DISCONNECT_TO_CONTINUE__
    a2dp_tmp_device_addr = *(device_addr);

    if( (hfga_get_current_state()==hfg_sco_disconnecting)||(hsga_get_current_state()==hsg_sco_disconnecting))
    {
        a2dp_wait_sco_disconnecting = TRUE;
        BT_A2DP_FUNC_ENTRY1(BT_A2DP_SIGNAL_WAIT_SCO_DISCONNECT, a2dp_wait_sco_disconnecting);
    }
    else
#endif        
    {
        A2dpContinueOpenReq(device_addr);
    }

}


/*****************************************************************************
 * FUNCTION
 *       A2dpStreamOpenCnf
 *
 * DESCRIPTION
 *      this function maybe called back for open stream req and rsp
 *
 * PARAMETERS
 *      sep        [IN]     
 *      info        [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static void A2dpStreamOpenCnf(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 result;
    U8 status = 0;
    bt_addr_struct device_addr;
    U8 stream_handler = UNKNOWN_STREAM_HANDLE; 
    BTBMRemoteNameSetting_T *deviceName;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    bt_a2dp_get_bt_device_addr(&(sep->remote_bd_addr), &device_addr);
    
    deviceName = btbmUtilConnectedDeviceNameEntryLookUp(&(sep->remote_bd_addr));

    /* search for allocated stream id in open request function */
    stream_handler = A2dpFindStreamIDbyAddr(&(sep->remote_bd_addr));
    
    if (/*stream_handler >= 0 &&*/ stream_handler < MAX_A2DP_DEV_NUM)
    {
	    BT_A2DP_FUNC_ENTRY4(BT_A2DP_OPEN_CNF_INFO, info->error, stream_handler, 
	        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf,
	        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res);
	    
	    BT_A2DP_FUNC_ENTRY6(BT_A2DP_ADDR, sep->remote_bd_addr.addr[0],
	        sep->remote_bd_addr.addr[1],
	        sep->remote_bd_addr.addr[2],
	        sep->remote_bd_addr.addr[3],
	        sep->remote_bd_addr.addr[4],
	        sep->remote_bd_addr.addr[5]);
    
        if (info->error != GAVDP_ERR_NO_ERR &&
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf < BT_A2DP_WAIT_CNF_OPEN)
            return;
			
        if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf <= BT_A2DP_WAIT_CNF_OPEN)
        {
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;
        }
    }
    
    if (info->error == GAVDP_ERR_NO_ERR)
    {
        result = BT_A2DP_RESULT_OK;
    }
    else
    {
        if (info->error == GAVDP_ERR_CONN_IN)
        {
        	  result = BT_A2DP_RESULT_NO_CONNECTION;
        }
        else
        {
            result = BT_A2DP_RESULT_TIMEOUT;
        }
        
        if (stream_handler != UNKNOWN_STREAM_HANDLE)
            A2dpReleaseStreamHandler(stream_handler);
    }

    if (/*stream_handler >= 0 &&*/ stream_handler < MAX_A2DP_DEV_NUM)
    {
        if(deviceName != 0)
            A2dpHandleStreamOpenCnf(result, stream_handler, &device_addr, (U8*)deviceName->dev_name, deviceName->dev_name_len);
        else
            A2dpHandleStreamOpenCnf(result, stream_handler, &device_addr, NULL, 0);
    }
    else
    {
    	if(info->error != GAVDP_ERR_NO_ERR)//[JM Ma]ALPS01007236
			return;
        /* remote device only send connect ind */
        stream_handler = A2dpGetStreamHandler();
        
        if (stream_handler < MAX_A2DP_SEP_NUM) {
            BT_A2DP_FUNC_ENTRY3(BT_A2DP_OPEN_IND, stream_handler,
                bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
                bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
         }
            
        if (stream_handler == UNKNOWN_STREAM_HANDLE)
        {
            GAVDP_ReleaseConnectionReq(sep); 
            return;
        }
        else
        {
            bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep = sep;
            
            if(deviceName != 0)
                A2dpHandleStreamOpenInd(stream_handler, &device_addr, (U8*)deviceName->dev_name, deviceName->dev_name_len);
            else
                A2dpHandleStreamOpenInd(stream_handler, &device_addr, NULL, 0);
            
            /* store device address in device list */
            bt_a2dp_get_bd_addr(&device_addr, &bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr);
            
            /* create a data link for FM over BT and CMGR_QoS */
            status = CMGR_CreateDataLink(&bt_a2dp_ctx.a2dp_device_list[stream_handler].cmgr_hdlr, &sep->remote_bd_addr);
				
            BT_A2DP_VAL_TRACE1(status);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamOpenInd
 *
 * DESCRIPTION
 *      handler a2dp open indication
 *
 * PARAMETERS
 *      sep     [IN]     
 *      info     [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static U8 A2dpStreamOpenInd(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_addr_struct device_addr;
    U8 status = 0;
    U8 i = 0, stream_handler = UNKNOWN_STREAM_HANDLE;
    BTBMRemoteNameSetting_T *deviceName = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if( BtSysAdpVerifyInPanicState() )
    {
        return BT_STATUS_FAILED;
    }

    bt_a2dp_get_bt_device_addr(&(sep->remote_bd_addr), &device_addr);
    
    deviceName = btbmUtilConnectedDeviceNameEntryLookUp(&(sep->remote_bd_addr));
    stream_handler = A2dpGetStreamHandler();

    if (stream_handler < MAX_A2DP_SEP_NUM) {
        BT_A2DP_FUNC_ENTRY3(BT_A2DP_OPEN_IND, stream_handler,
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    }
    if (stream_handler == UNKNOWN_STREAM_HANDLE)
    {
        stream_handler = A2dpFindStreamIDbyAddr(&(sep->remote_bd_addr));
        BT_A2DP_VAL_TRACE1(stream_handler);
        
        if (stream_handler != UNKNOWN_STREAM_HANDLE && 
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf <= BT_A2DP_WAIT_CNF_SDP_QUERY)
        {
            /* accept this connection indicaiton if we are trying to connect this device */
            GAVDP_EstablishConnectionRsp(sep, AVDTP_ERR_NO_ERROR);
            
            if(deviceName != 0)
                A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_OK, stream_handler, &device_addr, (U8*)deviceName->dev_name, deviceName->dev_name_len);
            else
                A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_OK, stream_handler, &device_addr, NULL, 0);
            
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;
        }
        else 
        {
            GAVDP_EstablishConnectionRsp(sep, AVDTP_ERR_IN_USE);
        }
        return BT_STATUS_FAILED;
    }
    
    /* create a data link for FM over BT and CMGR_QoS */
    status = CMGR_CreateDataLink(&bt_a2dp_ctx.a2dp_device_list[stream_handler].cmgr_hdlr, &sep->remote_bd_addr);

    BT_A2DP_VAL_TRACE1(status);

    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res == BT_A2DP_WAIT_RES_NONE && 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_NONE &&
        (status == BT_STATUS_PENDING || status == BT_STATUS_SUCCESS))
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_OPEN;
        bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep = sep;

        if(deviceName != 0)
            A2dpHandleStreamOpenInd(stream_handler, &device_addr, (U8*)deviceName->dev_name, deviceName->dev_name_len);
        else
            A2dpHandleStreamOpenInd(stream_handler, &device_addr, NULL, 0);

        /* store device address in device list */
        bt_a2dp_get_bd_addr(&device_addr, &bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr);
    }
    else
    {
        GAVDP_EstablishConnectionRsp(sep, AVDTP_ERR_BAD_STATE);

        if (stream_handler != UNKNOWN_STREAM_HANDLE)
            A2dpReleaseStreamHandler(stream_handler);
        
        status == BT_STATUS_FAILED;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamOpenRes
 *
 * DESCRIPTION
 *      reponse a2dp open indication
 *
 * PARAMETERS
 *      result                 [IN]     
 *      stream_handle     [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamOpenRes(U16 result, U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 ErrCode = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_OPEN_RES, result, stream_handle, bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_OPEN)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;

        if (result == BT_A2DP_RESULT_OK)
        {
            ErrCode = AVDTP_ERR_NO_ERROR;       
        }
        else
        {
            ErrCode = AVDTP_ERR_BAD_STATE;
        }

        GAVDP_EstablishConnectionRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, ErrCode);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_NgeoConfig
 *
 * DESCRIPTION
 *      negotiate config paramter
 *
 * PARAMETERS
 *      local_codec              [IN]     
 *      remote_codec_list     [IN]     
 *
 * RETURNS
 *      BOOL
 *
 *****************************************************************************/

static BOOL A2DP_NgeoConfig (AvdtpCodec *local_codec, AvdtpCodec *remote_codec_list)
{
    if (NULL == local_codec || NULL == remote_codec_list)
        return FALSE;

    if (local_codec->elements[2] < remote_codec_list->elements[2]) // min bit pool
    {
        local_codec->elements[2] = remote_codec_list->elements[2];
    }

    if (local_codec->elements[3] > remote_codec_list->elements[3]) // max bit pool
    {
        local_codec->elements[3] = remote_codec_list->elements[3];
    }

    if (local_codec->elements[2] > local_codec->elements[3] || 
        local_codec->elements[2] <= 0 || 
        local_codec->elements[3] <= 0)
        
        return FALSE;
    
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamReconfigReq
 *
 * DESCRIPTION
 *      send out stream reconfig request
 *
 * PARAMETERS
 *      audio_cap           [IN]     
 *      stream_handle     [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamReconfigReq(U8 stream_handle, bt_a2dp_audio_cap_struct *audio_cap)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    StreamEndPoint *tmp_sep = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleStreamReconfigCnf( BT_A2DP_RESULT_PANIC, stream_handle );
        return;
    }

    BT_A2DP_FUNC_ENTRY2(BT_A2DP_RECONFIG_REQ, stream_handle, bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if (!(bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf == BT_A2DP_WAIT_CNF_NONE && 
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_NONE))
    {
        A2dpHandleStreamReconfigCnf( BT_A2DP_RESULT_BAD_STATE, stream_handle);
        return;
    }

    tmp_sep = bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep;
    
    a2dp_convert_codec((A2dpCodec*)&tmp_sep->local_cfg_codec, audio_cap);

    if ( FALSE == A2DP_NgeoConfig(&tmp_sep->local_cfg_codec, &tmp_sep->current_cfg_codec) )
    {
        OS_Report("[A2DP][A2DPStreamReconfigReq]no matched codec settings---"); 
        A2dpHandleStreamReconfigCnf( BT_A2DP_RESULT_SEP_IND_USE, stream_handle );
        return;
    }

    bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf = BT_A2DP_WAIT_CNF_RECONFIG;

    GAVDP_ChangeParametersReq(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamReconfigCnf
 *
 * DESCRIPTION
 *      handler reconfig confirm event
 *
 * PARAMETERS
 *      sep           [IN]     
 *      info           [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamReconfigCnf(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 result;
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    BT_A2DP_FUNC_ENTRY2(BT_A2DP_RECONFIG_CNF, stream_handler, info->error);
    
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);
    ASSERT(bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_RECONFIG);

    if (info->error == AVDTP_ERR_NO_ERROR)
    {
        result = BT_A2DP_RESULT_OK;
    }
    else
    {
        result = BT_A2DP_RESULT_TIMEOUT;
    }

    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;

    A2dpHandleStreamReconfigCnf( result, stream_handler);
}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamReconfigInd
 *
 * DESCRIPTION
 *      handler reconfig indication event
 *
 * PARAMETERS
 *      sep           [IN]     
 *      info           [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamReconfigInd(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
        
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    if( BtSysAdpVerifyInPanicState() )
    {
        return;
    }

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_RECONFIG_IND, stream_handler,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res == BT_A2DP_WAIT_RES_NONE && 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_NONE)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_RECONFIG;

        A2dpHandleStreamReconfigInd(stream_handler, (A2dpCodec *)&sep->codec);
    }
    else
    {
        GAVDP_ChangeParametersRsp(sep, AVDTP_RSP_REJECT);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamReconfigRes
 *
 * DESCRIPTION
 *      response reconfig indication
 *
 * PARAMETERS
 *      result                       [IN]     
 *      stream_handle           [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamReconfigRes(U16 result, U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 ErrCode = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_RECONFIG_RES, result, stream_handle, bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_RECONFIG)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;

        if (result == BT_A2DP_RESULT_OK)
        {
            ErrCode = AVDTP_ERR_NO_ERROR;
        }
        else
        {
            ErrCode = AVDTP_ERR_BAD_STATE;
        }

        GAVDP_ChangeParametersRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, ErrCode);
    }
}



/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamStartReq
 *
 * DESCRIPTION
 *      send out a stream start request to specific stream handler
 *
 * PARAMETERS  
 *      stream_handle           [IN]     
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamStartReq(U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);

    /* in order to handler: received close cnf before start request (ALPS00414770) */
    if (NULL == bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep)
    {
        A2dpHandleStreamStartCnf(BT_A2DP_RESULT_BAD_STATE, stream_handle, 0, NULL);
        return;
    }

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_START_REQ, stream_handle, 
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleStreamStartCnf( BT_A2DP_RESULT_PANIC, stream_handle, 0, NULL);

		if(bt_a2dp_ctx.fm_over_bt_mode == 1){
				bt_a2dp_ctx.fm_over_bt_mode = 0;
		}
        return;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_START)
    {
        /* this is the case that start_req comes between start_ind and start_res */
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
        CMGR_SetQoS(&bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr, 1);

        GAVDP_StartStreamRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, AVDTP_RSP_ACCEPT);
        status = BT_STATUS_PENDING;
    }
    else
    {
        if (!(bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf == BT_A2DP_WAIT_CNF_NONE && 
            bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_NONE))
        {
			if(bt_a2dp_ctx.fm_over_bt_mode == 1){
				bt_a2dp_ctx.fm_over_bt_mode = 0;
			}
            A2dpHandleStreamStartCnf(BT_A2DP_RESULT_BAD_STATE, stream_handle, 0, NULL);
            return;
        }
#if 1
        // Pierre: move FM over BT start to here
        U16 l2cap_mtu = 0;
        U16 l2cap_cid = 0;
        U16 AVDTP_sig_sid = 0;
        U16 AVDTP_sig_cid = 0;
        U16 features = 0x0;

        if((bt_a2dp_ctx.fm_over_sbc_enabled == 0)&&(bt_a2dp_ctx.fm_over_bt_mode == 1))
        {
			OS_Report(("[FM over BT] A2DP_GetPreferredPayloadSize start"));
            l2cap_mtu = A2DP_GetPreferredPayloadSize(stream_handle);
			OS_Report(("[FM over BT] A2DP_GetPreferredPayloadSize end"));
            AVDTP_sig_sid = bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep->signal_channel->conn.l2ChannelId;
			OS_Report(("[FM over BT] L2CAP_GetL2CAPRemoteCid start"));
            AVDTP_sig_cid = L2CAP_GetL2CAPRemoteCid(AVDTP_sig_sid);
			OS_Report(("[FM over BT] L2CAP_GetL2CAPRemoteCid end"));
			OS_Report(("[FM over BT] AVDTP_MediaGetChannelId start"));
            l2cap_cid = AVDTP_MediaGetChannelId(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep->stream);
			OS_Report(("[FM over BT] AVDTP_MediaGetChannelId end"));
            // change to remote
            OS_Report(("[FM over BT] L2CAP_GetL2CAPRemoteCid start"));
            l2cap_cid = L2CAP_GetL2CAPRemoteCid(l2cap_cid);
			OS_Report(("[FM over BT] L2CAP_GetL2CAPRemoteCid end"));
            //AVDTP_sig_cid = bt_a2dp_ctx.signal_channel.sigChnl.rxId;
            bt_a2dp_ctx.fm_over_sbc_enabled = 1;
			OS_Report(("[FM over BT] MeWriteEnableFMoverSBC start"));

            setSbcCodecElements();
            MeWriteEnableFMoverSBC(bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr.remDev, 
                                    l2cap_mtu,
                                    AVDTP_sig_sid,
                                    AVDTP_sig_cid,
                                    l2cap_cid,
                                    bt_a2dp_ctx.codec.elements,
                                    features);
			Report(("MeWriteEnableFMoverSBC end"));

       }

#endif
        CMGR_SetQoS(&bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr, 1);

        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf = BT_A2DP_WAIT_CNF_START;
        GAVDP_StartStreamReq(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);
        
        status = BT_STATUS_PENDING;
    }
    
    if (status != BT_STATUS_PENDING)
    {
        A2dpHandleStreamStartCnf( BT_A2DP_RESULT_BAD_STATE, stream_handle, 0, NULL);
        
        if(bt_a2dp_ctx.fm_over_bt_mode == 1){
            bt_a2dp_ctx.fm_over_bt_mode = 0;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamStartCnf
 *
 * DESCRIPTION
 *      handler streaming start confirm event
 *
 * PARAMETERS  
 *      sep           [IN]    
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static void A2dpStreamStartCnf(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 result, prefer_size;
    BtStatus status;
    BOOL codec_unpack;
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    bt_a2dp_audio_cap_struct current_config;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);

    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    BT_A2DP_FUNC_ENTRY6(BT_A2DP_START_CNF, stream_handler, info->error, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_started, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].postpond_suspend);
    
    if (info->error == GAVDP_ERR_NO_ERR)
    {
        result = BT_A2DP_RESULT_OK;
    }
    else
    {
        result = BT_A2DP_RESULT_TIMEOUT;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_START)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;
        
        prefer_size = A2DP_GetPreferredPayloadSize(stream_handler);
        codec_unpack = bt_adp_a2dp_unpack_audio_cap((A2dpCodec *) &sep->current_cfg_codec, &current_config);
        
        if((result == BT_A2DP_RESULT_OK) && codec_unpack)
            A2dpHandleStreamStartCnf(result, stream_handler, prefer_size, &current_config);
        else{
			if(bt_a2dp_ctx.fm_over_bt_mode == 1){
				bt_a2dp_ctx.fm_over_bt_mode = 0;
			}
            A2dpHandleStreamStartCnf(result, stream_handler, 0, NULL);
        }
    }
    else
    {
        if(bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_started)
        {
            bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_started = FALSE;

            /* this is the case that suspend_req comes between start_res and started callback */
            if( bt_a2dp_ctx.a2dp_device_list[stream_handler].postpond_suspend )
            {
                bt_a2dp_ctx.a2dp_device_list[stream_handler].postpond_suspend = FALSE;

                GAVDP_SuspendStreamReq(bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep);
            }
        }
    }
}



/*****************************************************************************
 * FUNCTION
 *      A2dpStreamStartInd
 *
 * DESCRIPTION
 *      handler streaming start indication event
 *
 * PARAMETERS  
 *      sep           [IN]    
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamStartInd(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    U16 prefer_size = 0;
    bt_a2dp_audio_cap_struct current_config;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_START_IND, stream_handler, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        return;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res == BT_A2DP_WAIT_RES_NONE && 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_NONE)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_START;

        prefer_size = A2DP_GetPreferredPayloadSize(stream_handler);
        bt_adp_a2dp_unpack_audio_cap((A2dpCodec *) &sep->current_cfg_codec, &current_config);
        
        A2dpHandleStreamStartInd(stream_handler, prefer_size, &current_config);
    }
    else
    {
        GAVDP_StartStreamRsp(bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep, AVDTP_RSP_REJECT);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamStartRes
 *
 * DESCRIPTION
 *      response streaming start indiacation
 *
 * PARAMETERS  
 *      result                       [IN]    
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamStartRes(U16 result, U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 ErrCode = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (A2dpGetState() < BT_A2DP_STATE_OPEN) return;
    
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_START_RES, result, stream_handle,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_START)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;

        if (result == BT_A2DP_RESULT_OK)
        {
            ErrCode = AVDTP_ERR_NO_ERROR;

            bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_started = TRUE;
            CMGR_SetQoS(&bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr, 1);
        }
        else
        {
            ErrCode = AVDTP_ERR_BAD_STATE;
        }
#if 0
        // Pierre
        // AVDTP_StartStreamRsp(accept, &bt_a2dp_ctx.stream, (AvdtpError) result);
        if(result == BT_A2DP_RESULT_OK)
        {
            U16 l2cap_mtu = 0;
            U16 l2cap_cid = 0;
            U16 AVDTP_sig_sid = 0;
            U16 AVDTP_sig_cid = 0;
            U16 features = 0x0;

            if(bt_a2dp_ctx.fm_over_sbc_enabled == 0)
            {
                bt_a2dp_ctx.fm_over_sbc_enabled = 1;
                l2cap_mtu = A2DP_GetPreferredPayloadSize(stream_handle);
                //AVDTP_sig_cid = bt_a2dp_ctx.signal_channel.sigChnl.rxId; // 0x7
                AVDTP_sig_sid = bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep.signal_channel.conn.l2ChannelId;
                AVDTP_sig_cid = L2CAP_GetL2CAPRemoteCid(AVDTP_sig_sid);
                l2cap_cid = AVDTP_MediaGetChannelId(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep.stream);
                // change to remote
                l2cap_cid = L2CAP_GetL2CAPRemoteCid(l2cap_cid);
                setSbcCodecElements();
                MeWriteEnableFMoverSBC(bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr.remDev, 
                                        l2cap_mtu,
                                        AVDTP_sig_sid,
                                        AVDTP_sig_cid,
                                        l2cap_cid,
                                        bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep.codec.elements,
                                        features);
            }
            // AVDTP_StartStreamRsp(accept, &bt_a2dp_ctx.stream, (AvdtpError) result);
        }
#endif
        // Pierre move to here
        GAVDP_StartStreamRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, ErrCode);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamSuspendReq
 *
 * DESCRIPTION
 *      send stream suspend command to specific stream
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamSuspendReq(U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    // BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_SUSPEND_REQ, stream_handle, 
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_started);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleStreamSuspendCnf( BT_A2DP_RESULT_PANIC, stream_handle );
        return;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_SUSPEND)
    {
        /* this is the case suspend_req comes between suspend_ind and suspend_res */
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
        GAVDP_SuspendStreamRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, AVDTP_RSP_ACCEPT);
        A2dpHandleStreamSuspendCnf( BT_A2DP_RESULT_OK, stream_handle );
        return;
    }
    else
    {
        if (!(bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf == BT_A2DP_WAIT_CNF_NONE && 
            bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_NONE))
        {
            /* check if we need to postponse suspend until stream is started */
            if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_started &&
              bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf == BT_A2DP_WAIT_CNF_NONE && 
              bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_NONE)
            {
                /* this is the case that suspend_req comes between start_res and started callback */
                bt_a2dp_ctx.a2dp_device_list[stream_handle].postpond_suspend = TRUE;
            }
            else
            {
                A2dpHandleStreamSuspendCnf( BT_A2DP_RESULT_BAD_STATE, stream_handle);
            }
            return;
        }

        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf = BT_A2DP_WAIT_CNF_SUSPEND;
        GAVDP_SuspendStreamReq(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    }

}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamSuspendCnf
 *
 * DESCRIPTION
 *      handler suspend confirm event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamSuspendCnf(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 result;
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_SUSPEND_CNF, stream_handler, info->error, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    
    if (info->error == GAVDP_ERR_NO_ERR)
    {
        result = BT_A2DP_RESULT_OK;
    }
    else
    {
        result = BT_A2DP_RESULT_TIMEOUT;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_SUSPEND)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;

        A2dpHandleStreamSuspendCnf( result, stream_handler);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamSuspendInd
 *
 * DESCRIPTION
 *      handler suspend indication event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamSuspendInd(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_SUSPEND_IND, stream_handler, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        return;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res == BT_A2DP_WAIT_RES_NONE && 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_NONE)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_SUSPEND;

        A2dpHandleStreamSuspendInd(stream_handler);
    }
    else
    {
        GAVDP_SuspendStreamRsp(bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep, AVDTP_RSP_REJECT);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamSuspendRes
 *
 * DESCRIPTION
 *      response suspend command
 *
 * PARAMETERS   
 *      result                       [IN]
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamSuspendRes(U16 result, U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 ErrCode = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_SUSPEND_RES, result, stream_handle,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_SUSPEND)
    {

        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;

        if (result == BT_A2DP_RESULT_OK)
        {
            ErrCode = AVDTP_ERR_NO_ERROR;
        }
        else
        {
            ErrCode = AVDTP_ERR_BAD_STATE;
        }

        GAVDP_SuspendStreamRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, ErrCode);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamCloseReq
 *
 * DESCRIPTION
 *      send stream close request to specific stream 
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamCloseReq(U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    U8 wait_res = 0;
    StreamEndPoint *tmp_sep;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
	/*[JM]ALPS01043500:replace assertion to 'return' to handle situation of close requestion triggerd during close indication processing*/
    //ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);
    if(NULL == bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep)
		return;
	/*[JM] end*/
    wait_res = bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res;
    tmp_sep = bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep;

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_CLOSE_REQ, stream_handle, 
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);

    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf != BT_A2DP_WAIT_CNF_NONE)
    {
        A2dpHandleStreamCloseCnf( BT_A2DP_RESULT_BAD_STATE, stream_handle);
        return;
    }

    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleStreamCloseCnf( BT_A2DP_RESULT_PANIC, stream_handle );
        return;
    }
    
    /* cancel outstanding ind */
    if (wait_res > BT_A2DP_WAIT_RES_OPEN)
    {
        ASSERT(wait_res >= BT_A2DP_WAIT_RES_RECONFIG);

        switch (wait_res)
        {
            case BT_A2DP_WAIT_RES_RECONFIG:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_ChangeParametersRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
                
            case BT_A2DP_WAIT_RES_START:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_StartStreamRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
                
            case BT_A2DP_WAIT_RES_SUSPEND:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_SuspendStreamRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
        }
    }

    bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf = BT_A2DP_WAIT_CNF_CLOSE;
    GAVDP_ReleaseConnectionReq(tmp_sep);
}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamCloseCnf
 *
 * DESCRIPTION
 *      handler close confirm event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamCloseCnf(StreamEndPoint *sep, GavdpCallbackParms *info)
{

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 result;
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    U8 wait_cnf = 0, wait_res = 0;
	bt_addr_struct device_addr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    //ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);
    if (stream_handler == UNKNOWN_STREAM_HANDLE) return;

    wait_res = bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res;
    wait_cnf = bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf;
    
    BT_A2DP_FUNC_ENTRY4(BT_A2DP_CLOSE_CNF, stream_handler, info->error, wait_res, wait_cnf);
    
    if (info->error == GAVDP_ERR_NO_ERR)
    {
        result = BT_A2DP_RESULT_OK;
    }
    else
    {
        result = BT_A2DP_RESULT_TIMEOUT;
    }

	/*[JM]ALPS01000553,handle all error situation which caused close confirm(AVDTP Disconnect)*/
	switch(wait_cnf)
    {
		case BT_A2DP_WAIT_CNF_DATA_LINK:
		case BT_A2DP_WAIT_CNF_SDP_QUERY:
		case BT_A2DP_WAIT_CNF_OPEN:
			bt_a2dp_get_bt_device_addr(&bt_a2dp_ctx.a2dp_device_list[stream_handler].remote_bd_addr,&device_addr);
			A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_TIMEOUT,stream_handler,&device_addr,NULL,0);
			break;
		case BT_A2DP_WAIT_CNF_RECONFIG:
			A2dpHandleStreamReconfigCnf(BT_A2DP_RESULT_TIMEOUT,stream_handler);
			break;
		case BT_A2DP_WAIT_CNF_START:
			A2dpHandleStreamStartCnf(BT_A2DP_RESULT_TIMEOUT,stream_handler,0,NULL);
			break;
		case BT_A2DP_WAIT_CNF_SUSPEND:
			A2dpHandleStreamSuspendCnf(BT_A2DP_RESULT_TIMEOUT,stream_handler);
			break;
		case BT_A2DP_WAIT_CNF_CLOSE:
        A2dpHandleStreamCloseCnf( result, stream_handler);
			break;
		case BT_A2DP_WAIT_CNF_ABORT:
			A2dpHandleStreamAbortCnf(stream_handler);
			break;
		default:
			break;
    }
	
	switch(wait_res)
    {
		case BT_A2DP_WAIT_RES_OPEN:
			A2DP_StreamOpenRes(BT_A2DP_RESULT_TIMEOUT,stream_handler);
			break;
	    case BT_A2DP_WAIT_RES_RECONFIG:
			A2DP_StreamReconfigRes(BT_A2DP_RESULT_TIMEOUT,stream_handler);
			break;
		case BT_A2DP_WAIT_RES_START:
			A2DP_StreamStartRes(BT_A2DP_RESULT_TIMEOUT,stream_handler);
			break;
		case BT_A2DP_WAIT_RES_SUSPEND:
			A2DP_StreamSuspendRes(BT_A2DP_RESULT_TIMEOUT,stream_handler);
			break;
	    case BT_A2DP_WAIT_RES_CLOSE:
        A2dpCmgrLinkClose(stream_handler);
			break;
	    case BT_A2DP_WAIT_RES_ABORT:
			A2DP_StreamAbortRes(stream_handler);
			break;
		default:
			break;
    }
	
    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_NONE;
    bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;

    /* for disonnect req while streaming, some earphone will send closed first and then disconnected
       so we shall ignore this close ind in this case */

    if ((wait_cnf != BT_A2DP_WAIT_CNF_CLOSE || wait_res != BT_A2DP_WAIT_RES_NONE) 
		&& wait_res != BT_A2DP_WAIT_RES_CLOSE)
    {
		/*[JM]after most error situation handled, close indication need to be report to upper layer except these 2:
			1.local device wait close confirm(error situation happened during 'close request')
			2.remote device send close indication and connection lost immediately*/
		
        /*for some earphone will send stream closed directly to handset (not close ind)*/
        A2dpHandleStreamCloseInd(stream_handler);
        A2dpCmgrLinkClose(stream_handler);
        return;
    }
    
    if (info->error == GAVDP_ERR_NO_ERR)
    {
        A2dpCmgrLinkClose(stream_handler);
    }

}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamCloseInd
 *
 * DESCRIPTION
 *      handler close indication event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamCloseInd(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_CLOSE_IND, stream_handler, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        return;
    }

    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_CLOSE)
    {
        A2dpHandleStreamCloseCnf( BT_A2DP_RESULT_OK, stream_handler);
        return;
    }
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res == BT_A2DP_WAIT_RES_NONE && 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_NONE)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res = BT_A2DP_WAIT_RES_CLOSE;

        A2dpHandleStreamCloseInd(stream_handler);
    }
    else
    {
        GAVDP_ReleaseConnectionRsp(bt_a2dp_ctx.a2dp_device_list[stream_handler].local_sep, AVDTP_RSP_REJECT);
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamCloseRes
 *
 * DESCRIPTION
 *      send stream close response
 *
 * PARAMETERS   
 *      result                       [IN]
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamCloseRes(U16 result, U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 ErrCode = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_CLOSE_RES, result, stream_handle,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_CLOSE)
    {
        // bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;

        if (result == BT_A2DP_RESULT_OK)
        {
            ErrCode = AVDTP_ERR_NO_ERROR;
        }
        else
        {
            ErrCode = AVDTP_ERR_BAD_STATE;
        }

        ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);
        GAVDP_ReleaseConnectionRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep, ErrCode);
#if 0 // moved to close confirm function
        if (result == BT_A2DP_RESULT_OK)
        {
            A2dpCmgrLinkClose(stream_handle);
        }
#endif
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamAbortReq
 *
 * DESCRIPTION
 *      send stream abort request
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamAbortReq(U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    U8 wait_res;
    StreamEndPoint *tmp_sep;
    

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_ABORT_REQ, stream_handle, 
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    wait_res = bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res;
    tmp_sep = bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep;
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf != BT_A2DP_WAIT_CNF_NONE)
    {
        A2dpHandleStreamAbortCnf(stream_handle);
        return;
    }

    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleStreamAbortCnf(stream_handle);
        return;
    }

    /* cancel outstanding ind */
    if (wait_res >= BT_A2DP_WAIT_RES_OPEN)
    {
        switch (wait_res)
        {
            case BT_A2DP_WAIT_RES_OPEN:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_EstablishConnectionRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
                
            case BT_A2DP_WAIT_RES_RECONFIG:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_ChangeParametersRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
                
            case BT_A2DP_WAIT_RES_START:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_StartStreamRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
                
            case BT_A2DP_WAIT_RES_SUSPEND:
                
                bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;
                GAVDP_SuspendStreamRsp(tmp_sep, AVDTP_RSP_REJECT);
                break;
        }
    }

    bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf = BT_A2DP_WAIT_CNF_ABORT;

    GAVDP_AbortReq(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);

}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamAbortCnf
 *
 * DESCRIPTION
 *      handler stream abort confirm event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamAbortCnf(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handler = UNKNOWN_STREAM_HANDLE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    stream_handler = A2dpFindStreamIDbySep(sep);
    ASSERT(stream_handler != UNKNOWN_STREAM_HANDLE);

    BT_A2DP_FUNC_ENTRY4(BT_A2DP_ABORT_CNF, stream_handler, info->error, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_res, 
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf == BT_A2DP_WAIT_CNF_ABORT)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handler].wait_cnf = BT_A2DP_WAIT_CNF_NONE;

        A2dpHandleStreamAbortCnf(stream_handler);
        // A2dpReleaseStreamHandler(stream_handler)
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2dpStreamAbortInd
 *
 * DESCRIPTION
 *      handler stream abort indication event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamAbortInd(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handle = UNKNOWN_STREAM_HANDLE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    stream_handle = A2dpFindStreamIDbySep(sep);
    // ASSERT(stream_handle != UNKNOWN_STREAM_HANDLE);
    if(stream_handle == UNKNOWN_STREAM_HANDLE) return;

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_ABORT_IND, stream_handle, 
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if( BtSysAdpVerifyInPanicState() )
    {
        return;
    }

    bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_ABORT;

    A2dpHandleStreamAbortInd(stream_handle);
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_StreamAbortRes
 *
 * DESCRIPTION
 *      send stream abort reponse
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_StreamAbortRes(U8 stream_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);
    /*handle the case: abort ind -> close cnf -> abort res */
    // ASSERT(NULL != bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);
    if (NULL == bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep)
        return;
    
    BT_A2DP_FUNC_ENTRY3(BT_A2DP_ABORT_RES, stream_handle,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res,
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_cnf);
    
    if (bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res == BT_A2DP_WAIT_RES_ABORT)
    {
        bt_a2dp_ctx.a2dp_device_list[stream_handle].wait_res = BT_A2DP_WAIT_RES_NONE;

        GAVDP_AbortRsp(bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep);
    }
}



/*****************************************************************************
 * FUNCTION
 *      A2dpStreamDelaySendQosInd
 *
 * DESCRIPTION
 *      send qos to upper layer when timer out
 *
 * PARAMETERS   
 *      timer           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamDelaySendQosInd(EvmTimer *timer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    BT_A2DP_FUNC_ENTRY(BT_A2DP_DELAY_SEND_QOS);
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_a2dp_ctx.qos_timer.func = 0;
    bt_a2dp_ctx.qos_reported = FALSE;  

    A2dpHandleStreamQosInd(0,(U8) bt_a2dp_ctx.qos_index);
    
}

/*****************************************************************************
 * FUNCTION
 *      A2dpStreamSentDataCallback
 *
 * DESCRIPTION
 *      handler stream data sent event, used for caculate qos
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

static void A2dpStreamSentDataCallback(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 stream_handle = UNKNOWN_STREAM_HANDLE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_a2dp_mem_free(info->p.data);
    
    stream_handle = A2dpFindStreamIDbySep(sep);
    ASSERT(stream_handle != UNKNOWN_STREAM_HANDLE);
    
    bt_a2dp_ctx.a2dp_device_list[stream_handle].packet_buffer_count--;

    if(info != 0)
        A2dpHandleStreamDataSentOut(0);

}


/*****************************************************************************
 * FUNCTION
 *      A2DP_GetPreferredPayloadSize
 *
 * DESCRIPTION
 *      return the prefered payload size of a2dp packet
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

U16 A2DP_GetPreferredPayloadSize(U8 stream_handle)
{
    U16 payload_size;

    payload_size = (U16)AVDTP_MediaPacketSize(&bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep->stream) - 13;

    return payload_size;
}



 /*****************************************************************************
 * FUNCTION
 *      A2DP_GetStreamBuffer
 *
 * DESCRIPTION
 *      This function returns a buffer for storing stream data. This function will 
 *      be called by A2DP Adaption Layer when L1 arises a request to send stream 
 *      data. Note that the buffer must be allocated and freed by A2DP Profile.
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *      buffer_size                [IN]       The buffer size to be allocted.
 *      buffer                      [OUT]     The allocated buffer.
 *
 * RETURNS
 *      BT_A2DP_RESULT_OK                         Successful.
 *      BT_A2DP_RESULT_FATAL_ERROR          Fatal error
 *
 *****************************************************************************/
BT_A2DP_RESULT_ENUM A2DP_GetStreamBuffer(U8 stream_handle, U32 buffer_size, U8 **buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/   
    U8 *packet_buffer;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    packet_buffer = bt_a2dp_mem_alloc(buffer_size + 12);
    ASSERT(packet_buffer != NULL);
    
    if (packet_buffer == NULL)
    {
        /* Out of memory? */
        return BT_A2DP_RESULT_FATAL_ERROR;
    }
    else
    {
        /* The first 12 bytes are reserved for media header. */
        *buffer = packet_buffer + 12;
        
        return BT_A2DP_RESULT_OK;
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_SendStreamData
 *
 * DESCRIPTION
 *      used to send stream data to specified stream hanldle
 *      this function will also caculate qos
 *
 * PARAMETERS   
 *      stream_handle           [IN]
 *      data                        [IN]     a2dp data
 *      len                          [IN]
 *      sample_count            [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
void A2DP_SendStreamData(U8 stream_handle, U8 *data, U32 len, U32 sample_count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *buffer, i;
    U8 qos_index = 0;
    U16 glb_packet_count = 0;
    BT_A2DP_RESULT_ENUM result;

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
#ifdef A2DP_STREAM_DATA_DEBUG
        BT_A2DP_FUNC_ENTRY4(BT_A2DP_SEND_STREAM_DATA, i, bt_a2dp_ctx.a2dp_device_list[i].in_use,
            bt_a2dp_ctx.a2dp_device_list[i].local_sep->stream_state, len);
#endif       
        if (bt_a2dp_ctx.a2dp_device_list[i].in_use == KAL_TRUE &&
            bt_a2dp_ctx.a2dp_device_list[i].local_sep->stream_state == GAVDP_STATE_STREAMING)
        {
            //allocate stream buffer size (preferred_payload_size + 12)
            result = A2DP_GetStreamBuffer(i, len, &buffer);
            if (result != BT_A2DP_RESULT_OK)
            {
                /* This could happen only if there is no more memory available */
                return;
            }

            /* Get payload */
            memcpy(buffer, data, len);
 
            result = A2DP_StreamSendDataReq(i, buffer, len, sample_count);
#ifdef A2DP_STREAM_DATA_DEBUG
            BT_A2DP_VAL_TRACE1(result);
#endif          
            if (result != BT_A2DP_RESULT_OK)
            {
                //Report(("[A2DP] StreamSendDataReq result = %d", result));
            }

            bt_a2dp_ctx.a2dp_device_list[i].packet_buffer_count++;
            bt_a2dp_ctx.a2dp_device_list[i].data_timestamp += sample_count;
            bt_a2dp_ctx.a2dp_device_list[i].data_seq_no++;
            bt_a2dp_ctx.qos_counter++;

            if (bt_a2dp_ctx.a2dp_device_list[i].packet_buffer_count > glb_packet_count)
                glb_packet_count = bt_a2dp_ctx.a2dp_device_list[i].packet_buffer_count;
            
        }
    }



    /* QoS report */
    if (bt_a2dp_ctx.qos_counter >= QOS_COUNTER_HIGH_THRESHOLD ||
        glb_packet_count >= MAX_AVDTP_PKT_NO - 2 ||
        (bt_a2dp_ctx.qos_counter >= QOS_COUNTER_LOW_THRESHOLD && glb_packet_count >= 2))
    {
        if (glb_packet_count < 2)
        {
            qos_index = 0;
        }
        else if (glb_packet_count <= 3)
        {
            qos_index = 6;
        }
        else
        {
            qos_index = 10;
        }
        
        if(!((bt_a2dp_ctx.qos_reported) && (qos_index == 10)))
        {
            if(bt_a2dp_ctx.qos_timer.func == 0)
            {
               bt_a2dp_ctx.qos_index = qos_index;
               bt_a2dp_ctx.qos_timer.func = A2dpStreamDelaySendQosInd;
               EVM_StartTimer(&(bt_a2dp_ctx.qos_timer), 500);
            }
            
            /* Set QoS reported flag such that the QoS indicator won't be send if alreay sent out before */
            bt_a2dp_ctx.qos_reported = TRUE;      
        }
        
        bt_a2dp_ctx.qos_counter = 0;
        
    }
}

/*****************************************************************************
 * FUNCTION
 *         A2DP_StreamSendDataReq
 *
 * DESCRIPTION
 *        This function will be called by A2DP Adaption Layer when there is stream
 *        data to be send out.
 *        
 * PARAMETERS
 *       stream_handle           [IN]        Stream handle.           
 *       buffer                       [IN]        Stream data.
 *       buffer_size                [IN]        The size of stream data.
 *       sample_count            [IN]        Sample count for time stamp reference.
 *
 * RETURNS
 *      BT_A2DP_RESULT_OK                   Successful.
 *      BT_A2DP_RESULT_FATAL_ERROR          Failed to send stream data.
 *
 *****************************************************************************/
BT_A2DP_RESULT_ENUM A2DP_StreamSendDataReq(U8 stream_handle, U8 *buffer, U32 buffer_size, U32 sample_count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *packet_buffer = buffer - 12; /* Subtract offset of media header 
                                      * to get the starting address of 
                                      * the packet buffer. */
    

    GavdpRtpTag rtp_tag;
    GavdpError ErrCode;
    StreamEndPoint *sep;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ASSERT(/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM);

    sep = bt_a2dp_ctx.a2dp_device_list[stream_handle].local_sep;
    ASSERT(sep != NULL);
    
    /* This could happen if there is no any available payload to be sent.
     * We should free allocated memory automatically and return an error
     * to upper application.
     */
    if (buffer_size == 0)
    {
        bt_a2dp_mem_free( packet_buffer );
        return BT_A2DP_BAD_LENGTH;
    }

    /* fill in the rtp tag */
    rtp_tag.stream_handle = stream_handle;
    rtp_tag.data_seq_no = bt_a2dp_ctx.a2dp_device_list[stream_handle].data_seq_no;
    rtp_tag.data_timestamp = bt_a2dp_ctx.a2dp_device_list[stream_handle].data_timestamp;
    
    ErrCode = GAVDP_StreamSendDataReq(sep, &rtp_tag, buffer, buffer_size);
    
    if (ErrCode != GAVDP_ERR_NO_ERR)
    {
        bt_a2dp_mem_free( packet_buffer );
        return BT_A2DP_RESULT_FATAL_ERROR;
    }
    
    return BT_A2DP_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *      A2dpGavdpEventCallback
 *
 * DESCRIPTION
 *      this function is used to handle GAVDP callback event
 *
 * PARAMETERS   
 *      sep           [IN]
 *      info           [IN]    
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/
static void A2dpGavdpEventCallback(StreamEndPoint *sep, GavdpCallbackParms *info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    GavdpEvent event = info->event;
    bt_sep_info_struct *stream_info;
    U8 stream_handle = UNKNOWN_STREAM_HANDLE;
    U8 device_type = 0;
    U8 result = 0;
    struct sched_param param;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    BT_A2DP_FUNC_ENTRY1(BT_A2DP_GAVDP_EVENT_CALLBACK, event);
    
    switch (event)
    {
        case GAVDP_EVENT_STREAM_ESTABLISH_CNF:

            A2dpStreamOpenCnf(sep, info);

            if(info->error == GAVDP_ERR_NO_ERR) {
                stream_handle = A2dpFindStreamIDbySep(sep);
                if (/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM 
                    && bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr.remDev)
                CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr);
            }

            break;
            
        case GAVDP_EVENT_STREAM_ESTABLISH_IND:

            result = A2dpStreamOpenInd(sep, info);

            if (result != BT_STATUS_SUCCESS)
                break;
        	  
            device_type = BTBMGetDeviceModel(&sep->remote_bd_addr);
            switch (device_type)
            {
                case DEVICE_WIREVO_HSH200:
                    // Report(("WIREVO_HSH200 no role switch"));
                    break;
                default:
                    if(info->error == GAVDP_ERR_NO_ERR) {
                        stream_handle = A2dpFindStreamIDbySep(sep);
						if(stream_handle < MAX_A2DP_DEV_NUM)//To resolve Klockwork CR ALPS01395987
						{
                        	bt_a2dp_ctx.a2dp_device_list[stream_handle].device_type = device_type;
                        	if (bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr.remDev)
                            	CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr);
						}
                    }
                    break;
            }
            
            break;

        case GAVDP_EVENT_STREAM_START_CNF:
            
            A2dpStreamStartCnf(sep, info);

            if(info->error == GAVDP_ERR_NO_ERR) {
                stream_handle = A2dpFindStreamIDbySep(sep);
                BT_A2DP_FUNC_ENTRY1(BT_A2DP_STREAM_HANDLE, stream_handle);
                
                if (/*stream_handle >= 0 &&*/ stream_handle < MAX_A2DP_DEV_NUM) {
                    bt_a2dp_ctx.a2dp_device_list[stream_handle].device_type = BTBMGetDeviceModel(&sep->remote_bd_addr);
                    CMGR_SetDeviceAsMaster(&bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr);
                }

                // increase the priority of mtkbt 
            	param.sched_priority = 95;
            	kal_prompt_trace(MOD_BT, "[A2DP] increase the priority of mtkbt.");
            	  
            	sched_setscheduler(0, SCHED_RR, &param);
                SetPriorityByName("mtk_stp_psm", SCHED_RR, 95);
                SetPriorityByName("mtk_wmtd", SCHED_RR, 95);
            }

            break;
            
        case GAVDP_EVENT_STREAM_START_IND:

            A2dpStreamStartInd(sep, info);

            // increase the priority of mtkbt 
            param.sched_priority = 95;
            kal_prompt_trace(MOD_BT, "[A2DP] increase the priority of mtkbt.");

            sched_setscheduler(0, SCHED_RR, &param);
            SetPriorityByName("mtk_stp_psm", SCHED_RR, 95);
            SetPriorityByName("mtk_wmtd", SCHED_RR, 95);

            break;

        case GAVDP_EVENT_CHANGE_PARAMETERS_CNF:

            A2dpStreamReconfigCnf(sep, info);
            break;
            
        case GAVDP_EVENT_CHANGE_PARAMETERS_IND:

            A2dpStreamReconfigInd(sep, info);
            break;
            
        case GAVDP_EVENT_STREAM_SUSPEND_CNF:
            
            A2DP_StopFMOverSBC(info->p.remDev);
            A2dpStreamSuspendCnf(sep, info);

            // decrease the priority of mtkbt 
            param.sched_priority = 0;
            sched_setscheduler(0, SCHED_NORMAL, &param);
            kal_prompt_trace(MOD_BT, "[A2DP] decrease the priority of mtkbt.");

            SetPriorityByName("mtk_stp_psm", SCHED_NORMAL, 0);
            SetPriorityByName("mtk_wmtd", SCHED_NORMAL, 0); 
            break;
            
        case GAVDP_EVENT_STREAM_SUSPEND_IND:
            
            A2DP_StopFMOverSBC(info->p.remDev);
            A2dpStreamSuspendInd(sep, info);

            // decrease the priority of mtkbt 
            param.sched_priority = 0;
            sched_setscheduler(0, SCHED_NORMAL, &param);
            kal_prompt_trace(MOD_BT, "[A2DP] decrease the priority of mtkbt.");

            SetPriorityByName("mtk_stp_psm", SCHED_NORMAL, 0);
            SetPriorityByName("mtk_wmtd", SCHED_NORMAL, 0); 
            break;
            
        case GAVDP_EVENT_STREAM_RELEASE_CNF:

            A2DP_StopFMOverSBC(info->p.remDev);
            if(bt_a2dp_ctx.fm_over_bt_mode == 1)
            {
                bt_a2dp_ctx.fm_over_bt_mode = 0;
            }
            A2dpStreamCloseCnf(sep, info);
            
            // decrease the priority of mtkbt 
            param.sched_priority = 0;
            sched_setscheduler(0, SCHED_NORMAL, &param);
            kal_prompt_trace(MOD_BT, "[A2DP] decrease the priority of mtkbt.");

            SetPriorityByName("mtk_stp_psm", SCHED_NORMAL, 0);
            SetPriorityByName("mtk_wmtd", SCHED_NORMAL, 0);      
        	  
            break;
            
        case GAVDP_EVENT_STREAM_RELEASE_IND:

            A2DP_StopFMOverSBC(info->p.remDev);
            A2dpStreamCloseInd(sep, info);
            break;

        case GAVDP_EVENT_ABORT_CNF:

            A2DP_StopFMOverSBC(info->p.remDev);
            A2dpStreamAbortCnf(sep, info);
            break;
            
        case GAVDP_EVENT_ABORT_IND:

            A2DP_StopFMOverSBC(info->p.remDev);
            A2dpStreamAbortInd(sep, info);
            break;

        case GAVDP_EVENT_STREAM_PACKET_SENT:
            
            A2dpStreamSentDataCallback(sep, info);
            break;
            
        case GAVDP_EVENT_STREAM_PACKET_RECV:
            break;

        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_Activate
 *
 * DESCRIPTION
 *      this function is used active a2dp profile and register callback function to GAVDP
 *      1. register SDP record
 *      2. register SEP information to GAVDP
 *      3. malloc memory for GAVDP
 *      4. register CMGR callback function
 *
 * PARAMETERS   
 *      local_role           [IN]
 *      sep_num            [IN]    
 *      local_sep_list      [IN]
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_Activate(U8 local_role, U8 sep_num, bt_a2dp_audio_cap_struct *local_sep_list)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i = 0, j = 0;
    U8 local_service_id = 0;
    BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_A2DP_FUNC_ENTRY3(BT_A2DP_ACTIVATE, local_role, sep_num, bt_a2dp_ctx.is_a2dp_active);

    if (BtSysAdpVerifyInPanicState())
    {
        A2dpHandleActivateCnf(BT_A2DP_RESULT_PANIC);
        return;
    }
#if 0
    if (BTCoreVerifySysInitState() == FALSE)
    {
        A2dpHandleActivateCnf(BT_A2DP_RESULT_BAD_STATE);
        return;
    }       
#endif
    if (bt_a2dp_ctx.is_a2dp_active == TRUE)
    {
        A2dpHandleActivateCnf(BT_A2DP_RESULT_OK);
        return;
    }

    /* get SDP record attribs & add to SDP database */
    if (BT_A2DP_SOURCE == local_role)
    {
        bt_a2dp_ctx.sdp_record.num = SDPDB_GetLocalServiceAttribute( SC_AUDIO_SOURCE, &bt_a2dp_ctx.sdp_record.attribs, &local_service_id );
        SDP_AddRecord(&bt_a2dp_ctx.sdp_record);
    }
    else
    {
        bt_a2dp_ctx.sdp_record.num = SDPDB_GetLocalServiceAttribute( SC_AUDIO_SINK, &bt_a2dp_ctx.sdp_record.attribs, &local_service_id );
        SDP_AddRecord(&bt_a2dp_ctx.sdp_record);
    }

    bt_a2dp_ctx.local_sep_num = sep_num;
    for (i = 0; ((i < sep_num) && (i < MAX_A2DP_SEP_NUM)); i++)
    {
        //temp solution of ALPS00118063
        OS_MemSet((U8 *)&bt_a2dp_ctx.local_sep_List[i], 0, sizeof(StreamEndPoint));
	
        bt_a2dp_ctx.local_sep_List[i].codec.elements = get_ctrl_buffer(10);
        bt_a2dp_ctx.local_sep_List[i].local_cfg_codec.elements = get_ctrl_buffer(10);
        bt_a2dp_ctx.local_sep_List[i].remote_cfg_codec.elements = get_ctrl_buffer(10);
        bt_a2dp_ctx.local_sep_List[i].current_cfg_codec.elements = get_ctrl_buffer(10);

        for (j = 0; j < MAX_NUM_REMOTE_CAPS; j++)
            bt_a2dp_ctx.local_sep_List[i].remote_cap_info[j].elements = get_ctrl_buffer(10);

        /* convert audio cap to sep infor */
        if (bt_a2dp_ctx.local_sep_List[i].codec.elements != NULL)
        { 
            a2dp_convert_codec((A2dpCodec*)&bt_a2dp_ctx.local_sep_List[i].codec, &local_sep_list[i]);
        }
        
        bt_a2dp_ctx.local_sep_List[i].info.inUse = 0;
        bt_a2dp_ctx.local_sep_List[i].info.mediaType = AVDTP_MEDIA_TYPE_AUDIO;

        if (BT_A2DP_SOURCE == local_role)
            bt_a2dp_ctx.local_sep_List[i].info.streamType = AVDTP_STRM_ENDPOINT_SRC;
        else
            bt_a2dp_ctx.local_sep_List[i].info.streamType = AVDTP_STRM_ENDPOINT_SNK;

        /* set the GAVDP callback function */
        bt_a2dp_ctx.local_sep_List[i].callback = A2dpGavdpEventCallback;

        GAVDP_Register(&bt_a2dp_ctx.local_sep_List[i]);
    }

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        bt_a2dp_ctx.a2dp_device_list[i].local_sep = NULL;

        CMGR_RegisterHandler(&bt_a2dp_ctx.a2dp_device_list[i].cmgr_hdlr, A2dpCmgrEventCallback);
    }

    bt_a2dp_ctx.is_a2dp_active = TRUE;
    
    A2dpHandleActivateCnf(BT_A2DP_RESULT_OK);
}


/*****************************************************************************
 * FUNCTION
 *      A2DP_Deactivate
 *
 * DESCRIPTION
 *      this function is used deactive a2dp profile 
 *      1. deregister SDP record
 *      2. deregister SEP information to GAVDP
 *      3. free memory 
 *      4. deregister CMGR callback function
 *
 * PARAMETERS   
 *      void
 *
 * RETURNS
 *      void
 *
 *****************************************************************************/

void A2DP_Deactivate(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    U8 i = 0, j = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    BT_A2DP_FUNC_ENTRY1(BT_A2DP_DEACTIVATE, bt_a2dp_ctx.is_a2dp_active);
    
#if 0    
    if( BtSysAdpVerifyInPanicState() )
    {
        A2dpHandleDeactivateCnf(BT_A2DP_RESULT_PANIC);
        return;
    }

    if( BTCoreVerifySysInitState() == FALSE )
    {
        A2dpHandleDeactivateCnf(BT_A2DP_RESULT_BAD_STATE);
        return;
    }    
#endif
    if (bt_a2dp_ctx.is_a2dp_active == FALSE)
    {
        A2dpHandleDeactivateCnf(BT_A2DP_RESULT_OK);
        return;
    }

    //temp solution of ALPS00118063
    for (i = 0; i < MAX_A2DP_SEP_NUM; i++)
    {
        if ( BT_STATUS_SUCCESS == GAVDP_DeRegister(&bt_a2dp_ctx.local_sep_List[i]))
        {
        if (bt_a2dp_ctx.local_sep_List[i].codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].codec.elements);

        if (bt_a2dp_ctx.local_sep_List[i].local_cfg_codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].local_cfg_codec.elements);

        if (bt_a2dp_ctx.local_sep_List[i].remote_cfg_codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].remote_cfg_codec.elements);

        if (bt_a2dp_ctx.local_sep_List[i].current_cfg_codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].current_cfg_codec.elements);

        for (j = 0; j < MAX_NUM_REMOTE_CAPS; j++)
            if (bt_a2dp_ctx.local_sep_List[i].remote_cap_info[j].elements)
                free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].remote_cap_info[j].elements);
				
        OS_MemSet((U8 *)&bt_a2dp_ctx.local_sep_List[i], 0, sizeof(StreamEndPoint));				
        }
        else
        {
            // handle this case to avoid memory leak 
            A2dpHandleDeactivateCnf(BT_A2DP_RESULT_SEP_IND_USE);
            return;
		}		
    }
	/* remove cmgr data link */
    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        CMGR_RemoveDataLink(&bt_a2dp_ctx.a2dp_device_list[i].cmgr_hdlr);
        CMGR_RemoveCmgrWithDevice(&bt_a2dp_ctx.a2dp_device_list[i].cmgr_hdlr); 
        CMGR_DeregisterHandler(&bt_a2dp_ctx.a2dp_device_list[i].cmgr_hdlr);    
    }
/*	
    for (i = 0; i < MAX_A2DP_SEP_NUM; i++)
    {
        GAVDP_DeRegister(&bt_a2dp_ctx.local_sep_List[i]);

        if (bt_a2dp_ctx.local_sep_List[i].codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].codec.elements);

        if (bt_a2dp_ctx.local_sep_List[i].local_cfg_codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].local_cfg_codec.elements);

        if (bt_a2dp_ctx.local_sep_List[i].remote_cfg_codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].remote_cfg_codec.elements);

        if (bt_a2dp_ctx.local_sep_List[i].current_cfg_codec.elements)
            free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].current_cfg_codec.elements);

        for (j = 0; j < MAX_NUM_REMOTE_CAPS; j++)
            if (bt_a2dp_ctx.local_sep_List[i].remote_cap_info[j].elements)
                free_ctrl_buffer(bt_a2dp_ctx.local_sep_List[i].remote_cap_info[j].elements);
    }
*/

    /* remove SDP record */
    SDP_RemoveRecord(&bt_a2dp_ctx.sdp_record);

    bt_a2dp_ctx.is_a2dp_active = FALSE;

    OS_MemSet((U8 *)bt_a2dp_ctx.a2dp_device_list, 0, sizeof(A2dpDevContext) * MAX_A2DP_DEV_NUM);
    //temp solution of ALPS00118063	
    //OS_MemSet((U8 *)bt_a2dp_ctx.local_sep_List, 0, sizeof(StreamEndPoint) * MAX_A2DP_SEP_NUM);	

    GAVDP_DeInit();
    A2dpHandleDeactivateCnf(BT_A2DP_RESULT_OK);
    
}


BOOL A2DP_StreammingDataSendingVerify(void)
{
    U8 state = A2dpGetState();

    if (state == BT_A2DP_STATE_STREAMING)
        return TRUE;
    else
        return FALSE;

}

#if 0

void A2DP_Panic(void)
{    
    bt_addr_struct device_addr;
    U16 connect_id = (U16) bt_a2dp_ctx.connect_id;
    U8 wait_cnf = bt_a2dp_ctx.wait_cnf;

    BT_A2DP_FUNC_ENTRY3(BT_A2DP_PANIC, bt_a2dp_ctx.state, bt_a2dp_ctx.wait_cnf, bt_a2dp_ctx.wait_res);

    bt_a2dp_ctx.wait_cnf = BT_A2DP_WAIT_CNF_NONE;
    bt_a2dp_ctx.wait_res = BT_A2DP_WAIT_RES_NONE;

    bt_a2dp_get_bt_device_addr(&bt_a2dp_ctx.remote_bd_addr, &device_addr);

    switch (wait_cnf)
    {
        case BT_A2DP_WAIT_CNF_DATA_LINK:
        case BT_A2DP_WAIT_CNF_SDP_QUERY:
        case BT_A2DP_WAIT_CNF_CONNECT:
            A2dpHandleConnectCnf(0, BT_A2DP_RESULT_FATAL_ERROR, &device_addr);
            break;
        case BT_A2DP_WAIT_CNF_DISCONNECT:
            A2dpHandleDisconnectCnf(connect_id, BT_A2DP_RESULT_FATAL_ERROR);
            break;
        case BT_A2DP_WAIT_CNF_DISCOVER:
            A2dpHandleSepDiscoverCnf(connect_id, BT_A2DP_RESULT_FATAL_ERROR, 0, 0);
            break;
        case BT_A2DP_WAIT_CNF_GET_CAP:
            A2dpHandleCapabilitiesGetCnf(connect_id, BT_A2DP_RESULT_FATAL_ERROR);
            break;
        case BT_A2DP_WAIT_CNF_CONFIG:
            A2dpHandleStreamConfigCnf(0, BT_A2DP_RESULT_FATAL_ERROR, 0);
            break;
        case BT_A2DP_WAIT_CNF_OPEN:
            A2dpHandleStreamOpenCnf(BT_A2DP_RESULT_FATAL_ERROR, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_RECONFIG:
            A2dpHandleStreamReconfigCnf(BT_A2DP_RESULT_FATAL_ERROR, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_START:
            A2dpHandleStreamStartCnf(BT_A2DP_RESULT_FATAL_ERROR, bt_a2dp_ctx.stream.locStrmId, 0);
            break;
        case BT_A2DP_WAIT_CNF_SUSPEND:
            A2dpHandleStreamSuspendCnf(BT_A2DP_RESULT_FATAL_ERROR, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_CLOSE:
            A2dpHandleStreamCloseCnf(BT_A2DP_RESULT_FATAL_ERROR, bt_a2dp_ctx.stream.locStrmId);
            break;
        case BT_A2DP_WAIT_CNF_ABORT:
            A2dpHandleStreamAbortCnf(bt_a2dp_ctx.stream.locStrmId);
            break;
    }
}
#endif

void A2DP_Register(BTA2DPCallback Callback)
{
    bt_a2dp_ctx.a2dpCBfunc = Callback;
}

void a2dp_convert_codec(
                    A2dpCodec *dst, 
                    bt_a2dp_audio_cap_struct *src)
{
    U8 tmp;
    U8 dst_size;
    U8 codec_type = src->codec_type;
    U8 *codec = &dst->elements[0];

    switch (codec_type)
    {
        case BT_A2DP_SBC:
        {
            bt_a2dp_sbc_codec_cap_struct *sbc = &src->codec_cap.sbc;

            dst->codecType = AVDTP_CODEC_TYPE_SBC;

            tmp = sbc->sample_rate << 4;
            tmp |= sbc->channel_mode;
            codec[0] = tmp;

            tmp = sbc->block_len << 4;
            tmp |= sbc->subband_num << 2;
            tmp |= sbc->alloc_method;
            codec[1] = tmp;

            codec[2] = sbc->min_bit_pool;
            codec[3] = sbc->max_bit_pool;

            dst->elemLen = dst_size = 4;
            break;
        }
        case BT_A2DP_MP3:
        {
            bt_a2dp_mp3_codec_cap_struct *mp3 = &src->codec_cap.mp3;

            dst->codecType = AVDTP_CODEC_TYPE_MPEG1_2_AUDIO;

            tmp = mp3->layer << 5;
            tmp |= ((mp3->CRC == TRUE) ? 1 : 0) << 4;
            tmp |= mp3->channel_mode;
            codec[0] = tmp;

            tmp = ((mp3->MPF == TRUE) ? 1 : 0) << 6;
            tmp |= mp3->sample_rate;
            codec[1] = tmp;

            tmp = ((mp3->VBR == TRUE) ? 1 : 0) << 7;
            tmp |= (U8) ((mp3->bit_rate >> 8) & 0x7f);
            codec[2] = tmp;

            codec[3] = (U8) (mp3->bit_rate & 0xff);

            dst->elemLen = dst_size = 4;
            break;
        }
        case BT_A2DP_AAC:
        {
            bt_a2dp_aac_codec_cap_struct *aac = &src->codec_cap.aac;

            dst->codecType = AVDTP_CODEC_TYPE_MPEG2_4_AAC;

            codec[0] = aac->object_type;
            codec[1] = (U8) ((aac->sample_rate >> 4) & 0xff);

            tmp = (U8) (aac->sample_rate & 0xf) << 4;
            tmp |= aac->channels;
            codec[2] = tmp;

            tmp = ((aac->VBR == TRUE) ? 1 : 0) << 7;
            tmp |= (U8) ((aac->bit_rate >> 16) & 0x7f);
            codec[3] = tmp;

            codec[4] = (U8) ((aac->bit_rate >> 8) & 0xff);
            codec[5] = (U8) (aac->bit_rate & 0xff);

            dst->elemLen = dst_size = 6;
            break;
        }
        case BT_A2DP_ATRAC:
        {
            bt_a2dp_atrac_codec_cap_struct *atrac = &src->codec_cap.atrac;

            dst->codecType = AVDTP_CODEC_TYPE_ATRAC;

            tmp = atrac->version << 5;
            tmp |= atrac->channel_mode << 2;
            codec[0] = tmp;

            tmp = atrac->sample_rate << 4;
            tmp |= ((atrac->VBR == TRUE) ? 1 : 0) << 3;
            tmp |= (U8) ((atrac->bit_rate >> 16) & 0x7);
            codec[1] = tmp;

            codec[2] = (U8) ((atrac->bit_rate >> 8) & 0xff);
            codec[3] = (U8) (atrac->bit_rate & 0xff);
            codec[4] = (U8) ((atrac->max_sul >> 8) & 0xff);
            codec[5] = (U8) (atrac->max_sul & 0xff);
            codec[6] = 0;

            dst->elemLen = dst_size = 7;
            break;
        }
        default:
            dst_size = 0;
            ASSERT(0);  /* unsupported codec type */
            break;
    }
}


#ifdef BTMTK_ON_LINUX

U8 A2dpGetState(void)
{
    U8 i = 0;
    
    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        if (bt_a2dp_ctx.a2dp_device_list[i].local_sep)
        {
            if (bt_a2dp_ctx.a2dp_device_list[i].in_use == KAL_TRUE &&
                bt_a2dp_ctx.a2dp_device_list[i].local_sep->stream_state == GAVDP_STATE_STREAMING)
	            return BT_A2DP_STATE_STREAMING;
	            
            if (bt_a2dp_ctx.a2dp_device_list[i].in_use == KAL_TRUE &&
                bt_a2dp_ctx.a2dp_device_list[i].local_sep->stream_state == GAVDP_STATE_OPEN)
                    return BT_A2DP_STATE_OPEN;
        }
    }
    return BT_A2DP_STATE_ACTIVATED;
}

void A2DP_StopFMOverSBC(BtRemoteDevice *link)
{
#ifdef __FM_OVER_BT_CHIP__
    if (link != NULL)
    {
        BT_A2DP_FUNC_ENTRY4(BT_A2DP_STOP_FM, 0, bt_a2dp_ctx.fm_over_sbc_enabled, 
        bt_a2dp_ctx.fm_over_bt_mode,
        bt_a2dp_ctx.a2dp_device_list[0].cmgr_hdlr.remDev);
    }

    if(bt_a2dp_ctx.fm_over_bt_mode != 1)
		return;
    
    link = bt_a2dp_ctx.a2dp_device_list[0].cmgr_hdlr.remDev;
    
    if(link == NULL){
      return;
    }
    
    BT_A2DP_VAL_TRACE1(link->state);
    
    if(((link->state == BDS_CONNECTED)||(link->state == BDS_OUT_DISC)) && 
        (bt_a2dp_ctx.fm_over_sbc_enabled == 1))
    {
        MeWriteDisableFMoverSBC(link);
        bt_a2dp_ctx.fm_over_sbc_enabled = 0;
    }

#endif

}

void A2DP_FMContrllerStartReq(U8 stream_handle)
{
#ifdef __FM_OVER_BT_CHIP__

    bt_a2dp_ctx.fm_over_bt_mode = 1;
    BT_A2DP_FUNC_ENTRY1(BT_A2DP_START_FM, stream_handle);
	A2DP_StreamStartReq(stream_handle);

#endif
}
void A2DP_FMContrllerSuspendReq(U8 stream_handle)
{
#ifdef __FM_OVER_BT_CHIP__

	BT_A2DP_FUNC_ENTRY1(BT_A2DP_SUSPEND_FM, stream_handle);
	A2DP_StopFMOverSBC(bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr.remDev);
    
#endif
}
void A2DP_FMContrllerResumeReq(U8 stream_handle)
{
#ifdef __FM_OVER_BT_CHIP__

	BT_A2DP_FUNC_ENTRY1(BT_A2DP_RESUME_FM, stream_handle);
    A2DP_FMContrllerStopReq(stream_handle);
	A2DP_FMContrllerStartReq(stream_handle);

#endif
}
void A2DP_FMContrllerStopReq(U8 stream_handle)
{
#ifdef __FM_OVER_BT_CHIP__

    BT_A2DP_FUNC_ENTRY1(BT_A2DP_STOP_FM_REQ, stream_handle);
	A2DP_StopFMOverSBC(bt_a2dp_ctx.a2dp_device_list[stream_handle].cmgr_hdlr.remDev);
	A2DP_StreamSuspendReq(stream_handle);
	bt_a2dp_ctx.fm_over_bt_mode = 0;

#endif
}

static void setSbcCodecElements()
{
#ifdef __FM_OVER_BT_CHIP__

	// Pierre    
    BT_A2DP_FUNC_ENTRY2(BT_A2DP_FM_SET_CODEC_ELEMENT,
        bt_a2dp_ctx.a2dp_device_list[0].local_sep->codec.elemLen,
        bt_a2dp_ctx.a2dp_device_list[0].local_sep->codec.elements[0]);

    bt_a2dp_ctx.codec.elements = bt_a2dp_ctx.sbc_element;
    if(bt_a2dp_ctx.a2dp_device_list[0].local_sep->codec.elements[0] == 0x11){
    	bt_a2dp_ctx.codec.elements[0] = 0x11; // 48000Hz JOINT STEREO    
    }else{
    	bt_a2dp_ctx.codec.elements[0] = 0x12; // 48000Hz STEREO  
    }
    bt_a2dp_ctx.codec.elements[1] = 0x15;    
    bt_a2dp_ctx.codec.elements[2] = 0x13;    
    bt_a2dp_ctx.codec.elements[3] = 0x35;    
    //bt_a2dp_ctx.codec.elements[3] = 0x20; // for clip-D // 32 = 0x20

#endif
}

#endif
#endif /* __BT_A2DP_PROFILE__ */

