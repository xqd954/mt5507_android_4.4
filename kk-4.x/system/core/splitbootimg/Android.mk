
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := split_bootimg.cpp

LOCAL_MODULE := split_bootimg
LOCAL_MODULE_TAGS := optional
include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := split_bootimg.cpp

LOCAL_MODULE := split_bootimg
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

$(call dist-for-goals,droid,$(LOCAL_BUILT_MODULE))
