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
#define LOG_TAG "WAVExtractor"
#include <utils/Log.h>

#include "include/WAVExtractor.h"

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MetaData.h>
#include <utils/String8.h>
#include <cutils/bitops.h>

#define CHANNEL_MASK_USE_CHANNEL_ORDER 0
#define WAV_DTS_DETECT_LEN 0x1000

namespace android {

enum {
    WAVE_FORMAT_PCM        = 0x0001,
    WAVE_FORMAT_ALAW       = 0x0006,
    WAVE_FORMAT_MULAW      = 0x0007,
    WAVE_FORMAT_MSGSM      = 0x0031,
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
	WAVE_FORMAT_ADPCM_MS = 0x2,
	WAVE_FORMAT_ADPCM_IMA = 0x11,
	//WAVE_FORMAT_ADPCM_YAMAHA = 0x20,
	//WAVE_FORMAT_ADPCM_G726 = 0x45,
	//WAVE_FORMAT_ADPCM_IMA_DK4 = 0x61,
	//WAVE_FORMAT_ADPCM_IMA_DK3 = 0x62,
	//WAVE_FORMAT_ADPCM_IMA = 0x69,
	//WAVE_FORMAT_ADPCM_CT = 0x200,    
#endif
	WAVE_FORMAT_DTS        = 0x300,
    WAVE_FORMAT_EXTENSIBLE = 0xFFFE
};

static const char* WAVEEXT_SUBFORMAT = "\x00\x00\x00\x00\x10\x00\x80\x00\x00\xAA\x00\x38\x9B\x71";


static uint32_t U32_LE_AT(const uint8_t *ptr) {
    return ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0];
}

static uint16_t U16_LE_AT(const uint8_t *ptr) {
    return ptr[1] << 8 | ptr[0];
}

static uint8_t pData[WAV_DTS_DETECT_LEN];

struct WAVSource : public MediaSource {
    WAVSource(
            const sp<DataSource> &dataSource,
            const sp<MetaData> &meta,
            uint16_t waveFormat,
            int32_t bitsPerSample,
            off64_t offset, size_t size);

    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();
    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **buffer, const ReadOptions *options = NULL);

protected:
    virtual ~WAVSource();

private:
    static const size_t kMaxFrameSize;

    sp<DataSource> mDataSource;
    sp<MetaData> mMeta;
    uint16_t mWaveFormat;
    int32_t mSampleRate;
    int32_t mNumChannels;
    int32_t mBitsPerSample;
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
	int32_t mBlockAlign;
	int32_t mFrameSize;
#endif

    off64_t mOffset;
    size_t mSize;
    bool mStarted;
    MediaBufferGroup *mGroup;
    off64_t mCurrentPos;

    WAVSource(const WAVSource &);
    WAVSource &operator=(const WAVSource &);
};

WAVExtractor::WAVExtractor(const sp<DataSource> &source)
    : mDataSource(source),
      mValidFormat(false),
      mbFormatIsDts(false),
      mChannelMask(CHANNEL_MASK_USE_CHANNEL_ORDER) {
    mInitCheck = init();
}

WAVExtractor::~WAVExtractor() {
}

sp<MetaData> WAVExtractor::getMetaData() {
    sp<MetaData> meta = new MetaData;

    if (mInitCheck != OK) {
        return meta;
    }

    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_CONTAINER_WAV);

    return meta;
}

size_t WAVExtractor::countTracks() {
    return mInitCheck == OK ? 1 : 0;
}

sp<MediaSource> WAVExtractor::getTrack(size_t index) {
    if (mInitCheck != OK || index > 0) {
        return NULL;
    }
    return new WAVSource(
            mDataSource, mTrackMeta,
            mWaveFormat, mBitsPerSample, mDataOffset, mDataSize);
}

sp<MetaData> WAVExtractor::getTrackMetaData(
        size_t index, uint32_t flags) {
    if (mInitCheck != OK || index > 0) {
        return NULL;
    }

    return mTrackMeta;
}

