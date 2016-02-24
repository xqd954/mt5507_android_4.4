##	Andorid.mk
##	
##	 Created on: 2012-4-16
##	     Author: zhangsh

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  log.c    \
					memory.c \
					qlist.c  \
					thread.c \
					time.c   
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
LOCAL_CFLAGS := 
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libutil_static
LOCAL_MODULE_TAGS := eng

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

# Build shared library
include $(CLEAR_VARS)

LOCAL_MODULE := libutil

LOCAL_MODULE_TAGS := optional

LOCAL_WHOLE_STATIC_LIBRARIES = libutil_static

LOCAL_SHARED_LIBRARIES := \
	liblog


include $(BUILD_SHARED_LIBRARY)

