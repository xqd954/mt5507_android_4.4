/*
 * Copyright (C) 2009 The Android Open Source Project
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
#undef LOG_TO_FILE
//#define LOG_TO_FILE
#define LOG_TAG "MPEG4Extractor"
#include <utils/Log.h>

#include "include/MPEG4Extractor.h"
#include "include/SampleTable.h"
#include "include/ESDS.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <media/stagefright/foundation/ABitReader.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MetaData.h>
#include <utils/String8.h>
#ifndef ANDROID_DEFAULT_CODE
#include <sys/sysconf.h>
#include <asm/page.h>

#include "include/avc_utils.h"

#include "include/ESDS.h"
#define QUICKTIME_SUPPORT

#define MP4_PRINT_PATH//print parse flow to easy debug
#define CC_SUPPORT_TV_STAGEFRIGHT_4K2K //SUPPORT 4K
#endif

namespace android {
#ifndef ANDROID_DEFAULT_CODE
#ifdef CC_SUPPORT_TV_STAGEFRIGHT_4K2K
static int32_t VIDEO_MAX_W_H = 4096;//3840;
static int32_t VIDEO_MAX_RESOLUTION = 4096*2304;//3840*2160;
#else
static int32_t VIDEO_MAX_W_H = 1920;
static int32_t VIDEO_MAX_RESOLUTION = 1920*1088;
#endif

static int32_t VIDEO_MAX_FPS = 120;
static int32_t AVC_MAX_MACRO_PER_SECOND = 522240;//LEVEL 4.2
#endif

class MPEG4Source : public MediaSource {
public:
    // Caller retains ownership of both "dataSource" and "sampleTable".
    MPEG4Source(const sp<MetaData> &format,
                const sp<DataSource> &dataSource,
                int32_t timeScale,
                const sp<SampleTable> &sampleTable,
                Vector<SidxEntry> &sidx,
                off64_t firstMoofOffset);

    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(MediaBuffer **buffer, const ReadOptions *options = NULL);
#ifndef ANDROID_DEFAULT_CODE
	uint64_t getStartTimeOffsetUs()
	{
		return (uint64_t)mSampleTable->getStartTimeOffset()*mTimescale*1000000;
	}
#endif
    virtual status_t fragmentedRead(MediaBuffer **buffer, const ReadOptions *options = NULL);

protected:
    virtual ~MPEG4Source();

private:
    Mutex mLock;

    sp<MetaData> mFormat;
    sp<DataSource> mDataSource;
    int32_t mTimescale;
    sp<SampleTable> mSampleTable;
    uint32_t mCurrentSampleIndex;
    uint32_t mCurrentFragmentIndex;
    Vector<SidxEntry> &mSegments;
    off64_t mFirstMoofOffset;
    off64_t mCurrentMoofOffset;
    off64_t mNextMoofOffset;
    uint32_t mCurrentTime;
    int32_t mLastParsedTrackId;
    int32_t mTrackId;

    int32_t mCryptoMode;    // passed in from extractor
    int32_t mDefaultIVSize; // passed in from extractor
    uint8_t mCryptoKey[16]; // passed in from extractor
    uint32_t mCurrentAuxInfoType;
    uint32_t mCurrentAuxInfoTypeParameter;
    int32_t mCurrentDefaultSampleInfoSize;
    uint32_t mCurrentSampleInfoCount;
    uint32_t mCurrentSampleInfoAllocSize;
    uint8_t* mCurrentSampleInfoSizes;
    uint32_t mCurrentSampleInfoOffsetCount;
    uint32_t mCurrentSampleInfoOffsetsAllocSize;
    uint64_t* mCurrentSampleInfoOffsets;

    bool mIsAVC;
    size_t mNALLengthSize;

    bool mStarted;

    MediaBufferGroup *mGroup;

    MediaBuffer *mBuffer;

    bool mWantsNALFragments;

    uint8_t *mSrcBuffer;
#ifdef LOG_TO_FILE
    FILE *fp;
#endif

    size_t parseNALSize(const uint8_t *data) const;
    status_t parseChunk(off64_t *offset);
    status_t parseTrackFragmentHeader(off64_t offset, off64_t size);
    status_t parseTrackFragmentRun(off64_t offset, off64_t size);
    status_t parseSampleAuxiliaryInformationSizes(off64_t offset, off64_t size);
    status_t parseSampleAuxiliaryInformationOffsets(off64_t offset, off64_t size);

    struct TrackFragmentHeaderInfo {
        enum Flags {
            kBaseDataOffsetPresent         = 0x01,
            kSampleDescriptionIndexPresent = 0x02,
            kDefaultSampleDurationPresent  = 0x08,
            kDefaultSampleSizePresent      = 0x10,
            kDefaultSampleFlagsPresent     = 0x20,
            kDurationIsEmpty               = 0x10000,
        };

        uint32_t mTrackID;
        uint32_t mFlags;
        uint64_t mBaseDataOffset;
        uint32_t mSampleDescriptionIndex;
        uint32_t mDefaultSampleDuration;
        uint32_t mDefaultSampleSize;
        uint32_t mDefaultSampleFlags;

        uint64_t mDataOffset;
    };
    TrackFragmentHeaderInfo mTrackFragmentHeaderInfo;

    struct Sample {
        off64_t offset;
        size_t size;
        uint32_t duration;
        uint8_t iv[16];
        Vector<size_t> clearsizes;
        Vector<size_t> encryptedsizes;
    };
    Vector<Sample> mCurrentSamples;

    MPEG4Source(const MPEG4Source &);
    MPEG4Source &operator=(const MPEG4Source &);
};

// This custom data source wraps an existing one and satisfies requests
// falling entirely within a cached range from the cache while forwarding
// all remaining requests to the wrapped datasource.
// This is used to cache the full sampletable metadata for a single track,
// possibly wrapping multiple times to cover all tracks, i.e.
// Each MPEG4DataSource caches the sampletable metadata for a single track.

struct MPEG4DataSource : public DataSource {
    MPEG4DataSource(const sp<DataSource> &source);

    virtual status_t initCheck() const;
    virtual ssize_t readAt(off64_t offset, void *data, size_t size);
    virtual status_t getSize(off64_t *size);
    virtual uint32_t flags();

    status_t setCachedRange(off64_t offset, size_t size);

protected:
    virtual ~MPEG4DataSource();

private:
    Mutex mLock;

    sp<DataSource> mSource;
    off64_t mCachedOffset;
    size_t mCachedSize;
    uint8_t *mCache;

    void clearCache();

    MPEG4DataSource(const MPEG4DataSource &);
    MPEG4DataSource &operator=(const MPEG4DataSource &);
};

MPEG4DataSource::MPEG4DataSource(const sp<DataSource> &source)
    : mSource(source),
      mCachedOffset(0),
      mCachedSize(0),
      mCache(NULL) {
}

MPEG4DataSource::~MPEG4DataSource() {
    clearCache();
}

void MPEG4DataSource::clearCache() {
    if (mCache) {
        free(mCache);
        mCache = NULL;
    }

    mCachedOffset = 0;
    mCachedSize = 0;
}

status_t MPEG4DataSource::initCheck() const {
    return mSource->initCheck();
}

ssize_t MPEG4DataSource::readAt(off64_t offset, void *data, size_t size) {
    Mutex::Autolock autoLock(mLock);

    if (offset >= mCachedOffset
            && offset + size <= mCachedOffset + mCachedSize) {
        memcpy(data, &mCache[offset - mCachedOffset], size);
        return size;
    }

    return mSource->readAt(offset, data, size);
}

status_t MPEG4DataSource::getSize(off64_t *size) {
    return mSource->getSize(size);
}

uint32_t MPEG4DataSource::flags() {
    return mSource->flags();
}

status_t MPEG4DataSource::setCachedRange(off64_t offset, size_t size) {
    Mutex::Autolock autoLock(mLock);

    clearCache();

    mCache = (uint8_t *)malloc(size);

    if (mCache == NULL) {
        return -ENOMEM;
    }

    mCachedOffset = offset;
    mCachedSize = size;

    ssize_t err = mSource->readAt(mCachedOffset, mCache, mCachedSize);

    if (err < (ssize_t)size) {
        clearCache();

        return ERROR_IO;
    }

    return OK;
}

////////////////////////////////////////////////////////////////////////////////

static void hexdump(const void *_data, size_t size) {
    const uint8_t *data = (const uint8_t *)_data;
    size_t offset = 0;
    while (offset < size) {
        printf("0x%04x  ", offset);

        size_t n = size - offset;
        if (n > 16) {
            n = 16;
        }

        for (size_t i = 0; i < 16; ++i) {
            if (i == 8) {
                printf(" ");
            }

            if (offset + i < size) {
                printf("%02x ", data[offset + i]);
            } else {
                printf("   ");
            }
        }

        printf(" ");

        for (size_t i = 0; i < n; ++i) {
            if (isprint(data[offset + i])) {
                printf("%c", data[offset + i]);
            } else {
                printf(".");
            }
        }

        printf("\n");

        offset += 16;
    }
}

static const char *FourCC2MIME(uint32_t fourcc) {
    switch (fourcc) {
        case FOURCC('m', 'p', '4', 'a'):
            return MEDIA_MIMETYPE_AUDIO_AAC;

        case FOURCC('s', 'a', 'm', 'r'):
            return MEDIA_MIMETYPE_AUDIO_AMR_NB;

        case FOURCC('s', 'a', 'w', 'b'):
            return MEDIA_MIMETYPE_AUDIO_AMR_WB;

        case FOURCC('m', 'p', '4', 'v'):
            return MEDIA_MIMETYPE_VIDEO_MPEG4;

        case FOURCC('s', '2', '6', '3'):
        case FOURCC('h', '2', '6', '3'):
        case FOURCC('H', '2', '6', '3'):
            return MEDIA_MIMETYPE_VIDEO_H263;

        case FOURCC('a', 'v', 'c', '1'):
            return MEDIA_MIMETYPE_VIDEO_AVC;
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
		#ifdef MTK_VIDEO_HEVC_SUPPORT
		case FOURCC('h', 'v', 'c', '1'):
		case FOURCC('h', 'e', 'v', '1'):
			return MEDIA_MIMETYPE_VIDEO_HEVC;
		#endif

		case FOURCC('.', 'm', 'p', '3'):
			return MEDIA_MIMETYPE_AUDIO_MPEG;
			
        case FOURCC('a', 'c', '-', '3'):
            return MEDIA_MIMETYPE_AUDIO_AC3;

        case FOURCC('j', 'p', 'e', 'g'):
        case FOURCC('M', 'J', 'P', 'G'):
            return MEDIA_MIMETYPE_VIDEO_MJPEG;
#endif

        default:
            CHECK(!"should not be here.");
            return NULL;
    }
}

static bool AdjustChannelsAndRate(uint32_t fourcc, uint32_t *channels, uint32_t *rate) {
    if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AMR_NB, FourCC2MIME(fourcc))) {
        // AMR NB audio is always mono, 8kHz
        *channels = 1;
        *rate = 8000;
        return true;
    } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AMR_WB, FourCC2MIME(fourcc))) {
        // AMR WB audio is always mono, 16kHz
        *channels = 1;
        *rate = 16000;
        return true;
    }
    return false;
}

MPEG4Extractor::MPEG4Extractor(const sp<DataSource> &source)
    : mSidxDuration(0),
      mMoofOffset(0),
      mDataSource(source),
      mInitCheck(NO_INIT),
      mHasVideo(false),
#ifndef ANDROID_DEFAULT_CODE
      mHasAudio(false),
	#ifdef MTK_VIDEO_HEVC_SUPPORT
      mIsHevc(false),
	#endif
#endif
      mHeaderTimescale(0),
      mFirstTrack(NULL),
      mLastTrack(NULL),
      mFileMetaData(new MetaData),
      mFirstSINF(NULL),
      mIsDrm(false) {
      
#ifndef ANDROID_DEFAULT_CODE
	LOGD("=====================================\n");
    LOGD("[MP4 Playback capability info]£º\n");
    LOGD("=====================================\n");
#ifndef CC_SUPPORT_TV_STAGEFRIGHT_4K2K
    LOGD("Resolution = \"[(8,8) ~ (1920£¬1080)]\" \n");
#else
    LOGD("Resolution = \"[(8,8) ~ (3840£¬2160)]\" \n");
#endif
    LOGD("Support Codec = \"Video:MPEG4, H263, H264 ; Audio: AAC, AMR-NB/WB\" \n");
    LOGD("Profile_Level = \"MPEG4: Simple Profile ; H263: Baseline ; H264: Baseline/3.1, Main/3.1\" \n");
    LOGD("Max frameRate =  120fps \n");
    LOGD("Max Bitrate  = H264: 6Mbps  (720*480@30fps) ; MPEG4/H263: 20Mbps (864*480@30fps)\n");
    LOGD("=====================================\n");

#ifndef CC_SUPPORT_TV_STAGEFRIGHT_4K2K
	int64_t memory_size_byte = (int64_t)sysconf(_SC_PHYS_PAGES) * PAGE_SIZE;
	if (memory_size_byte > 256*1024*1024) {
		VIDEO_MAX_W_H = 1920;
		VIDEO_MAX_RESOLUTION = 1920*1088;
	}
	else {
		VIDEO_MAX_W_H = 864;
		VIDEO_MAX_RESOLUTION = 864*480;
	}

	LOGD("memory_size_byte=%lld, VIDEO_MAX_W_H=%d, VIDEO_MAX_RESOLUTION=%d", memory_size_byte, VIDEO_MAX_W_H, VIDEO_MAX_RESOLUTION);
#endif
#endif
}

MPEG4Extractor::~MPEG4Extractor() {
    Track *track = mFirstTrack;
    while (track) {
        Track *next = track->next;

        delete track;
        track = next;
    }
    mFirstTrack = mLastTrack = NULL;

    SINF *sinf = mFirstSINF;
    while (sinf) {
        SINF *next = sinf->next;
        delete sinf->IPMPData;
        delete sinf;
        sinf = next;
    }
    mFirstSINF = NULL;

    for (size_t i = 0; i < mPssh.size(); i++) {
        delete [] mPssh[i].data;
    }
}

uint32_t MPEG4Extractor::flags() const {
    return CAN_PAUSE |
            ((mMoofOffset == 0 || mSidxEntries.size() != 0) ?
                    (CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_SEEK) : 0);
}

sp<MetaData> MPEG4Extractor::getMetaData() {
    status_t err;
    if ((err = readMetaData()) != OK) {
        return new MetaData;
    }

    return mFileMetaData;
}

size_t MPEG4Extractor::countTracks() {
    status_t err;
    if ((err = readMetaData()) != OK) {
        LOGE("MPEG4Extractor::countTracks: no tracks");
        return 0;
    }

    size_t n = 0;
#ifndef ANDROID_DEFAULT_CODE
	size_t timeOffsetTrackNum = 0;
	Track *timeOffsetTrack1 = NULL;
	Track *timeOffsetTrack2 = NULL;
#endif
    Track *track = mFirstTrack;
    while (track)
    {
        ++n;
#ifndef ANDROID_DEFAULT_CODE
    	if (track->mStartTimeOffset != 0)
    	{
    		timeOffsetTrackNum++;
    		if (timeOffsetTrackNum > 2)
    		{
    			ALOGW("Unsupport edts list, %d tracks have time offset!!", timeOffsetTrackNum);
    			track->mStartTimeOffset = 0;
    			timeOffsetTrack1->mStartTimeOffset = 0;
    			timeOffsetTrack2->mStartTimeOffset = 0;
    		}
    		else
    		{
    			if (timeOffsetTrack1 == NULL)
    				timeOffsetTrack1 = track;
    			else
    			{
    				timeOffsetTrack2 = track;
    				if (timeOffsetTrack1->mStartTimeOffset > track->mStartTimeOffset)
    				{
    					timeOffsetTrack1->mStartTimeOffset -= track->mStartTimeOffset;
    					track->mStartTimeOffset = 0;
    				}
    				else
    				{
    					track->mStartTimeOffset -= timeOffsetTrack1->mStartTimeOffset;
    					timeOffsetTrack1->mStartTimeOffset = 0;
    				}
    			}
    		}
    	}
#endif
        track = track->next;
    }
    ALOGV("MPEG4Extractor::countTracks: %d tracks", n);
    return n;
}

sp<MetaData> MPEG4Extractor::getTrackMetaData(
        size_t index, uint32_t flags) {
    status_t err;
    if ((err = readMetaData()) != OK) {
        return NULL;
    }

    Track *track = mFirstTrack;
    while (index > 0) {
        if (track == NULL) {
            return NULL;
        }

        track = track->next;
        --index;
    }

    if (track == NULL) {
        return NULL;
    }

    if ((flags & kIncludeExtensiveMetaData)
            && !track->includes_expensive_metadata) {
        track->includes_expensive_metadata = true;

        const char *mime;
        CHECK(track->meta->findCString(kKeyMIMEType, &mime));
        if (!strncasecmp("video/", mime, 6)) {
            if (mMoofOffset > 0) {
                int64_t duration;
                if (track->meta->findInt64(kKeyDuration, &duration)) {
                    // nothing fancy, just pick a frame near 1/4th of the duration
                    track->meta->setInt64(
                            kKeyThumbnailTime, duration / 4);
                }
            } else {
                uint32_t sampleIndex;
                uint32_t sampleTime;
                if (track->sampleTable->findThumbnailSample(&sampleIndex) == OK
                        && track->sampleTable->getMetaDataForSample(
                            sampleIndex, NULL /* offset */, NULL /* size */,
                            &sampleTime) == OK) {
#ifndef ANDROID_DEFAULT_CODE//hai.li for Issue: ALPS32414
								if (mMovieTimeScale != 0)
									track->sampleTable->setStartTimeOffset((track->mStartTimeOffset/mMovieTimeScale)*track->timescale);
								track->meta->setInt64(
										kKeyThumbnailTime,
										((int64_t)sampleTime * 1000000 + (track->timescale >> 1)) / track->timescale
										+ ((int64_t)track->sampleTable->getStartTimeOffset())*1000000/track->timescale);
#else

                    track->meta->setInt64(
                            kKeyThumbnailTime,
                            ((int64_t)sampleTime * 1000000) / track->timescale);
#endif
                }
            }
        }
    }
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
	if (flags & kIncludeInterleaveInfo)
	{
		off64_t offset;
		track->sampleTable->getMetaDataForSample(0, &offset, NULL, NULL, NULL);
		track->meta->setInt64(kKeyFirstSampleOffset, offset);
		ALOGD("First sample offset in %s track is %lld", track->mIsVideo?"Video":"Audio", (int64_t)offset);
	}
#endif

    return track->meta;
}

static void MakeFourCCString(uint32_t x, char *s) {
    s[0] = x >> 24;
    s[1] = (x >> 16) & 0xff;
    s[2] = (x >> 8) & 0xff;
    s[3] = x & 0xff;
    s[4] = '\0';
}

