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
#define LOG_TAG "MatroskaExtractor"
#include <utils/Log.h>

#include "MatroskaExtractor.h"

#include "mkvparser.hpp"

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/Utils.h>
#include <utils/String8.h>

//MTK Begin
#include "../include/avc_utils.h"
#include <media/stagefright/foundation/ABuffer.h>

// big endian fourcc
#define BFOURCC(c1, c2, c3, c4) \
    (c4 << 24 | c3 << 16 | c2 << 8 | c1)
//MTK END
namespace android {

#ifndef ANDROID_DEFAULT_CODE
#define MKV_RIFF_WAVE_FORMAT_PCM            (0x0001)
#define MKV_RIFF_WAVE_FORMAT_ALAW           (0x0006)
#define MKV_RIFF_WAVE_FORMAT_MULAW          (0x0007)
#define MKV_RIFF_WAVE_FORMAT_MPEGL12        (0x0050)
#define MKV_RIFF_WAVE_FORMAT_MPEGL3         (0x0055)
#define MKV_RIFF_WAVE_FORMAT_AMR_NB         (0x0057)
#define MKV_RIFF_WAVE_FORMAT_AMR_WB         (0x0058)
#define MKV_RIFF_WAVE_FORMAT_AAC            (0x00ff)
#define MKV_RIFF_IBM_FORMAT_MULAW           (0x0101)
#define MKV_RIFF_IBM_FORMAT_ALAW            (0x0102)
#define MKV_RIFF_WAVE_FORMAT_WMAV1          (0x0160)
#define MKV_RIFF_WAVE_FORMAT_WMAV2          (0x0161)
#define MKV_RIFF_WAVE_FORMAT_WMAV3          (0x0162)
#define MKV_RIFF_WAVE_FORMAT_WMAV3_L        (0x0163)
#define MKV_RIFF_WAVE_FORMAT_AAC_AC         (0x4143)
#define MKV_RIFF_WAVE_FORMAT_VORBIS         (0x566f)
#define MKV_RIFF_WAVE_FORMAT_VORBIS1        (0x674f)
#define MKV_RIFF_WAVE_FORMAT_VORBIS2        (0x6750)
#define MKV_RIFF_WAVE_FORMAT_VORBIS3        (0x6751)
#define MKV_RIFF_WAVE_FORMAT_VORBIS1PLUS    (0x676f)
#define MKV_RIFF_WAVE_FORMAT_VORBIS2PLUS    (0x6770)
#define MKV_RIFF_WAVE_FORMAT_VORBIS3PLUS    (0x6771)
#define MKV_RIFF_WAVE_FORMAT_AAC_pm         (0x706d)
#define MKV_RIFF_WAVE_FORMAT_GSM_AMR_CBR    (0x7A21)
#define MKV_RIFF_WAVE_FORMAT_GSM_AMR_VBR    (0x7A22)

#define MKV_RIFF_WAVE_FORMAT_ADPCM_MS        (0x0002)
#define MKV_RIFF_WAVE_FORMAT_ADPCM_IMA       (0x0011)


#define MATROSKA_ENABLE_SHOW_EXEC_TIME 0

/* bitmapinfoheader fields specified in Microsoft documentation:
   http://msdn2.microsoft.com/en-us/library/ms532290.aspx */
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
typedef struct asf_bitmapinfoheader_s asf_bitmapinfoheader_t;

#define ASF_BITMAPINFOHEADER_SIZE 40

long getTimeMs(void)
{
    static long _tv_sec = 0;
    static long _tv_usec = 0;
    struct timeval tv;
    struct timezone tz;
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

#define MATROSKA_DBG(fmt, ...)   ALOGD("[DEBUG][%08ld][%s]line:%d "fmt, getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MATROSKA_ERR(fmt, ...)   ALOGE("[ERROR][%08ld][%s]line:%d "fmt, getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MATROSKA_INFO(fmt, ...)  ALOGI("[INFO][%08ld][%s]line:%d "fmt, getTimeMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)


static const uint32_t kMP3HeaderMask = 0xfffe0c00;//0xfffe0cc0 add by zhihui zhang no consider channel mode
static const char *MKVwave2MIME(uint16_t id) {
    switch (id) {
        case  MKV_RIFF_WAVE_FORMAT_AMR_NB:
        case  MKV_RIFF_WAVE_FORMAT_GSM_AMR_CBR:
        case  MKV_RIFF_WAVE_FORMAT_GSM_AMR_VBR:
            return MEDIA_MIMETYPE_AUDIO_AMR_NB;

        case  MKV_RIFF_WAVE_FORMAT_AMR_WB:
            return MEDIA_MIMETYPE_AUDIO_AMR_WB;

        case  MKV_RIFF_WAVE_FORMAT_AAC:
        case  MKV_RIFF_WAVE_FORMAT_AAC_AC:
        case  MKV_RIFF_WAVE_FORMAT_AAC_pm:
            return MEDIA_MIMETYPE_AUDIO_AAC;

        case  MKV_RIFF_WAVE_FORMAT_VORBIS:
        case  MKV_RIFF_WAVE_FORMAT_VORBIS1:
        case  MKV_RIFF_WAVE_FORMAT_VORBIS2:
        case  MKV_RIFF_WAVE_FORMAT_VORBIS3:
        case  MKV_RIFF_WAVE_FORMAT_VORBIS1PLUS:
        case  MKV_RIFF_WAVE_FORMAT_VORBIS2PLUS:
        case  MKV_RIFF_WAVE_FORMAT_VORBIS3PLUS:
            return MEDIA_MIMETYPE_AUDIO_VORBIS;

        case  MKV_RIFF_WAVE_FORMAT_MPEGL12:
        case  MKV_RIFF_WAVE_FORMAT_MPEGL3:
            return MEDIA_MIMETYPE_AUDIO_MPEG;

        case MKV_RIFF_WAVE_FORMAT_MULAW:
        case MKV_RIFF_IBM_FORMAT_MULAW:
            return MEDIA_MIMETYPE_AUDIO_G711_MLAW;

        case MKV_RIFF_WAVE_FORMAT_ALAW:
        case MKV_RIFF_IBM_FORMAT_ALAW:
            return MEDIA_MIMETYPE_AUDIO_G711_ALAW;

        case MKV_RIFF_WAVE_FORMAT_PCM:
            return MEDIA_MIMETYPE_AUDIO_RAW;


        case MKV_RIFF_WAVE_FORMAT_ADPCM_MS:
            return MEDIA_MIMETYPE_AUDIO_ADPCM_MS;
        case MKV_RIFF_WAVE_FORMAT_ADPCM_IMA:
            return MEDIA_MIMETYPE_AUDIO_ADPCM_IMA;

        case MKV_RIFF_WAVE_FORMAT_WMAV1:
        case MKV_RIFF_WAVE_FORMAT_WMAV2:
            return MEDIA_MIMETYPE_AUDIO_WMA;


        default:
            MATROSKA_DBG("unknown wave %x", id);
            return "";
    };
}

static const uint32_t AACSampleFreqTable[16] =
{
    96000, /* 96000 Hz */
    88200, /* 88200 Hz */
    64000, /* 64000 Hz */
    48000, /* 48000 Hz */
    44100, /* 44100 Hz */
    32000, /* 32000 Hz */
    24000, /* 24000 Hz */
    22050, /* 22050 Hz */
    16000, /* 16000 Hz */
    12000, /* 12000 Hz */
    11025, /* 11025 Hz */
    8000, /*  8000 Hz */
    -1, /* future use */
    -1, /* future use */
    -1, /* future use */
    -1  /* escape value */
};

static bool findAACSampleFreqIndex(uint32_t freq, uint8_t &index)
{
    uint8_t i;
    uint8_t num = sizeof(AACSampleFreqTable)/sizeof(AACSampleFreqTable[0]);
    for (i=0; i < num; i++) {
        if (freq == AACSampleFreqTable[i])
            break;
    }
    if (i > 11)
        return false;

    index = i;
    return true;
}


#define FORMATFOURCC "0x%08x:%c%c%c%c"
#define PRINTFOURCC(x) x,((uint8_t*)&x)[0],((uint8_t*)&x)[1],((uint8_t*)&x)[2],((uint8_t*)&x)[3]

static uint8_t charLower(uint8_t ch)
{
    uint8_t ch_out = ch;
    if(ch >= 'A' && ch<= 'Z')
        ch_out = ch + 32;

    return ch_out;
}

/* trans all FOURCC  to lower char */
static uint32_t lowerFourCC(uint32_t fourcc)
{
    uint8_t ch_1 = (uint8_t)charLower(fourcc>>24);
    uint8_t ch_2 = (uint8_t)charLower(fourcc>>16);
    uint8_t ch_3 = (uint8_t)charLower(fourcc>>8);
    uint8_t ch_4 = (uint8_t)charLower(fourcc);

    uint32_t fourcc_out = ch_1<<24 | ch_2<<16 | ch_3<<8 | ch_4;

    return fourcc_out;
}

static const char *BMKVFourCC2MIME(uint32_t fourcc) {
    
    uint32_t lowerFourcc = lowerFourCC(fourcc);
    
    switch (lowerFourcc) {
     case BFOURCC('m', 'p', '4', 'a')://mp4a
            return MEDIA_MIMETYPE_AUDIO_AAC;

        case BFOURCC('s', 'a', 'm', 'r')://samr
            return MEDIA_MIMETYPE_AUDIO_AMR_NB;

        case BFOURCC('s', 'a', 'w', 'b')://sawb
            return MEDIA_MIMETYPE_AUDIO_AMR_WB;

        case BFOURCC('d', 'i', 'v', '3')://div3 
        case BFOURCC('d', 'i', 'v', '4')://div4 
        case BFOURCC('x', 'v', 'i', 'd')://xvid
        case BFOURCC('d', 'i', 'v', 'x')://divx
        case BFOURCC('d', 'x', '5', '0')://dx50
        case BFOURCC('m', 'p', '4', 'v')://mp4v
            return MEDIA_MIMETYPE_VIDEO_MPEG4;

        case BFOURCC('s', '2', '6', '3')://s263
        case BFOURCC('h', '2', '6', '3')://h263
            return MEDIA_MIMETYPE_VIDEO_H263;

        case BFOURCC('a', 'v', 'c', '1')://avc1
        case BFOURCC('h', '2', '6', '4')://h264
            return MEDIA_MIMETYPE_VIDEO_AVC;

        case BFOURCC('e', 'm', '2', 'v')://em2v
        case BFOURCC('m', 'p', '2', 'v')://mp2v
            return MEDIA_MIMETYPE_VIDEO_MPEG2;

        case BFOURCC('v', 'p', '6', 'f')://vp6f
            return MEDIA_MIMETYPE_VIDEO_VP6;

        case BFOURCC('m', 'j', 'p', 'g')://mjpg
            return MEDIA_MIMETYPE_VIDEO_MJPEG;

        case BFOURCC('w', 'm', 'v', '3')://wmv3
        case BFOURCC('w', 'v', 'c', '1')://wvc1
            return MEDIA_MIMETYPE_VIDEO_WMV;
        default:
          MATROSKA_DBG("unknown fourcc " FORMATFOURCC, PRINTFOURCC(fourcc));
            return "";
    }
}


static bool get_mp3_info(
        uint32_t header, size_t *frame_size,
        int *out_sampling_rate = NULL, int *out_channels = NULL,
        int *out_bitrate = NULL) {
    *frame_size = 0;

    if (out_sampling_rate) {
        *out_sampling_rate = 0;
    }

    if (out_channels) {
        *out_channels = 0;
    }

    if (out_bitrate) {
        *out_bitrate = 0;
    }

    if ((header & 0xffe00000) != 0xffe00000) {
        ALOGD("line=%d", __LINE__);
        return false;
    }

    unsigned version = (header >> 19) & 3;

    if (version == 0x01) {
        ALOGD("line=%d", __LINE__);
        return false;
    }

    unsigned layer = (header >> 17) & 3;

    if (layer == 0x00) {
        ALOGD("line=%d", __LINE__);
        return false;
    }

    unsigned protection = (header >> 16) & 1;

    unsigned bitrate_index = (header >> 12) & 0x0f;

    if (bitrate_index == 0 || bitrate_index == 0x0f) {
        // Disallow "free" bitrate.

        ALOGD("line=%d", __LINE__);
        return false;
    }

    unsigned sampling_rate_index = (header >> 10) & 3;

    if (sampling_rate_index == 3) {

        ALOGD("line=%d", __LINE__);
        return false;
    }

    static const int kSamplingRateV1[] = { 44100, 48000, 32000 };
    int sampling_rate = kSamplingRateV1[sampling_rate_index];
    if (version == 2 /* V2 */) {
        sampling_rate /= 2;
    } else if (version == 0 /* V2.5 */) {
        sampling_rate /= 4;
    }

    unsigned padding = (header >> 9) & 1;

    if (layer == 3) {
        // layer I

        static const int kBitrateV1[] = {
            32, 64, 96, 128, 160, 192, 224, 256,
            288, 320, 352, 384, 416, 448
        };

        static const int kBitrateV2[] = {
            32, 48, 56, 64, 80, 96, 112, 128,
            144, 160, 176, 192, 224, 256
        };

        int bitrate =
            (version == 3 /* V1 */)
                ? kBitrateV1[bitrate_index - 1]
                : kBitrateV2[bitrate_index - 1];

        if (out_bitrate) {
            *out_bitrate = bitrate;
        }

        *frame_size = (12000 * bitrate / sampling_rate + padding) * 4;
    } else {
        // layer II or III

        static const int kBitrateV1L2[] = {
            32, 48, 56, 64, 80, 96, 112, 128,
            160, 192, 224, 256, 320, 384
        };

        static const int kBitrateV1L3[] = {
            32, 40, 48, 56, 64, 80, 96, 112,
            128, 160, 192, 224, 256, 320
        };

        static const int kBitrateV2[] = {
            8, 16, 24, 32, 40, 48, 56, 64,
            80, 96, 112, 128, 144, 160
        };

        int bitrate;
        if (version == 3 /* V1 */) {
            bitrate = (layer == 2 /* L2 */)
                ? kBitrateV1L2[bitrate_index - 1]
                : kBitrateV1L3[bitrate_index - 1];
        } else {
            // V2 (or 2.5)

            bitrate = kBitrateV2[bitrate_index - 1];
        }

        if (out_bitrate) {
            *out_bitrate = bitrate;
        }

        if (version == 3 /* V1 */) {
            *frame_size = 144000 * bitrate / sampling_rate + padding;
        } else {
            // V2 or V2.5
                            *frame_size = 72000 * bitrate / sampling_rate + padding;
        }
    }

    if (out_sampling_rate) {
        *out_sampling_rate = sampling_rate;
    }

    if (out_channels) {
        int channel_mode = (header >> 6) & 3;

        *out_channels = (channel_mode == 3) ? 1 : 2;
    }

    return true;
}

static int mkv_mp3HeaderStartAt(const uint8_t *start, int length, uint32_t header) {
    uint32_t code = 0;
    int i = 0;

    for(i=0; i<length; i++){
        //ALOGD("start[%d]=%x", i, start[i]);
        code = (code<<8) + start[i];
        //ALOGD("code=0x%8.8x, mask=0x%8.8x", code, kMP3HeaderMask);
        if ((code & kMP3HeaderMask) == (header & kMP3HeaderMask)) {
            // some files has no seq start code
            return i - 3;
        }
    }

    return -1;
}
#endif

struct DataSourceReader : public mkvparser::IMkvReader {
    DataSourceReader(const sp<DataSource> &source)
        : mSource(source) {
    }

    virtual int Read(long long position, long length, unsigned char* buffer) {
        CHECK(position >= 0);
        CHECK(length >= 0);

        if (length == 0) {
            return 0;
        }

        ssize_t n = mSource->readAt(position, buffer, length);

        if (n <= 0) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
            MATROSKA_ERR("readAt %d bytes, Read return -1", n);
            MATROSKA_ERR("position= %d, length= %d", position, length);

#else
            ALOGE("readAt %d bytes, Read return -1", n);
            ALOGE("position= %d, length= %d", position, length);
#endif
            return -1;
        }

        return 0;
    }

    virtual int Length(long long* total, long long* available) {
        off64_t size;
        if (mSource->getSize(&size) != OK) {
            *total = -1;
            *available = (long long)((1ull << 63) - 1);

            return 0;
        }

        if (total) {
            *total = size;
        }

        if (available) {
            *available = size;
        }

        return 0;
    }

private:
    sp<DataSource> mSource;

    DataSourceReader(const DataSourceReader &);
    DataSourceReader &operator=(const DataSourceReader &);
};

////////////////////////////////////////////////////////////////////////////////

struct BlockIterator {
    BlockIterator(MatroskaExtractor *extractor, unsigned long trackNum);

