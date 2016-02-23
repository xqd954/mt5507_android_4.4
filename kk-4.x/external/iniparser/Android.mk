#
# Build setmac command
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libiniparser

LOCAL_SRC_FILES := src/dictionary.c \
 									 src/iniparser.c

LOCAL_PRELINK_MODULE := false

#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)
