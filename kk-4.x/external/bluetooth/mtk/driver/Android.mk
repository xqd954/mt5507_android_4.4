
MTK_BT_SUPPORT := yes
#MTK_COMBO_SUPPORT := yes
#MTK_LIBUSB_SUPPORT := yes
MTK_EXTUSB_SUPPORT := yes

ifeq ($(MTK_BT_SUPPORT), yes)
LOCAL_PATH := $(call my-dir)

ifeq ($(MTK_COMBO_SUPPORT), yes)
include $(LOCAL_PATH)/combo/Android.mk
else ifeq ($(MTK_EXTUSB_SUPPORT), yes)
include $(LOCAL_PATH)/extusb/Android.mk
else ifeq ($(MTK_LIBUSB_SUPPORT), yes)
include $(LOCAL_PATH)/libusb/Android.mk
else
include $(LOCAL_PATH)/standalone/Android.mk
endif

endif
