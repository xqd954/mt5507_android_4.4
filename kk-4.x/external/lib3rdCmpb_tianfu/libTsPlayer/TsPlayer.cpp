#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include "TsPlayer.h"

#include "IMtkPb_Ctrl.h"
#include "IMtkPb_Misc.h"
#include "IMtkPb_Comm.h"
#include "IMtkPb_Info.h"
#include "IMtkPb_Ctrl_DTV.h"
#include "ipc_msgq.h"
#include "block_cond.h"


#define DUMP_TO_TS  (1)   //dump ts to u disk

#if ENABLE_LOCAL_TEST   // just for my own test

#if ENABLE_SHOW_EXEC_TIME //打印执行时间 (相对的)
#define TSPLAYER_DBG(fmt, ...)   fprintf(stderr, "[%08ld][%s]line:%d "fmt, CTsPlayer::getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_ERR(fmt, ...)   fprintf(stderr, "[%08ld][%s]line:%d "fmt, CTsPlayer::getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  fprintf(stderr, "[%08ld] cmpbCallback: "fmt, CTsPlayer::getTimeMs(),##__VA_ARGS__)
#else
#if ENABLE_PRINT_DEBUG_LOG //打印
#define TSPLAYER_DBG(fmt, ...)   fprintf(stderr, "[%s]line:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_ERR(fmt, ...)   fprintf(stderr, "[%s]line:%d Err: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  fprintf(stderr, "cmpbCallback: "fmt, ##__VA_ARGS__)
#else                     //不打印
#define TSPLAYER_DBG(fmt, ...)
#define TSPLAYER_ERR(fmt, ...)   fprintf(stderr, "[%s]line:%d Err: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  fprintf(stderr, "cmpbCallback: "fmt, ##__VA_ARGS__)
#endif
#endif

#else   // just for my own test

#define LOG_TAG "libTsPlayer"
#include <cutils/log.h>

#if ENABLE_SHOW_EXEC_TIME //打印执行时间 (相对的)
#define TSPLAYER_DBG(fmt, ...)   LOGD("[%08ld][%s]line:%d "fmt, CTsPlayer::getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_ERR(fmt, ...)   LOGE("[%08ld][%s]line:%d "fmt, CTsPlayer::getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  LOGI("[%08ld]cmpbCallback: "fmt, CTsPlayer::getTimeMs(),##__VA_ARGS__)
#else
#if ENABLE_PRINT_DEBUG_LOG //打印
#define TSPLAYER_DBG(fmt, ...)   LOGD("[%s]line:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_ERR(fmt, ...)   LOGE("[%s]line:%d Err: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  LOGI("cmpbCallback: "fmt, ##__VA_ARGS__)
#else                     //不打印
#define TSPLAYER_DBG(fmt, ...)
#define TSPLAYER_ERR(fmt, ...)   LOGE("[%s]line:%d Err: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TSPLAYER_INFO(fmt, ...)  LOGI("cmpbCallback: "fmt, ##__VA_ARGS__)
#endif
#endif

#endif

#ifndef UNUSED
#define UNUSED(x)               (void)x         /**<The return value of routine is not cared*/
#endif
#define PUSH_MAGIC_TAG                      0x85654321
#define MAX_NUM_HANDLES      ((unsigned short) 4096)

typedef struct _THREAD_DESCR_T
{
    unsigned int    z_stack_size;
    unsigned char   ui1_priority;
    unsigned short  ui2_num_msgs;
}   THREAD_DESCR_T;

typedef struct _GEN_CONFIG_T
{
    unsigned short  ui2_version;
    void*  pv_config;
    unsigned int  z_config_size;
    THREAD_DESCR_T  t_mheg5_thread;
}   GEN_CONFIG_T;



#ifndef FBIOPUT_OSD_SRCCOLORKEY
#define  FBIOPUT_OSD_SRCCOLORKEY    0x46fb
#endif

#ifndef FBIOPUT_OSD_SRCKEY_ENABLE
#define  FBIOPUT_OSD_SRCKEY_ENABLE  0x46fa
#endif


#ifndef FBIOPUT_OSD_SET_GBL_ALPHA
#define  FBIOPUT_OSD_SET_GBL_ALPHA  0x4500
#endif


//CMD STRING DEFINE
#define CMD_MSG_SET_MEDIA_INFO  'a'//"setMediaInfo, maybe alse SetEngineParam"
#define CMD_MSG_PLAY            'b'//"play"
#define CMD_MSG_RESUME          'c'//"resume"
#define CMD_MSG_SET_WINDOW      'd'//"setWindow"
#define CMD_MSG_PAUSE           'e'//"pause"
#define CMD_MSG_SET_SPEED       'f'//"setSpeed"
#define CMD_MSG_STOP            'g'//"stop"
#define CMD_MSG_REPLAY          'h'//"replay"
#define CMD_MSG_WINDOW_HIDE     'i'//"hide display window"
#define CMD_MSG_CHANGE_AV       'j'//"changeAduio&changeVideo"
#define CMD_MSG_FLUSH           'k'//"flush all play buffer"
#define CMD_MSG_EXIT            'E'//"EXIT"
#define CMD_MSG_GOTO_CHANGE_AV  'l'//"go to change av"
#define CMD_MSG_GOTO_PLAY       'm'//"go to paly"
#define CMD_MSG_AUDIO_CHG_DONE  'n'//"audio change done"
#define CMD_MSG_LAST_FRAME_TRUE  'o'//save last frame
#define CMD_MSG_LAST_FRAME_FALSE  'p'//save last frame



#define WRAP(x)  (1<<(x-'a'))

#define CMD_MSG_MAX_LEN      (4)  //要大于上面定义的最长的cmd str len

#if ENABLE_LOCAL_TEST   // just for my own test
static char* _pc_cmpb_src_tpye = NULL;
static char _c_src_type[24];
#endif

#if DUMP_TO_TS
static FILE* pfile_dump = NULL;
#endif


IMTK_PB_CB_ERROR_CODE_T CTsPlayer::cmpbPlayCallback(IMTK_PB_CTRL_EVENT_T eEventType, void* pvTag,  uint32_t u4Data)
{
    static int notify_time_times = 0;
    CTsPlayer* pThis = (CTsPlayer*)pvTag;

    if (!pThis)
    {
        TSPLAYER_ERR("fatal error: this is NULL, type:%d\n", eEventType);
        return IMTK_PB_CB_ERROR_CODE_OK;
    }

    if (eEventType == IMTK_PB_CTRL_EVENT_CUR_TIME_UPDATE)
    {
        if (u4Data)
        {
            //TSPLAYER_INFO("play time: %d\n", u4Data);
        }
        else
        {
            notify_time_times++;
            if (notify_time_times >= 7)
            {
                TSPLAYER_INFO("notify play time(0) more than 7 times\n");
                notify_time_times = 0;
            }
        }
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_VIDEO_CHANGE_DONE)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_VIDEO_CHANGE_DONE!! ui4FlagWaitAv=%d\n", pThis->ui4FlagWaitAv);

        pThis->ui4FlagWaitAv |= 1;
        if (pThis->ui4FlagWaitAv == 3)
        {
            pThis->ui4FlagNotSend = 0;
            trigger_block_exit(WRAP(CMD_MSG_CHANGE_AV));
            TSPLAYER_INFO("after video change down.video change:block value = 0x%x\n", get_block_value());
        }
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_AUDIO_CHANGE_DONE)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_AUDIO_CHANGE_DONE!! ui4FlagWaitAv=%d\n", pThis->ui4FlagWaitAv);

        pThis->ui4FlagWaitAv |= 2;
        trigger_block_exit(WRAP(CMD_MSG_AUDIO_CHG_DONE));
        if (pThis->ui4FlagWaitAv == 3)
        {
            pThis->ui4FlagNotSend = 0;
            trigger_block_exit(WRAP(CMD_MSG_CHANGE_AV));
            TSPLAYER_INFO("after audio change done.audio change:block value = 0x%x\n", get_block_value());
        }
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_RANGE_READY)  ///code normal
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_CODE_NORMAL!!\n");

        pThis->ui4FlagNotSend = 0;
        trigger_block_exit(WRAP(CMD_MSG_PLAY));
        TSPLAYER_INFO("after code normal. Code normal:block value = 0x%x\n", get_block_value());
    }
    #if 0
    else if (eEventType == IMTK_PB_CTRL_EVENT_SESS_START)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_SESS_START!!\n");

        pThis->ui4FlagNotSend = 0;
        trigger_block_exit(WRAP(CMD_MSG_PLAY));
        TSPLAYER_INFO("Sess start:block value = 0x%x\n", get_block_value());
    }
    #endif
    else if (eEventType == IMTK_PB_CTRL_EVENT_SPEED_UPDATE)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_SPEED_UPDATE:%d!!\n", u4Data);

        trigger_block_exit(WRAP(CMD_MSG_SET_SPEED));
        TSPLAYER_INFO("set speed:block value = 0x%x\n", get_block_value());

        pThis->ui4FlagNotSend = 0;
        pThis->ui4SpeedUpdateFlag = 1;
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_TOTAL_TIME_UPDATE)
    {
        TSPLAYER_INFO("total time: %d\n", u4Data);
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_PLAY_DONE )
    {
#if ENABLE_KEEP_LAST_FRAME
        //IMtkPb_Ctrl_SetVideoFreeze(pThis->ui4CmpbHandle, 0);
#endif
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_PLAY_DONE!!\n");
        pThis->ui4FlagPlayDone = 1;
        //pThis->ui4FirstPlayFlag = 0;
        pThis->ePlaySpeed = IMTK_PB_CTRL_SPEED_1X;
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_PLAYBACK_ERROR)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_PLAYBACK_ERROR,uData=%d!!\n",u4Data);
        if (IMTK_PB_CTRL_ERROR_AUDIO_UNPLAYABLE != u4Data)
        {
            TSPLAYER_INFO("reveived the msg:IMTK_PB_CTRL_ERROR_AUDIO_UNPLAYABLE");
            pThis->i4CmpbLastRet = 3;
        }
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_EOS )
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_EOS!!\n");
        //pThis->Stop();
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_EOF )
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_EOF!!\n");
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_BUFFER_UNDERFLOW)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_BUFFER_UNDERFLOW.\n");
    }
    else if (eEventType == IMTK_PB_CTRL_EVENT_BUFFER_READY)
    {
        TSPLAYER_INFO("IMTK_PB_CTRL_EVENT_BUFFER_READY.\n");
    }
    else
    {
        TSPLAYER_INFO("CMPB NOTFIY:type:%d \n", eEventType);
    }

    return IMTK_PB_CB_ERROR_CODE_OK;
}



