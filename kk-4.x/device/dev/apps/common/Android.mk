ifneq "$(HYBRID_MODE)" "true"
ifneq "$(ANDROID_TWO_WORLDS)" "true"
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# Copy the add-on library XML files in the system image.
#PRODUCT_COPY_FILES += \
#    $(LOCAL_PATH)/com.mediatek.tvcm.xml:system/etc/permissions/com.mediatek.tvcm.xml

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under,tvcommon) 
LOCAL_SRC_FILES += $(call all-java-files-under,tvcm)
#LOCAL_SRC_FILES += $(call all-java-files-under,netcm)
LOCAL_SRC_FILES += $(call all-java-files-under,mmpcm)
#Use prebuild gamekit.jar,replace source code.
LOCAL_SRC_FILES += $(call all-java-files-under,gamekit)
LOCAL_SRC_FILES += tvcommon/com/mediatek/tvcommon/ITVSelectorListener.aidl
LOCAL_AIDL_INCLUDES += $(LOCAL_PATH)/tvcommon/

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_JAVA_LIBRARIES := core framework
LOCAL_JAVA_LIBRARIES += android-common 

LOCAL_JAVA_LIBRARIES += com.mediatek.tv.custom
LOCAL_JAVA_LIBRARIES += com.mediatek.tv
LOCAL_JAVA_LIBRARIES += com.mediatek.mmp
LOCAL_JAVA_LIBRARIES += com.mediatek.dlna
LOCAL_JAVA_LIBRARIES += com.mediatek.dm
LOCAL_JAVA_LIBRARIES += com.mediatek.common.PhotoPlayer
LOCAL_JAVA_LIBRARIES += jcifs
LOCAL_JAVA_LIBRARIES += com.mediatek.common.capture

LOCAL_MODULE := com.mediatek.tvcm

LOCAL_CERTIFICATE := platform
include $(BUILD_JAVA_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := jcifs
#include $(BUILD_MULTI_PREBUILT)

# Use the folloing include to make our test apk.
#include $(call all-makefiles-under,$(LOCAL_PATH))
endif # endif ANDROID_TWO_WORLDS
endif # endif HYBRID_MODE

