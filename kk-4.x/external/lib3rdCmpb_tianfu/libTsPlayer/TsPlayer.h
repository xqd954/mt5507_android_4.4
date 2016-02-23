#ifndef _TSPLAYER_H_
#define _TSPLAYER_H_
#include <stdio.h>
#include <stdlib.h>
extern "C" {
#include "vformat.h"
#include "aformat.h"
}

#include <pthread.h>
#include <semaphore.h>
#include "IMtkPb_Ctrl.h"
#include "IMtkPb_Ctrl_DTV.h"

//#define ENABLE_VIDEO_HIDE_SHOW  (0)    /*让video show & hide 有实际的动作*/
#define ENABLE_PRINT_DEBUG_LOG  (1)    /*显示debug log 信息*/
#define ENABLE_WAIT_CHANGE_AV   (1)    /*1 -- 等待changeVideo & changeAudio 完成的时候再送数据*/
#define ENABLE_LOCAL_TEST       (0)    /*1 -- 编译本地测试的版本0 -- 编译实际使用的版本*/

#define ENABLE_KEEP_LAST_FRAME  (1)    /*换台保留最后一帧*/
#define ENABLE_DO_NOT_BLOCK_WRITE_DATA   (1)   /*1 -- 不阻塞WriteData()函数调用，直接返回失败  0 -- 阻塞*/


#if ENABLE_PRINT_DEBUG_LOG
#define ENABLE_SHOW_EXEC_TIME   (1)    /*打印TsPlyer 中函数执行的时间点，相对时间*/
#else
#define ENABLE_SHOW_EXEC_TIME   (0)
#endif


#define TRICKMODE_NONE       0x00
#define TRICKMODE_I          0x01
#define TRICKMODE_FFFB       0x02


typedef struct
{
    unsigned short	pid;//pid
    int				nVideoWidth;//视频宽度
    int				nVideoHeight;//视频高度
    int				nFrameRate;//帧率
    vformat_t		vFmt;//视频格式
    unsigned long	cFmt;//编码格式
} VIDEO_PARA_T, *PVIDEO_PARA_T;
typedef struct
{
    unsigned short	pid;//pid
    int				nChannels;//声道数
    int				nSampleRate;//采样率
    aformat_t		aFmt;//音频格式
    int				nExtraSize;
    unsigned char*	pExtraData;
} AUDIO_PARA_T, *PAUDIO_PARA_T;

class CTsPlayer;

class ITsPlayer
{
public:
    ITsPlayer() {}
    virtual ~ITsPlayer() {}
public:
    //取得播放模式
    virtual int  GetPlayMode() = 0;
    //显示窗口
    virtual int  SetVideoWindow(int x, int y, int width, int height) = 0;
    //16位色深需要设置colorkey来透出视频；
    virtual int  SetColorKey(int enable, int key565) = 0;
    //x显示视频
    virtual int  VideoShow(void) = 0;
    //隐藏视频
    virtual int  VideoHide(void) = 0;
    //初始化视频参数
    virtual int InitVideo(PVIDEO_PARA_T pVideoPara) = 0;
    //初始化音频参数
    virtual int InitAudio(PAUDIO_PARA_T pAudioPara) = 0;
    //开始播放
    virtual bool StartPlay() = 0;
    //把ts流写入
    virtual int WriteData(unsigned char* pBuffer, unsigned int nSize) = 0;
    //暂停
    virtual bool Pause() = 0;
    //继续播放
    virtual bool Resume() = 0;
    //快进快退
    virtual bool Fast() = 0;
    //停止快进快退
    virtual bool StopFast() = 0;
    //停止
    virtual bool Stop() = 0;

    virtual bool StopExt() = 0;
    //定位
    virtual bool Seek() = 0;
    //设定音量
    //设定音量
    virtual bool SetVolume(int volume) = 0;
    //获取音量
    virtual int GetVolume() = 0;
    //设定视频显示比例
    virtual bool SetRatio(int nRatio) = 0;
    //获取当前声道
    virtual int GetAudioBalance() = 0;
    //设置声道
    virtual bool SetAudioBalance(int nAudioBalance) = 0;
    //获取视频分辩率
    virtual void GetVideoPixels(int& width, int& height) = 0;
    virtual bool IsSoftFit() = 0;
    virtual void SetEPGSize(int w, int h) = 0;
    
//////////////////////for tianfu

uint32_t ui4CmpbHandle;   //cmpb handle

virtual void  enable_keep_last_frame( bool istrue) = 0; /// ENABLE_KEEP_LAST_FRAME = 1;


    virtual int   GetTSInfo(void* pBuf, uint32_t nLen, IMTK_PB_CTRL_TS_SINGLE_PAT_INFO_T*  ptTSInfo) = 0;
    virtual int   GetCurrentPTS(uint64_t* video_pts, uint64_t* audio_pts) = 0;

};