//thread for execute player command
void* CTsPlayer::playerCmdProcess(void* arg)
{
    char cCmd;
    char cMsgBuf[CMD_MSG_MAX_LEN];
    int i4MsgSize;
    IMTK_PB_ERROR_CODE_T eCmpbRet;
    IMTK_PB_CTRL_RECT_T tDisplaySrc;
    IMTK_PB_CTRL_RECT_T tDisplayDest;
    IMTK_PB_CTRL_ENGINE_PARAM_T tEngineParam;
    int i4Ret = -1;
    CTsPlayer* pThis = (CTsPlayer*)arg;
    bool is_last_frame = true;
#if 0
    struct sched_param sp;
    //sp.sched_priority = 1;
    int policy = sched_getscheduler(0);
    int max = sched_get_priority_max(policy);
    int min = sched_get_priority_min(policy);
    sp.sched_priority = max - 1;
    
    
    

    switch (policy)
    {
        case SCHED_RR:
            TSPLAYER_DBG("current: SCHED_RR\n");
            break;

        case SCHED_OTHER:
            TSPLAYER_DBG("current: SCHED_OTHER\n");
            break;

        case SCHED_FIFO:
            TSPLAYER_DBG("current: SCHED_FIFO\n");
            break;
    }

    TSPLAYER_DBG("set to SCHED_FIFO:%d (%d-%d)\n", max, min, max);
    sched_setscheduler(0, SCHED_FIFO, &sp);
#endif

    if (!pThis)
    {
        TSPLAYER_ERR("fatal error: this is NULL\n");
        return (void*)1;
    }

    while (1)
    {
        i4MsgSize = CMD_MSG_MAX_LEN;
        memset(cMsgBuf, 0, sizeof(cMsgBuf));

        i4Ret = msgq_recv(pThis->i4MsgQueHandle, (void*)cMsgBuf, &i4MsgSize, (unsigned int)(-1));
        if (i4Ret)
        {
            TSPLAYER_ERR("msgq_recv error.\n");
            continue;
        }

        cCmd = cMsgBuf[0];

        //TSPLAYER_DBG("msgq recv:%c.\n", cCmd);

        if (cCmd == CMD_MSG_EXIT)
        {
            TSPLAYER_DBG("msgq_recv EXIT, break loop now.\n");
            break;
        }

        switch (cCmd)
        {
            case CMD_MSG_SET_MEDIA_INFO:
#if 0  /*<!-- deeve 2012/3/14 8:02:57 PM */
                TSPLAYER_DBG("vfmt:%d(pid=%d 0x%x) afmt:%d(pid=%d 0x%x)\n", pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc, \
                             pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid, pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid, \
                             pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc, pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid, \
                             pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid);
#endif /*0*//* deeve 2012/3/14 8:02:57 PM --!>*/

                eCmpbRet = IMtkPb_Ctrl_SetMediaInfo(pThis->ui4CmpbHandle, &pThis->tCurtMediaInfo);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_SetMediaInfo()=%d\n", eCmpbRet);
                    pThis->i4CmpbLastRet = 2;
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_SetMediaInfo(apid:0x%x vpid:0x%x) OK\n", pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid,
                                 pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid);
                }
                break;

            case CMD_MSG_SET_WINDOW:
                tDisplayDest.u4X = pThis->ui4RectX;
                tDisplayDest.u4Y = pThis->ui4RectY;
                tDisplayDest.u4W = pThis->ui4RectW;
                tDisplayDest.u4H = pThis->ui4RectH;

                tDisplaySrc.u4X = 0;
                tDisplaySrc.u4Y = 0;
                tDisplaySrc.u4W = 1000;
                tDisplaySrc.u4H = 1000;

                eCmpbRet = IMtkPb_Ctrl_SetDisplayRectangle(pThis->ui4CmpbHandle, &tDisplaySrc, &tDisplayDest);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_SetDisplayRectangle()=%d\n", eCmpbRet);
                    //pThis->i4CmpbLastRet = eCmpbRet;
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_SetDisplayRectangle() OK\n");
                }

                break;

            case CMD_MSG_WINDOW_HIDE:
                tDisplayDest.u4X = 0;
                tDisplayDest.u4Y = 0;
                tDisplayDest.u4W = 0;
                tDisplayDest.u4H = 0;

                tDisplaySrc.u4X = 0;
                tDisplaySrc.u4Y = 0;
                tDisplaySrc.u4W = 1000;
                tDisplaySrc.u4H = 1000;

                eCmpbRet = IMtkPb_Ctrl_SetDisplayRectangle(pThis->ui4CmpbHandle, &tDisplaySrc, &tDisplayDest);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_SetDisplayRectangle(0)=%d\n", eCmpbRet);
                    //pThis->i4CmpbLastRet = eCmpbRet;
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_SetDisplayRectangle(0) OK\n");
                }
                break;

            case CMD_MSG_PLAY:
                pThis->ui4FlagNotSend = 1;
                
                TSPLAYER_DBG("reveived CMD_MSG_PLAY.Play:block value = 0x%x\n", get_block_value());

                trigger_block_entry(WRAP(CMD_MSG_PLAY));
                trigger_block_exit(WRAP(CMD_MSG_GOTO_PLAY));

                TSPLAYER_DBG("invoke IMtkPb_Ctrl_Play()\n");
                eCmpbRet = IMtkPb_Ctrl_Play(pThis->ui4CmpbHandle, 0);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_Play()=%d\n", eCmpbRet);
                    pThis->i4CmpbLastRet = 3;
                    trigger_block_exit(WRAP(CMD_MSG_PLAY));
                    TSPLAYER_INFO("IMtkPb_Ctrl_Play()Play err:block value = 0x%x\n", get_block_value());
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_Play() OK\n");
                    pThis->i4CmpbLastRet = 0;
                    pThis->ePlaySpeed = IMTK_PB_CTRL_SPEED_1X;
                }
                break;

            case CMD_MSG_RESUME:
                TSPLAYER_DBG("invoke IMtkPb_Ctrl_Play(resume)\n");
                eCmpbRet = IMtkPb_Ctrl_Play(pThis->ui4CmpbHandle, 0);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_Play()=%d\n", eCmpbRet);
                    //pThis->i4CmpbLastRet = 3;
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_Play() OK\n");
                    pThis->i4CmpbLastRet = 0;
                    pThis->ePlaySpeed = IMTK_PB_CTRL_SPEED_1X;
                }
                break;

            case CMD_MSG_PAUSE:
                TSPLAYER_DBG("received CMD_MSG_PAUSE:block value = 0x%x\n", get_block_value());  //未起播的时候不能进入setspeed操作，否则接口调用会失败
                wait_block_out(WRAP(CMD_MSG_PLAY) | WRAP(CMD_MSG_CHANGE_AV) | WRAP(CMD_MSG_SET_SPEED));

                eCmpbRet = IMtkPb_Ctrl_Pause(pThis->ui4CmpbHandle);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_Pause()=%d\n", eCmpbRet);
                    //pThis->i4CmpbLastRet = 4;
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_Pause() OK\n");
                }
                break;

            case CMD_MSG_STOP:
                TSPLAYER_DBG("reveived CMD_MSG_STOP. Stop:block value = 0x%x\n", get_block_value());
                wait_block_out(WRAP(CMD_MSG_PLAY) | WRAP(CMD_MSG_CHANGE_AV)); //等待上次play完成，快速换台的时候
