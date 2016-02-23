#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "playback.h"
#include "IMtkPb_Ctrl.h"
#include "IMtkPb_Misc.h"
#include "IMtkPb_Comm.h"
#include "IMtkPb_Info.h"
#include "IMtkPb_Ctrl_DTV.h"

#define CMPB_MAGIC_TAG 0x19860528
#define CMPB_PRIVATE_PS_BUF_SIZE (64 * 1024)

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#define NORMALMSG stderr

static int g_log_lvl = 5;

#define LOG(level, fmt...)\
    if(level <= g_log_lvl) {fprintf(NORMALMSG, fmt); fflush(NORMALMSG);}

#define ASSERT_NULL_POINTER(ptr)    \
                do{ \
                        if (!ptr){    \
                            LOG(0, "!!!got NULL pointer at [%d] line\n", __LINE__); \
                            return CMPB_ERROR;  \
                        }   \
                }while(0);

static IMTK_PB_HANDLE_T  g_handle = 0;
static char g_playFile[256] = {0};
static char g_ffFile[256] = {0};
static int g_speed = -1;
static event_callback_fct g_callback;
static IMTK_PB_CTRL_RECT_T g_display;
static int g_thread_running = 1;
static pthread_t g_thread;
static int g_startPushData = true;

static bool getMediaInfo(uint8_t * pu1buffer, uint  size, IMTK_PB_CTRL_SET_MEDIA_INFO_T * t_mediaInfo)
{
    ASSERT_NULL_POINTER(pu1buffer);
    ASSERT_NULL_POINTER(g_handle);
    LOG(5, "getMediaInfo fCMPBHandle = %x - push mode\n", g_handle);
    IMTK_PB_ERROR_CODE_T i4_ret = IMTK_PB_ERROR_CODE_OK;

    IMTK_PB_CTRL_TS_SINGLE_PAT_INFO_T t_tsInfo;
    memset(&t_tsInfo, 0, sizeof(t_tsInfo));

    i4_ret = IMtkPb_Ctrl_GetTSInfo(pu1buffer, size, &t_tsInfo);

    if ( i4_ret != IMTK_PB_ERROR_CODE_OK )
    {
        LOG(0 ,"IMtkPb_Ctrl_GetTSInfo ERROR i4_ret = %d- push mode\n", i4_ret);
        return false;
    }

    IMTK_PB_CTRL_VID_ENC_T  eVidEnc = IMTK_PB_CTRL_VID_ENC_UNKNOWN;
    IMTK_PB_CTRL_AUD_ENC_T  eAudEnc = IMTK_PB_CTRL_AUD_ENC_UNKNOWN;
    int32_t i4_Vid_pid = -1;
    int32_t i4_Aud_pid = -1;
    bool b_found = false;
    int audioTrackNum = 0;

    unsigned int i = 0;
    unsigned int j = 0;
    LOG(0, "\n------------- t_tsInfo.ui1_pmt_num : %d ----------------\n", t_tsInfo.ui1_pmt_num);
    for(i = 0; i < t_tsInfo.ui1_pmt_num; i++)
    {
        for(j = 0; j < t_tsInfo.at_pmt_info[i].ui1_strm_num; j++)
        {
            i4_Vid_pid = t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_pid;
            if (i4_Vid_pid < 0)
            {
                continue;
            }

            switch(t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type)
            {
            case 1://STREAM_TYPE_VIDEO_11172_2:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_MPEG1_2;
                break;

            case 2://STREAM_TYPE_VIDEO_13818_2:
            case 128://STREAM_TYPE_VIDEO_DIGICIPHER_2:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_MPEG1_2;
                break;

            case 16://STREAM_TYPE_VIDEO_14496_2:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_MPEG4;
                break;

            case 27://STREAM_TYPE_VIDEO_14496_10:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_H264;
                break;

            default:
                break;
            }
            if (IMTK_PB_CTRL_VID_ENC_UNKNOWN == eVidEnc)
            {
                continue;
            }

            b_found = true;
            break;
        }

        if (true == b_found)
            break;
    }

    b_found = false;
    for(i = 0; i < t_tsInfo.ui1_pmt_num; i++)
    {
        LOG(0, "-----------------t_tsInfo.at_pmt_info[%d].ui1_strm_num : %d -----------\n", i, t_tsInfo.at_pmt_info[i].ui1_strm_num);
        for(j = 0; j < t_tsInfo.at_pmt_info[i].ui1_strm_num; j++)
        {
            switch(t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type)
            {
            case 3://STREAM_TYPE_AUDIO_11172_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;
                break;

            case 4://STREAM_TYPE_AUDIO_13818_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;
                break;

            case 15://STREAM_TYPE_AUDIO_13818_7:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
                break;

            case 17://STREAM_TYPE_AUDIO_14496_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
                break;

            case 129://STREAM_TYPE_AUDIO_ATSC_A53:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                break;

            case 130://STREAM_TYPE_AUDIO_MSRT24:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                break;

            case 131://STREAM_TYPE_AUDIO_ATT_LABS_G729A:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                break;

            case 134://STREAM_TYPE_AUDIO_DTS_GSM610:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                break;

            default:
                break;
            }
            if (IMTK_PB_CTRL_AUD_ENC_UNKNOWN != eAudEnc)
            {
                audioTrackNum++;
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_UNKNOWN;
                continue;
            }
        }
    }
    LOG(0, "---------------- audioTrackNum : %d ----------------\n", audioTrackNum);
    for(i = 0; i < t_tsInfo.ui1_pmt_num; i++)
    {
        for(j = 0; j < t_tsInfo.at_pmt_info[i].ui1_strm_num; j++)
        {
            i4_Aud_pid = t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_pid;
            if (i4_Aud_pid < 0)
            {
                continue;
            }

            switch(t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type)
            {
            case 3://STREAM_TYPE_AUDIO_11172_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;
                break;

            case 4://STREAM_TYPE_AUDIO_13818_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;
                break;

            case 15://STREAM_TYPE_AUDIO_13818_7:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
                break;

            case 17://STREAM_TYPE_AUDIO_14496_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
                break;

            case 129://STREAM_TYPE_AUDIO_ATSC_A53:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                break;

            case 130://STREAM_TYPE_AUDIO_MSRT24:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                break;

            case 131://STREAM_TYPE_AUDIO_ATT_LABS_G729A:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                break;

            case 134://STREAM_TYPE_AUDIO_DTS_GSM610:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                break;

            default:
                break;
            }
            if (IMTK_PB_CTRL_AUD_ENC_UNKNOWN == eAudEnc)
            {
                continue;
            }

            b_found = true;
            break;
        }

        if (true == b_found)
            break;
    }

    memset(t_mediaInfo, 0, sizeof(IMTK_PB_CTRL_SET_MEDIA_INFO_T));
    t_mediaInfo->eMediaType = IMTK_PB_CTRL_MEDIA_TYPE_MPEG2_TS ;
    t_mediaInfo->u4TotalDuration = 0xFFFFFFFF; //49 * 90000;
    t_mediaInfo->u8Size = -1;
    t_mediaInfo->uFormatInfo.tTsInfo.eVidInfo.eVidEnc = eVidEnc;
    t_mediaInfo->uFormatInfo.tTsInfo.eVidInfo.u2Pid   = i4_Vid_pid;
    t_mediaInfo->uFormatInfo.tTsInfo.eAudInfo.eAudEnc = eAudEnc;
    t_mediaInfo->uFormatInfo.tTsInfo.eAudInfo.u2Pid   = i4_Aud_pid;

    if (188 == t_tsInfo.ui2_packet_size)
    {
        t_mediaInfo->uFormatInfo.tTsInfo.ePacketType = IMTK_PB_CTRL_MPEG2_TS_PACKET_TYPE_188BYTE;
    }
    else
    {
        t_mediaInfo->uFormatInfo.tTsInfo.ePacketType = IMTK_PB_CTRL_MPEG2_TS_PACKET_TYPE_192BYTE;
    }

    LOG(5 ,"---[INFO]---TS MEDIA INFO:packet type:%d!\n", t_tsInfo.ui2_packet_size);
    LOG(5 ,"---[INFO]---TS MEDIA INFO:video codec:%d!\n", (int)eVidEnc);
    LOG(5 ,"---[INFO]---TS MEDIA INFO:video pid:%d!\n", (int)i4_Vid_pid);
    LOG(5 ,"---[INFO]---TS MEDIA INFO:audio codec:%d!\n", (int)eAudEnc);
    LOG(5 ,"---[INFO]---TS MEDIA INFO:audio pid:%d!\n", (int)i4_Aud_pid);

    return true;
}

