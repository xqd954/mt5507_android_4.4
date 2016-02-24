LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := 

LOCAL_MODULE := com.tcl.tclvoicecontrol
LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_STATIC_JAVA_LIBRARIES := pre_tclvoicecontrol
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := pre_tclvoicecontrol
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)
LOCAL_SRC_FILES := java/com/tcl/tclvoicecontrol/classes.jar

include $(BUILD_PREBUILT)
include $(call all-makefiles-under,$(LOCAL_PATH))