#if 0 //ENABLE_KEEP_LAST_FRAME
                if(is_last_frame == true)
                    IMtkPb_Ctrl_SetVideoFreeze(pThis->ui4CmpbHandle, 1);  // 保留最后一帧
                else
                    IMtkPb_Ctrl_SetVideoFreeze(pThis->ui4CmpbHandle, 0);  // no 保留最后一帧
#endif
                TSPLAYER_DBG("invoke IMtkPb_Ctrl_Stop()\n");
                eCmpbRet = IMtkPb_Ctrl_Stop(pThis->ui4CmpbHandle);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_Stop()=%d\n", eCmpbRet);
                    pThis->i4CmpbLastRet = 6;
                }
                else
                {
                    TSPLAYER_DBG("IMtkPb_Ctrl_Stop() OK\n");
                    pThis->i4CmpbLastRet = 0;
                    pThis->ui4FlagPlayDone = 0;
                    pThis->ui4SpeedUpdateFlag = 0;
                }

                break;
            case CMD_MSG_LAST_FRAME_TRUE:
                  is_last_frame = true;
                  break;
            case CMD_MSG_LAST_FRAME_FALSE:
                  is_last_frame = false;
                  break;

            case CMD_MSG_CHANGE_AV:
                TSPLAYER_DBG("reveived CMD_MSG_CHANGE_AV. changeAV 1:block value = 0x%x\n", get_block_value());
                wait_block_out(WRAP(CMD_MSG_PLAY));

                pThis->ui4FlagWaitAv = 0;
                pThis->ui4FlagNotSend = 1;
                
                TSPLAYER_DBG("changeAV 2:block value = 0x%x\n", get_block_value());

                trigger_block_entry(WRAP(CMD_MSG_CHANGE_AV));
                trigger_block_exit(WRAP(CMD_MSG_GOTO_CHANGE_AV));
                TSPLAYER_DBG("IMtkPb_Ctrl_ChangeAV()now block value = 0x%x\n", get_block_value());

                if ((pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid != pThis->tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid)
                    || (pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc != pThis->tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc))
                {
                    if (pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid)
                    {
                        eCmpbRet = IMtkPb_Ctrl_ChangeAudio(pThis->ui4CmpbHandle, &pThis->tAudInfo);
                        if (eCmpbRet)
                        {
                            TSPLAYER_ERR("IMtkPb_Ctrl_ChangeAudio()=%d\n", eCmpbRet);
                            pThis->i4CmpbLastRet = 3;
                        }
                        else
                        {
                            TSPLAYER_DBG("IMtkPb_Ctrl_ChangeAudio(apid:%d) OK\n", pThis->tAudInfo.tMpgTsAudInfo.u2Pid);
                            trigger_block_entry(WRAP(CMD_MSG_AUDIO_CHG_DONE));
                        }
                    }
                    else
                    {
                        TSPLAYER_ERR("audio pid=0, must error, do not changed!!\n");
                        pThis->i4CmpbLastRet = 0;
                    }
                }
                else
                {
                    TSPLAYER_DBG("do NOT IMtkPb_Ctrl_ChangeAudio()\n");
                    pThis->ui4FlagWaitAv |= 2;
                }

                if ((pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid != pThis->tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid)
                    || (pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc != pThis->tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc))
                {
                    if (pThis->tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid)
                    {
                        if (pThis->ui4FlagWaitAv < 2) /*wait for audio change done*/
                        {
                            wait_block_out(WRAP(CMD_MSG_AUDIO_CHG_DONE));
                        }

                        eCmpbRet = IMtkPb_Ctrl_ChangeVideo(pThis->ui4CmpbHandle, &pThis->tVidInfo);
                        if (eCmpbRet)
                        {
                            TSPLAYER_ERR("IMtkPb_Ctrl_ChangeVideo()=%d\n", eCmpbRet);
                            pThis->i4CmpbLastRet = 3;
                        }
                        else
                        {
                            TSPLAYER_DBG("IMtkPb_Ctrl_ChangeVideo(vpid:%d) OK\n", pThis->tVidInfo.tMpgTsVidInfo.u2Pid);
                        }
                    }
                    else
                    {
                        TSPLAYER_ERR("video pid=0, must error, do not changed!!\n");
                        pThis->i4CmpbLastRet = 0;
                    }
                }
                else
                {
                    TSPLAYER_DBG("do NOT IMtkPb_Ctrl_ChangeVideo()\n");
                    pThis->ui4FlagWaitAv |= 1;
                }

                if (pThis->ui4FlagWaitAv == 3)
                {
                    pThis->ui4FlagNotSend = 0;
                    trigger_block_exit(WRAP(CMD_MSG_CHANGE_AV));
                    TSPLAYER_INFO("Warning: Not Changed:block value = 0x%x\n", get_block_value());
                }

                memcpy(&pThis->tLastMediaInfo, &pThis->tCurtMediaInfo, sizeof(pThis->tLastMediaInfo));

                TSPLAYER_DBG("OVER Change AV:block value = 0x%x ui4FlagWaitAv=%d\n", get_block_value(), pThis->ui4FlagWaitAv);
                
                break;

            case CMD_MSG_REPLAY:
                pThis->ui4FlagNotSend = 1;

                //wait_block_out(WRAP(CMD_MSG_PLAY) | WRAP(CMD_MSG_CHANGE_AV)); //等待上次play完成，快速换台的时候

                eCmpbRet = IMtkPb_Ctrl_Stop(pThis->ui4CmpbHandle);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_Stop(replay)=%d\n", eCmpbRet);
                    pThis->i4CmpbLastRet = 6;
                    pThis->ui4FlagNotSend = 0;
                    break;
                }

                memset(&tEngineParam, 0, sizeof(IMTK_PB_CTRL_ENGINE_PARAM_T));
                tEngineParam.u4PlayFlag = IMTK_PB_CTRL_PLAY_FLAG_VIDEO | IMTK_PB_CTRL_PLAY_FLAG_AUDIO;

                eCmpbRet = IMtkPb_Ctrl_SetEngineParam(pThis->ui4CmpbHandle, &tEngineParam);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_SetEngineParam(replay)=%d\n", eCmpbRet);
                }

                eCmpbRet = IMtkPb_Ctrl_SetMediaInfo(pThis->ui4CmpbHandle, &pThis->tCurtMediaInfo);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_SetMediaInfo(replay)=%d\n", eCmpbRet);
                }

                eCmpbRet = IMtkPb_Ctrl_Play(pThis->ui4CmpbHandle, 0);
                if (eCmpbRet)
                {
                    TSPLAYER_ERR("IMtkPb_Ctrl_Play(replay)=%d\n", eCmpbRet);
                    pThis->i4CmpbLastRet = 3;
                }

                pThis->ePlaySpeed = IMTK_PB_CTRL_SPEED_1X;
                pThis->ui4FlagNotSend = 0;
                pThis->ui4SpeedUpdateFlag = 0;
                TSPLAYER_DBG("replay OUT\n");
                break;

            default:
                TSPLAYER_ERR("cmd:%c not supported.\n", cCmd);
                break;
        }
    }

    TSPLAYER_DBG("playerCmdProcess() exit\n");
    return (void*)0;
}



#if ENABLE_SHOW_EXEC_TIME
long  CTsPlayer::getTimeMs(void)
{
    static long _tv_sec = 0;
    static long _tv_usec = 0;
    struct timeval tv;
    struct timezone tz;
    //unsigned long  u4_tm = (unsigned long) times(NULL);
    gettimeofday(&tv, &tz);
    if (_tv_sec == 0)
    {
        _tv_sec = tv.tv_sec;
        _tv_usec = tv.tv_usec;
        return 0;
    }
    else
    {
        return ( (tv.tv_sec - _tv_sec) * 1000) + ((tv.tv_usec - _tv_usec) / 1000);
    }
}
#endif


