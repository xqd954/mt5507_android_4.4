/*
 * Copyright (C) 2010 The Android Open Source Project
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

//#define LOG_NDEBUG 0
#define LOG_TAG "ESQueue"
#include <media/stagefright/foundation/ADebug.h>

#include "ESQueue.h"

#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/foundation/ABitReader.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/Utils.h>

#include "include/avc_utils.h"

#include <netinet/in.h>

#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_STAGEFRIGHT_USE_XLOG)
#include <cutils/xlog.h>
#endif

namespace android {
#ifndef ANDROID_DEFAULT_CODE
const uint16_t mtkAc3SampleRateTab[3] = {48000, 44100, 32000};
const uint8_t  mtkAc3ChannelsTab[8]   = {
    2, 1, 2, 3, 3, 4, 4, 5
};

const uint16_t mtkAc3FrameSizeTab[38][3] = {
    {64, 69, 96},
    {64, 70, 96},
    {80, 87, 120},
    {96, 104, 144},
    {96, 105, 144},
    {112, 121, 168},
    {112, 122, 168},
    {128, 139, 192},
    {128, 140, 192},
    {160, 174, 240},
    {160, 175, 240},
    {192, 208, 288},
    {192, 209, 288},
    {224, 243, 336},
    {224, 244, 336},
    {256, 278, 384},
    {256, 279, 384},
    {320, 348, 480},
    {320, 349, 480},
    {384, 417, 576},
    {384, 418, 576},
    {448, 487, 672},
    {448, 488, 672},
    {512, 557, 768},
    {512, 558, 768},
    {640, 696, 960},
    {640, 697, 960},
    {768, 835, 1152},
    {768, 836, 1152},
    {896, 975, 1344},
    {896, 976, 1344},
    {1024, 1114, 1536},
    {1024, 1115, 1536},
    {1152, 1253, 1728},
    {1152, 1254, 1728},
    {1280, 1393, 1920},
    {1280, 1394, 1920},
};
#endif

ElementaryStreamQueue::ElementaryStreamQueue(Mode mode, uint32_t flags)
    : mMode(mode),
#ifndef ANDROID_DEFAULT_CODE     
    mMP3Header(0),
    mSeeking(false),
    mAudioFrameDuration(20000),
    mVorbisStatus(0),
    mH264UsePPs(false),
#ifdef MTK_OGM_PLAYBACK_SUPPORT
    mfgSearchStartCodeOptimize(0),
    mDequeueMpegAudioOgm(0),
#endif
    mfgFirstFrmAfterSeek(false),
    mUseFrameBase(false),
#endif
    mFlags(flags) {
    }

sp<MetaData> ElementaryStreamQueue::getFormat() {
    return mFormat;
}

void ElementaryStreamQueue::clear(bool clearFormat) {
    if (mBuffer != NULL) {
        mBuffer->setRange(0, 0);

#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_OGM_PLAYBACK_SUPPORT)  
        if(mfgSearchStartCodeOptimize)
        {
            mBuffer->meta()->setInt32("LPos", 0);
        }
#endif
    }

    mRangeInfos.clear();
#ifndef ANDROID_DEFAULT_CODE
    if (mMode == H264) {
        accessUnits.clear();
    }

    mfgFirstFrmAfterSeek = false;
#endif
    if (clearFormat) {
        mFormat.clear();
    }
}

static bool IsSeeminglyValidADTSHeader(const uint8_t *ptr, size_t size) {
    if (size < 3) {
        // Not enough data to verify header.
        return false;
    }

    if (ptr[0] != 0xff || (ptr[1] >> 4) != 0x0f) {
        return false;
    }

    unsigned layer = (ptr[1] >> 1) & 3;

    if (layer != 0) {
        return false;
    }

    unsigned ID = (ptr[1] >> 3) & 1;
    unsigned profile_ObjectType = ptr[2] >> 6;

    if (ID == 1 && profile_ObjectType == 3) {
        // MPEG-2 profile 3 is reserved.
        return false;
    }
#ifndef ANDROID_DEFAULT_CODE
    //in case of find the wrong error
    uint8_t number_of_raw_data_blocks_in_frame;
    number_of_raw_data_blocks_in_frame = (ptr[6]) & 0x03;
    if(number_of_raw_data_blocks_in_frame !=0)
    {
        ALOGE("Error: fake header here number_of_raw_data_blocks_in_frame=%d",number_of_raw_data_blocks_in_frame);
        return false;
    }
#endif	

    return true;
}

static bool IsSeeminglyValidMPEGAudioHeader(const uint8_t *ptr, size_t size) {
    if (size < 3) {
        // Not enough data to verify header.
        return false;
    }

    if (ptr[0] != 0xff || (ptr[1] >> 5) != 0x07) {
        return false;
    }

    unsigned ID = (ptr[1] >> 3) & 3;

    if (ID == 1) {
        return false;  // reserved
    }

    unsigned layer = (ptr[1] >> 1) & 3;

    if (layer == 0) {
        return false;  // reserved
    }

    unsigned bitrateIndex = (ptr[2] >> 4);

    if (bitrateIndex == 0x0f) {
        return false;  // reserved
    }

    unsigned samplingRateIndex = (ptr[2] >> 2) & 3;

    if (samplingRateIndex == 3) {
        return false;  // reserved
    }

    return true;
}

status_t ElementaryStreamQueue::appendData(
    const void *data, size_t size, int64_t timeUs) {
    if (mBuffer == NULL || mBuffer->size() == 0) {
        switch (mMode) {
        case H264:
#ifdef MTK_VIDEO_HEVC_SUPPORT
        case HEVC:
#endif
            {
#if 0
                if (size < 4 || memcmp("\x00\x00\x00\x01", data, 4)) {
                    return ERROR_MALFORMED;
                }
#else
                uint8_t *ptr = (uint8_t *)data;

                ssize_t startOffset = -1;
                for (size_t i = 0; i + 3 < size; ++i) {
                    if (!memcmp("\x00\x00\x00\x01", &ptr[i], 4)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    ALOGI("found something resembling an H.264/MPEG/HEVC syncword "
                        "at offset %d",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
#endif
                break;
            }
#ifndef ANDROID_DEFAULT_CODE
        case MPEG_VIDEO:
#endif
        case MPEG4_VIDEO:
            {
#if 0
                if (size < 3 || memcmp("\x00\x00\x01", data, 3)) {
                    return ERROR_MALFORMED;
                }
#else
                uint8_t *ptr = (uint8_t *)data;

                ssize_t startOffset = -1;
                for (size_t i = 0; i + 2 < size; ++i) {
                    if (!memcmp("\x00\x00\x01", &ptr[i], 3)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    ALOGI("found something resembling an H.264/MPEG syncword "
                        "at offset %d",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
#endif
                break;
            }

#ifndef ANDROID_DEFAULT_CODE
        case AVS_VIDEO:
        case VC1_VIDEO:
            {
                uint8_t *ptr = (uint8_t *)data;

                ssize_t startOffset = -1;
                for (size_t i = 0; i + 2 < size; ++i) {
                    if (!memcmp("\x00\x00\x01", &ptr[i], 3)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    ALOGD("found something resembling an AVS/VC1 VIDEO syncword at "
                        "offset %ld",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
                break;
            }
#endif
        case AAC:
            {
                uint8_t *ptr = (uint8_t *)data;

#if 0
                if (size < 2 || ptr[0] != 0xff || (ptr[1] >> 4) != 0x0f) {
                    return ERROR_MALFORMED;
                }
#else
                ssize_t startOffset = -1;
                for (size_t i = 0; i < size; ++i) {
                    if (IsSeeminglyValidADTSHeader(&ptr[i], size - i)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    ALOGI("found something resembling an AAC syncword at "
                        "offset %d",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
#endif
                break;
            }

        case MPEG_AUDIO:
            {
                uint8_t *ptr = (uint8_t *)data;

                ssize_t startOffset = -1;
                for (size_t i = 0; i < size; ++i) {
                    if (IsSeeminglyValidMPEGAudioHeader(&ptr[i], size - i)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    ALOGI("found something resembling an MPEG audio "
                        "syncword at offset %d",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
                break;
            }
#ifndef ANDROID_DEFAULT_CODE
#ifdef DTS_SUPPORT
        case DTS:
            {
                uint8_t *ptr = (uint8_t *)data;
                ssize_t startOffset = -1;

                for (size_t i = 0; i + 1 < size; ++i) {
                    if (!memcmp("\x7F\xFE\x80\x01", &ptr[i], 4) ||
                        !memcmp("\x64\x58\x20\x25", &ptr[i], 4)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    LOGI("found something resembling an DTS syncword at offset %ld",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
                break;
            }
#endif
        case AC3:
            {
                uint8_t *ptr = (uint8_t *)data;
                ssize_t startOffset = -1;

                for (size_t i = 0; i + 1 < size; ++i) {
                    if (!memcmp("\x0B\x77", &ptr[i], 2)) {
                        startOffset = i;
                        break;
                    }
                }

                if (startOffset < 0) {
                    return ERROR_MALFORMED;
                }

                if (startOffset > 0) {
                    LOGI("found something resembling an AC3 syncword at offset %ld",
                        startOffset);
                }

                data = &ptr[startOffset];
                size -= startOffset;
                break;
            }
        case PSLPCM:
        case LPCM:
        case BDLPCM:

            break;
        case PES_METADATA:
            {
                ALOGD("appendData(PES_METADATA)");
                break;
            }
        case VORBIS_AUDIO:
            {
                uint8_t *ptr = (uint8_t *)data;
                if (!memcmp("\x76\x6f\x72\x62\x69\x73", &ptr[1], 6))
                {
                    if(mVorbisStatus > 6)
                    {
                        ALOGI("SKIP VORBIS header, type %d", ptr[0]);
                        data = &ptr[size];
                        size -= size;
                        break;
                    }
                    ALOGI("found VORBIS header, type %d", ptr[0]);
                }
                break;
            }
#endif 
        case PCM_AUDIO:
            {
                break;
            }
        default:
            TRESPASS();
            break;
        }
    }

    size_t neededSize = (mBuffer == NULL ? 0 : mBuffer->size()) + size;
    if (mBuffer == NULL || neededSize > mBuffer->capacity()) {
        neededSize = (neededSize + 65535) & ~65535;

        ALOGV("resizing buffer to size %d", neededSize);

        sp<ABuffer> buffer = new ABuffer(neededSize);
        if (mBuffer != NULL) {
            memcpy(buffer->data(), mBuffer->data(), mBuffer->size());
            buffer->setRange(0, mBuffer->size());
        } else {
            buffer->setRange(0, 0);
        }

        mBuffer = buffer;
    }

#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_OGM_PLAYBACK_SUPPORT)  
    if(mfgSearchStartCodeOptimize && ((mBuffer->size() == 0)))
    {
        mBuffer->meta()->setInt32("LPos", 0);
    }
#endif

    memcpy(mBuffer->data() + mBuffer->size(), data, size);
    mBuffer->setRange(0, mBuffer->size() + size);

    RangeInfo info;
    info.mLength = size;
    info.mTimestampUs = timeUs;
    mRangeInfos.push_back(info);

#if 0
    if (mMode == AAC) {
        ALOGI("size = %d, timeUs = %.2f secs", size, timeUs / 1E6);
        hexdump(data, size);
    }
#endif

    return OK;
}

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnit() {
    if ((mFlags & kFlag_AlignedData) && mMode == H264) {
        if (mRangeInfos.empty()) {
            return NULL;
        }

        RangeInfo info = *mRangeInfos.begin();
        mRangeInfos.erase(mRangeInfos.begin());

        sp<ABuffer> accessUnit = new ABuffer(info.mLength);
        memcpy(accessUnit->data(), mBuffer->data(), info.mLength);
        accessUnit->meta()->setInt64("timeUs", info.mTimestampUs);

        memmove(mBuffer->data(),
            mBuffer->data() + info.mLength,
            mBuffer->size() - info.mLength);

        mBuffer->setRange(0, mBuffer->size() - info.mLength);

        if (mFormat == NULL) {
            mFormat = MakeAVCCodecSpecificData(accessUnit);
        }

        return accessUnit;
    }

    switch (mMode) {
    case H264:
        return dequeueAccessUnitH264();
    case AAC:
        return dequeueAccessUnitAAC();
#ifndef ANDROID_DEFAULT_CODE
#ifdef MTK_VIDEO_HEVC_SUPPORT
    case HEVC:
        return dequeueAccessUnitHEVC();
#endif
#ifdef DTS_SUPPORT
    case DTS:
        return dequeueAccessUnitDTS();
#endif 
    case AC3:
        return dequeueAccessUnitAC3();
    case PSLPCM:
        return dequeueAccessUnitPSLPCM();
    case VORBIS_AUDIO:
        return dequeueAccessUnitVORBISAudio();
    case LPCM:         
        return dequeueAccessUnitLPCM();
    case BDLPCM:       
        return dequeueAccessUnitBDLPCM();
    case VC1_VIDEO:
        return dequeueAccessUnitVC1Video();
    case AVS_VIDEO:
        return dequeueAccessUnitAVSVideo();
    case PES_METADATA:
        return dequeueAccessUnitPESMetaData();
#endif
    case MPEG_VIDEO:
        return dequeueAccessUnitMPEGVideo();
    case MPEG4_VIDEO:
        return dequeueAccessUnitMPEG4Video();
    case PCM_AUDIO:
        return dequeueAccessUnitPCMAudio();
    default:
        CHECK_EQ((unsigned)mMode, (unsigned)MPEG_AUDIO);
#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_OGM_PLAYBACK_SUPPORT) //for tablet only
        if(mDequeueMpegAudioOgm) {
            return dequeueAccessUnitMPEGAudioOgm();
        } else 
#endif
        { return dequeueAccessUnitMPEGAudio();
        }
    }
}
#ifndef ANDROID_DEFAULT_CODE
static int findID3(const uint8_t *start, int offset, int length)
{
    int i = 0;
    int j = 0;

#if 0
    LOGI("dump data to check ID3 location");
    for(j = 0; j < 256; ++j) {
        ALOGD("%02d: %02x", j, start[j]);
    }
#endif

    for(i = offset; i<length; ++i){
        if((start[i] == 0x49) 
            && (start[i+1] == 0x44)
            && (start[i+2] == 0x33)){
            ALOGD("found right ID3 at %d", i);
            return i;
        }
    }
    ALOGE("can't find ID3");
    return -1;
}

static int findAPIC(const uint8_t *start, int offset, int length)
{
    int i = 0;
    int j = 0;

#if 0
    LOGI("dump data to check APIC location");
    for(j = 0; j < 256; ++j) {
        ALOGD("%02d: %02x", j, start[j]);
    }
#endif

    for(i = offset; i<length; ++i){
        if((start[i] == 0x41) 
            && (start[i+1] == 0x50)
            && (start[i+2] == 0x49)
            && (start[i+3] == 0x43)){
            ALOGD("found right APIC at %d", i);
            return i;
        }
    }
    ALOGE("can't find APIC");
    return -1;
}

static int findSOI(const uint8_t *start, int offset, int length)
{
    int i = 0;
    int j = 0;

#if 0
    LOGI("dump data to check SOI location");
    for(j = 0; j < 256; ++j) {
        ALOGD("%02d: %02x", j, start[j]);
    }
#endif

    for(i = offset; i<length; ++i){
        if((start[i] == 0xff) 
            && (start[i+1] == 0xd8)){
            ALOGD("found right SOI at %d", i);
            return i;
        }
    }
    ALOGE("can't find SOI");
    return -1;
}

static int findEOI(const uint8_t *start, int offset, int length)
{
    int i = 0;
    int j = 0;

#if 0
    LOGI("dump data to check EOI location");
    for(int j = 0; j < 256; ++j) {
        ALOGD("%02d: %02x", j, start[j]);
    }
#endif

    for(i = offset; i<length; ++i){
        if((start[i] == 0xff) 
            && (start[i+1] == 0xd9)){
            ALOGD("found right EOI at %d", i);
            return i;
        }
    }
    ALOGE("can't find EOI");
    return -1;
}


sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitPESMetaData() {
    bool find = false;
    int i, j;
    int length, start, end, apic_pos, id3_pos, tag_length;
    ALOGD("new member: dequeue pes meta");
    if (mBuffer->size() == 0) {
        return NULL;
    }

    const uint8_t *src =
        (const uint8_t *)mBuffer->data() + mBuffer->offset();

    start = findSOI(src, 0, mBuffer->size());
    end = findEOI(src, 0, mBuffer->size());
    id3_pos = findID3(src, 0, mBuffer->size());
    apic_pos = findAPIC(src, 0, mBuffer->size());
    if((-1 == id3_pos)
        ||(-1 == apic_pos)
        ||(apic_pos < id3_pos)){
        ALOGD("no legal id3 album picture");
    }

    tag_length = mBuffer->data()[apic_pos + 4]*0x1000000
        + mBuffer->data()[apic_pos + 5]*0x10000
        + mBuffer->data()[apic_pos + 6]*0x100
        + mBuffer->data()[apic_pos + 7];
    ALOGD("mBuffer->data()[apic_pos + 4] is 0x%x", mBuffer->data()[apic_pos + 4]);
    ALOGD("mBuffer->data()[apic_pos + 5] is 0x%x", mBuffer->data()[apic_pos + 5]);
    ALOGD("mBuffer->data()[apic_pos + 6] is 0x%x", mBuffer->data()[apic_pos + 6]);
    ALOGD("mBuffer->data()[apic_pos + 7] is 0x%x", mBuffer->data()[apic_pos + 7]);
    ALOGD("tag_length is %d", tag_length);

    length = end - start + 2;
    ALOGD("start at %d, end at %d, length is %d", start, end, length);
    if ((start == -1) || (end == -1)) {
        start = mBuffer->size();
        length = 0;
        ALOGD("can't find a legal jpeg bitstream.");
    }

    CHECK(mBuffer != NULL);
    mBuffer->setRange(mBuffer->offset() + start, length);

    if (mBuffer->size() == 0) {
        return NULL;
    }

    ALOGD("dump data to check SOI location");
    for(i = 0; i < 16; ++i) {
        ALOGD("%02d: %02x", i, mBuffer->data()[i]);
    }

    ALOGD("dump data to check EOI location");
    for(j = 16; j > 0; --j) {
        ALOGD("%02d: %02x", j, mBuffer->data()[length - j]);
    }

    sp<ABuffer> accessUnit = new ABuffer(length);
    memcpy(accessUnit->data(), mBuffer->data(), length);


    accessUnit->meta()->setString("mime", "image/jpeg", 10);
    accessUnit->meta()->setBuffer("pictureBuffer", accessUnit);
    accessUnit->meta()->setInt64("timeUs", 0);
    ALOGD("set album art picture here");

    if (mFormat == NULL) {
        mFormat = new MetaData;
        mFormat->setCString(kKeyMIMEType, "image/jpeg");
    }

    AString testMimeType;
    sp<ABuffer> testBuffer;
    if(((accessUnit)->meta()->findString("mime", &testMimeType)) &&((accessUnit)->meta()->findBuffer("pictureBuffer", &testBuffer)))
    {
        ALOGD("test get access unit meta() ok");
    }

    mBuffer->setRange(0, 0); // To avoid repeatedly read the same buffer

    return accessUnit;
}
#endif //ANDROID_DEFAULT_CODE
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitPCMAudio() {
    if (mBuffer->size() < 4) {
        return NULL;
    }

    ABitReader bits(mBuffer->data(), 4);
#ifndef ANDROID_DEFAULT_CODE
    if(bits.getBits(8) != 0xa0)
        return NULL;
#else
    CHECK_EQ(bits.getBits(8), 0xa0);
#endif
    unsigned numAUs = bits.getBits(8);
    bits.skipBits(8);
    unsigned quantization_word_length = bits.getBits(2);
    unsigned audio_sampling_frequency = bits.getBits(3);
    unsigned num_channels = bits.getBits(3);

#ifndef ANDROID_DEFAULT_CODE
    if(audio_sampling_frequency != 2 || num_channels != 1u)
        return NULL;
#else
    CHECK_EQ(audio_sampling_frequency, 2);  // 48kHz
    CHECK_EQ(num_channels, 1u);  // stereo!
#endif

    if (mFormat == NULL) {
        mFormat = new MetaData;
        mFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);
        mFormat->setInt32(kKeyChannelCount, 2);
        mFormat->setInt32(kKeySampleRate, 48000);
    }
#ifndef ANDROID_DEFAULT_CODE
#ifdef MTK_AUDIO_RAW_SUPPORT
    mFormat->setInt32(kKeyEndian, 2);	//1: big endian, 2: little endia
    mFormat->setInt32(kKeyBitWidth, 16); 
    mFormat->setInt32(kKeyPCMType, 1);	//1: WAV file, 2: BD file, 3: DVD_VOB file, 4: DVD_AOB file
    //mFormat->setInt32(kKeyChannelAssignment, 1);	// 2 channels
#endif//MTK_AUDIO_RAW_SUPPORT
#endif//ANDROID_DEFAULT_CODE

    static const size_t kFramesPerAU = 80;
    size_t frameSize = 2 /* numChannels */ * sizeof(int16_t);

    size_t payloadSize = numAUs * frameSize * kFramesPerAU;

    if (mBuffer->size() < 4 + payloadSize) {
        return NULL;
    }

    sp<ABuffer> accessUnit = new ABuffer(payloadSize);
    memcpy(accessUnit->data(), mBuffer->data() + 4, payloadSize);

    int64_t timeUs = fetchTimestamp(payloadSize + 4);
    CHECK_GE(timeUs, 0ll);
    accessUnit->meta()->setInt64("timeUs", timeUs);

    int16_t *ptr = (int16_t *)accessUnit->data();
    for (size_t i = 0; i < payloadSize / sizeof(int16_t); ++i) {
        ptr[i] = ntohs(ptr[i]);
    }

    memmove(
        mBuffer->data(),
        mBuffer->data() + 4 + payloadSize,
        mBuffer->size() - 4 - payloadSize);

    mBuffer->setRange(0, mBuffer->size() - 4 - payloadSize);

    return accessUnit;
}

