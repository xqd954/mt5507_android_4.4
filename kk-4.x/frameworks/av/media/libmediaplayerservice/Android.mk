LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=               \
    ActivityManager.cpp         \
    Crypto.cpp                  \
    Drm.cpp                     \
    HDCP.cpp                    \
    MediaPlayerFactory.cpp      \
    MediaPlayerService.cpp      \
    MediaRecorderClient.cpp     \
    MetadataRetrieverClient.cpp \
    MidiFile.cpp                \
    MidiMetadataRetriever.cpp   \
    RemoteDisplay.cpp           \
    SharedLibrary.cpp           \
    StagefrightPlayer.cpp       \
    StagefrightRecorder.cpp     \
    TestPlayerStub.cpp          \

LOCAL_SHARED_LIBRARIES :=       \
    libbinder                   \
    libcamera_client            \
    libcutils                   \
    liblog                      \
    libdl                       \
    libgui                      \
    libmedia                    \
    libsonivox                  \
    libstagefright              \
    libstagefright_foundation   \
    libstagefright_httplive     \
    libstagefright_omx          \
    libstagefright_wfd          \
    libutils                    \
    libvorbisidec               \

ifeq "$(SUPPORT_MTK_ANDROID_RM)" "true"
LOCAL_C_INCLUDES += \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/client \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager        \
	$(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/include\
    $(ANDROID_BUILD_TOP)/../mediatek/device/hwcomposer_kk
ifneq "$(RLS_CUSTOM_BUILD)" "true"
LOCAL_SHARED_LIBRARIES += libMtkRmClient
else
LOCAL_LDFLAGS  += -L$(ANDROID_BUILD_TOP)/../mediatek/device/prebuilt/ -lMtkRmClient
endif
endif

ifneq ($(RLS_CUSTOM_BUILD),true)
LOCAL_SHARED_LIBRARIES += libbinder.hwcomposer.$(TARGET_BOARD_PLATFORM)
else
LOCAL_LDFLAGS += -L$(MTK_VMLINUX_ROOT)/android/mediatek/device/prebuilt/ -lbinder.hwcomposer.$(TARGET_BOARD_PLATFORM)
endif

ifeq "$(MTKSTREAM_PLAYER_SUPPORT)" "true"
 LOCAL_SHARED_LIBRARIES += libmtkstrm_player
endif

LOCAL_STATIC_LIBRARIES :=       \
    libstagefright_nuplayer     \
    libstagefright_rtsp         \

ifneq "$(RLS_CUSTOM_BUILD)" "true"
LOCAL_SHARED_LIBRARIES += libmtkcmpb
else
LOCAL_LDFLAGS  += -L$(ANDROID_BUILD_TOP)/../mediatek/device/prebuilt/ -lmtkcmpb
#LOCAL_LDFLAGS  += -L$(ANDROID_BUILD_TOP)/../mediatek/device/prebuilt/ -lhttplive
#LOCAL_LDFLAGS  += -L$(ANDROID_BUILD_TOP)/../mediatek/device/prebuilt/ -lrtspplayer
endif


ifeq "$(USE_CMPB_AS_GALLERY_PLAYER)" "true"
LOCAL_CFLAGS   += -DCC_USE_CMPB_AS_GALLERY_PLAYER
LOCAL_CXXFLAGS += -DCC_USE_CMPB_AS_GALLERY_PLAYER
endif

LOCAL_C_INCLUDES :=                                                 \
    $(call include-path-for, graphics corecg)                       \
    $(TOP)/frameworks/av/media/libstagefright/include               \
    $(TOP)/frameworks/av/media/libstagefright/rtsp                  \
    $(TOP)/frameworks/av/media/libstagefright/wifi-display          \
    $(TOP)/frameworks/native/include/media/openmax                  \
    $(TOP)/external/tremolo/Tremolo                                 \
    $(TOP)/frameworks/av/media/libstagefright/chromium_http \
    $(TOP)/../mediatek/external/liveMedia/liveMedia/include \
    $(TOP)/../mediatek/external/liveMedia/BasicUsageEnvironment/include   \
    $(TOP)/../mediatek/external/liveMedia/groupsock/include   \
    $(TOP)/../mediatek/external/liveMedia/UsageEnvironment/include \
    $(TOP)/bionic \
    $(TOP)/bionic/libstdc++/include \
    $(TOP)/external/stlport/stlport \
    $(TOP)/external/stlport/src \
    $(TOP)/../mediatek/external/curl/include \
    $(TOP)/../mediatek/external/curl/include/curl \
    $(TOP)/external/openssl/include   \
    $(TOP)/external/expat/lib         \
    $(TOP)/system/core/include/utils   \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/include \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/ \
    $(TOP)/../../project_x/middleware/cmpb \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/protocol/playlist/include \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/protocol/playlist/src \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/protocol/rtspplayer/include \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/protocol/vospplayer/include \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/cmpb/protocol/inet_inc 

ifeq "$(MTKSTREAM_PLAYER_SUPPORT)" "true"
LOCAL_C_INCLUDES += $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/mtkstream/include
LOCAL_C_INCLUDES += $(TOP)/frameworks/av/include/drm
LOCAL_C_INCLUDES += $(TOP)/../../dtv_linux/mtk_util/source/netmagic/smartfs/include
LOCAL_C_INCLUDES += $(TOP)/../../dtv_linux/mtk_util/library/gnuarm-4.5.1_vfp_ca9/mtkdrm/include
LOCAL_C_INCLUDES += $(TOP)/../mediatek/frameworks/MtkResourceManager/client
LOCAL_C_INCLUDES += $(TOP)/../mediatek/frameworks/MtkResourceManager
LOCAL_C_INCLUDES += $(TOP)/../../dtv_linux/mtk_util/source/mtk_stream/inc
endif

LOCAL_MODULE:= libmediaplayerservice

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