CTsPlayer::CTsPlayer()
{
    IMTK_PB_CTRL_ENGINE_PARAM_T     tEngineParam;
    IMTK_PB_ERROR_CODE_T eCmpbRet;
#if 0
    int policy = sched_getscheduler(0);
    int max = sched_get_priority_max(policy);
    int min = sched_get_priority_min(policy);


    switch (policy)
    {
        case SCHED_RR:
            TSPLAYER_DBG("current: SCHED_RR\n");
            break;

        case SCHED_OTHER:
            TSPLAYER_DBG("current: SCHED_OTHER\n");
            break;

        case SCHED_FIFO:
            TSPLAYER_DBG("current: SCHED_FIFO\n");
            break;
    }

    TSPLAYER_DBG("set to SCHED_FIFO:(%d-%d)\n", min, max);
#endif

    ui4CmpbHandle = 0;
    i4CmpbLastRet = 1;  //init error

    memset(&tLastMediaInfo, 0, sizeof(tLastMediaInfo));
    memset(&tCurtMediaInfo, 0, sizeof(tCurtMediaInfo));

    memset(&tAudInfo, 0, sizeof(tAudInfo));
    memset(&tVidInfo, 0, sizeof(tVidInfo));

    ui4RectX = 0;
    ui4RectY = 0;
    ui4RectW = 0;
    ui4RectH = 0;

    ui4WriteDataTimes = 0;
    ui4FirstPlayFlag = 1;

    i4AudioBalance = 0;

    ui4FlagNotSend = 1;
    ui4SpeedUpdateFlag = 0;
    ui4SendFailTimes = 0;

    ui4FlagWaitAv = 0;    // 3--can send data

    ui4FlagDummyPlay = 1;
    ePlaySpeed = IMTK_PB_CTRL_SPEED_1X;
    i4MsgQueHandle = 0;

    ui4FlagPlayDone = 0;

    

    eCmpbRet = IMtkPb_Ctrl_Open(&ui4CmpbHandle,
                                IMTK_PB_CTRL_BUFFERING_MODEL_PUSH,
                                IMTK_PB_CTRL_APP_MASTER,
#if ENABLE_LOCAL_TEST   // just for my own test
                                (uint8_t*)_pc_cmpb_src_tpye
#else
                                (uint8_t*)"IPTV"
#endif
                               ); /*mpeg_real  IPTV  0*/
    if (eCmpbRet)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_Open()=%d\n", eCmpbRet);
        return;
    }

    IMtkPb_Ctrl_RegCallback(ui4CmpbHandle,
                            (void*)this,
                            cmpbPlayCallback
                           );

    memset(&tEngineParam, 0, sizeof(IMTK_PB_CTRL_ENGINE_PARAM_T));
    tEngineParam.u4PlayFlag = IMTK_PB_CTRL_PLAY_FLAG_VIDEO | IMTK_PB_CTRL_PLAY_FLAG_AUDIO;

    eCmpbRet = IMtkPb_Ctrl_SetEngineParam(ui4CmpbHandle, &tEngineParam);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_SetEngineParam()=%d\n", eCmpbRet);
        IMtkPb_Ctrl_Close(ui4CmpbHandle);
        ui4CmpbHandle = 0;
        return;
    }

    if (msgq_create(&i4MsgQueHandle))
    {
        TSPLAYER_ERR("create msgq error\n");
        IMtkPb_Ctrl_Close(ui4CmpbHandle);
        ui4CmpbHandle = 0;
        return;
    }

    block_cond_init(0);

    if (pthread_create(&pidCmdProcess, NULL, playerCmdProcess, this))
    {
        TSPLAYER_ERR("create cmd process thread error\n");
        IMtkPb_Ctrl_Close(ui4CmpbHandle);
        block_cond_destroy();
        ui4CmpbHandle = 0;
        return;
    }

    i4CmpbLastRet = 0;
    
#if ENABLE_LOCAL_TEST
    if (_pc_cmpb_src_tpye)
    {
        TSPLAYER_DBG("OUT OK: srcType=%s\n", _pc_cmpb_src_tpye);
    }
    else
    {
        TSPLAYER_DBG("OUT OK: srcType=NULL\n");
    }
#endif

}

CTsPlayer::~CTsPlayer()
{
    IMTK_PB_ERROR_CODE_T eCmpbRet;
    IMTK_PB_CTRL_STATE_T eCurrentState = IMTK_PB_CTRL_UNKNOWN;
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return;
    }

    ui4FlagNotSend = 1; // make send data is retrurn directly

    TSPLAYER_DBG("~CTsPlayer IN, block=%d\n", get_block_value());

    cMsSendgBuf[0] = CMD_MSG_STOP;
    msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1);

    //send exit cmd to quit
    cMsSendgBuf[0] = CMD_MSG_EXIT;
    msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1);

    TSPLAYER_DBG("send exit, pthread_join()\n");

    pthread_join(pidCmdProcess, NULL);

    IMtkPb_Ctrl_GetState(ui4CmpbHandle, &eCurrentState);
    TSPLAYER_DBG("thread is exit, eCmpbState=%d\n", eCurrentState);
    /*if (eCurrentState == IMTK_PB_CTRL_PLAYING || eCurrentState == IMTK_PB_CTRL_PAUSED)
    {
        eCmpbRet = IMtkPb_Ctrl_Stop(ui4CmpbHandle);
        if (eCmpbRet)
        {
            TSPLAYER_ERR("IMtkPb_Ctrl_Stop()=%d\n", eCmpbRet);
        }
    }*/

#if ENABLE_KEEP_LAST_FRAME
    //IMtkPb_Ctrl_SetVideoFreeze(ui4CmpbHandle, 0);
#endif

    eCmpbRet = IMtkPb_Ctrl_Close(ui4CmpbHandle);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_Close()=%d\n", eCmpbRet);
    }

    ui4CmpbHandle = 0;

    //TSPLAYER_DBG("block_cond_destroy()\n");
    block_cond_destroy();

    msgq_delete(i4MsgQueHandle);
    i4MsgQueHandle = 0;

    TSPLAYER_DBG("OUT OK\n");
}

//取得播放模式,保留，暂不用
int  CTsPlayer::GetPlayMode()
{
    //TSPLAYER_DBG("return 1\n");
    return 1;
}

int CTsPlayer::SetVideoWindow(int x, int y, int width, int height)
{
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    TSPLAYER_DBG("x=%d y=%d w=%d h=%d\n", x, y, width, height);

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return -1;
    }

    if ((x != (int)ui4RectX) || (y != (int)ui4RectY) || (width != (int)ui4RectW) || (height != (int)ui4RectH))
    {
        ui4RectX = x;
        ui4RectY = y;
        ui4RectW = width;
        ui4RectH = height;

        TSPLAYER_DBG("CMD_MSG_SET_WINDOW.\n");

        cMsSendgBuf[0] = CMD_MSG_SET_WINDOW;
        if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
        {
            TSPLAYER_ERR("msgq_send(SetVideoWindow) error\n");
            return -1;
        }
    }

    return 0;
}


int CTsPlayer::SetColorKey(int enable, int key565)
{
    int ret = 0;
#if ENABLE_PRINT_DEBUG_LOG
    TSPLAYER_DBG("enable=%d key565=%d\n", enable, key565);
#else
    UNUSED(enable);
    UNUSED(key565);
#endif
    return ret;
}

int CTsPlayer::VideoShow(void)
{
    TSPLAYER_DBG("VideoShow:nothing to do, block=0x%x\n", get_block_value());
    return 0;

}
int CTsPlayer::VideoHide(void)
{
    TSPLAYER_DBG("VideoHide:nothing to do.\n");
    return 0;
}