#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_OGM_PLAYBACK_SUPPORT)  
//Search start code optimiztion
//fgEnable == true: memorize last scanned buffer potition, start to search at last stop position 
//fgEnable == false: always start to search at the beginning of mbuffer queue 
void ElementaryStreamQueue::setSearchSCOptimize(bool fgEnable)
{
    mfgSearchStartCodeOptimize = fgEnable;
}

void ElementaryStreamQueue::setDequeueMpegAudioOgm(bool fgEnable)
{
    LOGE("set  mDequeueMpegAudioOgm :%d\n",fgEnable);
    mDequeueMpegAudioOgm = fgEnable;
}
#endif

#ifndef ANDROID_DEFAULT_CODE
#ifdef DTS_SUPPORT
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitDTS() {

    int  prevStartCode    = -1;
    int  currentStartCode = -1;
    uint8_t  *data = mBuffer->data();
    size_t   size = mBuffer->size();

    /*find syncword*/
    ssize_t  offset = 0;
    while (offset + 4 < size) {
        if (memcmp("\x7F\xFE\x80\x01", &data[offset], 4) &&
            memcmp("\x64\x58\x20\x25", &data[offset], 4) &&
            memcmp("\x1F\xFF\xE8\x00", &data[offset], 4)) {
            /*search for any type of the three kind of syncwd*/
            ++offset;
            continue;
        }

        prevStartCode    = currentStartCode;
        currentStartCode = data[offset];

        if (prevStartCode == -1) {
            if (offset) {
                memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
                size -= offset;
                fetchTimestamp(offset);
                mBuffer->setRange(0, size);
                offset = 0;
            }
        }

        if (prevStartCode != -1) {

            if (mFormat == NULL) {
                mFormat = new MetaData;
                mFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_DTS);
            }

            sp<ABuffer> accessUnit = new ABuffer(offset);
            memcpy(accessUnit->data(), data, offset);

            memmove(mBuffer->data(),
                mBuffer->data() + offset,
                mBuffer->size() - offset);

            mBuffer->setRange(0, mBuffer->size() - offset);

            int64_t timeUs = fetchTimestamp(offset);
            CHECK_GE(timeUs, 0ll);
            offset = 0;
            accessUnit->meta()->setInt64("timeUs", timeUs);

            return accessUnit;
        }
        ++offset;
    }//while
    return NULL;
}
#endif//DTS
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitAC3() {

    ssize_t  offset = -1;
    uint8_t  *ptr = mBuffer->data();
    size_t   size = mBuffer->size();
    uint8_t  bitStreamId = 0;
    uint8_t  fscod = 0;
    int      frmSize = 0;
    uint16_t sampleRate = 0;
    uint8_t  channelCount = 0;
    uint8_t  channelMode = 0;
    uint8_t  srShift = 0;
    uint8_t  lfeOn = 0;
    uint16_t actualSize = 0;
    uint8_t  bsid = 0;

    if (size < 10) {
        ALOGD("****NO enough data in AC3 ESQueue****");
        return NULL;
    }

    /*find first syncword*/
    for (size_t i = 0; i + 1 < size; ++i) {
        if (!memcmp(&ptr[i], "\x0B\x77", 2)) {
            offset = i;
            break;
        }
    }

    if (offset < 0) {
        return NULL;
    }
    ABitReader bits(mBuffer->data() + offset, mBuffer->size() - offset);
    CHECK_EQ(bits.getBits(16), 0x0B77);

    /*bsid was used to distinguish between ac3 and eac3*/
    bsid = (ptr[offset + 5] & 0xF8) >> 3;


    if (bsid <= 10) {/*ac3*/
        /*crc1 */
        bits.skipBits(16);

        /*fscod*/
        fscod = bits.getBits(2);    
        CHECK_NE(fscod, 3);

        /*frame size*/
        frmSize = bits.getBits(6);
        CHECK_LE(frmSize, 37);

        /*actual size of the syncframe (int byte)*/
        actualSize  = mtkAc3FrameSizeTab[frmSize][fscod] * 2;

        if (mFormat == NULL) {
            mFormat = new MetaData;

            LOGD("cinfigure AC3 Format\n");
            /*bit stream id*/
            bitStreamId = bits.getBits(5);
            //CHECK_LE(bitStreamId , 10); /*no eac3*/
            srShift = ((bitStreamId > 8) ? bitStreamId: 8) - 8;

            /*bit stream mode*/
            bits.skipBits(3);

            channelMode = bits.getBits(3);
            if (channelMode == 0x2) {
                bits.skipBits(2);
            } else {
                if ((channelMode & 0x1) && channelMode != 0x1)
                    bits.skipBits(2);
                if (channelMode & 0x4)
                    bits.skipBits(2);
            }

            lfeOn = bits.getBits(1);
            sampleRate = mtkAc3SampleRateTab[fscod] >> srShift;
            channelCount = mtkAc3ChannelsTab[channelMode] + lfeOn;

            mFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AC3);
            mFormat->setInt32(kKeySampleRate, sampleRate);
            mFormat->setInt32(kKeyChannelCount, channelCount);
            ALOGV("AC3 sample:%u, channel:%u",sampleRate, channelCount);
        }
    } else if (bsid <= 16) {/*EAC3*/
        bits.skipBits(2);              /*strmtyp*/
        bits.skipBits(3);              /*substreamid*/

        actualSize = (bits.getBits(11) + 1) << 1;

        if (mFormat == NULL) {
            mFormat = new MetaData;

            LOGD("cinfigure E-AC3 Format\n");
            fscod = bits.getBits(2);
            if (fscod == 3) {
                uint8_t fscod2 = bits.getBits(2);
                CHECK_NE(fscod2, 3);
                sampleRate  = mtkAc3SampleRateTab[fscod2] / 2;
            } else {
                bits.skipBits(2);     /*numblkscod*/
                sampleRate  = mtkAc3SampleRateTab[fscod];
            }

            channelMode = bits.getBits(3);
            lfeOn = bits.getBits(1);
            channelCount = mtkAc3ChannelsTab[channelMode] + lfeOn;

            mFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AC3);
            mFormat->setInt32(kKeySampleRate, sampleRate);
            mFormat->setInt32(kKeyChannelCount, channelCount);
            ALOGV("E-AC3 sample:%u, channel:%u",sampleRate, channelCount);
        }
    } else {
        ALOGD("[%s]***wrong bsid:%u***", __FUNCTION__, bsid);
        return NULL;
    }

    if (actualSize > size - offset) {
        LOGI("NO enough data: accessUnit size:%d > remaining data after offset :%d", actualSize, size-offset);
        return NULL;
    }

    sp<ABuffer> accessUnit = new ABuffer(actualSize);
    if (accessUnit == NULL) {
        return NULL;
    }

    memcpy(accessUnit->data(), mBuffer->data() + offset, actualSize);

    memmove(mBuffer->data(), mBuffer->data() + offset + actualSize,
        mBuffer->size() - offset -  actualSize);
    mBuffer->setRange(0, mBuffer->size() - offset - actualSize);

    int64_t timeUs = -1;
    timeUs = fetchTimestamp(actualSize);

    if (timeUs >= 0) {
        accessUnit->meta()->setInt64("timeUs", timeUs);
    }
    return accessUnit;
}

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitAAC() {
    if (mBuffer->size() == 0) {
        return NULL;
    }

    CHECK(!mRangeInfos.empty());

    const RangeInfo &info = *mRangeInfos.begin();
    if (mBuffer->size() < info.mLength) {
        return NULL;
    }

    CHECK_GE(info.mTimestampUs, 0ll);

    // The idea here is consume all AAC frames starting at offsets before
    // info.mLength so we can assign a meaningful timestamp without
    // having to interpolate.
    // The final AAC frame may well extend into the next RangeInfo but
    // that's ok.
    size_t offset = 0;
    while (offset < info.mLength) {
        if (offset + 7 > mBuffer->size()) {
            return NULL;
        }

        ABitReader bits(mBuffer->data() + offset, mBuffer->size() - offset);

        // adts_fixed_header

        CHECK_EQ(bits.getBits(12), 0xfffu);
        bits.skipBits(3);  // ID, layer
        bool protection_absent = bits.getBits(1) != 0;

        if (mFormat == NULL) {
            unsigned profile = bits.getBits(2);
            CHECK_NE(profile, 3u);
            unsigned sampling_freq_index = bits.getBits(4);
            bits.getBits(1);  // private_bit
            unsigned channel_configuration = bits.getBits(3);
            CHECK_NE(channel_configuration, 0u);
            bits.skipBits(2);  // original_copy, home

            mFormat = MakeAACCodecSpecificData(
                profile, sampling_freq_index, channel_configuration);

            mFormat->setInt32(kKeyIsADTS, true);

            int32_t sampleRate;
            int32_t numChannels;
            CHECK(mFormat->findInt32(kKeySampleRate, &sampleRate));
            CHECK(mFormat->findInt32(kKeyChannelCount, &numChannels));

            ALOGI("found AAC codec config (%d Hz, %d channels)",
                sampleRate, numChannels);
        } else {
            // profile_ObjectType, sampling_frequency_index, private_bits,
            // channel_configuration, original_copy, home
            bits.skipBits(12);
        }

        // adts_variable_header

        // copyright_identification_bit, copyright_identification_start
        bits.skipBits(2);

        unsigned aac_frame_length = bits.getBits(13);

        bits.skipBits(11);  // adts_buffer_fullness

        unsigned number_of_raw_data_blocks_in_frame = bits.getBits(2);

        if (number_of_raw_data_blocks_in_frame != 0) {
            // To be implemented.
            TRESPASS();
        }

        if (offset + aac_frame_length > mBuffer->size()) {
            return NULL;
        }

        size_t headerSize = protection_absent ? 7 : 9;

        offset += aac_frame_length;
    }

    int64_t timeUs = fetchTimestamp(offset);

    sp<ABuffer> accessUnit = new ABuffer(offset);
    memcpy(accessUnit->data(), mBuffer->data(), offset);

    memmove(mBuffer->data(), mBuffer->data() + offset,
        mBuffer->size() - offset);
    mBuffer->setRange(0, mBuffer->size() - offset);

    accessUnit->meta()->setInt64("timeUs", timeUs);

    return accessUnit;
}
#endif
int64_t ElementaryStreamQueue::fetchTimestamp(size_t size) {
    int64_t timeUs = -1;
    bool first = true;

    while (size > 0) {
        CHECK(!mRangeInfos.empty());

        RangeInfo *info = &*mRangeInfos.begin();
#ifndef ANDROID_DEFAULT_CODE
        //Add for Special MPEG File
        if ((first == false) && (timeUs == 0xFFFFFFFF) && (info->mTimestampUs != 0x0))
        {
            ALOGE("fetchTimestamp - Change: %lld  %lld \n", timeUs, info->mTimestampUs);
            timeUs = info->mTimestampUs;
        }
#endif //#ifndef ANDROID_DEFAULT_CODE

        if (first) {
            timeUs = info->mTimestampUs;
            first = false;
        }

        if (info->mLength > size) {
            info->mLength -= size;
            size = 0;
        } else {
            size -= info->mLength;

            mRangeInfos.erase(mRangeInfos.begin());
            info = NULL;
        }

    }

    if (timeUs == 0ll) {
        ALOGV("Returning 0 timestamp");
    }

    return timeUs;
}