status_t MPEG4Extractor::readMetaData() {
    ALOGD("+ readMetaData");
    
    if (mInitCheck != NO_INIT) 
	{
        ALOGD("return mInitCheck");
        return mInitCheck;
    }

    off64_t offset = 0;
    status_t err;
    while (true) {
        err = parseChunk(&offset, 0);
        if (err == OK) {
            continue;
        }

        uint32_t hdr[2];
        if (mDataSource->readAt(offset, hdr, 8) < 8) {
            break;
        }
        uint32_t chunk_type = ntohl(hdr[1]);
        if (chunk_type == FOURCC('s', 'i', 'd', 'x')) {
            // parse the sidx box too
            continue;
        } else if (chunk_type == FOURCC('m', 'o', 'o', 'f')) {
            // store the offset of the first segment
            mMoofOffset = offset;
        }
        break;
    }

    if (mInitCheck == OK) {
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE) //hai.li
		if (mHasAudio && !mHasVideo) {
			int32_t isOtherBrand = 0;
			if (mFileMetaData->findInt32(kKeyIs3gpBrand, &isOtherBrand) && isOtherBrand)
			{
				LOGD("File Type is audio/3gpp");
				mFileMetaData->setCString(kKeyMIMEType, "audio/3gpp");
			}
#ifdef QUICKTIME_SUPPORT
			else if (mFileMetaData->findInt32(kKeyIsQTBrand, &isOtherBrand) && isOtherBrand)
			{
				LOGD("File Type is audio/quicktime");
				mFileMetaData->setCString(kKeyMIMEType, "audio/quicktime");
			}
#endif
			else
			{
				LOGD("File Type is audio/mp4");
				mFileMetaData->setCString(kKeyMIMEType, "audio/mp4");
			}
		} else {
			int32_t isOtherBrand = 0;
			if (mHasVideo && mFileMetaData->findInt32(kKeyIs3gpBrand, &isOtherBrand) && isOtherBrand)
			{
				LOGD("File Type is video/3gpp");
				mFileMetaData->setCString(kKeyMIMEType, "video/3gpp");
			}
#ifdef QUICKTIME_SUPPORT
			else if (mFileMetaData->findInt32(kKeyIsQTBrand, &isOtherBrand) && isOtherBrand)
			{
				LOGD("File Type is video/quicktime");
				mFileMetaData->setCString(kKeyMIMEType, "video/quicktime");
			}
#endif
			else
			{
				#ifdef MTK_VIDEO_HEVC_SUPPORT
				LOGD("mIsHevc = %s", mIsHevc?"TRUE":"FALSE");
				if (mIsHevc) 
				{
					LOGD("File Type is video/hevc");
				}
				else
				#endif
				{
					LOGD("File Type is video/mp4");
				}
				mFileMetaData->setCString(kKeyMIMEType, "video/mp4");
			}
		}

#else
        if (mHasVideo) {
            mFileMetaData->setCString(kKeyMIMEType, "video/mp4");
        } else {
            mFileMetaData->setCString(kKeyMIMEType, "audio/mp4");
        }
#endif
        mInitCheck = OK;
    }
    else 
    {
        mInitCheck = err;
        LOGE("mInitCheck = err(%d) @line%d", err, __LINE__);
    }

    CHECK_NE(err, (status_t)NO_INIT);

    // copy pssh data into file metadata
    int psshsize = 0;
    for (size_t i = 0; i < mPssh.size(); i++) {
        psshsize += 20 + mPssh[i].datalen;
    }
    if (psshsize) {
        char *buf = (char*)malloc(psshsize);
        char *ptr = buf;
        for (size_t i = 0; i < mPssh.size(); i++) {
            memcpy(ptr, mPssh[i].uuid, 20); // uuid + length
            memcpy(ptr + 20, mPssh[i].data, mPssh[i].datalen);
            ptr += (20 + mPssh[i].datalen);
        }
        mFileMetaData->setData(kKeyPssh, 'pssh', buf, psshsize);
        free(buf);
    }
    
    ALOGD("- readMetaData");
    return mInitCheck;
}

char* MPEG4Extractor::getDrmTrackInfo(size_t trackID, int *len) {
    if (mFirstSINF == NULL) {
        return NULL;
    }

    SINF *sinf = mFirstSINF;
    while (sinf && (trackID != sinf->trackID)) {
        sinf = sinf->next;
    }

    if (sinf == NULL) {
        return NULL;
    }

    *len = sinf->len;
    return sinf->IPMPData;
}

// Reads an encoded integer 7 bits at a time until it encounters the high bit clear.
static int32_t readSize(off64_t offset,
        const sp<DataSource> DataSource, uint8_t *numOfBytes) {
    uint32_t size = 0;
    uint8_t data;
    bool moreData = true;
    *numOfBytes = 0;

    while (moreData) {
        if (DataSource->readAt(offset, &data, 1) < 1) {
            return -1;
        }
        offset ++;
        moreData = (data >= 128) ? true : false;
        size = (size << 7) | (data & 0x7f); // Take last 7 bits
        (*numOfBytes) ++;
    }

    return size;
}

status_t MPEG4Extractor::parseDrmSINF(off64_t *offset, off64_t data_offset) {
    uint8_t updateIdTag;
    if (mDataSource->readAt(data_offset, &updateIdTag, 1) < 1) 
	{
        return ERROR_IO;
    }
    data_offset ++;

    if (0x01/*OBJECT_DESCRIPTOR_UPDATE_ID_TAG*/ != updateIdTag) 
	{
        return ERROR_MALFORMED;
    }

    uint8_t numOfBytes;
    int32_t size = readSize(data_offset, mDataSource, &numOfBytes);
    if (size < 0) 
	{
        return ERROR_IO;
    }
    int32_t classSize = size;
    data_offset += numOfBytes;

    while(size >= 11 ) {
        uint8_t descriptorTag;
        if (mDataSource->readAt(data_offset, &descriptorTag, 1) < 1) 
		{
            return ERROR_IO;
        }
        data_offset ++;

        if (0x11/*OBJECT_DESCRIPTOR_ID_TAG*/ != descriptorTag) 
		{
            return ERROR_MALFORMED;
        }

        uint8_t buffer[8];
        //ObjectDescriptorID and ObjectDescriptor url flag
        if (mDataSource->readAt(data_offset, buffer, 2) < 2) 
		{
            return ERROR_IO;
        }
        data_offset += 2;

        if ((buffer[1] >> 5) & 0x0001) //url flag is set
		{ 
            return ERROR_MALFORMED;
        }

        if (mDataSource->readAt(data_offset, buffer, 8) < 8) 
		{
            return ERROR_IO;
        }
        data_offset += 8;

        if ((0x0F/*ES_ID_REF_TAG*/ != buffer[1])
                || ( 0x0A/*IPMP_DESCRIPTOR_POINTER_ID_TAG*/ != buffer[5])) 
        {
            return ERROR_MALFORMED;
        }

        SINF *sinf = new SINF;
        sinf->trackID = U16_AT(&buffer[3]);
        sinf->IPMPDescriptorID = buffer[7];
        sinf->next = mFirstSINF;
        mFirstSINF = sinf;

        size -= (8 + 2 + 1);
    }

    if (size != 0) 
	{
        return ERROR_MALFORMED;
    }

    if (mDataSource->readAt(data_offset, &updateIdTag, 1) < 1) 
	{
        return ERROR_IO;
    }
    data_offset ++;

    if(0x05/*IPMP_DESCRIPTOR_UPDATE_ID_TAG*/ != updateIdTag) 
	{
        return ERROR_MALFORMED;
    }

    size = readSize(data_offset, mDataSource, &numOfBytes);
    if (size < 0) 
	{
        return ERROR_IO;
    }
    classSize = size;
    data_offset += numOfBytes;

    while (size > 0) 
	{
        uint8_t tag;
        int32_t dataLen;
        if (mDataSource->readAt(data_offset, &tag, 1) < 1) 
		{
            return ERROR_IO;
        }
        data_offset ++;

        if (0x0B/*IPMP_DESCRIPTOR_ID_TAG*/ == tag) 
		{
            uint8_t id;
            dataLen = readSize(data_offset, mDataSource, &numOfBytes);
            if (dataLen < 0) 
			{
                return ERROR_IO;
            } 
			else if (dataLen < 4) 
            {
                return ERROR_MALFORMED;
            }
            data_offset += numOfBytes;

            if (mDataSource->readAt(data_offset, &id, 1) < 1) 
			{
                return ERROR_IO;
            }
            data_offset ++;

            SINF *sinf = mFirstSINF;
            while (sinf && (sinf->IPMPDescriptorID != id)) 
			{
                sinf = sinf->next;
            }
			
            if (sinf == NULL) 
			{
                return ERROR_MALFORMED;
            }
            sinf->len = dataLen - 3;
            sinf->IPMPData = new char[sinf->len];

            if (mDataSource->readAt(data_offset + 2, sinf->IPMPData, sinf->len) < sinf->len) 
			{
                return ERROR_IO;
            }
            data_offset += sinf->len;

            size -= (dataLen + numOfBytes + 1);
        }
    }

    if (size != 0) 
	{
        return ERROR_MALFORMED;
    }

    return UNKNOWN_ERROR;  // Return a dummy error.
}

#if !defined(ANDROID_DEFAULT_CODE)
#ifdef MP4_PRINT_PATH
static char pathsymbol[100];
static char type[5];
static const char* symbol = "===";
static int npath_deep;
#endif
#endif

struct PathAdder {
    PathAdder(Vector<uint32_t> *path, uint32_t chunkType)
        : mPath(path) {
        mPath->push(chunkType);

#if !defined(ANDROID_DEFAULT_CODE)
#ifdef MP4_PRINT_PATH
    	MakeFourCCString(chunkType, type);
		strcat(pathsymbol, symbol);		
		LOGI("%s%s", pathsymbol, type);
		npath_deep++;
#endif
#endif
    }

    ~PathAdder() {
        mPath->pop();
		
#if !defined(ANDROID_DEFAULT_CODE)
#ifdef MP4_PRINT_PATH
		pathsymbol[(--npath_deep)*3] = '\0';
#endif
#endif
    }

private:
    Vector<uint32_t> *mPath;

    PathAdder(const PathAdder &);
    PathAdder &operator=(const PathAdder &);
};

static bool underMetaDataPath(const Vector<uint32_t> &path) {
    return path.size() >= 5
        && path[0] == FOURCC('m', 'o', 'o', 'v')
        && path[1] == FOURCC('u', 'd', 't', 'a')
        && path[2] == FOURCC('m', 'e', 't', 'a')
        && path[3] == FOURCC('i', 'l', 's', 't');
}

// Given a time in seconds since Jan 1 1904, produce a human-readable string.
static void convertTimeToDate(int64_t time_1904, String8 *s) {
    time_t time_1970 = time_1904 - (((66 * 365 + 17) * 24) * 3600);

    char tmp[32];
    strftime(tmp, sizeof(tmp), "%Y%m%dT%H%M%S.000Z", gmtime(&time_1970));

    s->setTo(tmp);
}

status_t MPEG4Extractor::parseChunk(off64_t *offset, int depth) {
    ALOGV("entering parseChunk %lld/%d", *offset, depth);
    uint32_t hdr[2];
    if (mDataSource->readAt(*offset, hdr, 8) < 8) 
	{
        LOGE("readAt(%lld) failed @line%d", *offset, __LINE__);
        return ERROR_IO;
    }
    uint64_t chunk_size = ntohl(hdr[0]);
    uint32_t chunk_type = ntohl(hdr[1]);
    off64_t data_offset = *offset + 8;

    if (chunk_size == 1) 
	{
        if (mDataSource->readAt(*offset + 8, &chunk_size, 8) < 8) 
		{
            LOGE("readAt(%lld) failed @line%d", *offset + 8, __LINE__);
            return ERROR_IO;
        }
		
        chunk_size = ntoh64(chunk_size);
        data_offset += 8;

        if (chunk_size < 16) 
		{
            // The smallest valid chunk is 16 bytes long in this case.
            LOGE("Chunk size %lld too small! @line%d", chunk_size, __LINE__);
            return ERROR_MALFORMED;
        }
    } 
	else if (chunk_size < 8) 
    {
        // The smallest valid chunk is 8 bytes long.
        LOGE("Chunk size %lld too small! @line%d", chunk_size, __LINE__);
        return ERROR_MALFORMED;
    }

    char chunk[5];
    MakeFourCCString(chunk_type, chunk);
    off64_t chunk_data_size = *offset + chunk_size - data_offset;
	
    LOGI("chunk: %s, size: %lld , data size: %lld @ %lld, %d", chunk, chunk_size, chunk_data_size, *offset, depth);
	PathAdder autoAdder(&mPath, chunk_type);

    if (chunk_type != FOURCC('c', 'p', 'r', 't')
            && chunk_type != FOURCC('c', 'o', 'v', 'r')
            && mPath.size() == 5 && underMetaDataPath(mPath)) 
    {
        off64_t stop_offset = *offset + chunk_size;
        *offset = data_offset;
        while (*offset < stop_offset) 
		{
            status_t err = parseChunk(offset, depth + 1);
            if (err != OK) 
			{
                LOGE("parseChunk @%lld failed!", *offset);
                return err;
            }
        }

        if (*offset != stop_offset) 
		{
            LOGE("parseChunk finished@%lld but stop offset is %lld!", *offset, stop_offset);
            return ERROR_MALFORMED;
        }

        return OK;
    }

    switch(chunk_type) {
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
		case FOURCC('f', 't', 'y', 'p'):
		{
			uint8_t header[4];
            if (mDataSource->readAt(data_offset, header, 4) < 4) 
			{
                return ERROR_IO;
            }
			//ALOGD("HEADER=%x,%x,%x,%x", header[0], header[1], header[2], header[3]);
			if (!memcmp(header, "3gp", 3))
			{
				LOGD("3GPP is true");
				mFileMetaData->setInt32(kKeyIs3gpBrand, true);
			}
#ifdef QUICKTIME_SUPPORT
			else if (!memcmp(header, "qt", 2))
			{
				mFileMetaData->setInt32(kKeyIsQTBrand, true);
			}
#endif


            *offset += chunk_size;
            break;
		}
#endif
        case FOURCC('m', 'o', 'o', 'v'):
        case FOURCC('t', 'r', 'a', 'k'):
        case FOURCC('m', 'd', 'i', 'a'):
        case FOURCC('m', 'i', 'n', 'f'):
        case FOURCC('d', 'i', 'n', 'f'):
        case FOURCC('s', 't', 'b', 'l'):
        case FOURCC('m', 'v', 'e', 'x'):
        case FOURCC('m', 'o', 'o', 'f'):
        case FOURCC('t', 'r', 'a', 'f'):
        case FOURCC('m', 'f', 'r', 'a'):
        case FOURCC('u', 'd', 't', 'a'):
        case FOURCC('i', 'l', 's', 't'):
#ifndef ANDROID_DEFAULT_CODE
		//case FOURCC('e', 'd', 't', 's'): added by hai.li to support track time offset
#ifdef QUICKTIME_SUPPORT
		case FOURCC('w', 'a', 'v', 'e'): //for .mov
#endif
#endif
        case FOURCC('s', 'i', 'n', 'f'):
        case FOURCC('s', 'c', 'h', 'i'):
        case FOURCC('e', 'd', 't', 's'):
        {
            if (chunk_type == FOURCC('s', 't', 'b', 'l')) {
                ALOGV("sampleTable chunk is %d bytes long.", (size_t)chunk_size);

                if (mDataSource->flags()
                        & (DataSource::kWantsPrefetching
                            | DataSource::kIsCachingDataSource)) {
                    sp<MPEG4DataSource> cachedSource =
                        new MPEG4DataSource(mDataSource);

                    if (cachedSource->setCachedRange(*offset, chunk_size) == OK) {
                        mDataSource = cachedSource;
                    }
                }

                mLastTrack->sampleTable = new SampleTable(mDataSource);
            }

            bool isTrack = false;
            if (chunk_type == FOURCC('t', 'r', 'a', 'k')) {
                isTrack = true;

                Track *track = new Track;
                track->next = NULL;
                if (mLastTrack) {
                    mLastTrack->next = track;
                } else {
                    mFirstTrack = track;
                }
                mLastTrack = track;

                track->meta = new MetaData;
                track->includes_expensive_metadata = false;
                track->skipTrack = false;
                track->timescale = 0;
                track->meta->setCString(kKeyMIMEType, "application/octet-stream");
            }

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset;
            while (*offset < stop_offset) {
#if !defined(ANDROID_DEFAULT_CODE) && defined(QUICKTIME_SUPPORT)//for .mov file
				if (stop_offset - *offset == 4
						&& chunk_type == FOURCC('u', 'd', 't', 'a')) {
					uint32_t terminate_code;
					mDataSource->readAt(*offset, &terminate_code, 4);
					if (0 == terminate_code)
					{
						*offset += 4;//terminate code 0x00000000
						LOGD("Terminal code for 0x%8.8x", chunk_type);
					}
				}
				else {
					status_t err = parseChunk(offset, depth + 1);
					if (err != OK) {
						return err;
					}
				}
#else
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
#endif
            }

#if !defined(ANDROID_DEFAULT_CODE)
            if(*offset != stop_offset){
				if(chunk_type == FOURCC('s', 't', 'b', 'l'))
				{
					LOGE("STBL ERROR_MALFORMED, Skip this track, but still try to parse file left");
					mLastTrack->skipTrack = true;
					*offset = stop_offset;
				}
				else
				{
					return ERROR_MALFORMED;
				}
            }
#else
			if (*offset != stop_offset) {
		        return ERROR_MALFORMED;
			}
#endif

            if (isTrack) {
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)//hai.li
				if (mLastTrack->durationUs == 0)
				{
					LOGE("%s track duration is 0", mLastTrack->mIsVideo?"Video": "Audio");
					mLastTrack->skipTrack = true;
					//return UNKNOWN_ERROR;
				}
				if(mLastTrack->sampleCount ==0)
				{
					LOGE("%s track sampleCount is 0, maybe have data reference in the next box...", mLastTrack->mIsVideo?"Video": "Audio");
					//mLastTrack->skipTrack = true;
				}
				if (mLastTrack->mIsVideo)
				{
					//CHECK(mLastTrack->durationUs != 0);
					if (mLastTrack->durationUs != 0)
					{
						int64_t frame_rate = mLastTrack->sampleCount * 1000000LL / mLastTrack->durationUs;
						const char* mime;
						if ((frame_rate > VIDEO_MAX_FPS) && (mLastTrack->sampleCount > 1))
						{
							LOGE("[MP4 capability error]Unsupport video frame rate!!!fps = %lld", frame_rate);
							mLastTrack->skipTrack = true;
							mHasVideo = false;
						}
						else if (mLastTrack->meta->findCString(kKeyMIMEType, &mime) &&
								!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG4))
						{
							int32_t isCodecInfoInFirstFrame;
							if (mLastTrack->meta->findInt32(kKeyCodecInfoIsInFirstFrame, &isCodecInfoInFirstFrame)
								&& (isCodecInfoInFirstFrame != 0))
							{
								status_t err = setCodecInfoFromFirstFrame(mLastTrack);
								if (err != OK) {
									LOGE("setCodecInfoFromFirstFrame error %d", err);
									return err;
								}
							}
						}
					}
					if (mLastTrack->skipTrack)
					{
						mFileMetaData->setInt32(kKeyHasUnsupportVideo, true);
						LOGD("MP4 has unsupport video track");
						mHasVideo = false;
					}
				}
		 	// <--- Morris Yang check audio
				else if (mLastTrack->mIsAudio){
					int32_t num_channels;
                    //Remove multi-ch restriction in Extractor
					/*if (mLastTrack->meta->findInt32(kKeyChannelCount, &num_channels) && (num_channels > 2)) {
						LOGE ("[MP4 capability error]Unsupported num of channels: (%d), ignore audio track", num_channels);
						mLastTrack->skipTrack = true;
						mHasAudio = false;//hai.li
					}*/
					mLastTrack->meta->findInt32(kKeyChannelCount, &num_channels);
					LOGE ("[MP4 capability]num of channels: (%d),not ignore audio track", num_channels);

					const char *mime;
					int32_t aacObjectType;
					if (mLastTrack->meta->findCString(kKeyMIMEType, &mime)) {
					    if ((!strcasecmp(MEDIA_MIMETYPE_AUDIO_AAC, mime)) && mLastTrack->meta->findInt32(kKeyAacObjType, &aacObjectType)) {

						if ((aacObjectType != 2)    // AAC LC (Low Complexity)
						  && (aacObjectType != 4)      // AAC LTP (Long Term Prediction)
						  && (aacObjectType != 5)      // SBR (Spectral Band Replication)
					         && (aacObjectType != 29))   // PS (Parametric Stereo)
						{
							LOGE ("[AAC capability error]Unsupported audio object type: (%d), , ignore audio track", aacObjectType);
							mLastTrack->skipTrack = true;
							mHasAudio = false;//hai.li
						}
					    }
					}
					if (mLastTrack->skipTrack)
					{
						mHasAudio = false;
					}
				}
				else {//not video or audio track
					LOGI("Not a video/audio track");
				}
			// --->
#endif//#ifndef ANDROID_DEFAULT_CODE
                if (mLastTrack->skipTrack) {
                    Track *cur = mFirstTrack;

                    if (cur == mLastTrack) {
                        delete cur;
                        mFirstTrack = mLastTrack = NULL;
                    } else {
                        while (cur && cur->next != mLastTrack) {
                            cur = cur->next;
                        }
                        cur->next = NULL;
                        delete mLastTrack;
                        mLastTrack = cur;
                    }

                    return OK;
                }

                status_t err = verifyTrack(mLastTrack);

                if (err != OK) {
					LOGE("verifyTrack Fail");
                    return err;
                }
            } else if (chunk_type == FOURCC('m', 'o', 'o', 'v')) {
                mInitCheck = OK;

                if (!mIsDrm) {
                    return UNKNOWN_ERROR;  // Return a dummy error.
                } else {
                    return OK;
                }
            }
            break;
        }
