#
# Copyright (C) 2013 TCL Corp
#
#

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

ifneq ($(call tcl-test-cpp-files),)
# use Source files.
# ============================================================

include $(CLEAR_VARS)

LOCAL_MODULE := libtmedia_jni
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := android_media_TMediaPlayer.cpp 

LOCAL_SHARED_LIBRARIES := libcutils \
			  libutils \
			  libui \
			  libgui \
			  libskia \
			  libbinder \
			  libandroid_runtime \
			  libnativehelper \
			  libskia \
			  libgui \
			  libmedia

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
		    $(TOP)/frameworks/base/core/jni \
		    $(LOCAL_PATH)/lib/include \
		    frameworks/base/include/ui \
		    frameworks/base/include/utils \
		    external/skia/include/core \
		    external/skia/include/utils \
		    frameworks/native/include/gui \
		    frameworks/native/include/binder \
		    frameworks/native/include/ui

include $(BUILD_SHARED_LIBRARY)


$(DEFAULT_GOAL): $(LOCAL_PATH)/$(notdir $(LOCAL_BUILT_MODULE))
all_modules: $(LOCAL_PATH)/$(notdir $(LOCAL_BUILT_MODULE))
$(LOCAL_PATH)/$(notdir $(LOCAL_BUILT_MODULE)): $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)

else
# use binary files.
# ============================================================

	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_LIBS := \
		libtmedia_jni.so

	include $(BUILD_MULTI_PREBUILT)
endif
