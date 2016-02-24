#
# Copyright (C) 2013 TCL Corp
#
#

LOCAL_PATH:= $(call my-dir)
	# use Source files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	# This is the target being built.
	LOCAL_MODULE:= libwifimultsupport_jni
	# All of the source files that we will compile.
	LOCAL_SRC_FILES:= \
		Wifimultisupport.cpp
	# All of the shared libraries we link against.
	LOCAL_SHARED_LIBRARIES := \
		libnativehelper \
		libcutils \
		libutils \
		libdl

	# No static libraries.
	LOCAL_STATIC_LIBRARIES :=

	# Also need the JNI headers.
	LOCAL_C_INCLUDES += \
		$(JNI_H_INCLUDE)

	# No specia compiler flags.
	LOCAL_CFLAGS +=
	include $(BUILD_SHARED_LIBRARY)

$(DEFAULT_GOAL): $(LOCAL_PATH)/libwifimultsupport_jni.so
all_modules: $(LOCAL_PATH)/libwifimultsupport_jni.so
$(LOCAL_PATH)/libwifimultsupport_jni.so: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
