LOCAL_PATH:= $(call my-dir)

# the library
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)
            
LOCAL_MODULE := android.policy

LOCAL_JAVA_LIBRARIES += com.mediatek.util com.tcl.tvmanager
LOCAL_JAVA_LIBRARIES += com.tcl.deviceinfo
#LOCAL_JAVA_LIBRARIES += com.tcl.factory

include $(BUILD_JAVA_LIBRARY)

# additionally, build unit tests in a separate .apk
include $(call all-makefiles-under,$(LOCAL_PATH))
