LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
			main.c \
			input.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
					$(LOCAL_PATH)/../include
			
	
LOCAL_MODULE := test_faac

#LOCAL_SHARED_LIBRARIES := libfaac
LOCAL_STATIC_LIBRARIES := libfaac

LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


















