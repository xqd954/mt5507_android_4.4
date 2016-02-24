#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#

LOCAL_PATH := $(call my-dir)

# Build the Java library.
# ============================================================

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=
LOCAL_STATIC_JAVA_LIBRARIES := pre_adsystem
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := com.tcl.adsystem

# LOCAL_JAVA_LIBRARIES := services

include $(BUILD_JAVA_LIBRARY)


# use binary files.
# ============================================================
include $(CLEAR_VARS)

LOCAL_MODULE := pre_adsystem
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)
LOCAL_SRC_FILES := com.tcl.adsystem.jar
include $(BUILD_PREBUILT)

# Install the created library in the right location.
# ============================================================
include $(CLEAR_VARS)

LOCAL_MODULE := com.tcl.adsystem.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

#include $(BUILD_PREBUILT)
# The JNI component
# ============================================================
# Also build all of the sub-targets under this one: the library's
# associated JNI code, and a sample client of the library.

include $(call all-makefiles-under,$(LOCAL_PATH))