int CTsPlayer::InitVideo(PVIDEO_PARA_T pVideoPara)
{
    IMTK_PB_CTRL_VID_ENC_T e_video_encode = IMTK_PB_CTRL_VID_ENC_UNKNOWN;

    if (!pVideoPara)
    {
        TSPLAYER_ERR("pVideoPara=NULL arg error\n");
        return -1;
    }
    if (pVideoPara->pid >= 65535)
    {
        TSPLAYER_ERR("get vpid error.\n");
        return -1;
    }

    TSPLAYER_DBG("IN vFmt:%d pid:%d vdW:%d vdH:%d nFrameRate:%d cFmt:%ld\n", (int)pVideoPara->vFmt, pVideoPara->pid, \
                 pVideoPara->nVideoWidth, pVideoPara->nVideoHeight, pVideoPara->nFrameRate, pVideoPara->cFmt);

    tCurtMediaInfo.eMediaType = IMTK_PB_CTRL_MEDIA_TYPE_MPEG2_TS;
    tCurtMediaInfo.u4TotalDuration = 0xFFFFFFFF;
    tCurtMediaInfo.u8Size = -1;
    tCurtMediaInfo.uFormatInfo.tTsInfo.ePacketType = IMTK_PB_CTRL_MPEG2_TS_PACKET_TYPE_188BYTE;


    switch (pVideoPara->vFmt)
    {
        case VFORMAT_MPEG12:
            e_video_encode = IMTK_PB_CTRL_VID_ENC_MPEG1_2;
            break;

        case VFORMAT_MPEG4:
            e_video_encode = IMTK_PB_CTRL_VID_ENC_MPEG4;
            break;

        case VFORMAT_H264:
            e_video_encode = IMTK_PB_CTRL_VID_ENC_H264;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.uVidCodecInfo.t_h264_info.eFrmRate = frameRateConvert(pVideoPara->nFrameRate);
            break;

        case VFORMAT_MJPEG:
            e_video_encode = IMTK_PB_CTRL_VID_ENC_MJPEG;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.uVidCodecInfo.t_mjpeg_info.eFrmRate = frameRateConvert(pVideoPara->nFrameRate);
            break;

        case VFORMAT_REAL:
            e_video_encode = IMTK_PB_CTRL_VID_ENC_RV9_10;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.uVidCodecInfo.t_rv9_10_info.eFrmRate = frameRateConvert(pVideoPara->nFrameRate);
            tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.uVidCodecInfo.t_rv9_10_info.u4Width = pVideoPara->nVideoWidth;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.uVidCodecInfo.t_rv9_10_info.u4Height = pVideoPara->nVideoHeight;
            break;

        case VFORMAT_VC1:
            e_video_encode = IMTK_PB_CTRL_VID_ENC_VC1;
            break;

        case VFORMAT_JPEG:

        case VFORMAT_AVS:

        case VFORMAT_SW:

        case VFORMAT_UNSUPPORT:
        default:
            TSPLAYER_ERR("video format %d is not supported\n", pVideoPara->vFmt);
            break;
    }


    tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc = e_video_encode;
    tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid = pVideoPara->pid;
    //tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.uVidCodecInfo.t_h264_info.eFrmRate = frameRateConvert(pVideoPara->nFrameRate);

    return 0;

}

/*
typedef struct
{
    unsigned short    pid;//pid
    int                nChannels;//声道数
    int                nSampleRate;//采样率
    aformat_t        aFmt;//音频格式
    int                nExtraSize;
    unsigned char*    pExtraData;
} AUDIO_PARA_T, *PAUDIO_PARA_T;

*/
int CTsPlayer::InitAudio(PAUDIO_PARA_T pAudioPara)
{
    IMTK_PB_CTRL_AUD_ENC_T e_audio_encode = IMTK_PB_CTRL_AUD_ENC_UNKNOWN;

    if (!pAudioPara)
    {
        TSPLAYER_ERR("pAudioPara=NULL arg error\n");
        return -1;
    }
    if (pAudioPara->pid >= 65535)
    {
        TSPLAYER_ERR("get apid error.\n");
        return -1;
    }

    TSPLAYER_DBG("IN aFmt:%d pid:%d nCh:%d nSamp:%d nEx:%d exData:%s\n", (int)pAudioPara->aFmt, pAudioPara->pid, pAudioPara->nChannels, \
                 pAudioPara->nSampleRate, pAudioPara->nExtraSize, (pAudioPara->pExtraData ? (char*)pAudioPara->pExtraData : "<empty>"));

    tCurtMediaInfo.eMediaType = IMTK_PB_CTRL_MEDIA_TYPE_MPEG2_TS;
    tCurtMediaInfo.u4TotalDuration = 0xFFFFFFFF;
    tCurtMediaInfo.u8Size = -1;
    tCurtMediaInfo.uFormatInfo.tTsInfo.ePacketType = IMTK_PB_CTRL_MPEG2_TS_PACKET_TYPE_188BYTE;

    switch (pAudioPara->aFmt)
    {
        case FORMAT_MPEG:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_MPEG;
            //e_audio_encode = IMTK_PB_CTRL_AUD_ENC_MP3;
            break;

        case FORMAT_PCM_S16LE:
        case FORMAT_PCM_S16BE:
        case FORMAT_PCM_U8:
        case FORMAT_PCM_BLURAY:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_PCM;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_pcm_info.eNumCh = (IMTK_PB_CTRL_AUD_CH_NUM_T)pAudioPara->nChannels;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_pcm_info.eSampleRate = sampleRateConvert(pAudioPara->nSampleRate);
            break;

        case FORMAT_AAC:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_AAC;
            break;

        case FORMAT_DTS:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_DTS;
            break;

        case FORMAT_FLAC:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_FLAC;
            //tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_flac_info.u2SampleNumHigh12 = 0;
            //tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_flac_info.u4SampleNumLow24 = 0;
            break;

        case FORMAT_COOK:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_COOK;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_cook_info.eNumCh = (IMTK_PB_CTRL_AUD_CH_NUM_T)pAudioPara->nChannels;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_cook_info.eSampleRate = sampleRateConvert(pAudioPara->nSampleRate);
            break;

        case FORMAT_WMA:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_WMA;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_wma_info.u2FormatTag = 0x161;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_wma_info.eNumCh = (IMTK_PB_CTRL_AUD_CH_NUM_T)pAudioPara->nChannels;
            tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.uAudCodecInfo.t_wma_info.eSampleRate = sampleRateConvert(pAudioPara->nSampleRate);
            break;

        case FORMAT_AC3:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_DD;
            TSPLAYER_DBG("audio format is AC3\n");
            break;

        case FORMAT_ALAC:
        case FORMAT_ALAW:
        case FORMAT_MULAW:
        default:
            e_audio_encode = IMTK_PB_CTRL_AUD_ENC_MP3;  // here set a default value: mp3, just for local test
            TSPLAYER_ERR("audio format %d is not supported\n", pAudioPara->aFmt);
            break;
    }


    tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc = e_audio_encode;
    tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid = pAudioPara->pid;

    return 0;
}

bool CTsPlayer::StartPlay()
{
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }

    TSPLAYER_DBG("play IN 08.06 async\n");

#if DUMP_TO_TS
    if (access("/usbdisk/dump_ts", F_OK) == 0) // check whether if exist file /usbdisk/dump_ts
    {
        pfile_dump = fopen("/usbdisk/ts_dump.ts", "wb");
        if (pfile_dump == NULL)
        {
            TSPLAYER_ERR("open file error\n");
        }
        else
        {
            TSPLAYER_DBG("open file for dump ts ok\n");
        }
    }
    else
    {
        TSPLAYER_DBG("nonexistence flag file:/usbdisk/dump_ts\n");
    }
