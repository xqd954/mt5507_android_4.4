/*
**
** Copyright 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "IMediaPlayer"
#include <utils/Log.h>

#include <arpa/inet.h>
#include <stdint.h>
#include <sys/types.h>

#include <binder/Parcel.h>

#include <media/IMediaPlayer.h>
#include <media/IStreamSource.h>

#include <gui/IGraphicBufferProducer.h>
#include <utils/String8.h>

namespace android {

enum {
    DISCONNECT = IBinder::FIRST_CALL_TRANSACTION,
    SET_DATA_SOURCE_URL,
    SET_DATA_SOURCE_FD,
    SET_DATA_SOURCE_STREAM,
    PREPARE_ASYNC,
    START,
    STOP,
    IS_PLAYING,
    PAUSE,
    SEEK_TO,
    GET_CURRENT_POSITION,
    GET_DURATION,
    RESET,
    SET_AUDIO_STREAM_TYPE,
    SET_LOOPING,
    SET_VOLUME,
    INVOKE,
    SET_METADATA_FILTER,
    GET_METADATA,
    SET_AUX_EFFECT_SEND_LEVEL,
    ATTACH_AUX_EFFECT,
    SET_VIDEO_SURFACETEXTURE,
    SET_PARAMETER,
    GET_PARAMETER,
    SET_RETRANSMIT_ENDPOINT,
    GET_RETRANSMIT_ENDPOINT,
    SET_NEXT_PLAYER,
    /*MTK BEGINE*/
    SET_SURFACE_TYPE,
    SET_PLAYER_TYPE,
    SET_DISP_RECT,
    SET_SVCTX_PATH,
    SET_SPEED,
    GET_SPEED,
    SET_SUBTITLE_ATTRIBUTE,
    FLUSH_SUBTITLE,
    SET_AUDIO_TRACK,
    GET_BUFFER_PERCENT,
    ON_SUBTITLE,
    OFF_SUBTITLE,
    SET_SUBTITLE_DATA_SRC,
    SET_SUBTITLE_DATA_SRC_EX,
    SET_PCM_INFO,
    GET_IS_MP3_INFO,
    SET_SEEK_POSITION,
    SET_PLAYRT_ROLE,
    SET_AB_REPEAT,
    SET_TS,
    CAN_DO_TRICK,
    CAN_DO_SEEK,
    STEP,
    GET_AUD_TRAC_COUNT,
    GET_AUD_TRAC_INFO,
    GET_SUB_TITLE_TRAC_COUNT,
    GET_SUB_TITLE_TRAC_INFO,
    SET_SUB_TITLE_TRACK,
    SET_SW_VIDEO_DECODER,
    SET_SW_AUDIO_DECODER,
    GET_BYTE_POSITION,
    GET_DOWNLOAD_RATE,
    SET_RM_PRIORITY,
    /*MTK END*/
};

class BpMediaPlayer: public BpInterface<IMediaPlayer>
{
public:
    BpMediaPlayer(const sp<IBinder>& impl)
        : BpInterface<IMediaPlayer>(impl)
    {
    }

