LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += \
	-DBTMTK_ON_LINUX \
	
LOCAL_SRC_FILES += \
	bt_audio_a2dp_hw.c \
	bt_a2dp_android.c \
	sbc/sbc_anal.c \
	sbc/sbc_common.c \
	sbc/sbc_enc.c \

LOCAL_C_INCLUDES+= \
  $(LOCAL_PATH)/../btadp_ext/include \
  $(LOCAL_PATH)/../include \
  $(LOCAL_PATH)/../include/pal \
  $(LOCAL_PATH)/../include/common \
  $(LOCAL_PATH)/../include/profiles \
  $(LOCAL_PATH)/../include/common/default \
  $(LOCAL_PATH)/sbc 

include $(LOCAL_PATH)/../mtkbt.inc

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_SHARED_LIBRARIES += \
	libpower 

LOCAL_MODULE := audio.a2dp.default
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/hw

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
