#
# Build setmac command
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libpicapi

LOCAL_SRC_FILES := pictureapi.cpp
LOCAL_SHARED_LIBRARIES :=  liblog libjpeg libskia
LOCAL_CFLAGS := -I$(L_CFLAGS) -DANDROID_TOOLCHAIN

DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)

LOCAL_LDFLAGS += -L$(ANDROID_BUILD_TOP)/$(DEVICE_STRING)/DTV_OUT/shares_libraries \
	-lmtal
LOCAL_LDLIBS += -ljpeg
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../project_x/middleware/res_mngr/drv
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../project_x/middleware/inc
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/obj/src
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/./include/

                   
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/mtal_inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/app_if/mtal/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../chiling/driver/linux/include
LOCAL_C_INCLUDES += $(DRV_INC_DIR)
LOCAL_C_INCLUDES += $(COMMON_INC_DIR)

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/inc \
#          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/drv_inc \
#          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/sys \
#          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/ \
#          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/hw \
#          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/nptv8098/inc/drv \
#          $(LOCAL_PATH)/../../../../project_x/target/$(TARGET_IC)/$(subst mt,,$(TARGET_IC))_driver/private_inc

#LOCAL_CFLAGS += $(LOCAL_PATH)/../../../../../vm_linux/android/$(ANDROID_VERSION)/device/mtk/mt5396/prebuilt/lib

LOCAL_C_INCLUDES +=	$(JNI_H_INCLUDE)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../android/$(ANDROID_VERSION)/frameworks/base/opengl/include/GLES

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../android/$(ANDROID_VERSION)/external/jpeg

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../android/$(ANDROID_VERSION)/external/skia

LOCAL_MODULE_TAGS := eng

#ifeq "$(SUPPORT_4K2K)" "true"
LOCAL_CFLAGS += -DALL_PIC_VIDEOPATH
#endif

ifeq "$(SUPPORT_MTK_ANDROID_RM)" "true"
LOCAL_C_INCLUDES += \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/client \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager        \
    $(ANDROID_BUILD_TOP)/../mediatek/frameworks/MtkResourceManager/include
ifneq "$(RLS_CUSTOM_BUILD)" "true"
LOCAL_SHARED_LIBRARIES += libMtkRmClient
else
LOCAL_LDFLAGS  += -L$(ANDROID_BUILD_TOP)/../mediatek/device/prebuilt/ -lMtkRmClient
endif
endif


LOCAL_PRELINK_MODULE := false

#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)