struct NALPosition {
    size_t nalOffset;
    size_t nalSize;
};

#ifndef ANDROID_DEFAULT_CODE
void ElementaryStreamQueue::setSeeking(bool h264UsePPs) {
    mSeeking = true;
    mH264UsePPs = h264UsePPs;
}
void ElementaryStreamQueue::useFrameBase() {
    ALOGI("ESQueue frame base h264");
    mUseFrameBase = true;
}
bool ElementaryStreamQueue::IsIFrame(uint8_t *nalStart, size_t nalSize) {
    unsigned nalType = nalStart[0] & 0x1f;
    unsigned slice_type =0;		

    if((nalType >0 && nalType <6) || nalType == 19 )
    {
        ABitReader br(nalStart + 1, nalSize);
        unsigned first_mb_in_slice = parseUE(&br);
        slice_type = parseUE(&br);
    }  

    if ((mH264UsePPs && nalType == 7 ) ||        //PPS
        (!mH264UsePPs && (nalType == 5 || slice_type ==2 ||slice_type==7))) {   // I frame
        ALOGI("%s() nalType=%d slice_type=%d ,nalSize:%d",__FUNCTION__,nalType,slice_type, nalSize);
        return true;
    }
    return false;
}

#ifdef MTK_VIDEO_HEVC_SUPPORT
static void parseProfileTierLevel(ABitReader *br, unsigned subLayers) {

    //br->skipBits(120);//24 40 48 56  64 72

    br->getBits(2);//unsigned profile_space =
    br->getBits(1);//unsigned tier_flag =
    br->getBits(5);//unsigned profile_idc =
    for (int j = 0; j < 32; j++) {
        br->getBits(1);
    }
    br->getBits(4);//progressive_flag...only_constraint_flag
    br->skipBits(44);//reserved_zero_44bits

    unsigned general_level_idc = br->getBits(8);

    unsigned profilePresent[8];
    unsigned levelPresent[8];
    for (unsigned i = 0; i < subLayers; i++) {
        profilePresent[i] = br->getBits(1);
        levelPresent[i] = br->getBits(1);
    }
    if (subLayers > 0) {
        for (int i = subLayers; i < 8; i++) {
            unsigned reserved_zero_2bits = br->getBits(2);
            CHECK_EQ(reserved_zero_2bits, 0);
        }
    }
    for (unsigned i = 0; i < subLayers; i++) {
        if (profilePresent[i]) {
            br->skipBits(88);
        }
        if (levelPresent[i]) {
            br->skipBits(8);
        }
    }
    return ;
}

