LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

VM_LINUX_TOP ?= $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false 

LOCAL_MODULE := iptv_play_test

LOCAL_SRC_FILES := iptv_play_test.cpp info_get.c
          
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../libTsPlayer  $(VM_LINUX_TOP)/project_x/middleware/cmpb

LOCAL_LDFLAGS += -lTsPlayer   
                  
LOCAL_LDLIBS += -lpthread

LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/app        

#如果需要编译，请不要注释下面这行          
#include $(BUILD_EXECUTABLE)
      