#endif

    if (ui4FirstPlayFlag)  // the first play call IMtkPb_Ctrl_Play()
    {
        cMsSendgBuf[0] = CMD_MSG_SET_MEDIA_INFO;
        if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
        {
            TSPLAYER_ERR("msgq_send(setMediaInfo) error\n");
            i4CmpbLastRet = 2;
            return false;
        }

        TSPLAYER_DBG("START TO FIRST PLAY . \n");
        
        memset(cMsSendgBuf, 0, sizeof(cMsSendgBuf));
        cMsSendgBuf[0] = CMD_MSG_PLAY;
        if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
        {
            TSPLAYER_ERR("msgq_send(Play) error\n");
            i4CmpbLastRet = 3;
            return false;
        }

        memcpy(&tLastMediaInfo, &tCurtMediaInfo, sizeof(tLastMediaInfo));  // save to last info
        ui4FirstPlayFlag = 0;
        ui4FlagNotSend = 1; //do not allow to send data
    }
    else  // JUST change AV
    {
        TSPLAYER_DBG("It is not first play.block value = 0x%x\n", get_block_value());
        wait_block_out(WRAP(CMD_MSG_GOTO_PLAY) | WRAP(CMD_MSG_PLAY));  //等待stop() 函数的所有动作完成

        // just compare pid & format
        if ((tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid != tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid)
            || (tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid != tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid)
            || (tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc != tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc)
            || (tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc != tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc)
           )
        {
            TSPLAYER_DBG("C:vfmt:%d(pid=%d 0x%x) afmt:%d(pid=%d 0x%x)\n", tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc, \
                         tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid, tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid, \
                         tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc, tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid, \
                         tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid);
            TSPLAYER_DBG("L:vfmt:%d(pid=%d 0x%x) afmt:%d(pid=%d 0x%x)\n", tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.eVidEnc, \
                         tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid, tLastMediaInfo.uFormatInfo.tTsInfo.eVidInfo.u2Pid, \
                         tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.eAudEnc, tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid, \
                         tLastMediaInfo.uFormatInfo.tTsInfo.eAudInfo.u2Pid);

            memset(&tAudInfo, 0, sizeof(tAudInfo));
            memset(&tVidInfo, 0, sizeof(tVidInfo));

            memcpy(&tVidInfo.tMpgTsVidInfo, &(tCurtMediaInfo.uFormatInfo.tTsInfo.eVidInfo), sizeof(IMTK_PB_CTRL_MPEG2_TS_VID_INFO_T));
            memcpy(&tAudInfo.tMpgTsAudInfo, &(tCurtMediaInfo.uFormatInfo.tTsInfo.eAudInfo), sizeof(IMTK_PB_CTRL_MPEG2_TS_AUD_INFO_T));

            trigger_block_entry(WRAP(CMD_MSG_GOTO_CHANGE_AV));
            //保护changeAV的动作顺利完成，避免在本函数调用退出之后，立刻调用stop

            memset(cMsSendgBuf, 0, sizeof(cMsSendgBuf));
            cMsSendgBuf[0] = CMD_MSG_CHANGE_AV;
            if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
            {
                TSPLAYER_ERR("msgq_send(changeAV) error\n");
                i4CmpbLastRet = 3;
                return false;
            }

            //ui4FlagWaitAv = 0;  // 3--can send data, bug if the cmd run first
            ui4FlagNotSend = 1; //do not allow to send data
        }
        else
        {
            TSPLAYER_ERR("media info is NOT changed, nothing to do\n");
        }

    }

    ui4FlagDummyPlay = 0;
    ui4WriteDataTimes = 0;

    ui4SendFailTimes = 0;

    TSPLAYER_DBG("play OUT OK\n");

    return true;
}

int CTsPlayer::WriteData(unsigned char* pBuffer, unsigned int nSize)
{
    uint8_t*    pui1_buf;
    IMTK_PB_ERROR_CODE_T eCmpbRet;
#if DUMP_TO_TS
    int i4_wrt_size;
#endif
    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return 0;
    }

    if (pBuffer == NULL || nSize == 0)
    {
        TSPLAYER_ERR("pBuffer = 0x%x nSize = %u\n", (uint32_t)pBuffer, nSize);
        return 0;
    }

#if DUMP_TO_TS
    if (pfile_dump)
    {
        i4_wrt_size = fwrite(pBuffer, 1, nSize, pfile_dump);
        if (i4_wrt_size != (int)nSize)
        {
            TSPLAYER_ERR("dump write %d < %d\n", i4_wrt_size, nSize);
        }
        //return nSize;   //continue to play
    }
#endif

    if (i4CmpbLastRet)  //cmpb function error
    {
        ui4WriteDataTimes++;
        if ((ui4WriteDataTimes % 16) == 0)//防止打印太频繁
        {
            TSPLAYER_ERR("cmpb op error %d\n", i4CmpbLastRet);
        }
        TSPLAYER_DBG("because of cmpb function error, received PALYBACK_ERROR .\n");
        return nSize;
    }

    if (ui4FlagNotSend)
    {
        //TSPLAYER_DBG("reveived the ui4FlagNotSend.\n");
        if (ui4SendFailTimes == 0)
        {
            TSPLAYER_DBG("FIRST SendData when ui4FlagNotSend==TRUE\n");
        }
        else if ((ui4SendFailTimes % 16) == 0)
        {
            TSPLAYER_DBG("SendData when ui4FlagNotSend==TRUE  16 times\n");
        }
        ui4SendFailTimes++;
        
        //TSPLAYER_DBG("because of ui4FlagNotSend != 0, directly  return.\n");
        return 0;
    }

    eCmpbRet = IMtkPb_Ctrl_GetBuffer(ui4CmpbHandle, nSize, &pui1_buf);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_GetBuffer(size:%dkB)=%d\n", nSize/1024, eCmpbRet);
        usleep(40000);
        return 0;
    }

    memcpy(pui1_buf, pBuffer, nSize);

    eCmpbRet = IMtkPb_Ctrl_SendData(ui4CmpbHandle, nSize, pui1_buf);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_SendData()=%d\n", eCmpbRet);
        return 0;
    }

#if ENABLE_PRINT_DEBUG_LOG
    if (ui4WriteDataTimes == 0)
    {
        TSPLAYER_DBG("first SendData %d:\n", nSize);
        ui4WriteDataTimes = 1;
    }
    else
    {
        ui4WriteDataTimes++;
        if ((ui4WriteDataTimes % 32) == 0)
        {
            TSPLAYER_DBG("SendData size %dKB\n", nSize >> 5);
        }
    }
#else
    if (ui4WriteDataTimes == 0)
    {
        TSPLAYER_INFO("first SendData %d\n", nSize);
        ui4WriteDataTimes++;
    }
#endif
    return nSize;
}

bool CTsPlayer::Pause()
{
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    TSPLAYER_DBG("enter\n");

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }


    cMsSendgBuf[0] = CMD_MSG_PAUSE;
    if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
    {
        TSPLAYER_ERR("msgq_send(Pause) error\n");
        //i4CmpbLastRet = 4;
        return false;
    }

    return true;

}

bool CTsPlayer::Resume()
{
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    TSPLAYER_DBG("enter\n");

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }

    cMsSendgBuf[0] = CMD_MSG_RESUME;
    if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
    {
        TSPLAYER_ERR("msgq_send(Resume) error\n");
        //i4CmpbLastRet = 3;
        return false;
    }

    ui4SendFailTimes = 0;

    return true;
}

bool CTsPlayer::Fast()
{
    IMTK_PB_ERROR_CODE_T eCmpbRet;

    TSPLAYER_DBG("Fast IN.\n");

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }
    TSPLAYER_ERR("Fast,ui4FirstPlayFlag:%d \n",ui4FirstPlayFlag);

    if(ui4FirstPlayFlag == 1)
    {
        if(ui4FlagPlayDone == 0)
        {
            TSPLAYER_ERR("cannot setspeed before PLAY_DONE!!!\n");
            return false;
        }
    }
#if DUMP_TO_TS
    if (access("/usbdisk/dump_ts_fb", F_OK) == 0)
    {
        if (pfile_dump)
        {
            fclose(pfile_dump);
            pfile_dump = NULL;
        }
        pfile_dump = fopen("/usbdisk/ts_dump_fb.ts", "wb");
        if (pfile_dump == NULL)
        {
            TSPLAYER_ERR("open file error\n");
        }
        else
        {
            TSPLAYER_DBG("open file for dump ts ok\n");
        }
    }
    else
    {
        TSPLAYER_DBG("nonexistence flag file\n");
    }
#endif

    TSPLAYER_DBG("Fast() Setspeed1:block value = 0x%x\n", get_block_value());  //未起播的时候不能进入setspeed操作，否则接口调用会失败
    wait_block_out(WRAP(CMD_MSG_PLAY) | WRAP(CMD_MSG_CHANGE_AV) | WRAP(CMD_MSG_SET_SPEED));

    TSPLAYER_DBG("Fast()Setspeed2:block value = 0x%x\n", get_block_value());
    trigger_block_entry(WRAP(CMD_MSG_SET_SPEED));

    if(ePlaySpeed != IMTK_PB_CTRL_SPEED_FF_2X)
    {
        ui4SpeedUpdateFlag = 0;
        eCmpbRet = IMtkPb_Ctrl_SetSpeed(ui4CmpbHandle, IMTK_PB_CTRL_SPEED_FF_2X);
        if (eCmpbRet)
        {
            TSPLAYER_ERR("IMtkPb_Ctrl_SetSpeed(2X)=%d\n", eCmpbRet);
            trigger_block_exit(WRAP(CMD_MSG_SET_SPEED));
            TSPLAYER_DBG("Fast() set IMtkPb_Ctrl_SetSpeed error :block value = 0x%x\n", get_block_value());
        }
        else
        {
            TSPLAYER_DBG("IMtkPb_Ctrl_SetSpeed(2X) OK\n");
            ePlaySpeed = IMTK_PB_CTRL_SPEED_FF_2X;
        }

        ui4WriteDataTimes = 0;
        ui4SendFailTimes = 0;

        if (eCmpbRet)
        {
            TSPLAYER_ERR("out false\n");
            ui4FlagNotSend = 0; //set speed error, allow to send data

            return false;
        }
        else
        {
            if(ui4SpeedUpdateFlag)
            {
                ui4FlagNotSend = 0;
                ui4SpeedUpdateFlag = 0;
            }
            else
            {
                ui4FlagNotSend = 1;  //do not allow to send data, wait for change speed done
            }
            return true;
        }
    }
    else
    {
        TSPLAYER_DBG("Current Speed is IMTK_PB_CTRL_SPEED_FF_2X, do not need to setspeed again!!!\n");

        ui4WriteDataTimes = 0;
        ui4SendFailTimes = 0;

        trigger_block_exit(WRAP(CMD_MSG_SET_SPEED));

        return true;
    }
}

