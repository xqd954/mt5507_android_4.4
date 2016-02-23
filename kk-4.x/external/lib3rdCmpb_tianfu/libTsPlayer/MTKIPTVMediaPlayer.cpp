#include "MTKIPTVMediaPlayer.h"
#include <IPTVLogHelper.h>
#include <unistd.h>

#define CACHE_BUF_SIZE 128*1024
#define LOCAL_DEBUG 1

#define MEDIAPLAY_DUMP (0)
#define TSPLAYER_DBG(fmt, ...)   LOGD("[%s]line:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_ERR(fmt, ...)   LOGE("[%s]line:%d Err: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  LOGI("cmpbCallback: "fmt, ##__VA_ARGS__)

//#define TSPLAYER_DBG(fmt, ...)   fprintf(stderr, "[%08ld][%s]line:%d "fmt, CTsPlayer::getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
//#define TSPLAYER_ERR(fmt, ...)   fprintf(stderr, "[%08ld][%s]line:%d "fmt, CTsPlayer::getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)

using namespace iptv;

IPTVMediaPlayer *iptv::createIPTVMediaPlayerInstance() {
#ifdef LOCAL_DEBUG
    //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("iptv::createIPTVMediaPlayerInstance()");
#endif
 
    return new MTKIPTVMediaPlayer();
}
MTKIPTVMediaPlayer::MTKIPTVMediaPlayer()
{
    pTsplayer = GetTsPlayer();
    isWriteThreadRun = false;
    mPlayerState = NONE;
    isSetMediaInfo = false;
    //setIPTVbackground();
    //setlastframe(false);
    mWriteThreadPid = 0;
}
MTKIPTVMediaPlayer::~MTKIPTVMediaPlayer()
{
    if(pTsplayer)
    {
        delete pTsplayer;
        pTsplayer = NULL;
    }
    
}

////////////////////////add by yihuagang for background
extern "C" {
long a_brdcst_force_blue_mute_video(unsigned char ui1_focusID,bool b_blue_mute);
long a_brdcst_set_user_default_video_mute(
    bool  b_enable, 
    unsigned char ui1_red, 
    unsigned char ui1_green, 
    unsigned char ui1_blue);
}

IMTK_PB_CTRL_BG_COLOR_T  mColor;
int32_t MTKIPTVMediaPlayer::setIPTVbackground()
{
//      memset(&mColor, 0, sizeof(mColor));
//    if(IMTK_PB_ERROR_CODE_OK == IMtkPb_Ctrl_Get_BG_Color(&mColor))
//    {
//          TSPLAYER_DBG("MTKIPTVMediaPlayer::setIPTVbackground()  mColor.ui1_blue= %d",mColor.ui1_blue);
//        if(mColor.ui1_red == 0 && mColor.ui1_blue == 0xFF && mColor.ui1_green == 0)  //blue mute on
//        {
//            TSPLAYER_DBG("MTKIPTVMediaPlayer::setIPTVbackground()= black");
//            mColor.ui1_blue = 0; //set to black
//            IMtkPb_Ctrl_Set_BG_Color(&mColor);
//            mColor.ui1_blue = 0xFF; //restore color
//            //return 1;
//        }
//    } 
    TSPLAYER_DBG("MTKIPTVMediaPlayer::setIPTVbackground()= black");
    a_brdcst_set_user_default_video_mute(true,0,0,0);/////black
    return 0;  
}

int32_t MTKIPTVMediaPlayer::defaultbackground()
{
//    if(IMTK_PB_ERROR_CODE_OK == IMtkPb_Ctrl_Get_BG_Color(&mColor))
//    {
//        if(mColor.ui1_red == 0 && mColor.ui1_blue == 0 && mColor.ui1_green == 0)  //
//        {
//            mColor.ui1_blue = 0xFF; //set to blue
//            IMtkPb_Ctrl_Set_BG_Color(&mColor);
//            return 1;
//        }
//    }
    a_brdcst_set_user_default_video_mute(false,0,0,0);/////blue
    return 0;
}
///////////////////////////////////