static sp<ABuffer> FindHEVCNAL(
        const uint8_t *data, size_t size, unsigned nalType) {
    const uint8_t *nalStart;
    size_t nalSize;
    while (getNextNALUnit(&data, &size, &nalStart, &nalSize, true) == OK) {
        if ((unsigned)((nalStart[0] & 0x7E)>>1) == nalType) {
            sp<ABuffer> buffer = new ABuffer(nalSize);
            memcpy(buffer->data(), nalStart, nalSize);
            return buffer;
        }
    }

    return NULL;
}

/*turn 00 00 03 to 00 00*/
static status_t adjustSPS(uint8_t *sps, size_t * spsLen) {
    uint8_t *data = sps;
    size_t  size = *spsLen;
    size_t  offset = 0;

    while (offset + 2 <= size) {
        if (data[offset] == 0x00 && data[offset+1] == 0x00 && data[offset+2] == 0x03) {
            //found 00 00 03
            if (offset + 2 == size) {//00 00 03 as suffix
                *spsLen -=1;
                return OK;
            }

            offset += 2; //point to 0x03
            memcpy(data+offset, data+(offset+1), size - offset);//cover ox03

            size -= 1;
            *spsLen -= 1;
            continue;
        }
        ++offset;
    }

    return OK;
}

static void findHEVCSPSInfo(uint8_t *sps, unsigned spsLen, unsigned * spsWidth, unsigned * spsHeight) {

    sps += 2;
    spsLen -= 2;
    adjustSPS(sps, &spsLen);//clear emulation_prevention_three_byte
    ABitReader br(sps, spsLen);//no nalheader
    //clear emulation_prevention_three_byte
    br.skipBits(4);//sps_video_parameter_set_id
    unsigned subLayers = br.getBits(3);//sub layers num minus 1
    br.skipBits(1);

    //profile_tier_level
    parseProfileTierLevel(&br, subLayers);

    unsigned sps_seq_parameter_set_id = parseUE(&br);//sps_seq_parameter_set_id
    unsigned chroma_format_idc =  parseUE(&br);
    if (chroma_format_idc == 3) {
        br.skipBits(1);
    }
    *spsWidth = parseUE(&br);
    *spsHeight = parseUE(&br);
    ALOGD("[HEVC:SPS]subLayers:%u, sps_seq_parameter_set_id:%u, chroma_format_idc:%u, Width:%u, Height:%u",
        subLayers, sps_seq_parameter_set_id, chroma_format_idc,
        *spsWidth, *spsHeight);
    return ;
}

static sp<MetaData> MakeHEVCMetaData(const sp<ABuffer> &accessUnit) {
    const uint8_t *data = accessUnit->data();
    size_t size = accessUnit->size();

    sp<ABuffer> seqParamSet = FindHEVCNAL(data, size, 33);//start after00000001
    sp<ABuffer> picParamSet = FindHEVCNAL(data, size, 34);
    if (seqParamSet == NULL || seqParamSet == NULL) {
        return NULL;
    }

    unsigned spsLen = seqParamSet->size();
    unsigned ppsLen = seqParamSet->size();


    sp<ABuffer> csd = new ABuffer((spsLen + ppsLen + 28)*sizeof(uint8_t));
    uint8_t *out = csd->data();

    uint8_t* sps = seqParamSet->data();
    uint8_t* pps = picParamSet->data();

    out[0] = 1; // configurationVersion == 1
    out[1] = sps[1]; // profile_space:2 bits, tier_flag:1bits, profile_idc:5 bits
    /* 4 bytes profile_compability_indications */
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = 0;
    /* 2 bytes constraint_indicator_flags */
    out[6] = 0;
    out[7] = 0;
    out[8] = sps[ 3]; // level_idc   [OK?]
    out[9] = 0; //min_spatial_segmentation_idc
    out[10] = 0b11111100; //reserved 111111 + parallismType 2 bits
    out[11] = 0b11111101; //reserved  111111 + chromaFormat 2 bits->01: 4:2:0
    out[12] = 0b11111000; //reserved 11111 + bitDepthLumaMinus8 3bits
    out[13] = 0b11111000; //reserved 111111 + bitDepthChromaMinus8 3 bits
    /* 2 bytes avgFrameRate */
    out[14] = 0; // constantFrameRate: 2 bits, numTemporalLayers: 3 bits, temporalIdNested: 1 bit,
    out[15] = 0;
    //out[16] = (sps_len + pps_len) & 0b00000011; //lengthSizeMinusOne
    out[16] = 0xff; //lengthSizeMinusOne
    out[17] = 2; //numOfArrays
    /* add sps to array */
    out[18] = 0b10100000; //vps
    /* 2 bytes numOfNalus */
    out[19] = 0;
    out[20] = 1;
    /* 2 bytes nalUnitLength */
    out[21] = (spsLen>>8) & 0b11111111;
    out[22] = spsLen & 0b11111111;
    memcpy(out + 23, sps , spsLen);
    /* add pps to array */
    out[23 + spsLen] = 0b10100001; //sps
    /* 2 bytes numOfNalus */
    out[24 + spsLen] = 0;
    out[25 + spsLen] = 1;
    /* 2 bytes nalUnitLength */
    out[26 + spsLen] = (ppsLen >> 8) & 0b11111111;
    out[27 + spsLen] = ppsLen & 0b11111111;
    memcpy(out + 28 + spsLen, pps, ppsLen);

    unsigned pic_width_in_luma_samples;
    unsigned pic_height_in_luma_samples;

    findHEVCSPSInfo(sps, spsLen, &pic_width_in_luma_samples, &pic_height_in_luma_samples);

    sp<MetaData> meta = new MetaData;
    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_HEVC);
    //meta->setData(kKeyHVCC, kTypeHVCC, csd->data(), csd->size());
    meta->setInt32(kKeyWidth, pic_width_in_luma_samples);
    meta->setInt32(kKeyHeight, pic_height_in_luma_samples);

    return meta;
}

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitHEVC() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();
    Vector<NALPosition> nals;
    size_t totalSize = 0;
    status_t err;
    const uint8_t *nalStart;
    size_t nalSize;
    bool foundSlice = false;
    int preVCLIndex = -1;
    int nalNum = -1;

    while ((err = getNextNALUnit(&data, &size, &nalStart, &nalSize)) == OK) {
        CHECK_GT(nalSize, 0u);

        unsigned sliceType = (nalStart[0] & 0x7E)>>1;

        bool flush = false;

         if (sliceType == 35u) {
             /*delimiter starting an AU*/
            if (foundSlice && (preVCLIndex != -1)) {
               flush = true;
            }
           foundSlice = true;
         } else if ((sliceType <= 3u) || (sliceType >= 16u && sliceType <= 21u)) {
            //slice_segment_layer_rbsp()
            /*first_slice_segment_in_pic_flag*/
            unsigned firstSlice = (nalStart[2] & 0x80)>>7;

            if (firstSlice) {//firstSlice indicates an new AU
                if (foundSlice && (preVCLIndex != -1)) {
                  flush = true;
                }
                foundSlice = true;
            }
        }

        if (flush) {
            // The access unit will contain all nal units up to, but excluding
            // the current one, separated by 0x00 0x00 0x00 0x01 startcodes.

             ALOGV("[%s]flush sliceType = %u preVCLIndex:%d nals.size():%d",
                __FUNCTION__, sliceType, preVCLIndex, nals.size());

            size_t auSize = 4 * nalNum + totalSize;
            sp<ABuffer> accessUnit = new ABuffer(auSize);

            size_t dstOffset = 0;
            for (int i = 0; i < nalNum; ++i) {
                const NALPosition &pos = nals.itemAt(i);
                ALOGV("[hevc]pos:%d size:%d", pos.nalOffset, pos.nalSize);
                memcpy(accessUnit->data() + dstOffset, "\x00\x00\x00\x01", 4);

                memcpy(accessUnit->data() + dstOffset + 4,
                        mBuffer->data() + pos.nalOffset,
                        pos.nalSize);

                dstOffset += pos.nalSize + 4;
            }

            const NALPosition &pos = nals.itemAt(nalNum - 1);
            size_t nextScan = pos.nalOffset + pos.nalSize;

            memmove(mBuffer->data(),
                    mBuffer->data() + nextScan,
                    mBuffer->size() - nextScan);

            mBuffer->setRange(0, mBuffer->size() - nextScan);

            int64_t timeUs = fetchTimestamp(nextScan);
                CHECK_GE(timeUs, 0ll);

            accessUnit->meta()->setInt64("timeUs", timeUs);

            if (mFormat == NULL) {
                  mFormat = MakeHEVCMetaData(accessUnit);
            }
            return accessUnit;
        }

        NALPosition pos;
        pos.nalOffset = nalStart - mBuffer->data();
        pos.nalSize = nalSize;

        nals.push_back(pos);
        totalSize += nalSize;
        if (sliceType <= 31u) {/*latest VCL unit*/
            preVCLIndex = nals.size();//position of preVCLunit
        }
        nalNum = nals.size();
        ALOGV("nals add sliceType:%u, nals.size:%u", sliceType, nals.size());
    }
    CHECK_EQ(err, (status_t)-EAGAIN);

    return NULL;
}
#endif //MTK_VIDEO_HEVC_SUPPORT

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitH264() {
    const uint8_t *data = mBuffer->data();

    size_t size = mBuffer->size();
    Vector<NALPosition> nals;

    size_t totalSize = 0;

    status_t err;
    const uint8_t *nalStart;
    size_t nalSize;
    bool foundSlice = false;
    while ((err = getNextNALUnit(&data, &size, &nalStart, &nalSize)) == OK) {
        if (nalSize == 0) continue;

        unsigned nalType = nalStart[0] & 0x1f;
        bool flush = false;

        if (nalType == 1 || nalType == 5) {
            if (foundSlice) {
                ABitReader br(nalStart + 1, nalSize);
                unsigned first_mb_in_slice = parseUE(&br);

                if (first_mb_in_slice == 0) {
                    // This slice starts a new frame.

                    flush = true;
                }
            }

            foundSlice = true;
        } else if ((nalType == 9 || nalType == 7) && foundSlice) {
            // Access unit delimiter and SPS will be associated with the
            // next frame.

            flush = true;
        }

        if (flush) {
            // The access unit will contain all nal units up to, but excluding
            // the current one, separated by 0x00 0x00 0x00 0x01 startcodes.

            size_t auSize = 4 * nals.size() + totalSize;
            sp<ABuffer> accessUnit = new ABuffer(auSize);

#if !LOG_NDEBUG
            AString out;
#endif

            size_t dstOffset = 0;
            for (size_t i = 0; i < nals.size(); ++i) {
                const NALPosition &pos = nals.itemAt(i);

                unsigned nalType = mBuffer->data()[pos.nalOffset] & 0x1f;

#if !LOG_NDEBUG
                char tmp[128];
                sprintf(tmp, "0x%02x", nalType);
                if (i > 0) {
                    out.append(", ");
                }
                out.append(tmp);
#endif

                memcpy(accessUnit->data() + dstOffset, "\x00\x00\x00\x01", 4);

                memcpy(accessUnit->data() + dstOffset + 4,
                    mBuffer->data() + pos.nalOffset,
                    pos.nalSize);

                dstOffset += pos.nalSize + 4;
            }

            ALOGV("accessUnit contains nal types %s", out.c_str());

            const NALPosition &pos = nals.itemAt(nals.size() - 1);
            size_t nextScan = pos.nalOffset + pos.nalSize;

            memmove(mBuffer->data(),
                mBuffer->data() + nextScan,
                mBuffer->size() - nextScan);

            mBuffer->setRange(0, mBuffer->size() - nextScan);

            int64_t timeUs = fetchTimestamp(nextScan);
            CHECK_GE(timeUs, 0ll);

            accessUnit->meta()->setInt64("timeUs", timeUs);

            if (mFormat == NULL) {
                mFormat = MakeAVCCodecSpecificData(accessUnit);
            }

            return accessUnit;
        }

        NALPosition pos;
        pos.nalOffset = nalStart - mBuffer->data();
        pos.nalSize = nalSize;

        nals.push(pos);

        totalSize += nalSize;
    }
    CHECK_EQ(err, (status_t)-EAGAIN);

    return NULL;
}
#endif

