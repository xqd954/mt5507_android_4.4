#
# Build setmac command
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libvsstexch

LOCAL_SRC_FILES := vsstex.cpp
LOCAL_SHARED_LIBRARIES := libEGL libGLESv1_CM liblog
DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)

LOCAL_LDFLAGS += -L$(ANDROID_BUILD_TOP)/$(DEVICE_STRING)/DTV_OUT/shares_libraries \
	-lmtal
LOCAL_CFLAGS := -I$(L_CFLAGS) -DANDROID_TOOLCHAIN
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../project_x/middleware/res_mngr/drv
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../project_x/middleware/inc
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/obj/src
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/./include/

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../vm_linux/chiling/app_if/mtal/mtal_inc
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../vm_linux/chiling/app_if/mtal/include

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/mtal_inc \
#                    $(LOCAL_PATH)/../../../../chiling/app_if/mtal/include
                   
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/mtal_inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/driver/linux/include
LOCAL_C_INCLUDES += $(DRV_INC_DIR)
LOCAL_C_INCLUDES += $(COMMON_INC_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/inc \
          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/drv_inc \
          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/sys \
          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/ \
          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/hw \
          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/drv \
          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/private_inc

#LOCAL_CFLAGS += $(LOCAL_PATH)/../../../../../vm_linux/android/ics-4.x/device/mtk/mt5396/prebuilt/lib

LOCAL_C_INCLUDES +=	$(JNI_H_INCLUDE)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../android/ics-4.x/frameworks/base/opengl/include/GLES

LOCAL_MODULE_TAGS := optional


LOCAL_PRELINK_MODULE := false

#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)