class CTsPlayer : public ITsPlayer
{
public:
    CTsPlayer();
    virtual ~CTsPlayer();
public:
    //取得播放模式
    virtual int  GetPlayMode();
    //显示窗口
    virtual int  SetVideoWindow(int x, int y, int width, int height);
    //16位色深需要设置colorkey来透出视频；
    virtual int  SetColorKey(int enable, int key565);
    //x显示视频
    virtual int  VideoShow(void);
    //隐藏视频
    virtual int  VideoHide(void);
    //初始化视频参数
    virtual int InitVideo(PVIDEO_PARA_T pVideoPara);
    //初始化音频参数
    virtual int InitAudio(PAUDIO_PARA_T pAudioPara);
    //开始播放
    virtual bool StartPlay();
    //把ts流写入
    virtual int WriteData(unsigned char* pBuffer, unsigned int nSize);
    //暂停
    virtual bool Pause();
    //继续播放
    virtual bool Resume();
    //快进快退
    virtual bool Fast();
    //停止快进快退
    virtual bool StopFast();
    //停止
    virtual bool Stop();

    virtual bool StopExt();
    //定位
    virtual bool Seek();
    //设定音量
    //设定音量
    virtual bool SetVolume(int volume);
    //获取音量
    virtual int GetVolume();
    //设定视频显示比例
    virtual bool SetRatio(int nRatio);
    //获取当前声道
    virtual int GetAudioBalance();
    //设置声道
    virtual bool SetAudioBalance(int nAudioBalance);
    //获取视频分辩率
    virtual void GetVideoPixels(int& width, int& height);
    virtual bool IsSoftFit();
    virtual void SetEPGSize(int w, int h);
    
    /////////////////////////for tianfu
    
    virtual void  enable_keep_last_frame( bool istrue); /// ENABLE_KEEP_LAST_FRAME = 1;
    virtual int   GetTSInfo(void* pBuf, uint32_t nLen, IMTK_PB_CTRL_TS_SINGLE_PAT_INFO_T*  ptTSInfo);
    virtual int   GetCurrentPTS(uint64_t* video_pts, uint64_t* audio_pts);


    //thread for execute player command
    static void* playerCmdProcess(void* arg);
    //cmpb callback
    static IMTK_PB_CB_ERROR_CODE_T cmpbPlayCallback(IMTK_PB_CTRL_EVENT_T eEventType, void* pvTag,  uint32_t u4Data);

#if ENABLE_SHOW_EXEC_TIME
    //get current time: ms  (relatively)
    static long  getTimeMs(void);
#endif




///////////////////////////////

private:
    IMTK_PB_CTRL_FRAME_RATE_T frameRateConvert(int i4_frame_rate);
    IMTK_PB_CTRL_AUD_SAMPLE_RATE_T sampleRateConvert(int sample_rate);

private:
    
    int i4CmpbLastRet;       // 0--no error  1--init error  2--setmediainfo error  3--play/resume error  4--pause error  5--setspeed error  6--stop error
    IMTK_PB_CTRL_SET_MEDIA_INFO_T   tLastMediaInfo;   // media info last play info
    IMTK_PB_CTRL_SET_MEDIA_INFO_T   tCurtMediaInfo;   // media info this time play info
    IMTK_PB_CTRL_AUD_INFO_T tAudInfo;                 // audio info  for changeAudio
    IMTK_PB_CTRL_VID_INFO_T tVidInfo;                 // video info  for changeVideo

    uint32_t ui4RectX;        //video show area: x
    uint32_t ui4RectY;        //video show area: y
    uint32_t ui4RectW;        //video show area: width
    uint32_t ui4RectH;        //video show area: height
    
    

    uint32_t ui4WriteDataTimes; /* show times of sending data*/
    uint32_t ui4FirstPlayFlag;  /*just for the first time play*/
    uint32_t ui4SpeedUpdateFlag;  /*避免SPEED_UPDATE 比IMtk_Pb_Ctrl_SetSpeed 返回要早的时候出问题*/

    int i4AudioBalance;//1:左声道，2:右声道，3:双声道

#if ENABLE_DO_NOT_BLOCK_WRITE_DATA
    uint32_t ui4FlagNotSend;    /* 0 -- can  send data   1 -- can not send data，在速影不能保证writeData 和play/stop/pause等动作不同步的时候需要这个*/
    uint32_t ui4SendFailTimes;  /* 未成功送入数据的次数*/
#endif

#if ENABLE_WAIT_CHANGE_AV
    uint32_t ui4FlagWaitAv;           // 3--can send data  1--video 2--audio
#endif

    uint32_t ui4FlagPlayDone;

    uint32_t ui4FlagDummyPlay;       // 0 -- call start play  1 -- dummy play  (because of  function CTsPlayer::Stop() will  be called twice by Suing in one play )
    IMTK_PB_CTRL_SPEED_T ePlaySpeed; // current play speed, for CTsPlayer::Fast() & CTsPlayer::StopFast()

    int i4MsgQueHandle;          // msg queue handle which used to send command to thread for member function of CTsPlayer
    pthread_t   pidCmdProcess;   // pid of thread which receive command and excute command(play/stop/setmediainfo)

    /*使用异步执行的方式来提高起播速度，异步执行的动作都在另外一个线程来进行，
      即在上层调用CTsPlayer::Stop() 的时候完成cmpb 的IMtkPb_Ctrl_Stop(), IMtkPb_Ctrl_SetMediaInfo(), IMtkPb_Ctrl_Play()，
      等真正上层调用CTsPlayer::StartPlay()的时候再判断mediaInfo 信息是否有改变，如果有改变，
      则调用changeAudio & changeVideo 来更改(因为上海电信那里media info很少改变，所以可以提供起播速度)
       */
};

ITsPlayer* GetTsPlayer();

#if ENABLE_LOCAL_TEST
ITsPlayer* GetTsPlayerForTest(char* srcType);
#endif

#endif
