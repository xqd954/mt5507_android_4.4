LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
#############################################################
 
LOCAL_C_INCLUDES += \
  external/stlport/stlport \
  external/sqlite/dist \
  external/expat/ \
  external/icu4c/common \
  external/icu4c/i18n \
  $(libc_common_c_includes) \
  system/core/include/cutils \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/common/include

LOCAL_SRC_FILES += \
  src/DeviceInfo.cpp \
  common/src/DatabaseCtrl.cpp

LOCAL_SHARED_LIBRARIES := \
  libcutils \
  libutils  \
  libstdc++ \
  libstlport \
  libsqlite \
  libexpat

LOCAL_LDLIBS += -lpthread
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog


LOCAL_PRELINK_MODULE:=false
LOCAL_MODULE:= libDeviceInfoCmd
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