static bool setMediaInfo(uint8_t * pu1buffer, uint  size)
{
    ASSERT_NULL_POINTER(pu1buffer);
    ASSERT_NULL_POINTER(g_handle);
    LOG(5, "setMediaInfo fCMPBHandle = %x - push mode\n", g_handle);
    IMTK_PB_ERROR_CODE_T i4_ret = IMTK_PB_ERROR_CODE_OK;

    IMTK_PB_CTRL_SET_MEDIA_INFO_T t_mediaInfo;
    if (false == getMediaInfo(pu1buffer, size, &t_mediaInfo))
    {
        LOG(0 ,"getMediaInfo() failed!\n");
        return false;
    }

    i4_ret = IMtkPb_Ctrl_SetMediaInfo(g_handle, &t_mediaInfo);
    if ( i4_ret != IMTK_PB_ERROR_CODE_OK )
    {
        LOG(0 ,"SetMediaInfo ERROR i4_ret = %d- push mode\n", i4_ret);
        return false;
    }
    return true;
}

static void* cmpb_push_thread(void* pv_data)
{
    char * file = (char *)pv_data;
    FILE * fp = 0;
    unsigned int index = 0;
    unsigned char* buffer = 0;
    unsigned int read_cnt = 0;
    IMTK_PB_ERROR_CODE_T ret = IMTK_PB_ERROR_CODE_OK;

    /*start push*/
    LOG(5, "--------------------------Open file(%s) ...\n", file);

    if (NULL == (fp = fopen(file, "r")))
    {
        LOG (0, "#############Open file(%s) Failed !\n", file);
        return NULL;
    }

    fseek(fp, 0, SEEK_SET);

    while (g_thread_running)
    {
        LOG(6, "---------->get buffer...\n");

        while(false == g_startPushData)
        {
            LOG(0, "---------->Wait to push data...\n");
            usleep(200000);
        }

        ret = IMtkPb_Ctrl_GetBuffer (g_handle,
                                     CMPB_PRIVATE_PS_BUF_SIZE,
                                     &buffer);

        if (IMTK_PB_ERROR_CODE_OK == ret)
        {
            read_cnt = fread(buffer,
                             1,
                             CMPB_PRIVATE_PS_BUF_SIZE,
                             fp);

            if (read_cnt < CMPB_PRIVATE_PS_BUF_SIZE)
            {
                LOG(0, "############read_cnt = %d\n", read_cnt);
                if(read_cnt == 0)
                {
                    IMtkPb_Ctrl_ReleaseBuffer(g_handle, buffer);
                    IMtkPb_Ctrl_SetPushModelEOS(g_handle);
                    break;
                }
            }

            ret = IMtkPb_Ctrl_SendData(
                      g_handle,
                      read_cnt,
                      buffer);
            if (IMTK_PB_ERROR_CODE_OK != ret)
            {
                LOG(0, "===>IMtkPb_Ctrl_SendData error:%d \n", ret);
                exit(-1);
            }

            index++;
        }
        else if (IMTK_PB_ERROR_CODE_GET_BUF_PENDING == ret)
        {
            LOG(0, "IMTK_PB_ERROR_CODE_GET_BUF_PENDING.\n");
            usleep(200000);
        }
        else
        {
            usleep(200000);
        }
    }


    if(fp)
    {
        fclose(fp);
        fp = NULL;
    }

    pthread_exit(NULL);
    return NULL;
}