status_t WAVExtractor::init() {
    uint8_t header[12];
	uint16_t i;
	uint32_t tmpData;
    if (mDataSource->readAt(
                0, header, sizeof(header)) < (ssize_t)sizeof(header)) {
        return NO_INIT;
    }

    if (memcmp(header, "RIFF", 4) || memcmp(&header[8], "WAVE", 4)) {
        return NO_INIT;
    }
#if 0	
	//====== For DTS-CD detect ======
	if (mDataSource->readAt(12, pData, WAV_DTS_DETECT_LEN) < WAV_DTS_DETECT_LEN)
	{
		ALOGD("readAt fail!!===\n");
		return NO_INIT;
	}
	ALOGD("Start to Search DTS Header!!===\n");
	i = WAV_DTS_DETECT_LEN - 4;
	do
	{
		tmpData = U32_LE_AT(&pData[i]);
		if ((tmpData == 0x80017ffe) || (tmpData == 0xe8001fff))
		{
			mbFormatIsDts = true;
			mWaveFormat = WAVE_FORMAT_DTS;
			break;
		}
	}while (i -- >= 3);
	ALOGD("bIsDTS = %d, i = %d\n", mbFormatIsDts, i);
	ALOGD("pData[%d] = 0x%x, pData[%d] = 0x%x\n", i, pData[i], i+1, pData[i+1]);
	//=========================
#endif
	
    size_t totalSize = U32_LE_AT(&header[4]);

    off64_t offset = 12;
    size_t remainingSize = totalSize;
    while (remainingSize >= 8) {
        uint8_t chunkHeader[8];
        if (mDataSource->readAt(offset, chunkHeader, 8) < 8) {
            return NO_INIT;
        }

        remainingSize -= 8;
        offset += 8;

        uint32_t chunkSize = U32_LE_AT(&chunkHeader[4]);

        if (chunkSize > remainingSize) {
            return NO_INIT;
        }

        if (!memcmp(chunkHeader, "fmt ", 4)) {
            if (chunkSize < 16) {
                return NO_INIT;
            }

            uint8_t formatSpec[40];
            if (mDataSource->readAt(offset, formatSpec, 2) < 2) {
                return NO_INIT;
            }

            mWaveFormat = U16_LE_AT(formatSpec);
			ALOGD("mWaveFormat = %d\n", mWaveFormat);
            if (mWaveFormat != WAVE_FORMAT_PCM
                    && mWaveFormat != WAVE_FORMAT_ALAW
                    && mWaveFormat != WAVE_FORMAT_MULAW
                    && mWaveFormat != WAVE_FORMAT_EXTENSIBLE
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
                    && mWaveFormat != WAVE_FORMAT_ADPCM_MS
                    && mWaveFormat != WAVE_FORMAT_ADPCM_IMA                    
#endif
				) 
			{
				ALOGD("LPCM Format Unsupported!\n");
                return ERROR_UNSUPPORTED;
            }

            uint8_t fmtSize = 16;
            if (mWaveFormat == WAVE_FORMAT_EXTENSIBLE) {
                fmtSize = 40;
            }
            if (mDataSource->readAt(offset, formatSpec, fmtSize) < fmtSize) {
                return NO_INIT;
            }

            mNumChannels = U16_LE_AT(&formatSpec[2]);
            if (mWaveFormat != WAVE_FORMAT_EXTENSIBLE) {
                if (mNumChannels != 1 && mNumChannels != 2) {
                    ALOGW("More than 2 channels (%d) in non-WAVE_EXT, unknown channel mask",
                            mNumChannels);
                }
            } else {
                if (mNumChannels < 1 && mNumChannels > 8) {
                    return ERROR_UNSUPPORTED;
                }
            }

            mSampleRate = U32_LE_AT(&formatSpec[4]);

            if (mSampleRate == 0) {
                return ERROR_MALFORMED;
            }

            mBitsPerSample = U16_LE_AT(&formatSpec[14]);

            if (mWaveFormat == WAVE_FORMAT_PCM
                    || mWaveFormat == WAVE_FORMAT_EXTENSIBLE) {
                if (mBitsPerSample != 8 && mBitsPerSample != 16
                    && mBitsPerSample != 24) {
                    return ERROR_UNSUPPORTED;
                }
            }
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
			else if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
			{
				if (mBitsPerSample != 4) {
					ALOGE("ADPCM-MS/IMA mBitPerSample must be 4");
					return ERROR_UNSUPPORTED;
				}
			}
#endif
			else {
                CHECK(mWaveFormat == WAVE_FORMAT_MULAW
                        || mWaveFormat == WAVE_FORMAT_ALAW);
                if (mBitsPerSample != 8) {
					ALOGE("MULAW/ALAW mBitPerSample must be 8");
                    return ERROR_UNSUPPORTED;
                }
            }
			
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
			if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
			{
				mAvgBytesPerSec = U32_LE_AT(&formatSpec[8]);
				mBlockAlign = U16_LE_AT(&formatSpec[12]);
				if (mDataSource->readAt(offset+16, formatSpec, 4) < 4) {
					return NO_INIT;
				}
				mExtraDataSize = U16_LE_AT(formatSpec);
				mFrameSize = U16_LE_AT(&formatSpec[2]);
			}
#endif

            if (mWaveFormat == WAVE_FORMAT_EXTENSIBLE) {
                uint16_t validBitsPerSample = U16_LE_AT(&formatSpec[18]);
                if (validBitsPerSample != mBitsPerSample) {
                    if (validBitsPerSample != 0) {
                        ALOGE("validBits(%d) != bitsPerSample(%d) are not supported",
                                validBitsPerSample, mBitsPerSample);
                        return ERROR_UNSUPPORTED;
                    } else {
                        // we only support valitBitsPerSample == bitsPerSample but some WAV_EXT
                        // writers don't correctly set the valid bits value, and leave it at 0.
                        ALOGW("WAVE_EXT has 0 valid bits per sample, ignoring");
                    }
                }

                mChannelMask = U32_LE_AT(&formatSpec[20]);
                ALOGV("numChannels=%d channelMask=0x%x", mNumChannels, mChannelMask);
                if ((mChannelMask >> 18) != 0) {
                    ALOGE("invalid channel mask 0x%x", mChannelMask);
                    return ERROR_MALFORMED;
                }

                if ((mChannelMask != CHANNEL_MASK_USE_CHANNEL_ORDER)
                        && (popcount(mChannelMask) != mNumChannels)) {
                    ALOGE("invalid number of channels (%d) in channel mask (0x%x)",
                            popcount(mChannelMask), mChannelMask);
                    return ERROR_MALFORMED;
                }

                // In a WAVE_EXT header, the first two bytes of the GUID stored at byte 24 contain
                // the sample format, using the same definitions as a regular WAV header
                mWaveFormat = U16_LE_AT(&formatSpec[24]);
                if (mWaveFormat != WAVE_FORMAT_PCM
                        && mWaveFormat != WAVE_FORMAT_ALAW
                        && mWaveFormat != WAVE_FORMAT_MULAW) {
                    return ERROR_UNSUPPORTED;
                }
                if (memcmp(&formatSpec[26], WAVEEXT_SUBFORMAT, 14)) {
                    ALOGE("unsupported GUID");
                    return ERROR_UNSUPPORTED;
                }
            }

            mValidFormat = true;
        } else if (!memcmp(chunkHeader, "data", 4)) {
            if (mValidFormat) {
                mDataOffset = offset;
                mDataSize = chunkSize;

				//====== For DTS-CD detect ======
				if (mDataSource->readAt(offset, pData, WAV_DTS_DETECT_LEN) < WAV_DTS_DETECT_LEN)
				{
					ALOGD("ReadAt fail, skip DTS detect!!===\n");
				} else {
					ALOGD("Start to Search DTS Header!!===\n");
					ALOGD("pData[%d] = 0x%x, pData[%d] = 0x%x, pData[]%d\ = 0x%x\n", 0, pData[0], 1, pData[1], 2, pData[2]);
					i = WAV_DTS_DETECT_LEN - 4;
					do
					{
						tmpData = U32_LE_AT(&pData[i]);
						if ((tmpData == 0x80017ffe) || (tmpData == 0xe8001fff))
						{
							mbFormatIsDts = true;
							mWaveFormat = WAVE_FORMAT_DTS;
							mDataOffset += i;
							break;
						}
					}while (i -- >= 1);
					ALOGD("bIsDTS = %d, i = %d\n", mbFormatIsDts, i);
					ALOGD("pData[%d] = 0x%x, pData[%d] = 0x%x\n", i, pData[i], i+1, pData[i+1]);
					//========================= 
				}
                mTrackMeta = new MetaData;

                switch (mWaveFormat) {
                    case WAVE_FORMAT_PCM:
                        mTrackMeta->setCString(
                                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);
                        break;
                    case WAVE_FORMAT_ALAW:
                        mTrackMeta->setCString(
                                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_G711_ALAW);
                        break;
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
					case WAVE_FORMAT_ADPCM_MS:
						mTrackMeta->setCString(
								kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_ADPCM_MS);
						break;
												
					case WAVE_FORMAT_ADPCM_IMA:
						mTrackMeta->setCString(
								kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_ADPCM_IMA);						  
						break;
#endif
					case WAVE_FORMAT_DTS:
						mTrackMeta->setCString(
								kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_DTS);
						break;
                    default:
                        CHECK_EQ(mWaveFormat, (uint16_t)WAVE_FORMAT_MULAW);
                        mTrackMeta->setCString(
                                kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_G711_MLAW);
                        break;
                }

                mTrackMeta->setInt32(kKeyChannelCount, mNumChannels);
                //mTrackMeta->setInt32(kKeyChannelMask, mChannelMask);
                mTrackMeta->setInt32(kKeySampleRate, mSampleRate);
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
				if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
				{
					mTrackMeta->setInt32(kKeyBlockAlign, mBlockAlign);
					mTrackMeta->setInt32(kKeyFrameSize, mFrameSize);				
				}
#endif


                size_t bytesPerSample = mBitsPerSample >> 3;

                int64_t durationUs;

#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
				if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
				{
					durationUs = 1000000LL * mDataSize * mFrameSize / mBlockAlign / mSampleRate;
				}
				else
#endif 
				{
					durationUs =
					1000000LL * (mDataSize / (mNumChannels * bytesPerSample))
								/ mSampleRate;
				}

				if (mbFormatIsDts)
				{
					mTrackMeta->setInt32(kKeyDtsInfo, 1);
					ALOGD("Set MIME Type to DTS!!===\n");
				}

                mTrackMeta->setInt64(kKeyDuration, durationUs);

                return OK;
            }
        }

        offset += chunkSize;
    }

    return NO_INIT;
}