#ifdef ANDROID_DEFAULT_CODE
        case FOURCC('e', 'l', 's', 't'):
        {
            // See 14496-12 8.6.6
            uint8_t version;
            if (mDataSource->readAt(data_offset, &version, 1) < 1) {
                return ERROR_IO;
            }

            uint32_t entry_count;
            if (!mDataSource->getUInt32(data_offset + 4, &entry_count)) 
			{
                return ERROR_IO;
            }

            if (entry_count != 1) {
                // we only support a single entry at the moment, for gapless playback
                ALOGW("ignoring edit list with %d entries", entry_count);
            } else if (mHeaderTimescale == 0) {
                ALOGW("ignoring edit list because timescale is 0");
            } else {
                off64_t entriesoffset = data_offset + 8;
                uint64_t segment_duration;
                int64_t media_time;

                if (version == 1) {
                    if (!mDataSource->getUInt64(entriesoffset, &segment_duration) ||
                            !mDataSource->getUInt64(entriesoffset + 8, (uint64_t*)&media_time)) {
                        return ERROR_IO;
                    }
                } else if (version == 0) {
                    uint32_t sd;
                    int32_t mt;
                    if (!mDataSource->getUInt32(entriesoffset, &sd) ||
                            !mDataSource->getUInt32(entriesoffset + 4, (uint32_t*)&mt)) {
                        return ERROR_IO;
                    }
                    segment_duration = sd;
                    media_time = mt;
                } else {
                    return ERROR_IO;
                }

                uint64_t halfscale = mHeaderTimescale / 2;
                segment_duration = (segment_duration * 1000000 + halfscale)/ mHeaderTimescale;
                media_time = (media_time * 1000000 + halfscale) / mHeaderTimescale;

                int64_t duration;
                int32_t samplerate;
                if (mLastTrack->meta->findInt64(kKeyDuration, &duration) &&
                        mLastTrack->meta->findInt32(kKeySampleRate, &samplerate)) {

                    int64_t delay = (media_time  * samplerate + 500000) / 1000000;
                    mLastTrack->meta->setInt32(kKeyEncoderDelay, delay);

                    int64_t paddingus = duration - (segment_duration + media_time);
                    if (paddingus < 0) {
                        // track duration from media header (which is what kKeyDuration is) might
                        // be slightly shorter than the segment duration, which would make the
                        // padding negative. Clamp to zero.
                        paddingus = 0;
                    }
                    int64_t paddingsamples = (paddingus * samplerate + 500000) / 1000000;
                    mLastTrack->meta->setInt32(kKeyEncoderPadding, paddingsamples);
                }
            }
            *offset += chunk_size;
            break;
        }
#endif
        case FOURCC('f', 'r', 'm', 'a'):
        {
            uint32_t original_fourcc;
            if (mDataSource->readAt(data_offset, &original_fourcc, 4) < 4) {
                return ERROR_IO;
            }
            original_fourcc = ntohl(original_fourcc);
            ALOGV("read original format: %d", original_fourcc);
            mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(original_fourcc));
            uint32_t num_channels = 0;
            uint32_t sample_rate = 0;
            if (AdjustChannelsAndRate(original_fourcc, &num_channels, &sample_rate)) {
                mLastTrack->meta->setInt32(kKeyChannelCount, num_channels);
                mLastTrack->meta->setInt32(kKeySampleRate, sample_rate);
            }
            *offset += chunk_size;
            break;
        }

        case FOURCC('t', 'e', 'n', 'c'):
        {
            if (chunk_size < 32) {
                return ERROR_MALFORMED;
            }

            // tenc box contains 1 byte version, 3 byte flags, 3 byte default algorithm id, one byte
            // default IV size, 16 bytes default KeyID
            // (ISO 23001-7)
            char buf[4];
            memset(buf, 0, 4);
            if (mDataSource->readAt(data_offset + 4, buf + 1, 3) < 3) {
                return ERROR_IO;
            }
            uint32_t defaultAlgorithmId = ntohl(*((int32_t*)buf));
            if (defaultAlgorithmId > 1) {
                // only 0 (clear) and 1 (AES-128) are valid
                return ERROR_MALFORMED;
            }

            memset(buf, 0, 4);
            if (mDataSource->readAt(data_offset + 7, buf + 3, 1) < 1) {
                return ERROR_IO;
            }
            uint32_t defaultIVSize = ntohl(*((int32_t*)buf));

            if ((defaultAlgorithmId == 0 && defaultIVSize != 0) ||
                    (defaultAlgorithmId != 0 && defaultIVSize == 0)) {
                // only unencrypted data must have 0 IV size
                return ERROR_MALFORMED;
            } else if (defaultIVSize != 0 &&
                    defaultIVSize != 8 &&
                    defaultIVSize != 16) {
                // only supported sizes are 0, 8 and 16
                return ERROR_MALFORMED;
            }

            uint8_t defaultKeyId[16];

            if (mDataSource->readAt(data_offset + 8, &defaultKeyId, 16) < 16) {
                return ERROR_IO;
            }

            mLastTrack->meta->setInt32(kKeyCryptoMode, defaultAlgorithmId);
            mLastTrack->meta->setInt32(kKeyCryptoDefaultIVSize, defaultIVSize);
            mLastTrack->meta->setData(kKeyCryptoKey, 'tenc', defaultKeyId, 16);
            *offset += chunk_size;
            break;
        }

        case FOURCC('t', 'k', 'h', 'd'):
        {
            status_t err;
            if ((err = parseTrackHeader(data_offset, chunk_data_size)) != OK) {
                return err;
            }

            *offset += chunk_size;
            break;
        }

#ifndef ANDROID_DEFAULT_CODE
		case FOURCC('e', 'l', 's', 't')://added by hai.li to support track time offset
		{
			if (chunk_data_size < 4) {
				LOGE("ERROR_MALFORMED, LINE=%d", __LINE__);
                return ERROR_MALFORMED;
            }
			uint8_t header[8];
			uint8_t version;
			uint32_t entry_count;
            if (mDataSource->readAt(
                        data_offset, header, sizeof(header))
                    < (ssize_t)sizeof(header)) {
                return ERROR_IO;
            }
			version = header[0];
			entry_count = U32_AT(&header[4]);

			LOGE("header high=%d, header low=%d, entry_count=%d", *((uint32_t*)header), *((uint32_t*)(header+4)), entry_count);
			if (entry_count > 2)
			{
				ALOGW("Unsupported edit list, entry_count=%d > 2", entry_count);//The second entry is assumed as the duration of the track and normal play
				entry_count = 2;
			}
			else if (entry_count == 2)
				ALOGW("edit list entry_count=2, Assume the second entry is the duration of the track and normal play");

			mLastTrack->mElstEntryCount = entry_count;
			mLastTrack->mElstEntries = new Track::ElstEntry[entry_count];
			if (version == 1) {
				for (uint32_t i = 0; i < entry_count; i++)
				{
					uint8_t buffer[20];
					if (mDataSource->readAt(
							data_offset+4+4+i*20, buffer, sizeof(buffer))
							< (ssize_t)sizeof(buffer)){
						return ERROR_IO;
					}
					mLastTrack->mElstEntries[i].SegDuration = U64_AT(buffer);
					mLastTrack->mElstEntries[i].MediaTime = (int64_t)U64_AT(&buffer[8]);
					mLastTrack->mElstEntries[i].MediaRateInt = (int16_t)U16_AT(&buffer[16]);
					mLastTrack->mElstEntries[i].MediaRateFrac = (int16_t)U16_AT(&buffer[18]);
				}
			} else {
				for (uint32_t i = 0; i < entry_count; i++)
				{
					uint8_t buffer[12];
					if (mDataSource->readAt(
							data_offset+4+4+i*12, buffer, sizeof(buffer))
							< (ssize_t)sizeof(buffer)){
						return ERROR_IO;
					}
					mLastTrack->mElstEntries[i].SegDuration = U32_AT(buffer);
					if (0xffffffff == U32_AT(&buffer[4]))
						mLastTrack->mElstEntries[i].MediaTime = -1LL;
					else
						mLastTrack->mElstEntries[i].MediaTime = (int64_t)U32_AT(&buffer[4]);
					ALOGD("MediaTime=%d, entry.mediatime=%lld, i=%d", U32_AT(&buffer[4]), mLastTrack->mElstEntries[i].MediaTime, i);
					ALOGD("SegDuration=%d, entry.SegDuration=%lld, i=%d", U32_AT(&buffer[0]), mLastTrack->mElstEntries[i].SegDuration, i);
					mLastTrack->mElstEntries[i].MediaRateInt = (int16_t)U16_AT(&buffer[8]);
					mLastTrack->mElstEntries[i].MediaRateFrac = (int16_t)U16_AT(&buffer[10]);
				}
			}
			if ((mLastTrack->mElstEntries[0].MediaRateInt != 1) ||
					(mLastTrack->mElstEntries[0].MediaRateFrac != 0)){
					ALOGW("Unsupported edit list, MediaRate=%d.%d != 1",
							mLastTrack->mElstEntries[0].MediaRateInt, mLastTrack->mElstEntries[0].MediaRateFrac);
			}

			if (mLastTrack->mElstEntries[0].SegDuration > (uint64_t)((1LL>>32)-1))
			{
				ALOGW("Unsupported edit list, TimeOffset=%lld", mLastTrack->mElstEntries[0].SegDuration);
				mLastTrack->mStartTimeOffset = 0;
			}
			else if (mLastTrack->mElstEntries[0].MediaTime != -1){//added by vend_am00032
				mLastTrack->mStartTimeOffset = 0;
				ALOGW("Unsupported edit list, MediaTime=%lld", mLastTrack->mElstEntries[0].MediaTime);
			}
			else {
				mLastTrack->mStartTimeOffset = (uint32_t)mLastTrack->mElstEntries[0].SegDuration;
			}
			*offset += chunk_size;
			break;
		}
#endif
        case FOURCC('p', 's', 's', 'h'):
        {
            PsshInfo pssh;

            if (mDataSource->readAt(data_offset + 4, &pssh.uuid, 16) < 16) {
                return ERROR_IO;
            }

            uint32_t psshdatalen = 0;
            if (mDataSource->readAt(data_offset + 20, &psshdatalen, 4) < 4) {
                return ERROR_IO;
            }
            pssh.datalen = ntohl(psshdatalen);
            ALOGV("pssh data size: %d", pssh.datalen);
            if (pssh.datalen + 20 > chunk_size) {
                // pssh data length exceeds size of containing box
                return ERROR_MALFORMED;
            }

            pssh.data = new uint8_t[pssh.datalen];
            ALOGV("allocated pssh @ %p", pssh.data);
            ssize_t requested = (ssize_t) pssh.datalen;
            if (mDataSource->readAt(data_offset + 24, pssh.data, requested) < requested) {
                return ERROR_IO;
            }
            mPssh.push_back(pssh);

            *offset += chunk_size;
            break;
        }
		#ifdef MTK_VIDEO_HEVC_SUPPORT
		case FOURCC('k', 'y', 'w', 'd'):
		{
			LOGD("kywd");
			if (mPath.size() >= 3
				&& mPath[mPath.size() - 2] == FOURCC('u', 'd', 't', 'a')
				&& mPath[mPath.size() - 3] == FOURCC('t', 'r', 'a', 'k')) 
			{
			  if (chunk_data_size >= 16) 
			  {
				uint8_t buffer[16];
				if (mDataSource->readAt(data_offset, buffer, 16) < 16) 
				{
					return ERROR_IO;
				}
				const char* mtk_hevc_tag = "HEVC_MTK";
				if (!memcmp(mtk_hevc_tag, buffer+8, 8)) 
				{
					ALOGD("HEVC_MTK, setTrackMIMEType: video/hevc");
					mIsHevc = true;
					mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_HEVC);
				 }
			   }
			}
			*offset += chunk_size;
			break;
		}
		#endif

        case FOURCC('m', 'd', 'h', 'd'):
        {
            if (chunk_data_size < 4) {
                return ERROR_MALFORMED;
            }

            uint8_t version;
            if (mDataSource->readAt(
                        data_offset, &version, sizeof(version))
                    < (ssize_t)sizeof(version)) {
                return ERROR_IO;
            }

            off64_t timescale_offset;

            if (version == 1) {
                timescale_offset = data_offset + 4 + 16;
            } else if (version == 0) {
                timescale_offset = data_offset + 4 + 8;
            } else {
                return ERROR_IO;
            }

            uint32_t timescale;
            if (mDataSource->readAt(
                        timescale_offset, &timescale, sizeof(timescale))
                    < (ssize_t)sizeof(timescale)) {
                return ERROR_IO;
            }

            mLastTrack->timescale = ntohl(timescale);

            int64_t duration = 0;
            if (version == 1) {
                if (mDataSource->readAt(
                            timescale_offset + 4, &duration, sizeof(duration))
                        < (ssize_t)sizeof(duration)) {
                    return ERROR_IO;
                }
                duration = ntoh64(duration);
            } else {
                uint32_t duration32;
                if (mDataSource->readAt(
                            timescale_offset + 4, &duration32, sizeof(duration32))
                        < (ssize_t)sizeof(duration32)) {
                    return ERROR_IO;
                }
                // ffmpeg sets duration to -1, which is incorrect.
                if (duration32 != 0xffffffff) {
                    duration = ntohl(duration32);
                }
            }
#ifndef ANDROID_DEFAULT_CODE//hai.li
			mLastTrack->durationUs = (duration * 1000000) / mLastTrack->timescale;
			mLastTrack->meta->setInt64(
					kKeyDuration, mLastTrack->durationUs);
#else
            mLastTrack->meta->setInt64(
                    kKeyDuration, (duration * 1000000) / mLastTrack->timescale);
#endif
            uint8_t lang[2];
            off64_t lang_offset;
            if (version == 1) {
                lang_offset = timescale_offset + 4 + 8;
            } else if (version == 0) {
                lang_offset = timescale_offset + 4 + 4;
            } else {
                return ERROR_IO;
            }

            if (mDataSource->readAt(lang_offset, &lang, sizeof(lang))
                    < (ssize_t)sizeof(lang)) {
                return ERROR_IO;
            }

            // To get the ISO-639-2/T three character language code
            // 1 bit pad followed by 3 5-bits characters. Each character
            // is packed as the difference between its ASCII value and 0x60.
            char lang_code[4];
            lang_code[0] = ((lang[0] >> 2) & 0x1f) + 0x60;
            lang_code[1] = ((lang[0] & 0x3) << 3 | (lang[1] >> 5)) + 0x60;
            lang_code[2] = (lang[1] & 0x1f) + 0x60;
            lang_code[3] = '\0';

            mLastTrack->meta->setCString(
                    kKeyMediaLanguage, lang_code);

            *offset += chunk_size;
            break;
        }

        case FOURCC('s', 't', 's', 'd'):
        {
            if (chunk_data_size < 8) {
                return ERROR_MALFORMED;
            }

            uint8_t buffer[8];
            if (chunk_data_size < (off64_t)sizeof(buffer)) {
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, 8) < 8) {
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) {
                // Should be version 0, flags 0.
                return ERROR_MALFORMED;
            }

            uint32_t entry_count = U32_AT(&buffer[4]);

            if (entry_count > 1) {
                // For 3GPP timed text, there could be multiple tx3g boxes contain
                // multiple text display formats. These formats will be used to
                // display the timed text.
                // For encrypted files, there may also be more than one entry.
                const char *mime;
                CHECK(mLastTrack->meta->findCString(kKeyMIMEType, &mime));
                if (strcasecmp(mime, MEDIA_MIMETYPE_TEXT_3GPP) &&
                        strcasecmp(mime, "application/octet-stream")) {
                    // For now we only support a single type of media per track.
                    LOGI("Only support single type media per track, skip!");
                    mLastTrack->skipTrack = true;
                    *offset += chunk_size;
                    break;
                }
            }
            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + 8;
            for (uint32_t i = 0; i < entry_count; ++i) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('m', 'p', '4', 'a'):
        case FOURCC('e', 'n', 'c', 'a'):
        case FOURCC('s', 'a', 'm', 'r'):
        case FOURCC('s', 'a', 'w', 'b'):
#ifndef ANDROI_DEFAULT_CODE
		case FOURCC('.', 'm', 'p', '3'):
        case FOURCC('a', 'c', '-', '3'):
#endif
        {
#ifndef ANDROID_DEFAULT_CODE//hai.li
			mHasAudio = true;
			mLastTrack->mIsAudio = true;
#endif
            uint8_t buffer[8 + 20];
            if (chunk_data_size < (ssize_t)sizeof(buffer)) {
#if !defined(ANDROID_DEFAULT_CODE) && defined(QUICKTIME_SUPPORT)//for .mov file

				if (mPath.size() >= 2
						&& mPath[mPath.size() - 2] == FOURCC('w', 'a', 'v', 'e')) {
					*offset += chunk_size;
					return OK;
				}
				else {
					LOGE("ERROR_MALFORMED, LINE=%d", __LINE__);
					return ERROR_MALFORMED;
				}
#else
                // Basic AudioSampleEntry size.
                return ERROR_MALFORMED;
#endif
            }

            if (mDataSource->readAt(
                        data_offset, buffer, sizeof(buffer)) < (ssize_t)sizeof(buffer)) {
                return ERROR_IO;
            }

            uint16_t data_ref_index = U16_AT(&buffer[6]);
            uint32_t num_channels = U16_AT(&buffer[16]);

            uint16_t sample_size = U16_AT(&buffer[18]);
            uint32_t sample_rate = U32_AT(&buffer[24]) >> 16;

            if (chunk_type != FOURCC('e', 'n', 'c', 'a')) {
                // if the chunk type is enca, we'll get the type from the sinf/frma box later
                ALOGV("set audio track mime: %s\n", FourCC2MIME(chunk_type));
                mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(chunk_type));
                AdjustChannelsAndRate(chunk_type, &num_channels, &sample_rate);
            }
            LOGV("*** coding='%s' %d channels, size %d, rate %d\n",
                   chunk, num_channels, sample_size, sample_rate);
            mLastTrack->meta->setInt32(kKeyChannelCount, num_channels);
            mLastTrack->meta->setInt32(kKeySampleRate, sample_rate);

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + sizeof(buffer);
#ifndef ANDROID_DEFAULT_CODE
#ifdef QUICKTIME_SUPPORT//hai.li for .mov
			if (1 == U16_AT(&buffer[8]))//sound media version == 1
				*offset += 16;//4*4byte
#endif
            //Remove multi-ch restriction in Extractor
			/*if (num_channels > 2)
			{
				mLastTrack->skipTrack = true;
				*offset = stop_offset;
			}*/
			LOGE ("audio num of channels: (%d)", num_channels);