int32_t MTKIPTVMediaPlayer::setReadBufferCallback(ReadBufferCallback  m_readbufferfunction)
{
    m_readbuffer = m_readbufferfunction;
    return 0;
}

static int SetMediaInfoSuccess_flag = 0;  // 1: success    0: fail
#if MEDIAPLAY_DUMP
static FILE* app_dump = NULL;
#endif
void * MTKIPTVMediaPlayer::thread_write_data(void * param)
{

       if(param == NULL)
              return NULL;
       
        MTKIPTVMediaPlayer *player = static_cast<MTKIPTVMediaPlayer*>(param);
        
        char *mRecvBuffer = ( char *) new char [CACHE_BUF_SIZE];
        char *bakBuffer = ( char *) new char [CACHE_BUF_SIZE * 4 + 1];
        int read_size = 0;
        int read_pos = 0;
        int32_t ret = 0;
        int k = 4;
#if MEDIAPLAY_DUMP
        int i4_wrt_size = 0;
#endif
        int iRet = 0;
        if(mRecvBuffer == NULL || bakBuffer == NULL)
        {
            LOGE("MTKIPTVMediaPlayer::thread_write_data:Short of memery.");
            return NULL;
        }
        while(player->getWriteThreadRunSign())
        {
            int WriteDataFlag = 0;
            int WriteDataFlagValue = 0;
            if(player->getPlayerState() != PLAYING && player->getPlayerState() != FAST)
            {
                usleep(100000);
                continue;
            }

            read_size = player->m_readbuffer((unsigned char*)&mRecvBuffer[read_pos],(CACHE_BUF_SIZE - read_pos));

            if(read_size > 0)
            {
                read_pos += read_size;                
            }
        
            if(read_pos == CACHE_BUF_SIZE)
            {
                read_pos = 0;
            }
            else    
            {
             //  usleep(100000);
                continue;
            }
            //TSPLAYER_DBG("MTKIPTVMediaPlayer::thread_write_data() : player->isSetMediaInfo = %d",player->isSetMediaInfo);

            if(player->isSetMediaInfo == true)
            {
                TSPLAYER_DBG("MTKIPTVMediaPlayer::thread_write_data() : setMediaInfo");
                ret = player->setMediaInfo(mRecvBuffer,CACHE_BUF_SIZE);
                if (ret == -1)
                {
                    TSPLAYER_DBG("get media info error.\n");
                    memcpy(bakBuffer + CACHE_BUF_SIZE * (4 - k),mRecvBuffer,CACHE_BUF_SIZE);
                    k--;
                    SetMediaInfoSuccess_flag = 0;
                    if (0 == k)
                    {
                        TSPLAYER_DBG("\n send data error.\n");
                        return NULL;
                    }
                    TSPLAYER_DBG("setMediaInfo fail, need to memcpy to bak buffer, k = %d\n", k);
                    continue;
                }
                else
                {
                    TSPLAYER_DBG("first setMediaInfo success!\n");
                    SetMediaInfoSuccess_flag = 1;
                }
                    
                player->isSetMediaInfo = false;
                player->realPlay();
                    
            }

            //TSPLAYER_DBG("MTKIPTVMediaPlayer WANGXINYANG 2222\n");
            
            if (1 == SetMediaInfoSuccess_flag)
            {
            #if MEDIAPLAY_DUMP
                if (app_dump)
                {
                    i4_wrt_size = fwrite(mRecvBuffer, 1, CACHE_BUF_SIZE, app_dump);
                    if (i4_wrt_size != (int)CACHE_BUF_SIZE)
                    {
                        TSPLAYER_ERR("dump write %d < %d\n", i4_wrt_size, CACHE_BUF_SIZE);
                    }
                }
            #endif
                
                while(1)
                {
                    iRet = player->pTsplayer->WriteData((unsigned char*)mRecvBuffer, CACHE_BUF_SIZE);
                    if (iRet != CACHE_BUF_SIZE)
                    {
                        WriteDataFlag++;
                        TSPLAYER_ERR("write data error. iRet = %d\n", iRet);
                        usleep(10000);

                        if (WriteDataFlag > 500)
                        {
                            TSPLAYER_ERR("111 write data times has more than 500, so break the while.\n");
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                memcpy(bakBuffer + CACHE_BUF_SIZE *(4 - k + 1),mRecvBuffer,CACHE_BUF_SIZE);
                while(1)
                {
                    iRet = player->pTsplayer->WriteData((unsigned char*)bakBuffer, CACHE_BUF_SIZE *(4 - k + 1));
                    if (iRet != CACHE_BUF_SIZE *(4 - k + 1))
                    {
                        WriteDataFlagValue++;
                        TSPLAYER_ERR("setMediaInfo fail(exec mempcy). write data error. iRet = %d, k = %d\n", iRet,k);
                        usleep(10000);
                        if (WriteDataFlagValue > 500)
                        {
                            TSPLAYER_ERR("222 write data times has more than 500, so break the while.\n");
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                
                #if MEDIAPLAY_DUMP
                if (app_dump)
                {
                    i4_wrt_size = fwrite(bakBuffer, 1, CACHE_BUF_SIZE, app_dump);
                    if (i4_wrt_size != (int)CACHE_BUF_SIZE)
                    {
                        TSPLAYER_ERR("dump write %d < %d\n", i4_wrt_size, CACHE_BUF_SIZE);
                    }
                }
                #endif
            }
            
            //TSPLAYER_DBG("buffer manager k = %d\n",k);
                
        }

        if(mRecvBuffer)
        {
            delete[] mRecvBuffer;
            mRecvBuffer = NULL;            
        }

        if (bakBuffer)
        {
            delete[] bakBuffer;
            bakBuffer = NULL;
        }
    
        return NULL;

}

int32_t MTKIPTVMediaPlayer::initialize(void* param)
{
#ifdef LOCAL_DEBUG
            //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::initialize(void* param)");
#endif

    if(param == NULL)
    {
        LOGE("MTKIPTVMediaPlayer::initialize() : param is NULL");
        return -1;
    }
    
    if(pTsplayer == NULL)
    {
        LOGE("MTKIPTVMediaPlayer::initialize() : pTsplayer is NULL");
        return -1;
    }    

    pTsplayer->GetVideoPixels(mWidthPixels,mHeightPixels);

    m_x = 0;
    m_y = 0;
    m_width = mWidthPixels;
    m_height = mHeightPixels;         

    mPlayerState = INITED;

    return 0;
}

int32_t MTKIPTVMediaPlayer::clearBuffer()
{
    return 0;
}

int32_t MTKIPTVMediaPlayer::fastForward(int speed)
{
#ifdef LOCAL_DEBUG
            //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::fastForward(int speed)");
#endif
    speed = speed;
    if(!pTsplayer || mPlayerState == STOPED )
        return -1;
    
    if(pTsplayer->Fast() == false)
        return  -1;
        mPlayerState = FAST;

    return 0;
    
}
int32_t MTKIPTVMediaPlayer::fastRewind(int speed)
{
#ifdef LOCAL_DEBUG
            //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::fastRewind(int speed)");
#endif

    speed = speed;
    if(!pTsplayer || mPlayerState == STOPED )
        return -1;
    
    if(pTsplayer->Fast() == false)
        return  -1;
    
    mPlayerState = FAST;

    return 0;
}


int32_t MTKIPTVMediaPlayer::play()
{
#ifdef LOCAL_DEBUG
    //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::play()");
#endif

#if MEDIAPLAY_DUMP
    app_dump = fopen("/usbdisk/mediaplay_dump.ts", "ab+");
    if (app_dump == NULL)
    {
        TSPLAYER_ERR("open file error\n");
    }
    else
    {
        TSPLAYER_DBG("open file for dump ts ok\n");
    }
#endif
    if(isWriteThreadRun == false)
     {
    isWriteThreadRun = true;
    TSPLAYER_DBG("MTKIPTVMediaPlayer::play()  isWriteThreadRun == false");
    int  ret = pthread_create(&mWriteThreadPid, NULL, thread_write_data, this);

    if(ret != 0)
        return -1;
    
    }
    
    isSetMediaInfo = true;    
    mPlayerState = PLAYING;

    return 0;    
}
int32_t MTKIPTVMediaPlayer::realPlay()
{
#ifdef LOCAL_DEBUG
    //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::realPlay()");
#endif

    if (pTsplayer->SetVideoWindow(m_x,m_y,m_width,m_height) == -1)
    {
        return -1;
    }   

    if(pTsplayer->StartPlay() == false)
    {
        return -1;
    }
    
    return 0;
}
int32_t MTKIPTVMediaPlayer::setMediaInfo(char* buf,int size)
{
    IMTK_PB_CTRL_TS_SINGLE_PAT_INFO_T t_tsInfo;
    memset(&t_tsInfo, 0, sizeof(t_tsInfo));

#if 0
    int  i4_ret = IMtkPb_Ctrl_GetTSInfo(pTsplayer->ui4CmpbHandle, (uint8_t*)buf, size, &t_tsInfo);

    if ( i4_ret != IMTK_PB_ERROR_CODE_OK )
    {
        IPTV_ERR("IMtkPb_Ctrl_GetTSInfo()= %d- push mode\n", i4_ret);
        return -1;
    }
#else
    int  i4_ret = pTsplayer->GetTSInfo((uint8_t*)buf, size, &t_tsInfo);

    if ( i4_ret)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_GetTSInfo()= %d- push mode\n", i4_ret);
        return -1;
    }
#endif

    unsigned short i4_Vid_pid = -1;
    unsigned short i4_Aud_pid = -1;      
    IMTK_PB_CTRL_VID_ENC_T  eVidEnc = IMTK_PB_CTRL_VID_ENC_UNKNOWN;      
    IMTK_PB_CTRL_AUD_ENC_T  eAudEnc = IMTK_PB_CTRL_AUD_ENC_UNKNOWN;
    bool b_found = false;
    int audioTrackNum = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    vformat_t eViFormat =  VFORMAT_MPEG12;
    aformat_t eAuFormat = FORMAT_MPEG ;
    int ret = 0;
      
    TSPLAYER_DBG("\n t_tsInfo.ui1_pmt_num : %d \n", t_tsInfo.ui1_pmt_num);
    for(i = 0; i < t_tsInfo.ui1_pmt_num; i++)
    {
        for(j = 0; j < t_tsInfo.at_pmt_info[i].ui1_strm_num; j++)
        {
            i4_Vid_pid = t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_pid;
            if (i4_Vid_pid >= 65535)
            {
                continue;
            }
            if (j == t_tsInfo.at_pmt_info[i].ui1_strm_num)
            {
                TSPLAYER_DBG("get vpid error.\n");
                return -1;
            }

            TSPLAYER_DBG("\n ui2_strm_type : %d \n", t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type);

            switch(t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type)
            {
            case 1://STREAM_TYPE_VIDEO_11172_2:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_MPEG1_2;
                eViFormat = VFORMAT_MPEG12;
                break;

            case 2://STREAM_TYPE_VIDEO_13818_2:
            case 128://STREAM_TYPE_VIDEO_DIGICIPHER_2:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_MPEG1_2;
                eViFormat = VFORMAT_MPEG12;
                break;

            case 16://STREAM_TYPE_VIDEO_14496_2:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_MPEG4;
                eViFormat = VFORMAT_MPEG4;
                break;

            case 27://STREAM_TYPE_VIDEO_14496_10:
                eVidEnc = IMTK_PB_CTRL_VID_ENC_H264;
                eViFormat = VFORMAT_H264;
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
        TSPLAYER_DBG("\n t_tsInfo.at_pmt_info[%d].ui1_strm_num : %d \n", i, t_tsInfo.at_pmt_info[i].ui1_strm_num);
        for(j = 0; j < t_tsInfo.at_pmt_info[i].ui1_strm_num; j++)
        {
            switch(t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type)
            {
            case 3://STREAM_TYPE_AUDIO_11172_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;                
                eAuFormat = FORMAT_MPEG;
                break;

            case 4://STREAM_TYPE_AUDIO_13818_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;
                eAuFormat = FORMAT_MPEG;
                break;

            case 15://STREAM_TYPE_AUDIO_13818_7:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
               eAuFormat = FORMAT_AAC;
                break;

            case 17://STREAM_TYPE_AUDIO_14496_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
                eAuFormat = FORMAT_AAC;
                break;

            case 129://STREAM_TYPE_AUDIO_ATSC_A53:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                eAuFormat = FORMAT_AC3;
                break;

            case 130://STREAM_TYPE_AUDIO_MSRT24:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                eAuFormat =  FORMAT_DTS;
                break;

            case 131://STREAM_TYPE_AUDIO_ATT_LABS_G729A:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                eAuFormat = FORMAT_AC3;
                break;

            case 134://STREAM_TYPE_AUDIO_DTS_GSM610:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                eAuFormat = FORMAT_DTS;
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
    TSPLAYER_DBG("audioTrackNum : %d \n", audioTrackNum);
    for(i = 0; i < t_tsInfo.ui1_pmt_num; i++)
    {
        for(j = 0; j < t_tsInfo.at_pmt_info[i].ui1_strm_num; j++)
        {
            i4_Aud_pid = t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_pid;
            if (i4_Aud_pid >= 65535)
            {
                continue;
            }
            if (j == t_tsInfo.at_pmt_info[i].ui1_strm_num)
            {
                TSPLAYER_DBG("get apid error.\n");
                return -1;
            }

            switch(t_tsInfo.at_pmt_info[i].at_stream_info_list[j].ui2_strm_type)
            {
           case 3://STREAM_TYPE_AUDIO_11172_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;                
                eAuFormat = FORMAT_MPEG;
                break;

            case 4://STREAM_TYPE_AUDIO_13818_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_MPEG;
                eAuFormat = FORMAT_MPEG;
                break;

            case 15://STREAM_TYPE_AUDIO_13818_7:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
               eAuFormat = FORMAT_AAC;
                break;

            case 17://STREAM_TYPE_AUDIO_14496_3:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_AAC;
                eAuFormat = FORMAT_AAC;
                break;

            case 129://STREAM_TYPE_AUDIO_ATSC_A53:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                eAuFormat = FORMAT_AC3;
                break;

            case 130://STREAM_TYPE_AUDIO_MSRT24:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                eAuFormat =  FORMAT_DTS;
                break;

            case 131://STREAM_TYPE_AUDIO_ATT_LABS_G729A:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DD;
                eAuFormat = FORMAT_AC3;
                break;

            case 134://STREAM_TYPE_AUDIO_DTS_GSM610:
                eAudEnc = IMTK_PB_CTRL_AUD_ENC_DTS;
                eAuFormat = FORMAT_DTS;
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
    
    TSPLAYER_DBG("i4_Vid_pid = %d,i4_Aud_pid=%d",(unsigned short)i4_Vid_pid,(unsigned short)i4_Aud_pid);
    if ((i4_Vid_pid >= 65535) || (i4_Aud_pid >= 65535))
    {
        TSPLAYER_DBG("get audio or video\n");
        return -1;
    } 

    VIDEO_PARA_T video;
    memset(&video,0,sizeof(VIDEO_PARA_T));
    video.pid = (unsigned short)i4_Vid_pid;
    video.vFmt = eViFormat;
    
    video.nFrameRate =1;
    video.nVideoHeight = 1;
    video.nVideoWidth = 1;        
    ret = pTsplayer->InitVideo(&video);
    if (ret == -1)
    {
        TSPLAYER_DBG("InitVideo error. \n");
        return -1;
    }

    AUDIO_PARA_T audio;        
    memset(&audio,0,sizeof(AUDIO_PARA_T));
    audio.pid = (unsigned short)i4_Aud_pid;
    audio.aFmt = eAuFormat;
    audio.nChannels = 1;
    audio.nExtraSize = 1;
    audio.nSampleRate = 1;


    ret = pTsplayer->InitAudio(&audio);
    if (ret == -1)
    {
        TSPLAYER_DBG("InitVideo error. \n");
        return -1;
    }
        
    return 0;
}
int32_t MTKIPTVMediaPlayer::pause()
{
#ifdef LOCAL_DEBUG
            //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::pause()");
#endif
    if(mPlayerState != PLAYING)
        return -1;
    
    if(pTsplayer->Pause() == false)
        return -1;
    

    mPlayerState = PAUSE;

    return 0;

}
int32_t MTKIPTVMediaPlayer::resume()
{
#ifdef LOCAL_DEBUG
            //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::resume()");
#endif
    if(!pTsplayer || mPlayerState == PLAYING)
        return -1;
    
    if(mPlayerState == FAST)
    {
        if(pTsplayer->StopFast() == true)
        {
            mPlayerState = PLAYING;
            return 0;            
        }
    }
    
    if(mPlayerState == PAUSE)
    {    
        if(pTsplayer->Resume() == true)
        {
            mPlayerState = PLAYING;
            return 0;
        }
    }
    return -1;
}

int32_t MTKIPTVMediaPlayer::stop()
{
#ifdef LOCAL_DEBUG
    TSPLAYER_DBG("MTKIPTVMediaPlayer::start to stop()");
#endif

#if MEDIAPLAY_DUMP
    if (app_dump)
    {
        fclose(app_dump);
        app_dump = NULL;
    }
#endif

    if(!pTsplayer)
        return -1;
    TSPLAYER_DBG("MTKIPTVMediaPlayer::stop >> judeg the playerstate...\n");
    
    if(mPlayerState <= INITED || mPlayerState >= STOPED)
    {
        LOGE("MTKIPTVMediaPlayer::stop() no need to call. ");
        return 0;
    }

    TSPLAYER_DBG("MTKIPTVMediaPlayer::stop >> start to pthread_join the write_data_thread...\n");
    
    if (isWriteThreadRun) 
    {
        isWriteThreadRun = false;
        pthread_join(mWriteThreadPid, (void**)NULL);
    }

    TSPLAYER_DBG("MTKIPTVMediaPlayer::stop >> exit to pthread_join the write_data_thread...\n");
    
    if(mPlayerState != STOPED && mPlayerState != NONE)
    {
        if(pTsplayer->Stop() == false)
            {
            LOGE("MTKIPTVMediaPlayer::stop()error. ");
            return -1;
            }
    }

    TSPLAYER_DBG("MTKIPTVMediaPlayer:: exit to stop()");

    mWriteThreadPid = 0;
    mPlayerState = STOPED;
    isSetMediaInfo = false;
    return 0;

}


int32_t MTKIPTVMediaPlayer::stopExt()
{
#ifdef LOCAL_DEBUG
    TSPLAYER_DBG("MTKIPTVMediaPlayer::start to stopExt()");
#endif

    if(!pTsplayer)
        return -1;
    
    if(mPlayerState == STOPED)
    {
        if(pTsplayer->StopExt() == false)
        {
            LOGE("MTKIPTVMediaPlayer::stop()error. ");
            return -1;
        }
    }

    TSPLAYER_DBG("MTKIPTVMediaPlayer:: exit to stopExt()");

    return 0;

}


int32_t MTKIPTVMediaPlayer::setAVSync(bool isSync)
{
    
    isSync = isSync;
    return 0;

}
uint64_t MTKIPTVMediaPlayer::getCurrentPosition()
{
    uint64_t u8Vpts = 0, u8Apts = 0,cPTS = 0;

    int ret;
    
#if 0
    ret = (int)IMtkPb_Ctrl_GetCurrentPTS(pTsplayer->ui4CmpbHandle, &u8Vpts, &u8Apts);

    if (IMTK_PB_ERROR_CODE_OK != ret)
    {
        IPTV_ERR("GetCurrentPTS error [%d]!\n", (int)ret);
        return 0;
    }
#else
    ret = pTsplayer->GetCurrentPTS(&u8Vpts, &u8Apts);

    if (ret)
    {
        TSPLAYER_ERR("GetCurrentPTS error [%d]!\n", (int)ret);
        return 0;
    }
#endif
    if(u8Apts > 0)
    {
        cPTS = u8Apts;
    }
    else
    {
        cPTS = u8Vpts;
    }
    return cPTS /90000;

}


int32_t  MTKIPTVMediaPlayer::setDisplayArea(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
#ifdef LOCAL_DEBUG
        //LOGV(__PRETTY_FUNCTION__);
    TSPLAYER_DBG("MTKIPTVMediaPlayer::setDisplayArea(uint32_t left, uint32_t top, uint32_t width, uint32_t height)");
#endif
    LOGD("MTKIPTVMediaPlayer::setDisplayArea ");

    float scaleX = mWidthPixels/1280.0f;
    float scaleY = mHeightPixels/720.0f;
    m_x= left * scaleX;
    m_y = top * scaleY;
    m_width = width * scaleX;
    m_height = height * scaleY;
    
    if (left == 0 && top == 0 && width == 0 && height == 0)
    {
        m_width = mWidthPixels;
        m_height = mHeightPixels;
    }
    
    pTsplayer->SetVideoWindow(m_x,m_y,m_width,m_height);
    return 0;
}

int32_t  MTKIPTVMediaPlayer::setVolume(uint32_t volume)
{
    mVolume = volume;
    return pTsplayer->SetVolume(volume);

}
uint32_t MTKIPTVMediaPlayer::getVolume()
{
    return pTsplayer->GetVolume();
}

////////////////////////////////////////set audio balance
bool  MTKIPTVMediaPlayer::setAudioBalance(uint32_t nAudioBalance)
{

    return pTsplayer->SetAudioBalance(nAudioBalance);

}
uint32_t MTKIPTVMediaPlayer::getAudioBalance()
{
    return pTsplayer->GetAudioBalance();
}
/////////////////////////set enable_keep_last_frame
void MTKIPTVMediaPlayer::setlastframe(bool nEnable_keep_last_frame)
{

    pTsplayer->enable_keep_last_frame (nEnable_keep_last_frame);

}



int32_t  MTKIPTVMediaPlayer::setMute(bool isMute)
{
    if(isMute)
    {
        mVolume = pTsplayer->GetVolume();
        return pTsplayer->SetVolume(0);
    }
    else
    {
         return pTsplayer->SetVolume(mVolume);
    }
}

int32_t  MTKIPTVMediaPlayer::finalize()
{
#ifdef LOCAL_DEBUG
                //LOGV(__PRETTY_FUNCTION__);
        TSPLAYER_DBG("MTKIPTVMediaPlayer::finalize()");
#endif
       if(mPlayerState != STOPED)
              stop();
       mPlayerState = STOPED;
       
       return 0;

}


