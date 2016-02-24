#
# build libavutil libavformat libavcodec libswscale libavfilter
#

LOCAL_PATH := $(call my-dir)
FFMPEG_TOP := $(LOCAL_PATH)

include $(CLEAR_VARS)
include $(FFMPEG_TOP)/ffmpeg/libavutil/Android.mk
include $(FFMPEG_TOP)/ffmpeg/libavcodec/Android.mk
include $(FFMPEG_TOP)/ffmpeg/libavformat/Android.mk

# build ffprobe ffmpeg tool
#include $(FFMPEG_TOP)/ffmpeg/Android.mk

