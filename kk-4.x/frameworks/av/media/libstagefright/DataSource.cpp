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
#define LOG_TAG "DataSource"

#include "include/AMRExtractor.h"

#if CHROMIUM_AVAILABLE
#include "include/chromium_http_stub.h"
#endif

#include "include/AACExtractor.h"
#include "include/DRMExtractor.h"
#include "include/FLACExtractor.h"
#include "include/HTTPBase.h"
#include "include/MP3Extractor.h"
#include "include/MPEG2PSExtractor.h"
#include "include/MPEG2TSExtractor.h"
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
#ifdef MTK_AUDIO_APE_SUPPORT
#include "include/APEExtractor.h"
#endif
#endif  //#ifndef ANDROID_DEFAULT_CODE
#include "include/MPEG4Extractor.h"
#include "include/NuCachedSource2.h"
#include "include/OggExtractor.h"
#include "include/WAVExtractor.h"
#include "include/WVMExtractor.h"

#include "matroska/MatroskaExtractor.h"

#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/FileSource.h>
#include <media/stagefright/MediaErrors.h>
#include <utils/String8.h>

#include <cutils/properties.h>

#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
#include <media/stagefright/MediaDefs.h>
#include <dlfcn.h>
#endif  //#ifndef ANDROID_DEFAULT_CODE

#ifdef MTK_OGM_PLAYBACK_SUPPORT
#include "include/OgmExtractor.h"
#endif
#ifdef MTK_PS_PLAYBACK_SUPPORT
#include "include/MPEG2PSExtractor.h"
#endif

#ifdef MTK_AVI_PLAYBACK_SUPPORT
#include "include/MtkAVIExtractor.h"
#endif
namespace android {
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)

#ifdef MTK_ASF_PLAYBACK_SUPPORT
#define MTK_ASF_EXTRACTOR_LIB_NAME			"libasfextractor.so"
#define MTK_ASF_EXTRACTOR_RECOGNIZER_NAME	"mtk_asf_extractor_recognize"
#define MTK_ASF_EXTRACTOR_FACTORY_NAME		"mtk_asf_extractor_create_instance"
typedef sp<MediaExtractor> AsfFactory_Ptr(const sp<DataSource> &source);
typedef bool AsfRecognizer_Ptr(const sp<DataSource> &source);
bool SniffASF(const sp<DataSource> &source, String8 *mimeType, float *confidence, sp<AMessage> *) {
	//LOGI("SniffASF");
	bool ret = false;
	void* pAsfLib = NULL;

	pAsfLib = dlopen(MTK_ASF_EXTRACTOR_LIB_NAME, RTLD_NOW);
	if (NULL == pAsfLib) {
		LOGE ("SniffASF: %s", dlerror());
		return NULL;
	}

	AsfRecognizer_Ptr* asf_extractor_recognize = (AsfRecognizer_Ptr*) dlsym(pAsfLib, MTK_ASF_EXTRACTOR_RECOGNIZER_NAME);
	if (NULL == asf_extractor_recognize) {
		LOGE ("SniffASF: %s", dlerror());
		ret = false;
	}

	if (asf_extractor_recognize(source)) {
		*mimeType = MEDIA_MIMETYPE_CONTAINER_ASF;
		*confidence = 0.8;
        LOGI("SniffASF(%s) return true(%f)", mimeType->string(), *confidence);
		ret = true;
	}
	else {
        LOGI("SniffASF(%s) return false", mimeType->string());
		ret = false;
	}

	dlclose(pAsfLib);

	//LOGE ("SniffASF return %d", ret);
	return ret;
}
#endif

