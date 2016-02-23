ifneq "$(HYBRID_MODE)" "true"
ifneq "$(ANDROID_TWO_WORLDS)" "true"
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

#LOCAL_JAVA_LIBRARY := libtvss libtvcmss

LOCAL_JAVA_LIBRARIES += com.mediatek.tv.custom

LOCAL_JAVA_LIBRARIES += com.mediatek.tv
#LOCAL_JAVA_LIBRARIES += com.mediatek.tpsclient
LOCAL_JAVA_LIBRARIES += com.mediatek.tvcm
LOCAL_JAVA_LIBRARIES += com.mediatek.mmp
ifneq "$(RLS_CUSTOM_BUILD)" "true"
LOCAL_JAVA_LIBRARIES += com.mediatek.record
endif
LOCAL_JAVA_LIBRARIES += com.mediatek.dlna
LOCAL_JAVA_LIBRARIES += com.mediatek.dm
LOCAL_JAVA_LIBRARIES += jcifs
LOCAL_JAVA_LIBRARIES += com.mediatek.common.capture
LOCAL_JAVA_LIBRARIES += com.mediatek.common.PhotoPlayer
#LOCAL_STATIC_JAVA_LIBRARIES += libgamekit

#LOCAL_STATIC_JAVA_LIBRARIES :=  libtv libtvcm

LOCAL_SRC_FILES := \
        $(call all-java-files-under, src) 

#PRODUCT_LOCALES := ldpi \
#                   hdpi \
#                   mdpi \
#                   zh-rCN

#LOCAL_CLASSPATH := $(LOCAL_PATH)/lib
#LOCAL_SDK_VERSION := current

LOCAL_REQUIRED_MODULES:= libpicapi
LOCAL_PACKAGE_NAME := TKUI
LOCAL_CERTIFICATE := platform
LOCAL_OVERRIDES_PACKAGES := Home
LOCAL_PROGUARD_FLAG_FILES := proguard.flags

include $(BUILD_PACKAGE)

#include $(CLEAR_VARS)
#LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libgamekit:lib/gamekit.jar 
#include $(BUILD_MULTI_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libtv:lib/com.mediatek.tv.jar libtvcm:lib/com.mediatek.tvcm.jar 
#LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libtvcmss:lib/com.mediatek.tvcm.jar 


#include $(BUILD_MULTI_PREBUILT)


# additionally, build tests in sub-folders in a separate .apk
include $(call all-makefiles-under,$(LOCAL_PATH))
endif # endif ANDROID_TWO_WORLDS
endif # endif HYBRID_MODE

