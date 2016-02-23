#ifndef _IPTVMEDIAPLAYER_STUB_H_
#define _IPTVMEDIAPLAYER_STUB_H_

#ifndef _IPTVMEDIAPLAYER_H_
#include <IPTVMediaPlayer.h>
#endif

#ifdef LOCAL_DEBUG
#undef LOCAL_DEBUG
#endif

using namespace iptv;

class IPTVMediaPlayerStub : public IPTVMediaPlayer
{
public:

      virtual  int32_t  initialize(void*);

      virtual  int32_t  setReadBufferCallback(ReadBufferCallback callback);

      virtual  int32_t  clearBuffer();

      virtual  int32_t  setOnErrorCallback(OnErrorCallback callback);

      virtual  int32_t  play();

      virtual  int32_t  pause();

      virtual  int32_t  resume();

      virtual  int32_t  stop();
      virtual int32_t  stopExt();

      virtual  int32_t  setAVSync(bool isSync);

      virtual  uint64_t getCurrentPosition();

      virtual  int32_t  setDisplayArea(uint32_t left, uint32_t top, uint32_t width, uint32_t height);

      virtual  int32_t  setVolume(uint32_t volume);

      virtual  uint32_t getVolume();

      virtual  int32_t  setMute(bool isMute);

      virtual  bool     isMute();
	  //add by lirong
      virtual  int32_t  fastForward(int);
       
      virtual  int32_t  fastRewind(int);

      virtual  int32_t  finalize();
      
      //////////////////////////////////////set audio balance
		  virtual  bool  setAudioBalance(uint32_t nAudioBalance);
			
		  virtual  uint32_t getAudioBalance();
		
		///////////////////////////set lastframe /false  or true
		
		  virtual  void setlastframe(bool nEnable_keep_last_frame);
      
      virtual  int32_t setIPTVbackground();
			virtual  int32_t defaultbackground();

      virtual  ~IPTVMediaPlayerStub() { };
      
      ReadBufferCallback mReadBuffer;

      OnErrorCallback mOnErrorCallback;

};

#endif

