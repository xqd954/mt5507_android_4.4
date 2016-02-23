#ifndef MTK_IPTV_MEDIAPLAYER
#define MTK_IPTV_MEDIAPLAYER

#include "IPTVMediaPlayerStub.h"
#include "TsPlayer.h"


using namespace iptv;
enum playerState{NONE = 1,INITED, PLAYING,FAST,PAUSE,STOPED};


class MTKIPTVMediaPlayer : public IPTVMediaPlayerStub
{

	public :
		
		MTKIPTVMediaPlayer();
		int32_t  initialize(void*);

		int32_t  clearBuffer();

		int32_t  play();

		int32_t  pause();

		int32_t  resume();

		int32_t  stop();
		int32_t  stopExt();

		int32_t  setAVSync(bool isSync);

		uint64_t getCurrentPosition();

		int32_t  setDisplayArea(uint32_t left, uint32_t top, uint32_t width, uint32_t height);

		int32_t  setVolume(uint32_t volume);

		uint32_t getVolume();
		
		//////////////////////////////////////set audio balance
		bool  setAudioBalance(uint32_t nAudioBalance);
			
		uint32_t getAudioBalance();
		
		///////////////////////////set lastframe /false  or true
		
		void setlastframe(bool nEnable_keep_last_frame);
			
			
		
		int32_t  setMute(bool isMute);
		
        int32_t  fastForward(int);
       
        int32_t  fastRewind(int);

		int32_t  finalize();
		
		int32_t setReadBufferCallback(ReadBufferCallback  m_readbufferfunction);
		
		virtual ~MTKIPTVMediaPlayer();
		
		  bool getWriteThreadRunSign()
		    {
		          return isWriteThreadRun;
		    }
			  
			playerState getPlayerState()
			{
				return mPlayerState;
			}
	
			int32_t setMediaInfo(char* buf,int size);
			
			int32_t setIPTVbackground();
			int32_t defaultbackground();
		
private:
	int32_t realPlay();
	ITsPlayer* pTsplayer;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int mVolume;
	int mWidthPixels;
	int mHeightPixels;
	bool isWriteThreadRun;
	playerState mPlayerState;
	pthread_t mWriteThreadPid;
	bool isSetMediaInfo;

	ReadBufferCallback  m_readbuffer;
	static void* thread_write_data(void* param);



};
#endif
