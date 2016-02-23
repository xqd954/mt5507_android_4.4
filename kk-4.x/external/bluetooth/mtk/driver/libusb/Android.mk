LOCAL_PATH := $(call my-dir)
#$(call config-custom-folder,custom:hal/bluetooth)

###########################################################################
# MTK BT DRIVER SOLUTION
###########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  bt_drv.c \
  usb.c \
  upio.c

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/custom \
  $(LOCAL_PATH)/../inc \
  external/libusb/libusb

LOCAL_MODULE := libbluetoothdrv
LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libhardware_legacy libusb 
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)


