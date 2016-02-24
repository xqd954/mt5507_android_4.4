##
## This is libmediasurport build system
##
LOCAL_PATH := $(call my-dir)
MEDIA_TOP := $(LOCAL_PATH)
include $(CLEAR_VARS)

# build ffmpeg lib
include $(MEDIA_TOP)/ffmpeg/Android.mk

# build faac-1.28 lib
include $(MEDIA_TOP)/libfaac/Android.mk



