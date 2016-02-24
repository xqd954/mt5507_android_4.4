LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/../utils.mk

LOCAL_SRC_FILES := $(FFFILES)

LOCAL_C_INCLUDES := $(LOCAL_PATH)                \
					$(LOCAL_PATH)/../            \
					external/zlib/

LOCAL_CFLAGS += $(FFCFLAGS) -Dipv6mr_interface=ipv6mr_ifindex

LOCAL_SHARED_LIBRARIES := libz       \
			  libcutils  \
			  libutils   \
			  libdl

LOCAL_STATIC_LIBRARIES := 

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libavformat_
LOCAL_MODULE_TAGS := eng

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

