#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

# Build the Java library.
# ============================================================
#modify by tank@tcl.com
#include $(CLEAR_VARS)
#LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/snapshot)
#LOCAL_PROGUARD_ENABLED := disabled
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE := com.tcl.snapshot
#include $(BUILD_JAVA_LIBRARY)
#end tank@tcl.com

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/media)
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := com.tcl.media
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/screenrecorder)
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := com.tcl.screenrecorder

include $(BUILD_JAVA_LIBRARY)

#modify by tank@tcl.com
include $(call all-makefiles-under,$(LOCAL_PATH)/jni)
#end tank@tcl.com
