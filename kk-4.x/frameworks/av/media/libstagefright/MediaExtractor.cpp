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
#define LOG_TAG "MediaExtractor"
#include <utils/Log.h>

#include "include/AMRExtractor.h"
#include "include/MP3Extractor.h"
#include "include/MPEG4Extractor.h"
#include "include/WAVExtractor.h"
#include "include/OggExtractor.h"
#include "include/MPEG2PSExtractor.h"
#include "include/MPEG2TSExtractor.h"
#include "include/DRMExtractor.h"
#include "include/WVMExtractor.h"
#include "include/FLACExtractor.h"
#include "include/AACExtractor.h"
#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
#ifdef MTK_AUDIO_APE_SUPPORT
#include "include/APEExtractor.h"
#endif
#endif //#ifndef ANDROID_DEFAULT_CODE

#include "matroska/MatroskaExtractor.h"

#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaExtractor.h>
#include <media/stagefright/MetaData.h>
#include <utils/String8.h>

#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
#include <dlfcn.h>

#ifdef MTK_OGM_PLAYBACK_SUPPORT
#include "include/OgmExtractor.h"
#endif
#ifdef MTK_PS_PLAYBACK_SUPPORT
#include "include/MPEG2PSExtractor.h"
#endif
#ifdef MTK_AVI_PLAYBACK_SUPPORT
#include "include/MtkAVIExtractor.h"
#endif
#endif

namespace android {
#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)

#ifdef MTK_ASF_PLAYBACK_SUPPORT
#define MTK_ASF_EXTRACTOR_LIB_NAME			"libasfextractor.so"
#define MTK_ASF_EXTRACTOR_FACTORY_NAME		"mtk_asf_extractor_create_instance"
typedef sp<MediaExtractor> AsfFactory_Ptr(const sp<DataSource> &source);
sp<MediaExtractor> ASFExtractorCreateInstance(const sp<DataSource> &source)
{
	bool ret = false;
	void* pAsfLib = NULL;

	pAsfLib = dlopen(MTK_ASF_EXTRACTOR_LIB_NAME, RTLD_NOW);
	if (NULL == pAsfLib) {
		LOGE ("%s", dlerror());
		return NULL;
	}

	AsfFactory_Ptr* asf_extractor_create_instance = (AsfFactory_Ptr*) dlsym(pAsfLib, MTK_ASF_EXTRACTOR_FACTORY_NAME);
	if (NULL == asf_extractor_create_instance) {
		LOGE ("%s", dlerror());
		dlclose(pAsfLib);
		return NULL;
	}

	return asf_extractor_create_instance(source);
}
#endif

#ifdef MTK_FLV_PLAYBACK_SUPPORT
#define MTK_FLV_EXTRACTOR_LIB_NAME			"libflvextractor.so"
#define MTK_FLV_EXTRACTOR_FACTORY_NAME		"mtk_flv_extractor_create_instance"
typedef sp<MediaExtractor> FlvFactory_Ptr(const sp<DataSource> &source);
sp<MediaExtractor> FLVExtractorCreateInstance(const sp<DataSource> &source)
{
	bool ret = false;
	void* pFlvLib = NULL;

	LOGE ("FLVExtractorCreateInstance enter");
	pFlvLib = dlopen(MTK_FLV_EXTRACTOR_LIB_NAME, RTLD_NOW);
	if (NULL == pFlvLib) {
		LOGE ("%s", dlerror());
		return NULL;
	}
	FlvFactory_Ptr* flv_extractor_create_instance = (FlvFactory_Ptr*) dlsym(pFlvLib, MTK_FLV_EXTRACTOR_FACTORY_NAME);
	if (NULL == flv_extractor_create_instance) {
		LOGE ("%s", dlerror());
		dlclose(pFlvLib);
		return NULL;
	}
	return flv_extractor_create_instance(source);
}
#endif

#endif // #ifndef ANDROID_DEFAULT_CODE

sp<MetaData> MediaExtractor::getMetaData() {
    return new MetaData;
}

uint32_t MediaExtractor::flags() const {
    return CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_PAUSE | CAN_SEEK;
}

// static
sp<MediaExtractor> MediaExtractor::Create(
        const sp<DataSource> &source, const char *mime
#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
            , uint32_t flags
#endif
        )
{
    sp<AMessage> meta;

#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
    ALOGD("+MediaExtractor::Create");
#endif

    String8 tmp;
    if (mime == NULL) {
        float confidence;
        if (!source->sniff(&tmp, &confidence, &meta)) {
#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
            ALOGE("FAILED to autodetect media content.");
#else
            ALOGV("FAILED to autodetect media content.");
#endif

            return NULL;
        }

        mime = tmp.string();
        ALOGV("Autodetected media content as '%s' with confidence %.2f",
             mime, confidence);
    }

    bool isDrm = false;
    // DRM MIME type syntax is "drm+type+original" where
    // type is "es_based" or "container_based" and
    // original is the content's cleartext MIME type
    if (!strncmp(mime, "drm+", 4)) {
#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
        ALOGD("create MediaExtractor: original mime: %s", mime);
#endif
        const char *originalMime = strchr(mime+4, '+');
        if (originalMime == NULL) {
            // second + not found
            return NULL;
        }
        ++originalMime;
        if (!strncmp(mime, "drm+es_based+", 13)) {
            // DRMExtractor sets container metadata kKeyIsDRM to 1
            return new DRMExtractor(source, originalMime);
        } else if (!strncmp(mime, "drm+container_based+", 20)) {
            mime = originalMime;
            isDrm = true;
        } else {
            return NULL;
        }
    }

#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
    ALOGD("MediaExtractor::Create mime=%s", mime);
#endif

#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
    sp<MediaExtractor> ret;
#else
    MediaExtractor *ret = NULL;
#endif
    if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_MPEG4)
            || !strcasecmp(mime, "audio/mp4")) {
        ret = new MPEG4Extractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_MPEG)) {
        ret = new MP3Extractor(source, meta);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AMR_NB)
            || !strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AMR_WB)) {
        ret = new AMRExtractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_FLAC)) {
        ret = new FLACExtractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_WAV)) {
        ret = new WAVExtractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_OGG)) {
        ret = new OggExtractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_MATROSKA)) {
#if defined(CC_SUPPORT_TV_STAGEFRIGHT) 
        ret = new MatroskaExtractor(source, flags);
#else
        ret = new MatroskaExtractor(source);
#endif
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_MPEG2TS)) {
        ret = new MPEG2TSExtractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_WVM)) {
        // Return now.  WVExtractor should not have the DrmFlag set in the block below.
        return new WVMExtractor(source);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AAC_ADTS)) {
        ret = new AACExtractor(source, meta);
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_MPEG2PS)) {
#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
#ifdef MTK_PS_PLAYBACK_SUPPORT
        ret = new MPEG2PSExtractor(source);
#endif
#else
        ret = new MPEG2PSExtractor(source);
#endif
    }
