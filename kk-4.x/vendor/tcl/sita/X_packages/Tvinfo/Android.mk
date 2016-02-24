#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

define tcl-test-java-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.java" -and -not -name ".*") )
endef

define tcl-test-cpp-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.cpp" -and -not -name ".*") )
endef

define tcl-test-c-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.c" -and -not -name ".*") )
endef

include $(CLEAR_VARS)

LOCAL_JAVA_LIBRARIES := com.tcl.factory com.tcl.tvmanager
	
LOCAL_SRC_FILES := \
		$(call all-java-files-under)

LOCAL_MODULE := com.tcl.deviceinfo
LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled
include $(BUILD_JAVA_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
