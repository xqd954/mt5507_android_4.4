# Copyright 2008 The Android Open Source Project
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files)
LOCAL_MODULE := capture 
# LOCAL_JAVA_LIBRARIES += com.mediatek.tvcm
LOCAL_MODULE_TAGS := optional
include $(BUILD_JAVA_LIBRARY)

#include $(CLEAR_VARS)
#ALL_PREBUILT += $(TARGET_OUT)/bin/capture
#$(TARGET_OUT)/bin/input : $(LOCAL_PATH)/capture | $(ACP)
#	$(transform-prebuilt-to-target)
