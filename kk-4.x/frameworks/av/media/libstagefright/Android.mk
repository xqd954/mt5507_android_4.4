LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include frameworks/av/media/libstagefright/codecs/common/Config.mk

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"

ifndef SUPPORT_COLOR_CONVERT
export SUPPORT_COLOR_CONVERT := true
endif

DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)

ifeq ($(strip $(DTS_SUPPORT)),true)
LOCAL_CFLAGS += -DDTS_SUPPORT
endif

ifeq "$(SUPPORT_COLOR_CONVERT)" "true"
LOCAL_CFLAGS += -DVDEC_RGB_IOMMU
LOCAL_CXX_FLAGS += -DVDEC_RGB_IOMMU
DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)
LOCAL_LDFLAGS += -L$(ANDROID_BUILD_TOP)/$(DEVICE_STRING)/DTV_OUT/shares_libraries \
    -lmtal
endif

ifeq "$(SUPPORT_SWOMX_ENABLE)" "true"
LOCAL_CFLAGS += -DTEST_GRALLOC
LOCAL_CXX_FLAGS += -DTEST_GRALLOC
endif

MTK_USES_STAGEFRIGHT_DEFAULT_CODE=no
#MULTI_CH_PLAYBACK_SUPPORT=yes
MTK_AUDIO_APE_SUPPORT=yes
MTK_SWIP_VORBIS=yes
#MTK_SWIP_AAC=yes
HAVE_AWBENCODE_FEATURE=no
HAVE_AACENCODE_FEATURE=no

MTK_FLV_PLAYBACK_SUPPORT=yes
MTK_ASF_PLAYBACK_SUPPORT=yes
MTK_AVI_PLAYBACK_SUPPORT=yes
MTK_OGM_PLAYBACK_SUPPORT=yes

ifeq ($(strip $(MTK_FLV_PLAYBACK_SUPPORT)), yes)
LOCAL_CPPFLAGS += -DMTK_FLV_PLAYBACK_SUPPORT
LOCAL_CFLAGS += -DMTK_FLV_PLAYBACK_SUPPORT
endif

LOCAL_MTK_PATH:=../../../../../mediatek/$(ANDROID_VERSION)/frameworks-ext/av/media/libstagefright

endif

LOCAL_SRC_FILES:=                         \
        ACodec.cpp                        \
        AACExtractor.cpp                  \
        AACWriter.cpp                     \
        AMRExtractor.cpp                  \
        AMRWriter.cpp                     \
        AudioPlayer.cpp                   \
        AudioSource.cpp                   \
        AwesomePlayer.cpp                 \
        CameraSource.cpp                  \
        CameraSourceTimeLapse.cpp         \
        DataSource.cpp                    \
        DRMExtractor.cpp                  \
        ESDS.cpp                          \
        FileSource.cpp                    \
        HTTPBase.cpp                      \
        JPEGSource.cpp                    \
        MP3Extractor.cpp                  \
        MPEG2TSWriter.cpp                 \
        MPEG4Extractor.cpp                \
        MPEG4Writer.cpp                   \
        MediaAdapter.cpp                  \
        MediaBuffer.cpp                   \
        MediaBufferGroup.cpp              \
        MediaCodec.cpp                    \
        MediaCodecList.cpp                \
        MediaDefs.cpp                     \
        MediaExtractor.cpp                \
        MediaMuxer.cpp                    \
        MediaSource.cpp                   \
        MetaData.cpp                      \
        NuCachedSource2.cpp               \
        NuMediaExtractor.cpp              \
        OMXClient.cpp                     \
        OMXCodec.cpp                      \
        OggExtractor.cpp                  \
        SampleIterator.cpp                \
        SampleTable.cpp                   \
        SkipCutBuffer.cpp                 \
        StagefrightMediaScanner.cpp       \
        StagefrightMetadataRetriever.cpp  \
        SurfaceMediaSource.cpp            \
        ThrottledSource.cpp               \
        TimeSource.cpp                    \
        TimedEventQueue.cpp               \
        Utils.cpp                         \
        VBRISeeker.cpp                    \
        WAVExtractor.cpp                  \
        WVMExtractor.cpp                  \
        XINGSeeker.cpp                    \
        avc_utils.cpp                     \
        mp4/FragmentedMP4Parser.cpp       \
        mp4/TrackFragment.cpp

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
################################################################