    // disconnect from media player service
    void disconnect()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(DISCONNECT, data, &reply);
    }

    status_t setDataSource(const char* url,
            const KeyedVector<String8, String8>* headers)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeCString(url);
        if (headers == NULL) {
            data.writeInt32(0);
        } else {
            // serialize the headers
            data.writeInt32(headers->size());
            for (size_t i = 0; i < headers->size(); ++i) {
                data.writeString8(headers->keyAt(i));
                data.writeString8(headers->valueAt(i));
            }
        }
        remote()->transact(SET_DATA_SOURCE_URL, data, &reply);
        return reply.readInt32();
    }

    status_t setDataSource(int fd, int64_t offset, int64_t length) {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeFileDescriptor(fd);
        data.writeInt64(offset);
        data.writeInt64(length);
        remote()->transact(SET_DATA_SOURCE_FD, data, &reply);
        return reply.readInt32();
    }

    status_t setDataSource(const sp<IStreamSource> &source) {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeStrongBinder(source->asBinder());
        remote()->transact(SET_DATA_SOURCE_STREAM, data, &reply);
        return reply.readInt32();
    }

    // pass the buffered IGraphicBufferProducer to the media player service
    status_t setVideoSurfaceTexture(const sp<IGraphicBufferProducer>& bufferProducer)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        sp<IBinder> b(bufferProducer->asBinder());
        data.writeStrongBinder(b);
        remote()->transact(SET_VIDEO_SURFACETEXTURE, data, &reply);
        return reply.readInt32();
    }

    status_t prepareAsync()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(PREPARE_ASYNC, data, &reply);
        return reply.readInt32();
    }

    status_t start()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(START, data, &reply);
        return reply.readInt32();
    }

    status_t stop()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(STOP, data, &reply);
        return reply.readInt32();
    }

    status_t isPlaying(bool* state)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(IS_PLAYING, data, &reply);
        *state = reply.readInt32();
        return reply.readInt32();
    }

    status_t pause()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(PAUSE, data, &reply);
        return reply.readInt32();
    }

    status_t seekTo(int msec)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeInt32(msec);
        remote()->transact(SEEK_TO, data, &reply);
        return reply.readInt32();
    }

    status_t getCurrentPosition(int* msec)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(GET_CURRENT_POSITION, data, &reply);
        *msec = reply.readInt32();
        return reply.readInt32();
    }

    status_t getDuration(int* msec)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(GET_DURATION, data, &reply);
        *msec = reply.readInt32();
        return reply.readInt32();
    }

    status_t reset()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        remote()->transact(RESET, data, &reply);
        return reply.readInt32();
    }

    status_t setAudioStreamType(audio_stream_type_t stream)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeInt32((int32_t) stream);
        remote()->transact(SET_AUDIO_STREAM_TYPE, data, &reply);
        return reply.readInt32();
    }

    status_t setLooping(int loop)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeInt32(loop);
        remote()->transact(SET_LOOPING, data, &reply);
        return reply.readInt32();
    }

    status_t setVolume(float leftVolume, float rightVolume)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeFloat(leftVolume);
        data.writeFloat(rightVolume);
        remote()->transact(SET_VOLUME, data, &reply);
        return reply.readInt32();
    }

    status_t invoke(const Parcel& request, Parcel *reply)
    {
        // Avoid doing any extra copy. The interface descriptor should
        // have been set by MediaPlayer.java.
        return remote()->transact(INVOKE, request, reply);
    }

    status_t setMetadataFilter(const Parcel& request)
    {
        Parcel reply;
        // Avoid doing any extra copy of the request. The interface
        // descriptor should have been set by MediaPlayer.java.
        remote()->transact(SET_METADATA_FILTER, request, &reply);
        return reply.readInt32();
    }

    status_t getMetadata(bool update_only, bool apply_filter, Parcel *reply)
    {
        Parcel request;
        request.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        // TODO: Burning 2 ints for 2 boolean. Should probably use flags in an int here.
        request.writeInt32(update_only);
        request.writeInt32(apply_filter);
        remote()->transact(GET_METADATA, request, reply);
        return reply->readInt32();
    }

    status_t setAuxEffectSendLevel(float level)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeFloat(level);
        remote()->transact(SET_AUX_EFFECT_SEND_LEVEL, data, &reply);
        return reply.readInt32();
    }

    status_t attachAuxEffect(int effectId)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeInt32(effectId);
        remote()->transact(ATTACH_AUX_EFFECT, data, &reply);
        return reply.readInt32();
    }

    status_t setParameter(int key, const Parcel& request)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeInt32(key);
        if (request.dataSize() > 0) {
            data.appendFrom(const_cast<Parcel *>(&request), 0, request.dataSize());
        }
        remote()->transact(SET_PARAMETER, data, &reply);
        return reply.readInt32();
    }

    status_t getParameter(int key, Parcel *reply)
    {
        Parcel data;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        data.writeInt32(key);
        return remote()->transact(GET_PARAMETER, data, reply);
    }

    status_t setRetransmitEndpoint(const struct sockaddr_in* endpoint)
    {
        Parcel data, reply;
        status_t err;

        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        if (NULL != endpoint) {
            data.writeInt32(sizeof(*endpoint));
            data.write(endpoint, sizeof(*endpoint));
        } else {
            data.writeInt32(0);
        }

        err = remote()->transact(SET_RETRANSMIT_ENDPOINT, data, &reply);
        if (OK != err) {
            return err;
        }
        return reply.readInt32();
    }

    status_t setNextPlayer(const sp<IMediaPlayer>& player) {
        Parcel data, reply;
        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        sp<IBinder> b(player->asBinder());
        data.writeStrongBinder(b);
        remote()->transact(SET_NEXT_PLAYER, data, &reply);
        return reply.readInt32();
    }

    status_t getRetransmitEndpoint(struct sockaddr_in* endpoint)
    {
        Parcel data, reply;
        status_t err;

        data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
        err = remote()->transact(GET_RETRANSMIT_ENDPOINT, data, &reply);

        if ((OK != err) || (OK != (err = reply.readInt32()))) {
            return err;
        }

        data.read(endpoint, sizeof(*endpoint));

        return err;
    }

    
 /*MTK Begin*/    
#if 1
status_t setSurfaceType(int i4Type)
{
     //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
     //ALOGD("i4Type %d", i4Type);
     Parcel data, reply;
     data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
     data.writeInt32(i4Type);
     remote()->transact(SET_SURFACE_TYPE, data, &reply);
     return reply.readInt32();
}
#endif
status_t setPlayerType(int i4PlayerType)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("setPlayerType(%d)", i4PlayerType);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(i4PlayerType);
    remote()->transact(SET_PLAYER_TYPE, data, &reply);
    return reply.readInt32();
}
status_t setVideoRect(int i4left, int i4top, int i4right, int bottom)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("setVideoRect(%d,%d, %d, %d)", i4left, i4top, i4right, bottom);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(i4left);
    data.writeInt32(i4top);
    data.writeInt32(i4right);
    data.writeInt32(bottom);
    remote()->transact(SET_DISP_RECT, data, &reply);
    return reply.readInt32();
}

status_t setSvctxPath(const char *pu1_svctx_path)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("pu1_svctx_path %s",pu1_svctx_path);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeCString(pu1_svctx_path);
    remote()->transact(SET_SVCTX_PATH, data, &reply);
    
    return reply.readInt32();
}
bool setSubtitleDataSource(const char *url)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("url %s",url);

    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeCString(url);
    remote()->transact(SET_SUBTITLE_DATA_SRC, data, &reply);
    
    return reply.readInt32();
}