#endif
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('m', 'p', '4', 'v'):
        case FOURCC('e', 'n', 'c', 'v'):
        case FOURCC('s', '2', '6', '3'):
        case FOURCC('H', '2', '6', '3'):
        case FOURCC('h', '2', '6', '3'):
		#ifdef MTK_VIDEO_HEVC_SUPPORT
		case FOURCC('h', 'v', 'c', '1'):
		case FOURCC('h', 'e', 'v', '1'):
		#endif
        case FOURCC('a', 'v', 'c', '1'):
        case FOURCC('j', 'p', 'e', 'g'):
        case FOURCC('M', 'J', 'E', 'G'):
        {
            mHasVideo = true;

            uint8_t buffer[78];
            if (chunk_data_size < (ssize_t)sizeof(buffer)) {
                // Basic VideoSampleEntry size.
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, sizeof(buffer)) < (ssize_t)sizeof(buffer)) {
                return ERROR_IO;
            }

            uint16_t data_ref_index = U16_AT(&buffer[6]);
            uint16_t width = U16_AT(&buffer[6 + 18]);
            uint16_t height = U16_AT(&buffer[6 + 20]);

            // The video sample is not standard-compliant if it has invalid dimension.
            // Use some default width and height value, and
            // let the decoder figure out the actual width and height (and thus
            // be prepared for INFO_FOMRAT_CHANGED event).
            if (width == 0)  width  = 352;
            if (height == 0) height = 288;

            LOGD("*** coding='%s' data_ref_index=%d width=%d height=%d\n", chunk, data_ref_index, width, height);
			#ifndef ANDROID_DEFAULT_CODE
			#ifdef MTK_VIDEO_HEVC_SUPPORT
			const char *mime;
			if(chunk_type == FOURCC('a', 'v', 'c', '1'))
			{
				if(mLastTrack->meta->findCString(kKeyMIMEType, &mime) &&
					!strcasecmp(MEDIA_MIMETYPE_VIDEO_HEVC, mime)) 
				{
				
                    mLastTrack->meta->setCString(kKeyMIMEType,MEDIA_MIMETYPE_VIDEO_HEVC);
					ALOGD("Is MTK HEVC not AVC");
				}
				else
				{
				    mLastTrack->meta->setCString(kKeyMIMEType,MEDIA_MIMETYPE_VIDEO_AVC);
					ALOGD("Is AVC");
				}
			}
			else if(chunk_type == FOURCC('h', 'v', 'c', '1') || 
				   chunk_type == FOURCC('h', 'e', 'v', '1')) 
			{
				ALOGD("Is Standard HEVC");
                ALOGD("set video track mime: %s\n", FourCC2MIME(chunk_type));
                mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(chunk_type));
			}
			else
			#endif
			#endif
            if(chunk_type != FOURCC('e', 'n', 'c', 'v')) 
			{
                // if the chunk type is encv, we'll get the type from the sinf/frma box later
                LOGV("set video track mime: %s\n", FourCC2MIME(chunk_type));
                mLastTrack->meta->setCString(kKeyMIMEType, FourCC2MIME(chunk_type));
            }
            mLastTrack->meta->setInt32(kKeyWidth, width);
            mLastTrack->meta->setInt32(kKeyHeight, height);
			#ifndef ANDROID_DEFAULT_CODE//hai.li
			mLastTrack->mIsVideo = true;
			if(chunk_type != FOURCC('e', 'n', 'c', 'v'))
			{
				if (!strcasecmp(MEDIA_MIMETYPE_VIDEO_H263, FourCC2MIME(chunk_type))) 
				{
					mLastTrack->mIsH263 = true;
				}
			}

			if ((width > VIDEO_MAX_W_H) || (height > VIDEO_MAX_W_H) ||
				(width*height > VIDEO_MAX_RESOLUTION) || (width <= 0) || (height <= 0))
			{
				LOGE("[MP4 capability error]Unsupport video demension!!!width=%d, height=%d", width, height);
				mLastTrack->skipTrack = true;
				mHasVideo = false;
			}
			#endif//#ifndef ANDROID_DEFAULT_CODE

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + sizeof(buffer);
            while (*offset < stop_offset) 
			{
				#if !defined(ANDROID_DEFAULT_CODE) && defined(QUICKTIME_SUPPORT)//for .mov file
				if (stop_offset - *offset < 8)
				{
					*offset = stop_offset;//Maybe terminate box? 0x00000000
				}
				else 
				{
					status_t err = parseChunk(offset, depth + 1);
					if(err != OK) 
					{
						return err;
					}
				}
				#else
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) 
				{
                    return err;
                }
				#endif
            }

            if (*offset != stop_offset) 
			{
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('s', 't', 'c', 'o'):
        case FOURCC('c', 'o', '6', '4'):
        {
            status_t err =
                mLastTrack->sampleTable->setChunkOffsetParams(
                        chunk_type, data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }
#ifndef ANDROID_DEFAULT_CODE
			if (mLastTrack->mIsH263)
			{
				uint8_t header;
				uint64_t offset;
				if (chunk_type == FOURCC('s', 't', 'c', 'o')) {
					uint8_t temp[4];
					mDataSource->readAt(data_offset+8, temp, sizeof(temp));
					offset = U32_AT(temp);
				}
				else {
					uint8_t temp[8];
					mDataSource->readAt(data_offset+8, temp, sizeof(temp));
					offset = U64_AT(temp);
				}
				mDataSource->readAt(offset+4, &header, sizeof(header));
					if (((header & 0x1C) == 0) ||
						((header & 0x1C) == 0x1C) ||
						((header & 0x1C) == 0x18)) {//36~38bit for h263 first frame is invalid
						ALOGW("Invalid H263 resolution, byte 5 in first frame is %x", header);
						mLastTrack->skipTrack = true;
					}

			}
#endif
            *offset += chunk_size;
            break;
        }

        case FOURCC('s', 't', 's', 'c'):
        {
            status_t err =
                mLastTrack->sampleTable->setSampleToChunkParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            *offset += chunk_size;
            break;
        }

        case FOURCC('s', 't', 's', 'z'):
        case FOURCC('s', 't', 'z', '2'):
        {
            status_t err =
                mLastTrack->sampleTable->setSampleSizeParams(
                        chunk_type, data_offset, chunk_data_size);

            if (err != OK) {
				LOGE("SetSampeSizeParams Fail!");
                return err;
            }
#ifndef ANDROID_DEFAULT_CODE//hai.li to check unsupport video
			mLastTrack->sampleCount = mLastTrack->sampleTable->getSampleCount();
#endif

            size_t max_size;
            err = mLastTrack->sampleTable->getMaxSampleSize(&max_size);

#ifndef ANDROID_DEFAULT_CODE//hai.li for ISSUE: ALPS35871
			if (mLastTrack->mIsVideo && max_size > VIDEO_MAX_RESOLUTION*3/2)
			{
				mLastTrack->skipTrack = true;
				LOGE("ERROR:Video Sample size Wrong !!!skip this track(max_size:%u)", max_size);
			}
#endif
            if (err != OK) {				
				LOGE("getMaxSampleSize Fail!");
                return err;
            }

			ALOGD("Max Sample Size=%d", max_size);
			
            if (max_size != 0) 
			{
                // Assume that a given buffer only contains at most 10 chunks,
                // each chunk originally prefixed with a 2 byte length will
                // have a 4 byte header (0x00 0x00 0x00 0x01) after conversion,
                // and thus will grow by 2 bytes per chunk.
                mLastTrack->meta->setInt32(kKeyMaxInputSize, max_size + 10 * 2);
            } 
			else 
			{
                // No size was specified. Pick a conservatively large size.
                int32_t width, height;
                if (!mLastTrack->meta->findInt32(kKeyWidth, &width) ||
                    !mLastTrack->meta->findInt32(kKeyHeight, &height)) 
                {
                    ALOGE("No width or height, assuming worst case 1080p");
                    width = 1920;
                    height = 1080;
                }

                const char *mime;
                CHECK(mLastTrack->meta->findCString(kKeyMIMEType, &mime));
                if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_AVC)) 
				{
                    // AVC requires compression ratio of at least 2, and uses
                    // macroblocks
                    max_size = ((width + 15) / 16) * ((height + 15) / 16) * 192;
                } 
				else 
				{
                    // For all other formats there is no minimum compression
                    // ratio. Use compression ratio of 1.
                    max_size = width * height * 3 / 2;
                }
                mLastTrack->meta->setInt32(kKeyMaxInputSize, max_size);
            }
            *offset += chunk_size;

            // NOTE: setting another piece of metadata invalidates any pointers (such as the
            // mimetype) previously obtained, so don't cache them.
            const char *mime;
            CHECK(mLastTrack->meta->findCString(kKeyMIMEType, &mime));
            // Calculate average frame rate.
            if (!strncasecmp("video/", mime, 6)) 
			{
                size_t nSamples = mLastTrack->sampleTable->countSamples();
                int64_t durationUs;
                if (mLastTrack->meta->findInt64(kKeyDuration, &durationUs)) 
				{
                    if (durationUs > 0) 
					{
                        int32_t frameRate = (nSamples * 1000000LL +
                                    (durationUs >> 1)) / durationUs;
                        mLastTrack->meta->setInt32(kKeyFrameRate, frameRate);
                    }
                }
            }

            break;
        }

        case FOURCC('s', 't', 't', 's'):
        {
            status_t err =
                mLastTrack->sampleTable->setTimeToSampleParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            *offset += chunk_size;
            break;
        }

        case FOURCC('c', 't', 't', 's'):
        {
            status_t err =
                mLastTrack->sampleTable->setCompositionTimeToSampleParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            *offset += chunk_size;
            break;
        }

        case FOURCC('s', 't', 's', 's'):
        {
            status_t err =
                mLastTrack->sampleTable->setSyncSampleParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            *offset += chunk_size;
            break;
        }

        // @xyz
        case FOURCC('\xA9', 'x', 'y', 'z'):
        {
            // Best case the total data length inside "@xyz" box
            // would be 8, for instance "@xyz" + "\x00\x04\x15\xc7" + "0+0/",
            // where "\x00\x04" is the text string length with value = 4,
            // "\0x15\xc7" is the language code = en, and "0+0" is a
            // location (string) value with longitude = 0 and latitude = 0.
            if (chunk_data_size < 8) 
			{
				LOGE("The location chunk data size(%d) is not expected!", chunk_data_size);
                return ERROR_MALFORMED;
            }

            // Worst case the location string length would be 18,
            // for instance +90.0000-180.0000, without the trailing "/" and
            // the string length + language code.

			// For [DTV00613901] case, 18 is not enough. Because location = longitude + latitude + altitude.
			// 18 --> 30.
            // char buffer[18];
            char buffer[30];

            // Substracting 5 from the data size is because the text string length +
            // language code takes 4 bytes, and the trailing slash "/" takes 1 byte.
            off64_t location_length = chunk_data_size - 5;
            if (location_length >= (off64_t) sizeof(buffer)) 
			{
				LOGE("The location length(%d) exceeds buffer size(30)!", location_length);
                return ERROR_MALFORMED;
            }

            if(mDataSource->readAt(
                        data_offset + 4, buffer, location_length) < location_length) 
            {
            	LOGE("Read location failed!");
                return ERROR_IO;
            }

            buffer[location_length] = '\0';
			LOGI("Location: %s, length: %d.", buffer, location_length);
			
            mFileMetaData->setCString(kKeyLocation, buffer);
            *offset += chunk_size;
            break;
        }

        case FOURCC('e', 's', 'd', 's'):
        {
            if (chunk_data_size < 4) {
                return ERROR_MALFORMED;
            }
#ifndef ANDROID_DEFAULT_CODE
            if (chunk_data_size > 4000){
                     return ERROR_BUFFER_TOO_SMALL;
            }

            uint8_t  *buffer = (uint8_t *)malloc(chunk_data_size);

            if (buffer == NULL) {
              return -ENOMEM;
            }


            if (mDataSource->readAt(
                        data_offset, buffer, chunk_data_size) < chunk_data_size) 
            {
				free(buffer);
				LOGE("ERROR_IO, LINE=%d", __LINE__);
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) 
			{
                // Should be version 0, flags 0.
				free(buffer);
				LOGE("ERROR_MALFORMED, LINE=%d", __LINE__);
                return ERROR_MALFORMED;
            }
#else
            uint8_t buffer[256];
            if (chunk_data_size > (off64_t)sizeof(buffer)) {
                return ERROR_BUFFER_TOO_SMALL;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) {
                // Should be version 0, flags 0.
                return ERROR_MALFORMED;
            }
#endif
            mLastTrack->meta->setData(
                    kKeyESDS, kTypeESDS, &buffer[4], chunk_data_size - 4);

            if (mPath.size() >= 2
                    && mPath[mPath.size() - 2] == FOURCC('m', 'p', '4', 'a')) {
                // Information from the ESDS must be relied on for proper
                // setup of sample rate and channel count for MPEG4 Audio.
                // The generic header appears to only contain generic
                // information...

                status_t err = updateAudioTrackInfoFromESDS_MPEG4Audio(
                        &buffer[4], chunk_data_size - 4);
#ifndef ANDROID_DEFAULT_CODE
                if (err != OK) {
					LOGE("Update Audio Track Info From ESDS error, skip");
					mLastTrack->skipTrack = true;
                }

				const char* mime;
				if (mLastTrack->meta->findCString(kKeyMIMEType, &mime) &&
					(!strcmp(mime, MEDIA_MIMETYPE_AUDIO_MPEG))) {
					LOGE("Is MP3 Audio, remove esds codec info");
					mLastTrack->meta->remove(kKeyESDS);
				}
#else
                if (err != OK) {
                    return err;
                }
#endif
            }

#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
			if (mPath.size() >= 2
					&& mPath[mPath.size() - 2] == FOURCC('m', 'p', '4', 'v')) {

				//mLastTrack->meta->remove(kKeyESDS);//We should send esds to decoder for 3rd party applications, e.x. VideoEditor.
				ESDS esds(&buffer[4], chunk_data_size - 4);
				if (esds.InitCheck() == OK) {
					const void *codec_specific_data;
					size_t codec_specific_data_size;
					esds.getCodecSpecificInfo(
							&codec_specific_data, &codec_specific_data_size);
					mLastTrack->meta->setData(kKeyMPEG4VOS, 0, codec_specific_data, codec_specific_data_size);
				}
				else if (ERROR_UNSUPPORTED == esds.InitCheck())
				{
					ALOGW("Get vos from the first frame");
					mLastTrack->meta->setInt32(kKeyCodecInfoIsInFirstFrame, true);
				}
				else {
					LOGE("Parse esds error, skip video track");
					mLastTrack->skipTrack = true;
				}

			}
#endif

            *offset += chunk_size;
#ifndef ANDROID_DEFAULT_CODE
			free(buffer);
#endif  // ANDROID_DEFAULT_CODE
            break;
        }
		#ifndef ANDROID_DEFAULT_CODE		
		#ifdef MTK_VIDEO_HEVC_SUPPORT
		case FOURCC('h', 'v', 'c', 'C'):
		{	
			mIsHevc = true;
			LOGV("set video track mime: %s\n", MEDIA_MIMETYPE_VIDEO_HEVC);
			mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_HEVC);		  
			LOGD("HVCC size: %lld ", chunk_data_size);
			if (chunk_data_size > 1792)
			{
				return ERROR_BUFFER_TOO_SMALL;
			}	
			
			uint8_t *buffer = (uint8_t *)malloc(chunk_data_size);
			if((buffer == NULL)) 
			{
				return -ENOMEM;
			}
			
			if (mDataSource->readAt( data_offset, buffer, chunk_data_size) < chunk_data_size) 
			{
			   free(buffer);
			   ALOGE("ERROR_IO, LINE=%d", __LINE__);
			   return ERROR_IO;
			}
			
			LOGD("configurationVersion = %d", buffer[0]);
			if(buffer[0] != 1) 
			{
				LOGE("configurationVersion != 1");
				mLastTrack->skipTrack = true;
			}
			
			if (chunk_data_size < 23) 
			{
				ALOGW("Warning: HVCC size:%lld < 23", chunk_data_size);
				mLastTrack->skipTrack = true;
			}
			mLastTrack->meta->setData( kKeyHVCC, kTypeHVCC, buffer, chunk_data_size);

			*offset += chunk_size;
			free(buffer);
			break;
		}
		#endif
		#endif

        case FOURCC('a', 'v', 'c', 'C'):
        {
#ifndef ANDROID_DEFAULT_CODE
			if (chunk_data_size > 1792){
                     return ERROR_BUFFER_TOO_SMALL;
            }

            uint8_t *buffer = (uint8_t *)malloc(chunk_data_size);

            if (buffer == NULL) {
              return -ENOMEM;
            }

            if (mDataSource->readAt( data_offset, buffer, chunk_data_size) < chunk_data_size) {
				   free(buffer);
				   LOGE("ERROR_IO, LINE=%d", __LINE__);
                   return ERROR_IO;
            }
			
			#ifdef MTK_VIDEO_HEVC_SUPPORT         
			uint8_t *ptr1 = buffer;
			if (ptr1[1] == 0xFF) // profile 0xFF, is HEVC. PC HEVC assign
			{		
				LOGD("PC hevc_mtk, seTrackMIMEType: video/hevc\n");
				mIsHevc = true;
				mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_HEVC);
			}
			#endif            
