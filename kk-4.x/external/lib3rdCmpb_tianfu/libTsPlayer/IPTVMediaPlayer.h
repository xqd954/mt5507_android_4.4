#ifndef _IPTVMEDIAPLAYER_H_
#define _IPTVMEDIAPLAYER_H_

#include <stddef.h>
#include <stdint.h>

namespace iptv {

typedef int32_t (*ReadBufferCallback)(unsigned char *buffer, int32_t bufferSize);

typedef void (*OnErrorCallback)(int32_t errorCode);

class IPTVMediaPlayer
{
public:

      virtual  int32_t  initialize(void *) = 0;

      virtual  int32_t  setReadBufferCallback(ReadBufferCallback readBuffer) = 0;

      virtual  int32_t  clearBuffer() = 0;

      virtual  int32_t  setOnErrorCallback(OnErrorCallback onError) = 0;

      virtual  int32_t  play() = 0;

      virtual  int32_t  pause() = 0;

      virtual  int32_t  resume() = 0;

      virtual  int32_t  stop() = 0;

      virtual  int32_t  stopExt() = 0;
      virtual  int32_t  setAVSync(bool isSync) = 0;

      virtual  uint64_t getCurrentPosition() = 0;

      virtual  int32_t  setDisplayArea(uint32_t left, uint32_t top, uint32_t width, uint32_t height) = 0;

      virtual  int32_t  setVolume(uint32_t volume) = 0;

      virtual  uint32_t getVolume() = 0;

      virtual  int32_t  setMute(bool isMute) = 0;

      virtual  bool     isMute() = 0;

      virtual  int32_t  finalize() = 0;
            
      virtual  int32_t  fastForward(int) = 0;
       
      virtual  int32_t  fastRewind(int) = 0;
      
       //////////////////////////////////////set audio balance
		  virtual  bool  setAudioBalance(uint32_t nAudioBalance) = 0;
			
		  virtual  uint32_t getAudioBalance() = 0;
		
		///////////////////////////set lastframe /false  or true
		
		  virtual  void setlastframe(bool nEnable_keep_last_frame) = 0;
      
      virtual  int32_t setIPTVbackground() = 0;
			virtual  int32_t defaultbackground() = 0;


      virtual  ~IPTVMediaPlayer() { };
};

extern "C" IPTVMediaPlayer *createIPTVMediaPlayerInstance();

}

#endif