#ifndef ANDROID_DEFAULT_CODE
static const uint32_t kMP3HeaderMask = 0xfffe0c00;//0xfffe0cc0 add by zhihui zhang no consider channel mode
const size_t kMaxBytesChecked = 128 * 1024;
static int mp3HeaderStartAt(const uint8_t *start, int length, int header) {
    uint32_t code = 0;
    int i = 0;

    for(i=0; i<length; i++){
        code = (code<<8) + start[i];
        if ((code & kMP3HeaderMask) == (header & kMP3HeaderMask)) {
            // some files has no seq start code
            return i - 3;
        }
    }

    return -1;
}
status_t findMP3Header(const uint8_t* buf, ssize_t size, ssize_t *offset, int *pHeader) {
    uint32_t header1 = 0, header2 = 0;
    size_t frameSize = 0, frameSize2 = 0;
    bool retb = false;
    //header1 = U32_AT(buf+*offset);
    while (*offset+4 < size) {
        //bool retb = GetMPEGAudioFrameSize(header1, &frameSize,NULL,NULL,NULL,NULL); 
        //if(!retb)
        {
            //find 1st header and verify	
            for (size_t i = *offset; i < size - 4; i++) {
                if (IsSeeminglyValidMPEGAudioHeader(&buf[i], size-i)) {
                    *offset = i;
                    header1 = U32_AT(buf+*offset);
                    retb = GetMPEGAudioFrameSize(header1, &frameSize,NULL,NULL,NULL,NULL); 
                    if(!retb || (frameSize == 0))
                    {
                        //ALOGI("1.%s err 0x%x, ofst/retb/fSz=%d/%d/%d\n", __FUNCTION__, header1, *offset, retb, frameSize);
                        continue;
                    }
                    else
                    {
                        //ALOGI("2.%s 0x%x, ofst/retb/fSz=%d/%d/%d\n", __FUNCTION__, header1, *offset, retb, frameSize);
                        break;
                    }
                }
            }
            if(!retb || (frameSize == 0)){
                break;
            }
        }
        //find 2nd header and verify
        if (*offset+frameSize < size)
        {
            *offset += frameSize;
            header2 = U32_AT(buf+*offset);
            if ((header2 & kMP3HeaderMask) == (header1 & kMP3HeaderMask)) {
                *pHeader = header1;
                return OK;
            }
            else if(GetMPEGAudioFrameSize(header2, &frameSize2,NULL,NULL,NULL,NULL) && (frameSize2 > 0)){
                header1 = header2;
                //ALOGI("3.%s 2nd 0x%x, ofst/fSz/Sz %d/%d/%d\n", __FUNCTION__, header2, *offset, frameSize2, size);	
            }
            else //header1's frameSize has problem, re-find header1
            {
                *offset -= (frameSize - 1);
                //ALOGI("4.%s 2nd err 0x%x, new ofst/fSz/sz %d/%d/%d\n", __FUNCTION__, header2, *offset, frameSize2, size);	
            }
        }
        else {
            ALOGI("frame overflow buffer");
            break;
        }
    }
    ALOGI("%s():size:%d,Not found MP3Headr,buf:%2x %2x %2x %2x %2x %2x %2x %2x",__FUNCTION__, size,
        buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
    return UNKNOWN_ERROR;
}

#if defined(MTK_OGM_PLAYBACK_SUPPORT) //for tablet only
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitMPEGAudioOgm() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    if (size < 4) {
        return NULL;
    }
    uint32_t header = U32_AT(data);

    size_t frameSize;
    int samplingRate, numChannels, bitrate, numSamples;
#ifndef ANDROID_DEFAULT_CODE
    ssize_t offset = 0;
    // when mMP3Header is not set, set it
    if (mMP3Header == 0) {
        if (findMP3Header(data, size, &offset, &mMP3Header) != OK){
            if(size > kMaxBytesChecked) {
                ALOGE("findMP3Header fail size>%d, skip this buffer",kMaxBytesChecked);
                mBuffer->setRange(0, 0);
                return NULL;
            }
            else {
                ALOGW("Not get mMP3Header, size:%d", size);
                return NULL;
            }
        }
        else {
            ALOGI("mMP3Header:%x", mMP3Header);
        }
    }
    // mMP3Header is set, check header
    int start = mp3HeaderStartAt(data+offset, size-offset, mMP3Header);
    if (start>= 0 && start+offset+3<size) {
        offset += start;
        header = U32_AT(data+offset);
        ALOGI("header:%x",header);
        bool retb = GetMPEGAudioFrameSize(header, &frameSize, &samplingRate,
            &numChannels, &bitrate, &numSamples);
        if (!retb){
            ALOGI("GetMPEGAudioFrameSize fail, skip this buffer");
            mBuffer->setRange(0, 0);
            return NULL;
        }
        size -= offset;
    }
    else {
        ALOGI("not found mMP3Header,skip");
        mBuffer->setRange(0, 0);
        return NULL;
    }
#else
    CHECK(GetMPEGAudioFrameSize(
            header, &frameSize, &samplingRate, &numChannels,
            &bitrate, &numSamples));
#endif
    ALOGI("size = %d, frameSize = %d\n", size, frameSize);
    if (size < frameSize) {
        return NULL;
    }

    unsigned layer = 4 - ((header >> 17) & 3);
    ALOGI("layer = %d\n", layer);

    sp<ABuffer> accessUnit = new ABuffer(frameSize);
#ifndef ANDROID_DEFAULT_CODE
    memcpy(accessUnit->data(), data+offset, frameSize);
    memmove(mBuffer->data(), mBuffer->data() + offset + frameSize, mBuffer->size() - offset - frameSize);

    mBuffer->setRange(0, mBuffer->size() - offset - frameSize);

    int64_t timeUs = fetchTimestamp(offset+frameSize);
#else
    memcpy(accessUnit->data(), data, frameSize);

    memmove(mBuffer->data(),
        mBuffer->data() + frameSize,
        mBuffer->size() - frameSize);

    mBuffer->setRange(0, mBuffer->size() - frameSize);

    int64_t timeUs = fetchTimestamp(frameSize);
#endif
    CHECK_GE(timeUs, 0ll);

    accessUnit->meta()->setInt64("timeUs", timeUs);

    if (mFormat == NULL) {
        mFormat = new MetaData;

        switch (layer) {
        case 1:
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_I);
            break;
        case 2:
#ifndef ANDROID_DEFAULT_CODE
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
#else
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_II);
#endif
            break;
        case 3:
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
            break;
        default:
            TRESPASS();
        }

        mFormat->setInt32(kKeySampleRate, samplingRate);
        mFormat->setInt32(kKeyChannelCount, numChannels);
        mFormat->setInt32(kKeyMPEGAudLayer, (int32_t)layer);
    }
    return accessUnit;
}
#endif //OGM
#endif
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitMPEGAudio() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    if (size < 4) {
        return NULL;
    }

    uint32_t header = U32_AT(data);

    size_t frameSize;
    int samplingRate, numChannels, bitrate, numSamples;
#ifndef ANDROID_DEFAULT_CODE
    ssize_t offset = 0;
    // when mMP3Header is not set, set it
    if (mMP3Header == 0) {
        if (findMP3Header(data, size, &offset, &mMP3Header) != OK){
            if(size > kMaxBytesChecked) {
                ALOGE("findMP3Header fail size>%d, skip this buffer",kMaxBytesChecked);
                mBuffer->setRange(0, 0);
                return NULL;
            }
            else {
                ALOGW("Not get mMP3Header, size:%d", size);
                return NULL;
            }
        }
        else {
            ALOGI("mMP3Header:%x", mMP3Header);
        }
    }
    // mMP3Header is set, check header
    int start = mp3HeaderStartAt(data+offset, size-offset, mMP3Header);
    if (start>= 0 && start+offset+3<size) {
        offset += start;
        header = U32_AT(data+offset);
        ALOGV("header:%x",header);
        bool retb = GetMPEGAudioFrameSize(header, &frameSize, &samplingRate,
            &numChannels, &bitrate, &numSamples);
        if (!retb){
            ALOGE("GetMPEGAudioFrameSize fail, skip this buffer");
            mBuffer->setRange(0, 0);
            return NULL;
        }
        size -= offset;
    }
    else {
        ALOGW("not found mMP3Header,skip");
        mBuffer->setRange(0, 0);
        return NULL;
    }
#else
    CHECK(GetMPEGAudioFrameSize(
            header, &frameSize, &samplingRate, &numChannels,
            &bitrate, &numSamples));
#endif

    if (size < frameSize) {
        return NULL;
    }

    unsigned layer = 4 - ((header >> 17) & 3);
    ALOGI("layer = %d\n", layer);

    sp<ABuffer> accessUnit = new ABuffer(frameSize);
#ifndef ANDROID_DEFAULT_CODE
    memcpy(accessUnit->data(), data+offset, frameSize);
    memmove(mBuffer->data(), mBuffer->data() + offset + frameSize, mBuffer->size() - offset - frameSize);

    mBuffer->setRange(0, mBuffer->size() - offset - frameSize);

    int64_t timeUs = fetchTimestamp(offset+frameSize);
#else
    memcpy(accessUnit->data(), data, frameSize);

    memmove(mBuffer->data(),
        mBuffer->data() + frameSize,
        mBuffer->size() - frameSize);

    mBuffer->setRange(0, mBuffer->size() - frameSize);

    int64_t timeUs = fetchTimestamp(frameSize);
#endif
    CHECK_GE(timeUs, 0ll);

    accessUnit->meta()->setInt64("timeUs", timeUs);

    if (mFormat == NULL) {
        mFormat = new MetaData;

        switch (layer) {
        case 1:
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_I);
            break;
        case 2:
#ifndef ANDROID_DEFAULT_CODE
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
#else                    
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_II);
#endif
            break;
        case 3:
            mFormat->setCString(
                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
            break;
        default:
            TRESPASS();
        }

        mFormat->setInt32(kKeySampleRate, samplingRate);
        mFormat->setInt32(kKeyChannelCount, numChannels);
#ifndef ANDROID_DEFAULT_CODE		
#ifdef MTK_OGM_PLAYBACK_SUPPORT
        mFormat->setInt32(kKeyMPEGAudLayer, (int32_t)layer);
#endif
#endif
    }

    return accessUnit;
}

static void EncodeSize14(uint8_t **_ptr, size_t size) {
    CHECK_LE(size, 0x3fff);

    uint8_t *ptr = *_ptr;

    *ptr++ = 0x80 | (size >> 7);
    *ptr++ = size & 0x7f;

    *_ptr = ptr;
}

static sp<ABuffer> MakeMPEGVideoESDS(const sp<ABuffer> &csd) {
    sp<ABuffer> esds = new ABuffer(csd->size() + 25);

    uint8_t *ptr = esds->data();
    *ptr++ = 0x03;
    EncodeSize14(&ptr, 22 + csd->size());

    *ptr++ = 0x00;  // ES_ID
    *ptr++ = 0x00;

    *ptr++ = 0x00;  // streamDependenceFlag, URL_Flag, OCRstreamFlag

    *ptr++ = 0x04;
    EncodeSize14(&ptr, 16 + csd->size());

    *ptr++ = 0x40;  // Audio ISO/IEC 14496-3

    for (size_t i = 0; i < 12; ++i) {
        *ptr++ = 0x00;
    }

    *ptr++ = 0x05;
    EncodeSize14(&ptr, csd->size());

    memcpy(ptr, csd->data(), csd->size());

    return esds;
}

