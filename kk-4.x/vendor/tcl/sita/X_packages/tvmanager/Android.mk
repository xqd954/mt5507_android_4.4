LOCAL_PATH := $(call my-dir)



#com.tcl.factory
include $(CLEAR_VARS)
LOCAL_MODULE := com.tcl.factory
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := 
LOCAL_STATIC_JAVA_LIBRARIES := pre_factory
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := pre_factory
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)
LOCAL_SRC_FILES := com.tcl.factory.jar
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.tcl.factory.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)



#com.tcl.tvmanager
include $(CLEAR_VARS)
LOCAL_MODULE := com.tcl.tvmanager
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := 
LOCAL_STATIC_JAVA_LIBRARIES := pre_tvmanager
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := pre_tvmanager
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)
LOCAL_SRC_FILES := com.tcl.tvmanager.jar
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.tcl.tvmanager.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)


#com.tcl.snapshot
include $(CLEAR_VARS)
LOCAL_MODULE := com.tcl.snapshot
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := 
LOCAL_STATIC_JAVA_LIBRARIES := pre_snapshot
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := pre_snapshot
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)
LOCAL_SRC_FILES := com.tcl.snapshot.jar
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.tcl.snapshot.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