static CMPB_ERROR_CODE cmpb_player_setEngineParam()
{
    CMPB_ERROR_CODE ret = CMPB_OK;
    IMTK_PB_CTRL_ENGINE_PARAM_T param;

    param.u4PlayFlag = IMTK_PB_CTRL_PLAY_FLAG_AUDIO | IMTK_PB_CTRL_PLAY_FLAG_VIDEO;

    LOG(5, "IMtkPb_Ctrl_SetEngineParam\n");
    ret = (CMPB_ERROR_CODE)IMtkPb_Ctrl_SetEngineParam(g_handle, &param);
    LOG(5, "IMtkPb_Ctrl_SetEngineParam end\n");

    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(5, "##########IMtkPb_Ctrl_SetEngineParam error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
    return ret;
}

static CMPB_ERROR_CODE cmpb_player_setMediaInfo()
{
    ASSERT_NULL_POINTER(g_handle);
#if 0
    CMPB_ERROR_CODE ret = CMPB_OK;
    IMTK_PB_CTRL_SET_MEDIA_INFO_T   t_media_info;

    t_media_info.eMediaType = IMTK_PB_CTRL_MEDIA_TYPE_MPEG2_TS;
    t_media_info.u4TotalDuration = 0xFFFFFFFF;
    t_media_info.u8Size = -1;

    t_media_info.uFormatInfo.tTsInfo.eVidInfo.eVidEnc = IMTK_PB_CTRL_VID_ENC_H264;
    t_media_info.uFormatInfo.tTsInfo.eVidInfo.u2Pid = 288;

    t_media_info.uFormatInfo.tTsInfo.eAudInfo.eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
    t_media_info.uFormatInfo.tTsInfo.eAudInfo.u2Pid = 305;
    t_media_info.uFormatInfo.tTsInfo.ePacketType = IMTK_PB_CTRL_MPEG2_TS_PACKET_TYPE_188BYTE;

    ret = IMtkPb_Ctrl_SetMediaInfo(g_handle, &t_media_info);
    if (ret != IMTK_PB_ERROR_CODE_OK)
    {
        LOG(0, "##########IMtkPb_Ctrl_SetMediaInfo error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
#else
    FILE * fp = 0;
    unsigned char* buffer = NULL;
    unsigned int read_cnt = 0;
    CMPB_ERROR_CODE ret = CMPB_OK;

    if (NULL == (fp = fopen(g_playFile, "r")))
    {
        LOG (0, "#############Open file(%s) Failed !\n", g_playFile);
        return CMPB_ERROR;
    }

    buffer = (unsigned char*)malloc(CMPB_PRIVATE_PS_BUF_SIZE * 4);
    if(buffer)
    {
        read_cnt = fread(buffer,
                         1,
                         CMPB_PRIVATE_PS_BUF_SIZE * 4,
                         fp);

        if(setMediaInfo(buffer, CMPB_PRIVATE_PS_BUF_SIZE * 4) != true)
        {
            LOG (0, "#############setMediaInfo Failed !\n");
            ret = CMPB_ERROR;
        }
    }

    if(fp)
    {
        fclose(fp);
        fp = NULL;
    }

    if(buffer)
    {
        free(buffer);
        buffer = NULL;
    }
#endif
    return ret;
}

static CMPB_ERROR_CODE cmpb_player_open()
{
    CMPB_ERROR_CODE ret;

    LOG(0, "---------------------->The file path is [%s]!!!\n", g_playFile);

    ret = (CMPB_ERROR_CODE)IMtkPb_Ctrl_Open(&g_handle, IMTK_PB_CTRL_BUFFERING_MODEL_PUSH, IMTK_PB_CTRL_APP_MASTER, (uint8_t*)"IPTV");
    if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
    {
        LOG(0, "##########IMtkPb_Ctrl_Open error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }

    ret = (CMPB_ERROR_CODE)IMtkPb_Ctrl_RegCallback(g_handle, (void * )CMPB_MAGIC_TAG, g_callback);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_RegCallback error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }

    ret = cmpb_player_setEngineParam();
    if(CMPB_OK != ret)
    {
        LOG(0, "##########cmpb_player_setEngineParam error [%d]!\n", (int)ret);
        return ret;
    }

    ret = cmpb_player_setMediaInfo();
    if(CMPB_OK != ret)
    {
        LOG(0, "##########cmpb_player_setMediaInfo error [%d]!\n", (int)ret);
        return ret;
    }

    return ret;
}

static CMPB_ERROR_CODE cmpb_player_setDisplay(int x, int y, int w, int h)
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    g_display.u4X = x;
    g_display.u4Y = y;
    g_display.u4W = w;
    g_display.u4H = h;

    ret = (CMPB_ERROR_CODE)IMtkPb_Ctrl_SetDisplayRectangle (g_handle, &g_display, &g_display);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_SetDisplayRectangle error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
    return ret;
}

static CMPB_ERROR_CODE cmpb_payer_play(int time)
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = cmpb_player_setDisplay(0, 0, 1000, 1000);
    if(CMPB_OK != ret)
    {
        return ret;
    }
    ret = (CMPB_ERROR_CODE)IMtkPb_Ctrl_Play(g_handle, (uint32_t)time);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_Play error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
    return ret;
}

static CMPB_ERROR_CODE cmpb_player_stop()
{
    if(!g_handle)
    {
        LOG(0, "##########player is not opened!!!\n");
        return CMPB_OK;
    }

    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = (CMPB_ERROR_CODE)IMtkPb_Ctrl_Stop(g_handle);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_Stop error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }

    return ret;
}

static CMPB_ERROR_CODE cmpb_player_close()
{
    if(!g_handle)
    {
        LOG(0, "##########player is not opened!!!\n");
        return CMPB_OK;
    }
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = IMtkPb_Ctrl_Close(g_handle);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_Close error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
    g_handle = 0;
    return ret;
}

static CMPB_ERROR_CODE cmpb_payer_seek(int time)
{
    CMPB_ERROR_CODE ret = CMPB_OK;
    #if 0

    ret = IMtkPb_Ctrl_SetVideoFreeze(g_handle, true);
    if(IMTK_PB_ERROR_CODE_OK != ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_SetVideoFreeze error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
    #endif

    cmpb_player_stop();

    playback_push_thread_stop();

    playback_push_thread_start();

    ret = IMtkPb_Ctrl_Play(g_handle, (uint32_t)time);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_Play error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
#if 0
    ret = IMtkPb_Ctrl_SetVideoFreeze(g_handle, false);
    if(IMTK_PB_ERROR_CODE_OK != ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_SetVideoFreeze error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }
    #endif

    return ret;
}

static CMPB_ERROR_CODE cmpb_payer_getPTS(uint64 * cPTS)
{
    CMPB_ERROR_CODE ret = CMPB_OK;
    uint64_t u8Vpts = 0, u8Apts = 0;

    ASSERT_NULL_POINTER(g_handle);

    ret = IMtkPb_Ctrl_GetCurrentPTS(g_handle, &u8Vpts, &u8Apts);
    if(IMTK_PB_ERROR_CODE_OK != (IMTK_PB_ERROR_CODE_T)ret)
    {
        LOG(0, "##########IMtkPb_Ctrl_GetCurrentPTS error [%d]!\n", (int)ret);
        return CMPB_ERROR;
    }

    LOG(0, "======Current Vpts = %llu, Apts = %llu!!!\n", u8Vpts, u8Apts);

    if(cPTS != NULL)
    {
        if(u8Apts > 0)
        {
            *cPTS = u8Apts;
        }
        else
        {
            *cPTS = u8Vpts;
        }
    }

    return ret;
}

/*
static CMPB_ERROR_CODE cmpb_player_getCurrentPos(uint32 * ui4_cur_time, uint64 *ui8_cur_pos)
{
    ASSERT_NULL_POINTER(g_handle);

    CMPB_ERROR_CODE ret = CMPB_OK;
    ret = IMtkPb_Ctrl_GetCurrentPos(g_handle,
                                           ui4_cur_time,
                                           ui8_cur_pos);
    if (CMPB_OK != ret)
    {
        LOG(0, "IMtkPb_Ctrl_GetCurrentPos() Failed, return: %d !\n", ret);
        return CMPB_ERROR;
    }
    return ret;
}
*/

static CMPB_ERROR_CODE cmpb_player_state(IMTK_PB_CTRL_STATE_T * state)
{
    ASSERT_NULL_POINTER(g_handle);
    ASSERT_NULL_POINTER(state);

    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = IMtkPb_Ctrl_GetState(g_handle, state);
    if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
    {
        LOG(0, "IMtkPb_Ctrl_GetState() Failed ! %d\n", ret);
        return CMPB_ERROR;
    }
    return ret;
}

static CMPB_ERROR_CODE cmpb_player_pause()
{
    ASSERT_NULL_POINTER(g_handle);

    CMPB_ERROR_CODE ret = CMPB_OK;
    IMTK_PB_CTRL_STATE_T state = IMTK_PB_CTRL_PLAYING;

    cmpb_player_state(&state);
    if (state == IMTK_PB_CTRL_PLAYING)
    {
        ret = IMtkPb_Ctrl_Pause(g_handle);
        if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
        {
            LOG(0, "IMtkPb_Ctrl_Pause() Failed ! %d\n", ret);
            return CMPB_ERROR;
        }
    }

    return ret;
}

static CMPB_ERROR_CODE cmpb_player_unPause()
{
    ASSERT_NULL_POINTER(g_handle);

    CMPB_ERROR_CODE ret = CMPB_OK;
    IMTK_PB_CTRL_STATE_T state = IMTK_PB_CTRL_PLAYING;

    cmpb_player_state(&state);
    if (state != IMTK_PB_CTRL_PLAYING)
    {
        ret = IMtkPb_Ctrl_Play(g_handle, 0);
        if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
        {
            LOG(0, "cmpb_player_unPause() Failed ! %d\n", ret);
            return CMPB_ERROR;
        }
    }

    return ret;
}

static CMPB_ERROR_CODE cmpb_player_FF()
{
    ASSERT_NULL_POINTER(g_handle);

    CMPB_ERROR_CODE ret = CMPB_OK;
    IMTK_PB_CTRL_SPEED_T eSpeed = IMTK_PB_CTRL_SPEED_1X;

    if(g_speed == -1)
    {
        LOG(0, "Do not support FF!!!\n");
        return CMPB_ERROR;
    }

    playback_push_thread_stop();
    g_startPushData = false;

    playback_push_thread_start_with_file(g_ffFile);

    switch(g_speed)
    {
    case 2:
        eSpeed = IMTK_PB_CTRL_SPEED_FF_2X;
        break;

    case 4:
        eSpeed = IMTK_PB_CTRL_SPEED_FF_4X;
        break;

    case 8:
        eSpeed = IMTK_PB_CTRL_SPEED_FF_8X;
        break;

    case 16:
        eSpeed = IMTK_PB_CTRL_SPEED_FF_16X;
        break;

    case 32:
        eSpeed = IMTK_PB_CTRL_SPEED_FF_32X;
        break;

    default:
        LOG(0, "Current speed[%d] is not support!!!\n", g_speed);
        return CMPB_ERROR;
    }

    LOG(0, "---------Set speed to %d......\n", eSpeed);
    ret = IMtkPb_Ctrl_SetSpeed(g_handle, eSpeed);
    if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
    {
        LOG(0, "IMtkPb_Ctrl_SetSpeed() Failed ! %d\n", ret);
        return CMPB_ERROR;
    }

    return ret;
}

static CMPB_ERROR_CODE cmpb_player_resumeFromFF()
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = IMtkPb_Ctrl_SetSpeed(g_handle, IMTK_PB_CTRL_SPEED_1X);
    if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
    {
        LOG(0, "IMtkPb_Ctrl_SetSpeed() Failed ! %d\n", ret);
        return CMPB_ERROR;
    }

    playback_push_thread_stop();

    playback_push_thread_start_with_file(g_playFile);

    return ret;
}

static CMPB_ERROR_CODE cmpb_player_getAudTrack(int * audTrack)
{
    ASSERT_NULL_POINTER(g_handle);

    CMPB_ERROR_CODE ret = CMPB_OK;
    IMTK_PB_CTRL_GET_MEDIA_INFO_T mInfo;

    ret = IMtkPb_Ctrl_GetMediaInfo(g_handle, &mInfo);
    if ((IMTK_PB_ERROR_CODE_T)ret != IMTK_PB_ERROR_CODE_OK)
    {
        LOG(0, "IMtkPb_Ctrl_GetMediaInfo() Failed ! %d\n", ret);
        return CMPB_ERROR;
    }

    *audTrack = mInfo.u2AudioTrackNum;

    return ret;
}

CMPB_ERROR_CODE playback_init(char * playFile, char * ffFile, int speed, event_callback_fct pf_callback)
{
    if((playFile == NULL) || (ffFile == NULL))
    {
        return CMPB_ERROR;
    }
    g_callback = pf_callback;
    memset(g_playFile, '\0', sizeof(g_playFile));
    strncpy(g_playFile, playFile, strlen(playFile));

    memset(g_ffFile, '\0', sizeof(g_ffFile));
    strncpy(g_ffFile, ffFile, strlen(ffFile));

    g_speed = speed;

    return CMPB_OK;
}

CMPB_ERROR_CODE playback_uinit()
{
    g_handle = 0;
    g_callback = NULL;
    memset(g_playFile, '\0', sizeof(g_playFile));

    return CMPB_OK;
}

CMPB_ERROR_CODE playback_open()
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = cmpb_player_open();
    if (CMPB_OK != ret)
    {
        return ret;
    }

    return ret;
}

