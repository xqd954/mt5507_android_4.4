播放TS流:
播放以0x47开头的188字节的TS流
CTsPlayer基类:
class ITsPlayer{
public:
	ITsPlayer(){}
	virtual ~ITsPlayer(){}
public:
	//取得播放模式
	virtual int  GetPlayMode()=0;
	//显示窗口
	virtual int  SetVideoWindow(int x,int y,int width,int height)=0;
	//16位色深需要设置colorkey来透出视频；
	virtual int  SetColorKey(int enable,int key565)=0;
	//x显示视频
	virtual int  VideoShow(void)=0;
	//隐藏视频
	virtual int  VideoHide(void)=0;
	//初始化视频参数
	virtual void InitVideo(PVIDEO_PARA_T pVideoPara)=0;
	//初始化音频参数
	virtual void InitAudio(PAUDIO_PARA_T pAudioPara)=0;
	//开始播放
	virtual bool StartPlay()=0;
	//把ts流写入
	virtual int WriteData(unsigned char* pBuffer, unsigned int nSize)=0;
	//暂停
	virtual bool Pause()=0;
	//继续播放
	virtual bool Resume()=0;
	//快进快退
	virtual bool Fast()=0;
	//停止快进快退
	virtual bool StopFast()=0;
	//停止
	virtual bool Stop()=0;
    //定位
    virtual bool Seek()=0;
    //设定音量
	//设定音量
	virtual bool SetVolume(int volume)=0;
	//获取音量
	virtual int GetVolume()=0;
	//设定视频显示比例
	virtual bool SetRatio(int nRatio)=0;
	//获取当前声道
	virtual int GetAudioBalance()=0;
	//设置声道
	virtual bool SetAudioBalance(int nAudioBalance)=0;
	//获取视频分辩率
	virtual void GetVideoPixels(int& width, int& height)=0;
	//是否由软件拉伸，如果由硬件拉伸，请返回false
	virtual bool IsSoftFit()=0;
	//设置EPG大小，标清固定640*530，高清是1280*720，在高清平台中有些页面还是640*530，会随时在这两种分辩率中切换，所以要做到根据不同分辩率来进行拉伸
	virtual void SetEPGSize(int w, int h);
};

实现CTsPlayer类:

typedef struct{
	unsigned short	pid;//pid
	int				nVideoWidth;//视频宽度
	int				nVideoHeight;//视频高度
	int				nFrameRate;//帧率
	vformat_t		vFmt;//视频格式 IPTV的为VFORMAT_H264,看vformat.h
	unsigned long	cFmt;//编码格式 IPTV的为CODEC_TAG_H264
}VIDEO_PARA_T, *PVIDEO_PARA_T;
typedef struct{
	unsigned short	pid;//pid
	int				nChannels;//声道数
	int				nSampleRate;//采样率
	aformat_t		aFmt;//音频格式 IPTV的为AFORMAT_MPEG,看aformat.h
	int				nExtraSize;
	unsigned char*	pExtraData;	
}AUDIO_PARA_T, *PAUDIO_PARA_T;


类中具体方法如下:(可直接看TsPlayer.cpp)
	//取得播放模式，保留，直接返回1
	int  GetPlayMode();
	//显示窗口
	int  SetVideoWindow(int x,int y,int width,int height);
	//设置colorkey来透出视频；32位不用，返回-1
	int  SetColorKey(int enable,int key565);
	//显示视频
	int  VideoShow(void);
	//隐藏视频
	int  VideoHide(void);
	//初始化视频参数
	void InitVideo(PVIDEO_PARA_T pVideoPara);
	//初始化音频参数
	void InitAudio(PAUDIO_PARA_T pAudioPara);
	//开始播放
	bool StartPlay();
	//把ts流写入
	int WriteData(unsigned char* pBuffer, unsigned int nSize);
	//暂停
	bool Pause();
	//继续播放
	bool Resume();
	//快进快退,调用Fast后收到的TS流只有视频没有音频,不需要进行同步,收到一帧显示一帧
	bool Fast();
	//停止快进快退,之后收到的TS流为正常播放的,音视频都有,需要重新同步
	bool StopFast();
	//停止
	bool Stop();
	//定位,需要重新同步音视频
	bool Seek();
	//设定音量,该音量为当前影片的音量,而不是系统音量(0~100)
	bool SetVolume(int volume);
	//获取音量,该音量为当前影片的音量,而不是系统音量(0~100)
	int GetVolume();
	//设定视频显示比例,预留暂不用实现
	bool SetRatio(int nRatio);
	//获取当前声道
	int GetAudioBalance();
	//设置声道
	bool SetAudioBalance(int nAudioBalance);
	//获取视频系统显示分辩率，不是当前影片的分辩率
	void GetVideoPixels(int& width, int& height);
	//是否由软件拉伸，如果由硬件拉伸，请返回false
	virtual bool IsSoftFit()=0;
	//设置EPG大小，标清固定640*530，高清是1280*720，在高清平台中有些页面还是640*530，会随时在这两种分辩率中切换，所以要做到根据不同分辩率来进行拉伸
	virtual void SetEPGSize(int w, int h);

开始播放时,调用顺序:
InitVideo
InitAudio
SetVideoWindow
SetColorKey
VideoShow
StartPlay

新加一个函数
ITsPlayer* GetTsPlayer();
在cpp中
ITsPlayer* GetTsPlayer()
{
	return new CTsPlayer();
}
