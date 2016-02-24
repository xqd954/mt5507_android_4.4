#
# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH:= $(call my-dir)

define tcl-test-java-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.java" -and -not -name ".*") )
endef

define tcl-test-cpp-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.cpp" -and -not -name ".*") )
endef

define tcl-test-c-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.c" -and -not -name ".*") )
endef

ifneq ($(call tcl-test-c-files),)
	# use Source files.
	# ============================================================
	####
	include $(CLEAR_VARS)

	LOCAL_SRC_FILES:=\
		src/pppoe.c \
		src/if.c \
		src/debug.c \
		src/common.c \
		src/ppp.c \
		src/discovery.c \


	#LOCAL_STATIC_LIBRARIES :=

	LOCAL_SHARED_LIBRARIES := liblog

	#LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
	#LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
	LOCAL_MODULE_TAGS := optional
	LOCAL_MODULE := pppoe
	LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
	LOCAL_CFLAGS += '-DVERSION="3.10"' \
		'-DPPPD_PATH="/system/bin/pppd"'

	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/pppoe
all_modules: $(LOCAL_PATH)/pppoe
$(LOCAL_PATH)/pppoe: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
else
	# use binary files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_EXECUTABLES := \
		pppoe \

	include $(BUILD_MULTI_PREBUILT)
endif

include $(call all-makefiles-under,$(LOCAL_PATH))
