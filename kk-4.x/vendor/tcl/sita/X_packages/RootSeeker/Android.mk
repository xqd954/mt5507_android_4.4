#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

# Build the Java library.
# ============================================================
include $(CLEAR_VARS)

# This is the target being built.
LOCAL_SRC_FILES := $(call all-java-files-under, java)
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= com.tcl.seeker

include $(BUILD_JAVA_LIBRARY)

include $(call all-makefiles-under, $(LOCAL_PATH))