#else
            sp<ABuffer> buffer = new ABuffer(chunk_data_size);

            if (mDataSource->readAt(
                        data_offset, buffer->data(), chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }
#endif

//Parse SPS using avc_utils
#ifndef ANDROID_DEFAULT_CODE
			uint8_t *ptr = buffer;

			//CHECK_EQ(ptr[0], 1);	// configurationVersion == 1
			uint8_t profile = ptr[1];
			uint8_t level = ptr[3];

			// There is decodable content out there that fails the following
			// assertion, let's be lenient for now...
			// CHECK((ptr[4] >> 2) == 0x3f);  // reserved

			size_t lengthSize = 1 + (ptr[4] & 3);

			// commented out check below as H264_QVGA_500_NO_AUDIO.3gp
			// violates it...
			// CHECK((ptr[5] >> 5) == 7);  // reserved

			size_t numSeqParameterSets = ptr[5] & 31;
			ptr += 6;

			off_t bytesRead = 6;
			status_t err = OK;
			int32_t maxwidth=0, maxheight=0;
			uint32_t maxprofile=0, maxlevel=0;

			for (uint32_t i=0; i < numSeqParameterSets; i++)
			{
				uint16_t size;
				uint8_t *sps;
				struct SPSInfo spsinfo;
				size = U16_AT((const uint8_t *)&ptr[0]);
				bytesRead += size;
				if (bytesRead > chunk_data_size)
				{
					LOGE("avcC SPS size error!!");
					LOGE("ERROR_MALFORMED, LINE=%d", __LINE__);
					err = ERROR_MALFORMED;
					break;
				}
				sps = ptr + 2;
				err = FindAVCSPSInfo(sps, size, &spsinfo);

				if (err != OK)
				{
					LOGE("Parse SPS fail!!");
					break;
				}

				if (spsinfo.width > maxwidth)
					maxwidth = spsinfo.width;
				if (spsinfo.height > maxheight)
					maxheight = spsinfo.height;
				if (spsinfo.profile > maxprofile)
					maxprofile = spsinfo.profile;
				if (spsinfo.level > maxlevel)
					maxlevel = spsinfo.level;

				ptr += (size + 2);
			}

			if (err != OK)
			{
				free(buffer);
				return err;
			}
			LOGD("SPS Info: width=%d, height=%d, profile=%d, level=%d", maxwidth, maxheight, maxprofile, maxlevel);

			if ((maxwidth > VIDEO_MAX_W_H) || (maxheight > VIDEO_MAX_W_H) || (maxwidth*maxheight > VIDEO_MAX_RESOLUTION))
			{
				LOGW("[H264 capability error]Unsupported H264 video, width=%d, height=%d, profile=%d, level=%d", maxwidth, maxheight, maxprofile, maxlevel);
				mLastTrack->skipTrack = true;
			}
			else
			{
                 mLastTrack->meta->setData(
                     kKeyAVCC, kTypeAVCC, buffer, chunk_data_size);
			}

            *offset += chunk_size;
			free(buffer);
#else
            mLastTrack->meta->setData(
                    kKeyAVCC, kTypeAVCC, buffer->data(), chunk_data_size);

            *offset += chunk_size;
#endif
            break;
        }

        case FOURCC('d', '2', '6', '3'):
        {
            /*
             * d263 contains a fixed 7 bytes part:
             *   vendor - 4 bytes
             *   version - 1 byte
             *   level - 1 byte
             *   profile - 1 byte
             * optionally, "d263" box itself may contain a 16-byte
             * bit rate box (bitr)
             *   average bit rate - 4 bytes
             *   max bit rate - 4 bytes
             */
            char buffer[23];
#ifndef ANDROID_DEFAULT_CODE//Some files do not comply this rule
			if (chunk_data_size > 23)
#else
            if (chunk_data_size != 7 &&
                chunk_data_size != 23)
#endif
            {
                ALOGE("Incorrect D263 box size %lld", chunk_data_size);
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                    data_offset, buffer, chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }

            mLastTrack->meta->setData(kKeyD263, kTypeD263, buffer, chunk_data_size);

            *offset += chunk_size;
            break;
        }

        case FOURCC('m', 'e', 't', 'a'):
        {
            uint8_t buffer[4];
            if (chunk_data_size < (off64_t)sizeof(buffer)) {
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, 4) < 4) {
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) {
                // Should be version 0, flags 0.

                // If it's not, let's assume this is one of those
                // apparently malformed chunks that don't have flags
                // and completely different semantics than what's
                // in the MPEG4 specs and skip it.
                *offset += chunk_size;
                return OK;
            }

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + sizeof(buffer);
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('m', 'e', 'a', 'n'):
        case FOURCC('n', 'a', 'm', 'e'):
        case FOURCC('d', 'a', 't', 'a'):
        {
            if (mPath.size() == 6 && underMetaDataPath(mPath)) {
                status_t err = parseMetaData(data_offset, chunk_data_size);

                if (err != OK) {
                    return err;
                }
            }

            *offset += chunk_size;
            break;
        }

        case FOURCC('m', 'v', 'h', 'd'):
        {
            if (chunk_data_size < 24) {
                ALOGE("return %d",__LINE__);
                return ERROR_MALFORMED;
            }

            uint8_t header[24];
            if (mDataSource->readAt(
                        data_offset, header, sizeof(header))
                    < (ssize_t)sizeof(header)) {
                ALOGE("return %d",__LINE__);
                return ERROR_IO;
            }

            uint64_t creationTime;
            if (header[0] == 1) {
                creationTime = U64_AT(&header[4]);
#ifndef ANDROID_DEFAULT_CODE
				mMovieTimeScale = U32_AT(&header[20]);
#endif
                mHeaderTimescale = U32_AT(&header[20]);
            } else if (header[0] != 0) {
                ALOGE("return %d",__LINE__);
                return ERROR_MALFORMED;
            } else {
                creationTime = U32_AT(&header[4]);
#ifndef ANDROID_DEFAULT_CODE
				mMovieTimeScale = U32_AT(&header[12]);
#endif
                mHeaderTimescale = U32_AT(&header[12]);
            }

            String8 s;
            convertTimeToDate(creationTime, &s);

            mFileMetaData->setCString(kKeyDate, s.string());

            *offset += chunk_size;
            break;
        }

        case FOURCC('m', 'd', 'a', 't'):
        {
            ALOGV("mdat chunk, drm: %d", mIsDrm);
            if (!mIsDrm) {
                *offset += chunk_size;
                break;
            }

            if (chunk_size < 8) {
                ALOGE("return %d",__LINE__);
                return ERROR_MALFORMED;
            }

            return parseDrmSINF(offset, data_offset);
        }

        case FOURCC('h', 'd', 'l', 'r'):
        {
            uint32_t buffer;
            if (mDataSource->readAt(
                        data_offset + 8, &buffer, 4) < 4) {
                ALOGE("return %d",__LINE__);
                return ERROR_IO;
            }

            uint32_t type = ntohl(buffer);
            // For the 3GPP file format, the handler-type within the 'hdlr' box
            // shall be 'text'. We also want to support 'sbtl' handler type
            // for a practical reason as various MPEG4 containers use it.
            if (type == FOURCC('t', 'e', 'x', 't') || type == FOURCC('s', 'b', 't', 'l')) {
                mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_TEXT_3GPP);
            }

            *offset += chunk_size;
            break;
        }

        case FOURCC('t', 'x', '3', 'g'):
        {
            uint32_t type;
            const void *data;
            size_t size = 0;
            if (!mLastTrack->meta->findData(
                    kKeyTextFormatData, &type, &data, &size)) {
                size = 0;
            }

            uint8_t *buffer = new uint8_t[size + chunk_size];

            if (size > 0) {
                memcpy(buffer, data, size);
            }

            if ((size_t)(mDataSource->readAt(*offset, buffer + size, chunk_size))
                    < chunk_size) 
            {
                delete[] buffer;
                buffer = NULL;
                return ERROR_IO;
            }

            mLastTrack->meta->setData(
                    kKeyTextFormatData, 0, buffer, size + chunk_size);

            delete[] buffer;

            *offset += chunk_size;
            break;
        }

        case FOURCC('c', 'o', 'v', 'r'):
        {
            if (mFileMetaData != NULL) {
                ALOGV("chunk_data_size = %lld and data_offset = %lld",
                        chunk_data_size, data_offset);
                sp<ABuffer> buffer = new ABuffer(chunk_data_size + 1);
                if (mDataSource->readAt(
                    data_offset, buffer->data(), chunk_data_size) != (ssize_t)chunk_data_size) {
                    ALOGE("return %d",__LINE__);
                    return ERROR_IO;
                }
                const int kSkipBytesOfDataBox = 16;
                mFileMetaData->setData(
                    kKeyAlbumArt, MetaData::TYPE_NONE,
                    buffer->data() + kSkipBytesOfDataBox, chunk_data_size - kSkipBytesOfDataBox);
            }

            *offset += chunk_size;
            break;
        }

        case FOURCC('-', '-', '-', '-'):
        {
            mLastCommentMean.clear();
            mLastCommentName.clear();
            mLastCommentData.clear();
            *offset += chunk_size;
            break;
        }

        case FOURCC('s', 'i', 'd', 'x'):
        {
            parseSegmentIndex(data_offset, chunk_data_size);
            *offset += chunk_size;
            ALOGE("return %d",__LINE__);
            return UNKNOWN_ERROR; // stop parsing after sidx
        }

        default:
        {
            *offset += chunk_size;
            break;
        }
    }

    return OK;
}

status_t MPEG4Extractor::parseSegmentIndex(off64_t offset, size_t size) {
  ALOGV("MPEG4Extractor::parseSegmentIndex");

    if (size < 12) {
      return -EINVAL;
    }

    uint32_t flags;
    if (!mDataSource->getUInt32(offset, &flags)) {
        return ERROR_MALFORMED;
    }

    uint32_t version = flags >> 24;
    flags &= 0xffffff;

    ALOGV("sidx version %d", version);

    uint32_t referenceId;
    if (!mDataSource->getUInt32(offset + 4, &referenceId)) {
        return ERROR_MALFORMED;
    }

    uint32_t timeScale;
    if (!mDataSource->getUInt32(offset + 8, &timeScale)) {
        return ERROR_MALFORMED;
    }
    ALOGV("sidx refid/timescale: %d/%d", referenceId, timeScale);

    uint64_t earliestPresentationTime;
    uint64_t firstOffset;

    offset += 12;
    size -= 12;

    if (version == 0) {
        if (size < 8) {
            return -EINVAL;
        }
        uint32_t tmp;
        if (!mDataSource->getUInt32(offset, &tmp)) {
            return ERROR_MALFORMED;
        }
        earliestPresentationTime = tmp;
        if (!mDataSource->getUInt32(offset + 4, &tmp)) {
            return ERROR_MALFORMED;
        }
        firstOffset = tmp;
        offset += 8;
        size -= 8;
    } else {
        if (size < 16) {
            return -EINVAL;
        }
        if (!mDataSource->getUInt64(offset, &earliestPresentationTime)) {
            return ERROR_MALFORMED;
        }
        if (!mDataSource->getUInt64(offset + 8, &firstOffset)) {
            return ERROR_MALFORMED;
        }
        offset += 16;
        size -= 16;
    }
    ALOGV("sidx pres/off: %Ld/%Ld", earliestPresentationTime, firstOffset);

    if (size < 4) {
        return -EINVAL;
    }

    uint16_t referenceCount;
    if (!mDataSource->getUInt16(offset + 2, &referenceCount)) {
        return ERROR_MALFORMED;
    }
    offset += 4;
    size -= 4;
    ALOGV("refcount: %d", referenceCount);

    if (size < referenceCount * 12) {
        return -EINVAL;
    }

    uint64_t total_duration = 0;
    for (unsigned int i = 0; i < referenceCount; i++) {
        uint32_t d1, d2, d3;

        if (!mDataSource->getUInt32(offset, &d1) ||     // size
            !mDataSource->getUInt32(offset + 4, &d2) || // duration
            !mDataSource->getUInt32(offset + 8, &d3)) { // flags
            return ERROR_MALFORMED;
        }

        if (d1 & 0x80000000) {
            ALOGW("sub-sidx boxes not supported yet");
        }
        bool sap = d3 & 0x80000000;
        bool saptype = d3 >> 28;
        if (!sap || saptype > 2) {
            ALOGW("not a stream access point, or unsupported type");
        }
        total_duration += d2;
        offset += 12;
        ALOGV(" item %d, %08x %08x %08x", i, d1, d2, d3);
        SidxEntry se;
        se.mSize = d1 & 0x7fffffff;
        se.mDurationUs = 1000000LL * d2 / timeScale;
        mSidxEntries.add(se);
    }

    mSidxDuration = total_duration * 1000000 / timeScale;
    ALOGV("duration: %lld", mSidxDuration);

    int64_t metaDuration;
    if (!mLastTrack->meta->findInt64(kKeyDuration, &metaDuration) || metaDuration == 0) {
        mLastTrack->meta->setInt64(kKeyDuration, mSidxDuration);
    }
    return OK;
}



status_t MPEG4Extractor::parseTrackHeader(
        off64_t data_offset, off64_t data_size) {
    if (data_size < 4) {
        return ERROR_MALFORMED;
    }

    uint8_t version;
    if (mDataSource->readAt(data_offset, &version, 1) < 1) {
        return ERROR_IO;
    }

    size_t dynSize = (version == 1) ? 36 : 24;

    uint8_t buffer[36 + 60];

    if (data_size != (off64_t)dynSize + 60) {
        return ERROR_MALFORMED;
    }

    if (mDataSource->readAt(
                data_offset, buffer, data_size) < (ssize_t)data_size) {
        return ERROR_IO;
    }

    uint64_t ctime, mtime, duration;
    int32_t id;

    if (version == 1) {
        ctime = U64_AT(&buffer[4]);
        mtime = U64_AT(&buffer[12]);
        id = U32_AT(&buffer[20]);
        duration = U64_AT(&buffer[28]);
    } else if (version == 0) {
        ctime = U32_AT(&buffer[4]);
        mtime = U32_AT(&buffer[8]);
        id = U32_AT(&buffer[12]);
        duration = U32_AT(&buffer[20]);
    } else {
        return ERROR_UNSUPPORTED;
    }

    mLastTrack->meta->setInt32(kKeyTrackID, id);

    size_t matrixOffset = dynSize + 16;
    int32_t a00 = U32_AT(&buffer[matrixOffset]);
    int32_t a01 = U32_AT(&buffer[matrixOffset + 4]);
    int32_t dx = U32_AT(&buffer[matrixOffset + 8]);
    int32_t a10 = U32_AT(&buffer[matrixOffset + 12]);
    int32_t a11 = U32_AT(&buffer[matrixOffset + 16]);
    int32_t dy = U32_AT(&buffer[matrixOffset + 20]);

    uint32_t rotationDegrees;

    static const int32_t kFixedOne = 0x10000;
    if (a00 == kFixedOne && a01 == 0 && a10 == 0 && a11 == kFixedOne) {
        // Identity, no rotation
        rotationDegrees = 0;
    } else if (a00 == 0 && a01 == kFixedOne && a10 == -kFixedOne && a11 == 0) {
        rotationDegrees = 90;
    } else if (a00 == 0 && a01 == -kFixedOne && a10 == kFixedOne && a11 == 0) {
        rotationDegrees = 270;
    } else if (a00 == -kFixedOne && a01 == 0 && a10 == 0 && a11 == -kFixedOne) {
        rotationDegrees = 180;
    } else {
        ALOGW("We only support 0,90,180,270 degree rotation matrices");
        rotationDegrees = 0;
    }

    if (rotationDegrees != 0) {
        mLastTrack->meta->setInt32(kKeyRotation, rotationDegrees);
    }

    // Handle presentation display size, which could be different
    // from the image size indicated by kKeyWidth and kKeyHeight.
    uint32_t width = U32_AT(&buffer[dynSize + 52]);
    uint32_t height = U32_AT(&buffer[dynSize + 56]);
    mLastTrack->meta->setInt32(kKeyDisplayWidth, width >> 16);
    mLastTrack->meta->setInt32(kKeyDisplayHeight, height >> 16);

    return OK;
}

status_t MPEG4Extractor::parseMetaData(off64_t offset, size_t size) {
    if (size < 4) {
        ALOGE("return %d",__LINE__);
        return ERROR_MALFORMED;
    }

    uint8_t *buffer = new uint8_t[size + 1];
    if (mDataSource->readAt(
                offset, buffer, size) != (ssize_t)size) {
        delete[] buffer;
        buffer = NULL;
        ALOGE("return %d",__LINE__);
        return ERROR_IO;
    }

    uint32_t flags = U32_AT(buffer);

    uint32_t metadataKey = 0;
    char chunk[5];
    MakeFourCCString(mPath[4], chunk);
    ALOGV("meta: %s @ %lld", chunk, offset);
    switch (mPath[4]) {
        case FOURCC(0xa9, 'a', 'l', 'b'):
        {
            metadataKey = kKeyAlbum;
            break;
        }
        case FOURCC(0xa9, 'A', 'R', 'T'):
        {
            metadataKey = kKeyArtist;
            break;
        }
        case FOURCC('a', 'A', 'R', 'T'):
        {
            metadataKey = kKeyAlbumArtist;
            break;
        }
        case FOURCC(0xa9, 'd', 'a', 'y'):
        {
            metadataKey = kKeyYear;
            break;
        }
        case FOURCC(0xa9, 'n', 'a', 'm'):
        {
            metadataKey = kKeyTitle;
            break;
        }
        case FOURCC(0xa9, 'w', 'r', 't'):
        {
            metadataKey = kKeyWriter;
            break;
        }
        case FOURCC('c', 'o', 'v', 'r'):
        {
            metadataKey = kKeyAlbumArt;
            break;
        }
        case FOURCC('g', 'n', 'r', 'e'):
        {
            metadataKey = kKeyGenre;
            break;
        }
        case FOURCC(0xa9, 'g', 'e', 'n'):
        {
            metadataKey = kKeyGenre;
            break;
        }
        case FOURCC('c', 'p', 'i', 'l'):
        {
            if (size == 9 && flags == 21) {
                char tmp[16];
                sprintf(tmp, "%d",
                        (int)buffer[size - 1]);

                mFileMetaData->setCString(kKeyCompilation, tmp);
            }
            break;
        }
        case FOURCC('t', 'r', 'k', 'n'):
        {
            if (size == 16 && flags == 0) {
                char tmp[16];
                uint16_t* pTrack = (uint16_t*)&buffer[10];
                uint16_t* pTotalTracks = (uint16_t*)&buffer[12];
                sprintf(tmp, "%d/%d", ntohs(*pTrack), ntohs(*pTotalTracks));

                mFileMetaData->setCString(kKeyCDTrackNumber, tmp);
            }
            break;
        }
        case FOURCC('d', 'i', 's', 'k'):
        {
            if ((size == 14 || size == 16) && flags == 0) {
                char tmp[16];
                uint16_t* pDisc = (uint16_t*)&buffer[10];
                uint16_t* pTotalDiscs = (uint16_t*)&buffer[12];
                sprintf(tmp, "%d/%d", ntohs(*pDisc), ntohs(*pTotalDiscs));

                mFileMetaData->setCString(kKeyDiscNumber, tmp);
            }
            break;
        }
        case FOURCC('-', '-', '-', '-'):
        {
            buffer[size] = '\0';
            switch (mPath[5]) {
                case FOURCC('m', 'e', 'a', 'n'):
                    mLastCommentMean.setTo((const char *)buffer + 4);
                    break;
                case FOURCC('n', 'a', 'm', 'e'):
                    mLastCommentName.setTo((const char *)buffer + 4);
                    break;
                case FOURCC('d', 'a', 't', 'a'):
                    mLastCommentData.setTo((const char *)buffer + 8);
                    break;
            }

            // Once we have a set of mean/name/data info, go ahead and process
            // it to see if its something we are interested in.  Whether or not
            // were are interested in the specific tag, make sure to clear out
            // the set so we can be ready to process another tuple should one
            // show up later in the file.
            if ((mLastCommentMean.length() != 0) &&
                (mLastCommentName.length() != 0) &&
                (mLastCommentData.length() != 0)) {

                if (mLastCommentMean == "com.apple.iTunes"
                        && mLastCommentName == "iTunSMPB") {
                    int32_t delay, padding;
                    if (sscanf(mLastCommentData,
                               " %*x %x %x %*x", &delay, &padding) == 2) {
                        mLastTrack->meta->setInt32(kKeyEncoderDelay, delay);
                        mLastTrack->meta->setInt32(kKeyEncoderPadding, padding);
                    }
                }

                mLastCommentMean.clear();
                mLastCommentName.clear();
                mLastCommentData.clear();
            }
            break;
        }

        default:
            break;
    }

    if (size >= 8 && metadataKey) {
        if (metadataKey == kKeyAlbumArt) {
            mFileMetaData->setData(
                    kKeyAlbumArt, MetaData::TYPE_NONE,
                    buffer + 8, size - 8);
        } else if (metadataKey == kKeyGenre) {
            if (flags == 0) {
                // uint8_t genre code, iTunes genre codes are
                // the standard id3 codes, except they start
                // at 1 instead of 0 (e.g. Pop is 14, not 13)
                // We use standard id3 numbering, so subtract 1.
                int genrecode = (int)buffer[size - 1];
                genrecode--;
                if (genrecode < 0) {
                    genrecode = 255; // reserved for 'unknown genre'
                }
                char genre[10];
                sprintf(genre, "%d", genrecode);

                mFileMetaData->setCString(metadataKey, genre);
            } else if (flags == 1) {
                // custom genre string
                buffer[size] = '\0';

                mFileMetaData->setCString(
                        metadataKey, (const char *)buffer + 8);
            }
        } else {
            buffer[size] = '\0';

            mFileMetaData->setCString(
                    metadataKey, (const char *)buffer + 8);
        }
    }

    delete[] buffer;
    buffer = NULL;

    return OK;
}

sp<MediaSource> MPEG4Extractor::getTrack(size_t index) {
    status_t err;
    if ((err = readMetaData()) != OK) {
        return NULL;
    }

    Track *track = mFirstTrack;
    while (index > 0) {
        if (track == NULL) {
            return NULL;
        }

        track = track->next;
        --index;
    }

    if (track == NULL) {
        return NULL;
    }

    ALOGV("getTrack called, pssh: %d", mPssh.size());
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
		if ((track->mElstEntries != NULL) &&
			(track->mStartTimeOffset != 0) &&
			(mMovieTimeScale != 0))
		{
			track->sampleTable->setStartTimeOffset(track->mStartTimeOffset*track->timescale/mMovieTimeScale);
			ALOGD("track->mStartTimeOffset=%d, track->timescale=%d, mMovieTimeScale=%d", track->mStartTimeOffset, track->timescale, mMovieTimeScale);

			const char *mime;
			CHECK(track->meta->findCString(kKeyMIMEType, &mime));
			if (!strncasecmp("audio/", mime, 6)) {
				uint32_t PadSampleNum = track->sampleTable->getStartTimeOffset();
				if (PadSampleNum >= 512*1024)
				{
					LOGW("Unsupported too large audio time offset: %d samples!!", PadSampleNum);
					track->sampleTable->setStartTimeOffset(0);
					track->mStartTimeOffset = 0;
				}
				LOGE("audio time offset=%d", track->sampleTable->getStartTimeOffset());
				if (track->sampleTable->getStartTimeOffset() != 0)
					track->meta->setInt32(kKeyAudioPadEnable, true);
			}
		}//added by hai.li to support track time offset
    track->meta->setInt32(kKeySupportTryRead, 1);
#endif
    return new MPEG4Source(
            track->meta, mDataSource, track->timescale, track->sampleTable,
            mSidxEntries, mMoofOffset);
}