    bool eos() const;

    void advance();
    void reset();

    void seek(
            int64_t seekTimeUs, bool isAudio,
            int64_t *actualFrameTimeUs);

    const mkvparser::Block *block() const;
    int64_t blockTimeUs() const;

private:
    MatroskaExtractor *mExtractor;
    unsigned long mTrackNum;
#ifndef ANDROID_DEFAULT_CODE
    unsigned long mTrackType;
#endif

    const mkvparser::Cluster *mCluster;
    const mkvparser::BlockEntry *mBlockEntry;
    long mBlockEntryIndex;

    void advance_l();

    BlockIterator(const BlockIterator &);
    BlockIterator &operator=(const BlockIterator &);
};

struct MatroskaSource : public MediaSource {
    MatroskaSource(
            const sp<MatroskaExtractor> &extractor, size_t index);

    virtual status_t start(MetaData *params);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **buffer, const ReadOptions *options);

protected:
    virtual ~MatroskaSource();

private:
    enum Type {
        AVC,
        AAC,
#ifndef ANDROID_DEFAULT_CODE
        VP8,
        VORBIS,
        MPEG4,
        RV,
        MP2_3,
        COOK,
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        PCM,
        MP1,
#endif
#endif
        OTHER
    };

    sp<MatroskaExtractor> mExtractor;
    size_t mTrackIndex;
    Type mType;
    bool mIsAudio;
    BlockIterator mBlockIter;
    size_t mNALSizeLen;  // for type AVC

