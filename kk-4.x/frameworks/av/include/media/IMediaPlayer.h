/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_IMEDIAPLAYER_H
#define ANDROID_IMEDIAPLAYER_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/KeyedVector.h>
#include <system/audio.h>

// Fwd decl to make sure everyone agrees that the scope of struct sockaddr_in is
// global, and not in android::
struct sockaddr_in;



namespace android {

class Parcel;
class Surface;
class IStreamSource;
class IGraphicBufferProducer;

class IMediaPlayer: public IInterface
{
public:
    DECLARE_META_INTERFACE(MediaPlayer);

    virtual void            disconnect() = 0;

    virtual status_t        setDataSource(const char *url,
                                    const KeyedVector<String8, String8>* headers) = 0;
    virtual status_t        setDataSource(int fd, int64_t offset, int64_t length) = 0;
    virtual status_t        setDataSource(const sp<IStreamSource>& source) = 0;
    virtual status_t        setVideoSurfaceTexture(
                                    const sp<IGraphicBufferProducer>& bufferProducer) = 0;
    virtual status_t        prepareAsync() = 0;
    virtual status_t        start() = 0;
    virtual status_t        stop() = 0;
    virtual status_t        pause() = 0;
    virtual status_t        isPlaying(bool* state) = 0;
    virtual status_t        seekTo(int msec) = 0;
    virtual status_t        getCurrentPosition(int* msec) = 0;
    virtual status_t        getDuration(int* msec) = 0;
    virtual status_t        reset() = 0;
    virtual status_t        setAudioStreamType(audio_stream_type_t type) = 0;
    virtual status_t        setLooping(int loop) = 0;
    virtual status_t        setVolume(float leftVolume, float rightVolume) = 0;
    virtual status_t        setAuxEffectSendLevel(float level) = 0;
    virtual status_t        attachAuxEffect(int effectId) = 0;
    virtual status_t        setParameter(int key, const Parcel& request) = 0;
    virtual status_t        getParameter(int key, Parcel* reply) = 0;
    virtual status_t        setRetransmitEndpoint(const struct sockaddr_in* endpoint) = 0;
    virtual status_t        getRetransmitEndpoint(struct sockaddr_in* endpoint) = 0;
    virtual status_t        setNextPlayer(const sp<IMediaPlayer>& next) = 0;

    // Invoke a generic method on the player by using opaque parcels
    // for the request and reply.
    // @param request Parcel that must start with the media player
    // interface token.
    // @param[out] reply Parcel to hold the reply data. Cannot be null.
    // @return OK if the invocation was made successfully.
    virtual status_t        invoke(const Parcel& request, Parcel *reply) = 0;

    // Set a new metadata filter.
    // @param filter A set of allow and drop rules serialized in a Parcel.
    // @return OK if the invocation was made successfully.
    virtual status_t        setMetadataFilter(const Parcel& filter) = 0;

    // Retrieve a set of metadata.
    // @param update_only Include only the metadata that have changed
    //                    since the last invocation of getMetadata.
    //                    The set is built using the unfiltered
    //                    notifications the native player sent to the
    //                    MediaPlayerService during that period of
    //                    time. If false, all the metadatas are considered.
    // @param apply_filter If true, once the metadata set has been built based
    //                     on the value update_only, the current filter is
    //                     applied.
    // @param[out] metadata On exit contains a set (possibly empty) of metadata.
    //                      Valid only if the call returned OK.
    // @return OK if the invocation was made successfully.
    virtual status_t        getMetadata(bool update_only,
                                        bool apply_filter,
                                        Parcel *metadata) = 0;
    