// static
status_t MPEG4Extractor::verifyTrack(Track *track) {
    const char *mime;
    CHECK(track->meta->findCString(kKeyMIMEType, &mime));

	ALOGV("verifyTrack: track mime = %s", mime);
	
    uint32_t type;
    const void *data;
    size_t size;
    if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC)) 
	{
        if (!track->meta->findData(kKeyAVCC, &type, &data, &size)
                || type != kTypeAVCC) 
        {
        	LOGE("Malformed AVC track!");
            return ERROR_MALFORMED;
        }
    }
	#ifdef MTK_VIDEO_HEVC_SUPPORT 
	else if(!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_HEVC)) 
	{
        if(!track->meta->findData(kKeyHVCC, &type, &data, &size)
			|| type != kTypeHVCC) 
		{
			LOGE("Malformed HEVC track!");
            return ERROR_MALFORMED;
        }
    }
	#endif
	else if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG4)
            || !strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AAC)) 
    {
        if (!track->meta->findData(kKeyESDS, &type, &data, &size)
                || type != kTypeESDS) 
        {
        	LOGE("Malformed %s track!", mime);
            return ERROR_MALFORMED;
        }
    }

    if (!track->sampleTable->isValid()) 
	{
        // Make sure we have all the metadata we need.
        LOGE("SampleTable of track invalid!");
        return ERROR_MALFORMED;
    }

    return OK;
}

status_t MPEG4Extractor::updateAudioTrackInfoFromESDS_MPEG4Audio(
        const void *esds_data, size_t esds_size) {
    ESDS esds(esds_data, esds_size);

    uint8_t objectTypeIndication;
    if (esds.getObjectTypeIndication(&objectTypeIndication) != OK) {
        return ERROR_MALFORMED;
    }

    if (objectTypeIndication == 0xe1) {
        // This isn't MPEG4 audio at all, it's QCELP 14k...
        mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_QCELP);
#ifndef ANDROID_DEFAULT_CODE
		mLastTrack->skipTrack = true;
		LOGD("Skip qcelp audio track");
#endif
        return OK;
    }
#ifndef ANDROID_DEFAULT_CODE                                        //xingyu.zhou
	if (objectTypeIndication == 0x6B || objectTypeIndication == 0x69) {
		mLastTrack->meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_MPEG);
		return OK;
	}
#else
    if (objectTypeIndication  == 0x6b) {
        // The media subtype is MP3 audio
        // Our software MP3 audio decoder may not be able to handle
        // packetized MP3 audio; for now, lets just return ERROR_UNSUPPORTED
        ALOGE("MP3 track in MP4/3GPP file is not supported");
        return ERROR_UNSUPPORTED;
    }
#endif
    const uint8_t *csd;
    size_t csd_size;
    if (esds.getCodecSpecificInfo(
                (const void **)&csd, &csd_size) != OK) {
        return ERROR_MALFORMED;
    }


    if (csd_size == 0) {
        // There's no further information, i.e. no codec specific data
        // Let's assume that the information provided in the mpeg4 headers
        // is accurate and hope for the best.

        return OK;
    }

    if (csd_size < 2) {
        return ERROR_MALFORMED;
    }

    static uint32_t kSamplingRate[] = {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000, 7350
    };

    ABitReader br(csd, csd_size);
    uint32_t objectType = br.getBits(5);
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
   mLastTrack->meta->setInt32(kKeyAacObjType, objectType);
#endif

    if (objectType == 31) {  // AAC-ELD => additional 6 bits
        objectType = 32 + br.getBits(6);
    }

    //keep AOT type
    mLastTrack->meta->setInt32(kKeyAACAOT, objectType);

    uint32_t freqIndex = br.getBits(4);

    int32_t sampleRate = 0;
    int32_t numChannels = 0;
    if (freqIndex == 15) {
        if (csd_size < 5) {
            return ERROR_MALFORMED;
        }
        sampleRate = br.getBits(24);
        numChannels = br.getBits(4);
    } else {
        numChannels = br.getBits(4);

        if (freqIndex == 13 || freqIndex == 14) {
            return ERROR_MALFORMED;
        }

        sampleRate = kSamplingRate[freqIndex];
    }

    if (objectType == 5 || objectType == 29) { // SBR specific config per 14496-3 table 1.13
        uint32_t extFreqIndex = br.getBits(4);
        int32_t extSampleRate;
        if (extFreqIndex == 15) {
            if (csd_size < 8) {
                return ERROR_MALFORMED;
            }
            extSampleRate = br.getBits(24);
        } else {
            if (extFreqIndex == 13 || extFreqIndex == 14) {
                return ERROR_MALFORMED;
            }
            extSampleRate = kSamplingRate[extFreqIndex];
        }
        //TODO: save the extension sampling rate value in meta data =>
        //      mLastTrack->meta->setInt32(kKeyExtSampleRate, extSampleRate);
    }

    if (numChannels == 0) {
        return ERROR_UNSUPPORTED;
    }

    int32_t prevSampleRate;
    CHECK(mLastTrack->meta->findInt32(kKeySampleRate, &prevSampleRate));

    if (prevSampleRate != sampleRate) {
        ALOGV("mpeg4 audio sample rate different from previous setting. "
             "was: %d, now: %d", prevSampleRate, sampleRate);
    }

    mLastTrack->meta->setInt32(kKeySampleRate, sampleRate);

    int32_t prevChannelCount;
    CHECK(mLastTrack->meta->findInt32(kKeyChannelCount, &prevChannelCount));

    if (prevChannelCount != numChannels) {
        ALOGV("mpeg4 audio channel count different from previous setting. "
             "was: %d, now: %d", prevChannelCount, numChannels);
    }

    mLastTrack->meta->setInt32(kKeyChannelCount, numChannels);

    return OK;
}

#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
status_t MPEG4Extractor::setCodecInfoFromFirstFrame(Track *track)
{
	off64_t frame_offset;
	size_t  frame_size;
	void*   frame_data = NULL;
	track->sampleTable->getMetaDataForSample(0, &frame_offset, &frame_size, NULL, NULL);
	frame_data = malloc(frame_size);
	if (NULL == frame_data){
		LOGE("malloc first frame data buffer fail!");
		return ERROR_BUFFER_TOO_SMALL;
	}

	if (mDataSource->readAt(
				frame_offset, frame_data, frame_size)
			< frame_size) {
		LOGE("read first frame fail!!");
		return ERROR_IO;
	}

	const char* mime;
	if (!track->meta->findCString(kKeyMIMEType, &mime))
	{
		LOGE("No mime type track!!");
		return UNKNOWN_ERROR;
	}

	if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG4))
	{
		size_t vosend;
		for (vosend=0; (vosend < 200) && (vosend < frame_size-4); vosend++)
		{
			if (0xB6010000 == *(uint32_t*)((uint8_t*)frame_data + vosend))
			{
				break;//Send VOS until VOP
			}
		}
		track->meta->setData(kKeyMPEG4VOS, 0, frame_data, vosend);
		for (int32_t i=0; i<vosend; i++)
			ALOGD("VOS[%d] = 0x%x", i, *((uint8_t *)frame_data + i));
	}

	free(frame_data);
	return OK;
}
#endif

////////////////////////////////////////////////////////////////////////////////

MPEG4Source::MPEG4Source(
        const sp<MetaData> &format,
        const sp<DataSource> &dataSource,
        int32_t timeScale,
        const sp<SampleTable> &sampleTable,
        Vector<SidxEntry> &sidx,
        off64_t firstMoofOffset)
    : mFormat(format),
      mDataSource(dataSource),
      mTimescale(timeScale),
      mSampleTable(sampleTable),
      mCurrentSampleIndex(0),
      mCurrentFragmentIndex(0),
      mSegments(sidx),
      mFirstMoofOffset(firstMoofOffset),
      mCurrentMoofOffset(firstMoofOffset),
      mCurrentTime(0),
      mCurrentSampleInfoAllocSize(0),
      mCurrentSampleInfoSizes(NULL),
      mCurrentSampleInfoOffsetsAllocSize(0),
      mCurrentSampleInfoOffsets(NULL),
      mIsAVC(false),
      mNALLengthSize(0),
      mStarted(false),
      mGroup(NULL),
      mBuffer(NULL),
      mWantsNALFragments(false),
      mSrcBuffer(NULL) {
      
#ifdef LOG_TO_FILE
      char name[128];
      sprintf(name, "/sdcard/%x.raw", (int)mFormat.get());
      fp = fopen(name, "wb");
#endif

    mFormat->findInt32(kKeyCryptoMode, &mCryptoMode);
    mDefaultIVSize = 0;
    mFormat->findInt32(kKeyCryptoDefaultIVSize, &mDefaultIVSize);
    uint32_t keytype;
    const void *key;
    size_t keysize;
    if (mFormat->findData(kKeyCryptoKey, &keytype, &key, &keysize)) {
        CHECK(keysize <= 16);
        memset(mCryptoKey, 0, 16);
        memcpy(mCryptoKey, key, keysize);
    }

    const char *mime;
    bool success = mFormat->findCString(kKeyMIMEType, &mime);
    CHECK(success);

#ifndef ANDROID_DEFAULT_CODE
	#ifdef MTK_VIDEO_HEVC_SUPPORT
	mIsAVC = !strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC) || 
			 !strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_HEVC);
	#else
	mIsAVC = !strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC);
	#endif
#else
	mIsAVC = !strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC);
#endif

    if (mIsAVC) {
        uint32_t type;
        const void *data;
        size_t size;
		#ifndef ANDROID_DEFAULT_CODE		
		if(format->findData(kKeyAVCC, &type, &data, &size))
		{
			const uint8_t *ptr = (const uint8_t *)data;
			CHECK(size >= 7);
			CHECK_EQ((unsigned)ptr[0], 1u);  // configurationVersion == 1
			// The number of bytes used to encode the length of a NAL unit.
			mNALLengthSize = 1 + (ptr[4] & 3);
		}
		#ifdef MTK_VIDEO_HEVC_SUPPORT		
		else if(format->findData(kKeyHVCC, &type, &data, &size))
		{
			const uint8_t *ptr = (const uint8_t *)data;
			CHECK(size >= 23);
			CHECK_EQ((unsigned)ptr[0], 1u);  // configurationVersion == 1

			// The number of bytes used to encode the length of a NAL unit.
			mNALLengthSize = 1 + (ptr[21] & 3);
		}
		#endif		
		#else		
        CHECK(format->findData(kKeyAVCC, &type, &data, &size));

        const uint8_t *ptr = (const uint8_t *)data;

        CHECK(size >= 7);
        CHECK_EQ((unsigned)ptr[0], 1u);  // configurationVersion == 1

        // The number of bytes used to encode the length of a NAL unit.
        mNALLengthSize = 1 + (ptr[4] & 3);
		#endif
    }

    CHECK(format->findInt32(kKeyTrackID, &mTrackId));

    if (mFirstMoofOffset != 0) {
        off64_t offset = mFirstMoofOffset;
        parseChunk(&offset);
    }
}

MPEG4Source::~MPEG4Source() {
#ifdef LOG_TO_FILE
	fclose(fp);
#endif
    if (mStarted) {
        stop();
    }
    free(mCurrentSampleInfoSizes);
    free(mCurrentSampleInfoOffsets);
}

status_t MPEG4Source::start(MetaData *params) {
    Mutex::Autolock autoLock(mLock);

    CHECK(!mStarted);

    int32_t val;
    if (params && params->findInt32(kKeyWantsNALFragments, &val)
        && val != 0) {
        mWantsNALFragments = true;
    } else {
        mWantsNALFragments = false;
    }

    mGroup = new MediaBufferGroup;

    int32_t max_size;
    CHECK(mFormat->findInt32(kKeyMaxInputSize, &max_size));

    mGroup->add_buffer(new MediaBuffer(max_size));

    mSrcBuffer = new uint8_t[max_size];

    mStarted = true;

    return OK;
}

status_t MPEG4Source::stop() {
    Mutex::Autolock autoLock(mLock);

    CHECK(mStarted);

    if (mBuffer != NULL) {
        mBuffer->release();
        mBuffer = NULL;
    }

    delete[] mSrcBuffer;
    mSrcBuffer = NULL;

    delete mGroup;
    mGroup = NULL;

    mStarted = false;
    mCurrentSampleIndex = 0;

    return OK;
}

status_t MPEG4Source::parseChunk(off64_t *offset) {
    uint32_t hdr[2];
    if (mDataSource->readAt(*offset, hdr, 8) < 8) {
        return ERROR_IO;
    }
    uint64_t chunk_size = ntohl(hdr[0]);
    uint32_t chunk_type = ntohl(hdr[1]);
    off64_t data_offset = *offset + 8;

    if (chunk_size == 1) {
        if (mDataSource->readAt(*offset + 8, &chunk_size, 8) < 8) {
            return ERROR_IO;
        }
        chunk_size = ntoh64(chunk_size);
        data_offset += 8;

        if (chunk_size < 16) {
            // The smallest valid chunk is 16 bytes long in this case.
            return ERROR_MALFORMED;
        }
    } else if (chunk_size < 8) {
        // The smallest valid chunk is 8 bytes long.
        return ERROR_MALFORMED;
    }

    char chunk[5];
    MakeFourCCString(chunk_type, chunk);
    ALOGV("MPEG4Source chunk %s @ %llx", chunk, *offset);

    off64_t chunk_data_size = *offset + chunk_size - data_offset;

    switch(chunk_type) {

        case FOURCC('t', 'r', 'a', 'f'):
        case FOURCC('m', 'o', 'o', 'f'): {
            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset;
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset);
                if (err != OK) {
                    return err;
                }
            }
            if (chunk_type == FOURCC('m', 'o', 'o', 'f')) {
                // *offset points to the mdat box following this moof
                parseChunk(offset); // doesn't actually parse it, just updates offset
                mNextMoofOffset = *offset;
            }
            break;
        }

        case FOURCC('t', 'f', 'h', 'd'): {
                status_t err;
                if ((err = parseTrackFragmentHeader(data_offset, chunk_data_size)) != OK) {
                    return err;
                }
                *offset += chunk_size;
                break;
        }

        case FOURCC('t', 'r', 'u', 'n'): {
                status_t err;
                if (mLastParsedTrackId == mTrackId) {
                    if ((err = parseTrackFragmentRun(data_offset, chunk_data_size)) != OK) {
                        return err;
                    }
                }

                *offset += chunk_size;
                break;
        }

        case FOURCC('s', 'a', 'i', 'z'): {
            status_t err;
            if ((err = parseSampleAuxiliaryInformationSizes(data_offset, chunk_data_size)) != OK) {
                return err;
            }
            *offset += chunk_size;
            break;
        }
        case FOURCC('s', 'a', 'i', 'o'): {
            status_t err;
            if ((err = parseSampleAuxiliaryInformationOffsets(data_offset, chunk_data_size)) != OK) {
                return err;
            }
            *offset += chunk_size;
            break;
        }

        case FOURCC('m', 'd', 'a', 't'): {
            // parse DRM info if present
            ALOGV("MPEG4Source::parseChunk mdat");
            // if saiz/saoi was previously observed, do something with the sampleinfos
            *offset += chunk_size;
            break;
        }

        default: {
            *offset += chunk_size;
            break;
        }
    }
    return OK;
}

status_t MPEG4Source::parseSampleAuxiliaryInformationSizes(off64_t offset, off64_t size) {
    ALOGV("parseSampleAuxiliaryInformationSizes");
    // 14496-12 8.7.12
    uint8_t version;
    if (mDataSource->readAt(
            offset, &version, sizeof(version))
            < (ssize_t)sizeof(version)) {
        return ERROR_IO;
    }

    if (version != 0) {
        return ERROR_UNSUPPORTED;
    }
    offset++;

    uint32_t flags;
    if (!mDataSource->getUInt24(offset, &flags)) {
        return ERROR_IO;
    }
    offset += 3;

    if (flags & 1) {
        uint32_t tmp;
        if (!mDataSource->getUInt32(offset, &tmp)) {
            return ERROR_MALFORMED;
        }
        mCurrentAuxInfoType = tmp;
        offset += 4;
        if (!mDataSource->getUInt32(offset, &tmp)) {
            return ERROR_MALFORMED;
        }
        mCurrentAuxInfoTypeParameter = tmp;
        offset += 4;
    }

    uint8_t defsize;
    if (mDataSource->readAt(offset, &defsize, 1) != 1) {
        return ERROR_MALFORMED;
    }
    mCurrentDefaultSampleInfoSize = defsize;
    offset++;

    uint32_t smplcnt;
    if (!mDataSource->getUInt32(offset, &smplcnt)) {
        return ERROR_MALFORMED;
    }
    mCurrentSampleInfoCount = smplcnt;
    offset += 4;

    if (mCurrentDefaultSampleInfoSize != 0) {
        ALOGV("@@@@ using default sample info size of %d", mCurrentDefaultSampleInfoSize);
        return OK;
    }
    if (smplcnt > mCurrentSampleInfoAllocSize) {
        mCurrentSampleInfoSizes = (uint8_t*) realloc(mCurrentSampleInfoSizes, smplcnt);
        mCurrentSampleInfoAllocSize = smplcnt;
    }

    mDataSource->readAt(offset, mCurrentSampleInfoSizes, smplcnt);
    return OK;
}

status_t MPEG4Source::parseSampleAuxiliaryInformationOffsets(off64_t offset, off64_t size) {
    ALOGV("parseSampleAuxiliaryInformationOffsets");
    // 14496-12 8.7.13
    uint8_t version;
    if (mDataSource->readAt(offset, &version, sizeof(version)) != 1) {
        return ERROR_IO;
    }
    offset++;

    uint32_t flags;
    if (!mDataSource->getUInt24(offset, &flags)) {
        return ERROR_IO;
    }
    offset += 3;

    uint32_t entrycount;
    if (!mDataSource->getUInt32(offset, &entrycount)) {
        return ERROR_IO;
    }
    offset += 4;

    if (entrycount > mCurrentSampleInfoOffsetsAllocSize) {
        mCurrentSampleInfoOffsets = (uint64_t*) realloc(mCurrentSampleInfoOffsets, entrycount * 8);
        mCurrentSampleInfoOffsetsAllocSize = entrycount;
    }
    mCurrentSampleInfoOffsetCount = entrycount;

    for (size_t i = 0; i < entrycount; i++) {
        if (version == 0) {
            uint32_t tmp;
            if (!mDataSource->getUInt32(offset, &tmp)) {
                return ERROR_IO;
            }
            mCurrentSampleInfoOffsets[i] = tmp;
            offset += 4;
        } else {
            uint64_t tmp;
            if (!mDataSource->getUInt64(offset, &tmp)) {
                return ERROR_IO;
            }
            mCurrentSampleInfoOffsets[i] = tmp;
            offset += 8;
        }
    }

    // parse clear/encrypted data

    off64_t drmoffset = mCurrentSampleInfoOffsets[0]; // from moof

    drmoffset += mCurrentMoofOffset;
    int ivlength;
    CHECK(mFormat->findInt32(kKeyCryptoDefaultIVSize, &ivlength));

    // read CencSampleAuxiliaryDataFormats
    for (size_t i = 0; i < mCurrentSampleInfoCount; i++) {
        Sample *smpl = &mCurrentSamples.editItemAt(i);

        memset(smpl->iv, 0, 16);
        if (mDataSource->readAt(drmoffset, smpl->iv, ivlength) != ivlength) {
            return ERROR_IO;
        }

        drmoffset += ivlength;

        int32_t smplinfosize = mCurrentDefaultSampleInfoSize;
        if (smplinfosize == 0) {
            smplinfosize = mCurrentSampleInfoSizes[i];
        }
        if (smplinfosize > ivlength) {
            uint16_t numsubsamples;
            if (!mDataSource->getUInt16(drmoffset, &numsubsamples)) {
                return ERROR_IO;
            }
            drmoffset += 2;
            for (size_t j = 0; j < numsubsamples; j++) {
                uint16_t numclear;
                uint32_t numencrypted;
                if (!mDataSource->getUInt16(drmoffset, &numclear)) {
                    return ERROR_IO;
                }
                drmoffset += 2;
                if (!mDataSource->getUInt32(drmoffset, &numencrypted)) {
                    return ERROR_IO;
                }
                drmoffset += 4;
                smpl->clearsizes.add(numclear);
                smpl->encryptedsizes.add(numencrypted);
            }
        } else {
            smpl->clearsizes.add(0);
            smpl->encryptedsizes.add(smpl->size);
        }
    }


    return OK;
}

