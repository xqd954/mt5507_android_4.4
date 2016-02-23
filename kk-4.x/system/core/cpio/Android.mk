# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mkbootfs.c

LOCAL_MODULE := mkbootfs

include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    mkbootfs.c

LOCAL_MODULE := mkbootfs
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

$(call dist-for-goals,dist_files,$(LOCAL_BUILT_MODULE))