bool setSubtitleDataSourceEx(const char *urlSub, const char *urlIdx)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("url %s",urlSub);
    //ALOGD("urlIdx %s",urlIdx);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeCString(urlSub);
    data.writeCString(urlIdx);
    
    remote()->transact(SET_SUBTITLE_DATA_SRC_EX, data, &reply);
    return reply.readInt32();
}

bool onSubtitleTrack()
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(ON_SUBTITLE, data, &reply);
    
    return reply.readInt32();
}

bool offSubtitleTrack()
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(OFF_SUBTITLE, data, &reply);
    
    return reply.readInt32();
}

status_t setSpeed(int speed)
{
   //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
   //ALOGD("speed %d",speed);
   Parcel data, reply;
   data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
   data.writeInt32(speed);
   remote()->transact(SET_SPEED, data, &reply);
   
   return reply.readInt32();
}
 
bool setTS(int index)
{
   //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
   //ALOGD("index %d",index);
   Parcel data, reply;
   data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
   data.writeInt32(index);
   remote()->transact(SET_TS, data, &reply);
      
   return reply.readInt32();
}
    
bool canDoTrick(int speed)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("speed %d",speed);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(speed);
    remote()->transact(CAN_DO_TRICK, data, &reply);
    
    return reply.readInt32();
}

bool canDoSeek(int speed)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("speed %d",speed);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(speed);
    remote()->transact(CAN_DO_SEEK, data, &reply);
    
    return reply.readInt32();
}

bool step(int amount)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("amount %d",amount);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(amount);
    remote()->transact(STEP, data, &reply);
    
    return reply.readInt32();
}
bool setPlayerRole(int eType)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("setPlayerRole %d",eType);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(eType);
    remote()->transact(SET_PLAYRT_ROLE, data, &reply);
    
    return reply.readInt32();
}

bool setABRepeat(int type)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("setABRepeat %d",type);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(type);
    remote()->transact(SET_AB_REPEAT, data, &reply);
    
    return reply.readInt32();
}
bool seekToPosition(unsigned long long position)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("position %lld",position);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt64(position);
    remote()->transact(SET_SEEK_POSITION, data, &reply);
    
    return reply.readInt32();
}
bool getSpeed(int &speed)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_SPEED, data, &reply);
    speed = reply.readInt32();
    
    ALOGD("speed %d",speed);
    
    return reply.readInt32();
}

bool getSubtitleTrackCount(int & count)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_SUB_TITLE_TRAC_COUNT, data, &reply);
    count = reply.readInt32();
    
    ALOGD("count %d",count);
    
    return reply.readInt32();
}

bool getSubtitleTrackInfo(int trackIndex,
                      int &SubtitleType,
                      int &SubtitleCodeType,
                      String8 &SubtitleLanguage,
                      String8 &SubtitleTitle)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(trackIndex);
    
    remote()->transact(GET_SUB_TITLE_TRAC_INFO, data, &reply);
    SubtitleType = reply.readInt32();
    SubtitleCodeType = reply.readInt32();
    
    
    //String begin
    SubtitleLanguage = reply.readString8();
    ALOGD("SubtitleLanguage %s",SubtitleLanguage.string());
    SubtitleTitle = reply.readString8();
    ALOGD("SubtitleTitle %s",SubtitleTitle.string());
    
    return reply.readInt32();
}
bool getAudioTrackCount(int & count)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_AUD_TRAC_COUNT, data, &reply);
    count = reply.readInt32();
    
    //ALOGD("count %d",count);
    
    return reply.readInt32();
}

bool getAudioTrackInfo(bool IsAudio,
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
                       String8 &Language)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(IsAudio);
    data.writeInt32(trackIndex);
    
    remote()->transact(GET_AUD_TRAC_INFO, data, &reply);
    AudioTrackNum = reply.readInt32();
    BitRate = reply.readInt32();
    TotalPlayTime = reply.readInt32();
    
    CurrentPlayTime = reply.readInt32();
    SampleRate = reply.readInt32();
    CodecID = reply.readInt32();
    
    ChannelNumber = reply.readInt32();
    //String begin
    Year = reply.readString8();
    ALOGD("Year %s",Year.string());
    Title = reply.readString8();
    ALOGD("Title %s",Title.string());
    Album = reply.readString8();
    Artist = reply.readString8();
    Genre = reply.readString8();
    Director = reply.readString8();
    Copyright = reply.readString8();
    Language = reply.readString8();
    
    
    return reply.readInt32();
}

bool ifMP3Media(bool & fgIsMP3Media)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_IS_MP3_INFO, data, &reply);
    fgIsMP3Media = reply.readInt32();
    
    ALOGD("fgIsMP3Media %d",fgIsMP3Media);
    
    return reply.readInt32();
}

bool getBufferPercent(int &OutPercent)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_BUFFER_PERCENT, data, &reply);
    OutPercent = reply.readInt32();
    
    //ALOGD("OutPercent %d",OutPercent);
    
    return reply.readInt32();
}
bool flushSubtitleAttr(void)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(FLUSH_SUBTITLE, data, &reply);
    
    //ALOGD("flushSubtitleAttr");
    
    return reply.readInt32();
}

