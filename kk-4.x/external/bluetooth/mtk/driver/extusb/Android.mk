LOCAL_PATH := $(call my-dir)
#$(call config-custom-folder,custom:hal/bluetooth)

BT_PATH := external/bluetooth/mtk/blueangel
###########################################################################
# MTK ext BT DRIVER SOLUTION
###########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  extusb.c

LOCAL_C_INCLUDES := \
	 $(LOCAL_PATH)/../inc \


LOCAL_MODULE := libbluetoothdrv
LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)


