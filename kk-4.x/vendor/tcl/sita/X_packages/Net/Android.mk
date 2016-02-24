#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

# the library
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/net/ethernet)
LOCAL_MODULE:= com.tcl.net.ethernet
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_TAGS := optional

include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/net/pppoe)
LOCAL_MODULE:= com.tcl.net.pppoe
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_TAGS := optional

include $(BUILD_JAVA_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH)/lib)