bool CTsPlayer::StopFast()
{
    IMTK_PB_ERROR_CODE_T eCmpbRet;

    TSPLAYER_DBG("StopFast IN.\n");

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }

#if DUMP_TO_TS
    if (pfile_dump)
    {
        fclose(pfile_dump);
        pfile_dump = NULL;
    }

    if (access("/usbdisk/dump_ts", F_OK) == 0)
    {
        if (pfile_dump)
        {
            fclose(pfile_dump);
            pfile_dump = NULL;
        }
        pfile_dump = fopen("/usbdisk/ts_dump.ts", "wb");
        if (pfile_dump == NULL)
        {
            TSPLAYER_ERR("open file error\n");
        }
        else
        {
            TSPLAYER_DBG("open file for dump ts ok\n");
        }
    }
    else
    {
        TSPLAYER_DBG("nonexistence flag file\n");
    }
#endif

    TSPLAYER_DBG("StopFast() Setspeed1:block value = 0x%x\n", get_block_value());
    wait_block_out(WRAP(CMD_MSG_SET_SPEED)); //等待fast操作完成

    //TSPLAYER_DBG("Setspeed2:block value = 0x%x\n", get_block_value());
    //trigger_block_entry(WRAP(CMD_MSG_SET_SPEED ));

    eCmpbRet = IMtkPb_Ctrl_SetSpeed(ui4CmpbHandle, IMTK_PB_CTRL_SPEED_1X);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_SetSpeed(1X)=%d\n", eCmpbRet);
        //trigger_block_exit(WRAP(CMD_MSG_SET_SPEED ));
        //TSPLAYER_DBG("set speed:block value = 0x%x\n", get_block_value());
    }
    else
    {
        TSPLAYER_DBG("IMtkPb_Ctrl_SetSpeed(1X) OK\n");
        ePlaySpeed = IMTK_PB_CTRL_SPEED_1X;
    }

    ui4WriteDataTimes = 0;
    ui4SendFailTimes = 0;
    ui4FlagNotSend = 0;  // 退出快退没有等待change speed done
    ui4SpeedUpdateFlag = 0;

    if (eCmpbRet)
    {
        TSPLAYER_ERR("out false\n");
        //ui4FlagNotSend = 0; //set speed error, allow to send data
        return false;
    }
    else
    {
        //ui4FlagNotSend = 1;  //do not allow to send data, wait for change speed done
        return true;
    }


}

bool CTsPlayer::Stop()
{
    IMTK_PB_CTRL_STATE_T eCmpbState;
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }

    TSPLAYER_DBG("Stop() IN\n");
    if(ePlaySpeed == IMTK_PB_CTRL_SPEED_FF_2X)
    {
        StopFast(); //if fast,then stop it
    }

    if (ui4FlagDummyPlay == 0)
    {
        wait_block_out(WRAP(CMD_MSG_GOTO_CHANGE_AV) | WRAP(CMD_MSG_CHANGE_AV)); //等待startplay的动作完成

        IMtkPb_Ctrl_GetState(ui4CmpbHandle, &eCmpbState);
        TSPLAYER_DBG("eCmpbState=%d\n", eCmpbState);

        if (eCmpbState == IMTK_PB_CTRL_PLAYING || eCmpbState == IMTK_PB_CTRL_PAUSED)
        {
            cMsSendgBuf[0] = CMD_MSG_STOP;
            if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
            {
                TSPLAYER_ERR("msgq_send(Stop) error\n");
                i4CmpbLastRet = 6;
            }

            cMsSendgBuf[0] = CMD_MSG_SET_MEDIA_INFO;
            if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
            {
                TSPLAYER_ERR("msgq_send(setMediaInfo) error\n");
                i4CmpbLastRet = 2;
                return false;
            }

            trigger_block_entry(WRAP(CMD_MSG_GOTO_PLAY));
            //保护startplay ()函数的动作能顺利完成，在下次stop之前

            memset(cMsSendgBuf, 0, sizeof(cMsSendgBuf));
            cMsSendgBuf[0] = CMD_MSG_PLAY;
            if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
            {
                TSPLAYER_ERR("msgq_send(Play) error\n");
                i4CmpbLastRet = 3;
                return false;
            }
        }

        ui4FlagDummyPlay = 1;
        TSPLAYER_DBG("Stop OUT, i4CmpbLastRet=%d\n", i4CmpbLastRet);
    }
    else
    {
        TSPLAYER_DBG("do not need stop\n");
    }

#if DUMP_TO_TS
    if (pfile_dump)
    {
        fclose(pfile_dump);
        pfile_dump = NULL;
    }
#endif

    TSPLAYER_DBG("tsPlayer stop over.\n");

    return true;
}

bool CTsPlayer::StopExt()
{
    IMTK_PB_CTRL_STATE_T eCmpbState;
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    if (ui4CmpbHandle == 0)
    {
        TSPLAYER_ERR("first open error, please check\n");
        return false;
    }

    TSPLAYER_DBG("StopExt() IN\n");

    if (ui4FlagDummyPlay == 1)
    {
        wait_block_out(WRAP(CMD_MSG_GOTO_CHANGE_AV) | WRAP(CMD_MSG_CHANGE_AV)); //等待startplay的动作完成

        IMtkPb_Ctrl_GetState(ui4CmpbHandle, &eCmpbState);
        TSPLAYER_DBG("eCmpbState=%d\n", eCmpbState);

        //if (eCmpbState == IMTK_PB_CTRL_PLAYING || eCmpbState == IMTK_PB_CTRL_PAUSED)
        //{
            cMsSendgBuf[0] = CMD_MSG_STOP;
            if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
            {
                TSPLAYER_ERR("msgq_send(Stop) error\n");
                i4CmpbLastRet = 6;
            }            
       // }
        
        ui4FirstPlayFlag = 1;
        ui4FlagDummyPlay = 1;
        TSPLAYER_DBG("StopExt OUT, i4CmpbLastRet=%d\n", i4CmpbLastRet);
    }
    else
    {
        TSPLAYER_DBG("do not need stopExt\n");
    }

#if DUMP_TO_TS
    if (pfile_dump)
    {
        fclose(pfile_dump);
        pfile_dump = NULL;
    }
#endif

    TSPLAYER_DBG("tsPlayer stopExt over.\n");

    return true;
}
bool CTsPlayer::Seek()
{
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};

    TSPLAYER_DBG("Seek IN.\n");


    cMsSendgBuf[0] = CMD_MSG_REPLAY;
    if (msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1))
    {
        TSPLAYER_ERR("msgq_send(Seek-replay) error\n");
        i4CmpbLastRet = 3;
        return false;
    }

    ui4SendFailTimes = 0;
    ui4FlagNotSend = 1;

    return true;

}

int CTsPlayer::GetVolume()
{
    uint8_t ui1Vol;

    if (IMtkPb_Misc_GetAudioVolume(IMTK_PB_MISC_AUD_CHANNEL_ALL, &ui1Vol))
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_GetAudioVolume() error\n");
        return 0;
    }
    TSPLAYER_DBG("out volume:%d\n", ui1Vol);

    return ui1Vol;
}

