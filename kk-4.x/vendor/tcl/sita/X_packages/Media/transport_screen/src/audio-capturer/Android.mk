LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := audio_capture.c

LOCAL_C_INCLUDES:= external/tinyalsa/include    \
                   $(LOCAL_PATH)/../libutil     

LOCAL_SHARED_LIBRARIES := libtinyalsa \
                          liblog
                         
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libaudiocapture
LOCAL_MODULE_TAGS := eng
include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

##### test audio capture
#include $(CLEAR_VARS)
#LOCAL_SRC_FILES := test.c
#
#LOCAL_C_INCLUDES:= external/tinyalsa/include \
#                   $(LOCAL_PATH)/../libutil         
#                   
#LOCAL_STATIC_LIBRARIES := libaudiocapture
#
#LOCAL_SHARED_LIBRARIES := libtinyalsa \
#                          libutil     \
#                          liblog
#                         
#LOCAL_MODULE := test_audio_capture
#
#LOCAL_MODULE_TAGS := optional
#
#include $(BUILD_EXECUTABLE)

#
# test audio capture
#
#include $(CLEAR_VARS)
#LOCAL_SRC_FILES := audio_cap_test.c
#LOCAL_C_INCLUDES:= external/tinyalsa/include \
#                   $(LOCAL_PATH)/../libutil         
#                   
##LOCAL_STATIC_LIBRARIES := libaudiocapture
#
#LOCAL_SHARED_LIBRARIES := libaudiocapture \
#			  libtinyalsa \
#                          libutil     \
#                          liblog
#LOCAL_MODULE := audio_cap_test
#LOCAL_MODULE_TAGS := optional
#
#include $(BUILD_EXECUTABLE)