#ifdef MTK_FLV_PLAYBACK_SUPPORT
#define MTK_FLV_EXTRACTOR_LIB_NAME			"libflvextractor.so"
#define MTK_FLV_EXTRACTOR_RECOGNIZER_NAME	"mtk_flv_extractor_recognize"
#define MTK_FLV_EXTRACTOR_FACTORY_NAME		"mtk_flv_extractor_create_instance"
typedef sp<MediaExtractor> FlvFactory_Ptr(const sp<DataSource> &source);
typedef bool FlvRecognizer_Ptr(const sp<DataSource> &source);
bool SniffFLV(const sp<DataSource> &source, String8 *mimeType, float *confidence, sp<AMessage> *) {

	//LOGI("SniffFLV");
	bool ret = false;
	void* pFlvLib = NULL;
	pFlvLib = dlopen(MTK_FLV_EXTRACTOR_LIB_NAME, RTLD_NOW);
	if (NULL == pFlvLib) {
		LOGE ("SniffFLV: %s", dlerror());
		return NULL;
	}
	FlvRecognizer_Ptr* flv_extractor_recognize = (FlvRecognizer_Ptr*) dlsym(pFlvLib, MTK_FLV_EXTRACTOR_RECOGNIZER_NAME);
	if (NULL == flv_extractor_recognize) {
		LOGE ("SniffFLV: %s", dlerror());
		ret = false;
	}
	if (flv_extractor_recognize(source)) {
		*mimeType = MEDIA_MIMETYPE_CONTAINER_FLV;
		*confidence = 0.8;
        LOGI("SniffFLV(%s) return true(%f)", mimeType->string(), *confidence);
		ret = true;
	}
	else {
        LOGI("SniffFLV(%s) return false", mimeType->string());
		ret = false;
	}
	dlclose(pFlvLib);
	//LOGE ("SniffFLV return %d", ret);
	return ret;
}
#endif //#ifdef MTK_FLV_PLAYBACK_SUPPORT
#endif  //#ifndef ANDROID_DEFAULT_CODE

bool DataSource::getUInt16(off64_t offset, uint16_t *x) {
    *x = 0;

    uint8_t byte[2];
    if (readAt(offset, byte, 2) != 2) {
        return false;
    }

    *x = (byte[0] << 8) | byte[1];

    return true;
}

bool DataSource::getUInt24(off64_t offset, uint32_t *x) {
    *x = 0;

    uint8_t byte[3];
    if (readAt(offset, byte, 3) != 3) {
        return false;
    }

    *x = (byte[0] << 16) | (byte[1] << 8) | byte[2];

    return true;
}

bool DataSource::getUInt32(off64_t offset, uint32_t *x) {
    *x = 0;

    uint32_t tmp;
    if (readAt(offset, &tmp, 4) != 4) {
        return false;
    }

    *x = ntohl(tmp);

    return true;
}

bool DataSource::getUInt64(off64_t offset, uint64_t *x) {
    *x = 0;

    uint64_t tmp;
    if (readAt(offset, &tmp, 8) != 8) {
        return false;
    }

    *x = ntoh64(tmp);

    return true;
}