bool CTsPlayer::SetRatio(int nRatio)
{
#if ENABLE_PRINT_DEBUG_LOG
    TSPLAYER_DBG("nRatio=%d NOT implement at this moment\n", nRatio);
#else
    UNUSED(nRatio);
#endif
    return false;
}
bool CTsPlayer::SetVolume(int volume)
{
    uint8_t ui1Vol;

    TSPLAYER_DBG("volume=%d\n", volume);

    if (volume < 0)
    {
        TSPLAYER_ERR("arg error\n");
        return false;
    }

    if (IMtkPb_Misc_GetAudioVolume(IMTK_PB_MISC_AUD_CHANNEL_ALL, &ui1Vol))
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_GetAudioVolume() error\n");
        return false;
    }

    ui1Vol = (uint8_t)volume;

    if (IMtkPb_Misc_SetAudioVolume(IMTK_PB_MISC_AUD_CHANNEL_ALL, ui1Vol))
    {
        TSPLAYER_ERR("IMtkPb_Ctrl_SetAudioVolume() error\n");
        return false;
    }
    return true;
}
//获取当前声道,1:左声道，2:右声道，3:双声道
int CTsPlayer::GetAudioBalance()
{
    #if 0
    return 0;
    #else
    IMTK_PB_ERROR_CODE_T eCmpbRet;
    IMTK_PB_CTRL_AUD_DOWNMIX_MODE_T t_downmix_mode;

    TSPLAYER_DBG("current i4AudioBalance=%d\n", i4AudioBalance);

    eCmpbRet = IMtkPb_Ctrl_GetAudDownmixMode(&t_downmix_mode);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("get audio downmix mode error\n");
        i4AudioBalance = 3;
        return 3;
    }

    if (t_downmix_mode == IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL1)//左声道
    {
        i4AudioBalance = 1;
    }
    else if (t_downmix_mode == IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL2)//右声道
    {
        i4AudioBalance = 2;
    }
    else
    {
        i4AudioBalance = 3;
    }

    TSPLAYER_DBG("i4AudioBalance=%d  t_downmix_mode=%d\n", i4AudioBalance, (int)t_downmix_mode);

    /*记录声道*/
    return i4AudioBalance;
    #endif
}

//设置声道
//nAudioBlance:,1:左声道，2:右声道，3:双声道
bool CTsPlayer::SetAudioBalance(int nAudioBalance)
{
    #if 0
    nAudioBalance =0;
    return 0;
    #else
    IMTK_PB_ERROR_CODE_T eCmpbRet;
    IMTK_PB_CTRL_AUD_DOWNMIX_MODE_T t_downmix_mode;

    TSPLAYER_DBG("nAudioBalance=%d\n", nAudioBalance);

    if (nAudioBalance == 1) /*左声道*/
    {
        t_downmix_mode = IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL1;
    }
    else if (nAudioBalance == 2) /*右声道*/
    {
        t_downmix_mode = IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL2;
    }
    else if (nAudioBalance == 3)/*双声道*/
    {
        t_downmix_mode = IMTK_PB_CTRL_AUD_DOWNMIX_MODE_DUAL_OFF;
    }
    else
    {
        TSPLAYER_ERR("arg error\n");
        return false;
    }

    eCmpbRet = IMtkPb_Ctrl_SetAudDownmixMode(t_downmix_mode);
    if (eCmpbRet)
    {
        TSPLAYER_ERR("set audio downmix mode error\n");
        return false;
    }

    i4AudioBalance = nAudioBalance;
    return true;
    #endif
}
//获取视频分辩率
void CTsPlayer::GetVideoPixels(int& width, int& height)
{
    width = 1000;
    height = 1000;
    //TSPLAYER_DBG("(1000, 1000)\n");
}
bool CTsPlayer::IsSoftFit()
{
    //TSPLAYER_DBG("return true\n");
    return true;
}
void CTsPlayer::SetEPGSize(int w, int h)
{
#if ENABLE_PRINT_DEBUG_LOG
    TSPLAYER_DBG("w=%d h=%d\n", w, h);
    if (IsSoftFit())
    {
        TSPLAYER_DBG("nothing to do at IsSoftFit() return true\n");
    }
    else
    {
        TSPLAYER_ERR("NOT implement at IsSoftFit() return false\n");
    }
#else
    UNUSED(w);
    UNUSED(h);
#endif
}


IMTK_PB_CTRL_FRAME_RATE_T CTsPlayer::frameRateConvert(int i4FrameRate)
{
    if (i4FrameRate == 25)
    {
        TSPLAYER_DBG("i4FrameRate= 25 = 25\n");
        return IMTK_PB_CTRL_FRAME_RATE_25;
    }
    else
    {
        TSPLAYER_DBG("i4FrameRate=%d\n", i4FrameRate);
        return IMTK_PB_CTRL_FRAME_RATE_VARIABLE;
    }
}


////////////////for tianfu

void  CTsPlayer::enable_keep_last_frame( bool istrue)
{
    char cMsSendgBuf[CMD_MSG_MAX_LEN] = {0};
    if(istrue)
       cMsSendgBuf[0] = CMD_MSG_LAST_FRAME_TRUE;
    else
         cMsSendgBuf[0] = CMD_MSG_LAST_FRAME_FALSE;
    msgq_send(i4MsgQueHandle, (void*)cMsSendgBuf, strlen(cMsSendgBuf) + 1);
}

IMTK_PB_CTRL_AUD_SAMPLE_RATE_T CTsPlayer::sampleRateConvert(int i4SampleRate)
{
    switch (i4SampleRate)
    {
        case 8000:
            TSPLAYER_DBG("i4SampleRate: 8K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_8K;

        case 16000:
            TSPLAYER_DBG("i4SampleRate: 16K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_16K;

        case 32000:
            TSPLAYER_DBG("i4SampleRate: 32K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_32K;

        case 11025:
            TSPLAYER_DBG("i4SampleRate: 11K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_11K;

        case 22050:
            TSPLAYER_DBG("i4SampleRate: 22K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_22K;

        case 44100:
            TSPLAYER_DBG("i4SampleRate: 44K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_44K;

        case 12000:
            TSPLAYER_DBG("i4SampleRate: 12K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_12K;

        case 24000:
            TSPLAYER_DBG("i4SampleRate: 24K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_24K;

        case 48000:
            TSPLAYER_DBG("i4SampleRate: 48K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_48K;

        case 96000:
            TSPLAYER_DBG("i4SampleRate: 96K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_96K;

        case 192000:
            TSPLAYER_DBG("i4SampleRate: 192K\n");
            return IMTK_PB_CTRL_AUD_SAMPLE_RATE_192K;

        default:
            TSPLAYER_ERR("i4SampleRate %d is not supported, use default 16k\n", i4SampleRate);
            break;

    }

    return IMTK_PB_CTRL_AUD_SAMPLE_RATE_16K;
}
int   CTsPlayer::GetTSInfo(void* pBuf, uint32_t nLen, IMTK_PB_CTRL_TS_SINGLE_PAT_INFO_T*  ptTSInfo)
{
    if (pBuf && ptTSInfo && nLen > 0)
    {
        return (int)IMtkPb_Ctrl_GetTSInfo((uint8_t*)pBuf, nLen, ptTSInfo);
    }
    else
    {
        TSPLAYER_ERR("arg error!\n");
        return -1;
    }
}
int   CTsPlayer::GetCurrentPTS(uint64_t* video_pts, uint64_t* audio_pts)
{
    if (audio_pts && video_pts)
    {
        return (int)IMtkPb_Ctrl_GetCurrentPTS(ui4CmpbHandle, video_pts, audio_pts);
    }
    else
    {
        TSPLAYER_ERR("arg error!\n");
        return -1;
    }
}


extern "C" {
    int x_rtos_init (GEN_CONFIG_T*  pt_config);
    int handle_init (unsigned short   ui2_num_handles,
                     void**   ppv_mem_addr,
                     unsigned int*  pz_mem_size);
    int os_init(const void* pv_addr, unsigned int z_size);
    int c_rpc_init_client(void);
    int c_rpc_start_client(void);
}

static void _rpc_env_init(void)
{
    static int rpc_inited = 0;
    GEN_CONFIG_T  t_rtos_config;
    void*       pv_mem_addr = 0;
    unsigned int z_mem_size = 0xc00000;

    if (rpc_inited == 0)
    {
        memset(&t_rtos_config, 0, sizeof(GEN_CONFIG_T));
        x_rtos_init (&t_rtos_config);
        handle_init (MAX_NUM_HANDLES, &pv_mem_addr, &z_mem_size);
        os_init (pv_mem_addr, z_mem_size);

        c_rpc_init_client();
        c_rpc_start_client();

        rpc_inited = 1;
    }
}


ITsPlayer* GetTsPlayer()
{
    _rpc_env_init();
    return  new CTsPlayer();
}


#if ENABLE_LOCAL_TEST
ITsPlayer* GetTsPlayerForTest(char* srcType)
{
    _rpc_env_init();
    if (srcType)
    {
        memset(_c_src_type, 0, 24);
        strncpy(_c_src_type, srcType, 23);
        _pc_cmpb_src_tpye = _c_src_type;
    }
    else
    {
        _pc_cmpb_src_tpye = NULL;
    }
    return new CTsPlayer();
}
#endif


////////////for tianfu




