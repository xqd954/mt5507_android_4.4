# Copyright 2010 The Android Open Source Project


LOCAL_PATH:= $(call my-dir)

define tcl-test-java-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.java" -and -not -name ".*") )
endef

define tcl-test-cpp-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.cpp" -and -not -name ".*") )
endef

define tcl-test-c-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.c" -and -not -name ".*") )
endef

ifneq ($(call tcl-test-c-files),)
	# use Source files.
	# ============================================================
	####
	include $(CLEAR_VARS)


	LOCAL_SRC_FILES = uds_server.c
	LOCAL_STATIC_LIBRARIS := libc

	LOCAL_MODULE_TAGS := optional
	LOCAL_MODULE:= sserver
	#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
	LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
	LOCAL_MODULE_CLASS := EXECUTABLES


	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/sserver
all_modules: $(LOCAL_PATH)/sserver
$(LOCAL_PATH)/sserver: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
else
	# use binary files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_EXECUTABLES := \
		sserver \

	include $(BUILD_MULTI_PREBUILT)
endif



		