bool setSubtitleAttr(int eType, void * prAttr)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("eType %d",eType);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(eType);

    if(SUBTL_ATTR_TYPE_TIME_OFFSET==(E_Subtitle_Attr_Type)eType)
    {
        Subtitle_TimeOffset * prOffset = (Subtitle_TimeOffset *)prAttr;
        
        Subtitle_TimeOffset rTimeOffset;
        rTimeOffset.mTimeOffsetType = prOffset->mTimeOffsetType;
        rTimeOffset.mOffsetValue = prOffset->mOffsetValue;
        
        ALOGD("setSubtitleAttr %s %d mTimeOffsetType %d",__FILE__,__LINE__,(int)rTimeOffset.mTimeOffsetType);
        ALOGD("setSubtitleAttr %s %d mOffsetValue %d",__FILE__,__LINE__,(int)rTimeOffset.mOffsetValue);
        
        data.write(&rTimeOffset,sizeof(Subtitle_TimeOffset));
    }
    else if(FLAG_SUBTITLE_FONT_INFO==(E_Subtitle_Attr_Type)eType)
    {
        Subtitle_FontInfo * prInfo = (Subtitle_FontInfo *)prAttr;
        
        Subtitle_FontInfo rInfo;
        rInfo.mFontSizeType = prInfo->mFontSizeType;
        rInfo.mCustomSize = prInfo->mCustomSize;
        
        ALOGD("setSubtitleAttr %s %d mFontSizeType %d",__FILE__,__LINE__,(int)rInfo.mFontSizeType);
        ALOGD("setSubtitleAttr %s %d mCustomSize %d",__FILE__,__LINE__,(int)rInfo.mCustomSize);
        
        data.write(&rInfo,sizeof(Subtitle_FontInfo));
    }
    else if(FLAG_SUBTITLE_BG_COLOR==(E_Subtitle_Attr_Type)eType)
    {
        Subtitle_Color * prInfo = (Subtitle_Color *)prAttr;
        
        Subtitle_Color rInfo;
        rInfo.mA = prInfo->mA;
        rInfo.mR = prInfo->mR;
        rInfo.mG = prInfo->mG;
        rInfo.mB = prInfo->mB;
        
        ALOGD("setSubtitleAttr %s %d mA %d",__FILE__,__LINE__,(int)rInfo.mA);
        ALOGD("setSubtitleAttr %s %d mB %d",__FILE__,__LINE__,(int)rInfo.mB);
        
        data.write(&rInfo,sizeof(Subtitle_Color));
    }
    else if(FLAG_SUBTITLE_TEXT_COLOR==(E_Subtitle_Attr_Type)eType)
    {
        Subtitle_Color * prInfo = (Subtitle_Color *)prAttr;
        
        Subtitle_Color rInfo;
        rInfo.mA = prInfo->mA;
        rInfo.mR = prInfo->mR;
        rInfo.mG = prInfo->mG;
        rInfo.mB = prInfo->mB;
        
        ALOGD("setSubtitleAttr %s %d mA %d",__FILE__,__LINE__,(int)rInfo.mA);
        ALOGD("setSubtitleAttr %s %d mB %d",__FILE__,__LINE__,(int)rInfo.mB);
        
        data.write(&rInfo,sizeof(Subtitle_Color));
    }
    else if(FLAG_SUBTITLE_EDGE_COLOR==(E_Subtitle_Attr_Type)eType)
    {
        Subtitle_Color * prInfo = (Subtitle_Color *)prAttr;
        
        Subtitle_Color rInfo;
        rInfo.mA = prInfo->mA;
        rInfo.mR = prInfo->mR;
        rInfo.mG = prInfo->mG;
        rInfo.mB = prInfo->mB;
        
        ALOGD("setSubtitleAttr %s %d mA %d",__FILE__,__LINE__,(int)rInfo.mA);
        ALOGD("setSubtitleAttr %s %d mB %d",__FILE__,__LINE__,(int)rInfo.mB);
        
        data.write(&rInfo,sizeof(Subtitle_Color));
    }
    else if(FLAG_SUBTITLE_DISPLAY_OFFSET==(E_Subtitle_Attr_Type)eType)
    {
        Subtitle_DisplayOffset * prInfo = (Subtitle_DisplayOffset *)prAttr;
        
        Subtitle_DisplayOffset rInfo;
        rInfo.mX = prInfo->mX;
        rInfo.mY = prInfo->mY;
         
        ALOGD("setSubtitleAttr %s %d mX %d",__FILE__,__LINE__,(int)rInfo.mX);
        ALOGD("setSubtitleAttr %s %d mY %d",__FILE__,__LINE__,(int)rInfo.mY);
        
        data.write(&rInfo,sizeof(Subtitle_Color));
    }
    else if(SUBTL_ATTR_TYPE_SHOW_HIDE==(E_Subtitle_Attr_Type)eType)
    {
        
    }
    
    remote()->transact(SET_SUBTITLE_ATTRIBUTE, data, &reply);
    
    return reply.readInt32();
}
bool setAudioTrack(int track)
{
    //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    //ALOGD("track %d",track);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(track);
    remote()->transact(SET_AUDIO_TRACK, data, &reply);
    
    return reply.readInt32();
}

bool setSubtitleTrack(int track)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    ALOGD("track %d",track);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(track);
    remote()->transact(SET_SUB_TITLE_TRACK, data, &reply);
    
    return reply.readInt32();
}