status_t MPEG4Source::parseTrackFragmentHeader(off64_t offset, off64_t size) {

    if (size < 8) {
        return -EINVAL;
    }

    uint32_t flags;
    if (!mDataSource->getUInt32(offset, &flags)) { // actually version + flags
        return ERROR_MALFORMED;
    }

    if (flags & 0xff000000) {
        return -EINVAL;
    }

    if (!mDataSource->getUInt32(offset + 4, (uint32_t*)&mLastParsedTrackId)) {
        return ERROR_MALFORMED;
    }

    if (mLastParsedTrackId != mTrackId) {
        // this is not the right track, skip it
        return OK;
    }

    mTrackFragmentHeaderInfo.mFlags = flags;
    mTrackFragmentHeaderInfo.mTrackID = mLastParsedTrackId;
    offset += 8;
    size -= 8;

    ALOGV("fragment header: %08x %08x", flags, mTrackFragmentHeaderInfo.mTrackID);

    if (flags & TrackFragmentHeaderInfo::kBaseDataOffsetPresent) {
        if (size < 8) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt64(offset, &mTrackFragmentHeaderInfo.mBaseDataOffset)) {
            return ERROR_MALFORMED;
        }
        offset += 8;
        size -= 8;
    }

    if (flags & TrackFragmentHeaderInfo::kSampleDescriptionIndexPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mSampleDescriptionIndex)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (flags & TrackFragmentHeaderInfo::kDefaultSampleDurationPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mDefaultSampleDuration)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (flags & TrackFragmentHeaderInfo::kDefaultSampleSizePresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mDefaultSampleSize)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (flags & TrackFragmentHeaderInfo::kDefaultSampleFlagsPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mDefaultSampleFlags)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (!(flags & TrackFragmentHeaderInfo::kBaseDataOffsetPresent)) {
        mTrackFragmentHeaderInfo.mBaseDataOffset = mCurrentMoofOffset;
    }

    mTrackFragmentHeaderInfo.mDataOffset = 0;
    return OK;
}

