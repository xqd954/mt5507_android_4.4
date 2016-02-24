##
## Screen share ,Android.mk
##
##	 Created on: 2012-4-17
##	     Author: 陶婷
##	

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

ifeq ($(TARGET_PRODUCT),)

ifneq ($(call tcl-test-c-files),)

TOP_DIR_CAP_ADD_SHARE:=$(LOCAL_PATH)
#include $(ANDROID_BUILD_TOP)/frameworks/opt/emoji/Android.mk
include $(TOP_DIR_CAP_ADD_SHARE)/src/libutil/Android.mk
include $(TOP_DIR_CAP_ADD_SHARE)/src/libmediasurport/Android.mk
include $(TOP_DIR_CAP_ADD_SHARE)/src/video-capturer/mstar_capturer/Android.mk
include $(TOP_DIR_CAP_ADD_SHARE)/src/audio-capturer/Android.mk
include $(TOP_DIR_CAP_ADD_SHARE)/src/stream-muxer/Android.mk

#include $(TOP_DIR_CAP_ADD_SHARE)/TCL_screen_shared/Android.mk


else
# use binary files.
# ============================================================
include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := \
	src/stream-muxer/libmuxer.so

include $(BUILD_MULTI_PREBUILT)
endif

endif