    List<MediaBuffer *> mPendingFrames;

#ifndef ANDROID_DEFAULT_CODE
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    long long mBitsPerSample;
#endif
#endif
    status_t advance();

    status_t readBlock();
    void clearPendingFrames();

    MatroskaSource(const MatroskaSource &);
    MatroskaSource &operator=(const MatroskaSource &);

#ifndef ANDROID_DEFAULT_CODE
    status_t findMP3Header(uint32_t *header);
    unsigned char* mTrackContentAddData;
    size_t mTrackContentAddDataSize;
    bool mNewFrame;
    int64_t mCurrentTS;
    bool mFirstFrame;
    uint32_t mMP3Header;
public:
    void setCodecInfoFromFirstFrame();
#endif

};

MatroskaSource::MatroskaSource(
        const sp<MatroskaExtractor> &extractor, size_t index)
    : mExtractor(extractor),
      mTrackIndex(index),
      mType(OTHER),
      mIsAudio(false),
      mBlockIter(mExtractor.get(),
                 mExtractor->mTracks.itemAt(index).mTrackNum),
      mNALSizeLen(0) {
    sp<MetaData> meta = mExtractor->mTracks.itemAt(index).mMeta;

    const char *mime;
    CHECK(meta->findCString(kKeyMIMEType, &mime));

    mIsAudio = !strncasecmp("audio/", mime, 6);

    if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC)) {
        mType = AVC;

        uint32_t dummy;
        const uint8_t *avcc;
        size_t avccSize;
#ifndef ANDROID_DEFAULT_CODE
        if (!meta->findData(kKeyAVCC, &dummy, (const void **)&avcc, &avccSize))
        {
            sp<MetaData> metadata = NULL;
            while (metadata == NULL)
            {
                clearPendingFrames();
                 while (mPendingFrames.empty())
                 {
                     status_t err = readBlock();

                     if (err != OK)
                     {
                         clearPendingFrames();
                         break;
                     }
                 }

                if(!mPendingFrames.empty())
                {
                    MediaBuffer *buffer = *mPendingFrames.begin();
                    sp < ABuffer >  accessUnit = new ABuffer(buffer->range_length());
                    
                    MATROSKA_DBG("bigbuf->range_length() = %d",buffer->range_length());
                    
                    memcpy(accessUnit->data(),buffer->data(),buffer->range_length());
                    metadata = MakeAVCCodecSpecificData(accessUnit);
                }
            }
            CHECK(metadata->findData(kKeyAVCC, &dummy, (const void **)&avcc, &avccSize));
            
            MATROSKA_DBG("avccSize = %d ",avccSize);

            CHECK_GE(avccSize, 5u);
            
            meta->setData(kKeyAVCC, 0, avcc, avccSize);
            mBlockIter.reset();
            clearPendingFrames();
        }
#endif
        CHECK(meta->findData(
                    kKeyAVCC, &dummy, (const void **)&avcc, &avccSize));

        CHECK_GE(avccSize, 5u);

        mNALSizeLen = 1 + (avcc[4] & 3);
        MATROSKA_DBG("mNALSizeLen = %d", mNALSizeLen);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AAC)) {
        mType = AAC;
    }
#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_VPX))
    {
        mType = VP8;
    }
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_VORBIS))
    {
        mType = VORBIS;
    }
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG4))
    {
        mType = MPEG4;
    }
        else if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_REAL_VIDEO))
    {
        mType = RV;
    }
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_I))
    {
        mType = MP1;
        if (findMP3Header(&mMP3Header) != OK)
        {
            MATROSKA_DBG("No mp1 header found");
        }
        MATROSKA_DBG("mMP3Header=0x%8.8x", mMP3Header);
    }
#endif
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_MPEG))
    {
        mType = MP2_3;

        if (findMP3Header(&mMP3Header) != OK)
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        {
            MATROSKA_DBG("No mp3 header found");
        }
        MATROSKA_DBG("mMP3Header=0x%8.8x", mMP3Header);
#else
        {
            ALOGW("No mp3 header found");
        }
        ALOGD("mMP3Header=0x%8.8x", mMP3Header);
#endif
    }
        else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_REAL_AUDIO))
    {
        mType = COOK;
    }
        
#endif
}

MatroskaSource::~MatroskaSource() {
    clearPendingFrames();
}

#ifndef ANDROID_DEFAULT_CODE
status_t MatroskaSource::findMP3Header(uint32_t * header)
{
    if (header != NULL)
        *header = 0;

    uint32_t code = 0;
    while (0 == *header) {
        while (mPendingFrames.empty())
        {
            status_t err = readBlock();

            if (err != OK)
            {
                clearPendingFrames();
                return err;
            }
        }
        MediaBuffer *frame = *mPendingFrames.begin();
        size_t size = frame->range_length();
        size_t offset = frame->range_offset();
        size_t i;
        size_t frame_size;
        for (i=0; i<size; i++) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
            MATROSKA_DBG("data[%d]=%x", i, *((uint8_t*)frame->data()+offset+i));
#else
            ALOGD("data[%d]=%x", i, *((uint8_t*)frame->data()+offset+i));
#endif
            code = (code<<8) + *((uint8_t*)frame->data()+offset+i);
            if (get_mp3_info(code, &frame_size, NULL, NULL, NULL)) {
                *header = code;
                mBlockIter.reset();
    clearPendingFrames();
                return OK;
            }
        }
    }

    return ERROR_END_OF_STREAM;
}
#endif

status_t MatroskaSource::start(MetaData *params) {
    mBlockIter.reset();
#ifndef ANDROID_DEFAULT_CODE
    mNewFrame = true;
#endif

    return OK;
}

status_t MatroskaSource::stop() {
    clearPendingFrames();

    return OK;
}

sp<MetaData> MatroskaSource::getFormat() {
    return mExtractor->mTracks.itemAt(mTrackIndex).mMeta;
}

#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
void MatroskaSource::setCodecInfoFromFirstFrame() {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("Enter");
#else
    ALOGD("setCodecInfoFromFirstFrame");
#endif
    clearPendingFrames();
    //mBlockIter.seek(0);
    status_t err = readBlock();
    if (err != OK) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_ERR("read codec info from first block fail!");
#else
        ALOGE("read codec info from first block fail!");
#endif
        mBlockIter.reset();
        clearPendingFrames();
        return;
    }
    if (mPendingFrames.empty()) {
        return;
    }
    if (MPEG4 == mType) {
        /*if (0xB0010000 != *(uint32_t*)((uint8_t*)((*mPendingFrames.begin())->data())))
        {
            ALOGE("Can not find VOS in the first frame");//first frame is not start from VOS
            return;
        }*/
        size_t vosend;
        for (vosend=0; (vosend<200) && (vosend<(*mPendingFrames.begin())->range_length()-4); vosend++)
        {
            if (0xB6010000 == *(uint32_t*)((uint8_t*)((*mPendingFrames.begin())->data()) + vosend))
            {
                break;//Send VOS until VOP
            }
        }
        getFormat()->setData(kKeyMPEG4VOS, 0, (*mPendingFrames.begin())->data(), vosend);
        for (int32_t i=0; i<vosend; i++)
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
            MATROSKA_DBG("VOS[%d] = 0x%x", i, *((uint8_t *)((*mPendingFrames.begin())->data())+i));
#else
            ALOGD("VOS[%d] = 0x%x", i, *((uint8_t *)((*mPendingFrames.begin())->data())+i));
#endif
    }
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    else if (MP2_3 == mType || MP1 == mType)
#else
    else if (MP2_3 == mType)
#endif
    {
        uint32_t header = *(uint32_t*)((uint8_t*)(*mPendingFrames.begin())->data()+(*mPendingFrames.begin())->range_offset());
        header = ((header >> 24) & 0xff) | ((header >> 8) & 0xff00) | ((header << 8) & 0xff0000) | ((header << 24) & 0xff000000);
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("HEADER=0x%x", header);
#else
        ALOGD("HEADER=0x%x", header);
#endif
        size_t frame_size;
        int32_t out_sampling_rate;
        int32_t out_channels;
        int32_t out_bitrate;
        if(!get_mp3_info(header, &frame_size, &out_sampling_rate, &out_channels, &out_bitrate))
        {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
            MATROSKA_ERR("Get mp3 info fail");
#else
            ALOGE("Get mp3 info fail");
#endif
            return;
        }
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("mp3: frame_size=%d, sample_rate=%d, channel_count=%d, out_bitrate=%d",
            frame_size, out_sampling_rate, out_channels, out_bitrate);
#else
        ALOGD("mp3: frame_size=%d, sample_rate=%d, channel_count=%d, out_bitrate=%d",
            frame_size, out_sampling_rate, out_channels, out_bitrate);
#endif
        //Support 5.1
        if ((out_channels!=6) && (out_channels>2))
        {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
            MATROSKA_ERR("Unsupport mp3 channel count %d", out_channels);
#else
            ALOGE("Unsupport mp3 channel count %d", out_channels);
#endif
            return;
        }
        getFormat()->setInt32(kKeySampleRate, out_sampling_rate);
        getFormat()->setInt32(kKeyChannelCount, out_channels);


    }

    mBlockIter.reset();
    clearPendingFrames();

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("Leave");
#endif
}
#endif