status_t getCurrentBytePosition(int *mposition)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_BYTE_POSITION, data, &reply);
    *mposition = reply.readInt32();
    
    ALOGD("*mposition %d",*mposition);
    
    return reply.readInt32();
}

status_t getDownloadRate(double *rate)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    remote()->transact(GET_DOWNLOAD_RATE, data, &reply);
    *rate = reply.readDouble();
    
    ALOGD("*rate %lf",*rate);
    
    return reply.readInt32();
}
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
 status_t assignSWVideoDecoder(bool useSWDec)
 {
     ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
     ALOGD("useSWDec %d",useSWDec);
     Parcel data, reply;
     data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
     data.writeInt32(useSWDec);
     remote()->transact(SET_SW_VIDEO_DECODER, data, &reply);
     
     return reply.readInt32();
 }

status_t assignSWAudioDecoder(bool useSWDec)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    ALOGD("useSWDec %d",useSWDec);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(useSWDec);
    remote()->transact(SET_SW_AUDIO_DECODER, data, &reply);
    
    return reply.readInt32();
}

status_t setRMPriority(int i4value)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    ALOGD("i4value %d",i4value);
    Parcel data, reply;
    data.writeInterfaceToken(IMediaPlayer::getInterfaceDescriptor());
    data.writeInt32(i4value);
    remote()->transact(SET_RM_PRIORITY, data, &reply);
    
    return reply.readInt32();
}
#endif 
 /*MTK END*/    
};

IMPLEMENT_META_INTERFACE(MediaPlayer, "android.media.IMediaPlayer");

// ----------------------------------------------------------------------

status_t BnMediaPlayer::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case DISCONNECT: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            disconnect();
            return NO_ERROR;
        } break;
        case SET_DATA_SOURCE_URL: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            const char* url = data.readCString();
            KeyedVector<String8, String8> headers;
            int32_t numHeaders = data.readInt32();
            for (int i = 0; i < numHeaders; ++i) {
                String8 key = data.readString8();
                String8 value = data.readString8();
                headers.add(key, value);
            }
            reply->writeInt32(setDataSource(url, numHeaders > 0 ? &headers : NULL));
            return NO_ERROR;
        } break;
        case SET_DATA_SOURCE_FD: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int fd = data.readFileDescriptor();
            int64_t offset = data.readInt64();
            int64_t length = data.readInt64();
            reply->writeInt32(setDataSource(fd, offset, length));
            return NO_ERROR;
        }
        case SET_DATA_SOURCE_STREAM: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            sp<IStreamSource> source =
                interface_cast<IStreamSource>(data.readStrongBinder());
            reply->writeInt32(setDataSource(source));
            return NO_ERROR;
        }
        case SET_VIDEO_SURFACETEXTURE: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            sp<IGraphicBufferProducer> bufferProducer =
                    interface_cast<IGraphicBufferProducer>(data.readStrongBinder());
            reply->writeInt32(setVideoSurfaceTexture(bufferProducer));
            return NO_ERROR;
        } break;
        case PREPARE_ASYNC: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(prepareAsync());
            return NO_ERROR;
        } break;
        case START: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(start());
            return NO_ERROR;
        } break;
        case STOP: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(stop());
            return NO_ERROR;
        } break;
        case IS_PLAYING: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            bool state;
            status_t ret = isPlaying(&state);
            reply->writeInt32(state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case PAUSE: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(pause());
            return NO_ERROR;
        } break;
        case SEEK_TO: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(seekTo(data.readInt32()));
            return NO_ERROR;
        } break;
        case GET_CURRENT_POSITION: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int msec;
            status_t ret = getCurrentPosition(&msec);
            reply->writeInt32(msec);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case GET_DURATION: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int msec;
            status_t ret = getDuration(&msec);
            reply->writeInt32(msec);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case RESET: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(reset());
            return NO_ERROR;
        } break;
        case SET_AUDIO_STREAM_TYPE: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setAudioStreamType((audio_stream_type_t) data.readInt32()));
            return NO_ERROR;
        } break;
        case SET_LOOPING: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setLooping(data.readInt32()));
            return NO_ERROR;
        } break;
        case SET_VOLUME: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            float leftVolume = data.readFloat();
            float rightVolume = data.readFloat();
            reply->writeInt32(setVolume(leftVolume, rightVolume));
            return NO_ERROR;
        } break;
        case INVOKE: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            status_t result = invoke(data, reply);
            return result;
        } break;
        case SET_METADATA_FILTER: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setMetadataFilter(data));
            return NO_ERROR;
        } break;
        case GET_METADATA: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            bool update_only = static_cast<bool>(data.readInt32());
            bool apply_filter = static_cast<bool>(data.readInt32());
            const status_t retcode = getMetadata(update_only, apply_filter, reply);
            reply->setDataPosition(0);
            reply->writeInt32(retcode);
            reply->setDataPosition(0);
            return NO_ERROR;
        } break;
        case SET_AUX_EFFECT_SEND_LEVEL: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setAuxEffectSendLevel(data.readFloat()));
            return NO_ERROR;
        } break;
        case ATTACH_AUX_EFFECT: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(attachAuxEffect(data.readInt32()));
            return NO_ERROR;
        } break;
        case SET_PARAMETER: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int key = data.readInt32();

            Parcel request;
            if (data.dataAvail() > 0) {
                request.appendFrom(
                        const_cast<Parcel *>(&data), data.dataPosition(), data.dataAvail());
            }
            request.setDataPosition(0);
            reply->writeInt32(setParameter(key, request));
            return NO_ERROR;
        } break;
        case GET_PARAMETER: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            return getParameter(data.readInt32(), reply);
        } break;
        case SET_RETRANSMIT_ENDPOINT: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);

            struct sockaddr_in endpoint;
            int amt = data.readInt32();
            if (amt == sizeof(endpoint)) {
                data.read(&endpoint, sizeof(struct sockaddr_in));
                reply->writeInt32(setRetransmitEndpoint(&endpoint));
            } else {
                reply->writeInt32(setRetransmitEndpoint(NULL));
            }

            return NO_ERROR;
        } break;
        case GET_RETRANSMIT_ENDPOINT: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);

            struct sockaddr_in endpoint;
            status_t res = getRetransmitEndpoint(&endpoint);

            reply->writeInt32(res);
            reply->write(&endpoint, sizeof(endpoint));

            return NO_ERROR;
        } break;
        case SET_NEXT_PLAYER: {
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setNextPlayer(interface_cast<IMediaPlayer>(data.readStrongBinder())));

            return NO_ERROR;
        } break;
        
        /*MTK BEGIN*/
