LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := ffmpeg.c \
                   /cmdutils.c
LOCAL_MODULE := ffmpeg
LOCAL_MODULE_TAGS := eng
LOCAL_C_INCLUDE := $(LOCAL_PATH)/ \
                   $(LOCAL_PATH)/libavutil/ \
                   $(LOCAL_PATH)/libavformat \
                   $(LOCAL_PATH)/libavcodec 
LOCAL_SHARED_LIBRARIES := libavutil_ libavcodec_ libavformat_ libswscale_ libavfilter_ libavdevice_
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := ffprobe.c \
                   /cmdutils.c
LOCAL_MODULE := ffprobe
LOCAL_MODULE_TAGS := eng
LOCAL_C_INCLUDE := $(LOCAL_PATH)/ \
                   $(LOCAL_PATH)/libavutil/ \
                   $(LOCAL_PATH)/libavformat \
                   $(LOCAL_PATH)/libavcodec 
LOCAL_SHARED_LIBRARIES := libavutil_ libavcodec_ libavformat_ libswscale_ libavfilter_ libavdevice_
include $(BUILD_EXECUTABLE)




