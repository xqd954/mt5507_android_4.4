LOCAL_PATH:= $(call my-dir)

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

ifneq ($(call tcl-test-cpp-files),)
	# use Source files.
	# ============================================================
	####
	common_src_files := VolumeManager.cpp\
		CommandListener.cpp \
		VoldCommand.cpp \
		NetlinkManager.cpp \
		NetlinkHandler.cpp \
		Volume.cpp \
		DirectVolume.cpp \
		logwrapper.c \
		Process.cpp \
		Ext4.cpp \
		Fat.cpp \
		Ntfs.cpp \
		ExtFs.cpp \
		Hfsplus.cpp \
		Loop.cpp \
		Devmapper.cpp \
		ResponseCode.cpp \
		Xwarp.cpp \
		cryptfs.c \
		VolumeInfo.cpp \
		VolumeInfoManager.cpp \
		UmsVolume.cpp

	common_c_includes := \
		$(KERNEL_HEADERS) \
		system/extras/ext4_utils \
		external/openssl/include \
		vendor/tcl/addon/external/vold_module/lib \
		external/icu4c/common    \
		external/icu4c/i18n   \
		$(LOCAL_PATH)/../fstype_support/lib

	common_shared_libraries := \
		libext4_utils \
		liblogwrap \
		libsysutils \
		libcutils \
		libdiskconfig \
		libhardware_legacy \
		libcrypto \
		libvold_blkid_tcl \
		libicui18n \
		libicuuc \
		libz



	include $(CLEAR_VARS)

	LOCAL_MODULE := libvold_tcl

	LOCAL_SRC_FILES := $(common_src_files)

	LOCAL_C_INCLUDES := $(common_c_includes)

	LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)

	LOCAL_STATIC_LIBRARIES := libfs_mgr

	LOCAL_MODULE_TAGS := eng tests

	include $(BUILD_STATIC_LIBRARY)

	include $(CLEAR_VARS)

	LOCAL_MODULE:= vold_tcl

	LOCAL_SRC_FILES := \
		main.cpp \
		$(common_src_files)

	LOCAL_C_INCLUDES := $(common_c_includes)

	LOCAL_CFLAGS := -Werror=format

	ifeq ($(TARGET_PRODUCT), rtd299x_tv030)
		LOCAL_CFLAGS += -DREALTEK_VOLD_NTFS
	else ifeq($(TARGET_PRODUCT), mtk_mt5882)
		LOCAL_CFLAGS += -DMT5507_VOLD_NTFS
	else ifeq ($(TARGET_PRODUCT), sdk)
		# sample 
		LOCAL_CFLAGS += -DSIMULATOR_DEV
	endif

	LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)

	LOCAL_STATIC_LIBRARIES := \
		libfs_mgr \
		libscrypt_static \
		libmincrypt \
		libsparse_static


	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/vold_tcl
all_modules: $(LOCAL_PATH)/vold_tcl
$(LOCAL_PATH)/vold_tcl: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
	include $(CLEAR_VARS)

	LOCAL_SRC_FILES:= vdc.c

	LOCAL_MODULE:= vdc_tcl

	LOCAL_C_INCLUDES := $(KERNEL_HEADERS)

	LOCAL_CFLAGS := 

	LOCAL_SHARED_LIBRARIES := libcutils

	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/vdc_tcl
all_modules: $(LOCAL_PATH)/vdc_tcl
$(LOCAL_PATH)/vdc_tcl: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}

	#add by cuixk
	include $(CLEAR_VARS)
	LOCAL_SRC_FILES:= nlmsg.c
	LOCAL_MODULE:=nlmsg_tcl
	LOCAL_C_INCLUDES := $(KERNEL_HEADERS)
	LOCAL_CFLAGS :=
	LOCAL_SHARED_LIBRARIES := libcutils
	LOCAL_MODULE_TAGS := eng tests
	include $(BUILD_EXECUTABLE)
	#end add by cuixk

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/nlmsg_tcl
all_modules: $(LOCAL_PATH)/nlmsg_tcl
$(LOCAL_PATH)/nlmsg_tcl: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	

else
	# use binary files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_EXECUTABLES := \
		nlmsg_tcl \
		vdc_tcl \
		vold_tcl

	include $(BUILD_MULTI_PREBUILT)

endif