#ifndef ANDROID_DEFAULT_CODE
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitPSLPCM() {
    /*	http://www.mpucoder.com/DVD/ass-hdr.html
        10100DDD|NNNNNNNN|LLLLLLLL|LLLLLLLL|E M R NNNNN|BB SS R CCC|XXX YYYYY
        10000DDD(8bits) : sub stream ID 0xA0-A7 (DDD: id)
        NNNNNNNN(8bits) : number of frame headers
        LLL...(16bits) : first access unit pointer
E : audio emphasis flag (0:off, 1:on)
M : audio mute flag (0:off, 1:on)
R : reserved
NNNNN(5bits) : audio frame number
BB(2bits) : Bits per sample[bits] (0:16, 1:20, 2:24, 3:reserved)
SS(2bits) : Sampling frequency[kHz] (0:48, 1:96, etc:reserved)
CCC(3bits) : number of channels[ch] (CCC + 1)
XXX(3bits) : dynamic range X (linear gain = 2^(4-(X+(Y/30))))
YYYYY(5bits) : dynamic range Y (dB gain = 24.082 - 6.0206 X - 0.2007 Y)
*/
    uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();
    size_t offset = 0;	
    size_t frameSize;	
    int samplingRate, numChannels, bitrate, numSamples;

    if(mBuffer->size()< 7){
        return NULL;
    }
    ABitReader bits(mBuffer->data() + offset, mBuffer->size() - offset);		
    unsigned streamID = bits.getBits(8);		
    while( streamID < 0xa0 || streamID > 0xa7 ){
        streamID = bits.getBits(8);
        offset++;
    }
    /*unsigned frame_header_count=*/bits.getBits(8);
    unsigned first_access_unit_pointer=bits.getBits(16);
    //offset = offset  + 3 + first_access_unit_pointer;
    /*bool audio_emphasis_flag=*/bits.getBits(1);
    /*bool audio_mute_flag=*/bits.getBits(1);
    bits.getBits(1);
    unsigned audio_frame_num = bits.getBits(5);
    unsigned bits_per_sample=bits.getBits(2);
    if(bits_per_sample == 0){
        numSamples = 16;
    }
    else if(bits_per_sample == 1){
        numSamples = 20;
    }	
    else{
        numSamples = 24;
    }
    unsigned sampling_frequency= bits.getBits(2);
    if(sampling_frequency == 0){
        samplingRate = 48000;
    }
    else{
        samplingRate = 96000;
    }
    bits.getBits(1);
    numChannels = bits.getBits(3) + 1;
    /*unsigned dynamic_range_X=*/bits.getBits(3);
    /*unsigned dynamic_range_Y=*/bits.getBits(5);
    uint16_t framesize;
    //framesize = samplingRate * numSamples * numChannels * audio_frame_num / 8;
    //if (framesize > size) {
    if ((size-7)%2 != 0) {
        framesize = (size-7) - 1;
    }
    else {
        framesize = (size-7);
    }
    //}
    ALOGD("Warning: framesize:%d size:%d", framesize, size);

    sp<ABuffer> accessUnit = new ABuffer(framesize);
    uint8_t *ptr = accessUnit->data();

#ifdef MTK_AUDIO_RAW_SUPPORT
    memcpy(ptr, data+7, framesize);
#else
    // 16 bit big endian to little endian 
    int j = 0;
    for (int i=0; i < (framesize/2); i++) {
        j = 2*i; 
        ptr[j] = data[j+8];
        ptr[j+1] = data[j+7]; 
    }
#endif

    mBuffer->setRange(0, 0);

    int64_t timeUs = fetchTimestamp(size);
    ALOGD("PCM DEQUEUE timeUs=%d framesize is %d buffer size is %d size is %d offset is %d",timeUs,framesize,mBuffer->size(),size,offset);
    accessUnit->meta()->setInt64("timeUs", timeUs);

    if(numSamples!= 16 
#ifdef MTK_AUDIO_RAW_SUPPORT		
        && numSamples!= 24
#endif
      )
    {
        ALOGD("PCM BitWidth %d not support\n", numSamples);
        return accessUnit;
    }
    else if (mFormat == NULL) {
        mFormat = new MetaData;

        mFormat->setCString(
            kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);

        mFormat->setInt32(kKeySampleRate, samplingRate);
        mFormat->setInt32(kKeyChannelCount, numChannels);
#ifdef MTK_AUDIO_RAW_SUPPORT
        mFormat->setInt32(kKeyEndian, 1);	//1: big endian, 2: little endia
        mFormat->setInt32(kKeyBitWidth, numSamples);
        mFormat->setInt32(kKeyPCMType, 3);	//1: WAV file, 2: BD file, 3: DVD_VOB file, 4: DVD_AOB file
        mFormat->setInt32(kKeyChannelAssignment, 0x1b);	// 2 channels
        ALOGD("PCM SampleRate %d, ChannelCount %d, Big endian, BitWidth %d, PCMType:DVD_VOB\n", 
            samplingRate, numChannels, numSamples);

#endif
    }

    return accessUnit;
}
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitLPCM() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();
    const int64_t framesize = 1920;

    if (size < 4) {
        return NULL;
    }

    PCM_Header *pcmHeader = (PCM_Header *)data;
    if (pcmHeader->sub_stream_id != 0xA0 || pcmHeader->number_of_frame_header != 0x6) {
        ALOGE("pcmHeader incorrent, subid:%d, numHeader:%d", pcmHeader->sub_stream_id, pcmHeader->number_of_frame_header);
        mBuffer->setRange(0, 0);
        return NULL;
    }

    sp<ABuffer> accessUnit = new ABuffer(framesize);
    uint8_t *ptr = accessUnit->data();
#if 1
    int j = 0;
    for (int i=0; i < (framesize/2); i++) {
        j = 2*i; 
        ptr[j] = data[4+j+1];
        ptr[j+1] = data[4+j]; 
    }
#else
    memcpy(ptr, data+4, framesize);
#endif
    mBuffer->setRange(0, 0);

    int64_t timeUs = fetchTimestamp(size);
    CHECK_GE(timeUs, 0ll);
    accessUnit->meta()->setInt64("timeUs", timeUs);
    ALOGV("PCM: size:%d, timeUs:%lld", size, timeUs);

    if (mFormat == NULL) {
        mFormat = new MetaData;

        mFormat->setCString(
            kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);
        if (pcmHeader->audio_sampling_frequency == 1) {
            ALOGW("SampleRate 44100");
            mFormat->setInt32(kKeySampleRate, 44100);
        }
        else if (pcmHeader->audio_sampling_frequency == 2) {
            ALOGW("SampleRate 48000");
            mFormat->setInt32(kKeySampleRate, 48000);
        }
        else {
            mFormat.clear();
            ALOGW("SampleRate is uncorrect");
        }

        if (((pcmHeader->number_of_audio_channel != 1) && 
                (pcmHeader->number_of_audio_channel == 0)) ||
            pcmHeader-> quantization_word_length !=0){ 
            mFormat.clear();
            ALOGW("channel is uncorrect");
        }
        else {
            mFormat->setInt32(kKeyChannelCount, 2);
        }
    }

    return accessUnit;
}
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitBDLPCM() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    if (size < 4) {
        return NULL;
    }

    uint16_t framesize = U16_AT(data);
    data += 4;
    size -= 4;
    ALOGI("framesize:%d size:%d", framesize, size);
    if (framesize > size) {
        ALOGI("Warning: framesize:%d size:%d", framesize, size);
        if (size%2 != 0) {
            framesize = size - 1;
        }
        else {
            framesize = size;
        }
    }

    sp<ABuffer> accessUnit = new ABuffer(framesize);
    uint8_t *ptr = accessUnit->data();

    int j = 0;
    for (int i=0; i < (framesize/2); i++) {
        j = 2*i; 
        ptr[j] = data[j+1];
        ptr[j+1] = data[j]; 
    }


    mBuffer->setRange(0, 0);

    int64_t timeUs = fetchTimestamp(size+4);
    CHECK_GE(timeUs, 0ll);
    accessUnit->meta()->setInt64("timeUs", timeUs);

    if (mFormat == NULL) {
        mFormat = new MetaData;

        mFormat->setCString(
            kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);

        mFormat->setInt32(kKeySampleRate, 48000);
        mFormat->setInt32(kKeyChannelCount, 2);
    }

    return accessUnit;
}

static bool isPictureHeader(int startCode) {
    if ((startCode == 0xb3) ||
        (startCode == 0xb6) ||
        (startCode == 0xb7)) {
        return true;
    }
    return false;
}

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitAVSVideo() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    int  prevStartCode    = -1;
    int  currentStartCode = -1;

    size_t offset = 0;
    while (offset + 3 < size) {
        if (memcmp(&data[offset], "\x00\x00\x01", 3)) {
            ++offset;
            continue;
        }

        prevStartCode  = currentStartCode;

        if (isPictureHeader(data[offset + 3])
            || data[offset + 3] == 0xb0) {
            currentStartCode = data[offset + 3];
        } else {
            ++ offset;
            continue;
        }

        /*video sequence start, video_edit_code*/
        if (prevStartCode == -1 && currentStartCode == 0xb0) {
            memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
            size -= offset;
            (void)fetchTimestamp(offset);
            mBuffer->setRange(0, size);
            offset = 0;
        }

        /* parse sequence_header */
        if (prevStartCode == 0xb0 && isPictureHeader(currentStartCode)) {

            if (mFormat == NULL) {

                mFormat = new MetaData;
                ABitReader bits(data, offset);

                bits.skipBits(32);//0x000001b0
                uint8_t profile_id =  bits.getBits(8);
                ALOGD("dequeueAccessUnitAVSVideo:   profile_id is:%u", profile_id);

                uint8_t level_id =  bits.getBits(8);
                ALOGD("dequeueAccessUnitAVSVideo:   level_id is:%u", level_id);

                bits.skipBits(1);
                /*progressive_sequence*/

                uint32_t horizontal_size =  bits.getBits(14);
                ALOGD("dequeueAccessUnitAVSVideo:   horizontal:%u", horizontal_size);

                uint32_t vertical_size =  bits.getBits(14);
                ALOGD("dequeueAccessUnitAVSVideo:   vertical:%u", vertical_size);

                sp<ABuffer> csd = new ABuffer(offset);
                memcpy(csd->data(), data, offset);

                sp<ABuffer> esds = MakeMPEGVideoESDS(csd);

                mFormat->setData(kKeyESDS, kTypeESDS, esds->data(), esds->size());
                mFormat->setCString(kKeyMIMEType, "video/avs");
                mFormat->setInt32(kKeyWidth, horizontal_size);
                mFormat->setInt32(kKeyHeight, vertical_size);
            }

            memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
            size -= offset;
            (void)fetchTimestamp(offset);
            mBuffer->setRange(0, mBuffer->size() - offset);
            offset = 0;
        }

        if (isPictureHeader(prevStartCode)
            && (isPictureHeader(currentStartCode) || currentStartCode ==0xb0 )) {

            sp<ABuffer> accessUnit = new ABuffer(offset);
            memcpy(accessUnit->data(), data, offset);

            memmove(mBuffer->data(),
                mBuffer->data() + offset,
                mBuffer->size() - offset);

            mBuffer->setRange(0, mBuffer->size() - offset);

            int64_t timeUs = fetchTimestamp(offset);
            CHECK_GE(timeUs, 0ll);
            offset = 0;
            accessUnit->meta()->setInt64("timeUs", timeUs);

            return accessUnit;
        }

        ++offset;
    }/*while*/

    return NULL;
}
#endif  // ANDROID_DEFAULT_CODE



sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitMPEGVideo() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    bool sawPictureStart = false;
    int pprevStartCode = -1;
    int prevStartCode = -1;
    int currentStartCode = -1;

    size_t offset = 0;
