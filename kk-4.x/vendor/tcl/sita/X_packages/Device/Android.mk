#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
	
LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/devicemanager)

LOCAL_MODULE := com.tcl.devicemanager
LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
	
LOCAL_SRC_FILES := $(call all-java-files-under, java/com/tcl/os/storage)

LOCAL_MODULE := com.tcl.os.storage
LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled
include $(BUILD_JAVA_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