////////////////////////////////////////////////////////////////////////////////

BlockIterator::BlockIterator(
        MatroskaExtractor *extractor, unsigned long trackNum)
    : mExtractor(extractor),
      mTrackNum(trackNum),
      mCluster(NULL),
      mBlockEntry(NULL),
      mBlockEntryIndex(0) {
#ifndef ANDROID_DEFAULT_CODE
    mTrackType = mExtractor->mSegment->GetTracks()->GetTrackByNumber(trackNum)->GetType();
#endif
    reset();
}

bool BlockIterator::eos() const {
    return mCluster == NULL || mCluster->EOS();
}

void BlockIterator::advance() {
    Mutex::Autolock autoLock(mExtractor->mLock);
    advance_l();
}

void BlockIterator::advance_l() {
    for (;;) {
        long res = mCluster->GetEntry(mBlockEntryIndex, mBlockEntry);
        ALOGV("GetEntry returned %ld", res);

        long long pos;
        long len;
        if (res < 0) {
            // Need to parse this cluster some more

            CHECK_EQ(res, mkvparser::E_BUFFER_NOT_FULL);

            res = mCluster->Parse(pos, len);
            ALOGV("Parse returned %ld", res);

            if (res < 0) {
                // I/O error

                ALOGE("Cluster::Parse returned result %ld", res);

                mCluster = NULL;
                break;
            }

            continue;
        } else if (res == 0) {
            // We're done with this cluster

            const mkvparser::Cluster *nextCluster;
            res = mExtractor->mSegment->ParseNext(
                    mCluster, nextCluster, pos, len);
            ALOGV("ParseNext returned %ld", res);

            if (res != 0) {
                // EOF or error

                mCluster = NULL;
                break;
            }

            CHECK_EQ(res, 0);
            CHECK(nextCluster != NULL);
            CHECK(!nextCluster->EOS());

            mCluster = nextCluster;

            res = mCluster->Parse(pos, len);
            ALOGV("Parse (2) returned %ld", res);
            CHECK_GE(res, 0);

            mBlockEntryIndex = 0;
            continue;
        }

        CHECK(mBlockEntry != NULL);
        CHECK(mBlockEntry->GetBlock() != NULL);
        ++mBlockEntryIndex;

        if (mBlockEntry->GetBlock()->GetTrackNumber() == mTrackNum) {
            break;
        }
    }
}

void BlockIterator::reset() {
    Mutex::Autolock autoLock(mExtractor->mLock);

    mCluster = mExtractor->mSegment->GetFirst();
    mBlockEntry = NULL;
    mBlockEntryIndex = 0;

    do {
        advance_l();
    } while (!eos() && block()->GetTrackNumber() != mTrackNum);
}

void BlockIterator::seek(
        int64_t seekTimeUs, bool isAudio,
        int64_t *actualFrameTimeUs) {
    Mutex::Autolock autoLock(mExtractor->mLock);

    *actualFrameTimeUs = -1ll;

    const int64_t seekTimeNs = seekTimeUs * 1000ll;

    mkvparser::Segment* const pSegment = mExtractor->mSegment;

    // Special case the 0 seek to avoid loading Cues when the application
    // extraneously seeks to 0 before playing.
    if (seekTimeNs <= 0) {
        ALOGV("Seek to beginning: %lld", seekTimeUs);
        mCluster = pSegment->GetFirst();
        mBlockEntryIndex = 0;
        do {
            advance_l();
        } while (!eos() && block()->GetTrackNumber() != mTrackNum);
        return;
    }

    ALOGV("Seeking to: %lld", seekTimeUs);

    // If the Cues have not been located then find them.
    const mkvparser::Cues* pCues = pSegment->GetCues();
    const mkvparser::SeekHead* pSH = pSegment->GetSeekHead();
    if (!pCues && pSH) {
        const size_t count = pSH->GetCount();
        const mkvparser::SeekHead::Entry* pEntry;
        ALOGV("No Cues yet");

        for (size_t index = 0; index < count; index++) {
            pEntry = pSH->GetEntry(index);

            if (pEntry->id == 0x0C53BB6B) { // Cues ID
                long len; long long pos;
                pSegment->ParseCues(pEntry->pos, pos, len);
                pCues = pSegment->GetCues();
                ALOGV("Cues found");
                break;
            }
        }

        if (!pCues) {
            ALOGE("No Cues in file");
            return;
        }
    }
    else if (!pSH) {
        ALOGE("No SeekHead");
        return;
    }

    const mkvparser::CuePoint* pCP;
    while (!pCues->DoneParsing()) {
        pCues->LoadCuePoint();
        pCP = pCues->GetLast();

        if (pCP->GetTime(pSegment) >= seekTimeNs) {
            ALOGV("Parsed past relevant Cue");
            break;
        }
    }

    // The Cue index is built around video keyframes
    mkvparser::Tracks const *pTracks = pSegment->GetTracks();
    const mkvparser::Track *pTrack = NULL;
    for (size_t index = 0; index < pTracks->GetTracksCount(); ++index) {
        pTrack = pTracks->GetTrackByIndex(index);
        if (pTrack && pTrack->GetType() == 1) { // VIDEO_TRACK
            ALOGV("Video track located at %d", index);
            break;
        }
    }

    // Always *search* based on the video track, but finalize based on mTrackNum
    const mkvparser::CuePoint::TrackPosition* pTP;
    if (pTrack && pTrack->GetType() == 1) {
        pCues->Find(seekTimeNs, pTrack, pCP, pTP);
    } else {
        ALOGE("Did not locate the video track for seeking");
        return;
    }

    mCluster = pSegment->FindOrPreloadCluster(pTP->m_pos);

    CHECK(mCluster);
    CHECK(!mCluster->EOS());

    // mBlockEntryIndex starts at 0 but m_block starts at 1
    CHECK_GT(pTP->m_block, 0);
    mBlockEntryIndex = pTP->m_block - 1;

    for (;;) {
        advance_l();

        if (eos()) break;

        if (isAudio || block()->IsKey()) {
            // Accept the first key frame
            *actualFrameTimeUs = (block()->GetTime(mCluster) + 500LL) / 1000LL;
            ALOGV("Requested seek point: %lld actual: %lld",
                  seekTimeUs, actualFrameTimeUs);
            break;
        }
    }
}

const mkvparser::Block *BlockIterator::block() const {
    CHECK(!eos());

    return mBlockEntry->GetBlock();
}

int64_t BlockIterator::blockTimeUs() const {
    return (mBlockEntry->GetBlock()->GetTime(mCluster) + 500ll) / 1000ll;
}

////////////////////////////////////////////////////////////////////////////////

static unsigned U24_AT(const uint8_t *ptr) {
    return ptr[0] << 16 | ptr[1] << 8 | ptr[2];
}

static size_t clz(uint8_t x) {
    size_t numLeadingZeroes = 0;

    while (!(x & 0x80)) {
        ++numLeadingZeroes;
        x = x << 1;
    }

    return numLeadingZeroes;
}

void MatroskaSource::clearPendingFrames() {
    while (!mPendingFrames.empty()) {
        MediaBuffer *frame = *mPendingFrames.begin();
        mPendingFrames.erase(mPendingFrames.begin());

        frame->release();
        frame = NULL;
    }
}

#define BAIL(err) \
    do {                        \
        if (bigbuf) {           \
            bigbuf->release();  \
            bigbuf = NULL;      \
        }                       \
                                \
        return err;             \
    } while (0)

status_t MatroskaSource::readBlock() {
    CHECK(mPendingFrames.empty());

    if (mBlockIter.eos()) {
        return ERROR_END_OF_STREAM;
    }

    const mkvparser::Block *block = mBlockIter.block();

    int64_t timeUs = mBlockIter.blockTimeUs();

    for (int i = 0; i < block->GetFrameCount(); ++i) {
        const mkvparser::Block::Frame &frame = block->GetFrame(i);

        MediaBuffer *mbuf = new MediaBuffer(frame.len);
        mbuf->meta_data()->setInt64(kKeyTime, timeUs);
        mbuf->meta_data()->setInt32(kKeyIsSyncFrame, block->IsKey());

        long n = frame.Read(mExtractor->mReader, (unsigned char *)mbuf->data());
        if (n != 0) {
            mPendingFrames.clear();

            mBlockIter.advance();
            return ERROR_IO;
        }

        mPendingFrames.push_back(mbuf);
    }

    mBlockIter.advance();
#ifdef ANDROID_DEFAULT_CODE
    unsigned lacing = (block->Flags() >> 1) & 3;
#endif
    return OK;
}
#undef BAIL