#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AAC)) {
        ret = new AACExtractor(source,meta);
    }
#ifdef MTK_AVI_PLAYBACK_SUPPORT
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_AVI)) {
        ret = new MtkAVIExtractor(source);
    }
#endif
#ifdef MTK_ASF_PLAYBACK_SUPPORT
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_ASF)) { // Morris Yang add for ASF
		   ret  = ASFExtractorCreateInstance(source);
    }
#endif
#ifdef MTK_FLV_PLAYBACK_SUPPORT
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_FLV)) {
         ret  =  FLVExtractorCreateInstance(source);
    }
#endif//#ifdef MTK_FLV_PLAYBACK_SUPPORT
#ifdef MTK_AUDIO_APE_SUPPORT
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_APE)) {
        ret = new APEExtractor(source, meta);
    }
#endif
#ifdef MTK_OGM_PLAYBACK_SUPPORT
    else if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_OGM)) {
        ret = new OgmExtractor(source);
    }
#endif//#ifdef MTK_OGM_PLAYBACK_SUPPORT
#endif // #ifndef ANDROID_DEFAULT_CODE

#if defined(CC_SUPPORT_TV_STAGEFRIGHT) && !defined(ANDROID_DEFAULT_CODE)
    if (ret.get() != NULL)
#else
    if (ret != NULL)
#endif
    {
       if (isDrm) {
           ret->setDrmFlag(true);
       } else {
           ret->setDrmFlag(false);
       }
    }
    
#if defined(CC_SUPPORT_TV_STAGEFRIGHT)
    ALOGD("-MediaExtractor::Create");
#endif

    return ret;
}

}  // namespace android

