LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)
# Module name should match apk name to be installed
LOCAL_MODULE := RTKSystemDemo
LOCAL_SRC_FILES := RTKSystemDemo2.0.apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX:=$(COMMON_ANDROID_PACKAGE_SUFFIX)
#choose apk's location 
LOCAL_MODULE_PATH := $(TARGET_OUT_APPS)
LOCAL_CERTIFICATE:=platform
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
# Module name should match apk name to be installed
LOCAL_MODULE := 3Ddemo
LOCAL_SRC_FILES := 3Ddemo.apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX:=$(COMMON_ANDROID_PACKAGE_SUFFIX)
#choose apk's location 
LOCAL_MODULE_PATH := $(TARGET_OUT_APPS)
LOCAL_CERTIFICATE:=platform
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)