#
# Copyright (c) 2010-2012 TCL Corp.
# All rights reserved.
#
LOCAL_PATH := $(call my-dir)

define tcl-test-java-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.java" -and -not -name ".*") )
endef

define tcl-test-cpp-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.cpp" -and -not -name ".*") )
endef

define tcl-test-c-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.c" -and -not -name ".*") )
endef

# use Source files.
# ============================================================
include $(CLEAR_VARS)

LOCAL_MODULE := android.tclwidget

LOCAL_SRC_FILES := \
	$(call all-java-files-under,java/tclwidget6.0)

LOCAL_DEX_PREOPT := false
LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled
#LOCAL_REQUIRE_MODULES := tvos-framework-res
#LOCAL_JAVA_LIBRARIES := framework
#LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/../res/Android.mk
#LOCAL_STATIC_LIBRARIES := pre_tvos_res

#$(call add-required-deps, android-tclwidget, tvos-framework-res)

framework_tvos_res_source_path := APPS/tvos-framework-res_intermediates/src
LOCAL_INTERMEDIATE_SOURCES += $(framework_tvos_res_source_path)/tvos/R.java 
#			  $(framework_tvos_res_source_path)/tvos/Manifest.java 




include $(BUILD_JAVA_LIBRARY)

# Make sure that R.java and Manifest.java are built before we build
# the source for this library.
#tvos-framework_res_R_stamp := \
	$(call intermediates-dir-for,APPS,tvos-framework-res,,COMMON)/src/R.stamp
#$(full_classes_compiled_jar): $(tvos-framework_res_R_stamp)
#$(info ++++++++++++++++++++++++++++++++++++++++++++++++ $(tvos-framework_res_R_stamp))
$(info ++++++++++++++++++++++++++++++++++++++++++++++++ android.tclwidget)
#$(error xxx $(full_classes_compiled_jar))

#$(full_classes_compiled_jar): $(tvos-framework-res-package-target)
#$(info xxx $(full_classes_compiled_jar))
#$(info xxx $(tvos-framework-res-package-target))
#$(error xxx tvos-framework-res-package-target)




# Install the created library in the right location.
# ============================================================
include $(CLEAR_VARS)

LOCAL_MODULE := android.tclwidget.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))

