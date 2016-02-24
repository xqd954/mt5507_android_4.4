##	Android.mk
##	
##	 Created on: 2012-4-17
##	     Author: zhangsh
##	
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  stream_muxer.c \
					ts_muxer.c       
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/                   \
					$(LOCAL_PATH)/../libutil         \
					$(LOCAL_PATH)/../audio-capturer  \
					$(LOCAL_PATH)/../video-capturer/mstar_capturer \
					$(LOCAL_PATH)/../libmediasurport/ffmpeg/ffmpeg \
					$(LOCAL_PATH)/../segmenter
LOCAL_STATIC_LIBRARIES :=   libutil_static \
			    libaudiocapture \
			    libhlsmsarvideocap \
			    libavformat_ \
			    libavcodec_ \
			    libavutil_ \
			    libfaac
			
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libmuxer_static
LOCAL_MODULE_TAGS := eng

include $(BUILD_STATIC_LIBRARY)

######################################################


include $(CLEAR_VARS)
LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel

LOCAL_WHOLE_STATIC_LIBRARIES = libmuxer_static

LOCAL_SHARED_LIBRARIES :=   libcutils \
							liblog  libutils  libbinder                           \
							libcamera_client  \
							libui libgui libmedia libhardware libhardware_legacy  \
							libicuuc      \
							libstagefright_foundation libgabi++      \
							libtinyalsa libz libdl
LOCAL_STATIC_LIBRARIES :=   libutil_static \
			    libaudiocapture \
			    libhlsmsarvideocap \
			    libavformat_ \
			    libavcodec_ \
			    libavutil_ \
			    libfaac

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libmuxer
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/libmuxer.so
all_modules: $(LOCAL_PATH)/libmuxer.so
$(LOCAL_PATH)/libmuxer.so: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
######################################################


include $(CLEAR_VARS)
LOCAL_SRC_FILES :=  stream_muxer_test.c 
#LOCAL_SRC_FILES :=  stream_video_test.c  
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/                   \
					$(LOCAL_PATH)/../libutil         \
					$(LOCAL_PATH)/../audio-capturer  \
					$(LOCAL_PATH)/../video-capturer/mstar_capturer \
					$(LOCAL_PATH)/../libmediasurport/ffmpeg/ffmpeg
					
LOCAL_CFLAGS := 
LOCAL_SHARED_LIBRARIES :=   libcutils \
							liblog  libutils  libbinder                           \
							libcamera_client  \
							libui libgui libmedia libhardware libhardware_legacy  \
							libicuuc      \
							libstagefright_foundation libgabi++      \
							libtinyalsa libz libdl 
							
LOCAL_STATIC_LIBRARIES :=   libutil_static \
			    libaudiocapture \
			    libhlsmsarvideocap \
			    libavformat_ \
			    libavcodec_ \
			    libavutil_ \
			    libfaac \
			    libmuxer_static
			    
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := stream_muxer_test
#LOCAL_MODULE := stream_video_test
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)