status_t MatroskaSource::read(
        MediaBuffer **out, const ReadOptions *options) {
    *out = NULL;

    int64_t targetSampleTimeUs = -1ll;

    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;
    if (options && options->getSeekTo(&seekTimeUs, &mode)
            && !mExtractor->isLiveStreaming()) {
        clearPendingFrames();

        // The audio we want is located by using the Cues to seek the video
        // stream to find the target Cluster then iterating to finalize for
        // audio.
        int64_t actualFrameTimeUs;
        mBlockIter.seek(seekTimeUs, mIsAudio, &actualFrameTimeUs);

        if (mode == ReadOptions::SEEK_CLOSEST) {
            targetSampleTimeUs = actualFrameTimeUs;
        }
    }

    while (mPendingFrames.empty()) {
        status_t err = readBlock();

        if (err != OK) {
            clearPendingFrames();

            return err;
        }
    }

    MediaBuffer *frame = *mPendingFrames.begin();
    mPendingFrames.erase(mPendingFrames.begin());

    if (mType != AVC) {
        if (targetSampleTimeUs >= 0ll) {
            frame->meta_data()->setInt64(
                    kKeyTargetTime, targetSampleTimeUs);
        }

        *out = frame;

        return OK;
    }

    // Each input frame contains one or more NAL fragments, each fragment
    // is prefixed by mNALSizeLen bytes giving the fragment length,
    // followed by a corresponding number of bytes containing the fragment.
    // We output all these fragments into a single large buffer separated
    // by startcodes (0x00 0x00 0x00 0x01).

    const uint8_t *srcPtr =
        (const uint8_t *)frame->data() + frame->range_offset();

    size_t srcSize = frame->range_length();

    size_t dstSize = 0;
    MediaBuffer *buffer = NULL;
    uint8_t *dstPtr = NULL;

    for (int32_t pass = 0; pass < 2; ++pass) {
        size_t srcOffset = 0;
        size_t dstOffset = 0;
        while (srcOffset + mNALSizeLen <= srcSize) {
            size_t NALsize;
            switch (mNALSizeLen) {
                case 1: NALsize = srcPtr[srcOffset]; break;
                case 2: NALsize = U16_AT(srcPtr + srcOffset); break;
                case 3: NALsize = U24_AT(srcPtr + srcOffset); break;
                case 4: NALsize = U32_AT(srcPtr + srcOffset); break;
                default:
                    TRESPASS();
            }

            if (srcOffset + mNALSizeLen + NALsize > srcSize) {
                break;
            }

            if (pass == 1) {
                memcpy(&dstPtr[dstOffset], "\x00\x00\x00\x01", 4);

                memcpy(&dstPtr[dstOffset + 4],
                       &srcPtr[srcOffset + mNALSizeLen],
                       NALsize);
            }

            dstOffset += 4;  // 0x00 00 00 01
            dstOffset += NALsize;

            srcOffset += mNALSizeLen + NALsize;
        }

        if (srcOffset < srcSize) {
            // There were trailing bytes or not enough data to complete
            // a fragment.

            frame->release();
            frame = NULL;

            return ERROR_MALFORMED;
        }

        if (pass == 0) {
            dstSize = dstOffset;

            buffer = new MediaBuffer(dstSize);

            int64_t timeUs;
            CHECK(frame->meta_data()->findInt64(kKeyTime, &timeUs));
            int32_t isSync;
            CHECK(frame->meta_data()->findInt32(kKeyIsSyncFrame, &isSync));

            buffer->meta_data()->setInt64(kKeyTime, timeUs);
            buffer->meta_data()->setInt32(kKeyIsSyncFrame, isSync);

            dstPtr = (uint8_t *)buffer->data();
        }
    }

    frame->release();
    frame = NULL;

    if (targetSampleTimeUs >= 0ll) {
        buffer->meta_data()->setInt64(
                kKeyTargetTime, targetSampleTimeUs);
    }

    *out = buffer;

    return OK;
}

////////////////////////////////////////////////////////////////////////////////

MatroskaExtractor::MatroskaExtractor(const sp<DataSource> &source
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
, uint32_t flags
#endif
)
    : mDataSource(source),
      mReader(new DataSourceReader(mDataSource)),
      mSegment(NULL),
      mExtractedThumbnails(false),
      mIsWebm(false) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Enter", (int)this);
#endif
    off64_t size;
    mIsLiveStreaming =
        (mDataSource->flags()
            & (DataSource::kWantsPrefetching
                | DataSource::kIsCachingDataSource))
        && mDataSource->getSize(&size) != OK;

    mkvparser::EBMLHeader ebmlHeader;
    long long pos;

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, =====================================\n", (int)this);
    MATROSKA_DBG("this=%d, [MKV Playback capability info]\n", (int)this);
    MATROSKA_DBG("this=%d, =====================================\n", (int)this);
    MATROSKA_DBG("this=%d, Resolution = \"[(8,8) ~ (640£¬480)]\" \n", (int)this);
    MATROSKA_DBG("this=%d, Support Codec = \"Video:VP8 & H264 & MPEG4 &  V_MS; Audio: VORBIS & A_AAC & ACM\" \n", (int)this);
    MATROSKA_DBG("this=%d, Profile_Level = \"Don't limit H264 level\" \n", (int)this);
    //MATROSKA_DBG("Max frameRate =  120fps \n");
    MATROSKA_DBG("this=%d, Max Bitrate  = 1Mbps  (640*480@30fps)\n", (int)this);
    MATROSKA_DBG("this=%d, =====================================\n", (int)this);
#else
    ALOGD("=====================================\n");
    ALOGD("[MKV Playback capability info]\n");
    ALOGD("=====================================\n");
    ALOGD("Resolution = \"[(8,8) ~ (640£¬480)]\" \n");
    ALOGD("Support Codec = \"Video:VP8 & H264 & MPEG4 &  V_MS; Audio: VORBIS & A_AAC & ACM\" \n");
    ALOGD("Profile_Level = \"Don't limit H264 level\" \n");
    //ALOGD("Max frameRate =  120fps \n");
    ALOGD("Max Bitrate  = 1Mbps  (640*480@30fps)\n");
    ALOGD("=====================================\n");
#endif

    if (ebmlHeader.Parse(mReader, pos) < 0) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("this=%d, ebmlHeader parsed failed: file format invalid, return directly", (int)this);
#endif
        return;
    }

    if (ebmlHeader.m_docType && !strcmp("webm", ebmlHeader.m_docType)) {
        mIsWebm = true;
    }

    long long ret =
        mkvparser::Segment::CreateInstance(mReader, pos, mSegment);

    if (ret) {
        CHECK(mSegment == NULL);
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("this=%d, mkvparser::Segment::CreateInstance(): failed, return directly", (int)this);
#endif
        return;
    }

    ret = mSegment->ParseHeaders();
    //CHECK_EQ(ret, 0);
    if (ret < 0) {
        delete mSegment;
        mSegment = NULL;
        return;
    }

    long len;
    ret = mSegment->LoadCluster(pos, len);
    //CHECK_EQ(ret, 0);

    if (ret < 0) {
        delete mSegment;
        mSegment = NULL;
        return;
    }

#if 0
    const mkvparser::SegmentInfo *info = mSegment->GetInfo();
    ALOGI("muxing app: %s, writing app: %s",
         info->GetMuxingAppAsUTF8(),
         info->GetWritingAppAsUTF8());
#endif

    addTracks();

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Leave", (int)this);
#endif
}

MatroskaExtractor::~MatroskaExtractor() {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Enter", (int)this);
#endif
    delete mSegment;
    mSegment = NULL;

    delete mReader;
    mReader = NULL;
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Leave", (int)this);
#endif
}

size_t MatroskaExtractor::countTracks() {
    return mTracks.size();
}

sp<MediaSource> MatroskaExtractor::getTrack(size_t index) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, getTrack(): occur", (int)this);
#endif

    if (index >= mTracks.size()) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("this=%d, mTracks.size()==0, return directly.", (int)this);
#endif
        return NULL;
    }

#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)

    sp<MediaSource> matroskasource = new MatroskaSource(this, index);
    int32_t isinfristframe = false;
    MATROSKA_DBG("this=%d, index=%d", (int)this, index);
    if (mTracks.itemAt(index).mMeta->findInt32(kKeyCodecInfoIsInFirstFrame, &isinfristframe)
        && isinfristframe) {
        MATROSKA_DBG("this=%d, Codec info is in first frame", (int)this);
        (static_cast<MatroskaSource*>(matroskasource.get()))->setCodecInfoFromFirstFrame();
    }
    return matroskasource;
#else
    return new MatroskaSource(this, index);
#endif
}