CMPB_ERROR_CODE playback_play(int time)
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = cmpb_payer_play(time);
    if (CMPB_OK != ret)
    {
        return ret;
    }

    return ret;
}

CMPB_ERROR_CODE playback_stop()
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = cmpb_player_stop();
    if (CMPB_OK != ret)
    {
        return ret;
    }

    return ret;
}

void playback_speedUpdate()
{
    g_startPushData = true;
}

CMPB_ERROR_CODE playback_close()
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = cmpb_player_close();
    if (CMPB_OK != ret)
    {
        return ret;
    }

    return ret;
}

CMPB_ERROR_CODE playback_push_thread_start()
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    g_thread_running = true;

    ret = pthread_create(&g_thread,
                         NULL,
                         cmpb_push_thread,
                         (void *)g_playFile);

    if (ret != CMPB_OK)
    {
        perror("pthread:");
        return CMPB_ERROR;
    }
    return ret;
}

CMPB_ERROR_CODE playback_push_thread_start_with_file(char * file)
{
    CMPB_ERROR_CODE ret = CMPB_OK;

    g_thread_running = true;

    ret = pthread_create(&g_thread,
                         NULL,
                         cmpb_push_thread,
                         (void *)file);

    if (ret != CMPB_OK)
    {
        perror("pthread:");
        return CMPB_ERROR;
    }
    return ret;
}