#ifndef ANDROID_DEFAULT_CODE 
#ifdef MTK_OGM_PLAYBACK_SUPPORT //for tablet only

    int prev_sawPictureStart = 0;

    //Search start code optimiztion
    //mfgSearchStartCodeOptimize == true: memorize last scanned buffer potition, start to search at last stop position 
    //mfgSearchStartCodeOptimize == false: always start to search at the beginning of mbuffer queue 
    int32_t prev_offset = 0;
    if(!mSeeking)
    {
        if(mBuffer->meta()->findInt32("LPos", &prev_offset))
        {
            offset = (mfgSearchStartCodeOptimize && (mFormat != NULL) && (prev_offset > 4))?(prev_offset - 4): 0;
            if((offset>0) && mBuffer->meta()->findInt32("PicS", &prev_sawPictureStart))
            {
                sawPictureStart = (mfgSearchStartCodeOptimize && (mFormat != NULL))?(prev_sawPictureStart>0):false;
            }
        }
        ALOGV("offset %d/sawPictureStart %d\n", offset, sawPictureStart);
    }
#endif

    size_t lastGOPOff= -1;
#endif

    while (offset + 3 < size) {
        if (memcmp(&data[offset], "\x00\x00\x01", 3)) {
            ++offset;
            continue;
        }

        pprevStartCode = prevStartCode;
        prevStartCode = currentStartCode;
        currentStartCode = data[offset + 3];

        if (currentStartCode == 0xb3 && mFormat == NULL) {
            memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
            size -= offset;
            (void)fetchTimestamp(offset);
            offset = 0;
            mBuffer->setRange(0, size);
        }

        if ((prevStartCode == 0xb3 && currentStartCode != 0xb5)
            || (pprevStartCode == 0xb3 && prevStartCode == 0xb5)) {
            // seqHeader without/with extension

            if (mFormat == NULL) {
                CHECK_GE(size, 7u);

                unsigned width =
                    (data[4] << 4) | data[5] >> 4;

                unsigned height =
                    ((data[5] & 0x0f) << 8) | data[6];

                mFormat = new MetaData;
                mFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG2);
                mFormat->setInt32(kKeyWidth, width);
                mFormat->setInt32(kKeyHeight, height);

                ALOGI("found MPEG2 video codec config (%d x %d)", width, height);

                sp<ABuffer> csd = new ABuffer(offset);
                memcpy(csd->data(), data, offset);

                memmove(mBuffer->data(),
                    mBuffer->data() + offset,
                    mBuffer->size() - offset);

                mBuffer->setRange(0, mBuffer->size() - offset);
                size -= offset;
                (void)fetchTimestamp(offset);
                offset = 0;

                // hexdump(csd->data(), csd->size());

                sp<ABuffer> esds = MakeMPEGVideoESDS(csd);
                mFormat->setData(
                    kKeyESDS, kTypeESDS, esds->data(), esds->size());

                return NULL;
            }
        }
#ifndef ANDROID_DEFAULT_CODE 
        // save the GOP, send to decode when seek. if Format is NULL, lastGOPOff make no sense
        if (mSeeking && currentStartCode == 0xB8 && mFormat != NULL) {
            lastGOPOff = offset; 
        }
#endif
        if (mFormat != NULL && currentStartCode == 0x00) {
#ifndef ANDROID_DEFAULT_CODE 
            if (mSeeking) { 
                if (((data[offset+5]>>3) & 0x7) == 1) {          // I frame
                    mSeeking = false;

                    size_t tmpOff = offset;
                    if (lastGOPOff != -1) {
                        tmpOff = lastGOPOff;
                        ALOGI("Send GOP when seeking, offset:%x lastGOPOff:%x", offset, lastGOPOff);
                    }
                    memmove(mBuffer->data(), mBuffer->data() + tmpOff, size - tmpOff);
                    size -= tmpOff;
                    (void)fetchTimestamp(tmpOff);
                    offset = offset - tmpOff;
                    mBuffer->setRange(0, size);
                    ALOGI("Found I Frame when seeking");
                    mfgFirstFrmAfterSeek = true;
                }
                else {
                    offset++;
                    continue;
                }
            }
#endif
            // Picture start

            if (!sawPictureStart) {
                sawPictureStart = true;
            } else {
                sp<ABuffer> accessUnit = new ABuffer(offset);
                memcpy(accessUnit->data(), data, offset);

                memmove(mBuffer->data(),
                    mBuffer->data() + offset,
                    mBuffer->size() - offset);

                mBuffer->setRange(0, mBuffer->size() - offset);

                int64_t timeUs = fetchTimestamp(offset);
                CHECK_GE(timeUs, 0ll);

                offset = 0;
#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_OGM_PLAYBACK_SUPPORT)  
                if(mfgSearchStartCodeOptimize)
                {
                    mBuffer->meta()->setInt32("LPos", 0);
                    mBuffer->meta()->setInt32("PicS", 0);
                }
#endif

                accessUnit->meta()->setInt64("timeUs", timeUs);
#ifndef ANDROID_DEFAULT_CODE
#if 0
                //accessUnit->meta()->setInt32("invt", (int32_t)fgInvalidTimeUs);
                ALOGV("AU time %lld/%d,FrmType %d,%x %x %x %x %x %x,sz %d\n", timeUs, fgInvalidTimeUs, (*(accessUnit->data()+5)>>3) & 0x7,
                    *(accessUnit->data()+0), *(accessUnit->data()+1), *(accessUnit->data()+2), *(accessUnit->data()+3),
                    *(accessUnit->data()+4), *(accessUnit->data()+5),
                    accessUnit->size());
                if(mfgFirstFrmAfterSeek)
                {
                    if(fgInvalidTimeUs)
                    {
                        ALOGI("Keep search I with valid PTS\n");
                        mSeeking = true;
                        lastGOPOff = -1;
                        size = mBuffer->size();
                        sawPictureStart = false;
                        continue;
                    }
                    else
                    {
                        mfgFirstFrmAfterSeek = false;
                    }
                }
#endif
#endif
                ALOGV("returning MPEG video access unit at time %lld us",
                    timeUs);

                // hexdump(accessUnit->data(), accessUnit->size());

                return accessUnit;
            }
        }

        ++offset;
    }

#ifndef ANDROID_DEFAULT_CODE
    if (mSeeking) {                         // discard buffer
        (void)fetchTimestamp(offset);
        memmove(mBuffer->data(),
            mBuffer->data() + offset,
            mBuffer->size() - offset);

        mBuffer->setRange(0, mBuffer->size() - offset);
        offset = 0;
    }
#endif
#if !defined(ANDROID_DEFAULT_CODE) && defined(MTK_OGM_PLAYBACK_SUPPORT)  
    if(mfgSearchStartCodeOptimize)
    {
        mBuffer->meta()->setInt32("LPos", ((lastGOPOff != -1) && (!mfgFirstFrmAfterSeek) && (offset > 0))?(int)lastGOPOff:(int)offset);
        mBuffer->meta()->setInt32("PicS", (((lastGOPOff != -1) && (!mfgFirstFrmAfterSeek)) || (offset == 0))?0:(int)sawPictureStart);
        ALOGD("Saved Pos/PicS=0x%x/%d\n", ((lastGOPOff != -1) && (!mfgFirstFrmAfterSeek))?(int)lastGOPOff:(int)offset,
            (((lastGOPOff != -1) && (!mfgFirstFrmAfterSeek)) || (offset == 0))?0:(int)sawPictureStart);
    }
#endif
    return NULL;
}

static ssize_t getNextChunkSize(
    const uint8_t *data, size_t size) {
    static const char kStartCode[] = "\x00\x00\x01";

    if (size < 3) {
        return -EAGAIN;
    }

    if (memcmp(kStartCode, data, 3)) {
        TRESPASS();
    }

    size_t offset = 3;
    while (offset + 2 < size) {
        if (!memcmp(&data[offset], kStartCode, 3)) {
            return offset;
        }

        ++offset;
    }

    return -EAGAIN;
}

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitMPEG4Video() {
    uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    enum {
        SKIP_TO_VISUAL_OBJECT_SEQ_START,
        EXPECT_VISUAL_OBJECT_START,
        EXPECT_VO_START,
        EXPECT_VOL_START,
        WAIT_FOR_VOP_START,
        SKIP_TO_VOP_START,
#ifndef ANDROID_DEFAULT_CODE 
        WAIT_FOR_VOL_START,
#endif
    } state;

    if (mFormat == NULL) {
#ifndef ANDROID_DEFAULT_CODE
        state = WAIT_FOR_VOL_START;
#else
        state = SKIP_TO_VISUAL_OBJECT_SEQ_START;
#endif
    } else {
        state = SKIP_TO_VOP_START;
    }

    int32_t width = -1, height = -1;

    size_t offset = 0;
    ssize_t chunkSize;
    while ((chunkSize = getNextChunkSize(
                &data[offset], size - offset)) > 0) {
        bool discard = false;

        unsigned chunkType = data[offset + 3];

        switch (state) {
        case SKIP_TO_VISUAL_OBJECT_SEQ_START:
            {
                if (chunkType == 0xb0) {
                    // Discard anything before this marker.

                    state = EXPECT_VISUAL_OBJECT_START;
                } else {
#ifndef ANDROID_DEFAULT_CODE
                    offset += chunkSize;
#endif
                    discard = true;
                }
                break;
            }

        case EXPECT_VISUAL_OBJECT_START:
            {
                CHECK_EQ(chunkType, 0xb5);
                state = EXPECT_VO_START;
                break;
            }

        case EXPECT_VO_START:
            {
                CHECK_LE(chunkType, 0x1f);
                state = EXPECT_VOL_START;
                break;
            }

        case EXPECT_VOL_START:
            {
                CHECK((chunkType & 0xf0) == 0x20);

                CHECK(ExtractDimensionsFromVOLHeader(
                        &data[offset], chunkSize,
                        &width, &height));

                state = WAIT_FOR_VOP_START;
                break;
            }
#ifndef ANDROID_DEFAULT_CODE
        case WAIT_FOR_VOL_START:
            {
                if((chunkType & 0xf0) == 0x20)
                {
                    CHECK(ExtractDimensionsFromVOLHeader(
                            &data[offset], chunkSize,
                            &width, &height));

                    state = WAIT_FOR_VOP_START;
                    break;
                }
                else
                {
                    offset += chunkSize;
                    discard = true;
                }
                break;
            }
#endif

        case WAIT_FOR_VOP_START:
            {
                if (chunkType == 0xb3 || chunkType == 0xb6) {
                    // group of VOP or VOP start.

                    mFormat = new MetaData;
                    mFormat->setCString(
                        kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG4);

                    mFormat->setInt32(kKeyWidth, width);
                    mFormat->setInt32(kKeyHeight, height);

                    ALOGI("found MPEG4 video codec config (%d x %d)",
                        width, height);

                    sp<ABuffer> csd = new ABuffer(offset);
                    memcpy(csd->data(), data, offset);

                    // hexdump(csd->data(), csd->size());

                    sp<ABuffer> esds = MakeMPEGVideoESDS(csd);
                    mFormat->setData(
                        kKeyESDS, kTypeESDS,
                        esds->data(), esds->size());

                    discard = true;
                    state = SKIP_TO_VOP_START;
                }

                break;
            }

        case SKIP_TO_VOP_START:
            {
                if (chunkType == 0xb6) {
                    offset += chunkSize;
#ifndef ANDROID_DEFAULT_CODE
                    if (mSeeking) {
                        switch (data[4] & 0xC0) {
                        case 0x00:
                            mSeeking = false;
                            ALOGI("I frame");
                            break;
                        case 0x40:
                            ALOGI("P frame");
                            break;
                            //continue;
                        case 0x80:
                            ALOGI("B frame");
                            break;
                            //continue;
                        default:
                            ALOGI("default");
                            break;
                            //continue;
                        }
                    }

                    if(mSeeking)
                    {
                        discard = true;
                        break;
                    }
#endif
                    sp<ABuffer> accessUnit = new ABuffer(offset);
                    memcpy(accessUnit->data(), data, offset);

                    memmove(data, &data[offset], size - offset);
                    size -= offset;
                    mBuffer->setRange(0, size);

                    int64_t timeUs = fetchTimestamp(offset);
#ifndef ANDROID_DEFAULT_CODE
                    if(timeUs < 0)
                        timeUs = 0;
#else
                    CHECK_GE(timeUs, 0ll);
#endif

                    offset = 0;

                    accessUnit->meta()->setInt64("timeUs", timeUs);

                    ALOGV("returning MPEG4 video access unit at time %lld us",
                        timeUs);

                    // hexdump(accessUnit->data(), accessUnit->size());

                    return accessUnit;
                } else if (chunkType != 0xb3) {
                    offset += chunkSize;
                    discard = true;
                }

                break;
            }

        default:
            TRESPASS();
        }

        if (discard) {
            (void)fetchTimestamp(offset);
            memmove(data, &data[offset], size - offset);
            size -= offset;
            offset = 0;
            mBuffer->setRange(0, size);
        } else {
            offset += chunkSize;
        }
    }

    return NULL;
}