sp<MetaData> MatroskaExtractor::getTrackMetaData(
        size_t index, uint32_t flags) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Enter", (int)this);
#endif

    if (index >= mTracks.size()) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("this=%d, mTracks.size()==0, return directly.", (int)this);
#endif
        return NULL;
    }

    if ((flags & kIncludeExtensiveMetaData) && !mExtractedThumbnails
            && !isLiveStreaming()) {
        findThumbnails();
        mExtractedThumbnails = true;
    }

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Leave", (int)this);
#endif

    return mTracks.itemAt(index).mMeta;
}

bool MatroskaExtractor::isLiveStreaming() const {
    return mIsLiveStreaming;
}

static int bytesForSize(size_t size) {
    // use at most 28 bits (4 times 7)
    CHECK(size <= 0xfffffff);

    if (size > 0x1fffff) {
        return 4;
    } else if (size > 0x3fff) {
        return 3;
    } else if (size > 0x7f) {
        return 2;
    }
    return 1;
}

static void storeSize(uint8_t *data, size_t &idx, size_t size) {
    int numBytes = bytesForSize(size);
    idx += numBytes;

    data += idx;
    size_t next = 0;
    while (numBytes--) {
        *--data = (size & 0x7f) | next;
        size >>= 7;
        next = 0x80;
    }
}

static void addESDSFromCodecPrivate(
        const sp<MetaData> &meta,
        bool isAudio, const void *priv, size_t privSize) {

    int privSizeBytesRequired = bytesForSize(privSize);
    int esdsSize2 = 14 + privSizeBytesRequired + privSize;
    int esdsSize2BytesRequired = bytesForSize(esdsSize2);
    int esdsSize1 = 4 + esdsSize2BytesRequired + esdsSize2;
    int esdsSize1BytesRequired = bytesForSize(esdsSize1);
    size_t esdsSize = 1 + esdsSize1BytesRequired + esdsSize1;
    uint8_t *esds = new uint8_t[esdsSize];

    size_t idx = 0;
    esds[idx++] = 0x03;
    storeSize(esds, idx, esdsSize1);
    esds[idx++] = 0x00; // ES_ID
    esds[idx++] = 0x00; // ES_ID
    esds[idx++] = 0x00; // streamDependenceFlag, URL_Flag, OCRstreamFlag
    esds[idx++] = 0x04;
    storeSize(esds, idx, esdsSize2);
    esds[idx++] = isAudio ? 0x40   // Audio ISO/IEC 14496-3
                          : 0x20;  // Visual ISO/IEC 14496-2
    for (int i = 0; i < 12; i++) {
        esds[idx++] = 0x00;
    }
    esds[idx++] = 0x05;
    storeSize(esds, idx, privSize);
    memcpy(esds + idx, priv, privSize);

    meta->setData(kKeyESDS, 0, esds, esdsSize);

    delete[] esds;
    esds = NULL;
}
status_t addVorbisCodecInfo(
        const sp<MetaData> &meta,
        const void *_codecPrivate, size_t codecPrivateSize) {
    // hexdump(_codecPrivate, codecPrivateSize);

    if (codecPrivateSize < 1) {
        return ERROR_MALFORMED;
    }

    const uint8_t *codecPrivate = (const uint8_t *)_codecPrivate;

    if (codecPrivate[0] != 0x02) {
        return ERROR_MALFORMED;
    }

    // codecInfo starts with two lengths, len1 and len2, that are
    // "Xiph-style-lacing encoded"...

    size_t offset = 1;
    size_t len1 = 0;
    while (offset < codecPrivateSize && codecPrivate[offset] == 0xff) {
        len1 += 0xff;
        ++offset;
    }
    if (offset >= codecPrivateSize) {
        return ERROR_MALFORMED;
    }
    len1 += codecPrivate[offset++];

    size_t len2 = 0;
    while (offset < codecPrivateSize && codecPrivate[offset] == 0xff) {
        len2 += 0xff;
        ++offset;
    }
    if (offset >= codecPrivateSize) {
        return ERROR_MALFORMED;
    }
    len2 += codecPrivate[offset++];

    if (codecPrivateSize < offset + len1 + len2) {
        return ERROR_MALFORMED;
    }

    if (codecPrivate[offset] != 0x01) {
        return ERROR_MALFORMED;
    }
    meta->setData(kKeyVorbisInfo, 0, &codecPrivate[offset], len1);

    offset += len1;
    if (codecPrivate[offset] != 0x03) {
        return ERROR_MALFORMED;
    }

    offset += len2;
    if (codecPrivate[offset] != 0x05) {
        return ERROR_MALFORMED;
    }
    
#if 0//#ifdef CC_SUPPORT_TV_STAGEFRIGHT //Wait Porting Ready
    meta->setData(kKeyVorbisComment, 0, &codecPrivate[len1 + 3], len2);
#endif

    meta->setData(
            kKeyVorbisBooks, 0, &codecPrivate[offset],
            codecPrivateSize - offset);

    return OK;
}

void MatroskaExtractor::addTracks() {
    const mkvparser::Tracks *tracks = mSegment->GetTracks();
    
    mFileMetaData = new MetaData;
    
    for (size_t index = 0; index < tracks->GetTracksCount(); ++index) {
        const mkvparser::Track *track = tracks->GetTrackByIndex(index);

        if (track == NULL) {
            // Apparently this is currently valid (if unexpected) behaviour
            // of the mkv parser lib.
            continue;
        }

        const char *const codecID = track->GetCodecId();
        ALOGV("codec id = %s", codecID);
        ALOGV("codec name = %s", track->GetCodecNameAsUTF8());

        size_t codecPrivateSize;
        const unsigned char *codecPrivate =
            track->GetCodecPrivate(codecPrivateSize);

        enum { VIDEO_TRACK = 1, AUDIO_TRACK = 2 };

        sp<MetaData> meta = new MetaData;

        status_t err = OK;

        switch (track->GetType()) {
            case VIDEO_TRACK:
            {
                const mkvparser::VideoTrack *vtrack =
                    static_cast<const mkvparser::VideoTrack *>(track);

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
                MATROSKA_DBG("VIDEO_TRACK Found");

                long long width = vtrack->GetWidth();
                long long height = vtrack->GetHeight();
                if(width==0)
                {
                    width = 1366;
                }
                
                if(height==0)
                {
                    height = 768;
                }
                MATROSKA_DBG("VIDEO_TRACK width %lld\n",width);
                MATROSKA_DBG("VIDEO_TRACK height %lld\n",height);
                
                meta->setInt32(kKeyWidth, width);
                meta->setInt32(kKeyHeight, height);
#endif

                if (!strcmp("V_MPEG4/ISO/AVC", codecID)) {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC);
                    meta->setData(kKeyAVCC, 0, codecPrivate, codecPrivateSize);
                } 
               else if (!strcmp("V_MPEG4/ISO/ASP", codecID)) 
               {
                    if (codecPrivateSize > 0) {
                        meta->setCString(
                                kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG4);
                        addESDSFromCodecPrivate(
                                meta, false, codecPrivate, codecPrivateSize);
                    } else {
                        ALOGW("%s is detected, but does not have configuration.",
                                codecID);
                        continue;
                    }
                } 
                else if (!strcmp("V_VP8", codecID)) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_VP8);
                } 
                else if (!strcmp("V_VP9", codecID))
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_VP9);
                } 