status_t MPEG4Source::parseTrackFragmentRun(off64_t offset, off64_t size) {

    ALOGV("MPEG4Extractor::parseTrackFragmentRun");
    if (size < 8) {
        return -EINVAL;
    }

    enum {
        kDataOffsetPresent                  = 0x01,
        kFirstSampleFlagsPresent            = 0x04,
        kSampleDurationPresent              = 0x100,
        kSampleSizePresent                  = 0x200,
        kSampleFlagsPresent                 = 0x400,
        kSampleCompositionTimeOffsetPresent = 0x800,
    };

    uint32_t flags;
    if (!mDataSource->getUInt32(offset, &flags)) {
        return ERROR_MALFORMED;
    }
    ALOGV("fragment run flags: %08x", flags);

    if (flags & 0xff000000) {
        return -EINVAL;
    }

    if ((flags & kFirstSampleFlagsPresent) && (flags & kSampleFlagsPresent)) {
        // These two shall not be used together.
        return -EINVAL;
    }

    uint32_t sampleCount;
    if (!mDataSource->getUInt32(offset + 4, &sampleCount)) {
        return ERROR_MALFORMED;
    }
    offset += 8;
    size -= 8;

    uint64_t dataOffset = mTrackFragmentHeaderInfo.mDataOffset;

    uint32_t firstSampleFlags = 0;

    if (flags & kDataOffsetPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        int32_t dataOffsetDelta;
        if (!mDataSource->getUInt32(offset, (uint32_t*)&dataOffsetDelta)) {
            return ERROR_MALFORMED;
        }

        dataOffset = mTrackFragmentHeaderInfo.mBaseDataOffset + dataOffsetDelta;

        offset += 4;
        size -= 4;
    }

    if (flags & kFirstSampleFlagsPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &firstSampleFlags)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    uint32_t sampleDuration = 0, sampleSize = 0, sampleFlags = 0,
             sampleCtsOffset = 0;

    size_t bytesPerSample = 0;
    if (flags & kSampleDurationPresent) {
        bytesPerSample += 4;
    } else if (mTrackFragmentHeaderInfo.mFlags
            & TrackFragmentHeaderInfo::kDefaultSampleDurationPresent) {
        sampleDuration = mTrackFragmentHeaderInfo.mDefaultSampleDuration;
    } else {
        sampleDuration = mTrackFragmentHeaderInfo.mDefaultSampleDuration;
    }

    if (flags & kSampleSizePresent) {
        bytesPerSample += 4;
    } else if (mTrackFragmentHeaderInfo.mFlags
            & TrackFragmentHeaderInfo::kDefaultSampleSizePresent) {
        sampleSize = mTrackFragmentHeaderInfo.mDefaultSampleSize;
    } else {
        sampleSize = mTrackFragmentHeaderInfo.mDefaultSampleSize;
    }

    if (flags & kSampleFlagsPresent) {
        bytesPerSample += 4;
    } else if (mTrackFragmentHeaderInfo.mFlags
            & TrackFragmentHeaderInfo::kDefaultSampleFlagsPresent) {
        sampleFlags = mTrackFragmentHeaderInfo.mDefaultSampleFlags;
    } else {
        sampleFlags = mTrackFragmentHeaderInfo.mDefaultSampleFlags;
    }

    if (flags & kSampleCompositionTimeOffsetPresent) {
        bytesPerSample += 4;
    } else {
        sampleCtsOffset = 0;
    }

    if (size < sampleCount * bytesPerSample) {
        return -EINVAL;
    }

    Sample tmp;
	ALOGV("fragment sampleCount: %d", sampleCount);
    for (uint32_t i = 0; i < sampleCount; ++i) {
        if (flags & kSampleDurationPresent) {
            if (!mDataSource->getUInt32(offset, &sampleDuration)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        if (flags & kSampleSizePresent) {
            if (!mDataSource->getUInt32(offset, &sampleSize)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        if (flags & kSampleFlagsPresent) {
            if (!mDataSource->getUInt32(offset, &sampleFlags)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        if (flags & kSampleCompositionTimeOffsetPresent) {
            if (!mDataSource->getUInt32(offset, &sampleCtsOffset)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        ALOGV("adding sample %d at offset 0x%08llx, size %u, duration %u, "
              " flags 0x%08x", i + 1,
                dataOffset, sampleSize, sampleDuration,
                (flags & kFirstSampleFlagsPresent) && i == 0
                    ? firstSampleFlags : sampleFlags);
        tmp.offset = dataOffset;
        tmp.size = sampleSize;
        tmp.duration = sampleDuration;
        mCurrentSamples.add(tmp);

        dataOffset += sampleSize;
    }

    mTrackFragmentHeaderInfo.mDataOffset = dataOffset;

    return OK;
}

sp<MetaData> MPEG4Source::getFormat() {
    Mutex::Autolock autoLock(mLock);

    return mFormat;
}

size_t MPEG4Source::parseNALSize(const uint8_t *data) const {
    switch (mNALLengthSize) {
        case 1:
            return *data;
        case 2:
            return U16_AT(data);
        case 3:
            return ((size_t)data[0] << 16) | U16_AT(&data[1]);
        case 4:
            return U32_AT(data);
    }

    // This cannot happen, mNALLengthSize springs to life by adding 1 to
    // a 2-bit integer.
    CHECK(!"Should not be here.");

    return 0;
}

status_t MPEG4Source::read(
        MediaBuffer **out, const ReadOptions *options) {
    Mutex::Autolock autoLock(mLock);
    
    ALOGV("+read");
#if !defined(ANDROID_DEFAULT_MODE)
	const char *mime;
    CHECK(mFormat->findCString(kKeyMIMEType, &mime));
#endif

    CHECK(mStarted);

    if (mFirstMoofOffset > 0) {
        return fragmentedRead(out, options);
    }

#ifndef ANDROID_DEFAULT_MODE
    if (out != NULL)
#endif
    *out = NULL;

    int64_t targetSampleTimeUs = -1;
#ifndef ANDROID_DEFAULT_CODE//added by hai.li to support track time offset
	int64_t startTimeOffsetUs = ((int64_t)mSampleTable->getStartTimeOffset())*1000000/mTimescale;
#endif
    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;
#ifndef ANDROID_DEFAULT_CODE
	bool isTryRead = false;
#endif
    if (options && options->getSeekTo(&seekTimeUs, &mode)) {
#ifndef ANDROID_DEFAULT_CODE//added by hai.li to support track time offset
//		LOGE("SEEK TIME1=%lld", seekTimeUs);

		LOGD("seekTimeUs=%lld, seekMode=%d", seekTimeUs, mode);

		if (ReadOptions::SEEK_TRY_READ == mode) {
			isTryRead = true;
			mode = ReadOptions::SEEK_CLOSEST_SYNC;
		}
		if (startTimeOffsetUs != 0)
		{
			if (seekTimeUs < startTimeOffsetUs)
			{
				seekTimeUs = 0;
			}
			else
			{
				seekTimeUs -= startTimeOffsetUs;
			}

		}
//		LOGE("SEEK TIME2=%lld", seekTimeUs);
#endif
        uint32_t findFlags = 0;
        switch (mode) {
            case ReadOptions::SEEK_PREVIOUS_SYNC:
                findFlags = SampleTable::kFlagBefore;
                break;
            case ReadOptions::SEEK_NEXT_SYNC:
                findFlags = SampleTable::kFlagAfter;
                break;
            case ReadOptions::SEEK_CLOSEST_SYNC:
            case ReadOptions::SEEK_CLOSEST:
                findFlags = SampleTable::kFlagClosest;
                break;
            default:
                CHECK(!"Should not be here.");
                break;
        }

        uint32_t sampleIndex;
#ifdef ENABLE_PERF_JUMP_KEY_MECHANISM
		if (ReadOptions::SEEK_NEXT_SYNC == mode) {
			status_t err = mSampleTable->findSyncSampleNear(
					mCurrentSampleIndex, &sampleIndex, SampleTable::kFlagAfter);
			if (err != OK) {
				if (err == ERROR_OUT_OF_RANGE) {
					err = ERROR_END_OF_STREAM;
				}
                ALOGV("-read %d",__LINE__);
				return err;
			}
			syncSampleIndex = sampleIndex;
			ALOGD("SEEK_JUMP_NEXT_KEY, mCurrentSampleIndex=%d, sampleIndex=%d", mCurrentSampleIndex, sampleIndex);
		}
		else {
#endif
#ifndef ANDROID_DEFAULT_CODE//hai.li for Issue: ALPS32414
		status_t err = mSampleTable->findSampleAtTime(
				(seekTimeUs * mTimescale + 500000ll) / 1000000,
				&sampleIndex, findFlags);
#else
        status_t err = mSampleTable->findSampleAtTime(
                seekTimeUs * mTimescale / 1000000,
                &sampleIndex, findFlags);
#endif
        if (mode == ReadOptions::SEEK_CLOSEST) {
            // We found the closest sample already, now we want the sync
            // sample preceding it (or the sample itself of course), even
            // if the subsequent sync sample is closer.
            findFlags = SampleTable::kFlagBefore;
        }

        uint32_t syncSampleIndex;
        if (err == OK) {
            err = mSampleTable->findSyncSampleNear(
                    sampleIndex, &syncSampleIndex, findFlags);
        }

        uint32_t sampleTime;
        if (err == OK) {
            err = mSampleTable->getMetaDataForSample(
                    sampleIndex, NULL, NULL, &sampleTime);
			ALOGD("getMetaDataForSample: sampleIndex=%d, sampleTime=%d", sampleIndex, sampleTime);
        }

        if (err != OK) {
            if (err == ERROR_OUT_OF_RANGE) {
                // An attempt to seek past the end of the stream would
                // normally cause this ERROR_OUT_OF_RANGE error. Propagating
                // this all the way to the MediaPlayer would cause abnormal
                // termination. Legacy behaviour appears to be to behave as if
                // we had seeked to the end of stream, ending normally.
                err = ERROR_END_OF_STREAM;
            }
            ALOGV("end of stream");
            ALOGV("-read %d",__LINE__);
            return err;
        }
#ifdef ENABLE_PERF_JUMP_KEY_MECHANISM
		}
#endif

#ifndef ANDROID_DEFAULT_CODE//hai.li
#ifdef ENABLE_PERF_JUMP_KEY_MECHANISM
		if (mode == ReadOptions::SEEK_CLOSEST ||
			mode == ReadOptions::SEEK_JUMP_NEXT_KEY)
#else
        if (mode == ReadOptions::SEEK_CLOSEST)
#endif
		{
	        uint32_t sampleTime;
	        CHECK_EQ((status_t)OK, mSampleTable->getMetaDataForSample(
                    sampleIndex, NULL, NULL, &sampleTime));
            targetSampleTimeUs = (sampleTime * 1000000ll) / mTimescale + startTimeOffsetUs;
			LOGE("targetSampleTimeUs=%lld", targetSampleTimeUs);
        }
#else
        if (mode == ReadOptions::SEEK_CLOSEST) {
            targetSampleTimeUs = (sampleTime * 1000000ll) / mTimescale;
        }
#endif

        mCurrentSampleIndex = syncSampleIndex;
        if (mBuffer != NULL) {
            mBuffer->release();
            mBuffer = NULL;
        }

        // fall through
    }

    off64_t offset;
    size_t size;
    uint32_t cts;
    bool isSyncSample;
    bool newBuffer = false;
    if (mBuffer == NULL) {
        newBuffer = true;

        status_t err =
            mSampleTable->getMetaDataForSample(
                    mCurrentSampleIndex, &offset, &size, &cts, &isSyncSample);

        if (err != OK) {
#ifndef ANDROID_DEFAULT_CODE//added by hai.li for Issue:ALPS34394
			if (err == ERROR_OUT_OF_RANGE)
				err = ERROR_END_OF_STREAM;//Awesomeplayer only can handle this as eos
#endif
            ALOGV("-read %d",__LINE__);
            return err;
        }

#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
			if (isTryRead) {
				ALOGD("Try read");
				ssize_t result =
					mDataSource->readAt(offset, NULL, size);
				if (result == size) {
					ALOGD("Try read return ok");
                    ALOGV("-read %d",__LINE__);
					return OK;
				} else {
					ALOGD("Try read fail!");
                    ALOGV("-read %d",__LINE__);
					return INFO_TRY_READ_FAIL;
				}
			}
#endif
        err = mGroup->acquire_buffer(&mBuffer);

        if (err != OK) {
            CHECK(mBuffer == NULL);
            ALOGV("-read %d",__LINE__);
            return err;
        }
    }

    if (!mIsAVC || mWantsNALFragments) {
        if (newBuffer) {
            ssize_t num_bytes_read =
                mDataSource->readAt(offset, (uint8_t *)mBuffer->data(), size);
			
#if !defined(ANDROID_DEFAULT_MODE)
			LOGV("[%s--%d]Read SampleIndex:%d ifSync:%d", mime, __LINE__, mCurrentSampleIndex, isSyncSample);
#endif
            if (num_bytes_read < (ssize_t)size) {
                mBuffer->release();
                mBuffer = NULL;
                ALOGV("-read %d",__LINE__);
                return ERROR_IO;
            }

            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);
            mBuffer->meta_data()->clear();
#ifndef ANDROID_DEFAULT_CODE//modified by hai.li to support track time offset
            	mBuffer->meta_data()->setInt64(
                	   kKeyTime, ((int64_t)(cts+mSampleTable->getStartTimeOffset())* 1000000) / mTimescale);
#else
            mBuffer->meta_data()->setInt64(
                    kKeyTime, ((int64_t)cts * 1000000) / mTimescale);
#endif
#if !defined(ANDROID_DEFAULT_MODE)
			int64_t timestampUs;

			mBuffer->meta_data()->findInt64(kKeyTime, &timestampUs);
			ALOGD("[%s--%d]currentSample[%d] cts = %d, timestampUs = %lld, sampleStartTimeOffset = %d, targetSampleTimeUs = %lld, isSyncSample = %d, mTimescale = %d", 
				mime, __LINE__, mCurrentSampleIndex, cts, timestampUs, mSampleTable->getStartTimeOffset(), targetSampleTimeUs, isSyncSample, mTimescale);
#endif
            if (targetSampleTimeUs >= 0) {
                mBuffer->meta_data()->setInt64(
                        kKeyTargetTime, targetSampleTimeUs);
            }

            if (isSyncSample) {
                mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
            }

            ++mCurrentSampleIndex;
        }

        if (!mIsAVC) {
			#ifdef LOG_TO_FILE
			ALOGD("dump frame data(length%d) to file @line%d...", mBuffer->range_length(), __LINE__);
			fwrite(mBuffer->data() + mBuffer->range_offset(), 1, mBuffer->range_length(), fp);
			#endif
			
            *out = mBuffer;
            mBuffer = NULL;
            ALOGV("-read %d",__LINE__);
            return OK;
        }

        // Each NAL unit is split up into its constituent fragments and
        // each one of them returned in its own buffer.
#ifdef ANDROID_DEFAULT_CODE  //ALPS00238811
        CHECK(mBuffer->range_length() >= mNALLengthSize);
#endif
        const uint8_t *src =
            (const uint8_t *)mBuffer->data() + mBuffer->range_offset();

        size_t nal_size = parseNALSize(src);
#ifndef ANDROID_DEFAULT_CODE
		if ((mBuffer->range_length() < mNALLengthSize + nal_size) ||
				(mNALLengthSize + nal_size < mNALLengthSize))//When uint type nal_size is very large, e.g. 0xffff or 0xffffffff, the summary is small. In this case, there are some problems in later flow.
#else
		if (mBuffer->range_length() < mNALLengthSize + nal_size)
#endif
		{
			ALOGE("incomplete NAL unit.");

            mBuffer->release();
            mBuffer = NULL;
            ALOGV("-read %d",__LINE__);
            return ERROR_MALFORMED;
        }

        MediaBuffer *clone = mBuffer->clone();
        CHECK(clone != NULL);
        clone->set_range(mBuffer->range_offset() + mNALLengthSize, nal_size);

        CHECK(mBuffer != NULL);
        mBuffer->set_range(
                mBuffer->range_offset() + mNALLengthSize + nal_size,
                mBuffer->range_length() - mNALLengthSize - nal_size);
#ifdef ANDROID_DEFAULT_CODE  //ALPS00238811
        if (mBuffer->range_length() == 0)
#else
		if ((mBuffer->range_length() < mNALLengthSize) || (0 == nal_size))
#endif
		{
            mBuffer->release();
            mBuffer = NULL;
        }

		#ifdef LOG_TO_FILE
		ALOGD("dump frame data(length%d) to file @line%d...", clone->range_length(), __LINE__);
		fwrite(clone->data() + clone->range_offset(), 1, clone->range_length(), fp);
		#endif
		
        *out = clone;
        
        ALOGV("-read");
        
        return OK;
    } else {
        // Whole NAL units are returned but each fragment is prefixed by
        // the start code (0x00 00 00 01).
        ssize_t num_bytes_read = 0;
        int32_t drm = 0;
        bool usesDRM = (mFormat->findInt32(kKeyIsDRM, &drm) && drm != 0);
        if (usesDRM) {
            num_bytes_read =
                mDataSource->readAt(offset, (uint8_t*)mBuffer->data(), size);
        } else {
            num_bytes_read = mDataSource->readAt(offset, mSrcBuffer, size);
        }
		
#if !defined(ANDROID_DEFAULT_MODE)
		LOGV("[%s--%d]Read SampleIndex:%d ifSync:%d", mime, __LINE__, mCurrentSampleIndex, isSyncSample);
#endif

        if (num_bytes_read < (ssize_t)size) {
            mBuffer->release();
            mBuffer = NULL;
            ALOGV("-read %d",__LINE__);
            return ERROR_IO;
        }

        if (usesDRM) {
            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);

        } else {
            uint8_t *dstData = (uint8_t *)mBuffer->data();
            size_t srcOffset = 0;
            size_t dstOffset = 0;

            while (srcOffset < size) {
                bool isMalFormed = (srcOffset + mNALLengthSize > size);
                size_t nalLength = 0;
                if (!isMalFormed) {
                    nalLength = parseNALSize(&mSrcBuffer[srcOffset]);
                    srcOffset += mNALLengthSize;
                    isMalFormed = srcOffset + nalLength > size;
                }

                if (isMalFormed) {
                    ALOGE("Video is malformed");
                    mBuffer->release();
                    mBuffer = NULL;
                    ALOGV("-read %d",__LINE__);
                    return ERROR_MALFORMED;
                }

                if (nalLength == 0) {
                    continue;
                }

                CHECK(dstOffset + 4 <= mBuffer->size());

                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 1;
                memcpy(&dstData[dstOffset], &mSrcBuffer[srcOffset], nalLength);
                srcOffset += nalLength;
                dstOffset += nalLength;
            }
            CHECK_EQ(srcOffset, size);
            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, dstOffset);
        }

        mBuffer->meta_data()->clear();
#ifndef ANDROID_DEFAULT_CODE//modified by hai.li to support track time offset
        	mBuffer->meta_data()->setInt64(
            	    kKeyTime, ((int64_t)(cts+mSampleTable->getStartTimeOffset()) * 1000000) / mTimescale);
#else
        mBuffer->meta_data()->setInt64(
                kKeyTime, ((int64_t)cts * 1000000) / mTimescale);
#endif

#if !defined(ANDROID_DEFAULT_MODE)
		int64_t timestampUs;

		mBuffer->meta_data()->findInt64(kKeyTime, &timestampUs);
		ALOGD("[%s--%d]currentSample[%d] cts = %d, timestampUs = %lld, sampleStartTimeOffset = %d, targetSampleTimeUs = %lld, isSyncSample = %d, mTimescale = %d", 
			mime, __LINE__, mCurrentSampleIndex, cts, timestampUs, mSampleTable->getStartTimeOffset(), targetSampleTimeUs, isSyncSample, mTimescale);
#endif

        if (targetSampleTimeUs >= 0) {
            mBuffer->meta_data()->setInt64(
                    kKeyTargetTime, targetSampleTimeUs);
        }

        if (isSyncSample) {
            mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
        }

        ++mCurrentSampleIndex;
		
		#ifdef LOG_TO_FILE
		ALOGD("dump frame data(length%d) to file @line%d...", mBuffer->range_length(), __LINE__);
		fwrite(mBuffer->data() + mBuffer->range_offset(), 1, mBuffer->range_length(), fp);
		#endif

        *out = mBuffer;
        mBuffer = NULL;
        
        ALOGV("-read %d",__LINE__);
        return OK;
    }
}

status_t MPEG4Source::fragmentedRead(
        MediaBuffer **out, const ReadOptions *options) {

    ALOGV("MPEG4Source::fragmentedRead");

    CHECK(mStarted);

    *out = NULL;

    int64_t targetSampleTimeUs = -1;

    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;
    if (options && options->getSeekTo(&seekTimeUs, &mode)) {

        int numSidxEntries = mSegments.size();
        if (numSidxEntries != 0) {
            int64_t totalTime = 0;
            off64_t totalOffset = mFirstMoofOffset;
            for (int i = 0; i < numSidxEntries; i++) {
                const SidxEntry *se = &mSegments[i];
                if (totalTime + se->mDurationUs > seekTimeUs) {
                    // The requested time is somewhere in this segment
                    if ((mode == ReadOptions::SEEK_NEXT_SYNC) ||
                        (mode == ReadOptions::SEEK_CLOSEST_SYNC &&
                        (seekTimeUs - totalTime) > (totalTime + se->mDurationUs - seekTimeUs))) {
                        // requested next sync, or closest sync and it was closer to the end of
                        // this segment
                        totalTime += se->mDurationUs;
                        totalOffset += se->mSize;
                    }
                    break;
                }
                totalTime += se->mDurationUs;
                totalOffset += se->mSize;
            }
        mCurrentMoofOffset = totalOffset;
        mCurrentSamples.clear();
        mCurrentSampleIndex = 0;
        parseChunk(&totalOffset);
        mCurrentTime = totalTime * mTimescale / 1000000ll;
        }

        if (mBuffer != NULL) {
            mBuffer->release();
            mBuffer = NULL;
        }

        // fall through
    }

    off64_t offset = 0;
    size_t size;
    uint32_t cts = 0;
    bool isSyncSample = false;
    bool newBuffer = false;
    if (mBuffer == NULL) {
        newBuffer = true;

        if (mCurrentSampleIndex >= mCurrentSamples.size()) {
            // move to next fragment
            Sample lastSample = mCurrentSamples[mCurrentSamples.size() - 1];
            off64_t nextMoof = mNextMoofOffset; // lastSample.offset + lastSample.size;
            mCurrentMoofOffset = nextMoof;
            mCurrentSamples.clear();
            mCurrentSampleIndex = 0;
            parseChunk(&nextMoof);
                if (mCurrentSampleIndex >= mCurrentSamples.size()) {
                    return ERROR_END_OF_STREAM;
                }
        }

        const Sample *smpl = &mCurrentSamples[mCurrentSampleIndex];
        offset = smpl->offset;
        size = smpl->size;
        cts = mCurrentTime;
        mCurrentTime += smpl->duration;
        isSyncSample = (mCurrentSampleIndex == 0); // XXX

        status_t err = mGroup->acquire_buffer(&mBuffer);

        if (err != OK) {
            CHECK(mBuffer == NULL);
            ALOGV("acquire_buffer returned %d", err);
            return err;
        }
    }

    const Sample *smpl = &mCurrentSamples[mCurrentSampleIndex];
    const sp<MetaData> bufmeta = mBuffer->meta_data();
    bufmeta->clear();
    if (smpl->encryptedsizes.size()) {
        // store clear/encrypted lengths in metadata
        bufmeta->setData(kKeyPlainSizes, 0,
                smpl->clearsizes.array(), smpl->clearsizes.size() * 4);
        bufmeta->setData(kKeyEncryptedSizes, 0,
                smpl->encryptedsizes.array(), smpl->encryptedsizes.size() * 4);
        bufmeta->setData(kKeyCryptoIV, 0, smpl->iv, 16); // use 16 or the actual size?
        bufmeta->setInt32(kKeyCryptoDefaultIVSize, mDefaultIVSize);
        bufmeta->setInt32(kKeyCryptoMode, mCryptoMode);
        bufmeta->setData(kKeyCryptoKey, 0, mCryptoKey, 16);
    }

    if (!mIsAVC || mWantsNALFragments) {
        if (newBuffer) {
            ssize_t num_bytes_read =
                mDataSource->readAt(offset, (uint8_t *)mBuffer->data(), size);

            if (num_bytes_read < (ssize_t)size) {
                mBuffer->release();
                mBuffer = NULL;

                ALOGV("i/o error");
                return ERROR_IO;
            }

            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);
            mBuffer->meta_data()->setInt64(
                    kKeyTime, ((int64_t)cts * 1000000) / mTimescale);

            if (targetSampleTimeUs >= 0) {
                mBuffer->meta_data()->setInt64(
                        kKeyTargetTime, targetSampleTimeUs);
            }

            if (isSyncSample) {
                mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
            }

            ++mCurrentSampleIndex;
        }

        if (!mIsAVC) {
			#ifdef LOG_TO_FILE
			ALOGD("dump frame data(length%d) to file @line%d...", mBuffer->range_length(), __LINE__);
			fwrite(mBuffer->data() + mBuffer->range_offset(), 1, mBuffer->range_length(), fp);
			#endif
			
            *out = mBuffer;
            mBuffer = NULL;

            return OK;
        }

        // Each NAL unit is split up into its constituent fragments and
        // each one of them returned in its own buffer.

        CHECK(mBuffer->range_length() >= mNALLengthSize);

        const uint8_t *src =
            (const uint8_t *)mBuffer->data() + mBuffer->range_offset();

        size_t nal_size = parseNALSize(src);
        if (mBuffer->range_length() < mNALLengthSize + nal_size) {
            ALOGE("incomplete NAL unit.");

            mBuffer->release();
            mBuffer = NULL;

            return ERROR_MALFORMED;
        }

        MediaBuffer *clone = mBuffer->clone();
        CHECK(clone != NULL);
        clone->set_range(mBuffer->range_offset() + mNALLengthSize, nal_size);

        CHECK(mBuffer != NULL);
        mBuffer->set_range(
                mBuffer->range_offset() + mNALLengthSize + nal_size,
                mBuffer->range_length() - mNALLengthSize - nal_size);

        if (mBuffer->range_length() == 0) {
            mBuffer->release();
            mBuffer = NULL;
        }
		
		#ifdef LOG_TO_FILE
		ALOGD("dump frame data(length%d) to file @line%d...", clone->range_length(), __LINE__);
		fwrite(clone->data() + clone->range_offset(), 1, clone->range_length(), fp);
		#endif

        *out = clone;

        return OK;
    } else {
        ALOGV("whole NAL");
        // Whole NAL units are returned but each fragment is prefixed by
        // the start code (0x00 00 00 01).
        ssize_t num_bytes_read = 0;
        int32_t drm = 0;
        bool usesDRM = (mFormat->findInt32(kKeyIsDRM, &drm) && drm != 0);
        if (usesDRM) {
            num_bytes_read =
                mDataSource->readAt(offset, (uint8_t*)mBuffer->data(), size);
        } else {
            num_bytes_read = mDataSource->readAt(offset, mSrcBuffer, size);
        }

        if (num_bytes_read < (ssize_t)size) {
            mBuffer->release();
            mBuffer = NULL;

            ALOGV("i/o error");
            return ERROR_IO;
        }

        if (usesDRM) {
            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);

        } else {
            uint8_t *dstData = (uint8_t *)mBuffer->data();
            size_t srcOffset = 0;
            size_t dstOffset = 0;

            while (srcOffset < size) {
                bool isMalFormed = (srcOffset + mNALLengthSize > size);
                size_t nalLength = 0;
                if (!isMalFormed) {
                    nalLength = parseNALSize(&mSrcBuffer[srcOffset]);
                    srcOffset += mNALLengthSize;
                    isMalFormed = srcOffset + nalLength > size;
                }

                if (isMalFormed) {
                    ALOGE("Video is malformed");
                    mBuffer->release();
                    mBuffer = NULL;
                    return ERROR_MALFORMED;
                }

                if (nalLength == 0) {
                    continue;
                }

                CHECK(dstOffset + 4 <= mBuffer->size());

                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 1;
                memcpy(&dstData[dstOffset], &mSrcBuffer[srcOffset], nalLength);
                srcOffset += nalLength;
                dstOffset += nalLength;
            }
            CHECK_EQ(srcOffset, size);
            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, dstOffset);
        }

        mBuffer->meta_data()->setInt64(
                kKeyTime, ((int64_t)cts * 1000000) / mTimescale);

        if (targetSampleTimeUs >= 0) {
            mBuffer->meta_data()->setInt64(
                    kKeyTargetTime, targetSampleTimeUs);
        }

        if (isSyncSample) {
            mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
        }

        ++mCurrentSampleIndex;
		
		#ifdef LOG_TO_FILE
		ALOGD("dump frame data(length%d) to file @line%d...", mBuffer->range_length(), __LINE__);
		fwrite(mBuffer->data() + mBuffer->range_offset(), 1, mBuffer->range_length(), fp);
		#endif

        *out = mBuffer;
        mBuffer = NULL;

        return OK;
    }
}

MPEG4Extractor::Track *MPEG4Extractor::findTrackByMimePrefix(
        const char *mimePrefix) {
    for (Track *track = mFirstTrack; track != NULL; track = track->next) {
        const char *mime;
        if (track->meta != NULL
                && track->meta->findCString(kKeyMIMEType, &mime)
                && !strncasecmp(mime, mimePrefix, strlen(mimePrefix))) {
            return track;
        }
    }

    return NULL;
}

static bool LegacySniffMPEG4(
        const sp<DataSource> &source, String8 *mimeType, float *confidence) {
    uint8_t header[8];

    ssize_t n = source->readAt(4, header, sizeof(header));
    if (n < (ssize_t)sizeof(header)) 
	{
        return false;
    }

    if (!memcmp(header, "ftyp3gp", 7) || !memcmp(header, "ftypmp42", 8)
        || !memcmp(header, "ftyp3gr6", 8) || !memcmp(header, "ftyp3gs6", 8)
        || !memcmp(header, "ftyp3ge6", 8) || !memcmp(header, "ftyp3gg6", 8)
        || !memcmp(header, "ftypisom", 8) || !memcmp(header, "ftypM4V ", 8)
        || !memcmp(header, "ftypM4A ", 8) || !memcmp(header, "ftypf4v ", 8)
        || !memcmp(header, "ftypkddi", 8) || !memcmp(header, "ftypM4VP", 8)
        || !memcmp(header, "ftypiso4", 8)) 
    {
        *mimeType = MEDIA_MIMETYPE_CONTAINER_MPEG4;
        *confidence = 0.4;

        return true;
    }

    return false;
}

static bool isCompatibleBrand(uint32_t fourcc) {
    static const uint32_t kCompatibleBrands[] = {
        FOURCC('i', 's', 'o', 'm'),
        FOURCC('i', 's', 'o', '2'),
        FOURCC('a', 'v', 'c', '1'),
        #ifdef MTK_VIDEO_HEVC_SUPPORT
        FOURCC('i', 's', 'o', '4'),
        FOURCC('h', 'v', 'c', '1'),
        FOURCC('h', 'e', 'v', '1'),
        #endif
        FOURCC('3', 'g', 'p', '4'),
        FOURCC('m', 'p', '4', '1'),
        FOURCC('m', 'p', '4', '2'),

        // Won't promise that the following file types can be played.
        // Just give these file types a chance.
        FOURCC('q', 't', ' ', ' '),  // Apple's QuickTime
        FOURCC('M', 'S', 'N', 'V'),  // Sony's PSP

        FOURCC('3', 'g', '2', 'a'),  // 3GPP2
        FOURCC('3', 'g', '2', 'b'),
    };

    for (size_t i = 0;
         i < sizeof(kCompatibleBrands) / sizeof(kCompatibleBrands[0]);
         ++i) {
        if (kCompatibleBrands[i] == fourcc) {
            return true;
        }
    }

    return false;
}

// Attempt to actually parse the 'ftyp' atom and determine if a suitable
// compatible brand is present.
// Also try to identify where this file's metadata ends
// (end of the 'moov' atom) and report it to the caller as part of
// the metadata.
static bool BetterSniffMPEG4(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *meta) {
    // We scan up to 128 bytes to identify this file as an MP4.
    static const off64_t kMaxScanOffset = 128ll;

    off64_t offset = 0ll;
    bool foundGoodFileType = false;
    off64_t moovAtomEndOffset = -1ll;
    bool done = false;

#ifndef ANDROID_DEFAULT_CODE//hai.li: some files have no 'ftyp' atom, but they can be played in 2.2 version
    uint8_t header[12];
    // If type is not ftyp,mdata,moov or free, return false directly. Or else, it may be mpeg4 file.
    if (source->readAt(0, header, 12) != 12
		    || (memcmp("ftyp", &header[4], 4) && memcmp("mdat", &header[4], 4)
			    && memcmp("moov", &header[4], 4) && memcmp("free", &header[4], 4))) 
	{
        char type[5];
        MakeFourCCString((*((uint32_t *)&header[4])), type);
	    LOGE("return false, type=0x%8.8x, %s", *((uint32_t *)&header[4]), type);
	    return false;
    }
    *mimeType = MEDIA_MIMETYPE_CONTAINER_MPEG4;
    *confidence = 0.1f;
#endif  //ANDROID_DEFAULT_CODE

    while (!done && offset < kMaxScanOffset) 
	{
        uint32_t hdr[2];
        if (source->readAt(offset, hdr, 8) < 8) 
		{
            return false;
        }

        uint64_t chunkSize = ntohl(hdr[0]);
        uint32_t chunkType = ntohl(hdr[1]);
        off64_t chunkDataOffset = offset + 8;

        if (chunkSize == 1) 
		{
            if (source->readAt(offset + 8, &chunkSize, 8) < 8) 
			{
                return false;
            }

            chunkSize = ntoh64(chunkSize);
            chunkDataOffset += 8;

            if (chunkSize < 16) 
			{
                // The smallest valid chunk is 16 bytes long in this case.
                return false;
            }
        } 
		else if (chunkSize < 8) 
        {
            // The smallest valid chunk is 8 bytes long.
            return false;
        }

        off64_t chunkDataSize = offset + chunkSize - chunkDataOffset;

        char chunkstring[5];
        MakeFourCCString(chunkType, chunkstring);
        ALOGV("saw chunk type %s, size %lld @ %lld", chunkstring, chunkSize, offset);
        switch (chunkType) {
            case FOURCC('f', 't', 'y', 'p'):
            {
                if (chunkDataSize < 8) 
				{
                    return false;
                }

                uint32_t numCompatibleBrands = (chunkDataSize - 8) / 4;
                for (size_t i = 0; i < numCompatibleBrands + 2; ++i) 
				{
                    if (i == 1) 
					{
                        // Skip this index, it refers to the minorVersion,
                        // not a brand.
                        continue;
                    }

                    uint32_t brand;
                    if (source->readAt(chunkDataOffset + 4 * i, &brand, 4) < 4) 
                    {
                        return false;
                    }

                    brand = ntohl(brand);

                    if (isCompatibleBrand(brand)) 
					{
                        foundGoodFileType = true;
                        break;
                    }
                }

                if (!foundGoodFileType) 
				{
					#ifndef ANDROID_DEFAULT_CODE  //ALPS00112506 Don't use isCompatibleBrand to judge whether play or not
	            	ALOGW("Warning:ftyp brands is not isCompatibleBrand 1");
					#else //ANDROID_DEFAULT_CODE
                    return false;
					#endif
                }

                break;
            }

            case FOURCC('m', 'o', 'o', 'v'):
            {
                moovAtomEndOffset = offset + chunkSize;

                done = true;
                break;
            }

            default:
                break;
        }

        offset += chunkSize;
    }

#ifndef ANDROID_DEFAULT_CODE  //ALPS00112506 Don't use isCompatibleBrand to judge whether play or not
    //If foundGoodFileType, set confidence from 0.1f to 0.4f. Or else confidence is 0.1f
    if (foundGoodFileType) 
	{
	    *mimeType = MEDIA_MIMETYPE_CONTAINER_MPEG4;
	    *confidence = 0.4f;
    }
#else  // ANDROID_DEFAULT_CODE
    if (!foundGoodFileType)
	{
        return false;
    }

    *mimeType = MEDIA_MIMETYPE_CONTAINER_MPEG4;
    *confidence = 0.4f;
#endif

    if (moovAtomEndOffset >= 0) 
	{
        *meta = new AMessage;
        (*meta)->setInt64("meta-data-size", moovAtomEndOffset);

        ALOGV("found metadata size: %lld", moovAtomEndOffset);
    }

    return true;
}

bool SniffMPEG4(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *meta) {
    if (BetterSniffMPEG4(source, mimeType, confidence, meta)) 
	{
        ALOGI("SniffMPEG4(%s) BetterSniffMPEG4 return true(%f)", mimeType->string(), *confidence);
        return true;
    }

    if (LegacySniffMPEG4(source, mimeType, confidence)) 
	{
        ALOGW("Identified supported mpeg4 through LegacySniffMPEG4.");
        return true;
    }

    ALOGI("SniffMPEG4(%s) return false", mimeType->string());
    return false;
}

}  // namespace android