const size_t WAVSource::kMaxFrameSize = 32768;

WAVSource::WAVSource(
        const sp<DataSource> &dataSource,
        const sp<MetaData> &meta,
        uint16_t waveFormat,
        int32_t bitsPerSample,
        off64_t offset, size_t size)
    : mDataSource(dataSource),
      mMeta(meta),
      mWaveFormat(waveFormat),
      mSampleRate(0),
      mNumChannels(0),
      mBitsPerSample(bitsPerSample),
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
		mBlockAlign(0),
		mFrameSize(0),
#endif

      mOffset(offset),
      mSize(size),
      mStarted(false),
      mGroup(NULL) 
{
	//const char *mime;
    //bool success = mMeta->findCString(kKeyMIMEType, &mime);
    //CHECK(success);
	//if (strcasecmp(MEDIA_MIMETYPE_AUDIO_DTS, mime))
    {
    	CHECK(mMeta->findInt32(kKeySampleRate, &mSampleRate));
		CHECK(mMeta->findInt32(kKeyChannelCount, &mNumChannels));

#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
		if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
		{
			CHECK(mMeta->findInt32(kKeyBlockAlign, &mBlockAlign));
			CHECK(mMeta->findInt32(kKeyFrameSize, &mFrameSize));
		}
#endif
	}
	//else
	//{
	//	mCurrentPos = 0;
	//	ALOGD("DTS initial current position!!===\n");
	//}
    mMeta->setInt32(kKeyMaxInputSize, kMaxFrameSize);
}