#ifdef CC_SUPPORT_TV_STAGEFRIGHT 
                else if (!strcmp("V_MS/VFW/FOURCC", codecID))
                {
                    ALOGD("Video CodecID: V_MS/VFW/FOURCC");
                    if ((NULL == codecPrivate) || (codecPrivateSize < 20))
                    {
                        ALOGE("Unsupport video: V_MS/VFW/FOURCC has no invalid private data, codecPrivate=%p, codecPrivateSize=%d", codecPrivate, codecPrivateSize);
                    
                        mFileMetaData->setInt32(kKeyHasUnsupportVideo,true);
                        continue;
                    } 
                    else 
                    {
                        uint32_t fourcc = *(uint32_t *)(codecPrivate + 16);
                        const char* mime = BMKVFourCC2MIME(fourcc);
                            ALOGD("V_MS/VFW/FOURCC type is %s", mime);
                        if (!strncasecmp("video/", mime, 6)) 
                        {
                            meta->setCString(kKeyMIMEType, mime);
                        }
                        else 
                        {
                            ALOGE("V_MS/VFW/FOURCC continue");
                            mFileMetaData->setInt32(kKeyHasUnsupportVideo,true);
                            continue;
                        }
                        if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG4))
                        {
                            meta->setInt32(kKeyCodecInfoIsInFirstFrame,true);
                            meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG4);
                            MATROSKA_DBG("this=%d, Video Codec: V_MS/VFW/FOURCC, V_MPEG4", (int)this);
                        } 
                        else if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_AVC))
                        {
                            meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC);
                            MATROSKA_DBG("this=%d, Video Codec: V_MS/VFW/FOURCC, V_H264", (int)this);
                        } else if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_H263))
                        {
                            meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_H263);
                            MATROSKA_DBG("this=%d, Video Codec: V_MS/VFW/FOURCC, V_H263", (int)this);
                        }
                        else if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG2)) 
                        {
                            meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG2);
                            MATROSKA_DBG("this=%d, Video Codec: V_MS/VFW/FOURCC, V_MPEG2", (int)this);
                        }
                        else if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_MJPEG))
                        {
                            meta->setData(kKeyMJPG, 0, codecPrivate, codecPrivateSize);
                            meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MJPEG);
                            MATROSKA_DBG("this=%d, Video Codec: V_MS/VFW/FOURCC, V_MJPEG", (int)this);
                        }
                        else if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_WMV)) 
                        {
                            MATROSKA_DBG("this=%d, Video Codec: V_MS/VFW/FOURCC, V_WMV", (int)this);

                            asf_bitmapinfoheader_t *bmih = new asf_bitmapinfoheader_t;

                            uint8_t* _config = new uint8_t[codecPrivateSize + 5];
                    
                            // fill codecPrivate: begin
                            {
                                int32_t u2Height = 0;
                                uint8_t u1Profile = 8;
                                int32_t u2Width = 0;
                    
                                meta->findInt32(kKeyWidth, &u2Width);
                    
                                meta->findInt32(kKeyHeight, &u2Height);
                    
                                // ToDo: megaa tell me wmv3 codec,need set profile = 4;
                                if (fourcc == BFOURCC('w', 'm', 'v', '3')
                                    || fourcc == BFOURCC('W', 'M', 'V', '3'))
                                {
                                    u1Profile = 4;  // main  profile
                                    memcpy((char*)&bmih->biCompression, "WMV3", sizeof(uint32_t));
                                }
                                // ToDo: megaa tell me wvc-1 codec,need set profile  = 8;
                                else if(fourcc == BFOURCC('w', 'v', 'c', '1')
                                        || fourcc == BFOURCC('W', 'V', 'C', '1'))
                                {
                                    u1Profile = 8;  // advanced profile
                                    memcpy((char*)&bmih->biCompression, "WVC1", sizeof(uint32_t));
                                }
                                
         
                                bmih->biWidth = u2Width;
                                bmih->biHeight = u2Height;
                                
                                memcpy(_config, bmih, ASF_BITMAPINFOHEADER_SIZE);
                                
                                 // skip BitmapInfoHeader
                                 memcpy(_config + 5, codecPrivate + 40, codecPrivateSize - 40);  // reserve the first 5 BYTEs for profile, width and height indication
                            }
                            // fill codecPrivate: end
                    
                            meta->setData(kKeyWMVC, 0, _config, codecPrivateSize - 40 + 5 );
                            delete [] _config;
                        }
                    }
                }
                else if ((!strcmp("V_MPEG4/ISO/ASP", codecID)) ||
                         (!strcmp("V_MPEG4/ISO/SP", codecID))) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG4);
                            MATROSKA_DBG("this=%d, Video Codec: MPEG4", (int)this);
                
                    if (codecPrivate != NULL)
                    {
                        meta->setData(kKeyMPEG4VOS, 0, codecPrivate, codecPrivateSize);
                    }
                    else 
                    {
                            MATROSKA_DBG("this=%d, No specific codec private data, find it from the first frame", (int)this);
                            meta->setInt32(kKeyCodecInfoIsInFirstFrame,true);
                    }
                }
                else if (!strcmp("V_MPEG1", codecID)) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG1);
                    MATROSKA_DBG("this=%d, Video Codec: V_MPEG1", (int)this);
                }
                else if (!strcmp("V_MPEG2", codecID)) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_MPEG2);
                    MATROSKA_DBG("this=%d, Video Codec: V_MPEG2", (int)this);
                }
                else if (!strcmp("V_REAL/RV40", codecID)) 
                {
                    ALOGW("%s is not supported for L issue", codecID);
                }
                #ifdef MTK_VIDEO_HEVC_SUPPORT
                else if (!strcmp("V_MPEGH/ISO/HEVC", codecID)) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_HEVC);
                    MATROSKA_DBG("this=%d, Video Codec: V_MPEGH/ISO/HEVC", (int)this);
                }
                #endif
#endif
                else
                {
                    ALOGW("%s is not supported.", codecID);
                    continue;
                }
 #if 0
                const char *vCodecType;
                CHECK(meta->findCString(kKeyMIMEType, &vCodecType));
                MATROSKA_DBG("this=%d, Video--Codec: vCodecType %s", (int)this, vCodecType);
                if ((!strcasecmp(MEDIA_MIMETYPE_VIDEO_MPEG2, vCodecType))
                     ||(!strcasecmp(MEDIA_MIMETYPE_VIDEO_MPEG1, vCodecType)))
                {
                     MATROSKA_DBG("this=%d, V_MPEG2/V_MPEG1 codecPrivate[0 1 2 3]=%x %x %x %x, codecPrivateSize=%d", (int)this, codecPrivate[0],codecPrivate[1],codecPrivate[2],codecPrivate[3], codecPrivateSize);
                     sp<ABuffer> csd = new ABuffer(codecPrivateSize);
                     memcpy(csd->data(), codecPrivate, codecPrivateSize);
                
                     sp<ABuffer> esds = MakeMPEGVideoESDS(csd);
                     meta->setData(
                         kKeyESDS, kTypeESDS, esds->data(), esds->size());
                 }
