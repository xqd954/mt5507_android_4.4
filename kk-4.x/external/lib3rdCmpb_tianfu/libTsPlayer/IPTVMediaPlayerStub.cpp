#ifndef _IPTVMEDIAPLAYER_STUB_H_
#include "IPTVMediaPlayerStub.h"
#endif

using namespace iptv;

int32_t  IPTVMediaPlayerStub::initialize(void *) {
    return 0;
}

int32_t  IPTVMediaPlayerStub::setReadBufferCallback(ReadBufferCallback readBuffer) {
    mReadBuffer = readBuffer;
    return 0;
}

int32_t  IPTVMediaPlayerStub::clearBuffer() {
    return 0;
}

int32_t  IPTVMediaPlayerStub::setOnErrorCallback(OnErrorCallback onError) {
    mOnErrorCallback = onError;
    return 0;
}

int32_t  IPTVMediaPlayerStub::play() {
    return 0;
}

int32_t  IPTVMediaPlayerStub::pause() {
    return 0;
}

int32_t  IPTVMediaPlayerStub::resume() {
    return 0;
}

int32_t  IPTVMediaPlayerStub::stop() {
    return 0;
}

int32_t  IPTVMediaPlayerStub::stopExt() {
    return 0;
}
int32_t  IPTVMediaPlayerStub::setAVSync(bool isSync) {
    isSync = isSync;
    return 0;
}

uint64_t IPTVMediaPlayerStub::getCurrentPosition() {
    return 0;
}

int32_t  IPTVMediaPlayerStub::setDisplayArea(uint32_t left, uint32_t top, uint32_t width, uint32_t height) {
    left = left;
    top = top;
    width = width;
    height = height;
    return 0;
}

int32_t  IPTVMediaPlayerStub::setVolume(uint32_t volume) {
    volume = volume ;
    return 0;
}

uint32_t  IPTVMediaPlayerStub::getVolume() {
    return 0;
}

bool  IPTVMediaPlayerStub::isMute() {
    return false;
}

int32_t  IPTVMediaPlayerStub::setMute(bool isMute) {
    isMute = isMute;
    return 0;
}
//add by lirong
int32_t  IPTVMediaPlayerStub::fastForward(int speed)
{
    speed  = speed;
return 0;
}
int32_t  IPTVMediaPlayerStub::fastRewind(int speed)
{
    speed = speed;
return 0;
}

int32_t  IPTVMediaPlayerStub::finalize() {
    return 0;
}

//////////////////////////////////////set audio balance
bool  IPTVMediaPlayerStub::setAudioBalance(uint32_t nAudioBalance)
{
        nAudioBalance = nAudioBalance;
        return false;
}
            
uint32_t IPTVMediaPlayerStub::getAudioBalance()
{
        return false;
}
        
///////////////////////////set lastframe /false  or true
        
void IPTVMediaPlayerStub::setlastframe(bool nEnable_keep_last_frame)
{
    nEnable_keep_last_frame = nEnable_keep_last_frame;
}

int32_t IPTVMediaPlayerStub::setIPTVbackground()
{
    return 0;
}
int32_t IPTVMediaPlayerStub::defaultbackground()
{
    return 0;
}
            


