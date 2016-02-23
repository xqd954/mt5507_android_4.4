LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := avmute.bin

LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES :=avmute.c

# include mtimage header files
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../vm_linux/chiling/app_if/mtal/mtal_inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../vm_linux/chiling/app_if/mtal/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/mtal_inc \
                    $(LOCAL_PATH)/../../../../chiling/app_if/mtal/include

LOCAL_LDFLAGS += -L$(ANDROID_BUILD_TOP)/$(DEVICE_STRING)/DTV_OUT/shares_libraries \
    -lmtal

include $(BUILD_EXECUTABLE)