#endif 

                meta->setInt32(kKeyWidth, vtrack->GetWidth());
                meta->setInt32(kKeyHeight, vtrack->GetHeight());
                break;
            }

            case AUDIO_TRACK:
            {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
                MATROSKA_DBG("AUDIO_TRACK Found");
#endif
                const mkvparser::AudioTrack *atrack =
                    static_cast<const mkvparser::AudioTrack *>(track);

                if (!strcmp("A_AAC", codecID)) {
                    
                    #ifdef CC_SUPPORT_TV_STAGEFRIGHT
                    unsigned char aacCodecInfo[2]={0, 0};
                    if (codecPrivateSize >= 2)
                    {
                         uint8_t profile;
                         profile = (codecPrivate[0] >> 3) & 0x1f;//highest 5 bit
                         if ((profile != 2) //LC
                             && (profile != 4) //LTP
                             && (profile != 5) //SBR
                             && (profile != 29)) //PS
                         {
                             MATROSKA_DBG("this=%d, Unspport AAC: profile %d", (int)this, profile);
                             continue;
                         }
                         uint8_t chan_num;
                         chan_num = (codecPrivate[1] >> 3) & 0x0f;
                         if (((chan_num != 6) && (chan_num > 2)) || (chan_num < 1))
                         {
                             MATROSKA_DBG("this=%d, Unspport AAC: channel count=%d", (int)this, chan_num);

                             continue;
                         }
                    }
                    else if (NULL == codecPrivate)
                    {
                        if (!strcasecmp("A_AAC", codecID)) 
                        {
                            MATROSKA_DBG("this=%d, Unspport AAC: No profile", (int)this);
                            continue;
                        }
                        else  
                        {
                            uint8_t freq_index=-1;
                            uint8_t profile;
                            if (!findAACSampleFreqIndex((uint32_t)atrack->GetSamplingRate(), freq_index))
                            {
                                MATROSKA_ERR("this=%d, Unsupport AAC freq", (int)this);

                                continue;
                            }

                            if (((atrack->GetChannels() != 6) && (atrack->GetChannels() > 2)) || (atrack->GetChannels() < 1)) 
                            {
                                MATROSKA_ERR("this=%d, Unsupport AAC channel count %lld", (int)this, atrack->GetChannels());
                                continue;
                            }

                            if (!strcasecmp("A_AAC/MPEG4/LC", codecID) ||
                                !strcasecmp("A_AAC/MPEG2/LC", codecID))
                            {
                                profile = 2;
                            }
                            else if (!strcasecmp("A_AAC/MPEG4/LC/SBR", codecID) ||
                                !strcasecmp("A_AAC/MPEG2/LC/SBR", codecID))
                            {
                                profile = 5;
                            }
                            else if (!strcasecmp("A_AAC/MPEG4/LTP", codecID))
                            {
                                profile = 4;
                            }
                            else 
                            {
                                MATROSKA_ERR("this=%d, Unsupport AAC Codec profile %s", (int)this, codecID);
                                continue;
                            }

                            codecPrivate = aacCodecInfo;
                            codecPrivateSize = 2;
                            aacCodecInfo[0] |= (profile << 3) & 0xf1;   // put it into the highest 5 bits
                            aacCodecInfo[0] |= ((freq_index >> 1) & 0x07);     // put 3 bits
                            aacCodecInfo[1] |= ((freq_index << 7) & 0x80); // put 1 bit
                            aacCodecInfo[1] |= ((unsigned char)atrack->GetChannels()<< 3);

                            MATROSKA_DBG("this=%d, Make codec info 0x%x, 0x%x", (int)this, aacCodecInfo[0], aacCodecInfo[1]);
                        }
                    }
                    else 
                    {
                        MATROSKA_DBG("this=%d, Incomplete AAC Codec Info %d byte", (int)this, codecPrivateSize);
                        continue;
                    }
                    #endif
                    
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AAC);
                    CHECK(codecPrivateSize >= 2);

                    addESDSFromCodecPrivate(
                            meta, true, codecPrivate, codecPrivateSize);
                } else if (!strcmp("A_VORBIS", codecID)) {

                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_VORBIS);

                    err = addVorbisCodecInfo(
                            meta, codecPrivate, codecPrivateSize);
                    
                    MATROSKA_DBG("this=%d, Audio Codec: VORBIS", (int)this);
                    
                } 
                #ifdef CC_SUPPORT_TV_STAGEFRIGHT
                else if (!strcmp("A_MPEG/L1", codecID)) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_I);
                    MATROSKA_DBG("this=%d, Audio Codec: MPEG/L1", (int)this);
                    if ((atrack->GetChannels() != 6) && (atrack->GetChannels() > 2)) {
                        MATROSKA_ERR("this=%d, Unsupport MP1 Channel count=%lld", (int)this, atrack->GetChannels());
                        continue;
                    }
                    if ((atrack->GetSamplingRate() < 0.1) || (atrack->GetChannels() == 0))
                    {
                        meta->setInt32(kKeyCodecInfoIsInFirstFrame,true);
                    }
                }
                else if ((!strcmp("A_MPEG/L2", codecID)) ||
                        (!strcmp("A_MPEG/L3", codecID)))
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
                    MATROSKA_DBG("this=%d, Audio Codec: MPEG", (int)this);
                    if ((atrack->GetChannels() != 6) && (atrack->GetChannels() > 2)) {
                        MATROSKA_ERR("this=%d, Unsupport MP3 Channel count=%lld", (int)this, atrack->GetChannels());
                        continue;
                    }
                    if ((atrack->GetSamplingRate() < 0.1) || (atrack->GetChannels() == 0))
                    {
                        meta->setInt32(kKeyCodecInfoIsInFirstFrame,true);
                    }
                }
                else if ((!strcmp("A_DTS", codecID))) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_DTS);
                    MATROSKA_DBG("this=%d, Audio Codec: DTS", (int)this);
                }
                else if ((!strcmp("A_AC3", codecID))) 
                {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AC3);
                    MATROSKA_DBG("this=%d, Audio Codec: AC3", (int)this);
                }
                else if ((!strcmp("A_PCM/INT/LIT", codecID)) ||
                    (!strcmp("A_PCM/FLOAT/IEEE", codecID))) 
               {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);
                    MATROSKA_DBG("this=%d, Audio Codec: PCM", (int)this);
                    if (atrack->GetBitDepth() != 8 && atrack->GetBitDepth()  != 16
                        && atrack->GetBitDepth()  != 24 && atrack->GetBitDepth()  != 32) {
                            MATROSKA_ERR("this=%d, Unsupport BitsDepth %lld", (int)this, atrack->GetBitDepth());
                            continue;
                    }
                }
                else if ((!strcmp("A_MS/ACM", codecID))) 
                {
                    if ((NULL == codecPrivate) || (codecPrivateSize < 8))
                    {
                        MATROSKA_ERR("this=%d, Unsupport audio: A_MS/ACM has no invalid private data, codecPrivate=%p, codecPrivateSize=%d",
                        (int)this, codecPrivate, codecPrivateSize);
                        continue;
                    }
                    else 
                    {
                        uint16_t ID = *(uint16_t *)codecPrivate;
                        const char* mime = MKVwave2MIME(ID);
                            MATROSKA_DBG("this=%d, A_MS/ACM type is %s", (int)this, mime);
                        if (!strncasecmp("audio/", mime, 6))
                        {
                            meta->setCString(kKeyMIMEType, mime);
                        } 
                        else 
                        {
                            MATROSKA_ERR("this=%d, A_MS/ACM continue", (int)this);
                            continue;
                        }
                        if ((!strcasecmp("audio/x-adpcm-ms", mime)) ||
                            (!strcasecmp("audio/x-adpcm-ima", mime)))
                        {
                            if (atrack->GetBitDepth() != 4)
                            {
                                MATROSKA_ERR("this=%d, ADPCM-MS/IMA BitDepth must be 4!", (int)this);
                                continue;
                            }
                        }
                
                        if (!strcasecmp("audio/x-ms-wma", mime))
                        {
                            meta->setData(kKeyWMAC, 0, codecPrivate, codecPrivateSize);
                            MATROSKA_ERR("this=%d, [MKV]RetrieveWmaCodecSpecificData OK !", (int)this);
                        }
                
                        if ((!strcasecmp("audio/g711-alaw", mime)) ||
                            (!strcasecmp("audio/g711-mlaw", mime))) 
                        {
                            if (atrack->GetBitDepth() != 8)
                            {
                                MATROSKA_ERR("this=%d, ADPCM-MS/IMA BitDepth must be 8!", (int)this);
                                continue;
                            }
                        }
                    }
                }
                #endif
                else if (!strcmp("A_MPEG/L3", codecID)) {
                    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
                } else {
                    ALOGW("%s is not supported.", codecID);
                    continue;
                }

                meta->setInt32(kKeySampleRate, atrack->GetSamplingRate());
                meta->setInt32(kKeyChannelCount, atrack->GetChannels());
                
#ifndef ANDROID_DEFAULT_CODE
                meta->setInt32(kKeyMaxInputSize, 16384);
#endif
                break;
            }

            default:
                continue;
        }

        if (err != OK) {
            ALOGE("skipping track, codec specific data was malformed.");
            continue;
        }

        long long durationNs = mSegment->GetDuration();
        meta->setInt64(kKeyDuration, (durationNs + 500) / 1000);

        mTracks.push();
        TrackInfo *trackInfo = &mTracks.editItemAt(mTracks.size() - 1);
        trackInfo->mTrackNum = track->GetNumber();
        trackInfo->mMeta = meta;
        
        const char *fileMIME;
        //CHECK(meta->findCString(kKeyMIMEType, &fileMIME));
        if(meta->findCString(kKeyMIMEType, &fileMIME))
        {
            mFileMetaData->setCString(kKeyMIMEType, fileMIME);
        }
        else
        {
            ALOGE("No kKeyMIMEType in track.");
        }
    }

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Leave", (int)this);
#endif

}

void MatroskaExtractor::findThumbnails() {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Enter", (int)this);
#endif

    for (size_t i = 0; i < mTracks.size(); ++i) {
        TrackInfo *info = &mTracks.editItemAt(i);

        const char *mime;
        CHECK(info->mMeta->findCString(kKeyMIMEType, &mime));

        if (strncasecmp(mime, "video/", 6)) {
            continue;
        }

        BlockIterator iter(this, info->mTrackNum);
        int32_t j = 0;
        int64_t thumbnailTimeUs = 0;
        size_t maxBlockSize = 0;
        while (!iter.eos() && j < 20) {
            if (iter.block()->IsKey()) {
                ++j;

                size_t blockSize = 0;
                for (int k = 0; k < iter.block()->GetFrameCount(); ++k) {
                    blockSize += iter.block()->GetFrame(k).len;
                }

                if (blockSize > maxBlockSize) {
                    maxBlockSize = blockSize;
                    thumbnailTimeUs = iter.blockTimeUs();
                }
            }
            iter.advance();
        }
        info->mMeta->setInt64(kKeyThumbnailTime, thumbnailTimeUs);
    }

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("this=%d, Leave", (int)this);
#endif
}

sp<MetaData> MatroskaExtractor::getMetaData() {
    #ifndef ANDROID_DEFAULT_CODE
    return mFileMetaData;
    #else
    sp<MetaData> meta = new MetaData;
    
    meta->setCString(
            kKeyMIMEType,
            mIsWebm ? "video/webm" : MEDIA_MIMETYPE_CONTAINER_MATROSKA);
    
    return meta;
    #endif
}

uint32_t MatroskaExtractor::flags() const {
    uint32_t x = CAN_PAUSE;
    if (!isLiveStreaming()) {
        x |= CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_SEEK;
    }

    return x;
}

bool SniffMatroska(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("Enter");
#endif

    DataSourceReader reader(source);
    mkvparser::EBMLHeader ebmlHeader;
    long long pos;
    if (ebmlHeader.Parse(&reader, pos) < 0) {
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
        MATROSKA_DBG("SniffMatroska(%s) return false", mimeType->string());
#else
        ALOGI("SniffMatroska(%s) return false", mimeType->string());
#endif
        return false;
    }

    mimeType->setTo(MEDIA_MIMETYPE_CONTAINER_MATROSKA);
    *confidence = 0.6;

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    MATROSKA_DBG("SniffMatroska(%s) return true(%f)", mimeType->string(), *confidence);
    MATROSKA_DBG("Leave");
#else
    ALOGI("SniffMatroska(%s) return true(%f)", mimeType->string(), *confidence);
#endif

    return true;
}

}  // namespace android