#if 1
        case SET_SURFACE_TYPE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SURFACE_TYPE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setSurfaceType(data.readInt32()));
            return NO_ERROR;
        } break;
#endif
        case SET_PLAYER_TYPE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_PLAYER_TYPE)");

            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setPlayerType(data.readInt32()));
            return NO_ERROR;
        } break;
        
        case SET_DISP_RECT: {
             //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
             //ALOGD("case SET_DISP_RECT)");

            CHECK_INTERFACE(IMediaPlayer, data, reply);
            reply->writeInt32(setVideoRect(data.readInt32(), data.readInt32(),
               data.readInt32(), data.readInt32()));
            return NO_ERROR;
        } break;
        
        case SET_SVCTX_PATH: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SVCTX_PATH)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            const char* pu1_svctx_path = data.readCString();
            ALOGD("setSvctxPath %s %d %s",__FILE__,__LINE__,pu1_svctx_path);
            reply->writeInt32(setSvctxPath(pu1_svctx_path));
            return NO_ERROR;
        } break;
        
        case SET_SUBTITLE_DATA_SRC: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SUBTITLE_DATA_SRC)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            const char* pu1_URL = data.readCString();
            ALOGD("pu1_URL %s %d %s",__FILE__,__LINE__,pu1_URL);
            reply->writeInt32(setSubtitleDataSource(pu1_URL));
            return NO_ERROR;
        } break;
        
        case SET_SUBTITLE_DATA_SRC_EX: {
             //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SUBTITLE_DATA_SRC_EX)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            const char* urlSub = data.readCString();
            const char* urlIdx = data.readCString();
            ALOGD("urlSub %s %d %s",__FILE__,__LINE__,urlSub);
            ALOGD("urlIdx %s %d %s",__FILE__,__LINE__,urlIdx);
            reply->writeInt32(setSubtitleDataSourceEx(urlSub,urlIdx));
            return NO_ERROR;
        } break;
        
        case SET_SPEED: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SPEED)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("SET_SPEED %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(setSpeed(info));
            return NO_ERROR;
        } break;
        
        case SET_TS: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_TS)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("SET_TS %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(setTS(info));
            return NO_ERROR;
        } break;
        
        case CAN_DO_TRICK: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case CAN_DO_TRICK)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("CAN_DO_TRICK %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(canDoTrick(info));
            return NO_ERROR;
        } break;
        
        case CAN_DO_SEEK: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case CAN_DO_SEEK)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("CAN_DO_SEEK %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(canDoSeek(info));
            return NO_ERROR;
        } break;
        
        case STEP: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case STEP)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("STEP %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(step(info));
            return NO_ERROR;
        } break;
        
        case SET_PLAYRT_ROLE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_PLAYRT_ROLE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("setPlayerRole %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(setPlayerRole(info));
            return NO_ERROR;
        } break;
        
        case SET_AB_REPEAT: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_AB_REPEAT)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  info = data.readInt32();
            ALOGD("setABRepeat %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32(setABRepeat(info));
            return NO_ERROR;
        } break;
        
        case SET_SEEK_POSITION: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SEEK_POSITION)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            unsigned long long int  info = data.readInt64();
            ALOGD("seekToPosition %s %d %lld",__FILE__,__LINE__,info);
            reply->writeInt32(seekToPosition(info));
            return NO_ERROR;
        } break;
        case GET_SPEED: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_SPEED)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int msec;
            status_t ret = getSpeed(msec);
            ALOGD("getSpeed %s %d %d",__FILE__,__LINE__,msec);
            reply->writeInt32(msec);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case GET_SUB_TITLE_TRAC_COUNT: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_SUB_TITLE_TRAC_COUNT)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int tmp;
            status_t ret = getSubtitleTrackCount(tmp);
            ALOGD("getSubtitleTrackCount %s %d %d",__FILE__,__LINE__,tmp);
            reply->writeInt32(tmp);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case GET_SUB_TITLE_TRAC_INFO: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_SUB_TITLE_TRAC_INFO)");
        
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int SubtitleType,SubtitleCodeType;
            String8 SubtitleLanguage;
            String8 SubtitleTitle;
            
           int  trackIndex = data.readInt32();
           
            status_t ret = getSubtitleTrackInfo(trackIndex,
                                                SubtitleType,
                                                SubtitleCodeType,
                                                SubtitleLanguage,
                                                SubtitleTitle);
            
            ALOGD("SubtitleLanguage %s %d %s",__FILE__,__LINE__,SubtitleLanguage.string());
            ALOGD("SubtitleTitle %s %d %s",__FILE__,__LINE__,SubtitleTitle.string());
            reply->writeInt32(SubtitleType);
            reply->writeInt32(SubtitleCodeType);
            
            //String Begine
            reply->writeString8(SubtitleLanguage);
            reply->writeString8(SubtitleTitle);
            
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case GET_AUD_TRAC_COUNT: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_AUD_TRAC_COUNT)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int tmp;
            status_t ret = getAudioTrackCount(tmp);
            ALOGD("getAudioTrackCount %s %d %d",__FILE__,__LINE__,tmp);
            reply->writeInt32(tmp);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case GET_AUD_TRAC_INFO: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_AUD_TRAC_INFO)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int AudioTrackNum,BitRate,TotalPlayTime,CurrentPlayTime,
                SampleRate,CodecID,ChannelNumber;
            String8 Year;
            String8 Title;
            String8 Album;
            String8 Artist;
            String8 Genre;
            String8 Director;
            String8 Copyright;
            String8 Language;
            
           int  IsAudio = data.readInt32();
           int  trackIndex = data.readInt32();
           
            status_t ret = getAudioTrackInfo(IsAudio,
                                             trackIndex,
                                             AudioTrackNum,
                                             BitRate,
                                             TotalPlayTime,
                                             CurrentPlayTime,
                                             SampleRate,
                                             CodecID,
                                             ChannelNumber,
                                             Year,
                                             Title,
                                             Album,
                                             Artist,
                                             Genre,
                                             Director,
                                             Copyright,
                                             Language);
            
            ALOGD("Year %s %d %s",__FILE__,__LINE__,Year.string());
            ALOGD("Title %s %d %s",__FILE__,__LINE__,Title.string());
            reply->writeInt32(AudioTrackNum);
            reply->writeInt32(BitRate);
            reply->writeInt32(TotalPlayTime);
            
            reply->writeInt32(CurrentPlayTime);
            reply->writeInt32(SampleRate);
            reply->writeInt32(CodecID);
            reply->writeInt32(ChannelNumber);

            //String Begine
            reply->writeString8(Year);
            reply->writeString8(Title);
            
            reply->writeString8(Album);
            reply->writeString8(Artist);
            reply->writeString8(Genre);
            reply->writeString8(Director);
            
            reply->writeString8(Copyright);
            reply->writeString8(Language);
            
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case GET_IS_MP3_INFO: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_IS_MP3_INFO)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            bool info;
            status_t ret = ifMP3Media(info);
            ALOGD("getSpeed %s %d %d",__FILE__,__LINE__,info);
            reply->writeInt32((int)info);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case GET_BUFFER_PERCENT: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case GET_BUFFER_PERCENT)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int OutPercent;
            status_t ret = getBufferPercent(OutPercent);
            //ALOGD("getBufferPercent %s %d %d",__FILE__,__LINE__,OutPercent);
            reply->writeInt32(OutPercent);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case FLUSH_SUBTITLE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case FLUSH_SUBTITLE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            status_t ret = flushSubtitleAttr();
            ALOGD("flushSubtitleAttr %s %d ",__FILE__,__LINE__);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case ON_SUBTITLE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case ON_SUBTITLE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            status_t ret = onSubtitleTrack();
            ALOGD("flushSubtitleAttr %s %d ",__FILE__,__LINE__);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case OFF_SUBTITLE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case OFF_SUBTITLE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            status_t ret = offSubtitleTrack();
            ALOGD("flushSubtitleAttr %s %d ",__FILE__,__LINE__);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case SET_SUBTITLE_ATTRIBUTE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_SUBTITLE_ATTRIBUTE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  eType = data.readInt32();
             
             if(SUBTL_ATTR_TYPE_TIME_OFFSET==(E_Subtitle_Attr_Type)eType)
             {
                 Subtitle_TimeOffset rTimeOffset;
                 data.read(&rTimeOffset,sizeof(Subtitle_TimeOffset));
                 
                 ALOGD("setSubtitleAttr %s %d mTimeOffsetType %d",__FILE__,__LINE__,(int)rTimeOffset.mTimeOffsetType);
                 ALOGD("setSubtitleAttr %s %d mOffsetValue %d",__FILE__,__LINE__,(int)rTimeOffset.mOffsetValue);
                 
                 reply->writeInt32(setSubtitleAttr(eType,&rTimeOffset));
              }
             else if(FLAG_SUBTITLE_FONT_INFO==(E_Subtitle_Attr_Type)eType)
             {
                 Subtitle_FontInfo rInfo;
                 data.read(&rInfo,sizeof(Subtitle_FontInfo));
                 
                 ALOGD("setSubtitleAttr %s %d mFontSizeType %d",__FILE__,__LINE__,(int)rInfo.mFontSizeType);
                 ALOGD("setSubtitleAttr %s %d mCustomSize %d",__FILE__,__LINE__,(int)rInfo.mCustomSize);
                 
                 reply->writeInt32(setSubtitleAttr(eType,&rInfo));
             }
             else if(FLAG_SUBTITLE_BG_COLOR==(E_Subtitle_Attr_Type)eType)
             {
                 Subtitle_Color rInfo;
                 data.read(&rInfo,sizeof(Subtitle_Color));
                 
                 ALOGD("setSubtitleAttr %s %d mA %d",__FILE__,__LINE__,(int)rInfo.mA);
                 ALOGD("setSubtitleAttr %s %d mB %d",__FILE__,__LINE__,(int)rInfo.mB);
                 
                 reply->writeInt32(setSubtitleAttr(eType,&rInfo));
             }
             else if(FLAG_SUBTITLE_TEXT_COLOR==(E_Subtitle_Attr_Type)eType)
             {
                 Subtitle_Color rInfo;
                 data.read(&rInfo,sizeof(Subtitle_Color));
                 
                 ALOGD("setSubtitleAttr %s %d mA %d",__FILE__,__LINE__,(int)rInfo.mA);
                 ALOGD("setSubtitleAttr %s %d mB %d",__FILE__,__LINE__,(int)rInfo.mB);
                 
                 reply->writeInt32(setSubtitleAttr(eType,&rInfo));
             }
             else if(FLAG_SUBTITLE_EDGE_COLOR==(E_Subtitle_Attr_Type)eType)
             {
                 Subtitle_Color rInfo;
                 data.read(&rInfo,sizeof(Subtitle_Color));
                 
                 ALOGD("setSubtitleAttr %s %d mA %d",__FILE__,__LINE__,(int)rInfo.mA);
                 ALOGD("setSubtitleAttr %s %d mB %d",__FILE__,__LINE__,(int)rInfo.mB);
                 
                 reply->writeInt32(setSubtitleAttr(eType,&rInfo));
             }
             else if(FLAG_SUBTITLE_DISPLAY_OFFSET==(E_Subtitle_Attr_Type)eType)
             {
                 Subtitle_DisplayOffset rInfo;
                 data.read(&rInfo,sizeof(Subtitle_DisplayOffset));
                 
                 ALOGD("setSubtitleAttr %s %d mX %d",__FILE__,__LINE__,(int)rInfo.mX);
                 ALOGD("setSubtitleAttr %s %d mY %d",__FILE__,__LINE__,(int)rInfo.mY);
                 
                 reply->writeInt32(setSubtitleAttr(eType,&rInfo));
             }
            return NO_ERROR;
        } break;
        
        case SET_AUDIO_TRACK: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            //ALOGD("case SET_AUDIO_TRACK)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
             int  track = data.readInt32();
            ALOGD("setAudioTrack %s %d %d",__FILE__,__LINE__,track);
            reply->writeInt32(setAudioTrack(track));
            return NO_ERROR;
        } break;
        
        case SET_SUB_TITLE_TRACK: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            ALOGD("case SET_SUB_TITLE_TRACK)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int  track = data.readInt32();
            ALOGD("setSubtitleTrack %s %d %d",__FILE__,__LINE__,track);
            reply->writeInt32(setSubtitleTrack(track));
            return NO_ERROR;
        } break;
        
        case GET_BYTE_POSITION: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            ALOGD("case GET_BYTE_POSITION)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int tmp;
            status_t ret = getCurrentBytePosition(&tmp);
            ALOGD("getCurrentBytePosition %s %d %d",__FILE__,__LINE__,tmp);
            reply->writeInt32(tmp);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        
        case GET_DOWNLOAD_RATE: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            ALOGD("case GET_DOWNLOAD_RATE)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            double tmp;
            status_t ret = getDownloadRate(&tmp);
            ALOGD("getDownloadRate %s %d %lf",__FILE__,__LINE__,tmp);
            reply->writeDouble(tmp);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        case SET_SW_VIDEO_DECODER: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            ALOGD("case SET_SW_VIDEO_DECODER)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int  tmp = data.readInt32();
            ALOGD("assignSWVideoDecoder %s %d %d",__FILE__,__LINE__,tmp);
            reply->writeInt32(assignSWVideoDecoder((bool)tmp));
            return NO_ERROR;
        } break;
        
        case SET_SW_AUDIO_DECODER: {
            //ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            ALOGD("case SET_SW_AUDIO_DECODER)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int  tmp = data.readInt32();
            ALOGD("assignSWAudioDecoder %s %d %d",__FILE__,__LINE__,tmp);
            reply->writeInt32(assignSWAudioDecoder((bool)tmp));
            return NO_ERROR;
        } break;
        
        case SET_RM_PRIORITY: {
            ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
            ALOGD("case SET_RM_PRIORITY)");
            
            CHECK_INTERFACE(IMediaPlayer, data, reply);
            int  tmp = data.readInt32();
            ALOGD("setRMPriority %s %d %d",__FILE__,__LINE__,tmp);
            reply->writeInt32(setRMPriority(tmp));
            return NO_ERROR;
        } break;
#endif        
        /*MTK END*/
        
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

// ----------------------------------------------------------------------------

}; // namespace android
