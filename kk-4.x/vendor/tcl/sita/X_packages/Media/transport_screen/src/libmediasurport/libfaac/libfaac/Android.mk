LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
			aacquant.c\
			bitstream.c \
			fft.c frame.c \
			midside.c\
			psychkni.c\
			util.c \
			backpred.c\
			channels.c\
			filtbank.c \
			huffman.c \
			ltp.c \
			tns.c\
			kiss_fft/kiss_fftr.c\
			kiss_fft/kiss_fft.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
					$(LOCAL_PATH)/../include
			
	
LOCAL_MODULE := libfaac

TARGET_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)


