#ifndef ANDROID_DEFAULT_CODE
//=========================================================================================================//
//Vorbis Header Parsing Funcitons
//=========================================================================================================//
status_t vorbis_parse_infoheader(const uint8_t *data, uint8_t *ChannelCount, uint32_t *SampleRate, uint32_t *Bitrate)
{
    uint8_t vorbisAudioChannels;
    uint32_t vorbisSampleRate;
    uint32_t vorbisBitrateMax;
    uint32_t vorbisBitrateMin;
    uint32_t vorbisBitrateNominal;
    data += 7;	// Skip Type and vorbis syncword
    data += 4;	// Skip Vorbis Version
    vorbisAudioChannels = data[0];
    data += 1;
    vorbisSampleRate = U32LE_AT(data);
    data += 4;
    vorbisBitrateMax = U32LE_AT(data);
    data += 4;
    vorbisBitrateNominal = U32LE_AT(data);
    data += 4;
    vorbisBitrateMin = U32LE_AT(data);
    data += 4;
    data += 2;	// useless size field  

    *ChannelCount = vorbisAudioChannels;
    *SampleRate = vorbisSampleRate;
    *Bitrate = (vorbisBitrateNominal == 0)?(vorbisBitrateMax + vorbisBitrateMin)/2 : vorbisBitrateNominal;
    return OK;
}

sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitVORBISAudio()
{
    uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();
    sp<ABuffer> accessUnit = NULL;

    if(size == 0)	return NULL;

    //In this case, all data in should be a header buffer
    if(mVorbisStatus < 5)
    {
        if(data[0] == 1 && mVorbisStatus == 0)
        {
            if(mFormat == NULL)
            {
                uint8_t ChannelCount;
                uint32_t SampleRate;
                uint32_t Bitrate;
                vorbis_parse_infoheader(data, &ChannelCount, &SampleRate, &Bitrate);
                ALOGI("Vorbis Create Formate with %d channels, %d samplerate", ChannelCount, SampleRate);
                mFormat = new MetaData;
                mFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_VORBIS);
                mFormat->setData(kKeyVorbisInfo, 0, data, size);
                mFormat->setInt32(kKeySampleRate, SampleRate);
                mFormat->setInt32(kKeyChannelCount, ChannelCount);
                mFormat->setInt32(kKeyBitRate, Bitrate);
            }
            mVorbisStatus = 2;
            mBuffer->setRange(0, 0);
            return NULL;
        }
        else if(data[0] == 3 && mVorbisStatus == 2)
        {
            ALOGI("Parsing Comment header?");
            mFormat->setData(kKeyVorbisComment, 0, data, size);
            mBuffer->setRange(0, 0);
            mVorbisStatus = 4;
            return NULL;
        }
        else if(data[0] == 5 && mVorbisStatus == 4)
        {
            ALOGI("Parsing Books header, size = %d", size);
            mFormat->setData(kKeyVorbisBooks, 0, data, size);
            mBuffer->setRange(0, 0);
            mVorbisStatus = 6;
            return NULL;
        }
        else
        {
            ALOGI("Header info not completed, drop data, size = %d", size);
            mBuffer->setRange(0, 0);
            return NULL;
        }
    }
    else
    {
        size_t checksize = size;
        while(checksize >= 0xFF)	checksize -= 0xFF;
        if(checksize)
        {
            sp<ABuffer> accessUnit = new ABuffer(size);
            memcpy(accessUnit->data(), data, size);
            mBuffer->setRange(0, 0);
            int64_t timeUs = fetchTimestamp(size);
            if( timeUs < 0)	
            {
                ALOGI("timeUs error");
                timeUs = 0;
            }
            CHECK_GE(timeUs, 0ll);
            accessUnit->meta()->setInt64("timeUs", timeUs);
            //hexdump(accessUnit->data(), accessUnit->size());
            return accessUnit;
        }
        else
            return NULL;
    }
}
#endif
#ifndef ANDROID_DEFAULT_CODE
struct asf_bitmapinfoheader_s {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    uint8_t *data;
};
#if 0     // check Frame Type
static uint32_t glace = (uint32_t)"lace";
#endif 
#define ASF_BITMAPINFOHEADER_SIZE 40
sp<ABuffer> ElementaryStreamQueue::dequeueAccessUnitVC1Video() {
    const uint8_t *data = mBuffer->data();
    size_t size = mBuffer->size();

    int  prevStartCode    = -1;
    int  currentStartCode = -1;
    int  pprevStartCode = -1;

    size_t offset = 0;
    bool sawPicture = false;
    while (offset + 3 < size) {
        if (memcmp(&data[offset], "\x00\x00\x01", 3)) {
            ++offset;
            continue;
        }
        ALOGV("%02x %02x %02x %02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3]);
        pprevStartCode = prevStartCode;
        prevStartCode = currentStartCode;
        currentStartCode = data[offset + 3];

        /*video sequence start, video_edit_code*/
        if (currentStartCode == 0x0F && mFormat == NULL) {
            if (offset) {
                memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
                size -= offset;
                (void)fetchTimestamp(offset);
                mBuffer->setRange(0, size);
                offset = 0;
            }
        }

        /* parse sequence_header */
        if ((prevStartCode == 0x0F && currentStartCode != 0x0E)
            || (pprevStartCode == 0x0F && prevStartCode == 0x0E)) {
            if (mFormat == NULL) {
                ABitReader bits(data, offset);

                bits.skipBits(32);/*start code 00 00 01 0F*/
                uint8_t profile =  bits.getBits(2);
                ALOGD("dequeueAccessUnitVC1Video:   profile is:%u", profile);
                if (profile != 3 ) {
                    return NULL;
                }

                uint8_t level =  bits.getBits(3);
                ALOGD("dequeueAccessUnitVC1Video:   level is:%u", level);

                bits.skipBits(2); /*COLORDIFF_FORMAT*/
                bits.skipBits(3); /*FRMRTQ_POSTPROC*/
                bits.skipBits(5); /*BITRTQ_POSTPROC*/
                bits.skipBits(1); /*POSTPROCFLAG*/

                uint32_t pictureWidth = (bits.getBits(12) * 2) + 2;
                ALOGD("dequeueAccessUnitVC1Video:   pictureWidth:%u", pictureWidth);

                uint32_t pictureHeight = (bits.getBits(12) * 2) + 2;
                ALOGD("dequeueAccessUnitVC1Video:   pictureHeight:%u", pictureHeight);
#if 0     // check Frame Type
                bits.skipBits(1);     /* */
                uint32_t interlace = bits.getBits(1);
#endif

                sp<ABuffer> WMVCData = new ABuffer(offset + ASF_BITMAPINFOHEADER_SIZE);
                uint8_t  codecProfile = 8;/*codec specific data*/
                struct asf_bitmapinfoheader_s header;
                memset(&header, 0, ASF_BITMAPINFOHEADER_SIZE);
                header.biSize = offset + ASF_BITMAPINFOHEADER_SIZE;
                header.biWidth = pictureWidth;
                header.biHeight = pictureHeight;
                memcpy(&(header.biCompression), "WVC1", 4);
#if 0
                memcpy(WMVCData->data(), (void *)&codecProfile, 1);
                memcpy(WMVCData->data() + 1, (void *)&pictureWidth, 2);
                memcpy(WMVCData->data() + 3, (void *)&pictureHeight, 2);
#else
                memcpy(WMVCData->data(), (void *)&header, ASF_BITMAPINFOHEADER_SIZE);
#endif

                memcpy(WMVCData->data() + ASF_BITMAPINFOHEADER_SIZE, data, offset);

                mFormat = new MetaData;
                mFormat->setData(kKeyWMVC, 0, WMVCData->data(), WMVCData->size());
                mFormat->setCString(kKeyMIMEType, "video/x-ms-wmv");
                mFormat->setInt32(kKeyWidth, pictureWidth);
                mFormat->setInt32(kKeyHeight, pictureHeight);
#if 0     // check Frame Type
                mFormat->setInt32(glace, (int32_t)interlace);
#endif

                memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
                size -= offset;
                (void)fetchTimestamp(offset);
                mBuffer->setRange(0, mBuffer->size() - offset);
                offset = 0;
                return NULL;
            }

        }
        if (currentStartCode == 0x0E && mFormat != NULL && mSeeking) {
            mSeeking = false;
            memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
            size -= offset;
            (void)fetchTimestamp(offset);
            offset = 0;
            mBuffer->setRange(0, size);
            ALOGI("Found entry header when seeking");
        }
        if (currentStartCode == 0x0D && mFormat != NULL && !mSeeking) {
            /*frame start code*/
#if 0     // check Frame Type
            int32_t lace = 0;
            ALOGV("%2x %2x %2x %2x %2x", data[0], data[1], data[2], data[3], data[4]);
            uint8_t data0 = data[0];
            if (mFormat->findInt32(glace, &lace) && lace == 1) {
                ALOGI("It is interlace");
                if (data0 & 0x80 == 0x80) {
                    data0 = data0 << 2;
                }
                else {
                    data0 = data0 << 1;
                }
            }
            if ((data[4] & 0xE0) == 0xC0) {
                ALOGI("I frame");
            }
            else if ((data[4] & 0x80) == 0x0) {
                ALOGI("P frame");
            }
            else if ((data[4] & 0xC0) == 0x80) {
                ALOGI("B frame");
            }
            else if ((data[4] & 0xF0) == 0xE0) {
                ALOGI("BI frame");
            }
            else if ((data[4] & 0xF0) == 0xF0) {
                ALOGI("Skipped frame");
            }
#endif

            if (!sawPicture) {
                sawPicture = true;
            }
            else {
                sp<ABuffer> accessUnit = new ABuffer(offset);
                memcpy(accessUnit->data(), data, offset);

                memmove(mBuffer->data(),
                    mBuffer->data() + offset,
                    mBuffer->size() - offset);

                mBuffer->setRange(0, mBuffer->size() - offset);

                int64_t timeUs = fetchTimestamp(offset);
                CHECK_GE(timeUs, 0ll);
                offset = 0;
                accessUnit->meta()->setInt64("timeUs", timeUs);

                return accessUnit;
            }
        }
#if 0 
        else if (prevStartCode == 0x0E/* && currentStartCode == 0x0D*/) {
            /*entry point header*/
            ALOGI("entry point header");
            memmove(mBuffer->data(), mBuffer->data() + offset, size - offset);
            (void)fetchTimestamp(offset);
            size -= offset;
            offset = 0;
            mBuffer->setRange(0, size);
        }
#endif

        ++offset;
    }/*while*/
    if (mSeeking) {                         // discard buffer
        (void)fetchTimestamp(offset);
        memmove(mBuffer->data(),
            mBuffer->data() + offset,
            mBuffer->size() - offset);
        size -= offset;
        mBuffer->setRange(0, mBuffer->size() - offset);
        offset = 0;
    }

    return NULL;
}
#endif
}  // namespace android
