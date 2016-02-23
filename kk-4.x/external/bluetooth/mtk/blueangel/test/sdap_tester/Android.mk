
################################################
# Build Start
################################################
LOCAL_PATH := $(call my-dir)
CURR_PATH := $(LOCAL_PATH)
#BT_PATH := $(MTK_PATH_SOURCE)/protect/external/bluetooth/blueangel

################################################
# Build SDAP Tester
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	bt_sdap_tester.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/include/test \
	$(BT_PATH)/btadp_ext/sys/include \
	
include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils libutils

LOCAL_MODULE := bt_sdap_tester

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE:= false
include $(BUILD_EXECUTABLE)