ifeq ($(MULTI_CH_PLAYBACK_SUPPORT),yes)
LOCAL_CFLAGS += -DFLAC_MULTI_CH_SUPPORT
endif

ifeq ($(strip $(HEVC_SUPPORT)),true)
LOCAL_CFLAGS += -DMTK_VIDEO_HEVC_SUPPORT
LOCAL_SRC_FILES +=\
hevc_utils.cpp
endif

ifeq ($(MTK_AVI_PLAYBACK_SUPPORT),yes)
LOCAL_SRC_FILES += \
        $(LOCAL_MTK_PATH)/MtkAVIExtractor.cpp
LOCAL_CPPFLAGS += -DMTK_AVI_PLAYBACK_SUPPORT
endif

ifeq ($(MTK_OGM_PLAYBACK_SUPPORT),yes)
LOCAL_SRC_FILES += \
        $(LOCAL_MTK_PATH)/OgmExtractor.cpp
LOCAL_CPPFLAGS += -DMTK_OGM_PLAYBACK_SUPPORT
endif

ifeq ($(MTK_AUDIO_APE_SUPPORT),yes)
LOCAL_CFLAGS += -DMTK_AUDIO_APE_SUPPORT

LOCAL_SRC_FILES += \
        $(LOCAL_MTK_PATH)/APEExtractor.cpp \
        $(LOCAL_MTK_PATH)/apetag.cpp
endif

ifeq ($(MTK_SWIP_VORBIS),yes)
  LOCAL_CFLAGS += -DUSE_MTK_DECODER
ifeq ($(MULTI_CH_PLAYBACK_SUPPORT),yes)
  LOCAL_CFLAGS += -DVORBIS_MULTI_CH_SUPPORT
endif
endif

LOCAL_SRC_FILES += \
$(LOCAL_MTK_PATH)/FLACExtractor.cpp


################################################################
endif

LOCAL_C_INCLUDES:= \
        $(TOP)/frameworks/av/include/media/stagefright/timedtext \
        $(TOP)/frameworks/native/include/media/hardware \
        $(TOP)/frameworks/native/services/connectivitymanager \
        $(TOP)/external/flac/include \
        $(TOP)/external/tremolo \
        $(TOP)/external/openssl/include \

LOCAL_C_INCLUDES += $(TOP)/frameworks/native/include/media/openmax

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
LOCAL_C_INCLUDES += \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/base/include \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/base/media/libstagefright/include \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/base/media/libstagefright \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/external/amr \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/external/awb \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/external/vorbisenc \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks-ext/av/media/libstagefright \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks-ext/av/media/libstagefright/include \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/kernel/include/linux \
        $(TOP)/../mediatek/device/include/vcodec \
        $(TOP)/../mediatek/hardware/include \
        $(TOP)/../../chiling/app_if/mtal/include
endif

LOCAL_SHARED_LIBRARIES := \
        libbinder \
        libcamera_client \
        libconnectivitymanager \
        libcutils \
        libdl \
        libdrmframework \
        libexpat \
        libgui \
        libicui18n \
        libicuuc \
        liblog \
        libmedia \
        libsonivox \
        libssl \
        libstagefright_omx \
        libstagefright_yuv \
        libsync \
        libui \
        libutils \
        libvorbisidec \
        libz \
        libpowermanager

LOCAL_STATIC_LIBRARIES := \
        libstagefright_color_conversion \
        libstagefright_aacenc \
        libstagefright_matroska \
        libstagefright_timedtext \
        libvpx \
        libwebm \
        libstagefright_mpeg2ts \
        libstagefright_id3 \
        libFLAC \
        libmedia_helper