CMPB_ERROR_CODE playback_push_thread_stop()
{
    g_thread_running = 0;

    pthread_join(g_thread, NULL);

    return CMPB_OK;
}

CMPB_ERROR_CODE playback_getCurrentTime(unsigned int * cTime)
{
    uint64 cPTS = 0;
    CMPB_ERROR_CODE ret = CMPB_OK;

    ret = cmpb_payer_getPTS(&cPTS);
    if (CMPB_OK != ret)
    {
        return ret;
    }

    *cTime = cPTS / 90000;

    return ret;
}

//ÉèÖÃÉùµÀ
//nAudioBlance:,1:×óÉùµÀ£¬2:ÓÒÉùµÀ£¬3:Ë«ÉùµÀ

int g_nAudioBalance = 0;

int SetAudioBalance(int nAudioBalance)
{
	#if 0
	nAudioBalance =0;
	return 0;
	#else
    IMTK_PB_ERROR_CODE_T eCmpbRet;
    IMTK_PB_CTRL_AUD_DOWNMIX_MODE_T t_downmix_mode;

    //TSPLAYER_DBG("nAudioBalance=%d\n", nAudioBalance);
    printf("nAudioBalance=%d\n", nAudioBalance);

    if (nAudioBalance == 1) /*×óÉùµÀ*/
    {
        t_downmix_mode = IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL1;
    }
    else if (nAudioBalance == 2) /*ÓÒÉùµÀ*/
    {
        t_downmix_mode = IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL2;
    }
    else if (nAudioBalance == 3)/*Ë«ÉùµÀ*/
    {
        t_downmix_mode = IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL_OFF;
    }
    else
    {
        //TSPLAYER_ERR("arg error\n");
        printf("arg error\n");
        return 0;
    }

    eCmpbRet = IMtkPb_Ctrl_SetAudDownmixMode(t_downmix_mode);
    if (eCmpbRet)
    {
        //TSPLAYER_ERR("set audio downmix mode error\n");
        printf("set audio downmix mode error\n");
        return 0;
    }

    g_nAudioBalance = nAudioBalance;
    return 1;
    #endif
}