    /*MTK Begin*/    
    virtual status_t        setSurfaceType(int i4Type) = 0;
    virtual status_t        setPlayerType(int i4PlayerType) = 0;
    virtual status_t        setVideoRect(int i4left, int i4top, int i4right, int bottom) = 0;
    virtual status_t        setSvctxPath(const char *pu1_svctx_path) = 0;
    virtual status_t        setSpeed(int speed)=0;
    virtual bool        getSpeed(int &speed)=0;
    virtual bool        getSubtitleTrackCount(int & count)=0;
    virtual bool        getSubtitleTrackInfo(int trackIndex,
                          int &SubtitleType,
                          int &SubtitleCodeType,
                          String8 &SubtitleLanguage,
                          String8 &SubtitleTitle)=0;
    virtual bool        getAudioTrackCount(int & count)=0;
    virtual bool getAudioTrackInfo(bool IsAudio,
                           int trackIndex,
                           int & AudioTrackNum,
                           int & BitRate,
                           int & TotalPlayTime,
                           int & CurrentPlayTime,
                           int & SampleRate,
                           int & CodecID,
                           int & ChannelNumber,
                           String8 &Year,
                           String8 &Title,
                           String8 &Album,
                           String8 &Artist,
                           String8 &Genre,
                           String8 &Director,
                           String8 &Copyright,
                           String8 &Language)=0;
    virtual bool        setSubtitleAttr(int eType, void * prAttr)=0;
    virtual bool        flushSubtitleAttr()=0;
    virtual bool        setAudioTrack(int track)=0;
    virtual bool        setSubtitleTrack(int track)=0;
    virtual bool        getBufferPercent(int &OutPercent)=0;
    virtual bool        onSubtitleTrack()=0;
    virtual bool        offSubtitleTrack()=0;
    /*  Set the url of subtitle before play */
    virtual bool        setSubtitleDataSource(const char *url)=0;
    /*  Set the url of subtitle after play */
    virtual bool        setSubtitleDataSourceEx(const char *urlSub, const char *urlIdx)=0;
    virtual bool        ifMP3Media(bool & fgIsMP3Media)=0;
    virtual bool        seekToPosition(unsigned long long position)=0;
    virtual bool        setPlayerRole(int eType)=0;
    virtual bool        setABRepeat(int type)=0;
    virtual bool        setTS(int index)=0;
    virtual bool        canDoTrick(int speed)=0;
    virtual bool        canDoSeek(int speed)=0;
    virtual bool        step(int amount)=0;
    virtual status_t getCurrentBytePosition(int *mposition)=0;
    virtual status_t getDownloadRate(double *rate)=0;
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    virtual status_t assignSWVideoDecoder(bool useSWDec)=0;
    virtual status_t assignSWAudioDecoder(bool useSWDec)=0;
    virtual status_t setRMPriority(int i4value)=0;
#endif
    /*MTK END*/    
};

// ----------------------------------------------------------------------------

class BnMediaPlayer: public BnInterface<IMediaPlayer>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};
//MTK Begin
typedef enum
{
    SUBTL_ATTR_TYPE_TIME_OFFSET = 0,
    SUBTL_ATTR_TYPE_SHOW_HIDE,
    SUBTL_ATTR_TYPE_FONT_INFO,
    SUBTL_ATTR_TYPE_BG_COLOR,
    SUBTL_ATTR_TYPE_TEXT_COLOR,
	SUBTL_ATTR_TYPE_EDGE_COLOR,
    SUBTL_ATTR_TYPE_DISPLAY_RECT,
    SUBTL_ATTR_TYPE_DISPLAY_OFFSET
}E_Subtitle_Attr_Type;

#ifndef MAKE_BIT_MASK_32
#define MAKE_BIT_MASK_32(_val) (((uint32_t) 1) << (_val))
//Subtitle attribute type
#define FLAG_SUBTITLE_TIME_OFFSET       MAKE_BIT_MASK_32(0)
#define FLAG_SUBTITLE_SHOW_HIDE         MAKE_BIT_MASK_32(1)
#define FLAG_SUBTITLE_FONT_INFO         MAKE_BIT_MASK_32(2)
#define FLAG_SUBTITLE_BG_COLOR          MAKE_BIT_MASK_32(3)
#define FLAG_SUBTITLE_TEXT_COLOR        MAKE_BIT_MASK_32(4)
#define FLAG_SUBTITLE_EDGE_COLOR        MAKE_BIT_MASK_32(5)
#define FLAG_SUBTITLE_DISPLAY_RECT      MAKE_BIT_MASK_32(6)
#define FLAG_SUBTITLE_DISPLAY_OFFSET    MAKE_BIT_MASK_32(7)
#endif
class Subtitle_TimeOffset
{
    public:
        Subtitle_TimeOffset()
        {
            mTimeOffsetType = 0;
            mOffsetValue = 0;
        }
        int mTimeOffsetType;
        long mOffsetValue;
};
class Subtitle_FontInfo
{
    public:
        Subtitle_FontInfo()
        {
            mFontSizeType = 0;
            mCustomSize = 0;
        }
        int mFontSizeType;
        char mCustomSize;
};

class Subtitle_Color
{
    public:
        Subtitle_Color()
        {
            mA = 0;
            mR = 0;
            mG = 0;
            mB = 0;
        }
        char mA;
        char mR;
        char mG;
        char mB;
};

class Subtitle_DisplayRect
{
    public:
        Subtitle_DisplayRect()
        {
            mX = 0;
            mY = 0;
            mW = 0;
            mH = 0;
        }
        int mX;
        int mY;
        int mW;
        int mH;
};

class Subtitle_DisplayOffset
{
    public:
        Subtitle_DisplayOffset()
        {
            mX = 0;
            mY = 0;
        }
        int mX;
        int mY;
};

#define OUTPUT_VIDEO_Default 0x000  
#define OUTPUT_VIDEO_MAIN 0x100  
#define OUTPUT_VIDEO_SUB 0x200  
#define OUTPUT_GRAPHIC 0x400 

//customer Part:API name
static const char* const k_assignSWVideoDecoder              = "X-tv-assign-software-video-decoder";
static const char* const k_assignSWAudioDecoder              = "X-tv-assign-software-audio-decoder";
static const char* const k_outputPath              = "X-tv-output-path";
static const char* const k_RMPriority              = "X-tv-resource-manager-client-priority";
static const char* const k_PlayerType              = "X-tv-PlayerType";
static const char* const k_Retriever_PlayerType    = "X-tv-Retriever-Player";
//MTK END
}; // namespace android

#endif // ANDROID_IMEDIAPLAYER_H