LOCAL_SRC_FILES += \
        chromium_http_stub.cpp
LOCAL_CPPFLAGS += -DCHROMIUM_AVAILABLE=1

LOCAL_SHARED_LIBRARIES += libstlport
include external/stlport/libstlport.mk

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
################################################################################

ifneq ($(strip $(MTK_USES_STAGEFRIGHT_DEFAULT_CODE)),yes)
ifeq ($(MTK_AUDIO_APE_SUPPORT),yes)
LOCAL_C_INCLUDES += \
        $(TOP)/../mediatek/$(ANDROID_VERSION)/external/apedec

LOCAL_STATIC_LIBRARIES +=\
        libapedec_mtk \
        libdrvb_boxtv

endif
endif

ifeq ($(MULTI_CH_PLAYBACK_SUPPORT),yes)
ifeq ($(MTK_SWIP_AAC),yes)
LOCAL_CFLAGS += -DUSE_MTK_AAC
LOCAL_C_INCLUDES += \
$(TOP)/../mediatek/$(ANDROID_VERSION)/external/aacdec/new

LOCAL_STATIC_LIBRARIES += \
libheaacdec_mtk \
libdrvb_boxtv
endif
endif

ifeq ($(strip $(HAVE_AWBENCODE_FEATURE)),yes)
    LOCAL_CFLAGS += -DHAVE_AWBENCODE
endif

ifeq ($(strip $(HAVE_AACENCODE_FEATURE)),yes)
    LOCAL_CFLAGS += -DHAVE_AACENCODE
endif

################################################################################
endif

LOCAL_SHARED_LIBRARIES += \
        libstagefright_enc_common \
        libstagefright_avc_common \
        libstagefright_foundation \
        libdl

LOCAL_CFLAGS += -Wno-multichar

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
###
# ANDROID_DEFAULT_HTTP_STREAM is used to check android default http streaming
# how to: LOCAL_CFLAGS += ANDROID_DEFAULT_HTTP_STREAM
# notice: if ANDROID_DEFAULT_CODE define, ANDROID_DEFAULT_HTTP_STREAM must be defined
ifeq ($(strip $(MTK_USES_STAGEFRIGHT_DEFAULT_CODE)),yes)
LOCAL_CFLAGS += -DANDROID_DEFAULT_CODE
LOCAL_CFLAGS += -DANDROID_DEFAULT_HTTP_STREAM
else

# MTK PS PLAYBACK SUPPORT
LOCAL_CFLAGS += -DMTK_PS_PLAYBACK_SUPPORT
#LOCAL_STATIC_LIBRARIES += libstagefright_mpegps

ifeq ($(strip $(MTK_ASF_PLAYBACK_SUPPORT)), yes)
LOCAL_CFLAGS += -DMTK_ASF_PLAYBACK_SUPPORT
endif

LOCAL_SRC_FILES += $(LOCAL_MTK_PATH)/TableOfContentThread.cpp
endif

endif

ifeq "$(SUPPORT_MTK_ANDROID_RM)" "true"
LOCAL_C_INCLUDES += \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/client \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/       \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/include    
ifneq "$(RLS_CUSTOM_BUILD)" "true"
LOCAL_SHARED_LIBRARIES += libMtkRmClient
else
LOCAL_LDFLAGS  += -L$(ANDROID_BUILD_TOP)/../mediatek/device/prebuilt/ -lMtkRmClient
endif
endif

LOCAL_C_INCLUDES += \
    $(ANDROID_BUILD_TOP)/../mediatek/device/hwcomposer_kk

ifneq ($(RLS_CUSTOM_BUILD),true)
LOCAL_SHARED_LIBRARIES += libbinder.hwcomposer.$(TARGET_BOARD_PLATFORM)
else
LOCAL_LDFLAGS += -L$(ANDROID_BUILD_TOP)/android/mediatek/device/prebuilt/ -lbinder.hwcomposer.$(TARGET_BOARD_PLATFORM)
endif

LOCAL_MODULE:= libstagefright

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