CMPB_ERROR_CODE process_key(char key)
{
    if (key == '\0')
    {
        return CMPB_PENDING;
    }

    switch(key)
    {
    case KEY_COMMAND_QUIT:
        printf("quit\n");
        return CMPB_QUIT;

    case KEY_COMMAND_PAUSE:
        printf("pause\n");
        cmpb_player_pause();
        break;

    case KEY_COMMAND_RESUME:
        printf("resume\n");
        cmpb_player_unPause();
        break;

    case KEY_COMMAND_FF:
        printf("FF\n");
        cmpb_player_FF();
        break;

    case KEY_COMMAND_STOPFF:
        printf("Stop FF\n");
        cmpb_player_resumeFromFF();
        break;

    case KEY_COMMAND_SEEK:
        printf("Seek!!\n");
        cmpb_payer_seek(0);
        break;

    case KEY_COMMAND_GETPTS:
        printf("Get PTS : \n");
        cmpb_payer_getPTS(NULL);
        break;

    case KEY_COMMAND_GETAUDTRACK:
    {
        int audTrack = 0;
        cmpb_player_getAudTrack(&audTrack);

        printf("AudTrack NUM : %d \n", audTrack);
    }
        break;
        
    case KEY_COMMAND_AUDIOBALANCE:
    	  g_nAudioBalance++;
    	  if(g_nAudioBalance > 3)
    	  	g_nAudioBalance = 0;
        SetAudioBalance(g_nAudioBalance);
        break;

    default:
        break;
    }

    return CMPB_OK;
}
