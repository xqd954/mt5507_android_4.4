LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/../utils.mk

LOCAL_SRC_FILES := $(FFFILES)

LOCAL_C_INCLUDES := $(LOCAL_PATH)     \
					$(LOCAL_PATH)/../ \
					external/zlib/	\
					$(LOCAL_PATH)/../../../libfaac/include \
					
					

LOCAL_CFLAGS += $(FFCFLAGS) -O2 -mno-thumb-interwork -marm -Dipv6mr_interface=ipv6mr_ifindex

LOCAL_SHARED_LIBRARIES := libz      \
                          libcutils \
                          libutils  \
                          libdl

LOCAL_STATIC_LIBRARIES := libfaac libavutil_

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libavcodec_
LOCAL_MODULE_TAGS := eng

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

