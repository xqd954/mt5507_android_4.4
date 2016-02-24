#ifeq "$(Del_excrescentApk)" "true"
#$(warning  "not need build android standard PackageInstaller")
#else
#ifeq "$(Hele_tMedia_project)" "true"
#$(warning  "not need build android standard PackageInstaller for hele project")
#else
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    $(call all-subdir-java-files) \
    src/com/android/packageinstaller/EventLogTags.logtags

LOCAL_STATIC_JAVA_LIBRARIES += android-support-v4

LOCAL_PACKAGE_NAME := PackageInstaller
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

include $(BUILD_PACKAGE)

#endif
#endif
