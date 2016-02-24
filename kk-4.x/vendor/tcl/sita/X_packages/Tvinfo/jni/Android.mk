#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PREBUILT_LIBS := \
	libdeviceinfo_jni.so 
#	libDeviceInfoCmd.so

include $(BUILD_MULTI_PREBUILT)