WAVSource::~WAVSource() {
    if (mStarted) {
        stop();
    }
}

status_t WAVSource::start(MetaData *params) {
    ALOGD("WAVSource::start");

    CHECK(!mStarted);

    mGroup = new MediaBufferGroup;
    mGroup->add_buffer(new MediaBuffer(kMaxFrameSize));
	//if (mWaveFormat == WAVE_FORMAT_DTS)
	//{
	//	mOffset = 12;
	//	ALOGD("DTS initial offset = 12!!\n");
	//}
		
    if (mBitsPerSample == 8) {
        // As a temporary buffer for 8->16 bit conversion.
        mGroup->add_buffer(new MediaBuffer(kMaxFrameSize));
    }

    mCurrentPos = mOffset;

    mStarted = true;

    return OK;
}

status_t WAVSource::stop() {
    ALOGV("WAVSource::stop");

    CHECK(mStarted);

    delete mGroup;
    mGroup = NULL;

    mStarted = false;

    return OK;
}

sp<MetaData> WAVSource::getFormat() {
    ALOGV("WAVSource::getFormat");

    return mMeta;
}

status_t WAVSource::read(
        MediaBuffer **out, const ReadOptions *options) {
    *out = NULL;

    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;
    if (options != NULL && options->getSeekTo(&seekTimeUs, &mode)) {
        int64_t pos;
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
		if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
		{
			pos = ((seekTimeUs * mSampleRate) / 1000000 / mFrameSize * mBlockAlign);
		}
		else
#endif
		{
			pos = (seekTimeUs * mSampleRate) / 1000000 * mNumChannels * (mBitsPerSample >> 3);
		}
        if (pos > mSize) {
            pos = mSize;
        }
        mCurrentPos = pos + mOffset;
    }
	
	ALOGD("1. mCurrentPos = %lld\n", mCurrentPos);
    MediaBuffer *buffer;
    status_t err = mGroup->acquire_buffer(&buffer);
    if (err != OK) {
		ALOGD("Acquire Buffer Error!! mSampleRate = %d\n", mSampleRate);
        return err;
    }

#ifndef ANDROID_DEFAULT_CODE
	size_t maxBytesToRead=kMaxFrameSize;
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
	if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
	{
		maxBytesToRead = mBlockAlign; //TODO more data?
	}
	else
#endif
	{
		if(mBitsPerSample == 8)
			maxBytesToRead = kMaxFrameSize / 2;
		else if(mBitsPerSample == 24) 
			maxBytesToRead = kMaxFrameSize/3*3;
	}
#else
	size_t maxBytesToRead =
		mBitsPerSample == 8 ? kMaxFrameSize / 2 : kMaxFrameSize;
#endif
	ALOGD("2. mCurrentPos = %lld\n", mCurrentPos);
    size_t maxBytesAvailable =
        (mCurrentPos - mOffset >= (off64_t)mSize)
            ? 0 : mSize - (mCurrentPos - mOffset);

    if (maxBytesToRead > maxBytesAvailable) {
        maxBytesToRead = maxBytesAvailable;
    }
	//ALOGD("3. mCurrentPos = %lld\n", mCurrentPos);
	//if (mWaveFormat == WAVE_FORMAT_DTS)
	//{
	//	ALOGD("DTS detect success!!===\n");
	//	maxBytesToRead = 32768;
	//}
	//ALOGD("1:maxBytesToRead = %d, mCurrentPos = %lld====\n", maxBytesToRead, mCurrentPos);
    ssize_t n = mDataSource->readAt(
            mCurrentPos, buffer->data(),
            maxBytesToRead);
	ALOGD("2:maxBytesToRead = %d, mCurrentPos = %lld, n = %d====\n", maxBytesToRead, mCurrentPos, n);
    if (n <= 0) {
        buffer->release();
        buffer = NULL;
        return ERROR_END_OF_STREAM;
    }

    buffer->set_range(0, n);

    if (mWaveFormat == WAVE_FORMAT_PCM) {
        if (mBitsPerSample == 8) {
            // Convert 8-bit unsigned samples to 16-bit signed.

            MediaBuffer *tmp;
            CHECK_EQ(mGroup->acquire_buffer(&tmp), (status_t)OK);

            // The new buffer holds the sample number of samples, but each
            // one is 2 bytes wide.
            tmp->set_range(0, 2 * n);

            int16_t *dst = (int16_t *)tmp->data();
            const uint8_t *src = (const uint8_t *)buffer->data();
            ssize_t numBytes = n;

            while (numBytes-- > 0) {
                *dst++ = ((int16_t)(*src) - 128) * 256;
                ++src;
            }

            buffer->release();
            buffer = tmp;
        } else if (mBitsPerSample == 24) {
            // Convert 24-bit signed samples to 16-bit signed.

            const uint8_t *src =
                (const uint8_t *)buffer->data() + buffer->range_offset();
            int16_t *dst = (int16_t *)src;

            size_t numSamples = buffer->range_length() / 3;
            for (size_t i = 0; i < numSamples; ++i) {
                int32_t x = (int32_t)(src[0] | src[1] << 8 | src[2] << 16);
                x = (x << 8) >> 8;  // sign extension

                x = x >> 8;
                *dst++ = (int16_t)x;
                src += 3;
            }

            buffer->set_range(buffer->range_offset(), 2 * numSamples);
        }
    }
#if !defined(ANDROID_DEFAULT_CODE) && defined(CC_SUPPORT_TV_STAGEFRIGHT)
	if (mWaveFormat == WAVE_FORMAT_ADPCM_MS || mWaveFormat == WAVE_FORMAT_ADPCM_IMA)
	{
		buffer->meta_data()->setInt64(
				kKeyTime,
				1000000LL * (mCurrentPos - mOffset) * mFrameSize / mBlockAlign / mSampleRate);
	}
	else// if (mWaveFormat == WAVE_FORMAT_ALAW || mWaveFormat == WAVE_FORMAT_MULAW)
#endif
	{
		size_t bytesPerSample = mBitsPerSample >> 3;
	
		buffer->meta_data()->setInt64(
				kKeyTime,
				1000000LL * (mCurrentPos - mOffset)
					/ (mNumChannels * bytesPerSample) / mSampleRate);
	}
	//else
	//{
	//	buffer->meta_data()->setInt64(kKeyTime, 10);
	//}

    buffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
    mCurrentPos += n;

    *out = buffer;

    return OK;
}

////////////////////////////////////////////////////////////////////////////////

bool SniffWAV(
        const sp<DataSource> &source, String8 *mimeType, float *confidence,
        sp<AMessage> *) {
    LOGI("SniffWAV");
    char header[12];
    if (source->readAt(0, header, sizeof(header)) < (ssize_t)sizeof(header)) {
		LOGI("SniffWAV() return false");
        return false;
    }

    if (memcmp(header, "RIFF", 4) || memcmp(&header[8], "WAVE", 4)) {
		ALOGD("SniffWAV return FALSE Header is : %d %d %d %d %d %d %d %d", 
			header[0], header[1], header[2],header[3], header[8], header[9], header[10], header[11]);
        return false;
    }

//    sp<MediaExtractor> extractor = new WAVExtractor(source);
//    if (extractor->countTracks() == 0) {
//		ALOGD("SniffWAV Count Track return FALSE");
//        return false;
//    }

    *mimeType = MEDIA_MIMETYPE_CONTAINER_WAV;
    *confidence = 0.3f;
	ALOGD("SniffWAV return TRUE");
    return true;
}

}  // namespace android