status_t DataSource::getSize(off64_t *size) {
    *size = 0;

    return ERROR_UNSUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

Mutex DataSource::gSnifferMutex;
List<DataSource::SnifferFunc> DataSource::gSniffers;
bool DataSource::gSniffersRegistered = false;

bool DataSource::sniff(
        String8 *mimeType, float *confidence, sp<AMessage> *meta) {
    *mimeType = "";
    *confidence = 0.0f;
    meta->clear();

    {
        Mutex::Autolock autoLock(gSnifferMutex);
        if (!gSniffersRegistered) {
            return false;
        }
    }

    for (List<SnifferFunc>::iterator it = gSniffers.begin();
         it != gSniffers.end(); ++it) {
        String8 newMimeType;
        float newConfidence;
        sp<AMessage> newMeta;
        if ((*it)(this, &newMimeType, &newConfidence, &newMeta)) {
            if (newConfidence > *confidence) {
                *mimeType = newMimeType;
                *confidence = newConfidence;
                *meta = newMeta;
            }
        }
    }

    return *confidence > 0.0;
}

// static
void DataSource::RegisterSniffer_l(SnifferFunc func) {
    for (List<SnifferFunc>::iterator it = gSniffers.begin();
         it != gSniffers.end(); ++it) {
        if (*it == func) {
            return;
        }
    }

    gSniffers.push_back(func);
}

// static
void DataSource::RegisterDefaultSniffers() {
    Mutex::Autolock autoLock(gSnifferMutex);
    if (gSniffersRegistered) {
        return;
    }

#ifndef ANDROID_DEFAULT_CODE
    char value[PROPERTY_VALUE_MAX];
    if (property_get("drm.service.enabled", value, NULL)
            && (!strcmp(value, "1") || !strcasecmp(value, "true"))) {
        RegisterSniffer_l(SniffDRM);
    }
#endif

    RegisterSniffer_l(SniffMPEG4);
    RegisterSniffer_l(SniffMatroska);
    RegisterSniffer_l(SniffOgg);
    RegisterSniffer_l(SniffWAV);
    RegisterSniffer_l(SniffFLAC);
    RegisterSniffer_l(SniffAMR);
    RegisterSniffer_l(SniffMPEG2TS);
    RegisterSniffer_l(SniffMP3);
#ifndef ANDROID_DEFAULT_CODE
#ifdef MTK_AUDIO_APE_SUPPORT
    RegisterSniffer_l(SniffAPE);
#endif
    //RegisterSniffer(SniffMtkAAC);
#endif
    RegisterSniffer_l(SniffAAC);
#if defined (CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
#ifdef MTK_ASF_PLAYBACK_SUPPORT
    RegisterSniffer_l(SniffASF);
#endif
#ifdef MTK_FLV_PLAYBACK_SUPPORT
  	RegisterSniffer_l(SniffFLV);
#endif

#ifdef MTK_PS_PLAYBACK_SUPPORT
    RegisterSniffer_l(SniffMPEG2PS);
#endif
#else
    RegisterSniffer_l(SniffMPEG2PS);
#endif
#ifdef MTK_OGM_PLAYBACK_SUPPORT
    RegisterSniffer_l(SniffOgm);
#endif
    RegisterSniffer_l(SniffWVM);
#ifdef MTK_AVI_PLAYBACK_SUPPORT
    RegisterSniffer_l(MtkSniffAVI);
#endif

#ifdef ANDROID_DEFAULT_CODE
    // for android default code, the DRM sniffer should be registed for WV DRM here.
    char value[PROPERTY_VALUE_MAX];
    if (property_get("drm.service.enabled", value, NULL)
            && (!strcmp(value, "1") || !strcasecmp(value, "true"))) {
        RegisterSniffer_l(SniffDRM);
    }
#endif
    gSniffersRegistered = true;
}

// static
sp<DataSource> DataSource::CreateFromURI(
        const char *uri, const KeyedVector<String8, String8> *headers) {
    bool isWidevine = !strncasecmp("widevine://", uri, 11);

    sp<DataSource> source;
    if (!strncasecmp("file://", uri, 7)) {
        source = new FileSource(uri + 7);
    } else if (!strncasecmp("http://", uri, 7)
            || !strncasecmp("https://", uri, 8)
            || isWidevine) {
        sp<HTTPBase> httpSource = HTTPBase::Create();

        String8 tmp;
        if (isWidevine) {
            tmp = String8("http://");
            tmp.append(uri + 11);

            uri = tmp.string();
        }

        if (httpSource->connect(uri, headers) != OK) {
            return NULL;
        }

        if (!isWidevine) {
            String8 cacheConfig;
            bool disconnectAtHighwatermark;
            if (headers != NULL) {
                KeyedVector<String8, String8> copy = *headers;
                NuCachedSource2::RemoveCacheSpecificHeaders(
                        &copy, &cacheConfig, &disconnectAtHighwatermark);
            }

            source = new NuCachedSource2(
                    httpSource,
                    cacheConfig.isEmpty() ? NULL : cacheConfig.string());
        } else {
            // We do not want that prefetching, caching, datasource wrapper
            // in the widevine:// case.
            source = httpSource;
        }

# if CHROMIUM_AVAILABLE
    } else if (!strncasecmp("data:", uri, 5)) {
        source = createDataUriSource(uri);
#endif
    } else {
        // Assume it's a filename.
        source = new FileSource(uri);
    }

    if (source == NULL || source->initCheck() != OK) {
        return NULL;
    }

    return source;
}

String8 DataSource::getMIMEType() const {
    return String8("application/octet-stream");
}

}  // namespace android
